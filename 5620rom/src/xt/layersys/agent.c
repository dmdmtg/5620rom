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
 *	This is the host agent on the Blit. It is passed one argument,
 *	a pointer to a struct like:
 *	struct bagent{
 *		int size;	/* size of src string in and dest string out
 *		char * src;	/* the source byte string
 *		char * dest;	/* the destination byte string
 *	};
 *
 *	src points to a byte string obtained from the host by demux(), and upon
 *	return, demux expects dest to point to a byte string to be sent to
 *	the host. Size is the length of the src string when agent() is called
 *	and MUST BE the length of the returning dest string when agent()
 *	returns.
 */

#include <dmd.h>
#include <jioctl.h>
#include <layer.h>
#include <queue.h>
#include <dmdproc.h>
#include "proto.h"
#include <agent.h>
#include "vector.h"

#define	INSET	4
#define OUTSET	-2
#define	LBORDER	8
#define NBORDER 6	/* new border = LBORDER + OUTSET */
#define XMIN	28	/* minimum size window, width */
#define YMIN	28	/* minimum size window, length */

Rectangle insidedge = {LBORDER, LBORDER, XMAX-LBORDER, YMAX-LBORDER};
/* extern windowstart(); */
/* Rectangle checkrect(), getrect(); */

extern struct Proc *kbdproc;
extern Layer *lback;
extern char *ver_str;
char	*romidsrcaddr,
	*romiddestaddr;	/* pointers for copying rom version id string */
int	onetofive;	/* just a counter */

#undef agent
agent(ap)
#define agent Vagent
register struct bagent * ap;
{
	register struct Proc *p;
	struct	agentrect *arp;
	short *cmd;
	register int w;

	arp = (struct agentrect *)ap->src;
	cmd = (short *)ap->src;
	switch(*cmd)
	{
	case A_NEWLAYER:
	case A_NEW:
		p = 0;
		if(arp->r.corner.x == 0 && arp->r.origin.x == 0 &&
		   arp->r.corner.y == 0 && arp->r.origin.y == 0)
			arp->r = inset(getrect(),OUTSET + INSET);
		else
			arp->r = checkrect(arp->r);

		if(arp->r.corner.x-arp->r.origin.x>=XMIN &&
			arp->r.corner.y-arp->r.origin.y>=YMIN)
		{
			if(p=newproc(Sys[158]))/* 158 windowstart */	/* Assignment = */
			{
				p->rect=inset(arp->r, INSET+OUTSET);
				if(p->layer=newlayer(inset(arp->r,OUTSET)))
				{
					setborder(p);
					if ( *cmd == A_NEW )
						/* inform layers */
						mpxnewwind(p, C_NEW);
					else
						/* set jwinsize via reshape */
						mpxnewwind(p, C_RESHAPE);
				}else
				{
					p->state=0;
					p = 0;
				}
			}
		}
		if(p == 0)
			/* we failed to get the layer up */
			arp->command = -1;
		else
		{
			/* the layer is up, return the info */
			arp->command = 0;
			arp->chan = (short)getprocnum(p);
		}
		ap->dest = (char *)arp;
		break;

	case A_CURRENT:
		if(whichaddr(arp->chan)->layer)
		{
			tolayer( whichaddr(arp->chan)->layer );
			arp->command = 0;
		}
		else
			arp->command = -1;
		ap->dest = (char *)arp;
		break;

	case A_DELETE:
		if(whichaddr(arp->chan)->layer &&
			kbdproc->layer!= whichaddr(arp->chan)->layer)
		{
			w = arp->chan;
			p = whichaddr(w);
			mpxdelwind(w);
			dellayer(whichaddr(arp->chan)->layer);
			delproc(p);
			arp->command = 0;  
		}
		else
			arp->command = -1;
		ap->dest = (char *)arp;
		break;

	case A_TOP:
		if(whichaddr(arp->chan)->layer)
		{
			upfront( whichaddr(arp->chan)->layer );
			arp->command = 0;
		}
		else
			arp->command = -1;
		ap->dest = (char *)arp;
		break;

	case A_BOTTOM:
		if(whichaddr(arp->chan)->layer)
		{
			downback(whichaddr(arp->chan)->layer);
			arp->command = 0;
		}
		else
			arp->command = -1;
		ap->dest = (char *)arp;
		break;

	case A_MOVE:
		if(whichaddr(arp->chan)->layer)
		{
			if ( arp->r.origin.x < NBORDER )
				arp->r.origin.x = NBORDER;
			if ( arp->r.origin.y < NBORDER )
				arp->r.origin.y = NBORDER;
			move( whichaddr(arp->chan)->layer, arp->r.origin );
			arp->command = 0;
		}
		else
			arp->command = -1;
		ap->dest = (char *)arp;
		break;

	case A_RESHAPE:
		if(whichaddr(arp->chan)->layer)
		{
			if(arp->r.corner.x == 0 && arp->r.origin.x == 0 &&
			   arp->r.corner.y == 0 && arp->r.origin.y == 0)
				arp->r = inset(getrect(),OUTSET + INSET);
			else
				arp->r = checkrect(arp->r);

			reshape( whichaddr(arp->chan)->layer, arp->r );
			arp->command = 0;
		}
		else
			arp->command = -1;
		ap->dest = (char *)arp;
		break;


	case A_ROMVERSION:
		/* 32ld needs to know our firmware version (e.g. 8;7;5) */
		arp->command = 0;
		romiddestaddr = (char *)&arp->r;
		romidsrcaddr = (char *)ver_str+3;
		for (onetofive = 1; onetofive<=5; onetofive++) {
			*romiddestaddr++ = *romidsrcaddr++;
		};
		ap->dest = (char *)arp;
		break;

	case A_STACKSIZE:
		/* Host is telling us to set the stack size(bytes) for boot()   */
		/* NOTE: should have already started download (so pcb is setup) */
		/* casting 1st 2 shorts to an int (watch out for byte ordering) */

		whichaddr(arp->chan)->newstksize = (int)((arp->r.origin.y<<16) |
						      (arp->r.origin.x));
		whichaddr(arp->chan)->newstack = (int *)0; 	/* not there yet */
		arp->command = 0;
		ap->dest = (char *)arp;
		break;

	case A_EXIT:
		Psend(0, (char *)0, 0, C_EXIT);
		arp->command = 0;
		ap->dest = (char *)arp;
		break;

	default:
		arp->command = -1;
		ap->dest = ap->src;
		ap->size = 1;
	}

}

Rectangle
#undef checkrect
checkrect(r)
#define checkrect Vcheckrect
	Rectangle r;
{
	if ( r.origin.x < LBORDER )
	{
		r.corner.x = r.corner.x + (LBORDER - r.origin.x);
		r.origin.x = LBORDER;
	}
	if ( r.origin.y < LBORDER )
	{
		r.corner.y = r.corner.y + (LBORDER - r.origin.y);
		r.origin.y = LBORDER;
	}
	rectclip ( &r, insidedge );
	return (r);
}
