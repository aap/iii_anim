#include "iii_anim.h"

RwObject*
GetFirstObjectCallback(RwObject *object, void *data)
{
	*(RwObject**)data = object;
	return NULL;
}

RwObject*
GetFirstObject(RwFrame *frame)
{
	RwObject *object;
	object = NULL;
	RwFrameForAllObjects(frame, GetFirstObjectCallback, &object);
	return object;
}

WRAPPER int CPools__GetObjectRef(CObject *object) { EAXJMP(0x4A1B00); }

CCutsceneObject::ObjectExt CCutsceneObject::objectExt[450];

//
// CCutsceneObject
//

WRAPPER void CCutsceneObject::ctor_orig(void) { EAXJMP(0x4BA910); }
WRAPPER void CCutsceneObject::ProcessControl_orig(void) { EAXJMP(0x4BA9C0); }

CCutsceneObject*
CCutsceneObject::ctor(void)
{
	this->ctor_orig();
	ObjectExt *ext = this->getExt();
	ext->renderHead = true;
	ext->renderRightHand = true;
	ext->renderLeftHand = true;
	return this;
}

void
CCutsceneObject::ProcessControl(void)
{
	this->ProcessControl_orig();
	if(IsClumpSkinned(this->clump))
		this->UpdateRpHAnim();
}

void
CCutsceneObject::Render(void)
{
	if(IsClumpSkinned(this->clump)){
		if(this->GetRenderLeftHand())
			this->RenderLimb(BONE_SLhand);
		if(this->GetRenderRightHand())
			this->RenderLimb(BONE_SRhand);
		if(this->GetRenderHead())
			this->RenderLimb(BONE_Shead);
	}
	((CObject*)this)->Render();
}

void
CCutsceneObject::RenderLimb(int id)
{
	RpAtomic *atomic;
	CPedModelInfo *pedinfo = (CPedModelInfo*)CModelInfo::ms_modelInfoPtrs[this->nModelIndex];
	switch(id){
	case BONE_Shead:
		atomic = pedinfo->head;
		break;
	case BONE_SLhand:
		atomic = pedinfo->lhand;
		break;
	case BONE_SRhand:
		atomic = pedinfo->rhand;
		break;
	default:
		return;
	}
	if(atomic == NULL)
		return;
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(this->clump);
	int idx = RpHAnimIDGetIndex(hier, id);
	RwMatrix *mat = &RpHAnimHierarchyGetMatrixArray(hier)[idx];
	RwFrame *frame = RpAtomicGetFrame(atomic);
	frame->modelling = *mat;
	RwFrameUpdateObjects(frame);
	atomic->renderCallBack(atomic);
}

//
// CCutsceneHead
//

CCutsceneHead*
CCutsceneHead::ctor(CObject *object)
{
	((CCutsceneObject*)this)->ctor();
	this->vtable = (void**)0x5F7C08;
	ObjectExt *ext = this->getExt();
	ext->object = (CCutsceneObject*)object;	// strange....
	if(IsClumpSkinned(object->clump)){
		ext->object->SetRenderHead(false);
		ext->isSkinned = 1;
	}else{
		this->head = RpAnimBlendClumpFindFrame(object->clump, "Shead")->frame;
		RwObject *obj = GetFirstObject(this->head);
		if(obj)
			obj->flags &= ~rpATOMICRENDER;
		ext->isSkinned = 0;
	}
	return this;
}

float &CTimer__ms_fTimeStepNonClipped = *(float*)0x8E2C4C;

void
CCutsceneHead::ProcessControl(void)
{
	RpHAnimHierarchy *hier;
	CMatrix mat1, mat2;
	ObjectExt *ext = this->getExt();
	if(ext->isSkinned){
		ext->object->UpdateRpHAnim();
		ext->object->UpdateRwFrame();
		hier = GetAnimHierarchyFromSkinClump(ext->object->clump);
		int idx = RpHAnimIDGetIndex(hier, BONE_Shead);
		RwMatrix *ltm = &RpHAnimHierarchyGetMatrixArray(hier)[idx];
		if(RwV3dLength(&ltm->pos) > 100.0f){
			this->matrix.SetRotateY(M_PI/2);
			mat1.ctor(ltm, false);
			CMatrix::mult(&mat2, &mat1, &this->matrix);
			this->matrix.assign(&mat2);
			mat2.dtor();
			mat1.dtor();
		}
	}else{
		this->matrix.SetRotateY(M_PI/2);
		mat1.ctor(RwFrameGetLTM(this->head), false);
		CMatrix::mult(&mat2, &mat1, &this->matrix);
		this->matrix.assign(&mat2);
		mat2.dtor();
		mat1.dtor();
	}
	((CPhysical*)this)->ProcessControl();
	RpAtomic *atm = GetFirstAtomic(this->clump);
	hier = RpSkinAtomicGetHAnimHierarchy(atm);
	RpHAnimHierarchyAddAnimTime(hier, CTimer__ms_fTimeStepNonClipped * 0.02f);
}

void
CCutsceneHead::Render(void)
{
	RpHAnimHierarchy *hier;
	CMatrix mat1, mat2;
	ObjectExt *ext = this->getExt();
	if(ext->isSkinned){
		hier = GetAnimHierarchyFromSkinClump(ext->object->clump);
		RpHAnimHierarchyUpdateMatrices(hier);
		int idx = RpHAnimIDGetIndex(hier, BONE_Shead);
		RwMatrix *ltm = &RpHAnimHierarchyGetMatrixArray(hier)[idx];
		if(RwV3dLength(&ltm->pos) > 100.0f){
			this->matrix.SetRotateY(M_PI/2);
			mat1.ctor(ltm, false);
			CMatrix::mult(&mat2, &mat1, &this->matrix);
			this->matrix.assign(&mat2);
			mat2.dtor();
			mat1.dtor();
		}
	}else{
		this->matrix.SetRotateY(M_PI/2);
		mat1.ctor(RwFrameGetLTM(this->head), false);
		CMatrix::mult(&mat2, &mat1, &this->matrix);
		this->matrix.assign(&mat2);
		mat2.dtor();
		mat1.dtor();
	}
	this->UpdateRwFrame();
	RpAtomic *atm = GetFirstAtomic(this->clump);
	hier = RpSkinAtomicGetHAnimHierarchy(atm);
	RpHAnimHierarchyUpdateMatrices(hier);
	((CObject*)this)->Render();
}

void
objecthooks(void)
{
	InjectHook(0x404CA6, &CCutsceneObject::ctor);
	InjectHook(0x4BA5EC, &CCutsceneObject::ctor);
	Patch(0x5F7CA0, &CCutsceneObject::ProcessControl);
	Patch(0x5F7CB4, &CCutsceneObject::Render);

	InjectHook(0x404CE8, &CCutsceneHead::ctor);
	Patch(0x5F7C28, &CCutsceneHead::ProcessControl);
	Patch(0x5F7C3C, &CCutsceneHead::Render);
}
