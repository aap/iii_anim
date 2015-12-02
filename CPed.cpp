#include "iii_anim.h"

WRAPPER void CEntity::Render(void) { EAXJMP(0x474BD0); }

void
CEntity::UpdateRpHAnim(void)
{
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(this->clump);
	RpHAnimHierarchyUpdateMatrices(hier);
}

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
	ped->CEntity::Render();
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

void __declspec(naked)
cped_prerender_hook1(void)
{
	_asm{
		add	esp, 0x1C
		mov	ecx, [esp+0xC]
		push	dword ptr [ecx+4Ch]
		call	IsClumpSkinned
		add	esp, 4
		test	eax, eax
		je	x
		mov	ecx, [esp+0xC]
		call	CEntity::UpdateRpHAnim
	x:	mov	eax, [esp+0xC]
		push	dword ptr 0x4CFE1E
		retn
	}
}

void __declspec(naked)
cped_prerender_hook2(void)
{
	_asm{
		mov	[esp+0x30], 0
		int 3
		push	dword ptr 0x4CFE9A
		retn
	}
}

void
pedhooks(void)
{
	// call to CEntity::Render() in CPed::Render
	MemoryVP::InjectHook(0x4CF8F0, &CPed::AddWeaponModel, PATCH_JUMP);
	MemoryVP::InjectHook(0x4CF980, &CPed::RemoveWeaponModel, PATCH_JUMP);
	MemoryVP::InjectHook(0x4D0484, cped__render__hook);

	MemoryVP::InjectHook(0x4CFE17, cped_prerender_hook1, PATCH_JUMP);
	MemoryVP::InjectHook(0x4CFE92, cped_prerender_hook2, PATCH_JUMP);
}