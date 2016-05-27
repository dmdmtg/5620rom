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
#define	OWN_SETUP_H	1
#include	<dmd.h>
#include	<font.h>
#include	<setup.h>
#include	<options.h>
#include	<cursor.h>
#include	<queue.h>
#include	<kbd.h>

Rectangle	s_rect, kill_rect;
extern Rectangle pflrect;
Rectangle Jrect={0, 0, XMAX, YMAX};	/* Not strictly kosher */

Word setup_space[XMAX/(8*sizeof(Word))][NS*2];	/* space for setup to 	*/
						/* screenswap to 	*/
Bitmap setup_swap = { (Word *)setup_space, XMAX/(8*sizeof(Word)), 
			{{(short) 0, (short) 0},{(short) XMAX,(short) NS*2}},
			(char *) 0};

#undef maxaddr
long maxaddr[] = {0x740000, 0x800000}; /* last valid memory address */

#undef baud_speeds
char baud_speeds[NUMBAUD] = { (char) BD1200BPS, (char) BD2400BPS, (char) BD4800BPS,
			     (char) BD9600BPS, (char) BD19200BPS, (char) BD300BPS};

int	mhighlight;
int rsetup_pcb[20];		/* PCB for setup */
int setstk[400];		/* setup stack */

void	drawpads();
void	drawlabels();
void	s_drawopt();
Rectangle highlight();

/*
 * NOTE: Setup runs as a process off "setup_pcb".  It is always invoked by
 * the keyboard interrupt handler (auto2) which has the PCBP for the interrupted
 * user level process on the interrupt stack.  Thus, when setup does a RETPS
 * at the end, it resumes the interrupted user level process.   Note that
 * setup DOES NOT preserve the full mouse state, only cursor inhibited.
 * Entering setup at a point where the mouse cursor position is sensitive
 * may not be a good idea.
 */

#undef	setup
int
setup()
#define	setup	Ssetup
{
   int	s, where = 0;
   int	ret, savedc;
   Texture16	*oldcur;

	spl0();
	s_rect.origin.x = Jrect.origin.x;
	s_rect.origin.y = Jrect.corner.y - (2*NS);
	s_rect.corner = Jrect.corner;
	rectf(&setup_swap,setup_swap.rect,F_CLR);
	oldcur = cursswitch((Texture16 *)0);
	savedc = cursor.inhibit;
	while( cursor.inhibit ) cursallow();
	cursinhibit();
	screenswap(&setup_swap,setup_swap.rect,Rect(0,YMAX-(2*NS),XMAX,YMAX));
	SetupFlag = 1;
	if( VALCAPS ) caps_msg();
	if( VALNUM ) num_msg();
	cursallow();

	mhighlight = 0;
	drawpads();			/* draw setup pads */
	drawlabels(0);

	while( (s=s_waitkey(0)) > 0 )	/* s is # of PF key hit */
	 {
		if( s == 1 )	/* return to main menu */
		 {
			drawlabels(where);	/* erase current labels */
			where = 0;
			drawlabels(where);
			continue;
		   }
		s=s-FUNC1KEY+1;
		if( SetupTree[where+s-1].su_iocard && !IOCARD )
			ringbell();
		 else switch( SetupTree[where+s-1].su_keytype )
		  {
		   case 0:	/* wierd key */
				ringbell();
				break;
		   case 1:
		   		/* move to next level */
				drawlabels(where);	/* erase current labels */
				where = SetupTree[where+s-1].su_val;
				drawlabels(where);
				break;

		   case 2:	/* toggle option value */
				{
			          int	optval;
				  Point pos;
				  struct valus *valptr =
					(struct valus *)SetupTree[where+s-1].su_val;

					optval = (char)*(valptr->va_bram);
					pos = add(s_rect.origin,
					     Pt((s-1)*(10*CW+2*S_BORDER),1));
					if( s>3 ) pos.x += 3*CW;
					if( s>5 ) pos.x += 3*CW;
					s_drawopt(where+s-1,pos);	/* erase value */
					optval++;	/* bump value */
					if( valptr->va_names[optval] == (char *)0 )
						optval=0;
					spl7();		/* Critical Section */
					checkbram();
					*(valptr->va_bram) = (char)optval;
					setbram();
					spl0();
					if( valptr->va_init != (int(*)())NULL )
						(*valptr->va_init)(valptr->va_bram);
					s_drawopt(where+s-1,pos);
					break;
				  }

			
		   case 3:	/*  pf edit */
				drawlabels(where);	/* erase current labels */
				ret = (*(int (*)())SetupTree[where+s-1].su_val)(s);
				cursinhibit();
				rectf(&display,s_rect,F_CLR);
				if( VALCAPS ) caps_msg();
				if( VALNUM ) num_msg();
				cursallow();
				drawpads();
				if( ret < 0 )
				  {
					s = ret+1;	/* gotta get out */
					break;
				    }
				 else where = ret;
				drawlabels(where);
				break;

		   default:	/* eh? */
				string(&defont,
					"SETUP: Fatal Error/Illegal Key Type  (Press SETUP)",&display,s_rect.origin,F_STORE);
				break;
		}
		if( s <= 0 ) break;
	   }


	cursinhibit();
	qclear(&KBDQUEUE);	/* then empty any type-ahead to setup */	
	screenswap(&setup_swap,setup_swap.rect,Rect(0,YMAX-(2*NS),XMAX,YMAX));
	cursor.inhibit = savedc;
	cursswitch(oldcur);
	SetupFlag = 0;		/* reset setup flag */
	caps_msg();
	num_msg();
	if( s == 0 )		/* terminal reboot */
		reboot();
	 else
asm("		RETPS");	/* else just return to process */
				/* interrupted by keyboard interrupt routine */
				/* its PCBP is still on the interrupt stack */
}



void
drawpads()

{
   Rectangle	r;
   int		i, inc;

	cursinhibit();
	r.origin = s_rect.origin;
	r.corner.x = r.origin.x+CW*9 + 2*S_BORDER;
	r.corner.y = s_rect.corner.y;

	for( i=0; i<8; i++ )
	 {
		rectf(&display,r,F_XOR);
		r.origin.x += inc = CW*(i==2||i==4?13:10) + 2*S_BORDER;
		r.corner.x += inc;
	    }
	cursallow();
	mhighlight=0;
}


/*
 * drawlabels(n) draws the eight PF labels starting at index 'n'
 * in SetupTree.  Draws in XOR mode, so erasing is simply another
 * call.
 */

void
drawlabels(n)

  int	n;		/* starting index in SetupTree */

{

   Point	pos;
   int		i;
   void 	s_draw();

	pos=s_rect.origin; pos.y++;	/* One dot vertical border */

	for( i=0; i<8; pos.x += CW*(i==2||i==4?13:10) + 2*S_BORDER, i++, n++ )
	 {
		if( SetupTree[n].su_iocard && !IOCARD ) continue;
		s_draw(0,pos,SetupTree[n].su_lbl1);
		if( SetupTree[n].su_keytype == 2 ) s_drawopt(n,pos);
		  else s_draw(1,pos,SetupTree[n].su_lbl2);
		
	   }
}

void
s_drawopt(n,pos)
  int	n;
{
    int	optval;
    struct valus *valptr;

	valptr=(struct valus *)SetupTree[n].su_val;
	optval = (char)*(valptr->va_bram);
	s_draw(1,pos,valptr->va_names[optval]);
}

void 
s_draw(line,pos,s)

   int	line;
   Point	pos;
   char	*s;

{
   int	len=strlen(s);

	if( s == (char *)NULL ) return;
	pos.x += CW*(4-(len/2)) + S_BORDER; pos.y += NS*line;
	cursinhibit(); string(&defont,s,&display,pos,F_XOR); cursallow();
}



int
s_waitkey(whence)

  int	whence;		/* caller 0-main loop, 1-pfedit */
{

    char c;
    int  o;


	for(;;)
	 {
	     o=wait(KBD|MOUSE);

	     if( o&KBD )
	      {
		if( mhighlight ) /* turn off mouse stuff */
		 {
			highlight(whence,mhighlight);
			mhighlight=0;
		   }
		 if( (c=kbdchar()) >= FUNC1KEY && c <= FUNC8KEY )
				return(c);
		   else switch( c )
	            	  {
		  		case SETUP:	/* Setup Key exits setup */
		  	  	   	   return(-1);
		  		case RESET:	/* Shift/Setup is reset */
		  	   	   	   return(0);
		  		default:   if( whence ) return(c);
					    else ringbell();	/* beep! */
		  	   	   	   break;
	    	      	     }
		}
	       else if( o&MOUSE )
		  {
		     if( bttn3() )
			  {
				while( bttn3() ) wait(MOUSE);
				return(-1);
	   		    }
		     if( bttn2() )
			  {
				while( bttn2() ) wait(MOUSE);
				return(whence?-3:1);
	   		    }
		      if( ptinrect(mouse.xy, s_rect) )
			{
			   if( do_mouse(whence) )
				if( !whence && mhighlight>0 )
					return(mhighlight-1+FUNC1KEY);
			            else if( whence )
					  if( mhighlight > 0 )
						return(mhighlight-1+FUNC1KEY);
					   else return(-2);
			  }
		       else if( mhighlight )
			     {
				highlight(whence,mhighlight);
				mhighlight=0;
			      }
		     }
	   }
}

do_mouse(whence)
  int	whence;		/* 0-main loop, 1 - pfedit */

{

    int	k=0;
    Rectangle	r;
    int		i, inc;

		if( whence ) r.origin = pflrect.origin;
	 	 else r.origin = s_rect.origin;
		r.corner.x = r.origin.x+CW*9 + 2*S_BORDER;
		r.corner.y = s_rect.corner.y;

		for( i=0; i<8; i++ )
	 	{
			if( ptinrect(mouse.xy,r) )
		   	 {
				k = i+1;
				break;
		     	  }
			r.origin.x += inc = CW*(i==2||i==4?13:10) + 2*S_BORDER;
			r.corner.x += inc;
	    	  }
	if( mhighlight != k )
	 {
		if( mhighlight )
			highlight(whence,mhighlight);
		if( k ) highlight(whence,k);
		mhighlight=k;
	   }
	if( bttn1() )	/* button push -- wait for release */
	  {
		while( bttn1() ) wait(MOUSE);
		return(1);		/* return that we got a push */
	   }
	 else return(0);
}


Rectangle
highlight(whence,k)

   int	whence, k;

{

    Rectangle	r;


	if( k == 9 ) r = kill_rect;
	 else {
		if( whence ) r = pflrect;
	 	 else r = s_rect;
		r.origin.x += (k-1)*(CW*10 + 2*S_BORDER);
		if( k>3 ) r.origin.x += 3*CW;
		if( k>5 ) r.origin.x += 3*CW;
		r.corner.x = r.origin.x + 9*CW + 2*S_BORDER;
	      }

	r=inset(r,1);

	cursinhibit(); rectf(&display,r,F_XOR); cursallow();
	return(r);
}


/* Option Initialization Functions
 *
 *  These exist to perform any initialization required when an option
 *  value is changed.  A pointer to the appropriate initialization function
 *  is kept with the value list for an option, in the structure SetupVals.
 *
 *  These functions are passed a pointer to the BRAM location containing the
 *  new value.
 */

toneinit(p)
  char	*p;

{

	if( *p )
		kbdstatus &= ~TTY_CHIRP;
	 else
		kbdstatus |= TTY_CHIRP;
	while (!trysend(0,0)); /* shove it down the keyboard's throat */
}

eia_init(p)
  char	*p;
{
	aciainit(baud_speeds[VALABAUD]);	/* just reset whole port */
}

colorinit(p)

  char	*p;

{
	if (VALSCREENCOLOR)
		BonW();
	 else
		WonB();
}
