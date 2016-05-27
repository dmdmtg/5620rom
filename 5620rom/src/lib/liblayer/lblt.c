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
#include "layer.h"
/*ARGSUSED*/
static void
Lblt(l, r, db, fp, o)
	Layer *l;
	Rectangle r;
	Bitmap *db;
	register struct l_args {
		Bitmap *sb;
		Point q;
		Code f;
	} *fp;
	Obscured *o;
{
	bitblt(fp->sb, rsubp(r, fp->q), db, r.origin, fp->f);
}
#undef lblt
lblt(b, r, l, p, f)
#define lblt Slblt
	Bitmap *b;
	Rectangle r;
	Layer *l;
	Point p;
	Code f;
{
	Point q;
	Rectangle dr;
	q=sub(p, r.origin);
	dr=raddp(r, q);
	layerop(l, Lblt, dr, b, q, f);
}
