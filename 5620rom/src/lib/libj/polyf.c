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
/*
*	Polygon Filling Algorithm
*/
#include <dmd.h>

#define HUGE 2048
/* #define POLY_F -32768	 This constant is moved to dmd.h */
#define isbreak(A) (A->x == POLY_F)
#define paint(B,X1,X2,Y,C,F) ltexture(B,Rect(X1,Y,X2,Y+1),C,F);
#define CV(A) (short)A

typedef short number;
typedef long acnumber;

typedef struct seg {
	number x, y, X, Y;
	char stat;
	}  segS, * segP;

 segP sinp;
 short pbottom;

#undef polyf
polyf( bp, pol, n, t, f)
#define polyf Spolyf
Bitmap	*bp;
Point	pol[];
short	n;
Texture	*t;
Code	f;
{
	return pfill( bp, pol, n, t, Pt(0,0), f );
}

#undef ptinpoly
ptinpoly( p, pol, n)
#define ptinpoly Sptinpoly
Point	p;
Point	pol[];
short	n;
{
	return pfill( (Bitmap *)0, pol, n, (Texture *)0, p, (Code)0);
}


pfill( bp, pol, n, color, marker, code)
Bitmap	*bp;
Point	pol[];
short	n;
Texture *color;
Point	marker;
Code	code;
{
	register i, j, k;
	register Point *pp, *opp;
	register segP cur_side;

	pbottom = HUGE;		/* init. to too big a value for fill */

	if( (
	sinp = (segP)alloc((unsigned)((n+8)*sizeof(segS)))
	) == (segP)0) return(-1);

	cur_side = sinp;

	for(i=0,opp=pp=pol,k=0; i<n; i++,pp++) {
		if(isbreak(pp)){
			j = mk_sides(opp,pp,cur_side);
			cur_side += j;
			k += j;
			opp = pp + 1;
		}
	}
	k += mk_sides(opp,pp,cur_side);

	if( k )	i = realfill(bp,(short)k,color,marker,code);
	else	i = 0;

	free(sinp);
	return(i);
}

mk_sides(p1,p2,sides)
	Point *p1, *p2;
	segP sides;
{
	register Point * qq, * pp;
	register segP sp;
	number x, y, X, Y;
	short skount;	/* how many sides found */

	if(p2<=p1) return(0);
	qq = p2-1;	/* last point */
	x = qq->x;	y = qq->y;
	for(pp=p1,sp=sides,skount=0; pp<p2; pp++){
		X = pp->x;	Y = pp->y;
		if(Y<pbottom) pbottom = Y;
		sp->x = x;	sp->y = y;
		sp->X = X;	sp->Y = Y;
		sp->stat = 1;
		if(sp->Y > sp->y) sgswitch(sp);
		else if(sp->Y==sp->y && sp->X>sp->x) sgswitch(sp);
		x = X;	y = Y;
		sp++; skount++;
	}
	ed_sides(sides,skount);
	return(skount);
}

sgswitch(p) segP p;
{
	number xyt;
	xyt = p->y; p->y = p->Y; p->Y = xyt;
	xyt = p->x; p->x = p->X; p->X = xyt;
}

ed_sides(sides,n)
	segP sides;
{
	register segP sp;
	register segP sbf, snx;
	segP sfin;

	/* edit horizontals */
	sfin = sides + n;
	for(sp=sides;sp<sfin;sp++){
		if(!(sp->stat)) continue;
		if(sp->y != sp->Y) continue;
		if(sp==sides) sbf = sp + n - 1; 
		else sbf = sp-1;
		if(sp<sfin) snx = sp+1;
		else snx = sides;
		if(sbf->y==sp->y){	/* point before is below*/
			if(snx->y==sp->y){	/* both below */
				sp->stat = 0;
			}
			continue;
		}
		else if(sbf->Y==sp->y){	/*previous point above*/
			if(snx->Y==sp->y){	/* both above */
				sp->stat = 0;
			}
			continue;
		}
	}
}
/* Filling interior of arbitrary polygons */

#define COMP(A)  if(p1->A>p2->A) return(-1); else if(p1->A<p2->A) return(1);

place(p1,p2) segP p1, p2;
{
	COMP(y) else { 
		COMP(x) else { 
			COMP(X) else { 
				COMP(Y) else return(0);
			}
		}
	}
}

/* Fill a contour defined by a set of line segments */

realfill(bp,n_sides,shade,marker,code)
	Bitmap *bp;
	short n_sides;
	Texture * shade;
	Point marker;
	Code code;
{
	register i;
	register segP  sp;
	segP s1, s2, sfin, st;
	short yb;

	qsort(sinp,n_sides,sizeof(struct seg),place);

	sfin = sinp + n_sides;
	sp = sinp;
	i = 0;
	s1 = sp;
	yb = s1->y;
	for(;yb<=sp->y;sp++,i++);
	s2 = sp - 1;
	for(;yb>=pbottom;yb--){

		if( !shade && yb<marker.y ) return(0); /* for ptinpoly() */
		if( draw(bp,yb,s1,s2,shade,marker,code) ) return(1); /* "" */

		for(st=s1;st<=s2;st++){
			if(!(st->stat)) continue;
			if(yb<=st->Y) st->stat=0;
		}
		if(sp >= sfin) continue; /*nomore*/
		if(i>=n_sides) continue;
		for(; yb<=(sp->y+1)&&i<n_sides; sp++,i++){
			/*we have gone below next element */
			for(st=s1;st<=s2;st++){
				if(sp->y==st->Y){
					if(sp->x==st->X) st->stat=0;
					else if(sp->Y==sp->y&&sp->X==st->X) st->stat=0;
				}
			}
		}
		/*cleanup*/
		s2 = sp - 1;
		while(s1<sfin && !(s1->stat)) s1++;
	}
	return(0);
}

#define NCUTS 32
#define NCUTS_1 31

draw(bp,y,pr1,pr2,shade,marker,code)
	Bitmap	*bp;
	short	y;
	segP	pr1, pr2;
	Texture	*shade;
	Point	marker;
	Code	code;
{
	register segP pr;
	register i, j, k, n;
	short x[NCUTS];

	for(pr=pr1,n=0; pr<=pr2 && n<NCUTS_1 ; pr++)
		if(pr->stat && pr->y!=pr->Y) x[n++] = xcut(y,pr);

	if(n<2) return(0);			/* for ptinpoly() */
	do {
		k = 0;
		for(i=1;i<n;i++){
			if(x[i-1]>x[i]) {
				j=x[i-1]; 
				x[i-1]=x[i]; 
				x[i]=j; 
				k=1;
			}
		}
	} while(k);

	if(!shade) {				/* for ptinpoly() */
		if(marker.y != y) return(0);
		for(i=1;i<n;i +=2){
			if(x[i-1] <= marker.x && marker.x <=x[i]) return(1);
		}
		return(0);
	}

	for(i=1;i<n;i +=2) paint(bp,x[i-1],x[i],y,shade,code);	/* do the fill */
	return(0);
}

xcut(y,sgp)
	segP sgp;
{
	acnumber dx, dy, xy;
	acnumber x, x1, y1, x2, y2;
	x1 = sgp->x;	y1 = sgp->y;
	x2 = sgp->X;	y2 = sgp->Y;

	if(y1==y2) {
		x = x1>x2 ? x1: x2;
		return(CV(x));
	}

	dy = y1-y2; 
	dx = x1-x2; 
	xy = y1*x2-x1*y2;
	x = (y*dx+xy)/dy;	/*dy non zero because of prev. check*/
	return(CV(x));
}





