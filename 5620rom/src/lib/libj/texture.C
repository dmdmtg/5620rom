/* */
/*									*/
/*	Copyright (c) 1985,1986,1987,1988,1989,1990,1991,1992   AT&T	*/
/*			All Rights Reserved				*/
/*									*/
/*	  THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T.		*/
/*	    The copyright notice above does not evidence any		*/
/*	   actual or intended publication of such source code.		*/
/*									*/
/* */
#include <jerq.h>

#undef texture
texture(bp, rec, map, f)
#define texture Stexture
Bitmap *bp;
register Texture *map;
Rectangle rec;
{
	register Word *screenp;		/* r7 */
	register Word offset;		/* r6 */
	register Word bits;		/* r5 */
	register int	i;		/* i is the inner loop count    r4 */
	register dy = rec.corner.y-rec.origin.y;	/* r3 */
	int	bitindex;
	Word mask1, mask2;
	int	ii;
#define X	offset			/* kluge to allow reuse of regs */
#define Y	bits
#define DX	i

	DX = rec.corner.x - rec.origin.x;
	if (DX <= 0)
		return;
	X = rec.origin.x;
	Y = rec.origin.y;
	mask1 = ~topbits[(X & WORDMASK)];
	mask2 = topbits[((X + DX) & WORDMASK)];
	if(mask2 == 0) 
		mask2 = 0xffffffff;
	screenp = addr(bp, rec.origin);

	/*
	 * ii is -1 if all in one word, == 0 if two adjacent words.
	 */
	ii = ((X + DX - 1) >> WORDSHIFT) - (X >> WORDSHIFT) - 1;
	if (ii < 0) {		/* diddle with masks for single words */
		mask1 &= mask2;
		mask2 = ~mask1;
	}
	bitindex = Y & WORDMASK;
	offset = (bp->width - ii - 1) << 2;
	switch (f) {
	case F_STORE:
		while (dy-- > 0) {
			bits = map->bits[bitindex];
			bitindex = (bitindex + 1) & WORDMASK;
			*screenp = (bits & mask1) | (*screenp & ~mask1);
			i = ii;
			if (i >= 0) {
				screenp++;
				if ((i >>= 2) > 0) 
					do {
						*screenp = bits;
						*(screenp+1) = bits;
						*(screenp+2) = bits;
						*(screenp+3) = bits;
						screenp += 4;
					} while (--i > 0);
				if ((i = ii & 3) > 0) 
					do {
						*screenp++ = bits;
					} while (--i > 0);
				*screenp = (bits & mask2) | (*screenp & ~mask2);
			}
			asm("			addw2	%r6,%r7 ");
		}
		break;
	case F_OR:
		while (dy-- > 0) {
			bits = map->bits[bitindex];
			bitindex = (bitindex + 1) & WORDMASK;
			*screenp |= bits & mask1;
			i = ii;
			if ( i >= 0) {
				screenp++;
				if ((i >>= 2) > 0) 
					do {
						*screenp |= bits;
						*(screenp+1) |= bits;
						*(screenp+2) |= bits;
						*(screenp+3) |= bits;
						screenp += 4;
					} while (--i > 0);
				if (( i = ii & 3) > 0) 
					do {
						*screenp++ |= bits;
					} while (--i > 0);
				*screenp |= bits & mask2;
			}
			asm("			addw2	%r6,%r7 ");
		}
		break;
	case F_CLR:
		while (dy-- > 0) {
			bits = map->bits[bitindex];
			bitindex = (bitindex + 1) & WORDMASK;
			*screenp &= ~(bits & mask1);
			i = ii;
			if (i >= 0) {
				screenp++;
				bits = ~bits;
				if ((i >>= 2) > 0) 
					do {
						*screenp &= bits;
						*(screenp+1) &= bits;
						*(screenp+2) &= bits;
						*(screenp+3) &= bits;
						screenp += 4;
					} while (--i > 0);
				if ((i = ii & 3) > 0) 
					do {
						*screenp++ = bits;
					} while (--i > 0);
				bits  = ~bits;
				*screenp &= ~(bits & mask2);
			}
			asm("			addw2	%r6,%r7 ");
		}
		break;
	case F_XOR:
		while (dy-- > 0) {
			bits = map->bits[bitindex];
			bitindex = (bitindex + 1) & WORDMASK;
			*screenp ^= bits & mask1;
			i = ii;
			if (i >= 0) {
				screenp++;
				if ((i >>= 2) > 0) 
					do {
						*screenp ^= bits;
						*(screenp+1) ^= bits;
						*(screenp+2) ^= bits;
						*(screenp+3) ^= bits;
						screenp += 4;
					} while (--i > 0);
				if ((i = ii & 3) > 0) 
					do {
						*screenp++ ^= bits;
					} while (--i > 0);
				*screenp ^= bits & mask2;
			}
			asm("			addw2	%r6,%r7 ");
		}
	}
}
