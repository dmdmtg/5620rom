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
 * Definitions for globals involved with lines
 */
#ifdef LINE_C

short Jdmajor;		/* Delta for major direction */
short Jdminor;		/* Delta for minor direction */
short Jxmajor;	/* flag: is x the major direction? */
short Jslopeneg;	/* flag: is slope of line negative? */
Point Jsetline();
#undef PtCurrent 
Point PtCurrent;
#define PtCurrent SPtCurrent

#else

extern short Jdmajor;		/* Delta for major direction */
extern short Jdminor;		/* Delta for minor direction */
extern short Jxmajor;	/* flag: is x the major direction? */
extern short Jslopeneg;	/* flag: is slope of line negative? */
extern Point Jsetline();

#endif

