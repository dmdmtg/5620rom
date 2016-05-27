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
#include <queue.h>
#include <layer.h>
#include <dmdproc.h>
#include "vector.h"

#define STACK 16

#undef exec
exec(f)
#define exec Vexec
int (*f)();
{
	struct pcb	execpcb;
	int	execstk[STACK];

	execpcb.psw = 0x281e100;
	execpcb.pc = (char(*)())Sys[110];	/* execsw */
	execpcb.sp = (int)execstk;
	execpcb.sp_lower = (int)execstk;
	execpcb.sp_upper = (int)&execstk[STACK];
	execpcb.ap = (int)execstk;
	execpcb.fp = (int)execstk;
	execpcb.blkmv = 0;

	P->text = (char *)(f);			/* set start of program */
asm("	MOVW	%fp,%r0  ");	/* fp has addr of pcb,move it to r0 for CALLPS	*/
asm("	CALLPS	");			/* wake-up switcher with call process */
}

#undef zombexec
zombexec(f)
#define zombexec Vzombexec
int (*f)();
{
	struct pcb	zombpcb;
	int	zombstk[STACK];

	zombpcb.psw = 0x281e100;
	zombpcb.pc = (char(*)())Sys[161];	/* zombsw */
	zombpcb.sp = (int)zombstk;
	zombpcb.sp_lower = (int)zombstk;
	zombpcb.sp_upper = (int)&zombstk[STACK];
	zombpcb.ap = (int)zombstk;
	zombpcb.fp = (int)zombstk;
	zombpcb.blkmv = 0;
	P->text = (char *)(f);			/* set start of program */
asm("	MOVW	%fp,%r0  ");	/* fp has addr of pcb,move it to r0 for CALLPS	*/
asm("	CALLPS	");			/* wake-up switcher with call process */
}

#undef windowstart
windowstart()
#define windowstart Vwindowstart
{
	windowproc();
}
#undef execsw
execsw()
#define execsw Vexecsw
{
	restart(P,P->text);
asm("	RETPS  ");		/* the return from process execs to the */
				/* start of the new process */
}

#undef zombsw
zombsw()
#define zombsw Vzombsw
{
	restart(P,P->text);
asm("	subw2	&4,%isp  ");	/* adjust isp because we don't RETPS */
	sw(1);			/* suspend  the new process */
}

