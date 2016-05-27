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
	.file	"load.c"
	.data
	.align	4
	.def	crc16t_32;	.val	crc16t_32;	.scl	2;	.type	0375;	.dim	2,16;	.size	64;	.endef
	.globl	crc16t_32
crc16t_32:
	.word	16:0,16:49345
	.word	16:49537,16:320
	.word	16:49921,16:960
	.word	16:640,16:49729
	.word	16:50689,16:1728
	.word	16:1920,16:51009
	.word	16:1280,16:50625
	.word	16:50305,16:1088
	.word	16:0,16:52225
	.word	16:55297,16:5120
	.word	16:61441,16:15360
	.word	16:10240,16:58369
	.word	16:40961,16:27648
	.word	16:30720,16:46081
	.word	16:20480,16:39937
	.word	16:34817,16:17408
	.text
					# registers used are: 
					# r8 sequence counter
					# r7 pointer to packet on the stack
					# r6 crc accumulator
					# r5 total number of bytes in data sec.
					# r4 number of bytes written to mem.
					# r3 download flag 0=binary 1=hex
	.globl	load
load:
	SAVE	%r3
	addw2	&132,%sp		# make room for 132 byte packet on stack
	movb	&0x80,%r8		# init the sequence register
nextpkt:
#
#	state 0, read packet header
#
	BSBH	Ngetc			# go read the first byte of packet
	cmpw	%r0,%r8			# correct sequence ???
	BNEB	nextpkt			# no, go back
	movw	%fp,%r7			# init pointer to packet on the stack
	CLRW	%r6			# clear the crc acc.
	BSBH	crcbyte			# compute crc on each byte
#
#	state 1, read size of packet
#
	BSBH	Ngetc
	subw3	&4,%r0,%r4		# r4 number of bytes - 4
	BLB	nextpkt			# if less than 4 then error
	cmpw	%r0,&132		# if more than 132 then error
	BGB	nextpkt
	movw	%r0,%r5			# the number of bytes to be received
	BSBH	crcbyte			# compute crc on each byte
#
#	state 2, read data bytes in packet
#
datard:
	BSBH	Ngetc
	TSTW	%r0
	BLB	nextpkt			#if getc returns -1 then error
	movb	%r0,0(%r7)		# capture packet on the stack
	INCW	%r7
	BSBH	crcbyte			# compute crc on each byte
	DECW	%r5			# keep reading until all are read
	BGB	datard
#
#	state 3, check incomming crc against computed crc low byte
#
	BSBH	Ngetc
	TSTW	%r0			# getc error?
	jl	nextpkt			# yes, go back
	cmpb	%r0,%r6			# low byte of crc
	jne	nextpkt			# bad ?
#
#	state 4, check incomming crc against computed crc high byte
#
	BSBH	Ngetc
	TSTW	%r0			# getc error?
	jl	nextpkt			# yes, go back
	lrsw3	&8,%r6,%r1
	cmpb	%r0,%r1			# high byte of crc
	jne	nextpkt			# bad ??
#
#	state 5, good packet received, if four bytes long, then starting
#		address of program and return. If greater, copy the
#		data to the specified address
#
	TSTW	%r4
	jne	datawr
	BSBH	Nputc
	BSBH	Nputc
	BSBH	Nputc
	movw	0(%fp),%r0
	RESTORE	%r3
	RET
datawr:
	movw	0(%fp),%r0		# destintion address at front of packet
	movaw	4(%fp),%r1		# pointer to data in packet
wrloop:	.align	4			# cause branch back to word aligned fast
	movb	0(%r1),0(%r0)		# copy data from source to dest
	cmpb	0(%r1),0(%r0)		# check to make sure it got there
	jne	nextpkt			# if not then stop now and don't ack
					# should this be fatal?  error message?
	INCW	%r1			# adjust pointers
	INCW	%r0
	DECW	%r4			# are we done yet??
	BGB	wrloop			# no, not yet.
	BSBH	Nputc
	INCW	%r8			# increment sequence to next number
	andw2	&63,%r8
	orw2	&128,%r8
	jmp	nextpkt			# go back and do it again
#
#	byte at a time crc calculation
#	note: this is a subroutine, not a procedure!!!!!
#
crcbyte:
	xorw3	%r0,%r6,%r1
	andw3	&15,%r1,%r2
	addw2	%r2,%r2
	lrsw3	&3,%r1,%r1
	andw2	&30,%r1
	xorh3	crc16t_32+32(%r1),crc16t_32(%r2),%r0
	lrsw3	&8,%r6,%r6
	andw2	&0xff,%r6
	xorw2	%r0,%r6
	RSB

#
# 	r3 - flag indicating type of download
#	note: this is a subroutine, not a procedure!!!!!
#
Ngetc:
	TSTW	%r3
	jne	Ngetc1
	call	&0,hostgetc		# binary getc
	RSB
Ngetc1:
	call	&0,hostgetc		# hex getc
	ANDW3	&0xf,%r0,%r1
	call	&0,hostgetc
	ANDW2	&0xf,%r0
	LLSW3	&0x4,%r0,%r0
	ORW2	%r1,%r0
	RSB

#
# 	r3 - flag indicating type of download
#	r8 - data to be sent
#	note: this is a subroutine, not a procedure!!!!!
#
Nputc:
	TSTW	%r3
	jne	Nputc1
	PUSHW	%r8
	call	&1,hostputc
	RSB
Nputc1:
	MOVW	%r8,%r0
	ANDW2	&0xf,%r0
	ORW2	&0x40,%r0
	PUSHW	%r0
	call	&1,hostputc
	MOVW	%r8,%r0
	LRSW3	&0x4,%r0,%r0
	ANDW2	&0xf,%r0
	ORW2	&0x40,%r0
	PUSHW	%r0
	call	&1,hostputc
	RSB
