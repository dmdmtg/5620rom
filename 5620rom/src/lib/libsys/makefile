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
#	libsys makefile
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
	acia.o\
	buttons.o\
	cursblt.o\
	cursor.o\
	excep.o\
	kbd.o\
	logio.o\
	pf.o\
	queue.o\
	sccops.o\
	setup.o\
	wait.o

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

TITLE = libsys makefile
PRODUCTS = libsys.a l.o

MAKEARG= -$(MAKEFLAGS) DMDBIN=$(DMDBIN) DMDLIB=$(DMDLIB) DMDSRC=$(DMDSRC)\
	DMDINCLUDE=$(DMDINCLUDE) DMDSGS=$(DMDSGS) DMDDEMO=$(DMDDEMO)\
	MCC=$(MCC) MCFLAGS=$(MCFLAGS) MLD=$(MLD) MAR=$(MAR)

LCLCFLAGS=-J -g -O 
CLEAN=$(OBJ) libsys.a l.o

OBJ=acia.o buttons.o cursblt.o cursor.o excep.o kbd.o logio.o queue.o wait.o\
	sccops.o setup.o pf.o
CFILES=acia.c buttons.c cursor.c excep.c kbd.c logio.c queue.c wait.c\
	sccops.c setup.c pf.c

.c.o:
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c $<

.s.o:
	$(MCC) -c $<

all:	libsys.a l.o

libsys.a: $(OBJ)
	$(MAR) cr libsys.a `$(MLORDER) $(OBJ) | tsort`

save:
	-cp $(DMDLIB)/libsys.a olibsys.a

install: libsys.a l.o
	cp libsys.a $(DMDLIB)/libsys.a
	cp l.o $(DMDLIB)/l.o

########
#
#	All dependencies and rules not explicitly stated
#	(including header and nested header dependencies)
#
########

acia.o:	acia.c
acia.o:	$(INC)/dmd.h
acia.o:	$(INC)/sa.h
acia.o:	$(INC)/queue.h
acia.o:	$(INC)/setup.h
acia.o:	$(INC)/keycodes.h	# nested include from setup.h
acia.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c acia.c

buttons.o:	buttons.c
buttons.o:	$(INC)/dmd.h
buttons.o:	$(INC)/sa.h
buttons.o:	$(INC)/setup.h
buttons.o:	$(INC)/keycodes.h
buttons.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c buttons.c

cursblt.o:	cursblt.s
	$(MCC) -c cursblt.s

cursor.o:	cursor.c
cursor.o:	$(INC)/cursor.h
cursor.o:	$(INC)/dmd.h
cursor.o:	$(INC)/sa.h
cursor.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c cursor.c

excep.o:	excep.c
excep.o:	$(INC)/font.h
excep.o:	$(INC)/dmd.h
excep.o:	$(INC)/sa.h
excep.o:	$(INC)/kbd.h
excep.o:	$(INC)/setup.h
excep.o:	$(INC)/keycodes.h	# nested include from setup.h
excep.o:	$(INC)/sys/2681.h
	$(MCC) $(MCFLAGS) -J -g -c excep.c 	# no optimization

kbd.o:	$(INC)/msgs.h
kbd.o:	$(INC)/dmd.h
kbd.o:	$(INC)/sa.h
kbd.o:	kbd.c
kbd.o:	$(INC)/kbd.h
kbd.o:	$(INC)/queue.h
kbd.o:	$(INC)/setup.h
kbd.o:	$(INC)/keycodes.h	# nested include from setup.h
kbd.o:	$(INC)/sys/2681.h # nested include from dmd.h
kbd.o:	$(INC)/font.h 
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c kbd.c

logio.o:	logio.c
logio.o:	$(INC)/dmd.h
logio.o:	$(INC)/sys/2681.h # nested include from dmd.h
logio.o:	$(INC)/queue.h
logio.o:	$(INC)/setup.h
logio.o:	$(INC)/keycodes.h	# nested include from setup.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -DROMTERM -c logio.c

l.o:	l.s
	$(MCC) -c l.s

pf.o:	$(INC)/font.h
pf.o:	$(INC)/dmd.h
pf.o:	$(INC)/sa.h
pf.o:	$(INC)/kbd.h
pf.o:	pf.c
pf.o:	$(INC)/pf.h
pf.o:	$(INC)/setup.h
pf.o:	$(INC)/keycodes.h	# nested include from setup.h
pf.o:	$(INC)/options.h
pf.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c pf.c

queue.o:	queue.c
queue.o:	$(INC)/sa.h
queue.o:	$(INC)/queue.h
queue.o:	$(INC)/setup.h
queue.o:	$(INC)/keycodes.h	# nested include from setup.h
	$(MCC) $(MCFLAGS) -J -g -c queue.c 	# no optimization

sccops.o:	sccops.c
sccops.o:	$(INC)/dmd.h
sccops.o:	$(INC)/sa.h
sccops.o:	$(INC)/sys/2681.h
sccops.o:	$(INC)/scc.h
sccops.o:	$(INC)/queue.h
sccops.o:	$(INC)/setup.h
sccops.o:	$(INC)/keycodes.h	# nested include from setup.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c sccops.c

setup.o:	$(INC)/font.h
setup.o:	$(INC)/dmd.h
setup.o:	$(INC)/sa.h
setup.o:	setup.c
setup.o:	$(INC)/setup.h
setup.o:	$(INC)/keycodes.h	# nested include from setup.h
setup.o:	$(INC)/options.h
setup.o:	$(INC)/cursor.h
setup.o:	$(INC)/queue.h
setup.o:	$(INC)/kbd.h
setup.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c setup.c

wait.o:	$(INC)/dmd.h
wait.o:	$(INC)/sa.h
wait.o:	$(INC)/queue.h
wait.o:	$(INC)/sys/2681.h # nested include from dmd.h
wait.o:	wait.c
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c wait.c

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
