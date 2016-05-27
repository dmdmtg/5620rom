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
#define PFTOP_HEADINGS "f :"
#define PFBOT_HEADINGS "inschar  delchar   exit         <-      ->         next"
/*  #define PF_ED_KEY	0x82	*/
/*  #define PF_ED_LABEL	0x83	*/
#define PF_EXITKEY	0x84
#define PF_LEFTKEY	0x85
#define PF_RIGHTKEY	0x86
#define PF_NEXTKEY	0x87
#define PF_INSKEY	0x82
#define PF_DELKEY	0x83

#define PFMIN_KEYPOS	3
#define PFKEYLEN	50
#define PFMAX_KEYPOS	(PFMIN_KEYPOS + PFKEYLEN - 1)
#define PFMIN_LABPOS	60
#define PFLABLEN	8
#define PFMAX_LABPOS	(PFMIN_LABPOS + PFLABLEN - 1)
			/* several tricks take advantage of the editkey */
			/* function key being one less (in generated code) */
			/* than the editlabel function key		*/
#define PFMINPOS(ed)	(ed ? PFMIN_LABPOS : PFMIN_KEYPOS)
#define PFMAXPOS(ed)	(ed ? PFMAX_LABPOS : PFMAX_KEYPOS)
#define EDPOS(ed,cursor)  (ed ? (cursor) - PFMIN_LABPOS : (cursor) - PFMIN_KEYPOS)
#define EDLEN(ed)	(ed ? PFLABLEN : PFKEYLEN)
#define NOT_EDITTING	2	/* user is not in an editting mode */
