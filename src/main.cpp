#include "iii_anim.h"

HMODULE dllModule, hDummyHandle;

void **rwengine = *(void***)0x59EEB1;

int &CTimer__m_FrameCounter = *(int*)0x9412EC;

WRAPPER void *RwMallocAlign(uint, int) { EAXJMP(0x526FD0); }
WRAPPER void RwFreeAlign(void*) { EAXJMP(0x527000); }
WRAPPER void gtadelete(void*) { EAXJMP(0x5A07E0); }
WRAPPER void *gta_nw(int) { EAXJMP(0x5A0690); }

WRAPPER RwMatrix *RwMatrixUpdate(RwMatrix*) { EAXJMP(0x5A28E0); }
WRAPPER RwMatrix *RwMatrixInvert(RwMatrix*, const RwMatrix*) { EAXJMP(0x5A2C90); }
WRAPPER RwMatrix *RwMatrixTransform(RwMatrix*, const RwMatrix*, RwOpCombineType) { EAXJMP(0x5A31C0); }
WRAPPER RwMatrix *RwMatrixTranslate(RwMatrix * matrix, const RwV3d * translation, RwOpCombineType combineOp) { EAXJMP(0x05A3070); }
WRAPPER RwMatrix *RwMatrixScale(RwMatrix * matrix, const RwV3d * scale, RwOpCombineType combineOp) { EAXJMP(0x5A2EE0); }
WRAPPER RwMatrix *RwMatrixRotate(RwMatrix*, const RwV3d*, RwReal, RwOpCombineType) { EAXJMP(0x5A2BF0); }
WRAPPER RwMatrix *RwMatrixCreate(void) { EAXJMP(0x5A3330); }
WRAPPER RwBool RwMatrixDestroy(RwMatrix*) { EAXJMP(0x5A3300); }
WRAPPER RwMatrix *RwFrameGetLTM(RwFrame *) { EAXJMP(0x5A1CE0); }
WRAPPER RwFrame *RwFrameForAllChildren(RwFrame*, RwFrameCallBack, void*) { EAXJMP(0x5A1FC0); }
WRAPPER RwFrame *RwFrameForAllObjects(RwFrame*, RwObjectCallBack, void*) { EAXJMP(0x5A2340); }
WRAPPER RwFrame *RwFrameUpdateObjects(RwFrame*) { EAXJMP(0x5A1C60); }
WRAPPER RwFrame *RwFrameRemoveChild(RwFrame *) { EAXJMP(0x5A1ED0); }
WRAPPER RwBool RwFrameDestroy(RwFrame*) { EAXJMP(0x5A1A30); }
WRAPPER RwV3d *RwV3dTransformPoints(RwV3d*, const RwV3d*, RwInt32, const RwMatrix*) { EAXJMP(0x5A37D0); }
WRAPPER RwV3d *RwV3dTransformVectors(RwV3d * vectorsOut, const RwV3d * vectorsIn, RwInt32 numPoints, const RwMatrix * matrix) { EAXJMP(0x5A37E0); }
WRAPPER RwReal RwV3dLength(const RwV3d*) { EAXJMP(0x5A36A0); }
WRAPPER RwBool RpClumpDestroy(RpClump*) { EAXJMP(0x59F500); }
WRAPPER RpClump *RpClumpForAllAtomics(RpClump*, RpAtomicCallBack, void*) { EAXJMP(0x59EDD0); }
WRAPPER RpClump *RpClumpClone(RpClump*) { EAXJMP(0x59F1B0); }
WRAPPER RpClump *RpClumpRemoveAtomic(RpClump*, RpAtomic*) { EAXJMP(0x59F6B0); }
WRAPPER RpClump *RpClumpAddAtomic(RpClump*, RpAtomic*) { EAXJMP(0x59F680); }
WRAPPER RpClump *RpClumpStreamRead(RwStream *stream) { EAXJMP(0x59FC50); }
WRAPPER RwBool RpAtomicDestroy(RpAtomic*) { EAXJMP(0x59F020); }
WRAPPER RpAtomic *RpAtomicSetFrame(RpAtomic*, RwFrame*) { EAXJMP(0x5A0600); }
WRAPPER RpSkin *RpSkinGeometryGetSkin(RpGeometry*) { EAXJMP(0x5B1080); }
WRAPPER RpAtomic *RpSkinAtomicSetHAnimHierarchy(RpAtomic*, RpHAnimHierarchy*) { EAXJMP(0x5B1050); }
WRAPPER RpHAnimHierarchy *RpSkinAtomicGetHAnimHierarchy(const RpAtomic*) { EAXJMP(0x5B1070); }
WRAPPER RwBool RpHAnimHierarchyUpdateMatrices(RpHAnimHierarchy *hierarchy) { EAXJMP(0x5B1780); }
WRAPPER RpHAnimHierarchy *RpHAnimFrameGetHierarchy(RwFrame*) { EAXJMP(0x5B11F0); }
WRAPPER RwBool RpHAnimHierarchySetCurrentAnim(RpHAnimHierarchy*, RpHAnimAnimation*) { EAXJMP(0x5B1200); }
WRAPPER RwBool RpHAnimHierarchyAddAnimTime(RpHAnimHierarchy*, RwReal) { EAXJMP(0x5B1480); }
WRAPPER RwBool RpHAnimHierarchySubAnimTime(RpHAnimHierarchy *hierarchy, RwReal time) { EAXJMP(0x5B12B0); }
WRAPPER RpHAnimAnimation *RpHAnimAnimationStreamRead(RwStream *stream) { EAXJMP(0x5B1C10); }
WRAPPER RpGeometry *RpGeometryForAllMaterials(RpGeometry *geometry, RpMaterialCallBack fpCallBack, void *pData) { EAXJMP(0x5ACBF0); }

WRAPPER RpAtomic *AtomicDefaultRenderCallBack(RpAtomic *atomic) { EAXJMP(0x59E690); }


WRAPPER RwStream *RwStreamOpen(RwStreamType type, RwStreamAccessType accessType, const void *pData) { EAXJMP(0x5A3FE0); }
WRAPPER RwBool RwStreamClose(RwStream * stream, void *pData) { EAXJMP(0x5A3F10); }
WRAPPER RwBool RwStreamFindChunk(RwStream *stream, RwUInt32 type, RwUInt32 *lengthOut, RwUInt32 *versionOut) { EAXJMP(0x5AA540); }

WRAPPER const char *GetFrameNodeName(RwFrame *frame) { EAXJMP(0x527150); }
WRAPPER void *GetModelFromName(char *name) { EAXJMP(0x4010D0); }
WRAPPER RpAtomic *GetFirstAtomic(RpClump *clump) { EAXJMP(0x526420); }
WRAPPER int CFileMgr::OpenFile(const char*, const char*) { EAXJMP(0x479100); }
WRAPPER int CFileMgr::Read(int, void*, int) { EAXJMP(0x479140); }
WRAPPER void CFileMgr::Seek(int, int, int) { EAXJMP(0x479180); }
WRAPPER void CFileMgr::CloseFile(int) { EAXJMP(0x479200); }
WRAPPER void CQuaternion::Slerp(CQuaternion&, CQuaternion&, float, float, float) { EAXJMP(0x4BA1C0); }
WRAPPER void CQuaternion::Get(RwMatrix *mat) { EAXJMP(0x4BA0D0); }
WRAPPER void CMatrix::ctor(RwMatrix *, bool) { EAXJMP(0x4B8D90); }
WRAPPER void CMatrix::dtor(void) { EAXJMP(0x4B8DB0); }
WRAPPER void CMatrix::RotateX(float) { EAXJMP(0x4B9510); }
WRAPPER void CMatrix::SetRotateY(float) { EAXJMP(0x4B9340); }
WRAPPER void CMatrix::SetRotateZ(float) { EAXJMP(0x4B9370); }
WRAPPER void CMatrix::UpdateRW(void) { EAXJMP(0x4B8EC0); }
WRAPPER void CMatrix::Update(void) { EAXJMP(0x4B8E50); }
WRAPPER void CMatrix::AttachRW(RwMatrix *matrix, bool temporary) { EAXJMP(0x4B8E00); }
WRAPPER void CMatrix::Detach(void) { EAXJMP(0x4B8E30); }
WRAPPER void CMatrix::mult(CMatrix *out, CMatrix *in1, CMatrix *in2) { EAXJMP(0x4B9D60); }
WRAPPER void CMatrix::assign(CMatrix *in) { EAXJMP(0x4B8F40); }
WRAPPER void CVector::Normalize(void) { EAXJMP(0x4BA560); };
WRAPPER void CrossProduct(CVector *, CVector *, CVector *) { EAXJMP(0x4BA350); }

WRAPPER void CEntity::UpdateRwFrame(void) { EAXJMP(0x474330); }
WRAPPER void CPhysical::ProcessControl(void) { EAXJMP(0x495F10); }

WRAPPER void CVisibilityPlugins__SetClumpModelInfo(RpClump *clump, int clumpModelInfo) { EAXJMP(0x528ED0); }
WRAPPER RpAtomic *CVisibilityPlugins__RenderPlayerCB(RpAtomic*) { EAXJMP(0x528B30); }
WRAPPER void CVisibilityPlugins__RenderAlphaAtomic(RpAtomic*,int) { EAXJMP(0x5280B0); }
WRAPPER int CVisibilityPlugins__GetClumpAlpha(RpClump* clump) { EAXJMP(0x528F70); }
//WRAPPER void CVisibilityPlugins__SetClumpAlpha(RpClump* clump, int alpha) { EAXJMP(0x528F50); }



void **CModelInfo::ms_modelInfoPtrs = (void**)0x83D408;

static char *charclasses = (char*)0x618370;
static char *charset = (char*)0x618470;

int
gtastrcmp(const char *s1, const char *s2)
{
	char c1, c2;
	while(*s1){
		c1 = charclasses[*s1] & 0x40 ? *s1++ - 0x20 : *s1++;
		c2 = charclasses[*s2] & 0x40 ? *s2++ - 0x20 : *s2++;
		if(c1 != c2)
			return 1;
	}
	return *s2 != '\0';
}

int
lcstrcmp(const char *s1, const char *s2)
{
	int c1, c2;
	do{
		c1 = *s1 == -1 ? -1 : charset[*s1];
		c2 = *s2 == -1 ? -1 : charset[*s2];
		if(c1 < c2)
			return -1;
		if(c1 > c2)
			return 1;
		s1++;
		s2++;
	}while(c1);
	return 0;
}

static RpAtomic*
isSkinnedCb(RpAtomic *atomic, void *data)
{
	RpAtomic **ret = (RpAtomic **)data;
	if(*ret)
		return NULL;
	assert(atomic->geometry->object.type = rpGEOMETRY);
	if(RpSkinGeometryGetSkin(atomic->geometry))
		*ret = atomic;
	return atomic;
}

RpAtomic*
IsClumpSkinned(RpClump *c)
{
	RpAtomic *ret = NULL;
	assert(c->object.type == rpCLUMP);
	RpClumpForAllAtomics(c, isSkinnedCb, &ret);
	return ret;
}

void
dumpAnimGroups(void)
{
	FILE *f;
	f = fopen("animgrp_iii.txt", "wb");
	for(int i = 0; i < 25; i++){
		AnimAssocDefinition *def = &CAnimManager::ms_aAnimAssocDefinitions[i];
		fprintf(f, "%s %s %d %d\n", def->name, def->blockName, def->modelIndex, def->animCount);
		for(int j = 0; j < def->animCount; j++)
			fprintf(f, "\t%d %s %x\n", def->animInfoList[j].animId, def->animNames[j], def->animInfoList[j].flags);
	}
	fclose(f);
}

void AttachRimPipeToRwObject_dummy(RwObject*){}
void (*AttachRimPipeToRwObject)(RwObject *obj) = AttachRimPipeToRwObject_dummy;

typedef CColModel *(*t_GetPedColModel)(CPed *ped);
t_GetPedColModel Original_GetPedColModel;
CColModel *GetPedColModel(CPed *ped);

void (*InitialiseGame)(void);
void
InitialiseGame_hook(void)
{
	HMODULE sky = GetModuleHandleA("skygfx.dll");
	if(sky == NULL) sky = GetModuleHandleA("skygfx.asi");
	if(sky){
		AttachRimPipeToRwObject = (void (*)(RwObject*))GetProcAddress(sky, "AttachRimPipeToRwObject");
		if(AttachRimPipeToRwObject == NULL)
			AttachRimPipeToRwObject = AttachRimPipeToRwObject_dummy;
	}

	HMODULE tyres = GetModuleHandleA("IIIBurstableTyres.dll");
	if(tyres == NULL) tyres = GetModuleHandleA("IIIBurstableTyres.asi");
	if(tyres){
		void (*Set_GetPedColModel_CallBack)(t_GetPedColModel) = (void (*)(t_GetPedColModel))GetProcAddress(tyres, "?Set_GetPedColModel_CallBack@BurstableTyres@@YAXP6APAVCColModel@@PAVCPed@@@Z@Z");
		if(Set_GetPedColModel_CallBack)
			Set_GetPedColModel_CallBack(GetPedColModel);
	}

	InitialiseGame();
}

RwCamera *&pRwCamera = *(RwCamera**)0x72676C;


void __declspec(naked)
cutscene_copyanimation_hook(void)
{
	_asm{	
		mov	eax,[esp+4]
		push	ebx	// clump in ebx
		push	eax	// name from stack
		// this in ecx
		call	CAnimBlendAssocGroup::CopyAnimationForClump
		retn	4
	}
}

void
patch10(void)
{
	// Fail if RenderWare has already been started
	if(pRwCamera){
		MessageBox(NULL, "iii_anim cannot be loaded by the default Mss32 ASI loader.\nUse another ASI loader.", "Error", MB_ICONERROR | MB_OK);
		return;
	}

	InjectHook(0x401000, &CAnimBlendAssocGroup::ctor, PATCH_JUMP);
	InjectHook(0x401130, static_cast<void(CAnimBlendAssocGroup::*)(const char*)>(&CAnimBlendAssocGroup::CreateAssociations), PATCH_JUMP);
	InjectHook(0x401220, static_cast<void(CAnimBlendAssocGroup::*)(const char *name, RpClump *clump, char **names, int numAnims)>(&CAnimBlendAssocGroup::CreateAssociations), PATCH_JUMP);
	InjectHook(0x4012D0, &CAnimBlendAssocGroup::dtor, PATCH_JUMP);
	InjectHook(0x401300, static_cast<CAnimBlendAssociation*(CAnimBlendAssocGroup::*)(const char*)>(&CAnimBlendAssocGroup::GetAnimation), PATCH_JUMP);
	InjectHook(0x4013D0, static_cast<CAnimBlendAssociation*(CAnimBlendAssocGroup::*)(uint)>(&CAnimBlendAssocGroup::GetAnimation), PATCH_JUMP);
	InjectHook(0x4013E0, static_cast<CAnimBlendAssociation*(CAnimBlendAssocGroup::*)(const char*)>(&CAnimBlendAssocGroup::CopyAnimation), PATCH_JUMP);
	InjectHook(0x401420, static_cast<CAnimBlendAssociation*(CAnimBlendAssocGroup::*)(uint)>(&CAnimBlendAssocGroup::CopyAnimation), PATCH_JUMP);

	InjectHook(0x401460, &CAnimBlendAssociation::ctor, PATCH_JUMP);
	InjectHook(0x401520, &CAnimBlendAssociation::dtor, PATCH_JUMP);
	InjectHook(0x401560, static_cast<void(CAnimBlendAssociation::*)(RpClump *clump, CAnimBlendHierarchy *anim)>(&CAnimBlendAssociation::Init), PATCH_JUMP);
	InjectHook(0x401620, static_cast<void(CAnimBlendAssociation::*)(CAnimBlendAssociation&)>(&CAnimBlendAssociation::Init), PATCH_JUMP);
	InjectHook(0x4016A0, &CAnimBlendAssociation::AllocateAnimBlendNodeArray, PATCH_JUMP);
	InjectHook(0x4016F0, &CAnimBlendAssociation::FreeAnimBlendNodeArray, PATCH_JUMP);
	InjectHook(0x401700, &CAnimBlendAssociation::SetCurrentTime, PATCH_JUMP);
	InjectHook(0x401780, &CAnimBlendAssociation::SyncAnimation, PATCH_JUMP);
	InjectHook(0x4017B0, &CAnimBlendAssociation::GetNode, PATCH_JUMP);
	InjectHook(0x4017D0, &CAnimBlendAssociation::Start, PATCH_JUMP);
	InjectHook(0x4017E0, &CAnimBlendAssociation::SetBlend, PATCH_JUMP);
	InjectHook(0x401800, &CAnimBlendAssociation::SetDeleteCallback, PATCH_JUMP);
	InjectHook(0x401820, &CAnimBlendAssociation::SetFinishCallback, PATCH_JUMP);
	InjectHook(0x401840, &CAnimBlendAssociation::dtor2, PATCH_JUMP);
	InjectHook(0x4031F0, &CAnimBlendAssociation::UpdateTime, PATCH_JUMP);
	InjectHook(0x4032B0, &CAnimBlendAssociation::UpdateBlend, PATCH_JUMP);

	InjectHook(0x401880, &CAnimBlendClumpData::ctor, PATCH_JUMP);
	InjectHook(0x4018B0, &CAnimBlendClumpData::dtor, PATCH_JUMP);
	InjectHook(0x4018F0, &CAnimBlendClumpData::SetNumberOfBones, PATCH_JUMP);
	InjectHook(0x401930, &CAnimBlendClumpData::ForAllFrames, PATCH_JUMP);

	InjectHook(0x4019A0, &CAnimBlendHierarchy::Shutdown, PATCH_JUMP);
	InjectHook(0x4019C0, &CAnimBlendHierarchy::SetName, PATCH_JUMP);
	InjectHook(0x401AB0, &CAnimBlendHierarchy::RemoveAnimSequences, PATCH_JUMP);
	InjectHook(0x4019E0, &CAnimBlendHierarchy::CalcTotalTime, PATCH_JUMP);
	InjectHook(0x401A80, &CAnimBlendHierarchy::RemoveQuaternionFlips, PATCH_JUMP);
	InjectHook(0x401AD0, &CAnimBlendHierarchy::Uncompress, PATCH_JUMP);
	InjectHook(0x401B00, &CAnimBlendHierarchy::RemoveUncompressedData, PATCH_JUMP);

	InjectHook(0x401B10, &CAnimBlendNode::Init, PATCH_JUMP);
	InjectHook(0x401B30, &CAnimBlendNode::Update, PATCH_JUMP);
	InjectHook(0x401DC0, &CAnimBlendNode::NextKeyFrame, PATCH_JUMP);
	InjectHook(0x401E70, &CAnimBlendNode::CalcDeltas, PATCH_JUMP);
	InjectHook(0x401FE0, &CAnimBlendNode::GetCurrentTranslation, PATCH_JUMP);
	InjectHook(0x402110, &CAnimBlendNode::GetEndTranslation, PATCH_JUMP);
	InjectHook(0x4021B0, &CAnimBlendNode::FindKeyFrame, PATCH_JUMP);

	InjectHook(0x4022D0, &CAnimBlendSequence::ctor, PATCH_JUMP);
	InjectHook(0x402300, &CAnimBlendSequence::dtor, PATCH_JUMP);
	InjectHook(0x402330, &CAnimBlendSequence::SetName, PATCH_JUMP);
	InjectHook(0x402350, &CAnimBlendSequence::SetNumFrames, PATCH_JUMP);
	InjectHook(0x4023A0, &CAnimBlendSequence::RemoveQuaternionFlips, PATCH_JUMP);
	InjectHook(0x402470, &CAnimBlendSequence::dtor2, PATCH_JUMP);

	InjectHook(0x403380, CAnimManager::Initialise, PATCH_JUMP);
	InjectHook(0x4033B0, CAnimManager::Shutdown, PATCH_JUMP);
	InjectHook(0x403410, CAnimManager::UncompressAnimation, PATCH_JUMP);
	InjectHook(0x4034A0, CAnimManager::GetAnimationBlock, PATCH_JUMP);
	InjectHook(0x4034F0, CAnimManager::GetAnimation, PATCH_JUMP);
	InjectHook(0x4035B0, CAnimManager::GetAnimGroupName, PATCH_JUMP);
	InjectHook(0x4035C0, (CAnimBlendAssociation*(*)(int, int))&CAnimManager::CreateAnimAssociation, PATCH_JUMP);
	InjectHook(0x4035E0, (CAnimBlendAssociation *(*)(int, int))CAnimManager::GetAnimAssociation, PATCH_JUMP);
	InjectHook(0x403600, (CAnimBlendAssociation *(*)(int, const char*))CAnimManager::GetAnimAssociation, PATCH_JUMP);
	InjectHook(0x403620, CAnimManager::AddAnimation, PATCH_JUMP);
	InjectHook(0x4036A0, CAnimManager::AddAnimationAndSync, PATCH_JUMP);
	InjectHook(0x403710, CAnimManager::BlendAnimation, PATCH_JUMP);
	InjectHook(0x4038F0, CAnimManager::LoadAnimFiles, PATCH_JUMP);
	InjectHook(0x403A10, (void (*)(const char*))CAnimManager::LoadAnimFile, PATCH_JUMP);
	InjectHook(0x403A40, (void (*)(int, bool))CAnimManager::LoadAnimFile, PATCH_JUMP);
	InjectHook(0x404320, CAnimManager::RemoveLastAnimFile, PATCH_JUMP);

	InjectHook(0x4024B0, RpAnimBlendClumpUpdateAnimations, PATCH_JUMP);
	InjectHook(0x4031B0, (CAnimBlendAssociation *(*)(RpClump*))RpAnimBlendClumpGetFirstAssociation, PATCH_JUMP);
	InjectHook(0x405240, AnimBlendClumpDestroy, PATCH_JUMP);
	InjectHook(0x4052A0, RpAnimBlendAllocateData, PATCH_JUMP);
	InjectHook(0x405430, RpAnimBlendClumpFindFrame, PATCH_JUMP);
	InjectHook(0x405460, RpAnimBlendClumpFillFrameArray, PATCH_JUMP);
	InjectHook(0x405480, RpAnimBlendClumpInit, PATCH_JUMP);
	InjectHook(0x405500, RpAnimBlendClumpIsInitialized, PATCH_JUMP);
	InjectHook(0x405520, RpAnimBlendClumpSetBlendDeltas, PATCH_JUMP);
	InjectHook(0x405560, RpAnimBlendClumpRemoveAllAssociations, PATCH_JUMP);
	InjectHook(0x405570, RpAnimBlendClumpRemoveAssociations, PATCH_JUMP);
	InjectHook(0x4055C0, RpAnimBlendClumpGetAssociation, PATCH_JUMP);
	InjectHook(0x4055F0, RpAnimBlendClumpGetMainAssociation, PATCH_JUMP);
	InjectHook(0x405680, RpAnimBlendClumpGetMainPartialAssociation, PATCH_JUMP);
	InjectHook(0x4056D0, RpAnimBlendClumpGetMainAssociation_N, PATCH_JUMP);
	InjectHook(0x405710, RpAnimBlendClumpGetMainPartialAssociation_N, PATCH_JUMP);
	InjectHook(0x405750, (CAnimBlendAssociation *(*)(RpClump*, uint))RpAnimBlendClumpGetFirstAssociation, PATCH_JUMP);
	InjectHook(0x405780, (CAnimBlendAssociation *(*)(CAnimBlendAssociation*))RpAnimBlendGetNextAssociation, PATCH_JUMP);
	InjectHook(0x4057A0, (CAnimBlendAssociation *(*)(CAnimBlendAssociation*, uint))RpAnimBlendGetNextAssociation, PATCH_JUMP);

	InjectHook(0x4F8920, &CClumpModelInfo::CreateInstance, PATCH_JUMP);
	InjectHook(0x4F8830, &CClumpModelInfo::SetClump, PATCH_JUMP);
	InjectHook(0x510210, &CPedModelInfo::SetClump, PATCH_JUMP);
	InjectHook(0x473FC6, &DeleteRwObject_hook);
	InjectHook(0x50BAD0, &CModelInfo::AddPedModel, PATCH_JUMP);
	Patch(0x5FE004, &CPedModelInfo::DeleteRwObject);

	// patch CModelInfo::ShutDown() instead of rewriting that weird code
	Patch(0x50B3B9+2, &CModelInfo::ms_pedModelStore);
	Patch(0x50B6EC, &CModelInfo::ms_pedModelStore);
	Patch(0x50B6F4, &CModelInfo::ms_pedModelStore);
	Patch(0x50B70B, &CModelInfo::ms_pedModelStore);
	Patch(0x50B708, (BYTE)sizeof(CPedModelInfo));

	InterceptCall(&InitialiseGame, InitialiseGame_hook, 0x582E6C);

	// bat FX
	extern void CSpecialFX__Update_Patch();
	InjectHook(0x518D9C, CSpecialFX__Update_Patch, PATCH_CALL);
	InjectHook(0x518D9C+5, 0x518DBE, PATCH_JUMP);

#ifdef ADAPTHIERARCHY
	InjectHook(0x404D3A, cutscene_copyanimation_hook);
#endif

	pedikhooks();
	pedhooks();
	objecthooks();
	handhooks();

	//dumpAnimGroups();

	// DONE:
	//	-CCutsceneHead::CCutsceneHead
	//	-CCutsceneHead::ProcessControl
	//	-CCutsceneHead::Render
	//	-CCutsceneObject::ProcessControl
	//	-CCutsceneObject::Render
	//	 -CCutsceneObject::RenderLimb
	//	-CCopPed::CopAI
	//	 -CPedModelInfo::CreateHitColModelSkinned
	//	 -CPedModelInfo::AnimatePedColModelSkinned
	//	-CPed::SetModelIndex
	//	-CPed::StartFightDefend
	//	-CPed::FightStrike
	//	-CWorld::ProcessLineOfSightSectorList
	// -CPed::SpawnFlyingComponent	removed
	// TODO:
	// xbox only: script 756
	// hand shit: sub_14C8AA
	// mobile only: CCutsceneHead::RenderLimb ??
}

BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	if(reason == DLL_PROCESS_ATTACH){
		dllModule = hInst;
/*
		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
*/
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)&DllMain, &hDummyHandle);

		if (*(DWORD*)0x5C1E75 == 0xB85548EC)	// 1.0
			patch10();
		else
			return FALSE;
	}

	return TRUE;
}
