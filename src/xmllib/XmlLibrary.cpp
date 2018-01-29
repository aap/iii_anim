//base code from unknown
//psksvp@ccs.neu.edu has added/modified this code.
#include <stdio.h>
#include <stdlib.h>
#include "XmlLibrary.h"


#ifndef ASSERT
#define ASSERT(expr)
#endif

#ifndef elemof
#define elemof(array) (sizeof(array) / sizeof((array)[0]))
#endif

#ifndef TEXT
#define TEXT(a) a
#endif

namespace XmlLibrary
{
  static Null s_nullElem;
  Null& Element::null = s_nullElem;


  Attribute::Attribute(const std::string& strName, const std::string& strValue)
  {
    m_strName = strName;
    m_strValue = strValue;
  }

  const std::string& Attribute::Name(void) const
  {
    return m_strName;
  }

  const std::string& Attribute::Value(void) const
  {
    return m_strValue;
  }

  //////////////////////////////////////////////////////////////////////////////////
  Value::Value(void)
  {}

  void Value::Add(const std::string& strText)
  {
    m_strValue += strText;
  }

  void Value::Add(char c)
  {
    m_strValue += c;
  }

  Value::operator const std::string&(void) const
  {
    return m_strValue;
  }
  
  Value::operator const char*(void) const
  {
    return m_strValue.c_str();
  }

  Value::operator int(void) const
  {
    return ::atoi(m_strValue.c_str());
  }

  Value::operator unsigned int(void) const
  {
    return (unsigned int)::atoi(m_strValue.c_str());
  }

  Value::operator float(void) const
  {
    return (float)::atof(m_strValue.c_str());
  }

  Value::operator double(void) const
  {
    return ::atof(m_strValue.c_str());
  }
  
  Value::operator bool(void) const
  {
    return m_strValue[0] == '1' ? true : false;
  }
  
  Value::operator char(void) const
  {
    return m_strValue[0];
  }
  
  Value::operator unsigned char(void) const
  {
    return (unsigned char)m_strValue[0];
  }
  
  Value::operator short(void) const
  {
    return (short)::atoi(m_strValue.c_str());
  }
  
  Value::operator unsigned short(void) const
  {
    return (unsigned short)::atoi(m_strValue.c_str());
  }
  
  Value::operator long(void) const
  {
    return (long)::atoi(m_strValue.c_str());
  }
  
  Value::operator unsigned long(void) const
  {
    return (unsigned long)::atoi(m_strValue.c_str());
  }

  //////////////////////////////////////////////////////////////////////////////////
  Element::Element(void)
  {}

  Element::Element(const std::string& strName)
    :m_strName(strName)
  {}

  Element::~Element(void)
  {}

  bool Element::IsNull(void) const
  {
    return(false);
  }

  void Element::AddValue(const std::string& strText)
  {
    m_value.Add(strText);
  }

  void Element::AddValue(char c)
  {
    m_value.Add(c);
  }

  const std::string& Element::GetName(void) const
  {
    return m_strName;
  }

  const Value& Element::GetValue(void) const
  {
    return m_value;
  }

  const Element& Element::operator()(const char* szName, int nIndex) const
  {
    return(GetChild(szName, nIndex));
  }

  //////////////////////////////////////////////////////////////////////////////////
  Tag::Tag(const std::string& strName)
      :Element(strName)
  {}

  Tag::~Tag(void)
  {
    for(AttributeVector::iterator itAttrib = m_attributes.begin(); itAttrib < m_attributes.end(); ++itAttrib)
      delete *itAttrib;

    for(ElementVector::iterator itElem = m_childs.begin(); itElem < m_childs.end(); ++itElem)
      delete *itElem;
  }

  bool Tag::Add(Element* pChild)
  {
    m_childs.push_back(pChild);
    return(true);
  }

  void Tag::Add(Attribute* pAttribute)
  {
    m_attributes.push_back(pAttribute);
  }

  const ElementVector* Tag::GetChilds(void) const
  {
    return(m_childs.empty() ? NULL : &m_childs);
  }

  bool Tag::FindChild(const char* szName, ElementVector::const_iterator& it) const
  {
    for(; it < m_childs.end(); ++it)
    {
      if((*it)->GetName() == szName)
        return(true);
    }

    return(false);
  }

  const Element& Tag::GetChild(const char* szName, int nIndex) const
  {
    ElementVector::const_iterator it = m_childs.begin();
    for(int nCount = 0; nCount < nIndex; ++nCount)
    {
      if(!FindChild(szName, it))
        return(null);
      ++it;
    }

    if(!FindChild(szName, it))
      return(null);
    return(**it);
  }

  const AttributeVector* Tag::GetAttributes(void) const
  {
    return(m_attributes.empty() ? NULL : &m_attributes);
  }

  //////////////////////////////////////////////////////////////////////////////////
  Simple::Simple(const std::string& strName)
      :Element(strName)
  {}

  bool Simple::Add(Element* pChild)
  {
    return(false);
  }

  const ElementVector* Simple::GetChilds(void) const
  {
    return(NULL);
  }

  const Element& Simple::GetChild(const char* szName, int nIndex) const
  {
    return(null);
  }

  const AttributeVector* Simple::GetAttributes(void) const
  {
    return(NULL);
  }

  //////////////////////////////////////////////////////////////////////////////////
  Comment::Comment(const std::string& strComment)
      :Simple(TEXT("!"))
  {
    AddValue(strComment);
  }

  //////////////////////////////////////////////////////////////////////////////////
  Null::Null(void)
      :Simple(TEXT(""))
  {}

  bool Null::IsNull(void) const
  {
    return(true);
  }


  //////////////////////////////////////////////////////////////////////////////////
  SyntaxError::SyntaxError(int nLine, int nColumn)
      :m_nLine(nLine),m_nColumn(nColumn)
  {}

  int SyntaxError::GetLine(void) const
  {
    return(m_nLine);
  }

  int SyntaxError::GetColumn(void) const
  {
    return(m_nColumn);
  }

  //////////////////////////////////////////////////////////////////////////////////
  Bookmark::Bookmark(Parser& reader) :m_reader(reader), m_szSourceCurrent(reader.m_szSourceCurrent)
  //:m_reader(reader)
  {
    //m_CurPos = m_reader.m_pInputStream->tellg();
  }

  void Bookmark::Restore(void)
  {
    m_reader.m_szSourceCurrent = m_szSourceCurrent;
    //m_reader.m_pInputStream->seekg(m_CurPos);
  }

  void Bookmark::GetSubString(std::string& strString, int nNumEndSkip)
  {
    ASSERT(m_reader.m_szSourceCurrent + nNumEndSkip >= m_szSourceCurrent);
    strString = std::string(m_szSourceCurrent, m_reader.m_szSourceCurrent - m_szSourceCurrent - nNumEndSkip);
    /*
    std::istream::pos_type temp_pos = m_reader.m_pInputStream->tellg();
    {
      m_reader.m_pInputStream->seekg(m_CurPos);
      for(int i = 0; i < nNumEndSkip; i++)
      {
        strString += (char)m_reader.m_pInputStream->get();
      }
    }
    m_reader.m_pInputStream->seekg(temp_pos); */
  }

  void Bookmark::Reset(void)
  {
    m_szSourceCurrent = m_reader.m_szSourceCurrent;
    //m_CurPos = m_reader.m_pInputStream->tellg();
  }

  //////////////////////////////////////////////////////////////////////////////////
  inline bool IsSpace(char c)
  {
    return(c == TEXT(' ') || c == TEXT('\t') || c == TEXT('\r') || c == TEXT('\n'));
  }

  inline bool IsAlpha(char c)
  {
    return((c >= TEXT('a') && c <= TEXT('z')) || (c >= TEXT('A') && c <= TEXT('Z')));
  }

  inline bool IsDigit(char c)
  {
    return(c >= TEXT('0') && c <= TEXT('9'));
  }

  inline bool IsHexDigit(char c)
  {
    return(IsDigit(c) || (c >= TEXT('a') && c <= TEXT('f')) || (c >= TEXT('A') && c <= TEXT('F')));
  }

  inline int HexDigitValue(char c)
  {
    return((c >= TEXT('0') && c <= TEXT('9')) ? c - TEXT('0')
           : ((c >= TEXT('a') && c <= TEXT('f')) ? c - TEXT('a') + 10
              : c - TEXT('A') + 10));

  }

  inline bool IsAlphaDigit(char c)
  {
    return(IsAlpha(c) || IsDigit(c));
  }

  inline bool IsAlphaDigitEx(char c)
  {
    return(IsAlphaDigit(c) || c == TEXT('_') || c == TEXT('.') || c == TEXT(':') || c == TEXT('-'));
  }

  inline char LowCase(char c)
  {
    return(c >= TEXT('A') && c <= TEXT('Z') ? c - TEXT('A') + TEXT('a') : c);
  }


  inline bool IsXmlChar(char c)
  {
    return(c == 0x9 || c == 0xa || c == 0xd  || c >= 0x20);
  }

  //////////////////////////////////////////////////////////////////////////////////
  Parser::Parser(void) //:m_szSource(NULL),
    :m_nLine(1),
     m_nColumn(1),
     m_strXmlVersion(TEXT("1.0"))
  {}

  Parser::~Parser(void)
  {}

  void Parser::RaiseSyntaxError(void)
  {
    throw SyntaxError(m_nLine, m_nColumn);
  }

  bool Parser::ParseString(const char* pString)
  {
    Bookmark bookmark(*this);

    while(*pString != 0)
    {
      char c = NextChar();
      if(c != *pString)
      {
        bookmark.Restore();
        return(false);
      }

      ++pString;
    }

    return(true);
  }

  bool Parser::ParseStringNoCase(const char* pString)
  {
    Bookmark bookmark(*this);

    while(*pString != 0)
    {
      char c = NextChar();
      if(LowCase(c) != LowCase(*pString))
      {
        bookmark.Restore();
        return(false);
      }

      ++pString;
    }

    return(true);
  }

  bool Parser::ParseNumber(int& nNum)
  {
    char c = NextChar();
    if(!IsDigit(c))
      return(false);

    nNum = 0;
    while(IsDigit(c))
    {
      nNum = nNum * 10 + c - TEXT('0');
      c = NextChar();
    }

    PreviousChar();
    return(true);
  }

  bool Parser::ParseHexNumber(int& nNum)
  {
    char c = NextChar();
    if(!IsHexDigit(c))
      return(false);

    nNum = 0;
    while(IsHexDigit(c))
    {
      nNum = nNum * 16 + HexDigitValue(c);
      c = NextChar();
    }

    PreviousChar();
    return(true);
  }

  bool Parser::ParseChar(char c)
  {
    if(NextChar() != c)
    {
      PreviousChar();
      return(false);
    }
    return(true);
  }


  bool Parser::ParseSpaces(void)
  {
    char c = NextChar();
    if(!IsSpace(c))
    {
      PreviousChar();
      return(false);
    }

    do
      c = NextChar();
    while(IsSpace(c));
    PreviousChar();
    return(true);
  }

  bool Parser::ParseDeclBegining(const char* szString)
  {
    char c = NextChar();
    if(!IsSpace(c))
    {
      PreviousChar();
      return(false);
    }
    ParseSpaces();

    if(!ParseString(szString))
      return(false);

    if(!ParseEq())
      RaiseSyntaxError();

    return(true);
  }


  bool Parser::ParseXMLDecl(void)
  {
    if(!ParseStringNoCase(TEXT("<?xml")))
      return(false);

    if(!ParseVersionInfo(m_strXmlVersion))
      RaiseSyntaxError();

    ParseEncodingDecl();
    ParseSpaces();

    if(!ParseString(TEXT("?>")))
      RaiseSyntaxError();

    return(true);
  }

  bool Parser::ParseEq(void)
  {
    Bookmark bookmark(*this);

    ParseSpaces();
    if(!ParseChar(TEXT('=')))
    {
      bookmark.Restore();
      return(false);
    }
    ParseSpaces();

    return(true);
  }

  bool Parser::ParseVersionInfo(std::string& strVersion)
  {
    if(!ParseDeclBegining(TEXT("version")))
      return(false);

    char c = NextChar();
    if(c != TEXT('\'') && c != TEXT('\"'))
      RaiseSyntaxError();

    if(!ParseVersionNum(strVersion) || NextChar() != c)
      RaiseSyntaxError();

    return(true);
  }

  bool Parser::ParseVersionNum(std::string& strVersion)
  {
    Bookmark bookmark(*this);

    char c = NextChar();
    if(!IsAlphaDigitEx(c))
      return(false);

    c = NextChar();
    while(IsAlphaDigitEx(c))
      c = NextChar();

    PreviousChar();

    bookmark.GetSubString(strVersion);
    return(true);
  }


  bool Parser::ParseEncodingDecl(void)
  {
    if(!ParseDeclBegining(TEXT("encoding")))
      return(false);

    char c = NextChar();
    if(c != TEXT('\'') && c != TEXT('\"'))
      RaiseSyntaxError();

    if(ParseEncName() || NextChar() != c)
      RaiseSyntaxError();

    return(true);
  }

  bool Parser::ParseEncName(void)
  {
    char c = NextChar();
    if(!IsAlpha(c))
      return(false);

    c = NextChar();
    while(IsAlphaDigitEx(c))
      c = NextChar();

    PreviousChar();

    return(true);
  }

  void Parser::ParseMiscs(void)
  {
    for(;;)
    {
      ParseSpaces();

      std::auto_ptr<Comment> pElem(ParseComment());
      if(NULL == pElem.get())
        break;
    }
  }

  Comment* Parser::ParseComment(void)
  {
    if(!ParseString(TEXT("<!--")))
      return(NULL);

    Bookmark bookmark(*this);
    for(;;)
    {
      if(ParseString(TEXT("--")))
      {
        if(!ParseChar(TEXT('>')))
          RaiseSyntaxError();
        break;
      }

      if(NextChar() == 0)
        RaiseSyntaxError();
    }

    std::string strComment;
    bookmark.GetSubString(strComment, 3);

    return(new Comment(strComment));
  }


  bool Parser::ParseName(std::string& strName)
  {
    Bookmark bookmark(*this);

    char c = NextChar();
    if(!IsAlpha(c) && c != TEXT('_') && c != TEXT(':'))
    {
      PreviousChar();
      return(false);
    }

    for(;;)
    {
      c = NextChar();
      if(!IsAlphaDigitEx(c))
        break;
    }

    PreviousChar();

    bookmark.GetSubString(strName);
    return(true);
  }

  bool Parser::ParseReference(char& cRef)
  {
    if(!ParseChar(TEXT('&')))
      return(false);

    char c = NextChar();
    if(c != TEXT('#'))
    {
      PreviousChar();
      std::string strReferenceName;
      if(!ParseName(strReferenceName) || !ParseChar(TEXT(';')))
        RaiseSyntaxError();
      if(!MapReferenceName(strReferenceName, cRef))
        RaiseSyntaxError();

      return(true);
    }

    c = NextChar();
    int nNum = 0;
    if(c == TEXT('x'))
    {
      if(!ParseHexNumber(nNum))
        RaiseSyntaxError();
    }
    else
    {
      PreviousChar();
      if(!ParseNumber(nNum))
        RaiseSyntaxError();
    }

    if(!ParseChar(TEXT(';')))
      RaiseSyntaxError();

    cRef = static_cast<char>(nNum);
    return(true);
  }

  bool Parser::ParseAttValue(std::string& strValue)
  {
    char cDelim = NextChar();
    if(cDelim != TEXT('\'') && cDelim != TEXT('\"'))
    {
      PreviousChar();
      return(false);
    }

    Bookmark bookmark(*this);

    char c = NextChar();
    while(c != cDelim)
    {
      switch(c)
      {
          case 0:
          RaiseSyntaxError();

          case TEXT('<'):
                RaiseSyntaxError();

          case TEXT('&'):
          {
            PreviousChar();
            std::string strBefore;
            bookmark.GetSubString(strBefore);
            strValue += strBefore;

            char c;
            if(!ParseReference(c))
              RaiseSyntaxError();

            strValue += c;
            bookmark.Reset();
          }
          break;

          default:
          c = NextChar();
          break;
      }
    }

    std::string strRemaining;
    bookmark.GetSubString(strRemaining, 1);
    strValue += strRemaining;

    return(true);
  }

  bool Parser::ParseAttribute(Tag* pElem)
  {
    std::string strName;
    if(!ParseName(strName))
      return(false);

    std::string strValue;
    if(!ParseEq() || !ParseAttValue(strValue))
      RaiseSyntaxError();

    std::auto_ptr<Attribute> pAttrib(new Attribute(strName, strValue));
    pElem->Add(pAttrib.release());

    return(true);
  }

  bool Parser::ParseCDATA(Element& element)
  {
    if(!ParseString(TEXT("<![CDATA[")))
      return(false);

    Bookmark bookmark(*this);
    for(;;)
    {
      if(ParseString(TEXT("]]>")))
        break;

      if(!IsXmlChar(NextChar()))
        RaiseSyntaxError();
    }

    std::string strCDATA;
    bookmark.GetSubString(strCDATA, 3);
    element.AddValue(strCDATA);
    return(true);
  }

  Tag* Parser::ParseTagBegining(void)
  {
    if(!ParseChar(TEXT('<')))
      return(NULL);

    std::string strName;
    if(!ParseName(strName))
      RaiseSyntaxError();

    std::auto_ptr<Tag> pElem(new Tag(strName));

    while(ParseSpaces() && ParseAttribute(pElem.get()))
      ;

    return(pElem.release());
  }

  bool Parser::ParseETag(Element& element)
  {
    if(!ParseString(TEXT("</")))
      return(false);

    std::string strEndTagName;
    if(!ParseName(strEndTagName))
      RaiseSyntaxError();

     if(stricmp(strEndTagName.c_str(), element.GetName().c_str()))//if(strEndTagName != element.GetName())
      RaiseSyntaxError();

    ParseSpaces();
    if(!ParseChar(TEXT('>')))
      RaiseSyntaxError();

    return(true);
  }

  bool Parser::ParseMarkup(Element& element)
  {
    std::auto_ptr<Comment> pComment(ParseComment());
    if(NULL != pComment.get())
    {
      if(!element.Add(pComment.release()))
        RaiseSyntaxError();
      return(true);
    }

    if(ParseCDATA(element))
      return(true);

    std::auto_ptr<Tag> pTag(ParseElement());
    if(NULL != pTag.get())
    {
      if(!element.Add(pTag.release()))
        RaiseSyntaxError();
      return(true);
    }

    return(false);
  }

  void Parser::ParseContentETag(Tag& element)
  {
    Bookmark bookmark(*this);

    char c = NextChar();
    for(;;)
    {
      if(ParseString(TEXT("]]>")))
        RaiseSyntaxError();

      switch(c)
      {
        case 0:
          RaiseSyntaxError();
          break;

        case TEXT('&'):
        case TEXT('<'):
        {
          PreviousChar();
          std::string strValue;
          bookmark.GetSubString(strValue);
          element.AddValue(strValue);

          if(c == TEXT('&'))
          {
            if(!ParseReference(c))
              RaiseSyntaxError();
            element.AddValue(c);
          }
          else
          {
            if(ParseETag(element))
              return;
            else if(!ParseMarkup(element))
              RaiseSyntaxError();
          }

          bookmark.Reset();
        }
        break;

        default:
          break;
      }

      c = NextChar();
    }
  }

  Tag* Parser::ParseElement(void)
  {
    std::auto_ptr<Tag> pTag(ParseTagBegining());
    if(pTag.get() == NULL)
      return(NULL);

    char c = NextChar();
    if(c == TEXT('/'))
    {
      c = NextChar();
      if(c != TEXT('>'))
        RaiseSyntaxError();

      return(pTag.release());
    }

    if(c != TEXT('>'))
      RaiseSyntaxError();

    ParseContentETag(*pTag);
    return(pTag.release());
  }


  Element* Parser::ParseDocument(void)
  {
    ParseXMLDecl();
    ParseMiscs();

    std::auto_ptr<Element> pElem(ParseElement());
    if(NULL == pElem.get())
      RaiseSyntaxError();

    ParseMiscs();
    return(pElem.release());
  }

  /*
  Element& Parser::Parse(std::istream* pInputStream)
  {
    m_pInputStream = pInputStream;
    m_nLine             = 1;
    m_nColumn           = 1;

    std::auto_ptr<Element> temp(ParseDocument());
    m_pRootElem=temp;

    return(*m_pRootElem);
  } */
  
  Element& Parser::Parse(const char* szXmlSource, unsigned int nSourceSize)
  {
    m_szSource          = szXmlSource;
    m_szSourceCurrent   = m_szSource;
    m_szSourceEnd       = m_szSource + nSourceSize;

    m_nLine             = 1;
    m_nColumn           = 1;

    std::auto_ptr<Element> temp(ParseDocument());
    m_pRootElem=temp;

    return(*m_pRootElem);
  }
  
  char Parser::NextChar(void)
  {
    if(m_szSourceCurrent >= m_szSourceEnd)
      return(0);
    char c = *m_szSourceCurrent++;
    /*
    int c = m_pInputStream->get();
    if(c == std::istream::traits_type::eof())
      return 0; */

    if(c == TEXT('\r'))
    {
      if(m_szSourceCurrent >= m_szSourceEnd)
        return(0);
      c = *m_szSourceCurrent++;
      /*
      c = m_pInputStream->get();
      if(c == std::istream::traits_type::eof())
        return 0; */
    }

    if(c == TEXT('\n'))
      ++m_nLine, m_nColumn = 1;
    else
      ++m_nColumn;

    return (char)c;
  }

  void Parser::PreviousChar(void)
  {
    if(m_szSourceCurrent - 1 < m_szSource)
      m_szSourceCurrent = m_szSource;
    else
      m_szSourceCurrent -= 1;
    //m_pInputStream->unget();
  }


  struct MapReference
  {
    const char* szName;
    char c;
  }

  static const s_MapReference[] =
  {
    { TEXT("lt"),   TEXT('<') },
    { TEXT("gt"),   TEXT('>') },
    { TEXT("amp"),  TEXT('&') },
    { TEXT("apos"), TEXT('\"') },
    { TEXT("quot"), TEXT('\'') }
  };

  bool Parser::MapReferenceName(const std::string& strName, char& c)
  {
    for(unsigned int nIndex = 0; nIndex < elemof(s_MapReference); ++nIndex)
    {
      if(strName == s_MapReference[nIndex].szName)
      {
        c = s_MapReference[nIndex].c;
        return(true);
      }
    }

    return(false);
  }

  ///////////////////////////////////////////////////////////////////////
}
