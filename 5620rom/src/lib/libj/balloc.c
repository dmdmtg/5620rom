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
/*
 * All Bitmaps come from here
 */
/*VARARGS*/
Bitmap *
/* A Rectangle......! */
#undef realballoc
realballoc(r, caller)
#define realballoc Srealballoc
Rectangle  r; 
char	*caller;
{
	register short	rox = r.origin.x, roy = r.origin.y;
	register short	rcx = r.corner.x, rcy = r.corner.y;
	register short	left, right;
	register Bitmap *bp;

	if ((bp = (Bitmap * )realalloc(sizeof(Bitmap), caller)) == 0)
		return 0;
	left = rox & ~WORDMASK;
	if ((right = rcx) & WORDMASK) {
		right |= WORDMASK;
		right++;
	}
	bp->width = (right - left) >> WORDSHIFT;
	bp->rect.origin.x = rox;
	bp->rect.origin.y = roy;
	bp->rect.corner.x = rcx;
	bp->rect.corner.y = rcy;
	if (realgcalloc(((unsigned long)bp->width << 2) * (rcy - roy),
	    (long **) & bp->base, caller) == 0) {
		free((char *) bp);
		return 0;
	}
	bp->_null = 0;
	rectf(bp, bp->rect, F_OR);
	return bp;
}

#undef balloc
Bitmap *
balloc(r)
#define balloc Sballoc
Rectangle r;
{
	return realballoc(r, (char *)0);
}


#undef bfree
void
bfree(bp)
#define bfree Sbfree
register Bitmap *bp;
{
	if (bp) {
		gcfree((char *)bp->base);
		free((char *)bp);
	}
}
