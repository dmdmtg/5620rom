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
/* logio.c	Data structures and routines for I/O with logical
 *		devices on the DMD.
 */

#include <dmd.h>
#include <queue.h>
#include <setup.h>


/* Note: The receive queue for physical device 3, the send-only printer,
 * can be the same as its output queue since the output initiation
 * routine takes characters off its queue as fast as they're put on.
 */


extern aciapoke(), sopoke(), sccApoke(), sccBpoke();
extern aciagetc(), ringbell(), sccAgetc(), sccBgetc();
extern aciaputc(), sccAputc(), sccBputc();
extern acioctl(), sopbaud(), sccAioctl(), sccBioctl();

struct physops devtab[4] = {
/* 0=A */  { &RCVQUEUE, &OUTQUEUE, aciapoke, aciagetc, aciaputc, acioctl },
/* 1=C */  { &SCCAINQ,  &SCCAOUTQ, sccApoke, sccAgetc, sccAputc, sccAioctl },
/* 2=D */  { &SCCBINQ,  &SCCBOUTQ, sccBpoke, sccBgetc, sccBputc, sccBioctl },
/* 3=B */  { &SOPQUEUE, &SOPQUEUE, sopoke,   ringbell, ringbell, sopbaud }
};
struct physops *hostops, *prntops, *aux1ops, *aux2ops;

#define ACIA 0
#define SCCA 1
#define SCCB 2
#define SONLY 3

probe_io()	/* Check for presence of I/O card */
		/* setbram() should be called after this */
{
	extern int rexcep_tab[];
	int tmpa, tmpb;
	register r8;

	tmpa = rexcep_tab[10];		/* first patch memory fault vector */
	tmpb = rexcep_tab[11];
	rexcep_tab[10] = 0x281e000;
	asm(" MOVAW	catchfault,rexcep_tab+(5*8)+4");
	/* Try to read the I/O card's DCE status register */
	asm(" MOVB	0x300033,%r8");
	rexcep_tab[10] = tmpa;		/* restore memory fault vector */
	rexcep_tab[11] = tmpb;
	tmpa = IOCARD;
	IOCARD = ((r8 & 0xc0) == 0);	/* top 2 bits are 0 on I/O card */
	if ( !IOCARD || !tmpa ) {	/* If there's no I/O card present */
			/* or one was just added, initialize mapping table */
		physmap[HOST].byte = ACIA; /* 2681 port A */
		physmap[PRINTER].byte = IOCARD? SCCA : SONLY;
		physmap[AUX1].byte = IOCARD? SCCB : SONLY;
		physmap[AUX2].byte = SONLY;
	}
	return;

	/* Memory fault handler for terminals with no I/O card */
	asm("catchfault:");	/* memory fault occurred: no I/O card */
	asm(" MOVB	&0xC0,%r8");
	/* Bump saved %pc past the MOVB 0x300033,%r8 */
	asm(" ADDW2	&7,-8(%sp)");
	asm(" RETG");
}

logports()	/* make ram device pointers correspond to bram */
{
	hostops = &devtab[physmap[HOST].byte];
	prntops = &devtab[physmap[PRINTER].byte];
	aux1ops = &devtab[physmap[AUX1].byte];
	aux2ops = &devtab[physmap[AUX2].byte];
}

sendchar(c)	/* send c to the logical HOST device */
char c;
{
	register n;

	if (n = qputc(hostops->outq,c))
		(*hostops->xpoke)();
	return n;  /* zero if queue was full */
}

unsigned char rcvtemp;		/* for encoding */
int rcvcount /* = 6 */;		/* for encoding */
/* 
 *  A note on LAN encoding:  rcvchar implements the receive side of
 *  LAN encoding for stand-alone programs which aren't layersys.  Layersys
 *  has it's own (slightly different) encoding scheme implemented in demux.
 *  Encoding is never used in resident-terminal.
 *
 *  The send side of encoding is not implemented by sendchar for performance
 *  reasons.  It is instead implemented by sendnchars in comm.c in libj on
 *  the att distribution tape.   The send side for layers is in sendpkt in
 *  control.c
 */

rcvchar()	/* get character from logical HOST port */
{
	register c=qgetc(hostops->rcvq);

	if(c!=-1)
		c&=0xFF;
	if (ENCODE_ENABLE && VALDWNLDFLAG == 1 )   /* don't use if layers is up */
	 {
	   while( c < 0x40 && c != 0x20 ) /* scan for string delimiter */
		while( (c=qgetc(hostops->rcvq)) == -1 ) wait(RCV);
	   if (c == 0x20)
	     {
		rcvcount = 6;
		while( (c=qgetc(hostops->rcvq)) == -1 ) wait(RCV);
	      }
	   if ((rcvcount += 2) == 8)
		   {
			rcvcount = 2;
			rcvtemp = c;
			while( (c=qgetc(hostops->rcvq)) == -1 ) wait(RCV);
		     }
	   return((c & 0x3f) | ((rcvtemp << rcvcount) & 0xc0));
	  }
	 else return c;
}

int
psendchar(c)	/* send c to logical PRNTR device; returns 0 if q full */
char c;
{
	register n;

	if (n = qputc(prntops->outq,c))
		(*prntops->xpoke)();
	return n;
}

prcvchar()
{
	return qgetc(prntops->rcvq);
}

int
aux1outch(c)
{
	register n;

	if (n = qputc(aux1ops->outq,c))
		(*aux1ops->xpoke)();
	return n;
}

aux1inch()
{
	return qgetc(aux1ops->rcvq);
}

aux2outch(c)
{
	register n;

	if (n = qputc(aux2ops->outq,c))
		(*aux2ops->xpoke)();
	return n;   /* zero if queue was full */
}

aux2inch()
{
	return qgetc(aux2ops->rcvq);
}

hostputc(c)
{
	(*hostops->xpoll)(c);
}

hostgetc()
{
	return (*hostops->rpoll)();
}

sendbreak()	/* send break on logical HOST port */
{
	(*hostops->ioctl)(STARTBRK,0);
}

disconnect()	/* drop DTR momentarily on logical HOST port */
{
	(*hostops->ioctl)(DISCON,0);
}

stopbreak()
{
	(*hostops->ioctl)(STOPBRK,0);
}

stopdisc()
{
	(*hostops->ioctl)(STOPDISC,0);
}
