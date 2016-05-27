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
#define	A_NEWLAYER	1	/* make a new layer */
#define	A_CURRENT	2	/* make layer process current */
#define	A_DELETE	3	/* delete a layer */
#define	A_TOP		4	/* bring a layer to top */
#define	A_BOTTOM	5	/* put a layer on bottom */
#define	A_MOVE		6	/* move a layer */
#define	A_RESHAPE	7	/* reshape a layer */
#define	A_NEW		8	/* make a new layer and send C_NEW to layers */
#define	A_EXIT		9	/* exit layers program */

/* Leave some room for future mouse operations to be implemented 	*/

#define	A_ROMVERSION	20	/* tell us your rom version, e.g. 8;7;5 */
#define	A_STACKSIZE	21	/* supply terminal with an alternate
				   amount of stack space to be used with
				   the current download			*/


struct agentrect{
	short	command;	/* either newlayer, reshape or current */
	short	chan;
	Rectangle r;		/* rectangle description */
};
