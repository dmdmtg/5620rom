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
 * mainstk for layersys - this is separate from the stacks declared in
 * lpcbs.c bacause it can float around layersys bss, but the other stacks
 * (in particular istack) have to be bound to the same address as in romterm.
*/

#define MSTK    150

int mainstk[MSTK];		/* stack for main() in control.c */

int pad_pad[42];		/* PAD END OF LAYERSYS - RETHINK THIS AFTER
				   VERSION 8;7;3 */
