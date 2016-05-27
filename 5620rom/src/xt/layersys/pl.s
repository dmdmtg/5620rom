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
	.file	"pl.c"
	.set	MSTKZ,(4*150)		# number of bytes in mainstk
	.set	Firm, 0x200		# Address of Firmware jump table
	.text
	.globl	_start
#
# This file is the ROM version of the layers startup routine and
# interrupt handlers.  Since initial exception and interrupt handler
# PCs are named directly, these cannot be patched by downloading
# but must be patched by a downloaded startup routine.
#

_start: MOVAW	mainstk,%sp		# set the stack pointer (in .bss)
	MOVAW	mainstk,main_pcb+8	# plug mainstk into main_pcb
	MOVW	%sp,main_pcb+24		# lower stack bound
	ADDW3	&(MSTKZ-4),%sp,main_pcb+28 # upper stack bound
	MOVAW	excep_proc, proc_pcb+4	# process exception handler PC
	MOVAW	excep_stack, stack_pcb+4 # stack exception handler PC
	MOVAW	host_int, host_pcb+4
	MOVAW	out_int, out_pcb+4
	MOVAW	piohint, pioh_pcb+4
	MOVAW	key_int, key_pcb+4
	MOVAW	msvid_int, msvid_pcb+4
	MOVAW	excep_int, piol_pcb+4
	MOVAW	excep_int, int_pcb+4
	MOVAW	excep_tab+4, %r1	# &normal exception handler table[0].PC
	MOVB	&16, %r2		# 16 entries in normal excep tab
	MOVAW	excep_norm, %r3		# filled in with excep_norm
back:	MOVW	%r3, 0(%r1)		# copy, copy, copy...
	ADDW2	&8, %r1			# point to next entry
	DECB	%r2			# done?
	BNEB	back
	call	&0, *(Sys+(4*122))	# main through vector table
	RETPS

	.text
	.globl	msvid_int
msvid_int:
	SAVE	%r8
	MOVB	0x200013,%r5
	BITB	&0x40,%r5
	BEB	mous_chk
	BITB	&0x04,%r5	# update only at 60 hz. rate
	BNEB	mous_chk	# if fall through, have	a video	interrupt
	call	&0,*(Firm+(4*8))	# auto1 - calls	cursor update in cursor.c
	call	&0,*(Sys+(4*96))	# clockroutine  through table
mous_chk:
	BITB	&0xb0,%r5
	BEB	msvid_exit
				# if fall through, have	a mouse	interrupt
	call	&0,*(Firm+(4*10))	# auto4 - call buttons handler in buttons.c
	pushw	&1			# 1 is control
	call	&1,*(Sys+(4*166))	# sysrun - make control runnable
	MOVH	mouse+8,%r3	# get the updated button setting
	MOVW	kbdproc,%r4	# get kbdproc
	BEB	msvid_exit	# skip ahead if there is no kbdproc
	ANDW3	&0x28,0x50(%r4),%r2	# and the state and MOUSELOCAL|USER
	CMPW	&0x28,%r2	# test if MOUSELOCAL|USER are set
	BNEB	msvid_exit	# if not, don't update
	MOVW	0x48(%r4),%r4	# get the kbdproc's data field
	ANDH2	%r3,0x1c(%r4)	# update kbrproc's mouse.bouttons released
msvid_exit:
	RESTORE	%r8
	RETPS

	.text
	.globl	key_int
key_int:
	call	&0,*(Firm+(4*9))	# auto2
	RETPS

	.text
	.globl	host_int
host_int:
	call	&0,*Firm+(4*217)	# aciarxint in libsys/acia.c
	RETPS

	.text
	.globl	out_int
out_int:
	call	&0,*(Firm+(4*3))	# aciatrint
	RETPS

	.data
#	.globl	ticks0
#	.bss	ticks0,4,4		# defined in vector.s

	.text
	.globl	realtime
realtime: movw	ticks0,%r0
	RET
