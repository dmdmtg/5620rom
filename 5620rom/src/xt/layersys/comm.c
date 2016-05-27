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
#include <queue.h>
#include <dmdproc.h>
#include <setup.h>
#include <kbd.h>
#include "vector.h"

/*
 * clockroutine() called at video interrupt time.
 *	reads chars off the keyboard, sends things to host
 */

extern int	SetupFlag;		/* only if in ROM!! */

extern short	second, ticks; /* defined in vector.s */
extern long	ticks0;	 /* already in l.s */
/* extern boot(); */
extern char 	*patchedspot;	/* real ones are in trap.c */
extern char 	patch;		/* real ones are in trap.c */
extern int	kbdrepeat;
void	clipcursor();
#define	CONTROL	1
#define	qpeekc(Q)	((Q)->c_head?((Q)->c_head->word):-1)
#define MBUTTON	1	/* for givemouse */
#define MPOSITION	2	/* for givemouse */

/*ARGSUSED*/
#undef clockroutine
clockroutine()
#define clockroutine Vclockroutine
{
	register c;
	extern struct Proc *kbdproc;
	register struct Proc *p;
	register int i,j;

	ticks0++;
	if (--ticks <= 0) {
		ticks = 60;	/* really HZ */
		second = 1;
	}
	clipcursor();	/* turn off cursor in inhibited layers */

	for (p = &proctab[CONTROL+1]; p < &proctab[NPROC]; p++)
		if (p->nticks > 0 && --p->nticks == 0)
			p->state |= WAKEUP | RUN;
	setrun(&proctab[CONTROL]);

	if(( kbdrepeat & RPTHAVECHR ) && (kbdrepeat & RPTON ) )
		if( VALREPEAT == 4 ) kbdrpt();		/* can't mod by 0 */
		 else if( ticks%(4-VALREPEAT) == 0 ) kbdrpt();
	/* keyboard repeat must be done here in case setup is running */
	if ((p = kbdproc) == 0)
		return;
	while (KBDQUEUE.c_cc > 0 && !SetupFlag ) {
		c = qgetc(&KBDQUEUE);
		/* the following code is commented out because
		 * the 60hz and button interrupts are at the same level 
		 */
		/* if (c == 0x81) {	/* DISCON; show what's up */
			/* rectf(&display, P->rect, F_XOR); */
			/* do;  */
			/* while (bttn123() == 0) */
				/* ; */
			/* if (bttn2()) { */
				/* patchedspot = (char *)P->pcb.pc; */
				/* patch = *patchedspot; */
				/* *patchedspot = 0; /* Cause Illegal inst. trap */
			/* } */
			/* rectf(&display, P->rect, F_XOR); */
			/* do;  */
			/* while (bttn123()) */
				/* ; */
		/* }  */
		/* else  */
		if (p->state & KBDLOCAL) {
			p->state |= WAKEUP;
		}
		if ((p->state & KBDLOCAL) && ( c == '\r' )) {
			switch(VALRETKEY) {
			case 0:
				break;
			case 1:
				c = '\n';
				break;
			case 2:
				qputc(&p->kbdqueue,c);
				if(VALDUPLEX) {
					/* half duplex */
				}
				c = '\n';
				break;
			}
			qputc(&p->kbdqueue,c);
		}
		else if((c >= 0x82) && (c <= 0x89) && !(p->state & NOPFEXPAND)) { /* pf key!! */
				j = (c & 0xf) -2;
				i = 0;
				while ((c=BRAM->pfkeys[j][i].byte) && 
					(++i <= 50)) {
					if(qputc(&p->kbdqueue,c) && VALDUPLEX) {
						/* half duplex */
					}
				}
		}
		else 
			qputc(&p->kbdqueue,c);
	}
	if (!(p->state & KBDLOCAL) && !SetupFlag )
		while ((c = qpeekc(&p->kbdqueue)) != -1) {
			if (p->text != (char *)Sys[91]) {	/* boot, KLUDGE, but it helps */
				switch (c) {
				/* No special treatment for DEL */
				case 0x13:  /* ^S */
					if (!VALSENDCTRLS)
						p->state |= BLOCKED;
					else if (mpxkbdchar(c) == -1)
						goto out;
					break;
				case 0x11:  /* ^Q */
					if (!VALSENDCTRLS) {
						p->state &= ~(BLOCKED|UNBLOCKED);
						p->state |= WAKEUP;
					}
					else if (mpxkbdchar(c) == -1)
						goto out;
					break;
				case 0x8c:	/* No local clear function */
				case 0x81:	/* No disconnect in layers */
					break;
				case 0x80:	/* BREAK */
				case 0x7F:
					c = 0x7F;  /* DELETE, for now */
					if (!VALSENDCTRLS) {
						p->state &= ~(BLOCKED|UNBLOCKED);
						p->state |= WAKEUP;
					}
					/* fall through */
				default:
					if (mpxkbdchar(c) == -1)
						goto out;
				}
			}
			(void)qgetc(&p->kbdqueue);
		}
out:
	mpxkbdflush();
	givemouse(p,MBUTTON|MPOSITION);
}

extern struct curtab {
	short	*map;
	short	dx, dy;
} *curtabp;
struct Cursor{		/* duplicate of cursor.h */
        unsigned x,y;
        unsigned oldx, oldy;
        char quad, oldquad;
        short inhibit;
        short up;
};
extern struct Cursor cursor;
extern Layer	*lfront;
/* static */short	cursclipt;
void
clipcursor()	/* Turn off cursor if it overlaps any layer which */
{		/* has it inhibited */
/* What we really want here is to turn off the cursor if any part
 * of it touches the visible part of a layer which has inhibited it.
 * It's impractical to do this perfectly, since this routine is
 * called on each clock interrupt, so this is the next approximation,
 * inhibiting the cursor if it intersects the screen rectangle of an
 * inhibited layer and is not completely contained within an obscuring
 * layer.  */
	Rectangle mr;	/* 16 x 16 rectangle of mouse cursor icon */
	register Layer	*l;

	mr.origin.x = mouse.xy.x + curtabp[cursor.quad].dx;
	mr.origin.y = mouse.xy.y + curtabp[cursor.quad].dy;
	mr.corner.x = mr.origin.x + 16;
	mr.corner.y = mr.origin.y + 16;
	for (l=lfront; l; l=l->back) {
		if (rectXrect(mr,l->rect) && proctab[whichproc(l)].inhibited) {
			if (cursor.up) {
				cursclipt++;
				cursinhibit();
			}
			return;
		}
		/* Check no further if completely contained */
		if (mr.origin.x >= l->rect.origin.x &&
		    mr.origin.y >= l->rect.origin.y &&
		    mr.corner.x <= l->rect.corner.x &&
		    mr.corner.y <= l->rect.corner.y)	break;
	}

	/* If we get here, the cursor doesn't overlap any inhibited layer
	 * or is enclosed by a visible non-inhibited layer. */
	if (cursclipt) {
		cursclipt = 0;
		cursallow();
	}
}

#undef givemouse
givemouse(p,update)
#define givemouse Vgivemouse
register struct Proc *p;
register int update;
{
	if ((p->state & (MOUSELOCAL | USER)) == (MOUSELOCAL | USER)) {
		register struct Mouse *m = &((struct udata *)p->data)->mouse;
		if (update & MPOSITION) {
			m->xy = mouse.xy;
#define	o	p->rect.origin
#define	c	p->rect.corner
			m->jxy.x = muldiv(mouse.xy.x - o.x, XMAX, c.x - o.x);
			m->jxy.y = muldiv(mouse.xy.y - o.y, YMAX, c.y - o.y);
		}
		if (update & MBUTTON)
			m->buttons = mouse.buttons;
	}
}


#undef sleep
void
sleep(s)
#define sleep Vsleep
{
	register struct Proc *p = P;
	register alarmed = p->state & ALARMREQD;
	register long	nticks;
	extern long	ticks0;
	nticks = ticks0 + p->nticks;
	alarm(s);
	Uwait(ALARM);
	/* a little dance because sleep calls alarm */
	if (alarmed) {
		spl1();
		if (nticks > ticks0)
			p->nticks = nticks - ticks0;
		else	/* we missed his wakeup! */
			p->state |= WAKEUP;
		spl0();
		p->state |= ALARMREQD;
	} else
		p->state &= ~ALARMREQD;
}


#undef alarm
void
alarm(s)
#define alarm Valarm
{
	P->state |= ALARMREQD;
	if (s > 0)
		P->nticks = s;
}


/*	already in l.s 
 **
 **	long
 **	realtime()
 **	{
 **	return ticks0;
 **	}
 */
