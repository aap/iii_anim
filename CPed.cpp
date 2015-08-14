#include "iii_anim.h"

WRAPPER void __fastcall CEntity__Render(void *self) { EAXJMP(0x474BD0); }

int &CPools__ms_pPedPool = *(int*)0x8F2C60;
WRAPPER int __fastcall CPool_CPed_CPlayerPed_GetIndex(int self, int, CPed *ped) { EAXJMP(0x43EB70); }

RpAtomic *weaponAtomics[140];

void
CPed::renderLimb(int node)
{
	RpAtomic *atomic;
	CPedModelInfo *pedinfo = (CPedModelInfo*)CModelInfo::ms_modelInfoPtrs[this->modelID];
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
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(this->clump);
	int idx = RpHAnimIDGetIndex(hier, this->frames[node]->nodeID);
	RwMatrix *mat = &RpHAnimHierarchyGetMatrixArray(hier)[idx];
	RwFrame *frame = RpAtomicGetFrame(atomic);
	memcpy(RwFrameGetMatrix(frame), mat, 64);
	RwFrameUpdateObjects(frame);
	atomic->renderCallBack(atomic);
}

void __fastcall
cped__render__hook(CPed* ped)
{
	CEntity__Render(ped);
	if(IsClumpSkinned(ped->clump)){
		ped->renderLimb(2);
		ped->renderLimb(5);
		ped->renderLimb(6);
		int pedid = CPool_CPed_CPlayerPed_GetIndex(CPools__ms_pPedPool, 0, ped);
		RpAtomic *atomic = weaponAtomics[pedid];
		if(atomic){
			RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ped->clump);
			int idx = RpHAnimIDGetIndex(hier, ped->frames[6]->nodeID);
			RwMatrix *mat = &RpHAnimHierarchyGetMatrixArray(hier)[idx];
			RwFrame *frame = RpAtomicGetFrame(atomic);
			memcpy(RwFrameGetMatrix(frame), mat, 64);
			RwFrameUpdateObjects(frame);
			atomic->renderCallBack(atomic);
		}
	}
}

void
CPed::AddWeaponModel(int id)
{
	if(id == -1)
		return;
	CPedModelInfo *pedInfo = (CPedModelInfo*)CModelInfo::ms_modelInfoPtrs[id];
	RpAtomic *atomic = ((RpAtomic* (__thiscall*)(void*)) pedInfo->vtable[3])(pedInfo);
	if(IsClumpSkinned(this->clump)){
		int pedid = CPool_CPed_CPlayerPed_GetIndex(CPools__ms_pPedPool, 0, this);
		weaponAtomics[pedid] = atomic;
	}else{
		RwFrameDestroy(RpAtomicGetFrame(atomic));
		RpAtomicSetFrame(atomic, this->frames[6]->frame);
		RpClumpAddAtomic(this->clump, atomic);
	}
	this->weaponModelId = id;
}

void
CPed::RemoveWeaponModel(int id)
{
	this->weaponModelId = -1;
	if(IsClumpSkinned(this->clump)){
		int pedid = CPool_CPed_CPlayerPed_GetIndex(CPools__ms_pPedPool, 0, this);
		RpAtomic *atomic = weaponAtomics[pedid];
		if(atomic){
			RwFrame *frame = RpAtomicGetFrame(atomic);
			RpAtomicDestroy(atomic);
			RwFrameDestroy(frame);
			weaponAtomics[pedid] = NULL;
		}
	}else{
		RwFrameForAllObjects(this->frames[6]->frame, (RwObjectCallBack)0x4CF950, NULL);	// RemoveAllModelCB
	}
}

void
pedhooks(void)
{
	// call to CEntity::Render() in CPed::Render
	MemoryVP::InjectHook(0x4CF8F0, &CPed::AddWeaponModel, PATCH_JUMP);
	MemoryVP::InjectHook(0x4CF980, &CPed::RemoveWeaponModel, PATCH_JUMP);
	MemoryVP::InjectHook(0x4D0484, cped__render__hook);
}