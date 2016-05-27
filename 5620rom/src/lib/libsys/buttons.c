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
#include <setup.h>

#undef binit
binit(){
#define binit Sbinit
	/* init	the buttons */
	DUART->ipc_acr = 0x8f;	/* allow change	of state interrupt */
	/* enable mouse	and 60 hz. interrupts */
	DUART->is_imr =	0x80;
}
#undef auto4
auto4(){
#define auto4 Sauto4
	register unsigned char c;

	DUART->a_cmnd = NO_OP;	/* toggle r/w lead before doing a read */
	c = ~(DUART->ip_opcr);
	c = ((c & 0x8) >> 1) | (c & 0x3);
	if( VALMOUSE )	/* left-handed mouse  --  flip button states */
		mouse.buttons = ((c&4)>>2) | (c&2) | ((c&1)<<2);
	 else mouse.buttons = c;
}
