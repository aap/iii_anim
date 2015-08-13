#include "iii_anim.h"

WRAPPER void __fastcall CEntity__Render(int self) { EAXJMP(0x474BD0); }

void
renderLimb(int cped, int node)
{
	RpAtomic *atomic;
	CPedModelInfo *pedinfo = (CPedModelInfo*)CModelInfo::ms_modelInfoPtrs[*(RwUInt16*)(cped+92)];
	switch(node){
	case 2:
		atomic = pedinfo->head;
		break;
	case 5:
		atomic = pedinfo->lhand;
		break;
	case 6:
		atomic = pedinfo->rhand;
		break;
	default:
		return;
	}
	if(atomic == NULL)
		return;
	RpClump *clump = *(RpClump**)(cped + 0x4C);
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(clump);
	AnimBlendFrameData **frames = (AnimBlendFrameData **)(cped+0x1A4);
	int idx = RpHAnimIDGetIndex(hier, frames[node]->nodeID);
	RwMatrix *mat = &RpHAnimHierarchyGetMatrixArray(hier)[idx];
	RwFrame *frame = RpAtomicGetFrame(atomic);
	memcpy(RwFrameGetMatrix(frame), mat, 64);
	RwFrameUpdateObjects(frame);
	atomic->renderCallBack(atomic);
}

void __fastcall
cped__render__hook(int cped)
{
	CEntity__Render(cped);
	RpClump *clump = *(RpClump**)(cped + 0x4C);
	if(IsClumpSkinned(clump)){
		renderLimb(cped, 2);
		renderLimb(cped, 5);
		renderLimb(cped, 6);
	}
}

void
pedhooks(void)
{
	// call to CEntity::Render() in CPed::Render
	MemoryVP::InjectHook(0x4D0484, cped__render__hook);
}