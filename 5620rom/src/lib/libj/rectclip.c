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
#undef rectclip
rectclip(rp, b)		/* first by reference, second by value */
#define rectclip Srectclip
	register Rectangle *rp;
	Rectangle b;
{
	register Rectangle *bp= &b;
	/*
	 * Expand rectXrect() in line for speed
	 */
#define c corner
#define o origin
	if((rp->o.x<bp->c.x && bp->o.x<rp->c.x &&
	    rp->o.y<bp->c.y && bp->o.y<rp->c.y)==0)
		return 0;
	/* They must overlap */
	if(rp->origin.x<bp->origin.x)
		rp->origin.x=bp->origin.x;
	if(rp->origin.y<bp->origin.y)
		rp->origin.y=bp->origin.y;
	if(rp->corner.x>bp->corner.x)
		rp->corner.x=bp->corner.x;
	if(rp->corner.y>bp->corner.y)
		rp->corner.y=bp->corner.y;
	return 1;
}
