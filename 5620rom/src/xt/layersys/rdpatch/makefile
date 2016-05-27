# 
#									
#	Copyright (c) 1985,1986,1987,1988,1989,1990,1991,1992   AT&T	
#			All Rights Reserved				
#									
#	  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.		
#	    The copyright notice above does not evidence any		
#	   actual or intended publication of such source code.		
#									
# 
# The DMDCC line below will cause the host sgs to be used. Change it if
# this is a problem.
DMDCC=$(DMD)/bin/dmdcc
ROOT=../../../..
INC = $(ROOT)/include
DMDBIN=$(ROOT)/bin
DMDLIB=$(ROOT)/lib
DMDSRC=$(ROOT)/src
DMDINCLUDE=$(ROOT)/include
UINC=/usr/include
DMDDEMO=$(ROOT)/demo
MCC=$(DMDCC)
MLD=m32ld
MAR=ar
FRC=FRC
MLORDER=m32lorder
PDIR=../../proto
LCLCFLAGS = -Uvax -J -O -g -DBlit -DMPXTERM -I$(ROOT)/include -I../../proto

all : patch patch.nm

patch :	map lrom.o startup.o mainpcb.o pl.o  # and other patch files
	m32cc -Rlrom.o startup.o pl.o -o patch map -m > patch.map

patch.nm : patch
	m32nm -e -x patch >patch.nm

lrom.s:	lsys.nm.2.0 badname.f termsed.f
	sed -f badname.f -f termsed.f lsys.nm.2.0 > lrom.s

lrom.o: lrom.s
	$(MCC) -c lrom.s
	
mainpcb.o : mainpcb.c
	$(MCC) -c -J -DMPXTERM mainpcb.c

startup.o:  $(INC)/sys/2681.h # nested include from jerq.h
startup.o : $(INC)/jerq.h
startup.o : $(INC)/setup.h
startup.o : startup.c
	$(MCC) -c -J -DMPXTERM startup.c

pl.o:	pl.s
	$(MCC) -c -g pl.s
