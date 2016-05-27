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
#undef ptinrect
ptinrect(p, r)
#define ptinrect Sptinrect
	Point p;
	Rectangle r;
{
	return(p.x>=r.origin.x && p.x<r.corner.x
	    && p.y>=r.origin.y && p.y<r.corner.y);
}
