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
#	roms makefile
#
########

CLEAN =\
	aconv \
	combined.000 \
	combined.001 \
	combined.100 \
	combined.101 \
	combined.200 \
	combined.201 \
	combined.300 \
	combined.301


roms:	aconv ../combined
	./aconv -w 4 -m 20000  -p 16 ../combined

aconv:  aconv.c
	cc -O -s -I$(DMD)/include/sgs aconv.c -o aconv

clean:
	rm -f $(CLEAN)
