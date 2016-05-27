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
/*
 * Move a layer to the front of the screen
 */
#include <dmd.h>
#include "layer.h"

#undef lfront
#undef lback
Layer *lfront, *lback;
#define lfront Slfront
#define lback Slback

#undef upfront
upfront(lp)
#define upfront Supfront
	register Layer *lp;
{
	register Layer *fr;
	register Obscured *op, *next;
	if(lp==0 || lp->front==0)
		return 1;	/* Why continue? */
	cursinhibit();
    for(fr=lp->front; fr; fr=fr->front){
	/* Do they overlap? */
	if(rectXrect(fr->rect, lp->rect)==0)	/* Nothing to do */
		continue;
	for(op=lp->obs; op; op=next){
		next=op->next;	/* remember... */
		if(op->lobs==fr){	/* overlap */
			/*
			 * Swap obscured portion and screen
			 */
			screenswap(op->bmap, op->rect, op->rect);
			/*
			 * Put the obscured box into the new rear layer
			 */
			if(op->prev)
				op->prev->next=op->next;
			else		/* first in list */
				lp->obs=op->next;
			if(op->next)
				op->next->prev=op->prev;
			/* Put in fr */
			op->next=fr->obs;
			op->prev=0;
			op->lobs=lp;
			if(fr->obs)
				fr->obs->prev=op;
			fr->obs=op;
		}
	}
    }
    /*
     * Put lp at front
     */
    if(lfront!=lp){
	if(lback==lp)
		lback=lp->front;
	if(lp->back)
		lp->back->front=lp->front;
	lp->front->back=lp->back;
	lfront->front=lp;
	lp->back=lfront;
	lp->front=0;
	lfront=lp;
    }
    /*
     * lp is now at the front, so chain along all layers and mark any
     * obscured boxes behind lp as obscured by lp
     */
    for(fr=lback; fr!=lp; fr=fr->front)
	for(op=fr->obs; op; op=op->next)
		if(rectXrect(op->rect, lp->rect))
			op->lobs=lp;
    cursallow();
    return(1);
}
