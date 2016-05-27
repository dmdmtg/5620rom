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
Lrectf(l, r, db, f, o)
	Layer *l;
	Rectangle r;
	Bitmap *db;
	Code *f;
	Obscured *o;
{
	rectf(db, r, *f);
}
#undef lrectf
lrectf(l, r, f)
#define lrectf Slrectf
	Layer *l;
	Rectangle r;
	Code f;
{
	layerop(l, Lrectf, r, f,0,0);
}
