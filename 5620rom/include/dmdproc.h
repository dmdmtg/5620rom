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
#ifndef PROC_H
#define PROC_H	1
#define	NPROC	8
#define	STKSIZ	2048
#define MINSTKSIZ	400		/* minimum stack size */
#define	CBSIZE	64	/* NPROC*CBSIZE <= queue.h/NCHARS */

#ifndef	QUEUE_H
struct cbuf {	/* duplicated from queue.h for simplicity */
	struct cbuf *next;
	short	word;
};
struct clist {
	struct cbuf *c_tail;
	struct cbuf *c_head;
	short	c_cc;
	short	state;
};
#endif
struct pcb {
	int	psw;
	char	(*pc)();
	int	sp;
	int	sp_lower;
	int	sp_upper;
	int	ap;
	int	fp;
	int	reg[9];		/* 9 user registers */
	int	blkmv;
};
struct Proc {
	struct pcb pcb;
	char	*text;
	char	*data;
	char	*bss;
	int	state;
	Layer	 * layer;
	Rectangle rect;	/* rectangle on screen */
	struct clist kbdqueue;
	char	**traploc;
	int	*traptype;
	int	nticks;
	Point	curpt;	/* for the jline etc. crowd */
	Texture16 * cursor;
	short	inhibited;
	short	nchars;
	unsigned char	cbuf[CBSIZE*3];	/* circular buffer */
	unsigned char	*cbufpin, *cbufpout;
	int	*stack;	/* int for alignment  */
	int	*newstack, newstksize;	/* for changing stack size */
};

#ifndef	MPX_H
#	ifdef	TASK_C
	struct Proc *newproc();
	struct Proc proctab[NPROC];
	extern struct Proc *P;		/* declared in vector.s */
#	else
	extern struct Proc *newproc();
	extern struct Proc proctab[NPROC];
	extern struct Proc *P;
#	endif
#endif

#ifdef	MPXTERM
#define PROCSTATES
#endif
#ifdef PANDORA
#define PROCSTATES
#endif
#ifdef PROCSTATES
/*
 * Data at start of user program
 */
struct udata {
	Rectangle Drect;
	Layer	 * Jdisplayp;
	int	argc;
	char	**argv;
	struct Mouse mouse;
};

/* Two system processes */

#define	DEMUX	0		/* packet discipline */
#define	CONTROL 1		/* user interface (does menus, etc.) */

/* states */

#define	RUN		1	/* ready to be scheduled */
#define	BUSY		2	/* active */
#define	BLOCKED		4	/* blocked by user with ^S */
#define	USER		8	/* a user-32ld'd process */
#define	KBDLOCAL	16	/* has requested the KBD */
#define	MOUSELOCAL	32	/* has requested the MOUSE */
#define	GOTMOUSE	64	/* currently owns MOUSE */
#define	WAKEUP		128	/* tell CONTROL to issue setrun(p) */
#define	MOVED		256	/* layer got moved */
#define	UNBLOCKED	512	/* Has been unblocked */
#define	ZOMBIE		1024	/* proc died horribly; waiting for debugger */
#define	RESHAPED	2048	/* layer got reshaped */
#define	ZOMBOOT		4096	/* put in ZOMBIE state after booting */
#define	ALARMREQD	8192	/* has requested an alarm */	
#define LOCKLAYER	0x8000  /* can't delete layer with mouse */
#define PIPETO		0x3F0000 /* pipe chars to layer 0x3F0000 >> 16
				    (for dmdp) - see recvchars() in demux.c -
				    room for 64 layers intentionally reserved
				    in case more layers supported in future */
#define NOPFEXPAND	0x400000 /* do not expand the PF keys */

#define	setrun(p)	((p)->state|=RUN)
#define	setnorun(p)	((p)->state&=~RUN)

#else
#define	MOVED		256	/* layer got moved */
#define	RESHAPED	2048	/* layer got reshaped */
#define LOCKLAYER	0x8000  /* can't delete layer with mouse */
#endif	MPXTERM
#endif
