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
/*
 * vitty.c 1.1.1.15 Changed 12/6/84 13:37:03
 *
 * termcap:
 * TD|dmd|5620|ttydmd|tty5620|5620 terminal 88 columns:\
 *	:co#88:li#70:am:bs:pt:\
 *	:cm=\E[%i%d;%dH:nd=\E[C:up=\E[A:\
 *	:cl=\E[H\E[J:cd=\E[J:ce=\E[K:\
 *	:al=\E[L:dl=\E[M:\
 *	:im=:ei=:ic=\E[@:dm=:ed=:dc=\E[P:\
 *	:sr=\E[T:sf=\E[S:\
 *	:kl=\E[D:kr=\E[C:ku=\E[A:kd=\E[B:kh=\E[H:
 *
 *
 * vitty is the resident terminal program.  It gets called soon after
 * power-up and runs until a download is requested.  It also runs after
 * the downloaded program exits.
 *
 * It tries to be an ANSI 3.64 terminal.
 */
#include <dmd.h>
#include <font.h>
#include <queue.h>
#include <kbd.h>
#include <setup.h>

#define ESC	'\033'
#define ERR	-1
#define LASTCHAR 0
#define MORECHARS 1
#define NONUMBERS 2
#define MAX_ESC_TOKENS	2
#define BLINK_TIME 	30
#ifndef VALCURSOR
#define VALCURSOR VALKEYTONE
#endif

/*** UPDATE BOTH ver_str AND ver_int BEFORE AN OFFICIAL RELEASE ***/
char *ver_str = "\033[?8;7;5c";	
int ver_int = 0x00080705;	/* ver_str as a hex integer */

version()
{
	return(ver_int);
}

Point	cur;		/* in character positions */
Point	savecur;	/* saved value of above */
extern int kbdstatus;	/* keyboard status int */
extern char baud_speeds[];	/* used for initing line */
int	remote;	/* on line?  only works with dale's wiring mods */
int	btime; /* used to count time for a blinking cursor */

Point	Pt();
Rectangle	Rect();
Point	  pt();
Rectangle rect();


extern struct physops *hostops;
extern Font	defont;
extern Bitmap	display;

int main()
{
	char buf[2];
	char cbuf;
	char	badbuf[4];	/* used to display invalid escape sequences */
	int token[MAX_ESC_TOKENS];
	int numtokens;
	int token0;	/* used when default values should be used */
	register char c;
	register j;
	Rectangle r;
	Fontchar *iptr;
	Font *f;
	Point p;
	int curson;
	int p_rev=0, p_us=0;	/* reverse, underscore mode flags */

	buf[1]=0;
	remote=1;
	*DADDR = 0;		/* do jinit by hand */
	checkbram();		/* zero out bram if checksum is bad */
	VALDWNLDFLAG = 0;
	setbram();
	if (VALSCREENCOLOR)
		BonW();
	else
		WonB();
	qinit();
	aciainit(baud_speeds[VALBAUD]);	
	binit();
	kbdinit();
	cursinit();
	spl0();
	cursinhibit();
	request(RCV);	/* KBD gets handled in introutine() */
	cur.x=0;
	cur.y=0;
	curse();
	nap(30);	/* allow time for keyboard to send status on caps lock */
	caps_msg();
	num_msg();
	while(VALDWNLDFLAG == 0) {
		curson = 1;
		btime = 0;
		while ((own() & RCV) == 0)
			if ((VALCURSOR || !curson)  &&  btime >= BLINK_TIME) {
				btime = 0;
				curson = 1 - curson;
				curse();
			}
		if (curson)
			curse();
		buf[0] = qgetc(hostops->rcvq)&0x7F;
	Nocurse:
		switch(buf[0]){
		case '\007':		/* bell */
			ringbell();	/* ding!!!*/
			break;
		case '\t':		/* tab modulo 8 */
			cur.x|=7;
			cur.x++;
			break;
		case '\033':		/* start of esc sequence */
			curse();	/* turn cursor back on so it wont */
					/* disappear during esc seqs */
			while((cbuf = getchar()) == '\033');	/* ignore leading escapes */
			curse();
			switch(cbuf) {
			case 'c':	/* reset terminal */
				kbdstatus |= 0x04;
				while (!trysend(0,0)); /* like ringbell */
				asm(" JMP reboot");
			case '7':	/* save cursor position */
				savecur = cur;
				break;
			case '8':	/* restore cursor position */
				cur = savecur;
				break;
			default:	/* unrecognized escape seq */
				break;
			case '[':	/* Now for the real decoding */
				curse();	/* turn cursor on so it wont */
						/* disappear during esc seqs */
				numtokens = numbers(token,&cbuf);
						/* numbers breaks up the    */
						/* tokens of the sequence,  */
						/* with the key character   */
						/* going to cbuf	    */
				curse();
				token0 = (numtokens ? token[0] : 1);
				if(token0 == 0)
					token0++;
				switch (cbuf) {
				default:	/* unrecognized sequence */
					break;

				case '\033':	/* escape character */
						goto Nocurse;
				case 'v':		/* download */
					checkbram();
					spl7();		/* Nothing stops us now */
					VALDWNLDFLAG =1;
					setbram();
					return(token[0] | (token[1] << 16));
						
				case 'q':	/* set pf key */
					{
					   char	pfbuf[PFKEYSIZE]; /* from setup.h */
					   int  i;

					   if ( (numtokens != 2) ||
						(token[0] < 1) ||
					        (token[0] > NUMPFKEYS) ||
						(token[1] > PFKEYSIZE) )
						     break;	/* illegal */
					   for (i=0; i<token[1]; i++)
						pfbuf[i] = getchar();
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
					if((numtokens == 1) && (token[0] != 0))
						break;
					sendchar('\033');
					sendchar('[');
					sendchar((char)VALENCODING+'0');
					sendchar('F');
					break;
					 
				case 'A':	/* cursor up */
					--cur.y;
					break;
				case 'B':	/* cursor down */
					cur.y++;
					break;
				case 'C':	/* cursor right */
					if (cur.x < XCMAX)
						cur.x++;
					break;
				case 'D':	/* cursor left */
					goto casebackspace;
				case 'K':
					if (token[0] == 0)   /* clear to EOL */
						clear(Rect(cur.x, cur.y, XCMAX+1, cur.y+1));
	/*				else if (token[0] == 2) /* clear Line */
	/*					clear(Rect(0, cur.y, XCMAX+1, cur.y+1));	*/
	/*  TAKEN out due to TTY request, since vi, etc. rarely use */
					break;
				case 'J':
					if (token[0] == 0)   /* clear to EOS */
					{
						clear(Rect(cur.x, cur.y, XCMAX+1,
							cur.y+1));
						clear(Rect(0, cur.y+1, XCMAX+1,
							YCMAX+1));
					}
					else if (token[0] == 2) /* clear screen */
					{
						clear(Rect(0, 0, XCMAX+1, YCMAX+1));
					}
					break;
				case 'c':	/* send terminal id */
					if((numtokens == 1) && (token[0] != 0))
						break;
					{ register char *cp;
					for(cp=ver_str;*cp;cp++)
						sendchar(*cp);
					}
					break;
				case 'f':
				case 'H':	/* cursor move */
					/*
					* Although to the users the home position looks
					* like (1,1) the real home position will be
					* (0,0).  So we must check the parameters that
					* they give and subtract 1 to get our frame
					* of reference.
					*/
					if(numtokens == 0)
						cur.x = cur.y = 0;
					else
					{
						if(token[0]) token[0]--;
								/* out of range will */
						cur.y = (token[0] > YCMAX ? YCMAX:token[0]);
						if(numtokens == 2) {
							if(token[1]) token[1]--;
							cur.x = (token[1] > XCMAX ? XCMAX:token[1]);
						}
						else
							cur.x = 0;
					}
					break;
				case 'S':
						scroll(Rect(0,  token0 , XCMAX+1, YCMAX+1), Pt(0, 0), YCMAX);
					break;
				case 'T':
						if (token0 > YCMAX)
							j = 0;
						else 
							j = YCMAX + 1 - token0;
						scroll(Rect(0, 0, XCMAX+1, j), Pt(0, token0), YCMAX);
					break;
				case 'm':	/* character attributes */
					for( j=0; j<numtokens; j++)
						switch(	token[j] ) {
						   case 0: p_us=p_rev=0;
								break;
						   case 2:
						   case 5:
						   case 7: p_rev=1; break;
						   case 4: p_us=1; break;
						 }
					 break;
				case '@':    /*  Insert chars */
						if((token0 + cur.x) > XCMAX)
							{
							clear(Rect(cur.x,cur.y,XCMAX+1,cur.y+1));
						}
						else
							scroll(Rect(cur.x, cur.y,XCMAX + 1 - token0, cur.y+1), Pt(cur.x+token0, cur.y), YCMAX);
					break;
				case 'P':   /* Delete chars */
						scroll(Rect(cur.x+token0, cur.y, XCMAX+1, cur.y+1), Pt(cur.x, cur.y), YCMAX);
					break;
				case 'L':   /* Insert lines */
						if((token0 + cur.y) > YCMAX)
						{
							clear(Rect(0,cur.y,XCMAX+1,YCMAX+1));
						}
						else
							scroll(Rect(0, cur.y, XCMAX+1, YCMAX + 1 - token0), Pt(0,cur.y+token0), YCMAX);
					break;				
				case 'M':  /* Delete lines */
						scroll(Rect(0, cur.y+token0, XCMAX+1, YCMAX + 1), Pt(0, cur.y), YCMAX);
					break;				
				case 'g': /* request memory size  */
						qputstr(&OUTQUEUE,"\033[");
						qputc(&OUTQUEUE,(char)VALMAXADDR + '0');
						qputc(&OUTQUEUE,'g');
						aciatrint();
					break;
				case 'h': /* set RAM split for layers / s.a. */
						checkbram();
						spl7();
						VALRAMSPLIT = token[0];
						setbram();
						spl0();
						break;
					
				}
			}
			break;
		case '\b':		/* backspace */
		casebackspace:
			--cur.x;
			break;
		case '\n':		/* linefeed */
		casenewline:
			newline();
			break;
		case '\r':		/* carriage return */
			cur.x=0;
			break;
		default:
			/*
			* used to be just the line
			* string(&defont,buf,&display,pt(cur),F_STORE);
			*/
			c = (char) buf[0];
			if( c < 0x20 || c == 0177 )	/* control char */
			 if( VALCONTROLS == 1 ) /* invisible */
				break;
			  else if( VALCONTROLS == 2 ) /* spaces */
					c = ' ';
			f = &defont;
			p = pt(cur);
			iptr=f->info + c;
			r.origin.y = 0;
			r.corner.y = f->height;
			r.origin.x = iptr->x;
			r.corner.x = (iptr+1)->x;
			p.x += ((iptr->left & 0x80) ? iptr->left | 0xffffff00 : iptr->left);
			p.y += r.origin.y;
			bitblt(f->bits,r,&display,p,F_STORE);
			if( p_us )	/* underscore */
				segment(&display,Pt(p.x,p.y+NS-2),Pt(p.x+CW,p.y+NS-2),F_STORE);
			if( p_rev )	/* reverse video */
				rectf(&display,Rpt(p,Pt(p.x+CW,p.y+NS)),F_XOR);
			cur.x++;
			break;
		}
		if(cur.x > XCMAX) {
			newline();
			cur.x = 0;
		}
		clippt(&cur, YCMAX - 1);
		if(hostops->rcvq->c_cc>0) {
			buf[0] = qgetc(hostops->rcvq)&0x7F;
			goto Nocurse;
		}
		curse();
	}
}
numbers(n,c)
int *n; char *c;
{		/* a simple token scanning routine, looks for up to 2 (3?)
		   numbers, separated by ';', followed by a single char.
		   The numbers are stuffed into array n, the char in c */
		
	register int numtokens=0;
	
	for (numtokens = 0; numtokens < MAX_ESC_TOKENS; numtokens++)
	{
		switch ( getanum(&(n[numtokens]) , c) )	
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

getanum(n,c)	/* this algorithm isnt perfect, eg, it will accept ';;' */
int *n;
char *c;
{
	register char tmpc;
	register int num = 0;
	register int found=0;

	while(  ((tmpc = getchar()) <= '9') && (tmpc >= '0')  )
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
newline()
{
	register nl;
	if(cur.y >= YCMAX){
		scroll(Rect(0, nl=nlcount(), XCMAX+1, YCMAX+1), Pt(0, 0), YCMAX);
		cur.y = YCMAX+1-nl;
	}else
		cur.y++;
	if (VALNEWLINE)
		cur.x = 0;
}
nlcount()
{
	register struct cbuf *p,*pp;
	register int i=0;
	if(p=hostops->rcvq->c_head)	/* assignment = */
		for (i=0; (pp=p->next)!=0 && p->word!='\033' && p->word!='\013'; p=pp)
			if (p->word == '\n')
				i++;
	return(i>0? (i<YCMAX? i : YCMAX) : 1);
}

curse()
{
  Rectangle	r;

	r.origin = pt(cur);
	r.corner = add(r.origin,Pt(CW,NS));
	rectf(&display, r, F_XOR);
}
getchar()
{
	while(hostops->rcvq->c_cc==0)
		wait(RCV);
	return qgetc(hostops->rcvq)&0x7F;
}
getnum(){
	return getchar()-' ';
}
Point
pt(p)
	Point p;
{
	register short *a= &p.x;
	*a*=CW; *a+++=XMARGIN;
	*a*=NS; *a  +=YMARGIN;
	return p;
}
Rectangle
rect(r)
	Rectangle r;
{
	register short *a= &r.origin.x;
	*a*=CW; *a+++=XMARGIN;
	*a*=NS; *a+++=YMARGIN;
	*a*=CW; *a+++=XMARGIN;
	*a*=NS; *a  +=YMARGIN;
	return r;
}
/*
 * Scroll rectangle r horizontally or vertically to p.  Clear the
 * area that opens up.
 */
scroll(r, p, effymax)
	Rectangle r;
	Point p;
	int effymax;	/* effective YCMAX, used so pfkeys can use scroll and clippt */
{
	clippt(&r.origin, effymax);
	clippt(&r.corner, effymax);
	clippt(&p, effymax);
	if(eqpt(p, r.origin))
		return;
	bitblt(&display, rect(r), &display, pt(p), F_STORE);
	if(p.x==r.origin.x){	/* vertical scroll */
		if(p.y<r.origin.y)	/* scroll up; clear bottom */
			clear(Rpt(Pt(p.x, r.corner.y-r.origin.y+p.y), r.corner));
		else			/* scroll down; clear top */
			clear(Rpt(r.origin, Pt(r.corner.x, p.y)));
	}else{			/* horizontal scroll */
		if(p.x<r.origin.x)	/* scroll left; clear right */
			clear(Rpt(Pt(r.corner.x-r.origin.x+p.x, p.y), r.corner));
		else			/* scroll right; clear left */
			clear(Rpt(r.origin, Pt(p.x, r.corner.y)));
	}
}
clippt(p, effymax)
	register Point *p;
	int effymax;	/* effective value of YCMAX, so pfkey can use clippt */
{
	if(p->x<0)
		p->x=0;
	if(p->y<0)
		p->y=0;
	if(p->x>XCMAX+1)
		p->x=XCMAX+1;
	if(p->y>effymax+1)
		p->y=effymax+1;
}
clear(r)
	Rectangle r;
{
	rectf(&display, rect(r), F_CLR);
}
/*
 * introutine() called at video interrupt time.
 *	reads chars off the keyboard, sends things to host
 */

int blocked,ublocked;
extern int kbdrepeat;
extern int SetupFlag;
int Break_cnt;/* This counter is used to time the sending of the 'break' signal */
char	disc_on;
int rpt_time;
#define	HIWAT	(NCHARS-100)
#define	LOWAT	100
#define CTRLS	'\023'
#define CTRLQ	'\021'
introutine()
{
	char c;
	register i, j;

	if((kbdrepeat & RPTHAVECHR) && (kbdrepeat & RPTON) && (++rpt_time >= (4-VALREPEAT))) {
		rpt_time = 0;
		kbdrpt();
	}	/* must be done before SetupFlag tested */
	if( SetupFlag ) return;			/* setup owns the keyboard */
	while(KBDQUEUE.c_cc>0 && !Break_cnt) {
		if ((c=qgetc(&KBDQUEUE)) & 0x80) {
			if(c==0x80) {		/* Break! */
				sendbreak();
				Break_cnt= 24; /*24/60 sec ~ 400 msec break time*/
			}
			else if (c == 0x81) {		/* Disconnect*/
				disconnect();
				Break_cnt = 24;
				disc_on = 1;
			}
			else if(c == 0x8c)		/* local clear key */
				{
				qputstr(hostops->rcvq,"\033[H\033[2J");
				continue;
				}
			else if (c <= 0x89)		/* Pf key: overflow possible */
			{
				j = (c & 0xf) - 2;
				i = 0;
				while ((c = BRAM->pfkeys[j][i].byte) &&
					(++i <= PFKEYSIZE))
				{
					if (VALDUPLEX) /* halfduplex? */
						qputc(hostops->rcvq, c);
					sendchar(c);
				}
				continue;
			}

			else
				sendchar(c);
		} else {
			if (c == CTRLS && blocked <= 0) { /* this is first ^S */
				int x = spl7();
				int wasblocked = blocked;
				blocked = 1;  /* make sure host_int doesn't send one */
				splx(x);
				if (wasblocked == 0)
					sendchar(CTRLS);
			} else {
				if (blocked == 1) /* allow autoblocking again */
					blocked = 0;
				if (c=='\r')	/* CR */
				{
					switch (VALRETKEY){
					case 0:	/* CR */
						break;
					case 1:	/* LF */
						c = '\n';
						break;
					case 2:	/* CR/LF */
						/* UGH */
						sendchar('\r');
						if (VALDUPLEX) /* halfduplex? */
							qputc(hostops->rcvq, c);
						c = '\n';
					}
				}
				sendchar(c);
			}
		}
		if (VALDUPLEX && !Break_cnt)		/* In half duplex? */
			qputc(hostops->rcvq, c);
	}	/* end of while */

	btime++;  /* used for blinking the cursor */
	/*
	* unblock the terminal if the communication line is dropped
	*/
#ifdef NOTDEF
	DUART->a_cmnd = NO_OP;	/* toggle r/w lead before doing a read */
	if((DUART->ip_opcr & 0x40) != 0)
		blocked = 0;
#endif
	if(blocked == -1 && hostops->rcvq->c_cc<LOWAT) {
		sendchar(CTRLQ); /* restart if autoblocked */
		blocked=0;
	}
	if(Break_cnt && --Break_cnt <= 0) {
		if (disc_on) {
			disc_on = 0;
			stopdisc();
		} else
			stopbreak();
		Break_cnt = 0;
	}
}
