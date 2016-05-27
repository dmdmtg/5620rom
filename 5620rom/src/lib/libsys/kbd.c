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
#include <kbd.h>
#include <setup.h>
#include <font.h>

#define ESC	'\033'
#define ESCB	0200
#define DEL	'\177'
#define	NUL	0200|0xd		/* code for NUL 'cause can't use 0 */
#define K_KEY	0
#define K_CHAR	1


extern int *pvtab;			/* defined in link.c as Sys */
#define hostinit	*(pvtab+181)	/* # Sys[181] : hst_init */

#undef	SetupFlag
int	SetupFlag;		/* indicates we're in Setup */
#define	SetupFlag	SSetupFlag

extern int	setup_pcb;	/* PCB for setup code */

extern Point    pt();

/* keycodes from the keyboard MOS chip are translated into codes placed
 * on the keyboard queue (KBDQUEUE) by this table (and a little help from
 * some code).  The format of an entry is
 *
 *		Code from Keyboard, Code to put on Queue
 *
 * Most of the keys on the main keypad (e.g. 'A', space, '1', etc.) are sent
 * by the MOS chip in ASCII.  These aren't in this table.  The special keys
 * are handled as described below.
 *
 * Controls get put on the queue as the ASCII code for the control
 * char (e.g. CTRL/A is 0x01).  Ditto for tab, escape, backspace and delete.
 * Return is put on the queue as 0x13, regardless of the setting of the 
 * Return Key definition in setup -- that secondary translation is handled
 * by whoever removes Return from the keyboard queue (if they care).
 *
 * Break, disconnect, and the pf keys are put on as
 * 0x80=Break, 0x81=Disconnect and 0x82 through 0x89 are f1,...,f8.
 *
 * The arrow keys are put on as the ANSI 3.64 escape sequence.  This is 
 * indicated in the table as ESCB logically ORed with the ASCII letter
 * terminating the sequence; for example, ESCB | 'A' ==> ESC [ A.  Home Down
 * is special -- it sends ESC [ 70;0 H.  It's up to people pulling that off
 * the queue to interpret it properly; that's for 5410/5420 compatibility.
 *
 * SETUP and Shift/SETUP (reset) are handled directly by the keyboard interrupt
 * routine - these only appear on the queue when setup itself is running.
 * Num Lock and Caps Lock are
 * also handled internally, as obviously, are Shift and Control.
 *
 *  
 * WARNING: A binary search (in keytabsearch) is used to find keys in this
 * table.   The entries must be sorted into ascending order!
 */

unsigned char keytab[][2] = {
	0x8f, 0x81,	 	/* shifted key 42, discon  */
	0x90, ESCB | 'B', 	/* shifted key 109, down arrow */
	0x91, ESCB | 'H', 	/* shifted key 97, left-up arrow */
	0x92, ESCB | 'A', 	/* shifted key 98, up arrow */
	0x93, ESCB | 0xa, 	/* shifted key 99, home down arrow */
	0x94, '4',	 	/* shifted key 79, 4 */
	0x95, '5', 	/* shifted key 80, 5 */
	0x96, '6', 	/* shifted key 81, 6 */
	0x97, '7', 	/* shifted key 60, 7 */
	0x98, '8', 	/* shifted key 61, 8 */
	0x99, '9', 	/* shifted key 62, 9 */
	0x9a, ESCB | 'D',  		/* shifted key 108, left arrow */
	0x9b, ESCB | 'C', 		/* shifted key 110, right arrow */
	0xaf, 0x80, 	/* unshifted key 42, break */
	0xb0, '\0', 	/* control key 35, ignored */
	0xb1, '\0', 	/* control key 26, ignored */
	0xb3, '\0', 	/* control key 28, ignored */
	0xb4, '\0', 	/* control key 29, ignored */
	0xb5, '\0', 	/* control key 30, ignored */
	0xb6, '\012', 	/* unshifted key 96, line-feed */
	0xb7, '\0',  	/* control key 32, ignored */
	0xb8, '\0',  	/* control key 33, ignored */
	0xb9, '\0',  	/* control key 34, ignored */
	0xba, NUL,  	/* control key 27, ignored */
	0xbb, '\0',  	/* control key 75, ignored */
	0xbc, '\0', 	/* control key 31, ignored */
	0xbd, '\0', 	/* control key 37, ignored */
	0xbe, '\0', 	/* control key 59, ignored */
	0xbf, '\0', 	/* control key 36, ignored */
	0xc0, ESCB | 'H', 	/* unshifted key 97, left-up arrow */
	0xc1, ESCB | 'A', 	/* unshifted key 98, up arrow */
	0xc2, ESCB | 'B', 	/* unshifted key 109, down arrow */
	0xc3, ESCB | 'C', 	/* unshifted key 110, right arrow */
	0xc4, ESCB | 'D',  	/* unshifted key 108, left arrow */
	0xc6, ESCB | 0xa,  	/* unshifted key 99, home down arrow */
	0xc7, '\r', 		/* shifted key 77, return */
	0xc8, ESCB | 2, 	/* shifted key 3, f1 */
	0xc9, ESCB | 3, 	/* shifted key 4, f2 */
	0xca, ESCB | 4, 	/* shifted key 5, f3 */
	0xcb, ESCB | 5, 	/* shifted key 8, f4 */
	0xcc, ESCB | 6, 	/* shifted key 9, f5 */
	0xcd, ESCB | 7, 	/* shifted key 12, f6 */
	0xce, ESCB | 8, 	/* shifted key 13, f7 */
	0xcf, ESCB | 9, 	/* shifted key 14, f8 */
	0xd0, '\t', 	/* unshifted key 45, tab */
	0xd1, '\b', 	/* shifted key 38, bs */
	0xd2, '7', 	/* unshifted key 60, 7 */
	0xd3, '4', 	/* unshifted key 79, 4 */
	0xd4, '8', 	/* unshifted key 61, 8 */
	0xd5, '5', 	/* unshifted key 80, 5 */
	0xd6, '9', 	/* unshifted key 62, 9 */
	0xd7, '6', 	/* unshifted key 81, 6 */
	0xde, DEL, 	/* shifted key 39, del */
	0xe2, '\0', 	/* control key 76, */
	0xe3, ESC,  	/* unshifted key 25, esc */
	0xe5, ESCB | 0xb,  	/* unshifted key 40, clr */
	0xe7, '\r', 	/* unshifted key 77, return */
	0xe8, ESCB | 2, 	/* unshifted key 3, f1 */
	0xe9, ESCB | 3, 	/* unshifted key 4, f2 */
	0xea, ESCB | 4, 	/* unshifted key 5, f3 */
	0xeb, ESCB | 5, 	/* unshifted key 8, f4 */
	0xec, ESCB | 6, 	/* unshifted key 9, f5 */
	0xed, ESCB | 7, 	/* unshifted key 12, f6 */
	0xee, ESCB | 8, 	/* unshifted key 13, f7 */
	0xef, ESCB | 9, 	/* unshifted key 14, f8 */
	0xf0, '\t', 	/* shifted key 45, tab */
	0xf1, '\b', 	/* unshifted key 38, bs */
	0xf2, '\0',  	/* shifted key 107, num-lock */
	0xf3, ESC,  	/* shifted key 25, esc */
	0xf5, ESCB | 0xc,  	/* shifted key 40, local clear functin */
	0xf6, '\n', 	/* shifted key 96, line-feed */
	0xfe, DEL,  	/* unshifted key 39, del */
};

#define MUXPORT	0x8
#define MUX_K	0
#define MUX_P	1

int	muxdir;

#undef kbdrepeat
#undef kbdstatus
int	kbdrepeat, 	/* code for key being repeated */
	rptcount, 	/* timer for repeat -- called out of clock interupt */
	kbdstatus, 	/* status byte to/from keyboard */
	dispstatus;	/* ? */

#define kbdrepeat Skbdrepeat
#define kbdstatus Skbdstatus

/*
 * Numeric lock translation.  When the keyboard is in "Numeric Lock" (indicated
 * by a status byte) the codes from the numeric keypad (on the right of the
 * keyboard) are translated using this table.
 */

unsigned char	numlocktab[7] = {'1','2','0','.','-',0xc5,'3'};

#undef kbdchar
kbdchar()
#define kbdchar Skbdchar
{
	return qgetc(&KBDQUEUE);
}


#undef kbdinit
kbdinit()
#define kbdinit Skbdinit
{
	/* init	the keyboard */
	DUART->b_cmnd = RESET_RECV | DIS_TX | DIS_RX;
	DUART->b_cmnd = RESET_TRANS;
	DUART->b_cmnd = RESET_ERR;
	DUART->b_cmnd = RESET_MR;
	DUART->mr1_2b = CHAR_ERR | PAR_ENB | EVN_PAR | CBITS8;
	DUART->mr1_2b = NRML_MOD | ONEP000SB;
	DUART->b_sr_csr	 = BD4800BPS;
	DUART->b_cmnd = RESET_MR | ENB_TX | ENB_RX;
	DUART->scc_sopbc = 0x80; /* turn on DTR for aux printer port */
	DUART->scc_ropbc = MUXPORT; /* set output pins for kbd tx port*/
	muxdir = MUX_K;  /* initially, mux points to kbd */
	/* turn chirps on/off depending on BRAM */

	if (VALKEYTONE)
		kbdstatus = 0;	/* no chirp */
	else
		kbdstatus = TTY_CHIRP;	/* chirp, chirp */
	DUART->b_data = kbdstatus | 0x02; /* request status */
}

#undef ringbell
void
ringbell()
#define ringbell Sringbell
{
	kbdstatus |= TTY_ALARM;  /* done in one instruction */
	while (trysend(0,MUX_K) == 0);
	kbdstatus &= ~TTY_ALARM;
}

#undef	sopbaud
sopbaud(code,arg)
{
	/* Switch to keyboard so next printer char uses new baud rate */
	while (!kpready(MUX_K))
		;
}

#undef	sopoke
void
sopoke()	/* Put a char out the send-only port */
{
	register c;
	while ( (c=qgetc(&SOPQUEUE)) != -1 )
		while (trysend(c,MUX_P) == 0)
			;
}

trysend(byte,dest)	/* try to send byte to keyboard or printer */
 register byte,dest;
{
	register done,x;

	done = 0;
	x = spl7();
		if (kpready(dest)) {
			if (dest == MUX_K)
				DUART->b_data = kbdstatus;
			else
				DUART->b_data = byte;
			done = 1;
		}
	splx(x);
	return done;
}

kpready(kp)	/* Is the keyboard/printer port ready? */
 register kp;	/* this should be called with interrupts off */
{
	DUART->b_cmnd = NO_OP;	/* toggle r/w lead before doing a read */
	if (muxdir == kp) {
		if (DUART->b_sr_csr & XMT_RDY)
			return 1;
		else	return 0;
	} else {
		if (DUART->b_sr_csr & XMT_EMT) { /* last char is all gone */
			/* okay to switch the mux now */
			if (kp) {  /* printer port */
				DUART->scc_sopbc = MUXPORT; /* set mux */
				DUART->b_sr_csr = BD4800BPS & 0xf0 |
						  baud_speeds[VALBBAUD] & 0xf;
			} else {   /* keyboard */
				DUART->scc_ropbc = MUXPORT; /* reset mux */
				DUART->b_sr_csr = BD4800BPS;
			}
			muxdir = kp;
			return 1;
		} else
			return 0;
	}
}

/*
 * Keyboard Interrupt Service Routine
 *
 *	Called as a function; it expects caller to do RETPS.
 */

#undef auto2
auto2()
#define auto2 Sauto2
{
	register i;
	register start, end;
	Point	setupcur;
	char	s;
	char	c;

	/* don't actually set the repeat bit until the character after the control code */
	
	DUART->b_cmnd = NO_OP;	/* toggle r/w lead before doing a read */
	s = DUART->b_sr_csr;
	DUART->b_cmnd = NO_OP;	/* toggle r/w lead before doing a read */
	c = DUART->b_data;

	if (s & (FRM_ERR | OVR_RUN)) {
		DUART->b_cmnd = RESET_ERR;
		return; /* framing error or overrun error */
	}

	if (s & PAR_ERR) {
		/* parity error indicates this is a control word */
		/* see what state caps lock is in 	*/
		qsetbram(&VALCAPS,((c & 0x4) ? 0 : 1)); /* set the caps lock flag */
		caps_msg();
		if (c & 0x10) {
			/* turn repeat off */
			kbdrepeat = 0;
			rptcount = 0;
		}
		else { /* turn repeat on, the next character is to be repeated */
			kbdrepeat = RPTON;
		}
		return;
	}

	rptcount = 0;	/* new charcter so restart repeat timer	*/
	if (c & 0x80  ) {
		switch (c & 0xff) {
		case 0xb2:	/* numeric lock toggle */
		case 0xf2:
			if( kbdrepeat & RPTON )
			 {
				kbdrepeat=0;
				return;
			  }
			if (VALNUM == 1)
				qsetbram (&VALNUM,0);
			else if (VALNUM == 0)
				qsetbram (&VALNUM,1);
			num_msg();
			return;
		case 0x8e:	/* un/shited key 41 (Shift Set-up) */
			if( kbdrepeat & RPTON )
			 {
				kbdrepeat=0;
				return;
			  }
			if( SetupFlag )		/* setup code will handle it */
			 {
				qputc( &KBDQUEUE, c );
				return;
			  }
			if (!VALDWNLDFLAG)	/* in resident terminal */	
			   test32(1);		/* do selftest and then reboot */
			reboot();		/* otherwise, just reboot */
		case 0xae:	/* setup */
			if( kbdrepeat & RPTON )
			 {
				kbdrepeat=0;
				return;
			  }
			if( SetupFlag )		/* setup code will handle it */
			 {
				qputc( &KBDQUEUE, c );
				return;
			  }
			/* What's really going on here?  The interrupt stack
 			 * has the PCBP for the process that this keyboard
			 * interrupt suspended.  When we do a RETPS (actually, our
			 * caller does it), the PCBP at the top of the interrupt
			 * stack is used to restart the process.  But what we're
			 * doing here is placing the PCBP for setup at the top
			 * of the interrupt stack.  That forces the RETPS to start
			 * the setup process.  When it finishes, it does a RETPS,
			 * which resumes the originally interrupted process.
			 */
/* 	SetupFlag = 1;		will be reset by setup */
			qclear(&KBDQUEUE);	/* dump the queue */
asm("			ADDW2	&4, %isp ");
asm("			MOVAW	rsetup_pcb, -4(%isp) ");
			return;		/* branch off to setup */
		}
		if ((VALNUM) && (c >= 0xc0) && (c <= 0xc6)) {
			c = numlocktab[c - 0xc0];
			qputc(&KBDQUEUE, (int)c);
			if((kbdrepeat & RPTMASK) == RPTON) {
				kbdrepeat = RPTHAVECHR | RPTON;
				kbdrepeat += (c & 0xff);
			}
			return;
		}

		i = keytabsearch(c);
/*
 *	here either keytab[i][K_KEY] == c, or c is not in the table
 *	If c is not in the table, there is an error somewhere, so ring the bell
 */
		if (c == keytab[i][K_KEY]) {
		   if(((c = (keytab[i][K_CHAR]) & 0xff) >= 0x80) &&
		       (c <= 0x89) ) /* pfkey, break, disconnect, clear */
			{
				if(kbdrepeat & RPTON)
					kbdrepeat = 0; /* don't want to repeat */
			         else qputc( &KBDQUEUE, c );
				return;
			}
			kchkchar(c);
			if ((kbdrepeat & RPTMASK) == RPTON) {
				/* this is it! */
				kbdrepeat = RPTHAVECHR | RPTON | RPTLOOKUP;
				kbdrepeat += i;
			}
		}
		else {
			ringbell();
		}
	}
	else {
		qputc(&KBDQUEUE, (int)c);
		if((kbdrepeat & RPTMASK) == RPTON) {
			kbdrepeat = RPTHAVECHR | RPTON;
			kbdrepeat += (c & 0xff);
		}
	}
}



kchkchar(c)
register char c;
{
	switch (c & 0xff) {
	case 0x8a:	/* HOME DOWN	*/
		qputstr( &KBDQUEUE, "\033[70;1H");
		break;
	case 0x8b:	/* CLEAR */
		if(kbdrepeat & RPTON) { /* turn off repeat */
			kbdrepeat = 0;
			return;
		}
		qputstr( &KBDQUEUE, "\033[2J");
		break;
	case 0x8c:	/* local clear function */
		if(kbdrepeat & RPTON) {
			kbdrepeat = 0;
			return;
		}
		else
			qputc(&KBDQUEUE, c);
		break;
	case 0x8d:	/* null */
		qputc(&KBDQUEUE, (char)0);
		break;
	case 0x80 | 'A':
	case 0x80 | 'B':
	case 0x80 | 'C':
	case 0x80 | 'D':
	case 0x80 | 'H':
		qputstr( &KBDQUEUE, "\033[");
		qputc( &KBDQUEUE, c & 0x7f);
		break;
	default:
		if (c != '\0')
			qputc(&KBDQUEUE, (int)c);
	}
}


/* This guy is called off the clock interrupt routine when it's time
 * to repeat a character.  It's up to the clock interrupt routine to
 * decide how often to call this routine, which determines the repeat
 * rate.
 */
#undef kbdrpt
kbdrpt()
#define kbdrpt	Skbdrpt
{
	char	c;

	if (kbdrepeat & RPTLOOKUP) {
		if ((c = keytab[kbdrepeat & 0xff][K_CHAR]) & 0x80) {
			kchkchar(c);
		}
		else if (c != '\0') {
			qputc(&KBDQUEUE, (int)c);
		}
	}
	else {
		qputc(&KBDQUEUE, kbdrepeat & 0xff);
	}
}

keytabsearch(c)		/* binary search of keytab[] for char c */
unsigned char c;
{
	register int start, end, i;
/*
 *	Assume start, end and i are indexes into keytab,
 *	an array of two element structures. The elements are:
 *		keytab[][2], the two elements are:
 *			char key;
 *			char c; -- the char to put on queue, 0 -> key ignored
 */

		start = 0;
		end = sizeof(keytab) / sizeof(keytab[0]);
		for (i = (start + end) / 2; 
		    (keytab[i][K_KEY] != c) && (start <= end); 
		    i = (start + end) / 2)
			if (c > keytab[i][K_KEY])
				start = i + 1;
			else
				end = i - 1;

	/*
	 *	Either keytab[i][K_KEY] == c, or c is not in the table
	 *	If c is not in the table, there is an error somewhere,
	 *	but caller will worry about that.
	 */
	return (i);
}

/* Pictures of NUM and CAPS indicators that pop up on the bottom of the screen */

Word num_lock[] = {
			0xFFFFFFFF,0xFFFFFFFF,0xF993939F,0xF993939F,
			0xF893911F,0xF893929F,0xF813929F,0xF913939F,
			0xF913939F,0xF990139F,0xF998339F,0xFFFFFFFF,
			0xFFFFFFFF,0xFFFFFFFF };

Word caps_lock[] = {
			0xFFFFFFFF,0xFFFFFFFF,0xC3C78381,0x81830101,
			0x9D39393F,0x9F393907,0x9F010183,0x9F3903F9,
			0x9D393FF9,0xC1393F01,0xC3393F03,0xFFFFFFFF,
			0xFFFFFFFF,0xFFFFFFFF };


Bitmap num_icon = { (Word *)num_lock, 1,
			{{(short)0,(short)0},{(short)32,(short)14}},
			(char *)0};

Bitmap caps_icon = { (Word *)caps_lock, 1,
			{{(short)0,(short)0},{(short)32,(short)14}},
			(char *)0};
	
/*
 * Points at which to draw caps and num icon
 */

Point	caps_pt = { (short)269, (short)(YMAX-NS) };
Point	num_pt = { (short)480, (short)(YMAX-NS) };


caps_msg()
{
	if( VALDWNLDFLAG && !SetupFlag )	/* Don't draw it unless
						   resident terminal or setup is
						   running.  */
		return;
	cursinhibit();
	bitblt(&caps_icon,caps_icon.rect,&display,caps_pt,(VALCAPS ? F_STORE : F_CLR));
	cursallow();
}
num_msg()
{
	int mode;

	if( VALDWNLDFLAG && !SetupFlag )	/* Don't draw it unless
						   resident terminal or setup is
						   running.  */
		return;
	cursinhibit();
	bitblt(&num_icon,num_icon.rect,&display,num_pt,(VALNUM ? F_STORE : F_CLR));
	cursallow();
}

/* RAW mode keyboard get routine.   Polls keyboard and returns
 * raw code from keyboard MOS.  Note that it ignores anything on the queue.
 */

#undef kgetc
kgetc()
#define kgetc Skgetc
{
	register int	c;
	register char	s;
	register start, end, i;


	DUART->b_cmnd = NO_OP;	/* toggle r/w lead before doing a read */
	while ((s = (DUART->b_sr_csr & (RCV_RDY | FRM_ERR | PAR_ERR |
					 OVR_RUN | RCVD_BRK))) == 0)
		DUART->b_cmnd = NO_OP;	/* toggle r/w lead before doing a read */
	if (!(s & RCV_RDY))
		return(-1);
	DUART->b_cmnd = NO_OP;	/* toggle r/w lead before doing a read */
	c = DUART->b_data;
	if ((s & (FRM_ERR | PAR_ERR | OVR_RUN | RCVD_BRK)) == 0) /* just a char */
 		if(kbdrepeat & RPTON) {
			kbdrepeat = 0;	/* turn off repeat */
			return(-1);
		}
	 	else return(c);
	else if ((s & PAR_ERR) && (s & RCV_RDY))
		if(c & 0x10) {
			/* not repeating anything */
			kbdrepeat = 0;
			rptcount = 0;
		}
		else   /*
			* the keyboard will send us a character to repeat
			* which we will throw away.
			*/
			kbdrepeat = RPTON;
	       else if(s & OVR_RUN) /* reset on overrun error */
			DUART->b_cmnd = RESET_ERR;
	return(-1);
}

