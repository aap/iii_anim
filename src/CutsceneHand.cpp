#include "iii_anim.h"
#include "XMLConfigure.h"

int useCutsceneHands = 1;

char *HAND_TXDSLOT_NAME = "hand";
char *HAND_TXD_PATH = "anim\\CSHands.txd";


WRAPPER void object_delete(void*) { EAXJMP(0x4BAEA0); }
WRAPPER void *object_nw(unsigned int) { EAXJMP(0x4BAE70); }

char &CTimer__m_CodePause = *(char*)0x95CDB1;

CRunningScript *&CTheScripts__pActiveScripts = *(CRunningScript**)0x8E2BF4;

WRAPPER void CWorld__Add(CEntity *entity) { EAXJMP(0x4AE930); }
CCutsceneObject **CCutsceneMgr__ms_pCutsceneObjects = (CCutsceneObject **)0x862170;
int &CCutsceneMgr__ms_numCutsceneObjs = *(int *)0x942FA4;
char* CCutsceneMgr__ms_cutsceneName = (char*)0x70D9D0;
char & CCutsceneMgr__ms_cutsceneProcessing = *(char*)0x95CD9F;

WRAPPER CObject *AddCutsceneHead(CObject *object, int mi) { EAXJMP(0x404CD0); }
WRAPPER int CCutsceneMgr__GetCutsceneTimeInMilleseconds() { EAXJMP(0x4051B0); }

WRAPPER int CTxdStore__AddTxdSlot(char  const* name) { EAXJMP(0x5274E0)0; }
WRAPPER void CTxdStore__RemoveTxdSlot(int index) { EAXJMP(0x527520); }
WRAPPER char *CTxdStore__GetTxdName(int index) { EAXJMP(0x527590); }
WRAPPER bool CTxdStore__LoadTxd(int index, char  const* filename) { EAXJMP(0x5276B0); }
WRAPPER void CTxdStore__AddRef(int index) { EAXJMP(0x527930); }
WRAPPER void CTxdStore__SetCurrentTxd(int index) { EAXJMP(0x5278C0); }
WRAPPER int CTxdStore__FindTxdSlot(char  const* name) { EAXJMP(0x5275D0); }
WRAPPER void CTxdStore__PushCurrentTxd() { EAXJMP(0x527900); }
WRAPPER void CTxdStore__PopCurrentTxd() { EAXJMP(0x527910); }

WRAPPER CBaseModelInfo *CModelInfo__GetModelInfo(char const *, int *) { EAXJMP(0x50B860); }

struct AtomicByNameData
{
	char name[256];
	RpAtomic *atomic;
};

struct HandInfo
{
	RpClump *clump;
	char *atomicName;
	char *fileName;
};

struct HandAnimInfo
{
	char *animFile;
	RpHAnimAnimation *anim;
};

HandAnimInfo handAnimDefs[MAX_GENDERS][MAX_HANDS] =
{
	{
		{ "anim\\SLhandWM.anm", NULL },
		{ "anim\\SRhandWM.anm", NULL }
	},

	{
		{ "anim\\SLhandWF.anm", NULL },
		{ "anim\\SRhandWF.anm", NULL }
	}
};

HandInfo handDefs[MAX_GENDERS][MAX_RACES][MAX_HANDS] =
{
	{
		{
			{ NULL, "SLhandPalmCM", "anim\\SLhandCM.dff" },
			{ NULL, "SRhandPalmCM", "anim\\SLhandCM.dff" }
		},

		{
			{ NULL, "SLhandPalmWM", "anim\\SLhandWM.dff" },
			{ NULL, "SRhandPalmWM", "anim\\SRhandWM.dff" }
		}
	},

	{
		{
			{ NULL, "SLhandPalmCF", "anim\\SLhandCF.dff" },
			{ NULL, "SRhandPalmCF", "anim\\SRhandCF.dff" }
		},

		{
			{ NULL, "SLhandPalmWF", "anim\\SLhandWF.dff" },
			{ NULL, "SRhandPalmWF", "anim\\SRhandWF.dff" }
		}
	}
};

HandInfo stickDefs[MAX_PROPS][MAX_HANDS] =
{
	{
		{ NULL, "SRhandPalmWF-Stick", "anim\\SRhandWFStick.dff" },
		{ NULL, "SRhandPalmWF-Stick", "anim\\SRhandWFStick.dff" }
	},

	{
		{ NULL, "SRhandPalmWF-Stick", "anim\\SRhandWFStick.dff" },
		{ NULL, "SRhandPalmWF-Stick", "anim\\SRhandWFStick.dff" }
	}
};

char *aHands[] =
{
	"SLhand",
	"SRhand"
};

CCutsceneHand::HandInstance CCutsceneHand::Instances[MAX_CUTSCENEPEDS][MAX_HANDS];


template <typename T> void * func2ptr(T o)
{
	auto member = o;
	void*& ptr = (void*&)member;
	return ptr;
}

void *_CCutsceneHand_vtable[]  =
{
	func2ptr(&CCutsceneHand::hand_virtual_dtor),
	(void *)0x4951F0,
	(void *)0x4954B0,
	func2ptr(&CCutsceneHand::SetModelIndex),
	(void *)0x473E90,
	func2ptr(&CCutsceneHand::CreateRwObject),
	func2ptr(&CCutsceneHand::DeleteRwObject),
	(void *)0x495150,
	func2ptr(&CCutsceneHand::ProcessControl),
	(void *)0x4961A0,
	(void *)0x496F10,
	(void *)0x4BBDA0,
	func2ptr(&CCutsceneHand::PreRender),
	func2ptr(&CCutsceneHand::Render),
	(void *)0x4A7E70,
	(void *)0x4A7F00,
	(void *)0x405940,
	(void *)0x49F790
};


WRAPPER int CFileMgr__LoadFile(const char *path, char *buf, int size, const char *mode) { EAXJMP(0x479090); }


void CCutsceneHand::Init()
{
	const char *curfilename = "";
	printf("CCutsceneHand::Init\n");

	InitXML();
	if(!useCutsceneHands)
		return;
	InitialiseCharacters();

	int errors = 0;

	for(int g = 0; g < MAX_GENDERS; g++){
		for(int r = 0; r < MAX_RACES; r++){
			for(int h = 0; h < MAX_HANDS; h++){
				SetupTxd();
				curfilename = handDefs[g][r][h].fileName;
				RwStream *stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, curfilename);
				if(stream == NULL) goto cantopen;
				if(!RwStreamFindChunk(stream, rwID_CLUMP, NULL, NULL)) goto noclump;
				handDefs[g][r][h].clump = RpClumpStreamRead(stream);
				RwStreamClose(stream, NULL);
				errors |= handDefs[g][r][h].clump == NULL;
				assert(handDefs[g][r][h].clump);

				printf("RpClumpStreamRead: 0x%X, %s\n", handDefs[g][r][h].clump, curfilename);
			}
		}
	}

	for(int p = 0; p < MAX_PROPS; p++){
		for(int h = 0; h < MAX_HANDS; h++){
			SetupTxd();
			curfilename = stickDefs[p][h].fileName;
			RwStream *stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, curfilename);
			if(stream == NULL) goto cantopen;
			if(!RwStreamFindChunk(stream, rwID_CLUMP, NULL, NULL)) goto noclump;
			stickDefs[p][h].clump = RpClumpStreamRead(stream);
			RwStreamClose(stream, NULL);
			errors |= stickDefs[p][h].clump == NULL;
			assert(stickDefs[p][h].clump);

			printf("RpClumpStreamRead: 0x%X, %s\n", stickDefs[p][h].clump, curfilename);
		}
	}

	for(int g = 0; g < MAX_GENDERS; g++){
		for(int h = 0; h < MAX_HANDS; h++){
			curfilename = handAnimDefs[g][h].animFile;
			RwStream *stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, curfilename);
			if(stream == NULL) goto cantopen;
			if(!RwStreamFindChunk(stream, rwID_HANIMANIMATION, NULL, NULL)) goto noanim;
			handAnimDefs[g][h].anim = RpHAnimAnimationStreamRead(stream);
			RwStreamClose(stream, NULL);
			errors |= handAnimDefs[g][h].anim == NULL;
			assert(handAnimDefs[g][h].anim);

			printf("RpHAnimAnimationStreamRead: 0x%X, 0x%X %d %f %s\n", handAnimDefs[g][h].anim, handAnimDefs[g][h].anim->pFrames, handAnimDefs[g][h].anim->numFrames, handAnimDefs[g][h].anim->duration, curfilename);
		}
	}

	if(errors){
		MessageBox(NULL, "Could not load Cutscene hands, please put all needed files into the anim directory", "Error", MB_ICONERROR | MB_OK);
		exit(1);
	}
	return;
	static char errstr[256];
cantopen:
	snprintf(errstr, 256, "Could not open file %s, please put all needed files into the anim directory", curfilename);
	MessageBox(NULL, errstr, "Error", MB_ICONERROR | MB_OK);
	exit(1);
noclump:
	snprintf(errstr, 256, "Could not find clump in file %s, please put all needed files into the anim directory", curfilename);
	MessageBox(NULL, errstr, "Error", MB_ICONERROR | MB_OK);
	exit(1);
noanim:
	snprintf(errstr, 256, "Could not find animation in file %s, please put all needed files into the anim directory", curfilename);
	MessageBox(NULL, errstr, "Error", MB_ICONERROR | MB_OK);
	exit(1);
}

void CCutsceneHand::SetupTxd()
{
	int slot = CTxdStore__FindTxdSlot(HAND_TXDSLOT_NAME);
	if(slot == -1)
		slot = CTxdStore__AddTxdSlot(HAND_TXDSLOT_NAME);

	CTxdStore__LoadTxd(slot, HAND_TXD_PATH);
	CTxdStore__AddRef(slot);
	CTxdStore__SetCurrentTxd(slot);
}

void CCutsceneHand::DestroyTxd()
{
	int slut = CTxdStore__FindTxdSlot(HAND_TXDSLOT_NAME);
	if(slut != -1)
		CTxdStore__RemoveTxdSlot(slut);
}

static XmlLibrary::Parser *g_parser = NULL;
static XmlLibrary::Element *g_element = NULL;

void CCutsceneHand::InitXML()
{
	printf("CCutsceneHand::InitXML\n");

	const int xml_buff_size = 64000;

	if(g_parser == NULL){
		char *xml_buff = new char[xml_buff_size];

		if(xml_buff){
			int ret;
			ret = CFileMgr__LoadFile("data\\CutsceneHands.xml", xml_buff, xml_buff_size, "r");
			if(ret < 0){
				useCutsceneHands = 0;
				delete[] xml_buff;
				return;
			}
			g_parser = new XmlLibrary::Parser();
			g_element = &g_parser->Parse(xml_buff, xml_buff_size);

			delete[] xml_buff;
		}
	}
}

void CCutsceneHand::CleanUp()
{
	printf("CCutsceneHand::CleanUp\n");
	if(!useCutsceneHands)
		return;

	for(int g = 0; g < MAX_GENDERS; g++){
		for(int h = 0; h < MAX_HANDS; h++){
			if(handAnimDefs[g][h].anim){
				RpHAnimAnimationDestroy(handAnimDefs[g][h].anim);
				handAnimDefs[g][h].anim = NULL;
			}
			printf("RpHAnimAnimationDestroy: 0x%X, %s\n", handAnimDefs[g][h].anim, handAnimDefs[g][h].animFile);
		}
	}

	for(int g = 0; g < MAX_GENDERS; g++){
		for(int r = 0; r < MAX_RACES; r++){
			for(int h = 0; h < MAX_HANDS; h++){
				if(handDefs[g][r][h].clump){
					RpClumpDestroy(handDefs[g][r][h].clump);
					handDefs[g][r][h].clump = NULL;
				}
				printf("RpClumpDestroy: 0x%X, %s\n", handDefs[g][r][h].clump, handDefs[g][r][h].fileName);
			}
		}
	}

	for(int p = 0; p < MAX_PROPS; p++ ){
		for(int h = 0; h < MAX_HANDS; h++){
			if(stickDefs[p][h].clump){
				RpClumpDestroy(stickDefs[p][h].clump);
				stickDefs[p][h].clump = NULL;
			}
			printf("RpClumpDestroy: 0x%X, %s\n", stickDefs[p][h].clump, stickDefs[p][h].fileName);
		}
	}

	DestroyTxd();

	if(g_parser){
		delete g_parser;

		g_parser = NULL;
		g_element = NULL;
	}
}

void CCutsceneHand::HandInstance::Initialise()
{
	m_fAnimTime = 0.0f;

	m_vecScale.x = 1.0f;
	m_vecScale.y = 1.0f;
	m_vecScale.z = 1.0f;

	m_pAtomic = NULL;

	field_4 = 0;

	m_bHasHands = true;

	m_Color.r = 255;
	m_Color.g = 255;
	m_Color.b = 255;
	m_Color.a = 255;

	m_aName[0] = '\0';

	m_Gender = HANDEDNESS_MALE;
	m_Race = RACE_WHITE;
	m_Prop = PROP_NONE;
}

void CCutsceneHand::InitialiseCharacters()
{
	for(int p = 0; p < MAX_CUTSCENEPEDS; p++)
		for(int h = 0; h < MAX_HANDS; h++)
			Instances[p][h].Initialise();
}

RwObject*
hideAtomicCB(RwObject *object, void *data)
{
	if(RwObjectGetType(object) == rpATOMIC){
		RpAtomic *atomic = (RpAtomic*)object;
		RpAtomicSetFlags(atomic, RpAtomicGetFlags(atomic) & ~rpATOMICRENDER);
	}
	return object;
}

RwObject*
showAtomicCB(RwObject *object, void *data)
{
	if(RwObjectGetType(object) == rpATOMIC){
		RpAtomic *atomic = (RpAtomic*)object;
		RpAtomicSetFlags(atomic, RpAtomicGetFlags(atomic) | rpATOMICRENDER);
	}
	return object;
}

CCutsceneHand *CCutsceneHand::hand_ctor(CObject *object, Handedness hand)
{
	ctor(); // CCutsceneObject
	vtable = _CCutsceneHand_vtable;

	ObjectExt *ext = getExt();
	ext->m_bIsSkinned = false;
	ext->m_pObject = (CCutsceneObject *)object;
	ext->m_Hand = hand;
	ext->m_nPedIndex = GetIdForCharacter(object->nModelIndex);
	Instances[ext->m_nPedIndex][ext->m_Hand].Initialise();

	char *name = ((CBaseModelInfo *)CModelInfo::ms_modelInfoPtrs[object->nModelIndex])->name;

	strncpy(Instances[ext->m_nPedIndex][ext->m_Hand].m_aName, name, 15);

	ext->m_bIsSkinned = IsClumpSkinned(object->clump) != NULL;
	if(!ext->m_bIsSkinned){
		AnimBlendFrameData *frameData = RpAnimBlendClumpFindFrame(object->clump, aHands[ext->m_Hand]);

		ext->m_pHandFrame = frameData->frame;

		// hide regular hands
		// TODO: this may not be what we want. but this whole code is a mess.
		RwFrameForAllObjects(ext->m_pHandFrame, hideAtomicCB, NULL);

		AtomicByNameData atomicData;

		atomicData.atomic = NULL;
		atomicData.name[0] = '\0';

		Race race = Instances[ext->m_nPedIndex][ext->m_Hand].m_Race;
		Prop prop = Instances[ext->m_nPedIndex][ext->m_Hand].m_Prop;
		Gender gender = Instances[ext->m_nPedIndex][ext->m_Hand].m_Gender;

		char *atomicName = NULL;
		if(Instances[ext->m_nPedIndex][ext->m_Hand].m_Prop == PROP_STICK)
			atomicName = stickDefs[prop][ext->m_Hand].atomicName;
		else
			atomicName = handDefs[gender][race][ext->m_Hand].atomicName;

		strcpy(atomicData.name, atomicName);

		//wtf ? where is FindHandAtomicByName ?

		Instances[ext->m_nPedIndex][ext->m_Hand].m_pAtomic = atomicData.atomic;

		if(Instances[ext->m_nPedIndex][ext->m_Hand].m_pAtomic)
			RpAtomicSetFlags(Instances[ext->m_nPedIndex][ext->m_Hand].m_pAtomic, RpAtomicGetFlags(Instances[ext->m_nPedIndex][ext->m_Hand].m_pAtomic) & rpATOMICRENDER);

	}else{
		if(ext->m_pObject){
			if(ext->m_Hand == HANDEDNESS_LEFT)
				ext->m_pObject->SetRenderLeftHand(false);
			else if(ext->m_Hand == HANDEDNESS_RIGHT)
				ext->m_pObject->SetRenderRightHand(false);
		}
	}

	CTxdStore__PushCurrentTxd();
	int slot = CTxdStore__FindTxdSlot(HAND_TXDSLOT_NAME);
	CTxdStore__SetCurrentTxd(slot);
	CreateRwObject();
	CTxdStore__PopCurrentTxd();

	return this;
}

void CCutsceneHand::handdtor()
{
	vtable = _CCutsceneHand_vtable;
	DeleteRwObject();
	Instances[getExt()->m_nPedIndex][getExt()->m_Hand].Initialise();
	dtor(); // CCutsceneObject
}

CCutsceneHand *CCutsceneHand::hand_virtual_dtor(char flag)
{
	if(this){
		if(flag & 2)
			destroy_array(this, &CCutsceneHand::handdtor);
		else{
			vtable = _CCutsceneHand_vtable;
			dtor(); // CCutsceneObject
			if(flag & 1)
				object_delete(this);
		}
	}

	return this;
}

void CCutsceneHand::SetModelIndex(unsigned int mi)
{
	nModelIndex = mi;
}

void CCutsceneHand::CreateRwObject()
{
	ObjectExt *ext = getExt();
	SetSkinnedModel(ext->m_nPedIndex,
					(Handedness)ext->m_Hand,
					Instances[ext->m_nPedIndex][ext->m_Hand].m_Gender,
					Instances[ext->m_nPedIndex][ext->m_Hand].m_Race,
					Instances[ext->m_nPedIndex][ext->m_Hand].m_Stature,
					Instances[ext->m_nPedIndex][ext->m_Hand].m_Prop );
}

void CCutsceneHand::DeleteRwObject()
{
	if(clump){
		matrix.Detach();

		if(RwObjectGetType(clump) == rpCLUMP)
			RpClumpDestroy(clump);
		clump = NULL;
	}
}

void CCutsceneHand::ProcessControl()
{
	ObjectExt *ext = getExt();
	if(!ext->m_bIsSkinned){
		RwMatrixSetIdentity(&matrix.matrix);

		CMatrix mat1, mat2;
		mat1.ctor(RwFrameGetLTM(ext->m_pHandFrame), false);
		CMatrix::mult(&mat2, &mat1, &this->matrix);
		this->matrix.assign(&mat2);
		mat2.dtor();
		mat1.dtor();
	}else{
		ConfigureForCharacter(ext->m_nPedIndex);

		ext->m_pObject->UpdateRpHAnim();
		ext->m_pObject->UpdateRwFrame();

		RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ext->m_pObject->clump);
		int idx = RpHAnimIDGetIndex(hier, ext->m_Hand == HANDEDNESS_RIGHT ? BONE_SRhand : BONE_SLhand);
		RwMatrix *ltm = &RpHAnimHierarchyGetMatrixArray(hier)[idx];

		if(RwV3dLength(&ltm->pos) > 100.0f){
			CMatrix mat1, mat2;
			RwMatrixSetIdentity(&matrix.matrix);

			mat1.ctor(ltm, false);
			CMatrix::mult(&mat2, &mat1, &this->matrix);
			this->matrix.assign(&mat2);
			mat2.dtor();
			mat1.dtor();
		}
	}


	if(Instances[ext->m_nPedIndex][ext->m_Hand].m_pAtomic){
		RpHAnimHierarchy *hier = RpSkinAtomicGetHAnimHierarchy(Instances[ext->m_nPedIndex][ext->m_Hand].m_pAtomic);
		assert(hier);
		RpHAnimHierarchySetCurrentAnimTime(hier, Instances[ext->m_nPedIndex][ext->m_Hand].m_fAnimTime);
		RpHAnimHierarchyUpdateMatrices(hier);
	}


	((CCutsceneObject*)this)->ProcessControl();
}

void CCutsceneHand::PreRender()
{
	((CEntity*)this)->PreRender();
}

void CCutsceneHand::Render()
{
	ObjectExt *ext = getExt();
	if(!Instances[ext->m_nPedIndex][ext->m_Hand].m_bHasHands)
		return;
	if(Instances[ext->m_nPedIndex][ext->m_Hand].m_pAtomic) //check it
		RpMaterialSetColor(RpGeometryGetMaterial(RpAtomicGetGeometry(Instances[ext->m_nPedIndex][ext->m_Hand].m_pAtomic), 0), (RwRGBA *)&Instances[ext->m_nPedIndex][ext->m_Hand].m_Color);

	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(ext->m_pObject->clump);
	if(hier)
		RpHAnimHierarchyUpdateMatrices(hier);
	// not needed...
	//int idx = RpHAnimIDGetIndex(hier, ext->m_Hand == HANDEDNESS_RIGHT ? BONE_SRhand : BONE_SLhand);
	//RwMatrix *ltm = RpHAnimHierarchyGetMatrixArray(hier);

	UpdateSkin();
	UpdateRwFrame();

	hier = RpSkinAtomicGetHAnimHierarchy(GetFirstAtomic(clump));
	assert(hier);
	RpHAnimHierarchyUpdateMatrices(hier);

	((CObject*)this)->Render();
}

RwMatrix *CCutsceneHand::GetBoneMatrix(RpClump *clump, int bone)
{
	RpHAnimHierarchy *hier = GetAnimHierarchyFromSkinClump(clump);
	if(hier){
		int idx = RpHAnimIDGetIndex(hier, bone);
	
		if(idx < 0)
			return NULL;
		else
			return &RpHAnimHierarchyGetMatrixArray(hier)[idx];
	}else{
		RwObjectAssociation assoc;
		assoc.name = (char*)ConvertBoneTag2BoneName(bone);
		assoc.out = NULL;
		CClumpModelInfo::FindFrameFromNameCB(RpClumpGetFrame(clump), &assoc);
		if(assoc.out)
			return RwFrameGetLTM(assoc.out);
		return NULL;
	}
}

void CCutsceneHand::CopyBoneMatrix(RpClump *clump, int bone, RwMatrix *out)
{
	if(GetBoneMatrix(clump, bone) != NULL)
		RwMatrixCopy(out, GetBoneMatrix(clump, bone));
}

void CCutsceneHand::UpdateSkin()
{
	ObjectExt *ext = getExt();
	if(!ext->m_bIsSkinned){
		RwMatrixSetIdentity(&matrix.matrix);

		CMatrix mat1, mat2;
		mat1.ctor(RwFrameGetLTM(ext->m_pHandFrame), false);
		CMatrix::mult(&mat2, &mat1, &this->matrix);
		this->matrix.assign(&mat2);
		mat2.dtor();
		mat1.dtor();
	}else{
		RwMatrixSetIdentity(&matrix.matrix);

		RwMatrix *b = GetBoneMatrix(ext->m_pObject->clump, ext->m_Hand == HANDEDNESS_RIGHT ? BONE_SRhand : BONE_SLhand );
		if( b != NULL){
			CMatrix mat1, mat2;
			mat1.ctor(b, false);
			CMatrix::mult(&mat2, &mat1, &this->matrix);
			this->matrix.assign(&mat2);
			mat2.dtor();
			mat1.dtor();
		}
	}

	RwV3d pos;
	RwMatrix mat;

	pos.x = Instances[ext->m_nPedIndex][ext->m_Hand].m_vecPos.x;
	pos.y = Instances[ext->m_nPedIndex][ext->m_Hand].m_vecPos.y;
	pos.z = Instances[ext->m_nPedIndex][ext->m_Hand].m_vecPos.z;

	CopyBoneMatrix(ext->m_pObject->clump, ext->m_Hand == HANDEDNESS_RIGHT ? BONE_Slowerarmr : BONE_Slowerarml, &mat);

	mat.pos.x = 0.0f; mat.pos.y = 0.0f; mat.pos.z = 0.0f;

	RwMatrixUpdate(&mat);

	RwV3dTransformVectors(&pos, &pos, 1, &mat);

	RwMatrixTranslate(&RpClumpGetFrame(clump)->modelling, &pos, rwCOMBINEPOSTCONCAT);

	RwMatrixScale(&RpClumpGetFrame(clump)->modelling, (RwV3d *)&Instances[ext->m_nPedIndex][ext->m_Hand].m_vecScale, rwCOMBINEPRECONCAT);

	this->matrix.Update();

	if(Instances[ext->m_nPedIndex][ext->m_Hand].m_pAtomic)
		RpHAnimHierarchyUpdateMatrices(RpSkinAtomicGetHAnimHierarchy(Instances[ext->m_nPedIndex][ext->m_Hand].m_pAtomic));
}

void CCutsceneHand::SetSkinnedModel(int pedIndex, Handedness hand, Gender gender, Race race, Stature stature, Prop prop)
{
	if ( clump
		&& Instances[pedIndex][hand].m_Gender == gender
		&& Instances[pedIndex][hand].m_Race == race
		&& Instances[pedIndex][hand].m_Prop == prop
		&& Instances[pedIndex][hand].m_Stature == stature )
	{
		return;
	}


	Instances[pedIndex][hand].m_Gender = gender;
	Instances[pedIndex][hand].m_Race = race;
	Instances[pedIndex][hand].m_Prop = prop;
	Instances[pedIndex][hand].m_Stature = stature;

	if ( clump )
		((void (__thiscall *)(CCutsceneHand*))(*(void ***)this)[6])(this);

	if ( prop == PROP_STICK ){
		assert(stickDefs[prop][hand].clump);
		clump = RpClumpClone(stickDefs[prop][hand].clump);
	}else{
		assert(handDefs[gender][race][hand].clump);
		clump = RpClumpClone(handDefs[gender][race][hand].clump);
	}

	if ( RwObjectGetType(clump) == rpCLUMP )
	{
		RwFrame *frame = RpClumpGetFrame(clump);
		RwMatrixSetIdentity(&frame->modelling);
		matrix.AttachRW(&frame->modelling, false);
	}

	AtomicByNameData atomicData;
	atomicData.atomic = NULL;
	atomicData.name[0] = '0';

	char *atomicName;
	if ( prop == PROP_STICK )
		atomicName = stickDefs[prop][hand].atomicName;
	else
		atomicName = handDefs[gender][race][hand].atomicName;

	strcpy(atomicData.name, atomicName);

	RpClumpForAllAtomics(clump, FindHandAtomicByName, &atomicData);

	Instances[pedIndex][hand].m_pAtomic = atomicData.atomic;

	InitialiseHandAnimation();
}

RpAtomic *CCutsceneHand::FindHandAtomicByName(RpAtomic *atomic, void *data)
{
	const char *nodeName = GetFrameNodeName(RpAtomicGetFrame(atomic));

	if ( !strcmp(((AtomicByNameData *)data)->name, nodeName) )
	{
		((AtomicByNameData *)data)->atomic = atomic;
		return NULL;
	}
	return atomic;
}

void CCutsceneHand::InitialiseHandAnimation()
{
	ObjectExt *ext = getExt();
	RpHAnimHierarchy *hier = RpHAnimFrameGetHierarchy(RpAtomicGetFrame(Instances[ext->m_nPedIndex][ext->m_Hand].m_pAtomic));
	RpSkinAtomicSetHAnimHierarchy(Instances[ext->m_nPedIndex][ext->m_Hand].m_pAtomic, hier);
	RpHAnimHierarchySetCurrentAnim(hier, handAnimDefs[Instances[ext->m_nPedIndex][ext->m_Hand].m_Gender][ext->m_Hand].anim);
	RpHAnimHierarchySetCurrentAnimTime(hier, 0.0f);
}

void CCutsceneHand::ConfigureForCharacter(int id)
{
	XMLConfigure conf(*g_element, id);
	conf.Evaluate();

	HandState &hs = conf.GetHand((Handedness)getExt()->m_Hand);

	SetColour(hs);
	SetAnimationTime(hs);
	SetModel(hs);
	SetScale(hs);
	SetPosition(hs);
}

void CCutsceneHand::SetColour(HandState &hs)
{
	Instances[getExt()->m_nPedIndex][getExt()->m_Hand].m_Color = hs.m_Color;
}

void CCutsceneHand::SetAnimationTime(HandState &hs)
{
	ObjectExt *ext = getExt();
	if(hs.m_bIsAnimStatic)
		Instances[ext->m_nPedIndex][ext->m_Hand].m_fAnimTime = hs.m_fAnimTime;
	else if(!CTimer__m_CodePause)
		Instances[ext->m_nPedIndex][ext->m_Hand].m_fAnimTime = CCutsceneMgr__GetCutsceneTimeInMilleseconds() * 0.001f;
}

void CCutsceneHand::SetModel(HandState &hs)
{
	ObjectExt *ext = getExt();
	if(ext->m_bIsSkinned){
		Instances[ext->m_nPedIndex][ext->m_Hand].m_bHasHands = !hs.m_Model.empty();

		if(ext->m_pObject){
			if(ext->m_Hand == HANDEDNESS_LEFT)
				ext->m_pObject->SetRenderLeftHand(Instances[ext->m_nPedIndex][ext->m_Hand].m_bHasHands == false);
			else if(ext->m_Hand == HANDEDNESS_RIGHT )
				ext->m_pObject->SetRenderRightHand(Instances[ext->m_nPedIndex][ext->m_Hand].m_bHasHands == false);
		}

		if(Instances[ext->m_nPedIndex][ext->m_Hand].m_bHasHands){
			hs.m_Prop = (Prop)!stricmp(hs.m_Model.c_str(), XMLConfigure::HAND_MODEL_STICK);
			SetSkinnedModel(ext->m_nPedIndex, (Handedness)ext->m_Hand, hs.m_Gender, hs.m_Race, hs.m_Stature, hs.m_Prop);
		}
	}else{
		if(ext->m_pObject){
			if(ext->m_Hand == HANDEDNESS_LEFT)
				ext->m_pObject->SetRenderLeftHand(Instances[ext->m_nPedIndex][ext->m_Hand].m_bHasHands == false);
			else if(ext->m_Hand == HANDEDNESS_RIGHT)
				ext->m_pObject->SetRenderRightHand(Instances[ext->m_nPedIndex][ext->m_Hand].m_bHasHands == false);
		}
	}
}

void CCutsceneHand::SetScale(HandState &hs)
{
	Instances[getExt()->m_nPedIndex][getExt()->m_Hand].m_vecScale = hs.m_vecScale;
}

void CCutsceneHand::SetPosition(HandState &hs)
{
	Instances[getExt()->m_nPedIndex][getExt()->m_Hand].m_vecPos = hs.m_vecPos;
}

int CCutsceneHand::GetModelIndexFromName(const std::string &name)
{
	for ( int p = 0; p < MAX_CUTSCENEPEDS; p++ )
	{
		if ( !stricmp(Instances[p][HANDEDNESS_LEFT].m_aName, name.c_str())
			&& !stricmp(Instances[p][HANDEDNESS_RIGHT].m_aName, name.c_str()) )
		{
			return p;
		}

	}

	return -1;
}

bool CCutsceneHand::IsCutsceneRunning(const std::string &name)
{
	if ( !stricmp(CCutsceneMgr__ms_cutsceneName, name.c_str()) )
		return true;

	if ( CTheScripts__pActiveScripts )
	{
		for ( CRunningScript *sc = CTheScripts__pActiveScripts; sc; sc = sc->m_pNext )
		{
			if ( !stricmp(sc->m_szScriptName, name.c_str()) )
				return true;
		}
	}

	return false;
}

/*
static void __declspec(naked) LoadCutsceneData_hook()
{
	__asm
	{
		pushad
		call CCutsceneHand::Init
		popad
	}

	EAXJMP(0x479100);
}
*/

__declspec(naked) char * cutscene_strcpy(char * d, const char * s)
{
	EAXJMP(0x5A0920);
}

static char * LoadCutsceneData_hook(char * d, const char * s)
{
	char *r = cutscene_strcpy(d, s);
	
	CCutsceneHand::Init();
	
	return r;
}

static void __declspec(naked) DeleteCutsceneData_hook()
{
	__asm
	{
		pushad
		call CCutsceneHand::CleanUp
		popad
	}

	EAXJMP(0x4A1150);
}

void _AddCutsceneHand(CObject *object, int mi)
{
	CCutsceneHand *hand1 = (CCutsceneHand *)object_nw(0x198);
	if(hand1)
		hand1->hand_ctor(object, CCutsceneHand::HANDEDNESS_RIGHT);
	else
		hand1 = NULL;
	assert(hand1);
	((void (__thiscall *)(CCutsceneHand*, unsigned int))(*(void ***)hand1)[3])(hand1, mi);
	CWorld__Add(hand1);
	CCutsceneMgr__ms_pCutsceneObjects[CCutsceneMgr__ms_numCutsceneObjs++] = hand1;


	CCutsceneHand *hand2 = (CCutsceneHand *)object_nw(0x198);
	if(hand2)
		hand2->hand_ctor(object, CCutsceneHand::HANDEDNESS_LEFT);
	else
		hand2 = NULL;
	assert(hand2);
	((void (__thiscall *)(CCutsceneHand*, unsigned int))(*(void ***)hand2)[3])(hand2, mi);
	CWorld__Add(hand2);
	CCutsceneMgr__ms_pCutsceneObjects[CCutsceneMgr__ms_numCutsceneObjs++] = hand2;
}

CObject *AddCutsceneHead_hook(CObject *object, int mi)
{
	auto r = AddCutsceneHead(object, mi);
	if(useCutsceneHands)
		_AddCutsceneHand(object, mi);
	return r;
}


static RpAtomic*
GetAtomicByNameCB(RpAtomic *atomic, void *data)
{
	AtomicByNameData *info = (AtomicByNameData*)data;
	const char *nodeName = GetFrameNodeName(RpAtomicGetFrame(atomic));
	if(strcmpi(nodeName, info->name) == 0){
		info->atomic = atomic;
		return NULL;
	}
	return atomic;
}

RpAtomic*
GetAtomicByName(RpClump *clump, char *name)
{
	AtomicByNameData info;
	strcpy(info.name, name);
	info.atomic = NULL;
	RpClumpForAllAtomics(clump, GetAtomicByNameCB, &info);
	return info.atomic;
}

void
ToggleObjectRendering(char *name, char *atmname)
{
	CBaseModelInfo *info = CModelInfo__GetModelInfo(name, NULL);
	if(info){
		RpClump *clump = ((RpClump *(__thiscall *)(CBaseModelInfo *))(*(void ***)info)[5])(info);
		if(clump){
			RpAtomic *atomic = GetAtomicByName(clump, atmname);
			if(atomic)
				RpAtomicSetFlags(atomic, RpAtomicGetFlags(atomic) ^ rpATOMICRENDER);
		}
	}
}

void _hideObjs()
{
	if(!useCutsceneHands)
		return;

	// Hiding by name because atomic indices aren't reliable
	if(CCutsceneMgr__ms_cutsceneProcessing){
		if(CCutsceneHand::IsCutsceneRunning("D4_GTA2"))
			ToggleObjectRendering("d4props", "stick");

		if(CCutsceneHand::IsCutsceneRunning("A6_BAIT"))
			ToggleObjectRendering("whip", "stick");
	}
	if(CCutsceneHand::IsCutsceneRunning("A7_ETG") || CCutsceneHand::IsCutsceneRunning("A8_PS"))
		ToggleObjectRendering("whip", "stick");
}

static void __declspec(naked) hideObjsCutscene_Hook()
{
	__asm
	{
		call _hideObjs
		retn
	}
}

void handhooks(void)
{
	//InjectHook(0x4046BF, LoadCutsceneData_hook, PATCH_CALL); //fucked up with modloader
	InjectHook(0x4046AE, LoadCutsceneData_hook, PATCH_CALL);
	InjectHook(0x4049A5, DeleteCutsceneData_hook, PATCH_CALL);
	InjectHook(0x447505, AddCutsceneHead_hook, PATCH_CALL);

	InjectHook(0x4051A2, hideObjsCutscene_Hook, PATCH_JUMP);
	InjectHook(0x4048D3, hideObjsCutscene_Hook, PATCH_JUMP);
}
