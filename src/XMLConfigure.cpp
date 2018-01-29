#include "XMLConfigure.h"
#include <algorithm>

const char *XMLConfigure::XML_ATTRIBUTE_ID = "ID";
const char *XMLConfigure::XML_ATTRIBUTE_NAME = "NAME";
const char *XMLConfigure::XML_ATTRIBUTE_SIDE = "SIDE";
const char *XMLConfigure::XML_ATTRIBUTE_LEFT = "LEFT";
const char *XMLConfigure::XML_ATTRIBUTE_RIGHT = "RIGHT";
const char *XMLConfigure::XML_ATTRIBUTE_BOTH = "BOTH";
const char *XMLConfigure::XML_ATTRIBUTE_MODEL = "MODEL";
const char *XMLConfigure::XML_ATTRIBUTE_GENDER = "GENDER";
const char *XMLConfigure::XML_ATTRIBUTE_MALE = "MALE";
const char *XMLConfigure::XML_ATTRIBUTE_FEMALE = "FEMALE";
const char *XMLConfigure::XML_ATTRIBUTE_RACE = "RACE";
const char *XMLConfigure::XML_ATTRIBUTE_BLACK = "BLACK";
const char *XMLConfigure::XML_ATTRIBUTE_WHITE = "WHITE";
const char *XMLConfigure::XML_ATTRIBUTE_SCALE_X = "SCALEX";
const char *XMLConfigure::XML_ATTRIBUTE_SCALE_Y = "SCALEY";
const char *XMLConfigure::XML_ATTRIBUTE_SCALE_Z = "SCALEZ";
const char *XMLConfigure::XML_ATTRIBUTE_ROTATE_X = "ROTATEX";
const char *XMLConfigure::XML_ATTRIBUTE_ROTATE_Y = "ROTATEY";
const char *XMLConfigure::XML_ATTRIBUTE_ROTATE_Z = "ROTATEZ";
const char *XMLConfigure::XML_ATTRIBUTE_POSITION_X = "POSITIONX";
const char *XMLConfigure::XML_ATTRIBUTE_POSITION_Y = "POSITIONY";
const char *XMLConfigure::XML_ATTRIBUTE_POSITION_Z = "POSITIONZ";
const char *XMLConfigure::XML_ATTRIBUTE_RED = "RED";
const char *XMLConfigure::XML_ATTRIBUTE_GREEN = "GREEN";
const char *XMLConfigure::XML_ATTRIBUTE_BLUE = "BLUE";
const char *XMLConfigure::XML_ATTRIBUTE_CUTSCENE_START = "CUTSCENE_START";
const char *XMLConfigure::XML_ATTRIBUTE_CUTSCENE_END = "CUTSCENE_END";
const char *XMLConfigure::XML_ATTRIBUTE_ANIMATION_START = "START";
const char *XMLConfigure::XML_ATTRIBUTE_ANIMATION_END = "END";
const char *XMLConfigure::XML_ATTRIBUTE_ANIMATION_TIME = "TIME";
const char *XMLConfigure::XML_ATTRIBUTE_ANIMATION_TYPE = "TYPE";
const char *XMLConfigure::XML_ATTRIBUTE_ANIMATION_STATIC = "STATIC";
const char *XMLConfigure::XML_ATTRIBUTE_ANIMATION_LOOP = "LOOP";
const char *XMLConfigure::XML_TAG_ROOT = "GTA_XML";
const char *XMLConfigure::XML_TAG_CUTSCENE = "CUTSCENE";
const char *XMLConfigure::XML_TAG_CHARACTER = "CHARACTER";
const char *XMLConfigure::XML_TAG_HAND = "HAND";
const char *XMLConfigure::XML_TAG_ANIMATION = "ANIMATION";
const char *XMLConfigure::HAND_MODEL_ORIGINAL = "NON-SKINNED";
const char *XMLConfigure::HAND_MODEL_STICK = "SKINNED-STICK";


void HandState::Init()
{
	m_Model = "";

	dword4 = 0;

	m_fAnimStart = 0.0f;
	m_fAnimEnd = 0.0f;
	m_fAnimTime = 0.0f;
	m_bIsAnimStatic = false;
	m_fAnimCutsceneStart = 0.0f;
	m_fAnimCutsceneEnd = 0.0f;

	dword20 = 0;

	m_vecScale.x = 1.0f; m_vecScale.y = 1.0f; m_vecScale.z = 1.0f;

	m_vecRotate.x = 0.0f; m_vecRotate.y = 0.0f; m_vecRotate.z = 0.0f;

	m_vecPos.x = 0.0f; m_vecPos.y = 0.0f; m_vecPos.z = 0.0f;

	m_Color.r = 255; m_Color.g = 255; m_Color.b = 255; m_Color.a = 255;

	m_Gender = CCutsceneHand::HANDEDNESS_MALE;
	m_Race = CCutsceneHand::RACE_WHITE;

	m_Stature = CCutsceneHand::STATURE_1;
	m_Prop = CCutsceneHand::PROP_NONE;
}

HandState &HandState::operator=(const HandState &right)
{
	m_Model = right.m_Model;
	dword4 = right.dword4;
	m_fAnimStart = right.m_fAnimStart;
	m_fAnimEnd = right.m_fAnimEnd;
	m_fAnimTime = right.m_fAnimTime;
	m_bIsAnimStatic = right.m_bIsAnimStatic;
	m_fAnimCutsceneStart = right.m_fAnimCutsceneStart;
	m_fAnimCutsceneEnd = right.m_fAnimCutsceneEnd;
	dword20 = right.dword20;
	m_vecScale.x = right.m_vecScale.x;
	m_vecScale.y = right.m_vecScale.y;
	m_vecScale.z = right.m_vecScale.z;
	m_vecRotate.x = right.m_vecRotate.x;
	m_vecRotate.y = right.m_vecRotate.y;
	m_vecRotate.z = right.m_vecRotate.z;
	m_vecPos.x = right.m_vecPos.x;
	m_vecPos.y = right.m_vecPos.y;
	m_vecPos.z = right.m_vecPos.z;
	m_Color.r = right.m_Color.r;
	m_Color.g = right.m_Color.g;
	m_Color.b = right.m_Color.b;
	m_Color.a = right.m_Color.a;
	m_Race = right.m_Race;
	m_Gender = right.m_Gender;
	m_Prop = right.m_Prop;
	m_Stature = right.m_Stature;
	return *this;
}

bool XMLConfigure::IsCutsceneRunning(const std::string &name)
{
	return CCutsceneHand::IsCutsceneRunning(name);
}

void XMLConfigure::Init()
{
	m_CutsceneName = "";
	m_HandSide = CCutsceneHand::HANDEDNESS_BOTH;
	m_curElemet = m_xmlRoot;

	_handstate18.Init();
	_handstate74.Init();
	m_RightHand.Init();
	m_LeftHand.Init();
}

extern int CCutsceneMgr__GetCutsceneTimeInMilleseconds();
XMLConfigure::XMLConfigure(XmlLibrary::Element &root, int player)
{
	m_xmlRoot = &root;
	m_fCutsceneTime = CCutsceneMgr__GetCutsceneTimeInMilleseconds();
	m_nModelIndex = player;

	_handstate18.Init();
	_handstate74.Init();
	m_RightHand.Init();
	m_LeftHand.Init();

	Init();
}


void XMLConfigure::Animation()
{
	const XmlLibrary::AttributeVector *attributes = m_curElemet->GetAttributes();

	AnimationType(attributes);
	AnimationTime(attributes);
	AnimationCutsceneStart(attributes);
	AnimationCutsceneEnd(attributes);
	AnimationStart(attributes);
	AnimationEnd(attributes);
}

void XMLConfigure::Cutscene()
{
	const XmlLibrary::AttributeVector *attributes = m_curElemet->GetAttributes();

	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsName);

		if ( it != attributes->end() )
		{
			m_CutsceneName = (*it)->Value();
		}
	}

	if ( m_CutsceneName.empty() || IsCutsceneRunning(m_CutsceneName) )
		ProcessChildren();
}

void XMLConfigure::Process()
{
	if ( m_curElemet && !m_curElemet->IsNull() )
	{
		if ( !stricmp(m_curElemet->GetName().c_str(), XML_TAG_ROOT) )
		{
			ProcessChildren();
		}
		else if ( !stricmp(m_curElemet->GetName().c_str(), XML_TAG_CUTSCENE) )
		{
			Cutscene();
		}
		else if ( !stricmp(m_curElemet->GetName().c_str(), XML_TAG_CHARACTER) )
		{
			Character();
		}
		else if ( !stricmp(m_curElemet->GetName().c_str(), XML_TAG_HAND) )
		{
			Hand();
		}
		else if ( !stricmp(m_curElemet->GetName().c_str(), XML_TAG_ANIMATION) )
		{
			Animation();
		}
	}
}

void XMLConfigure::ProcessChildren()
{
	const XmlLibrary::ElementVector *childs = m_curElemet->GetChilds();

	if ( childs )
	{
		for ( XmlLibrary::ElementVector::const_iterator it = childs->begin(); it != childs->end(); ++it )
		{
			m_curElemet = *it;
			Process();
		}
	}

}

void XMLConfigure::Hand()
{
	const XmlLibrary::AttributeVector *attributes = m_curElemet->GetAttributes();

	CCutsceneHand::Handedness savedHandSide;
	HandState saved18;
	HandState saved74;

	savedHandSide = m_HandSide;

	saved18 = _handstate18;
	saved74 = _handstate74;

	HandSide(attributes);
	HandModel(attributes);
	HandGender(attributes);
	HandRace(attributes);
	HandScaleX(attributes);
	HandScaleY(attributes);
	HandScaleZ(attributes);
	HandRotateX(attributes);
	HandRotateY(attributes);
	HandRotateZ(attributes);
	HandPositionX(attributes);
	HandPositionY(attributes);
	HandPositionZ(attributes);
	HandColourRed(attributes);
	HandColourGreen(attributes);
	HandColourBlue(attributes);

	ProcessChildren();

	m_HandSide = savedHandSide;

	_handstate18 = saved18;
	_handstate74 = saved74;
}

void XMLConfigure::Character()
{
	const XmlLibrary::AttributeVector *attributes = m_curElemet->GetAttributes();

	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsName);

		if ( it != attributes->end() )
		{
			int mi = CCutsceneHand::GetModelIndexFromName((*it)->Value().c_str());

			if ( mi != -1 && mi != -3 && mi != m_nModelIndex )
				return;
		}

	}

	ProcessChildren();
}

void XMLConfigure::Evaluate()
{
	Init();
	Process();
}

HandState &XMLConfigure::GetHand(CCutsceneHand::Handedness hand)
{
	if ( hand == CCutsceneHand::HANDEDNESS_RIGHT )
		return m_RightHand;
	else
		return m_LeftHand;
}


void XMLConfigure::HandSide(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsSide);

		if ( it != attributes->end() )
		{
			if ( !stricmp((*it)->Value().c_str(), XML_ATTRIBUTE_RIGHT) )
				m_HandSide = CCutsceneHand::HANDEDNESS_RIGHT;
			else if ( !stricmp((*it)->Value().c_str(), XML_ATTRIBUTE_LEFT) )
				m_HandSide = CCutsceneHand::HANDEDNESS_LEFT;
			else if ( !stricmp((*it)->Value().c_str(), XML_ATTRIBUTE_BOTH) )
				m_HandSide = CCutsceneHand::HANDEDNESS_BOTH;
			else
				m_HandSide = CCutsceneHand::HANDEDNESS_UNDEFINED;
		}
	}
}

void XMLConfigure::HandModel(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsModel);

		if ( it != attributes->end() )
		{
			std::string modelname = (*it)->Value();
			if ( !stricmp((*it)->Value().c_str(), HAND_MODEL_ORIGINAL) )
				modelname = "";

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_Model = modelname;
				m_RightHand.m_Model = modelname;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_Model = modelname;
				m_LeftHand.m_Model = modelname;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_Model = modelname;
				m_RightHand.m_Model = modelname;

				_handstate74.m_Model = modelname;
				m_LeftHand.m_Model = modelname;
			}
		}
	}
}

void XMLConfigure::HandGender(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsGender);

		if ( it != attributes->end() )
		{
			CCutsceneHand::Gender gender = CCutsceneHand::HANDEDNESS_MALE;

			if ( !stricmp((*it)->Value().c_str(), XML_ATTRIBUTE_MALE) )
				gender = CCutsceneHand::HANDEDNESS_MALE;
			else if ( !stricmp((*it)->Value().c_str(), XML_ATTRIBUTE_FEMALE) )
				gender = CCutsceneHand::HANDEDNESS_FEMALE;

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_Gender = gender;
				m_RightHand.m_Gender = gender;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_Gender = gender;
				m_LeftHand.m_Gender = gender;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_Gender = gender;
				m_RightHand.m_Gender = gender;

				_handstate74.m_Gender = gender;
				m_LeftHand.m_Gender = gender;
			}
		}
	}
}

void XMLConfigure::HandRace(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsRace);

		if ( it != attributes->end() )
		{
			CCutsceneHand::Race race = CCutsceneHand::RACE_WHITE;

			if ( !stricmp((*it)->Value().c_str(), XML_ATTRIBUTE_BLACK) )
				race = CCutsceneHand::RACE_BLACK;
			else if ( !stricmp((*it)->Value().c_str(), XML_ATTRIBUTE_WHITE) )
				race = CCutsceneHand::RACE_WHITE;

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_Race = race;
				m_RightHand.m_Race = race;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_Race = race;
				m_LeftHand.m_Race = race;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_Race = race;
				m_RightHand.m_Race = race;

				_handstate74.m_Race = race;
				m_LeftHand.m_Race = race;
			}
		}
	}
}

void XMLConfigure::HandScaleX(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsScaleX);

		if ( it != attributes->end() )
		{
			float val;
			sscanf((*it)->Value().c_str(), "%f", &val);

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_vecScale.x = val;
				m_RightHand.m_vecScale.x = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_vecScale.x = val;
				m_LeftHand.m_vecScale.x = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_vecScale.x = val;
				m_RightHand.m_vecScale.x = val;

				_handstate74.m_vecScale.x = val;
				m_LeftHand.m_vecScale.x = val;
			}
		}
	}
}

void XMLConfigure::HandScaleY(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsScaleY);

		if ( it != attributes->end() )
		{
			float val;
			sscanf((*it)->Value().c_str(), "%f", &val);

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_vecScale.y = val;
				m_RightHand.m_vecScale.y = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_vecScale.y = val;
				m_LeftHand.m_vecScale.y = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_vecScale.y = val;
				m_RightHand.m_vecScale.y = val;

				_handstate74.m_vecScale.y = val;
				m_LeftHand.m_vecScale.y = val;
			}
		}
	}
}

void XMLConfigure::HandScaleZ(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsScaleZ);

		if ( it != attributes->end() )
		{
			float val;
			sscanf((*it)->Value().c_str(), "%f", &val);

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_vecScale.z = val;
				m_RightHand.m_vecScale.z = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_vecScale.z = val;
				m_LeftHand.m_vecScale.z = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_vecScale.z = val;
				m_RightHand.m_vecScale.z = val;

				_handstate74.m_vecScale.z = val;
				m_LeftHand.m_vecScale.z = val;
			}
		}
	}
}

void XMLConfigure::HandRotateX(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsRotateX);

		if ( it != attributes->end() )
		{
			float val;
			sscanf((*it)->Value().c_str(), "%f", &val);

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_vecRotate.x = val;
				m_RightHand.m_vecRotate.x = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_vecRotate.x = val;
				m_LeftHand.m_vecRotate.x = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_vecRotate.x = val;
				m_RightHand.m_vecRotate.x = val;

				_handstate74.m_vecRotate.x = val;
				m_LeftHand.m_vecRotate.x = val;
			}
		}
	}
}

void XMLConfigure::HandRotateY(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsRotateY);

		if ( it != attributes->end() )
		{
			float val;
			sscanf((*it)->Value().c_str(), "%f", &val);

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_vecRotate.y = val;
				m_RightHand.m_vecRotate.y = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_vecRotate.y = val;
				m_LeftHand.m_vecRotate.y = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_vecRotate.y = val;
				m_RightHand.m_vecRotate.y = val;

				_handstate74.m_vecRotate.y = val;
				m_LeftHand.m_vecRotate.y = val;
			}
		}
	}
}

void XMLConfigure::HandRotateZ(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsRotateZ);

		if ( it != attributes->end() )
		{
			float val;
			sscanf((*it)->Value().c_str(), "%f", &val);

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_vecRotate.z = val;
				m_RightHand.m_vecRotate.z = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_vecRotate.z = val;
				m_LeftHand.m_vecRotate.z = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_vecRotate.z = val;
				m_RightHand.m_vecRotate.z = val;

				_handstate74.m_vecRotate.z = val;
				m_LeftHand.m_vecRotate.z = val;
			}
		}
	}
}

void XMLConfigure::HandPositionX(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsPositionX);

		if ( it != attributes->end() )
		{
			float val;
			sscanf((*it)->Value().c_str(), "%f", &val);

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_vecPos.x = val;
				m_RightHand.m_vecPos.x = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_vecPos.x = val;
				m_LeftHand.m_vecPos.x = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_vecPos.x = val;
				m_RightHand.m_vecPos.x = val;

				_handstate74.m_vecPos.x = val;
				m_LeftHand.m_vecPos.x = val;
			}
		}
	}
}

void XMLConfigure::HandPositionY(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsPositionY);

		if ( it != attributes->end() )
		{
			float val;
			sscanf((*it)->Value().c_str(), "%f", &val);

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_vecPos.y = val;
				m_RightHand.m_vecPos.y = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_vecPos.y = val;
				m_LeftHand.m_vecPos.y = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_vecPos.y = val;
				m_RightHand.m_vecPos.y = val;

				_handstate74.m_vecPos.y = val;
				m_LeftHand.m_vecPos.y = val;
			}
		}
	}
}

void XMLConfigure::HandPositionZ(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsPositionZ);

		if ( it != attributes->end() )
		{
			float val;
			sscanf((*it)->Value().c_str(), "%f", &val);

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_vecPos.z = val;
				m_RightHand.m_vecPos.z = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_vecPos.z = val;
				m_LeftHand.m_vecPos.z = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_vecPos.z = val;
				m_RightHand.m_vecPos.z = val;

				_handstate74.m_vecPos.z = val;
				m_LeftHand.m_vecPos.z = val;
			}
		}
	}
}

void XMLConfigure::HandColourRed(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsRed);

		if ( it != attributes->end() )
		{
			int val;
			sscanf((*it)->Value().c_str(), "%d", &val);

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_Color.r = val;
				m_RightHand.m_Color.r = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_Color.r = val;
				m_LeftHand.m_Color.r = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_Color.r = val;
				m_RightHand.m_Color.r = val;

				_handstate74.m_Color.r = val;
				m_LeftHand.m_Color.r = val;
			}
		}
	}
}

void XMLConfigure::HandColourGreen(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsGreen);

		if ( it != attributes->end() )
		{
			int val;
			sscanf((*it)->Value().c_str(), "%d", &val);

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_Color.g = val;
				m_RightHand.m_Color.g = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_Color.g = val;
				m_LeftHand.m_Color.g = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_Color.g = val;
				m_RightHand.m_Color.g = val;

				_handstate74.m_Color.g = val;
				m_LeftHand.m_Color.g = val;
			}
		}
	}
}

void XMLConfigure::HandColourBlue(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsBlue);

		if ( it != attributes->end() )
		{
			int val;
			sscanf((*it)->Value().c_str(), "%d", &val);

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_Color.b = val;
				m_RightHand.m_Color.b = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_Color.b = val;
				m_LeftHand.m_Color.b = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_Color.b = val;
				m_RightHand.m_Color.b = val;

				_handstate74.m_Color.b = val;
				m_LeftHand.m_Color.b = val;
			}
		}
	}
}


void XMLConfigure::AnimationCutsceneStart(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsCutsceneStart);

		if ( it != attributes->end() )
		{
			float val;
			sscanf((*it)->Value().c_str(), "%f", &val);

			val *= 100.0f;

			if ( m_fCutsceneTime > val )
			{
				if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
				{
					_handstate18.m_fAnimCutsceneStart = val;
					m_RightHand.m_fAnimCutsceneStart = val;
				}
				else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
				{
					_handstate74.m_fAnimCutsceneStart = val;
					m_LeftHand.m_fAnimCutsceneStart = val;
				}
				else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
				{
					_handstate18.m_fAnimCutsceneStart = val;
					m_RightHand.m_fAnimCutsceneStart = val;

					_handstate74.m_fAnimCutsceneStart = val;
					m_LeftHand.m_fAnimCutsceneStart = val;
				}
			}
		}
	}
}

void XMLConfigure::AnimationCutsceneEnd(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsCutsceneEnd);

		if ( it != attributes->end() )
		{
			float val;
			sscanf((*it)->Value().c_str(), "%f", &val);

			val *= 100.0f;

			if ( m_fCutsceneTime > val )
			{
				if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
				{
					_handstate18.m_fAnimCutsceneEnd = val;
					m_RightHand.m_fAnimCutsceneEnd = val;
				}
				else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
				{
					_handstate74.m_fAnimCutsceneEnd = val;
					m_LeftHand.m_fAnimCutsceneEnd = val;
				}
				else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
				{
					_handstate18.m_fAnimCutsceneEnd = val;
					m_RightHand.m_fAnimCutsceneEnd = val;

					_handstate74.m_fAnimCutsceneEnd = val;
					m_LeftHand.m_fAnimCutsceneEnd = val;
				}
			}
		}
	}
}

void XMLConfigure::AnimationStart(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsAnimationStart);

		if ( it != attributes->end() )
		{
			float val;
			sscanf((*it)->Value().c_str(), "%f", &val);

			val *= 100.0f;

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_fAnimStart = val;
				m_RightHand.m_fAnimStart = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_fAnimStart = val;
				m_LeftHand.m_fAnimStart = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_fAnimStart = val;
				m_RightHand.m_fAnimStart = val;

				_handstate74.m_fAnimStart = val;
				m_LeftHand.m_fAnimStart = val;
			}
		}
	}
}

void XMLConfigure::AnimationEnd(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsAnimationEnd);

		if ( it != attributes->end() )
		{
			float val;
			sscanf((*it)->Value().c_str(), "%f", &val);

			val *= 100.0f;

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_fAnimEnd = val;
				m_RightHand.m_fAnimEnd = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_fAnimEnd = val;
				m_LeftHand.m_fAnimEnd = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_fAnimEnd = val;
				m_RightHand.m_fAnimEnd = val;

				_handstate74.m_fAnimEnd = val;
				m_LeftHand.m_fAnimEnd = val;
			}
		}
	}
}

void XMLConfigure::AnimationType(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsAnimationType);

		if ( it != attributes->end() )
		{
			bool bIsStatic = false;

			if ( !stricmp((*it)->Value().c_str(), XML_ATTRIBUTE_ANIMATION_STATIC) )
				bIsStatic = true;
			else if ( !stricmp((*it)->Value().c_str(), XML_ATTRIBUTE_ANIMATION_LOOP) )
				bIsStatic = false;

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_bIsAnimStatic = bIsStatic;
				m_RightHand.m_bIsAnimStatic = bIsStatic;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_bIsAnimStatic = bIsStatic;
				m_LeftHand.m_bIsAnimStatic = bIsStatic;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_bIsAnimStatic = bIsStatic;
				m_RightHand.m_bIsAnimStatic = bIsStatic;

				_handstate74.m_bIsAnimStatic = bIsStatic;
				m_LeftHand.m_bIsAnimStatic = bIsStatic;
			}
		}
	}
}

void XMLConfigure::AnimationTime(const XmlLibrary::AttributeVector *attributes)
{
	if ( attributes )
	{
		XmlLibrary::AttributeVector::const_iterator it = std::find_if(attributes->begin(), attributes->end(), AttributeIsAnimationTime);

		if ( it != attributes->end() )
		{
			float val;
			sscanf((*it)->Value().c_str(), "%f", &val);

			if ( m_HandSide == CCutsceneHand::HANDEDNESS_RIGHT )
			{
				_handstate18.m_fAnimTime = val;
				m_RightHand.m_fAnimTime = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_LEFT )
			{
				_handstate74.m_fAnimTime = val;
				m_LeftHand.m_fAnimTime = val;
			}
			else if ( m_HandSide == CCutsceneHand::HANDEDNESS_BOTH )
			{
				_handstate18.m_fAnimTime = val;
				m_RightHand.m_fAnimTime = val;

				_handstate74.m_fAnimTime = val;
				m_LeftHand.m_fAnimTime = val;
			}
		}
	}
}