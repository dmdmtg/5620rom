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
#include <pconfig.h>
#include <proto.h>
#include <packets.h>
#include <setup.h>
#include "vector.h"

#define MAXKBD	CBSIZE
extern short second;
extern struct Pchannel pconvs[NPROC];
extern char kbdlist[MAXKBD];
extern char *kbdlistp;
extern struct Proc *queueproc;

#undef Sw
Sw()
#define Sw VSw
{
	if (second) {
		second = 0;
		if (Ptflag)
			ptimeout();
	}
	sw(P != whichaddr(CONTROL));	/* if control, clock will restart us */
}

#undef Psend
Psend(a, b, c, d)
#define Psend VPsend
char	*b;
{
	while (psend(a, b, c, d) == -1)
		Sw();
}


void
#undef sendnchars
sendnchars(n, p)
#define sendnchars Vsendnchars
int	n; 
char	*p;
{
	register int	cc;

	do {
		if ((cc = n) > (MAXPKTDSIZE - 1))
			cc = MAXPKTDSIZE - 1;
		Psend(getprocnum(P), p, cc, C_SENDNCHARS);
	} while (p += cc, (n -= cc) > 0);
}


extern short	sendbusy; /* defined in vector.s */
extern struct physops *hostops;

int
#undef sendpkt
sendpkt(p, n)
#define sendpkt Vsendpkt
register char	*	p;
register int	n;
{
	register int	sr;
	unsigned char c1, c2, c3;
	short i;

	while (hostops->outq->c_cc > CBSIZE / 2)
		sw(1);
	while (sendbusy) 
		sw(1);
	sendbusy = 1;
	sr = spl1();
/*	trdisable();	 */
  	if (!ENCODE_ENABLE) {
	    do
		qputc(hostops->outq, *(unsigned char *)p++);
	    while (--n);
  	}
	else {
	    for (i = 0 ; i < n ; i+=3) {
		c1 = *p++;
		if (i+1 < n)
			c2 = *p++;
		else
			c2 = 0;
		if (i+2 < n)
			c3 = *p++;
		else
			c3 = 0;

		if (i == 0)			/* packet header */
		    qputc(hostops->outq, (c1&0xc0)>>2 | (c2&0xc0)>>4 | (c3&0xc0)>>6);
		else
		    qputc(hostops->outq,0x40|(c1&0xc0)>>2|(c2&0xc0)>>4|(c3&0xc0)>>6);
		qputc(hostops->outq, 0x40 | (c1&0x3f));
		if (i+1 < n)
			qputc(hostops->outq, 0x40 | (c2&0x3f));
		if (i+2 < n)
			qputc(hostops->outq, 0x40 | (c3&0x3f));
	    }
	}
/*	trenable();	 */
	splx(sr);
	sendbusy = 0;
/*	aciatrint();	 */
	(*hostops->xpoke)();	/* start up output */
	return 0;
}


#undef mpxnewwind
mpxnewwind(p, c)
#define mpxnewwind Vmpxnewwind
register struct Proc *p;
char	c;
{
	char	mesg[6];
	register int	dx, dy;
	register char	*	cp = mesg;

	dx = p->rect.corner.x - p->rect.origin.x;
	dy = p->rect.corner.y - p->rect.origin.y;
	*cp++ = (dx - 6) / 9;
	*cp++ = (dy - 6) / 16;
	*cp++ = dx;
	*cp++ = (dx >> 8);
	*cp++ = dy;
	*cp++ = (dy >> 8);
	Psend(getprocnum(p), mesg, sizeof mesg, c);
}


int
#undef mpxsendchar
mpxsendchar(c, p)
#define mpxsendchar Vmpxsendchar
char	c;
struct Proc *p;
{
	if (sendbusy || (hostops->outq->c_cc >= (CBSIZE / 2)))
		return - 1;	/* avoid "sw" in "sendpkt" */
	return psend(getprocnum(p), &c, 1, C_SENDCHAR);
}


#undef mpxkbdflush
mpxkbdflush()	/* cannot hang: must return on wait ==> can't use sendnchars */
#define mpxkbdflush Vmpxkbdflush
{
	register int	cc,n,i;
	register char	*left, *right;

	if (kbdlistp <= kbdlist || sendbusy || (hostops->outq->c_cc >= (CBSIZE / 2)))
		return;	/* avoid "sw" in "sendpkt" */
	n = (int)(kbdlistp - kbdlist);
	if (( cc = n) > (MAXPKTDSIZE - 1))
		cc = MAXPKTDSIZE - 1;
	if (psend(getprocnum(queueproc), kbdlist,cc, C_SENDNCHARS) != -1) {
		if (n == cc)
			kbdlistp = kbdlist;
		else {
			for(i=n-cc,left=kbdlist,right= &kbdlist[cc]; i > 0 ; i--)
				*left++ = *right++;
			kbdlistp = left;
		}
	}
}


#undef mpxdelwind
mpxdelwind(w)
#define mpxdelwind Vmpxdelwind
{
	Psend(w, (char *)0, 0, C_DELETE);
}


#undef mpxublk
mpxublk(p)
#define mpxublk Vmpxublk
register struct Proc *p;
{
	register int	l = getprocnum(p);

	while (pconvs[l].user > 0) {
		pconvs[l].user--;
		Psend(l, (char *)0, 0, C_UNBLK);
	}
}


#undef mpxcore
mpxcore(w)
#define mpxcore Vmpxcore
{
	Psend(w, (char *)0, 0, C_BRAINDEATH);
}


