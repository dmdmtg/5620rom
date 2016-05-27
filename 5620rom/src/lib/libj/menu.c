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

#define scale(x, inmin, inmax, outmin, outmax)\
	(outmin + muldiv(x-inmin,outmax-outmin,inmax-inmin))

#define bound(x, low, high) min(high, max( low, x ))

#define SPACING		14
#define DISPLAY		16
#define CHARWIDTH	9
#define DELTA		6
#define BARWIDTH	18

int Menu_length, Menu_width;
Rectangle Menu_mr, Menu_sr, Menu_tr;  /* menu, scroll, text */
Point Menubaro, Menubarc;
char **Menu_table;
char *(*Mgenerator)();

static char *
tablegen(i)
{
	return Menu_table[i];
}

#undef menuhit
menuhit(m, but)
#define menuhit Smenuhit
register Menu *m;
{
	register int i, j, top, newtop, hit, newhit, items, lines;
	Point p, q, savep;
	register Bitmap *b;
	register char *s, *from, *to, fill[100];

#define sro Menu_sr.origin
#define src Menu_sr.corner
#define tro Menu_tr.origin
#define trc Menu_tr.corner
#define mro Menu_mr.origin
#define mrc Menu_mr.corner

	Mgenerator = (Menu_table=m->item) ? tablegen : m->generator;
	p = mouse.xy;
	/*
	* get maximum Menu_length of menu items so they can be centered
	*/
	for(Menu_length = items = 0; s=(*Mgenerator)(items, Menu_table); ++items)
		Menu_length = max(Menu_length, strlen(s));
	if(items == 0)
		return -1;
	Menu_width = Menu_length*CHARWIDTH+10;
	sro.x = sro.y = src.x = tro.x = mro.x = mro.y = 0;
	if(items <= DISPLAY)
		lines = items;
	else{
		lines = DISPLAY;
		tro.x = src.x = BARWIDTH;
		sro.x = sro.y = 1;
	}
#define ASCEND 2
	tro.y = ASCEND;
	mrc = trc = add(tro, Pt(Menu_width, min(items, lines)*SPACING));
	src.y = mrc.y-1;
	newtop = bound(m->prevtop, 0, items-lines);
 /* m->prevtop never initialized so it uses last value stored in menu structure */
	p.y -= bound(m->prevhit, 0, lines-1)*SPACING+SPACING/2;
	p.x = bound(p.x-(src.x+Menu_width/2), 0, XMAX-mrc.x);
	p.y = bound(p.y, 0, YMAX-mrc.y);
	Menu_sr = raddp(Menu_sr, p);
	Menu_tr = raddp(Menu_tr, p);
	Menu_mr = raddp(Menu_mr, p);

	b = balloc(Menu_mr);
	cursinhibit();
	if(b)
		bitblt(&display, Menu_mr, b, mro, F_STORE);
	rectf(&display, Menu_mr, (b ? F_OR : F_XOR));
	rectf(&display, inset(Menu_mr, 1), (b ? F_CLR : F_XOR));
	cursallow();

PaintMenu:
	top = newtop;
	drawmenu(top, items, lines);
	savep = mouse.xy;
	for(newhit = hit = -1; bttn(but); nap(2)){
		if(ptinrect(p = mouse.xy, Menu_sr)){
			if(ptinrect(savep,Menu_tr)){
				/*
				* center the mouse cursor vertically in the scroll bar
				*/
				p.y = (Menubaro.y+Menubarc.y)/2;
				cursset(p);
			}
			newtop = scale(p.y, sro.y, src.y, 0, items);
			newtop = bound(newtop-DISPLAY/2, 0, items-DISPLAY);
			if(newtop != top) {
				erasemenu(b, top, hit, items, lines);
				goto PaintMenu;
			}
		}else if(ptinrect(savep,Menu_sr)){
			register dx, dy;
			/*
			* Menu_try to keep the mouse cursor inside the scroll bar
			*/
			if(abs(dx = p.x-savep.x) < DELTA)
				dx = 0;
			if(abs(dy = p.y-savep.y) < DELTA)
				dy = 0;
			if(abs(dy) >= abs(dx))
				dx = 0;
			else
				dy = 0;
			cursset(p = add(savep, Pt(dx,dy)));
		}
		savep = p;
		newhit = -1;
		if( ptinrect(p, Menu_mr) && (p.x >= src.x) ){
			newhit = bound((p.y-tro.y)/SPACING, 0, lines-1);
			if(abs(newhit - hit)==1 && hit>=0
			 && abs(tro.y+SPACING*newhit+SPACING/2-p.y) > SPACING/3)
				newhit = hit;
		}
		if(newhit != hit){
			flip(Menu_tr, hit);
			flip(Menu_tr, hit = newhit);
		}
		if(newhit==0 && top>0){
			newtop = top-1;
			p.y += SPACING;
			cursset(p);
			erasemenu(b, top, hit, items, lines);
			goto PaintMenu;
		}
		if(newhit==DISPLAY-1 && top<items-lines){
			newtop = top+1;
			p.y -= SPACING;
			cursset(p);
			erasemenu(b, top, hit, items, lines);
			goto PaintMenu;
		}
	}

	if(b) {
		cursinhibit();
		screenswap(b, b->rect, b->rect);
		cursallow();
		bfree(b);
	} else {
		erasemenu(b, top, hit, items, lines);
		rectf(&display, Menu_mr, F_XOR);
		rectf(&display, inset(Menu_mr, 1), F_XOR);
	}

	if(hit>=0){
		m->prevhit = hit;
		m->prevtop = top;
		return hit+top;
	} else
		return -1;
}

static
erasemenu(b, top, hit, items, lines)
Bitmap *b;
int top, hit, items, lines;
{

	if( b ) {
		cursinhibit();
		rectf(&display, inset(Menu_mr, 1), F_CLR);
		cursallow();
	} else {
		flip(Menu_tr, hit);
		drawmenu(top, items, lines);
	}
}


static
drawmenu(top, items, lines)
register int top, items;
int lines;
{
	register char *from, *to;
	char fill[100];
	register int i, j;
	Point p, q;

	if(items > DISPLAY){
		/* 
		* Too many menu items so build the scroll bar
		*/
		Menubaro.y = scale(top, 0, items, sro.y, src.y);
		Menubaro.x = Menu_sr.origin.x;
		Menubarc.y = scale(top+DISPLAY, 0, items, sro.y, src.y);
		Menubarc.x = Menu_sr.corner.x;
		rectf(&display, Rpt(Menubaro,Menubarc), F_XOR);
	}
	for(p=tro, i=top; i < min(top+lines, items); ++i){
		q = p;
		from = Mgenerator(i, Menu_table);
		for(to = &fill[0]; *from; ++from) {
			if(*from & 0x80) {
				for(j=Menu_length-(strlen(from+1)+(to-&fill[0])); j-->0;) {
					*to++ = *from & 0x7F;
				}
			} else {
				*to++ = *from;
			}
		}
		*to = '\0';
		q.x += (Menu_width-jstrwidth(fill))/2;
		string(&defont, fill, &display, q, F_XOR);
		p.y += SPACING;
	}
}

static
flip(r,n)
	Rectangle r;
{
	if(n<0)
		return;
	++r.origin.x;
	r.corner.y = (r.origin.y += SPACING*n-1) + SPACING;
	--r.corner.x;
	rectf(&display, r, F_XOR);
}


