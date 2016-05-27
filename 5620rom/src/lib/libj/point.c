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
#undef point
void
point(b, pt, f)
#define point Spoint
	register Bitmap *b;
	Point pt;
	register f;
{
	register bit;
	register Word *p=addr(b, pt);
	if(ptinrect(pt, b->rect)==0)
		return;
	bit=1<<(WORDSIZE-1)-(pt.x&WORDMASK);
	if (f == F_XOR)
		*p ^= bit;
	else
		if (f == F_CLR)
			*p &= ~bit;
		else
			*p |= bit;
}
