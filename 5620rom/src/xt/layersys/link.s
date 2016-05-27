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

# Linkage with romterm (these are in section .linkage) 

	.data

	.word	Free_RAM	# int	ls_bss
	.word	_start		# int	lsys_start
	.word	Sys		# int	*pvtab
	.word	alenvtab	# int	lenvtab - absolute defined in vector_tab
	.word	RSys		# int	*rvtab
