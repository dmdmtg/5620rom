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
#	term makefile
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
ROOT = ../..
LAYERSYS = ../xt/layersys

INC_LIST	=\
	-I$(INC)

CLEAN =\
	.c.o\
	.s.o\
	bootrom.o\
	bram.o\
	link.o\
	load.o\
	lowmem.o\
	pcbs.o\
	selftest.o\
	vector.o\
	vitty.o

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

TITLE = term makefile
PRODUCTS = trom.o 
LCLCFLAGS=
MNM=m32nm
MLFLAGS=-m -L$(DMDLIB)
MCFLAGS=-g -O -J -I$(INC) -DROMTERM
CLEAN = *.o trom.s romterm romterm.nm romterm.map builddate.c $(LAYERSYS)/trom.o
FRC=FRC

OBJ=vitty.o bootrom.o load.o link.o lowmem.o pcbs.o selftest.o bram.o vector.o builddate.o
CFILES=vitty.c bootrom.s load.s link.c lowmem.c pcbs.c selftest.c bram.c vector.s

all:	trom.o

.c.o:	
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c $<

.s.o:	
	$(MCC) $(MCFLAGS) -c $<

romterm: $(OBJ) $(DMDLIB)/libj.a $(DMDLIB)/libsys.a \
$(DMDLIB)/liblayer.a map $(DMDLIB)/libc.a
	m32ld $(MLFLAGS) -o romterm $(OBJ) -llayer -lsys -lj -lc map > romterm.map

trom.s:	romterm.nm badname.f termsed.f
	sed -f badname.f -f termsed.f romterm.nm > trom.s

trom.o: 
	$(MCC) $(MCFLAGS) -c trom.s
	
builddate.o:	$(FRC)
		echo 'char build_date[]="' `date '+VERSION DATE: %h %d, 19%y'` '";' >builddate.c
		$(MCC) $(MCFLAGS) -c builddate.c
romterm.nm:	romterm
	$(MNM) -e -x -n romterm > romterm.nm

save:
	-cp romterm oromterm
	-cp trom.o otrom.o

install: trom.o
	cp trom.o $(LAYERSYS)/trom.o

$(FRC):

########
#
#	All dependencies and rules not explicitly stated
#	(including header and nested header dependencies)
#
########

bootrom.o:	bootrom.s
	$(MCC) $(MCFLAGS) -c bootrom.s

bram.o:	bram.c
bram.o:	$(INC)/setup.h $(INC)/keycodes.h $(INC)/dmd.h $(INC)/font.h $(INC)/sys/2681.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c bram.c

link.o:	link.c
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c link.c
	@echo "Premature run through loader: need .bss section"
	$(MLD) -r -a -o link.tmp link.o	
	mv link.tmp link.o

load.o:	load.s
	$(MCC) $(MCFLAGS) -c load.s

lowmem.o:	lowmem.c
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c lowmem.c

pcbs.o:	pcbs.c
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c pcbs.c
	@echo "Premature run through loader: need .bss section"
	$(MLD) -r -a -o pcbs.tmp pcbs.o	
	@echo "Expect Undefined Symbols and loader warning"
	mv pcbs.tmp pcbs.o

trom.o:		trom.s
selftest.o:	$(INC)/duart.h
selftest.o:	$(INC)/font.h
selftest.o:	$(INC)/dmd.h
selftest.o:	selftest.c
selftest.o:	$(INC)/selftest.h
selftest.o:	$(INC)/setup.h
selftest.o:	$(INC)/scc.h
selftest.o:	$(INC)/keycodes.h # nested include from setup.h
selftest.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c selftest.c

vector.o:	vector.s
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c vector.s

vitty.o:	$(INC)/font.h
vitty.o:	$(INC)/dmd.h
vitty.o:	$(INC)/kbd.h
vitty.o:	$(INC)/queue.h
vitty.o:	$(INC)/setup.h
vitty.o:	$(INC)/keycodes.h # nested include from setup.h
vitty.o:	$(INC)/sys/2681.h # nested include from dmd.h
vitty.o:	vitty.c
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c vitty.c

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
