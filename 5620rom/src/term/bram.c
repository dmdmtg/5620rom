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
/* @(#) bram.c 1.1.1.7 (Changed 10/19/84 12:01:11) */
#include <setup.h>
#include <dmd.h>
#include <font.h>
#define ESC	'\033'
#define	BRAM_SIZE	2048	/* size of BRAM in bytes */

checkbram()	/* return 1 if bram has valid checksum.  If not, clear */
		/* the battery backed ram, putting in a valid checksum */
{
	register long i = 0;
	register int j,level;
	register struct bram_mem *p = BRAM->opts.opt_array;

	level=spl7();		/* critical section */
	for (; p<&BRAM->opts.opt_array[BRAM_SIZE-2]; p++)
		i += p->byte;
	j =  (BRAM->opts.opt_array[BRAM_SIZE - 2].byte << 8) + 
		BRAM->opts.opt_array[BRAM_SIZE - 1].byte;
	i %= (1 << 16);		/* truncate to checksum size */
	if ( i == j)
	  {
		splx(level);
		return(1);
	     }
	initbram();		/* reinitialize */
	splx(level);
	return(0);
}

initbram()
{
	register struct bram_mem *p = BRAM->opts.opt_array;

	for (; p<&BRAM->opts.opt_array[BRAM_SIZE]; p++)
		p->byte = (unsigned char) 0;	/* first clear it */

	/* Assume no I/O card and 256K bytes of RAM, so... */
	VALPPRNT = 3;			/* printer is port 'B' */
	VALPAUX1 = 3;			/* AUX1 is port 'B' */
	VALPAUX2 = 3;			/* AUX2 is port 'B' */
	setbram();	/* reset checksum */
	logports();	/* so RAM pointers correspond with BRAM */
}

setbram()	/* reset checksum in bram... done when bram updated */
{
	register long i = 0;
	register int j, level;
	register struct bram_mem *p = BRAM->opts.opt_array;

	level=spl7();
	for (; p<&BRAM->opts.opt_array[BRAM_SIZE-2]; p++)
		i += p->byte;
	BRAM->opts.opt_array[BRAM_SIZE - 1].byte = i & 0xff;
	BRAM->opts.opt_array[BRAM_SIZE - 2].byte = (i >> 8) & 0xff;
	splx(level);
}


#undef bramgetstr
int
bramgetstr(keynum,str,maxlen)	/* get pfkey "keynum" from bram into "str" */
#define bramgetstr Sbramgetstr
int keynum;
register char *str;
int maxlen;  /* max length allowed for string */
{
	register int i = 0;
        if (keynum >= 0x82 && keynum <= 0x89)
        {
          keynum = (keynum & 0xf) -2;
	  while ((*str++ = BRAM->pfkeys[keynum][i].byte) && (++i < (maxlen-1) ));
          if(*str != '\0')
	  	*str = '\0';        /* guarantee null termination if max */
        }
        /* length is reached or invalid key number */
        return i;	/* return the number of chars read, excluding NULL */
}

#undef bramputstr
bramputstr(keynum,str)	/* store string "str" into pfkey "keynum" in bram */
#define bramputstr Sbramputstr
int keynum;
char *str;
{
	register int i = 0;

	while (BRAM->pfkeys[keynum][i++].byte = (unsigned char) (*str++));
}



/* For quick set BRAM (I don't want to disable interrupts too long!), let's
   assume the checksum is correct.
   Mostly use to reset VALCAPS and VALNUM.
*/
#undef qsetbram
qsetbram(VAL,val)
#define qsetbram Sqsetbram
char *VAL, val;
{
    register int i;
    register int level;

	level = spl7();		/* critical section */
	i =  (BRAM->opts.opt_array[BRAM_SIZE - 2].byte << 8) + 
		BRAM->opts.opt_array[BRAM_SIZE - 1].byte;	/* read checksum */
	if (*VAL >= val)
		i -= (*VAL - val);
	else
		i += (val - *VAL);
	*VAL = val;		/* set the value , then the checksum: */
	BRAM->opts.opt_array[BRAM_SIZE - 1].byte = i & 0xff;
	BRAM->opts.opt_array[BRAM_SIZE - 2].byte = (i >> 8) & 0xff;
	splx(level);
}

