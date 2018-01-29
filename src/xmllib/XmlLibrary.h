//base code from unknown
//psksvp@ccs.neu.edu has added/modified this code.
#ifndef __XML_LIBRARY__
#define __XML_LIBRARY__

#include <memory>
#include <vector>
#include <string>
#include <istream>



namespace XmlLibrary
{
  class Attribute;
  class Element;
  class Null;

  typedef std::vector<Attribute*>  AttributeVector;
  typedef std::vector<Element*>    ElementVector;

  class  Attribute
  {
    std::string  m_strName;
    std::string  m_strValue;

  public:
    Attribute(const std::string& strName, const std::string& strValue);

    const std::string& Name(void) const;
    const std::string& Value(void) const;
  };

  class  Value
  {
    std::string  m_strValue;
  public:
    Value(void);
    void Add(const std::string& strText);
    void Add(char c);
  
    operator const std::string&(void) const;
    operator const char*(void) const;
    operator bool(void) const;
    operator char(void) const;
    operator unsigned char(void) const;
    operator short(void) const;
    operator unsigned short(void) const;
    operator int(void) const;
    operator unsigned int(void) const;
    operator long(void) const;
    operator unsigned long(void) const;
    operator float(void) const;
    operator double(void) const;
  };

  class  Element
  {
  private:
    const std::string    m_strName;
    Value           m_value;

  public:
    static Null& null;

  public:
    Element(void);
    Element(const std::string& strName);
    virtual ~Element(void);

    void AddValue(const std::string& strText);
    void AddValue(char c);

    const std::string& GetName(void) const;
    const Value& GetValue(void) const;

    const Element& operator()(const char* szName, int nIndex = 0) const;

  public:
    virtual bool IsNull(void) const;
    virtual bool Add(Element* pChild) = 0;
    virtual const ElementVector* GetChilds(void) const = 0;
    virtual const Element& GetChild(const char* szName, int nIndex = 0) const = 0;
    virtual const AttributeVector* GetAttributes(void) const = 0;
  };

  class  Tag :public Element
  {
  private:
    AttributeVector  m_attributes;
    ElementVector    m_childs;

  public:
    Tag(const std::string& strName);
    ~Tag(void);

    void Add(Attribute* pAttribute);

    virtual bool Add(Element* pChild);
    virtual const ElementVector* GetChilds(void) const;
    virtual const Element& GetChild(const char* szName, int nIndex = 0) const;
    virtual const AttributeVector* GetAttributes(void) const;

  private:
    bool FindChild(const char* szName, ElementVector::const_iterator& it) const;
  };

  class  Simple :public Element
  {
  public:
    Simple(const std::string& strName);
    virtual bool Add(Element* pChild);
    virtual const ElementVector* GetChilds(void) const;
    virtual const Element& GetChild(const char* szName, int nIndex = 0) const;
    virtual const AttributeVector* GetAttributes(void) const;
  };

  class  Comment :public Simple
  {
  public:
    Comment(const std::string& strComment);
  };

  class  Null :public Simple
  {
  public:
    Null(void);

  private:
    virtual bool IsNull(void) const;
  };

  class  SyntaxError
  {
  private:
    int m_nLine;
    int m_nColumn;

  public:
    SyntaxError(int nLine, int nColumn);

    int GetLine(void) const;
    int GetColumn(void) const;
  };

  class Parser;
  class  Bookmark
  {
  private:
    Parser&     m_reader;
    const char* m_szSourceCurrent;
    //std::istream::pos_type m_CurPos; 
    int         m_nLine;
    int         m_nColumn;

  public:
    Bookmark(Parser& reader);
    void Restore(void);
    void GetSubString(std::string& strString, int nNumEndSkip = 0);
    void Reset(void);
  };

  class  Parser
  {
    friend class Bookmark;
  private:
    const char*         m_szSource;
    const char*         m_szSourceCurrent;
    const char*         m_szSourceEnd;
    //std::istream* m_pInputStream;
    int                 m_nLine;
    int                 m_nColumn;

    std::string              m_strXmlVersion;
    std::auto_ptr<Element>   m_pRootElem;

  public:
    Parser(void);
    virtual ~Parser(void);

    //Element& Parse(std::istream* pInputStream);
    Element& Parse(const char* szXmlSource, unsigned int nSourceSize);
  private:
    char NextChar(void);
    void PreviousChar(void);


    bool ParseSpaces(void);
    bool ParseString(const char* pString);
    bool ParseStringNoCase(const char* pString);
    bool ParseNumber(int& nNum);
    bool ParseHexNumber(int& nNum);
    bool ParseChar(char c);
    bool ParseName(std::string& strName);

    bool ParseDeclBegining(const char* szString);
    bool ParseXMLDecl(void);
    bool ParseEq(void);
    bool ParseVersionInfo(std::string& strVersion);
    bool ParseVersionNum(std::string& strVersion);
    bool ParseEncodingDecl(void);
    bool ParseEncName(void);
    void ParseMiscs(void);
    bool ParseReference(char& c);
    bool ParseAttValue(std::string& strValue);
    bool ParseAttribute(Tag* pElem);
    bool ParseETag(Element& element);
    void ParseContentETag(Tag& element);
    bool ParseMarkup(Element& element);
    bool ParseCDATA(Element& element);

    Comment* ParseComment(void);
    Tag* ParseTagBegining(void);
    Tag* ParseElement(void);
    Element* ParseDocument(void);

    bool MapReferenceName(const std::string& strName, char& c);

    void RaiseSyntaxError(void);
  };
}


#endif

