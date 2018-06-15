#include "iii_anim.h"

WRAPPER void __fastcall CBaseModelInfo__AddTexDictionaryRef(int self) { EAXJMP(0x4F6B80); }
WRAPPER void CClumpModelInfo::SetFrameIds(RwObjectNameIdAssocation *ids) { EAXJMP(0x4F8BB0); }
WRAPPER void __fastcall CPedModelInfo__CreateHitColModel(void *self) { EAXJMP(0x5104D0); }
WRAPPER void __fastcall CPedModelInfo__DeleteRwObject_orig(CPedModelInfo*) { EAXJMP(0x510280); }

WRAPPER RpAtomic *CClumpModelInfo::SetAtomicRendererCB(RpAtomic*, void*) { EAXJMP(0x4F8940); }
WRAPPER RwFrame *CClumpModelInfo::FindFrameFromNameCB(RwFrame *frame, void *data) { EAXJMP(0x4F8960); }
WRAPPER RwFrame *CClumpModelInfo::FindFrameFromIdCB(RwFrame *frame, void *data) { EAXJMP(0x4F8AD0); }

// Render a ped with clump alpha
RpAtomic*
CVisibilityPlugins__RenderPedCB(RpAtomic *atomic)
{
	int alpha = CVisibilityPlugins__GetClumpAlpha(RpAtomicGetClump(atomic));
	if(alpha == 255)
		AtomicDefaultRenderCallBack(atomic);
	else
		CVisibilityPlugins__RenderAlphaAtomic(atomic, alpha);
	return atomic;
}

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
		assert(clone->object.type == rpCLUMP);
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
	assert(clump->object.type == rpCLUMP);
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

		assert(atomic->geometry->object.type = rpGEOMETRY);
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
	AttachRimPipeToRwObject((RwObject*)atomic);
	RpClumpRemoveAtomic(limbs->clump, atomic);
	RwFrameRemoveChild(frame);
	return atomic;
}

// Only used with unskinned geometry
RwObjectNameIdAssocation CPedModelInfo::m_pPedIds[12] = {
	{ "Smid",	PED_TORSO, 0, },	// that is strange...
	{ "Shead",	PED_HEAD, 0, },
	{ "Supperarml",	PED_UPPERARML, 0, },
	{ "Supperarmr",	PED_UPPERARMR, 0, },
	{ "SLhand",	PED_HANDL, 0, },
	{ "SRhand",	PED_HANDR, 0, },
	{ "Supperlegl",	PED_UPPERLEGL, 0, },
	{ "Supperlegr",	PED_UPPERLEGR, 0, },
	{ "Sfootl",	PED_FOOTL, 0, },
	{ "Sfootr",	PED_FOOTR, 0, },
	{ "Slowerlegr",	PED_LOWERLEGR, 0, },
	{ NULL,	0, 0, },
};

void
CPedModelInfo::SetClump(RpClump *clump)
{
	int isplayer = strcmp(this->name, "player") == 0;
	RpAtomic *atomic;
	// set renderCB before removing Atomics from Clump
	assert(clump->object.type == rpCLUMP);
	if(isplayer)
		RpClumpForAllAtomics(clump, CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins__RenderPlayerCB);
	if(atomic = IsClumpSkinned(clump)){
		LimbCBarg limbs = { this, clump, 0, 0, 0 };
		RpClumpForAllAtomics(clump, CPedModelInfo__findLimbsCb, &limbs);
	}
	this->CClumpModelInfo::SetClump(clump);
	this->SetFrameIds(m_pPedIds);
	if(this->hitColModel == NULL && !IsClumpSkinned(clump))
		this->CreateHitColModel();
	// again, because CClumpModelInfo::SetClump resets renderCB
	if(isplayer)
		RpClumpForAllAtomics(clump, CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins__RenderPlayerCB);
	else if(IsClumpSkinned(clump))
		// Skinned models have no low detail clump and the default render CB doesn't handle clump alpha
		RpClumpForAllAtomics(clump, CClumpModelInfo::SetAtomicRendererCB, CVisibilityPlugins__RenderPedCB);
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
	if(CModelInfo::ms_pedModelStore.numElements >= MAXPEDMODELS)
		printf("Size of this thing:%d needs increasing\n", MAXPEDMODELS);
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
	assert(atomic->geometry->object.type = rpGEOMETRY);
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
	assert(clump->object.type == rpCLUMP);
	RpClumpForAllAtomics(clump, AtomicRemoveAnimFromSkinCB, NULL);
	RpClumpDestroy(clump);
}

struct ColModelNodeInfo
{
	char *name;
	int pednode;
	int piecetype;
	float x;
	float z;
	float radius;
};


/*
// Names and tags mixed because there is no tag for everything
ColLimb m_pColNodeInfos[8] = {
	{ NULL,         PED_HEAD,        6,  0.0f,   0.05f, 0.2f },
	{ "Storso",     0,               0,  0.0f,   0.15f, 0.2f },
	{ "Storso",     0,               0,  0.0f,  -0.05f, 0.3f },
	{ NULL,         PED_TORSO,       1,  0.0f,  -0.07f, 0.3f },
	{ NULL,         PED_UPPERARML,   2,  0.07f, -0.1f,  0.2f },
	{ NULL,         PED_UPPERARMR,   3, -0.07f, -0.1f,  0.2f },
	{ "Slowerlegl", 0,               4,  0.0f,   0.07f, 0.25f },
	{ NULL,         PED_LOWERLEGR,   5,  0.0f,   0.07f, 0.25f },
};
*/
// We can't use strings for skinned animations but we want all bone
// tags, so do a little hack with PED_TORSO2 and PED_LOWERLEGL
#define NUMPEDINFONODES 10
ColModelNodeInfo m_pColNodeInfos[NUMPEDINFONODES] = {
	{ NULL,         PED_HEAD,          6,  0.0f,   0.05f, 0.2f },
	{ "Storso",     PED_ACTUAL_TORSO,  0,  0.0f,   0.15f, 0.2f },	// Storso
	{ "Storso",     PED_ACTUAL_TORSO,  0,  0.0f,  -0.05f, 0.3f },	// Storso
	{ "Smid",       PED_ACTUAL_MID,    1,  0.0f,  -0.07f, 0.3f },	// Smid
	{ NULL,         PED_UPPERARML,     2,  0.07f, -0.1f,  0.2f },
	{ NULL,         PED_UPPERARMR,     3, -0.07f, -0.1f,  0.2f },
	{ "Slowerlegl", PED_LOWERLEGL,     4,  0.0f,   0.07f, 0.25f },
	{ NULL,         PED_LOWERLEGR,     5,  0.0f,   0.07f, 0.25f },
	// Add the feet from VC
	{ NULL,		PED_FOOTL,         4,  0.0f,   0.0f, 0.15f },
	{ NULL,		PED_FOOTR,         5,  0.0f,   0.0f, 0.15f },
};
/*
// VC, but doesn't work a lot better either
#define NUMPEDINFONODES 10
ColModelNodeInfo m_pColNodeInfos[NUMPEDINFONODES] = {
	{ NULL,		PED_HEAD,       6,  0.0f,   0.05f, 0.15f },
	{ "Storso",	PED_TORSO,      0,  0.0f,   0.15f, 0.2f },
	{ "Storso",	PED_TORSO,      0,  0.0f,  -0.05f, 0.25f },
	{ NULL,		PED_TORSO,      1,  0.0f,  -0.25f, 0.25f },
	{ NULL,		PED_UPPERARML,  2,  0.03f, -0.05f, 0.16f },
	{ NULL,		PED_UPPERARMR,  3, -0.03f, -0.05f,  0.16f },
	{ "Slowerlegl",	PED_LOWERLEGL,  4,  0.0f,   0.15f, 0.2f },
	{ NULL,		PED_LOWERLEGR,  5,  0.0f,   0.15f, 0.2f },
	{ NULL,		PED_FOOTL,      4,  0.0f,   0.15f, 0.15f },
	{ NULL,		PED_FOOTR,      5,  0.0f,   0.15f, 0.15f },
};
*/

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
	for(int i = 0; i < NUMPEDINFONODES; i++){
		RwV3d pos;
		pos.x = pos.y = pos.z = 0.0f;
		*mat = *invmat;
		int id = ConvertPedNode2BoneTag(m_pColNodeInfos[i].pednode);
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

CColModel*
CPedModelInfo::AnimatePedColModel(CColModel *colmodel, RwFrame *frame)
{
	RwObjectAssociation search;
	RwMatrix *mat = RwMatrixCreate();
	CColSphere *spheres = colmodel->spheres;
	
	for(int i = 0; i < NUMPEDINFONODES; i++){
		search.name = m_pColNodeInfos[i].name;
		search.out = NULL;
		RwFrameForAllChildren(frame, FindFrameFromNameCB, &search);
		
		RwFrame *f = search.out;
		if(f){
			RwMatrixCopy(mat, RwFrameGetMatrix(f));
			
			for(f = RwFrameGetParent(f); f; f = RwFrameGetParent(f)){
				RwMatrixTransform(mat, &f->modelling, rwCOMBINEPOSTCONCAT);
				if(RwFrameGetParent(f) == frame)
					break;
			}
			
			spheres[i].center.x = mat->pos.x + m_pColNodeInfos[i].x;
			spheres[i].center.y = mat->pos.y + 0.0f;
			spheres[i].center.z = mat->pos.z + m_pColNodeInfos[i].z;
		}
	}
	
	return colmodel;
}

CColModel*
CPedModelInfo::AnimatePedColModelWorld(CColModel *colmodel, RwFrame *frame)
{
	RwObjectAssociation search;
	RwMatrix *mat = RwMatrixCreate();
	CColSphere *spheres = colmodel->spheres;
	
	for(int i = 0; i < NUMPEDINFONODES; i++){
		if(m_pColNodeInfos[i].name){
			search.name = m_pColNodeInfos[i].name;
			search.out = NULL;
			RwFrameForAllChildren(frame, FindFrameFromNameCB, &search);
		}else{
			search.id = m_pColNodeInfos[i].pednode;
			search.out = NULL;
			RwFrameForAllChildren(frame, FindFrameFromIdCB, &search);
		}
		
		RwFrame *f = search.out;
		if(f){
			RwMatrixCopy(mat, RwFrameGetMatrix(f));
			
			RwV3d point = {0.0f, 0.0f, 0.0f};
			RwV3dTransformPoints(&point, &point, 1, mat);
			
			spheres[i].center.x = point.x + m_pColNodeInfos[i].x;
			spheres[i].center.y = point.y + 0.0f;
			spheres[i].center.z = point.z + m_pColNodeInfos[i].z;
		}
	}
	
	return colmodel;
}

CColModel*
CPedModelInfo::AnimatePedColModelSkinnedWorld(RpClump *clump)
{
	CColModel *colmodel = this->hitColModel;
	if(colmodel == NULL)
		this->CreateHitColModelSkinned(clump);
	
	CColSphere *spheres = colmodel->spheres;
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(clump);
	
	for(int i = 0; i < NUMPEDINFONODES; i++)
	{	
		RwV3d point = {0.0f, 0.0f, 0.0f};
		int id = ConvertPedNode2BoneTag(m_pColNodeInfos[i].pednode);
		int idx = RpHAnimIDGetIndex(hier, id);
		
		RwV3dTransformPoints(&point, &point, 1, &RpHAnimHierarchyGetMatrixArray(hier)[idx]);
		
		spheres[i].center.x = point.x + m_pColNodeInfos[i].x;
		spheres[i].center.y = point.y + 0.0f;
		spheres[i].center.z = point.z + m_pColNodeInfos[i].z;
	}
	
	return colmodel;
}

void
CPedModelInfo::CreateHitColModelSkinned(RpClump *clump)
{
	CVector center;
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(clump);
	CColModel *colmodel = (CColModel*)gta_nw(sizeof(CColModel));
	colmodel->ctor();
	CColSphere *spheres = (CColSphere*)RwMalloc(NUMPEDINFONODES*sizeof(CColSphere));
	RwMatrix *invmat, *mat;
	invmat = RwMatrixCreate();
	mat = RwMatrixCreate();
	RwMatrixInvert(invmat, &RpClumpGetFrame(clump)->modelling);

	for(int i = 0; i < NUMPEDINFONODES; i++){
		*mat = *invmat;
		int id = ConvertPedNode2BoneTag(m_pColNodeInfos[i].pednode);	// this is wrong, wtf R* ???
		int idx = RpHAnimIDGetIndex(hier, id);

		// This doesn't really work as the positions are not initialized yet
		RwMatrixTransform(mat, &RpHAnimHierarchyGetMatrixArray(hier)[idx], rwCOMBINEPRECONCAT);
		RwV3d pos;
		pos.x = pos.y = pos.z = 0.0f;
		RwV3dTransformPoints(&pos, &pos, 1, mat);

		center.x = pos.x + m_pColNodeInfos[i].x;
		center.y = pos.y + 0.0f;
		center.z = pos.z + m_pColNodeInfos[i].z;
		spheres[i].Set(m_pColNodeInfos[i].radius, &center, 17, m_pColNodeInfos[i].piecetype);
	}
	RwMatrixDestroy(invmat);
	RwMatrixDestroy(mat);
	colmodel->spheres = spheres;
	colmodel->numSpheres = NUMPEDINFONODES;
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
	RwObjectAssociation search;
	CVector center;
	CColModel *colmodel = (CColModel*)gta_nw(sizeof(CColModel));
	colmodel->ctor();
	CColSphere *spheres = (CColSphere*)RwMalloc(NUMPEDINFONODES*sizeof(CColSphere));
	RwFrame *root = RpClumpGetFrame(this->clump);
	RwMatrix *mat = RwMatrixCreate();
	for(int i = 0; i < NUMPEDINFONODES; i++){
		if(m_pColNodeInfos[i].name){
			search.name = m_pColNodeInfos[i].name;
			search.out = NULL;
			RwFrameForAllChildren(root, FindFrameFromNameCB, &search);
		}else{
			search.id = m_pColNodeInfos[i].pednode;
			search.out = NULL;
			RwFrameForAllChildren(root, FindFrameFromIdCB, &search);
		}
		RwFrame *f = search.out;
		if(f){
			float radius = m_pColNodeInfos[i].radius;
			if(m_pColNodeInfos[i].piecetype == 6)
				RwFrameForAllObjects(f, (RwObjectCallBack)0x5104A0, &radius);	// FindHeadRadiusCB
			RwMatrixTransform(mat, &f->modelling, rwCOMBINEREPLACE);
			const char *name = GetFrameNodeName(f);
			for(f = RwFrameGetParent(f); f; f = RwFrameGetParent(f)){
				name = GetFrameNodeName(f);
				RwMatrixTransform(mat, &f->modelling, rwCOMBINEPOSTCONCAT);
				if(RwFrameGetParent(f) == root)
					break;
			}
			center.x = mat->pos.x + m_pColNodeInfos[i].x;
			center.y = mat->pos.y + 0.0f;
			center.z = mat->pos.z + m_pColNodeInfos[i].z;
			spheres[i].Set(radius, &center, 17, m_pColNodeInfos[i].piecetype);
		}else
			assert(0);
	}
	RwMatrixDestroy(mat);
	colmodel->spheres = spheres;
	colmodel->numSpheres = NUMPEDINFONODES;
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
