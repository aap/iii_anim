#include "iii_anim.h"

WRAPPER void CEntity::SetModelIndex(int id) { EAXJMP(0x473E70); }
WRAPPER void CEntity::Render(void) { EAXJMP(0x474BD0); }
WRAPPER void CEntity::PreRender(void) { EAXJMP(0x474350); }
WRAPPER bool CEntity::GetIsOnScreen(void) { EAXJMP(0x474CC0); }
WRAPPER void CObject::Render(void) { EAXJMP(0x4BB1E0); }

WRAPPER CPed *FindPlayerPed() { EAXJMP(0x4A1150); }

WRAPPER RwObject *LookForBatCB(RwObject *object, void *data) { EAXJMP(0x518BF0); }

WRAPPER int CPools__GetPedRef(CPed *ped) { EAXJMP(0x4A1A80); }

WRAPPER void CPed::SetPedStats(int x) { EAXJMP(0x4C5330); }
WRAPPER CPed *CPed::ctor_orig(uint type) { EAXJMP(0x4C41C0); }
WRAPPER void CPed::SpawnFlyingComponent(int nodeId, bool unk) { EAXJMP(0x4EB060); }
WRAPPER bool CPed::UseGroundColModel(void) { EAXJMP(0x4CE730); }


RpAtomic *weaponAtomics[MAXPEDS];

extern "C"
{
	__declspec(dllexport) RpAtomic *IIIAnimGetPedWeaponAtomic(CPed *ped)
	{
		int pedid = CPools__GetPedRef(ped) >> 8;
		return weaponAtomics[pedid];
	}
}


int frameptr;
int otherptr;
int thisptr;

void
CEntity::UpdateRpHAnim(void)
{
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(this->clump);
	RpHAnimHierarchyUpdateMatrices(hier);
}


CPed*
CPed::ctor(uint type)
{
	this->ctor_orig(type);
	int pedid = CPools__GetPedRef(this) >> 8;
	weaponAtomics[pedid] = NULL;
	return this;
}

void
CPed::SetModelIndex(int id)
{
	this->CEntity::SetModelIndex(id);
	RpAnimBlendClumpInit(this->clump);
	RpAnimBlendClumpFillFrameArray(this->clump, this->frames);
	CPedModelInfo *pedinfo = (CPedModelInfo*)CModelInfo::ms_modelInfoPtrs[this->nModelIndex];
	this->SetPedStats(pedinfo->pedStats);
	this->headingRate = this->pedStats[8];
	this->animGroup = pedinfo->animGroup;
	CAnimManager::AddAnimation(this->clump, this->animGroup, 3);
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	clumpData->pedPosition = &this->vecAnimMoveDelta;
	if(pedinfo->hitColModel == NULL)
		pedinfo->CreateHitColModelSkinned(this->clump);
	if(!IsClumpSkinned(clump))
		AttachRimPipeToRwObject((RwObject*)clump);
}

RpMaterial*
SetAlphaCB(RpMaterial *material, void *data)
{
	material->color.alpha = *(int*)data;
	return material;
}

void
CPed::renderLimb(int node)
{
	RpAtomic *atomic;
	CPedModelInfo *pedinfo = (CPedModelInfo*)CModelInfo::ms_modelInfoPtrs[this->nModelIndex];
	switch(node){
	case PED_HEAD:
		atomic = pedinfo->head;
		break;
	case PED_HANDL:
		atomic = pedinfo->lhand;
		break;
	case PED_HANDR:
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
	frame->modelling = *mat;
	RwFrameUpdateObjects(frame);
	int alpha = CVisibilityPlugins__GetClumpAlpha(clump);
	RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), SetAlphaCB, &alpha);
	atomic->renderCallBack(atomic);
}

void __fastcall
cped__render__hook(CPed *ped)
{
	ped->CEntity::Render();

	if(IsClumpSkinned(ped->clump)){
		ped->renderLimb(PED_HEAD);
		ped->renderLimb(PED_HANDL);
		ped->renderLimb(PED_HANDR);
		int pedid = CPools__GetPedRef(ped) >> 8;
		RpAtomic *atomic = weaponAtomics[pedid];
		if(atomic){
			RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ped->clump);
			int idx = RpHAnimIDGetIndex(hier, ped->frames[PED_HANDR]->nodeID);
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
		int pedid = CPools__GetPedRef(this) >> 8;
		weaponAtomics[pedid] = atomic;
		if(id == 183)	// finger
			AttachRimPipeToRwObject((RwObject*)atomic);
	}else{
		RwFrameDestroy(RpAtomicGetFrame(atomic));
		RpAtomicSetFrame(atomic, this->frames[PED_HANDR]->frame);
		RpClumpAddAtomic(this->clump, atomic);
	}
	this->weaponModelId = id;
}

void
CPed::RemoveWeaponModel(int id)
{
	this->weaponModelId = -1;
	if(IsClumpSkinned(this->clump)){
		int pedid = CPools__GetPedRef(this) >> 8;
		RpAtomic *atomic = weaponAtomics[pedid];
		if(atomic){
			RwFrame *frame = RpAtomicGetFrame(atomic);
			RpAtomicDestroy(atomic);
			RwFrameDestroy(frame);
			weaponAtomics[pedid] = NULL;
		}
	}else{
		RwFrameForAllObjects(this->frames[PED_HANDR]->frame, (RwObjectCallBack)0x4CF950, NULL);	// RemoveAllModelCB
	}
}

bool
CPed::IsPedHeadAbovePos(float dist)
{
	RwMatrix mat;
	RwV3d vec = { 0.0f, 0.0f, 0.0f };
	if(IsClumpSkinned(this->clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(this->clump);
		RwInt32 idx = RpHAnimIDGetIndex(hier, this->frames[PED_HEAD]->nodeID);
		RwV3dTransformPoints(&vec, &vec, 1, &RpHAnimHierarchyGetMatrixArray(hier)[idx]);
	}else{
		CPedIK::GetWorldMatrix(this->frames[PED_HEAD]->frame, &mat);
		vec = mat.pos;
	}
	return dist + this->matrix.matrix.pos.z < vec.z;
}

char
CPed::DoesLOSBulletHitPed(CColPoint *colpoint)
{
	RwMatrix mat;
	RwV3d pos = { 0.0f, 0.0f, 0.0f };
	if(IsClumpSkinned(this->clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(this->clump);
		RwInt32 idx = RpHAnimIDGetIndex(hier, this->frames[PED_HEAD]->nodeID);
		RwV3dTransformPoints(&pos, &pos, 1, &RpHAnimHierarchyGetMatrixArray(hier)[idx]);
	}else{
		CPedIK::GetWorldMatrix(this->frames[PED_HEAD]->frame, &mat);
		pos = mat.pos;
	}
	if(this->dwAction == 36 || pos.z > colpoint->point.z)
		return 1;
	return pos.z + 0.2f <= colpoint->point.z ? 0 : 2;
}

class PedEx : public CPed
{
public:
	void SizeHead();
};

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

		mov	ecx, [esp+0xC]
		call    PedEx::SizeHead
	x:

		mov	eax, [esp+0xC]
		push	dword ptr 0x4CFE1E
		retn
	}
}

void
PreRender2(void)
{
	frameptr += 0x88;
	CPed *ped = *(CPed**)(frameptr-0x7C);
	RwV3d *vec = (RwV3d*)(frameptr-0x64);
	if(IsClumpSkinned(ped->clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ped->clump);
		RwInt32 idx = RpHAnimIDGetIndex(hier, ped->frames[ped->byteBodyPartBleeding]->nodeID);
		RwV3dTransformPoints(vec, vec, 1, &RpHAnimHierarchyGetMatrixArray(hier)[idx]);
	}else{
		for(RwFrame *f = ped->frames[ped->byteBodyPartBleeding]->frame; f; f = (RwFrame *)f->object.parent )
			RwV3dTransformPoints(vec, vec, 1, &f->modelling);
	}
}

void __declspec(naked)
cped_prerender_hook2(void)
{
	_asm{
		mov	[esp+0x30], 0
		mov	[frameptr], esp
		call	PreRender2
		push	dword ptr 0x4CFED2
		retn
	}
}

void
CalculateNewVelocity(void)
{
	frameptr += 0x40;
	CPed *ped = (CPed*)thisptr;
	CPedIK::LimbOrientation *orient = (CPedIK::LimbOrientation*)(frameptr-0x10);

	if(IsClumpSkinned(ped->clump)){
		/*
		orient->theta = RAD2DEG(orient->phi);
		RwV3d axis = { -1.0f, 0.0f, 0.0f };
		RtQuatRotate(&ped->frames[PED_Supperlegl]->hanimframe->q, &axis, orient->theta, rwCOMBINEPRECONCAT);
		RtQuatRotate(&ped->frames[PED_Supperlegr]->hanimframe->q, &axis, orient->theta, rwCOMBINEPRECONCAT);
		ped->bfFlagsI |= 0x20;
		*/

		orient->theta = RAD2DEG(orient->phi);
		RwV3d axis1 = { 1.0f, 0.0f, 0.0f };
		RwV3d axis2 = { 0.0f, 0.0f, 1.0f };
		RtQuatRotate(&ped->frames[PED_UPPERLEGL]->hanimframe->q, &axis2, RAD2DEG(0.1f), rwCOMBINEPOSTCONCAT);
		RtQuatRotate(&ped->frames[PED_UPPERLEGL]->hanimframe->q, &axis1, orient->theta, rwCOMBINEPOSTCONCAT);
		RtQuatRotate(&ped->frames[PED_UPPERLEGR]->hanimframe->q, &axis2, RAD2DEG(0.1f), rwCOMBINEPOSTCONCAT);
		RtQuatRotate(&ped->frames[PED_UPPERLEGR]->hanimframe->q, &axis1, orient->theta, rwCOMBINEPOSTCONCAT);
		ped->bfFlagsI |= 0x20;

	}else{
		orient->theta = 0.0f;
		ped->pedIK.RotateTorso(ped->frames[PED_UPPERLEGL], orient, 0);
		ped->pedIK.RotateTorso(ped->frames[PED_UPPERLEGR], orient, 0);
	}
}

void __declspec(naked)
CalculateNewVelocity_hook(void)
{
	_asm{
		mov	[frameptr], esp
		mov	[thisptr], ebx
		sub	esp,108
		fsave	[esp]
		call	CalculateNewVelocity
		frstor	[esp]
		add	esp,108
		push	dword ptr 0x4C78C7
		retn
	}
}

void
PlayFootSteps1(void)
{
	CPed *ped = (CPed*)thisptr;
	RwV3d *vec = (RwV3d*)(frameptr+0x18);
	vec->x = vec->y = vec->z = 0.0f;
	if(IsClumpSkinned(ped->clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ped->clump);
		RwInt32 idx = RpHAnimIDGetIndex(hier, ped->frames[PED_FOOTL]->nodeID);
		RwV3dTransformPoints(vec, vec, 1, &RpHAnimHierarchyGetMatrixArray(hier)[idx]);
	}else{
		for(RwFrame *f = ped->frames[PED_FOOTL]->frame; f; f = (RwFrame *)f->object.parent )
			RwV3dTransformPoints(vec, vec, 1, &f->modelling);
	}
}

void
PlayFootSteps2(void)
{
	CPed *ped = (CPed*)thisptr;
	RwV3d *vec = (RwV3d*)(frameptr+0x3C);
	vec->x = vec->y = vec->z = 0.0f;
	if(IsClumpSkinned(ped->clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ped->clump);
		RwInt32 idx = RpHAnimIDGetIndex(hier, ped->frames[PED_FOOTR]->nodeID);
		RwV3dTransformPoints(vec, vec, 1, &RpHAnimHierarchyGetMatrixArray(hier)[idx]);
	}else{
		for(RwFrame *f = ped->frames[PED_FOOTR]->frame; f; f = (RwFrame *)f->object.parent )
			RwV3dTransformPoints(vec, vec, 1, &f->modelling);
	}
}

void __declspec(naked)
PlayFootSteps_hook1(void)
{
	_asm{
		mov	[frameptr], esp
		mov	[thisptr], ebx
		sub	esp,108
		fsave	[esp]
		call	PlayFootSteps1
		frstor	[esp]
		add	esp,108
		push	dword ptr 0x4CC855
		retn
	}
}

void __declspec(naked)
PlayFootSteps_hook2(void)
{
	_asm{
		mov	[frameptr], esp
		mov	[thisptr], ebx
		sub	esp,108
		fsave	[esp]
		call	PlayFootSteps2
		frstor	[esp]
		add	esp,108
		push	dword ptr 0x4CCA3F
		retn
	}
}

void
Attack1(void)
{
	frameptr += 0x80;
	CPed *ped = (CPed*)thisptr;
	RwV3d *vec = (RwV3d*)(frameptr-0x48);
	int weapon = *(int*)(frameptr-0x7C);
	RwV3d *wvec = (RwV3d*)(weapon+0x28);
	*vec = *wvec;
	if(IsClumpSkinned(ped->clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ped->clump);
		RwInt32 idx = RpHAnimIDGetIndex(hier, ped->frames[PED_HANDR]->nodeID);
		RwV3dTransformPoints(vec, vec, 1, &RpHAnimHierarchyGetMatrixArray(hier)[idx]);
	}else{
		for(RwFrame *f = ped->frames[PED_HANDR]->frame; f; f = (RwFrame *)f->object.parent )
			RwV3dTransformPoints(vec, vec, 1, &f->modelling);
	}
}

// single handed weapon
void
Attack2(void)
{
	frameptr += 0x80;
	CPed *ped = (CPed*)thisptr;
	CAnimBlendAssociation *blendAssoc = (CAnimBlendAssociation*)otherptr;
	RwV3d *vec = (RwV3d*)(frameptr-0x54);
	int n = blendAssoc->animId == 46 ? 10 : 6;
	if(IsClumpSkinned(ped->clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ped->clump);
		RwInt32 idx = RpHAnimIDGetIndex(hier, ped->frames[n]->nodeID);
		RwV3dTransformPoints(vec, vec, 1, &RpHAnimHierarchyGetMatrixArray(hier)[idx]);
	}else{
		for(RwFrame *f = ped->frames[n]->frame; f; f = (RwFrame *)f->object.parent )
			RwV3dTransformPoints(vec, vec, 1, &f->modelling);
	}
}

void __declspec(naked)
Attack_hook1(void)
{
	_asm{
		mov	[frameptr], esp
		mov	[thisptr], ebx
		sub	esp,108
		fsave	[esp]
		call	Attack1
		frstor	[esp]
		add	esp,108
		push	dword ptr 0x4E70F1
		retn
	}
}

void __declspec(naked)
Attack_hook2(void)
{
	_asm{
		mov	[frameptr], esp
		mov	[otherptr], ebp
		mov	[thisptr], ebx
		sub	esp,108
		fsave	[esp]
		call	Attack2
		frstor	[esp]
		add	esp,108
		push	dword ptr 0x4E6EA0
		retn
	}
}

void
FireInstantHit(void)
{
	frameptr += 0x4C0;
	CPed *ped = *(CPed**)(frameptr+0x4);
	RwV3d *vec = (RwV3d*)(frameptr-0x43C);

	if(IsClumpSkinned(ped->clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ped->clump);
		RwInt32 idx = RpHAnimIDGetIndex(hier, ped->frames[PED_HANDR]->nodeID);
		RwV3dTransformPoints(vec, vec, 1, &RpHAnimHierarchyGetMatrixArray(hier)[idx]);
	}else{
		for(RwFrame *f = ped->frames[PED_HANDR]->frame; f; f = (RwFrame *)f->object.parent )
			RwV3dTransformPoints(vec, vec, 1, &f->modelling);
	}
}

void __declspec(naked)
FireInstantHit_hook(void)
{
	_asm{
		mov	[frameptr], esp
		sub	esp,108
		fsave	[esp]
		call	FireInstantHit
		frstor	[esp]
		add	esp,108
		push	dword ptr 0x55D838
		retn
	}
}

void
FinishLaunchCB1(void)
{
	frameptr += 0x88;
	CPed *ped = (CPed*)otherptr;
	RwV3d *vec = (RwV3d*)(frameptr-0x40);
	if(IsClumpSkinned(ped->clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ped->clump);
		RwInt32 idx = RpHAnimIDGetIndex(hier, ped->frames[PED_FOOTL]->nodeID);
		RwV3dTransformPoints(vec, vec, 1, &RpHAnimHierarchyGetMatrixArray(hier)[idx]);
	}else{
		for(RwFrame *f = ped->frames[PED_FOOTL]->frame; f; f = (RwFrame *)f->object.parent )
			RwV3dTransformPoints(vec, vec, 1, &f->modelling);
	}
}

void
FinishLaunchCB2(void)
{
	frameptr += 0x88;
	CPed *ped = (CPed*)otherptr;
	RwV3d *vec = (RwV3d*)(frameptr-0x40);
	if(IsClumpSkinned(ped->clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ped->clump);
		RwInt32 idx = RpHAnimIDGetIndex(hier, ped->frames[PED_FOOTR]->nodeID);
		RwV3dTransformPoints(vec, vec, 1, &RpHAnimHierarchyGetMatrixArray(hier)[idx]);
	}else{
		for(RwFrame *f = ped->frames[PED_FOOTR]->frame; f; f = (RwFrame *)f->object.parent )
			RwV3dTransformPoints(vec, vec, 1, &f->modelling);
	}
}

void __declspec(naked)
FinishLaunchCB_hook1(void)
{
	_asm{
		mov	[frameptr], esp
		mov	[otherptr], ebp
		sub	esp,108
		fsave	[esp]
		call	FinishLaunchCB1
		frstor	[esp]
		add	esp,108
		push	dword ptr 0x4D7837
		retn
	}
}

void __declspec(naked)
FinishLaunchCB_hook2(void)
{
	_asm{
		mov	[frameptr], esp
		mov	[otherptr], ebp
		sub	esp,108
		fsave	[esp]
		call	FinishLaunchCB2
		frstor	[esp]
		add	esp,108
		push	dword ptr 0x4D798F
		retn
	}
}

void
Fight(void)
{
	frameptr += 0x88;
	CPed *ped = (CPed*)otherptr;
	RwV3d *vec = (RwV3d*)(frameptr-0x1C);
	int n = -1;
	switch(ped->dwLastHitState){
	case 1: case 7: case 11:
		n = 6;
		break;
	case 6:
		n = 5;
		break;
	case 8: case 9: case 10: case 12:
		n = 10;
		break;
	case 5:
		n = 2;
		break;
	case 4:
		n = 11;
		break;
	}
	if(IsClumpSkinned(ped->clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ped->clump);
		RwInt32 idx = RpHAnimIDGetIndex(hier, ped->frames[n]->nodeID);
		RwV3dTransformPoints(vec, vec, 1, &RpHAnimHierarchyGetMatrixArray(hier)[idx]);
	}else{
		for(RwFrame *f = ped->frames[n]->frame; f; f = (RwFrame *)f->object.parent )
			RwV3dTransformPoints(vec, vec, 1, &f->modelling);
	}
}

void __declspec(naked)
Fight_hook(void)
{
	_asm{
		mov	[frameptr], esp
		mov	[otherptr], ebp
		sub	esp,108
		fsave	[esp]
		call	Fight
		frstor	[esp]
		add	esp,108
		push	dword ptr 0x4E80B5
		retn
	}
}

void
CopAI(void)
{
	frameptr += 0x90;
	CPed *ped = (CPed*)thisptr;
	RwV3d *vec = (RwV3d*)(frameptr-0x60);
	if(IsClumpSkinned(ped->clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ped->clump);
		RwInt32 idx = RpHAnimIDGetIndex(hier, ped->frames[PED_HANDR]->nodeID);
		RwV3dTransformPoints(vec, vec, 1, &RpHAnimHierarchyGetMatrixArray(hier)[idx]);
	}else{
		for(RwFrame *f = ped->frames[PED_HANDR]->frame; f; f = (RwFrame *)f->object.parent )
			RwV3dTransformPoints(vec, vec, 1, &f->modelling);
	}
}

void __declspec(naked)
CopAI_hook(void)
{
	_asm{
		mov	[frameptr], esp
		mov	[thisptr], ebx
		sub	esp,108
		fsave	[esp]
		call	CopAI
		frstor	[esp]
		add	esp,108
		push	dword ptr 0x4C201F
		retn
	}
}

WRAPPER int CParticle__AddParticle(int type, CVector *v1, CVector *v2, CEntity *e, float a5, int a6, int a7, int a8, int a9) { EAXJMP(0x50D140); }

void
StartFightDefend(void)
{
	CPed *ped = (CPed*)otherptr;
	RwMatrix mat;
	CVector v1, v2;
	v2.x = v2.y = 0.0f;
	v2.z = 0.1f;

	if(IsClumpSkinned(ped->clump)){
		// TODO test
		RwV3d pos;
		pos.x = pos.y = pos.z = 0.0f;
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ped->clump);
		RwInt32 idx = RpHAnimIDGetIndex(hier, ped->frames[PED_HEAD]->nodeID);
		RwV3dTransformPoints(&pos, &pos, 1, &RpHAnimHierarchyGetMatrixArray(hier)[idx]);
		for(int i = 0; i < 4; i++){
			v1.x = pos.x - 0.2f * ped->matrix.matrix.up.x;
			v1.y = pos.y - 0.2f * ped->matrix.matrix.up.y;
			v1.z = pos.z - 0.2f * ped->matrix.matrix.up.z;
			CParticle__AddParticle(4, &v1, &v2, 0, 0.0f, 0, 0, 0, 0);
		}
	}else{
		CPedIK::GetWorldMatrix(ped->frames[PED_HEAD]->frame, &mat);
		for(int i = 0; i < 4; i++){
			v1.x = mat.pos.x - 0.2f * ped->matrix.matrix.up.x;
			v1.y = mat.pos.y - 0.2f * ped->matrix.matrix.up.y;
			v1.z = mat.pos.z - 0.2f * ped->matrix.matrix.up.z;
			CParticle__AddParticle(4, &v1, &v2, 0, 0.0f, 0, 0, 0, 0);
		}
	}
}

void __declspec(naked)
StartFightDefend_hook(void)
{
	_asm{
		mov	[otherptr], ebp
		sub	esp,108
		fsave	[esp]
		call	StartFightDefend
		frstor	[esp]
		add	esp,108
		push	dword ptr 0x4E78E4
		retn
	}
}



CColModel &CTempColModels__ms_colModelPedGroundHit = *(CColModel*)0x880480;

CColModel*
CPed::GetColModel(void)
{
	if(IsClumpSkinned(clump))
		return ((CPedModelInfo*)CModelInfo::ms_modelInfoPtrs[nModelIndex])->AnimatePedColModelSkinned(clump);
	if(UseGroundColModel())
		return &CTempColModels__ms_colModelPedGroundHit;
	return ((CPedModelInfo*)CModelInfo::ms_modelInfoPtrs[nModelIndex])->colModel;
}

void __declspec(naked)
ProcessLineOfSightSectorList_hook2(void)
{
	_asm{
		call	CPed::GetColModel
		mov	edx,eax
		push	0x4B0D77
		retn
	}
}

CColModel*
GetFightColModel(CPed *ped)
{
	if(IsClumpSkinned(ped->clump))
		return ((CPedModelInfo*)CModelInfo::ms_modelInfoPtrs[ped->nModelIndex])->AnimatePedColModelSkinned(ped->clump);
	if(ped->dwAction == 36 || ped->dwAction == 48 || ped->dwAction == 49 || !ped->IsPedHeadAbovePos(-0.3f))
		return &CTempColModels__ms_colModelPedGroundHit;
	return ((CPedModelInfo*)CModelInfo::ms_modelInfoPtrs[ped->nModelIndex])->colModel;
}

void __declspec(naked)
FightStrike_hook2(void)
{
	_asm{
		// have to clean up floating point shit first
		fcompp
		fstp	st

		push	ebp
		call	GetFightColModel
		add	esp,4
		mov	esi,eax
		push	0x4E9055
		retn
	}
}

// For Burstable tyre mod
CColModel*
GetPedColModel(CPed *ped)
{
	if(IsClumpSkinned(ped->clump)){
		return ((CPedModelInfo*)CModelInfo::ms_modelInfoPtrs[ped->nModelIndex])->AnimatePedColModelSkinned(ped->clump);
	}else{
		extern CColModel *( __cdecl *Original_GetPedColModel)(CPed *ped);
		return Original_GetPedColModel(ped);
	}
}

void CSpecialFX__Update_Patch()
{
	if(IsClumpSkinned(FindPlayerPed()->clump)){
		int pedid = CPools__GetPedRef(FindPlayerPed()) >> 8;
		RpAtomic *atomic = weaponAtomics[pedid];

		LookForBatCB((RwObject*)atomic, (void *)CModelInfo::ms_modelInfoPtrs[172]);
	}else
		RwFrameForAllObjects(FindPlayerPed()->frames[PED_HANDR]->frame, LookForBatCB, (void*)CModelInfo::ms_modelInfoPtrs[172]);
}

enum tParticleType
{
	PARTICLE_BLOOD_SMALL = 0x5,
	PARTICLE_TEST = 0x41,
};

WRAPPER void * __cdecl CParticle__AddParticle(tParticleType type, CVector const &vecPos, CVector const &vecDir, CEntity *pEntity, float fSize, int nRotationSpeed, int nRotation, int nCurFrame, int nLifeSpan) { EAXJMP(0x50D140); }

bool &CGame__nastyGame = *(bool*)0x5F4DD4;
WRAPPER RwFrame *RecurseFrameChildrenVisibilityCB(RwFrame *f, void *v) { EAXJMP(0x4EAE20); }

void
CPed::RemoveBodyPart(int nodeId, bool unk)
{
	if(frames[nodeId]->frame == NULL){
		printf("Trying to remove ped component");
		return;
	}

	if(!IsClumpSkinned(clump)){
		if(!CGame__nastyGame)
			return;

		if(nodeId != PED_HEAD)
			SpawnFlyingComponent(nodeId, unk);
	
		RecurseFrameChildrenVisibilityCB(frames[nodeId]->frame, NULL);
	
		RwV3d point = { 0.0f, 0.0f, 0.0f };
		for(RwFrame *f = frames[nodeId]->frame; f; f = (RwFrame *)f->object.parent)
			RwV3dTransformPoints(&point, &point, 1, &f->modelling);
	}

	CVector point(0.0f, 0.0f, 0.0f);
	pedIK.GetComponentPosition((RwV3d *)&point, nodeId);

	if(GetIsOnScreen()){
		CParticle__AddParticle(PARTICLE_TEST, point, CVector(0.0f, 0.0f, 0.0f), 0, 0.1f, 0, 0, 0, 0);

		for(int i = 0; i < 16; i++)
			CParticle__AddParticle(PARTICLE_BLOOD_SMALL, point, CVector(0.0f, 0.0f, 0.03f), 0, 0.0f, 0, 0, 0, 0);
	}

	bfFlagsC = bfFlagsC & ~20 | 0x20;
	byteBodyPartBleeding = nodeId;
}

void PedEx::SizeHead()
{
	if((bfFlagsC >> 5) & 1 && byteBodyPartBleeding == PED_HEAD){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(clump);
		int bonetag = ConvertPedNode2BoneTag(PED_HEAD);
		RwUInt32 index = RpHAnimIDGetIndex(hier, bonetag);
		RwMatrix *pMatrix = RpHAnimHierarchyGetMatrixArray(hier);
		RwV3d scale = { 0.0f, 0.0f, 0.0f };
		RwMatrixScale(&pMatrix[index], &scale, rwCOMBINEPRECONCAT);
	}

	/*
	#define ARRAY_SIZE(array)               (sizeof(array) / sizeof(array[0]))
	if ( ((unsigned char)bfFlagsC >> 5) & 1
		&& byteBodyPartBleeding == PED_Shead
		|| byteBodyPartBleeding == PED_Supperlegr
		|| byteBodyPartBleeding == PED_Supperlegl
		|| byteBodyPartBleeding == PED_Supperarmr
		|| byteBodyPartBleeding == PED_Supperarml )
	{
		static int head[] = { BONE_Shead };

		static int armL[] = { BONE_SLhand, BONE_Supperarml, BONE_Slowerarml };
		static int armR[] = { BONE_SRhand, BONE_Supperarmr, BONE_Slowerarmr };

		static int legl[] = { BONE_Sfootl, BONE_Supperlegl, BONE_Slowerlegl };
		static int legr[] = { BONE_Sfootr, BONE_Supperlegr, BONE_Slowerlegr };

		struct bodypartgroup
		{
			int num;
			int *array;
		} g[] =
		{
			{ 0, NULL },
			{ 0, NULL },
			{ ARRAY_SIZE(head), head },
			{ ARRAY_SIZE(armL), armL },
			{ ARRAY_SIZE(armR), armR },
			{ ARRAY_SIZE(armL), armL },
			{ ARRAY_SIZE(armR), armR },
			{ ARRAY_SIZE(legl), legl },
			{ ARRAY_SIZE(legr), legr },
			{ ARRAY_SIZE(legl), legl },
			{ ARRAY_SIZE(legr), legr },
			{ ARRAY_SIZE(legr), legr },
			{ ARRAY_SIZE(legl), legl }
		};


		for ( int i = 0; i < g[byteBodyPartBleeding].num; i++ )
		{
			RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(clump);
			int bonetag = g[byteBodyPartBleeding].array[i];
			RwUInt32 index = RpHAnimIDGetIndex(hier, bonetag);
			RwMatrix *pMatrix = RpHAnimHierarchyGetMatrixArray(hier);
			RwV3d scale = { 0.0f, 0.0f, 0.0f };
			RwMatrixScale(&pMatrix[index], &scale, rwCOMBINEPRECONCAT);
		}
	}
	#undef ARRAY_SIZE
	*/
}

static void (__thiscall *playerpedcontrol)(CPlayerPed *ped);
void CPlayerPed::ProcessControl_hook(void)
{
	playerpedcontrol(this);
	if(IsClumpSkinned(this->clump))
		// Update HAnim in first person mode
		if((this->bfFlagsB & 4) == 0)
			UpdateRpHAnim();
}

void
pedhooks(void)
{
	InjectHook(0x4C52A0, &CPed::SetModelIndex, PATCH_JUMP);
	// call to CEntity::Render() in CPed::Render
	InjectHook(0x4CF8F0, &CPed::AddWeaponModel, PATCH_JUMP);
	InjectHook(0x4CF980, &CPed::RemoveWeaponModel, PATCH_JUMP);
	InjectHook(0x4EB670, &CPed::IsPedHeadAbovePos, PATCH_JUMP);
	InjectHook(0x4EB5C0, &CPed::DoesLOSBulletHitPed, PATCH_JUMP);
	InjectHook(0x4D0484, cped__render__hook);

	InjectHook(0x4CFE17, cped_prerender_hook1, PATCH_JUMP);
	InjectHook(0x4CFE92, cped_prerender_hook2, PATCH_JUMP);

	InjectHook(0x4C788B, CalculateNewVelocity_hook, PATCH_JUMP);
	InjectHook(0x4CC811, PlayFootSteps_hook1, PATCH_JUMP);
	InjectHook(0x4CC9FB, PlayFootSteps_hook2, PATCH_JUMP);
	InjectHook(0x4D7818, FinishLaunchCB_hook1, PATCH_JUMP);
	InjectHook(0x4D7970, FinishLaunchCB_hook2, PATCH_JUMP);
	InjectHook(0x4E70AE, Attack_hook1, PATCH_JUMP);
	InjectHook(0x4E6E81, Attack_hook2, PATCH_JUMP);	// one handed weapons
	InjectHook(0x4E8026, Fight_hook, PATCH_JUMP);
	InjectHook(0x4E781A, StartFightDefend_hook, PATCH_JUMP);
	InjectHook(0x4C1FF0, CopAI_hook, PATCH_JUMP);

	InterceptVmethod(&playerpedcontrol, &CPlayerPed::ProcessControl_hook, 0x5FA520);

	InjectHook(0x4EF7EA, &CPed::ctor);
	InjectHook(0x4C2E4D, &CPed::ctor);
	InjectHook(0x4C11BE, &CPed::ctor);
	InjectHook(0x4BFF3D, &CPed::ctor);

	// not CPed but what the hell

	InjectHook(0x55D813, FireInstantHit_hook, PATCH_JUMP);

	// We *have* to animate the hit colmodel because
	// the initial bone positions at creation are wrong!
	// redirect two short jumps to alignment bytes and insert jump there
	InjectHook(0x4B0D42, ProcessLineOfSightSectorList_hook2, PATCH_JUMP);
	InjectHook(0x4E900C, FightStrike_hook2, PATCH_JUMP);

	InjectHook(0x4EAEE0, &CPed::RemoveBodyPart, PATCH_JUMP);



	//MemoryVP::Patch<BYTE>(0x55D2E0, 0xCC);	// CWeapon::FireInstantHit	done
	//MemoryVP::Patch<BYTE>(0x4EB5C0, 0xCC);	// CPed::DoesLOSBulletHitPed	done
	//MemoryVP::Patch<BYTE>(0x4EB670, 0xCC);	// CPed::IsPedHeadAbovePos	done
//	MemoryVP::Patch<BYTE>(0x4E7780, 0xCC);	// CPed::StartFightDefend
	//MemoryVP::Patch<BYTE>(0x4E8EC0, 0xCC);	// CPed::FightStrike		done
	//MemoryVP::Patch<BYTE>(0x4E7EE0, 0xCC);	// CPed::Fight			done
	//MemoryVP::Patch<BYTE>(0x4E6BA0, 0xCC);	// CPed::Attack			done
	//MemoryVP::Patch<BYTE>(0x4C73F0, 0xCC);	// CPed::CalculateNewVelocity	done
	//MemoryVP::Patch<BYTE>(0x4CC6C0, 0xCC);	// CPed::PlayFootSteps		done
	//MemoryVP::Patch<BYTE>(0x4D7490, 0xCC);	// CPed::FinishLaunchCB		done
}