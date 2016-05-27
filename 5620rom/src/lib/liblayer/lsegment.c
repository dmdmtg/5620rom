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
#include <line.h>

/*ARGSUSED*/
static void
Lsegment(l, r, db, fp, o)
	Layer *l;
	Rectangle r;
	Bitmap *db;
	register struct l_args {
		Point p0, p1;
		Code f;
	}*fp;
	Obscured *o;
{
	Jclipline(db, r, fp->p0, fp->p1, fp->f);
}

#undef lsegment
lsegment(l, p, q, f)
#define lsegment Slsegment
	Layer *l;
	Point p, q;
	Code f;
{
	Point p0, p1;
	p0=p;
	if(p.x==q.x && p.y==q.y)
		return;
	p1=Jsetline(p, q);
	layerop(l, Lsegment, l->rect, p0, p1, f);
}
