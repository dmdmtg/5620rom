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
#ifndef	OPTIONS_H

#define	OPTIONS_H	1

#define	MAXVALS	8
#define PFEDITMENU	72	/* location in SetupTree of pf edit menu */

#define	S_BORDER	1	/* size of horizontal border around PF key pads */

#define NEEDS_IO_CARD	1	/* FLag for options requiring I/O Card */

#undef pfedit
int	pfedit();


struct valus {	/* can't use this name */
	unsigned char	*va_bram;	/* address of option in Bram */
	char	*va_names[MAXVALS];	/* Option value strings in order */
	int	(*va_init)();		/* initialization function */
    } ;

struct setuptree {
	char	*su_lbl1,
		*su_lbl2;	/* Function Key labels (lines 1 and 2) */
	char	su_keytype;	/* Type of key */
	int	su_val;		/* because you can't initialize unions */
	char	su_iocard;	/* TRUE if needs I/O card */
    };

#ifdef	OWN_SETUP_H

#undef	sopbaud
#undef	sccinit
#undef	logports

int	eia_init();
int	colorinit();
int	toneinit();
int	sccinit();
int	sopbaud();
int	logports();

struct valus SetupVals[] = {

	{ &VALABAUD,	{ "1200","2400","4800","9600","19200","300", 0},eia_init},
	{ &VALRETKEY,	{ "CR", "LF", "CR/LF", 0}, (int (*)())NULL},
	{ &VALNEWLINE,	{ "Index", "NL ", 0}, (int (*)())NULL},
	{ &VALADUPLEX,	{ "Full", "Half", 0}, (int (*)())NULL},
 	{ &VALKEYTONE,	{ "On", "Off", 0}, toneinit},
 	{ &VALSCREENCOLOR,{ "Dark", "Light", 0}, colorinit},
	{ &VALAPRTY,	{ "None", "Odd", "Even", 0}, eia_init},
	{ &VALABITS,	{ "8", "7", 0}, eia_init},
	{ &VALATYPE,	{ "RS-232", 0}, (int (*)())NULL},
	{ &VALBBAUD,	{ "1200","2400","4800","9600","19200","300", 0}, sopbaud},
	{ &VALBPRTY,	{ "Even", 0},(int (*)())NULL},
	{ &VALBBITS,	{ "8", 0}, (int (*)())NULL},
	{ &VALBTYPE,	{ "RS-232", 0}, (int (*)())NULL},
	{ &VALCBAUD,	{ "1200","2400","4800","9600","19200","300", 0}, sccinit},
	{ &VALCPRTY,	{ "None", "Odd", "Even", 0},sccinit},
	{ &VALCBITS,	{ "8", "7", 0},sccinit},
	{ &VALCTYPE,	{ "RS-232", "RS-422", 0},sccinit},
	{ &VALPHOST,	{ "Port A", "Port C", "Port D", 0}, logports},
	{ &VALENCODING,	{ "Off", "On", 0}, (int (*)())NULL},
	{ &VALPPRNT,	{ "Port A", "Port C", "Port D", "Port B", 0}, logports},
	{ &VALMOUSE,	{ "Right", "Left", 0}, (int (*)())NULL},
	{ &VALREPEAT,	{ "15 cps", "20 cps", "30 cps", "60 cps", 0}, (int (*)())NULL},
	{ &VALCURSOR,	{ "No Blink", "Blink", 0}, (int (*)())NULL},
	{ &VALCONTROLS,	{ "Visible", "Invisible", "Spaces", 0},  (int (*)())NULL},
	{ &VALDBAUD,	{ "1200","2400","4800","9600","19200","300", 0}, sccinit},
	{ &VALDPRTY,	{ "None", "Odd", "Even", 0}, sccinit},
	{ &VALDBITS,	{ "8", "7", 0}, sccinit},
	{ &VALDTYPE,	{ "RS-232", "RS-422", 0}, sccinit},
	{ &VALPAUX1,	{ "Port A", "Port C", "Port D", "Port B", 0}, logports},
	{ &VALPAUX2,	{ "Port A", "Port C", "Port D", "Port B", 0}, logports},
	{ &NOAUTOFLOW,	{ "On", "Off", 0}, (int (*)())NULL},
	{ &A_RECFLOW,	{ "Off", "On", 0}, (int (*)())NULL},
	{ &C_RECFLOW,	{ "Off", "On", 0}, (int (*)())NULL},
	{ &D_RECFLOW,	{ "Off", "On", 0}, (int (*)())NULL},
	{ &VALSENDCTRLS,{ "No", "Yes", 0}, (int (*)())NULL}
   };	/* add entries at the end only ! */


struct setuptree SetupTree[] = {

	{"Port","Options",(char)1,(int)8},
	{"Host","Options",(char)1,(int)40},
	{"Prefer-","ences",(char)1,(int)56},

	{"Edit PF","Keys",(char)1,(int)72},
	{"Printer","Options",(char)1,(int)48, NEEDS_IO_CARD},

	{"AUX Port","Options",(char)1,(int)88, NEEDS_IO_CARD},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},


/*8*/
	{"Main","Menu",(char)1,(int)0},
	{"Port A","Options",(char)1,(int)16},
	{"Port B","Options",(char)1,(int)24},

	{"Port C","Options",(char)1,(int)32, NEEDS_IO_CARD},
	{"Port D","Options",(char)1,(int)80, NEEDS_IO_CARD},

	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},


/*16*/
	{"Main","Menu",(char)1,(int)0},
	{"Speed",(char *)NULL,(char)2,(int)&SetupVals[0]},
	{"Parity",(char *)NULL,(char)2,(int)&SetupVals[6]},

	{"Bits/Char",(char *)NULL,(char)2,(int)&SetupVals[7]},
	{"Type",(char *)NULL,(char)2,(int)&SetupVals[8]},

	{"Rcv Flow",(char *)NULL,(char)2,(int)&SetupVals[31]},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},


/*24*/
	{"Main","Menu",(char)1,(int)0},
	{"Speed",(char *)NULL,(char)2,(int)&SetupVals[9]},
	{"Parity",(char *)NULL,(char)2,(int)&SetupVals[10]},

	{"Bits/Char",(char *)NULL,(char)2,(int)&SetupVals[11]},
	{"Type",(char *)NULL,(char)2,(int)&SetupVals[12]},

	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},


/*32*/
	{"Main","Menu",(char)1,(int)0},
	{"Speed",(char *)NULL,(char)2,(int)&SetupVals[13], NEEDS_IO_CARD},
	{"Parity",(char *)NULL,(char)2,(int)&SetupVals[14], NEEDS_IO_CARD},

	{"Bits/Char",(char *)NULL,(char)2,(int)&SetupVals[15], NEEDS_IO_CARD},
	{"Type",(char *)NULL,(char)2,(int)&SetupVals[16], NEEDS_IO_CARD},

	{"Rcv Flow",(char *)NULL,(char)2,(int)&SetupVals[32], NEEDS_IO_CARD},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},


/*40*/
	{"Main","Menu",(char)1,(int)0},
	{"Duplex",(char *)NULL,(char)2,(int)&SetupVals[3]},
	{"Encoding",(char *)NULL,(char)2,(int)&SetupVals[18]},

	{"Gen Flow",(char *)NULL,(char)2,(int)&SetupVals[30]},
	{"Pass Flow",(char *)NULL,(char)2,(int)&SetupVals[34]},

	{"Host",(char *)NULL,(char)2,(int)&SetupVals[17], NEEDS_IO_CARD},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},


/*48*/
	{"Main","Menu",(char)1,(int)0},
	{"Printer",(char *)NULL,(char)2,(int)&SetupVals[19], NEEDS_IO_CARD},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},

	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},

	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},


/*56*/
	{"Main","Menu",(char)1,(int)0},
	{"Screen",(char *)NULL,(char)2,(int)&SetupVals[5]},
	{"Controls",(char *)NULL,(char)2,(int)&SetupVals[23]},

	{"Key Tone",(char *)NULL,(char)2,(int)&SetupVals[4]},
	{"Cursor",(char *)NULL,(char)2,(int)&SetupVals[22]},

	{"Repeat",(char *)NULL,(char)2,(int)&SetupVals[21]},
	{"Mouse",(char *)NULL,(char)2,(int)&SetupVals[20]},
	{"More",(char *)NULL,(char)1,(int)64},


/*64*/
	{"Main","Menu",(char)1,(int)0},
	{"Newline",(char *)NULL,(char)2,(int)&SetupVals[2]},
	{"Return",(char *)NULL,(char)2,(int)&SetupVals[1]},

	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},

	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{"More",(char *)NULL,(char)1,(int)56},


/*72*/	{"Edit","f1",(char)3,(int)pfedit},
	{"Edit","f2",(char)3,(int)pfedit},
	{"Edit","f3",(char)3,(int)pfedit},

	{"Edit","f4",(char)3,(int)pfedit},
	{"Edit","f5",(char)3,(int)pfedit},

	{"Edit","f6",(char)3,(int)pfedit},
	{"Edit","f7",(char)3,(int)pfedit},
	{"Edit","f8",(char)3,(int)pfedit},

/*80*/
	{"Main","Menu",(char)1,(int)0},
	{"Speed",(char *)NULL,(char)2,(int)&SetupVals[24], NEEDS_IO_CARD},
	{"Parity",(char *)NULL,(char)2,(int)&SetupVals[25], NEEDS_IO_CARD},

	{"Bits/Char",(char *)NULL,(char)2,(int)&SetupVals[26], NEEDS_IO_CARD},
	{"Type",(char *)NULL,(char)2,(int)&SetupVals[27], NEEDS_IO_CARD},

	{"Rcv Flow",(char *)NULL,(char)2,(int)&SetupVals[33], NEEDS_IO_CARD},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},

/*88*/
	{"Main","Menu",(char)1,(int)0},
	{"AUX 1",(char *)NULL,(char)2,(int)&SetupVals[28], NEEDS_IO_CARD},
	{"AUX 2",(char *)NULL,(char)2,(int)&SetupVals[29], NEEDS_IO_CARD},

	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},

	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},
	{(char *)NULL,(char *)NULL,(char)0,(int)0},

};	/* add entries at the end only! */

#else

extern struct setuptree SetupTree[];
extern struct valus SetupVals[];

#endif

#endif
