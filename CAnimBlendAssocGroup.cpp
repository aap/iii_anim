#include "iii_anim.h"

CAnimBlendAssocGroup::CAnimBlendAssocGroup(void) { ctor(); }
CAnimBlendAssocGroup::~CAnimBlendAssocGroup(void) { dtor(); }

void
CAnimBlendAssocGroup::ctor(void)
{
	this->assocList = NULL;
	this->numAssociations = 0;
}

void
CAnimBlendAssocGroup::dtor(void)
{
	if(this->assocList){
		destroy_array(this->assocList, &CAnimBlendAssociation::dtor);
		this->assocList = NULL;
		this->numAssociations = 0;
	}
}

void
CAnimBlendAssocGroup::CreateAssociations(const char *name)
{
	if(this->assocList)
		dtor();

	CAnimBlock *animBlock = CAnimManager::GetAnimationBlock(name);
	int numAnims = animBlock->numAnims;
	void *mem = gta_nw(sizeof(CAnimBlendAssociation) * numAnims + 8);
	this->assocList = (CAnimBlendAssociation *)construct_array(
		mem, &CAnimBlendAssociation::ctor, &CAnimBlendAssociation::dtor,
		sizeof(CAnimBlendAssociation), numAnims);
	this->numAssociations = 0;

	for(int i = 0; i < numAnims; i++){
		CAnimBlendHierarchy *anim = &CAnimManager::ms_aAnimations[animBlock->animIndex + i];
		void *model = GetModelFromName(anim->name);
		if(model){
			// wtf?
			// CClumpModelInfo::CreateInstance()
			RpClump *clump = ((RpClump* (__thiscall*)(void*)) (*(void***)model)[3])(model);
			RpAnimBlendClumpInit(clump);
			this->assocList[i].Init(clump, anim);
			RpClumpDestroy(clump);
			this->assocList[i].animId = i;
		}
	}
	this->numAssociations = numAnims;
}

void
CAnimBlendAssocGroup::CreateAssociations(const char *name, RpClump *clump, char **names, int numAnims)
{
	if(this->assocList)
		dtor();
	CAnimBlock *animBlock = CAnimManager::GetAnimationBlock(name);
	void *mem = gta_nw(sizeof(CAnimBlendAssociation) * numAnims + 8);
	this->assocList = (CAnimBlendAssociation *)construct_array(
		mem, &CAnimBlendAssociation::ctor, &CAnimBlendAssociation::dtor,
		sizeof(CAnimBlendAssociation), numAnims);
	this->numAssociations = 0;

	for(int i = 0; i < numAnims; i++){
		CAnimBlendHierarchy *anim = CAnimManager::GetAnimation(names[i], animBlock);
		this->assocList[i].Init(clump, anim);
		this->assocList[i].animId = i;
	}
	this->numAssociations = numAnims;
}


CAnimBlendAssociation*
CAnimBlendAssocGroup::GetAnimation(const char *name)
{
	for(int i = 0; i < this->numAssociations; i++)
		if(gtastrcmp(this->assocList[i].hierarchy->name, name) == 0)
			return &this->assocList[i];
	return NULL;
}

CAnimBlendAssociation*
CAnimBlendAssocGroup::GetAnimation(uint i)
{
	return &this->assocList[i];
}

CAnimBlendAssociation*
CAnimBlendAssocGroup::CopyAnimation(const char *name)
{
	CAnimBlendAssociation *anim = GetAnimation(name);
	if(anim){
		CAnimManager::UncompressAnimation(anim->hierarchy);
		CAnimBlendAssociation *copy = (CAnimBlendAssociation *)gta_nw(sizeof(CAnimBlendAssociation));
		if(copy)
			copy = new (copy) CAnimBlendAssociation(*anim);
		return copy;
	}
	return NULL;
}

CAnimBlendAssociation*
CAnimBlendAssocGroup::CopyAnimation(uint i)
{
	CAnimBlendAssociation *anim;
	anim = &this->assocList[i];
	if(anim){
		CAnimManager::UncompressAnimation(anim->hierarchy);
		CAnimBlendAssociation *copy = (CAnimBlendAssociation *)gta_nw(sizeof(CAnimBlendAssociation));
		if(copy)
			copy = new (copy) CAnimBlendAssociation(*anim);
		return copy;
	}
	return NULL;
}
