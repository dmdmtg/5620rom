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
 * pcb file for romterm
*/

#define PCBSZ	20
#define PCBCORE 8  /* PCBSZ - (space for register saves) */
#define STKSZ	100
#define ISTKSZ	64
#define RESETSZ 256
#define EXCEP_TAB_SIZE	32
#define EXCEP_PSW  0x281e100
#define DEMON_ADDR 0x40050459	/* address of DEMON bpt, trap handlers */

extern int _start,key_int,host_int,out_int,msvid_int;
extern int excep_norm(), excep_stack(), excep_proc(); /* exception handlers */
extern int excep_int(), piohint();
extern int setup();		/* setup code */

/* RAM versions of PCBs --- these are the ones actually used */

int rreset_pcb[PCBSZ];		/* ram area for copy of RESET EXCEPTION pcb */
int rproc_pcb[PCBSZ];		/* ram area for copy of PROCESS EXCEPTION pcb */
int rstack_pcb[PCBSZ];		/* ram area for copy of STACK EXCEPTION pcb */
int rhost_pcb[PCBSZ];		/* host line input interrupt	 IPL 15 */
int rout_pcb[PCBSZ];		/* host line output interrupt	 IPL 15 */
int rpioh_pcb[PCBSZ];		/* high priority PIO		 IPL 15 */
int rkey_pcb[PCBSZ];		/* keyboard interrupt		 IPL 14 */
int rmsvid_pcb[PCBSZ];		/* 60 Hz clock interrupt	 IPL 14 */
int rpiol_pcb[PCBSZ];		/* low priority PIO		 IPL 14 */
int rint_pcb[PCBSZ];		/* stray interrupt		 IPL ?? */

int rexcep_tab[EXCEP_TAB_SIZE];	/* Normal Exception gate table */

int rmain_pcb[PCBSZ];	/* Downloaded Program PCB -- MUST BE LAST! */

extern int	rsetup_pcb[];	/* Setup PCB declared in setup.c */

/* Stacks (in RAM, of course) */

int istack[ISTKSZ];
int ststk[RESETSZ];
int procstk[STKSZ];
int stackstk[STKSZ];
int hoststk[STKSZ];
int outstk[STKSZ];
int piohstk[STKSZ];
int keystk[STKSZ];
int msvidstk[STKSZ];
int piolstk[STKSZ];
int strstk[STKSZ];
extern int setstk[];	/* declared in setup.c */	


/* ROM area for initialized copies of pcbs
 *
 * NOTE: In same order as RAM copies above (see bootrom.s) 
 * bootrom.s copies these pcb's into ram - the format of each
 * pcb entry is:
 *
 *		BLOCK SIZE
 *		BLOCK ADDRESS
 *		PCB
 *
 *	   ending with a zero block size - it was done this way to save
 *	   space in rom - the reason for init_pcb and reset_pcb
 *	   (both below) is that reset_pcb is used out of rom during
 *	   reset and then copied into ram before calling vitty - the
 *	   two words in init_pcb put reset_pcb in the same format as
 *	   above (block size, block address, pcb) to make this copy
 *	   from rom to ram easy.
 */

int init_pcb[] = {
	PCBSZ,
	(int)rreset_pcb
};

int reset_pcb[] = {			/* reset_pcb is the process control */
	0x281e180,			/* block for boot, load, and vitty  */
	(int)&_start,
	(int)ststk,
	0x281e100,
	(int)&_start,
	(int)ststk,
	(int)ststk,
	(int)&ststk[RESETSZ-1],
	0,0,0,0, 0,0,0,0, 0,0,0,0,


					/* proc_pcb is the process control */
					/* block for process exceptions    */
	PCBCORE,
	(int)rproc_pcb,
	0x281e180,
	(int)excep_proc,
	(int)procstk,
	0x281e100,
	(int)excep_proc,
	(int)procstk,
	(int)procstk,
	(int)&procstk[STKSZ-1],

					/* stack_pcb is the process control */
					/* block for stack exceptions       */
	PCBCORE,
	(int)rstack_pcb,
	0x281e180,
	(int)excep_stack,
	(int)stackstk,
	0x281e100,
	(int)excep_stack,
	(int)stackstk,
	(int)stackstk,
	(int)&stackstk[STKSZ-1],
					/* host_pcb */
	PCBCORE,
	(int)rhost_pcb,
	0x281e180,	/* IPL = 15 */
	(int)&host_int,
	(int)hoststk,
	0x281e100,	/* IPL = 15 */
	(int)&host_int,
	(int)hoststk,
	(int)hoststk,
	(int)&hoststk[STKSZ-1],

					/* out_pcb */
	PCBCORE,
	(int)rout_pcb,
	0x281e180,	/* IPL = 15 */
	(int)&out_int,
	(int)outstk,
	0x281e100,	/* IPL = 15 */
	(int)&out_int,
	(int)outstk,
	(int)outstk,
	(int)&outstk[STKSZ-1],

					/* pioh_pcb */
	PCBCORE,
	(int)rpioh_pcb,
	0x281e180,
	(int)piohint,			/* I/O board interrupt routine */
	(int)piohstk,			/* (in libsys/sccops.c) */
	0x281e100,
	(int)piohint,
	(int)piohstk,
	(int)piohstk,
	(int)&piohstk[STKSZ-1],


					/* key_pcb */
	PCBCORE,
	(int)rkey_pcb,
	0x281c180,	/* IPL = 14 */
	(int)&key_int,
	(int)keystk,
	0x281c100,	/* IPL = 14 */
	(int)&key_int,
	(int)keystk,
	(int)keystk,
	(int)&keystk[STKSZ-1],

					/* msvid_pcb */
	PCBCORE,
	(int)rmsvid_pcb,
	0x281c180,	/* IPL = 14 */
	(int)&msvid_int,
	(int)msvidstk,
	0x281c100,	/* IPL = 14 */
	(int)&msvid_int,
	(int)msvidstk,
	(int)msvidstk,
	(int)&msvidstk[STKSZ-1],

					/* piol_pcb */
					/* same as for pioh (dummy pcb) -
					   this is for level 14 pio */
	PCBCORE,
	(int)rpiol_pcb,
	0x281e180,
	(int)excep_int,
	(int)piolstk,
	0x281e100,
	(int)excep_int,
	(int)piolstk,
	(int)piolstk,
	(int)&piolstk[STKSZ-1],

					/* int_pcb */
					/* PCB for stray interrupts */
	PCBCORE,
	(int)rint_pcb,
	0x281e180,
	(int)excep_int,
	(int)strstk,
	0x281e100,
	(int)excep_int,
	(int)strstk,
	(int)strstk,
	(int)&strstk[STKSZ-1],

	PCBCORE,		/* Setup */
	(int)rsetup_pcb,
	0x281e180,		/* executes with interrupts enabled */
	(int)setup,
	(int)setstk,
	0x281e180,
	(int)setup,
	(int)setstk,
	(int)setstk,
	(int)&setstk[399],	/* uses a bigger stack */

					/* excep_tab */
	EXCEP_TAB_SIZE,
	(int)rexcep_tab,
	EXCEP_PSW, (int)excep_norm,
	EXCEP_PSW, (int)excep_norm, /* change to DEMON_ADDR for demon breakpoints */
	EXCEP_PSW, (int)excep_norm,
	EXCEP_PSW, (int)excep_norm,
	EXCEP_PSW, (int)excep_norm,
	EXCEP_PSW, (int)excep_norm,
	EXCEP_PSW, (int)excep_norm,
	EXCEP_PSW, (int)excep_norm,
	EXCEP_PSW, (int)excep_norm,
	EXCEP_PSW, (int)excep_norm,
	EXCEP_PSW, (int)excep_norm,
	EXCEP_PSW, (int)excep_norm,
	EXCEP_PSW, (int)excep_norm,
	EXCEP_PSW, (int)excep_norm,
	EXCEP_PSW, (int)excep_norm, /* change to DEMON_ADDR for demon breakpoints */
	EXCEP_PSW, (int)excep_norm,

					/* main_pcb is the process control  */
					/* block for downloaded programs    */
					/* not used in romterm */
	PCBCORE,
	(int)rmain_pcb,
	0x281e180,
	0,
	0,
	0x281e100,
	0,
	0,
	0,		
	0,

	0 		/* NULL block size says end copy */
};
