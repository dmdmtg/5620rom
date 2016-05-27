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
/*
* Make sure that the order of entries are put in the same order
* as they are in the vector.s (sa_template.s) file.
* Also add new entries to the end of each section  .. not 
* in the middle.
*/

#ifndef Firm	
#define	Firm	((int	*)0x200)
#endif


/*
 * FCast	macros
 */
#ifndef FCast
#define	FCast(t,	x)	((t (*)())Firm[x])
#endif
#ifndef FIPoint
#define	FIPoint(x)	(*FCast(Point, x))
#endif
#ifndef FIRectangle
#define	FIRectangle(x)	(*FCast(Rectangle, x))
#endif
#ifndef FIint
#define	FIint(x)		(*FCast(int, x))
#endif
#ifndef FIlong
#define	FIlong(x)	(*FCast(long, x))
#endif
#ifndef FIshort
#define	FIshort(x)	(*FCast(short, x))
#endif
#ifndef FIpBitmap
#define	FIpBitmap(x)	(*FCast(Bitmap *, x))
#endif
#ifndef FIpTexture
#define	FIpTexture(x)	(*FCast(Texture *, x))
#endif
#ifndef FIpText16
#define	FIpText16(x)	(*FCast(Texture16 *, x))
#endif
#ifndef FIpLayer
#define	FIpLayer(x)	(*FCast(Layer *,	x))
#endif
#ifndef FIpWord
#define	FIpWord(x)	(*FCast(Word *, x))
#endif
#ifndef FIpchar
#define	FIpchar(x)	(*FCast(char *, x))
#endif
#ifndef FIvoid
#define	FIvoid(x)	(*FCast(void, x))
#endif
#ifndef FIpProc
#define	FIpProc(x)	(*FCast(struct Proc *, x))
#endif
#ifndef DCast
#define	DCast(t, x)	(*((t *)Firm[x]))
#endif
#ifndef DACast
#define DACast(t, x)	(((t *)Firm[x]))
#endif
#ifndef DIPoint
#define	DIPoint(x)	DCast(Point, x)
#endif
#ifndef DIpRectangle
#define	DIpRectangle(x)	DCast(Rectangle *, x)
#endif
#ifndef DIchar
#define	DIchar(x)	DCast(char, x)
#endif
#ifndef DIshort
#define	DIshort(x)	DCast(short, x)
#endif
#ifndef DIint
#define	DIint(x)	DCast(int, x)
#endif
#ifndef DIlong
#define	DIlong(x)	DCast(long, x)
#endif
#ifndef DIFont
#define	DIFont(x)	DCast(Font, x)
#endif
#ifndef	DITex16
#define	DITex16(x)		DCast(Texture16, x)
#endif
#ifndef	DITex
#define	DITex(x)		DCast(Texture, x)
#endif
#ifndef DIBitmap
#define	DIBitmap(x)	DCast(Bitmap, x)
#endif
#ifndef DIpBitmap
#define	DIpBitmap(x)	DCast(Bitmap *, x)
#endif
#ifndef DIpTexture
#define	DIpTexture(x)	DCast(Texture *, x)
#endif
#ifndef DIpText16
#define	DIpText16(x)	DCast(Texture16 *, x)
#endif
#ifndef DIpLayer
#define	DIpLayer(x)	DCast(Layer *, x)
#endif
#ifndef DIpWord
#define	DIpWord(x)	DCast(Word *, x)
#endif
#ifndef DIpint
#define	DIpint(x)	DCast(int *, x)
#endif
#ifndef DIpchar
#define	DIpchar(x)	DCast(char *, x)
#endif
#ifndef DIpProc
#define	DIpProc(x)	DCast(struct Proc *, x)
#endif
#ifndef DIplong
#define	DIplong(x)	DCast(long *, x)
#endif

/*
 * "Firm" calls
 */
#ifndef Sabs
#define Sabs(a)			FIint(0)(a)
#endif
#ifndef Saciapaws
#define Saciapaws()	FIint(1)()
#endif
#ifndef Saciainit
#define Saciainit(a)	FIint(2)(a)
#endif
#ifndef Saciatrint
#define Saciatrint()	FIint(3)()
#endif
#ifndef Sadd
#define Sadd(a, b)		FIPoint(4)(a, b)
#endif
#ifndef Saddr
#define Saddr(a, b)		FIpWord(5)(a, b)
#endif
#ifndef Salloc
#define Salloc(a)		FIpchar(6)(a)
#endif
#ifndef Sallocinit
#define Sallocinit(a, b)	FIint(7)(a, b)
#endif
#ifndef Sauto1
#define Sauto1()		FIint(8)()
#endif
#ifndef Sauto2
#define Sauto2()		FIint(9)()
#endif
#ifndef Sauto4
#define Sauto4()		FIint(10)()
#endif
#ifndef Sballoc
#define Sballoc(a)		FIpBitmap(11)(a)
#endif
#ifndef Sbfree
#define Sbfree(a)		FIvoid(12)(a)
#endif
#ifndef Sbinit
#define Sbinit()		FIint(13)()
#endif
#ifndef Sbitblt
#define Sbitblt(a, b, c, d, e)		FIvoid(14)(a, b, c, d, e)
#endif
#ifndef Spfkey
#define Spfkey(a, b, c)	FIint(15)(a, b, c)
#endif
#ifndef Sbramputstr
#define Sbramputstr(a, b)	FIint(16)(a, b)
#endif
#ifndef Sceil
#define Sceil(a, b)		FIshort(17)(a, b)
#endif
#ifndef Scheckbram
#define Scheckbram()	FIint(18)()
#endif
#ifndef Scursallow
#define Scursallow()	FIvoid(19)()
#endif
#ifndef Scursblt
#define Scursblt(a, b, c)	FIint(20)(a, b, c)
#endif
#ifndef Scursinhibit
#define Scursinhibit()	FIvoid(21)()
#endif
#ifndef Scursinit
#define Scursinit()	FIint(22)()
#endif
#ifndef Scursset
#define Scursset(a)	FIvoid(23)(a)
#endif
#ifndef Scursswitch
#define Scursswitch(a)	FIpText16(24)(a)
#endif
#ifndef Sdellayer
#define Sdellayer(a)	FIint(25)(a)
#endif
#ifndef Sdisconnect
#define Sdisconnect()	FIint(26)()
#endif
#ifndef Sdiv
#define Sdiv(a, b)		FIPoint(27)(a, b)
#endif
#ifndef Seqpt
#define Seqpt(a, b)		FIint(28)(a, b)
#endif
#ifndef Seqrect
#define Seqrect(a, b)		FIint(29)(a, b)
#endif
#ifndef Sexcep_int
#define Sexcep_int()	FIint(30)()
#endif
#ifndef Sexcep_norm
#define Sexcep_norm()	FIint(31)()
#endif
#ifndef Sexcep_proc
#define Sexcep_proc()	FIint(32)()
#endif
#ifndef Sexcep_stack
#define Sexcep_stack()	FIint(33)()
#endif
#ifndef Sexcep_msg
#define Sexcep_msg(a, b, c, d)	FIint(34)(a, b, c, d)
#endif
#ifndef Sfloor
#define Sfloor(a, b)		FIshort(35)(a, b)
#endif
#ifndef Sfree
#define Sfree(a)		FIvoid(36)(a)
#endif
#ifndef Sfreeall
#define Sfreeall(a)	FIint(37)(a)
#endif
#ifndef Sgcalloc
#define Sgcalloc(a, b)	FIpchar(38)(a, b)
#endif
#ifndef Sgcfree
#define Sgcfree(a)		FIvoid(39)(a)
#endif
#ifndef Sgcfreeall
#define Sgcfreeall(a)	FIint(40)(a)
#endif
#ifndef Sgcinit
#define Sgcinit()		FIint(41)()
#endif
#ifndef Sgetnum
#define Sgetnum()		FIint(42)()
#endif
#ifndef Sinset
#define Sinset(a, b)		FIRectangle(43)(a, b)
#endif
#ifndef Sjline
#define Sjline(a, b)		FIvoid(44)(a, b)
#endif
#ifndef Sjlineto
#define Sjlineto(a, b)	FIvoid(45)(a, b)
#endif
#ifndef Sjmove
#define Sjmove(a)		FIvoid(46)(a)
#endif
#ifndef Sjmoveto
#define Sjmoveto(a)	FIvoid(47)(a)
#endif
#ifndef Sjpoint
#define Sjpoint(a, b)		FIvoid(48)(a, b)
#endif
#ifndef Sjrectf
#define Sjrectf(a, b)		FIvoid(49)(a, b)
#endif
#ifndef Sjsegment
#define Sjsegment(a, b, c)	FIvoid(50)(a, b, c)
#endif
#ifndef Sjstring
#define Sjstring(a)	FIPoint(51)(a)
#endif
#ifndef Sjstrwidth
#define Sjstrwidth(a)	FIint(52)(a)
#endif
#ifndef Sjtexture
#define Sjtexture(a, b, c)	FIvoid(53)(a, b, c)
#endif
#ifndef Skbdchar
#define Skbdchar()	FIint(54)()
#endif
#ifndef Skbdinit
#define Skbdinit()	FIint(55)()
#endif
#ifndef Skgetc
#define Skgetc()		FIint(56)()
#endif
#ifndef Slayerop
#define Slayerop(a, b, c, d, e, f)	FIint(57)(a, b, c, d, e, f)
#endif
#ifndef Slbitblt
#define Slbitblt(a, b, c, d, e)	FIvoid(58)(a, b, c, d, e)
#endif
#ifndef Slblt
#define Slblt(a, b, c, d, e)		FIint(59)(a, b, c, d, e)
#endif
#ifndef SLbox
#define SLbox(a)		FIint(60)(a)
#endif
#ifndef SLgrey
#define SLgrey(a)		FIint(61)(a)
#endif
#ifndef Sload
#define Sload()		FIint(62)()
#endif
#ifndef Slpoint
#define Slpoint(a, b, c)		FIvoid(63)(a, b, c)
#endif
#ifndef Slrectf
#define Slrectf(a, b, c)		FIvoid(64)(a, b, c)
#endif
#ifndef Slsegment
#define Slsegment(a, b, c, d)	FIvoid(65)(a, b, c, d)
#endif
#ifndef Sltexture
#define Sltexture(a, b, c, d)	FIvoid(66)(a, b, c, d)
#endif
#ifndef Svitty
#define Svitty()		FIint(67)()
#endif
#ifndef Smenuhit
#define Smenuhit(a, b)	FIint(68)(a, b)
#endif
#ifndef Smul
#define Smul(a, b)		FIPoint(69)(a, b)
#endif
#ifndef Snap
#define Snap(a)		FIvoid(70)(a)
#endif
#ifndef Snewlayer
#define Snewlayer(a)	FIpLayer(71)(a)
#endif
#ifndef Sown
#define Sown()		FIint(72)()
#endif
#ifndef Spfedit
#define Spfedit(a)		FIint(73)(a)
#endif
#ifndef Spoint
#define Spoint(a, b, c)		FIvoid(74)(a, b, c)
#endif
#ifndef SPt
#define SPt(a, b)		FIPoint(75)(a, b)
#endif
#ifndef Sptinrect
#define Sptinrect(a, b)	FIint(76)(a, b)
#endif
#ifndef Sqclear
#define Sqclear(a)		FIint(77)(a)
#endif
#ifndef Sqgetc
#define Sqgetc(a)		FIint(78)(a)
#endif
#ifndef Sqinit
#define Sqinit()		FIint(79)()
#endif
#ifndef Sqputc
#define Sqputc(a, b)		FIint(80)(a, b)
#endif
#ifndef Sqputstr
#define Sqputstr(a, b)	FIint(81)(a, b)
#endif
#ifndef Sraddp
#define Sraddp(a, b)		FIRectangle(82)(a, b)
#endif
#ifndef Srcvchar
#define Srcvchar()	FIint(83)()
#endif
#ifndef Srealalloc
#define Srealalloc(a, b)	FIpchar(84)(a, b)
#endif
#ifndef Srealballoc
#define Srealballoc(a, b)	FIpBitmap(85)(a, b)
#endif
#ifndef Srealgcalloc
#define Srealgcalloc(a, b, c)	FIpchar(86)(a, b, c)
#endif
#ifndef Sreboot
#define Sreboot()		FIint(87)()
#endif
#ifndef SRect
#define SRect(a, b, c, d)		FIRectangle(88)(a, b, c, d)
#endif
#ifndef Srectclip
#define Srectclip(a, b)		FIint(89)(a, b)
#endif
#ifndef Srectf
#define Srectf(a, b, c)		FIvoid(90)(a, b, c)
#endif
#ifndef SrectXrect
#define SrectXrect(a, b)		FIint(91)(a, b)
#endif
#ifndef Sringbell
#define Sringbell()		FIvoid(92)()
#endif
#ifndef SRpt
#define SRpt(a, b)		FIRectangle(93)(a, b)
#endif
#ifndef Srsubp
#define Srsubp(a, b)		FIRectangle(94)(a, b)
#endif
#ifndef Sscreenswap
#define Sscreenswap(a, b, c)		FIvoid(95)(a, b, c)
#endif
#ifndef Sscroll
#define Sscroll(a, b, c)		FIint(96)(a, b, c)
#endif
#ifndef Ssegment
#define Ssegment(a, b, c, d)		FIvoid(97)(a, b, c, d)
#endif
#ifndef Ssendbreak
#define Ssendbreak()		FIint(98)()
#endif
#ifndef Ssendchar
#define Ssendchar(a)		FIint(99)(a)
#endif
/* 100 was sendnchars -- taken out by MJK */
#ifndef Ssetbram
#define Ssetbram()		FIint(101)()
#endif
#ifndef Ssetup
#define Ssetup()		FIint(102)()
#endif
#ifndef Sspl0
#define Sspl0()		FIint(103)()
#endif
#ifndef Sspl1
#define Sspl1()		FIint(104)()
#endif
#ifndef Sspl4
#define Sspl4()		FIint(105)()
#endif
#ifndef Sspl5
#define Sspl5()		FIint(106)()
#endif
#ifndef Sspl6
#define Sspl6()		FIint(107)()
#endif
#ifndef Sspl7
#define Sspl7()		FIint(108)()
#endif
#ifndef Ssplx
#define Ssplx(a)		FIint(109)(a)
#endif
#ifndef Sstring
#define Sstring(a, b, c, d, e)		FIPoint(110)(a, b, c, d, e)
#endif
#ifndef Sstrlen
#define Sstrlen(a)		FIint(111)(a)
#endif
#ifndef Sstrwidth
#define Sstrwidth(a, b)		FIint(112)(a, b)
#endif
#ifndef Ssub
#define Ssub(a, b)		FIPoint(113)(a, b)
#endif
#ifndef Stest32
#define Stest32()		FIint(114)()
#endif
#ifndef Stexture
#define Stexture(a, b, c, d)		FIvoid(115)(a, b, c, d)
#endif
#ifndef Strdisable
#define Strdisable()		FIint(116)()
#endif
#ifndef Strenable
#define Strenable()		FIint(117)()
#endif
#ifndef Ststdcd
#define Ststdcd()		FIint(118)()
#endif
#ifndef Supfront
#define Supfront(a)		FIint(119)(a)
#endif
#ifndef Sversion
#define Sversion()		FIint(120)()
#endif
#ifndef Swait
#define Swait(a)		FIint(121)(a)
#endif
#ifndef Spsendchar
#define Spsendchar		FIint(156)
#endif
#ifndef Spiohint
#define Spiohint 		FIint(184)
#endif
#ifndef Sqsetbram
#define Sqsetbram		FIint(220)
#endif

/* Section for Variables */

#ifndef Sallocb		
#define Sallocb		DCast(union store *, 122)
#endif
#ifndef Sallocendp		
#define Sallocendp		DIpchar(123)
#endif
#ifndef Sallocstartp		
#define Sallocstartp		DIpchar(124)
#endif
#ifndef Sblocked		
#define Sblocked		DIint(125)
#endif
#ifndef Scbufs		
#define Scbufs		DACast(struct cbuf, 126)
#endif
#ifndef Scur		
#define Scur		DIPoint(127)
#endif
#ifndef Scursor		
#define Scursor		DCast(struct Cursor, 128)
#endif
#ifndef Scurtabp		
#define Scurtabp		DCast(struct curtab *, 129)
#endif
#ifndef Sdefont		
#define Sdefont		DIFont(130)
#endif
#ifndef Sdisplay		
#define Sdisplay		DIBitmap(131)
#endif
#ifndef Sdtr		
#define Sdtr		DIint(132)
#endif
#ifndef SENDAREA		
#define SENDAREA		DIplong(133)
#endif
#ifndef Sfreelist		
#define Sfreelist		DCast(struct cbuf *, 134)
#endif
#ifndef Sinterrupt		
#define Sinterrupt		DIshort(135)
#endif
#ifndef Skbdrepeat		
#define Skbdrepeat		DIint(136)
#endif
#ifndef Skbdstatus		
#define Skbdstatus		DIint(137)
#endif
#ifndef Slback		
#define Slback		DIpLayer(138)
#endif
#ifndef Slfront		
#define Slfront		DIpLayer(139)
#endif
#ifndef Smaxaddr		
#define Smaxaddr		DACast(long, 140)
#endif
#ifndef Smouse		
#define Smouse		DCast(struct Mouse, 141)
#endif
#ifndef SNAVAIL		
#define SNAVAIL		DIlong(142)
#endif
#ifndef Snextlong		
#define Snextlong		DIplong(143)
#endif
#ifndef SNLONGS		
#define SNLONGS		DIplong(144)
#endif
#ifndef SPtCurrent		
#define SPtCurrent		DIPoint(145)
#endif
#ifndef Squeues		
#define Squeues		DACast(struct clist , 146)
#endif
#ifndef Sremote		
#define Sremote		DIint(147)
#endif
#ifndef Ssavecur		
#define Ssavecur		DIPoint(148)
#endif
#ifndef Sstdcurtab		
#define Sstdcurtab		DACast(struct curtab , 149)
#endif
#ifndef Sublocked		
#define Sublocked		DIint(150)
#endif
#ifndef Susercurtab		
#define Susercurtab		DACast(struct curtab , 151)
#endif
/*
* WARNING *** add new entries to the table after
* the last entry here.
*/
#ifndef Sbotbits		
#define Sbotbits		DACast(long, 152)
#endif
#ifndef Sbaud_speeds	
#define Sbaud_speeds	DACast(char, 153)
#endif
#ifndef Stopbits		
#define Stopbits		DACast(long, 154)
#endif
#ifndef Sver_str
#define Sver_str	DCast(char *, 155)
#endif
#ifndef Skbdrpt
#define	Skbdrpt			FIvoid(157)
#endif
#ifndef SSetupFlag
#define	SSetupFlag			DIint(158)
#endif
#ifndef	SC_crosshair
#define SC_crosshair	DITex16(159)
#endif
#ifndef	SC_clock	
#define SC_clock	DITex16(160)
#endif
#ifndef	SC_arrows
#define SC_arrows       DITex16(161)
#endif
#ifndef	SC_skull	
#define SC_skull	DITex16(164)
#endif
#ifndef	SC_sweep	
#define SC_sweep	DITex16(165)
#endif
#ifndef	SC_target	
#define SC_target 	DITex16(166)
#endif
#ifndef	SC_confirm 	
#define SC_confirm	DITex16(167)
#endif
#ifndef	SC_cup	
#define SC_cup	DITex16(168)
#endif
#ifndef	SC_deadmouse	
#define SC_deadmouse	DITex16(169)
#endif
#ifndef	ST_background
#define ST_background	DITex(173)
#endif
#ifndef	ST_darkgrey
#define ST_darkgrey	DITex(174)
#endif
#ifndef	ST_grey
#define ST_grey	DITex(175)
#endif
#ifndef	ST_lightgrey
#define ST_lightgrey	DITex(176)
#endif
#ifndef	ST_white
#define ST_white	DITex(177)
#endif
#ifndef	ST_black
#define ST_black	DITex(178)
#endif
#ifndef	ST_checks
#define ST_checks	DITex(179)
#endif
#ifndef Sqsort
#define Sqsort		FIvoid(180)
#endif
#ifndef Spolyf
#define Spolyf		FIint(181)
#endif
/* #ifndef Sjpolyf
   #define Sjpolyf		FIint(182)
   #endif
 --- No more 'j' routine, said BTL --- 
*/
#ifndef Sptinpoly
#define Sptinpoly	FIint(183)
#endif
#ifndef	Sdownback	
#define Sdownback	FIint(185)
#endif
#ifndef Sprcvchar
#define	Sprcvchar		FIint(186)
#endif
#ifndef Slogports
#define	Slogports		FIint(187)
#endif
#ifndef Shostputc    
#define	Shostputc		FIint(188)
#endif
#ifndef Shostgetc    
#define	Shostgetc		FIint(189)
#endif
#ifndef Sstopbreak
#define	Sstopbreak		FIint(190)
#endif
#ifndef Sstopdisc
#define	Sstopdisc		FIint(191)
#endif
#ifndef Saux1inch 
#define	Saux1inch		FIint(192)
#endif
#ifndef Saux1outch
#define	Saux1outch		FIint(193)
#endif
#ifndef Saux2inch 
#define	Saux2inch		FIint(194)
#endif
#ifndef Saux2outch
#define	Saux2outch		FIint(195)
#endif
#ifndef Saciapoke
#define	Saciapoke		FIint(196)
#endif
#ifndef Saciagetc
#define	Saciagetc		FIint(197)
#endif
#ifndef Saciaputc
#define	Saciaputc		FIint(198)
#endif
#ifndef Sacioctl 
#define	Sacioctl		FIint(199)
#endif
#ifndef SsccApoke
#define	SsccApoke		FIint(200)
#endif
#ifndef SsccAgetc
#define	SsccAgetc		FIint(201)
#endif
#ifndef SsccAputc
#define	SsccAputc		FIint(202)
#endif
#ifndef SsccAioctl
#define	SsccAioctl		FIint(203)
#endif
#ifndef SsccBpoke
#define	SsccBpoke		FIint(204)
#endif
#ifndef SsccBgetc
#define	SsccBgetc		FIint(205)
#endif
#ifndef SsccBputc
#define	SsccBputc		FIint(206)
#endif
#ifndef SsccBioctl
#define	SsccBioctl		FIint(207)
#endif
#ifndef Ssopoke  
#define	Ssopoke 		FIint(208)
#endif
#ifndef Ssopbaud
#define	Ssopbaud		FIint(209)
#endif
#ifndef Sdevtab
#define	Sdevtab		DACast(struct physops , 210)
#endif
#ifndef Shostops
#define	Shostops	DCast(struct physops *, 211)
#endif
#ifndef Sprntops
#define	Sprntops	DCast(struct physops *, 212)
#endif
#ifndef Saux1ops 
#define	Saux1ops 	DCast(struct physops *, 213)
#endif
#ifndef Saux2ops 
#define	Saux2ops 	DCast(struct physops *, 214)
#endif
#ifndef Ssccinit 
#define	Ssccinit 	FIint(215)
#endif
#ifndef Sprobe_io
#define	Sprobe_io 	FIint(216)
#endif
#ifndef	Saciarxint
#define	Saciarxint	FIint(217)
#endif
#ifndef Sobsclean
#define Sobsclean	FIint(218)
#endif
#ifndef abs
#define abs(a)	Sabs(a)
#endif
#ifndef aciapaws
#define aciapaws()	Saciapaws()
#endif
#ifndef aciainit
#define aciainit(a)	Saciainit(a)
#endif
#ifndef aciatrint
#define aciatrint()	Saciatrint()
#endif
#ifndef add
#define add(a, b)	Sadd(a, b)
#endif
#ifndef addr
#define addr(a, b)	Saddr(a, b)
#endif
#ifndef alloc
#define alloc(a)	Salloc(a)
#endif
#ifndef allocinit
#define allocinit(a, b)	Sallocinit(a, b)
#endif
#ifndef auto1
#define auto1()	Sauto1()
#endif
#ifndef auto2
#define auto2()	Sauto2()
#endif
#ifndef auto4
#define auto4()	Sauto4()
#endif
#ifndef balloc
#define balloc(a)	Sballoc(a)
#endif
#ifndef bfree
#define bfree(a)	Sbfree(a)
#endif
#ifndef binit
#define binit()	Sbinit()
#endif
#ifndef bitblt
#define bitblt(a, b, c, d, e)	Sbitblt(a, b, c, d, e)
#endif
#ifndef pfkey
#define pfkey(a, b, c)	Spfkey(a, b, c)  /* bramgetstr */
#endif
#ifndef bramputstr
#define bramputstr(a, b)	Sbramputstr(a, b)
#endif
#ifndef ceil
#define ceil(a, b)	Sceil(a, b)
#endif
#ifndef checkbram
#define checkbram()	Scheckbram()
#endif
#ifndef cursallow
#define cursallow()	Scursallow()
#endif
#ifndef cursblt
#define cursblt(a, b, c)	Scursblt(a, b, c)
#endif
#ifndef cursinhibit
#define cursinhibit()	Scursinhibit()
#endif
#ifndef cursinit
#define cursinit()	Scursinit()
#endif
#ifndef cursset
#define cursset(a)	Scursset(a)
#endif
#ifndef cursswitch
#define cursswitch(a)	Scursswitch(a)
#endif
#ifndef dellayer
#define dellayer(a)	Sdellayer(a)
#endif
#ifndef disconnect
#define disconnect()	Sdisconnect()
#endif
#ifndef div
#define div(a, b)	Sdiv(a, b)
#endif
#ifndef eqpt
#define eqpt(a, b)	Seqpt(a, b)
#endif
#ifndef eqrect
#define eqrect(a, b)	Seqrect(a, b)
#endif
#ifndef excep_int
#define excep_int()	Sexcep_int()
#endif
#ifndef excep_norm
#define excep_norm()	Sexcep_norm()
#endif
#ifndef excep_proc
#define excep_proc()	Sexcep_proc()
#endif
#ifndef excep_stack
#define excep_stack()	Sexcep_stack()
#endif
#ifndef excep_msg
#define excep_msg(a, b, c, d)	Sexcep_msg(a, b, c, d)
#endif
#ifndef floor
#define floor(a, b)	Sfloor(a, b)
#endif
#ifndef free
#define free(a)	Sfree(a)
#endif
#ifndef freeall
#define freeall(a)	Sfreeall(a)
#endif
#ifndef gcalloc
#define gcalloc(a, b)	Sgcalloc(a, b)
#endif
#ifndef gcfree
#define gcfree(a)	Sgcfree(a)
#endif
#ifndef gcfreeall
#define gcfreeall(a)	Sgcfreeall(a)
#endif
#ifndef gcinit
#define gcinit()	Sgcinit()
#endif
#ifndef getnum
#define getnum()	Sgetnum()
#endif
#ifndef inset
#define inset(a, b)	Sinset(a, b)
#endif
#ifndef jline
#define jline(a, b)	Sjline(a, b)
#endif
#ifndef jlineto
#define jlineto(a, b)	Sjlineto(a, b)
#endif
#ifndef jmove
#define jmove(a)	Sjmove(a)
#endif
#ifndef jmoveto
#define jmoveto(a)	Sjmoveto(a)
#endif
#ifndef jpoint
#define jpoint(a, b)	Sjpoint(a, b)
#endif
#ifndef jrectf
#define jrectf(a, b)	Sjrectf(a, b)
#endif
#ifndef jsegment
#define jsegment(a, b, c)	Sjsegment(a, b, c)
#endif
#ifndef jstring
#define jstring(a)	Sjstring(a)
#endif
#ifndef jstrwidth
#define jstrwidth(a)	Sjstrwidth(a)
#endif
#ifndef jtexture
#define jtexture(a, b, c)	Sjtexture(a, b, c)
#endif
#ifndef kbdchar
#define kbdchar()	Skbdchar()
#endif
#ifndef kbdinit
#define kbdinit()	Skbdinit()
#endif
#ifndef kgetc
#define kgetc()	Skgetc()
#endif
#ifndef layerop
#define layerop(a, b, c, d,e,f)	Slayerop(a, b, c, d,e,f)
#endif
#ifndef lbitblt
#define lbitblt(a, b, c, d, e)	Slbitblt(a, b, c, d, e)
#endif
#ifndef lblt
#define lblt(a, b, c, d, e)	Slblt(a, b, c, d, e)
#endif
#ifndef Lbox
#define Lbox(a)	SLbox(a)
#endif
#ifndef Lgrey
#define Lgrey(a)	SLgrey(a)
#endif
#ifndef load
#define load()	Sload()
#endif
#ifndef lpoint
#define lpoint(a, b, c)	Slpoint(a, b, c)
#endif
#ifndef lrectf
#define lrectf(a, b, c)	Slrectf(a, b, c)
#endif
#ifndef lsegment
#define lsegment(a, b, c, d)	Slsegment(a, b, c, d)
#endif
#ifndef ltexture
#define ltexture(a, b, c, d)	Sltexture(a, b, c, d)
#endif
#ifndef vitty
#define vitty()	Svitty()
#endif
#ifndef menuhit
#define menuhit(a, b)	Smenuhit(a, b)
#endif
#ifndef mul
#define mul(a, b)	Smul(a, b)
#endif
#ifndef nap
#define nap(a)	Snap(a)
#endif
#ifndef newlayer
#define newlayer(a)	Snewlayer(a)
#endif
#ifndef own
#define own()	Sown()
#endif
#ifndef pfedit
#define pfedit(a)	Spfedit(a)
#endif
#ifndef point
#define point(a, b, c)	Spoint(a, b, c)
#endif
#ifndef Pt
#define Pt(a, b)	SPt(a, b)
#endif
#ifndef ptinrect
#define ptinrect(a, b)	Sptinrect(a, b)
#endif
#ifndef qclear
#define qclear(a)	Sqclear(a)
#endif
#ifndef qgetc
#define qgetc(a)	Sqgetc(a)
#endif
#ifndef qinit
#define qinit()	Sqinit()
#endif
#ifndef qputc
#define qputc(a, b)	Sqputc(a, b)
#endif
#ifndef qputstr
#define qputstr(a, b)	Sqputstr(a, b)
#endif
#ifndef raddp
#define raddp(a, b)	Sraddp(a, b)
#endif
#ifndef rcvchar
#define rcvchar()	Srcvchar()
#endif
#ifndef realalloc
#define realalloc(a, b)	Srealalloc(a, b)
#endif
#ifndef realballoc
#define realballoc(a, b)	Srealballoc(a, b)
#endif
#ifndef realgcalloc
#define realgcalloc(a, b, c)	Srealgcalloc(a, b, c)
#endif
#ifndef reboot
#define reboot()	Sreboot()
#endif
#ifndef Rect
#define Rect(a, b, c, d)	SRect(a, b, c, d)
#endif
#ifndef rectclip
#define rectclip(a, b)	Srectclip(a, b)
#endif
#ifndef rectf
#define rectf(a, b, c)	Srectf(a, b, c)
#endif
#ifndef rectXrect
#define rectXrect(a, b)	SrectXrect(a, b)
#endif
#ifndef ringbell
#define ringbell()	Sringbell()
#endif
#ifndef Rpt
#define Rpt(a, b)	SRpt(a, b)
#endif
#ifndef rsubp
#define rsubp(a, b)	Srsubp(a, b)
#endif
#ifndef screenswap
#define screenswap(a, b, c)	Sscreenswap(a, b, c)
#endif
#ifndef scroll
#define scroll(a, b, c)	Sscroll(a, b, c)
#endif
#ifndef segment
#define segment(a, b, c, d)	Ssegment(a, b, c, d)
#endif
#ifndef sendbreak
#define sendbreak()	Ssendbreak()
#endif
#ifndef sendchar
#define sendchar(a)	Ssendchar(a)
#endif
#ifndef setbram
#define setbram()	Ssetbram()
#endif
#ifndef setup
#define setup()	Ssetup()
#endif
#ifndef spl0
#define spl0()	Sspl0()
#endif
#ifndef spl1
#define spl1()	Sspl1()
#endif
#ifndef spl4
#define spl4()	Sspl4()
#endif
#ifndef spl5
#define spl5()	Sspl5()
#endif
#ifndef spl6
#define spl6()	Sspl6()
#endif
#ifndef spl7
#define spl7()	Sspl7()
#endif
#ifndef splx
#define splx(a)	Ssplx(a)
#endif
#ifndef string
#define string(a, b, c, d, e)	Sstring(a, b, c, d, e)
#endif
#ifndef strlen
#define strlen(a)	Sstrlen(a)
#endif
#ifndef strwidth
#define strwidth(a, b)	Sstrwidth(a, b)
#endif
#ifndef sub
#define sub(a, b)	Ssub(a, b)
#endif
#ifndef test32
#define test32()	Stest32()
#endif
#ifndef texture
#define texture(a, b, c, d)	Stexture(a, b, c, d)
#endif
#ifndef trdisable
#define trdisable()	Strdisable()
#endif
#ifndef trenable
#define trenable()	Strenable()
#endif
#ifndef tstdcd
#define tstdcd()	Ststdcd()
#endif
#ifndef upfront
#define upfront(a)	Supfront(a)
#endif
#ifndef version
#define version()	Sversion()
#endif
#ifndef wait
#define wait(a)	Swait(a)
#endif
#ifndef allocb	
#define allocb	Sallocb
#endif
#ifndef allocendp	
#define allocendp	Sallocendp
#endif
#ifndef allocstartp	
#define allocstartp	Sallocstartp
#endif
#ifndef blocked	
#define blocked	Sblocked
#endif
#ifndef cbufs	
#define cbufs	Scbufs
#endif
#ifndef cur	
#define cur	Scur
#endif
#ifndef cursor	
#define cursor	Scursor
#endif
#ifndef curtabp	
#define curtabp	Scurtabp
#endif
#ifndef defont	
#define defont	Sdefont
#endif
#ifndef display	
#define display	Sdisplay
#endif
#ifndef dtr	
#define dtr	Sdtr
#endif
#ifndef ENDAREA	
#define ENDAREA	SENDAREA
#endif
#ifndef freelist	
#define freelist	Sfreelist
#endif
#ifndef interrupt	
#define interrupt	Sinterrupt
#endif
#ifndef kbdrepeat	
#define kbdrepeat	Skbdrepeat
#endif
#ifndef kbdstatus	
#define kbdstatus	Skbdstatus
#endif
#ifndef lback	
#define lback	Slback
#endif
#ifndef lfront	
#define lfront	Slfront
#endif
#ifndef maxaddr	
#define maxaddr	Smaxaddr
#endif
#ifndef mouse	
#define mouse	Smouse
#endif
#ifndef NAVAIL	
#define NAVAIL	SNAVAIL
#endif
#ifndef nextlong	
#define nextlong	Snextlong
#endif
#ifndef NLONGS	
#define NLONGS	SNLONGS
#endif
#ifndef PtCurrent	
#define PtCurrent	SPtCurrent
#endif
#ifndef queues	
#define queues	Squeues
#endif
#ifndef remote	
#define remote	Sremote
#endif
#ifndef savecur	
#define savecur	Ssavecur
#endif
#ifndef stdcurtab	
#define stdcurtab	Sstdcurtab
#endif
#ifndef ublocked	
#define ublocked	Sublocked
#endif
#ifndef usercurtab	
#define usercurtab	Susercurtab
#endif
#ifndef botbits	
#define botbits	Sbotbits
#endif
#ifndef baud_speeds 
#define baud_speeds Sbaud_speeds
#endif
#ifndef topbits	
#define topbits	Stopbits
#endif
#ifndef ver_str
#define ver_str Sver_str
#endif
#ifndef kbdrpt
#define kbdrpt	Skbdrpt
#endif
#ifndef	SetupFlag
#define SetupFlag SSetupFlag
#endif
#ifndef	C_crosshair
#define C_crosshair	SC_crosshair
#endif
#ifndef	C_clock	
#define C_clock		SC_clock	
#endif
#ifndef	C_arrows
#define C_arrows	SC_arrows
#endif
#ifndef	C_skull	
#define C_skull		SC_skull	
#endif
#ifndef	C_sweep	
#define C_sweep		SC_sweep	
#endif
#ifndef	C_target	
#define C_target		SC_target	
#endif
#ifndef	C_confirm	
#define C_confirm		SC_confirm	
#endif
#ifndef	C_cup	
#define C_cup		SC_cup	
#endif
#ifndef	C_deadmouse	
#define C_deadmouse		SC_deadmouse	
#endif
#ifndef	T_background	
#define T_background		ST_background
#endif
#ifndef	T_darkgrey
#define T_darkgrey		ST_darkgrey
#endif
#ifndef	T_grey
#define T_grey			ST_grey
#endif
#ifndef	T_lightgrey
#define T_lightgrey		ST_lightgrey
#endif
#ifndef	T_white
#define T_white			ST_white
#endif
#ifndef	T_black
#define T_black			ST_black
#endif
#ifndef	T_checks
#define T_checks		ST_checks
#endif
#ifndef qsort
#define qsort		Sqsort		
#endif
#ifndef polyf
#define polyf		Spolyf	
#endif
/*  #ifndef jpolyf
    #define jpolyf		Sjpolyf	
    #endif
*/
#ifndef ptinpoly
#define ptinpoly	Sptinpoly
#endif
#ifndef downback
#define downback	Sdownback
#endif
#ifndef prcvchar
#define	prcvchar	Sprcvchar
#endif
#ifndef logports
#define	logports	Slogports
#endif
#ifndef hostputc    
#define	hostputc	Shostputc
#endif
#ifndef hostgetc    
#define	hostgetc	Shostgetc
#endif
#ifndef stopbreak
#define	stopbreak	Sstopbreak
#endif
#ifndef stopdisc
#define	stopdisc	Sstopdisc
#endif
#ifndef aux1inch 
#define	aux1inch	Saux1inch
#endif
#ifndef aux1outch
#define	aux1outch	Saux1outch
#endif
#ifndef aux2inch 
#define	aux2inch	Saux2inch
#endif
#ifndef aux2outch
#define	aux2outch	Saux2outch
#endif
#ifndef aciapoke
#define	aciapoke	Saciapoke
#endif
#ifndef aciagetc
#define	aciagetc	Saciagetc
#endif
#ifndef aciaputc
#define	aciaputc	Saciaputc
#endif
#ifndef acioctl 
#define	acioctl	Sacioctl
#endif
#ifndef sccApoke
#define	sccApoke	SsccApoke
#endif
#ifndef sccAgetc
#define	sccAgetc	SsccAgetc
#endif
#ifndef sccAputc
#define	sccAputc	SsccAputc
#endif
#ifndef sccAioctl
#define	sccAioctl	SsccAioctl
#endif
#ifndef sccBpoke
#define	sccBpoke	SsccBpoke
#endif
#ifndef sccBgetc
#define	sccBgetc	SsccBgetc
#endif
#ifndef sccBputc
#define	sccBputc	SsccBputc
#endif
#ifndef sccBioctl
#define	sccBioctl	SsccBioctl
#endif
#ifndef sopoke  
#define	sopoke	Ssopoke
#endif
#ifndef sopbaud
#define	sopbaud	Ssopbaud
#endif
#ifndef devtab
#define	devtab	Sdevtab
#endif
#ifndef hostops
#define	hostops	Shostops
#endif
#ifndef prntops
#define	prntops	Sprntops
#endif
#ifndef aux1ops 
#define	aux1ops	Saux1ops
#endif
#ifndef aux2ops 
#define	aux2ops	Saux2ops
#endif
#ifndef sccinit 
#define	sccinit	Ssccinit
#endif
#ifndef probe_io
#define	 probe_io	Sprobe_io
#endif
#ifndef aciarxint
#define	aciarxint	Saciarxint
#endif
#ifndef obsclean
#define obsclean	Sobsclean
#endif
#ifndef psendchar
#define psendchar		Spsendchar
#endif
#ifndef piohint
#define piohint 		Spiohint  
#endif
#ifndef qsetbram
#define qsetbram		Sqsetbram 
#endif
