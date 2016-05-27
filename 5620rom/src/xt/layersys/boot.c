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
#include <dmdproc.h>
#include "vector.h"


void getstk()
{	/* allocate the stack for a booted process */
	if( P->newstksize == 0 )		/* set with AGENT call */
		P->newstksize = STKSIZ;		/* use default */
	if( P->newstksize < MINSTKSIZ )		/* protect against fools */
		P->newstksize = MINSTKSIZ;
	if( P->newstksize == P->pcb.sp_upper - P->pcb.sp_lower )
	  {
		P->newstack = (int *)NULL;
		P->newstksize = 0;
	     }
	 else
		P->newstack = (int *)realalloc(P->newstksize,&proctab[1]);	/* control owns stacks */
}


#undef bldargs
bldargs(argc, argv)
#define bldargs Vbldargs
register char	**argv;
{
	register i;
	register char	*p = (char *)(argv + argc + 1);
	for (i = 0; i < argc; i++) {
		*argv++ = p;
		do
			*p = getchar();
		while (*p++);
	}
	*argv++ = 0;
}


#undef setdata
setdata(p)
#define setdata Vsetdata
register struct Proc *p;
{
	register struct udata *u = ((struct udata *)p->data);
	u->Drect = p->rect;
	u->Jdisplayp = p->layer;
}


#undef getchar
getchar()
#define getchar Vgetchar
{
	register c;
	register struct Proc *p = P;

	while (p->nchars == 0) {
		mpxublk(p);
		sw(0);
	}
	c = *(p->cbufpout)++;
	if (p->cbufpout >= &p->cbuf[sizeof(p->cbuf)])
		p->cbufpout = p->cbuf;
	if ((--p->nchars) == 0)
		mpxublk(p);	/* shouldn't be necessary, but... */
	return c;
}


long
#undef getlong
getlong()
#define getlong Vgetlong
{
	long	l;
	register char	*p = (char *)&l;
	register i;
	for (i = 0; i < 4; i++)
		*p++ = getchar();
	return(l);
}


#undef shutdown
shutdown(p)
#define shutdown Vshutdown
register struct Proc *p;
{
	extern struct Proc *debugger;
	extern struct Proc *kbdproc;
	Lbox(p->layer);
	setborder(p);
	if (debugger == p)
		debugger = 0;
	mpxublk(p);
	p->state &= ~(KBDLOCAL | MOUSELOCAL | GOTMOUSE | USER | NOPFEXPAND);
	p->nticks = 0;
	qclear(&p->kbdqueue);
	if ((p->state & ZOMBIE) == 0)
		freemem(p);
}


#undef freemem
freemem(p)
#define freemem Vfreemem
register struct Proc *p;
{
	/* extern int	end; */
	if (p->text > (char *)Sys[163])		/* Sys[163] == end */
		freeall((char *)p);
}
