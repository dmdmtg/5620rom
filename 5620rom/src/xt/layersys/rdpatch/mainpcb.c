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
 * pcbs and stacks for layersys program. Note that the layersys ifile
 * (map) in this directory places pcbs BEFORE stacks. Also note that this
 * is a "phantom" file to satisfy the compiler. pcbs and stacks are actually
 * dynamically created from romterm pcbs in pl.s to save having to keep
 * the following in rom.
*/

#define PCBSZ	20
#define STKSZ	100
#define ISTKSZ	64
#define MSTK	150  /* must agree with MSTK in mainstk.c */
#define RESETSZ 256
#define EXCEP_TAB_SIZE	32
#define EXCEP_PSW  0x281e100

extern int excep_int();

extern  int _start,
	key_int,
	host_int,
	out_int,
	msvid_int;

extern int mainstk[];


/* Stacks (in .bss)  [Must agree with ordering in romterm] */

int main_pcb[PCBSZ] = {			/* main_pcb is the process control  */
	0x281e180,			/* block for downloaded programs    */
	(int)&_start,
	(int)mainstk,
	0x281e100,
	(int)&_start,
	(int)mainstk,
	(int)mainstk,
	(int)&mainstk[MSTK-1],
	0,0,0,0, 0,0,0,0, 0,0,0,0,
};
