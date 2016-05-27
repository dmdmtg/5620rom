/* */
/*									*/
/*	Copyright (c) 1985,1986,1987,1988,1989,1990,1991,1992   AT&T	*/
/*			All Rights Reserved				*/
/*									*/
/*	  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.		*/
/*	    The copyright notice above does not evidence any		*/
/*	   actual or intended publication of such source code.		*/
/*									*/
/* */
#include <dmd.h>
#include <cursor.h>
#undef mouse
#undef stdcurtab
#undef usercurtab
#undef curtabp
struct Mouse mouse;
static short c_ul[]={
	0xFF80,	0xFE00,	0xF800,	0xFC00,
	0xFE00,	0xDF00,	0xCF80,	0x87C0,
	0x83E0,	0x01F0,	0x00F8,	0x007C,
	0x003E,	0x001F,	0x000E,	0x0004,
};
static short c_ll[]={
	0x0004,	0x000E,	0x001F,	0x003E,
	0x007C,	0x00F8,	0x01F0,	0x83E0,
	0x87C0,	0xCF80,	0xDF00,	0xFE00,
	0xFC00,	0xF800,	0xFE00,	0xFF80,
};
static short c_ur[]={
	0x01FF,	0x007F,	0x001F,	0x003F,
	0x007F,	0x00FB,	0x01F3,	0x03E1,
	0x07C1,	0x0F80,	0x1F00,	0x3E00,
	0x7C00,	0xF800,	0x7000,	0x2000,
};
static short c_lr[]={
	0x2000,	0x7000,	0xF800,	0x7C00,
	0x3E00,	0x1F00,	0x0F80,	0x07C1,
	0x03E1,	0x01F3,	0x00FB,	0x007F,
	0x003F,	0x001F,	0x007F,	0x01FF,
};
static short box3[]= {
	 0x40FF, 0xE001, 0x7001, 0x3801,
	 0x1C01, 0x0E41, 0x07C1, 0x03C1,
	 0x83C1, 0x87C1, 0x8001, 0x8001,
	 0x8001, 0x8001, 0x8001, 0xFFFF,
};
static short box4[]={
	 0xFF02, 0x8007, 0x800E, 0x801C,
	 0x8038, 0x8270, 0x83F0, 0x83E0,
	 0x83C1, 0x83E1, 0x8001, 0x8001,
	 0x8001, 0x8001, 0x8001, 0xFFFF,
};
static short box1[]={
	 0xFFFF, 0x8001, 0x8001, 0x8001,
	 0x8001, 0x8001, 0x87C1, 0x83C1,
	 0x03C1, 0x07C1, 0x0E41, 0x1C01,
	 0x3801, 0x7001, 0xE001, 0x40FF,
};
static short box2[]={
	 0xFFFF, 0x8001, 0x8001, 0x8001,
	 0x8001, 0x8001, 0x83E1, 0x83C1,
	 0x83C0, 0x83E0, 0x8270, 0x8038,
	 0x801C, 0x800E, 0x8007, 0xFF02,
};
struct curtab{
	short	*map;
	short	dx;
	short	dy;
}stdcurtab[4]={		/* 4 quadrants */
	c_ur,	-15,	0,
	c_ul,	0,	0,
	c_lr,	-15,	-15,
	c_ll,	0,	-15,
},
 boxcurtab[4]={
	box1,	-10,	-6,
	box2,	-6,	-6,
	box3,	-10,	-10,
	box4,	-6,	-10,
}, usercurtab[4];
#define stdcurtab Sstdcurtab
#define usercurtab Susercurtab
struct curtab *curtabp;
#define curtabp Scurtabp
#undef cursor
struct Cursor cursor;
#define  cursor Scursor
/*static*/ int	ud, lr;
#define	QRL_MASK	01
#define	QUD_MASK	02
#define	QRIGHT		0
#define	QLEFT		01
#define	QUP		0
#define	QDOWN		02
#define	QLEFT_MARGIN	XMAX/3
#define	QRIGHT_MARGIN	XMAX*2/3
#define	QUP_MARGIN	YMAX/3
#define	QDOWN_MARGIN	YMAX*2/3

newquad(x, y, quad)
	register x, y, quad;
{
	ud=quad&QUD_MASK;
	lr=quad&QRL_MASK;
	if(x < QLEFT_MARGIN)
		lr=QLEFT;
	if(x > QRIGHT_MARGIN)
		lr=QRIGHT;
	if(y < QUP_MARGIN)
		ud=QUP;
	if(y > QDOWN_MARGIN)
		ud=QDOWN;
	return ud|lr;
}
#undef cursinhibit
void
cursinhibit(){
#define cursinhibit Scursinhibit
	register sr=spl1();
	if(cursor.inhibit++==0 && cursor.up)
		flipcursor(cursor.x, cursor.y, cursor.quad);
	splx(sr);
}
#undef cursallow
void
cursallow(){
#define cursallow Scursallow
	if(cursor.inhibit>0)
		cursor.inhibit--;
}
#define	MOUSEMAX	2048	/* 1/2 resolution of mouse registers */
/*static*/ struct data {
	int	last;
	int	zero;
}xdata,	ydata;
static int
update(var, datap, max)
	register var, max;
	register struct	data *datap;
{
	register d;
	d=var-datap->last;
	if(d>MOUSEMAX)
		datap->zero+=MOUSEMAX*2;
	else if(d<-MOUSEMAX)
		datap->zero-=MOUSEMAX*2;
	datap->last=var;
	d=var-datap->zero;	/* re-use of d */
	if(d<0){
		datap->zero=var;
		d=0;
	}else if(d>=max){
		datap->zero=var-max;
		d=max-1;
	}
	return d;
}
static
setxy(){
	register struct	curtab *qp;
	register x, y, quad;
	x=  *XMOUSE & 0xfff;	/* coordinates go the wrong way, so negate */
	y= -(*YMOUSE & 0xfff);
	cursor.oldx=cursor.x;
	cursor.oldy=cursor.y;
	cursor.oldquad=cursor.quad;
	x=update(x, &xdata, XMAX);
	y=update(y, &ydata, YMAX);
	quad=newquad(x,	y, cursor.quad);
	if ((quad != cursor.quad) && (curtabp == boxcurtab)) {
		if ((quad & 1) == QLEFT && (cursor.quad & 1) == QRIGHT)
			x += 4;
		if ((quad & 1) == QRIGHT && (cursor.quad & 1) == QLEFT)
			x -= 4;
		if ((quad & 2) == QUP && (cursor.quad & 2) == QDOWN)
			y += 4;
		if ((quad & 2) == QDOWN && (cursor.quad &2) == QUP)
			y -= 4;
	}
	qp= &curtabp[quad];
	if(x< -qp->dx)
		x= -qp->dx;
	if(x+(16+qp->dx)>XMAX)
		x=XMAX-(16+qp->dx);
	if(y< -qp->dy)
		y= -qp->dy;
	if(y+(16+qp->dy)>YMAX)
		y=YMAX-(16+qp->dy);
	if(x==cursor.oldx && y==cursor.oldy && cursor.up)
		return(0);
	cursor.x=x;
	cursor.y=y;
	cursor.quad=quad;
	mouse.xy.x=x;
	mouse.xy.y=y;
	mouse.jxy=mouse.xy;
	return(1);
}
#undef cursset
void
cursset(p)
#define cursset Scursset
	Point p;
{
	xdata.zero=xdata.last-p.x;
	ydata.zero=ydata.last-p.y;
}
Bitmap cursbm;
static
flipcursor(x, y, q)
{
	register struct	curtab *qp;
	register s;
	qp= &curtabp[q];
	cursbm.base=(Word *) qp->map;
	cursblt(&display, qp->map, Pt(x+qp->dx,	y+qp->dy));
		/* almost equal	to
		/* bitblt(&cursbm, cursbm.rect,	&display,
		/*		Pt(x+qp->dx, y+qp->dy),	F_XOR);	  */
	cursor.up=1-cursor.up;
}
#undef interrupt
short interrupt;
#define interrupt Sinterrupt
#undef cursinit
cursinit(){
#define cursinit Scursinit
	curtabp=stdcurtab;
	cursbm.width=1;
	cursbm.rect.corner.x=cursbm.rect.corner.y=16;
	cursor.inhibit = 0;
}
#undef auto1
auto1(){
#define auto1 Sauto1
	if(setxy() && cursor.inhibit==0){
		if(cursor.up)
			flipcursor(cursor.oldx,	cursor.oldy, cursor.oldquad);
		flipcursor(cursor.x, cursor.y, cursor.quad);
	}
	interrupt++;
}
#undef nap
void nap(n){
#define nap Snap
	if(n<=0)
		return;
	interrupt=0;
	do; while(interrupt<n);
}

/* static*/ Texture *old;
Texture	*
#undef cursswitch
cursswitch(p)
#define cursswitch Scursswitch
	register Texture *p;
{
	register struct	curtab *ctp;
	register Texture *t=old;
	old=p;
	cursinhibit();
	if(p==0)
		curtabp=stdcurtab;
	else if (p == (Texture *) -1)
		curtabp=boxcurtab;
	else{
		curtabp=usercurtab;
		for(ctp=usercurtab;
			 ctp<&usercurtab[4];
				ctp++){
			ctp->map=(short	*)p;
			ctp->dx=ctp->dy= -8;
		}
	}
	cursallow();
	return t;
}
