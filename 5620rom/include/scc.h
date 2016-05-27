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
/* scc.h	Register definitions for the Zilog 8530 Serial
 *		Communications Controller and associated I/O
 *		expansion hardware on version 2.0 DMDs.
 */


struct scc {
	int : 24;
	unsigned char bctrl;	/* Channel B control/status */
	int : 24;
	unsigned char bdata;	/* Channel B data (RR8/WR8) */
	int : 24;
	unsigned char actrl;	/* Channel A control/status */
	int : 24;
	unsigned char adata;	/* Channel A data (RR8/WR8) */
};


struct ioxregs {
	int : 24;
	unsigned char intvec;	/* Clear interrupt and read vector command */
	int : 32;  int : 32;  int : 32; int : 24;
	unsigned char select;	/* Clock and driver select register */
	int : 32;  int : 32;  int : 32; int : 24;
	unsigned char mstat;	/* Modem status leads: TM, DSR, RI */
};


#define SCC	((struct scc *)0x300000)
#define IOX	((struct ioxregs *)0x300010)


/*  SCC control write register bits: */

/*  WR0: */
#define CLR_IUS		(7<<3)
#define CLR_ERR		(6<<3)
#define CLR_TXINT	(5<<3)
#define CLR_EXTERN	(2<<3)

/*  WR1: */
#define RXINTALL	(2<<3)
#define PAR_SPCL	0x04
#define EN_TXINT	0x02
#define EN_EXTERN	0x01

/*  WR3: */
#define RX_7BITS	(1<<6)
#define RX_8BITS	(3<<6)
#define RCVR_ON		0x01

/*  WR4: */
#define CLK_X16		(1<<6)
#define CLK_X32		(2<<6)
#define ONE_STOP	(1<<2)
#ifdef EVEN_PAR
#undef EVEN_PAR
#endif
#define EVEN_PAR	0x02
#define PAR_ON		0x01
#define PAR_OFF		0x00

/*  WR5: */
#define DTR_ON		0x80
#define TX_7BITS	(1<<5)
#define TX_8BITS	(3<<5)
#define SND_BRK		0x10
#define TXMTR_ON	0x08
#define RTS_ON		0x02

/*  WR9: */
#define HARD_RESET	(3<<6)
#define A_RESET		(2<<6)
#define B_RESET		(1<<6)
#define MINT_EN		0x08
#define MOD_VECTOR	0x01

/*  WR11: */
#define RCLK_BRG	(2<<5)
#define TCLK_BRG	(2<<3)
#define BRG_OUT		(2<<0)

/*  WR14: */
#define LOOP_BK		0x10
#define PCLK2BRG	0x02
#define EN_BRG		0x01

/*  WR15: */
#define EN_BRKINT	0x80
#define EN_DCDINT	0x08
#define EN_BRG0INT	0x02


/*  SCC status read register bits: */

/*  RR0: */
#define RCVD_BRK	0x80
#define CTS_ON		0x20
#define DCD_ON		0x08
#define TX_RDY		0x04
#define BRG_ZRO		0x02
#define RX_CHAR		0x01

/*  RR1: */
#define FRAM_ERR	0x40
#define OVERUN_ERR	0x20
#define PRTY_ERR	0x10
#define TX_EMPTY	0x01


/*  I/O expansion clock/driver select register bits: */
#define CHB_232		0x80
#define CHB_CLKOUT	0x40
#define CHB_4MHz	(3<<4)
#define CHA_232		0x08
#define CHA_CLKOUT	0x04
#define CHA_4MHz	(3<<0)


/*  Extra modem status bits: */
#define TM_B		040
#define DSR_B		020
#define RI_B		010
#define TM_A		004
#define DSR_A		002
#define RI_A		001
