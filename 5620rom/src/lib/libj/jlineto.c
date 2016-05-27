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
#include <line.h>
/*static*/ x0, y0;
/* extern Bitmap display; */


#undef jmoveto
void
jmoveto(xy)
#define jmoveto Sjmoveto
	Point xy;
{
	PtCurrent=xy;
}
#undef jlineto
void
jlineto(xy, f)
#define jlineto Sjlineto
	Point xy;
{
	jsegment(PtCurrent, xy, f);
	PtCurrent=xy;
}
#undef jmove
void
jmove(xy)
#define jmove Sjmove
	Point xy;
{
	PtCurrent.x+=xy.x;
	PtCurrent.y+=xy.y;
}
#undef jline
void
jline(xy, f)
#define jline Sjline
	Point xy;
{
	Point next;
	next=add(PtCurrent, xy);
	jsegment(PtCurrent, next, f);
	PtCurrent=next;
}
#undef jsegment
void
jsegment(p, q, f)
#define jsegment Sjsegment
	Point p, q;
{
	segment(&display, p, q, f);
	PtCurrent=q;
}

#undef segment
void
segment(b, p, q, f)
#define segment Ssegment
	Bitmap *b;
	Point p, q;
{
	if(p.x==q.x && p.y==q.y)
		return;
	q=Jsetline(p, q);
	Jclipline(b, b->rect, p, q, f);
}
#undef floor
short
floor(x,y)
#define floor Sfloor
	register long x;
	register y;
{
	if (y<=0) {
		if (y==0) return((short) x);
		y= -y; x= -x;
	}
	if (x<0) x -= y-1;
	return(x/y);
}
#undef ceil
short
ceil(x,y)
#define ceil Sceil
	register long x;
	register y;
{
	if (y<=0) {
		if (y==0) return((short) x);
		y= -y; x= -x;
	}
	if (x>0) x += y-1;
	return(x/y);
}
Jminor(x)
	register x;
{
	register y;
	y=floor(2*(long)(x-x0)*Jdminor+Jdmajor, 2*Jdmajor)+y0;
	return Jslopeneg? -y : y;
}
Jmajor(y)
	register y;
{
	register x;
	x=ceil(2*(long)((Jslopeneg? -y : y)-y0)*Jdmajor-Jdminor, 2*Jdminor)+x0;
	if(Jdminor)
		while(Jminor(x-1)==y)
			x--;
	return x;
}
Point
Jsetline(p, q)
	Point p, q;
{
	register dx, dy, t;
	Point endpt;
	short swapped=0;
	Jxmajor=1;
	Jslopeneg=0;
	dx=q.x-p.x;
	dy=q.y-p.y;
	if(abs(dy) > abs(dx)){	/* Steep */
		Jxmajor=0;
#define XYswap(p)	t=p.x; p.x=p.y; p.y=t
		XYswap(p);
		XYswap(q);
#define Swap(x, y)	t=x; x=y; y=t
		Swap(dx, dy);
	}
	if(dx<0){
		swapped++;
		Swap(p.x, q.x); Swap(p.y, q.y);
		dx= -dx; dy= -dy;
	}
	if(dy<0){
		Jslopeneg++;
		dy= -dy; p.y= -p.y; q.y= -q.y;
	}
	Jdminor=dy;
	Jdmajor=dx;
	x0=p.x;
	y0=p.y;
	endpt.x=swapped? p.x+1 : q.x-1;
	endpt.y=Jminor(endpt.x);
	if(!Jxmajor){
		XYswap(endpt);
	}
	return(endpt);
}
Jsetdda(x)
	register x;
{
	register y;
	y=Jminor(x);
	if(Jslopeneg)
		y= -y;
	return (short)((2*(x-x0)+2)*(long)Jdminor
		-(2*(y-y0)+1)*(long)Jdmajor);
}
