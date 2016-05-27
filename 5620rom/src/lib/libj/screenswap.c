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

#undef screenswap
void
screenswap(bp, rect, screenrect)
#define screenswap Sscreenswap
	register Bitmap *bp;
	Rectangle rect;
	Rectangle screenrect;
{
	bitblt(&display, screenrect, bp, rect.origin, F_XOR);
	bitblt(bp, rect, &display, screenrect.origin, F_XOR);
	bitblt(&display, screenrect, bp, rect.origin, F_XOR);
}
