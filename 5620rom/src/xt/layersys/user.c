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
#include <layer.h>
#include <dmdproc.h>
#include <font.h>
#include "vector.h"
/* static inhibited; */
/* Texture16 *Ucursswitch(); */

extern  Bitmap	display;

#define MBUTTON 	1	/* for givemouse */
#define MPOSITION	2	/* for givemouse */

Point
#undef transform
transform(p)
#define transform Vtransform
	Point p;
{
#	define	o P->rect.origin
#	define	c P->rect.corner
	p.x=muldiv(c.x-o.x, p.x, XMAX)+o.x;
	p.y=muldiv(c.y-o.y, p.y, YMAX)+o.y;
#undef	o
#undef	c
	return p;
}
Rectangle
#undef rtransform
rtransform(r)
#define rtransform Vrtransform
	Rectangle r;
{
	r.origin=transform(r.origin);
	r.corner=transform(r.corner);
	return r;
}
char *
#undef Ualloc
Ualloc(n)
#define Ualloc VUalloc
	unsigned n;
{
	extern char *realalloc();
	return realalloc(n, (char *)P);
}
Bitmap *
#undef Uballoc
Uballoc(r)
#define Uballoc VUballoc
	Rectangle r;
{
	extern Bitmap *realballoc();
	return realballoc(r, (char *)P);
}
#undef Ujinit
Ujinit(){
#define Ujinit VUjinit
	P->inhibited=0;
	P->cursor=0;
}
#undef Ujline
Ujline(p, f)
#define Ujline VUjline
	Point p;
	Code f;
{
	Point next;
	next=add(P->curpt, p);
	Ujsegment(P->curpt, next, f);
}
#undef Ujlineto
Ujlineto(p, f)
#define Ujlineto VUjlineto
	Point p;
	Code f;
{
	Ujsegment(P->curpt, p, f);
}
#undef Ujmove
Ujmove(p)
#define Ujmove VUjmove
	Point p;
{
	P->curpt=add(P->curpt, p);
}
#undef Ujmoveto
Ujmoveto(p)
#define Ujmoveto VUjmoveto
	Point p;
{
	P->curpt=p;
}
#undef Ujpoint
Ujpoint(p, f)
#define Ujpoint VUjpoint
	Point p;
	Code f;
{
	Upoint(P->layer, transform(p), f);
}
#undef Ujrectf
Ujrectf(r, f)
#define Ujrectf VUjrectf
	Rectangle r;
	Code f;
{
	Urectf(P->layer, rtransform(r), f);
}
#undef Ujsegment
Ujsegment(p, q, f)
#define Ujsegment VUjsegment
	Point p, q;
	Code f;
{
	Point pp,qq;
	pp=transform(p);
	qq=transform(q);
	if(!eqpt(pp, qq))
		Usegment(P->layer, transform(p), transform(q), f);
	P->curpt=q;
}
extern Font defont;
Point
#undef Ujstring
Ujstring(s)
#define Ujstring VUjstring
	char *s;
{
	int x;
	Point p;
	p=string(&defont, s, P->layer, transform(P->curpt), F_XOR);
	x=p.x;
	P->curpt.x+=muldiv(jstrwidth(s),XMAX,P->rect.corner.x-P->rect.origin.x);
	while(transform(P->curpt).x<x)
		P->curpt.x++;
	return P->curpt;
}
#undef Ujtexture
Ujtexture(r, t, f)
#define Ujtexture VUjtexture
	Rectangle r;
	Texture *t;
	Code f;
{
	Utexture(P->layer, rtransform(r), t, f);
}
#undef Uexit
Uexit(){
#define Uexit VUexit
	/* extern windowstart(); */
	shutdown(P);
	exec(Sys[158]); /* windowstart */
}
#undef Urequest
Urequest(r)
#define Urequest VUrequest
{
	extern struct Proc *kbdproc;
	register struct Proc *p=P;
	if(r&KBD)
		p->state|=KBDLOCAL;
	else
		p->state&=~KBDLOCAL;
	if(r&MOUSE){
		p->state|=MOUSELOCAL;
		if(kbdproc==p){
			p->state|=GOTMOUSE;
			givemouse(p,MBUTTON|MPOSITION);
		}
		sleep(1);	/* Let control() update the mouse */
	}else{
		/* Ucursallow(); */
		Ucursswitch((Texture16 *) 0);
		p->state&=~(GOTMOUSE|MOUSELOCAL);
	}
	if(r&ALARM)
		p->state|=ALARMREQD;
	else
		p->state&=~ALARMREQD;

	return(r);
}
#undef Urcvchar
Urcvchar(){
#define Urcvchar VUrcvchar
	if(P->nchars==0)
		return -1;
	return(getchar());
}
#undef Ukbdchar
Ukbdchar(){
#define Ukbdchar VUkbdchar
	if((P->state&KBDLOCAL)==0)
		return -1;
	return(qgetc(&P->kbdqueue));
}
#undef Uown
Uown()
#define Uown VUown
{
	register got=CPU|SEND|PSEND;
	if(P->state&GOTMOUSE)
		got|=MOUSE;
	if(P->kbdqueue.c_cc>0)
		got|=KBD;
	if(P->nchars>0)
		got|=RCV;
	if(P->state&ALARMREQD && P->nticks== 0)
		got|=ALARM;
	return got;
}
#undef Uwait
Uwait(r)
#define Uwait VUwait
	register r;
{
	register u;

	if((r&RCV) && P->nchars==0)
		mpxublk(P);
	sw(1);
	spl1();
	while((u=Uown()&r)==0 && (r&CPU)==0){
		spl0();
		sw(0);
		spl1();
	}
	spl0();
	return u;
}
#undef Ucursallow
Ucursallow(){
#define Ucursallow VUcursallow
	if (P->inhibited > 0)
		P->inhibited--;
	/* if the mouse cursor is not otherwise inhibited, it will
	 * appear again after two clock ticks -- on the first,
	 * cursallow will be called, and on the next it'll show up */
}
#undef Ucursset
Ucursset(p)
#define Ucursset VUcursset
	Point p;
{
	if(P->state&GOTMOUSE){
		cursset(p);
		givemouse(P,MBUTTON|MPOSITION);
	}
}
#undef Ucursinhibit
Ucursinhibit(){
#define Ucursinhibit VUcursinhibit
	register  x;

	if (P->inhibited++ == 0) {
		x = spl4();
		clipcursor();
		splx(x);
	}
}
Texture16 *
#undef Ucursswitch
Ucursswitch(t)
#define Ucursswitch VUcursswitch
	register Texture16 *t;
{
	Texture16 *ot;
	ot=0;
	if(P->state&MOUSELOCAL){
		ot=P->cursor;
		if((whichlayer() == P->layer) && (P->state&GOTMOUSE))
			cursswitch(t);
		P->cursor=t;
	}
	sleep(1);
	return ot;
}
Point
#undef string
string(f,s,b,p,fc)
#define string Vstring
	register Font *f;
	char *s;
	register Layer *b;
	Point p;
	int fc;
{
	register c;
	int full = (fc == F_STORE);
	Point q;
	Rectangle r;
	register Fontchar *i;
	register inhibited=0;
	if(fc!=F_XOR && b->base<=(display.base +(25*1024))){
		Ucursinhibit();
		inhibited=1;
	}
	if (full) {
		r.origin.y = 0;
		r.corner.y = f->height;
	}
	for (; c = *s++; p.x += i->width) {
		i = f->info + c;
		if (!full) {
			r.origin.y = i->top;
			r.corner.y = i->bottom;
		}
		r.origin.x = i->x;
		r.corner.x = (i+1)->x;
		q.x = p.x+((i->left & 0x80) ? i->left | 0xffffff80 : i->left);
		q.y = p.y+r.origin.y;
		if (b->obs == 0)
			bitblt(f->bits,r,b,q,fc);
		else
			lblt(f->bits,r,b,q,fc);
	}
	if(inhibited)
		Ucursallow();
	return(p);
}
#undef Usendchar
Usendchar(c)
#define Usendchar VUsendchar
	char c;
{
	while(mpxsendchar(c, P) == -1)
		sw(1);
	return (1);	/* always return 1 in layers */
}
#undef Upoint
Upoint(l, p, f)
#define Upoint VUpoint
	register Layer *l;
	Point p;
{
	register inhibited=0;
	if(f!=F_XOR && l->base<=(display.base +(25*1024))){
		Ucursinhibit();
		inhibited=1;
	}
	lpoint(l, p, f);
	if(inhibited)
		Ucursallow();
}
#undef Ubitblt
Ubitblt(s, r, d, p, f)
#define Ubitblt VUbitblt
	Layer *s, *d;
	Rectangle r;
	Point p;
{
	register inhibited=0;
	if((f!=F_XOR && d->base<=(display.base+(25*1024))) 
		|| s->base<=(display.base + (25*1024))){
		Ucursinhibit();
		inhibited=1;
	}
	lbitblt(s, r, d, p, f);
	if(inhibited)
		Ucursallow();
}
#undef Urectf
Urectf(l, r, f)
#define Urectf VUrectf
	Layer *l;
	Rectangle r;
{
	register inhibited=0;
	if(f!=F_XOR && l->base<=(display.base + (25*1024))){
		Ucursinhibit();
		inhibited=1;
	}
	lrectf(l, r, f);
	if(inhibited)
		Ucursallow();
}
#undef Usegment
Usegment(l, p, q, f)
#define Usegment VUsegment
	Layer *l;
	Point p, q;
{
	register inhibited=0;
	if(f!=F_XOR && l->base<=(display.base + (25*1024))){
		Ucursinhibit();
		inhibited=1;
	}
	lsegment(l, p, q, f);
	if(inhibited)
		Ucursallow();
}
#undef Utexture
Utexture(l, r, t, f)
#define Utexture VUtexture
	Layer *l;
	Rectangle r;
	Texture *t;
{
	register inhibited=0;
	if(f!=F_XOR && l->base<=(display.base + (25*1024))){
		Ucursinhibit();
		inhibited=1;
	}
	/* speed hack; texture clips anyway */
	if(l->obs)
		ltexture(l, r, t, f);
	else
		texture(l, r, t, f);
	if(inhibited)
		Ucursallow();
}
#undef Uscreenswap
Uscreenswap(b, r, s)
#define Uscreenswap VUscreenswap
	Bitmap *b;
	Rectangle r, s;
{
	cursinhibit();
	screenswap(b, r, s);
	cursallow();
}



#undef Upolyf
Upolyf (l, pa, n, t, f)
#define Upolyf VUpolyf
	Layer *l;
	Point *pa;
	short n;
	Texture *t;
	Code f;
{
	register inhibited=0;
	if(f!=F_XOR && l->base<=(display.base + (25*1024))){
		Ucursinhibit();
		inhibited=1;
	}
	polyf (l, pa, n, t, f);
	if(inhibited)
		Ucursallow();
}


#undef Uclipbttn
Uclipbttn()
#define Uclipbttn VUclipbttn
{

	if ((P->state & GOTMOUSE) && mouse.buttons && (P->layer == whichlayer()))
		return(mouse.buttons);
	else
		return(0);
}





