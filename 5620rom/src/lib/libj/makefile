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
#	libj makefile
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
	.C.o\
	.c.o\
	.s.o\
	addr.o\
	alloc.o\
	arith.o\
	balloc.o\
	bitblt.o\
	clip.o\
	defont.o\
	gcalloc.o\
	icons.o\
	jlineto.o\
	jpoint.o\
	jstring.o\
	jtexture.o\
	line.o\
	masks.o\
	menu.o\
	min.o\
	point.o\
	polyf.o\
	ptinrect.o\
	rect.o\
	rectXrect.o\
	rectclip.o\
	rectf.o\
	screenswap.o\
	string.o\
	strwidth.o\
	texture.o

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


TITLE = libj makefile
PRODUCTS = libj.a


MAKEARG= -$(MAKEFLAGS) 

MLORDER=m32lorder
LCLCFLAGS=-J -g -O
CLEAN= $(OBJ) libj.a


CC_CMD =	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c

OBJ=gcalloc.o icons.o jpoint.o jstring.o menu.o balloc.o min.o point.o ptinrect.o alloc.o \
    rectXrect.o rectf.o jtexture.o rectclip.o screenswap.o string.o bitblt.o \
    strwidth.o defont.o texture.o jlineto.o addr.o masks.o line.o arith.o \
    clip.o rect.o  polyf.o

CFILES= alloc.c balloc.c bitblt.c clip.c gcalloc.c icons.c jlineto.c jpoint.c \
	jstring.c jtexture.c line.c masks.c menu.c min.c point.c ptinrect.c \
	rectXrect.c rectclip.c rectf.c screenswap.c string.c \
	strwidth.c defont.c texture.c width.c rect.s  polyf.c

.C.o:
	echo ".C files no longer built"

.c.o:
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c $<

.s.o:
	$(MCC) -c $<

libj.a: $(OBJ)
	$(MAR) cr libj.a `$(MLORDER) $(OBJ) | tsort`
	$(MAR) ma clip.o libj.a line.o

texture.o:	texture.s
	$(MCC) $(MCFLAGS) -g -c texture.s

min.o: 	min.s
	$(MCC) -c min.s

arith.o: arith.s
	$(MCC) -c arith.s

rect.o: rect.s
	$(MCC) -c rect.s

addr.o: addr.s
	$(MCC) -c addr.s

save:
	-cp $(DMDLIB)/libj.a olibj.a

install:libj.a
	cp libj.a $(DMDLIB)/libj.a


########
#
#	All dependencies and rules not explicitly stated
#	(including header and nested header dependencies)
#
########

alloc.o:	$(INC)/sa.h
alloc.o:	$(INC)/dmd.h
alloc.o:	$(INC)/sa.h
alloc.o:	$(INC)/sys/2681.h # nested include from dmd.h
alloc.o:	alloc.c
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c alloc.c

balloc.o:	balloc.c
balloc.o:	$(INC)/dmd.h
balloc.o:	$(INC)/sa.h
balloc.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c balloc.c

bitblt.o:	$(INC)/dmd.h
bitblt.o:	$(INC)/sa.h
bitblt.o:	$(INC)/sys/2681.h # nested include from dmd.h
bitblt.o:	bitblt.c
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c bitblt.c


clip.o:	clip.c
clip.o:	$(INC)/dmd.h
clip.o:	$(INC)/sa.h
clip.o:	$(INC)/line.h
clip.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c clip.c

defont.o:	defont.c
defont.o:	$(INC)/font.h
defont.o:	$(INC)/dmd.h
defont.o:	$(INC)/sa.h
defont.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c defont.c

gcalloc.o:	gcalloc.c
gcalloc.o:	$(INC)/dmd.h
gcalloc.o:	$(INC)/sa.h
gcalloc.o:	$(INC)/setup.h
gcalloc.o:	$(INC)/keycodes.h # nested include from setup.h
gcalloc.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c gcalloc.c

icons.o:	icons.c
icons.o:	$(INC)/dmd.h
icons.o:	$(INC)/sys/2681.h
icons.o:	$(INC)/sa.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c icons.c

jlineto.o:	$(INC)/dmd.h
jlineto.o:	$(INC)/sa.h
jlineto.o:	jlineto.c
jlineto.o:	$(INC)/line.h
jlineto.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c jlineto.c

jpoint.o:	$(INC)/dmd.h
jpoint.o:	$(INC)/sa.h
jpoint.o:	jpoint.c
jpoint.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c jpoint.c

jstring.o:	$(INC)/font.h
jstring.o:	$(INC)/dmd.h
jstring.o:	$(INC)/sa.h
jstring.o:	jstring.c
jstring.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c jstring.c

jtexture.o:	$(INC)/dmd.h
jtexture.o:	$(INC)/sa.h
jtexture.o:	jtexture.c
jtexture.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c jtexture.c

line.o:	$(INC)/dmd.h
line.o:	$(INC)/sa.h
line.o:	line.c
line.o:	$(INC)/line.h
line.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c line.c

masks.o:	masks.c
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c masks.c

menu.o:	$(INC)/font.h
menu.o:	$(INC)/dmd.h
menu.o:	$(INC)/sa.h
menu.o:	menu.c
menu.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c menu.c

point.o:	$(INC)/dmd.h
point.o:	$(INC)/sa.h
point.o:	point.c
point.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c point.c

polyf.o:	$(INC)/dmd.h
polyf.o:	$(INC)/sa.h
polyf.o:	polyf.c
polyf.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c polyf.c

ptinrect.o:	$(INC)/dmd.h
ptinrect.o:	$(INC)/sa.h
ptinrect.o:	ptinrect.c
ptinrect.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c ptinrect.c

rectXrect.o:	$(INC)/dmd.h
rectXrect.o:	$(INC)/sa.h
rectXrect.o:	rectXrect.c
rectXrect.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c rectXrect.c

rectclip.o:	$(INC)/dmd.h
rectclip.o:	$(INC)/sa.h
rectclip.o:	rectclip.c
rectclip.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c rectclip.c

rectf.o:	$(INC)/dmd.h
rectf.o:	$(INC)/sa.h
rectf.o:	rectf.c
rectf.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c rectf.c

screenswap.o:	$(INC)/dmd.h
screenswap.o:	$(INC)/sa.h
screenswap.o:	screenswap.c
screenswap.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c screenswap.c

string.o:	$(INC)/font.h
string.o:	$(INC)/dmd.h
string.o:	$(INC)/sa.h
string.o:	string.c
string.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c string.c

strwidth.o:	$(INC)/font.h
strwidth.o:	$(INC)/dmd.h
strwidth.o:	$(INC)/sa.h
strwidth.o:	strwidth.c
strwidth.o:	$(INC)/sys/2681.h # nested include from dmd.h
	$(MCC) $(MCFLAGS) $(LCLCFLAGS) -c strwidth.c

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
