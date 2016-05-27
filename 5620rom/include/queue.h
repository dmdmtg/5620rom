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
#define	QUEUE_H
#define	NCHARS	512
#define NULL ((char *)0)
#define	STOPPED	1

struct cbuf {
	struct	cbuf *next;
	short	word;
};
struct clist {
	struct	cbuf *c_tail;
	struct	cbuf *c_head;
	short	c_cc;
	short	state;
};


#ifdef QUEUE_C
#undef cbufs
#undef queues
#undef freelist
struct cbuf cbufs[NCHARS];
struct clist queues[8];
struct cbuf *freelist;
#endif
#ifdef ROMTERM
extern struct cbuf cbufs[NCHARS];
extern struct clist queues[8];
extern struct cbuf *freelist;
#endif
#ifdef MPXTERM
#ifndef MPXPATCH
extern struct cbuf cbufs[NCHARS];
extern struct clist queues[8];
extern struct cbuf *freelist;
#endif
#endif

#define	RCVQUEUE	queues[0]	/* chars from host */
#define	KBDQUEUE	queues[1]	/* chars from keyboard */
#define	OUTQUEUE	queues[2]	/* chars to send to host */
#define SOPQUEUE	queues[3]	/* for send-only port */
#define SCCAINQ		queues[4]
#define SCCAOUTQ	queues[5]
#define SCCBINQ		queues[6]
#define SCCBOUTQ	queues[7]

struct physops {
	struct clist *rcvq;	/* Input queue for physical device */
	struct clist *outq;	/* Output queue for physical device */
	int	(*xpoke)();	/* Transmit start-up routine */
	int	(*rpoll)();	/* Polled input routine */
	int	(*xpoll)();	/* Polled output routine */
	int	(*ioctl)();	/* I/O control - baud rate, etc. */
};
