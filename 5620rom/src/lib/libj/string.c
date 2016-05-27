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
#include <dmd.h>
#include <font.h>

#undef string
Point
string(f, s, b, p, fc)
#define string Sstring
	register Font *f;
	char *s;
	register Bitmap *b;
	Point p;
	Code fc;
{
	register c;
	int full = (fc == F_STORE);
	Rectangle r;
	register Fontchar *i;
	if (full) {
		r.origin.y = 0;
		r.corner.y = f->height;
	}
	for (; c = *s++; p.x += i->width) {
		i = f->info + c;
		if (!full) {
			r.origin.y = i->top;
			r.corner.y = i->bottom;
		}
		r.origin.x = i->x;
		r.corner.x = (i+1)->x;
		bitblt(f->bits,r,b,Pt(p.x+
			((i->left & 0x80) ? i->left | 0xffffff00 : i->left),
			p.y+r.origin.y),fc);
	}
	return(p);
}

