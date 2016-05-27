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
/* sccops.c
 *
 * SCC-specific routines: initialization, interrupt handlers, etc.
 *
 */

#include <dmd.h>
#include <scc.h>
#include <queue.h>
#include <setup.h>

/*static*/
char	wr5a, wr5b, sccAbsy, sccBbsy, stopAout, stopBout;

/* Time constants for the baud rate generator with a 4.9152 MHz, x32 clock */
/* The order is determined by options.h */
static char scchibaud[6] = {
		(char)0x00,	/* 1200 baud */
		(char)0x00,	/* 2400 baud */
		(char)0x00,	/* 4800 baud */
		(char)0x00,	/* 9600 baud */
		(char)0x00,	/* 19200 baud */
		(char)0x00	/* 300 baud */
	};
static char scclobaud[6] = {
		(char)0x3e,	/* 1200 baud */
		(char)0x1e,	/* 2400 baud */
		(char)0x0e,	/* 4800 baud */
		(char)0x06,	/* 9600 baud */
		(char)0x02,	/* 19200 baud */
		(char)0xfe	/* 300 baud */
	};


/* Unshifted SCC register values for character size not counting parity bit */
static char sccharz[2] = {
		(char)0x03,	/* 8 bits */
		(char)0x01	/* 7 bits */
	};

/* Unshifted SCC register values for transmit and receive parity */
static char sccpar[3] = {
		(char)0x00,	/* No parity bit */
		(char)0x01,	/* Odd parity */
		(char)0x03	/* Even parity */
	};


#undef	sccinit
sccinit()
{
	int x = spl7();

		/* set drivers for RS232 or RS422 */
	IOX->select = ((1-VALCTYPE) | (1-VALDTYPE)<<4)<<3 |
				0x10 | 0x1;
	SCC->actrl = 0; /* make sure reg. ptr's are in sync */
	SCC->bctrl = 0;
	SCC->actrl = 9; SCC->actrl = HARD_RESET | MINT_EN | MOD_VECTOR;
	SCC->actrl = 2; SCC->actrl = 0;  /* unmodified vector = 0 */
	SCC->actrl = 15; SCC->actrl = 0; /* disable external interrupts */
	SCC->bctrl = 15; SCC->bctrl = 0;

	SCC->actrl = 10; SCC->actrl = 0; /* NRZ encoding */
	SCC->bctrl = 10; SCC->bctrl = 0;
	  /* Use x32 clock */
	SCC->actrl = 4; SCC->actrl = CLK_X32 | ONE_STOP | sccpar[VALCPRTY];
	SCC->bctrl = 4; SCC->bctrl = CLK_X32 | ONE_STOP | sccpar[VALDPRTY];
	  /* (TRxC is an input) */
	SCC->actrl = 11; SCC->actrl = RCLK_BRG | TCLK_BRG;
	SCC->bctrl = 11; SCC->bctrl = RCLK_BRG | TCLK_BRG;

	SCC->actrl = 12; SCC->actrl = scclobaud[VALCBAUD];
	SCC->actrl = 13; SCC->actrl = scchibaud[VALCBAUD];
	SCC->bctrl = 12; SCC->bctrl = scclobaud[VALDBAUD];
	SCC->bctrl = 13; SCC->bctrl = scchibaud[VALDBAUD];

	SCC->actrl = 14; SCC->actrl = EN_BRG; /* (clock source is RTxC) */
	SCC->bctrl = 14; SCC->bctrl = EN_BRG;

	SCC->actrl = 1; SCC->actrl = RXINTALL | PAR_SPCL | EN_TXINT;
	SCC->bctrl = 1; SCC->bctrl = RXINTALL | PAR_SPCL | EN_TXINT;

	SCC->actrl = 3; SCC->actrl = sccharz[VALCBITS]<<6 | RCVR_ON;
	SCC->bctrl = 3; SCC->bctrl = sccharz[VALDBITS]<<6 | RCVR_ON;

	SCC->actrl=5; SCC->actrl=wr5a=DTR_ON|sccharz[VALCBITS]<<5|TXMTR_ON|RTS_ON;
	SCC->bctrl=5; SCC->bctrl=wr5b=DTR_ON|sccharz[VALDBITS]<<5|TXMTR_ON|RTS_ON;
	splx(x);
}

#undef	sccApoke
sccApoke()
{
	if (!stopAout && !TAS(&sccAbsy))
		SCC->adata = qgetc(&SCCAOUTQ);
}

#undef	sccBpoke
sccBpoke()
{
	if (!stopBout && !TAS(&sccBbsy))
		SCC->bdata = qgetc(&SCCBOUTQ);
}

#undef	sccAgetc
sccAgetc()
{
	while (!(SCC->actrl & RX_CHAR))
		;
	return SCC->adata;
}

#undef	sccBgetc
sccBgetc()
{
	while (!(SCC->bctrl & RX_CHAR))
		;
	return SCC->bdata;
}

#undef	sccAputc
sccAputc(c)
{
	while (!(SCC->actrl & TX_RDY))
		;
	SCC->adata = (char) c;
}

#undef	sccBputc
sccBputc(c)
{
	while (!(SCC->bctrl & TX_RDY))
		;
	SCC->bdata = (char) c;
}

#undef	sccAioctl
sccAioctl(code,arg)
{
	int	x = spl7();	/* so register addressing stays in sync */
	
	switch (code) {
#ifdef NOTDEF
	case SETBAUD:
		/* first disable BRG */
		SCC->actrl = 14; SCC->actrl = 0;
		SCC->actrl = 12; SCC->actrl = scclobaud[arg];
		SCC->actrl = 13; SCC->actrl = scchibaud[arg];
		SCC->actrl = 14; SCC->actrl = PCLK2BRG | EN_BRG;
		break;
	case SETCHARZ:
		SCC->actrl = 3; SCC->actrl = sccharz[arg]|RCVR_ON;
		SCC->actrl = 5; SCC->actrl=wr5a=sccharz[arg]|DTR_ON|TXMTR_ON|RTS_ON;
		break;
	case SETPRTY:
		SCC->actrl = 4; SCC->actrl = sccpar[arg] | ONE_STOP;
		break;
#endif
	case STARTBRK:
		SCC->actrl = 5; SCC->actrl = wr5a | SND_BRK;
		break;
	case DISCON:
		SCC->actrl = 5; SCC->actrl = wr5a & ~DTR_ON;
		break;
	case STOPBRK:
	case STOPDISC:
		SCC->actrl = 5; SCC->actrl = wr5a;
		break;
	}
	splx(x);
}

#undef	sccBioctl
sccBioctl(code,arg)
{
	int	x = spl7();	/* so register addressing stays in sync */

	switch (code) {
#ifdef NOTDEF
	case SETBAUD:
		/* first disable BRG */
		SCC->bctrl = 14; SCC->bctrl = 0;
		SCC->bctrl = 12; SCC->bctrl = scclobaud[arg];
		SCC->bctrl = 13; SCC->bctrl = scchibaud[arg];
		SCC->bctrl = 14; SCC->bctrl = PCLK2BRG | EN_BRG;
		break;
	case SETCHARZ:
		SCC->bctrl = 3; SCC->bctrl = sccharz[arg]|RCVR_ON;
		SCC->bctrl = 5; SCC->bctrl = wr5b = sccharz[arg]|DTR_ON|TXMTR_ON|RTS_ON;
		break;
	case SETPRTY:
		SCC->bctrl = 4; SCC->bctrl = sccpar[arg] | ONE_STOP;
		break;
#endif
	case STARTBRK:
		SCC->bctrl = 5; SCC->bctrl = wr5b | SND_BRK;
		break;
	case DISCON:
		SCC->bctrl = 5; SCC->bctrl = wr5b & ~DTR_ON;
		break;
	case STOPBRK:
	case STOPDISC:
		SCC->bctrl = 5; SCC->bctrl = wr5b;
		break;
	}
	splx(x);
}

#define	CTRLS	'\023'
#define CTRLQ	'\021'
#undef piohint
piohint()
{
	register c,v;

	if (!IOCARD)
		excep_int();
	switch (v=IOX->intvec) { /* clear interrupt, read modified vector */
	case 0: /* txb */
txBint:		if (stopBout || (c=qgetc(&SCCBOUTQ)) == -1) {
			SCC->bctrl = CLR_TXINT;
			sccBbsy = 0;
		} else
			SCC->bdata = c;
		break;
	case 2: /* extb */
		SCC->bctrl = CLR_EXTERN;
		break;
	case 4: /* rxb */
	case 6: /* parb */
		/* Recognize flow control characters in resident terminal
		 * mode, when encoding is enabled in layers or stand-alone,
		 * and for printer/aux ports */
		if ((c = SCC->bdata) == CTRLS && D_RECFLOW &&
			   (!VALDWNLDFLAG || ENCODE_ENABLE || VALPHOST != 2)) {
			stopBout = 1;
			break;
		}
		if (c == CTRLQ && stopBout) {
			stopBout = 0;
			sccBbsy = 1;
			goto txBint;
		}
#define HIWAT 350
		/* Generate flow control for host port ONLY, and
		 * only in resident terminal mode */
		if (qputc(&SCCBINQ,c) == HIWAT && !VALDWNLDFLAG && !NOAUTOFLOW
				&& !blocked && VALPHOST == 2) {
			sendchar(CTRLS);
			blocked = -1;
		}
		if (VALDWNLDFLAG == 3 && VALPHOST == 2)
			/* sysrun(0)	-- let demux run */
			(*(int(*)())((int *)0x71d700)[166])(0);
		break;
	case 8: /* txa */
txAint:		if (stopAout || (c=qgetc(&SCCAOUTQ)) == -1) {
			SCC->actrl = CLR_TXINT;
			sccAbsy = 0;
		} else
			SCC->adata = c;
		break;
	case 10: /* exta */
		SCC->bctrl = CLR_EXTERN;
		break;
	case 12: /* rxa */
	case 14: /* para */
		if ((c = SCC->adata) == CTRLS && C_RECFLOW &&
			   (!VALDWNLDFLAG || ENCODE_ENABLE || VALPHOST != 1)) {
			stopAout = 1;
			break;
		}
		if (c == CTRLQ && stopAout) {
			stopAout = 0;
			sccAbsy = 1;
			goto txAint;
		}
		if (qputc(&SCCAINQ,c) == HIWAT && !VALDWNLDFLAG && !NOAUTOFLOW
				&& !blocked && VALPHOST == 1) {
			sendchar(CTRLS);
			blocked = -1;
		}
		if (VALDWNLDFLAG == 3 && VALPHOST == 1)
			/* sysrun(0)	-- let demux run */
			(*(int(*)())((int *)0x71d700)[166])(0);
		break;
	default: /* something's wrong */
		sccinit();
	}
	if (v < 8)
		SCC->bctrl = CLR_IUS;
	else	SCC->actrl = CLR_IUS;  /* clear interrupt */
	asm(" RETPS");
}
