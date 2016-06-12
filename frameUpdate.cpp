#include "iii_anim.h"

void
FrameUpdateCallBackWith3dVelocityExtraction(AnimBlendFrameData *frame, CAnimBlendNode **nodes)
{
	CVector vec, pos;
	CQuaternion q, rot;
	float totalBlendAmount = 0.0f;
	float x = 0.0f, y = 0.0f, z = 0.0f;
	float curx = 0.0f, cury = 0.0f, curz = 0.0f;
	float endx = 0.0f, endy = 0.0f, endz = 0.0f;
	rot.x = rot.y = rot.z = rot.w = 0.0f;
	pos.x = pos.y = pos.z = 0.0f;
	bool looped = false;
	RwMatrix *mat = &frame->frame->modelling;
	CAnimBlendNode **node;

	if(*nodes){
		node = nodes+1;
		do{
			if((*node)->sequence && ((*node)->blendAssoc->flags & CAnimBlendAssociation::Partial))
				totalBlendAmount += (*node)->blendAssoc->blendAmount;
			node++;
		}while(*node);
	}

	node = nodes+1;
	do{
		if((*node)->sequence && (*node)->sequence->flag & 2 && (*node)->blendAssoc->flags & 0x40){
			(*node)->GetCurrentTranslation(&vec, 1.0f-totalBlendAmount);
			curx += vec.x;
			cury += vec.y;
			curz += vec.z;
		}
		node++;
	}while(*node);

	node = nodes+1;
	do{
		if((*node)->sequence){
			bool nodelooped = (*node)->Update(vec, q, 1.0f-totalBlendAmount);
			rot.Add(q);
			if((*node)->sequence->flag & 2){
				pos.Add(vec);
				if((*node)->blendAssoc->flags & 0x40){
					x += vec.x;
					y += vec.y;
					z += vec.z;
					looped |= nodelooped;
					if(nodelooped){
						(*node)->GetEndTranslation(&vec, 1.0f-totalBlendAmount);
						endx += vec.x;
						endy += vec.y;
						endz += vec.z;
					}
				}
			}
		}
		++*node;
		node++;
	}while(*node);

	if(!(frame->flag & 2)){
		mat->at.z = mat->up.y = mat->right.x = 1.0f;
		mat->up.x = mat->right.z = mat->right.y = 0.0f;
		mat->at.y = mat->at.x = mat->up.z = 0.0f;
		mat->pos.z = mat->pos.y = mat->pos.x = 0.0f;
		mat->flags |= 0x20003;

		float norm = rot.x*rot.x + rot.y*rot.y + rot.z*rot.z + rot.w*rot.w;
		if(norm == 0.0f)
			rot.w = 1.0f;
		else
			rot.Mult(1.0f/sqrt(norm));
		rot.Get(mat);
	}

	if(!(frame->flag & 4)){
		gpAnimBlendClump->pedPosition->x = x - curx;
		gpAnimBlendClump->pedPosition->y = y - cury;
		gpAnimBlendClump->pedPosition->z = z - curz;
		if(looped){
			gpAnimBlendClump->pedPosition->x += endx;
			gpAnimBlendClump->pedPosition->y += endy;
			gpAnimBlendClump->pedPosition->z += endz;
		}
		mat->pos.x = pos.x - x + frame->pos.x;
		mat->pos.y = pos.y - y + frame->pos.y;
		mat->pos.z = pos.z - z + frame->pos.z;
	}
	RwMatrixUpdate(mat);
}

void
FrameUpdateCallBackWithVelocityExtraction(AnimBlendFrameData *frame, CAnimBlendNode **nodes)
{
	CVector vec, pos;
	CQuaternion q, rot;
	float totalBlendAmount = 0.0f;
	float x = 0.0f, y = 0.0f;
	float curx = 0.0f, cury = 0.0f;
	float endx = 0.0f, endy = 0.0f;
	rot.x = rot.y = rot.z = rot.w = 0.0f;
	pos.x = pos.y = pos.z = 0.0f;
	bool looped = false;
	RwMatrix *mat = &frame->frame->modelling;
	CAnimBlendNode **node;

	if(*nodes)
		for(node = nodes+1; *node; node++)
			if((*node)->sequence && ((*node)->blendAssoc->flags & CAnimBlendAssociation::Partial))
				totalBlendAmount += (*node)->blendAssoc->blendAmount;

	for(node = nodes+1; *node; node++)
		if((*node)->sequence && (*node)->sequence->flag & 2 && (*node)->blendAssoc->flags & 0x40){
			(*node)->GetCurrentTranslation(&vec, 1.0f-totalBlendAmount);
			cury += vec.y;
			if((*node)->blendAssoc->flags & 0x1000)
				curx += vec.x;
		}

	for(node = nodes+1; *node; node++){
		if((*node)->sequence){
			bool nodelooped = (*node)->Update(vec, q, 1.0f-totalBlendAmount);
			rot.Add(q);
			if((*node)->sequence->flag & 2){
				pos.Add(vec);
				if((*node)->blendAssoc->flags & 0x40){
					y += vec.y;
					if((*node)->blendAssoc->flags & 0x1000)
						x += vec.x;
					looped |= nodelooped;
					if(nodelooped){
						(*node)->GetEndTranslation(&vec, 1.0f-totalBlendAmount);
						endy += vec.y;
						if((*node)->blendAssoc->flags & 0x1000)
							endx += vec.x;
					}
				}
			}
		}
		++*node;
	}

	if(!(frame->flag & 2)){
		mat->at.z = mat->up.y = mat->right.x = 1.0f;
		mat->up.x = mat->right.z = mat->right.y = 0.0f;
		mat->at.y = mat->at.x = mat->up.z = 0.0f;
		mat->pos.z = mat->pos.y = mat->pos.x = 0.0f;
		mat->flags |= 0x20003;

		float norm = rot.x*rot.x + rot.y*rot.y + rot.z*rot.z + rot.w*rot.w;
		if(norm == 0.0f)
			rot.w = 1.0f;
		else
			rot.Mult(1.0f/sqrt(norm));
		rot.Get(mat);
	}

	if(!(frame->flag & 4)){
		gpAnimBlendClump->pedPosition->x = x - curx;
		gpAnimBlendClump->pedPosition->y = y - cury;
		if(looped){
			gpAnimBlendClump->pedPosition->x += endx;
			gpAnimBlendClump->pedPosition->y += endy;
		}
		mat->pos.x = pos.x - x;
		mat->pos.y = pos.y - y;
		mat->pos.z = pos.z;
		if(mat->pos.z >= -0.8f)
			if(mat->pos.z >= -0.4f)
				mat->pos.z += frame->pos.z;
			else
				mat->pos.z += (2.5f * mat->pos.z + 2.0f) * frame->pos.z;
		mat->pos.x += frame->pos.x;
		mat->pos.y += frame->pos.y;
	}
	RwMatrixUpdate(mat);
}

void
FrameUpdateCallBack(AnimBlendFrameData *frame, void *arg)
{
	CAnimBlendNode **nodes = (CAnimBlendNode**)arg;
	CVector vec, pos;
	CQuaternion q, rot;
	float totalBlendAmount = 0.0f;
	rot.x = rot.y = rot.z = rot.w = 0.0f;
	pos.x = pos.y = pos.z = 0.0f;
	CAnimBlendNode **node;
	RwMatrix *mat = &frame->frame->modelling;

	if (frame->flag & 8 && gpAnimBlendClump->pedPosition){
		if(frame->flag & 0x10)
			FrameUpdateCallBackWith3dVelocityExtraction(frame, nodes);
		else
			FrameUpdateCallBackWithVelocityExtraction(frame, nodes);
		return;
	}

	if(*nodes)
		for(node = nodes+1; *node; node++)
			if((*node)->sequence && ((*node)->blendAssoc->flags & CAnimBlendAssociation::Partial))
				totalBlendAmount += (*node)->blendAssoc->blendAmount;

	for(node = nodes+1; *node; node++){
		if((*node)->sequence){
			(*node)->Update(vec, q, 1.0f-totalBlendAmount);
			rot.Add(q);
			if((*node)->sequence->flag & 2)
				pos.Add(vec);
		}
		++*node;
	}

	if(!(frame->flag & 2)){
		mat->at.z = mat->up.y = mat->right.x = 1.0f;
		mat->up.x = mat->right.z = mat->right.y = 0.0f;
		mat->at.y = mat->at.x = mat->up.z = 0.0f;
		mat->pos.z = mat->pos.y = mat->pos.x = 0.0f;
		mat->flags |= 0x20003;

		float norm = rot.x*rot.x + rot.y*rot.y + rot.z*rot.z + rot.w*rot.w;
		if(norm == 0.0f)
			rot.w = 1.0f;
		else
			rot.Mult(1.0f/sqrt(norm));
		rot.Get(mat);
	}

	if(!(frame->flag & 4)){
		mat->pos.x = pos.x + frame->pos.x;
		mat->pos.y = pos.y + frame->pos.y;
		mat->pos.z = pos.z + frame->pos.z;
	}
	RwMatrixUpdate(mat);
}


void
FrameUpdateCallBackSkinnedWith3dVelocityExtraction(AnimBlendFrameData *frame, CAnimBlendNode **nodes)
{
	CVector vec, pos;
	CQuaternion q, rot;
	float totalBlendAmount = 0.0f;
	float x = 0.0f, y = 0.0f, z = 0.0f;
	float curx = 0.0f, cury = 0.0f, curz = 0.0f;
	float endx = 0.0f, endy = 0.0f, endz = 0.0f;
	rot.x = rot.y = rot.z = rot.w = 0.0f;
	pos.x = pos.y = pos.z = 0.0f;
	bool looped = false;
	RpHAnimStdKeyFrame *frameData = frame->hanimframe;
	CAnimBlendNode **node;

	if(*nodes){
		node = nodes+1;
		do{
			if((*node)->sequence && ((*node)->blendAssoc->flags & CAnimBlendAssociation::Partial))
				totalBlendAmount += (*node)->blendAssoc->blendAmount;
			node++;
		}while(*node);
	}

	node = nodes+1;
	do{
		if((*node)->sequence && (*node)->sequence->flag & 2 && (*node)->blendAssoc->flags & 0x40){
			(*node)->GetCurrentTranslation(&vec, 1.0f-totalBlendAmount);
			curx += vec.x;
			cury += vec.y;
			curz += vec.z;
		}
		node++;
	}while(*node);

	node = nodes+1;
	do{
		if((*node)->sequence){
			bool nodelooped = (*node)->Update(vec, q, 1.0f-totalBlendAmount);
			if(q.x*rot.x + q.y*rot.y + q.z*rot.z + q.w*rot.w < 0.0f)
				rot.Sub(q);
			else
				rot.Add(q);
			if((*node)->sequence->flag & 2){
				pos.Add(vec);
				if((*node)->blendAssoc->flags & 0x40){
					x += vec.x;
					y += vec.y;
					z += vec.z;
					looped |= nodelooped;
					if(nodelooped){
						(*node)->GetEndTranslation(&vec, 1.0f-totalBlendAmount);
						endx += vec.x;
						endy += vec.y;
						endz += vec.z;
					}
				}
			}
		}
		++*node;
		node++;
	}while(*node);

	if(!(frame->flag & 2)){
		float norm = rot.x*rot.x + rot.y*rot.y + rot.z*rot.z + rot.w*rot.w;
		if(norm == 0.0f)
			rot.w = 1.0f;
		else
			rot.Mult(1.0f/sqrt(norm));
		frameData->q.imag.x = rot.x;
		frameData->q.imag.y = rot.y;
		frameData->q.imag.z = rot.z;
		frameData->q.real = rot.w;
	}

	if(!(frame->flag & 4)){
		gpAnimBlendClump->pedPosition->x = x - curx;
		gpAnimBlendClump->pedPosition->y = y - cury;
		gpAnimBlendClump->pedPosition->z = z - curz;
		if(looped){
			gpAnimBlendClump->pedPosition->x += endx;
			gpAnimBlendClump->pedPosition->y += endy;
			gpAnimBlendClump->pedPosition->z += endz;
		}
		frameData->t.x = pos.x - x + frame->pos.x;
		frameData->t.y = pos.y - y + frame->pos.y;
		frameData->t.z = pos.z - z + frame->pos.z;
	}	
}

void
FrameUpdateCallBackSkinnedWithVelocityExtraction(AnimBlendFrameData *frame, CAnimBlendNode **nodes)
{
	CVector vec, pos;
	CQuaternion q, rot;
	float totalBlendAmount = 0.0f;
	float x = 0.0f, y = 0.0f;
	float curx = 0.0f, cury = 0.0f;
	float endx = 0.0f, endy = 0.0f;
	rot.x = rot.y = rot.z = rot.w = 0.0f;
	pos.x = pos.y = pos.z = 0.0f;
	bool looped = false;
	RpHAnimStdKeyFrame *frameData = frame->hanimframe;
	CAnimBlendNode **node;

	if(*nodes){
		node = nodes+1;
		do{
			if((*node)->sequence && ((*node)->blendAssoc->flags & CAnimBlendAssociation::Partial))
				totalBlendAmount += (*node)->blendAssoc->blendAmount;
			node++;
		}while(*node);
	}

	node = nodes+1;
	do{
		if((*node)->sequence && (*node)->sequence->flag & 2 && (*node)->blendAssoc->flags & 0x40){
			(*node)->GetCurrentTranslation(&vec, 1.0f-totalBlendAmount);
			cury += vec.y;
			if((*node)->blendAssoc->flags & 0x1000)
				curx += vec.x;
		}
		node++;
	}while(*node);

	node = nodes+1;
	do{
		if((*node)->sequence){
			bool nodelooped = (*node)->Update(vec, q, 1.0f-totalBlendAmount);
			if(q.x*rot.x + q.y*rot.y + q.z*rot.z + q.w*rot.w < 0.0f)
				rot.Sub(q);
			else
				rot.Add(q);
			if((*node)->sequence->flag & 2){
				pos.Add(vec);
				if((*node)->blendAssoc->flags & 0x40){
					y += vec.y;
					if((*node)->blendAssoc->flags & 0x1000)
						x += vec.x;
					looped |= nodelooped;
					if(nodelooped){
						(*node)->GetEndTranslation(&vec, 1.0f-totalBlendAmount);
						endy += vec.y;
						if((*node)->blendAssoc->flags & 0x1000)
							endx += vec.x;
					}
				}
			}
		}
		++*node;
		node++;
	}while(*node);

	if(!(frame->flag & 2)){
		float norm = rot.x*rot.x + rot.y*rot.y + rot.z*rot.z + rot.w*rot.w;
		if(norm == 0.0f)
			rot.w = 1.0f;
		else
			rot.Mult(1.0f/sqrt(norm));
		frameData->q.imag.x = rot.x;
		frameData->q.imag.y = rot.y;
		frameData->q.imag.z = rot.z;
		frameData->q.real = rot.w;
	}

	if(!(frame->flag & 4)){
		gpAnimBlendClump->pedPosition->x = x - curx;
		gpAnimBlendClump->pedPosition->y = y - cury;
		if(looped){
			gpAnimBlendClump->pedPosition->x += endx;
			gpAnimBlendClump->pedPosition->y += endy;
		}
		frameData->t.x = pos.x - x;
		frameData->t.y = pos.y - y;
		frameData->t.z = pos.z;
		if(frameData->t.z >= -0.8f)
			if(frameData->t.z >= -0.4f)
				frameData->t.z += frame->pos.z;
			else
				frameData->t.z += (2.5f * frameData->t.z + 2.0f) * frame->pos.z;
		frameData->t.x += frame->pos.x;
		frameData->t.y += frame->pos.y;
	}	
}

void
FrameUpdateCallBackSkinned(AnimBlendFrameData *frame, void *arg)
{
	CAnimBlendNode **nodes = (CAnimBlendNode**)arg;
	CVector vec, pos;
	CQuaternion q, rot;
	float totalBlendAmount = 0.0f, posBlendAmount = 0.0f;
	rot.x = rot.y = rot.z = rot.w = 0.0f;
	pos.x = pos.y = pos.z = 0.0f;
	RpHAnimStdKeyFrame *frameData = frame->hanimframe;
	CAnimBlendNode **node;

	if (frame->flag & 8 && gpAnimBlendClump->pedPosition){
		if(frame->flag & 0x10)
			FrameUpdateCallBackSkinnedWith3dVelocityExtraction(frame, nodes);
		else
			FrameUpdateCallBackSkinnedWithVelocityExtraction(frame, nodes);
		return;
	}

	if(*nodes){
		node = nodes+1;
		do{
			if((*node)->sequence && ((*node)->blendAssoc->flags & CAnimBlendAssociation::Partial))
				totalBlendAmount += (*node)->blendAssoc->blendAmount;
			node++;
		}while(*node);
	}

	node = nodes+1;
	do{
		if((*node)->sequence){
			bool nodelooped = (*node)->Update(vec, q, 1.0f-totalBlendAmount);
			if(q.x*rot.x + q.y*rot.y + q.z*rot.z + q.w*rot.w < 0.0f)
				rot.Sub(q);
			else
				rot.Add(q);
			if((*node)->sequence->flag & 2){
				pos.Add(vec);
				posBlendAmount += (*node)->blendAssoc->blendAmount;
			}
		}
		++*node;
		node++;
	}while(*node);

	if(!(frame->flag & 2)){
		float norm = rot.x*rot.x + rot.y*rot.y + rot.z*rot.z + rot.w*rot.w;
		if(norm == 0.0f)
			rot.w = 1.0f;
		else
			rot.Mult(1.0f/sqrt(norm));
		frameData->q.imag.x = rot.x;
		frameData->q.imag.y = rot.y;
		frameData->q.imag.z = rot.z;
		frameData->q.real = rot.w;
	}

	if(!(frame->flag & 4)){
		frameData->t.x = posBlendAmount * pos.x + frame->pos.x * (1.0f - posBlendAmount);
		frameData->t.y = posBlendAmount * pos.y + frame->pos.y * (1.0f - posBlendAmount);
		frameData->t.z = posBlendAmount * pos.z + frame->pos.z * (1.0f - posBlendAmount);
	}
}