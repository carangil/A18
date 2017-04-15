/*
	HEADER:		CUG149;
	TITLE:		Intel Hex to Motorola S-Record Converter;
	FILENAME:	HEXTOS.C;
	VERSION:	0.1;
	DATE:		08/08/1985;

	DESCRIPTION:	"This program converts the Intel hex object files from
			the 6801 cross-assembler to Motorola S-record object
			files that can be read my Motorola's monitors and
			program loaders.";

	KEYWORDS:	Software Development, Utilities, Motorola, Hex Files;

	SYSTEM:		CP/M-80, HP-UX, MSDOS, PCDOS;
	COMPILERS:	AZTEC C II, Eco-C, HP-UX, Microsoft C;

	WARNINGS:	"Since this program has been compiled on a UNIX look-
			alike (HP-UX), portability to UNIX should be trivial.
			Since it has compiled on three full-featured compilers,
			portability to MSDOS should be easy.";

	AUTHORS:	William C. Colley III;
*/

/*
		Intel Hex to Motorola S-Record Converter

		Copyright (c) 1985 William C. Colley, III

In dealing with Motorola processors, one is dealing with two object file
formats.  Most EPROM programmers, CP/M-based software, etc. uses the Intel
hexadecimal object format, so that is what my cross-assemblers produce.
Motorola's loaders almost all use a different format called the Motorola
S-record format.  This utility converts an Intel hex file to Motorola S-record
format.  The command line looks like this:

	hextoc input_file output_file

Compilation, assembly, and linkage instructions for various compilers:

	A)  AZTEC C II (version 1.06B):

		1)  Comment out all compiler names below except "AZTEC_C_II".

		2)  Run the following command lines:

			A>cc hextos
			A>as hextos -zap
			A>ln hextos.o -lc
			A>era hextos.o

	B)  Eco-C (version 3.10):

		1)  Comment out all compiler names below except "ECO_C".

		2)  Run the following command lines:

			A>cp hextos -i -m
			A>l80 hextos,hextos/n/e
			A>era hextos.mac
			A>era hextos.rel

	C)  HP-UX (a UNIX look-alike on an HP 9000 Series 200/500, 68000-based
	    machine):

		1)  Comment out all compiler names below except "HP_UX".

		2)  Run the following command line:

			. cc hextos.c

	D)  Microsoft C (version 3.00):

		1)  Comment out all compiler names below except "MICROSOFT_C".

		2)  Run the following command line:

			C>cl a68.c a68eval.c a68util.c
*/

/* Compiler dependencies:						*/

#define		AZTEC_C_II		1
/* #define	ECO_C			1				*/
/* #define	HP_UX			1				*/
/* #define	MICROSOFT_C		1				*/

#ifdef	AZTEC_C_II

#define	getc(f)		agetc(f)
#define	putc(c,f)	aputc(c,f)

#endif

#include <stdio.h>

/* Warning and Error messages:						*/

#define	BADEOF		"Unexpected EOF on Intel hex file"
#define	BADREC		"Bad record in Intel hex file"
#define	DSKFULL		"Disk or directory full"
#define	HEXOPEN		"Could not open Intel hex file"
#define	SCREATE		"Could not create S-record file"
#define	USAGE		"Usage:  hextos infile outfile"

#define	START		0
#define	DATA		1
#define	END		9

static unsigned sum;
static unsigned hdr[] = { 'H', 'D', 'R', '9' };

#define	HDRSIZ		(sizeof(hdr) / sizeof(unsigned))

static FILE *ihex = NULL;
static FILE *srec = NULL;

main(argc,argv)
int argc;
char **argv;
{
    static char c;
    static unsigned addr, *bp, buf[256], cnt, getb(), siz;
    void error(), putr(), scan();

    puts("Intel Hex to Motorola S-Record Converter Ver 0.1");
    puts("Copyright (c) 1985 William C. Colley, III\n");

    if (argc != 3) error(USAGE);
    if (!(ihex = fopen(*++argv,"r"))) error(HEXOPEN);
    if (!(srec = fopen(*++argv,"w"))) error(SCREATE);

    putr(START,HDRSIZ,0,hdr);

    for (;;) {
	sum = 0;  scan(':');
	cnt = getb();  addr = getb();  addr = (addr << 8) + getb();
	switch (getb()) {
	    case 0:	for (bp = buf; bp <= buf + cnt; *bp++ = getb());
			if (sum & 0xff) error(BADREC);
			for (bp = buf; siz = cnt > 30 ? 30 : cnt; cnt -= siz) {
			    putr(DATA,siz,addr,bp);  addr += siz;  bp += siz;
			}
			break;

	    case 1:	getb();  fclose(ihex);
			if (cnt || (sum & 0xff)) error(BADREC);
			putr(END,0,addr,NULL);
			if (fclose(srec) == EOF) error(DSKFULL);
			puts("File successfully converted");
			return;

	    default:	error(BADREC);
	}
    }
}

static void scan(c)
char c;
{
    static char d;
    void error();

    while ((d = getc(ihex)) != EOF) if ((d & 0177) == c) return;
    error(BADEOF);
}

static unsigned getb()
{
    static unsigned getn(), u;

    u = getn();
    sum += (u = (u << 4) + getn());
    return u;
}

static unsigned getn()
{
    static char c;
    void error();

    if ((c = getc(ihex)) == EOF) error(BADEOF);
    if ((c = toupper(c & 0177)) >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - ('A' - 10);
    error(BADREC);
}

static char digit[] = "0123456789ABCDEF";

static void putr(typ,cnt,adr,dat)
unsigned typ,cnt,adr;
unsigned *dat;
{
    void error(), putb();

    putc('S',srec);  putc(digit[typ],srec);
    sum = 0;  putb(cnt + 3);
    putb(adr >> 8);  putb(adr & 0xff);
    while (cnt--) putb(*dat++);
    putb(~sum & 0xff);  putc('\n',srec);
    if (ferror(srec)) error(DSKFULL);
}

static void putb(b)
unsigned b;
{
    sum += b;
    putc(digit[b >> 4],srec);
    putc(digit[b & 0x0f],srec);
}

static void error(msg)
char *msg;
{
    printf("Error -- %s\n",msg);
    exit(EOF);
}
