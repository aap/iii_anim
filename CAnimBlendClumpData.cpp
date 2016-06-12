#include "iii_anim.h"

CAnimBlendClumpData::CAnimBlendClumpData(void) { ctor(); }

CAnimBlendClumpData::~CAnimBlendClumpData(void) { dtor(); }

void
CAnimBlendClumpData::ctor(void)
{
	this->numFrames = 0;
	this->pedPosition = NULL;
	this->frames = NULL;
	this->nextAssoc = NULL;
	this->prevAssoc = NULL;
}

void
CAnimBlendClumpData::dtor(void)
{
	if(this->prevAssoc)
		*(void**)this->prevAssoc = this->nextAssoc;
	if(this->nextAssoc)
		*((void**)this->nextAssoc + 1) = this->prevAssoc;
	this->nextAssoc = NULL;
	this->prevAssoc = NULL;
	if(this->frames)
		RwFreeAlign(this->frames);
}

void
CAnimBlendClumpData::SetNumberOfBones(int n)
{
	if(this->frames)
		RwFreeAlign(this->frames);
	this->frames = (AnimBlendFrameData*)RwMallocAlign((sizeof(AnimBlendFrameData)*n + 0x3F)&~0x3F, 64);
	this->numFrames = n;
}

void
CAnimBlendClumpData::ForAllFrames(void (*cb)(AnimBlendFrameData*, void*), void *arg)
{
	for(int i = 0; i < this->numFrames; i++)
		cb(&this->frames[i], arg);
}
