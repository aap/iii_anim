#include "iii_anim.h"

HMODULE dllModule, hDummyHandle;

void **rwengine = *(void***)0x59EEB1;

WRAPPER void *RwMallocAlign(uint, int) { EAXJMP(0x526FD0); }
WRAPPER void RwFreeAlign(void*) { EAXJMP(0x527000); }
WRAPPER void gtadelete(void*) { EAXJMP(0x5A07E0); }
WRAPPER void *gta_nw(int) { EAXJMP(0x5A0690); }

WRAPPER RwMatrix *RwMatrixUpdate(RwMatrix*) { EAXJMP(0x5A28E0); }
WRAPPER RwMatrix *RwMatrixInvert(RwMatrix*, const RwMatrix*) { EAXJMP(0x5A2C90); }
WRAPPER RwMatrix *RwMatrixTransform(RwMatrix*, const RwMatrix*, RwOpCombineType) { EAXJMP(0x5A2EE0); }
WRAPPER RwFrame *RwFrameForAllChildren(RwFrame*, RwFrameCallBack, void*) { EAXJMP(0x5A1FC0); }
WRAPPER RwFrame *RwFrameUpdateObjects(RwFrame*) { EAXJMP(0x5A1C60); }
WRAPPER RwFrame *RwFrameRemoveChild(RwFrame *) { EAXJMP(0x5A1ED0); }
WRAPPER RwBool RwFrameDestroy(RwFrame*) { EAXJMP(0x5A1A30); }
WRAPPER RwV3d *RwV3dTransformPoints(RwV3d*, const RwV3d*, RwInt32, const RwMatrix*) { EAXJMP(0x5A37D0); }
WRAPPER RwBool RpClumpDestroy(RpClump*) { EAXJMP(0x59F500); }
WRAPPER RpClump *RpClumpForAllAtomics(RpClump*, RpAtomicCallBack, void*) { EAXJMP(0x59EDD0); }
WRAPPER RpClump *RpClumpClone(RpClump*) { EAXJMP(0x59F1B0); }
WRAPPER RpClump *RpClumpRemoveAtomic(RpClump*, RpAtomic*) { EAXJMP(0x59F6B0); }
WRAPPER RpClump *RpClumpAddAtomic(RpClump*, RpAtomic*) { EAXJMP(0x59F680); }
WRAPPER RwBool RpAtomicDestroy(RpAtomic*) { EAXJMP(0x59F020); }
WRAPPER RpSkin *RpSkinGeometryGetSkin(RpGeometry*) { EAXJMP(0x5B1080); }
WRAPPER RpAtomic *RpSkinAtomicSetHAnimHierarchy(RpAtomic*, RpHAnimHierarchy*) { EAXJMP(0x5B1050); }
WRAPPER RpHAnimHierarchy *RpSkinAtomicGetHAnimHierarchy(const RpAtomic*) { EAXJMP(0x5B1070); }
WRAPPER RwBool RpHAnimHierarchyUpdateMatrices(RpHAnimHierarchy *hierarchy) { EAXJMP(0x5B1780); }
WRAPPER RpHAnimHierarchy *RpHAnimFrameGetHierarchy(RwFrame*) { EAXJMP(0x5B11F0); }
WRAPPER RwBool RpHAnimHierarchySetCurrentAnim(RpHAnimHierarchy*, RpHAnimAnimation*) { EAXJMP(0x5B1200); }

WRAPPER const char *GetFrameNodeName(RwFrame *frame) { EAXJMP(0x527150); }
WRAPPER void *GetModelFromName(char *name) { EAXJMP(0x4010D0); }
WRAPPER RpAtomic *GetFirstAtomic(RpClump *clump) { EAXJMP(0x526420); }
WRAPPER int CFileMgr::OpenFile(const char*, const char*) { EAXJMP(0x479100); }
WRAPPER int CFileMgr::Read(int, void*, int) { EAXJMP(0x479140); }
WRAPPER void CFileMgr::Seek(int, int, int) { EAXJMP(0x479180); }
WRAPPER void CFileMgr::CloseFile(int) { EAXJMP(0x479200); }
WRAPPER void CQuaternion::Slerp(CQuaternion&, CQuaternion&, float, float, float) { EAXJMP(0x4BA1C0); }
WRAPPER void CQuaternion::Get(RwMatrix *mat) { EAXJMP(0x4BA0D0); }

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
	return 0;
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
	if(RpSkinGeometryGetSkin(atomic->geometry))
		*ret = atomic;
	return atomic;
}

RpAtomic*
IsClumpSkinned(RpClump *c)
{
	RpAtomic *ret = NULL;
	RpClumpForAllAtomics(c, isSkinnedCb, &ret);
	return ret;
}

/*
RpAtomic *atomicArray[20];
int atomicArraySP = 0;

static RpAtomic*
atomicsToArrayCB(RpAtomic *atomic, void *)
{
	return atomicArray[atomicArraySP++] = atomic;
}

void
atomicsToArray(RpClump *clump)
{
	atomicArraySP = 0;
	RpClumpForAllAtomics(clump, atomicsToArrayCB, NULL);
}
*/

void
patch10(void)
{
/*
	if(sizeof(CAnimBlendSequence) != 0x2C ||		//
	   sizeof(CAnimBlendHierarchy) != 0x28 ||
	   sizeof(CAnimBlock) != 0x20 ||
	   sizeof(CAnimBlendNode) != 0x1C ||
	   sizeof(CAnimBlendAssociation) != 0x40 ||		//
	   sizeof(CAnimBlendAssocGroup) != 0x8 ||
	   sizeof(AnimBlendFrameData) != 0x14 ||
	   sizeof(CAnimBlendClumpData) != 0x14 ||
	   sizeof(RFrame) != 0x14 ||
	   sizeof(RTFrame) != 0x20){
		printf("SIZE MISMATCH\n");
		return;
	}
	printf("sizes ok\n");
*/

	MemoryVP::InjectHook(0x401000, &CAnimBlendAssocGroup::ctor, PATCH_JUMP);
	MemoryVP::InjectHook(0x401130, static_cast<void(CAnimBlendAssocGroup::*)(const char*)>(&CAnimBlendAssocGroup::CreateAssociations), PATCH_JUMP);
	MemoryVP::InjectHook(0x401220, static_cast<void(CAnimBlendAssocGroup::*)(const char *name, RpClump *clump, char **names, int numAnims)>(&CAnimBlendAssocGroup::CreateAssociations), PATCH_JUMP);
	MemoryVP::InjectHook(0x4012D0, &CAnimBlendAssocGroup::dtor, PATCH_JUMP);
	MemoryVP::InjectHook(0x401300, static_cast<CAnimBlendAssociation*(CAnimBlendAssocGroup::*)(const char*)>(&CAnimBlendAssocGroup::GetAnimation), PATCH_JUMP);
	MemoryVP::InjectHook(0x4013D0, static_cast<CAnimBlendAssociation*(CAnimBlendAssocGroup::*)(uint)>(&CAnimBlendAssocGroup::GetAnimation), PATCH_JUMP);
	MemoryVP::InjectHook(0x4013E0, static_cast<CAnimBlendAssociation*(CAnimBlendAssocGroup::*)(const char*)>(&CAnimBlendAssocGroup::CopyAnimation), PATCH_JUMP);
	MemoryVP::InjectHook(0x401420, static_cast<CAnimBlendAssociation*(CAnimBlendAssocGroup::*)(uint)>(&CAnimBlendAssocGroup::CopyAnimation), PATCH_JUMP);

	MemoryVP::InjectHook(0x401460, &CAnimBlendAssociation::ctor, PATCH_JUMP);
	MemoryVP::InjectHook(0x401520, &CAnimBlendAssociation::dtor, PATCH_JUMP);
	MemoryVP::InjectHook(0x401560, static_cast<void(CAnimBlendAssociation::*)(RpClump *clump, CAnimBlendHierarchy *anim)>(&CAnimBlendAssociation::Init), PATCH_JUMP);
	MemoryVP::InjectHook(0x401620, static_cast<void(CAnimBlendAssociation::*)(CAnimBlendAssociation&)>(&CAnimBlendAssociation::Init), PATCH_JUMP);
	MemoryVP::InjectHook(0x4016A0, &CAnimBlendAssociation::AllocateAnimBlendNodeArray, PATCH_JUMP);
	MemoryVP::InjectHook(0x4016F0, &CAnimBlendAssociation::FreeAnimBlendNodeArray, PATCH_JUMP);
	MemoryVP::InjectHook(0x401700, &CAnimBlendAssociation::SetCurrentTime, PATCH_JUMP);
	MemoryVP::InjectHook(0x401780, &CAnimBlendAssociation::SyncAnimation, PATCH_JUMP);
	MemoryVP::InjectHook(0x4017B0, &CAnimBlendAssociation::GetNode, PATCH_JUMP);
	MemoryVP::InjectHook(0x4017D0, &CAnimBlendAssociation::Start, PATCH_JUMP);
	MemoryVP::InjectHook(0x4017E0, &CAnimBlendAssociation::SetBlend, PATCH_JUMP);
	MemoryVP::InjectHook(0x401800, &CAnimBlendAssociation::SetDeleteCallback, PATCH_JUMP);
	MemoryVP::InjectHook(0x401820, &CAnimBlendAssociation::SetFinishCallback, PATCH_JUMP);
	MemoryVP::InjectHook(0x401840, &CAnimBlendAssociation::dtor2, PATCH_JUMP);
	MemoryVP::InjectHook(0x4031F0, &CAnimBlendAssociation::UpdateTime, PATCH_JUMP);
	MemoryVP::InjectHook(0x4032B0, &CAnimBlendAssociation::UpdateBlend, PATCH_JUMP);

	MemoryVP::InjectHook(0x401880, &CAnimBlendClumpData::ctor, PATCH_JUMP);
	MemoryVP::InjectHook(0x4018B0, &CAnimBlendClumpData::dtor, PATCH_JUMP);
	MemoryVP::InjectHook(0x4018F0, &CAnimBlendClumpData::SetNumberOfBones, PATCH_JUMP);
	MemoryVP::InjectHook(0x401930, &CAnimBlendClumpData::ForAllFrames, PATCH_JUMP);

	MemoryVP::InjectHook(0x4019A0, &CAnimBlendHierarchy::Shutdown, PATCH_JUMP);
	MemoryVP::InjectHook(0x4019C0, &CAnimBlendHierarchy::SetName, PATCH_JUMP);
	MemoryVP::InjectHook(0x401AB0, &CAnimBlendHierarchy::RemoveAnimSequences, PATCH_JUMP);
	MemoryVP::InjectHook(0x4019E0, &CAnimBlendHierarchy::CalcTotalTime, PATCH_JUMP);
	MemoryVP::InjectHook(0x401A80, &CAnimBlendHierarchy::RemoveQuaternionFlips, PATCH_JUMP);
	MemoryVP::InjectHook(0x401AD0, &CAnimBlendHierarchy::Uncompress, PATCH_JUMP);
	MemoryVP::InjectHook(0x401B00, &CAnimBlendHierarchy::RemoveUncompressedData, PATCH_JUMP);

	MemoryVP::InjectHook(0x401B10, &CAnimBlendNode::Init, PATCH_JUMP);
	MemoryVP::InjectHook(0x401B30, &CAnimBlendNode::Update, PATCH_JUMP);
	MemoryVP::InjectHook(0x401DC0, &CAnimBlendNode::NextKeyFrame, PATCH_JUMP);
	MemoryVP::InjectHook(0x401E70, &CAnimBlendNode::CalcDeltas, PATCH_JUMP);
	MemoryVP::InjectHook(0x401FE0, &CAnimBlendNode::GetCurrentTranslation, PATCH_JUMP);
	MemoryVP::InjectHook(0x402110, &CAnimBlendNode::GetEndTranslation, PATCH_JUMP);
	MemoryVP::InjectHook(0x4021B0, &CAnimBlendNode::FindKeyFrame, PATCH_JUMP);

	MemoryVP::InjectHook(0x4022D0, &CAnimBlendSequence::ctor, PATCH_JUMP);
	MemoryVP::InjectHook(0x402300, &CAnimBlendSequence::dtor, PATCH_JUMP);
	MemoryVP::InjectHook(0x402330, &CAnimBlendSequence::SetName, PATCH_JUMP);
	MemoryVP::InjectHook(0x402350, &CAnimBlendSequence::SetNumFrames, PATCH_JUMP);
	MemoryVP::InjectHook(0x4023A0, &CAnimBlendSequence::RemoveQuaternionFlips, PATCH_JUMP);
	MemoryVP::InjectHook(0x402470, &CAnimBlendSequence::dtor2, PATCH_JUMP);

	MemoryVP::InjectHook(0x403380, CAnimManager::Initialise, PATCH_JUMP);
	MemoryVP::InjectHook(0x4033B0, CAnimManager::Shutdown, PATCH_JUMP);
	MemoryVP::InjectHook(0x403410, CAnimManager::UncompressAnimation, PATCH_JUMP);
	MemoryVP::InjectHook(0x4034A0, CAnimManager::GetAnimationBlock, PATCH_JUMP);
	MemoryVP::InjectHook(0x4034F0, CAnimManager::GetAnimation, PATCH_JUMP);
	MemoryVP::InjectHook(0x4035B0, CAnimManager::GetAnimGroupName, PATCH_JUMP);
	MemoryVP::InjectHook(0x4035C0, CAnimManager::CreateAnimAssociation, PATCH_JUMP);
	MemoryVP::InjectHook(0x4035E0, (CAnimBlendAssociation *(*)(int, int))CAnimManager::GetAnimAssociation, PATCH_JUMP);
	MemoryVP::InjectHook(0x403600, (CAnimBlendAssociation *(*)(int, const char*))CAnimManager::GetAnimAssociation, PATCH_JUMP);
	MemoryVP::InjectHook(0x403620, CAnimManager::AddAnimation, PATCH_JUMP);
	MemoryVP::InjectHook(0x4036A0, CAnimManager::AddAnimationAndSync, PATCH_JUMP);
	MemoryVP::InjectHook(0x403710, CAnimManager::BlendAnimation, PATCH_JUMP);
	MemoryVP::InjectHook(0x4038F0, CAnimManager::LoadAnimFiles, PATCH_JUMP);
	MemoryVP::InjectHook(0x403A10, (void (*)(const char*))CAnimManager::LoadAnimFile, PATCH_JUMP);
	MemoryVP::InjectHook(0x403A40, (void (*)(int, bool))CAnimManager::LoadAnimFile, PATCH_JUMP);
	MemoryVP::InjectHook(0x404320, CAnimManager::RemoveLastAnimFile, PATCH_JUMP);

	MemoryVP::InjectHook(0x4024B0, RpAnimBlendClumpUpdateAnimations, PATCH_JUMP);
	MemoryVP::InjectHook(0x4031B0, (CAnimBlendAssociation *(*)(RpClump*))RpAnimBlendClumpGetFirstAssociation, PATCH_JUMP);
	MemoryVP::InjectHook(0x405240, AnimBlendClumpDestroy, PATCH_JUMP);
	MemoryVP::InjectHook(0x4052A0, RpAnimBlendAllocateData, PATCH_JUMP);
	MemoryVP::InjectHook(0x405430, RpAnimBlendClumpFindFrame, PATCH_JUMP);
	MemoryVP::InjectHook(0x405460, RpAnimBlendClumpFillFrameArray, PATCH_JUMP);
	MemoryVP::InjectHook(0x405480, RpAnimBlendClumpInit, PATCH_JUMP);
	MemoryVP::InjectHook(0x405500, RpAnimBlendClumpIsInitialized, PATCH_JUMP);
	MemoryVP::InjectHook(0x405520, RpAnimBlendClumpSetBlendDeltas, PATCH_JUMP);
	MemoryVP::InjectHook(0x405560, RpAnimBlendClumpRemoveAllAssociations, PATCH_JUMP);
	MemoryVP::InjectHook(0x405570, RpAnimBlendClumpRemoveAssociations, PATCH_JUMP);
	MemoryVP::InjectHook(0x4055C0, RpAnimBlendClumpGetAssociation, PATCH_JUMP);
	MemoryVP::InjectHook(0x4055F0, RpAnimBlendClumpGetMainAssociation, PATCH_JUMP);
	MemoryVP::InjectHook(0x405680, RpAnimBlendClumpGetMainPartialAssociation, PATCH_JUMP);
	MemoryVP::InjectHook(0x4056D0, RpAnimBlendClumpGetMainAssociation_N, PATCH_JUMP);
	MemoryVP::InjectHook(0x405710, RpAnimBlendClumpGetMainPartialAssociation_N, PATCH_JUMP);
	MemoryVP::InjectHook(0x405750, (CAnimBlendAssociation *(*)(RpClump*, uint))RpAnimBlendClumpGetFirstAssociation, PATCH_JUMP);
	MemoryVP::InjectHook(0x405780, (CAnimBlendAssociation *(*)(CAnimBlendAssociation*))RpAnimBlendGetNextAssociation, PATCH_JUMP);
	MemoryVP::InjectHook(0x4057A0, (CAnimBlendAssociation *(*)(CAnimBlendAssociation*, uint))RpAnimBlendGetNextAssociation, PATCH_JUMP);

	MemoryVP::InjectHook(0x4F8920, &CClumpModelInfo::CreateInstance, PATCH_JUMP);
	MemoryVP::InjectHook(0x4F8830, &CClumpModelInfo::SetClump, PATCH_JUMP);
	MemoryVP::InjectHook(0x510210, &CPedModelInfo::SetClump, PATCH_JUMP);
	MemoryVP::InjectHook(0x473FC6, &DeleteRwObject_hook);
	MemoryVP::InjectHook(0x50BAD0, &CModelInfo::AddPedModel, PATCH_JUMP);
	MemoryVP::Patch(0x5FE004, &CPedModelInfo::DeleteRwObject);

	pedhooks();
}

BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	if(reason == DLL_PROCESS_ATTACH){
		dllModule = hInst;

/*		AllocConsole();
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);*/

		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)&DllMain, &hDummyHandle);

		if (*(DWORD*)0x5C1E75 == 0xB85548EC)	// 1.0
			patch10();
		else
			return FALSE;
	}

	return TRUE;
}
