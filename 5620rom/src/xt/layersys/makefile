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
#	layersys makefile
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
	/lib/crtm.o\
	agent.o\
	boot.o\
	bootmain.o\
	comm.o\
	control.o\
	controlio.o\
	crtm.o\
	demux.o\
	excep.o\
	exec.o\
	link.o\
	pcheck.o\
	pinit.o\
	pl.o\
	precv.o\
	psend.o\
	ptimeout.o\
	task.o\
	trap.o\
	user.o\
	windowproc.o\
	rvector.o \
	vector.o \
	lpcbs.o \
	mainstk.o

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


TITLE = layersys makefile
PRODUCTS = layersys

FRC=FRC


MAKEARG= -$(MAKEFLAGS) DMDBIN=$(DMDBIN) DMDLIB=$(DMDLIB) DMDSRC=$(DMDSRC)\
	DMDINCLUDE=$(DMDINCLUDE) DMDSGS=$(DMDSGS) DMDDEMO=$(DMDDEMO)\
	DMDFONT=$(DMDFONT) DMDICON=$(DMDICON)\
	MCC=$(MCC) MCFLAGS=$(MCFLAGS) MLD=$(MLD) MAR=$(MAR)

AUXSEARCH=../proto
MAS=$(DMDBIN)/bmcc
MNM=m32nm
LCLCFLAGS = -Uvax -J -O -g -DMPXTERM -I$(PDIR) -DBlit 
LCLCFLGS = -Uvax -J -g -DMPXTERM -I$(PDIR) -DBlit 	

LIBS= $(DMDLIB)/liblayer.a $(DMDLIB)/libsys.a $(DMDLIB)/libj.a $(DMDLIB)/libc.a

PDIR = ../proto
PHEADERS = $(PDIR)/packets.h $(PDIR)/proto.h $(PDIR)/pstats.h $(PDIR)/pconfig.h
PFILES = $(PDIR)/pcheck.c $(PDIR)/pinit.c \
	 $(PDIR)/precv.c $(PDIR)/psend.c $(PDIR)/ptimeout.c
POBJS = pcheck.o pinit.o precv.o psend.o ptimeout.o

TFILES = boot.c bootmain.c comm.c control.c demux.c exec.c link.s task.c trap.c user.c \
windowproc.c agent.c excep.s rvector.s vector.s controlio.c
TOBJS =  boot.o bootmain.o comm.o control.o demux.o exec.o link.o task.o trap.o user.o \
windowproc.o agent.o excep.o rvector.o vector.o controlio.o
CFILE = crtm.s
COBJ = crtm.o

OBJS = $(TOBJS) $(POBJS)

CLEAN=layersys $(OBJS) vector.s rvector.s lsym lpcbs.o mainstk.o pl.o combined layersys.map layersys.nm combined.map Map map crtm.o

.c.o:
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c $<
.s.o:
	$(MCC) -c -g $<
.s~.s:
	get $<

all:	Map layersys $(COBJ) combined

$(COBJ): crtm.s
	$(MCC) -c -g $<

map:	$(DMDSRC)/term/romterm.map Map fixmap mapprog $(DMDSRC)/term/romterm.nm
	./fixmap head $(DMDSRC)/term/romterm.map

Map:	makefile	
	echo	"lpcbs.o"					     "\n" \
		"MEMORY {"					     "\n" \
		'	rom(RXI):	org=0x0,	len=0x20000' "\n" \
		'	ram:		org=0x700000,	len=0x40000' "\n" \
		"}"						     "\n" \
		"SECTIONS {"					     "\n" \
		" GROUP TADDR : {"				     "\n" \
		"  .linkage : {link.o(.data)}"			     "\n" \
		"	.text :	{}"				     "\n" \
		"	.data  : {}"				     "\n" \
		'	} >rom'					     "\n" \
		" GROUP BADDR : {"				     "\n" \
		"	.pcbs :	{ lpcbs.o (.data) "		     "\n" \
		"		  lpcbs.o (.bss)  "		     "\n" \
		"		}"				     "\n" \
		"	.lvecs : { vector.o (.data) }"		     "\n" \
		"	.bss : {}"				     "\n" \
		"	.dummy : { Free_RAM = .;  }"				"\n" \
		"	}"					     "\n" \
		"}"						     > Map


combined: $(DMDSRC)/term/romterm comb-map layersys 
	cp layersys layersys.tmp
	cp $(DMDSRC)/term/romterm $(DMDSRC)/term/romterm.tmp
	m32strip  layersys $(DMDSRC)/term/romterm
	@echo "expect 3 warning messages from m32ld"
	m32ld -a -o combined -m comb-map >combined.map
	mv layersys.tmp layersys
	mv $(DMDSRC)/term/romterm.tmp $(DMDSRC)/term/romterm

$(FRC):
	

layersys: $(OBJS) pl.o $(DMDLIB)/libsys.a $(DMDLIB)/liblayer.a $(DMDLIB)/libj.a map lpcbs.o trom.o mainstk.o
	m32ld -L../../../lib -a -m -o layersys $(OBJS) trom.o map pl.o \
		-lsys -llayer -lj mainstk.o > layersys.map
	$(MNM) -e -x layersys > layersys.nm


pcheck.o:	$(PDIR)/pcheck.c vector.h
		$(MCC) $(MCFLAGS) $(LCLCFLAGS) -DROMCRC -c $(PDIR)/pcheck.c
pinit.o:	$(PDIR)/pinit.c $(PHEADERS) vector.h
		$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c $(PDIR)/pinit.c
precv.o:	$(PDIR)/precv.c $(PHEADERS) vector.h
		$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c $(PDIR)/precv.c
psend.o:	$(PDIR)/psend.c $(PHEADERS) vector.h
		$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c $(PDIR)/psend.c
ptimeout.o:	$(PDIR)/ptimeout.c $(PHEADERS) vector.h
		$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c $(PDIR)/ptimeout.c

save:
	-cp $(DMDLIB)/layersys olayersys

install:	lsym

lsym:	layersys
#	m32cprs layersys lsym		# taken out because of a bug in SGS
	cp layersys lsym		# this replaces the above line
	m32strip -l lsym

########
#
#	All dependencies and rules not explicitly stated
#	(including header and nested header dependencies)
#
########

agent.o:	agent.c
agent.o:	vector.h
agent.o:	$(INC)/agent.h
agent.o:	$(INC)/dmd.h
agent.o:	$(INC)/dmdproc.h
agent.o:	$(INC)/jioctl.h
agent.o:	$(INC)/layer.h
agent.o:	$(PDIR)/proto.h
agent.o:	$(INC)/queue.h
agent.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c agent.c

boot.o:	boot.c
boot.o:	vector.h
boot.o:	$(INC)/dmd.h
boot.o:	$(INC)/dmdproc.h
boot.o:	$(INC)/layer.h
boot.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c boot.c

bootmain.o:	bootmain.c
bootmain.o:	vector.h
bootmain.o:	$(INC)/dmd.h
bootmain.o:	$(INC)/dmdproc.h
bootmain.o:	$(INC)/layer.h
bootmain.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c bootmain.c

comm.o:	comm.c
comm.o:	vector.h
comm.o:	$(INC)/dmd.h
comm.o:	$(INC)/dmdproc.h
comm.o:	$(INC)/kbd.h
comm.o:	$(INC)/layer.h
comm.o:	$(INC)/queue.h
comm.o:	$(INC)/setup.h
comm.o:	$(INC)/keycodes.h # nested include from setup.h
comm.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c comm.c

control.o:	control.c
control.o:	vector.h
control.o:	$(INC)/dmd.h
control.o:	$(INC)/dmdproc.h
control.o:	$(INC)/layer.h
control.o:	$(INC)/msgs.h
control.o:	$(INC)/queue.h
control.o:	$(INC)/setup.h
control.o:	$(INC)/keycodes.h # nested include from setup.h
control.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c control.c

controlio.o:	controlio.c
controlio.o:	vector.h
controlio.o:	$(INC)/dmd.h
controlio.o:	$(INC)/dmdproc.h
controlio.o:	$(INC)/layer.h
controlio.o:	$(INC)/msgs.h
controlio.o:	$(PDIR)/packets.h
controlio.o:	$(PDIR)/pconfig.h
controlio.o:	$(PDIR)/proto.h
controlio.o:	$(INC)/queue.h
controlio.o:	$(INC)/setup.h
controlio.o:	$(INC)/keycodes.h # nested include from setup.h
controlio.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c controlio.c

demux.o:	demux.c
demux.o:	vector.h
demux.o:	$(INC)/dmd.h
demux.o:	$(INC)/dmdproc.h
demux.o:	$(INC)/layer.h
demux.o:	$(INC)/msgs.h
demux.o:	$(PDIR)/packets.h
demux.o:	$(PDIR)/pconfig.h
demux.o:	$(PDIR)/proto.h
demux.o:	$(INC)/queue.h
demux.o:	$(INC)/sys/2681.h # nested include from dmd.h
demux.o:	$(INC)/sys/jioctl.h
demux.o:	$(INC)/setup.h
demux.o:	$(INC)/keycodes.h # nested include from setup.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c demux.c

excep.o:	excep.s
	$(MCC) -c -g excep.s

exec.o:	exec.c
exec.o:	vector.h
exec.o:	$(INC)/dmd.h
exec.o:	$(INC)/dmdproc.h
exec.o:	$(INC)/layer.h
exec.o:	$(INC)/queue.h
exec.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c exec.c

link.o:	link.s
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c link.s

lpcbs.o: lpcbs.c
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c lpcbs.c
	@echo "Premature run through loader: need .bss section"
	$(MLD) -r -a -o lpcbs.tmp lpcbs.o
	@echo "Undefined symbols expected"
	mv lpcbs.tmp lpcbs.o

pl.o:	pl.s
	$(MCC) -c -g pl.s

task.o:	$(INC)/dmd.h
task.o:	$(INC)/dmdproc.h
task.o:	$(INC)/layer.h
task.o:	$(INC)/sys/2681.h # nested include from dmd.h
task.o:	task.c
task.o:	vector.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c task.c

trap.o:	$(INC)/font.h
trap.o:	$(INC)/dmd.h
trap.o:	$(INC)/dmdproc.h
trap.o:	$(INC)/layer.h
trap.o:	$(INC)/sys/2681.h # nested include from dmd.h
trap.o:	trap.c
trap.o:	vector.h
trap.o:	$(INC)/cursor.h
	$(MCC) $(MCFLAGS) $(LCLCFLGS) -c trap.c	  # No optimization

user.o:	$(INC)/font.h
user.o:	$(INC)/dmd.h
user.o:	$(INC)/dmdproc.h
user.o:	$(INC)/layer.h
user.o:	$(INC)/sys/2681.h # nested include from dmd.h
user.o:	user.c
user.o:	vector.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c user.c

vector.s:	vector_tab
		/lib/cpp -P -URVECTOR vector_tab >vector.s
vector.o:	vector.s
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c vector.s
rvector.s:	vector_tab
		/lib/cpp -P -DRVECTOR vector_tab >rvector.s
rvector.o:	rvector.s
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c rvector.s

windowproc.o:	$(INC)/font.h
windowproc.o:	$(INC)/dmd.h
windowproc.o:	$(INC)/dmdproc.h
windowproc.o:	$(INC)/kbd.h
windowproc.o:	$(INC)/layer.h
windowproc.o:	$(INC)/queue.h
windowproc.o:	$(INC)/setup.h
windowproc.o:	$(INC)/keycodes.h # nested include from setup.h
windowproc.o:	$(INC)/sys/2681.h # nested include from dmd.h
windowproc.o:	$(INC)/sa.h
windowproc.o:	windowproc.c
windowproc.o:	vector.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c windowproc.c

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


makefile_all:	makefile

install: 	# rules, if any, specified above
