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
#ifndef	SETUP_H
#define	SETUP_H	1

#ifndef	KEYCODE_H
#include	<keycodes.h>
#endif

#define BRAM ((struct blop *) 0x600000)	/* address of battery backed ram */

/* These defines are used inline to reference option values */

#define BAUD	0
#define RETKEY	1
#define NEWLINE	2
#define DUPLEX	3
#define KEYTONE 4
#define SCREENCOLOR	5
#define PFKEYS	6
#define NUMSETUPS	7
#define NUMOPTS	6
#define NUMEXOPTS 33

#define NUMBAUD		6
#define NUMRETKEYS	3
#define NUMNEWLINE	2
#define NUMDUPLEX	2
#define NUMKEYTONE	2
#define NUMSCREENCOLOR	2
#define NUMPFKEYS	8
#define PFKEYSIZE	50


#define VALABAUD	BRAM->opts.opt_array[BAUD].byte
#define VALRETKEY	BRAM->opts.opt_array[RETKEY].byte
#define VALNEWLINE	BRAM->opts.opt_array[NEWLINE].byte
#define VALADUPLEX	BRAM->opts.opt_array[DUPLEX].byte
#define VALKEYTONE	BRAM->opts.opt_array[KEYTONE].byte
#define VALSCREENCOLOR	BRAM->opts.opt_array[SCREENCOLOR].byte
#define VALCAPS		BRAM->capslock.byte
#define VALNUM		BRAM->numlock.byte
#define VALDWNLDFLAG	BRAM->dwnldflag.byte
#define VALMAXADDR	BRAM->bmaxaddr.byte

#define	VALBAUD		VALABAUD
#define	VALDUPLEX	VALADUPLEX

/* Options added in 8;7;5 firmware */

#define	VALAPRTY	BRAM->exopts[0].byte
#define	VALABITS	BRAM->exopts[1].byte
#define	VALATYPE	BRAM->exopts[2].byte
#define	VALBBAUD	BRAM->exopts[3].byte
#define	VALBPRTY	BRAM->exopts[4].byte
#define	VALBBITS	BRAM->exopts[5].byte
#define	VALBTYPE	BRAM->exopts[6].byte
#define	VALCBAUD	BRAM->exopts[7].byte
#define	VALCPRTY	BRAM->exopts[8].byte
#define	VALCBITS	BRAM->exopts[9].byte
#define	VALCTYPE	BRAM->exopts[10].byte
#define	VALENCODING	BRAM->exopts[11].byte
#define	VALMOUSE	BRAM->exopts[12].byte
#define	VALREPEAT	BRAM->exopts[13].byte
#define	VALCURSOR	BRAM->exopts[14].byte
#define	ENCODE_ENABLE	BRAM->exopts[15].byte
#define	VALCONTROLS	BRAM->exopts[16].byte
#define	VALDBAUD	BRAM->exopts[17].byte
#define	VALDPRTY	BRAM->exopts[18].byte
#define	VALDBITS	BRAM->exopts[19].byte
#define	VALDTYPE	BRAM->exopts[20].byte
#define	VALIDMAP	BRAM->exopts[21].byte
#define	VALPHOST	BRAM->exopts[22].byte
#define	VALPPRNT	BRAM->exopts[23].byte
#define	VALPAUX1	BRAM->exopts[24].byte
#define	VALPAUX2	BRAM->exopts[25].byte
#define	IOCARD		BRAM->exopts[26].byte
#define NOAUTOFLOW	BRAM->exopts[27].byte
#define A_RECFLOW	BRAM->exopts[28].byte
#define C_RECFLOW	BRAM->exopts[29].byte
#define D_RECFLOW	BRAM->exopts[30].byte
#define VALRAMSPLIT	BRAM->exopts[31].byte
#define VALSENDCTRLS	BRAM->exopts[32].byte


#define mapvalid VALIDMAP
#define physmap  (&BRAM->exopts[22])	/* So you can say physmap[2].byte */

#define HOST	0	/* indices for physmap[] */
#define PRINTER	1
#define AUX1	2
#define AUX2	3

#define SETBAUD  0	/* I/O control function codes */
#define SETCHARZ 1
#define SETPRTY  2
#define STARTBRK 3
#define DISCON   4
#define STOPBRK  5
#define STOPDISC 6

#undef newline		/* a different newline ... (ah, for separate name pools) */

/* This is what Battery RAM looks like as a C structure.  It's byte addressable
 * and occupies 8K byte addresses in the processor address space, but only every
 * fourth byte is used for storage.   Only bytes with addresses ending in 2,
 * 6, a or e are valid storage locations.
 */

struct bram_mem {
	int:16;		unsigned char byte;
	int:8 
	};
/*
 * This declares the options layout in BRAM.  It's a little weird because
 * some options were added in 8;7;5 firmware that weren't present in 8;7;3
 * firmware.  For compatibility, the new options are added at the end, although
 * it would have been neater to simply expand the opt_array.  (ah, for forward
 * planning)
 */

struct blop {				/* weird names */
	union blip{			/* must've meant something to someone */
		struct vals {
			int:16;		unsigned char baud;
			int:8;
			int:16;		unsigned char retkey;
			int:8;
			int:16;		unsigned char newline;
			int:8;
			int:16;		unsigned char duplex;
			int:8;
			int:16;		unsigned char keytone;
			int:8;
			int:16;		unsigned char screencolor;
			int:8;
			} individ;
		struct bram_mem opt_array[NUMOPTS];
		} opts;
	struct bram_mem pfkeys[NUMPFKEYS][PFKEYSIZE+1];	/* PF keys */
	struct bram_mem numlock;		/* num lock flag */
	struct bram_mem capslock;		/* caps lock flag */
	struct bram_mem dwnldflag;		/* Downloaded Program Flag: */
						/* 0 - No program loaded */
						/* 1 - S.A. program running */
						/* 3 - Layers running */
	struct bram_mem bmaxaddr;		/* RAM size: 0=256KB, 1=1MB */
	struct bram_mem exopts[NUMEXOPTS];	/* Extra options for 8;7;5 */
	};

/* These are here for historical reasons */

/* Font properties */
#define	CW	9	/* width of a character */
#define	NS	14	/* newline size; height of a character */
#define	CURSOR	"\1"	/* By convention */

#define	XMARGIN	3	/* inset from border */
#define	YMARGIN	3
#define XCMAX ((XMAX-2*XMARGIN)/CW-1)
#define YCMAX ((YMAX-2*YMARGIN)/NS-3)

#endif
