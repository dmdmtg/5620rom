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
/*#C  This program, aconv.c version 1.18, was created on 3/29/82.		*/
/*#C									*/
/*#C  This program is for converting various input file formats into	*/
/*#C  various other output formats, generally to produce files suitable	*/
/*#C  for programming proms on Intel MDS systems or Data-I/O machines.	*/
/*  The following options exist:					*/
/*									*/
/*	-c	This indicates the the normal checksum computing	*/
/*		is to be omitted. A checksum is normally computed	*/
/*		over all data in the input file and is stored in place	*/
/*		of the last two bytes of the output.			*/
/*									*/
/*	-if arg	This specifies the input file format as listed below.	*/
/*		Default = a. 						*/
/*	-of arg	This specifies the output file format as listed below.	*/
/*		Default = h.						*/
/*	   a --	Indicates a file in the common object file a.out	*/
/*		format.							*/
/*	   b --	Indicates a file in continuous binary format.		*/
/*	   h --	Indicates a file in Intel ASCII hex format.		*/
/*	   m --	Indicates a file in MICA binary format.			*/
/*	   s --	Indicates a file in Motorola S-record hex format.	*/
/*									*/
/*	-m arg	This specifies the size of the memory to be filled	*/
/*		in hex. If this option is specified, the output		*/
/*		files will be zero padded up to the size given.		*/
/*		If not specified, the data in the input files will	*/
/*		be used to determine the size of the output.		*/
/*									*/
/*	-o arg	This specifies an offset from the start address in 	*/
/*		hex. Output files are created beginning with the file	*/
/*		set containing the address which is the offset amount	*/
/*		from the start address (see -s option below).		*/
/*		Default = 0.						*/
/*									*/
/*	-p arg	This specifies the prom size in K bytes ie. .25, .5, 1,	*/
/*		2, 4, 8, ... 256, for output formats h, s, and a.	*/
/*		Default = 8.						*/
/*									*/
/*	-s arg	This specifies an alternate starting address in		*/
/*		hex and is used for output formats h and s to prevent	*/
/*		null data sets from being created when the first data	*/
/*		is found at an address other than 0 in the input files.	*/
/*		Output files are numbered beginning with the file set	*/
/*		containing the address specified. The checksum is	*/
/*		computed starting with this set of files.		*/
/*		Default = 0.						*/
/*									*/
/*	-w arg	This option specifies the memory width or word size	*/
/*		(in bytes) for machines from 1 to 8 bytes wide. It is	*/
/*		used for input format m and output formats m, h and s.	*/
/*		Default = 2.						*/
/*									*/
/*	infile	This specifies the name of the input file. The		*/
/*		output file is the same as the input file followed	*/
/*		by a three character suffix.				*/
/*		Default = m68a.out					*/
/*									*/


#include <stdio.h>

#define M32SGS 1	/* If set to 1, common object file format a.out files */
			/* can be used with this program. If set to 1 the     */
			/* following header files must be able to be found.  */

#if M32SGS
#include "filehdr.h"
#include "scnhdr.h"
#define IFDFAULT 'a'	/* default for input file format */
#else
#define IFDFAULT 'm'	/* MICA default for non a.out systems */
#endif

#define OFDFAULT 'h'	/* default for output file format */
#define	IDFAULT	"m68a.out"	/* default input file name */
#define ODFAULT 0	/* default offset */
#define PDFAULT 8192L	/* default prom size */
#define SDFAULT 0	/* default start address */
#define WDFAULT 2	/* default word size in bytes */

#ifndef M68MAGIC
#define M68MAGIC 0x6868	/* magic number for a.out files for Motorola 68000 */
#endif
#define M68MAG2 0x6800	/* secondary magic number for M 68000 files */

#define USAGE "usage\naconv [-c -if(type) -of(type) -m(size) -o(amt) -p(size) -s(addr) -w(width) file]"
#define BUFSIZE	1024		/* size of input buffer */
#define MAXSIZE 64 * 1024L	/* max prom size for records with 2 byte addr */
#define RECSIZE	16		/* size of output hex record */
#define ADDSIZE 3		/* size of output S-rec address (2 or 3) */
#define BIGNUM 0x40000000L	/* biggest number I'm willing to handle */
#define SET 1
#define CLEAR 0

char ident[] = "@(#)aconv.c    1.18    3/29/82";
FILE *infp, *tempfp;
long	plist[]={	/* list of acceptable prom sizes */
	256,512,1024,2048,
	4096,8192,16384,
	32768,49152,65536,
	96 * 1024L,128 * 1024L,256 * 1024L};
long	start_addr;	/* starting address of prom images */
long	max_addr;	/* max address in input and output files */
long	size;		/* size of proms for h & s, size of chunk for b & m */
long	cpos;		/* current input and temp file position */
short	width;		/* bytes per word in input file */
char	Ssum;		/* record sum for hex records */
char	iform;		/* used to indicate the input file format */
char	oform;		/* used to indicate the output file format */
char	tflag;		/* test flag */
char	cflag;		/* checksum flag */
char	dflag;		/* flag to indicate null data files */
char	ibuf[BUFSIZE];	/* input buffer */
char	errstring[81];	/* error string storage, one line */
long	memsize;	/* size of memory to fill */



main(argc,argv)

int argc;
char *argv[];

{
	extern FILE *infp, *tempfp;
	extern long	start_addr;	/* starting address of prom images */
	extern long	max_addr;	/* max address in input and output files */
	extern long	size;		/* prom size or chunk size */
	extern long	cpos;		/* current input and temp file position */
	extern short	width;		/* bytes per word in input file */
	extern char	iform;		/* used to indicate the input file format */
	extern char	oform;		/* used to indicate the output file format */
	extern char	tflag;		/* test flag */
	extern char	cflag;		/* checksum flag */
	extern char	dflag;		/* flag to indicate null data files */
	extern char	ibuf[];	/* input buffer */
	extern char	errstring[];	/* error string storage, one line */
#if M32SGS
	struct filehdr fhdr;		/* file header for cof a.out output */
	struct scnhdr shdr;		/* section header for cof a.out output */
#endif
	FILE	*ofp[8];
	FILE	*tmpfile();
	double	atof();
	long	time();
	long	offset;		/* offset from start of prom image */
	long	rec_addr;	/* address of current hex record */
	long	nseg;		/* current segment for ofiles */
	long	tmpsto[2];	/* temp storage for MICA output */
	short	precsize;	/* processing record=RECSIZE*width */
	short	i,j,k;
	char	ofile[8][16];	/* output file name array */
	char	*strptr;	/* pointer to input arg strings */
	char	*strrchr();
	char	*ifile;		/* input filename */
	char	numptr;		/* offset to variable portion of ofile */
	char	ashift;		/* output S-rec address shifter */
	char	addsize;	/* output S-rec address size */
	char	dbytes;		/* num. of bytes in top wd MICA output */
	char	chunks;		/* num. of long chunks in MICA output */
	char	recsize;	/* current record size for h & s outputs */
	char	basedata;	/* holds base address data for h output */
	char	type;		/* type byte for h output */
	char	tform;		/* temp format store */
	char	wflag;		/* flag to indicate written files */
	register short bbcnt;		/* bytes in input buffer */
	register char sum;		/* hex record checksum */


	if(ADDSIZE == 3) addsize = 3;
	else addsize = 2;
	memsize = 0;
	offset = ODFAULT;
	start_addr = SDFAULT;
	size = PDFAULT;
	width = WDFAULT;
	ifile = IDFAULT;
	iform = IFDFAULT;
	oform = OFDFAULT;
	cflag = SET;
	dflag = CLEAR;
	wflag = CLEAR;
/******/tflag = CLEAR;

		/* parse input arguments */
	while (--argc > 0 && (*++argv)[0] == '-') {
		strptr = argv[0]+1;
		if(*(strptr+1) == 'f') {
		    tform = *strptr;
		    if(*(strptr+2) == 0) strptr = (++argv)[0];
		    else strptr += 2;
		    switch(tform) {
			case 'i':	/* input format specification */
			    switch(*strptr) {
#if M32SGS
				case 'a':
#endif
				case 'b':
				case 'h':
				case 'm':
				case 's':
					iform = *strptr;
					break;
				default:
					xit("invalid input format specification",2);
					break;
			    }
			    break;
			case 'o':	/* output format specification */
			    switch(*strptr) {
#if M32SGS
				case 'a':
#endif
				case 'b':
				case 'h':
				case 'm':
				case 's':
					oform = *strptr;
					break;
				default:
					xit("invalid output format specification",3);
					break;
			    }
			    break;
			default:
			    xit(USAGE,4);
			    break;
		    }
		}
		else switch (*strptr) {
			case 't':
				tflag = SET;
				break;
			case 'c':	/* omit checksum processing */
				cflag = CLEAR;
				break;
			case 'w':	/* alternate memory width */
				if(*(++strptr) == 0) {
					strptr = (++argv)[0];
					argc--;
				}
				if(strlen(strptr) > 1)
					xit("invalid width, only 1 through 8 allowed",5);
				switch (*strptr) {
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
						width = *strptr - '0';
						break;
					default:
						xit("invalid width, only 1 through 8 allowed",6);
						break;
				}
				break;
			case 'p':	/* prom size specified */
				if(*(++strptr) == 0) {
				    strptr = (++argv)[0];
				    argc--;
				}
				size = (long)(1024 * atof(strptr));
				i = 0;
				while(size != plist[i])
				    if(++i == sizeof(plist))
					xit("invalid prom size, use .5, 1, 2, 4, ... 64.",7);
				break;
			case 'o':
				if(*(++strptr) == 0) {
					strptr = (++argv)[0];
					argc--;
				}
				if((*(strptr+1) == 'x') || (*(strptr+1) == 'X'))
					strptr += 2;
				if(sscanf(strptr,"%lx",(long *)&offset) == 0)
					xit("bad offset address",9);
				if((offset > BIGNUM) || (offset < 0L))
					xit("offset address to large",10);
				break;
			case 's':
				if(*(++strptr) == 0) {
					strptr = (++argv)[0];
					argc--;
				}
				if((*(strptr+1) == 'x') || (*(strptr+1) == 'X'))
					strptr += 2;
				if(sscanf(strptr,"%lx",(long *)&start_addr) == 0)
					xit("bad start address",11);
				if((start_addr > BIGNUM) || (start_addr < 0L))
					xit("start address to large",12);
				break;
			case 'm':
				if(*(++strptr) == 0) {
					strptr = (++argv)[0];
					argc--;
				}
				if((*(strptr+1) == 'x') || (*(strptr+1) == 'X'))
					strptr += 2;
				if(sscanf(strptr,"%lx",(long *)&memsize) == 0)
					xit("bad memory size",50);
				if((memsize > BIGNUM) || (memsize < 0L))
					xit("memory size to large",51);
				break;
			default:
				xit(USAGE,13);
		}
	}

		/* get input file name if any */
	if(argc > 1) xit(USAGE,14);
	if(argc == 1) ifile = *argv;

	if((iform == 'm') && (start_addr != 0))
		xit("start address must be 0 for MICA format",15);
	if((iform == 'b') && (start_addr != 0))
		xit("start address must be 0 for binary format",16);

		/* open input file */
	if((infp = fopen(ifile,"r")) == NULL) {
		sprintf(errstring,"can't open %s",ifile);
		xit(errstring,17);
	}

		/* open temp file */
	if((tempfp = tmpfile()) == NULL)
		xit("can't open temporary file",18);

		/* create output file names and set size */

	if((strptr = strrchr(ifile,'/')) != NULL)
		ifile = ++strptr;
	switch(oform) {
	    case 'm':
		if(width <= 4) {
		    dbytes = width;
		    chunks = 1;
		}
		else {
		    dbytes = width - 4;
		    chunks = 2;
		}
		size = width;
		strncpy(ofile[0],ifile,9);
		strcat(ofile[0],".mic");
		fprintf(stdout,"creating MICA file %s for %d byte wide memory",ofile[0],width);
		break;
	    case 'b':
		size = 1;
		strncpy(ofile[0],ifile,9);
		strcat(ofile[0],".bin");
		fprintf(stdout,"creating binary file %s",ofile[0]);
		break;
	    case 's':
		if((size > MAXSIZE) && (ADDSIZE == 2))
		    xit("prom size too large for S-record with 2 byte address",58);
	    case 'h':
#if M32SGS
	    case 'a':
#endif
		size = size * width;
		numptr = strlen(ifile) > 9 ? 9 : strlen(ifile);
		for(i=0; i < width; i++) {
		    strncpy(ofile[i],ifile,9);
		    sprintf(&ofile[i][numptr],".%.1d",(width-i-1));
		}
		numptr += 2;
		fprintf(stdout,"creating files for %gK proms in %d byte wide memory",(float)size/1024/width,width);
		break;
	    default:
		xit("unknown output file format, aconv bug",52);
	}

		/* init variables */
	max_addr = 0;
	precsize = RECSIZE * width;
	nseg = (start_addr + offset) / size - (start_addr / size);
	offset = ((start_addr + offset) / size) * size;
	start_addr = (start_addr / size) * size;
	if(offset > 0) fprintf(stdout," starting at %lX hex",offset);
	fprintf(stdout,"\n");

			/* process input file */

	switch (iform) {
		case 's':	/* input S record format */
			inptH();
			break;
		case 'h':	/* input hex file format */
			inptH();
			break;
		case 'b':	/* input binary file format */
			inptB();
			break;
		case 'm':	/* input MICA format */
			inptM();
			break;
#if M32SGS
		case 'a':	/* input COF a.out format */
			inptA();
			break;
#endif
		default:
			xit("unknown input file format, aconv program bug",19);
	}


		/* processing for output files */

	if(memsize > 0) {
		if(max_addr > memsize) xit("data found beyond memory size specified",53);
		else max_addr = memsize;
	}
	for(cpos=0; cpos<max_addr; cpos+=size);
	max_addr = cpos;
	cpos = start_addr;
	rewind(tempfp);
/******/if(tflag) fprintf(stdout,"nseg=%lX size=%lX start_addr=%lX offset=%lX max_addr=%lX\n",nseg,size,start_addr,offset,max_addr);


		/* processing for MICA output files */
	if(oform == 'm') {
		if((ofp[0] = fopen(ofile[0],"w")) == NULL)
			xit("can't open output file",20);
		while(cpos < max_addr) {
		    if(wflag == CLEAR) dflag = CLEAR;
		    bbcnt = dosum(width);
		    if(cpos > offset) {
			wflag = SET;
			tmpsto[0] = 0;
			for(i=0; i<dbytes; ++i) {
				tmpsto[0] <<= 8;
				tmpsto[0] |= (long)ibuf[i] & 0xff;
			}
			if(width > 4) {
				tmpsto[1] = 0;
				for(; i<width; ++i) {
				    tmpsto[1] <<= 8;
				    tmpsto[1] |= (long)ibuf[i] & 0xff;
				}
			}
			if(fwrite(tmpsto,sizeof(long),chunks,ofp[0]) < 0)
				xit("can't write output file",54);
			if((width == 1) && (bbcnt == 2)) {
				tmpsto[0] = (long)ibuf[1] & 0xff;
				fwrite(tmpsto,sizeof(long),1,ofp[0]);
			}
		    }
		}
		if(wflag == CLEAR) xit("no data found after offset, null file created",55);
		if(dflag == CLEAR) fprintf(stdout,"warning: %s contains no non-zero data!\n",ofile[0]);
		fclose(ofp[0]);
		exit(0);
	}

		/* processing for binary output files */
	else if(oform == 'b') {
		if((ofp[0] = fopen(ofile[0],"w")) == NULL)
			xit("can't open output file",20);
		while(cpos < max_addr) {
		    if(wflag == CLEAR) dflag = CLEAR;
		    bbcnt = dosum(BUFSIZE - 2);
		    if(cpos > offset) {
			wflag = SET;
			if(fwrite(ibuf,1,bbcnt,ofp[0]) < 0)
			    xit("can't write output file",21);
		    }
		}
		if(wflag == CLEAR) xit("no data found after offset, null file created",56);
		if(dflag == CLEAR) fprintf(stdout,"warning: %s contains no non-zero data!\n",ofile[0]);
		fclose(ofp[0]);
		exit(0);
	}

#if M32SGS
		/* processing for a.out output files */
	else if(oform == 'a') {
	    fhdr.f_magic = M68MAGIC;
	    fhdr.f_nscns = 1;
	    fhdr.f_timdat = time((long *)0);
#if pdp11
	    fhdr.f_flags = F_RELFLG|F_EXEC|F_LSYMS|F_AR16WR;
#endif
#if vax
	    fhdr.f_flags = F_RELFLG|F_EXEC|F_LSYMS|F_AR32WR;
#endif

	    strcpy(shdr.s_name,_TEXT);
	    shdr.s_size = size;
	    shdr.s_scnptr = sizeof(struct filehdr) + sizeof(struct scnhdr);

	    --numptr;
	    while(cpos < max_addr) {
		if((cpos >= offset) && ((cpos % size) == 0)) {
/******/		if(tflag) fprintf(stdout,"writing output files at cpos=%lX\n",cpos);
			wflag = SET;
			sprintf(&ofile[width-1][numptr],"a%.2ld",nseg);
			fprintf(stdout, "%s\n",ofile[width-1]);
			if((ofp[0] = fopen(ofile[width-1],"w")) == NULL)
				xit("can't open output file",24);
			++nseg;
			dflag = CLEAR;
			shdr.s_paddr = cpos;
			shdr.s_vaddr = cpos;
			if(fwrite(&fhdr,sizeof(struct filehdr),1,ofp[0]) <= 0)
			    xit("can't write output file",59);
			if(fwrite(&shdr,sizeof(struct scnhdr),1,ofp[0]) <= 0)
			    xit("can't write output file",60);
		}

		dosum(256);

			/* if within range output prom hex files */
		if(cpos > offset) {
		    if(fwrite(ibuf,1,256,ofp[0]) < 0)
			xit("can't write output file",58);
			/* if done with current block, close file */
		    if((cpos % size) == 0) {
			fclose(ofp[0]);
			if(dflag == CLEAR) fprintf(stdout,"warning: %s co.tzins no non-zero data!\n",ofile[width-1]);
		    }
		}
	    }
	    if(wflag != SET) xit("no data found after offset, no output files created",26);
	    exit(0);	/* normal exit */
	}
#endif

		/* processing for hex and S-record output files */

	basedata = 0;
	type = 0;
	recsize = RECSIZE;
	while(cpos < max_addr) {
		if((cpos >= offset) && ((cpos % size) == 0)) {
/******/		if(tflag)
				fprintf(stdout,"writing output files at cpos=%lX\n",cpos);

			/* open destination files */
			wflag = SET;
			for(i=0; i<width; i++) {
				sprintf(&ofile[i][numptr],"%.2ld",nseg);
				if((i == 4) && (width > 6)) fprintf(stdout,"\n");
				fprintf(stdout, "%s  ",ofile[i]);
				if((ofp[i] = fopen(ofile[i],"w")) == NULL)
					xit("can't open output file",24);
			}
			fprintf(stdout,"\n");
			++nseg;
			rec_addr = 0;
			dflag = CLEAR;
		}

			/* fill buffer and compute checksum */
		if((oform == 'h') && (rec_addr >= MAXSIZE))
		{
		    rec_addr = 0;
		    basedata += 16;
		    ibuf[0] = basedata;
		    ibuf[1] = 0;
		    recsize = 2;
		    type = 2;
		}
		else dosum(precsize);

			/* if within range output prom hex files */
		if((cpos - precsize) >= offset) {
			for(i=0; i < width; i++) {
			    if(oform == 'h') {
				sum = recsize + (rec_addr & 0x00ff) +
					(rec_addr >> 8) + type;
				putc(':',ofp[i]);
				puthex(recsize,ofp[i]);
				puthex((char)((rec_addr>>8) & 0xff),ofp[i]);
				puthex((char)(rec_addr & 0xff),ofp[i]);
				puthex(type,ofp[i]);
			    }
			    else {
				putc('S',ofp[i]);
				if(addsize == 3) putc('2',ofp[i]);
				else putc('1',ofp[i]);
				ashift = (addsize - 1) * 8;
				sum = recsize + addsize + 1;
				puthex(recsize + addsize +1,ofp[i]);
				for(k=0; k<addsize; ++k) {
				    sum += (rec_addr>>ashift) & 0xff;
				    puthex((char)((rec_addr>>ashift) & 0xff),ofp[i]);
				    ashift -= 8;
				}
			    }
			    for(j=0; j < recsize; ++j) {
				sum += ibuf[(width * j) + i];
				puthex(ibuf[(width * j) + i],ofp[i]);
			    }
			    if(oform == 'h') sum = -sum;
			    else sum = ~sum;
			    puthex(sum,ofp[i]);
			    putc('\n',ofp[i]);

				/* if done with current block, close file */
			    if((cpos >= offset) && ((cpos % size) == 0)) {
				if(oform == 'h') fprintf(ofp[i],":00000001\n");
				else if(addsize = 3) fprintf(ofp[i],"S904000000FB\n");
				else fprintf(ofp[i],"S9030000FC\n");
				fclose(ofp[i]);
				if(dflag == CLEAR) fprintf(stdout,"warning: %s contains no non-zero data!\n",ofile[i]);
			    }
			}
		}
		if(type == 2)
		{
		    type = 0;
		    recsize = RECSIZE;
		}
		else rec_addr += recsize;
	}
	if(wflag != SET) xit("no data found after offset, no output files created",26);

	exit(0);	/* normal exit */
}



#if M32SGS
	/* process a.out files */
inptA()
{
	struct	filehdr fhdr;
	struct	scnhdr shdr;
	extern FILE *infp, *tempfp;
	extern long	start_addr;	/* starting address of prom images */
	extern long	max_addr;	/* max address in input and output files */
	extern char	tflag;		/* test flag */
	long	ftell();
	long	sect_size;	/* remaining bytes in current section */
	long	sect_addr;	/* current address in current section */
	register long cpos;		/* current input and temp file position */
	register char ibuf[BUFSIZE];	/* input buffer */
	register short bbcnt;		/* bytes in input buffer */
	register char i;		/* loop counter */



	fread(&fhdr,sizeof(struct filehdr),1,infp);	/* get file header */
	if((fhdr.f_magic < B16MAGIC) || (fhdr.f_magic > MTVMAGIC))
	    if((fhdr.f_magic != M68MAGIC) && (fhdr.f_magic != M68MAG2))
		xit("input file not common object file format",27);
	fread(ibuf,fhdr.f_opthdr,1,infp);		/* disgard any opt header */
	for(i=fhdr.f_nscns; i>0; --i) {		/* do for each section */
		fread(&shdr, sizeof(struct scnhdr), 1, infp); /* get section hdr */
		cpos = ftell(infp);				/* log current pos in file */
/******/	if(tflag)
			fprintf(stdout, "section name=%s  size=%lX  addr=%lX, filept=%lX\n",
			    shdr.s_name,shdr.s_size,shdr.s_paddr,shdr.s_scnptr);


		/* get data if this is a good prom section */
		if((shdr.s_scnptr > 0) && (shdr.s_size > 0)) {
			if(shdr.s_paddr < 0L) xit("section address out of range",28);
			if(shdr.s_paddr < start_addr) {
				sprintf(errstring,"data found at address %lX, before start address",shdr.s_paddr);
				xit(errstring,29);
			}
			if( shdr.s_paddr >= memsize )
			 {
			   fprintf(stderr,"Skipping section %s at address %lx -- beyond memory limit.\n",
				shdr.s_name,shdr.s_paddr);
			   continue;
			  }
			if((shdr.s_scnptr > BIGNUM) || (shdr.s_scnptr < 0L))
				xit("section pointer out of range",30);
			if(fseek(infp, shdr.s_scnptr, 0) != 0)
				xit("unable to seek to location in input file",31);
			if((shdr.s_size > BIGNUM) || (shdr.s_size < 0L))
				xit("section size out of range",32);
			sect_size = shdr.s_size;	/* init section size */
			sect_addr = shdr.s_paddr - start_addr;
			if((shdr.s_paddr + shdr.s_size) < 0L)
				xit("section address out of range",33);
			if((shdr.s_paddr + shdr.s_size) > max_addr)
				max_addr = shdr.s_paddr + shdr.s_size;


			/* process the sction*/
			if(fseek(tempfp,sect_addr,0) != 0)
				xit("improper seek in tempfile",34);
/******/		if(tflag)
				fprintf(stdout,"writing tempfile at %lX for %lX\n",sect_addr,sect_size);
			while(sect_size > 0) {
				bbcnt = (short)( sect_size > BUFSIZE ? BUFSIZE : sect_size);
				if((bbcnt = fread(ibuf, 1, bbcnt, infp)) < 0)
					xit("problem reading input file",23);
				sect_size -= bbcnt;
				if((bbcnt = fwrite(ibuf,1,bbcnt,tempfp)) < 0)
					xit("can't write temp file",35);
			}
			fseek(infp, cpos, 0);	/* backup to next section */
		}
	}
}
#endif


char conv_tab[256] = "****************\
****************\
****************\
0123456789******\
*:;<=>?*********\
****************\
*:;<=>?*********";


	/* process hex file */
inptH()
{

	extern FILE *infp, *tempfp;
	extern long	start_addr;	/* starting address of prom images */
	extern long	max_addr;	/* max address in input and output files */
	extern char	Ssum;		/* record checksum */
	extern char	iform;		/* used to indicate the input file format */
	extern char	tflag;		/* test flag */
	long	address;		/* address in hex record */
	long	laddress;		/* last address in temp file */
	long	base;			/* base address in Intel hex file */
	unsigned short line;		/* line number in hex record file */
	char	addsize;		/* size of address in hex record */
	char	cnt;			/* count from hex record */
	char	c;			/* input char from hex record */
	char	type;			/* record type in Intel hex */
	register char ibuf[BUFSIZE];	/* input buffer */
	register char i;		/* loop counter */


		/* process the hex record file */
	line = 1;
	laddress = 0;
	base = 0;

	for(;;) {
		if(iform == 's') {
			/* get in sync */
			while ( (c = (char) getc(infp)) != 'S') {
				if (c == EOF) xit("early EOF in hex record file",36);
				if (c == '\n') ++line;
			}

			/* test S number */
			if((c = (char)getc(infp)) == '9') break;
			if(c == '8') break;
			if(c == '2') addsize = 3;
			else addsize = 2;

			Ssum = 0;

			/* get record count */
			cnt = getbyt();
			cnt -= (addsize + 1);

			/* get address */
			address = 0;
			for (i=0; i<addsize; i++){
				address <<= 8;
				address |= (getbyt() & 0xff);
			}
		}

		else {
			/* get in sync */
			while ( (c = (char) getc(infp)) != ':') {
				if (c == EOF) xit("early EOF in hex record file",37);
				if (c == '\n') ++line;
			}

			Ssum = 0;
			/* get record count */
			if((cnt = getbyt()) == 0) break;

			/* get address */
			address = 0;
			addsize = 2;
			for (i=0; i<addsize; i++){
				address <<= 8;
				address |= (getbyt() & 0xff);
			}
			address += base;

		/* get type byte */
			type = getbyt();
			switch(type)
			{
			    case 0:	/* data record */
				break;
			    case 1:	/* end of file record */
				return;
			    case 2:	/* base address record */
				base = 0;
				for(i=0; i<cnt; i++)
				{
				    base <<= 8;
				    base |= (getbyt() & 0xff);
				}
				base <<= 4;
				continue;
			    case 3:	/* start address record */
				continue;
			    default:	/* unknown type */
				xit("unknown Intel hex record type",61);
			}
		}


/******/	if(tflag && ((line - 1) % 500) == 0)
			fprintf(stdout,"address=%lx line=%d\n",address,line);

		/* test and adjust address */
		if(address < start_addr) {
			sprintf(errstring,"data found at address %lX, before start address",address);
			xit(errstring,38);
		}
		if((address + cnt) > max_addr)
			max_addr = address + cnt;
		address -= start_addr;

		/* get record data */
		if(fread(ibuf,1,(cnt * 2),infp) != (cnt * 2))
			xit("problem reading hex record file",39);

		for(i=0; i < (cnt * 2); i += 2) {
			if((ibuf[i] = (conv_tab[ibuf[i]] - '0')) < 0)
				 xit("illegal character in hex record",40);
			if((ibuf[i+1] = (conv_tab[ibuf[i+1]] - '0')) < 0)
				 xit("illegal character in hex record",41);
			ibuf[i/2] = (ibuf[i] * 16) + ibuf[i+1];
			Ssum += ibuf[i/2];
		}

		/* validate input checksum */
		getbyt();
		if(iform == 's') Ssum +=1;
		if (Ssum != 0)
			fprintf(stdout,"warning: error in hex record input checksum: line %d\n",
			    line);

		/* write temp file */
		if(laddress != address) {
/******/		if(tflag) fprintf(stdout,"seek address=%lX\n",address);
			if(fseek(tempfp,address,0) != 0)
				xit("improper seek in tempfile",42);
		}
		if(fwrite(ibuf,1,cnt,tempfp) < 0)
			xit("can't write temp file",43);
		laddress = address + cnt;
	}
}


		/* process binary input file */
inptB()
{
	extern FILE *infp, *tempfp;
	extern long max_addr;
	extern char tflag;
	register char ibuf[BUFSIZE];
	register short cnt;

	while((cnt = fread(ibuf,1,BUFSIZE,infp)) > 0) {
		if(tflag) fprintf(stdout,"writing at address %lX for %d\n",max_addr,cnt);
		max_addr += cnt;
		if(fwrite(ibuf,1,cnt,tempfp) < 0)
			xit("unable to write temp file",44);
	}
}

		/* process MICA input file */
inptM()
{
	extern FILE *infp, *tempfp;
	extern long max_addr;
	extern char tflag;
	extern short width;
	char chunks;
	char cnt;
	long tmpsto[2];
	char dbytes;
	char shift;
	char i;
	register char ibuf[BUFSIZE];

	if(width <= 4) {
		chunks = 1;
		dbytes = width;
	}
	else {
		chunks = 2;
		dbytes = width - 4;
	}
	while((cnt = fread(tmpsto,sizeof(long),chunks,infp)) > 0) {
		if(cnt != chunks) xit("data found in MICA file does not match width specified",55);
		max_addr += width;
		shift = (dbytes-1) * 8;
		for(i=0; i<dbytes; ++i) {
			ibuf[i] = tmpsto[0]>>shift & 0xff;
			shift -= 8;
		}
		if(width > 4) {
			shift = 24;
			for(; i<width; ++i) {
			    ibuf[i] = tmpsto[1]>>shift & 0xff;
			    shift -= 8;
			}
		}
		if(fwrite(ibuf,1,width,tempfp) < 0)
			xit("unable to write temp file",45);
	}
}


		/* get input and compute checksum */
dosum(numbytes)
short numbytes;
{
	extern FILE *tempfp;
	extern long cpos, max_addr;
	extern char cflag, dflag, ibuf[];
	static unsigned short chksum = 0;
	register short cnt, i;

	if((cnt = fread(ibuf,1,numbytes,tempfp)) < 0)
		xit("can't read tempfile",25);
	for( ; cnt < numbytes; cnt++)
		ibuf[cnt] = 0;
	for (i=0; i < numbytes; i++) {
		if(ibuf[i] != 0)
			dflag = SET;
		if((cpos++ < (max_addr - 2)) && cflag) {
			chksum += (ibuf[i] & 0x00ff);
			chksum =  ((chksum & 0x8000) != 0) ?
				(chksum << 1 | 1) : chksum << 1;
		}
		else if(cflag) {
		/* done store checksum */
			chksum = ~ chksum;
			fprintf(stdout,"checksum 0x%X being written",chksum);
			if((ibuf[i] != 0) || (ibuf[i+1] != 0))
			    fprintf(stdout,", over non-zero data,");
			fprintf(stdout," in last two bytes\n");
			ibuf[i++] = chksum & 0x00ff;
			ibuf[i] = chksum >> 8;
			++cpos;
		}
		if(cpos == max_addr) {
		    cnt = ++i;
		    break;
		}
	}
	return(cnt);
}


	/* get byte */
getbyt()
{
	extern FILE *infp;
	extern char Ssum;
	short rcode;
	short val;

	rcode = fscanf(infp, "%2x", &val);
	if (rcode == EOF) xit("early EOF in hex record file",46);
	if (rcode == 0) xit("illegal char in hex record file",47);

	Ssum += val;
	return(val);
}


	/* write out low bytes in upper-case hex */
puthex(val,ofp1)

char val;
FILE *ofp1;

{

	char *hex_ptr = "0123456789ABCDEF";

	putc(hex_ptr[(val>>4) & 15],ofp1);
	putc(hex_ptr[val&15],ofp1);
}

	/* write error message then exit */
xit(s,e)
char *s;
{
	fprintf(stderr,"\nERROR: %s\n",s);
	exit(e);
}
