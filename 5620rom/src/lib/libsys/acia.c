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
#include "queue.h"
#include <setup.h>

/* These tables map BRAM option settings into DUART parameters.
 * They are indexed by the BRAM option value when initializing
 * the DUART. 
 */

unsigned char _2681par[] = {	/* parity settings */
		NO_PAR,
		ODD_PAR,	/* order set in options.h */
		EVN_PAR
	};
unsigned char _2681bits[] = {	/* character size settings */
		CBITS8,		/* order set in options.h */
		CBITS7
	};

#undef	dtr
int dtr;
char	aciabsy;	/* output-active flag */
char	aciastop;

#undef	aciainit
aciainit(speed)
char speed;
{
	/* init	the host side */
	int i;

	DUART->a_cmnd =	RESET_RECV | DIS_RX;
	DUART->a_cmnd =	RESET_TRANS | DIS_TX ;
	DUART->a_cmnd =	RESET_ERR;
	DUART->a_cmnd =	RESET_MR;
	DUART->mr1_2a =	CHAR_ERR | _2681par[VALAPRTY] | _2681bits[VALABITS];
	DUART->mr1_2a =	NRML_MOD | ONEP000SB;
	DUART->a_sr_csr	= speed;
	/* configure the output	port */
	DUART->ip_opcr = 0x70;
	DUART->ipc_acr = 0x8f;		/* Choose 2nd set of baud rates */
	DUART->scc_sopbc = 0x01;	/* make sure DTR is on */
	DUART->a_cmnd =	ENB_TX | ENB_RX; /* enable TX and RX last */
	if (VALSCREENCOLOR)
		BonW();
	else
		WonB();
	dtr = 1;
}

#undef aciapoke
aciapoke()	/* Initiate output to the duart */
{
	if (!aciastop && !TAS(&aciabsy))
		DUART->a_cmnd = ENB_TX;	/* Causes an interrupt */
}

TAS(fp)		/* Test and set a binary semaphore */
char *fp;
{
	asm(" MOVW	&1,%r0");
	asm(" SWAPBI	*0(%ap)");
}

#undef aciarxint
aciarxint()
{
#define CTRLS	'\023'
#define CTRLQ	'\021'
#define HIWAT	412
	register c;

	DUART->a_cmnd = 0;
	if ((c = DUART->a_data) == CTRLS && A_RECFLOW &&
		   (!VALDWNLDFLAG || ENCODE_ENABLE || VALPHOST != 0)) {
		aciastop = 1;
		return;
	}
	if (c == CTRLQ && aciastop) {
		aciastop = 0;
		aciapoke();
	}
	if (qputc(&RCVQUEUE,c) == HIWAT && !VALDWNLDFLAG && !NOAUTOFLOW
			&& !blocked && VALPHOST == 0) {
		sendchar(CTRLS);
		blocked = -1;
	}
	if (VALDWNLDFLAG == 3 && VALPHOST == 0)
		/* sysrun(0);	-- let demux run */
		(*(int(*)())((int *)0x71d700)[166])(0);
}


#undef aciatrint
aciatrint(){
	register c;

	/* should check	here for carrier fail */
	DUART->a_cmnd = 0;	/* toggle r/w lead */
	if(DUART->a_sr_csr & XMT_RDY) {
		if(aciastop || (c=qgetc(&OUTQUEUE)) == -1 /* no chars to send */
			  || (dtr == 0)) {	   /* term is discon   */
			DUART->a_cmnd =	DIS_TX;	   /* forget it	*/
			aciabsy = 0;
		}
		else	DUART->a_data =	c;	   /* send the char */
	}
}
#undef	dtrctl
dtrctl(flg) {
	dtr=flg;
	aciatrint();
}
#undef	tstdcd
tstdcd() {
	DUART->a_cmnd = NO_OP;	/* toggle r/w lead before doing a read */
	return((DUART->ip_opcr & 0x40) != 0);
}
#undef	trenable
trenable(){	/* used in sendpkt of layers */
	int	i;

	DUART->a_cmnd =	ENB_TX;
}
#undef	trdisable
trdisable(){	/* used in sendpkt of layers */
	int	i;

	DUART->a_cmnd =	DIS_TX;
}

#undef	aciapaws
aciapaws()
{
	/* No longer need the delay */
}

#undef	acioctl
acioctl(code,arg)
{
	register int sr=spl5();

	switch(code) {
	case STARTBRK:
		DUART->ip_opcr = 0x30; /* turn off the transmit intr */
		DUART->scc_ropbc = 0x40;
		DUART->a_cmnd =	(STRT_BRK|ENB_TX);
		break;
	case DISCON:
		DUART->scc_ropbc=0x01;	/* disable DTR	*/
		break;
	case STOPBRK:
		DUART->a_cmnd =	(STOP_BRK|DIS_TX);
		DUART->ip_opcr = 0x70; /* turn on the transmit intr */
		DUART->scc_sopbc = 0x40;
		break;
	case STOPDISC:
		DUART->scc_sopbc = 0x01;	/* Turn back on DTR */
		break;
	}
	splx(sr);
}
