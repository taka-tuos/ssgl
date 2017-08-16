#include "ssgl.h"
#include <stdlib.h>

typedef struct {
	int w,h;
	uint32 *pixel;
} gl_2dtexture;

typedef struct {
	float	*g_Vertex;
	float	*g_Uvvert;
	int		*g_Vindex;
	int		*g_Polnum;
	int		*g_Polcol;
	int		*g_Matrial;
	int		g_Polnum_a;
	gl_2dtexture *g_Textures;
	gl_vertex3d pos;
	gl_vertex3d rot;
} gl_mqomodel;

gl_mqomodel gl_readmqo(char *filename, float scale);
int gl_callmodel(gl_ctl *ctl, gl_mqomodel m);
uint32 *gl_loadimg(char *fname, int *w, int *h);