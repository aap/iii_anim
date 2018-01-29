#include "iii_anim.h"

RwFreeList *&RpHAnimAnimationFreeList = *(RwFreeList**)0x9405B0;
RpHAnimInterpolatorInfo *RpHAnimInterpolatorInfoBlock = (RpHAnimInterpolatorInfo*)0x661608;
RwInt32 &RpHAnimInterpolatorInfoBlockNumEntries = *(RwInt32*)0x661888;

RpHAnimInterpolatorInfo*
RpHAnimGetInterpolatorInfo(RwInt32 typeID)
{
	for(RwInt32 i = 0; i < RpHAnimInterpolatorInfoBlockNumEntries; i++)
		if(RpHAnimInterpolatorInfoBlock[i].typeID == typeID)
			return &RpHAnimInterpolatorInfoBlock[i];
	return NULL;
}

RpHAnimAnimation*
RpHAnimAnimationCreate(RwInt32 typeID, RwInt32 numFrames, RwInt32 flags, RwReal duration)
{
	RpHAnimInterpolatorInfo *info = RpHAnimGetInterpolatorInfo(typeID);
	if(info == NULL)
		return NULL;
	RpHAnimAnimation *anim = (RpHAnimAnimation*)RwFreeListAlloc(RpHAnimAnimationFreeList);
	anim->duration = duration;
	anim->flags = flags;
	anim->interpInfo = info;
	anim->numFrames = numFrames;
	anim->pFrames = RwMalloc(numFrames*info->keyFrameSize);
	return anim;
}

RpHAnimAnimation*
RpHAnimAnimationDestroy(RpHAnimAnimation *animation)
{
	if(animation->pFrames){
		RwFree(animation->pFrames);
		animation->pFrames = NULL;
	}
	RwFreeListFree(RpHAnimAnimationFreeList, animation);
	return animation;
}

RwMatrix*
RpHAnimHierarchyGetMatrixArray(RpHAnimHierarchy *hierarchy)
{
	return hierarchy->pMatrixArray;
}

RwBool
RpHAnimHierarchySetCurrentAnimTime(RpHAnimHierarchy *hierarchy, RwReal time)
{
/*
	RwReal t = time - hierarchy->currentTime;
	if ( t < 0.0f )
		RpHAnimHierarchySubAnimTime(hierarchy, -t);
	else
		RpHAnimHierarchyAddAnimTime(hierarchy, t);
*/
	RwReal t = time - hierarchy->currentTime;

	if ( t < 0.0f )
		RpHAnimHierarchySubAnimTime(hierarchy, -t);
	else
	{
		if ( hierarchy->pNextFrame )
			RpHAnimHierarchyAddAnimTime(hierarchy, t);
		else
		{
			RpHAnimHierarchySetCurrentAnim(hierarchy, hierarchy->pCurrentAnim);
			RpHAnimHierarchyAddAnimTime(hierarchy, time);
		}
	}

	return TRUE;
}

RwInt32
RpHAnimIDGetIndex(RpHAnimHierarchy *hierarchy, RwInt32 ID)
{
	for(RwInt32 i = 0; i < hierarchy->numNodes; i++)
		if(hierarchy->pNodeInfo[i].nodeID == ID)
			return i;
	return -1;
}

RpHAnimAtomicGlobalVars &RpHAnimAtomicGlobals_ = *(RpHAnimAtomicGlobalVars*)0x9405A8;

RwInt32
RpHAnimFrameGetID(RwFrame *frame)
{
	return *RWPLUGINOFFSET(RwInt32, frame, RpHAnimAtomicGlobals_.engineOffset);
}

RwBool
RpHAnimFrameSetID(RwFrame *frame, RwInt32 id)
{
	*RWPLUGINOFFSET(RwInt32, frame, RpHAnimAtomicGlobals_.engineOffset) = id;
	return 1;
}

const RwMatrixWeights*
RpSkinGetVertexBoneWeights(RpSkin *skin)
{
	return skin->weights;
}

const RwMatrix*
RpSkinGetSkinToBoneMatrices(RpSkin *skin)
{
	return skin->inverseMatrices;
}

RwUInt32
RpSkinGetNumBones(RpSkin *skin)
{
	return skin->numBones;
}

//RwReal
//RwV3dLength(const RwV3d *in)
//{
//	return sqrt(in->x*in->x + in->y*in->y + in->z*in->z);
//}

RwReal
RwV3dNormalize(RwV3d *out, const RwV3d *in)
{
	float len = in->x*in->x + in->y*in->y + in->z*in->z;
	if(len == 0.0f)
		return len;
	float r = 1/sqrt(len);
	RwV3dScale(out, in, r);
	return len;
}

RtQuat*
QuatRotate(RtQuat *out, const RwV3d *axis, float angle)
{
	angle *= M_PI_2/180.0f;
	out->real = cos(angle);
	RwV3dNormalize(&out->imag, axis);
	RwV3dScale(&out->imag, &out->imag, sin(angle));
	return out;
}

RtQuat*
RtQuatRotate(RtQuat *quat, const RwV3d *axis, RwReal angle, RwOpCombineType combineOp)
{
	RtQuat p, q;
	if(quat && axis){
		if(combineOp == rwCOMBINEREPLACE)
			return QuatRotate(quat, axis, angle);
		else if(combineOp == rwCOMBINEPRECONCAT){
			RtQuatAssign(&p, quat);
			QuatRotate(&q, axis, angle);
			RtQuatMultiply(quat, &p, &q);
			return quat;
		}else if(combineOp == rwCOMBINEPOSTCONCAT){
			RtQuatAssign(&p, quat);
			QuatRotate(&q, axis, angle);
			RtQuatMultiply(quat, &q, &p);
			return quat;
		}
		return NULL;
	}
	return NULL;
}
