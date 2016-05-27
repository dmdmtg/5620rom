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
#define MPX_H
#include <dmdproc.h>
#include <font.h>
#include <kbd.h>
#include <setup.h>
#include "vector.h"
#undef cursor
#undef cur
#undef savecur
extern struct Proc *P;
extern Sys[];
extern Font defont;
extern char *ver_str;
extern long ticks0;

#define ERR	-1
#define LASTCHAR 0
#define MORECHARS 1
#define NONUMBERS 2
#define MAX_ESC_TOKENS	2
#define NEWLINESIZE	16	/* because of stipples */
#define INNEWLINESIZE	NEWLINESIZE -1
#define INCW		CW - 1 
#define BLINK_TIME	30
#ifndef VALCURSOR
#define VALCURSOR	VALKEYTONE
#endif

#undef XMAX
#undef YMAX
#define XMAX	(((P->rect.corner.x - (P->rect.origin.x +3 +XMARGIN))/CW)  -1)
#define YMAX	(((P->rect.corner.y - (P->rect.origin.y +3 +YMARGIN))/NEWLINESIZE) -1)

/* Pinfo structures are maintained on the stack for each process running
 * windowproc.  They are also passed around a lot as parameters.
 */

struct Pinfo {
	Point cur;
	Point savecur;
	int xmax;
	int ymax;
	int p_u, p_rev;	/* underscore and reverse, respectively */
};

/* Point     pt(); */
/* Rectangle RECT(); */

/* Windowproc is the default terminal program for layers.
 *
 * It is started by control when a new layer is created.
 * It is started by demux when a program running in a layer
 * dies.  It's sort of a user program and sort of part of layersys.
 *
 * windowproc text and data is shared among all layers.  Thus, no
 * global variables allowed!  Everything's got to be on the stack
 * so that a separate copy is maintained for each layer running
 * windowproc.
 *
 */

#undef windowproc
windowproc()
#define windowproc Vwindowproc
{

	register char c;
	char cbuf;
	int token[MAX_ESC_TOKENS];
	int numtokens,token0;
	register i;
	struct Pinfo *pptr, pinfo;


	pptr = &pinfo;
	pptr->p_u = pptr->p_rev = 0;	/* initially no attributes */
	P->state &= ~MOVED;
	P->state |= RESHAPED;
	while(1) {
		c = readchar(pptr);
Nocurse:
		switch(c){
		case '\007':	    /* bell */
			ringbell();
			break;
		case '\t':	    /* tab modulo 8 */
			pptr->cur.x|=7;
			pptr->cur.x++;
			break;
		case '\033':	    /* start of esc sequence */
			while((c = readchar(pptr)) == '\033');	/* ignore leading escapes */
			switch(c) {
			case 'c':	/* reset terminal */
				pptr->cur.x = pptr->cur.y = 0;
				pptr->savecur = pptr->cur;
				ringbell ();
				clear(RECT( Rect(0, 0, pptr->xmax+1, pptr->ymax+1)), 1);
				break;
			case '7':	/* save cursor position */
				pptr->savecur = pptr->cur;
				break;
			case '8':	/* restore cursor position */
				pptr->cur = pptr->savecur;
				break;
			default:    /* unrecognized escape seq */
				break;
			case '[':	/* Now for the real decoding */
				numtokens = numbers(token,&cbuf,pptr);

				/* numbers breaks up the tokens of the sequence, */
				/* with the key character going to cbuf.	*/
				/* Note the wierdness with token0; it isn't */
				/* necessarily equal to token[0] */

				token0 = (numtokens ? token[0] : 1);
				if(token0 == 0) token0++;

				switch (cbuf) {
				default:    /* unrecognized sequence */
					break;
				case '\033':	/* ESC char */
					c = cbuf;
					goto Nocurse;
				case 'q':	/* set pf key */
					{
					   char	pfbuf[PFKEYSIZE]; /* from setup.h */
					   int  i,j;

					   if ( (numtokens != 2) ||
						(token[0] < 1) ||
					        (token[0] > NUMPFKEYS) ||
						(token[1] > PFKEYSIZE) )
						     break;	/* illegal */
					   for (i=0; i<token[1]; i++)
						pfbuf[i] = readchar(pptr);
					   j = --token[0];	/* for array ref */
					   spl4();		/* Only EIA and PIOH
								   interrupts */
					   checkbram();		/* mark checksum */
					   for (i=0; i<token[1]; i++)
					     BRAM->pfkeys[j][i].byte = pfbuf[i];
					   BRAM->pfkeys[j][token[1]].byte = 0;
					   setbram();	/* fix BRAM checksum */
					   spl0();		/* interrupts OK */
					   break;
					  }
				case 'F':	/* encoding inquire */
					{
					   char buf[4];

					     if((numtokens == 1) && (token[0] != 0))
						       break;
					     buf[0] = '\033';
					     buf[1] = '[';
					     buf[3] = 'F';
					     buf[2] = (char)VALENCODING+'0';
					     sendnchars(4,buf);
					     break;
					 }
				case 'A':	/* cursor up */
					--pptr->cur.y;
					break;
				case 'B':	/* cursor down */
					pptr->cur.y++;
					break;
				case 'C':	/* cursor right */
					if(++pptr->cur.x > pptr->xmax )
						pptr->cur.x = pptr->xmax;	/* clip to right */
					;
					break;
				case 'D':	/* cursor left */
					goto casebackspace;
				case 'K':
					if (token[0] == 0)   /* clear to EOL */
						clear(RECT(Rect(pptr->cur.x, pptr->cur.y, pptr->xmax+1, pptr->cur.y+1)),1);
					break;
				case 'J':
					if (token[0] == 0)   /* clear to EOS */
					{
						clear( RECT( Rect(pptr->cur.x, pptr->cur.y, pptr->xmax+1, pptr->cur.y+1)), 1);
						clear(RECT( Rect(0, pptr->cur.y+1, pptr->xmax+1, pptr->ymax+1)), 1);
					}
					else if (token[0] == 2) /* clear screen */
					{
						clear(RECT( Rect(0, 0, pptr->xmax+1, pptr->ymax+1)), 1);
					}
					break;
				case 'c':	/* send terminal id */
					if((numtokens == 1) && (token[0] != 0))
						break;
					sendnchars(strlen(ver_str),ver_str); /* from vitty.c */
					break;
				case 'f':
				case 'H':	/* cursor move */
					if (numtokens == 0)
						pptr->cur.x = pptr->cur.y = 0;
					else {
						if(token[0]) token[0]--;
						pptr->cur.y = (token[0] > pptr->ymax ? pptr->ymax:token[0]);
						if(numtokens == 2)  {
							if(token[1]) token[1]--;
							pptr->cur.x = (token[1] > pptr->xmax ? pptr->xmax:token[1]);
						}
						else
							pptr->cur.x = 0;
					}
					break;
				case 'S':
					lscroll(Rect(0, (token0 > pptr->ymax? pptr->ymax+1 : token0), pptr->xmax+1, pptr->ymax+1), Pt(0, 0), pptr);
					break;
				case 'T':
					if (token0 > pptr->ymax)
						i = 0;
					else
						i = pptr->ymax + 1 - token0;
					lscroll(Rect(0, 0, pptr->xmax+1, i), Pt(0, token0), pptr);
					break;
				case 'm':	/* character attributes */
					for( i=0; i<numtokens; i++)
						switch(	token[i] ) {
						   case 0: pptr->p_u=pptr->p_rev=0;
								break;
						   case 2:
						   case 5:
						   case 7: pptr->p_rev=1; break;
						   case 4: pptr->p_u=1; break;
						 }
					 break;
				case '@':  /* insert char */
					lscroll(Rect(pptr->cur.x, pptr->cur.y, pptr->xmax+1-token0, pptr->cur.y+1), Pt(pptr->cur.x+token0, pptr->cur.y), pptr);
					break;
				case 'P':  /*  char delete */
					lscroll(Rect(pptr->cur.x+token0, pptr->cur.y, pptr->xmax+2, pptr->cur.y+1), Pt(pptr->cur.x, pptr->cur.y), pptr);
					break;
				case 'L':  /* line insert */
					if(token0 > pptr->ymax) i = 0;
					else i = pptr->ymax +1 - token0;
					lscroll(Rect(0, pptr->cur.y, pptr->xmax+2, i), Pt(0, pptr->cur.y+token0), pptr);
					break;
				case 'M': /* line delete */
					lscroll(Rect(0, pptr->cur.y+token0, pptr->xmax+2, pptr->ymax+1), Pt(0, pptr->cur.y), pptr);
					break;
				case 'g': /* request for terminal RAM size */
					{
					char buf[4];

					buf[0] = '\033';
					buf[1] = '[';
					buf[3] = 'g';
					buf[2] = (char)VALMAXADDR + '0'; /* 0 = 256K, 1 = 1M */
					sendnchars(4,buf);
					break;
					}




				}
			}
			break;
		case '\b':	    /* backspace */
casebackspace:
			--pptr->cur.x;
			break;
		case '\n':	    /* linefeed */
casenewline:
			newline(pptr);
			break;
		case '\r':	    /* carriage return */
			pptr->cur.x=0;
			break;
		default:
			if( c < 0x20 || c == 0177 )	/* control char */
			 if( VALCONTROLS == 1 ) /* invisible */
				break;
			  else if( VALCONTROLS == 2 ) /* spaces */
					c = ' ';
			writec( c, pptr, F_OR);
			pptr->cur.x++;
			break;
		}
		if(pptr->cur.x > pptr->xmax) {
			newline(pptr);
			pptr->cur.x = 0;
		}
		if(pptr->cur.x > pptr->xmax+1)
			pptr->cur.x = pptr->xmax+1;
		else if(pptr->cur.x < 0)
			pptr->cur.x = 0;
		if(pptr->cur.y > pptr->ymax)
			pptr->cur.y = pptr->ymax;
		else if(pptr->cur.y < 0)
			pptr->cur.y = 0;
		if(P->nchars > 0) {
			c = readchar(pptr);
			goto Nocurse;
		}
	}
}
#undef numbers
numbers(n,c,pptr)
#define numbers Vnumbers
int *n; 
char *c;
struct Pinfo *pptr;
{	    /* a simple token scanning routine, looks for up to 2 (3?)
	   numbers, separated by ';', followed by a single char.
	   The numbers are stuffed into array n, the char in c */

	register int numtokens=0;

	for (numtokens = 0; numtokens < MAX_ESC_TOKENS; numtokens++)
	{
		switch ( getanum(&(n[numtokens]) , c, pptr) )
		{
		default:
		case ERR:
			return(ERR);
		case MORECHARS:
			break;
		case NONUMBERS:
			if(numtokens == 0)
				return(0);
		case LASTCHAR:
			return(numtokens + 1);
		}
	}
	return(ERR);
}

#undef getanum
getanum(n,c,pptr)    /* this algorithm isnt perfect, eg, it will accept ';;' */
#define getanum Vgetanum
int *n;
char *c;
struct Pinfo *pptr;
{
	register char tmpc;
	register int num = 0;
	register int found=0;

	while(	((tmpc = readchar(pptr)) <= '9') && (tmpc >= '0')  )
	{
		num = num * 10 + (int) (tmpc - '0');
		found = 1;
	}
	*n = num;
	if ( tmpc == ';' )
		return(MORECHARS);
	else
	{
		*c = tmpc;
		if (found)
			return(LASTCHAR);
		else
			return(NONUMBERS);
	}
}
#undef curse
curse(pptr)
#define curse Vcurse
struct Pinfo *pptr;
{
	Point	p;

	 p = pt(pptr->cur);
			/* Used to be:	writec( '\1', pptr, F_XOR); */

	if( P->layer->obs == 0 )	/* we aren't obscured, use speed hack */
		rectf(P->layer,Rpt(p,Pt(p.x+CW,p.y+NEWLINESIZE)),F_XOR);
	 else lrectf( P->layer,Rpt(p,Pt(p.x+CW,p.y+NEWLINESIZE)),F_XOR);
}

nocurse(pptr)
struct Pinfo *pptr;
{
	Point	p;

   p = pt(pptr->cur);

   if (P->layer->obs == 0)	/* we aren't obscured, use speed hack */
      rectf(P->layer,Rpt(Pt(p.x+1,p.y+1),Pt(p.x+INCW,p.y+INNEWLINESIZE)),F_XOR);
   else 
      lrectf(P->layer,Rpt(Pt(p.x+1,p.y+1),Pt(p.x+INCW,p.y+INNEWLINESIZE)),F_XOR);
}

#undef readchar
readchar(pptr)
#define readchar Vreadchar
struct Pinfo *pptr;
{
	register char c;
	register int curson;	/* 0: disappeared, 1:present (for block cursor) */
	register int curcur;	/* 0: non current(box), 1: current(block) */
	register int alarmed;
	register int ticks;
	register int wasblocked;

	wasblocked = 0;
	curson = 0; curcur = 1;
	if (P->state & ALARMREQD) {
		alarmed = 1;
		ticks = P->nticks + ticks0;
	}
	else
		alarmed = 0;
	while ((P->state & RESHAPED) || P->nchars == 0 || (P->state & BLOCKED)) {
		if(P->state & MOVED) 
			P->state &= ~(RESHAPED|MOVED);
		if(P->state & RESHAPED) {
			P->state &= ~RESHAPED;
			pptr->cur.x = 0;
			pptr->cur.y = 0;
			pptr->savecur.x = pptr->savecur.y = 0;
			pptr->xmax = XMAX;
			pptr->ymax = YMAX;
			curse(pptr);
			curson = 1;
			if (P==kbdproc) {
				curcur = 1;
			}
			else {
				nocurse (pptr);
				curcur = 0;
			}
		}
		if (!curson) {
			curse(pptr);
			curson = 1;
		}
		if (P != kbdproc) {
			nocurse(pptr);
			curcur = 1 - curcur;
		}
		while (P->nchars == 0 || (P->state & BLOCKED)) {
			alarm(BLINK_TIME);
			if( P->state & BLOCKED ) {
				wasblocked = 1;
				Uwait(ALARM);
			}
			else
				Uwait(RCV|ALARM);
			if ((P->state & RESHAPED) == 0)
				if (P != kbdproc) {
					if (!curson)
					{	curse (pptr);
						curson = 1 - curson;
					}
					if (curcur) {
						nocurse(pptr);
						curcur = 0;
					}
				}
				else {
					if (!curcur) {
						nocurse (pptr);
						curcur = 1;
					}
					if ((VALCURSOR || !curson) && P->nticks == 0) {
						curse(pptr);
						curson = 1 - curson;
					}
				}
			else
				break;
		}
	}

	/* Always fade away if input arrives */
	if (wasblocked)
		mpxublk(P);
	if (curson)
		curse(pptr);
	if (!curcur) 
		nocurse (pptr);

	if (alarmed)
		P->nticks = (ticks > ticks0) ? ticks - ticks0 : 0;
	else {
		P->state &= ~(ALARMREQD);
		P->nticks = 0;
	}
	c = *(P->cbufpout++) & 0x7F;
	P->nchars--;
	if(P->cbufpout >= &P->cbuf[sizeof(P->cbuf)])
		P->cbufpout = P->cbuf;
	return(c);
}
Point
#undef pt
pt(p)
#define pt Vpt
Point p;
{
	register short *a= &p.x;
	*a*=CW; 
	*a++ +=(XMARGIN + P->rect.origin.x);
	*a*=NEWLINESIZE; 
	*a   +=(YMARGIN + P->rect.origin.y);
	return p;
}
Rectangle
#undef RECT
RECT(r)
#define RECT VRECT
Rectangle r;
{
	register short *a= &r.origin.x;
	*a*=CW; 
	*a++ +=(XMARGIN + P->rect.origin.x);
	*a*=NEWLINESIZE; 
	*a++ +=(YMARGIN + P->rect.origin.y);
	*a*=CW; 
	*a++ +=(XMARGIN + P->rect.origin.x );
	*a*=NEWLINESIZE; 
	*a   +=(YMARGIN + P->rect.origin.y  );
	return r;
}
/*
 * Scroll rectangle r horizontally or vertically to p.	Clear the
 * area that opens up.
 */
#undef lscroll
lscroll(r, p, pptr)
#define lscroll Vlscroll
Rectangle r;
Point p;
struct Pinfo *pptr;
{


	clippt(&r.origin,pptr);
	clippt(&r.corner,pptr);
	clippt(&p,pptr);
	if(eqpt(p, r.origin))
		return;
	Ucursinhibit();
	lbitblt(P->layer, RECT(r), P->layer, pt(p), F_STORE);
	if(p.x==r.origin.x){	/* vertical scroll */
		if(p.y<r.origin.y)	/* scroll up; clear bottom */
			clear(RECT(Rpt(Pt(p.x, r.corner.y-r.origin.y+p.y), r.corner)), 0);
		else /* scroll down; clear top */
			clear(RECT(Rpt(r.origin, Pt(r.corner.x, p.y))), 0);
	} else{ 	 /* horizontal scroll */
		if(p.x<r.origin.x)	/* scroll left; clear right */
			clear(RECT(Rpt(Pt(r.corner.x-r.origin.x+p.x, p.y), r.corner)), 0);
		else /* scroll right; clear left */
			clear(RECT(Rpt(r.origin, Pt(p.x, r.corner.y))), 0);
	}
	Ucursallow();
}
#undef clippt
clippt(p,pptr)
#define clippt Vclippt
register Point *p;
struct Pinfo *pptr;
{
	if(p->x<0)
		p->x=0;
	if(p->y<0)
		p->y=0;
	if(p->x>pptr->xmax+1)
		p->x=pptr->xmax+1;
	if(p->y>pptr->ymax+1)
		p->y=pptr->ymax+1;
}

#undef writec
writec( c, pptr, mode)
#define writec Vwritec

  register char c;
  struct Pinfo *pptr;
  Code mode;
{
	Point cursor;
	Point p;
	Rectangle r;
	register cursoff = 0;
	register Fontchar *fp = defont.info + c;

	cursor.x = pptr->cur.x;
	cursor.y = pptr->cur.y;
	r.origin.x = fp->x;
	r.corner.x = (fp+1)->x;
	r.origin.y = 0;
	r.corner.y = defont.height;
	p = pt(cursor);
	if (mode != F_XOR) {
		Ucursinhibit();
		cursoff++;
		mode = F_STORE;
	}
	if (P->layer->obs == 0)
		bitblt(defont.bits, r, P->layer, p, mode);
	else
		lblt(defont.bits, r, P->layer, p, mode);
	if( pptr->p_u ) /* underscore */
		lsegment(P->layer,Pt(p.x,p.y+NS-2),Pt(p.x+CW,p.y+NS-2),F_STORE);
	if( pptr->p_rev ) /* reverse video */
		lrectf(	P->layer, Rpt(p,Pt(p.x+CW,p.y+NS)),F_XOR);
	if (cursoff)
		Ucursallow();
}
#undef newline
newline(pptr)
#define newline Vnewline
struct Pinfo *pptr;
{
	register nl = nlcount(pptr);
	if(pptr->cur.y >= pptr->ymax){
		Ucursinhibit();
		lscrolx(P->layer,nl);
		clear(Rect(P->rect.origin.x, P->rect.corner.y-(NEWLINESIZE*nl),
		    P->rect.corner.x, P->rect.corner.y), 0);
		Ucursallow();
		pptr->cur.y = pptr->ymax+1-nl;
	} else
		pptr->cur.y++;
	if (VALNEWLINE)
		pptr->cur.x = 0;
}

#undef lscrolx
lscrolx(l,lines)
#define lscrolx Vlscrolx
register Layer *l;
register lines;
{
	Rectangle r;

	r = inset(l->rect,4);
	r.origin.y += (NEWLINESIZE*lines) + YMARGIN;
	lbitblt(l, r, l, Pt(r.origin.x, r.origin.y  - (NEWLINESIZE * lines)), F_STORE);
}
#undef nlcount
nlcount(pptr)
#define nlcount Vnlcount
struct Pinfo *pptr;
{
	register unsigned char *p = P->cbufpout;
	register unsigned char *pp = &P->cbuf[sizeof(P->cbuf)];
	register int i = 0;
	register int j;
	if(j = P->nchars) /* assignment */
		while (j>0 && *p !='\033' && *p !='\013') {
			if (*p == '\n')
				i++;
			if(p++ == pp)
				p = P->cbuf;
			j--;
		}
	return(i>0? (i<pptr->ymax? i : pptr->ymax) : 
	1);
}
