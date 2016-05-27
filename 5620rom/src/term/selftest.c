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
#include <duart.h>
#include <setup.h>
#include <selftest.h>
#include <dmd.h>
#include <font.h>
#include <scc.h>



/******************************************************************/
/*     This is the second pass at the diagnostic test programs for */
/*  Teletype's 5620 Dot Mapped Display Terminal                   */
/******************************************************************/

int burn1;
extern Font defont;
extern Bitmap display;
extern char build_date[];		/* timestamp for firmware build */

test32(which)
int which;
{
	int k, z;
	short *d;
        register int *start;	/* in R8 */
        register int *end = (int *)0x719000; 	/* in R7 */

    
	asm("	ORW2	&0x281e000,%psw");
	asm("	NOP	");
	d = (short *)0x500000;
	*d = 0;
	burn1 = 0;

	asm("	MOVAW	rkey_pcb+0x18,%r1");
	asm("	MOVAW	rkey_pcb+0x0C,%r2");
	asm("	MOVW	%r7,%r0");	/* MOVW &end,%r0 */
	asm("savepcb:	");
	asm("	MOVW	0(%r2),0(%r0)");
	asm("	ADDW2	&0x04,%r2");
	asm("	ADDW2	&0x04,%r0");
	asm("	CMPW	%r2,%r1");
	asm("	BNEB	savepcb");
	
AGAIN:
	*((char *)0x20003F) = 0x01;	/* turn off DTR */

        start = (int *)0x700000;
  
        while (start < end)
              *start++ = 0;             /* blank screen */
   
	if(which != 0) draw_ver();

        drawchar("ROM TEST",Pt(10,10));
	rom();

	lit_draw("SHORTRAM TEST");
	shortram();

	if(which == 0)
	{
		lit_draw("WAITING FOR KEYBOARD STATUS");
		if(t_kbd() == 3) {
				/* external jumper present */
			lit_draw("RAM TEST");
			ram();
			lit_draw("NONVOLATILE MEMORY TEST");
			bram();
			lit_draw("I/O TEST");
			duart(LCL_LOOP);
			flicker(1,50000);
			lit_draw("EXTERNAL DUART TEST");
			cppcbs();
			duart(NORM_OP);
			flicker(3,25000);
			if(IOCARD)
			{
				lit_draw("EXTRA IO BOARD TEST");
				scctest();
			}
			else
			{
				lit_draw("PIO and MOUSE TEST");
				pio_mse();
			}

     			flicker(4,12500);
			while(burn1)
				asm("	NOP	");
			goto AGAIN;
		}
	}
	else
	{
		lit_draw("RAM TEST");
		ram();
		drawchar("MEMORY SIZE: ", Pt(323,790));
		if (VALMAXADDR)
			drawchar("1M", Pt(440,790));
		else
			drawchar("256K", Pt(440,790));
		lit_draw("NONVOLATILE MEMORY TEST");
		bram();
		lit_draw("I/O TEST");
		duart(LCL_LOOP);
		track();
		type();
		reppcb();
	}
}

draw_ver()
{

        extern int ls_bss;          /*1st available RAM address*/
        extern char *ver_str;       /*terminal ID string*/
	char *p, *ptmp;
        char tmp[100];
                                   
	drawchar("KEYBOARD REQUESTED SELF-TEST", Pt(269,650));

	drawchar("COPYRIGHT 1983 TELETYPE CORPORATION", Pt(238,710));

        drawchar("TERMINAL VERSION NUMBER: ", Pt(260,730));
	for(p = &ver_str[3], ptmp = tmp ; *p != 'c' ; ++p, ++ptmp)
		*ptmp = *p;
	*ptmp = '\0';
        drawchar(tmp, Pt(485,730));
	drawchar(build_date,Pt(269,750));

        drawchar("1ST AVAILABLE MEMORY ADDRESS: 0x", Pt(224,770));
        l_into_xc(ls_bss, tmp);  
        drawchar(tmp, Pt(512,770));
}
 
reppcb()
{
	asm("	MOVAW	rkey_pcb+0x18,%r1");
	asm("	MOVAW	rkey_pcb+0x0C,%r2");
	asm("	MOVW	%r7,%r0");	/* MOVW	&end,%r0 */
	asm("setpcb:	");
	asm("	MOVW	0(%r0),0(%r2)");
	asm("	ADDW2	&0x04,%r2");
	asm("	ADDW2	&0x04,%r0");
	asm("	CMPW	%r2,%r1");
	asm("	BNEB	setpcb");
}

rom()
{
	register char p, *cnt, flag, test;
	register unsigned short chksum;
	register char *max_addr;
	max_addr = (char *)0x007FFF;
	flag = 0;
RECHK:
	p = (char)(chksum = (unsigned short)(cnt = (char *)0));
RRECHK:
	for( ; cnt < (max_addr - 1); cnt++)
	{
		chksum += ( *cnt & 0x00FF);
		chksum = ((chksum & 0x8000) != 0) ? (chksum << 1 | 1) : chksum << 1;
	}
	chksum = ~(chksum);
	if( *(max_addr -1) != (chksum & 0x00FF))
		p = 1;
	if( *max_addr  != (chksum >> 8))
		p = 1;
	if((p ) && (flag < 4))
	{
		max_addr = (char *)(0x00FFFF + (flag * 0x010000));
		chksum = ~(chksum);
		p = 0;
		flag++;
		goto RRECHK;	/* there might be a 2nd bank of rom */
	}
	if((p ) && (flag == 4))
	{
		max_addr = (char *)0x007FFF;
		flag = 0;
		burn1 = 1;
		goto RECHK;
	}
}

int ramflag;
int maxtemp;
ram()
{
	short *d;
	/* make sure C compiler knows about register use */
	register int srctch8;   /* r8 */
	register int srctch7;   /* r7 */
	register int srctch6;   /* r6 */
	register int srctch5;   /* r5 */
	register int srctch4;   /* r4 */
	register int srctch3;   /* r3 */

	asm("REXIT:     ");
	asm("   MOVW    &0x700000,%r5");                /* start of RAM memory */
	
	asm("   MOVW    &0x720000,%r6");        /* start 2nd half of RAM memory*/

	asm("   MOVW    &0x740000,%r7");                /* end of RAM memory */

	asm("   MOVW    &0x00,%r4");    /* start with a "good" status */
	d = (short *)0x500000;
		/* initialize maxtemp to 64K chip memory size */
	ramflag = *d = maxtemp = 0;

	/* Write background pattern into high memory        */
	/* r0 = starting address                            */
	/* r1 = final address                               */
	/* r2 = data pattern                                */
	/* r3 = complement data pattern*/
	/* r6 = starting address of 2nd half of RAM memory  */
	/* r7 = address of the end of RAM memory            */

	asm("HIPAT:     ");
	asm("   MOVW    %r6,%r0");
	asm("   MOVW    %r7,%r1");
	asm("   MOVW    &0x00,%r2");
	asm("BACKLP1:   ");
	asm("   MOVW    %r2,0(%r0)");
	asm("   CMPW    %r2,0(%r0)");
	asm("   BEB     BBACK1");
	asm("	MOVW	&0x01,ramflag");
	burn1 = 1;
	asm("   JMP     BACKLP1");
	asm("BBACK1:    ");
	asm("   ADDW2   &0x00010001,%r2");
	asm("   ADDW2   &0x04,%r0");
	asm("   CMPW    %r0,%r1");
	asm("   BNEB    BACKLP1");
	asm("   MOVW    %r6,%r0");
	asm("   MOVW    %r7,%r1");
	asm("   MOVW    &0x00,%r2");
	asm("HIPAT2:    ");
	asm("   CMPW    %r2,0(%r0)");
	asm("   BEB     BBACK2");       
	asm("	MOVW	&0x01,ramflag");
	burn1 = 1;
	asm("   JMP     HIPAT");
	asm("BBACK2:    ");
	asm("   ADDW2   &0x00010001,%r2");
	asm("   ADDW2   &0x04,%r0");
	asm("   CMPW    %r0,%r1");
	asm("   BNEB    HIPAT2");

	/* Relocate low memory to high memory           */
	
	/* r0 = starting address of 1st half of memory  */
	/* r1 = starting address of 2nd half of memory  */
	/* r5 = starting address of memory              */
	/* r6 = address of 2nd half of memory           */

	asm("SLOWPAT:    ");
	asm("   MOVW    %r5,%r0");
	asm("   MOVW    %r6,%r1");
	asm("LMLOOP:    ");
	asm("   MOVW    0(%r0),0(%r1)");        /* copy low to high memory */
	asm("   ADDW2   &0x4,%r0");
	asm("   ADDW2   &0x4,%r1");
	asm("   CMPW    %r0,%r6");              /* test for last address   */
	asm("   BNEB    LMLOOP");

	/* write background pattern in low memory       */

	/* r0 = starting address                        */
	/* r1 = final address                           */
	/* r2 = data pattern                            */
	/* r5 = starting address of RAM memory          */
	/* r6 = starting address, 2nd half of RAM memory*/

	*d = 0x8000;

	asm("LOWPAT:     ");
	asm("   MOVW    %r5,%r0");
	asm("   MOVW    %r6,%r1");
	asm("   MOVW    &0x00,%r2");
	asm("BACKLP2:   ");
	asm("   MOVW    %r2,0(%r0)");
	asm("   CMPW    %r2,0(%r0)");
	asm("   BEB     BBACK3");
	asm("	MOVW	&0x01,ramflag");
	burn1 = 1;
	asm("   JMP     BACKLP2");
	asm("BBACK3:    ");
	asm("   ADDW2   &0x00010001,%r2");
	asm("   ADDW2   &0x04,%r0");
	asm("   CMPW    %r0,%r1");
	asm("   BNEB    BACKLP2");
	asm("   MOVW    %r5,%r0");
	asm("   MOVW    %r6,%r1");
	asm("   MOVW    &0x00,%r2");
	asm("LOWPAT2:   ");
	asm("   CMPW    %r2,0(%r0)");
	asm("   BEB     BBACK4");       
	asm("	MOVW	&0x01,ramflag");
	burn1 = 1;
	asm("   JMP     LOWPAT");
	asm("BBACK4:    ");
	asm("   ADDW2   &0x00010001,%r2");
	asm("   ADDW2   &0x04,%r0");
	asm("   CMPW    %r0,%r1");
	asm("   BNEB    LOWPAT2");


	/* relocate high memory to low memory */

	/* r0 = starting address of low memory          */
	/* r1 = starting address of high                */
	/* r5 = starting address of RAM memory          */
	/* r6 = starting address, 2nd half of RAM memory*/
	/* r7 = end of RAM                              */

	asm("HMTOLM:    ");
	asm("   MOVW    %r5,%r0");
	asm("   MOVW    %r6,%r1");
	asm("HMLOOP:    ");
	asm("   CMPW    %r1,&0x740000");
	asm("   BEH     EXIT");
	asm("   MOVW    0(%r1),0(%r0)");
	asm("   ADDW2   &0x4,%r1");
	asm("   ADDW2   &0x4,%r0");
	asm("   BRB     HMLOOP");

	/* If an error was detected in the protected portion  */
	/* of the memory attempt to restore the RAM memory to */
	/* its original state despite the error.      */

	asm("EXIT:      ");
	asm("   MOVW    &0x00,%r2");
	asm("   CMPW    %r2,%r4");
	asm("   BNEH    REXIT");

/**********************************************************************/
/* Test to see if there is 256K words of memory, and if so, test if all*/
/**********************************************************************/

	asm("   MOVW    &0x700000,%r6");
	asm("   MOVW    &0x740000,%r7");
	asm("   MOVW    &0x0000,0(%r6)");
	asm("   MOVW    &0x0055,0(%r7)");
	asm("   CMPW    0(%r6),0(%r7)");
	asm("   BEH     LEAVE");
	asm("   MOVW    &0x1,maxtemp");
	asm("   MOVW    &0x740000,%r6");
	asm("   MOVW    &0x800000,%r7");

	asm("DIPAT:     ");
	asm("   MOVW    %r6,%r0");
	asm("   MOVW    %r7,%r1");
	asm("   MOVW    &0x00,%r2");
	asm("DACKLP1:   ");
	asm("   MOVW    %r2,0(%r0)");
	asm("   CMPW    %r2,0(%r0)");
	asm("   BEB     DBACK1");
	asm("	MOVW	&0x01,ramflag");
	burn1 = 1;
	asm("   JMP     DACKLP1");
	asm("DBACK1:    ");
	asm("   CMPW    %r2,&0xFFFFFFFF");
	asm("   BEB     DUP1");
	asm("   ADDW2   &0x00010001,%r2");
	asm("   JMP     DUP2");
	asm("DUP1:      ");
	asm("   MOVW    &0x00,%r2");
	asm("DUP2:      ");
	asm("   ADDW2   &0x04,%r0");
	asm("   CMPW    %r0,%r1");
	asm("   BNEB    DACKLP1");
	asm("   MOVW    %r6,%r0");
	asm("   MOVW    %r7,%r1");
	asm("   MOVW    &0x00,%r2");
	asm("DIPAT2:    ");
	asm("   CMPW    %r2,0(%r0)");
	asm("   BEB     DBACK2");       
	asm("	MOVW	&0x01,ramflag");
	burn1 = 1;
	asm("   JMP     DIPAT");
	asm("DBACK2:    ");
	asm("   CMPW    %r2,&0xFFFFFFFF");
	asm("   BEB     DUP3");
	asm("   ADDW2   &0x00010001,%r2");
	asm("   JMP     DUP4");
	asm("DUP3:      ");
	asm("   MOVW    &0x00,%r2");
	asm("DUP4:      ");
	asm("   ADDW2   &0x04,%r0");
	asm("   CMPW    %r0,%r1");
	asm("   BNEB    DIPAT2");

	asm("LEAVE:     ");
	checkbram();
	VALMAXADDR=maxtemp;
	setbram();
	*d = 0;
}

bram()
{
        register char *TEMPSTRT, *BRAMSTRT, *p, test;
        register unsigned long i;

        TEMPSTRT = (char *)0x720000;
        BRAMSTRT = (char *)0x600002;
	test = 0;

        for(i = 0; i < 2048L; i++)
        {
                *TEMPSTRT = *BRAMSTRT;
                TEMPSTRT++;
                BRAMSTRT += 4;
        }
BRAMLAB:
        for(i = 0x600002; i <0x602000; (i+=4))
        {
                p = (char *)i;
                *p = (i & 0x0000FF);
        }
        for(i = 0x600002; i < 0x602000; (i+=4))
        {
                p = (char *)i;
                if((i & 0x0000FF) != *p)
		{
			test = 1;
			burn1 = 1;
                        goto BRAMLAB;
		}
                *p = ~(i & 0x0000FF);
        }
        for(i = 0x600002; i < 0x602000; (i+=4))
        {
                p = (char *)i;
                if( (char)(~(i & 0x0000FF)) != *p )
		{
			test = 1;
			burn1 = 1;
                        goto BRAMLAB;
		}
        }
        TEMPSTRT = (char *)0x720000;
        BRAMSTRT = (char *)0x600002;

        for(i = 0; i < 2048L; i++)
        {
                *BRAMSTRT = *TEMPSTRT;
                TEMPSTRT++;
                BRAMSTRT += 4;
        }
}
char board, board1, board2;
char *CRX, *MR1_2X, *C_SRX, *T_RHRX, *OPCR_I;

duart(MODE)
char MODE;
{
	int hold, counter, i, j;
	char data, rdata, set, rset, rd, test, x;
	unsigned char rates[7];
	char *IM_SR, *SETP_CMD, *RSTP_CMD, *IP_ACR;
	/* initialize the 2681 duart */
	IP_ACR   = ((char *)0x200013);
	IM_SR    = ((char *)0x200017);
	OPCR_I   = ((char *)0x200037);
	SETP_CMD = ((char *)0x20003B);
	RSTP_CMD = ((char *)0x20003F);
	rates[0] = BD110BPS;
	rates[1] = BD300BPS;
	rates[2] = BD1200BPS;
	rates[3] = BD2400BPS;
	rates[4] = BD4800BPS;
	rates[5] = BD9600BPS;
	rates[6] = BD19200BPS;
	test = j = 0;

/*      This test can only be implemented if the output port signals are
 *      hard wired to the input port signals,(or at least temporarily clipped
 *      together
 */
/* Test #0 */

	/*  The first time thru, when error occurrs it sets the errorflg */
	/*  need to do 10 more times but this time ignores the error */



	while(test|| j<2 ) {
		if(j>=2) test = j = 0;
		if(j++ == 0){  /* test on channel A will be done */
			MR1_2X = ((char *)0x200003);
			C_SRX =  ((char *)0x200007);
			CRX =    ((char *)0x20000B);
			T_RHRX = ((char *)0x20000F);
		}
		else {
			MR1_2X = ((char *)0x200023);
			C_SRX =  ((char *)0x200027);
			CRX =    ((char *)0x20002B);
			T_RHRX = ((char *)0x20002F);
		}
                *CRX	= RESET_REC | DIS_RX;
		*CRX	= RESET_TRANS | DIS_TX;
		*CRX	= RESET_MR | RESET_ERR | RESET_BCH;
		
		*MR1_2X = 0xA3;  /* RTS,FFULL,char,even par,8 char */
		*MR1_2X = 0xA7;  /* lcl-loop mode,RTS,no CTS,1 stop */
		*IP_ACR = 0x00;  /* BRG 0,disable all IP interrupts */

		*OPCR_I = 0x00;  /* set all o/ports as general output ports */
		*RSTP_CMD = 1;	/* turn off DTR */
		*IM_SR  = 0x00;  /* mask all interrupts */

/* Test #1 */   /* test the mode registers */
		/* start with channel A */
		data = 0x01;
		set  = 0xFE;
		while(data != 0)
		{
		    *CRX = 0x10;  /* reset MR pointer */
		    *MR1_2X = set;
		    *MR1_2X = (data & 0x3F);
		    /*now, read back each of the registers */
		    *CRX = 0x10;
		    rset = *MR1_2X;
		    rdata = *MR1_2X;
		    if(rset != set || rdata != (data & 0x3F))
			goto ERROR;
		    data = data << 1;
		    set = (set << 1) + 0x01;
		}

 /* Test #2 */  /* now, test the channel A recievers and transmitters */       
                *CRX	= RESET_TRANS | DIS_TX;
		*CRX    = RESET_REC | DIS_RX;
		*CRX	= RESET_MR ;	
		*MR1_2X = CHAR_ERR | PAR_ENB | EVEN_PAR | CBITS8;
		*MR1_2X = MODE | TWOSTOPB;
		*C_SRX  = BD19200BPS;
		*IP_ACR = 0x80;
	        *CRX	= ENB_TX | ENB_RX;
        	/* the test begins after a short delay */
		/* TEST - test data receive bit in the status register */
		/* TEST - check character validity */
		/* TEST - was the receive flag cleared ? */
		/* transmit and wait for receiver flag */
		for(data=0x55,i=0; i<2; i++,data<<1) {
			*T_RHRX = data;
			if(tready(0, RCV_RDY)) goto ERROR;
			*CRX = 0x00;
                	rdata = *T_RHRX;
			if(rdata != data) goto ERROR;
			if(i==0){
				*CRX = 0x00;
                        	if(rdata =(*C_SRX & RCV_RDY) != 0)
					goto ERROR;
			}
		}

/* Test #3 */   /* walk a one through at baud rates 110, 300, */
		/* 1200, 2400, 4800, 9600, 19200 for channel A. */
		for(counter = 0; counter < 7 ; counter++)
		{
			if(walk_x(rates[counter],CBITS8,TWOSTOPB,PAR_ENB,EVEN_PAR,MODE))
				goto ERROR;
		}


		/* walk a one through at baud rates 110, 300, */
		/* TEST - test different data lengths, channel A */
		if(walk_x(BD19200BPS,CBITS7,TWOSTOPB,PAR_ENB,EVEN_PAR,MODE))
			goto ERROR;
		if(walk_x(BD19200BPS,CBITS6,TWOSTOPB,PAR_ENB,EVEN_PAR,MODE))
			goto ERROR;
		if(walk_x(BD19200BPS,CBITS5,TWOSTOPB,PAR_ENB,EVEN_PAR,MODE))
			goto ERROR;


		/* TEST - test transmission with differnet parities */
		/* channel A */
/*
		if(walk_x(BD19200BPS,CBITS8,TWOSTOPB,PAR_ENB,EVEN_PAR,MODE))
			goto ERROR;
This test was run up above
*/
		if(walk_x(BD19200BPS,CBITS8,TWOSTOPB,NO_PAR,EVEN_PAR,MODE))
			goto ERROR;

		/* TEST - the transmission at 1 and 1.5 stop bits */
		/* try channel A */
		if(walk_x(BD19200BPS,CBITS8,ONESTOPB,PAR_ENB,ODD_PAR,MODE))
			goto ERROR;
		if(walk_x(BD19200BPS,CBITS8,ONENHALF,PAR_ENB,ODD_PAR,MODE))
			goto ERROR;

/* Test #4 */   /* check the trans. empty and rec. ready bits in the */
		/* status register for correct operation. */
		/* channel A */
                *CRX    = RESET_REC | DIS_RX;
		*CRX    = RESET_TRANS | DIS_TX;
		*CRX    = RESET_MR ;	
		*MR1_2X = CHAR_ERR | PAR_ENB | EVEN_PAR | CBITS8;
		*MR1_2X = MODE | TWOSTOPB;
		*C_SRX  = BD1200BPS;
		*IP_ACR = 0x00;
		*CRX    = ENB_TX | ENB_RX;
		/* TEST - start transmit - TxEMT/TxRDY = 0 ? */

		if(tready(0x55, XMT_RDY)) goto ERROR;

		*T_RHRX = 0xAA;
		*CRX = 0x00;
		rdata = (*C_SRX & (XMT_EMT | XMT_RDY));
		if(rdata != 0 )
			goto ERROR;

		/* TEST - TxEMT/TDRDY = 1 ? */
		for(i=0; i<2; i++){
			if(tready(0, RCV_RDY)) goto ERROR;
			rdata   = *T_RHRX;
		}
		*CRX = 0x00;
		rdata   = (*C_SRX & (XMT_EMT | XMT_RDY));
		if(rdata != (XMT_EMT | XMT_RDY))
			goto ERROR;

/* Test #5 */   /* Test rcvd break and parity error SR bits */
		/*channel A */
		*CRX    = RESET_REC | DIS_RX;
		*CRX    = RESET_TRANS | DIS_TX;
		*CRX	= RESET_MR;
		*MR1_2X = CHAR_ERR | PAR_ENB | ODD_PAR | CBITS8;
		*MR1_2X = MODE | TWOSTOPB;
		*C_SRX  = BD1200BPS;
		*IP_ACR = 0x00;
		*CRX	= ENB_RX | ENB_TX;
                *CRX    = STRT_BRK;
		t_delayer(10000);
		*CRX    = STOP_BRK;
                *CRX = 0x00;
	
		rdata = (*C_SRX & RCVD_BRK);
		if(rdata != RCVD_BRK)
			goto ERROR;
                *CRX = 0x00;
		rdata = *T_RHRX;
                *CRX = 0x00;
		rdata = (*C_SRX & RCVD_BRK);
		if(rdata != 0)
			goto ERROR;


		/* Test the OVERRUN and FIFOFULL SR bits */

		for(i=0; i<4; i++) {
			if(tready(data, XMT_RDY)) goto ERROR;
                	*CRX = 0x00;
			while((*C_SRX & RCV_RDY) == 0)
			{*CRX = 0x00;}
			data = (data << 1) + i;
			if(i==2) {
			*CRX = 0x00;
			rdata = rd = (*C_SRX & FIFOFULL);
			if(rd != FIFOFULL)
				goto ERROR;

			}
		}
		*T_RHRX = 0x55;
		hold = 0;
		*CRX = 0x00;
	        while((rdata = (rd = (*C_SRX & OVER_ERR))) == 0)
		{
			*CRX = 0x00;
 		}
		*CRX = RESET_ERR;
		*CRX = 0x00;
		rdata = rd = (*C_SRX & (RCVD_BRK | FRM_ERR | PAR_ERR | OVER_ERR));
		if(rd)
			goto ERROR;
		rdata = 0;

		if((MODE == NORM_OP) && ( j == 1))
		{
                        *CRX = RESET_REC | DIS_RX;
			*CRX = RESET_TRANS | DIS_TX;	
			*CRX = RESET_MR;
			
			*MR1_2X = CHAR_ERR | PAR_ENB | EVEN_PAR | CBITS8;
			*MR1_2X = MODE | ONESTOPB;
			*C_SRX = BD9600BPS;
			*IP_ACR = 0x80;
			board1 = board2 = 0x00;
			*IM_SR = 0x00;
			*OPCR_I = 0x53;
			*RSTP_CMD = 0x20;
                        *CRX = ENB_RX | ENB_TX;
			t_delayer(2500);
			asm("	ANDW2	&0xFFFE1FFF,%psw");
			asm("	NOP	");
                 	t_delayer(2500);
			while(!board2)
			{
				*CRX = 0x00;
                                while(!board1)
				{
                         	*CRX = 0x00;
				}
				*T_RHRX = 0xAA;
			}
			*OPCR_I = 0x03;         /* set all output ports as general ports */
			*SETP_CMD = 0x01;
			*CRX = 0x00;
			if(((x = *OPCR_I) & 0x70) != 0x00)
				test++;
			*RSTP_CMD = 0x01;
			*CRX = 0x00;
			if(((x = *OPCR_I) & 0x70) != 0x70)
				test++;
		}

		*CRX     = RESET_REC;
		*CRX     = RESET_TRANS;
		*CRX    = RESET_MR;

		while(((*C_SRX) & RCV_RDY) == RCV_RDY)
                {*CRX = 0x00;
			rd = *T_RHRX;
                 	*CRX = 0x00;
                }
		if(test == 0) continue;
ERROR:          test++;
		burn1 = 1;
		}    /* end of while loop */
}


tready(data,mode)
char data;
int mode;
{
	register char rdata;
	extern char * C_SRX;


	*CRX = 0x00;
	if(mode == XMT_RDY) *T_RHRX = data;
        *CRX = 0x00;
	while(((rdata = *C_SRX) & mode) == 0) {
		t_delayer(2500);
	}
	return(0);
}
		


stout_int()
{
	extern char board1;
	char *RSTP_CMD;
	RSTP_CMD = (char *)0x20003F;
	board1 = 0x01;
	*OPCR_I = 0x13;
	*RSTP_CMD = 0x40;
	t_delayer(1250);
	asm("	RETPS	");
}

sthost_int()
{
	extern char board2;
	char *T_RHRA, x;
	T_RHRA = (char *)0x20000F;
	x = *T_RHRA;
	board2 = 0x01;
	asm("	RETPS	");
}

t_delayer(count)
int count;
{
	while(count-- > 0);
}


walk_x(baud,cbits,sbits,par_enb,parity,MODE)
unsigned char  baud, cbits, sbits, par_enb, parity, MODE;

{
	register unsigned char data, sh_count,rd;
	
	/* initialize the UART */
        *CRX    = RESET_REC | DIS_RX;
	*CRX    = RESET_TRANS | DIS_TX;
	*CRX	= RESET_MR;
	*MR1_2X = CHAR_ERR | par_enb | parity | cbits;
	*MR1_2X = MODE | sbits;
	*C_SRX  = baud;

	if(baud == BD19200BPS)
		*OPCR_I = 0x80;
	else
		*OPCR_I = 0x00;
 
        *CRX	= ENB_RX | ENB_TX;

/*
	switch(cbits)
	{
		case CBITS5:
			chkcnt = 5;
			break;
		case CBITS6:
			chkcnt = 6;
			break;
		case CBITS7:
			chkcnt = 7;
			break;
		case CBITS8:
			chkcnt = 8;
			break;
	}
*/
	
	data = 0x01;
	for(sh_count = 0; sh_count < cbits + 5; sh_count++)
	{
		if(walk_x1(XMT_RDY)) return(1);
		*T_RHRX = data;         /* write data */
		if(walk_x1(RCV_RDY)) return(1);
                *CRX = 0x00;
		rd = *T_RHRX;           /* read data */
		if(rd != data)
			return(1);
		else
			data = data << 1;
	}
	return(0);
}
walk_x1(value)
int value;
{
	register i;

	i = 0;
        *CRX = 0x00;
	while((*C_SRX & value) == 0)
	{
		*CRX = 0x00;
	}
	return(0);
}


t_kbd()
{
	register  x;
	char *CRB, *IP_ACR, *IM_SR;
	char *MR1_2B, *C_SRB, *T_RHRB, *RSTP_CMD, *OPCR_I;
 
	/* initialize the 2681 duart */
        IP_ACR	 = ((char *)0x200013);
        IM_SR	 = ((char *)0x200017);
        MR1_2B	 = ((char *)0x200023);
        C_SRB	 = ((char *)0x200027);
        CRB	 = ((char *)0x20002B);
        T_RHRB	 = ((char *)0x20002F);
        RSTP_CMD = ((char *)0x20003F);
	OPCR_I	= ((char *)0x200037);

        *CRB = RESET_TRANS | DIS_TX;
	*CRB = RESET_REC | DIS_RX;
        *CRB = RESET_ERR;
	*CRB = RESET_MR; 
	*MR1_2B = CHAR_ERR | PAR_ENB | EVEN_PAR | CBITS8;
	*MR1_2B = 0X00 | ONESTOPB;
	*C_SRB = BD4800BPS;
	*IP_ACR = 0X80;
	*OPCR_I = 0X00;
	*RSTP_CMD = 0XFF;
        *IM_SR = 0X00;
	*CRB = ENB_RX | ENB_TX;

	*T_RHRB = 0x02;
	*CRB = 0x00;
TRY:	while((*C_SRB & 0x21) == 0)
	        *CRB = 0x00;	
	if(( *C_SRB & 0x21) == 0x21)
	{
		*CRB = RESET_ERR;
		*CRB = 0x00;
		x = *T_RHRB;
		*CRB = 0x00;
		if(( x & 0x01 ) == 0 )
			return(1);		/* GOOD KBD */
		else
			return(2);		/* BAD KBD */
	}
        *CRB = 0x00;
	if(( *C_SRB & 0x01 ) == 0x01)
	{
                *CRB = 0x00;
		x = *T_RHRB;
		if( x == 0x02 )
			return(3);		/* loopback connector present */
		else
			return(4);		/* it must be garbage */
	}
}
int oldx, oldy, xm, ym, intcnt;
track()
{
	short * dis_strt = (short *)(0x500000);
        int always();

	intcnt = 0;
	cppcbs();
	*dis_strt = 0;
	lit_draw("MOUSE TEST - hit RETURN to exit");
	drawchar("buttons depressed = ",Pt(10,28));
	kbdinit();
        mousetest(always,"+");
}

cppcbs()
{
	asm("	.text	");

	/* zero out ram where pcbs will reside */
	asm("   MOVAW   rreset_pcb,%r0");
	asm("   MOVAW   rmain_pcb,%r1");
	asm("bobclr:    ");
	asm("   CLRW    0(%r0)");
	asm("   addw2   &4,%r0");
	asm("   cmpw    %r0,%r1");
	asm("   BNEB    bobclr");

	asm("	MOVAW	init_pcb,%r0"); /*# r0 = beginning pcb blocks */
	asm("vectors:	");
	asm("	MOVW	0(%r0),%r2");
	asm("	BEB	setpcbp	");
	asm("	ADDW2	&0x04,%r0");
	asm("	MOVW	0(%r0),%r1");
	asm("	addw2	&4,%r0");
	asm("	MOVBLW");
	asm("   BRB    vectors");
	asm("setpcbp:");
	asm("	MOVAW	rreset_pcb+12,%pcbp");
	asm("	MOVAW	stmsvid_int, rmsvid_pcb+4");
	asm("	MOVAW	stmsvid_int, rmsvid_pcb+0x10");
	asm("	MOVAW	stkey_int,rkey_pcb+4");
	asm("	MOVAW	stkey_int,rkey_pcb+0x10");
	asm("	MOVAW	stout_int,rout_pcb+4");
	asm("	MOVAW	stout_int,rout_pcb+0x10");
	asm("	MOVAW	sthost_int,rhost_pcb+4");
	asm("	MOVAW	sthost_int,rhost_pcb+0x10");
	asm("	MOVAW	XIO_int,rpioh_pcb+4");
	asm("	MOVAW	XIO_int,rpioh_pcb+0x10");
}


mousetest(wait, c)
int (*wait)();
char *c;
{ 
	extern int oldx, oldy, xm, ym;
	extern char board;
        register char *IP_ACR, *C_SRB, *CRB, *IM_SR, *MR1_2B;
	register char * oldc, x, *RSTP_CMD;
	int i;

	IP_ACR = (char *)0x200013;
	CRB = (char *)0x20002B;
	MR1_2B = (char *)0x200023;
	C_SRB = (char *)0x200027;
	IM_SR = (char *)0x200017;
	RSTP_CMD = (char *)0x20003F;
	*CRB = RESET_TRANS | DIS_TX;
	*CRB = RESET_REC | DIS_RX;
        *CRB = RESET_ERR;
	*CRB = RESET_MR;
	*MR1_2B = CHAR_ERR | PAR_ENB | EVEN_PAR | CBITS8;
	*MR1_2B = 0x00 | ONESTOPB;
 	*C_SRB = BD4800BPS; 
	*IM_SR = 0x00;
	*OPCR_I = 0x20; /* set all general registers except rvideo */
	*RSTP_CMD = 0xdc;
	*IP_ACR = 0x8f;
        *CRB = ENB_RX;
	board = 0x00;
	oldc = c;
	oldx = 100;
	oldy = 100;
	xm = 100;
	ym = 100;
	drawchar("+",Pt(oldx,oldy));
	*IM_SR = 0x80;
	*CRX = 0x00;
	x = *IP_ACR;
	asm("	ANDW2	&0xFFFE1FFF,%psw");
	asm("	NOP	");
TRACK:
	while(board != 0xe7)
        {
        }
	asm("	MOVW	&0x281e100,%psw");
	asm("	NOP	");
	drawchar("+",Pt(oldx,oldy));
}

always()
{
        register j = 2000;
        do;
        while (j--)
                ;       /* kill time */
        return;
}

drawchar(c, p)
char *c;
Point p;
{
        string(&defont, c, &display, p, F_XOR);
}



stmsvid_int()
{
	extern int oldx, oldy, xm, ym, intcnt;
	char x, *IP_ACR;
	IP_ACR = (char *)0x200013;
	x = *IP_ACR;
	if((x & 0x80) == 0x80)
	{
		drawchar("LEFT",Pt(210,28));
	}
	if((x & 0x20) == 0x20)
	{
		drawchar("MIDDLE",Pt(270,28));
	}
	if((x & 0x10) == 0x10)
	{
		drawchar("RIGHT",Pt(340,28));
	}
	if((x & 0x44) == 0x40)
	{
		if(intcnt < 2)
		{
			intcnt += 1;
			goto MB;
		}
		intcnt = 0;
		drawchar("+",Pt(oldx,oldy));
BM:
                xm = *XMOUSE & 1023;
                ym = *YMOUSE & 1023;
                if (xm < 0 || ym < 0)   /* bad data */
			goto BM;
                if (xm > XMAX - 10)
                        xm = XMAX - 10;
                if (ym > YMAX )
                        ym = YMAX ;
                /* xm = XMAX - xm; */
                ym = YMAX - ym;
                oldx = xm; 
                oldy = ym;
		drawchar("+",Pt(oldx,oldy));
	}
MB:
	asm("	RETPS	");
}

stkey_int()
{
	extern char board;
	board = KBDC;
	asm("	RETPS	");
}
type()
{
	register x, y;
	char *T_RHRB,*MR1_2B, *IP_ACR, *CRB, *C_SRB, c, buf[2];

	buf[1] = 0;
	T_RHRB = (char *)0x20002F;
	C_SRB = (char *)0x200027;
	CRB = (char *)0x20002B;
	MR1_2B = (char *)0x200023;
	IP_ACR = (char *)0x200013;

	*CRB = RESET_TRANS |DIS_TX;
	*CRB = RESET_REC | DIS_RX;
	*CRB = RESET_ERR;
	*CRB = RESET_MR;
	*MR1_2B = CHAR_ERR | PAR_ENB | EVEN_PAR | CBITS8;
	*MR1_2B = 0x00 | ONESTOPB;
	*C_SRB = BD4800BPS;
	*IP_ACR = 0x80;
        *CRB = ENB_RX;
	lit_draw("KEYBOARD TEST - hit RETURN to exit");
	drawchar("NON-ASCII CHARACTERS WILL BE UNDERLINED",Pt(10,28));
STATBYTE:
        *CRB = 0x00;
	while((*C_SRB & 0x21) == 0)
		*CRB = 0x00;
        *CRB = 0x00;
	if((*C_SRB & 0x20) == 0x20)
	{
		*CRB = RESET_ERR;
		*CRB = 0x00;
                c = *T_RHRB;
		goto STATBYTE;
	}
TY:
	x = 10;
	y = 50;

  while((c = *T_RHRB) != 0xe7) {
	*CRB = 0x00;
        if(c & 0x80) {
		drawchar("_", Pt(x,y));
		c &= 0x7f;
	}
	buf[0] = c;
	drawchar(buf, Pt(x,y));
	*CRB = 0x00;

TTY:    while((*C_SRB & 0x21) == 0)
	        *CRB = 0x00;	
	*CRB = 0x00;
        if((*C_SRB & 0x20) == 0x20)
	{
		*CRB = RESET_ERR;
                *CRB = 0x00;
		c = *T_RHRB;
		goto TTY;
	}
	x += 10;
	if( x >= 800) {
		x = 10;
		y += 15;
	}
	if(y >= 360) {
		blank();
		goto TY;
	}

  }
	
}


blank()
{
	register int *start = (int *)0x700000;
	register int *end = (int *)0x709500;

	while(start < end)
		*start++ = 0;   /*clear top half of screen*/
}
pio_mse()
{
	register unsigned long old, new, *mouse;
	register char za, zb, look, *dc, i, test;
	unsigned long t;

	mouse = (unsigned long *)0x400000;
	test = 0;
PIO:
	old = ( *mouse & 0x0FFF0FFF );
	t = 0x300003;
	for( i = 0; i < 17; i++)
	{
		dc = (char *)t;
		look = *dc;
		za = t & 0x7c;
		za = (za >> 2);
		zb = look & 0x1f;
		if(za != zb)
		{
			t_shade();
			test = 1;
			burn1 = 1;
			goto PIO;
		}
		if((look & 0xE0) != 0xE0)
		{
			color();
			test = 1;
			burn1 = 1;
			goto PIO;
		}
		t += 4;
	}
	new = ((*mouse) & 0x0FFF0FFF);
	if( old == new )
	{
		test = 1;
		burn1 = 1;
		goto PIO;
	}
}

t_shade()
{
	register int *start = (int *)0x700000;
	register int *end = (int *)0x719000;

	while(start < end)
		*start++ = 0xAAAAAAAA;
}

color()
{
	register int *start = (int *)0x700000;
	register int *end = (int *)0x719000;

	while(start < end)
		*start++ = 0xFFFFFFFF;
}


flicker(pulsno,timedel)
int pulsno, timedel;
{
	int i;
	char *SETP_CMD, *RSTP_CMD;
	SETP_CMD = (char *)0x20003B;
	RSTP_CMD = (char *)0x20003F;
	*OPCR_I = 0x03;         /* set all output ports as general ports */
	for(i = 0; i < pulsno; i++)
	{
	*SETP_CMD = 0x01;
	t_delayer(timedel);
	*RSTP_CMD = 0x01;
	t_delayer(timedel);
	}
}
lit_draw(msg)
	char *msg;
{
	blank();
	drawchar(msg,Pt(10,10));
}
wt(){
	int i;
	i = 0;
}

shortram()
{
	register int srctch0; 	/* r0 */
	register int srctch1;	/* r1 */
	register int srctch2;	/* r2 */
	register int srctch3;	/* r3 */
	register int srctch4;	/* r4 */
	burn1 = 0;

/*****************************************************************************/
/*   check and set memory size as 256K or 1M                                  */
/*****************************************************************************/

	asm("	MOVW	&0x700000,%r1");
	asm("	MOVW	&0x740000,%r2");
	asm("	MOVW	&0x0000,0(%r1)");
	asm("	MOVW	&0x0055,0(%r2)");
	asm("	CMPW	0(%r1),0(%r2)");
	asm("	BEH	SRAM1");
	checkbram();
	VALMAXADDR = 1;
	setbram();

	asm("SRAM1:	");
	asm("	MOVW	&0x7FFFFFFF,%r0");
	asm("	MOVW	&0x00,%r2");
	asm("SRAM2:	");
	asm("	MULW3	%r2,&0x04,%r3");
	asm("	ADDW3	%r3,&0x71C000,%r4");
	asm("	MOVW	%r0,0(%r4)");
	asm("	MOVW	0(%r4),%r1");
	asm("	CMPW	%r0,%r1");
	asm("	BEB	SRAM3");
	burn1 = 1;
	asm("	BRB	SRAM1");
	asm("SRAM3:	");
	asm("	ROTW	&0x01,%r0,%r0");
	asm("	ADDW2	&0x01,%r2");
	asm("	CMPW	%r2,&0x100");
	asm("	BNEH	SRAM2");
}


/*JIMTOP*/

char XX, bd1, bd2, test;
unsigned char *ctrl, *sccdata;
/*ctrl:     takes the place of SCC->actrl or SCC->bctrl
 *sccdata:  takes the place of SCC->adata or SCC->bdata
 *XX:       sets channel to RS232 or RS422
 *bd1, bd2: test constants used in pio_int()
 */

scctest()
{
	int j;
	test = 0;
	creg();			/*test control register*/
	while(test)
	{
		test = 0;
		creg();
	}
	for(j = 0; j < 5; j++)
	{
		if(test)	/*check for error*/
		{
		test = 0;
		j--;		/*if error occured repeat same test*/
		}
	scc_main(j);		/*call SCC test*/
	}
	dcereg();		/*test DCE STATUS register*/
	while(test)
	{
		test = 0;
		dcereg();
	}
}

creg()
{
int i;
/*The following routine tests the functionallity of the CONTROL REGISTER*/
  	for(i=0;i<255;i++)
       	       {
		IOX->select = i;
		if (IOX->select != i) goto ERR1;   	/*check value written*/
		}
return;
ERR1: err1();
}

dcereg()
{
/*The following routine tests the functionallity of the DCE STATUS REG. 
 *This test assumes that appropriate loop-back connectors are in place.
 *LOOP-BACK CONNECTOR SPECIFICATIONS
 *Tie DTRA (J04-20) to RIA (J04-22), & DSRA (J04-6).  Tie DTRB (J05-20)
 *to RIB (J05-22) & DSRB (J05-6).
 *
 *Tie 422TRA (J03-30) to 422DMA (J03-29).  Tie /422TRA (J03-12) to /422ICA
 *(J03-15), /422DMA (J03-11) & /422TMA (J03-18).
 *
 *Tie 422TRB (J06-30) to 422DMB (J06-29).  Tie /422TRB (J06-12) to /422ICB
 *(J06-15), /422DMB (J06-11) & /422TMB (J06-18).
 *
 *Bits 6 & 7 of the DCE STATUS REG. are always low.
 *
 */
char i;
i = SCC->actrl;				/*dummy read to synchronize control reg*/
i = SCC->bctrl;
SCC->actrl = 14; SCC->actrl = 0;	/*Make sure DTR is enabled*/
SCC->bctrl = 14; SCC->bctrl = 0;
SCC->actrl = 5;  SCC->actrl = DTR_ON;	/*set DTR output pin low. */
SCC->bctrl = 5;  SCC->bctrl = DTR_ON;
IOX->select = 0x88;			/*select 232 mode */

if((IOX->mstat & 0xdb) != 0) goto ERR1;	/*Read and check status register */

IOX->select = 0;			/*select 422 mode */

if(IOX->mstat != 0) goto ERR1;		/*Read and check status register */

SCC->actrl = 5; SCC->actrl = 0; 	/*set DTR output pin high */
SCC->bctrl = 5; SCC->bctrl = 0;

if(IOX->mstat != 0x3f) goto ERR1;	/*Read and check status register */

IOX->select = 0x88;			/*select 232 mode */

if((IOX->mstat & 0xdb) != 0x1b) goto ERR1; /*Read and check status register */
return;
ERR1: err1();
}

err1()
{
	test++;
  	burn1 = 1;
}

scc_main(x)
int x;
{
	int counter, i, hold;
	char rdata, data;
	asm("	ORW2	&0x281e000,%psw");		/* disable CPU interrupts*/
	asm("	NOP	");
	asm("	TSTW	%r0");
	asm("	TSTW	%r0");

		switch(x)
		{
		case 0:	ctrl = (unsigned char *)0x30000B;	/*test with channel A and RS232*/
			sccdata = (unsigned char *)0x30000F;	/*selected*/
			XX = 0x99;
			break;
	
		case 1:	ctrl = (unsigned char *)0x300003;	/*test with channel B and RS232*/
			sccdata = (unsigned char *)0x300007;	/*selected*/
			XX = 0x99;
			break;

		case 2:	ctrl = (unsigned char *)0x30000B;	/*test with channel A and RS422*/
			sccdata = (unsigned char *)0x30000F;	/*selected*/
			XX = 0x11;
			break;

		case 3: ctrl = (unsigned char *)0x300003;	/*test with channel B and RS422*/
			sccdata = (unsigned char *)0x300007;	/*selected*/
			XX = 0x11;
			break;
		case 4: return;
		}

	/* Initialize SCC */

	*ctrl = 0;						/*1*/
	*ctrl = 9; *ctrl = HARD_RESET | MINT_EN | MOD_VECTOR;	/*2*/
	t_delayer(0);						/*3*/
	*ctrl = 15; *ctrl = 0;					/*4*/
	IOX->select = XX;                                       /*6*/
	*ctrl = 11; *ctrl = RCLK_BRG | TCLK_BRG;		/*7*/
	*ctrl = 1; *ctrl = 0x00;				/*8*/

	/*Notes on above initialization
	 *1) Synchronize register pointers
	 *2) Force hardware reset and enable master interrupt
	 *3) Delay to give SCC time to complete reset
	 *4) Disable all external status/interrupts
	 *5) Set PCLK as the BRG clock source
	 *6) Select RS232 interface for channel A and B
	 *7) Channel A receiver and transmitter use the BRG as their clock
	 *8) Disable Tx and Rx interrupts
	 */

	/* walk a one through at baud rates 300,1200,2400,4800,9600,19200 */

	if(walk_xio(0xFE,0x00,8,RX_8BITS,TX_8BITS,ONE_STOP,PAR_ON,EVEN_PAR)) /* 300 */
		goto ERR1;
 	if(walk_xio(0x3E,0x00,8,RX_8BITS,TX_8BITS,ONE_STOP,PAR_ON,EVEN_PAR)) /*1200 */
		goto ERR1;
	if(walk_xio(0x1E,0x00,8,RX_8BITS,TX_8BITS,ONE_STOP,PAR_ON,EVEN_PAR)) /*2400 */
		goto ERR1;
	if(walk_xio(0x0E,0x00,8,RX_8BITS,TX_8BITS,ONE_STOP,PAR_ON,EVEN_PAR)) /*4800 */
		goto ERR1;
  	if(walk_xio(0x06,0x00,8,RX_8BITS,TX_8BITS,ONE_STOP,PAR_ON,EVEN_PAR)) /*9600 */
		goto ERR1; 
	if(walk_xio(0x02,0x00,8,RX_8BITS,TX_8BITS,ONE_STOP,PAR_ON,EVEN_PAR)) /*19200*/
		goto ERR1;

	/* Test new character length and all the parity possibilitier*/


	if(walk_xio(0x06,0x00,7,RX_7BITS,TX_7BITS,ONE_STOP,PAR_ON,EVEN_PAR))
		goto ERR1;
	if(walk_xio(0x06,0x00,7,RX_7BITS,TX_7BITS,ONE_STOP,PAR_OFF,EVEN_PAR))
		goto ERR1;
	if(walk_xio(0x06,0x00,7,RX_7BITS,TX_7BITS,ONE_STOP,PAR_ON,ODD_PAR))
		goto ERR1;

	/* check the TX_EMPTY and RX_CHAR bits in the status registers */

	*ctrl = 0;						  /*0*/
	*ctrl = 4; *ctrl = ONE_STOP | PAR_ON | EVEN_PAR | CLK_X32;/*1*/
	*ctrl = 12; *ctrl = 0x06;                                 /*2*/
	*ctrl = 13; *ctrl = 0x00;                                 /*3*/
	IOX->select = XX;					  /*5*/
	*ctrl = 14; *ctrl = EN_BRG;
	*ctrl = 3; *ctrl = RX_8BITS;                              /*6*/
	*ctrl = 3; *ctrl = RX_8BITS | RCVR_ON;                    /*7*/
	*ctrl = 5; *ctrl = TX_8BITS;                              /*8*/
	*ctrl = 5; *ctrl = TX_8BITS | TXMTR_ON;                   /*9*/

	/*comments on above SCC init.
	 *line 0:  sync. pointer register
	 *line 1:  select one stop bit and enable even parity
	 *line 2:  lower byte of 9600 baud time constant
	 *line 3:  upper byte of 9600 baud time constant
	 *line 4:  enable the baud rate generator
	 *line 5:  select RS232 or RS 422
	 *line 6:  set received character length to 8 bits
	 *line 7:  enable receiver
	 *line 8:  set transmitted character lenth to 8 bits
	 *line 9:  enable transmitter
	 */

	if(treadyio(0x55, TX_EMPTY,1))			/*check TX_EMPTY stat. bit*/
	goto ERR1;
	if(treadyio(0, RX_CHAR, 2)) 			/*check RX_CHAR status bit*/
	goto ERR1;
	rdata = *sccdata;				/*read received char.*/

	/* test the received break status bit*/

	*ctrl = 5; *ctrl =  TX_8BITS | TXMTR_ON | SND_BRK;/*send break*/ 
	t_delayer(100000);				/*allow break to be rcvd.*/
	asm("	CLRB	*ctrl");
	rdata = ( *ctrl & RCVD_BRK );
	if(rdata != RCVD_BRK)
		goto ERR1;
	*ctrl = 5; *ctrl = TX_8BITS | TXMTR_ON;		/*turn off break*/
	t_delayer(100000);
	rdata = *sccdata;		/*read null char. generator by break*/

	rdata = (*ctrl & RCVD_BRK);	/*check to see if RCVD_BRK was reset*/
	if(rdata != 0)
		goto ERR1;


	/* check overrun error */

	data = 0x55;
	for( i=0; i<4; i++)
	{
		hold = 0;
		if(treadyio(data, TX_RDY, 3))		/*transmit four chars.*/
		goto ERR1;
		rdata = *ctrl;
		while((rdata & RX_CHAR) == 0)		/*wait for char to be rcvd*/
		{
		t_delayer(2500);
		if(hold++ > 0xFF)
		goto ERR1;
		rdata = *ctrl;
		}
	}

	if(treadyio(data, TX_RDY, 3))			/*transmit a fith char to*/
	goto ERR1;
	hold = 0;					/*cause overrun error*/
	rdata = *sccdata;        /*read first three char sent*/
	rdata = *sccdata;
	rdata = *sccdata;
	rdata = *sccdata;        /*read char that caused overrun, this should*/
				/*cause OVERUN_ERR to be set*/
	asm("	MOVB	&1,*ctrl");
	rdata = *ctrl;
	while((rdata & OVERUN_ERR) == 0)
	{
		t_delayer(2500);
		if(hold++ > 0xFF)
			goto ERR1;
		asm("	MOVB	&1,*ctrl");
		rdata = *ctrl;
	}
	*ctrl = CLR_ERR;         /* reset overrun error */
	asm("	MOVB	&0x01,*ctrl");
	rdata = (*ctrl & ( FRAM_ERR | OVERUN_ERR | PRTY_ERR));
	if(rdata)
		goto ERR1;

	pio_int();		/*test Tx and Rx interrupts*/
	return;
ERR1:
	err1();	
}

walk_xio(baud1,baud2,bc,rcbits,tcbits,sbits,par_enb,parity)
unsigned char baud1, baud2, rcbits, tcbits, sbits, par_enb, parity;
int bc;
/*baud1 is the lower byte of the BRG time constant
 *baud2 is the upper byte of the BRG time constant
 *bc is the number of bits per character
 *rcbits sets the number of received bits per character
 *tcbits sets the number of transmitted bits per character
 *sbits sets the number of stop bits
 *par_enb is used to enable or disble parity
 *parity enables odd or even parity
 */
{
	register unsigned char data, sh_count, rd;
	
	/* initialize the SCC */
	*ctrl =  4; *ctrl = sbits | par_enb | parity | CLK_X32;              /*1*/
	*ctrl = 12; *ctrl = baud1;                                 /*2*/
	*ctrl = 13; *ctrl = baud2;                                 /*3*/
	*ctrl = 14; *ctrl = EN_BRG;               	           /*4*/
	*ctrl =  3; *ctrl = rcbits;                                /*5*/
	*ctrl =  3; *ctrl = rcbits | RCVR_ON;                      /*6*/
	*ctrl =  5; *ctrl = tcbits;                                /*7*/
	*ctrl =  5; *ctrl = tcbits | TXMTR_ON;                     /*8*/
  
	data = 0x01;
	for( sh_count = 0; sh_count < bc; sh_count++)
	{
		if(walk_io1(TX_RDY)) return(1);
		/*check to see if Tx buffer is empty*/
 

		*sccdata = data;		/* transmit data */


		if(walk_io1(RX_CHAR)) return(1);
		/*check to see if a character was received*/


		rd = *sccdata;			/*read received character*/
	   if(rcbits == RX_8BITS)
		{
		if(rd != data)			/*check data integrity*/
			return(1);
		else
			data = data << 1;	/*shift '1' over to next bit*/
		}
 	   else
	   {
		rd = (rd & 0x7f);
		if(rd != data)			/*check data integrity*/
			return(1);
		else
			data = data << 1;	/*shift '1' over to next bit*/
	   }
	}
	*ctrl =  5; *ctrl = tcbits;		/*disable Tx*/
	*ctrl =  3; *ctrl = rcbits;		/*disable Rx*/
	*ctrl =  14; *ctrl = 0;			/*disable BRG*/
	
	return(0);
}

walk_io1(val)
int val;
/*val will be either TX_RDY or RX_CHAR*/
{
	register i, x;
	register char rdata;
	
	i = 0;
	asm("	CLRB	*ctrl");
	rdata = *ctrl;
	while((rdata & val) == 0)
	{
		t_delayer(2500);
		asm("	CLRB	*ctrl");
		rdata = *ctrl;
		if(i++ > 0xFF) 
			return(1);
	}
	return(0);
}
treadyio(data,mode,y)
char data, mode;
int y;
/*data is char. to be transmitted if necessary
 *mode is the particular status bit to be checked
 *y selects the case for the switch
 */
{
	register char rdata, x;
	register int hold;
	
	hold = 0;
	switch(y)
	{
	case 1: *sccdata = data;
		*ctrl = 1;
		x = 1; break;
	case 2:
		*ctrl = 0;
		x = 0; break;
	case 3:
		*sccdata = data;
		*ctrl = 0;
		x = 0; break;
	}
	rdata = *ctrl;
	while((rdata & mode) == 0)
	{
		t_delayer(2500);
		if (hold++ > 0xff) return (1);
		asm("	MOVB	%r7,*ctrl");
		rdata = *ctrl;
	}
	return(0);
}
pio_int()
{
char data;
int hold1, hold2;

	*ctrl =  2; *ctrl = 0;                            /* 0*/
	*ctrl =  4; *ctrl = ONE_STOP | CLK_X32;           /* 1*/
	*ctrl = 12; *ctrl = 0x06;                         /* 2*/
	*ctrl = 13; *ctrl = 0x00;                         /* 3*/
	IOX->select = XX;				  /* 5*/
	*ctrl = 14; *ctrl = EN_BRG;      	          /* 4*/
	*ctrl = 1; *ctrl = EN_TXINT;                      /* 6*/
	*ctrl = 3; *ctrl = RX_8BITS;                      /* 7*/
	*ctrl = 5; *ctrl = TX_8BITS;                      /* 8*/
	*ctrl = 3; *ctrl = RX_8BITS | RCVR_ON;            /* 9*/
	*ctrl = 5; *ctrl = TX_8BITS | TXMTR_ON;           /*10*/

	/*comments on above SCC init
	 *line 0:  set unmodified interrupt vector to zero
	 *line 1:  select one stop bit
	 *line 2:  lower byte of the 9600 baud time constant
	 *line 3:  upper byte of the 9600 baud time constant
	 *line 4:  enable the BRG
	 *line 5:  select RS232 or RS422 interface
	 *line 6:  enable transmit interrupts
	 *line 7:  set received char. length to 8 bits
	 *line 8:  set transmitted char. length to 8 bits
	 *line 9:  enable the receiver
	 *line 10: enable the transmitter
	 */

	asm("	ANDW2	&0xFFFE1FFF,%psw");	/*enable CPU interrupts*/
	asm("	NOP	");
	asm("	TSTW	%r0");
	bd1 = bd2 = 0;				/*set test flags to wait for ints*/
	hold1 = hold2 = 0;
	*sccdata = 0x00;                         /*transmit a char*/
	while( !bd1 )				/*wait for transmitter ready int*/
	{
		if(hold1++ > 0xFF)
			goto ERR1;
	}
	data = *sccdata;
	*ctrl = 1; *ctrl = RXINTALL;
	*sccdata = 0x00;
	while( !bd2 )				/*wait for receive interrupt*/	
	{
		if(hold2++ > 0xFF)
			goto ERR1;
	}
	*ctrl = 9; *ctrl = 0;			/*reset SCC interrupts*/
	return;
ERR1:	err1();
}


XIO_int()					/*SCC interrupt service routine*/
{
	char a, iv;
	iv = IOX->intvec;			/*read interrupt vector*/
	if((a = (iv & 0x0e)) == 0) pioTX_int();
	if(a == 0x08) pioTX_int();
	if(a == 0x04) pioRX_int();
	if(a == 0x0c) pioRX_int();
	*ctrl = 0; *ctrl = CLR_IUS;
	asm("	RETPS	");
}
pioTX_int()					/*transmitter interrupt routine*/
{
	bd1 = 0x01;			/*set test flag to get out of while loop*/
	*ctrl = CLR_TXINT;
}
pioRX_int()					/*receiver interrupt routine*/
{
	char data;
	data = *sccdata; 			/*read received data*/
	bd2 = 0x01;			/*set test flag to get out of while loop*/
}

