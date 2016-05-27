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
#include	<dmd.h>
#include	<font.h>
#include	<setup.h>
#include	<options.h>
#include	<keycodes.h>

char *pfeditl[] = { "Main Menu","Next Key","Previous","<-","->","Insert","Delete","Erase",0};
char *confirm = "Confirm";   /* used with Erase */
char	s_cpos;		/* cursor position in characters */
Rectangle	pflrect;
int	s_insert;

extern int	mhighlight;
extern Rectangle	s_rect;

int
pfedit(n)
  int	n;		/* key hit to enter */

{

  Rectangle	pfrect;
  Point		pos, leftedge, rightedge, epos;
  char		pfname[4],
		*p;
  char		pfvalue[PFKEYSIZE+1];
  int		pflen,			/* # of Non-NULL characters in pfvalue */
		i, j,			/* classic scratch variables */
		s,			/* return value from s_waitkey */
		s_erase;		/* 1 if erase pushed ONCE */

    do {
	pfrect.origin = s_rect.origin;
	pfrect.corner = add(pfrect.origin,Pt(XMAX,NS));

	pflrect.origin = add(s_rect.origin,Pt(0,NS));
	pflrect.corner = s_rect.corner;

	for( pflen=0, p = (char *)&BRAM->pfkeys[n-1][0].byte;
	     pflen<PFKEYSIZE && *p != (char)0; pflen++, p+=4 ) pfvalue[pflen] = *p;
	for( i=pflen; i<PFKEYSIZE+1; i++ ) pfvalue[i] = (char)0;

	cursinhibit();

	rectf(&display,s_rect,F_CLR);
	if( VALCAPS ) caps_msg();
	if( VALNUM ) num_msg();
	drawpads();
	rectf(&display,pfrect,F_CLR);
	pfname[0]='f'; pfname[1]=n+'0'; pfname[2]=':'; pfname[3]='\0';
	pos = string(&defont,pfname,&display,pfrect.origin,F_XOR);
	rightedge = string(&defont,pfvalue,&display,pos,F_XOR);
	leftedge = pos;
	rightedge.y += NS;	/* so that {leftedge,rightedge} is a Rectangle */

	for( pos=pflrect.origin, i=0; /* Draw PF key labels for pf edit */
		 i<8; pos.x += CW*(i==2||i==4?13:10)+2*S_BORDER, i++)
				s_draw(0,pos,pfeditl[i]);
	epos.y = pos.y;	/* set position to write Confirm */
	epos.x = pos.x - CW*10 - 2*S_BORDER;

	s_cpos=0;		/* text cursor position */
	s_insert = 0;		/* insert mode off */
	s_erase = 0;		/* initially confirm erasing */
	pos = leftedge;		/* Point value of text cursor position */
	s_curse(pos);		/* text cursor on */
	
	cursallow();		/* mouse cursor back on */

	while( (s=s_waitkey(1)) != 0 && s != -1 && 
		s != -3 && s != FUNC1KEY && s != FUNC2KEY  && s != FUNC3KEY )
	 {
		if( s_erase && s!=FUNC8KEY ) { 
			s_erase = 0;
			s_draw(0,epos,confirm);
			s_draw(0,epos,pfeditl[7]);
		}
		if( s == -2 )	/* mouse button hit outside pf keys */
		 {
			if( ptinrect(mouse.xy,pfrect) )
			 {
				s_curse(pos);		/* erase cursor */
				if( mouse.xy.x < leftedge.x ) s_cpos = 0;
				 else s_cpos = (mouse.xy.x - leftedge.x)/CW;
				if( s_cpos > pflen ) s_cpos = pflen;
				if( s_cpos >= PFKEYSIZE ) s_cpos = PFKEYSIZE-1;
				pos.x = s_cpos*CW+leftedge.x;
				s_curse(pos);		/* move cursor */
			   }
			 continue;
		   }

		s_curse(pos);		/* erase cursor */
		switch( s )		/* branch on char hit */
		 {
		   case FUNC4KEY:	/* <- */
				  if( s_cpos == 0 ) break;
				   else s_cpos--;
				  break;
		   case FUNC5KEY:	/* -> */
		   		  s_cpos++;
				  if( s_cpos > pflen || s_cpos > PFKEYSIZE-1 ) s_cpos--;
				  break;
		   case FUNC6KEY:	/* Insert */
				  s_insert = ~s_insert;
				  break;

		   case FUNC7KEY:	/* Delete */
				  if( pflen == 0 || s_cpos == pflen ) break;
				  string(&defont,&pfvalue[s_cpos],&display,pos,F_XOR);
				  for( i=s_cpos; i<pflen; i++ )
					pfvalue[i] = pfvalue[i+1];
				  pflen--;	/* one less to worry about */
				  string(&defont,&pfvalue[s_cpos],&display,pos,F_XOR);
				  break;
		   case FUNC8KEY:	/* erase PF key */
				  if( !s_erase ) 
				   {
					s_erase++;
					s_draw(0,epos,pfeditl[7]);
					s_draw(0,epos,confirm);
					break;
				     }
				  string(&defont,pfvalue,&display,leftedge,F_XOR);
				  rightedge.x = leftedge.x;
				  pflen=0;
				  pfvalue[0]=(char)0;
				  s_cpos=0;
				  s_draw(0,epos,confirm);
				  s_draw(0,epos,pfeditl[7]);
				  s_erase--;
				  break;

		   default:			/* the fun stuff */
				  if( s_insert ) /* two cases */
				    {
					string(&defont,&pfvalue[s_cpos],
						&display,pos,F_XOR);
					for( i=PFKEYSIZE-1; i>s_cpos; i-- )
						pfvalue[i] = pfvalue[i-1];
					pfvalue[s_cpos] = s;
					if( pflen < PFKEYSIZE ) pflen++;
					string(&defont,&pfvalue[s_cpos],
						&display,pos,F_XOR);
				      }
				    else {
					    pfname[2] = s;	/* kludge! */
					    string(&defont,&pfname[2],&display,
							pos,F_STORE);
					    pfvalue[s_cpos] = s;
					    if( s_cpos == pflen && pflen < PFKEYSIZE ) { 
							pflen++;
							pfvalue[pflen] = (char)0;
		}
					  }
				  s_cpos++;
				  if( s_cpos > PFKEYSIZE-1 ) s_cpos--;
				  break;
		 }
	
		rightedge.x = leftedge.x + pflen*CW;
		pos.x = s_cpos*CW+leftedge.x;
		s_curse(pos);		/* cursor back out */
	  }
	spl7();		/* Critcal Code */
	checkbram();
	for( i=0, p = (char *)&BRAM->pfkeys[n-1][0].byte; i<=pflen; i++, p+=4 )
		*p = pfvalue[i];
	setbram();
	spl0();
	if( s == FUNC2KEY )
	   if( n == 8 ) n=1;
	     else n++;
	if( s == FUNC3KEY )
	   if( n == 1 ) n=8;
	     else n--;
      } while( s == FUNC3KEY || s == FUNC2KEY );
      if( s == -3 ) return(0);
      if( s <= 0 ) return( s-1 );
      return(0);
}

char	s_csr[] = "\001";

s_curse(pos)
  Point	pos;

{

	if( s_insert )	/* insert mode indicated by thin cursor */
		segment(&display,pos,add(pos,Pt(0,NS)),F_XOR);
	 else string(&defont,s_csr,&display,pos,F_XOR);
}
