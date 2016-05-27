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

/* Linkage with ROM layersys (these are in section .linkage) */
/* This file is built with romterm.  A slightly modified version
 * which actually sets the values is built with layersys, and linked
 * to the same address.
 */
int	ls_bss;		/* pointer to end of layersys bss */
int	lsys_start;	/* pointer to startup routine for ROM layersys */
int	*pvtab;		/* pointer to RAM layersys vector table */
int	lenvtab;	/* number of entries in vector table */
int	*rvtab[1];	/* pointer to ROM copy of layersys vector table */
