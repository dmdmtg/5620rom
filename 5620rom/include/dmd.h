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
 *  Model 5620 DMD
 */

/*
 *	Hardware dependant definitions
 */

#include <sys/2681.h>

#define	DUART		((struct duart *)0x200000)
#define	TVIDEO		(DUART->scc_sopbc = 0x02)
#define	RVIDEO		(DUART->scc_ropbc = 0x02)
#define BonW()		RVIDEO
#define	WonB()		TVIDEO
#define	DADDR		((unsigned short *)(0x500000))	/* display base address/4 */
#define	YMOUSE		((short *)(0x400000))
#define	XMOUSE		((short *)(0x400002))
#define	WORDSHIFT	5
#define	WORDSIZE	32
#define	WORDMASK	(WORDSIZE-1)
#define	ONES		0xFFFFFFFF
#define	FIRSTBIT	((unsigned)0x80000000)
#define	LASTBIT		((unsigned)0x1)
#define	XMAX		800
#define	YMAX		1024

/*
 *	Graphics definitions
 */

#define POLY_F -32768		/* for polygon fill programs */

typedef int	Word;		/* words are now 32 bits	*/

typedef unsigned int	UWord;	/* words are now 32 bits	*/

typedef struct Point {
	short	x;
	short	y;
} Point;

typedef struct Rectangle {
	Point origin;
	Point corner;
} Rectangle;

typedef struct Bitmap {
	Word	*base;		/* pointer to start of data */
	unsigned width;		/* width in 32 bit words of total data area */
	Rectangle rect;		/* rectangle in data area, local coords */
	char	*_null;		/* unused, always zero */
} Bitmap;

typedef struct Menu{
	char	**item;			/* string array, ending with 0 */
	short	prevhit;		/* retained from previous call */
	short	prevtop;		/* also retained from previous call */
	char	*(*generator)();	/* used if item == 0 */
} Menu;

typedef struct Texture {
	Word bits[32];
} Texture;

typedef struct Texture16 {
	short	bits[16];
} Texture16;

struct Mouse {
	Point	xy, jxy;
	short	buttons;
};

extern Rectangle Jrect;

#ifndef MPX
#define button(i)	(mouse.buttons & (8 >> i))
#define button1()	(mouse.buttons&4)
#define button2()	(mouse.buttons&2)
#define button3()	(mouse.buttons&1)
#define button12()	(mouse.buttons&6)
#define button13()	(mouse.buttons&5)
#define button23()	(mouse.buttons&3)
#define button123()	(mouse.buttons&7)
#endif
#define bttn(i)		(mouse.buttons & (8 >> i))
#define bttn1()		(mouse.buttons&4)
#define bttn2()		(mouse.buttons&2)
#define bttn3()		(mouse.buttons&1)
#define bttn12()	(mouse.buttons&6)
#define bttn13()	(mouse.buttons&5)
#define bttn23()	(mouse.buttons&3)
#define bttn123()	(mouse.buttons&7)

#define	muldiv(a,b,c)	((long)((a)*((long)b)/(c)))

/*
 * Function Codes
 */
typedef int	Code;
#define	F_STORE	((Code) 0)	/* target = source */
#define	F_OR	((Code) 1)	/* target |= source */
#define	F_CLR	((Code) 2)	/* target &= ~source */
#define	F_XOR	((Code) 3)	/* target ^= source */

#define	NULL	((char *)0)
#define	KBD	1
#define	SEND	2
#define	MOUSE	4
#define	RCV	8
#define	CPU	16
#define ALARM	32
#define PSEND	64

#ifdef	MPX		/* compiling for layers environment */
#include <mpx.h>
extern Rectangle Drect;

#else	/* it's a stand-alone program */
		/* two ways to circumvent the ROM vectors: */
		/*	1) Have ROMTERM defined (true when compiling in term) 
		/*	2) Have MPXTERM defined and MPXPATCH undefined
		/*	   (true only when compiling ROM pieces of layersys)
		 */
#ifndef ROMTERM
#ifdef	MPXTERM
#ifdef	MPXPATCH
#include <sa.h>	/* ~ROMTERM & MPXTERM & MPXPATCH */
#endif
#else
#include <sa.h>	/* ~ROMTERM */
#endif
#endif
#ifndef Pt
/* declare some common functions...if the vector table is being used, these
   should not be done since the cast mechanism handles the return values
   properly
 */
Point add(), sub(), mul(), div(), jstring(), string(), Pt();
Rectangle rsubp(), raddp(), inset(), Rpt(), Rect();
Word *addr();
char	*alloc(), *gcalloc();
Bitmap *balloc();
Texture16 *cursswitch();
extern struct Mouse mouse;
extern Word topbits[], botbits[];	/* now full 32 bit words */
#endif
#ifndef	MPXTERM
#undef sw
#undef request
#undef sleep
#undef transform
#undef rtransform
#define	sw(n)
#define	request(n)
#define sleep(n) nap(n)
#define	exit()	reboot()
#define	transform(p)	(p)
#define	rtransform(r)	(r)
#define	Drect	Jrect
#endif
#endif
#ifdef	lint
#undef	sleep
#undef	request
#undef	sw
#undef	unblock
#endif
