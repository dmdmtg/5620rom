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
#define TASK_C
#define PSW 0x281a100		/* set to ipl 13 */

#include <dmd.h>
#include <layer.h>
#include <dmdproc.h>
#include "vector.h"

struct Proc *
#undef newproc
newproc(f)
#define newproc Vnewproc
int	(*f)();
{
	register struct Proc *u;
	register i;

	for (i = 0, u = proctab; i < NPROC; i++, u++) {
		if ((u->state & BUSY) == 0) {
			u->stack = 0;
			if( (u->newstack=(int *)realalloc(STKSIZ,(char *)&proctab[1])) == (int *)NULL )
				return(0);			/* nope */
			u->newstksize = STKSIZ;
			u->state |= BUSY | RESHAPED;
			restart(u, f);
			u->layer = 0;
			u->nchars = 0;
			u->cbufpin = u->cbuf;
			u->cbufpout = u->cbuf;
			u->traptype = (int *)0;
			u->traploc = (char **)0;
			return(u);
		}
	}
	return(0);
}


#undef restart
restart(p, loc)
#define restart Vrestart
register struct Proc *p;
register (*loc)();
{
	register i;
	p->pcb.psw = PSW;
	p->pcb.pc = (char(*)())loc;
	if( p->newstack != 0 )		/* have a new stack */
	 {
	   if( p->stack != 0 )		/* get rid of current one */
		free(p->stack);
	   p->stack = p->newstack;
	  }
	p->pcb.sp = (int)(p->stack);
	p->pcb.sp_lower = p->pcb.sp;
	p->pcb.sp_upper = p->pcb.sp_lower + (p->newstack ? p->newstksize : STKSIZ);
	p->pcb.ap = p->pcb.sp;
	p->pcb.fp = p->pcb.sp;
	for (i = 8; i >= 0; i--)
		p->pcb.reg[i] = 0;	/*registers 0 to 8 */
	p->pcb.blkmv = 0;	/* no block moves */
	p->newstack = (int *)(p->newstksize = 0);	/* we took care of this already */
	p->text = (char *)(loc);
}


/*	sw - user process calls sw() with the run flag as arguement
**	     if run = 1, then the process will run as soon as all
**	     the other processes have had a chance to run.
**	     if run == 0, then the process will "sleep" until
**	     woken by something (probably interrupt driven) 
*/
#undef sw
void
sw(run)
#define sw Vsw
int	run;
{
	if (run == 0)			/* I don't want to run */
		setnorun(P);		/* clear run flag      */
asm("	MOVAW	swpcbp,%r0  ");		/* move it to r0 for CALLPS	*/
asm("	CALLPS	");			/* wake-up switcher with call process */
					/* save context of calling process */
	return;				/* This is where the suspended process resumes */
}


/*	switcher - a round robin scheduler. Switcher looks for
**		   a process with it's RUN flag set. If none
**		   are found it waits.
**		   When a process is found with it's RUN flag set,
**		   the address of the process control block is
**		   placed on the interrupt stack, and a RETurn
**		   from Process Switch is executed, continuing the process
*/

extern struct Proc *last; /* defined in vector.s */
#undef switcher
switcher()
#define switcher Vswitcher
{
	register struct Proc *p = last;	/* hopefully r8 */
	register struct Proc *Demux = proctab;

	if (P != Demux){
		if ((Demux->state & (RUN | ZOMBIE)) == RUN  ) {
			last = P;
			P = Demux;		/* set current process to Demux */
asm(" 			MOVW	%r7,-4(%isp) "); 	/* same as resume */
asm(" 			RETPS	");
		}
		else {
			p = P;
		}
	}
	do {
		p++;	/* 'cos we are the current p */
		if (p >= &proctab[NPROC])
			p = proctab;
		if ((p->state & (RUN | ZOMBIE)) == RUN  ) {
			P = p;			/* set current process to p */
asm(" 			MOVW	%r8,-4(%isp) "); 	/* same as resume */
asm(" 			RETPS	");
		}
	} while (1);
}
#define STACK	16

int	swstk[STACK];
int	swpcbp[20];		/* pcb for the switcher		       */

#undef swinit
swinit()
#define swinit Vswinit
{
	register int * pcbptr = swpcbp;

	*pcbptr = 0x281a180; 		/* psw for switcher, r+i bits set only */
	*(pcbptr+1) = (int)Sys[152]; 	/* switcher, initial pc for the switcher*/
	*(pcbptr+2) = (int)swstk; 	/* initial stack for the switcher*/
	*(pcbptr+3) = 0x281a100;
	*(pcbptr+4) = (int)Sys[152]; 	/* switcher, initial pc for the switcher*/
	*(pcbptr+5) = (int)swstk;
	*(pcbptr+6) = (int)swstk;
	*(pcbptr+7) = (int)&swstk[STACK-1];
	*(pcbptr+8) = (int)swstk;	/* ap */
	*(pcbptr+9) = (int)swstk;	/* fp */
	*(pcbptr+19) = 0;		/* no block moves */
	last = proctab;			/* initialization to avoid ROM problem */
};
