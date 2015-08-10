#include "iii_anim.h"

WRAPPER RwFrame *FrameForAllChildrenCountCallBack(RwFrame*, void*) { EAXJMP(0x405310); }
WRAPPER const char *GetFrameNodeName(RwFrame *frame) { EAXJMP(0x527150); }

int &ClumpOffset = *(int*)0x8F1B84;
WRAPPER int CVisibilityPlugins__GetFrameHierarchyId(RwFrame*) { EAXJMP(0x528D80); }

CAnimBlendClumpData *&gpAnimBlendClump = *(CAnimBlendClumpData**)0x621000;

RpAtomic*
GetAnimHierarchyCallback(RpAtomic *atomic, void *data)
{
	*(RpHAnimHierarchy**)data = RpSkinAtomicGetHAnimHierarchy(atomic);
	return NULL;
}

RpHAnimHierarchy*
GetAnimHierarchyFromSkinClump(RpClump *clump)
{
	RpHAnimHierarchy *hier = NULL;
	RpClumpForAllAtomics(clump, GetAnimHierarchyCallback, &hier);
	return hier;
}

void
RpAnimBlendClumpUpdateAnimations(RpClump *clump, float timeDelta, bool doRender)
{
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	float totalLength = 0.0f;
	float totalBlend = 0.0f;
	gpAnimBlendClump = clumpData;

	CAnimBlendNode *nodes[17];	// 17 correct?
	int j = 0;
	nodes[0] = 0;
	void *next;
	for(void *link = clumpData->nextAssoc; link; link = next){
		CAnimBlendAssociation *a = (CAnimBlendAssociation*)((void**)link - 1);
		// have to get next pointer before calling UpdateBlend()
		next = *(void**)link;
		if(!a->UpdateBlend(timeDelta))
			continue;
		CAnimManager::UncompressAnimation(a->hierarchy);
		nodes[++j] = a->GetNode(0);
		if(a->flags & CAnimBlendAssociation::Movement){
			totalLength += a->hierarchy->totalLength / a->speed * a->blendAmount;
			totalBlend += a->blendAmount;
		}else
			nodes[0] = (CAnimBlendNode*)1;
	}
	nodes[++j] = NULL;

	if(IsClumpSkinned(clump)){
		clumpData->ForAllFrames(FrameUpdateCallBackSkinned, nodes);
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(clump);
		RpHAnimHierarchyUpdateMatrices(hier);
	}else
		clumpData->ForAllFrames(FrameUpdateCallBack, nodes);

	for(void *link = clumpData->nextAssoc; link; link = *(void**)link){
		CAnimBlendAssociation *a = (CAnimBlendAssociation*)((void**)link - 1);
		a->UpdateTime(timeDelta, totalLength == 0.0f ? 1.0f : totalBlend / totalLength);
	}
	RwFrameUpdateObjects(RpClumpGetFrame(clump));
}

CAnimBlendAssociation*
RpAnimBlendClumpGetFirstAssociation(RpClump *clump)
{
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	if(RpAnimBlendClumpIsInitialized(clump) && clumpData->nextAssoc)
		return (CAnimBlendAssociation*)((void**)clumpData->nextAssoc - 1);
	return NULL;
}

void
AnimBlendClumpDestroy(RpClump *clump)
{
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	if(clumpData){
		RpAnimBlendClumpRemoveAllAssociations(clump);
		clumpData->dtor();
		gtadelete(clumpData);
		*RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset) = NULL;
	}
}

void
RpAnimBlendAllocateData(RpClump *clump)
{
	CAnimBlendClumpData *clumpData = (CAnimBlendClumpData*)gta_nw(sizeof(CAnimBlendClumpData));
	if(clumpData)
		clumpData->ctor();
	*RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset) = clumpData;
}

const char*
ConvertBoneTag2BoneName(int tag)
{
	static char *names[] = {
		"Swaist",
		"Supperlegr",
		"Slowerlegr",
		"Sfootr",
		"Supperlegl",
		"Slowerlegl",
		"Sfootl",
		"Smid",
		"Storso",
		"Shead",
		"Supperarmr",
		"Slowerarmr",
		"SRhand",
		"Supperarml",
		"Slowerarml",
		"SLhand"
	};
	if(tag > 15)
		return NULL;
	return names[tag];
}

static AnimBlendFrameData *foundFrame;

void
FrameFindCallbackSkinned(AnimBlendFrameData *frame, void *arg)
{
	const char *name = ConvertBoneTag2BoneName(frame->nodeID);
	if(name && gtastrcmp(name, (char*)arg) == 0)
		foundFrame = frame;
}

static void
FrameFindCallback(AnimBlendFrameData *frame, void *arg)
{
	const char *name = GetFrameNodeName(frame->frame);
	if(gtastrcmp(name, (char*)arg) == 0)
		foundFrame = frame;
}

AnimBlendFrameData*
RpAnimBlendClumpFindFrame(RpClump *clump, const char *name)
{
	foundFrame = NULL;
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	if(IsClumpSkinned(clump))
		clumpData->ForAllFrames(FrameFindCallbackSkinned, (void*)name);
	else
		clumpData->ForAllFrames(FrameFindCallback, (void*)name);
	return foundFrame;
}

static void
FillFrameArrayCallback(AnimBlendFrameData *frame, void *arg)
{
	AnimBlendFrameData **frames = (AnimBlendFrameData**)arg;
	frames[CVisibilityPlugins__GetFrameHierarchyId(frame->frame)] = frame;
}

int
ConvertPedNode2BoneTag(int node)
{
	static int tags[] = { 0, 8, 9, 13, 10, 15, 12, 4, 1, 6, 3, 2, 5 };
	if(node > 12)
		return -1;
	return tags[node];
}

void
RpAnimBlendClumpFillFrameArray(RpClump *clump, AnimBlendFrameData **frames)
{
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	if(IsClumpSkinned(clump)){
		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(clump);
		for(int i = 1; i < 12; i++)
			frames[i] = &clumpData->frames[RpHAnimIDGetIndex(hier, ConvertPedNode2BoneTag(i))];
	}else
		clumpData->ForAllFrames(FillFrameArrayCallback, frames);
}

static RwFrame*
FrameForAllChildrenFillFrameArrayCallBack(RwFrame *frame, void *arg)
{
	AnimBlendFrameData **frames = (AnimBlendFrameData**)arg;
	(*frames)->frame = frame;
	++*frames;
	RwFrameForAllChildren(frame, FrameForAllChildrenFillFrameArrayCallBack, arg);
	return frame;
}

static void
FrameInitCallBack(AnimBlendFrameData *frameData, void*)
{
	frameData->flag = 0;
	RwV3d *pos = &frameData->frame->modelling.pos;
	frameData->pos.x = pos->x;
	frameData->pos.y = pos->y;
	frameData->pos.z = pos->z;
	frameData->nodeID = -1;
}

void
SkinGetBonePositionsToTable(RpClump *clump, RwV3d *boneTable)
{
	const RwMatrix *mats;
	RwMatrix m, invmat;
	int stack[32];
	if(boneTable == NULL)
		return;

//	RpAtomic *atomic = GetFirstAtomic(clump);		// mobile
	RpAtomic *atomic = IsClumpSkinned(clump);		// xbox
	RpSkin *skin = RpSkinGeometryGetSkin(atomic->geometry);
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(clump);
	boneTable[0].x = boneTable[0].y = boneTable[0].z = 0.0f;
	RwUInt32 numBones = RpSkinGetNumBones(skin);
	RwV3d *out = boneTable+1;
	int j = 0;
	int sp = 0;
	for(uint i = 1; i < numBones; i++){
		mats = RpSkinGetSkinToBoneMatrices(skin);
		RwMatrixCopy(&m, &mats[i]);
		RwMatrixInvert(&invmat, &m);
		RwV3dTransformPoints(out++, &invmat.pos, 1, &mats[j]);
		if(hier->pNodeInfo[i].flags & 2)
			stack[++sp] = j;
		if(hier->pNodeInfo[i].flags & 1)
			j = stack[sp--];
		else
			j = i;
	}
}

void
ZeroFlag(AnimBlendFrameData *frame, void*)
{
	frame->flag = 0;
}

void
RpAnimBlendClumpInitSkinned(RpClump *clump)
{
	RwV3d boneTab[64];
	RpAnimBlendAllocateData(clump);
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	RpAtomic *atomic = IsClumpSkinned(clump);
	RpSkin *skin = RpSkinGeometryGetSkin(atomic->geometry);
	RwUInt32 numBones = RpSkinGetNumBones(skin);
	clumpData->SetNumberOfBones(numBones);
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(clump);
	memset(boneTab, 0, sizeof(boneTab));
	SkinGetBonePositionsToTable(clump, boneTab);

	AnimBlendFrameData *frames = clumpData->frames;
	for(int i = 0; i < numBones; i++){
		frames[i].nodeID = hier->pNodeInfo[i].nodeID;
		frames[i].pos = boneTab[i];
		frames[i].hanimframe = (RpHAnimStdKeyFrame*)rpHANIMHIERARCHYGETINTERPFRAME(hier, i);
	}
	clumpData->ForAllFrames(ZeroFlag, NULL);
	clumpData->frames[0].flag |= 8;
}

void
RpAnimBlendClumpInit(RpClump *clump)
{
	if(IsClumpSkinned(clump)){
		RpAnimBlendClumpInitSkinned(clump);
		return;
	}
	RpAnimBlendAllocateData(clump);
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	RwFrame *frame = RpClumpGetFrame(clump);
	int numFrames = 0;
	RwFrameForAllChildren(frame, FrameForAllChildrenCountCallBack, &numFrames);
	clumpData->SetNumberOfBones(numFrames);
	AnimBlendFrameData *frames = clumpData->frames;
	RwFrameForAllChildren(frame, FrameForAllChildrenFillFrameArrayCallBack, &frames);
	clumpData->ForAllFrames(FrameInitCallBack, NULL);
	clumpData->frames[0].flag |= 8;
}

bool
RpAnimBlendClumpIsInitialized(RpClump *clump)
{
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	return clumpData && clumpData->numFrames != 0;
}

void
RpAnimBlendClumpSetBlendDeltas(RpClump *clump, uint mask, float delta)
{
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	for(void *link = clumpData->nextAssoc; link; link = *(void**)link){
		CAnimBlendAssociation *a = (CAnimBlendAssociation*)((void**)link - 1);
		if(mask == 0 || (a->flags & mask))
			a->blendDelta = delta;
	}
}

void
RpAnimBlendClumpRemoveAllAssociations(RpClump *clump)
{
	RpAnimBlendClumpRemoveAssociations(clump, 0);
}

void
RpAnimBlendClumpRemoveAssociations(RpClump *clump, uint mask)
{
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	void *next;
	for(void *link = clumpData->nextAssoc; link; link = next){
		next = *(void**)link;
		CAnimBlendAssociation *a = (CAnimBlendAssociation*)((void**)link - 1);
		if(mask == 0 || (a->flags & mask))
			if(a)
				(*(void(__thiscall**)(CAnimBlendAssociation*, int))a->vtable)(a, 1);
	}
}

CAnimBlendAssociation*
RpAnimBlendClumpGetAssociation(RpClump *clump, uint id)
{
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	for(void *link = clumpData->nextAssoc; link; link = *(void**)link){
		CAnimBlendAssociation *a = (CAnimBlendAssociation*)((void**)link - 1);
		if(a->animId == id)
			return a;
	}
	return NULL;
}

CAnimBlendAssociation*
RpAnimBlendClumpGetMainAssociation(RpClump *clump, CAnimBlendAssociation **outAssoc, float *outFloat)
{
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	CAnimBlendAssociation *retval = NULL, *assoc2 = NULL;
	float acc1 = 0.0f, acc2 = 0.0f;
	for(void *link = clumpData->nextAssoc; link; link = *(void**)link){
		CAnimBlendAssociation *a = (CAnimBlendAssociation*)((void**)link - 1);
		if(a->flags & CAnimBlendAssociation::Partial)
			continue;
		if(a->blendAmount <= acc2){
			if(a->blendAmount > acc1){
				acc1 = a->blendAmount;
				assoc2 = a;
			}
		}else{
			acc1 = acc2;
			acc2 = a->blendAmount;
			assoc2 = retval;
			retval = a;
		}
	}
	if(outAssoc)
		*outAssoc = assoc2;
	if(*outFloat)
		*outFloat = acc1;
	return retval;
}

CAnimBlendAssociation*
RpAnimBlendClumpGetMainPartialAssociation(RpClump *clump)
{
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	CAnimBlendAssociation *retval = NULL;
	float maxBlend = 0.0f;
	for(void *link = clumpData->nextAssoc; link; link = *(void**)link){
		CAnimBlendAssociation *a = (CAnimBlendAssociation*)((void**)link - 1);
		if(a->blendAmount > maxBlend){
			maxBlend = a->blendAmount;
			retval = a;
		}
	}
	return retval;
}

CAnimBlendAssociation*
RpAnimBlendClumpGetMainAssociation_N(RpClump *clump, int n)
{
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	int i = 0;
	for(void *link = clumpData->nextAssoc; link; link = *(void**)link){
		CAnimBlendAssociation *a = (CAnimBlendAssociation*)((void**)link - 1);
		if(i == n && (a->flags & CAnimBlendAssociation::Partial) == 0)
			return a;
		i++;
	}
	return NULL;
}

CAnimBlendAssociation*
RpAnimBlendClumpGetMainPartialAssociation_N(RpClump *clump, int n)
{
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	int i = 0;
	for(void *link = clumpData->nextAssoc; link; link = *(void**)link){
		CAnimBlendAssociation *a = (CAnimBlendAssociation*)((void**)link - 1);
		if(i == n && (a->flags & CAnimBlendAssociation::Partial))
			return a;
		i++;
	}
	return NULL;
}

CAnimBlendAssociation*
RpAnimBlendClumpGetFirstAssociation(RpClump *clump, uint mask)
{
	CAnimBlendClumpData *clumpData = *RWPLUGINOFFSET(CAnimBlendClumpData*, clump, ClumpOffset);
	for(void *link = clumpData->nextAssoc; link; link = *(void**)link){
		CAnimBlendAssociation *a = (CAnimBlendAssociation*)((void**)link - 1);
		if(a->flags & mask)
			return a;
	}
	return NULL;
}

CAnimBlendAssociation*
RpAnimBlendGetNextAssociation(CAnimBlendAssociation *assoc)
{
	if(assoc->next)
		return (CAnimBlendAssociation*)((void**)assoc->next - 1);
	return NULL;
}

CAnimBlendAssociation*
RpAnimBlendGetNextAssociation(CAnimBlendAssociation *assoc, uint mask)
{
	for(void *link = assoc->next; link; link = *(void**)link){
		CAnimBlendAssociation *a = (CAnimBlendAssociation*)((void**)link - 1);
		if(a->flags & mask)
			return a;
	}
	return NULL;
}

