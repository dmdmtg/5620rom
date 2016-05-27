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
	.globl	excep_norm
excep_norm:
	ORW2	&0x281e000,%psw		# IPL up to 15
	NOP			# let the bits settle
	TSTW	%r0		# let psw bits settle 
	PUSHW	%r0		# save temporary registers
	PUSHW	%r1		# save temporary registers
	PUSHW	%r2		# save temporary registers
	PUSHW	&3		# type = NORMAL
	PUSHAW	-24(%sp)	# push address of saved pc -(24) 
	PUSHAW	-24(%sp)	# push address of saved psw -24 off of new stack
	PUSHW	-28(%sp)	# push saved psw -28 off of new stack (PUSH)
	PUSHW	%pcbp		# push saved pcbp (not used!)
	call	&5,*(Sys+(4*153))	# /* trap */trap will decide to return or not
	POPW	%r2		#restore temporary registers
	POPW	%r1		#restore temporary registers
	POPW	%r0		#restore temporary registers
	RETG			# resume of trap allows (brkpt or trace )




	.globl	excep_stack
excep_stack:
	MOVW	-0x4(%isp),%r0	# get saved %pcbp 
	PUSHW	&2		# type = STACK
	PUSHAW	0x4(%r0)	# push address of saved pc 
	PUSHAW	0x0(%r0)	# push address of saved psw 
	PUSHW	0x0(%r0)	# push saved psw 
	PUSHW	%r0		# push saved pcbp 
	call	&5,*(Sys+(4*153))	# /* trap */ let trap handle the situation
	RETPS			# if trap returns

	.globl	excep_int
excep_int:
	PUSHW	&4		# type = STRAY INTERRUPT
	JMP	com

	.globl	excep_proc
excep_proc:
	PUSHW	&1		# type = PROCESS
com:	PUSHW	&0		# push address of saved pc  (NONE!)
	PUSHW	&0		# push address of saved psw (NONE!)
	PUSHW	%psw		# push psw of current proc!!!
	PUSHW	-0x4(%isp)	# push saved %pcbp 
	call	&5,*(Sys+(4*153))	# /* trap */ let trap handle the situation
	RETPS			# if trap returns
