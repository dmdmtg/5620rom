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




Obscured *Ohead,*Otail;  /* the new obscured list being generated */
int abovel;  /* true if the given calcobs is above l */

/**********************************************************************
 *
 *  obsclean(l) takes the layer l and recreates its obscured list.
 *  This is done by putting l->rect into the new obscured list. Every layer's rect
 *  compared to all the rectangles currently in the obscured list, and any new
 *  subrectangles are put on the list. When this is done, all the rectangles
 *  in the obscured list which are not obscured are removed, the others have
 *  their bitmap filled in. The new obscured list then replaces the old
 *  obscured list.
 *
 ************************************************************************/

#undef obsclean
obsclean(l)
#define obsclean Sobsclean
Layer *l;
{
	register Obscured *next;
	register Obscured *newobs;
	register Obscured *o;
	register int i;
	register int fail;
	Rectangle r;
	Layer *l2;
        /* Layer dummy;    needed because lbitblt only works with a layer, not a bitmap */

	if (l && l->obs) {
		abovel = 0;
		fail = ((Ohead = (Obscured *)(alloc(sizeof(Obscured)))) == 0);
		Otail = Ohead;
		if (!fail) {
			Ohead->rect = l->rect;
			Ohead->lobs = l;
			Ohead->next = (Obscured *)0;
			Ohead->prev = (Obscured *)0;
		}
		for (l2=lback; l2 && !fail; l2=l2->front)
			if (l2==l)
				abovel = 1;
			else if (rectXrect(l2->rect,l->rect))
				fail=calcobs(l2);
		for (newobs = Ohead; newobs && !fail; newobs = next) {
			next = newobs->next;
			if (newobs->lobs != l) {
				/* this is an obscured rectangle */
					if ((newobs->bmap = balloc(newobs->rect))==0) {
						fail = 1;
					}
					else {
					     /* dummy.rect = newobs->rect;
						dummy.base = newobs->bmap->base;
						dummy.width = newobs->bmap.width;
						dummy.obs = (Obscured *)0;
						lbitblt(l,dummy.rect,&dummy,dummy.rect.origin,F_STORE); */ 
						/* because everything in the obscured list was there
						   before, it's quicker to just use bitblt           */
						for (o=l->obs; o; o=o->next) {
						/* what needs to be copied is already in the obscured list,
						   so just search that and copy with bitblt                 */
							r= o->rect;
							if (rectclip(&r,newobs->rect))
								bitblt(o->bmap,r,newobs->bmap,r.origin,F_STORE); 
						} 
					}
			}
			else {
				/* remove newobs from the obscured list */
				if (newobs->prev)
					newobs->prev->next = newobs->next;
				else
					Ohead = newobs->next;
				if (newobs->next)
					newobs->next->prev = newobs->prev;
				free((char *)newobs);
			}
		}
		if (fail)
			freeOlist(Ohead);
		else {
			freeOlist(l->obs);
			l->obs = Ohead;
		}
	}
}

calcobs(l2)
Layer *l2;
{
/*
 *   Compare l2->rect with each rectangle in the new obscured list and put back the
 *   subrectangles generated.
 */
	Rectangle r;
	int fail;
	Obscured *sr, *tail;

	tail = Otail;
	fail = 0;
	do {
		r = l2->rect;
		sr = Ohead;
		Ohead = Ohead->next;
		if (Ohead)
			Ohead->prev = (Obscured *)0;
		if (rectXrect(r,sr->rect)) {
			if (r.origin.x > sr->rect.origin.x && !fail) {
				fail = pushO(Rect(sr->rect.origin.x,sr->rect.origin.y,r.origin.x,sr->rect.corner.y),sr->lobs);
				sr->rect.origin.x = r.origin.x;
			}
			if (r.origin.y > sr->rect.origin.y) {
				fail = pushO(Rect(sr->rect.origin.x,sr->rect.origin.y,sr->rect.corner.x,r.origin.y),sr->lobs);
				sr->rect.origin.y = r.origin.y;
			}
			if (r.corner.x < sr->rect.corner.x && !fail) {
				fail = pushO(Rect(r.corner.x,sr->rect.origin.y,sr->rect.corner.x,sr->rect.corner.y),sr->lobs);
				sr->rect.corner.x = r.corner.x;
			}
			if (r.corner.y < sr->rect.corner.y && !fail) {
				fail = pushO(Rect(sr->rect.origin.x,r.corner.y,sr->rect.corner.x,sr->rect.corner.y),sr->lobs);
				sr->rect.corner.y = r.corner.y;
			}
						if (!fail) {
				if (abovel) /* is l2 on top of l */
					fail = pushO(sr->rect,l2);
				else
					fail = pushO(sr->rect,sr->lobs);
				free((char *)sr);
			}
		}
		else
			fail = pushO(sr->rect,  sr->lobs);
		free((char *)sr);
	} while(sr != tail);
	return(fail);
}

pushO(r,l2)
Rectangle r;
Layer *l2;
{
/*  put the given rectangle on the list */

	Obscured *newobs;

	if ((newobs = (Obscured *)alloc(sizeof(Obscured))) == 0)
		return(1);
	else {
		if (Ohead) {
			Otail->next = newobs;
			newobs->prev = Otail;
		}
		else {
			newobs->prev = (Obscured *)0;
			Ohead = newobs;
		}
		newobs->next = (Obscured *)0;
		newobs->rect = r;
		newobs->lobs = l2;
		Otail = newobs;
		return(0);
	}
}

freeOlist(Olist)
Obscured *Olist;
{
/*  free up the list of obscured rectangles */
	register Obscured *next;

	for(;Olist;Olist=next) {
		next=Olist->next;
		if (Olist->bmap)
			bfree(Olist->bmap);
		free((char *)Olist);
	}
}
