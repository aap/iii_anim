#include "iii_anim.h"

void
CAnimBlendNode::Init(void)
{
	this->blendAssoc = NULL;
	this->sequence = NULL;
	this->time = 0.0f;
	this->frame1 = -1;
	this->frame0 = -1;
}

bool
CAnimBlendNode::Update(CVector &vec, CQuaternion &quat, float f)
{
	bool retval = 0;
	vec.x = vec.y = vec.z = 0.0f;
	quat.x = quat.y = quat.z = quat.w = 0.0f;
	if(this->blendAssoc->flags & 1){
		this->time -= this->blendAssoc->timeStep;
		if(this->time <= 0.0f)
			retval = this->NextKeyFrame();
	}
	float blendAmount = this->blendAssoc->blendAmount;
	if(!(this->blendAssoc->flags & CAnimBlendAssociation::Partial))
		blendAmount *= f;
	if(blendAmount <= 0.0f)
		return retval;
	RTFrame *frame0 = (RTFrame*)GETFRAME(this->sequence, this->frame0);
	RTFrame *frame1 = (RTFrame*)GETFRAME(this->sequence, this->frame1);
	float r = frame0->time == 0.0f ? 0.0f : (frame0->time - this->time) / frame0->time;
	if(this->sequence->flag & 2){
		vec.x = blendAmount*(r*frame0->pos.x + (1.0f-r)*frame1->pos.x);
		vec.y = blendAmount*(r*frame0->pos.y + (1.0f-r)*frame1->pos.y);
		vec.z = blendAmount*(r*frame0->pos.z + (1.0f-r)*frame1->pos.z);
	}
	if(this->sequence->flag & 1){
		quat.Slerp(frame1->rot, frame0->rot, this->theta0, this->theta1, r);
		quat.x *= blendAmount;
		quat.y *= blendAmount;
		quat.z *= blendAmount;
		quat.w *= blendAmount;
	}
	return retval;
}

bool
CAnimBlendNode::NextKeyFrame(void)
{
	bool retval = 0;
	if(this->sequence->numFrames <= 1)
		return 0;
	this->frame1 = this->frame0;
	while(this->time <= 0.0f){
		this->frame0++;
		if(this->frame0 >= this->sequence->numFrames){
			if(!(this->blendAssoc->flags & 2)){
				this->frame0--;
				this->time = 0.0f;
				return 0;
			}
			retval = 1;
			this->frame0 = 0;
		}
		this->time += ((RFrame*)GETFRAME(this->sequence, this->frame0))->time;
	}
	this->frame1 = this->frame0 - 1;
	if(this->frame1 < 0)
		this->frame1 += this->sequence->numFrames;
	this->CalcDeltas();
	return retval;
}

void
CAnimBlendNode::CalcDeltas(void)
{
	CAnimBlendSequence *seq = this->sequence;
	if(!(seq->flag & 1))
		return;
	RFrame *frame0 = (RFrame*)GETFRAME(seq, this->frame0);
	RFrame *frame1 = (RFrame*)GETFRAME(seq, this->frame1);
	float norm = frame0->rot.x*frame1->rot.x +
	             frame0->rot.y*frame1->rot.y +
	             frame0->rot.z*frame1->rot.z +
	             frame0->rot.w*frame1->rot.w;
	if(norm > 1.0f)
		norm = 1.0f;
	this->theta0 = M_PI*0.5f - atan2(sqrt(1.0f - norm*norm), norm);
	this->theta1 = this->theta0 != 0.0f ? 1.0f/sin(this->theta0) : 0.0f;
}

void
CAnimBlendNode::GetCurrentTranslation(CVector *vec, float f)
{
	vec->x = vec->y = vec->z = 0.0f;
	float blendAmount = this->blendAssoc->blendAmount;
	if(!(this->blendAssoc->flags & CAnimBlendAssociation::Partial))
		blendAmount *= f;
	if(blendAmount > 0.0f){
		RTFrame *frame0 = (RTFrame*)GETFRAME(this->sequence, this->frame0);
		RTFrame *frame1 = (RTFrame*)GETFRAME(this->sequence, this->frame1);
		float r = frame0->time == 0.0f ? 0.0f : (frame0->time - this->time) / frame0->time;
		if(this->sequence->flag & 2){
			vec->x = blendAmount*(r*frame0->pos.x + (1.0f-r)*frame1->pos.x);
			vec->y = blendAmount*(r*frame0->pos.y + (1.0f-r)*frame1->pos.y);
			vec->z = blendAmount*(r*frame0->pos.z + (1.0f-r)*frame1->pos.z);
		}
	}
}

void
CAnimBlendNode::GetEndTranslation(CVector *vec, float f)
{
	vec->x = vec->y = vec->z = 0.0f;
	float blendAmount = this->blendAssoc->blendAmount;
	if(!(this->blendAssoc->flags & CAnimBlendAssociation::Partial))
		blendAmount *= f;
	if(blendAmount > 0.0f){
		RTFrame *last = (RTFrame*)GETFRAME(this->sequence, this->sequence->numFrames-1);
		if(this->sequence->flag & 2){
			vec->x = last->pos.x*blendAmount;
			vec->y = last->pos.y*blendAmount;
			vec->z = last->pos.z*blendAmount;
		}
	}
}

// TODO: rewrite the ugly loop
void
CAnimBlendNode::FindKeyFrame(float time)
{
	if(this->sequence->numFrames < 1)
		return;
	this->frame1 = this->frame0 = 0;
	if(this->sequence->numFrames == 1){
		this->time = 0.0f;
		this->CalcDeltas();
		return;
	}
	float t = time;
	while(1){
		this->frame0++;
		RFrame *frame = (RFrame*)GETFRAME(this->sequence, this->frame0);
		if(t <= frame->time)
			break;
		t -= frame->time;
		if(this->frame0 + 1 >= this->sequence->numFrames){
			if(!(this->blendAssoc->flags & 2)){
				this->CalcDeltas();
				this->time = 0.0f;
				return;
			}
			this->frame0 = 0;
		}
		this->frame1 = this->frame0;
	}
	this->time = ((RFrame*)GETFRAME(this->sequence, this->frame0))->time - t;
	this->CalcDeltas();
}
