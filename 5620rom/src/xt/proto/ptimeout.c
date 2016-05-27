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
**	Process timeouts for packets
*/

#include	"pconfig.h"
#include	"proto.h"
#include	"packets.h"
#include	"pstats.h"
#include	"../layersys/vector.h"



void
#undef ptimeout
ptimeout(sig)
#define ptimeout Vptimeout
	int		sig;
{
	register Pch_p	pcp;
	register Pks_p	psp;
	register int	i;
	register int	retrys;

#	ifndef	Blit
	signal(sig, ptimeout);
#	endif
	Ptflag = 0;

	if ( precvpkt.timo > 0 && ++Ptflag && (precvpkt.timo -= Pscanrate) <= 0 )
	{
		precvpkt.state = PR_NULL;
		ptrace("RECV TIMEOUT");
	}

	for ( pcp = pconvs, retrys = 0 ; pcp < pconvsend && retrys < MAXTIMORETRYS ; pcp++ )
		for ( psp = pcp->nextpkt, i = NPCBUFS ; i-- ; )
		{
			if ( psp->timo > 0 && ++Ptflag && (psp->timo -= Pscanrate) <= 0 )
			{
				ptrace("XMIT TIMEOUT");
				psp->timo = Pxtimeout;
#				ifndef	Blit
				(*Pxfuncp)(Pxfdesc, (char *)&psp->pkt, psp->size);
#				else
				(*Pxfuncp)(((char *)&psp->pkt)+2, psp->size);
#				endif
				PSTATS(PS_TIMOPKT);
				plogpkt(&psp->pkt, PLOGOUT);
				ptrace("END TIMEOUT");
				if ( ++retrys >= MAXTIMORETRYS )
					break;
			}
			if ( ++psp >= &pcp->pkts[NPCBUFS] )
				psp = pcp->pkts;
		}

#	ifndef	Blit
	if ( Ptflag )
		(void)alarm(Pscanrate);
#	endif
}
