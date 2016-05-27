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
#define	INSET	2
#undef Lbox
Lbox(l)
#define Lbox SLbox
	register Layer *l;
{
	cursinhibit();
	lrectf(l, l->rect, F_STORE);
	lrectf(l, inset(l->rect, INSET), F_CLR);
	cursallow();
}
#undef Lgrey
Lgrey(r)
#define Lgrey SLgrey
	Rectangle r;
{
	cursinhibit();
	texture(&display, r, &T_background, F_STORE);
	cursallow();
}
