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
########
#
#	libc makefile
#
########
#
#	Standard Macros
#
########
AUX_CLEAN = 
MAKE.LO = make.lo
MAKE.ROOT = $(ROOT)/make.root
MKGEN = mkgen
ROOT = ../../..
SGSX = 

INC_LIST	=

CLEAN =\
	abs.o\
	strlen.o\
	qsort.o

all:
install:	all

##########
#
#	make.root
#
##########

DMDBIN=$(ROOT)/bin
DMDLIB=$(ROOT)/lib
DMDSRC=$(ROOT)/src
INC=$(ROOT)/include
UINC=/usr/include
DMDDEMO=$(ROOT)/demo
MCC=DMD=$(ROOT) $(DMDCC)
MLD=m32ld
MAR=dmdar
MLORDER=m32lorder

########
#
#	make.lo
#
########

TITLE = libc makefile
PRODUCTS = libc.a


MAKEARG= -$(MAKEFLAGS) 

MLORDER=m32lorder
LCLCFLAGS=-g -O -J
CLEAN=$(OBJ) libc.a

OBJ=abs.o strlen.o qsort.o

.c.o:
	$(MCC) $(MCFLAGS) -c $<

.s.o:
	$(MCC) -c $<

libc.a:	abs.o strlen.o qsort.o
	$(MAR) cr libc.a `$(MLORDER) $(OBJ) | tsort`

install:	libc.a
	cp libc.a $(DMDLIB)/libc.a


########
#
#	All dependencies and rules not explicitly stated
#	(including header and nested header dependencies)
#
########

abs.o:	$(INC)/sa.h
abs.o:	abs.c
	$(MCC) $(MCFLAGS) -c abs.c

strlen.o:	$(INC)/sa.h
strlen.o:	strlen.c
	$(MCC) $(MCFLAGS) -c strlen.c

qsort.o:	$(INC)/sa.h
qsort.o:	qsort.c
	$(MCC) $(MCFLAGS) -c qsort.c

########
#
#	Standard Targets
#
#	all		builds all the products specified by PRODUCTS
#	clean		removes all temporary files (ex. installable object)
#	clobber		"cleans", and then removes $(PRODUCTS)
#	makefile	regenerates makefile
#	install		installs products; user defined in make.lo 
#
########

all:		$(PRODUCTS)

clean:
		rm -f $(CLEAN) $(AUX_CLEAN)

clobber:	clean
		rm -f $(PRODUCTS)

makefile:	$(MAKE.LO) $(MAKE.ROOT)
		$(MKGEN) >make.out
		if [ -s make.out ]; then mv make.out makefile; fi

makefile_all:	makefile

install: 	# rules, if any, specified above
