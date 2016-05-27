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

#undef jstrwidth
jstrwidth(s)
#define jstrwidth Sjstrwidth
	char *s;
{
	return(strwidth(&defont,s));
}

#undef strwidth
strwidth(f,s)
#define strwidth Sstrwidth
	Font *f;
	register char *s;
{
	register wid=0;
	register Fontchar *info;
	info = f->info;
	for(; *s; s++)
		wid+=info[*s].width;
	return(wid);
}
