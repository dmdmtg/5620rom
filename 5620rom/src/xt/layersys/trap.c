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
/* Trap.c should not be compiled with an optimizer, since it will
   use %r8 to speed up things.
*/


#include <dmd.h>
#include <layer.h>
#include <dmdproc.h>
#include <font.h>
#include "vector.h"
#include <cursor.h>
				/* Following actually defined in vector.s: */
extern char     *patchedspot;	/* pointer to location patched with BPT */
extern char     patch;		/* what the real contents are of *patchedspot */
extern struct Proc *debugger;	/* which process is the debugger */
extern struct Cursor cursor;
extern Font	defont;

/* Types of traps */
#define RESET   0		/* Reset -- we'll never see it */
#define PROCESS 1
#define STACK   2
#define NORMAL  3
#define INTERRUPT 4
#define TRACE   1
#define BRKPT   14

char    *trapmsg[] =    {	/* Messages indexed by trap type */
                        "",
                        "Process",
                        "Stack",
                        "Normal",
                        "Interrupt",
                };

Word	poof_bits[] = {
	0x00081000, 0x003C3800, 0x01FFFF40, 0x17FFFFE0,
	0x1FFFFFF0, 0x1FFFFFF0, 0x1FFFFFF0, 0x3FFFFFF8,
	0x7FFFFFFC, 0x7FFFFFFE, 0xFFFFFFFE, 0xFFFFFFFC,
	0x7FFFFFFC, 0x7FFFFFFC, 0x3FFFFFFA, 0x1FFFFFF0,
	0x1FFFFFE0, 0x3FE7CFE0, 0x03838380, 0x00038000,
	0x00038000, 0x00038000, 0x00838800, 0x22438000,
	0x00038200, 0x00039112, 0x4A139040, 0x10439908,
	0x112B8360, 0x249CFD00, 0x0176CE10, 0x00FFFB02 };

Bitmap	poof = { (Word *)poof_bits, 1,
			{{(short)0,(short)0},{(short)32,(short)32}},
			(char *)0};


char	ttmsg[128];		/* buffer for trap message */
char	*ttp;			/* pointer into it */
void tell_excep();

#undef trap
trap(type,bad_pc,bad_psw,psw_bad,bad_pcbp)	/* Called at spl7() */
#define trap Vtrap
 int     type;		/* trap type */
 char    **bad_pc;	/* pointer to PC at time of exception on process stack */
 int     *bad_psw;	/* pointer to PCBP "				     " */
 int     psw_bad;	/* PSW at time of exception */
 int     bad_pcbp;	/* PCBP at time of exception */
{
    int			isc = ((psw_bad >> 3) & 0xf);
    unsigned long	rel_pc;
    char 		hex[11];

        if(type == INTERRUPT) {
                return;				/* Ignored in layers */
        }
        if(patchedspot){
                *patchedspot=patch;
                patchedspot=0;
        }
        P->state|=ZOMBIE;

        P->traploc = bad_pc; /* set whenever any exception occurs */
        P->traptype = bad_psw;

        if(debugger && (type == NORMAL) && ((isc==TRACE)||(isc==BRKPT))){       
                spl0();
                sw(0);          /* BPT or trace */
                /* ZOMBIE bit is now off; we are free to continue */
                return;
        }
	ttp = ttmsg;
        copy( trapmsg[type] );
        copy(" exception #");
        copy(itox(isc,hex));
        if(type != PROCESS) {
                copy(" at ");
                copy(itox((unsigned long )*bad_pc,hex));
		rel_pc = (unsigned  long)*bad_pc - (unsigned  long)P->text;
		if( (unsigned long)rel_pc <= 0x50000 ) /* disregard absurd values */
		 {
			copy(" [.text+");
			copy(itox(rel_pc,hex));
			copy("]");
		   }
        }
        if(type != NORMAL) {
                copy(", Faulted PCBP is ");
                copy(itox(bad_pcbp,hex));
        }
	*ttp = '\0';
        if(P-proctab<=1){       /* DEMUX, CONTROL */
                error(P-proctab==0?"demux":"control", ttmsg );
        }
        while (cursor.inhibit > 0)  /* Re-enable mouse */
                cursallow();
        mpxcore((int)(P-proctab));
	/*
	 * This is a bit subtle.  The ZOMBIE bit implies only that sw()
	 * will never schedule.  So the process does not get killed, but
	 * it will never run again (except maybe by the debugger)
	 */
	P->state|=ZOMBIE;
	spl0();			/* take processor level down. */
	tell_excep();
	sw(0);			/* switch out */
}

void tell_excep()

{
    char	c;
    int	i,numc, len=strlen(ttmsg);
    Point	cp;

	cp = add(P->rect.origin,Pt(poof.rect.corner.x,10));
	numc = (P->rect.corner.x - cp.x) / defont.info[' '].width;
	i = (len/numc + ( len % numc != 0 )-1)*defont.height + poof.rect.corner.y;

	lrectf(P->layer,Rpt(P->rect.origin,
	    Pt(P->rect.corner.x,P->rect.origin.y+i)),F_OR);
	lbitblt(&poof,poof.rect,P->layer,P->rect.origin,F_XOR);
	for( i=0; i<len && cp.y+14 <= P->layer->rect.corner.y ; i += numc)		
	 {
		c=ttmsg[i+numc]; ttmsg[i+numc] = (char)0;
		string(&defont, &ttmsg[i], P->layer, cp, F_XOR);
		cp = add(cp,Pt(0,14)); ttmsg[i+numc] = c;
	   }
}

#undef copy
copy(s)
#define copy Vcopy
 char *s;
{

	do  *ttp++= *s;
	while(*s++ != (char)NULL);
	ttp--;			/* overwrite null  next time */
}

char *
#undef itox
itox(n,hex)
#define itox Vitox
        register char *hex;
	register unsigned long n;
{
	register char *hp, *hp2;
	register char temp;

#define swapitox(a,b) 	temp = *b; *b = *a; *a = temp;

	hp = hp2 = hex;
	*hp++ = '\0';
        if(n>0)
                do{
			if ((*hp++ = (n&0xf) + '0') > '9')
				*(hp - 1) += 'A' - '9' - 1;
                        n>>=4;
                }while(n);
        else
		*hp++ = '0';
	*hp++ = 'x';
	*hp = '0';
	while (hp2 < hp) {
		swapitox(hp2,hp);
		hp2++;
		hp--;
	}
	return hex;
}
#undef error
error(s1, s2)
#define error Verror
        char *s1, *s2;
{
    int	c;
        jrectf(Rect(0, (YMAX - (3 * 16)), XMAX, YMAX), F_OR);
        jmoveto(Pt(10, (YMAX-(2*16))));
        jstring(s1);
        jstring(": ");
        jstring(s2);
	jmoveto(Pt(10, YMAX-16));
	jstring("* SYSTEM EXCEPTION *             Press BREAK to restart terminal.");
	/* Bye-bye */
	while( (c=kgetc()) != 0xaf  &&  c != 0x8f );	/* poll for break */
	reboot();		/* and get out */
}
