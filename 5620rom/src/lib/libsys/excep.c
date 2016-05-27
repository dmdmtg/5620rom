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
#include <font.h>
#include <setup.h>
#include <kbd.h>
#include <sys/2681.h>


#undef excep_norm
excep_norm()
{
	register char *str_p;	/* %r8 */

	asm(" ORW2  &0x281e000,%psw");	/* IPL up to 15 */
	asm(" NOP"); 	/* let the bits settle */
	asm(" PUSHW  -0x24(%sp)");	/* push saved pc -(8 + save dist) */
	asm(" PUSHW  -0x24(%sp)");	/* push saved psw */
	str_p = "Normal";
	asm(" PUSHW  %r8");	/* push string ptr */
	asm(" PUSHW  %pcbp");	/* push pcbp */
	asm(" CALL  -4*4(%sp),excep_msg");	/* this routine won't return */
}


#undef excep_stack
excep_stack()
{
	register char *str_p;	/* %r8 */

	asm(" MOVW  -0x4(%isp),%r8");	/* get saved %pcbp */
	asm(" PUSHW  0x4(%r8)");	/* push saved pc */
	asm(" PUSHW  0x0(%r8)");	/* push saved psw */
	str_p = "Stack";
	asm(" PUSHW  %r8");	/* push string ptr */
	asm(" PUSHW  %r8");	/* push current pcbp */
	asm(" CALL  -4*4(%sp),excep_msg");
}

#undef excep_proc
excep_proc()
{
	register char *str_p;	/* %r8 */

	asm(" PUSHW  -4(%isp)");	/* push pcbp an extra time, since no pc */
	asm(" PUSHW  %psw");	/* push current psw */
	str_p = "Process";
	asm(" PUSHW  %r8");	/* push string ptr */
	asm(" PUSHW -4(%isp)");	/* push current pcbp */
	asm(" CALL  -4*4(%sp),excep_msg");
}

#undef excep_int
excep_int()
{
	register char *str_p;	/* %r8 */

	asm(" PUSHW  -4(%isp)");	/* push pcbp an extra time, since no pc */
	asm(" PUSHW  %psw");	/* push current psw */
	str_p = "Stray Interrupt";
	asm(" PUSHW  %r8");	/* push string ptr */
	asm(" PUSHW -4(%isp)");	/* push current pcbp */
	asm(" CALL  -4*4(%sp),excep_msg");
}


#undef excep_msg
excep_msg(pc, psw, s, pcbp)
int pc, psw;
char *s;
int pcbp;
{
	Point p;	/* saves current "string" (ie, printf) position */
	char message[9];	/* temporary string for printouts */

	ringbell();
	rectf(&display, Rect(0,YCMAX*NS + YMARGIN,XMAX,YMAX), F_CLR); /* clear lower screen */
	p = string(&defont,s,&display,Pt(20,(YCMAX+1)*NS + YMARGIN),F_XOR);
	p = string(&defont," Exception #0x",&display,p,F_XOR);
	l_into_xc((psw >>3) &0xf,message);
	p = string(&defont,message,&display,p,F_XOR);

	if (s[0] != 'P'){
		p = string(&defont," at 0x",&display,p,F_XOR);
		l_into_xc(pc, message);
		p = string(&defont,message,&display,p,F_XOR);
	}
	if (s[0] != 'N')
	{
		p = string(&defont,", Faulted PCBP=",&display,p,F_XOR);
		l_into_xc(pcbp,message);
		p = string(&defont,message,&display,p,F_XOR);
	}
		
	string(&defont,"!  Type the SETUP key to continue",&display,p,F_XOR);

/*	asm(" ANDW2 &0xffff0fff,%psw");	/* allow interrupts */
	while (((pc =kgetc()) != 0x8e) && (pc != 0xae));
			/* wait for setup key to continue */
	asm(" JMP *860");	/* reboot */
}


l_into_xc(a, v)
int a;
char *v;
{
	register i, j, k;

	k = 0;
	for (i=0; i<8; i++)
	{
		j = ((a >> ((7 - i) * 4)) & 0xf);
		if ((j == 0) && (k == 0))
			continue;
		if (j > 9)
			v[k] = j - 0xa + 'A';
		else
			v[k] = j + '0';
		k++;
	}
	if (k == 0)
		v[k++] = '0';
	v[k] = (char) 0;
}
