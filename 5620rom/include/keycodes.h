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
/* keycodes from the keyboard queue.
 *
 * Most of the keys on the main keypad (e.g. 'A', space, '1', etc.) are put
 * on the keyboard queue in ASCII.  The special keys are handled as described
 * below.  Controls get put on the queue as the ASCII code for the control
 * char (e.g. CTRL/A is 0x01).  Ditto for tab, escape, backspace and delete.
 * Return is put on the queue as 0x13, regardless of the setting of the 
 * Return Key definition in setup.
 *
 * Break, disconnect, and the pf keys are put on as
 * 0x80 through 0x89 respectively.
 *
 * The arrow keys are put on as the ANSI 3.64 escape sequence.
 *
 * SETUP and Shift/SETUP (reset) are handled directly by the keyboard interrupt
 * routine - these only appear on the queue when setup itself is running.
 * Num Lock and Caps Lock are
 * also handled internally, as obviously, are Shift and Control.  
 */

#ifndef	KEYCODE_H
#define	KEYCODE_H	1
#define BRKKEY		0x80	/* Break */
#define	DISCONKEY	0x81	/* DISCON (Shift/Break) */
#define	SETUP		0xae	/* Setup */
#define	RESET		0x8e	/* Shift/Setup */

#define FUNC1KEY	0x82	/* f1 */
#define FUNC2KEY	0x83	/* f2 */
#define FUNC3KEY	0x84	/* f3 */
#define FUNC4KEY	0x85	/* f4 */
#define FUNC5KEY	0x86	/* f5 */
#define FUNC6KEY	0x87	/* f6 */
#define FUNC7KEY	0x88	/* f7 */
#define FUNC8KEY	0x89	/* f8 */

#define	UP	"\033[A"	/* Up Arrow */
#define	DOWN	"\033[B"	/* Down Arrow */
#define LEFT	"\033[D"	/* Left Arrow */
#define RIGHT	"\033[C"	/* Right Arrow */
#define	HOME	"\033[H"	/* Left Up Arrow, Home */
#define	HOMEDOWN	"\033[70;1H"	/* Left Down Arrow, Home Down */

#endif
