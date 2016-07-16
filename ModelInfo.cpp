#include "iii_anim.h"

WRAPPER void CVisibilityPlugins__SetClumpModelInfo(RpClump *clump, int clumpModelInfo) { EAXJMP(0x528ED0); }
WRAPPER void __fastcall CBaseModelInfo__AddTexDictionaryRef(int self) { EAXJMP(0x4F6B80); }
WRAPPER void CClumpModelInfo::SetFrameIds(int ids) { EAXJMP(0x4F8BB0); }
//WRAPPER void __fastcall CPedModelInfo__CreateHitColModel(int self) { EAXJMP(0x5104D0); }
WRAPPER void __fastcall CPedModelInfo__DeleteRwObject_orig(CPedModelInfo*) { EAXJMP(0x510280); }

WRAPPER RpAtomic *CVisibilityPlugins__RenderPlayerCB(RpAtomic*) { EAXJMP(0x528B30); }

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
	RpClump *clump = this->clump;
	if(clump == NULL)
		return NULL;

//	RpClump *clone, *clone2;
//	clone2 = RpClumpClone(clump);
//	clone = RpClumpClone(clone2);	// to reverse order of frames again...
//	RpClumpDestroy(clone2);
//	//clone = clone2;

	RpClump *clone = RpClumpClone(clump);
	if(IsClumpSkinned(clone)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromClump(clone);
		RpClumpForAllAtomics(clone, SetHierarchyForSkinAtomic, hier);
		RpHAnimAnimation *anim = HAnimAnimationCreateForHierarchy(hier);
		RpHAnimHierarchySetCurrentAnim(hier, anim);
		RpHAnimHierarchySetFlags(hier, rpHANIMHIERARCHYUPDATEMODELLINGMATRICES|rpHANIMHIERARCHYUPDATELTMS);
		// xbox nonsense:
		// v6 = RpSkinGeometryGetSkin(v5->geometry);
		// RpSkinGetNumBones(v6);
		RpHAnimHierarchyUpdateMatrices(hier);
	}
	return clone;	
}

void
CClumpModelInfo::SetClump(RpClump *clump)
{
	this->clump = clump;
	CVisibilityPlugins__SetClumpModelInfo(clump, (int)this);
	CBaseModelInfo__AddTexDictionaryRef((int)this);
	RpClumpForAllAtomics(clump, CClumpModelInfo::SetAtomicRendererCB, NULL);
	if(this->type == 1 || // CSimpleModelInfo
	   this->type == 3 || // CTimeModelInfo
	   this->type == 4){  // CClumpModelInfo
		// set world pipeline??? o_O
	}

	// xbox skin is not compatible with player head!
//	if(strncmp(this->name, "playerh", 8) == 0)
//		RpClumpForAllAtomics(clump, CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins__RenderPlayerCB);

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

WRAPPER RpAtomic *CClumpModelInfo::SetAtomicRendererCB(RpAtomic*, void*) { EAXJMP(0x4F8940); }

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
		RpClumpForAllAtomics(clump, CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins__RenderPlayerCB);
	if(atomic = IsClumpSkinned(clump)){
		LimbCBarg limbs = { this, clump, 0, 0, 0 };
		RpClumpForAllAtomics(clump, CPedModelInfo__findLimbsCb, &limbs);
	}
	this->CClumpModelInfo::SetClump(clump);
	this->SetFrameIds(0x5FE7A4);	// CPedModelInfo::m_pPedIds
	if(this->hitColModel == NULL && !IsClumpSkinned(clump))
		this->CreateHitColModel();
	// again, because CClumpModelInfo::SetClump resets renderCB
	if(isplayer)
		RpClumpForAllAtomics(clump, CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins__RenderPlayerCB);
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

// this is very weird....names and IDs mixed, WHY?
ColLimb m_pColNodeInfos[8] = {
	{ NULL,         PED_Shead,       6,  0.0f,   0.05f, 0.2f },
	{ "Storso",     0,               0,  0.0f,   0.15f, 0.2f },
	{ "Storso",     0,               0,  0.0f,  -0.05f, 0.3f },
	{ NULL,         PED_Storso,      1,  0.0f,  -0.07f, 0.3f },
	{ NULL,         PED_Supperarml,  2,  0.07f, -0.1f,  0.2f },
	{ NULL,         PED_Supperarmr,  3, -0.07f, -0.1f,  0.2f },
	{ "Slowerlegl", 0,               4,  0.0f,   0.07f, 0.25f },
	{ NULL,         PED_Slowerlegr,  5,  0.0f,   0.07f, 0.25f },
};

//ColLimb *ColLimbs = (ColLimb*)0x5FE848;

CColModel*
CPedModelInfo::AnimatePedColModelSkinned(RpClump *clump)
{
	CColModel *colmodel = this->hitColModel;
	if(colmodel == NULL){
		this->CreateHitColModelSkinned(clump);
		return this->hitColModel;
	}
	RwMatrix *invmat, *mat;
	CColSphere *spheres = colmodel->spheres;
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(clump);
	invmat = RwMatrixCreate();
	mat = RwMatrixCreate();
	RwMatrixInvert(invmat, &RpClumpGetFrame(clump)->modelling);
	for(int i = 0; i < 8; i++){
		RwV3d pos;
		pos.x = pos.y = pos.z = 0.0f;
		*mat = *invmat;
		int id = ConvertPedNode2BoneTag(m_pColNodeInfos[i].id);
		int idx = RpHAnimIDGetIndex(hier, id);
		RwMatrixTransform(mat, &RpHAnimHierarchyGetMatrixArray(hier)[idx], rwCOMBINEPRECONCAT);
		RwV3dTransformPoints(&pos, &pos, 1, mat);
		spheres[i].center.x = pos.x + m_pColNodeInfos[i].x;
		spheres[i].center.y = pos.y + 0.0f;
		spheres[i].center.z = pos.z + m_pColNodeInfos[i].z;
	}
	RwMatrixDestroy(invmat);
	RwMatrixDestroy(mat);
	return colmodel;
}

void
CPedModelInfo::CreateHitColModelSkinned(RpClump *clump)
{
	CVector center;
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(clump);
	CColModel *colmodel = (CColModel*)gta_nw(sizeof(CColModel));
	colmodel->ctor();
	CColSphere *spheres = (CColSphere*)RwMalloc(8*sizeof(CColSphere));
	RwMatrix *invmat, *mat;
	invmat = RwMatrixCreate();
	mat = RwMatrixCreate();
	RwMatrixInvert(invmat, &RpClumpGetFrame(clump)->modelling);
	for(int i = 0; i < 8; i++){
		*mat = *invmat;
		int id = ConvertPedNode2BoneTag(m_pColNodeInfos[i].id);	// this is wrong, wtf R* ???
		int idx = RpHAnimIDGetIndex(hier, id);
		RwMatrixTransform(mat, &RpHAnimHierarchyGetMatrixArray(hier)[idx], rwCOMBINEPRECONCAT);
		RwV3d pos;
		pos.x = pos.y = pos.z = 0.0f;
		RwV3dTransformPoints(&pos, &pos, 1, mat);
		center.x = pos.x + m_pColNodeInfos[i].x;
		center.y = pos.y + 0.0f;
		center.z = pos.z + m_pColNodeInfos[i].z;
		spheres[i].Set(m_pColNodeInfos[i].radius, &center, 17, m_pColNodeInfos[i].flag);
	}
	RwMatrixDestroy(invmat);
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
	CColModel *colmodel = (CColModel*)gta_nw(sizeof(CColModel));
	colmodel->ctor();
	CColSphere *spheres = (CColSphere*)RwMalloc(8*sizeof(CColSphere));
	RwFrame *root = RpClumpGetFrame(this->clump);
	RwMatrix *mat = RwMatrixCreate();
	for(int i = 0; i < 8; i++){
		if(m_pColNodeInfos[i].name){
			search.name = m_pColNodeInfos[i].name;
			search.out = NULL;
			RwFrameForAllChildren(root, (RwFrameCallBack)0x4F8960, &search);	// CClumpModelInfo::FindFrameFromNameCB
		}else{
			search.id = m_pColNodeInfos[i].id;
			search.out = NULL;
			RwFrameForAllChildren(root, (RwFrameCallBack)0x4F8AD0, &search);	// CClumpModelInfo::FindFrameFromIdCB
		}
		RwFrame *f = search.out;
		if(f){
			float radius = m_pColNodeInfos[i].radius;
			if(m_pColNodeInfos[i].flag == 6)
				RwFrameForAllObjects(root, (RwObjectCallBack)0x5104A0, &radius);	// FindHeadRadiusCB
			*mat = f->modelling;
			for(f = RwFrameGetParent(f); f != root; f = RwFrameGetParent(f))
				RwMatrixTransform(mat, &f->modelling, rwCOMBINEPOSTCONCAT);
			center.x = mat->pos.x + m_pColNodeInfos[i].x;
			center.y = mat->pos.y + 0.0f;
			center.z = mat->pos.z + m_pColNodeInfos[i].z;
			spheres[i].Set(radius, &center, 17, m_pColNodeInfos[i].flag);
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
