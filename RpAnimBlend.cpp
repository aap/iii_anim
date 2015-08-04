#include "iii_anim.h"

WRAPPER const char *GetFrameNodeName(RwFrame *frame) { EAXJMP(0x527150); }

int &ClumpOffset = *(int*)0x8F1B84;

static AnimBlendFrameData *foundFrame;

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
	clumpData->ForAllFrames(FrameFindCallback, (void*)name);
	return foundFrame;
}

WRAPPER void
RpAnimBlendClumpInit(RpClump *clump)
{
	EAXJMP(0x405480);
}