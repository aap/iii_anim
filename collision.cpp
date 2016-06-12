#include "iii_anim.h"

int &CGame__currLevel = *(int*)0x941514;

void
CColModel::ctor(void)
{
	this->numSpheres = 0;
	this->spheres = 0;
	this->numLines = 0;
	this->lines = 0;
	this->numBoxes = 0;
	this->boxes = 0;
	this->numTriangles = 0;
	this->vertices = 0;
	this->triangles = 0;
	this->unk2 = 0;
	this->level = CGame__currLevel;
	this->unk1 = 1;
}

void
CColSphere::Set(float radius, CVector *center, uchar mat, uchar flag)
{
	this->radius = radius;
	this->center = *center;
	this->mat = mat;
	this->flag = flag;
}

void
CColBox::Set(CVector *min, CVector *max, uchar mat, uchar flag)
{
	this->min = *min;
	this->max = *max;
	this->mat = mat;
	this->flag = flag;
}

void
CColLine::Set(CVector *p0, CVector *p1)
{
	this->p0 = *p0;
	this->p1 = *p1;
}

void
CColTriangle::Set(int a, int b, int c, uchar mat)
{
	this->a = a;
	this->b = b;
	this->c = c;
	this->mat = mat;
}
