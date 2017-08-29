#include "iii_anim.h"

int &CGame__currLevel = *(int*)0x941514;

#if 0

struct DebugLine
{
	CVector v1;
	CVector v2;
	uint col1;
	uint col2;
};
#define MAXDEBUGLINES 20000
DebugLine debugLines[MAXDEBUGLINES];
int numDebuglines;

WRAPPER RwBool RwRenderStateSet(RwRenderState state, void *value) { EAXJMP(0x5A43C0); }
WRAPPER void CLines__RenderLineWithClipping(float x1, float y1, float z1, float x2, float y2, float z2, uint c1, uint c2) { EAXJMP(0x50A3B0); }

void
addDebugLine(float x1, float y1, float z1, float x2, float y2, float z2, uint c1, uint c2)
{
	int n;
	if(numDebuglines >= MAXDEBUGLINES)
		return;
	n = numDebuglines++;
	debugLines[n].v1.x = x1;
	debugLines[n].v1.y = y1;
	debugLines[n].v1.z = z1;
	debugLines[n].v2.x = x2;
	debugLines[n].v2.y = y2;
	debugLines[n].v2.z = z2;
	debugLines[n].col1 = c1;
	debugLines[n].col2 = c2;
}

void
renderDebugLines(void)
{
	int i;
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)1);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
	for(i = 0; i < numDebuglines; i++)
		CLines__RenderLineWithClipping(debugLines[i].v1.x,
			debugLines[i].v1.y,
			debugLines[i].v1.z,
			debugLines[i].v2.x,
			debugLines[i].v2.y,
			debugLines[i].v2.z,
			debugLines[i].col1, debugLines[i].col2);
	numDebuglines = 0;
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, 0);
}

//WRAPPER void CTheScripts__ScriptDebugLine3D(float x1, float y1, float z1, float x2, float y2, float z2, uint c1, uint c2) { EAXJMP(0x4534E0); }
//void CTheScripts__ScriptDebugLine3D(RwV3d v1, RwV3d v2, uint c1, uint c2) { CTheScripts__ScriptDebugLine3D(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, c1, c2); }
void CTheScripts__ScriptDebugLine3D(float x1, float y1, float z1, float x2, float y2, float z2, uint c1, uint c2) { addDebugLine(x1, y1, z1, x2, y2, z2, c1, c2); }
void CTheScripts__ScriptDebugLine3D(RwV3d v1, RwV3d v2, uint c1, uint c2) { CTheScripts__ScriptDebugLine3D(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, c1, c2); }

void
renderBox(CColBox *box, uint col, RwMatrix *mat)
{
	RwV3d verts[8];
	verts[0].x = box->min.x;
	verts[0].y = box->min.y;
	verts[0].z = box->min.z;
	verts[1].x = box->max.x;
	verts[1].y = box->min.y;
	verts[1].z = box->min.z;
	verts[2].x = box->min.x;
	verts[2].y = box->max.y;
	verts[2].z = box->min.z;
	verts[3].x = box->max.x;
	verts[3].y = box->max.y;
	verts[3].z = box->min.z;
	verts[4].x = box->min.x;
	verts[4].y = box->min.y;
	verts[4].z = box->max.z;
	verts[5].x = box->max.x;
	verts[5].y = box->min.y;
	verts[5].z = box->max.z;
	verts[6].x = box->min.x;
	verts[6].y = box->max.y;
	verts[6].z = box->max.z;
	verts[7].x = box->max.x;
	verts[7].y = box->max.y;
	verts[7].z = box->max.z;
	RwV3dTransformPoints(verts, verts, 8, mat);

	CTheScripts__ScriptDebugLine3D(verts[0], verts[1], col, col);
	CTheScripts__ScriptDebugLine3D(verts[1], verts[3], col, col);
	CTheScripts__ScriptDebugLine3D(verts[3], verts[2], col, col);
	CTheScripts__ScriptDebugLine3D(verts[2], verts[0], col, col);

	CTheScripts__ScriptDebugLine3D(verts[0+4], verts[1+4], col, col);
	CTheScripts__ScriptDebugLine3D(verts[1+4], verts[3+4], col, col);
	CTheScripts__ScriptDebugLine3D(verts[3+4], verts[2+4], col, col);
	CTheScripts__ScriptDebugLine3D(verts[2+4], verts[0+4], col, col);

	CTheScripts__ScriptDebugLine3D(verts[0], verts[4], col, col);
	CTheScripts__ScriptDebugLine3D(verts[1], verts[5], col, col);
	CTheScripts__ScriptDebugLine3D(verts[2], verts[6], col, col);
	CTheScripts__ScriptDebugLine3D(verts[3], verts[7], col, col);
}

void
renderSphere(CColSphere *sphere, uint col, RwMatrix *matrix)
{
	RwV3d c;
	RwV3d verts[6];
	RwV3dTransformPoints(&c, (RwV3d*)&sphere->center, 1, matrix);
	verts[0] = verts[1] = verts[2] = verts[3] = verts[4] = verts[5] = c;
	verts[0].z += sphere->radius;	// top
	verts[1].z -= sphere->radius;	// bottom
/*
	verts[2].x += sphere->radius;
	verts[2].y += sphere->radius;
	verts[3].x += sphere->radius;
	verts[3].y -= sphere->radius;
	verts[4].x -= sphere->radius;
	verts[4].y += sphere->radius;
	verts[5].x -= sphere->radius;
	verts[5].y -= sphere->radius;
*/
	verts[2].x += sphere->radius;
	verts[3].x -= sphere->radius;
	verts[4].y += sphere->radius;
	verts[5].y -= sphere->radius;

	CTheScripts__ScriptDebugLine3D(verts[0], verts[2], col, col);
	CTheScripts__ScriptDebugLine3D(verts[0], verts[3], col, col);
	CTheScripts__ScriptDebugLine3D(verts[0], verts[4], col, col);
	CTheScripts__ScriptDebugLine3D(verts[0], verts[5], col, col);
	CTheScripts__ScriptDebugLine3D(verts[1], verts[2], col, col);
	CTheScripts__ScriptDebugLine3D(verts[1], verts[3], col, col);
	CTheScripts__ScriptDebugLine3D(verts[1], verts[4], col, col);
//	CTheScripts__ScriptDebugLine3D(verts[1], verts[5], col, col);
//	CTheScripts__ScriptDebugLine3D(verts[2], verts[3], col, col);
//	CTheScripts__ScriptDebugLine3D(verts[3], verts[5], col, col);
//	CTheScripts__ScriptDebugLine3D(verts[5], verts[4], col, col);
}

void
renderTriangle(CColTriangle *tri, CVector *v, uint col, RwMatrix *matrix)
{
	RwV3d verts[3];
	verts[0] = *(RwV3d*)&v[tri->a];
	verts[1] = *(RwV3d*)&v[tri->b];
	verts[2] = *(RwV3d*)&v[tri->c];
	RwV3dTransformPoints(verts, verts, 3, matrix);
	CTheScripts__ScriptDebugLine3D(verts[0], verts[1], col, col);
	CTheScripts__ScriptDebugLine3D(verts[1], verts[2], col, col);
	CTheScripts__ScriptDebugLine3D(verts[2], verts[0], col, col);
}

void
renderCol(CColModel *col, CMatrix *matrix)
{
	int i;
	renderBox(&col->boundingBox, 0xFF0000FF, &matrix->matrix);
	for(i = 0; i < col->numBoxes; i++)
		renderBox(&col->boxes[i], 0xFFFFFFFF, &matrix->matrix);
	for(i = 0; i < col->numSpheres; i++)
		renderSphere(&col->spheres[i], 0xFF00FFFF, &matrix->matrix);
	for(i = 0; i < col->numTriangles; i++)
		renderTriangle(&col->triangles[i], col->vertices, 0x00FF00FF, &matrix->matrix);
}

void
renderEntityCol(CEntity *e)
{
	RwV3d v, campos;
	v = e->matrix.matrix.pos;
	campos = RwFrameGetLTM(RwCameraGetFrame((RwCamera*)((RwGlobals*)RwEngineInstance)->curCamera))->pos;
	RwV3dSub(&v, &v, &campos);
	float dist = RwV3dDotProduct(&v, &v);
	CBaseModelInfo *mi = (CBaseModelInfo*)CModelInfo::ms_modelInfoPtrs[e->nModelIndex];
	if(mi->type == 6){	// Ped{
		CPedModelInfo *pmi = (CPedModelInfo*)mi;
		renderCol(pmi->hitColModel, &e->matrix);
	}else
	if(sqrt(dist) < 100.0f)
		renderCol(mi->colModel, &e->matrix);
}

static void (__thiscall *pedRender_)(CEntity *e);
static void (__thiscall *civilianpedRender_)(CEntity *e);
static void (__thiscall *coppedRender_)(CEntity *e);
static void (__thiscall *emergencypedRender_)(CEntity *e);
static void (__thiscall *playerpedRender_)(CEntity *e);

static void (__thiscall *buildingRender_)(CEntity *e);
static void (__thiscall *treadableRender_)(CEntity *e);

static void (__thiscall *vehicleRender_)(CEntity *e);
static void (__thiscall *automobileRender_)(CEntity *e);
static void (__thiscall *boatRender_)(CEntity *e);
static void (__thiscall *heliRender_)(CEntity *e);
static void (__thiscall *planeRender_)(CEntity *e);
static void (__thiscall *trainRender_)(CEntity *e);

struct EntityRender : CEntity
{
	void pedRender(void) { pedRender_(this); renderEntityCol(this); }
	void civilianpedRender(void) { civilianpedRender_(this); renderEntityCol(this); }
	void coppedRender(void) { coppedRender_(this); renderEntityCol(this); }
	void emergencypedRender(void) { emergencypedRender_(this); renderEntityCol(this); }
	void playerpedRender(void) { playerpedRender_(this); renderEntityCol(this); }

	void buildingRender(void) { buildingRender_(this); renderEntityCol(this); }
	void treadableRender(void) { treadableRender_(this); renderEntityCol(this); }

	void vehicleRender(void) { vehicleRender_(this); renderEntityCol(this); }
	void automobileRender(void) { automobileRender_(this); renderEntityCol(this); }
	void boatRender(void) { boatRender_(this); renderEntityCol(this); }
	void heliRender(void) { heliRender_(this); renderEntityCol(this); }
	void planeRender(void) { planeRender_(this); renderEntityCol(this); }
	void trainRender(void) { trainRender_(this); renderEntityCol(this); }
};

void
hookEntityVtables(void)
{
	InterceptVmethod(&pedRender_, &EntityRender::pedRender, 0x5F8C60);
	InterceptVmethod(&civilianpedRender_, &EntityRender::civilianpedRender, 0x5F81D0);
	InterceptVmethod(&coppedRender_, &EntityRender::coppedRender, 0x5F82D8);
	InterceptVmethod(&emergencypedRender_, &EntityRender::emergencypedRender, 0x5F83A8);
	InterceptVmethod(&playerpedRender_, &EntityRender::playerpedRender, 0x5FA534);

	InterceptVmethod(&buildingRender_, &EntityRender::buildingRender, 0x5EBF78);
	InterceptVmethod(&treadableRender_, &EntityRender::treadableRender, 0x5EBFE8);

	InterceptVmethod(&vehicleRender_, &EntityRender::vehicleRender, 0x6028DC);
	InterceptVmethod(&automobileRender_, &EntityRender::automobileRender, 0x600C50);
	InterceptVmethod(&boatRender_, &EntityRender::boatRender, 0x600ED8);
	InterceptVmethod(&heliRender_, &EntityRender::heliRender, 0x601EE4);
	InterceptVmethod(&planeRender_, &EntityRender::planeRender, 0x602210);
	InterceptVmethod(&trainRender_, &EntityRender::trainRender, 0x602450);

	InjectHook(0x48E080, renderDebugLines);
}

#endif


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
