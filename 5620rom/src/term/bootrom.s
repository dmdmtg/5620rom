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
#
#	ROM startup code
#
#	set up some hardware dependent names
#
	.set	BAUD_VAL,0x600002	# BRAM address of baud speed index
	.set	IOCARD,0x6006f2		# I/O Card present flag
	.set	VALPHOST,0x6006e2	# physical index of logical host port
	.set	_ENCODEFLAG, 0x6006c6	# encoding flag
	.set	MAXADDR, 0x600686	# Size of memory
	.set	_RAMSPLIT, 0x600704	# percentage of RAM for alloc vs. gcalloc
	.set	_ramstrt,0x700000	# start	of display ram
	.set	_ramend,0x740000 - 4	# end of display ram
	.set	pcbsize,20
	.set	ERR_RCV,0x71
	.set	XMTEMT,0x0c
	.set	ERR_FLG,0x70
	.set	FLG_CLR,0x40
	.set	EXT_RESET_MASK,0x3
	.set	_hostcmd,0x20000b
	.set	_hostdata,0x20000f
	.set	_hoststat,0x200007
	.set	_DADDR,0x500000
	.set	DELAYC,0x3677d		# count for initial delay loop
#
	.globl	display			# The display bitmap in	rom at last!
	.data
display:
	.word	_ramstrt		# pointer to the base of the display
	.word	25			# width	in 32 bit words
	.word	16:0,16:0		# rect.origin.x, rect.origin.y
	.word	16:800,16:1024		# rect.corner.x, rect.corner.y
	.word	0			# _null

	.text

	.globl	_start
_start:			# _start is called on reset
	LRSW3	&3,%psw,%r8
	MOVAW	istack,%isp		# initialize the interrupt stack
	ORW2	&0x2800000, %psw	# disable cache for 32b
	NOP				# let the bits settle

	MOVW	&DELAYC, %r1	# setup for delay until 32 Mhz timer
	NOP
hang10:	DECW	%r1		# comes up.   Zzzzzzzzzzzzz
	NOP			# (only needed after power-on reset,
	BGUB	hang10		# but doesn't hurt otherwise)

	call	&0,checkbram	# make sure bram isn't garbage
	ANDW2	&0xf,%r8
	CMPW	&EXT_RESET_MASK,%r8
	BNEB	no_diags	# if not an external reset, skip diags
	
# Start of Power-On Only stuff...
	
	call	&0,probe_io	# test for i/o card (in logio.c)
	PUSHW	&0		# do power-up self test
	call	&1,test32	# Note: this will size RAM

# End of power-on specific stuff.  This is done on ALL resets

no_diags:
	CLRB	0x60067a	# VALNUM = 0 - not NUM LOCK;
	CLRB	_RAMSPLIT	# reset partitioning of RAM to 50/50
	CLRB	0x600682	# VALDWNLDFLAG = 0 - running resident terminal
	CLRB	0x6006c6	# ENCODE_ENABLE = 0 - non encoded data;
	call	&0,setbram
	MOVW	&0x281e100,%psw		# make sure all	interrupts are off
	NOP				# let the bits settle
	MOVW	&_ramstrt,%r0
	LLSB3   &2,$MAXADDR, %r2	# memory size index from test32 
	MOVW	maxaddr(%r2),%r1	# last valid memory address
	.align	4			# align	the branch back	address
clear:					# to gain speed.
	CLRW	0(%r0)			# clear	the display ram	from top
	addw2	&4,%r0			# to bottom also clearing the
	cmpw	%r0,%r1			# bss variables	stored in the
	BNEB	clear			# display ram.
	JSB	setvectors		# copy PCBS to RAM

setpcbp:
	movaw	rreset_pcb+12,%pcbp	# move the pcbp	to point at ram

#
	TSTB	IOCARD			# Need I/O Card initialization?
	BEB	go_main			# Nope.
	call	&0,sccinit
go_main:
	call	&0,logports		# Logical I/O stuff initialization
	call	&0,main			# call vitty (main) and	don't come back
					# 'till you want to download
#
# Here to begin a download....
#
	.globl	sald
sald:
	MOVW	&0x281e100,%psw		# make sure all	interrupts are off
	NOP				# let the bits settle
	ANDW3	&0xffff,%r0,%r8		# save return value from vitty for later
	ANDW3	&0x0fff0000,%r0,%r3	# save return value from vitty for later
	ROTW	&0x10,%r3,%r3		# type of download in r3 (binary or hex)
	MOVB	&0x0,_ENCODEFLAG	# reset BRAM encoding flag
	CMPB	&2,%r3			# 2 - enable encoding for after download
	BNEB	dld1
	MOVB	&0x1,_ENCODEFLAG	# set BRAM encoding flag
dld1:	PUSHW	%r3			# save both flags
	PUSHW	%r8
	call	&0, setbram		# and redo checksum
	POPW	%r8			# restore flags to register
	POPW	%r3

	MOVB	&0x2,0x20003f		# set to white bits on black bckgrd
	MOVW	&_ramstrt,%r0		# clearing from start of RAM through all 
	SUBW3	%r0,&msvidstk,%r1
	ADDW2	&0x14b0,%r1		# add the length of the stacks (msvid+pio+main)
	ARSW3	&2,%r1,%r1		# get it in terms of words, not bytes
	MOVH	%r1,_DADDR		# point	display	at base so we can watch
	LLSB3   &2,$0x600686,%r2	# UGH! ICK! UGH! hard path to variable!
	MOVW	maxaddr(%r2),%r1	# last valid memory address
	.align	4			# align	the branch back	address
_clr:					# to gain speed.
	CLRW	0(%r0)			# clear	the display ram	from top
	addw2	&4,%r0			# to bottom also clearing the
	cmpw	%r0,%r1			# bss variables	stored in the
	BNEB	_clr			# display ram.
#
	call	&0,logports		# reset RAM pointers to physical devices
#
# Handle some special things for ROM copy of layersys
#
	JSB	setvectors		# copy pcbs up, this includes setup's
	PUSHW	%r3
	TSTW	%r8			# Are we doing a plain vanilla download?
	BEH	dld			# yup.
	MOVW	&_ramstrt,%r0		# start of ram
	SUBW3	%r0,&0x722c94,%r1	# start address of windowproc
	ARSW3	&2, %r1, %r1		# get in terms of words not bytes
	MOVH	%r1,_DADDR		# change display pointer to beginning
	MOVW	pvtab,%r1		# r1 = pointer to RAM table
	NOP
	MOVW	lenvtab,%r2		# r2 = length in bytes
	ARSW3	&2,%r2,%r2		# get it in words.
	NOP
	MOVW	rvtab,%r0		# r0 = pointer to ROM copy
	NOP
	MOVBLW				# move it.
	NOP

	MOVW	lsys_start, rmain_pcb+4	# store starting PC
	CMPW	%r8,&1			# 1 means still got something to load
	BNEB	gogo			# running from ROM -- take off.
#
# Here to actually start up a stand-alone download
#	

dld:
					# Set video like v1.1 roms
	TSTB    0x600016		# if(VALSCREENCOLOR)
	BEB     dld2
	MOVB    &0x2,0x20003f		# BonW();
	NOP     
	BRB     dld3
dld2:
	MOVB    &0x2,0x20003b		# WonB();
	NOP     
dld3:
	PUSHW	&0x61
	MOVW	hostops,%r0
	call	&1,*0x10(%r0)		# (*hostops->xpoll)('a');
	POPW	%r3
	call	&0,load			# download the new program and return
					# start	address	in r0
	TSTW	%r0
	je	reboot			# if zero then reboot
gogo:	movaw	rmain_pcb,%r0		# address of the process control block
	CALLPS				# switch out to	the new	process

	.globl	reboot
reboot:
exit:
	movaw	reset_pcb,%r0		# when return then reboot
	CALLPS				# reboot the entire system
	jmp	reboot			#  boo-boo if you get here

#
# setvectors -	called during initialization and before booting layersys to
#		copy PCBs from ROM to RAM.
#
# This is a subroutine, not a procedure! (use JSB, not CALL)
#
setvectors:				# copy the interrupt pcb's from	rom to ram
	MOVAW	init_pcb,%r0		# r0 = beginning pcb blocks
vectors:
	MOVW	0(%r0),%r2		# r2 = number of words to move
	REQLU				# end loop on zero count
	addw2	&4,%r0			# bump r0 to point to source address
	MOVW	0(%r0),%r1		# r1 = destination address
	addw2	&4,%r0			# bump r0 to point to first work to move
	MOVBLW				# move the block
	BRB	vectors			# do the next one
#
#	interrupt handlers
#
	.globl	key_int
key_int:
	call	&0,auto2		# call keyboard	handler	in kbd.c
	RETPS

	.globl	msvid_int
msvid_int:
	pushw	&introutine	  # romterm introutine 
	call	&1, msvd_hndl
	RETPS

	.globl	msvd_hndl
# This handler will be used by l.s and bootrom.s
# In roms because, possible compatibility problems with 5625
msvd_hndl:
	MOVB	0x200013,%r5
	BITB	&0x40,%r5
	BEB	mous_chk
	BITB	&0x04,%r5	# only update at 60hz
	BNEB	mous_chk	# if fall through, have	a video	interrupt
	call	&0,auto1	# calls	cursor update in cursor.c
	call	&0,*0(%ap)	# calls	user provided introutine
mous_chk:
	BITB	&0xb0,%r5
	BEB	msvid_exit
				# if fall through, have	a mouse	interrupt
	call	&0,auto4	# call buttons handler in buttons.c
msvid_exit:
	RET			# return from subroutine call

	.globl	host_int
host_int:
	call	&0,aciarxint	# receive interrupt handler in libsys/acia.c
	RETPS

	.globl	out_int
out_int:
	call	&0,aciatrint		# call the xmit	handler	in acia.c
	RETPS

#
#	Putc: pump out a character to the host.
#
	.globl	aciaputc
aciaputc:
	call	&0,aciapoke		# enable the transmitter
	MOVB	&0,_hostcmd		# toggle r/w lead
	BITB	&XMTEMT,_hoststat	# ready	for next byte?
	BEB	aciaputc			# no; loop
	MOVB	3(%ap),_hostdata		# transmit
	RET				# return from subroutine call
#
#	Getc: receive character	from host.
#		result	is in %r0
#		return	-1 if error bit
	.globl	aciagetc
aciagetc:
	MOVB	&0,_hostcmd		# toggle r/w lead
	movb	_hoststat, %r0
	BITB	&ERR_RCV,%r0		# error	or data?
	BEB	aciagetc			# neither; loop
	BITB	&ERR_FLG,%r0		# error?
	BEB	getcOK
	MOVB	_hostdata,%r0		# read data to reset
	MOVB	&FLG_CLR,_hostcmd	#clear the error flags
	movw	&-1, %r0		# return -1 for	error
	RET
getcOK:
	MOVB	&0,_hostcmd		# toggle r/w lead
	MOVB	_hostdata,{uword}%r0	# read data
	RET
#
# Set IPL stuff.  These routines correspond roughly to the ones used
# in the PDP-11 UNIX kernel.  If you don't know about those, don't worry.
#
#	spln n=0,1,4,5,6,7 sets processor level to "n".  0 is low, 7 is high.
#		spl0		0 (all interrupts allowable - normal level)
#		spl1,spl4	14 (only level 15 interrupts allowed - PIOH, EIA)
#		spl5,spl6,spl7	15 (all interrupts disabled)
#	splx(n)	(n should be 0,14, or 15) sets IPL to n.
#
# All return previous PSW, so you can do:
#
#	int	i;
#	i=spl7();	/* i gets current PSW, IPL becomes 15 */
#	   ..		/* important stuff here */
#	splx(i);	/* restore previous level */
#
	.globl	spl0
spl0:	movw	%psw,%r0
	ANDW2	&0xfffe1fff,%psw	# turn off all priority bits
	NOP
	RET

	.globl	spl1
	.globl	spl4
spl1:
spl4:	movw	%psw,%r0
	INSFW	&4,&13,&14,%psw
	NOP
	RET

	.globl	spl5
	.globl	spl6
	.globl	spl7
spl5:
spl6:
spl7:	movw	%psw,%r0
	ORW2	&0x281e000,%psw
	NOP
	RET
	.globl	splx
splx:
	movw	%psw,%r0
	LRSW3	&13,0(%ap),%r1
	INSFW	&4,&13,%r1,%psw
	NOP
	RET
