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

#undef lpoint
lpoint(l, p, f)
#define lpoint Slpoint
	register Layer *l;
	Point p;
	register Code f;
{
	register Obscured *o;
	if(ptinrect(p, l->rect)){
		for(o=l->obs; o; o=o->next)
			if(ptinrect(p, o->rect)){
				point(o->bmap, p, f);
				return;
			}
		point(l, p, f);
	}
}
