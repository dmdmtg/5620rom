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
**	Initialise structures for Channel talking
*/

#ifndef Blit
#include	<signal.h>
#endif Blit
#include	"pconfig.h"
#include	"proto.h"
#include	"packets.h"
#define	DECLARE				/* Define statistics array here */
#include	"pstats.h"
#include	"../layersys/vector.h"
#define NPROC	8

struct Pchannel pconvs[NPROC];
struct Pconfig pconfig;


Pch_p		pconvsend;		/* Pointer to end of pconvs */
Pbyte		pseqtable[2*SEQMOD];	/* Table of sequence numbers */
/* static */ Ph_t	Zheader;



int
#undef pinit
pinit(channels)
#define pinit Vpinit
register int	channels;
{
	register int	i;

#	ifndef	Blit
	if ( channels > MAXPCHAN )
		return(-1);
#	endif

	pconfig.xfuncp = (int(*)())Sys[148]; /* sendpkt */
	pconfig.rfuncp = (int(*)())Sys[145]; /* recvpkt */
	pconfig.rcfuncp = (void(*)())Sys[145]; /* recvpkt */
	if ( Pscanrate == 0 || Prtimeout == 0 || Pxtimeout == 0 ) {
		Pscanrate = PSCANRATE;
		Prtimeout = PRTIMEOUT;
		Pxtimeout = PXTIMEOUT;
	}

	pconvsend = &pconvs[channels];

	while ( channels-- > 0 ) {
		register Pch_p	pcp = &pconvs[channels];

		pcp->nextpkt = pcp->pkts;
		pcp->freepkts = NPCBUFS;

		for ( i = 0 ; i < NPCBUFS ; i++) {
			/* static Ph_t	Zheader; */

			pcp->pkts[i].state = PX_NULL;
			pcp->pkts[i].timo = 0;
			pcp->pkts[i].pkt.header = Zheader;
		}
	}

	for ( i = 0 ; i < 2 * SEQMOD ; i++)
		pseqtable[i] = i % SEQMOD;

#	ifndef	Blit
	signal(SIGALRM, ptimeout);
	Ptflag = 0;
#	endif
	return(0);
}



#ifdef	PDEBUG

FILE *	ptracefd	 = stderr;

void
#undef ptracepkt
ptracepkt(pkp, s)
#define ptracepkt Vptracepkt
Pkt_p		pkp;
char	*		s;
{
	register Pbyte *ucp;
	register int	size;

	fprintf(ptracefd, "tracepkt: %s -- ", s);
	if ( (size = pkp->header.dsize) > 10 )
		size = 10;
	size += sizeof(Ph_t) + EDSIZE;
	for ( ucp = (Pbyte * )pkp ; size--; )
		fprintf(ptracefd, "<%o>", *ucp++);
	fprintf(ptracefd, "\n");
}



static struct Pktstate log[PKTHIST];
static Pks_p		inlog	 = log;
static Pks_p		outlog	 = log;
static short	logflag;

void
#undef plogpkt
plogpkt(pkp, ident)
#define plogpkt Vplogpkt
Pkt_p		pkp;
unsigned char	ident;
{
	inlog->pkt = *pkp;
	inlog->state = ident;
	if ( ++inlog >= &log[PKTHIST] )
		inlog = log;
	if ( logflag ) {
		outlog = inlog;
		logflag = 0;
	}
	if ( inlog == outlog )
		logflag++;
}



void
#undef pdumphist
pdumphist(s)
#define pdumphist Vpdumphist
char	*	s;
{
	fprintf(ptracefd, "\nDumphist: %s\n", s);
	while ( outlog != inlog || logflag ) {
		logflag = 0;
		ptracepkt(&outlog->pkt, outlog->state == PLOGIN ? "recv" : "xmit");
		if ( ++outlog >= &log[PKTHIST] )
			outlog = log;
	}
	fflush(ptracefd);
}


#endif	PDEBUG
