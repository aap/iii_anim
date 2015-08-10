#include "iii_anim.h"

int &CAnimBlendAssociation_VTable = *(int*)0x5EA02C;

CAnimBlendAssociation::CAnimBlendAssociation(void) { ctor(); }
CAnimBlendAssociation::CAnimBlendAssociation(CAnimBlendAssociation &a)
{
	this->vtable = &CAnimBlendAssociation_VTable;
	this->nodes = NULL;
	this->blendAmount = 1.0f;
	this->blendDelta = 0.0f;
	this->currentTime = 0.0f;
	this->speed = 1.0f;
	this->timeStep = 0.0f;
	this->callbackType = 0;
	this->next = NULL;
	this->prev = NULL;
	this->Init(a);
}
CAnimBlendAssociation::~CAnimBlendAssociation(void) { dtor(); }

void
CAnimBlendAssociation::ctor(void)
{
	this->vtable = &CAnimBlendAssociation_VTable;
	this->nodes = NULL;
	this->blendAmount = 1.0f;
	this->blendDelta = 0.0f;
	this->currentTime = 0.0f;
	this->speed = 1.0f;
	this->timeStep = 0.0f;
	this->animId = -1;
	this->flags = 0;
	this->callbackType = 0;
	this->next = NULL;
	this->prev = NULL;
}

void
CAnimBlendAssociation::dtor(void)
{
	this->vtable = &CAnimBlendAssociation_VTable;	//?
	if(this->prev)
		*(void**)this->prev = this->next;
	if(this->next)
		*((void**)this->next + 1) = this->prev;
	this->next = 0;
	this->prev = 0;
}

void
CAnimBlendAssociation::Init(RpClump *clump, CAnimBlendHierarchy *anim)
{
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	this->numNodes = clumpData->numFrames;
	this->AllocateAnimBlendNodeArray(this->numNodes);
	for(int i = 0; i < this->numNodes; i++){
		this->nodes[i].sequence = (CAnimBlendSequence*)0xbeefbeef;
		this->nodes[i].blendAssoc = this;
	}
	this->hierarchy = anim;
	AnimBlendFrameData *frameData;
	for(int i = 0; i < anim->numSequences; i++){
		CAnimBlendSequence *seq = &anim->blendSequences[i];
		frameData = RpAnimBlendClumpFindFrame(clump, seq->name);
		if(frameData && seq->numFrames > 0)
			this->nodes[frameData - clumpData->frames].sequence = seq;
	}
}

void
CAnimBlendAssociation::Init(CAnimBlendAssociation &anim)
{
	this->hierarchy = anim.hierarchy;
	this->numNodes = anim.numNodes;
	this->flags = anim.flags;
	this->animId = anim.animId;
	this->AllocateAnimBlendNodeArray(this->numNodes);
	for(int i = 0; i < this->numNodes; i++){
		this->nodes[i].theta0 = anim.nodes[i].theta0;
		this->nodes[i].theta1 = anim.nodes[i].theta1;
		this->nodes[i].frame0 = anim.nodes[i].frame0;
		this->nodes[i].frame1 = anim.nodes[i].frame1;
		this->nodes[i].time = anim.nodes[i].time;
		this->nodes[i].sequence = anim.nodes[i].sequence;
		this->nodes[i].blendAssoc = this;
	}
}

void
CAnimBlendAssociation::AllocateAnimBlendNodeArray(int n)
{
	this->nodes = (CAnimBlendNode*)RwMallocAlign((sizeof(CAnimBlendNode)*this->numNodes + 0x3F)&~0x3F, 64);
	for(int i = 0; i < this->numNodes; i++)
		this->nodes[i].Init();
}

void
CAnimBlendAssociation::FreeAnimBlendNodeArray(void)
{
	RwFreeAlign(this->nodes);
}

void
CAnimBlendAssociation::SetCurrentTime(float time)
{
	for(this->currentTime = time;
	    this->currentTime >= this->hierarchy->totalLength;
	    this->currentTime -= this->hierarchy->totalLength)
		if(!(this->flags & 2))
			return;
	CAnimManager::UncompressAnimation(this->hierarchy);
	for(int i = 0; i < this->numNodes; i++)
		if(this->nodes[i].sequence)
			this->nodes[i].FindKeyFrame(this->currentTime);
}

void
CAnimBlendAssociation::SyncAnimation(CAnimBlendAssociation *anim)
{
	this->SetCurrentTime(anim->currentTime / anim->hierarchy->totalLength * this->hierarchy->totalLength);
}

CAnimBlendNode*
CAnimBlendAssociation::GetNode(int i)
{
	return &this->nodes[i];
}

void
CAnimBlendAssociation::Start(float time)
{
	this->flags |= 1;
	this->SetCurrentTime(time);
}

void
CAnimBlendAssociation::SetBlend(float blendAmount, float blendDelta)
{
	this->blendAmount = blendAmount;
	this->blendDelta = blendDelta;
}

void
CAnimBlendAssociation::SetDeleteCallback(void (*callback)(CAnimBlendAssociation*, void*), void *arg)
{
	this->callbackType = 2;
	this->callback = callback;
	this->callbackArg = arg;
}

void
CAnimBlendAssociation::SetFinishCallback(void (*callback)(CAnimBlendAssociation*, void*), void *arg)
{
	this->callbackType = 1;
	this->callback = callback;
	this->callbackArg = arg;
}

void
CAnimBlendAssociation::dtor2(char flag)
{
	if(this)
		if(flag & 2)
			destroy_array(this, &CAnimBlendAssociation::dtor);
		else{
			dtor();
			if(flag & 1)
				gtadelete(this);
		}
}

void
CAnimBlendAssociation::UpdateTime(float f1, float f2)
{
	if((this->flags & 1) == 0)
		return;
	this->timeStep = (this->flags & 0x20 ? f2 * this->hierarchy->totalLength : this->speed) * f1;
	this->currentTime += this->timeStep;
	if(this->currentTime >= this->hierarchy->totalLength){
		if(this->flags & 2)
			this->currentTime -= this->hierarchy->totalLength;
		else{
			this->currentTime = this->hierarchy->totalLength;
			this->flags &= ~1;
			if(this->flags & 8){
				this->flags |= 4;
				this->blendDelta = -4.0f;
			}
			if(this->callbackType == 1){
				this->callbackType = 0;
				this->callback(this, this->callbackArg);
			}
		}
	}
}

bool
CAnimBlendAssociation::UpdateBlend(float timeDelta)
{
	this->blendAmount += this->blendDelta * timeDelta;
	if(isnan(this->blendAmount) || this->blendAmount > 0.0f || this->blendDelta >= 0.0f)
		goto xyz;
	this->blendAmount = 0.0f;
	if(this->blendDelta < 0.0f)
		this->blendDelta = 0.0f;
	if(this->flags & 4){
		if(this->callbackType == 1 || this->callbackType == 2)
			this->callback(this, this->callbackArg);
		if(this)
			// destructor (dtor2)
			(*(void(__thiscall**)(CAnimBlendAssociation*, int))this->vtable)(this, 1);
		return 0;
	}else{
xyz:
		if(this->blendAmount > 1.0f){
			this->blendAmount = 1.0f;
			if(this->blendDelta > 0.0f)
				this->blendDelta = 0.0f;
		}
		return 1;
	}
}
