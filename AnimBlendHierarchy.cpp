#include "iii_anim.h"

WRAPPER void CLink_CAnimBlendHierarchy::Insert(CLink_CAnimBlendHierarchy*) { EAXJMP(0x404460); }
WRAPPER void CLink_CAnimBlendHierarchy::Remove(void) { EAXJMP(0x404480); }

WRAPPER void CLinkList_CAnimBlendHierarchy::Init(int n) { EAXJMP(0x404370); }
WRAPPER void CLinkList_CAnimBlendHierarchy::Shutdown(void) { EAXJMP(0x4043E0); }
WRAPPER CLink_CAnimBlendHierarchy *CLinkList_CAnimBlendHierarchy::Insert(CAnimBlendHierarchy**) { EAXJMP(0x404400); }
WRAPPER void CLinkList_CAnimBlendHierarchy::Remove(CLink_CAnimBlendHierarchy*) { EAXJMP(0x404440); }

CAnimBlendHierarchy::CAnimBlendHierarchy(void)
{
	this->blendSequences = NULL;
	this->numSequences = 0;
	this->compressed = 0;
	this->totalLength = 0.0f;
	this->linkPtr = NULL;
}

void
CAnimBlendHierarchy::dtor(void) { }

CAnimBlendHierarchy::~CAnimBlendHierarchy(void) { }

void
CAnimBlendHierarchy::Shutdown(void)
{
	RemoveAnimSequences();
	this->compressed = 0;
	this->linkPtr = NULL;
}

void
CAnimBlendHierarchy::SetName(const char *name)
{
	strncpy(this->name, name, 24);
}

void
CAnimBlendHierarchy::CalcTotalTime(void)
{
	this->totalLength = 0.0f;
	for(int i = 0; i < this->numSequences; i++){
		CAnimBlendSequence *seq = &this->blendSequences[i];
		float acc = 0.0f;
		for(int j = 0; j < seq->numFrames; j++)
			acc += ((RFrame*)GETFRAME(seq, j))->time;
		if(acc > this->totalLength)
			this->totalLength = acc;
	}
}

void
CAnimBlendHierarchy::RemoveQuaternionFlips(void)
{
	for(int i = 0; i < this->numSequences; i++)
		this->blendSequences[i].RemoveQuaternionFlips();
}

void
CAnimBlendHierarchy::RemoveAnimSequences(void)
{
	if(this->blendSequences)
		destroy_array(this->blendSequences, &CAnimBlendSequence::dtor);
	this->numSequences = 0;
}

void
CAnimBlendHierarchy::Uncompress(void)
{
	if(this->totalLength == 0.0f)
		this->CalcTotalTime();
	this->compressed = 0;
}

void
CAnimBlendHierarchy::RemoveUncompressedData(void)
{
	this->compressed = 1;
}
