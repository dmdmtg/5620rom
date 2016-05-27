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
extern int reset_pcb[20], rkey_pcb[20],rhost_pcb[20];
extern int rintr_pcb[20],rout_pcb[20],rmsvid_pcb[20];
extern int rpioh_pcb[20],rpiol_pcb[20];
extern int rproc_pcb[20], rstack_pcb[20], rint_pcb[20], rexcep_tab[20];
extern int excep_norm();

int first_level[32] = { (int)rexcep_tab, 0x70480, 0x70480, 0x70480,
			0x70480, 0x70480, 0x70480, 0x70480,
			0x70480, 0x70480, 0x70480, 0x70480,
			0x70480, 0x70480, 0x70480, 0x70480,
			0x70480, 0x70480, 0x70480, 0x70480,
			0x70480, 0x70480, 0x70480, 0x70480,
			0x70480, 0x70480, 0x70480, 0x70480,
			0x70480, 0x70480, 0x70480, 0x70480
			};
int reset = (int)reset_pcb;
int proc = (int)rproc_pcb;
int stack = (int)rstack_pcb;


int intr[64] = {
	(int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, 
	(int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, 
	(int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, 
	(int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, 
	(int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, 
	(int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, 
	(int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, 
	(int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, (int)rhost_pcb, 
	(int)rout_pcb, (int)rout_pcb, (int)rout_pcb, (int)rout_pcb, 
	(int)rout_pcb, (int)rout_pcb, (int)rout_pcb, (int)rout_pcb, 
	(int)rout_pcb, (int)rout_pcb, (int)rout_pcb, (int)rout_pcb, 
	(int)rout_pcb, (int)rout_pcb, (int)rout_pcb, (int)rout_pcb, 
	(int)rpioh_pcb, (int)rpioh_pcb, (int)rpioh_pcb, (int)rpioh_pcb, 
	(int)rpioh_pcb, (int)rpioh_pcb, (int)rpioh_pcb, (int)rpioh_pcb, 
	(int)rkey_pcb, (int)rkey_pcb, (int)rkey_pcb, (int)rkey_pcb, 
	(int)rmsvid_pcb, (int)rmsvid_pcb, (int)rpiol_pcb, (int)rint_pcb ,
};
