#include "iii_anim.h"

WRAPPER void CVisibilityPlugins__SetClumpModelInfo(RpClump *clump, int clumpModelInfo) { EAXJMP(0x528ED0); }
WRAPPER void __fastcall CBaseModelInfo__AddTexDictionaryRef(int self) { EAXJMP(0x4F6B80); }
WRAPPER void CClumpModelInfo::SetFrameIds(int ids) { EAXJMP(0x4F8BB0); }
WRAPPER void __fastcall CPedModelInfo__CreateHitColModel(int self) { EAXJMP(0x5104D0); }
WRAPPER void __fastcall CPedModelInfo__DeleteRwObject_orig(CPedModelInfo*) { EAXJMP(0x510280); }

static RpAtomic*
SetHierarchyForSkinAtomic(RpAtomic *atomic, void *data)
{
	RpSkinAtomicSetHAnimHierarchy(atomic, (RpHAnimHierarchy*)data);
	return NULL;
}

RpHAnimAnimation*
HAnimAnimationCreateForHierarchy(RpHAnimHierarchy *hier)
{
	RpHAnimAnimation *anim = RpHAnimAnimationCreate(rpHANIMSTDKEYFRAMETYPEID, hier->numNodes, 0, 0.0f);
	if(anim == NULL)
		return NULL;
	RpHAnimStdKeyFrame *frame = (RpHAnimStdKeyFrame*)anim->pFrames;
	for(int i = 0; i < hier->numNodes; i++){
		frame->q.real = 1.0f;
		frame->q.imag.x = frame->q.imag.y = frame->q.imag.z = 0.0f;
		frame->t.x = frame->t.y = frame->t.z = 0.0f;
		frame->time = 0.0f;
		frame->prevFrame = NULL;
		frame++;
	}
	return anim;
}

RpClump*
CClumpModelInfo::CreateInstance(void)
{
	RpClump *clump = this->clump;
	if(clump == NULL)
		return NULL;

	RpClump *clone2 = RpClumpClone(clump);
	RpClump *clone = RpClumpClone(clone2);	// to reverse order of... something again...
	RpClumpDestroy(clone2);
//	clone = clone2;

//	RpClump *clone = RpClumpClone(clump);
	if(IsClumpSkinned(clone)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromClump(clone);
		RpClumpForAllAtomics(clone, SetHierarchyForSkinAtomic, hier);
		RpHAnimAnimation *anim = HAnimAnimationCreateForHierarchy(hier);
		RpHAnimHierarchySetCurrentAnim(hier, anim);
		RpHAnimHierarchySetFlags(hier, rpHANIMHIERARCHYUPDATEMODELLINGMATRICES|rpHANIMHIERARCHYUPDATELTMS);
		// Xbox has more some more code (skin related)
	}
	return clone;	
}

void
CClumpModelInfo::SetClump(RpClump *clump)
{
	this->clump = clump;
	CVisibilityPlugins__SetClumpModelInfo(clump, (int)this);
	CBaseModelInfo__AddTexDictionaryRef((int)this);
	RpClumpForAllAtomics(clump, (RpAtomicCallBack)0x4F8940, 0);	// CClumpModelInfo::SetAtomicRendererCB
	// Xbox does something depending on *(this+42) here
	if(strcmp(this->name, "playerh") == 0)
		RpClumpForAllAtomics(clump, (RpAtomicCallBack)0x4F8940, (void*)0x528B30);	// CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins::RenderPlayerCB

	if(IsClumpSkinned(clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromClump(clump);
		// mobile
		RpClumpForAllAtomics(clump, SetHierarchyForSkinAtomic, hier);
		RpAtomic *atomic = GetFirstAtomic(clump);
		// Xbox
//		RpAtomic *atomic = IsClumpSkinned(clump);
//		RpSkinAtomicSetHAnimHierarchy(atomic, hier);

		RpSkin *skin = RpSkinGeometryGetSkin(atomic->geometry);
		// ignore const, lol
		RwMatrixWeights *weights = (RwMatrixWeights*)RpSkinGetVertexBoneWeights(skin);
		for(int i = 0; i < atomic->geometry->numVertices; i++){
			RwReal sum = weights->w0 + weights->w1 + weights->w2 + weights->w3;
			weights->w0 /= sum;
			weights->w1 /= sum;
			weights->w2 /= sum;
			weights->w3 /= sum;
			weights++;
		}
		RpHAnimHierarchySetFlags(hier, rpHANIMHIERARCHYUPDATEMODELLINGMATRICES|rpHANIMHIERARCHYUPDATELTMS);
	}
}

//RpAtomic *limbAtomics[3];

struct LimbCBarg {
	CPedModelInfo *pedmodelinfo;
	RpClump *clump;
	RpAtomic *atomics[3];
};

RpAtomic*
CPedModelInfo__findLimbsCb(RpAtomic *atomic, void *data)
{
	LimbCBarg *limbs = (LimbCBarg*)data;
	RwFrame *frame = RpAtomicGetFrame(atomic);
	const char *name = GetFrameNodeName(frame);
	if(lcstrcmp(name, "Shead01") == 0){
		limbs->atomics[0] = atomic;
		limbs->pedmodelinfo->head = atomic;
	}else if(lcstrcmp(name, "SLhand01") == 0){
		limbs->atomics[1] = atomic;
		limbs->pedmodelinfo->lhand = atomic;
	}else if(lcstrcmp(name, "SRhand01") == 0){
		limbs->atomics[2] = atomic;
		limbs->pedmodelinfo->rhand = atomic;
	}else
		return atomic;
	RpClumpRemoveAtomic(limbs->clump, atomic);
	RwFrameRemoveChild(frame);
	return atomic;
}

void
CPedModelInfo::SetClump(RpClump *clump)
{
	int isplayer = strcmp(this->name, "player") == 0;
	RpAtomic *atomic;
	// set renderCB before removing Atomics from Clump
	if(isplayer)
		RpClumpForAllAtomics(clump, (RpAtomicCallBack)0x4F8940, (void*)0x528B30);	// CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins::RenderPlayerCB
	if(atomic = IsClumpSkinned(clump)){
		RpClumpRemoveAtomic(clump, atomic);
		RpClumpAddAtomic(clump, atomic);
		LimbCBarg limbs = { this, clump, 0, 0, 0 };
		RpClumpForAllAtomics(clump, CPedModelInfo__findLimbsCb, &limbs);
	}
	this->CClumpModelInfo::SetClump(clump);
	this->SetFrameIds(0x5FE7A4);	// CPedModelInfo::m_pPedIds
	if(this->hitColModel == NULL && !IsClumpSkinned(clump))
		CPedModelInfo__CreateHitColModel((int)this);
	// again, because CClumpModelInfo::SetClump resets renderCB
	if(isplayer)
		RpClumpForAllAtomics(clump, (RpAtomicCallBack)0x4F8940, (void*)0x528B30);	// CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins::RenderPlayerCB
}

CPedModelInfo::CPedModelInfo(void)
{
	((void(__thiscall*)(CPedModelInfo*))0x50BFA0)(this);
	this->head = this->lhand = this->rhand = NULL;
}

CStore_PedModelInfo CModelInfo::ms_pedModelStore = { 0 };

CPedModelInfo*
CModelInfo::AddPedModel(int id)
{
	if(CModelInfo::ms_pedModelStore.numElements >= 90)
		printf("Size of this thing:%d needs increasing\n", 90);
	CPedModelInfo *modelInfo;
	modelInfo = &CModelInfo::ms_pedModelStore.objects[CModelInfo::ms_pedModelStore.numElements++];
	modelInfo->clump = NULL;
	CModelInfo::ms_modelInfoPtrs[id] = modelInfo;
	return modelInfo;
}

void
CPedModelInfo::DeleteRwObject(void)
{
	RwFrame *frame;
#define DESTROY(a) \
	if(this->a){\
		frame = RpAtomicGetFrame(this->a);\
		RpAtomicDestroy(this->a);\
		RwFrameDestroy(frame);\
		this->a = NULL;\
	}
	DESTROY(head)
	DESTROY(lhand)
	DESTROY(rhand)
#undef	DESTROY

	CPedModelInfo__DeleteRwObject_orig(this);
}

RpAtomic*
AtomicRemoveAnimFromSkinCB(RpAtomic *atomic, void*)
{
	if(RpSkinGeometryGetSkin(RpAtomicGetGeometry(atomic)) == NULL)
		return atomic;
	RpHAnimHierarchy *hier = RpSkinAtomicGetHAnimHierarchy(atomic);
	if(hier->pCurrentAnim){
		RpHAnimAnimationDestroy(hier->pCurrentAnim);
		hier->pCurrentAnim = NULL;
	}
	return atomic;
}

void
DeleteRwObject_hook(RpClump *clump)
{
	RpClumpForAllAtomics(clump, AtomicRemoveAnimFromSkinCB, NULL);
	RpClumpDestroy(clump);
}