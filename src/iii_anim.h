#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#pragma warning(disable: 4996)  // POSIX name


#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <rwcore.h>
#include <rwplcore.h>
#include <rpworld.h>
#include <rpskin.h>
#include <rphanim.h>
#include <new>
#include <string>

typedef unsigned int uint;
typedef uintptr_t addr;

#include "MemoryMgr.h"

#define ADAPTHIERARCHY

#define MAXOBJECTS 450
#define MAXPEDMODELS 90
#define MAXPEDS 140



#define RAD2DEG(x) (180.0f*(x)/M_PI)

void pedhooks(void);
void pedikhooks(void);
void objecthooks(void);
void handhooks(void);

#define RwEngineInstance (*rwengine)
extern void **rwengine;

extern int &CTimer__m_FrameCounter;

void *RwMallocAlign(uint size, int alignment);
void RwFreeAlign(void*);
void gtadelete(void*);
void *gta_nw(int);

// from skygfx for rim pipeline
extern void (*AttachRimPipeToRwObject)(RwObject *obj);

const char *GetFrameNodeName(RwFrame *frame);
void *GetModelFromName(char *name);
RpAtomic *GetFirstAtomic(RpClump *clump);
RpAtomic *IsClumpSkinned(RpClump*);
RpHAnimHierarchy* GetAnimHierarchyFromSkinClump(RpClump *clump);
RpHAnimHierarchy* GetAnimHierarchyFromClump(RpClump *clump);
const char *ConvertBoneTag2BoneName(int tag);
int ConvertPedNode2BoneTag(int node);

void DeleteRwObject_hook(RpClump *clump);

struct RpSkin
{
	RwUInt32 numBones;
	RwMatrix *inverseMatrices;
	RwUInt8 *indices;
	RwMatrixWeights *weights;
	RwInt32 maxWeightsUsed;
	RwInt32 singleIndex;
	void *data;
};

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

struct CMatrix
{
	RwMatrix matrix;
	RwMatrix *pMatrix;
	bool haveRwMatrix;

	void ctor(RwMatrix *, bool);
	void dtor(void);
	void RotateX(float);
	void SetRotateY(float);
	void SetRotateZ(float);
	void UpdateRW(void);
	void Update(void);
	void assign(CMatrix*);
	void AttachRW(RwMatrix *matrix, bool temporary);
	void Detach();

	static void mult(CMatrix *out, CMatrix *in1, CMatrix *in2);
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

	CVector() { }

	CVector(float fX, float fY, float fZ) : x(fX), y(fY), z(fZ) { }

	CVector(const RwV3d &v) : x(v.x), y(v.y), z(v.z) { }

	void Add(CVector &v) {
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
	};
	void Normalize(void);
};

class CVector2D {
public:
	float x, y;
};

void CrossProduct(CVector *, CVector *, CVector *);

class CRGBA
{
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

#include "collision.h"

class CAnimBlendSequence;
class CAnimBlendHierarchy;
class CAnimBlock;
class CAnimBlendNode;
class CAnimBlendAssociation;
class CAnimBlendAssocGroup;
struct AnimBlendFrameData;
class CAnimBlendClumpData;
class CAnimManager;

struct CEntity;

struct CCollPoly
{
	CVector vecColPolyPoint1;
	CVector vecColPolyPoint2;
	CVector vecColPolyPoint3;
	bool bIsValidCollision;
};

struct CWeapon
{
	int nWeaponId;
	int nWeaponState;
	int nAmmoLoaded;
	int nAmmoTotal;
	int dwNextShotTime;
	bool bAddRotOffset;
};

struct CPlaceable
{
	void **vtable;
	CMatrix matrix;
};
static_assert(sizeof(CPlaceable) == 0x4C, "CPlaceable: wrong size");

struct CEntity : CPlaceable
{
	RpClump *clump;
	char bfTypeStatus;
	char bfFlagsA;
	char bfFlagsB;
	char bfFlagsC;
	char bfFlagsD;
	char bfFlagsE;
	char __f0052[2];
	short unk1;
	short RandomSeed;
	short nModelIndex;
	short level;
	int pFirstRef;

	void SetModelIndex(int id);
	void Render(void);
	void PreRender(void);
	void UpdateRpHAnim(void);
	void UpdateRwFrame(void);
	bool GetIsOnScreen(void);
};
static_assert(sizeof(CEntity) == 0x64, "CEntity: wrong size");

struct CPhysical : CEntity
{
	int uAudioEntityId;
	int unk1;
	int field_6C;
	int field_70;
	int m_dwLastTimeCollided;
	CVector vecMoveSpeed;
	CVector vecTurnSpeed;
	CVector vecShiftVector1;
	CVector vecShiftVector2;
	CVector vecShiftVector3;
	CVector vecShiftVector4;
	float fMass;
	float fTurnMass;
	float fForceMultiplier;
	float fAirResistance;
	int fElasticity;
	float fPercentSubmerged;
	CVector vecCentreOfMass;
	void *pEntryInfoNode;
	int pMovingListNode;
	char field_EC;
	char field_ED;
	char uCollidingNum;
	char field_EF;
	CEntity *pCollisionRecords[6];
	float fTotSpeed;
	float fCollisionPower;
	float pPhysColliding;
	CVector vecCollisionPower;
	short wComponentCol;
	char byteMoveFlags;
	char byteCollFlags;
	char byteLastCollType;
	char byteZoneLevel;
	short __padding;

	void ProcessControl(void);
};
static_assert(sizeof(CPhysical) == 0x128, "CPhysical: wrong size");

struct CPed;

struct CPedIK
{
	struct LimbOrientation
	{
		float phi;
		float theta;
	};

	CPed *ped;
	LimbOrientation headOrient;
	LimbOrientation torsoOrient;
	LimbOrientation upperArmOrient;
	LimbOrientation lowerArmOrient;
	int flags;

	static float *ms_headInfo;
	static float *ms_torsoInfo;
	static float *ms_headRestoreInfo;
	static float *ms_upperArmInfo;
	static float *ms_lowerArmInfo;

	void GetComponentPosition(RwV3d *pos, int id);
	int LookInDirection(float phi, float theta);
	void RotateTorso(AnimBlendFrameData*, LimbOrientation*, bool);
	bool RestoreLookAt(void);
	bool PointGunInDirection(float, float);
	bool PointGunInDirectionUsingArm(float, float);
	void RotateHead(void);
	static void __stdcall ExtractYawAndPitchLocal(RwMatrixTag *, float *, float *);
	static void ExtractYawAndPitchLocalSkinned(AnimBlendFrameData *, float *, float *);
	static void __stdcall ExtractYawAndPitchWorld(RwMatrixTag *, float *, float *);
	static RwMatrix *GetWorldMatrix(RwFrame *, RwMatrixTag *);
	static int __stdcall MoveLimb(LimbOrientation *, float, float, float * /* LimbMovementInfo* */);
};

struct CPed : public CPhysical
{
	CCollPoly polyColliding;
	int fCollisionSpeed;
	uchar bfFlagsA;
	uchar bfFlagsB;
	uchar bfFlagsC;
	uchar bfFlagsD;
	uchar bfFlagsE;
	uchar bfFlagsF;
	uchar bfFlagsG;
	uchar bfFlagsH;
	uchar bfFlagsI;
	uchar pad_15D[3];
	uchar bytePedStatus;
	uchar field_161;
	uchar pad_162[2];
	int dwObjective;
	int dwPrevObjective;
	int field_16C;
	int field_170;
	int field_174;
	int field_178;
	int field_17C;
	int field_180;
	int dwPedFormation;
	int dwFearFlags;
	int pThreatEntity;
	float fEventOrThreatX;
	float fEventOrThreatY;
	int dwEventType;
	int pEventEntity;
	float fAngleToEvent;
	AnimBlendFrameData *frames[12];
	int animGroup;
	int pVehicleAnim;
	CVector vecAnimMoveDelta;		// ????
	CVector2D vecOffsetSeek;		// ???? really 2d?
	CPedIK pedIK;
	float fActionX;
	float fActionY;
	int dwActionTimer;
	int dwAction;
	int dwLastAction;
	int dwMoveState;
	int dwStoredActionState;
	int dwPrevActionState;
	int dwWaitState;
	int dwWaitTimer;
	int pPathNodesStates[8];
	char stPathNodeStates[80];
	short wPathNodes;
	short wCurPathNode;
	char bytePathState;
	char pad_2B5[3];
	int pNextPathNode;
	int pLastPathNode;
	float fHealth;
	float fArmour;
	short wRouteLastPoint;
	short wRoutePoints;
	short wRoutePos;
	short wRouteType;
	short wRouteCurDir;
	short field_2D2;
	float fMovedX;
	float fMovedY;
	float fRotationCur;
	float fRotationDest;
	float headingRate;
	short wEnterType;
	short wWalkAroundType;
	int pCurPhysSurface;
	CVector vecOffsetFromPhysSurface;
	int pCurSurface;
	CVector vecSeekVehicle;
	int pSeekTarget;
	int pVehicle;
	char byteIsInVehicle;
	char pad_315[3];
	int field_318;
	char field_31C;
	char field_31D;
	short wPhoneId;
	int dwLookingForPhone;
	int dwPhoneTalkTimer;
	int pLastAccident;
	int dwPedType;
	float *pedStats;
	float fFleeFromPosX;
	float fFleeFromPosY;
	int pFleeFrom;
	int dwFleeTimer;
	int field_344;
	int dwLastThreatTimer;
	int pVehicleColliding;
	char byteStateUnused;
	char pad_351[3];
	int dwTimerUnused;
	int pTargetUnused;
	CWeapon stWeps[13];
	int field_494;
	char field_498;
	char field_499;
	char byteWepSkills;
	char byteWepAccuracy;
	int pPointGunAt;
	CVector vecHitLastPos;
	int dwLastHitState;
	char byteFightFlags1;
	char byteFightFlags2;
	char pad_4B2[2];
	int pPedFire;
	int pPedFight;
	int fLookDirection;
	int weaponModelId;
	int dwLeaveCarTimer;
	int dwGetUpTimer;
	int dwLookTimer;
	int dwStandardTimer;
	int dwAttackTimer;
	int dwLastHitTime;
	int dwHitRecoverTimer;
	int field_4E0;
	int field_4E4;
	int field_4E8;
	int field_4EC;
	char field_4F0;
	char field_4F1;
	char byteBodyPartBleeding;
	char field_4F3;
	int pNearPeds[10];
	short nNearPeds;
	char byteLastDamWep;
	char pad_51F;
	char field_520;
	char pad_521[3];
	int dwTalkTimer;
	short wTalkTypeLast;
	short wTalkType;
	CVector vecSeekPosEx;
	float fSeekExAngle;
	int field_53C;

	CPed *ctor(uint type);
	void SetModelIndex(int id);
	void SetPedStats(int x);
	void renderLimb(int node);
	void AddWeaponModel(int id);
	void RemoveWeaponModel(int i);
	bool IsPedHeadAbovePos(float dist);
	char DoesLOSBulletHitPed(CColPoint *colpoint);
	void RemoveBodyPart(int nodeId, bool unk);
	void SpawnFlyingComponent(int nodeId, bool unk);
	bool UseGroundColModel(void);

	CColModel *GetColModel(void);

	CPed *ctor_orig(uint type);
};
static_assert(sizeof(CPed) == 0x540, "CPed: wrong size");

struct CPlayerPed : public CPed
{
	void ProcessControl_hook(void);
};

struct CObject : public CPhysical
{
	int field_128;
	int field_12C;
	int field_130;
	int field_134;
	int field_138;
	int field_13C;
	int field_140;
	int field_144;
	int field_148;
	int field_14C;
	int field_150;
	int field_154;
	int field_158;
	int field_15C;
	int field_160;
	int field_164;
	int field_168;
	int field_16C;
	int field_170;
	int field_174;
	int field_178;
	int field_17C;
	int field_180;
	int field_184;
	int field_188;
	int field_18C;
	int field_190;
	int field_194;

	void Render();
};
static_assert(sizeof(CObject) == 0x198, "CObject: wrong size");

int CPools__GetObjectRef(CObject *object);

struct CCutsceneObject : public CObject
{
	struct ObjectExt
	{
		// CCutsceneObject
		bool renderHead;
		bool renderRightHand;
		bool renderLeftHand;

		// CCutsceneHead
		int b;		// unknown
		CCutsceneObject *object;
		int d;		// unknown
		int isSkinned;

		// CCutsceneHand
		CCutsceneObject *m_pObject;
		int m_Hand;
		RwFrame *m_pHandFrame;
		int m_bIsSkinned;
		int m_nPedIndex;
	};

	CCutsceneObject *ctor(void);
	void ProcessControl(void);
	void Render(void);
	void PreRender();

	static ObjectExt objectExt[MAXOBJECTS];
	ObjectExt *getExt(void){ return &objectExt[CPools__GetObjectRef(this) >> 8]; }
	void SetRenderHead(bool r) { getExt()->renderHead = r; }
	bool GetRenderHead(void) { return getExt()->renderHead; }
	void SetRenderRightHand(bool r) { getExt()->renderRightHand = r; }
	bool GetRenderRightHand(void) { return getExt()->renderRightHand; }
	void SetRenderLeftHand(bool r) { getExt()->renderLeftHand = r; }
	bool GetRenderLeftHand(void) { return getExt()->renderLeftHand; }
	void RenderLimb(int id);

	void ctor_orig(void);
	void dtor(void);
	void ProcessControl_orig(void);
};
static_assert(sizeof(CCutsceneObject) == 0x198, "CCutsceneObject: wrong size");

struct CCutsceneHead : public CCutsceneObject
{
	RwFrame *head;

	CCutsceneHead *ctor(CObject *object);
	void ProcessControl(void);
	void Render(void);
};
static_assert(sizeof(CCutsceneHead) == 0x19C, "CCutsceneHead: wrong size");

class CRunningScript
{
public:
	CRunningScript *m_pNext;
	CRunningScript *m_pPrev;
	char m_szScriptName[8];
	unsigned int *m_pIp;
	char *m_aGosubStack[6];
	unsigned __int16 m_nGosubIndex;
	char _pad0[2];
	int m_aLocalVars[18];
	char m_bCompareFlag;
	char m_bMissionFlag;
	char m_bNotSleeps;
	char _pad1[1];
	unsigned int m_nWakeTime;
	unsigned __int16 m_nLogicalOp;
	char m_bNegFlag;
	char m_bWastedBustedCheck;
	char m_bWastedOrBusted;
	char m_bMissionThread;
	short field_86;
};
static_assert(sizeof(CRunningScript) == 0x88, "CRunningScript: wrong size");


#define MAX_HANDS        (2)
#define MAX_GENDERS      (2)
#define MAX_RACES        (2)
#define MAX_PROPS        (2)
#define MAX_CUTSCENEPEDS (30) //5

class CCutsceneHand : public CCutsceneObject
{
public:
	enum Handedness
	{
		HANDEDNESS_LEFT = 0,
		HANDEDNESS_RIGHT,
		HANDEDNESS_BOTH,
		HANDEDNESS_UNDEFINED,
	};

	enum Gender
	{
		GENDER_MALE = 0,
		GENDER_FEMALE,
		GENDER_SHEMALE,
	};

	enum Race
	{
		RACE_BLACK = 0,
		RACE_WHITE,
	};

	enum Stature
	{
		STATURE_0 = 0,
		STATURE_1
	};

	enum Prop
	{
		PROP_NONE = 0,
		PROP_STICK,
	};

	class HandInstance
	{
	public:
		RpAtomic *m_pAtomic;
		DWORD field_4;
		char m_bHasHands;
		CRGBA m_Color;
		char _pad0[3];
		CVector m_vecPos;
		float m_fAnimTime;
		char m_aName[15];
		char _pad1[1];
		CCutsceneHand::Gender m_Gender;
		CCutsceneHand::Race m_Race;
		CCutsceneHand::Prop m_Prop;
		CCutsceneHand::Stature m_Stature;
		CVector m_vecScale;

		HandInstance()
		{
			Initialise();
		}

		void Initialise();
	};

	static HandInstance Instances[MAX_CUTSCENEPEDS][MAX_HANDS];

	static void Init();
	static void InitXML();
	static void CleanUp();

	static void SetupTxd();
	static void DestroyTxd();

	static void InitialiseCharacters();

	CCutsceneHand *hand_ctor(CObject *object, Handedness hand);
	void handdtor();
	CCutsceneHand *hand_virtual_dtor(char flag);

	void SetModelIndex(unsigned int mi);
	void CreateRwObject();
	void DeleteRwObject();
	void ProcessControl();
	void PreRender();
	void Render();

	static RwMatrix *GetBoneMatrix(RpClump *clump, int bone);
	static void CopyBoneMatrix(RpClump *clump, int bone, RwMatrix *out);
	void UpdateSkin();

	void SetSkinnedModel(int pedIndex, Handedness hand, Gender gender, Race race, Stature stature, Prop prop);
	static RpAtomic *FindHandAtomicByName(RpAtomic *atomic, void *data);
	void InitialiseHandAnimation();

	static int GetModelIndexFromName(const std::string &name);

	static bool IsCutsceneRunning(const std::string &name);

	int GetIdForCharacter(int id)
	{
		return ( id > 0 ) ? id - 25 : 0;
	}

	void ConfigureForCharacter(int id);
	void SetColour(class HandState &hs);
	void SetAnimationTime(class HandState &hs);
	void SetModel(class HandState &hs);
	void SetScale(class HandState &hs);
	void SetPosition(class HandState &hs);
};

static_assert(sizeof(CCutsceneHand) == 0x198, "CCutsceneHand: wrong size");
//
// Model Info
//

struct CBaseModelInfo
{
	void **vtable;
	char     name[24];	// no idea what the size really is
	CColModel *colModel;
	void *twodeffect;
	short id;
	ushort refCount;
	short txdSlot;
	uchar type;
	uchar num2dEffects;
	bool freeCol;
};
static_assert(sizeof(CBaseModelInfo) == 0x30, "CBaseModelInfo: wrong size");

struct RwObjectNameIdAssocation
{
	char *name;
	int id;
	int flags;
};

struct RwObjectAssociation
{
	union {
		char *name;
		int id;
	};
	RwFrame *out;
};

struct CClumpModelInfo : public CBaseModelInfo
{
	RpClump *clump;

	RpClump *CreateInstance(void);
	void SetClump(RpClump*);
	void SetFrameIds(RwObjectNameIdAssocation *ids);

	static RpAtomic *SetAtomicRendererCB(RpAtomic*, void*);
	static RwFrame *FindFrameFromNameCB(RwFrame *frame, void *data);
	static RwFrame *FindFrameFromIdCB(RwFrame *frame, void *data);
};
static_assert(sizeof(CClumpModelInfo) == 0x34, "CBaseModelInfo: wrong size");

struct CPedModelInfo : public CClumpModelInfo
{
	int animGroup;
	int unk1;
	int pedStats;
	int unk2;
	CColModel *hitColModel;
	RpAtomic *head;
	RpAtomic *lhand;
	RpAtomic *rhand;

	CPedModelInfo(void);
	void SetClump(RpClump *clump);
	void DeleteRwObject(void);
	void CreateHitColModel(void);
	void CreateHitColModelSkinned(RpClump *clump);

	CColModel *AnimatePedColModelSkinned(RpClump *clump);
	CColModel *AnimatePedColModelSkinnedWorld(RpClump *clump);
	CColModel *AnimatePedColModel(CColModel *colmodel, RwFrame *frame);
	CColModel *AnimatePedColModelWorld(CColModel *colmodel, RwFrame *frame);

	static RwObjectNameIdAssocation m_pPedIds[12];
};

struct CStore_PedModelInfo {
	int numElements;
	CPedModelInfo objects[MAXPEDMODELS];
};

struct CModelInfo
{
	static CPedModelInfo *AddPedModel(int id);
	static void **ms_modelInfoPtrs;

	static CStore_PedModelInfo ms_pedModelStore;
};

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

void CVisibilityPlugins__SetClumpModelInfo(RpClump *clump, int clumpModelInfo);
RpAtomic *CVisibilityPlugins__RenderPlayerCB(RpAtomic*);
void CVisibilityPlugins__RenderAlphaAtomic(RpAtomic*,int);
int CVisibilityPlugins__GetClumpAlpha(RpClump* clump);
//void CVisibilityPlugins__SetClumpAlpha(RpClump* clump, int alpha);

//extern RpAtomic *atomicArray[20];
//extern int atomicArraySP;
//void atomicsToArray(RpClump *clump);

enum BoneTag {
	BONE_Swaist,
	BONE_Supperlegr,
	BONE_Slowerlegr,
	BONE_Sfootr,
	BONE_Supperlegl,
	BONE_Slowerlegl,
	BONE_Sfootl,
	BONE_Smid,
	BONE_Storso,
	BONE_Shead,
	BONE_Supperarmr,
	BONE_Slowerarmr,
	BONE_SRhand,
	BONE_Supperarml,
	BONE_Slowerarml,
	BONE_SLhand,
};

enum PedNode {
	PED_WAIST,
	PED_TORSO,	// Smid on PS2/PC, Storso on mobile/xbox. We follow mobile/xbox (makes kicking on ground look better)
	PED_HEAD,
	PED_UPPERARML,
	PED_UPPERARMR,
	PED_HANDL,
	PED_HANDR,
	PED_UPPERLEGL,
	PED_UPPERLEGR,
	PED_FOOTL,
	PED_FOOTR,
	PED_LOWERLEGR,
	// This is not valid apparently
	PED_LOWERLEGL,
	// actual fixed nodes needed for hit col model
	PED_ACTUAL_TORSO,
	PED_ACTUAL_MID,
};

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
	CAnimBlendAssociation::CAnimBlendAssociation(CAnimBlendAssociation &a, RpClump *clump);
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

	void CopyForClump(CAnimBlendAssociation &anim, RpClump *clump);
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
	CAnimBlendAssociation *CopyAnimation(uint i, RpClump *clump);
	CAnimBlendAssociation *CopyAnimation(const char *name, RpClump *clump);
	// need non-overloaded function for inline assembly
	CAnimBlendAssociation *CopyAnimationNoClump(const char *name) { return CopyAnimation(name); }
	CAnimBlendAssociation *CopyAnimationForClump(const char *name, RpClump *clump) { return CopyAnimation(name, clump); }
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
	void *nextAssoc;	// pointer to CAnimBlendAssociation::next
	void *prevAssoc;
	int numFrames;
	// xbox has a 4b field here
	CVector *pedPosition;
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
	static CAnimBlendAssociation *CreateAnimAssociation(int groupId, int animId, RpClump *clump);
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
	static AnimAssocDefinition *ms_aAnimAssocDefinitions;	// 25
	static CLinkList_CAnimBlendHierarchy *ms_animCache;
};
