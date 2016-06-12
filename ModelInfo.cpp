#include "iii_anim.h"

WRAPPER void CVisibilityPlugins__SetClumpModelInfo(RpClump *clump, int clumpModelInfo) { EAXJMP(0x528ED0); }
WRAPPER void __fastcall CBaseModelInfo__AddTexDictionaryRef(int self) { EAXJMP(0x4F6B80); }
WRAPPER void CClumpModelInfo::SetFrameIds(int ids) { EAXJMP(0x4F8BB0); }
//WRAPPER void __fastcall CPedModelInfo__CreateHitColModel(int self) { EAXJMP(0x5104D0); }
WRAPPER void __fastcall CPedModelInfo__DeleteRwObject_orig(CPedModelInfo*) { EAXJMP(0x510280); }

static RpAtomic*
SetHierarchyForSkinAtomic(RpAtomic *atomic, void *data)
{
	RpSkinAtomicSetHAnimHierarchy(atomic, (RpHAnimHierarchy*)data);
	return NULL;
}

RpHAnimAnimation*
HAnimAnimationCreateForHierarchy(RpHAnimHierarchy *hier)
{
	RpHAnimAnimation *anim = RpHAnimAnimationCreate(rpHANIMSTDKEYFRAMETYPEID, hier->numNodes, 0, 0.0f);
	if(anim == NULL)
		return NULL;
	RpHAnimStdKeyFrame *frame = (RpHAnimStdKeyFrame*)anim->pFrames;
	for(int i = 0; i < hier->numNodes; i++){
		frame->q.real = 1.0f;
		frame->q.imag.x = frame->q.imag.y = frame->q.imag.z = 0.0f;
		frame->t.x = frame->t.y = frame->t.z = 0.0f;
		frame->time = 0.0f;
		frame->prevFrame = NULL;
		frame++;
	}
	return anim;
}

RpClump*
CClumpModelInfo::CreateInstance(void)
{
	RpClump *clone, *clone2;
	RpClump *clump = this->clump;
	if(clump == NULL)
		return NULL;

	clone2 = RpClumpClone(clump);
	clone = RpClumpClone(clone2);	// to reverse order of... something again...
	RpClumpDestroy(clone2);
	//clone = clone2;

//	RpClump *clone = RpClumpClone(clump);
	if(IsClumpSkinned(clone)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromClump(clone);
		RpClumpForAllAtomics(clone, SetHierarchyForSkinAtomic, hier);
		RpHAnimAnimation *anim = HAnimAnimationCreateForHierarchy(hier);
		RpHAnimHierarchySetCurrentAnim(hier, anim);
		RpHAnimHierarchySetFlags(hier, rpHANIMHIERARCHYUPDATEMODELLINGMATRICES|rpHANIMHIERARCHYUPDATELTMS);
/*	xbox:
		v6 = RpSkinGeometryGetSkin(v5->geometry);
		RpSkinGetNumBones(v6);
		RpHAnimHierarchyUpdateMatrices(v3);
*/
	}
	return clone;	
}

void
CClumpModelInfo::SetClump(RpClump *clump)
{
	this->clump = clump;
	CVisibilityPlugins__SetClumpModelInfo(clump, (int)this);
	CBaseModelInfo__AddTexDictionaryRef((int)this);
	RpClumpForAllAtomics(clump, (RpAtomicCallBack)0x4F8940, 0);	// CClumpModelInfo::SetAtomicRendererCB
	// Xbox sets world pipeline depending on *(this+42) here
	if(strcmp(this->name, "playerh") == 0)
		RpClumpForAllAtomics(clump, (RpAtomicCallBack)0x4F8940, (void*)0x528B30);	// CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins::RenderPlayerCB

	if(IsClumpSkinned(clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromClump(clump);
		// mobile
		// RpClumpForAllAtomics(clump, SetHierarchyForSkinAtomic, hier);
		// RpAtomic *atomic = GetFirstAtomic(clump);
		// Xbox
		RpAtomic *atomic = IsClumpSkinned(clump);
		RpSkinAtomicSetHAnimHierarchy(atomic, hier);

		RpSkin *skin = RpSkinGeometryGetSkin(atomic->geometry);
		// ignore const, lol
		RwMatrixWeights *weights = (RwMatrixWeights*)RpSkinGetVertexBoneWeights(skin);
		for(int i = 0; i < atomic->geometry->numVertices; i++){
			RwReal sum = weights->w0 + weights->w1 + weights->w2 + weights->w3;
			weights->w0 /= sum;
			weights->w1 /= sum;
			weights->w2 /= sum;
			weights->w3 /= sum;
			weights++;
		}
		RpHAnimHierarchySetFlags(hier, rpHANIMHIERARCHYUPDATEMODELLINGMATRICES|rpHANIMHIERARCHYUPDATELTMS);
	}
}

struct LimbCBarg {
	CPedModelInfo *pedmodelinfo;
	RpClump *clump;
	RpAtomic *atomics[3];
};

RpAtomic*
CPedModelInfo__findLimbsCb(RpAtomic *atomic, void *data)
{
	LimbCBarg *limbs = (LimbCBarg*)data;
	RwFrame *frame = RpAtomicGetFrame(atomic);
	const char *name = GetFrameNodeName(frame);
	if(lcstrcmp(name, "Shead01") == 0){
		limbs->atomics[0] = atomic;
		limbs->pedmodelinfo->head = atomic;
	}else if(lcstrcmp(name, "SLhand01") == 0){
		limbs->atomics[1] = atomic;
		limbs->pedmodelinfo->lhand = atomic;
	}else if(lcstrcmp(name, "SRhand01") == 0){
		limbs->atomics[2] = atomic;
		limbs->pedmodelinfo->rhand = atomic;
	}else
		return atomic;
	RpClumpRemoveAtomic(limbs->clump, atomic);
	RwFrameRemoveChild(frame);
	return atomic;
}

void
CPedModelInfo::SetClump(RpClump *clump)
{
	int isplayer = strcmp(this->name, "player") == 0;
	RpAtomic *atomic;
	// set renderCB before removing Atomics from Clump
	if(isplayer)
		RpClumpForAllAtomics(clump, (RpAtomicCallBack)0x4F8940, (void*)0x528B30);	// CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins::RenderPlayerCB
	if(atomic = IsClumpSkinned(clump)){
		LimbCBarg limbs = { this, clump, 0, 0, 0 };
		RpClumpForAllAtomics(clump, CPedModelInfo__findLimbsCb, &limbs);
	}
	this->CClumpModelInfo::SetClump(clump);
	this->SetFrameIds(0x5FE7A4);	// CPedModelInfo::m_pPedIds
	if(this->hitColModel == NULL && !IsClumpSkinned(clump))
		this->CreateHitColModel();
		//CPedModelInfo__CreateHitColModel((int)this);
	// again, because CClumpModelInfo::SetClump resets renderCB
	if(isplayer)
		RpClumpForAllAtomics(clump, (RpAtomicCallBack)0x4F8940, (void*)0x528B30);	// CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins::RenderPlayerCB
}

CPedModelInfo::CPedModelInfo(void)
{
	((void(__thiscall*)(CPedModelInfo*))0x50BFA0)(this);
	this->head = this->lhand = this->rhand = NULL;
}

CStore_PedModelInfo CModelInfo::ms_pedModelStore = { 0 };

CPedModelInfo*
CModelInfo::AddPedModel(int id)
{
	if(CModelInfo::ms_pedModelStore.numElements >= 90)
		printf("Size of this thing:%d needs increasing\n", 90);
	CPedModelInfo *modelInfo;
	modelInfo = &CModelInfo::ms_pedModelStore.objects[CModelInfo::ms_pedModelStore.numElements++];
	modelInfo->clump = NULL;
	CModelInfo::ms_modelInfoPtrs[id] = modelInfo;
	return modelInfo;
}

void
CPedModelInfo::DeleteRwObject(void)
{
	RwFrame *frame;
#define DESTROY(a) \
	if(this->a){\
		frame = RpAtomicGetFrame(this->a);\
		RpAtomicDestroy(this->a);\
		RwFrameDestroy(frame);\
		this->a = NULL;\
	}
	DESTROY(head)
	DESTROY(lhand)
	DESTROY(rhand)
#undef	DESTROY

	CPedModelInfo__DeleteRwObject_orig(this);
}

RpAtomic*
AtomicRemoveAnimFromSkinCB(RpAtomic *atomic, void*)
{
	if(RpSkinGeometryGetSkin(RpAtomicGetGeometry(atomic)) == NULL)
		return atomic;
	RpHAnimHierarchy *hier = RpSkinAtomicGetHAnimHierarchy(atomic);
	if(hier->pCurrentAnim){
		RpHAnimAnimationDestroy(hier->pCurrentAnim);
		hier->pCurrentAnim = NULL;
	}
	return atomic;
}

void
DeleteRwObject_hook(RpClump *clump)
{
	RpClumpForAllAtomics(clump, AtomicRemoveAnimFromSkinCB, NULL);
	RpClumpDestroy(clump);
}

struct ColLimb
{
	char *name;
	int id;
	int flag;
	float x;
	float z;
	float radius;
};

ColLimb *ColLimbs = (ColLimb*)0x5FE848;

CColModel*
CPedModelInfo::AnimatePedColModelSkinned(RpClump *clump)
{
	CColModel *colmodel = this->hitColModel;
	if(colmodel == NULL){
		this->CreateHitColModelSkinned(clump);
		return this->hitColModel;
	}
	RwMatrix *m1, *m2;
	CColSphere *spheres = colmodel->spheres;
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(clump);
	m1 = RwMatrixCreate();
	m2 = RwMatrixCreate();
	RwMatrixInvert(m1, &RpClumpGetFrame(clump)->modelling);
	for(int i = 0; i < 8; i++){
		*m2 = *m1;
		int id = ConvertPedNode2BoneTag(ColLimbs[i].id);
		int idx = RpHAnimIDGetIndex(hier, id);
		RwMatrixTransform(m2, &RpHAnimHierarchyGetMatrixArray(hier)[idx], rwCOMBINEPRECONCAT);
		RwV3d pos;
		pos.x = pos.y = pos.z = 0.0f;
		RwV3dTransformPoints(&pos, &pos, 1, m2);
		spheres[i].center.x = pos.x + ColLimbs[i].x;
		spheres[i].center.y = pos.y + 0.0f;
		spheres[i].center.z = pos.z + ColLimbs[i].z;
	}
	RwMatrixDestroy(m1);
	RwMatrixDestroy(m2);
	return colmodel;
}

void
CPedModelInfo::CreateHitColModelSkinned(RpClump *clump)
{
	CVector center;
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(clump);
	CColModel *colmodel = (CColModel*)gta_nw(0x58);
	colmodel->ctor();
	CColSphere *spheres = (CColSphere*)RwMalloc(8*sizeof(CColSphere));
	RwMatrix *m1, *m2;
	m1 = RwMatrixCreate();
	m2 = RwMatrixCreate();
	RwMatrixInvert(m1, &RpClumpGetFrame(clump)->modelling);
	for(int i = 0; i < 8; i++){
		*m2 = *m1;
		int id = ConvertPedNode2BoneTag(ColLimbs[i].id);
		int idx = RpHAnimIDGetIndex(hier, id);
		RwMatrixTransform(m2, &RpHAnimHierarchyGetMatrixArray(hier)[idx], rwCOMBINEPRECONCAT);
		RwV3d pos;
		pos.x = pos.y = pos.z = 0.0f;
		RwV3dTransformPoints(&pos, &pos, 1, m2);
		center.x = pos.x + ColLimbs[i].x;
		center.y = pos.y + 0.0f;
		center.z = pos.z + ColLimbs[i].z;
		spheres[i].Set(ColLimbs[i].radius, &center, 17, ColLimbs[i].flag);
	}
	RwMatrixDestroy(m1);
	RwMatrixDestroy(m2);
	colmodel->spheres = spheres;
	colmodel->numSpheres = 8;
	center.x = center.y = center.z = 0.0f;
	colmodel->boundingSphere.Set(2.0f, &center, 0, 0);
	CVector min, max;
	min.x = min.y = -0.5f;
	min.z = -1.2f;
	max.x = max.y = 0.5f;
	max.z = 1.2f;
	colmodel->boundingBox.Set(&min, &max, 0, 0);
	colmodel->level = 0;
	this->hitColModel = colmodel;
}

void
CPedModelInfo::CreateHitColModel(void)
{
	struct {
		union {
			char *name;
			int id;
		};
		RwFrame *out;
	} search;
	CVector center;
	CColModel *colmodel = (CColModel*)gta_nw(0x58);
	colmodel->ctor();
	CColSphere *spheres = (CColSphere*)RwMalloc(8*sizeof(CColSphere));
	RwFrame *root = RpClumpGetFrame(this->clump);
	RwMatrix *mat = RwMatrixCreate();
	for(int i = 0; i < 8; i++){
		if(ColLimbs[i].name){
			search.name = ColLimbs[i].name;
			search.out = NULL;
			RwFrameForAllChildren(root, (RwFrameCallBack)0x4F8960, &search);
		}else{
			search.id = ColLimbs[i].id;
			search.out = NULL;
			RwFrameForAllChildren(root, (RwFrameCallBack)0x4F8AD0, &search);
		}
		RwFrame *f = search.out;
		if(f){
			float radius = ColLimbs[i].radius;
			if(ColLimbs[i].id == 6)
				RwFrameForAllObjects(root, (RwObjectCallBack)0x5104A0, &radius);
			*mat = f->modelling;
			for(f = RwFrameGetParent(f); f != root; f = RwFrameGetParent(f))
				RwMatrixTransform(mat, &f->modelling, rwCOMBINEPOSTCONCAT);
			center.x = mat->pos.x + ColLimbs[i].x;
			center.y = mat->pos.y + 0.0f;
			center.z = mat->pos.z + ColLimbs[i].z;
			spheres[i].Set(radius, &center, 17, ColLimbs[i].flag);
		}
	}
	RwMatrixDestroy(mat);
	colmodel->spheres = spheres;
	colmodel->numSpheres = 8;
	center.x = center.y = center.z = 0.0f;
	colmodel->boundingSphere.Set(2.0f, &center, 0, 0);
	CVector min, max;
	min.x = min.y = -0.5f;
	min.z = -1.2f;
	max.x = max.y = 0.5f;
	max.z = 1.2f;
	colmodel->boundingBox.Set(&min, &max, 0, 0);
	colmodel->level = 0;
	this->hitColModel = colmodel;
}
