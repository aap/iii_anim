#include "iii_anim.h"

int &CAnimBlendSequence_VTable = *(int*)0x5EA060;

CAnimBlendSequence::CAnimBlendSequence(void) { ctor(); }

CAnimBlendSequence::~CAnimBlendSequence(void) { dtor(); }

void
CAnimBlendSequence::ctor(void)
{
	this->vtable = &CAnimBlendSequence_VTable;
	this->flag = 0;
	this->numFrames = 0;
	this->keyFrames = NULL;
	this->keyFramesCompressed = NULL;
	this->boneTag = -1;
}

void
CAnimBlendSequence::dtor(void)
{
	this->vtable = &CAnimBlendSequence_VTable;
	if(this->keyFrames)
		RwFree(this->keyFrames);
}

void
CAnimBlendSequence::dtor2(char flag)
{
	if(this)
		if(flag & 2)
			destroy_array(this, &CAnimBlendSequence::dtor);
		else{
			dtor();
			if(flag & 1)
				gtadelete(this);
		}
}

void
CAnimBlendSequence::SetName(const char *name)
{
	strncpy(this->name, name, 24);
}

void
CAnimBlendSequence::SetBoneTag(int tag)
{
	this->boneTag = tag;
}

void
CAnimBlendSequence::SetNumFrames(int numFrames, bool TS)
{
	void *frames;
	if(!TS){
		// just rotation
		this->flag |= 1;
		frames = RwMalloc(sizeof(RFrame) * numFrames);
	}else{
		// rotation and translate (ignore scale)
		this->flag |= 3;
		frames = RwMalloc(sizeof(RTFrame) * numFrames);
	}
	this->keyFrames = frames;
	this->numFrames = numFrames;
}

void
CAnimBlendSequence::RemoveQuaternionFlips(void)
{
	if(this->numFrames < 2)
		return;
	CQuaternion q, *p;
	RFrame *frame = (RFrame*)this->keyFrames;
	q = frame->rot;
	for(int i = 1; i < this->numFrames; i++){
		frame = (RFrame*)(GETFRAME(this, i));
		p = &frame->rot;
		if(q.x*p->x + q.y*p->y + q.z*p->z + q.w*p->w < 0.0f){
			p->x = -p->x;
			p->y = -p->y;
			p->z = -p->z;
			p->w = -p->w;
		}
		q = *p;
	}
}
