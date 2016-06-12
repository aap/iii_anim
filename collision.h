typedef BYTE byte;
typedef BYTE uchar;

struct CColSphere
{
	CVector center;
	float radius;
	uchar mat;
	uchar flag;

	void Set(float radius, CVector *center, uchar mat, uchar flag);
};

struct CColBox
{
	CVector min;
	CVector max;
	uchar mat;
	uchar flag;

	void Set(CVector *min, CVector *max, uchar mat, uchar flag);
};

struct CColLine
{
	CVector p0;
	int pad0;
	CVector p1;
	int pad1;

	void Set(CVector *p0, CVector *p1);
};

struct CColTriangle
{
	short a;
	short b;
	short c;
	uchar mat;

	void Set(int a, int b, int c, uchar mat);
};

struct CColPoint
{
	CVector point;
	int pad1;
	CVector normal;
	int pad2;
	byte surfaceTypeA;
	byte pieceTypeA;
	byte pieceTypeB;
	byte surfaceTypeB;
	float depth;
};

struct CColModel
{
	CColSphere boundingSphere;
	CColBox boundingBox;
	short numSpheres;
	short numLines;
	short numBoxes;
	short numTriangles;
	int level;
	byte unk1;
	CColSphere *spheres;
	CColLine *lines;
	CColBox *boxes;
	CVector *vertices;
	CColTriangle *triangles;
	int unk2;

	void ctor(void);
};
