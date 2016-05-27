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
/*ARGSUSED*/
static void
Ltexture(l, r, db, fp, o)
	Layer *l;
	Rectangle r;
	Bitmap *db;
	register struct l_args {
		Word *map;
		Code f;
	}*fp;
	Obscured *o;
{
	texture(db, r, fp->map, fp->f);
}
#undef ltexture
ltexture(l, r, map, f)
#define ltexture Sltexture
	Layer *l;
	Rectangle r;
	Word *map;
	Code f;
{
	layerop(l, Ltexture, r, map, f,0);
}
