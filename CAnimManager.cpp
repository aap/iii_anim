#include "iii_anim.h"

AnimAssocDefinition *CAnimManager::ms_aAnimAssocDefinitions = (AnimAssocDefinition*)0x5EB724;
CAnimBlock *CAnimManager::ms_aAnimBlocks = (CAnimBlock*)0x6F01A0;
CAnimBlendHierarchy *CAnimManager::ms_aAnimations = (CAnimBlendHierarchy*)0x70F430;
int &CAnimManager::ms_numAnimBlocks = *(int*)0x885AF8;
int &CAnimManager::ms_numAnimations = *(int*)0x8E2DD4;
CAnimBlendAssocGroup *&CAnimManager::ms_aAnimAssocGroups = *(CAnimBlendAssocGroup**)0x8F583C;
CLinkList_CAnimBlendHierarchy *CAnimManager::ms_animCache = (CLinkList_CAnimBlendHierarchy*)0x9414DC;

void
CAnimManager::Initialise(void)
{
	ms_numAnimations = 0;
	ms_numAnimBlocks = 0;
	CAnimManager::ms_animCache->Init(25);
}

void
CAnimManager::Shutdown(void)
{
	for(int i = 0; i < CAnimManager::ms_numAnimations; i++)
		CAnimManager::ms_aAnimations[i].Shutdown();
	CAnimManager::ms_animCache->Shutdown();
	if(CAnimManager::ms_aAnimAssocGroups)
		destroy_array(CAnimManager::ms_aAnimAssocGroups, &CAnimBlendAssocGroup::dtor);
}

void
CAnimManager::UncompressAnimation(CAnimBlendHierarchy *anim)
{
	if(!anim->compressed){
		if(anim->linkPtr){
			anim->linkPtr->Remove();
			CAnimManager::ms_animCache->head.Insert(anim->linkPtr);
		}
	}else{
		CLink_CAnimBlendHierarchy *link = CAnimManager::ms_animCache->Insert(&anim);
		if(link == NULL){
			CAnimManager::ms_animCache->tail.prev->item->RemoveUncompressedData();
			CAnimManager::ms_animCache->Remove(CAnimManager::ms_animCache->tail.prev);
			link = CAnimManager::ms_animCache->Insert(&anim);
		}
		anim->linkPtr = link;
		anim->Uncompress();
	}
}

CAnimBlock*
CAnimManager::GetAnimationBlock(const char *name)
{
	for(int i = 0; i < CAnimManager::ms_numAnimBlocks; i++)
		if(lcstrcmp(CAnimManager::ms_aAnimBlocks[i].name, name) == 0)
			return &CAnimManager::ms_aAnimBlocks[i];
	return NULL;
}

CAnimBlendHierarchy*
CAnimManager::GetAnimation(const char *name, CAnimBlock *animBlock)
{
	CAnimBlendHierarchy *anim = &CAnimManager::ms_aAnimations[animBlock->animIndex];
	for(int i = 0; i < animBlock->numAnims; i++){
		if(lcstrcmp(anim->name, name) == 0)
			return anim;
		anim++;
	}
	return NULL;
}

const char *
CAnimManager::GetAnimGroupName(int i)
{
	return CAnimManager::ms_aAnimAssocDefinitions[i].name;
}

CAnimBlendAssociation*
CAnimManager::CreateAnimAssociation(int groupId, int animId)
{
	return CAnimManager::ms_aAnimAssocGroups[groupId].CopyAnimation(animId);
}

CAnimBlendAssociation*
CAnimManager::GetAnimAssociation(int groupId, int animId)
{
	return CAnimManager::ms_aAnimAssocGroups[groupId].GetAnimation(animId);
}

CAnimBlendAssociation*
CAnimManager::GetAnimAssociation(int groupId, const char *name)
{
	return CAnimManager::ms_aAnimAssocGroups[groupId].GetAnimation(name);
}

CAnimBlendAssociation*
CAnimManager::AddAnimation(RpClump *clump, int groupId, int animId)
{
	CAnimBlendAssociation *anim = CAnimManager::CreateAnimAssociation(groupId, animId);
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	if (anim->flags & CAnimBlendAssociation::Movement){
		CAnimBlendAssociation *syncanim = NULL;
		void *next;
		for(next = clumpData->nextAssoc; next; next = *(void**)next){
			syncanim = (CAnimBlendAssociation*)((void**)next - 1);
			if(syncanim->flags & CAnimBlendAssociation::Movement)
				break;
		}
		if(next){
			anim->SyncAnimation(syncanim);
			anim->flags |= 1;
		}else
			anim->Start(0.0f);
	}else
		anim->Start(0.0f);

	// insert into linked list;
	void *tmp = &anim->next;
	if(clumpData->nextAssoc)
		*((void**)clumpData->nextAssoc + 1) = tmp;
	*(void**)tmp = clumpData->nextAssoc;
	anim->prev = clumpData;
	clumpData->nextAssoc = tmp;
	return anim;
}

CAnimBlendAssociation*
CAnimManager::AddAnimationAndSync(RpClump *clump, CAnimBlendAssociation *syncanim, int groupId, int animId)
{
	CAnimBlendAssociation *anim = CAnimManager::CreateAnimAssociation(groupId, animId);
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	if (anim->flags & CAnimBlendAssociation::Movement && syncanim){
		anim->SyncAnimation(syncanim);
		anim->flags |= 1;
	}else
		anim->Start(0.0f);

	// insert into linked list;
	void *tmp = &anim->next;
	if(clumpData->nextAssoc)
		*((void**)clumpData->nextAssoc + 1) = tmp;
	*(void**)tmp = clumpData->nextAssoc;
	anim->prev = clumpData;
	clumpData->nextAssoc = tmp;
	return anim;
}

CAnimBlendAssociation*
CAnimManager::BlendAnimation(RpClump *clump, int groupId, int animId, float delta)
{
	int removePrevAnim = 0;
	int isMovement, isPartial;
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	CAnimBlendAssociation *anim = CAnimManager::ms_aAnimAssocGroups[groupId].GetAnimation(animId);
	isMovement = !!(anim->flags & CAnimBlendAssociation::Movement);
	isPartial = !!(anim->flags & CAnimBlendAssociation::Partial);
	void *next;
	CAnimBlendAssociation *found = NULL, *movementAnim = NULL;
	for(next = clumpData->nextAssoc; next; next = *(void**)next){
		anim = (CAnimBlendAssociation*)((void**)next - 1);
		if(isMovement && (anim->flags & CAnimBlendAssociation::Movement))
			movementAnim = anim;
		if(anim->animId == animId)
			found = anim;
		else{
			if(isPartial == !!(anim->flags & CAnimBlendAssociation::Partial)){
				if(anim->blendAmount <= 0.0f)
					anim->blendDelta = -1.0f;
				else{
					float x = -delta*anim->blendAmount;
					if(x < anim->blendDelta || !isPartial)
						anim->blendDelta = x;
				}
				anim->flags |= 4;
				removePrevAnim = 1;
			}
		}
	}
	if(found){
		found->blendDelta = (1.0f - found->blendAmount)*delta;
		if(!(found->flags & 1) && found->currentTime == found->hierarchy->totalLength)
			found->Start(0.0f);
	}else{
		found = CAnimManager::AddAnimationAndSync(clump, movementAnim, groupId, animId);
		if(!removePrevAnim && !isPartial){
			found->blendAmount = 1.0f;
			return found;
		}
		found->blendAmount = 0.0f;
		found->blendDelta = delta;
	}
	CAnimManager::UncompressAnimation(found->hierarchy);
	return found;
}

void
CAnimManager::LoadAnimFiles(void)
{
	CAnimManager::LoadAnimFile("ANIM\\PED.IFP");
	void *mem = gta_nw(sizeof(CAnimBlendAssocGroup) * 25 + 8);
	CAnimManager::ms_aAnimAssocGroups = (CAnimBlendAssocGroup *)construct_array(
		mem, &CAnimBlendAssocGroup::ctor, &CAnimBlendAssocGroup::dtor,
		sizeof(CAnimBlendAssocGroup), 25);

	for(int i = 0; i < 25; i++){
		void *model = CModelInfo::ms_modelInfoPtrs[CAnimManager::ms_aAnimAssocDefinitions[i].modelIndex];
		// wtf?
		//((void (__thiscall*)(void*)) (*(void***)model)[5])(model);	// no-op
		// CClumpModelInfo::CreateInstance()
		RpClump *clump = ((RpClump* (__thiscall*)(void*)) (*(void***)model)[3])(model);
		RpAnimBlendClumpInit(clump);
		CAnimBlendAssocGroup *group = &CAnimManager::ms_aAnimAssocGroups[i];
		AnimAssocDefinition *def = &CAnimManager::ms_aAnimAssocDefinitions[i];
		group->CreateAssociations(def->blockName, clump, def->animNames, def->animCount);
		for(int j = 0; j < group->numAssociations; j++)
			group->GetAnimation(def->animInfoList[j].animId)->flags |= def->animInfoList[j].flags;
		RpClumpDestroy(clump);
	}
}

void
CAnimManager::LoadAnimFile(const char *filename)
{
	int fd = CFileMgr::OpenFile(filename, "rb");
	CAnimManager::LoadAnimFile(fd, 1);
	return CFileMgr::CloseFile(fd);
}

void
CAnimManager::LoadAnimFile(int fd, bool removeUncompressed)
{
	#define ROUNDSIZE(x) if((x) & 3) (x) += 4 - ((x)&3)
	struct IfpHeader {
		char ident[4];
		RwUInt32 size;
	};
	IfpHeader anpk, info, name, dgan, cpan, anim;
	int numANPK;
	char buf[256];
	float *fbuf = (float*)buf;

	CFileMgr::Read(fd, &anpk, sizeof(IfpHeader));
	if(strncmp(anpk.ident, "ANLF", 4) == 0){
		ROUNDSIZE(anpk.size);
		CFileMgr::Read(fd, buf, anpk.size);
		numANPK = *(int*)buf;
	}else if(strncmp(anpk.ident, "ANPK", 4) == 0){
		CFileMgr::Seek(fd, -8, 1);
		numANPK = 1;
	}

	for(int i = 0; i < numANPK; i++){
		CFileMgr::Read(fd, &anpk, sizeof(IfpHeader));
		ROUNDSIZE(anpk.size);
		CFileMgr::Read(fd, &info, sizeof(IfpHeader));
		ROUNDSIZE(info.size);
		CFileMgr::Read(fd, buf, info.size);
		CAnimBlock *animBlock = &CAnimManager::ms_aAnimBlocks[CAnimManager::ms_numAnimBlocks++];
		strncpy(animBlock->name, buf+4, 24);
		animBlock->numAnims = *(int*)buf;
		animBlock->animIndex = CAnimManager::ms_numAnimations;
		for(int j = 0; j < animBlock->numAnims; j++){
			CAnimBlendHierarchy *hier = &CAnimManager::ms_aAnimations[CAnimManager::ms_numAnimations++];
			CFileMgr::Read(fd, &name, sizeof(IfpHeader));
			ROUNDSIZE(name.size);
			CFileMgr::Read(fd, buf, name.size);
			hier->SetName(buf);

			CFileMgr::Read(fd, &dgan, sizeof(IfpHeader));
			ROUNDSIZE(dgan.size);
			CFileMgr::Read(fd, &info, sizeof(IfpHeader));
			ROUNDSIZE(info.size);
			CFileMgr::Read(fd, buf, info.size);

			hier->numSequences = *(int*)buf;
			void *mem = gta_nw(sizeof(CAnimBlendSequence)*hier->numSequences + 8);
			hier->blendSequences = (CAnimBlendSequence*)construct_array(mem, &CAnimBlendSequence::ctor, &CAnimBlendSequence::dtor,
			                                                            sizeof(CAnimBlendSequence), hier->numSequences);
			CAnimBlendSequence *seq = hier->blendSequences;
			for(int k = 0; k < hier->numSequences; k++, seq++){
				CFileMgr::Read(fd, &cpan, sizeof(IfpHeader));
				ROUNDSIZE(dgan.size);
				CFileMgr::Read(fd, &anim, sizeof(IfpHeader));
				ROUNDSIZE(anim.size);
				CFileMgr::Read(fd, buf, anim.size);
				if(anim.size == 44)
					seq->SetBoneTag(*(int*)(buf+40));
				seq->SetName(buf);
				int numFrames = *(int*)(buf+28);
				if(numFrames == 0)
					continue;
				CFileMgr::Read(fd, &info, sizeof(info));
				if(strncmp(info.ident, "KR00", 4) == 0){
					seq->SetNumFrames(numFrames, 0);
					RFrame *frame = (RFrame*)GETFRAME(seq, 0);
					for(int l = 0; l < numFrames; l++, frame++){
						CFileMgr::Read(fd, buf, 0x14);
						frame->rot.x = -fbuf[0];
						frame->rot.y = -fbuf[1];
						frame->rot.z = -fbuf[2];
						frame->rot.w = fbuf[3];
						frame->time = fbuf[4];
					}
				}else if(strncmp(info.ident, "KRT0", 4) == 0){
					seq->SetNumFrames(numFrames, 1);
					RTFrame *frame = (RTFrame*)GETFRAME(seq, 0);
					for(int l = 0; l < numFrames; l++, frame++){
						CFileMgr::Read(fd, buf, 0x20);
						frame->rot.x = -fbuf[0];
						frame->rot.y = -fbuf[1];
						frame->rot.z = -fbuf[2];
						frame->rot.w = fbuf[3];
						frame->pos.x = fbuf[4];
						frame->pos.y = fbuf[5];
						frame->pos.z = fbuf[6];
						frame->time = fbuf[7];
					}
				}else if(strncmp(info.ident, "KRTS", 4) == 0){
					seq->SetNumFrames(numFrames, 1);
					RTFrame *frame = (RTFrame*)GETFRAME(seq, 0);
					for(int l = 0; l < numFrames; l++, frame++){
						CFileMgr::Read(fd, buf, 0x2C);
						frame->rot.x = -fbuf[0];
						frame->rot.y = -fbuf[1];
						frame->rot.z = -fbuf[2];
						frame->rot.w = fbuf[3];
						frame->pos.x = fbuf[4];
						frame->pos.y = fbuf[5];
						frame->pos.z = fbuf[6];
						frame->time = fbuf[10];
					}
				}
				for(int l = seq->numFrames-1; l > 0; l--){
					RFrame *f1 = (RFrame*)GETFRAME(seq, l);
					RFrame *f2 = (RFrame*)GETFRAME(seq, l-1);
					f1->time -= f2->time;
				}
			}
			hier->RemoveQuaternionFlips();
			if(removeUncompressed)
				hier->RemoveUncompressedData();
			else
				hier->CalcTotalTime();
		}
	}
}

void
CAnimManager::RemoveLastAnimFile(void)
{
	CAnimManager::ms_numAnimBlocks--;
	CAnimManager::ms_numAnimations = CAnimManager::ms_aAnimBlocks[CAnimManager::ms_numAnimBlocks].animIndex;
	for(int i = 0; i < CAnimManager::ms_aAnimBlocks[CAnimManager::ms_numAnimBlocks].numAnims; i++)
		CAnimManager::ms_aAnimations[CAnimManager::ms_aAnimBlocks[CAnimManager::ms_numAnimBlocks].animIndex + i].RemoveAnimSequences();
}
