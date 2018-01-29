#pragma once
#include <windows.h>
#include <cstring>
#include <string>
#include "xmllib/XmlLibrary.h"
#include "iii_anim.h"

class HandState
{
public:
	std::string m_Model;
	DWORD dword4;
	float m_fAnimStart;
	float m_fAnimEnd;
	float m_fAnimTime;
	bool m_bIsAnimStatic;
	float m_fAnimCutsceneStart;
	float m_fAnimCutsceneEnd;
	DWORD dword20;
	CVector m_vecScale;
	CVector m_vecRotate;
	CVector m_vecPos;
	CRGBA m_Color;
	CCutsceneHand::Race m_Race;
	CCutsceneHand::Gender m_Gender;
	CCutsceneHand::Prop m_Prop;
	CCutsceneHand::Stature m_Stature;

	void Init();

	HandState &operator=(const HandState &right);
};

class XMLConfigure
{
	XmlLibrary::Element *m_xmlRoot;
	float m_fCutsceneTime;
	int m_nModelIndex;
	std::string m_CutsceneName;
	XmlLibrary::Element *m_curElemet;
	CCutsceneHand::Handedness m_HandSide;
	HandState _handstate18;
	HandState _handstate74;
	HandState m_RightHand;
	HandState m_LeftHand;

	static const char *XMLConfigure::XML_ATTRIBUTE_ID;

	static const char *XMLConfigure::XML_ATTRIBUTE_NAME;

	static const char *XMLConfigure::XML_ATTRIBUTE_SIDE;
	static const char *XMLConfigure::XML_ATTRIBUTE_LEFT;
	static const char *XMLConfigure::XML_ATTRIBUTE_RIGHT;
	static const char *XMLConfigure::XML_ATTRIBUTE_BOTH;

	static const char *XMLConfigure::XML_ATTRIBUTE_MODEL;

	static const char *XMLConfigure::XML_ATTRIBUTE_GENDER;
	static const char *XMLConfigure::XML_ATTRIBUTE_MALE;
	static const char *XMLConfigure::XML_ATTRIBUTE_FEMALE;

	static const char *XMLConfigure::XML_ATTRIBUTE_RACE;
	static const char *XMLConfigure::XML_ATTRIBUTE_BLACK;
	static const char *XMLConfigure::XML_ATTRIBUTE_WHITE;

	static const char *XMLConfigure::XML_ATTRIBUTE_SCALE_X;
	static const char *XMLConfigure::XML_ATTRIBUTE_SCALE_Y;
	static const char *XMLConfigure::XML_ATTRIBUTE_SCALE_Z;

	static const char *XMLConfigure::XML_ATTRIBUTE_ROTATE_X;
	static const char *XMLConfigure::XML_ATTRIBUTE_ROTATE_Y;
	static const char *XMLConfigure::XML_ATTRIBUTE_ROTATE_Z;

	static const char *XMLConfigure::XML_ATTRIBUTE_POSITION_X;
	static const char *XMLConfigure::XML_ATTRIBUTE_POSITION_Y;
	static const char *XMLConfigure::XML_ATTRIBUTE_POSITION_Z;

	static const char *XMLConfigure::XML_ATTRIBUTE_RED;
	static const char *XMLConfigure::XML_ATTRIBUTE_GREEN;
	static const char *XMLConfigure::XML_ATTRIBUTE_BLUE;

	static const char *XMLConfigure::XML_ATTRIBUTE_CUTSCENE_START;
	static const char *XMLConfigure::XML_ATTRIBUTE_CUTSCENE_END;

	static const char *XMLConfigure::XML_ATTRIBUTE_ANIMATION_START;
	static const char *XMLConfigure::XML_ATTRIBUTE_ANIMATION_END;
	static const char *XMLConfigure::XML_ATTRIBUTE_ANIMATION_TIME;
	static const char *XMLConfigure::XML_ATTRIBUTE_ANIMATION_TYPE;
	static const char *XMLConfigure::XML_ATTRIBUTE_ANIMATION_STATIC;
	static const char *XMLConfigure::XML_ATTRIBUTE_ANIMATION_LOOP;

	static const char *XMLConfigure::XML_TAG_ROOT;

	static const char *XMLConfigure::XML_TAG_CUTSCENE;

	static const char *XMLConfigure::XML_TAG_CHARACTER;

	static const char *XMLConfigure::XML_TAG_HAND;

	static const char *XMLConfigure::XML_TAG_ANIMATION;

	static const char *XMLConfigure::HAND_MODEL_ORIGINAL;


	static bool AttributeIsId(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_ID);
	}

	static bool AttributeIsName(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_NAME);
	}

	static bool AttributeIsModel(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_MODEL);
	}

	static bool AttributeIsRace(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_RACE);
	}

	static bool AttributeIsGender(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_GENDER);
	}

	static bool AttributeIsSide(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_SIDE);
	}

	static bool AttributeIsScaleX(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_SCALE_X);
	}

	static bool AttributeIsScaleY(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_SCALE_Y);
	}

	static bool AttributeIsScaleZ(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_SCALE_Z);
	}

	static bool AttributeIsRotateX(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_ROTATE_X);
	}

	static bool AttributeIsRotateY(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_ROTATE_Y);
	}

	static bool AttributeIsRotateZ(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_ROTATE_Z);
	}

	static bool AttributeIsPositionX(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_POSITION_X);
	}

	static bool AttributeIsPositionY(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_POSITION_Y);
	}

	static bool AttributeIsPositionZ(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_POSITION_Z);
	}

	static bool AttributeIsRed(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_RED);
	}

	static bool AttributeIsGreen(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_GREEN);
	}

	static bool AttributeIsBlue(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_BLUE);
	}

	static bool AttributeIsCutsceneStart(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_CUTSCENE_START);
	}

	static bool AttributeIsCutsceneEnd(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_CUTSCENE_END);
	}

	static bool AttributeIsAnimationTime(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_ANIMATION_TIME);
	}

	static bool AttributeIsAnimationType(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_ANIMATION_TYPE);
	}

	static bool AttributeIsAnimationStart(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_ANIMATION_START);
	}

	static bool AttributeIsAnimationEnd(XmlLibrary::Attribute * const &attribute)
	{
		return !stricmp(attribute->Name().c_str(), XML_ATTRIBUTE_ANIMATION_END);
	}

	bool IsCutsceneRunning(std::string const &name);

	void Init();

public:
	static const char *XMLConfigure::HAND_MODEL_STICK;

	XMLConfigure(XmlLibrary::Element &root, int _player);

	void Animation();
	void Cutscene();
	void Process();
	void ProcessChildren();
	void Hand();
	void Character();
	void Evaluate();

	HandState &GetHand(CCutsceneHand::Handedness hand);

	void HandSide(const XmlLibrary::AttributeVector *attributes);
	void HandModel(const XmlLibrary::AttributeVector *attributes);
	void HandGender(const XmlLibrary::AttributeVector *attributes);
	void HandRace(const XmlLibrary::AttributeVector *attributes);
	void HandScaleX(const XmlLibrary::AttributeVector *attributes);
	void HandScaleY(const XmlLibrary::AttributeVector *attributes);
	void HandScaleZ(const XmlLibrary::AttributeVector *attributes);
	void HandRotateX(const XmlLibrary::AttributeVector *attributes);
	void HandRotateY(const XmlLibrary::AttributeVector *attributes);
	void HandRotateZ(const XmlLibrary::AttributeVector *attributes);
	void HandPositionX(const XmlLibrary::AttributeVector *attributes);
	void HandPositionY(const XmlLibrary::AttributeVector *attributes);
	void HandPositionZ(const XmlLibrary::AttributeVector *attributes);
	void HandColourRed(const XmlLibrary::AttributeVector *attributes);
	void HandColourGreen(const XmlLibrary::AttributeVector *attributes);
	void HandColourBlue(const XmlLibrary::AttributeVector *attributes);

	void AnimationCutsceneStart(const XmlLibrary::AttributeVector *attributes);
	void AnimationCutsceneEnd(const XmlLibrary::AttributeVector *attributes);
	void AnimationStart(const XmlLibrary::AttributeVector *attributes);
	void AnimationEnd(const XmlLibrary::AttributeVector *attributes);
	void AnimationType(const XmlLibrary::AttributeVector *attributes);
	void AnimationTime(const XmlLibrary::AttributeVector *attributes);

};

