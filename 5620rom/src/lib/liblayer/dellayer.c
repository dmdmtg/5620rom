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


#undef dellayer
dellayer(l)
#define dellayer Sdellayer
	register Layer *l;
{
	register Obscured *op;
	register Layer *l2;
	if(l==0)
		return 0;
	if(upfront(l)==0)
		return -1;
	Lgrey(l->rect);	/* Make rectangle into background */
	downback(l);
	/*
	 * Window to be deleted is now at rear; free the obscure's
	 */
	for(op=l->obs; op; op=op->next){
		bfree(op->bmap);
		free((char *)op);
	}
	/*
	 * Remove layer from list
	 */
	lback=l->front;
	if(lfront==l)
		lfront=0;
	if(l->front)
		l->front->back=0;
	for(l2=lfront;l2;l2=l2->back)
		if (rectXrect(l2->rect,l->rect))
			obsclean(l2);
	free((char *)l);
	return 0;
}
