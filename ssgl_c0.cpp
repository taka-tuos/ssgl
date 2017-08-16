#include "ssgl.h"

gl_vertex3d gl_getvec(float x, float y, float z)
{
	gl_vertex3d ret;
	ret.x = x;
	ret.y = y;
	ret.z = z;

	return ret;
}

gl_vertex3d gl_getvec(float x, float y, float z, float u, float v, float w)
{
	gl_vertex3d ret;
	ret.x = x;
	ret.y = y;
	ret.z = z;
	ret.u = u;
	ret.v = v;
	ret.w = w;

	return ret;
}

gl_vertex3d gl_getvec(float x, float y, float z, float u, float v)
{
	gl_vertex3d ret;
	ret.x = x;
	ret.y = y;
	ret.z = z;
	ret.u = u;
	ret.v = v;
	ret.w = 0;

	return ret;
}


void gl_rotate3D(gl_vertex3d *newvertex,gl_vertex3d oldvertex, float angle, int axis)
{
	float cosX, sinX, x, y, z;

	x = oldvertex.x;
	y = oldvertex.y;
	z = oldvertex.z;

	sinX = sin(angle);
	cosX = cos(angle);

	newvertex->x = x;
	newvertex->y = y;
	newvertex->z = z;
	
	switch(axis)
	{
		case 0:
			newvertex->y = y * cosX - z * sinX;
			newvertex->z = y * sinX + z * cosX;
			break;
		case 1:
			newvertex->x = x * cosX + z * sinX;
			newvertex->z = z * cosX - x * sinX;
			break;
		case 2:
			newvertex->x = x * cosX - y * sinX;
			newvertex->y = x * sinX + y * cosX;
			break;
	}
	newvertex->u = oldvertex.u;
	newvertex->v = oldvertex.v;
}

gl_vertex3d ssgl_3dto2d(gl_ctl ctl, int i)
{
	gl_vertex3d src = ctl.pos[i];
	src.x -= ctl.mvpos.x;
	src.y -= ctl.mvpos.y;
	src.z -= ctl.mvpos.z;
	gl_rotate3D(&src,src,ctl.rtpos.x,0);
	gl_rotate3D(&src,src,ctl.rtpos.y,1);
	gl_rotate3D(&src,src,ctl.rtpos.z,2);
	float fov = ctl.sf / (src.z*2 + ctl.sf);
	float u = (src.u != GL_UNTEX)? (src.u*fov):(GL_UNTEX);
	float v = (src.v != GL_UNTEX)? (src.v*fov):(GL_UNTEX);
	float z = src.z;
	return gl_getvec((src.x*fov*2/320*ctl.pixels.w+(ctl.pixels.w/2)),(src.y*fov*2/240*ctl.pixels.h+(ctl.pixels.h/2)),z,u,v,fov);
}

gl_ctl gl_init(uint32 *p, int w, int h)
{
	gl_ctl ret;
	ret.nowvert = 0;
	ret.pixels.w = w;
	ret.pixels.h = h;
	ret.pixels.p = p;
	ret.pixels.zfar = (float *)malloc(w*h*sizeof(float));
	ret.mvpos = gl_getvec(0,0,0);
	ret.rtpos = gl_getvec(0,0,0);
	ret.ang = 90;
	return ret;
}

void gl_vertex(gl_ctl *ctl, float x, float y, float z, float u, float v)
{
	ctl->pos[ctl->nowvert].x = x;
	ctl->pos[ctl->nowvert].y = y;
	ctl->pos[ctl->nowvert].z = z;
	ctl->pos[ctl->nowvert].u = u;
	ctl->pos[ctl->nowvert].v = v;
	ctl->nowvert++;
}

void gl_vertex(gl_ctl *ctl, float x, float y, float z)
{
	ctl->pos[ctl->nowvert].x = x;
	ctl->pos[ctl->nowvert].y = y;
	ctl->pos[ctl->nowvert].z = z;
	ctl->pos[ctl->nowvert].u = GL_UNTEX;
	ctl->pos[ctl->nowvert].v = GL_UNTEX;
	ctl->nowvert++;
}

void gl_angle(gl_ctl *ctl, float angle)
{
	ctl->ang = angle;
}

void gl_texture(gl_ctl *ctl, unsigned int *p, int w, int h)
{
	ctl->texture.p = p;
	ctl->texture.w = w;
	ctl->texture.h = h;
}

void gl_color(gl_ctl *ctl, uint32 color)
{
	ctl->color = color;
}

void gl_clear(gl_ctl *ctl)
{
	memset(ctl->pixels.zfar,0,ctl->pixels.h*ctl->pixels.w*sizeof(float));
}

void gl_crotate(gl_ctl *ctl, float x, float y, float z)
{
	ctl->rtpos = gl_getvec(x,y,z);
}

void gl_ctransform(gl_ctl *ctl, float x, float y, float z)
{
	ctl->mvpos = gl_getvec(x,y,z);
}
/*
int gl_alphamix(int c1, int c2, int alpha)
{
	int r, g, b;
	int r1, g1, b1;
	int r2, g2, b2;
	int i = 2;
	
	r1 = (c1 >> 16) & 0xff;
	g1 = (c1 >> 8) & 0xff;
	b1 = c1 & 0xff;
	
	r2 = (c2 >> 16) & 0xff;
	g2 = (c2 >> 8) & 0xff;
	b2 = c2 & 0xff;
	
	r = (r2 * alpha + r1 * (255 - alpha)) / 255;   // Bチャンネル.
	g = (g2 * alpha + g1 * (255 - alpha)) / 255;   // Gチャンネル.
	b = (b2 * alpha + b1 * (255 - alpha)) / 255;   // Rチャンネル.
	
	if(r > 255) r = 255;
	if(g > 255) g = 255;
	if(b > 255) b = 255;
	
	
	return (r << 16) | (g << 8) | b;
}*/

int gl_min(int *all)
{
    int min=all[0];
    for(int i=1;i<4;++i){
        if(all[0]>all[i]){
            min=all[i];
        }
    }  
 
    return min;
 
}
int gl_max(int *all)
{
    int max=all[0];
    for(int i=1;i<4;++i){
        if(all[0]<all[i]){
            max=all[i];
        }
    }  
 
    return max;
     
}

int gl_dpl(gl_ctl *ctl, int flags, gl_vertex3d *npos, gl_vertex3d *cpos)
{
	int i,tmpcol;

	int txl = 0;

	tmpcol = ctl->color;
	ctl->fgs = flags;

	if(flags == GL_FLSHADE) {
		gl_vertex3d vv1,vv2,vv3,vv4,vv5;

		vv5 = gl_getvec(45*3.14/180,0,0);

		vv1.x = cpos[1].x - cpos[0].x;
		vv1.y = cpos[1].y - cpos[0].y;
		vv1.z = cpos[1].z - cpos[0].z;

		vv2.x = cpos[2].x - cpos[1].x;
		vv2.y = cpos[2].y - cpos[1].y;
		vv2.z = cpos[2].z - cpos[1].z;

		gl_vec3cross(&vv3,&vv1,&vv2);
		gl_vec3normalize(&vv4,&vv3);
		vv3 = vv4;

		//gl_rotate3D(&vv3,vv3,vv5.x,0);
		//gl_rotate3D(&vv3,vv3,vv5.y,1);
		//gl_rotate3D(&vv3,vv3,vv5.z,2);

		if(vv3.z < 0.0f) {
			ctl->nowvert=0;
			return 0;
		}

		ctl->vvz = fabs(vv3.y);

		int r, g, b;

		gl_fromcolor(ctl->color,&r,&g,&b);

		r*=fabs(vv3.y);
		g*=fabs(vv3.y);
		b*=fabs(vv3.y);

		ctl->color = gl_tocolor(r,g,b);
		ctl->tmp = tmpcol;
	}

	bool txf = true;

	for(i=0;i<ctl->nowvert;i++) {
		if(cpos[i].u >= 16777210.0f || cpos[i].v >= 16777210.0f) {
			txf = false;
			break;
		}
	}

	if(txf == true) gl_renderpolytex(*ctl,cpos,ctl->nowvert);
	else gl_renderpolyfil(*ctl,cpos,ctl->nowvert);
	ctl->color=tmpcol;
	ctl->nowvert=0;
}

int gl_render(gl_ctl *ctl, int flags)
{
	gl_vertex3d cpos[256];
	gl_vertex3d npos[256];
	int i;
	uint32 tmpcol;
	ctl->sf = 1000.0f / tan((ctl->ang / 2.0f*3.14159f/180.0f));
	int na[256],nb[256];
	memset(na,0,sizeof(na));
	memset(nb,0,sizeof(nb));
	for(i = 0; i < ctl->nowvert; i++) {
		cpos[i] = ssgl_3dto2d(*ctl,i);
		npos[i] = ctl->pos[i];
		if(cpos[i].z < -200) {
			ctl->nowvert=0;
			return 0;
		}
		if(cpos[i].x > 10000.0f || cpos[i].x < -10000.0f || cpos[i].y > 10000.0f || cpos[i].y < -10000.0f) {
			ctl->nowvert=0;
			return 0;
		}
		npos[i].x -= ctl->mvpos.x;
		npos[i].y -= ctl->mvpos.y;
		npos[i].z -= ctl->mvpos.z;
		gl_rotate3D(&npos[i],npos[i],ctl->rtpos.x,0);
		gl_rotate3D(&npos[i],npos[i],ctl->rtpos.y,1);
		gl_rotate3D(&npos[i],npos[i],ctl->rtpos.z,2);
		na[i] = cpos[i].x;
		nb[i] = cpos[i].y;
	}

	gl_vertex3d tcpos[16][256];
	gl_vertex3d tnpos[16][256];

	if(ctl->nowvert > 3) {
		int j,k;
		for(i=1,j=0;i<ctl->nowvert-1;i++) {
			tnpos[j][0] = npos[0];
			tnpos[j][1] = npos[(i+0)];
			tnpos[j][2] = npos[(i+1)];
			tcpos[j][0] = cpos[0];
			tcpos[j][1] = cpos[(i+0)];
			tcpos[j][2] = cpos[(i+1)];
			j++;
		}
		for(i=0;i<j;i++) {
			ctl->nowvert = 3;
			gl_dpl(ctl,flags,tnpos[i],tcpos[i]);
		}
	} else {
		memcpy(tcpos[0],cpos,sizeof(cpos));
		memcpy(tnpos[0],npos,sizeof(npos));
		gl_dpl(ctl,flags,tnpos[0],tcpos[0]);
	}

	return 1;
}
/*
void gl_vec3normalize(gl_vertex3d *pout, gl_vertex3d *pv)
{
	float len;
	float x, y, z;

	x = (pv->x);
	y = (pv->y);
	z = (pv->z);
	len = sqrt(x * x + y * y + z * z);

	if(len < (1e-6)) return;

	len = 1.0 / len;
	x *= len;
	y *= len;
	z *= len;

	pout->x = x;
	pout->y = y;
	pout->z = z;

	return;
}*/
/*
void gl_vec3cross(gl_vertex3d *pout, gl_vertex3d *pv1, gl_vertex3d *pv2)
{
	gl_vertex3d vec;
	float x1, y1, z1, x2, y2, z2;
	
	x1 = (pv1->x);
	y1 = (pv1->y);
	z1 = (pv1->z);
	x2 = (pv2->x);
	y2 = (pv2->y);
	z2 = (pv2->z);
 
	vec.x = (y1 * z2 - z1 * y2);
	vec.y = (z1 * x2 - x1 * z2);
	vec.z = (x1 * y2 - y1 * x2);
	*pout = vec;
}
*/

void gl_renderpolytex(gl_ctl ctl,gl_vertex3d *lpVertex,int VerCou)
{
	int m,m2,anum,bnum,oldm;
	float maxy,miny;
	float maxx,minx;
	float x1,x2;
	float dd;
	float fy,fx;
	int i,x,y;
	int sy,ey;
	float dxA,dxB;
	gl_vertex3d *lpVer;

	float xx1,xx2;
	int xs1,xs2;
	float tmp;

	float wid;
	float z1,z2;
	float dzA,dzB;
	float zz1,zz2;
	float dz;

	float w1,w2;
	float dwA,dwB;
	float ww1,ww2;
	float dw;

	float h1,h2;
	float dhA,dhB;
	float hh1,hh2;
	float dh;

	float u1,u2,v1,v2;
	float duA,duB,dvA,dvB;
	float xu1,xu2,xv1,xv2;
	float du,dv;
	int u,v,tx,ty;

	bool txf = true;

	if(VerCou<3) return;

	int r2,g2,b2;

	gl_fromcolor(ctl.tmp,&r2,&g2,&b2);

	/*for(i=0;i<VerCou;i++) {
		if(lpVertex[i].u >= 16777210.0f || lpVertex[i].v >= 16777210.0f) {
			txf = false;
			break;
		}
	}*/

	//Y座標値で最大・最小のものを求める
	m=0;
	lpVer=lpVertex;
	maxy=miny=lpVer->y;
	lpVer++;
	for(i=1;i<VerCou;i++){
		fy=lpVer->y;
		if(miny>fy){
			m=i;
			miny=fy;
		}
		if(maxy<fy) maxy=fy;
		lpVer++;
	}

	m2=0;
	lpVer=lpVertex;
	maxx=minx=lpVer->x;
	lpVer++;

	for(i=1;i<VerCou;i++){
		fx=lpVer->x;
		if(minx>fx){
			m2=i;
			minx=fx;
		}
		if(maxx<fx) maxx=fx;
		lpVer++;
	}

	sy=(int)gl_floor(miny+0.5);
	ey=(int)gl_floor(maxy+0.5);
	if(sy==ey) return;
	if(ey < 0) return;
	if(sy > ctl.pixels.h) return;
	if(maxx < 0) return;
	if(minx > ctl.pixels.w) return;


	//左側にある頂点と増加値の計算
	anum=bnum=m;
	do{
		anum++;
		if(anum>VerCou-1) anum=0;
	} while(lpVertex[anum].y<sy+0.5);
	m=anum-1;if(m<0) m=VerCou-1;

	dd=lpVertex[anum].y-lpVertex[m].y;
	dxA=(lpVertex[anum].x-lpVertex[m].x)/dd;
	dzA=(lpVertex[anum].z-lpVertex[m].z)/dd;
	duA=(lpVertex[anum].u-lpVertex[m].u)/dd;
	dvA=(lpVertex[anum].v-lpVertex[m].v)/dd;
	dwA=(lpVertex[anum].w-lpVertex[m].w)/dd;
	x1=lpVertex[m].x;
	z1=lpVertex[m].z;
	u1=lpVertex[m].u;
	v1=lpVertex[m].v;
	w1=lpVertex[m].w;

	//右側にある頂点と増加値の計算
	do{
		bnum--;
		if(bnum<0) bnum=VerCou-1;
	} while(lpVertex[bnum].y<sy+0.5);
	m=bnum+1;if(m>VerCou-1) m=0;

	dd=lpVertex[bnum].y-lpVertex[m].y;
	dxB=(lpVertex[bnum].x-lpVertex[m].x)/dd;
	dzB=(lpVertex[bnum].z-lpVertex[m].z)/dd;
	duB=(lpVertex[bnum].u-lpVertex[m].u)/dd;
	dvB=(lpVertex[bnum].v-lpVertex[m].v)/dd;
	dwB=(lpVertex[bnum].w-lpVertex[m].w)/dd;
	x2=lpVertex[m].x;
	z2=lpVertex[m].z;
	u2=lpVertex[m].u;
	v2=lpVertex[m].v;
	w2=lpVertex[m].w;

	int txl = 0;

	for(y=sy;y<ey;y++){
		//左側稜線の処理
		if(lpVertex[anum].y<y+0.5){
			do{
				anum++;
				if(anum>VerCou-1) anum=0;
			} while(lpVertex[anum].y<y+0.5);
			oldm=anum-1;if(oldm<0) oldm=VerCou-1;

			dd=lpVertex[anum].y-lpVertex[oldm].y;
			dxA=(lpVertex[anum].x-lpVertex[oldm].x)/dd;
			dzA=(lpVertex[anum].z-lpVertex[oldm].z)/dd;
			duA=(lpVertex[anum].u-lpVertex[oldm].u)/dd;
			dvA=(lpVertex[anum].v-lpVertex[oldm].v)/dd;
			dwA=(lpVertex[anum].w-lpVertex[oldm].w)/dd;
			x1=lpVertex[oldm].x;
			z1=lpVertex[oldm].z;
			u1=lpVertex[oldm].u;
			v1=lpVertex[oldm].v;
			w1=lpVertex[oldm].w;
		}
		//右稜線の処理
		if(lpVertex[bnum].y<y+0.5){
			do{
				bnum--;
				if(bnum<0) bnum=VerCou-1;
			} while(lpVertex[bnum].y<y+0.5);
			oldm=bnum+1;if(oldm>VerCou-1) oldm=0;

			dd=lpVertex[bnum].y-lpVertex[oldm].y;
			dxB=(lpVertex[bnum].x-lpVertex[oldm].x)/dd;
			dzB=(lpVertex[bnum].z-lpVertex[oldm].z)/dd;
			duB=(lpVertex[bnum].u-lpVertex[oldm].u)/dd;
			dvB=(lpVertex[bnum].v-lpVertex[oldm].v)/dd;
			dwB=(lpVertex[bnum].w-lpVertex[oldm].w)/dd;
			x2=lpVertex[oldm].x;
			z2=lpVertex[oldm].z;
			u2=lpVertex[oldm].u;
			v2=lpVertex[oldm].v;
			w2=lpVertex[oldm].w;
		}

		//X軸方向の描画
		xx1=x1;xx2=x2;
		zz1=z1;zz2=z2;
		xu1=u1;xu2=u2;
		xv1=v1;xv2=v2;
		ww1=w1;ww2=w2;

		if(xx2<xx1){
			tmp=xx1;xx1=xx2;xx2=tmp;
			tmp=zz1;zz1=zz2;zz2=tmp;
			tmp=xu1;xu1=xu2;xu2=tmp;
			tmp=xv1;xv1=xv2;xv2=tmp;
			tmp=ww1;ww1=ww2;ww2=tmp;
		}
		xs1=(int)gl_floor(xx1+0.5);
		xs2=(int)gl_floor(xx2+0.5);

		wid=xx2-xx1;
		if(wid>0.0){
			//Z方向の増加値
			//（Xが1増加するたびにZはどれだけ増加するか）
			dz=(zz2-zz1)/wid;
			dw=(ww2-ww1)/wid;

			//UV値の増加値
			//（Xが1増加するたびにU,Vはどれだけ増加するか）
			du=(xu2-xu1)/wid;
			dv=(xv2-xv1)/wid;
		}

		//１ライン分を描画
		for(x=xs1;x<xs2;x++){
			if(x >= 0 && x < ctl.pixels.w && y >= 0 && y < ctl.pixels.h) {
				//テクスチャのUV値を補正する
				float fu=xu1/ww1;
				float fv=xv1/ww1;

				//float ud = fu*(float)ctl.texture.w-gl_floor(fu*(float)ctl.texture.w);
				//float vd = 1.0-(fv*(float)ctl.texture.h-gl_floor(fv*(float)ctl.texture.h));

				int col = ctl.color;
				int alpha = 0xff;

				//テクスチャ上の対応点を求める
				u=(int)gl_floor(fu*(float)ctl.texture.w);
				v=(int)gl_floor(fv*(float)ctl.texture.h);
				tx=u % ctl.texture.w;
				ty=v % ctl.texture.h;
				ty=ctl.texture.h-1-ty;	  //座標の上下反転
				if(tx < 0) tx *= -1;
				if(ty < 0) ty *= -1;
				while(tx > ctl.texture.w) tx -= ctl.texture.w;
				while(ty > ctl.texture.h) ty -= ctl.texture.h;

				/*float a00,a01,a10,a11;

				a00 = ((1.0-ud)+(1.0-vd))/2;
				a01 = ud;
				a10 = vd;
				a11 = (ud+vd)/2;

				float divuv = a00+a01+a10+a11;

				float r00,g00,b00;
				float r01,g01,b01;
				float r10,g10,b10;
				float r11,g11,b11;

				gl_fromcolor(ctl.texture.p[(ty+0) * ctl.texture.w + (tx+0)],&r00,&g00,&b00);
				gl_fromcolor(ctl.texture.p[(ty+0) * ctl.texture.w + (tx+1)],&r01,&g01,&b01);
				gl_fromcolor(ctl.texture.p[(ty+1) * ctl.texture.w + (tx+0)],&r10,&g10,&b10);
				gl_fromcolor(ctl.texture.p[(ty+1) * ctl.texture.w + (tx+1)],&r11,&g11,&b11);

				float rf,gf,bf;

				rf = (r00*a00 + r01*a01 + r10*a10 + r11*a11) / divuv;
				gf = (g00*a00 + g01*a01 + g10*a10 + g11*a11) / divuv;
				bf = (b00*a00 + b01*a01 + b10*a10 + b11*a11) / divuv;*/


				//テクスチャ上の(tx,ty)での色を求める
				col = ctl.texture.p[ty * ctl.texture.w + tx];
				//col = gl_tocolor(rf,gf,bf);
				alpha = ctl.texture.p[ty * ctl.texture.w + tx]>>24;

				if(ctl.fgs == GL_FLSHADE) {
					int r1,g1,b1;
					int rf,gf,bf;
					gl_fromcolor(col,&r1,&g1,&b1);
					
					r1 = (r1 * r2 + r2 * (255 - r2)) / 255;
					g1 = (g1 * g2 + g2 * (255 - g2)) / 255;
					b1 = (b1 * b2 + b2 * (255 - b2)) / 255;

					rf = r1 * ctl.vvz;
					gf = g1 * ctl.vvz;
					bf = b1 * ctl.vvz;

					col = gl_tocolor(rf,gf,bf);
				}

				float nz = zz1/ww1;

				//SetPixel(hdc,x,y,col);
				float z = ctl.pixels.zfar[y * ctl.pixels.w + x];
				if(z == 0) z = 10000.0f;
				if(nz < z && alpha != 0) {
					ctl.pixels.p[y * ctl.pixels.w + x] = gl_alphamix(ctl.pixels.p[y * ctl.pixels.w + x],col,alpha);
					ctl.pixels.zfar[y * ctl.pixels.w + x] = nz;
				}
			}

			zz1+=dz;		//Z方向の増加値の加算
			ww1+=dw;
			
			//UV値の増加値の加算
			xu1+=du;
			xv1+=dv;
		}

		x1+=dxA;x2+=dxB;	//X方向の増加値の加算
		z1+=dzA;z2+=dzB;	//Z方向の増加値の加算

		//UV値の増加値の加算
		u1+=duA;u2+=duB;
		v1+=dvA;v2+=dvB;
		w1+=dwA;w2+=dwB;
	}
}

void gl_renderpolyfil(gl_ctl ctl,gl_vertex3d *lpVertex,int VerCou)
{
	int m,anum,bnum,oldm;
	float maxy,miny;
	float x1,x2;
	float dd;
	float fy;
	int i,x,y;
	int sy,ey;
	float dxA,dxB;
	gl_vertex3d *lpVer;

	float xx1,xx2;
	int xs1,xs2;
	float tmp;

	float wid;
	float z1,z2;
	float dzA,dzB;
	float zz1,zz2;
	float dz;

	float w1,w2;
	float dwA,dwB;
	float ww1,ww2;
	float dw;

	bool txf = true;

	if(VerCou<3) return;

	//Y座標値で最大・最小のものを求める
	m=0;
	lpVer=lpVertex;
	maxy=miny=lpVer->y;
	lpVer++;
	for(i=1;i<VerCou;i++){
		fy=lpVer->y;
		if(miny>fy){
			m=i;
			miny=fy;
		}
		if(maxy<fy) maxy=fy;
		lpVer++;
	}
	sy=(int)gl_floor(miny+0.5);
	ey=(int)gl_floor(maxy+0.5);
	if(sy==ey) return;

	//左側にある頂点と増加値の計算
	anum=bnum=m;
	do{
		anum++;
		if(anum>VerCou-1) anum=0;
	} while(lpVertex[anum].y<sy+0.5);
	m=anum-1;if(m<0) m=VerCou-1;

	dd=lpVertex[anum].y-lpVertex[m].y;
	dxA=(lpVertex[anum].x-lpVertex[m].x)/dd;
	dzA=(lpVertex[anum].z-lpVertex[m].z)/dd;
	dwA=(lpVertex[anum].w-lpVertex[m].w)/dd;
	x1=lpVertex[m].x;
	z1=lpVertex[m].z;
	w1=lpVertex[m].w;

	//右側にある頂点と増加値の計算
	do{
		bnum--;
		if(bnum<0) bnum=VerCou-1;
	} while(lpVertex[bnum].y<sy+0.5);
	m=bnum+1;if(m>VerCou-1) m=0;

	dd=lpVertex[bnum].y-lpVertex[m].y;
	dxB=(lpVertex[bnum].x-lpVertex[m].x)/dd;
	dzB=(lpVertex[bnum].z-lpVertex[m].z)/dd;
	dwB=(lpVertex[bnum].w-lpVertex[m].w)/dd;
	x2=lpVertex[m].x;
	z2=lpVertex[m].z;
	w2=lpVertex[m].w;

	for(y=sy;y<ey;y++){
		//左側稜線の処理
		if(lpVertex[anum].y<y+0.5){
			do{
				anum++;
				if(anum>VerCou-1) anum=0;
			} while(lpVertex[anum].y<y+0.5);
			oldm=anum-1;if(oldm<0) oldm=VerCou-1;

			dd=lpVertex[anum].y-lpVertex[oldm].y;
			dxA=(lpVertex[anum].x-lpVertex[oldm].x)/dd;
			dzA=(lpVertex[anum].z-lpVertex[oldm].z)/dd;
			dwA=(lpVertex[anum].w-lpVertex[oldm].w)/dd;
			x1=lpVertex[oldm].x;
			z1=lpVertex[oldm].z;
			w1=lpVertex[oldm].w;
		}
		//右稜線の処理
		if(lpVertex[bnum].y<y+0.5){
			do{
				bnum--;
				if(bnum<0) bnum=VerCou-1;
			} while(lpVertex[bnum].y<y+0.5);
			oldm=bnum+1;if(oldm>VerCou-1) oldm=0;

			dd=lpVertex[bnum].y-lpVertex[oldm].y;
			dxB=(lpVertex[bnum].x-lpVertex[oldm].x)/dd;
			dzB=(lpVertex[bnum].z-lpVertex[oldm].z)/dd;
			dwB=(lpVertex[bnum].w-lpVertex[oldm].w)/dd;
			x2=lpVertex[oldm].x;
			z2=lpVertex[oldm].z;
			w2=lpVertex[oldm].w;
		}

		//X軸方向の描画
		xx1=x1;xx2=x2;
		zz1=z1;zz2=z2;
		ww1=w1;ww2=w2;

		if(xx2<xx1){
			tmp=xx1;xx1=xx2;xx2=tmp;
			tmp=zz1;zz1=zz2;zz2=tmp;
			tmp=ww1;ww1=ww2;ww2=tmp;
		}
		xs1=(int)gl_floor(xx1+0.5);
		xs2=(int)gl_floor(xx2+0.5);

		wid=xx2-xx1;
		if(wid>0.0){
			//Z方向の増加値
			//（Xが1増加するたびにZはどれだけ増加するか）
			dz=(zz2-zz1)/wid;
			dw=(ww2-ww1)/wid;
		}

		//１ライン分を描画
		for(x=xs1;x<xs2;x++){
			int col = ctl.color;

			float nz = zz1/ww1;
			
			if(x >= 0 && x < ctl.pixels.w && y >= 0 && y < ctl.pixels.h) {
				float z = ctl.pixels.zfar[y * ctl.pixels.w + x];
				if(z == 0) z = 10000.0f;
				if(nz < z) {
					ctl.pixels.p[y * ctl.pixels.w + x] = col;
					ctl.pixels.zfar[y * ctl.pixels.w + x] = nz;
				}
			}

			zz1+=dz;		//Z方向の増加値の加算
			ww1+=dw;		//W方向の増加値の加算
		}

		x1+=dxA;x2+=dxB;	//X方向の増加値の加算
		z1+=dzA;z2+=dzB;	//Z方向の増加値の加算
		w1+=dwA;w2+=dwB;	//W方向の増加値の加算
	}
}
