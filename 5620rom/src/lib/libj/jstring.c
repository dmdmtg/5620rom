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
#include <font.h>

/*extern Font defont;*/
/*extern Point PtCurrent;*/
/*extern Point string();*/
#undef jstring
Point jstring(s)
#define jstring Sjstring
	register char *s;
{
	return PtCurrent=string(&defont,s,&display,PtCurrent,F_XOR);
}
