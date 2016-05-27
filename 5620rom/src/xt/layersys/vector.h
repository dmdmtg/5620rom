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

extern int	Sys[];

/*
 * Cast	macros
 */
#define	Cast(t,	x)	((t (*)())Sys[x])
#define	TPoint(x)	Cast(Point, x)
#define	TRectangle(x)	Cast(Rectangle,	x)
#define	Tint(x)		Cast(int, x)
#define	TpBitmap(x)	Cast(Bitmap *, x)
#define	TpLayer(x)	Cast(Layer *, x)
#define	TpWord(x)	Cast(Word *, x)
#define	Tpchar(x)	Cast(char *, x)
#define	Tvoid(x)	Cast(void, x)
#define	TpProc(x)	Cast(struct Proc *, x)
#define	IPoint(x)	(*Cast(Point, x))
#define	IRectangle(x)	(*Cast(Rectangle, x))
#define	Iint(x)		(*Cast(int, x))
#define	Ilong(x)	(*Cast(long, x))
#define	IpBitmap(x)	(*Cast(Bitmap *, x))
#define	IpTexture(x)	(*Cast(Texture *, x))
#define	IpText16(x)	(*Cast(Texture16 *, x))
#define	IpLayer(x)	(*Cast(Layer *,	x))
#define	IpWord(x)	(*Cast(Word *, x))
#define	Ipchar(x)	(*Cast(char *, x))
#define	Ivoid(x)	(*Cast(void, x))
#define	IpProc(x)	(*Cast(struct Proc *, x))

/*
 * "Sys" calls
 */
#define	Vadd(p, q)	IPoint(2)(p, q)	/*'add'*/
#define	Vaddr(b,	p)	IpWord(3)(b, p)	/*'addr'*/
#define	VUalloc(u)	Ipchar(4)(u)	/*'Ualloc'*/
#define	VUballoc(r)	IpBitmap(5)(r)	/*'Uballoc'*/
#define	Vbfree(p)	Ivoid(6)(p)	/*'bfree'*/
#define	VUbitblt(sb, r, db, p, f)	Ivoid(7)(sb, r,	db, p, f)	/*'Ubitblt'*/
#define	VUcursallow()	Ivoid(8)()	/*'Ucursallow'*/
#define	VUcursinhibit()	Ivoid(9)()	/*'Ucursinhibit'*/
#define	VUcursswitch(c)	IpText16(10)(c)	/*'Ucursswitch'*/
#define	Vdellayer(l)	Iint(11)(l)	/*'dellayer'*/
#define	Vdiv(p, n)	IPoint(12)(p, n)	/*'div'*/
#define	Veqrect(r, s)	Iint(13)(r, s)	/*'eqrect'*/
#define	VUexit()		Ivoid(14)()	/*'Uexit'*/
#define	Vfree(p)		Ivoid(15)(p)	/*'free'*/
#define	Vinset(r, n)	IRectangle(16)(r, n)	/*'inset'*/
#define	VUjinit()		Ivoid(17)()	/*'Ujinit'*/
#define	VUjline(p, f)	Ivoid(18)(p, f)	/*'Ujline'*/
#define	VUjlineto(p, f)	Ivoid(19)(p, f)	/*'Ujlineto'*/
#define	VUjmove(p)	Ivoid(20)(p)	/*'Ujmove'*/
#define	VUjmoveto(p)	Ivoid(21)(p)	/*'Ujmoveto'*/
#define	VUjpoint(p, f)	Ivoid(22)(p, f)	/*'Ujpoint'*/
#define	VUjrectf(r, f)	Ivoid(23)(r, f)	/*'Ujrectf'*/
#define	VUjsegment(p, q, f)	Ivoid(24)(p, q,	f)	/*'Ujsegment'*/
#define	VUjstring(s)	IPoint(25)(s)	/*'Ujstring'*/
#define	VUjtexture(r, m, f)	Ivoid(26)(r, m,	f)	/*'Ujtexture'*/
#define	VUkbdchar()		Iint(27)()		/*'Ukbdchar'*/
#define	Vnap(s)		Ivoid(28)(s)		/*'nap'*/
#define	VUpoint(l, p, f)	Ivoid(29)(l, p,	f)	/*'Upoint'*/
#define	VUrectf(l, r, f)	Ivoid(30)(l, r,	f)	/*'Urectf'*/
#define	VUsegment(l, p, q, f)	Ivoid(31)(l, p,	q, f)	/*'Usegment'*/
#define	Vsleep(s)	Ivoid(32)(s)		/*'sleep'*/
#define	VUtexture(l, r, m, f)	Ivoid(33)(l, r,	m, f)	/*'Utexture'*/
#define	Vmenuhit(m, n)	Iint(34)(m, n)	/*'menuhit'*/
#define	Vmul(p, n)	IPoint(35)(p, n)	/*'mul'*/
#define	Vnewlayer(r)	IpLayer(36)(r)	/*'newlayer'*/
#define	VUown()		Iint(37)()	/*'Uown'*/
#define	Vptinrect(p, r)	Iint(38)(p, r)	/*'ptinrect'*/
#define	Vraddp(r, p)	IRectangle(39)(r, p)	/*'raddp'*/
#define	VUrcvchar()	Iint(40)()	/*'Urcvchar'*/
#define	VrectXrect(r, s)	Iint(41)(r, s)	/*'rectXrect'*/
#define	Vrectclip(pr, r)	Iint(42)(pr, r)	/*'rectclip'*/
#define	VUrequest(r)	Iint(43)(r)	/*'Urequest'*/
#define	Vrsubp(r, p)	IRectangle(44)(r, p)	/*'rsubp'*/
#define	VUscreenswap(b, r, s)	Ivoid(45)(b, r,	s)	/*'Uscreenswap'*/
#define	VUsendchar(c)	Ivoid(46)(c)	/*'Usendchar'*/
#define	Vsendnchars(n, p)	Ivoid(47)(n,p)	/*'sendnchars'*/
#define	Vstring(F, s, b,	p, f)	IPoint(48)(F, s, b, p, f)	/*'string'*/
#define	Vstrwidth(F, s)	Iint(49)(F,s)	/*'strwidth'*/
#define	Vsub(p, q)	IPoint(50)(p, q)	/*'sub'*/
#define	Vsw(n)		Ivoid(51)(n)	/*'sw'*/
#define	Vupfront(l)	Iint(52)(l)	/*'upfront'*/
#define	VUwait(n)		Iint(53)(n)	/*'Uwait'*/
#define	Vclear(r,s)	Ivoid(54)(r, s)	/*'clear'*/
#define	Vdebug()		IpProc(55)()	/*'debug'*/
#define	Vrealgcalloc(n, w)	Ipchar(56)(n, w, P)	/*'realgcalloc'*/
#define	Vgcfree(s)	Ivoid(57)(s)		/*'gcfree'*/
#define	Vgetrect()	IRectangle(58)()	/*'getrect'*/
#define	Valarm(n)	Ivoid(59)(n)	/*'alarm'*/
#define	Vlpoint(b, p, f)	Ivoid(60)(b, p,	f)		/*'lpoint'*/
#define	Vlrectf(b, r, f)	Ivoid(61)(b, r,	f)		/*'lrectf'*/
#define	Vlsegment(b, p, q, f)	Ivoid(62)(b, p,	q, f)		/*'lsegment'*/
#define	Vltexture(b, r, t, f)	Ivoid(63)(b, r,	t, f)		/*'ltexture'*/
#define	Vtransform(p)	IPoint(64)(p)	/*'transform'*/
#define	Vrtransform(p)	IRectangle(65)(p)	/*'rtransform'*/
#define	Vrealtime()	Ilong(66)()	/*'realtime'*/
#define	Vcursinhibit()	Ivoid(67)()	/*'cursinhibit'*/
#define	Vcursallow()	Ivoid(68)()	/*'cursallow'*/
#define	VUcursset(p)	Ivoid(69)(p)	/*'Ucursset'*/
#define	Vnewproc(f)	IpProc(70)(f)	/*'newproc'*/
#define	Vmpxnewwind(p,c)	Ivoid(71)(p,c)	/*'mpxnewwind'*/
#define	Vnewwindow(f)	Ivoid(72)(f)	/*'newwindow'*/
#define	Vtolayer(l)	Ivoid(73)(l)	/*'tolayer'*/
#define Vjstrwidth(s)	Iint(74)(s)	/*'jstrwidth'*/
/*
 * Vector calls
 */
#define VBottom()		Iint(75)()
#define VControl()		Ivoid(76)()
#define VCurrent()		Iint(77)()
#define VDelete()		Iint(78)()
#define VExit()			Iint(79)()
#define VMove()			Iint(80)()
#define VNew()			Iint(81)()
#define VPsend(a, b, c, d)	Iint(82)(a, b, c, d)
#define VReply(x)		Ivoid(83)(x)
#define VReshape()		Iint(84)()
#define VRetry()			Iint(85)()
#define VSw()			Iint(86)()
#define VTop()			Iint(87)()
#define V_start()		Ivoid(88)()
#define Vagent(a)		Iint(89)(a)
#define Vbldargs(x, y)		Iint(90)(x, y)
#define Vboot()			Iint(91)()
#define Vbttns(a)		Iint(92)(a)
#define Vcanon(x, y)		IRectangle(93)(x, y)
#define Vcheckrect(r)		IRectangle(94)(r)
#define Vclippt(p,f)		Iint(95)(p,f)
#define Vclockroutine()		Iint(96)()
#define Vcontrol()		Iint(97)()
#define Vcopy(s)			Iint(98)(s)
#define Vcrc(s, n)		Iint(99)(s, n)
#define Vcurse(c)		Iint(100)(c)
#define Vdelproc(p)		Iint(101)(p)
#define Vdemux()			Iint(102)()
#define Vdobutton(b)		Iint(103)(b)
#define Vdoctl(s, n)		Iint(104)(s, n)
#define Verror(x, y)		Iint(105)(x, y)
#define Vexcep_norm()		Iint(106)()
#define Vexcep_proc()		Iint(107)()
#define Vexcep_stack()		Iint(108)()
#define Vexec(f)			Iint(109)(f)
#define Vexecsw()		Iint(110)()
#define Vfreemem(p)		Iint(111)(p)
#define Vgetanum(a,b,c)		Iint(112)(a,b,c)
#define Vgetchar()		Iint(113)()
#define Vgetlong()		Iint(114)()
#define Vgivemouse(p,update)		Iint(115)(p,update)
#define Vhost_int()		Ivoid(116)()
#define Vitox		Ipchar(117)
#define Vjstring(s)		IPoint(118)(s) /* for layersys jstring functions */
#define Vkey_int()		Ivoid(119)()
#define Vlscroll(a, b, c)	Iint(120)(a, b, c)
#define Vlscrolx(a,b)		Iint(121)(a,b)
#define Vmain()			Iint(122)()
#define Vmax(a, b)		Iint(123)(a, b)
#define Vmin(a, b)		Iint(124)(a, b)
#define Vmove(l, p)		Iint(125)(l, p)
#define Vmpxcore(w)		Iint(126)(w)
#define Vmpxdelwind(w)		Iint(127)(w)
#define Vmpxkbdchar(c)		Iint(128)(c)
#define Vmpxkbdflush()		Iint(129)()
#define Vmpxsendchar(c, p)	Iint(130)(c, p)
#define Vmpxublk(p)		Iint(131)(p)
#define Vmsvid_int()		Ivoid(132)()
#define Vnewline(c)		Iint(133)(c)
#define Vnlcount(a)		Iint(134)(a)
#define Vnumbers(a,b,c)		Iint(135)(a,b,c)
#define Vout_int()		Ivoid(136)()
#define Voutline(r)		Iint(137)(r)
#define Vpinit(c)		Iint(138)(c)
#define Vprecv(c)		Ivoid(139)(c)
#define Vpsend(a, b, c, d)	Iint(140)(a, b, c, d)
#define Vpt(p)			IPoint(141)(p)
#define Vptimeout(a)		Iint(142)(a)
#define Vreadchar(x)		Iint(143)(x)
#define VRECT(r)			IRectangle(144)(r)
#define Vrecvchars(l, p, n)	Iint(145)(l, p, n)
#define Vreshape(l, r)		Iint(146)(l, r)
#define Vrestart(p, x)		Iint(147)(p, x)
#define Vsendpkt(p, n)		Iint(148)(p, n)
#define Vsetdata(p)		Iint(149)(p)
#define Vshade(l)		Iint(150)(l)
#define Vshutdown(p)		Iint(151)(p)
#define Vswitcher()		Iint(152)()
#define Vtrap(a,b)		Iint(153)(a,b)
#define Vwhichbutton()		Iint(154)()
#define Vwhichlayer()		IpLayer(155)()
#define Vwhichproc(l)		Iint(156)(l)
#define Vwindowproc()		Iint(157)()
#define Vwindowstart()		Iint(158)()
#define Vwritec( a, b, c)	Iint(159)( a, b, c)
#define Vzombexec(f)		Iint(160)(f)
#define Vzombsw()		Iint(161)()
#define Vswinit()		Iint(162)()
#define Vversion()		Iint(164)()
#define Vringbell()		Ivoid(165)()
#define sysrun(a)		Ivoid(166)(a)

#ifdef MPX_H
#define I_Ref(t,i)		(*(t *)Sys[i])	/* Indirect reference */
#define P_Array(t,i)		((t *)Sys[i])	/* pointer to array */
#define D_Ref(t,i)		(*(t *)&Sys[i])	/* Direct reference */

/*#define cup			I_Ref(Texture16,167)
*/
#define kbdproc			D_Ref(struct Proc *,168)
/*#define debugger		D_Ref(struct Proc *,169)
#define second			D_Ref(short,170)
#define ticks			D_Ref(short,171)
#define ticks0			D_Ref(long,172)
#define proctab			P_Array(struct Proc,173)
#define pconvs			P_Array(struct Pchannel,174)
#define pconfig			I_Ref(struct Pconfig,175)
#define menufn			((int (**)())Sys[176])
#define windowmenu		I_Ref(struct Menu,177)
#define usermouse		D_Ref(short,178)
#define kbdlist			P_Array(char,179)
#define kbdlistp		D_Ref(char *,180)
#define hst_init		D_Ref(short,181)
#define bullseye		I_Ref(Texture16,182)
#define skull			I_Ref(Texture16,183)
#define boxcurs			I_Ref(Texture16,184)
#define menutext		P_Array(char *,185)
#define sendbusy		D_Ref(short,186)
*/
#define shademap		I_Ref(Texture,187)
/*#define rebootflag		D_Ref(short,188)
#define last			D_Ref(struct Proc *,189)
#define patchedspot		D_Ref(char *,190)
#define patch			D_Ref(char,191)
*/
#endif

/* New entries
*/
#define VUpolyf			Iint(214)	/* 'Upolyf' */
/* #define VUjpolyf		Iint(215)	--- 'Ujpolyf' No more j routine ---
*/
#define Vptinpoly		Iint(216)	/* 'ptinpoly' */
#define	VUclipbttn		Iint(217)	/* 'Uclipbttn' */
#define	Vpoint2layer()		IpProc(218)()	/*'point2layer'*/
#define	Vwhichaddr(n)		IpProc(219)(n)	/*'whichaddr'*/
#define Vdownback(lp)		Iint(220)(lp)	/*'downback'*/
#define Vcbufavail(t)		Iint(221)(t)		/*'cbufavail'*/
#define Vpasschar(c, t)		Ivoid(222)(c, t)	/*'passchar'*/


#define	add(p, q)		Vadd(p, q)
#define	addr(b,	p)		Vaddr(b,	p)
#define	Ualloc(u)		VUalloc(u)
#define	Uballoc(r)		VUballoc(r)
#define	bfree(p)		Vbfree(p)
#define	Ubitblt(sb, r, db, p, f)		VUbitblt(sb, r, db, p, f)
#define	Ucursallow()		VUcursallow()
#define	Ucursinhibit()		VUcursinhibit()
#define	Ucursswitch(c)		VUcursswitch(c)
#define	dellayer(l)		Vdellayer(l)
#define	div(p, n)		Vdiv(p, n)
#define	eqrect(r, s)		Veqrect(r, s)
#define	Uexit()		VUexit()
#define	free(p)		Vfree(p)
#define	inset(r, n)		Vinset(r, n)
#define	Ujinit()		VUjinit()
#define	Ujline(p, f)		VUjline(p, f)
#define	Ujlineto(p, f)		VUjlineto(p, f)
#define	Ujmove(p)		VUjmove(p)
#define	Ujmoveto(p)		VUjmoveto(p)
#define	Ujpoint(p, f)		VUjpoint(p, f)
#define	Ujrectf(r, f)		VUjrectf(r, f)
#define	Ujsegment(p, q, f)		VUjsegment(p, q, f)
#define	Ujstring(s)		VUjstring(s)
#define	Ujtexture(r, m, f)		VUjtexture(r, m, f)
#define	Ukbdchar()		VUkbdchar()
#define	nap(s)		Vnap(s)
#define	Upoint(l, p, f)		VUpoint(l, p, f)
#define	Urectf(l, r, f)		VUrectf(l, r, f)
#define	Usegment(l, p, q, f)		VUsegment(l, p, q, f)
#define	sleep(s)		Vsleep(s)
#define	Utexture(l, r, m, f)		VUtexture(l, r, m, f)
#define	menuhit(m, n)		Vmenuhit(m, n)
#define	mul(p, n)		Vmul(p, n)
#define	newlayer(r)		Vnewlayer(r)
#define	Uown()		VUown()
#define	ptinrect(p, r)		Vptinrect(p, r)
#define	raddp(r, p)		Vraddp(r, p)
#define	Urcvchar()		VUrcvchar()
#define	rectXrect(r, s)		VrectXrect(r, s)
#define	rectclip(pr, r)		Vrectclip(pr, r)
#define	Urequest(r)		VUrequest(r)
#define	rsubp(r, p)		Vrsubp(r, p)
#define	Uscreenswap(b, r, s)		VUscreenswap(b, r, s)
#define	Usendchar(c)		VUsendchar(c)
#define	sendnchars(n, p)		Vsendnchars(n, p)
#define	string(F, s, b,	p, f)		Vstring(F, s, b,	p, f)
#define	strwidth(F, s)		Vstrwidth(F, s)
#define	sub(p, q)		Vsub(p, q)
#define	sw(n)		Vsw(n)
#define	upfront(l)		Vupfront(l)
#define	Uwait(n)		VUwait(n)
#define	clear(r,s)		Vclear(r,s)
#define	debug()		Vdebug()
#define	realgcalloc(n, w)		Vrealgcalloc(n, w)
#define	gcfree(s)		Vgcfree(s)
#define	getrect()		Vgetrect()
#define	alarm(n)		Valarm(n)
#define	lpoint(b, p, f)		Vlpoint(b, p, f)
#define	lrectf(b, r, f)		Vlrectf(b, r, f)
#define	lsegment(b, p, q, f)		Vlsegment(b, p, q, f)
#define	ltexture(b, r, t, f)		Vltexture(b, r, t, f)
#define	transform(p)		Vtransform(p)
#define	rtransform(p)		Vrtransform(p)
#define	realtime()		Vrealtime()
#define	cursinhibit()		Vcursinhibit()
#define	cursallow()		Vcursallow()
#define	Ucursset(p)		VUcursset(p)
#define	newproc(f)		Vnewproc(f)
#define	mpxnewwind(p,c)		Vmpxnewwind(p,c)
#define	newwindow(f)		Vnewwindow(f)
#define	tolayer(l)		Vtolayer(l)
#define jstrwidth(s)		Vjstrwidth(s)
/*
 * Vector calls
 */
#define Bottom()		VBottom()
#define Control()		VControl()
#define Current()		VCurrent()
#define Delete()		VDelete()
#define Exit()		VExit()
#define Move()		VMove()
#define New()		VNew()
#define Psend(a, b, c, d)		VPsend(a, b, c, d)
#define Reply(x)		VReply(x)
#define Reshape()		VReshape()
#define Retry()		VRetry()
#define Sw()		VSw()
#define Top()		VTop()
#define _start()		V_start()
#define agent(a)		Vagent(a)
#define bldargs(x, y)		Vbldargs(x, y)
#define boot()		Vboot()
#define bttns(a)		Vbttns(a)
#define canon(x, y)		Vcanon(x, y)
#define checkrect(r)		Vcheckrect(r)
#define clippt(p,f)		Vclippt(p,f)
#define clockroutine()		Vclockroutine()
#define control()		Vcontrol()
#define copy(s)		Vcopy(s)
#define crc(s, n)		Vcrc(s, n)
#define curse(c)		Vcurse(c)
#define delproc(p)		Vdelproc(p)
#define demux()		Vdemux()
#define dobutton(b)		Vdobutton(b)
#define doctl(s, n)		Vdoctl(s, n)
#define error(x, y)		Verror(x, y)
#define excep_norm()		Vexcep_norm()
#define excep_proc()		Vexcep_proc()
#define excep_stack()		Vexcep_stack()
#define exec(f)		Vexec(f)
#define execsw()		Vexecsw()
#define freemem(p)		Vfreemem(p)
#define getanum(a,b,c)		Vgetanum(a,b,c)
#define getchar()		Vgetchar()
#define getlong()		Vgetlong()
#define givemouse(p,update)		Vgivemouse(p,update)
#define host_int()		Vhost_int()
#define itox		Vitox
#define jstring(s)		Vjstring(s)
#define key_int()		Vkey_int()
#define lscroll(a, b, c)		Vlscroll(a, b, c)
#define lscrolx(a,b)		Vlscrolx(a,b)
#define main()		Vmain()
#define max(a, b)		Vmax(a, b)
#define min(a, b)		Vmin(a, b)
#define move(l, p)		Vmove(l, p)
#define mpxcore(w)		Vmpxcore(w)
#define mpxdelwind(w)		Vmpxdelwind(w)
#define mpxkbdchar(c)		Vmpxkbdchar(c)
#define mpxkbdflush()		Vmpxkbdflush()
#define mpxsendchar(c, p)		Vmpxsendchar(c, p)
#define mpxublk(p)		Vmpxublk(p)
#define msvid_int()		Vmsvid_int()
#define newline(c)		Vnewline(c)
#define nlcount(a)		Vnlcount(a)
#define numbers(a,b,c)		Vnumbers(a,b,c)
#define out_int()		Vout_int()
#define outline(r)		Voutline(r)
#define pinit(c)		Vpinit(c)
#define precv(c)		Vprecv(c)
#define psend(a, b, c, d)		Vpsend(a, b, c, d)
#define pt(p)		Vpt(p)
#define ptimeout(a)		Vptimeout(a)
#define readchar(x)		Vreadchar(x)
#define RECT(r)		VRECT(r)
#define recvchars(l, p, n)		Vrecvchars(l, p, n)
#define reshape(l, r)		Vreshape(l, r)
#define restart(p, x)		Vrestart(p, x)
#define sendpkt(p, n)		Vsendpkt(p, n)
#define setdata(p)		Vsetdata(p)
#define shade(l)		Vshade(l)
#define shutdown(p)		Vshutdown(p)
#define switcher()		Vswitcher()
#define trap(a,b)		Vtrap(a,b)
#define whichbutton()		Vwhichbutton()
#define whichlayer()		Vwhichlayer()
#define whichproc(l)		Vwhichproc(l)
#define windowproc()		Vwindowproc()
#define windowstart()		Vwindowstart()
#define writec( a, b, c)		Vwritec( a, b, c)
#define zombexec(f)		Vzombexec(f)
#define zombsw()		Vzombsw()
#define swinit()		Vswinit()
#define version()		Vversion()
#define ringbell()		Vringbell()
#define Upolyf			VUpolyf
/*   #define Ujpolyf			VUjpolyf
*/
#define ptinpoly		Vptinpoly
#define Uclipbttn()		VUclipbttn()
#define	point2layer()		Vpoint2layer()
#define	whichaddr(n)		Vwhichaddr(n)
#define downback(lp)		Vdownback(lp)
#define cbufavail(t)		Vcbufavail(t)
#define passchar(c, t)		Vpasschar(c, t)
