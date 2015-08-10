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
	RpClump *clone = RpClumpClone(clump);
	if(IsClumpSkinned(clone)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromClump(clone);
		RpClumpForAllAtomics(clone, SetHierarchyForSkinAtomic, hier);
		RpHAnimAnimation *anim = HAnimAnimationCreateForHierarchy(hier);
		RpHAnimHierarchySetCurrentAnim(hier, anim);
		RpHAnimHierarchySetFlags(hier, rpHANIMHIERARCHYUPDATEMODELLINGMATRICES|rpHANIMHIERARCHYUPDATELTMS);
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
	if(strcmp((char*)(self+4), "playerh") == 0)
		RpClumpForAllAtomics(clump, (RpAtomicCallBack)0x4F8940, (void*)0x528B30);	// CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins::RenderPlayerCB

	if(IsClumpSkinned(clump)){
		RpHAnimHierarchy *hier;
		RwFrameForAllChildren(RpClumpGetFrame(clump), GetAnimHierarchyFromClumpCB, &hier);
		RpClumpForAllAtomics(clump, SetHierarchyForSkinAtomic, hier);
		RpAtomic *atomic = GetFirstAtomic(clump);	// *not* what the xbox does :(
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
		// TODO: find (and detach) limbs
	}
	CClumpModelInfo__SetClump(self, 0, clump);
	CClumpModelInfo__SetFrameIds(self, 0, 0x5FE7A4);	// CPedModelInfo::m_pPedIds
	if(*(int*)(self + 68) == 0 && !IsClumpSkinned(clump))
		CPedModelInfo__CreateHitColModel(self);

	if(strcmp((char*)(self+4), "player") == 0)
		RpClumpForAllAtomics(clump, (RpAtomicCallBack)0x4F8940, (void*)0x528B30);	// CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins::RenderPlayerCB

}