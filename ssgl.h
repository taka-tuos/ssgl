#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef __SSGL_H__
#define __SSGL_H__

typedef unsigned int uint32;

typedef struct {
	float x, y, z, u, v, w, s;
} gl_vertex3d;

typedef struct {
	uint32 *p;
	float *zfar;
	int w,h;
} gl_screen;

typedef struct {
	unsigned int *p;
	int w,h;
} gl_texture2d;

typedef struct {
	int nowvert;
	gl_vertex3d pos[256];
	gl_vertex3d mvpos;
	gl_vertex3d rtpos;
	gl_screen pixels;
	gl_texture2d texture;
	uint32 color;
	float sf;
	float ang;
	float vvz;
	uint32 tmp;
	int txl;
	int fgs;
} gl_ctl;

/* useful func */
gl_ctl gl_init(unsigned int *p, int w, int h);
void gl_vertex(gl_ctl *ctl, float x, float y, float z, float u, float v);
void gl_vertex(gl_ctl *ctl, float x, float y, float z);
void gl_crotate(gl_ctl *ctl, float x, float y, float z);
void gl_ctransform(gl_ctl *ctl, float x, float y, float z);
void gl_angle(gl_ctl *ctl, float angle);
void gl_texture(gl_ctl *ctl, uint32 *p, int w, int h);
void gl_color(gl_ctl *ctl, uint32 color);
int  gl_render(gl_ctl *ctl, int flags);
void gl_clear(gl_ctl *ctl);

/* difficult use func */
void gl_rotate3D(gl_vertex3d *newvertex,gl_vertex3d oldvertex, float angle, int axis);
void gl_renderpolytex(gl_ctl ctl,gl_vertex3d *lpVertex,int VerCou);
void gl_renderpolyfil(gl_ctl ctl,gl_vertex3d *lpVertex,int VerCou);
gl_vertex3d gl_getvec(float x, float y, float z, float u, float v, float w);
gl_vertex3d gl_getvec(float x, float y, float z, float u, float v);
gl_vertex3d gl_getvec(float x, float y, float z);
//void gl_vec3normalize(gl_vertex3d *pout, gl_vertex3d *pv);
//void gl_vec3cross(gl_vertex3d *pout, gl_vertex3d *pv1, gl_vertex3d *pv2);


#define gl_tocolor(r,g,b) (((int)r << 16) | ((int)g << 8) | (int)b)
#define gl_fromcolor(c,r,g,b) (*r = (c >> 16) & 0xff, *g = (c >> 8) & 0xff, *b = c & 0xff)
#define gl_alphamix(c1,c2,alpha) gl_tocolor(((((c2 >> 16) & 0xff) * alpha + ((c1 >> 16) & 0xff) * (255 - alpha)) / 255),((((c2 >> 8) & 0xff) * alpha + ((c1 >> 8) & 0xff) * (255 - alpha)) / 255),(((c2 & 0xff) * alpha + (c1 & 0xff) * (255 - alpha)) / 255))
#define gl_vec3cross(pout, pv1, pv2) ((pout)->x=((pv1)->y)*((pv2)->z)-((pv1)->z)*((pv2)->y),(pout)->y=((pv1)->z)*((pv2)->x)-((pv1)->x)*((pv2)->z),(pout)->z=((pv1)->x)*((pv2)->y)-((pv1)->y)*((pv2)->x))
#define gl_vec3normalize(pout,pv) { (pout)->x=(pv)->x,(pout)->y=(pv)->y,(pout)->z=(pv)->z; float __n = sqrt((pv)->x * (pv)->x + (pv)->y * (pv)->y + (pv)->z * (pv)->z); if(__n > 1e-6) { (pout)->x *= 1.0f/__n,(pout)->y *= 1.0f/__n,(pout)->z *= 1.0f/__n; } }

#define gl_floor(n) floor(n)



#define GL_UNTEX 16777215.0f

#define GL_NOSHADE 0
#define GL_FLSHADE 1

#endif
