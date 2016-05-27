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
#define QUEUE_C
#include <sa.h>
#include "queue.h"
#include <setup.h>
#undef NULL
#define	NULL		((struct cbuf *)0)
#define pswmax()	asm("	PUSHW	%psw	");\
			asm("	ORW2	&0x281e000,%psw ");\
			asm("	NOP	");\

#define pswback()	asm("	POPW	%psw	");\
			asm("	NOP	");\

/*
 * These are mostly for speed, not size.
 * UGH ICK UGH (but fast)
*/

#undef qinit
qinit()
#define qinit Sqinit
{
	register struct cbuf *p, *q;

	q = NULL;
	for(p=cbufs; p< &cbufs[NCHARS]; p++)  {
		p->next = q;
		q = p;
	}
	freelist = q;
}

#undef qputc
qputc(q, w)
#define qputc Sqputc
	register struct clist *q;
	register w;
{
	register cc;
	register struct cbuf *p;
	pswmax();
	p = freelist;
	if (p==NULL || (q->c_cc > 3 * NCHARS / 4 && VALDWNLDFLAG)) {
		/* limit any individual queue to 3/4 of all the queue space in layers */
		pswback();
		if(!VALDWNLDFLAG ) {
			qclear(q);
			ringbell();	/* send a warning to the user */
		}
		return(0);
	}
	freelist = p->next;
	p->next = NULL;
	p->word = w;
	if (q->c_cc == 0)
		q->c_head = q->c_tail = p;
	else {
		q->c_tail->next = p;
		q->c_tail = p;
	}
	cc= ++q->c_cc;
	pswback();
	return(cc);
}

#undef qputstr
qputstr(q, str)
#define qputstr Sqputstr
	register struct clist *q;
	register char *str;
{
	register retval;
	while((*str != '\0') && ((retval = qputc(q, *str++)) >= 0));
	return retval;
}

#undef qgetc
qgetc(q)
#define qgetc Sqgetc
	register struct clist *q;
{
	register struct cbuf *p;
	register x;
	pswmax();
	if ((p=q->c_head)==NULL) {
		pswback();
		return(-1);
	}
	if (--q->c_cc == 0)
		q->c_head = q->c_tail = NULL;
	else
		q->c_head = p->next;
	p->next = freelist;
	x = p->word;
	freelist = p;
	pswback();
	return(x);
}

#undef qclear
qclear(q)
#define qclear Sqclear
	register struct clist *q;
{
	register dummy;
	pswmax();
	if(q->c_cc == 0){
		pswback();
		return;
	}
	q->c_cc = 0;
	q->c_tail->next = freelist;
	freelist = q->c_head;
	q->c_head = NULL;
	q->c_tail = NULL;
	pswback();
}
