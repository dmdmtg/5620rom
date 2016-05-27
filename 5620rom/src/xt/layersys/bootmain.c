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

#define	ypos(a)	(P->rect.corner.y-muldiv(a, P->rect.corner.y - P->rect.origin.y, nbytes))

/* long	getlong(); */

Texture16 cup = {
	0x0100, 0x00E0, 0x0010, 0x03E0, 0x0400, 0x0FE0, 0x123C, 0x1FE2,
	0x101A, 0x101A, 0x1002, 0x103C, 0x1810, 0x6FEC, 0x4004, 0x3FF8,
};

#undef boot
boot()
#define boot Vboot
{
	register argc;
	register char	**argv;
	register char	*address;
	/* char	*Ualloc(); */
	/* int	windowstart(); */
	register unsigned	nbytes, i, nargchars;
	int	text, data, bss;
	short	oypos ;
	short 	nypos ;

	Urequest(MOUSE);
	Ucursswitch(&cup);
	argc = getlong();
	nargchars = getlong();
	argv = (char **)Ualloc(nargchars + (argc + 1) * sizeof(char *));
	if (argv == 0)		/* if we couldn't alloc before we can't again */
		address = 0;
	text = getlong();	/* if alloc failed we'll still read the chars */
	data = getlong();
	bss = getlong();
	nbytes = text+data; /* nbytes=text+data, the amount to be downloaded */
	address = Ualloc((unsigned)(nbytes+bss));
	getstk();
	if(address == 0 || (P->newstack == 0 && P->newstksize != 0) )
	  {
		freeall((char *)P);	/* clear the arg space */
		P->newstksize = 0;	/* forget about the new stack size */
		address = 0;		/* force download to abort */
	    }
	P->text = address;
	P->data = address + text;
	P->bss = P->data +data;
	mpxublk(P);
	sendnchars(4, (char *) &P->text);	/* 32ld detects zero address */
	if(address == 0)
		sw(0);			/* wait for 32ld to kill us */
	bldargs(argc, argv);
	for (i = 0; i < nbytes; i++) {
		*address++ = getchar();
		if (i) {
			nypos = ypos(i);
			oypos = ypos(i-1);
			if (nypos != oypos)
				lrectf(P->layer, Rect(P->rect.origin.x, nypos,
			   	    P->rect.corner.x, oypos), F_XOR);
		}
	}
	mpxublk(P);
	clear(P->rect, 1);
	Urequest(0);
	P->state |= USER;
	setdata(P);
#define	udp	((struct udata *)(P->data)) /* start of data section */
	udp->argc = argc;
	udp->argv = argv;     /* these get set as arguments to main in crtm.o */
	P->curpt.x = P->curpt.y = 0; /* set Current Point to upper left corner */
	if (P->state & ZOMBOOT) {
		P->state &= ~ZOMBOOT;
		P->state |= ZOMBIE;
		zombexec(P->text);
	}
	exec(P->text);
}


