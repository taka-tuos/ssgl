#include "mqoloader.h"
#include "ssgl.h"
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

char *gpc_getop(char *s)
{
	char *str = (char *)malloc(512);

	int i;
	char *p = s;

	for(;*p==' '||*p==0x09;p++);
	for(i = 0;*p!='(';p++,i++) {
		str[i]=*p;
	}

	str[i]=0;

	return str;
}

char *gpc_getargstr(char *s,int num)
{
	char *str = (char *)malloc(512);

	int i;
	char *p = s;

	for(;*p==' '||*p==0x09;p++);
	for(i = 0;*p!='(';p++,i++);

	p++;

	for(i=0;i<num;i++) {
		for(;*p!=' '&&*p!=')';p++);
		p++;
	}

	for(i=0;*p!=' '&&*p!=')';p++,i++) {
		str[i]=*p;
	}

	str[i]=0;

	return str;
}

char *gpc_getskipspt(char *s)
{
	for(;*s == ' ' || *s =='\t';s++);
	return s;
}

uint32 *gl_loadimg(char *fname, int *w, int *h) {
	int tx, ty, bpp;
	int i;
	stbi_uc *picture = stbi_load(fname,&tx,&ty,&bpp,4);
	uint32 *bmp = (uint32 *) malloc(tx*ty*4);

	if(picture == 0) return 0;

	*w = tx;
	*h = ty;

	for(i = 0; i < tx*ty; i++) {
		bmp[i] = gl_tocolor(picture[i*4+0],picture[i*4+1],picture[i*4+2]) | (picture[i*4+3] << 24);
	}

	stbi_image_free(picture);

	return bmp;
}

int gl_ispath(char *s)
{
	int i = 0;
	for(;*s != 0; s++) if(*s == '/' || *s == '\\') i = 1;

	return i;
}

char *gl_gpath(char *p)
{
	int i;
	for(i = strlen(p); i != 0; i--) {
		if(p[i] == '/' || p[i] == '\\') { p[i + 1] = 0; break; }
	}
	return p;
}

#define MODE_MAT 1
#define MODE_VER 2
#define MODE_POL 3

int gl_callmodel(gl_ctl *ctl, gl_mqomodel m)
{
	//gl_clear(ctl);
	int	i, j, u, v, w;
	float x, y, z, a, b;
	int dp = 0;
	w = 0;
	for (i = 0; i < m.g_Polnum_a; i++) {
		for (j = 0; j < m.g_Polnum[i]; j++) {
			u = w + j;
			v = w + ((j + 1) % m.g_Polnum[i]);
			x = m.g_Vertex[m.g_Vindex[u]*3+0];
			y = m.g_Vertex[m.g_Vindex[u]*3+1];
			z = m.g_Vertex[m.g_Vindex[u]*3+2];
			a = m.g_Uvvert[u*2+0];
			b = m.g_Uvvert[u*2+1];
			gl_vertex3d v;
			v = gl_getvec(x,y,z);
			gl_rotate3D(&v,v,m.rot.x,0);
			gl_rotate3D(&v,v,m.rot.y,1);
			gl_rotate3D(&v,v,m.rot.z,2);
			v.x += m.pos.x;
			v.y += m.pos.y;
			v.z += m.pos.z;
			gl_vertex(ctl,v.x,v.y,v.z,a,1.0f-b);
		}
		if(m.g_Textures[m.g_Matrial[i]].pixel != 0) {
			gl_texture(ctl,m.g_Textures[m.g_Matrial[i]].pixel,m.g_Textures[m.g_Matrial[i]].w,m.g_Textures[m.g_Matrial[i]].h);
		}
		gl_color(ctl,m.g_Polcol[i]);
		dp += gl_render(ctl,GL_FLSHADE);
		w += m.g_Polnum[i];
	}

	return dp;
}

gl_mqomodel gl_readmqo(char *filename, float scale)
{
	gl_mqomodel model;
	FILE *fp = fopen(filename,"rt");
	char fbk[256];

	int mats[256];
	bool txf[256];

	int mode = 0;

	int mc = 0;
	int vc = 0;
	int pc = 0;

	int ppc = 0;

	strcpy(fbk,filename);

	while(!feof(fp)) {
		char mln[1024];
		char *tln;
		fgets(mln,1024,fp);
		if(strcmp(mln,"Eof") == 0) break;
		tln = gpc_getskipspt(mln);
		if(strncmp(tln,"Material",strlen("Material")) == 0) {
			mode = MODE_MAT;
			int a = atoi(tln + strlen("Material"));
			model.g_Textures  = (gl_2dtexture *)malloc(a*sizeof(gl_2dtexture));
			continue;
		}
		if(strncmp(tln,"vertex",strlen("vertex")) == 0) {
			mode = MODE_VER;
			int a = atoi(tln + strlen("vertex"));
			model.g_Vertex = (float *)malloc(a*3*sizeof(float));
			continue;
		}
		if(strncmp(tln,"face",strlen("face")) == 0) {
			mode = MODE_POL;
			int a = atoi(tln + strlen("face"));
			model.g_Uvvert = (float *)malloc(a*8*sizeof(float));
			model.g_Vindex = (int *)malloc(a*4*sizeof(int));
			model.g_Polnum = (int *)malloc(a*4*sizeof(int));
			model.g_Polcol = (int *)malloc(a*4*sizeof(int));
			model.g_Matrial = (int *)malloc(a*sizeof(int));
			continue;
		}
		if(mode != 0 && tln[0] == '}') mode = 0;

		if(mode == MODE_MAT) {
			if(tln[0] == '\"') {
				float r,g,b;
				char *p = tln;
				for(;*p != 'c';p++);
				char *arg;
				arg = gpc_getargstr(p,0);
				r = atof(arg);
				arg = gpc_getargstr(p,1);
				g = atof(arg);
				arg = gpc_getargstr(p,2);
				b = atof(arg);

				for(;strncmp(p,"tex",3) != 0 && *p != 0;p++);
				if(*p != 0) {
					arg = gpc_getargstr(p,0);
					arg++;
					arg[strlen(arg)-1] = 0;
					char *tn = arg;
					char fp[256];
					char bp[256];
					getcwd(bp,256);
					realpath(fp,gl_gpath(fbk));
					chdir(fp);
					int w,h;
					model.g_Textures[mc].pixel = gl_loadimg(tn,&w,&h);
					model.g_Textures[mc].h = h;
					model.g_Textures[mc].w = w;
					chdir(bp);
					if(model.g_Textures[mc].pixel != 0) txf[mc] = true;
				} else {
					model.g_Textures[mc].pixel = 0;
					txf[mc] = false;
				}


				r *= 255.0f;
				g *= 255.0f;
				b *= 255.0f;

				mats[mc] = (((int)r << 16) | ((int)g << 8) | ((int)b << 0));
				mc++;
			}
		}

		if(mode == MODE_VER) {
			sscanf(tln,"%f %f %f",&model.g_Vertex[vc*3+0],&model.g_Vertex[vc*3+1],&model.g_Vertex[vc*3+2]);
			model.g_Vertex[vc*3+0] *= scale;
			model.g_Vertex[vc*3+1] *= scale;
			model.g_Vertex[vc*3+2] *= scale;
			vc++;
		}

		if(mode == MODE_POL) {
			if(strchr("0123456789",tln[0]) != 0) {
				char *p = tln;
				char *arg;
				int i;
				gl_vertex3d smp[256];
				model.g_Polnum[pc] = tln[0] - '0';
				for(i = 0; i < tln[0] - '0'; i++) {
					arg = gpc_getargstr(p,i);
					int ind = atoi(arg);
					model.g_Vindex[ppc+i] = ind;
				}

				for(;*p != 'M';p++);
				p--;

				arg = gpc_getargstr(p,0);
				int ms = atoi(arg);
				model.g_Polcol[pc] = mats[ms];
				model.g_Matrial[pc] = ms;

				for(;*p != 'U' && *p != 0;p++);
				if(*p != 0 && txf[ms] == true) {
					int w = tln[0] - '0';
					for(i = 0; i < w * 2; i++) {
						arg = gpc_getargstr(p,i);
						model.g_Uvvert[ppc*2+i] = atof(arg);
					}
				} else {
					int w = tln[0] - '0';
					for(i = 0; i < w * 2; i++) {
						model.g_Uvvert[ppc*2+i] = GL_UNTEX;
					}
				}



				pc++;
				ppc += tln[0] - '0';
			}
		}
	}

	model.g_Polnum_a = pc;
	model.pos = gl_getvec(0,0,0);
	model.rot = gl_getvec(0,0,0);

	fclose(fp);
	return model;
}
