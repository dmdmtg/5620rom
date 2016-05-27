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
#	liblayer makefile
#
########
#
#	Standard Macros
#
########
AUX_CLEAN = 
INC = $(ROOT)/include
MAKE.LO = make.lo
MAKE.ROOT = $(ROOT)/make.root
MKGEN = mkgen
ROOT = ../../..
SGSX = 

INC_LIST	=\
	-I$(INC)

CLEAN =\
	.c.o\
	.s.o\
	dellayer.o\
	downback.o\
	layerop.o\
	lbitblt.o\
	lblt.o\
	lbox.o\
	lpoint.o\
	lrectf.o\
	lsegment.o\
	ltexture.o\
	newlayer.o\
	obsclean.o\
	upfront.o

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
DMDINCLUDE=$(ROOT)/include
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

TITLE = liblayer makefile
PRODUCTS = liblayer.a

AS=m32as
CFLAGS=-I$(DMDINCLUDE) -g -O
MAKEARG= -$(MAKEFLAGS) DMDBIN=$(DMDBIN) DMDLIB=$(DMDLIB) DMDSRC=$(DMDSRC)\
	DMDINCLUDE=$(DMDINCLUDE) DMDSGS=$(DMDSGS) DMDDEMO=$(DMDDEMO)\
	MCC=$(MCC) MCFLAGS=$(MCFLAGS) MLD=$(MLD) MAR=$(MAR)
CLEAN= $(OBJ) liblayer.a
MLORDER=m32lorder
LCLCFLAGS=-J -g -O

SOURCE=dellayer.c layerop.c lbitblt.c lblt.c lpoint.c lrectf.c lsegment.c ltexture.c newlayer.c upfront.c lbox.c downback.c obsclean.c

UINC = .

OBJ=layerop.o lbitblt.o lblt.o lrectf.o dellayer.o\
	lpoint.o ltexture.o lsegment.o newlayer.o upfront.o lbox.o downback.o obsclean.o 

.c.o:
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c $<

.s.o:
	$(MCC) -c $<

liblayer.a: $(OBJ)
	$(MAR) cr liblayer.a `$(MLORDER) $(OBJ) | tsort`

save:
	-cp $(DMDLIB)/liblayer.a oliblayer.a

lint:
	lint $(SOURCE)

print:
	@pr layer.h $(SOURCE)

install:	liblayer.a
	cp liblayer.a $(DMDLIB)/liblayer.a

########
#
#	All dependencies and rules not explicitly stated
#	(including header and nested header dependencies)
#
########

dellayer.o:	dellayer.c
dellayer.o:	$(INC)/dmd.h
dellayer.o:	$(INC)/sa.h
dellayer.o:	$(INC)/layer.h
dellayer.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c dellayer.c

downback.o:	$(INC)/dmd.h
downback.o:	$(INC)/sa.h
downback.o:	$(INC)/dmdproc.h
downback.o:	$(INC)/layer.h
downback.o:	$(INC)/sys/2681.h
downback.o:	downback.c
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c downback.c

layerop.o:	$(INC)/dmd.h
layerop.o:	$(INC)/sa.h
layerop.o:	$(INC)/layer.h
layerop.o:	layerop.c
layerop.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c layerop.c

lbitblt.o:	$(INC)/font.h
lbitblt.o:	$(INC)/dmd.h
lbitblt.o:	$(INC)/sa.h
lbitblt.o:	$(INC)/layer.h
lbitblt.o:	lbitblt.c
lbitblt.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c lbitblt.c

lblt.o:	$(INC)/dmd.h
lblt.o:	$(INC)/sa.h
lblt.o:	$(INC)/layer.h
lblt.o:	lblt.c
lblt.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c lblt.c

lbox.o:	$(INC)/dmd.h
lbox.o:	$(INC)/sa.h
lbox.o:	$(INC)/layer.h
lbox.o:	lbox.c
lbox.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c lbox.c

lpoint.o:	$(INC)/dmd.h
lpoint.o:	$(INC)/sa.h
lpoint.o:	$(INC)/layer.h
lpoint.o:	lpoint.c
lpoint.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c lpoint.c

lrectf.o:	$(INC)/dmd.h
lrectf.o:	$(INC)/sa.h
lrectf.o:	$(INC)/layer.h
lrectf.o:	lrectf.c
lrectf.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c lrectf.c

lsegment.o:	$(INC)/dmd.h
lsegment.o:	$(INC)/sa.h
lsegment.o:	$(INC)/layer.h
lsegment.o:	$(INC)/line.h
lsegment.o:	lsegment.c
lsegment.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c lsegment.c

ltexture.o:	$(INC)/dmd.h
ltexture.o:	$(INC)/sa.h
ltexture.o:	$(INC)/layer.h
ltexture.o:	ltexture.c
ltexture.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c ltexture.c

newlayer.o:	$(INC)/dmd.h
newlayer.o:	$(INC)/sa.h
newlayer.o:	$(INC)/layer.h
newlayer.o:	newlayer.c
newlayer.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c newlayer.c

obsclean.o:	$(INC)/dmd.h
obsclean.o:	$(INC)/sa.h
obsclean.o:	$(INC)/layer.h
obsclean.o:	$(INC)/sys/2681.h # nested include from dmd.h
obsclean.o:	obsclean.c
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c obsclean.c

upfront.o:	$(INC)/dmd.h
upfront.o:	$(INC)/sa.h
upfront.o:	$(INC)/layer.h
upfront.o:	$(INC)/sys/2681.h # nested include from dmd.h
upfront.o:	upfront.c
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c upfront.c

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
