#include <SDL/SDL.h>
#include <time.h>
#include "use.h"
#include "mqoloader.h"
#include "ssgl.h"

#define TEX_USE 1
#define DEMO_TRNGL 0
#define RECT_SIZE 50.0f
#define ANGLE_VIEW 120.0f
#define TEXNAME "Water.png"
#define RECT_COL 0xffffff
#define WINDOW_X 640
#define WINDOW_Y (WINDOW_X/4*3)
#define MSAA 0
#define WDEMO 1

#if TEX_USE==1
#define UV0 0
#define UV1 1
#else
#define UV0 GL_UNTEX
#define UV1 GL_UNTEX
#endif

#define PAR_N 256
#define PAR_N2 8

gl_vertex3d vels[PAR_N];
gl_vertex3d poss[PAR_N];
int flg[PAR_N];

/* Core i3-550 + GTX650 + Windows7 x64 Ult */
/* 204800 texpolys/sec (with shading) */
/* 409600 polys/sec (with shading) */
/* 409600 texpolys/sec (without shading) */
/* 409600 polys/sec (without shading) */
/* game limit 102400 polys/sec */

void SDL_x2Surface(SDL_Surface *dst, SDL_Surface *src)
{
	int x, y;
	for(y = 0; y < src->h; y++) {
		for(x = 0; x < src->w; x++) {
			int c = ((int *)src->pixels)[((y+0)) * src->w + ((x+0))];
			((int *)dst->pixels)[((y<<1) + 0) * dst->w + ((x<<1) + 0)] = c;
			((int *)dst->pixels)[((y<<1) + 0) * dst->w + ((x<<1) + 1)] = c;
			((int *)dst->pixels)[((y<<1) + 1) * dst->w + ((x<<1) + 0)] = c;
			((int *)dst->pixels)[((y<<1) + 1) * dst->w + ((x<<1) + 1)] = c;
		}
	}
}

void SDL_d2Surface(SDL_Surface *dst, SDL_Surface *src)
{
	int x, y;
	for(y = 0; y < src->h; y += 2) {
		for(x = 0; x < src->w; x += 2) {
			int r[4],g[4],b[4];
			int rf,gf,bf;

			gl_fromcolor(((int *)src->pixels)[(y + 0) * src->w + (x + 0)],&r[0],&g[0],&b[0]);
			gl_fromcolor(((int *)src->pixels)[(y + 0) * src->w + (x + 1)],&r[1],&g[1],&b[1]);
			gl_fromcolor(((int *)src->pixels)[(y + 1) * src->w + (x + 0)],&r[2],&g[2],&b[2]);
			gl_fromcolor(((int *)src->pixels)[(y + 1) * src->w + (x + 1)],&r[3],&g[3],&b[3]);

			rf = (r[0] + r[1] + r[2] + r[3]) / 4;
			gf = (g[0] + g[1] + g[2] + g[3]) / 4;
			bf = (b[0] + b[1] + b[2] + b[3]) / 4;

			((int *)dst->pixels)[(y>>1) * dst->w + (x>>1)] = gl_tocolor(rf,gf,bf);
		}
	}
}

int dm_drawmqo_0(gl_ctl *ctl, gl_mqomodel m, float angle)
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
			//y -= 3000;
			//y -= 40;
			gl_rotate3D(&v,gl_getvec(x,y,z),angle,1);
			gl_rotate3D(&v,v,angle,1);
			gl_rotate3D(&v,v,angle,1);
			gl_rotate3D(&v,v,45*3.14/180,0);
			gl_rotate3D(&v,v,3.14,0);
			v.z += 20;
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

int dm_drawmqo_1(gl_ctl *ctl, gl_mqomodel m, int n)
{
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
			//a = GL_UNTEX;
			//b = GL_UNTEX;
			a = m.g_Uvvert[u*2+0];
			b = m.g_Uvvert[u*2+1];
			x += poss[n].x;
			y += poss[n].y;
			z += poss[n].z;
			//if(z > PAR_N*16) flg[n] = 0;
			//poss[n].x += vels[n].x;
			//poss[n].y += vels[n].y;
			//poss[n].z += vels[n].z;
			gl_vertex3d v = gl_getvec(x,y,z);
			gl_rotate3D(&v,v,3.14,0);
			v.z += 20;
			gl_vertex(ctl,v.x,v.y,v.z,a,1.0f-b);
		}
		if(m.g_Textures[m.g_Matrial[i]].pixel != 0) {
			gl_texture(ctl,m.g_Textures[m.g_Matrial[i]].pixel,m.g_Textures[m.g_Matrial[i]].w,m.g_Textures[m.g_Matrial[i]].h);
		}
		gl_color(ctl,m.g_Polcol[i]);
		dp += gl_render(ctl,GL_NOSHADE);
		w += m.g_Polnum[i];
	}

	return dp;
}

gl_mqomodel m2,m3;
uint32 *skbk;
gl_ctl ctl;

SDL_Surface *sdl_screen;
SDL_Surface *x2_screen;

float angle = 0;
float m3x=0,m3y=0,m3z=0,m3r=0;

float wy[256*256];

int t(void *s)
{
	int x,y,i;

	int tx, ty;
	uint32 *bmp = gl_loadimg(TEXNAME,&tx,&ty);

	memset(wy,0,sizeof(wy));

	while(true) {
#if MSAA == 0
		memset(x2_screen->pixels,0xff,WINDOW_X*WINDOW_Y*4);
		memcpy(x2_screen->pixels,skbk,x2_screen->w*x2_screen->h*4);
#else
		memset(x2_screen->pixels,0xff,WINDOW_X*2*WINDOW_Y*2*4);
		memcpy(x2_screen->pixels,skbk,x2_screen->w*x2_screen->h*4);
#endif

		gl_clear(&ctl);

		//dm_drawmqo_0(&ctl,m,angle);

		//Uint8 *ks = SDL_GetKeyState(NULL);

		/*float ddy = 0;

		if(ks[SDLK_UP]) ddy=0.4;
		if(ks[SDLK_DOWN]) ddy=-0.4;

		if(ks[SDLK_KP8]) m3z+=1.2;
		if(ks[SDLK_KP2]) m3z-=1.2;

		if(ks[SDLK_LEFT]) m3r+=0.2;
		if(ks[SDLK_RIGHT]) m3r-=0.2;

		float rnd = (float)rand() / (float)RAND_MAX * 0.2 - 0.1;

		float rx = 0,ry = ddy,pr = m3r;
		float dx,dy;

		dx = rx * cos(pr) - ry * sin(pr);
		dy = rx * sin(pr) + ry * cos(pr);

		m3x += dx;
		m3y += dy;

		m3.rot.z = m3.rot.z*0.9 + (3.14+m3r)*0.1;
		m3.pos.z = m3.pos.z*0.9 + (rnd*4) * 0.1+m3z;
		m3.pos.y = m3.pos.y*0.9 + (rnd*64) * 0.1+m3y;
		m3.pos.x = m3.pos.x*0.9 + (rnd*2) * 0.1+m3x;
		*/

		/*int n;
		for(n=0;n<PAR_N;n++) if(flg[n] == 0) break;

		flg[n] = 1;

		gl_vertex3d v;

		float ma[] = { -4,4 };

		//v = gl_getvec(m3.pos.x,m3.pos.y+5,m3.pos.z+10);

		gl_rotate3D(&v,gl_getvec(ma[n%2],5,10),-m3.rot.z-3.14,2);

		v.x += m3.pos.x;
		v.y += -m3.pos.y;
		v.z += -m3.pos.z;

		poss[n] = v;
		vels[n] = gl_getvec(0,0,16);*/
		
		gl_callmodel(&ctl,m3);

		/*for(y = 0; y < 64; y++) {
			for(x = 0; x < 64; x++) {
				float rnd = (float)rand() / (float)RAND_MAX * 8.0f;
				wy[y*64+x] = wy[y*64+x] * 0.9 + rnd * 0.1;
			}
		}*/

		for(y = 0; y < 64 - 1; y++) {
			for(x = 0; x < 64 - 1; x++) {
				gl_texture(&ctl,bmp,tx,ty);
				gl_vertex3d v;
				gl_rotate3D(&v,gl_getvec((x+0-32)*64,wy[(y+1)*32+(x+0)]+40,(y+1-32)*64,0,0),0,0);
				gl_vertex(&ctl,v.x,v.y+40,v.z-fmod(angle,64.0f),v.u,v.v);
				gl_rotate3D(&v,gl_getvec((x+1-32)*64,wy[(y+1)*32+(x+1)]+40,(y+1-32)*64,1,0),0,0);
				gl_vertex(&ctl,v.x,v.y+40,v.z-fmod(angle,64.0f),v.u,v.v);
				gl_rotate3D(&v,gl_getvec((x+1-32)*64,wy[(y+0)*32+(x+1)]+40,(y+0-32)*64,1,1),0,0);
				gl_vertex(&ctl,v.x,v.y+40,v.z-fmod(angle,64.0f),v.u,v.v);
				gl_rotate3D(&v,gl_getvec((x+0-32)*64,wy[(y+0)*32+(x+0)]+40,(y+0-32)*64,0,1),0,0);
				gl_vertex(&ctl,v.x,v.y+40,v.z-fmod(angle,64.0f),v.u,v.v);
				gl_render(&ctl,GL_FLSHADE);
			}
		}

		for(i = 0; i < PAR_N; i++) {
			if(flg[i] == 1) dm_drawmqo_1(&ctl,m2,i);
		}
#if MSAA != 0
		//gl_render(&ctl,GL_FLSHADE);
		//gl_clear(&ctl);
		SDL_d2Surface(sdl_screen,x2_screen);
#endif
		angle += 16;
		SDL_UpdateRect(sdl_screen,0,0,0,0);
	}
}

#define FPS 60

void adjustFPS(void) {
	static unsigned long maetime=SDL_GetTicks();
	static int frame=0;
	long sleeptime;
	frame++;
	sleeptime=(frame<FPS)?
		(maetime+(long)((double)frame*(1000.0/FPS))-SDL_GetTicks()):
		(maetime+1000-SDL_GetTicks());
	if(sleeptime>0)SDL_Delay(sleeptime);
	if(frame>=FPS) {
		frame=0;
		maetime=SDL_GetTicks();
	}
}

int main(int argc, char **argv)
{
	//SDL_Surface *sdl_screen;
	SDL_Event sdl_event;

	//SDL_Surface *x2_screen;

	SDL_Init(SDL_INIT_VIDEO);
	sdl_screen = SDL_SetVideoMode(WINDOW_X,WINDOW_Y,32,SDL_HWSURFACE);
#if MSAA == 0
	x2_screen = sdl_screen;
#else
	x2_screen = SDL_CreateRGBSurface(SDL_SWSURFACE,WINDOW_X*2,WINDOW_Y*2,32,0xff0000,0x00ff00,0x0000ff,0);
#endif
	
	int tx, ty;
	uint32 *bmp = gl_loadimg(TEXNAME,&tx,&ty);
	int skx,sky;
	uint32 *skb = gl_loadimg("skybox_front.png",&skx,&sky);
#if MSAA == 0
	/*gl_ctl*/ ctl = gl_init((uint32 *)x2_screen->pixels, WINDOW_X, WINDOW_Y);
#else
	/*gl_ctl*/ ctl = gl_init((uint32 *)x2_screen->pixels, WINDOW_X*2, WINDOW_Y*2);
#endif
	gl_color(&ctl,RECT_COL);
	gl_texture(&ctl,bmp,tx,ty);

	float angle = 0;
	int i;

	float wy[256*256];

	memset(wy,0,sizeof(wy));

	unsigned char g = 0;

	gl_angle(&ctl,ANGLE_VIEW);

	//ctl.mvpos.z = -500;

	/*gl_mqomodel*/ //m = gl_readmqo("./エリア18　saigon_jungle/saigon_jungle_ssgl.mqo",0.1f);
	/*gl_mqomodel*/ m3 = gl_readmqo("F15.mqo",0.1f);

	m3.rot.z = 3.14;
	m3.pos.y = -40;

	/*gl_mqomodel*/ m2 = gl_readmqo("part.mqo",4.0f);

	for(i = 0; i < PAR_N; i++) {
		poss[i] = gl_getvec(0,-35,i*16);
		vels[i] = gl_getvec(0,0,4);
		flg[i] = 0;
	}

	float m3x=0,m3y=0,m3z=0,m3r=0;

	int x,y;

	/*uint32 **/skbk = (uint32 *) malloc(x2_screen->w*x2_screen->h*4);

	for(y = 0; y < x2_screen->h; y++) {
		for(x = 0; x < x2_screen->w; x++) {
			float xp = (float)x / (float)x2_screen->w;
			float yp = (float)y / (float)x2_screen->h;
			int ix = xp * skx;
			int iy = yp * sky;
			skbk[y * x2_screen->w + x] = skb[iy * skx + ix];
		}
	}

	SDL_CreateThread(t,0);

	while(!poll_event(&sdl_event)) {
		adjustFPS();
		/*
#if MSAA == 0
		memset(x2_screen->pixels,0xff,WINDOW_X*WINDOW_Y*4);
		memcpy(x2_screen->pixels,skbk,x2_screen->w*x2_screen->h*4);
#else
		memset(x2_screen->pixels,0xff,WINDOW_X*2*WINDOW_Y*2*4);
		memcpy(x2_screen->pixels,skbk,x2_screen->w*x2_screen->h*4);
#endif*/
#if WDEMO != 0
		/*gl_clear(&ctl);

		//dm_drawmqo_0(&ctl,m,angle);*/

		Uint8 *ks = SDL_GetKeyState(NULL);

		float ddy = 0;

		if(ks[SDLK_UP]) ddy=0.4;
		if(ks[SDLK_DOWN]) ddy=-0.4;

		if(ks[SDLK_KP8]) m3z+=1.2;
		if(ks[SDLK_KP2]) m3z-=1.2;

		if(ks[SDLK_LEFT]) m3r+=0.2;
		if(ks[SDLK_RIGHT]) m3r-=0.2;

		float rnd = (float)rand() / (float)RAND_MAX * 0.2 - 0.1;

		float rx = 0,ry = ddy,pr = m3r;
		float dx,dy;

		dx = rx * cos(pr) - ry * sin(pr);
		dy = rx * sin(pr) + ry * cos(pr);

		m3x += dx;
		m3y += dy;

		m3.rot.z = m3.rot.z*0.9 + (3.14+m3r)*0.1;
		m3.pos.z = m3.pos.z*0.9 + (rnd*4) * 0.1+m3z;
		m3.pos.y = m3.pos.y*0.9 + (rnd*64) * 0.1+m3y;
		m3.pos.x = m3.pos.x*0.9 + (rnd*2) * 0.1+m3x;




		int n;
		for(n=0;n<PAR_N;n++) if(flg[n] == 0) break;

		gl_vertex3d v;

		float ma[] = { -4,4 };

		//v = gl_getvec(m3.pos.x,m3.pos.y+5,m3.pos.z+10);

		gl_rotate3D(&v,gl_getvec(ma[n%2],5,10),-m3.rot.z-3.14,2);

		v.x += m3.pos.x;
		v.y += -m3.pos.y;
		v.z += -m3.pos.z;

		poss[n] = v;
		vels[n] = gl_getvec(0,0,16);

		flg[n] = 1;

		for(i=0;i<PAR_N;i++) {
			poss[i].x += vels[i].x;
			poss[i].y += vels[i].y;
			poss[i].z += vels[i].z;
			if(poss[i].z > PAR_N*16) flg[i] = 0;
		}


		/*gl_callmodel(&ctl,m3);*/

		for(y = 0; y < 64; y++) {
			for(x = 0; x < 64; x++) {
				float rnd = (float)rand() / (float)RAND_MAX * 8.0f;
				wy[y*64+x] = wy[y*64+x] * 0.9 + rnd * 0.1;
			}
		}

	/*	for(y = 0; y < 64 - 1; y++) {
			for(x = 0; x < 64 - 1; x++) {
				gl_texture(&ctl,bmp,tx,ty);
				gl_vertex3d v;
				gl_rotate3D(&v,gl_getvec((x+0-32)*64,wy[(y+1)*32+(x+0)]+40,(y+1-32)*64,0,0),0,0);
				gl_vertex(&ctl,v.x,v.y+40,v.z-fmod(angle,64.0f),v.u,v.v);
				gl_rotate3D(&v,gl_getvec((x+1-32)*64,wy[(y+1)*32+(x+1)]+40,(y+1-32)*64,1,0),0,0);
				gl_vertex(&ctl,v.x,v.y+40,v.z-fmod(angle,64.0f),v.u,v.v);
				gl_rotate3D(&v,gl_getvec((x+1-32)*64,wy[(y+0)*32+(x+1)]+40,(y+0-32)*64,1,1),0,0);
				gl_vertex(&ctl,v.x,v.y+40,v.z-fmod(angle,64.0f),v.u,v.v);
				gl_rotate3D(&v,gl_getvec((x+0-32)*64,wy[(y+0)*32+(x+0)]+40,(y+0-32)*64,0,1),0,0);
				gl_vertex(&ctl,v.x,v.y+40,v.z-fmod(angle,64.0f),v.u,v.v);
				gl_render(&ctl,GL_FLSHADE);
			}
		}

		for(i = 0; i < PAR_N; i++) {
			if(flg[i] == 1) dm_drawmqo_1(&ctl,m2,i);
		}*/
#else
#if DEMO_TRNGL == 1
		gl_clear(&ctl);
		gl_vertex3d v;
		gl_rotate3D(&v,gl_getvec( RECT_SIZE,-RECT_SIZE,0,UV1,UV1),angle,1);
		gl_vertex(&ctl,v.x,v.y,v.z,v.u,v.v);
		gl_rotate3D(&v,gl_getvec( RECT_SIZE, RECT_SIZE,0,UV1,UV0),angle,1);
		gl_vertex(&ctl,v.x,v.y,v.z,v.u,v.v);
		gl_rotate3D(&v,gl_getvec(-RECT_SIZE, RECT_SIZE,0,UV0,UV0),angle,1);
		gl_vertex(&ctl,v.x,v.y,v.z,v.u,v.v);
		gl_render(&ctl,GL_FLSHADE);
		gl_rotate3D(&v,gl_getvec(-RECT_SIZE, RECT_SIZE,0,UV0,UV0),angle,1);
		gl_vertex(&ctl,v.x,v.y,v.z,v.u,v.v);
		gl_rotate3D(&v,gl_getvec(-RECT_SIZE,-RECT_SIZE,0,UV0,UV1),angle,1);
		gl_vertex(&ctl,v.x,v.y,v.z,v.u,v.v);
		gl_rotate3D(&v,gl_getvec( RECT_SIZE,-RECT_SIZE,0,UV1,UV1),angle,1);
		gl_vertex(&ctl,v.x,v.y,v.z,v.u,v.v);
		gl_render(&ctl,GL_FLSHADE);
#else
		gl_clear(&ctl);
		gl_vertex3d v;
		gl_rotate3D(&v,gl_getvec( RECT_SIZE,-RECT_SIZE,0,UV1,UV1),angle,1);
		gl_vertex(&ctl,v.x,v.y,v.z,v.u,v.v);
		gl_rotate3D(&v,gl_getvec( RECT_SIZE, RECT_SIZE,0,UV1,UV0),angle,1);
		gl_vertex(&ctl,v.x,v.y,v.z,v.u,v.v);
		gl_rotate3D(&v,gl_getvec(-RECT_SIZE, RECT_SIZE,0,UV0,UV0),angle,1);
		gl_vertex(&ctl,v.x,v.y,v.z,v.u,v.v);
		gl_rotate3D(&v,gl_getvec(-RECT_SIZE,-RECT_SIZE,0,UV0,UV1),angle,1);
		gl_vertex(&ctl,v.x,v.y,v.z,v.u,v.v);
		gl_render(&ctl,GL_FLSHADE);
#endif
#endif/*
#if MSAA != 0
		//gl_render(&ctl,GL_FLSHADE);
		//gl_clear(&ctl);
		//SDL_d2Surface(sdl_screen,x2_screen);
#endif
		angle += 16;
		SDL_UpdateRect(sdl_screen,0,0,0,0);*/
	}

	SDL_Quit();

	return 0;
}
