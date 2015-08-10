#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <rwcore.h>
#include <rwplcore.h>
#include <rpworld.h>
#include <rpskin.h>
#include <new>
#include "MemoryMgr.h"

typedef unsigned int uint;

#define RwEngineInstance (*rwengine)
extern void **rwengine;

void *RwMallocAlign(uint size, int alignment);
void RwFreeAlign(void*);
void gtadelete(void*);
void *gta_nw(int);

void *GetModelFromName(char *name);
RpAtomic *GetFirstAtomic(RpClump *clump);
RpAtomic *IsClumpSkinned(RpClump*);

extern void **CModelInfo__ms_modelInfoPtrs;

template<typename CT, typename DT> inline void*
construct_array(void *mem, CT ctor, DT dtor, int sz, int nelem)
{
	_asm{
		push	nelem
		push	sz
		push	dtor
		push	ctor
		push	mem
		mov	eax, 0x59CCB0	// _construct_new_array
		call	eax
		add	esp, 20
	}
}


template<typename AT, typename FT> inline void
destroy_array(AT mem, FT f)
{
	_asm{
		push	f
		push	mem
		mov	eax, 0x5A0620	// _destroy_new_array
		call	eax
		add	esp, 8
	}
}

int gtastrcmp(const char *s1, const char *s2);
int lcstrcmp(const char *s1, const char *s2);

class CFileMgr {
public:
	static int OpenFile(const char *name, const char *mode);
	static int Read(int fd, void *buf, int n);
	static void Seek(int fd, int offset, int whence);
	static void CloseFile(int fd);
};

class CQuaternion {
public:
	float x, y, z, w;

	void Slerp(CQuaternion &q1, CQuaternion &q2, float theta0, float theta1, float r);
	void Get(RwMatrix *mat);
	void Add(CQuaternion &q) {
		this->x += q.x;
		this->y += q.y;
		this->z += q.z;
		this->w += q.w;
	};
	void Sub(CQuaternion &q) {
		this->x -= q.x;
		this->y -= q.y;
		this->z -= q.z;
		this->w -= q.w;
	};
	void Mult(float r) {
		this->x *= r;
		this->y *= r;
		this->z *= r;
		this->w *= r;
	}
};

class CVector {
public:
	float x, y, z;

	void Add(CVector &v) {
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
	};
};

class CAnimBlendSequence;
class CAnimBlendHierarchy;
class CAnimBlock;
class CAnimBlendNode;
class CAnimBlendAssociation;
class CAnimBlendAssocGroup;
struct AnimBlendFrameData;
class CAnimBlendClumpData;
class CAnimManager;

RpClump *__fastcall CClumpModelInfo__CreateInstance(int self);
void __fastcall CClumpModelInfo__SetClump(int self, int, RpClump *clump);
void __fastcall CPedModelInfo__SetClump(int self, int, RpClump *clump);

void FrameUpdateCallBack(AnimBlendFrameData *frame, void *arg);
void FrameUpdateCallBackSkinned(AnimBlendFrameData *frame, void *arg);

extern int &ClumpOffset;
extern CAnimBlendClumpData *&gpAnimBlendClump;
void AnimBlendClumpDestroy(RpClump *clump);
void RpAnimBlendClumpUpdateAnimations(RpClump *clump, float timeDelta, bool doRender);
CAnimBlendAssociation *RpAnimBlendClumpGetFirstAssociation(RpClump *clump);
void RpAnimBlendAllocateData(RpClump *clump);
AnimBlendFrameData *RpAnimBlendClumpFindFrame(RpClump *clump, const char *name);
void RpAnimBlendClumpFillFrameArray(RpClump *clump, AnimBlendFrameData **frames);
void RpAnimBlendClumpInit(RpClump *);
bool RpAnimBlendClumpIsInitialized(RpClump *clump);
void RpAnimBlendClumpSetBlendDeltas(RpClump *clump, uint mask, float delta);
void RpAnimBlendClumpRemoveAllAssociations(RpClump *clump);
void RpAnimBlendClumpRemoveAssociations(RpClump *clump, uint mask);
CAnimBlendAssociation *RpAnimBlendClumpGetAssociation(RpClump *clump, uint mask);
CAnimBlendAssociation *RpAnimBlendClumpGetMainAssociation(RpClump *clump, CAnimBlendAssociation **, float *);
CAnimBlendAssociation *RpAnimBlendClumpGetMainPartialAssociation(RpClump *clump);
CAnimBlendAssociation *RpAnimBlendClumpGetMainAssociation_N(RpClump *clump, int n);
CAnimBlendAssociation *RpAnimBlendClumpGetMainPartialAssociation_N(RpClump *clump, int n);
CAnimBlendAssociation *RpAnimBlendClumpGetFirstAssociation(RpClump *clump, uint mask);
CAnimBlendAssociation *RpAnimBlendGetNextAssociation(CAnimBlendAssociation *assoc);
CAnimBlendAssociation *RpAnimBlendGetNextAssociation(CAnimBlendAssociation *assoc, uint mask);

RpClump *createInstance_hook(RpClump *clump);

struct RFrame {
	CQuaternion rot;
	float time;
};

struct RTFrame {
	CQuaternion rot;
	float time;
	CVector pos;
};

class CLink_CAnimBlendHierarchy {
public:
	CAnimBlendHierarchy *item;
	CLink_CAnimBlendHierarchy *prev;
	CLink_CAnimBlendHierarchy *next;

	void Insert(CLink_CAnimBlendHierarchy*);
	void Remove(void);
};

class CLinkList_CAnimBlendHierarchy {
public:
	CLink_CAnimBlendHierarchy head, tail;
	CLink_CAnimBlendHierarchy freeHead, freeTail;
	CLink_CAnimBlendHierarchy *links;

	void Init(int n);
	void Shutdown(void);
	CLink_CAnimBlendHierarchy *Insert(CAnimBlendHierarchy**);
	void Remove(CLink_CAnimBlendHierarchy*);
};

#define GETFRAME(seq, i) ((char*)(seq)->keyFrames + (((seq)->flag & 2) ? sizeof(RTFrame) : sizeof(RFrame))*(i))
#define GETCFRAME(seq, i) ((char*)(seq)->keyFramesCompressed + (((seq)->flag & 2) ? sizeof(RTFrame) : sizeof(RFrame))*(i))

// complete
class CAnimBlendSequence
{
public:
	void *vtable;
	int flag;
	char name[24];
	int numFrames;
	void *keyFrames;
	void *keyFramesCompressed;
	int boneTag;	// can't insert in the middle, code assumes this layout but no size

	CAnimBlendSequence(void);
	~CAnimBlendSequence(void);
	void ctor(void);
	void dtor(void);
	void dtor2(char flag);
	void SetName(const char *name);
	void SetBoneTag(int tag);
	void SetNumFrames(int numFrames, bool TS);
	void RemoveQuaternionFlips(void);
};

// complete
class CAnimBlendHierarchy
{
public:
	char name[24];
	CAnimBlendSequence *blendSequences;
	short numSequences;
	short compressed;
	float totalLength;
	CLink_CAnimBlendHierarchy *linkPtr;

	CAnimBlendHierarchy(void);
	~CAnimBlendHierarchy(void);
	void dtor(void);
	void Shutdown(void);
	void SetName(const char *name);
	void CalcTotalTime(void);
	void RemoveQuaternionFlips(void);
	void RemoveAnimSequences(void);
	void Uncompress(void);
	void RemoveUncompressedData(void);

	void CalcTotalTimeCompressed(void);
};

class CAnimBlock
{
public:
	char name[24];
	int animIndex;	// into CAnimBlock::ms_aAnimations[]
	int numAnims;
};

// complete
class CAnimBlendNode
{
public:
	float theta0;
	float theta1;
	int frame0;
	int frame1;
	float time;
	CAnimBlendSequence *sequence;		// !!
	CAnimBlendAssociation *blendAssoc;	// reference to owner

	void CalcDeltasCompressed(void);
	void SetupKeyFrameCompressed(void);
	void FindKeyFrame(float time);
	void GetEndTranslation(CVector *vec, float f);
	void GetCurrentTranslation(CVector *vec, float f);
	void CalcDeltas(void);
	bool NextKeyFrame(void);
	bool Update(CVector &vec, CQuaternion &quat, float f);
	void Init(void);
};

// complete
class CAnimBlendAssociation
{
public:
	enum Flags {
		Partial = 0x10,
		Movement = 0x20
	};
	void *vtable;
	void *next;				// pointer to next "next"
	void *prev;				// pointer to to previous variable pointing to "next"
	int numNodes;				// taken from CAnimBlendClumpData::numFrames
	CAnimBlendNode *nodes; 
	CAnimBlendHierarchy *hierarchy;		// !!
	float blendAmount;
	float blendDelta;
	float currentTime;
	float speed;
	float timeStep;
	int animId;
	int flags;
	int callbackType;
	void (*callback)(CAnimBlendAssociation*, void*);
	void *callbackArg;

	CAnimBlendAssociation(void);
	CAnimBlendAssociation(CAnimBlendAssociation&);
	~CAnimBlendAssociation(void);
	void ctor(void);
	void dtor(void);
	void dtor2(char flag);
	void Init(RpClump *clump, CAnimBlendHierarchy *anim);
	void Init(CAnimBlendAssociation&);
	void AllocateAnimBlendNodeArray(int);
	void FreeAnimBlendNodeArray(void);
	void SetCurrentTime(float time);
	void SyncAnimation(CAnimBlendAssociation *anim);
	CAnimBlendNode *GetNode(int i);
	void Start(float time);
	void SetBlend(float blendAmount, float blendDelta);
	void SetDeleteCallback(void (*callback)(CAnimBlendAssociation*, void*), void *arg);
	void SetFinishCallback(void (*callback)(CAnimBlendAssociation*, void*), void *arg);
	void UpdateTime(float f1, float f2);
	bool UpdateBlend(float f);
};

// complete
class CAnimBlendAssocGroup
{
public:
	CAnimBlendAssociation *assocList;
	int numAssociations;

	CAnimBlendAssocGroup(void);
	~CAnimBlendAssocGroup(void);
	void ctor(void);
	void dtor(void);
	void CreateAssociations(const char *name);
	void CreateAssociations(const char *name, RpClump *clump, char **names, int numAnims);
	CAnimBlendAssociation *GetAnimation(uint i);
	CAnimBlendAssociation *GetAnimation(const char *name);
	CAnimBlendAssociation *CopyAnimation(uint i);
	CAnimBlendAssociation *CopyAnimation(const char *name);
};

struct AnimBlendFrameData
{
	int flag;
	RwV3d pos;
	union {
		RwFrame *frame;
		RpHAnimStdKeyFrame *hanimframe;
	};
	int nodeID;
};

// complete
class CAnimBlendClumpData
{
public:
	void *nextAssoc;			// pointer to CAnimBlendAssociation::next
	void *prevAssoc;
	int numFrames;
	float *d;		// pointer into CPed structure (position probably)
	AnimBlendFrameData *frames;

	CAnimBlendClumpData(void);
	~CAnimBlendClumpData(void);
	void ctor(void);
	void dtor(void);
	void SetNumberOfBones(int n);
	void ForAllFrames(void (*cb)(AnimBlendFrameData*, void*), void *arg);
};

struct AnimAssocDefinition
{
	char *name;
	char *blockName;
	int modelIndex;
	int animCount;
	char **animNames;
	struct {
		int animId;
		int flags;
	} *animInfoList;
};

// complete
class CAnimManager
{
public:
	static void Initialise(void);
	static void Shutdown(void);
	static void UncompressAnimation(CAnimBlendHierarchy *hier);
	static CAnimBlock *GetAnimationBlock(const char *name);
	static CAnimBlendHierarchy *GetAnimation(const char *name, CAnimBlock *animBlock);
	static const char *GetAnimGroupName(int i);
	static CAnimBlendAssociation *CreateAnimAssociation(int groupId, int animId);
	static CAnimBlendAssociation *GetAnimAssociation(int groupId, int animId);
	static CAnimBlendAssociation *GetAnimAssociation(int groupId, const char *name);
	static CAnimBlendAssociation *AddAnimation(RpClump *clump, int groupId, int animId);
	static CAnimBlendAssociation *AddAnimationAndSync(RpClump *clump, CAnimBlendAssociation *, int groupId, int animId);
	static CAnimBlendAssociation *BlendAnimation(RpClump *clump, int groupId, int animId, float speed);
	static void LoadAnimFiles(void);
	static void LoadAnimFile(const char*);
	static void LoadAnimFile(int, bool);
	static void RemoveLastAnimFile(void);

	static int &ms_numAnimations;
	static CAnimBlendHierarchy *ms_aAnimations;
	static int &ms_numAnimBlocks;
	static CAnimBlock *ms_aAnimBlocks;
	static CAnimBlendAssocGroup *&ms_aAnimAssocGroups;
	static AnimAssocDefinition *ms_aAnimAssocDefinitions;
	static CLinkList_CAnimBlendHierarchy *ms_animCache;
};
