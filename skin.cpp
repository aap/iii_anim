#include "iii_anim.h"

static RwFrame*
GetAnimHierarchyFromClumpCB(RwFrame *frame, void *data)
{
	RpHAnimHierarchy *hier = RpHAnimFrameGetHierarchy(frame);
	if(hier){
		*(RpHAnimHierarchy**)data = hier;
		return NULL;
	}
	RwFrameForAllChildren(frame, GetAnimHierarchyFromClumpCB, data);
	return frame;
}

static RpHAnimHierarchy*
GetAnimHierarchyFromClump(RpClump *clump)
{
	RpHAnimHierarchy *retval = NULL;
	RwFrameForAllChildren(RpClumpGetFrame(clump), GetAnimHierarchyFromClumpCB, &retval);
	return retval;
}

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

RpClump* __fastcall
CClumpModelInfo__CreateInstance(int self)
{
	RpClump *clump = *(RpClump**)(self + 48);
	if(clump == NULL)
		return NULL;

	RpClump *clone2 = RpClumpClone(clump);
	RpClump *clone = RpClumpClone(clone2);	// to reverse order of... something again...
	RpClumpDestroy(clone2);
//	clone = clone2;

//	RpClump *clone = RpClumpClone(clump);
	RpAtomic *atomic;
	if(atomic = IsClumpSkinned(clone)){
//		atomicsToArray(clump);
//		atomicsToArray(clone2);
//		int count1 = atomicArraySP;
//		atomicsToArray(clone);
//		int count2 = atomicArraySP;
//		assert(count1 == count2);
		RpHAnimHierarchy *hier = GetAnimHierarchyFromClump(clone);
//		RpSkinAtomicSetHAnimHierarchy(atomic, hier);
		RpClumpForAllAtomics(clone, SetHierarchyForSkinAtomic, hier);	// first atomic hardcoded!!
		RpHAnimAnimation *anim = HAnimAnimationCreateForHierarchy(hier);
		RpHAnimHierarchySetCurrentAnim(hier, anim);
		RpHAnimHierarchySetFlags(hier, rpHANIMHIERARCHYUPDATEMODELLINGMATRICES|rpHANIMHIERARCHYUPDATELTMS);
		// Xbox has more some more code (skin related)
	}
	return clone;	
}

WRAPPER void CVisibilityPlugins__SetClumpModelInfo(RpClump *clump, int clumpModelInfo) { EAXJMP(0x528ED0); }
WRAPPER void __fastcall CBaseModelInfo__AddTexDictionaryRef(int self) { EAXJMP(0x4F6B80); }
WRAPPER void __fastcall CClumpModelInfo__SetFrameIds(int self, int, int ids) { EAXJMP(0x4F8BB0); }
WRAPPER void __fastcall CPedModelInfo__CreateHitColModel(int self) { EAXJMP(0x5104D0); }

void __fastcall
CClumpModelInfo__SetClump(int self, int, RpClump *clump)
{
	*(RpClump**)(self + 48) = clump;
	CVisibilityPlugins__SetClumpModelInfo(clump, self);
	CBaseModelInfo__AddTexDictionaryRef(self);
	RpClumpForAllAtomics(clump, (RpAtomicCallBack)0x4F8940, 0);	// CClumpModelInfo::SetAtomicRendererCB
	// Xbox does something depending on *(self+42) here
	if(strcmp((char*)(self+4), "playerh") == 0)
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

RpAtomic *limbAtomics[3];

struct LimbCBarg {
	int pedmodelinfo;
	RpClump *clump;
	RwInt32 id[3];
};

RpAtomic*
CPedModelInfo__findLimbsCb(RpAtomic *atomic, void *data)
{
	LimbCBarg *limbs = (LimbCBarg*)data;
	RwFrame *frame = RpAtomicGetFrame(atomic);
	const char *name = GetFrameNodeName(frame);
	if(lcstrcmp(name, "Shead01") == 0){
		RpHAnimFrameSetID(frame, 0x1000);
		limbs->id[0] = RpHAnimFrameGetID(frame);
		limbAtomics[0] = atomic;
	}else if(lcstrcmp(name, "SLhand01") == 0){
		RpHAnimFrameSetID(frame, 0x1001);
		limbs->id[1] = RpHAnimFrameGetID(frame);
		limbAtomics[1] = atomic;
	}else if(lcstrcmp(name, "SRhand01") == 0){
		RpHAnimFrameSetID(frame, 0x1002);
		limbs->id[2] = RpHAnimFrameGetID(frame);
		limbAtomics[2] = atomic;
	}else
		return atomic;
	RpClumpRemoveAtomic(limbs->clump, atomic);
	RwFrameRemoveChild(frame);
//	atomic->renderCallBack(atomic);	// wtf? android does this...for instancing or what?
	return atomic;
}

void __fastcall
CPedModelInfo__SetClump(int self, int, RpClump *clump)
{
/*
	CClumpModelInfo__SetClump(self, 0, clump);
	CClumpModelInfo__SetFrameIds(self, 0, 0x5FE7A4);	// CPedModelInfo::m_pPedIds
	if(*(int*)(self + 68) == 0)
		CPedModelInfo__CreateHitColModel(self);
	if(strcmp((char*)(self+4), "player") == 0)
		RpClumpForAllAtomics(clump, (RpAtomicCallBack)0x4F8940, (void*)0x528B30);	// CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins::RenderPlayerCB
*/
	if(IsClumpSkinned(clump)){
		LimbCBarg limbs = { self, clump, 0, 0, 0 };
		RpClumpForAllAtomics(clump, CPedModelInfo__findLimbsCb, &limbs);
		RpAtomic *skinned = GetFirstAtomic(clump);
		RpClumpRemoveAtomic(clump, skinned);
		RpClumpAddAtomic(clump, limbAtomics[0]);
		RpClumpAddAtomic(clump, limbAtomics[1]);
		RpClumpAddAtomic(clump, limbAtomics[2]);
		RpClumpAddAtomic(clump, skinned);
	}
	CClumpModelInfo__SetClump(self, 0, clump);
	CClumpModelInfo__SetFrameIds(self, 0, 0x5FE7A4);	// CPedModelInfo::m_pPedIds
	if(*(int*)(self + 68) == 0 && !IsClumpSkinned(clump))
		CPedModelInfo__CreateHitColModel(self);
	// Xbox
	if(strcmp((char*)(self+4), "player") == 0)
		RpClumpForAllAtomics(clump, (RpAtomicCallBack)0x4F8940, (void*)0x528B30);	// CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins::RenderPlayerCB

}

/* indices:  SHead01: 2
             SLHand01: 5
             SRHand01: 6
*/

void
updateLimbs(RpClump *clump)
{
//	static RwInt32 indices[] = { 2, 5, 6 };
	static RwInt32 indices[] = { 9, 15, 12 };
	atomicsToArray(clump);
	RpHAnimHierarchy *hier = GetAnimHierarchyFromClump(clump);
	RwMatrix *matrices = RpHAnimHierarchyGetMatrixArray(hier);
	for(int i = 1; i < atomicArraySP; i++){
		RpAtomic *atomic = atomicArray[i];
		RwFrame *frame = RpAtomicGetFrame(atomic);
		RwInt32 id = RpHAnimFrameGetID(frame);
//		RwInt32 idx = indices[id-0x1000];
		RwInt32 idx = RpHAnimIDGetIndex(hier, indices[id-0x1000]);
//		RwMatrixTransform(RwFrameGetMatrix(frame), &matrices[idx], rwCOMBINEREPLACE);
		memcpy(RwFrameGetMatrix(frame), &matrices[idx], 64);
		RwFrameUpdateObjects(frame);
	}
}
