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
#include <msgs.h>
#include <setup.h>
#include "vector.h"

extern struct Proc *debugger;	/* defined in trap.c */
/* extern int	recvchars(), sendpkt(); */
#define	crecvchars	recvchars


int	(*menufn[9])(); /* = { */
	/* New, 	Reshape, Move, 	Top, 	Bottom, 	Current, Delete, */
	/* Exit, 	0 */
/* }; */
Menu windowmenu;
#ifndef lback
extern Layer	*lfront, *lback;
#endif
#ifndef baud_speeds
extern char	baud_speeds[];
#endif
#ifndef maxaddr
extern  long	maxaddr[];
#endif
#ifndef display
extern Bitmap	display;
#endif


#define	INSET	4
#define OUTSET	-2
#define XMIN	28
#define YMIN	28
/* Layer *whichlayer(); */
extern struct Proc *kbdproc; /* defined in vector.s */
extern short	usermouse; /* kbdproc (a USER proc) has the mouse under its paw */
struct Proc *queueproc;  /* this is which proc currently has characters in kbdlist */
#define	MAXKBD	CBSIZE
char	kbdlist[MAXKBD];
extern char *kbdlistp; /* defined in vector.s */
#undef	UP
#define	UP	0
#undef	DOWN
#define	DOWN	1
#define MBUTTON	1	/* for givemouse */
#define MPOSITION	2	/* for givemouse */
/* int	control(), windowstart(), demux(); */
extern struct Mouse mouse;	/* we want THE mouse */
#undef main
main()
#define main Vmain
{
	/* extern int	end; */
	/* jinit() by hand... */
	extern char * menutext[];
	unsigned int	asize;		/* calculated size of alloc pool */
	kbdproc = 0;
	usermouse = 0; /* now in bss section */
	kbdlistp = kbdlist; /* now in bss section */
	menufn[0] = (int(*)())Sys[81];		/* New */
	menufn[1] = (int(*)())Sys[84];		/* Reshape */
	menufn[2] = (int(*)())Sys[80];		/* Move */
	menufn[3] = (int(*)())Sys[87];		/* Top */
	menufn[4] = (int(*)())Sys[75];		/* Bottom */
	menufn[5] = (int(*)())Sys[77];		/* Current */
	menufn[6] = (int(*)())Sys[78];		/* Delete */
	menufn[7] = (int(*)())Sys[79];		/* Exit */
	windowmenu.item = menutext;
	*DADDR = 0;

	VALDWNLDFLAG |= 0x2;	 /* flag that layers is running */
	setbram();

	BonW();
	qinit();
	aciainit(baud_speeds[VALBAUD]);
	binit();
	kbdinit();
	Lgrey(display.rect);
	cursinit();

	/* Following allocates memory.  RAM is split according to a BRAM
	   value that can be set in resident terminal with the (undocumented)
	   ESC [ value h escape sequence. (value)/10 of memory is given to
	   alloc.  If value = 0, the default of 50% is used. 
	   VALMAXADDR is the BRAM address of a flag indicating memory size.  
		0 - 256K, 1 - 1024K.  Make multiple of 4 , Sys[163] == end
	   gc space is defined as the rest of memory not taken by alloc.
	 */
	asize = (maxaddr[VALMAXADDR]-(int)Sys[163])*(VALRAMSPLIT?VALRAMSPLIT:5)/10;
	allocinit( (int *)Sys[163], (int *)(((int)Sys[163]+asize)&0xfffffffc) );
	gcinit();
	if (pinit(NPROC) == -1)
		error("pinit", "-1");
	spl0();
	swinit();
	P = newproc(Sys[102]);	/* demux,process 0 , make sure P is init. */

	setrun(newproc(Sys[97]));	/* control,process 1 */
	sw(0);		/* a subtle point	*/
	/*	The sw() above does a call process which stores 
	**	the process control block for the LAYERSYS process
	** 	The CALLPS saves the pcbp for above on the isp
	**	This pcbp will be lost when switcher() finds a process
	**	to run in the 8 available processes in the process tables
	**	and does a RETPS........but that is okay.
	**	We always reboot() to get back to ground zero.
	*/
}


#undef bttns
void
bttns(updown)
#define bttns(a) Vbttns(a)
{
	do; 
	while ((bttn123() != 0) != updown)
		;
}



/* short	usermouse = 0;*/ /* kbdproc (a USER proc) has the mouse under its paw */
extern short	hst_init /*  = 0 (defined in vector.s) Host is initialized */ ;
extern struct physops *hostops;

#undef control
control()
#define control Vcontrol
{
	register Layer *lp;
	register struct Proc *p, *pp;

	cursinhibit();
	while( !hst_init ) {
		Sw();
	   }
	cursallow();

	for (; ; ) {
		pp = 0;
		lp = whichlayer();
		for (p = proctab + CONTROL + 1; p < &proctab[NPROC]; p++) {
			if (p->state & WAKEUP) {
				p->state &= ~WAKEUP;
				setrun(p);
			}
			if (lp && p->layer == lp)
				pp = p;		/* pp pointed at by mouse */
		}
		if (usermouse && (pp != kbdproc || (pp->state & GOTMOUSE) == 0)) {
			usermouse = 0;
			cursswitch((Texture * )0);
			cursallow();
		} else if (!usermouse && pp) {
Check_mouse:	
			if (pp == kbdproc && (pp->state & GOTMOUSE)) {
				usermouse = 1;
				cursswitch(pp->cursor);
				if (pp->inhibited)
					cursinhibit();
			}
		}
		if (bttn123()) {
			if (lp == 0 || (pp->state & GOTMOUSE) == 0) {
				dobutton(whichbutton());
				/* make sure kbdproc doesn't think
				   buttons are down */
				if (kbdproc) {
					givemouse(kbdproc,MBUTTON|MPOSITION);
					goto Check_mouse;	/* usermouse==0 */
				}
			}
			if (pp && pp->state & GOTMOUSE)
				givemouse(pp,MBUTTON|MPOSITION);
		}
		if (hostops->rcvq->c_cc)
			setrun(&proctab[DEMUX]);
		Sw();
	}
}

#undef	sysrun
sysrun(who)		/* called from interrupt - IPL high */
/* No `V' call available -- only called from interrupt routines */
 int who;
{
	proctab[who].state |= RUN;	/* just make it runnable */
}

Texture16 bullseye = {
	0x07E0, 0x1FF8, 0x399C, 0x63C6, 0x6FF6, 0xCDB3, 0xD99B, 0xFFFF,
	    0xFFFF, 0xD99B, 0xCDB3, 0x6FF6, 0x63C6, 0x399C, 0x1FF8, 0x07E0,
};


Texture16 skull = {
	0x0000, 0x0000, 0x0000, 0xC003, 0xE7E7, 0x3FFC, 0x0FF0, 0x0DB0,
	    0x07E0, 0x0660, 0x37EC, 0xE427, 0xC3C3, 0x0000, 0x0000, 0x0000
};
Texture16 boxcurs = {
	0x43FF, 0xE001, 0x7001, 0x3801, 0x1D01, 0x0F01, 0x8701, 0x8F01,
	    0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0x8001, 0xFFFF,
};

Texture16 arrows = {
	 0xFC3F, 0xFC3F, 0xE00F, 0xD81B,
	 0xCC33, 0xC663, 0x02C0, 0x0380,
	 0x0380, 0xC640, 0xCC63, 0xD833,
	 0xF01B, 0xE007, 0xFC3F, 0xFC3F,
};

#undef New
New()
#define New VNew
{
	newwindow(Sys[158]);	/* windowstart */
}


#undef Exit
Exit()
#define Exit VExit
{
	/* buttons are now down; let the user out with button 1 or 2 */
	while (bttn123())
		if (bttn12()) {
			bttns(UP);
			return;
		}
	Psend(0, (char *)0, 0, C_EXIT);
}


#undef Delete
Delete()
#define Delete VDelete
{
	register Layer *l = whichlayer();
	register struct Proc *p;
	register w;
	if (l && kbdproc->layer != l) {
		w = whichproc(l);
		p = &proctab[w];
		if(p->state & LOCKLAYER) { /* application prog set LOCKLAYER */
			return;
		}
		mpxdelwind(w);
		delproc(p);
		dellayer(l);
	}
}


#undef delproc
delproc(p)
#define delproc Vdelproc
register struct Proc *p;
{
	p->state = 0;		/* exit(w) */
	p->nticks = 0;
	qclear(&p->kbdqueue);
	freemem(p);
	free(p->stack);		/* get rid of his stack, too */
	p->layer = 0;		/* sigh */
}


#undef Top
Top()
#define Top VTop
{
	upfront(whichlayer());
}


#undef Bottom
Bottom()
#define Bottom VBottom
{
	downback(whichlayer());
}


#undef Current
Current()
#define Current VCurrent
{
	register Layer *l;
	l = whichlayer();
	if (l)
		tolayer(l);
}


Rectangle
#undef canon
canon(p1, p2)
#define canon Vcanon
Point p1, p2;
{
	Rectangle r;
	r.origin.x = min(p1.x, p2.x);
	r.origin.y = min(p1.y, p2.y);
	r.corner.x = max(p1.x, p2.x);
	r.corner.y = max(p1.y, p2.y);
	return(r);
}


Rectangle
#undef getrect
getrect()
#define getrect Vgetrect
{
	Rectangle r;
	Point p1, p2;
	cursswitch(-1);
	bttns(UP);
	bttns(DOWN);
	p1 = mouse.xy;
	p2 = p1;
	r = canon(p1, p2);
	outline(r);
	for (; bttn3(); nap(3)) {
		outline(r);
		p2 = mouse.xy;
		r = canon(p1, p2);
		outline(r);
	}
	outline(r);	/* undraw for the last time */
	cursswitch((P->state & USER) ? P->cursor : (Texture16 * )0);
	return r;
}


#undef Reshape
Reshape()
#define Reshape VReshape
{
	register Layer *l;
	Rectangle r;
	l = whichlayer();
	if (l == 0)
		return;
	r = inset(getrect(),INSET + OUTSET);
	reshape(l, r);
}


#undef reshape
reshape(l, r)
#define reshape Vreshape
register Layer *l;
Rectangle r;
{
	register struct Proc *p;

	p = &proctab[whichproc(l)];
	if (r.corner.x - r.origin.x >=XMIN && r.corner.y - r.origin.y >=YMIN) {
		Point save;
		save = l->rect.origin;
		dellayer(l);
		p->state &= ~MOVED;
		p->state |= RESHAPED;
		l = newlayer(inset(r,OUTSET));
		if (l == 0) {
			r.origin = sub (save, Pt(OUTSET,OUTSET)); 
			r.corner = add (r.origin, Pt(XMIN, YMIN));
			l = newlayer(inset(r,OUTSET));
			if (l == 0) {	/* oh shit */
				delproc(p);
				mpxdelwind((int)(p - proctab));
				return;
			}
		}
		p->layer = l;
		p->rect = inset(r, INSET + OUTSET);
		setborder(p);
	}
	if (p->state & USER)
		setdata(p);
	mpxnewwind(p, C_RESHAPE);
}


#undef Move
Move()
#define Move VMove
{
	register Layer *l;
	Point tp, p, op, dp;
	l = whichlayer();
	if (l == 0)
		return;
	dp = sub(l->rect.corner, l->rect.origin);
	cursswitch(&arrows);
	p = l->rect.origin;
        tp = sub(mouse.xy, p);
        if (bttn12())
		goto Return;
	while (bttn3()) {
		if (bttn12())
			goto Return;
		outline(Rpt(p, add(p, dp)));
		nap(3);
		op = p;
		p = sub(mouse.xy, tp);
		/* using boxcurs, can't get off top or left! */ 
                if (p.x + dp.x >= XMAX - 6) 
                        p.x = XMAX - 6 - dp.x; 
                if (p.y + dp.y >= YMAX - 6) 
                        p.y = YMAX - 6 - dp.y; 
                if (p.x < 6)
                        p.x = 6;
                if (p.y < 6)
                        p.y = 6;
                outline(Rpt(op, add(op, dp))); 
                cursset(add(p, tp)); 
        } 
        move(l, p); 
Return:
        cursset(mouse.xy);
        /* No C_RESHAPE required */ 
}

#undef move
move(l, p)
#define move Vmove
Layer *l;
Point p;
{
	Point dp;
	register Layer *nl;
	register struct Proc *procp;

	dp = sub(l->rect.corner, l->rect.origin);
	if (p.x + dp.x >= XMAX - 6)
		p.x = XMAX - 6 - dp.x;
	if (p.y + dp.y >= YMAX - 6)
		p.y = YMAX - 6 - dp.y;
	procp = &proctab[whichproc(l)];
	nl = newlayer(Rpt(p, add(p, dp)));
	if (nl == 0)
		return;
	cursinhibit();
	lbitblt(l, l->rect, nl, p, F_STORE);
	cursallow();
	procp->layer = nl;
	procp->rect = inset(nl->rect, INSET);
	dellayer(l);
	if (procp->state & USER)
		setdata(procp);
	if ((procp->state & RESHAPED) == 0)
		procp->state |= MOVED | RESHAPED; /* RESHAPED on for old progs */
	l = nl;
	setborder(procp);
	return;			/* No C_RESHAPE required */
}

/* button hit to indicate which process, invoked by debugger */
struct Proc *
#undef debug
debug()
#define debug Vdebug
{
	debugger = P;
	return(point2layer());
}

struct Proc *
#undef point2layer
point2layer()
#define point2layer Vpoint2layer
{
	extern Texture16 bullseye;
	register Layer *l;
	struct Proc *z = 0;
	Texture16 *old_mcurse;

	old_mcurse = (Texture16 *)cursswitch(&bullseye);
	bttns(DOWN);
	if (bttn3() && (l = whichlayer()))
		z = &proctab[whichproc(l)];
	bttns(UP);
	cursswitch(old_mcurse); 	/* reset to original cursor */
	return z;
}


struct Proc *
#undef whichaddr
whichaddr(procnum)
#define whichaddr Vwhichaddr
int procnum;
{

	return(&proctab[procnum]);
}

int
getprocnum(p)
struct Proc *p;
{
	return((int)(p - proctab));
}


#define	EXIT	7
char	*menutext[] = {
	"New", "Reshape", "Move", "Top", "Bottom", "Current", "Delete", 
	"Exit", 0
};




/* Menu windowmenu = {  */
/* 	menutext }; */


#undef dobutton
dobutton(b)
#define dobutton Vdobutton
{
	register hit;
	register Layer *l;
	switch (b) {
	case 1:
		if (l = whichlayer()) {
			upfront(l);
			tolayer(l);
		}
		break;
	case 2:
		break;	/* dunno... */
	case 3:
		if ((hit = menuhit(&windowmenu, 3)) >= 0) {
			if (hit == 0)	/* a little different because of getrect */
				New();
			else {
				cursswitch(hit == EXIT ? &skull : &bullseye);
				bttns(DOWN);
				if (bttn3())
					(*menufn[hit])();
				cursswitch((Texture16 * )0);
			}
		}
		break;
	default:
		break;
	}
	bttns(UP);
}


#undef whichproc
whichproc(l)
#define whichproc Vwhichproc
register Layer *l;
{
	register struct Proc *p;
	for (p = proctab + CONTROL + 1; p < proctab + NPROC; p++)
		if (p->layer == l && (p->state & BUSY))
			return((int)(p - proctab));
	return(CONTROL + 1);	/* HELP?? */
}


#undef whichbutton
whichbutton()
#define whichbutton Vwhichbutton
{
	register b = mouse.buttons;
	if (b & 4)
		return 1;
	if (b & 2)
		return 2;
	if (b & 1)
		return 3;
	return 0;
}


#undef newwindow
newwindow(fn)
#define newwindow Vnewwindow
int	(*fn)();
{
	register struct Proc *p;
	Rectangle r;

	r = inset(getrect(),INSET + OUTSET);
	if (r.corner.x - r.origin.x >= XMIN && r.corner.y - r.origin.y >=YMIN) {
		if (p = newproc(fn)) {	/* Assignment = */
			p->rect = inset(r, INSET + OUTSET);
			if (p->layer = newlayer(inset(r,OUTSET))) {
				mpxnewwind(p, C_NEW);
				tolayer(p->layer);
				setrun(p);
			} else {
				p->state = 0;
				free(p->stack); /* newproc() alloc'd it */
			}
		}
	}
}



/* #define	MAXKBD	CBSIZE */
/* char	kbdlist[MAXKBD], *kbdlistp = kbdlist; */
#undef mpxkbdchar
mpxkbdchar(c)
#define mpxkbdchar Vmpxkbdchar
{
	if (kbdlistp >= &kbdlist[MAXKBD])
		return - 1;
	if (kbdlistp == kbdlist)
		queueproc = kbdproc;
	else if (queueproc != kbdproc)
		return(-1);
	*kbdlistp++ = c;
	return 0;
}


#undef outline
void
outline(r)
#define outline Voutline
Rectangle  r;
{
	register dx = r.corner.x-r.origin.x-1, dy = r.corner.y-r.origin.y-1;
	jmoveto(r.origin);
	jline(Pt(dx, 0), F_XOR);
	jline(Pt(0, dy), F_XOR);
	jline(Pt(-dx, 0), F_XOR);
	jline(Pt(0, -dy), F_XOR);
}


#undef min
min(a, b)
#define min Vmin
{
	return(a < b ? a : b);
}


#undef max
max(a, b)
#define max Vmax
{
	return(a > b ? a : b);
}


Layer *
#undef whichlayer
whichlayer()
#define whichlayer Vwhichlayer
{
	register Layer *lp;

	if (mouse.xy.x <= 8 || mouse.xy.y <= 8 || mouse.xy.x >= (XMAX - 9) ||
		mouse.xy.y >= (YMAX - 9))
		return(0);
	for (lp = lfront; lp; lp = lp->back)
		if (ptinrect(mouse.xy, lp->rect))
			return(lp);
	return(0);
}


#undef tolayer
tolayer(l)
#define tolayer Vtolayer
register Layer *l;
{
	register struct Proc *p;
	register struct Proc *pp = 0;
	for (p = proctab; p < &proctab[NPROC]; p++)
		if ((p->state & BUSY) && l == p->layer) {
			if (kbdproc != p) {
				pp = kbdproc;
				kbdproc=p;
				if (pp) {
					setborder(pp);
					pp->state &= ~GOTMOUSE;
				}
				setborder(p);
				if ((pp) && ((pp->state & (MOUSELOCAL|USER)) == (MOUSELOCAL|USER)))
					((struct udata *)pp->data)->mouse.buttons = 0;
			}
			if (p->state & MOUSELOCAL) {
				p->state |= GOTMOUSE;
				setrun(p);
			}
			break;
		}
}



#undef clear
clear(r, inh)
#define clear Vclear
Rectangle r;
{
	if (inh)
		cursinhibit();
	lrectf(P->layer, r, F_CLR);
	if (inh)
		cursallow();
}

border(l, r, i, c)	/* no flashing! */
	register Layer *l;
	Rectangle r;
	register i;
	Code c;
{
	lrectf(l, Rect(r.origin.x, r.origin.y, r.corner.x, r.origin.y+i), c);
	lrectf(l, Rect(r.origin.x, r.corner.y-i, r.corner.x, r.corner.y), c);
	lrectf(l, Rect(r.origin.x, r.origin.y+i, r.origin.x+i, r.corner.y-i), c);
	lrectf(l, Rect(r.corner.x-i, r.origin.y+i, r.corner.x, r.corner.y-i), c);
}

setborder(p)
	register struct Proc *p;
{
	cursinhibit();
	border(p->layer, p->layer->rect, INSET, F_OR);
	cursallow();
	if(p!=kbdproc)
		border(p->layer, inset(p->layer->rect, 1), INSET-1, F_XOR);
}

/* For testing only */
#undef	shademap 
Texture	shademap;
#undef	shade
shade(l)
  Layer	*l;

{
	/* No Effect */
}
