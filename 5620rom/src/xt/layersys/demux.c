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
#include <layer.h>
#include <sys/jioctl.h>
#include <queue.h>
#include <msgs.h>
#include <dmdproc.h>
#include "pconfig.h"
#include "proto.h"
#include "packets.h"
#include "vector.h"
#include <setup.h>


extern struct Proc *kbdproc;
/* extern char *itox(); */
extern short      rebootflag; /* defined in vector.s */
extern struct physops *hostops;

#undef demux
demux(){
#define demux Vdemux
  int count = 6;
  unsigned char c, temp;

      while(!rebootflag)
	{
		while(hostops->rcvq->c_cc==0) sw(0);	/* nothing for us to do */
		if (!ENCODE_ENABLE) precv((char)qgetc(hostops->rcvq)); /* read packet */
	 	 else
		   if ((c = (char)qgetc(hostops->rcvq)) == -1) precv((char) c);
		       else
			 if ((c & 0xe0) == 0x20 || (count+=2) == 8)
			   {
				count = 0;
				temp = c;
			     }
			  else precv((char)((c & 0x3f) | ((temp << count) & 0xc0)));
	}
       nap(60);        /* let output queue drain */
       reboot();
}

int
#undef recvchars
recvchars(l, p, n)
#define recvchars Vrecvchars
     int l; char * p; int n;
{
       register struct Proc *pp;
       register char *s;
       register unsigned char *cp;
     register int i;

   if(l==0){       /* that's me!! */
               doctl(p, n);
               Pcdata=C_UNBLK; /* only needed for UTS */
               return 0;
            }
	pp= whichaddr(l);
	if (pp->state&PIPETO) /* for dmdp - redirect character */
		pp = whichaddr((pp->state >> 16) & 0x3F);

        if (pp->state&BUSY) {
           if (pp->traploc == (char **) 0) /* no exception */      
                        setrun(pp);     
                else if(((*pp->traptype &0x7b)!=0xb) && ((*pp->traptype&0x7b)!=0x73))
                   return(0); /* took a bad exception*/
            }
       else
            return 0;       /* process dead for sure  */

    if((i=n)>0){
            if(i>(sizeof(pp->cbuf)-pp->nchars))
                     return 1;       /* oops! */
             cp=pp->cbufpin;
         s=p;
            do{
                     pp->nchars++;
                   *cp++ = *s++;
                   if(cp>= &pp->cbuf[sizeof(pp->cbuf)])
                            cp=pp->cbuf;
            }while(--i>0);
          pp->cbufpin=cp;
 }
       if(pp->nchars<=(2*CBSIZE) && !(pp->state&BLOCKED))
              Pcdata = C_UNBLK;
       else if(++pconvs[l].user > NPCBUFS)     /* Inc. # of packets blocked */
         pconvs[l].user = NPCBUFS;
       return 0;
}

/*
 * for dmdp
 * returns room available in the cbuf of the layer passed as an argument
*/
#undef cbufavail
cbufavail(target)
#define cbufavail Vcbufavail
struct Proc *target;
{
	return(sizeof(target->cbuf) - target->nchars);
}

/*
 * for dmdp
 * pass the character to the selected layer
*/
#undef passchar
passchar(c, target)
#define passchar Vpasschar
int c;
struct Proc *target;
{

	register unsigned char *cp;

	cp = target->cbufpin;
	target->nchars++;
	*cp++ = c;
	if(cp>= &target->cbuf[sizeof(target->cbuf)])
		cp = target->cbuf;
	target->cbufpin = cp;

	setrun(target);
}

#undef doctl
doctl(s, n)
#define doctl Vdoctl
   register char *s;
{
      char cmd=s[0];
  register struct Proc *p= whichaddr(s[1]);
	char hex[11];
        /* extern boot(), windowstart(); */
     extern int end;
 extern short	hst_init;	/* Host is initialized */ 
struct bagent ap;

       switch(cmd){
    case JTIMO&0xFF:
                if(n!=3){
#                      ifdef   DEBUG
                   error("JTIMO n!=3", itox((unsigned long)n,hex));
#                   endif
                   return;
         }
               Prtimeout=s[1];
         Pxtimeout=s[2];
	 hst_init = 1;
         break;
#         ifdef   JTIMOM
  case JTIMOM&0xFF:
               if(n!=5){
#                      ifdef   DEBUG
                   error("JTIMOM n!=5", itox((unsigned long)n,hex));
#                  endif
                   return;
         }
               Prtimeout=max(2, (s[1] | (s[2]<<8))/1000);
              Pxtimeout=max(3, (s[3] | (s[4]<<8))/1000);
	      hst_init = 1;
              break;
#         endif
   case JBOOT&0xFF:
        case JTERM&0xFF:
        case JZOMBOOT&0xFF:
#            ifdef   DEBUG
           if(n!=2){
                       error("doctl n!=2", itox((unsigned long)n,hex));
                    return;
         }
#              endif
           if(s[1]==0){    /* i.e. demux */
                        rebootflag++;
                   break;
          }
         if (p->state & BUSY) {
	  	 freemem(p);
            	 shutdown(p);
       		 p->nchars=0;
           	 p->cbufpout = p->cbufpin;
              	 restart(p, cmd==(JTERM&0xFF)? Sys[158] : Sys[91]);   /* windowstart or boot */
         	 if(cmd==(JZOMBOOT&0xFF))
                        p->state|=ZOMBOOT;
             	 setrun(p);
         }
	 break;
  case JAGENT&0xFF:
               /*
               * agent() is passed a pointer to an agent structure,
            * modifies the contents of that structure (dest pointer and size)
               * and the results are used to Psend to the host.
                */
             {
               char kludge[MAXPKTDSIZE+1];
             register int i;

         ap.size = s[1] & 0xFF;
          ap.src = s;
             for(i=2;i<=n;i++) {     /* temp. fix for word align prob. */
                    s[i-2] = s[i];
          }
               agent(&ap);
             if((ap.size < 1) || (ap.size > MAXPKTDSIZE))
                    ap.size = MAXPKTDSIZE;
          kludge[0] = ap.size;
            for ( i=1; i<ap.size; i++)
                      kludge[i] = *(ap.dest)++;
               Psend(0, kludge, ap.size + 1, JAGENT & 0xFF);
           break;
          }
       default:
                error("unk ctl", itox((unsigned long)cmd,hex));
     }
}
