/*
	HEADER:		CUG149;
	TITLE:		6805 Cross-Assembler (Portable);
	FILENAME:	A685UTIL.C;
	VERSION:	0.3;
	DATE:		08/27/1988;

	DESCRIPTION:	"This program lets you use your computer to assemble
			code for the Motorola 6805 family microprocessors.
			The program is written in portable C rather than BDS
			C.  All	assembler features are supported except
			relocation linkage, and macros.";

	KEYWORDS:	Software Development, Assemblers, Cross-Assemblers,
			Motorola, MC6805;

	SYSTEM:		CP/M-80, CP/M-86, HP-UX, MSDOS, PCDOS, QNIX;
	COMPILERS:	Aztec C86, Aztec CII, CI-C86, Eco-C, Eco-C88, HP-UX,
			Lattice C, Microsoft C,	QNIX C;

	WARNINGS:	"This program has compiled successfully on 2 UNIX
			compilers, 5 MSDOS compilers, and 2 CP/M compilers.
			A port to BDS C would be extremely difficult, but see
			volume CUG113.  A port to Toolworks C is untried."

	AUTHORS:	William C. Colley III;
*/

/*
		      6805 Cross-Assembler in Portable C

		   Copyright (c) 1985 William C. Colley, III

Revision History:

Ver	Date		Description

0.0	SEP 1985	Adapted from version 3.2 of the portable 6801 cross-
			assembler.  WCC3.

0.1	JUL 1986	Added compilation instructions and tweaks for CI-C86,
			Eco-C88, and Lattice C.  WCC3.

0.2	JAN 1987	Fixed bug that made "FCB 0," legal syntax.  WCC3.

0.3	AUG 1988	Fixed a bug in the command line parser that puts it
			into a VERY long loop if the user types a command line
			like "A685 FILE.ASM -L".  WCC3 per Alex Cameron.

This module contains the following utility packages:

	1)  symbol table building and searching

	2)  opcode and operator table searching

	3)  listing file output

	4)  hex file output

	5)  error flagging
*/

/*  Get global goodies:  */

#include "a685.h"

/*  Make sure that MSDOS compilers using the large memory model know	*/
/*  that calloc() returns pointer to char as an MSDOS far pointer is	*/
/*  NOT compatible with the int type as is usually the case.		*/

char *calloc();

/*  Get access to global mailboxes defined in A68.C:			*/

extern char errcode, line[], title[];
extern int eject, listhex;
extern unsigned address, bytes, errors, listleft, obj[], pagelen;

/*  The symbol table is a binary tree of variable-length blocks drawn	*/
/*  from the heap with the calloc() function.  The root pointer lives	*/
/*  here:								*/

static SYMBOL *sroot = NULL;

/*  Add new symbol to symbol table.  Returns pointer to symbol even if	*/
/*  the symbol already exists.  If there's not enough memory to store	*/
/*  the new symbol, a fatal error occurs.				*/

SYMBOL *new_symbol(nam)
char *nam;
{
    SCRATCH int i;
    SCRATCH SYMBOL **p, *q;
    void fatal_error();

    for (p = &sroot; (q = *p) && (i = strcmp(nam,q -> sname)); )
	p = i < 0 ? &(q -> left) : &(q -> right);
    if (!q) {
	if (!(*p = q = (SYMBOL *)calloc(1,sizeof(SYMBOL) + strlen(nam))))
	    fatal_error(SYMBOLS);
	strcpy(q -> sname,nam);
    }
    return q;
}

/*  Look up symbol in symbol table.  Returns pointer to symbol or NULL	*/
/*  if symbol not found.						*/

SYMBOL *find_symbol(nam)
char *nam;
{
    SCRATCH int i;
    SCRATCH SYMBOL *p;

    for (p = sroot; p && (i = strcmp(nam,p -> sname));
	p = i < 0 ? p -> left : p -> right);
    return p;
}

/*  Opcode table search routine.  This routine pats down the opcode	*/
/*  table for a given opcode and returns either a pointer to it or	*/
/*  NULL if the opcode doesn't exist.					*/

OPCODE *find_code(nam)
char *nam;
{
    OPCODE *bsearch();

    static OPCODE opctbl[] = {
	{ IMMED + DIR + EXT + IDX + EXT_IDX + 3,	0xb9,	"ADC"	},
	{ IMMED + DIR + EXT + IDX + EXT_IDX + 3,	0xbb,	"ADD"	},
	{ IMMED + DIR + EXT + IDX + EXT_IDX + 3,	0xb4,	"AND"	},
	{ DIR + IDX + 2,				0x38,	"ASL"	},
	{ NULL + 1,					0x48,	"ASLA"	},
	{ NULL + 1,					0x58,	"ASLX"	},
	{ DIR + IDX + 2,				0x37,	"ASR"	},
	{ NULL + 1,					0x47,	"ASRA"	},
	{ NULL + 1,					0x57,	"ASRX"	},
	{ REL + 2,					0x24,	"BCC"	},
	{ BIT + DIR + 2,				0x11,	"BCLR"	},
	{ REL + 2,					0x25,	"BCS"	},
	{ REL + 2,					0x27,	"BEQ"	},
	{ REL + 2,					0x28,	"BHCC"	},
	{ REL + 2,					0x29,	"BHCS"	},
	{ REL + 2,					0x22,	"BHI"	},
	{ REL + 2,					0x24,	"BHS"	},
	{ REL + 2,					0x2f,	"BIH"	},
	{ REL + 2,					0x2e,	"BIL"	},
	{ IMMED + DIR + EXT + IDX + EXT_IDX + 3,	0xb5,	"BIT"	},
	{ REL + 2,					0x25,	"BLO"	},
	{ REL + 2,					0x23,	"BLS"	},
	{ REL + 2,					0x2c,	"BMC"	},
	{ REL + 2,					0x2b,	"BMI"	},
	{ REL + 2,					0x2d,	"BMS"	},
	{ REL + 2,					0x26,	"BNE"	},
	{ REL + 2,					0x2a,	"BPL"	},
	{ REL + 2,					0x20,	"BRA"	},
	{ BIT + REL + DIR + 3,				0x01,	"BRCLR"	},
	{ REL + 2,					0x21,	"BRN"	},
	{ BIT + REL + DIR + 3,				0x00,	"BRSET"	},
	{ BIT + DIR + 2,				0x10,	"BSET"	},
	{ REL + 2,					0xad,	"BSR"	},
	{ NULL + 1,					0x98,	"CLC"	},
	{ NULL + 1,					0x9a,	"CLI"	},
	{ DIR + IDX + 2,				0x3f,	"CLR"	},
	{ NULL + 1,					0x4f,	"CLRA"	},
	{ NULL + 1,					0x5f,	"CLRX"	},
	{ IMMED + DIR + EXT + IDX + EXT_IDX + 3,	0xb1,	"CMP"	},
	{ DIR + IDX + 2,				0x33,	"COM"	},
	{ NULL + 1,					0x43,	"COMA"	},
	{ NULL + 1,					0x53,	"COMX"	},
	{ IMMED + DIR + EXT + IDX + EXT_IDX + 3,	0xb3,	"CPX"	},
	{ DIR + IDX + 2,				0x3a,	"DEC"	},
	{ NULL + 1,					0x4a,	"DECA"	},
	{ NULL + 1,					0x5a,	"DECX"	},
	{ NULL + 1,					0x5a,	"DEX"	},
	{ PSEUDO + ISIF,				ELSE,	"ELSE"	},
	{ PSEUDO,					END,	"END"	},
	{ PSEUDO + ISIF,				ENDIF,	"ENDIF"	},
	{ IMMED + DIR + EXT + IDX + EXT_IDX + 3,	0xb8,	"EOR"	},
	{ PSEUDO,					EQU,	"EQU"	},
	{ PSEUDO,					FCB,	"FCB"	},
	{ PSEUDO,					FCC,	"FCC"	},
	{ PSEUDO,					FDB,	"FDB"	},
	{ PSEUDO + ISIF,				IF,	"IF"	},
	{ DIR  + IDX + 2,				0x3c,	"INC"	},
	{ NULL + 1,					0x4c,	"INCA"	},
	{ PSEUDO,					INCL,	"INCL"	},
	{ NULL + 1,					0x5c,	"INCX"	},
	{ NULL + 1,					0x5c,	"INX"	},
	{ DIR + EXT + IDX + EXT_IDX + 3,		0xbc,	"JMP"	},
	{ DIR + EXT + IDX + EXT_IDX + 3,		0xbd,	"JSR"	},
	{ IMMED + DIR + EXT + IDX + EXT_IDX + 3,	0xb6,	"LDA"	},
	{ IMMED + DIR + EXT + IDX + EXT_IDX + 3,	0xbe,	"LDX"	},
	{ DIR + IDX + 2,				0x38,	"LSL"	},
	{ NULL + 1,					0x48,	"LSLA"	},
	{ NULL + 1,					0x58,	"LSLX"	},
	{ DIR + IDX + 2,				0x34,	"LSR"	},
	{ NULL + 1,					0x44,	"LSRA"	},
	{ NULL + 1,					0x54,	"LSRX"	},
	{ NULL + 1,					0x42,	"MUL"	},
	{ DIR + IDX + 2,				0x30,	"NEG"	},
	{ NULL + 1,					0x40,	"NEGA"	},
	{ NULL + 1,					0x50,	"NEGX"	},
	{ NULL + 1,					0x9d,	"NOP"	},
	{ IMMED + DIR + EXT + IDX + EXT_IDX + 3,	0xba,	"ORA"	},
	{ PSEUDO,					ORG,	"ORG"	},
	{ PSEUDO,					PAGE,	"PAGE"	},
	{ PSEUDO,					RMB,	"RMB"	},
	{ DIR + IDX + 2,				0x39,	"ROL"	},
	{ NULL + 1,					0x49,	"ROLA"	},
	{ NULL + 1,					0x59,	"ROLX"	},
	{ DIR + IDX + 2,				0x36,	"ROR"	},
	{ NULL + 1,					0x46,	"RORA"	},
	{ NULL + 1,					0x56,	"RORX"	},
	{ NULL + 1,					0x9c,	"RSP"	},
	{ NULL + 1,					0x80,	"RTI"	},
	{ NULL + 1,					0x81,	"RTS"	},
	{ IMMED + DIR + EXT + IDX + EXT_IDX + 3,	0xb2,	"SBC"	},
	{ NULL + 1,					0x99,	"SEC"	},
	{ NULL + 1,					0x9b,	"SEI"	},
	{ PSEUDO,					SET,	"SET"	},
	{ DIR + EXT + IDX + EXT_IDX + 3,		0xb7,	"STA"	},
	{ NULL + 1,					0x8e,	"STOP"	},
	{ DIR + EXT + IDX + EXT_IDX + 3,		0xbf,	"STX"	},
	{ IMMED + DIR + EXT + IDX + EXT_IDX + 3,	0xb0,	"SUB"	},
	{ NULL + 1,					0x83,	"SWI"	},
	{ NULL + 1,					0x97,	"TAX"	},
	{ PSEUDO,					TITLE,	"TITLE"	},
	{ DIR + IDX + 2,				0x3d,	"TST"	},
	{ NULL + 1,					0x4d,	"TSTA"	},
	{ NULL + 1,					0x5d,	"TSTX"	},
	{ NULL + 1,					0x9f,	"TXA"	},
	{ NULL + 1,					0x8f,	"WAIT"	}
    };

    return bsearch(opctbl,opctbl + (sizeof(opctbl) / sizeof(OPCODE)),nam);
}

/*  Operator table search routine.  This routine pats down the		*/
/*  operator table for a given operator and returns either a pointer	*/
/*  to it or NULL if the opcode doesn't exist.				*/

OPCODE *find_operator(nam)
char *nam;
{
    OPCODE *bsearch();

    static OPCODE oprtbl[] = {
	{ BINARY + LOG1  + OPR,		AND,		"AND"	},
	{ BINARY + RELAT + OPR,		'=',		"EQ"	},
	{ BINARY + RELAT + OPR,		GE,		"GE"	},
	{ BINARY + RELAT + OPR,		'>',		"GT"	},
	{ UNARY  + UOP3  + OPR,		HIGH,		"HIGH"	},
	{ BINARY + RELAT + OPR,		LE,		"LE"	},
	{ UNARY  + UOP3  + OPR,		LOW,		"LOW"	},
	{ BINARY + RELAT + OPR,		'<',		"LT"	},
	{ BINARY + MULT  + OPR,		MOD,		"MOD"	},
	{ BINARY + RELAT + OPR,		NE,		"NE"	},
	{ UNARY  + UOP2  + OPR,		NOT,		"NOT"	},
	{ BINARY + LOG2  + OPR,		OR,		"OR"	},
	{ BINARY + MULT  + OPR,		SHL,		"SHL"	},
	{ BINARY + MULT  + OPR,		SHR,		"SHR"	},
	{ REG,				'X',		"X"	},
	{ BINARY + LOG2  + OPR,		XOR,		"XOR"	}
    };

    return bsearch(oprtbl,oprtbl + (sizeof(oprtbl) / sizeof(OPCODE)),nam);
}

static OPCODE *bsearch(lo,hi,nam)
OPCODE *lo, *hi;
char *nam;
{
    SCRATCH int i;
    SCRATCH OPCODE *chk;

    for (;;) {
	chk = lo + (hi - lo) / 2;
	if (!(i = ustrcmp(chk -> oname,nam))) return chk;
	if (chk == lo) return NULL;
	if (i < 0) lo = chk;
	else hi = chk;
    }
}

static int ustrcmp(s,t)
char *s, *t;
{
    SCRATCH int i;

    while (!(i = toupper(*s++) - toupper(*t)) && *t++);
    return i;
}

/*  Buffer storage for line listing routine.  This allows the listing	*/
/*  output routines to do all operations without the main routine	*/
/*  having to fool with it.						*/

static FILE *list = NULL;

/*  Listing file open routine.  If a listing file is already open, a	*/
/*  warning occurs.  If the listing file doesn't open correctly, a	*/
/*  fatal error occurs.  If no listing file is open, all calls to	*/
/*  lputs() and lclose() have no effect.				*/

void lopen(nam)
char *nam;
{
    FILE *fopen();
    void fatal_error(), warning();

    if (list) warning(TWOLST);
    else if (!(list = fopen(nam,"w"))) fatal_error(LSTOPEN);
    return;
}

/*  Listing file line output routine.  This routine processes the	*/
/*  source line saved by popc() and the output of the line assembler in	*/
/*  buffer obj into a line of the listing.  If the disk fills up, a	*/
/*  fatal error occurs.							*/

void lputs()
{
    SCRATCH int i, j;
    SCRATCH unsigned *o;
    void check_page(), fatal_error();

    if (list) {
	i = bytes;  o = obj;
	do {
	    fprintf(list,"%c  ",errcode);
	    if (listhex) {
		fprintf(list,"%04x  ",address);
		for (j = 4; j; --j) {
		    if (i) { --i;  ++address;  fprintf(list," %02x",*o++); }
		    else fprintf(list,"   ");
		}
	    }
	    else fprintf(list,"%18s","");
	    fprintf(list,"   %s",line);  strcpy(line,"\n");
	    check_page();
	    if (ferror(list)) fatal_error(DSKFULL);
	} while (listhex && i);
    }
    return;
}

/*  Listing file close routine.  The symbol table is appended to the	*/
/*  listing in alphabetic order by symbol name, and the listing file is	*/
/*  closed.  If the disk fills up, a fatal error occurs.		*/

static int col = 0;

void lclose()
{
    void fatal_error(), list_sym();

    if (list) {
	if (sroot) {
	    list_sym(sroot);
	    if (col) fprintf(list,"\n");
	}
	fprintf(list,"\f");
	if (ferror(list) || fclose(list) == EOF) fatal_error(DSKFULL);
    }
    return;
}

static void list_sym(sp)
SYMBOL *sp;
{
    void check_page();

    if (sp) {
	list_sym(sp -> left);
	fprintf(list,"%04x  %-10s",sp -> valu,sp -> sname);
	if (col = ++col % SYMCOLS) fprintf(list,"    ");
	else {
	    fprintf(list,"\n");
	    if (sp -> right) check_page();
	}
	list_sym(sp -> right);
    }
    return;
}

static void check_page()
{
    if (pagelen && !--listleft) eject = TRUE;
    if (eject) {
	eject = FALSE;  listleft = pagelen;  fprintf(list,"\f");
	if (title[0]) { listleft -= 2;  fprintf(list,"%s\n\n",title); }
    }
    return;
}

/*  Buffer storage for hex output file.  This allows the hex file	*/
/*  output routines to do all of the required buffering and record	*/
/*  forming without the	main routine having to fool with it.		*/

static FILE *hex = NULL;
static unsigned cnt = 0;
static unsigned addr = 0;
static unsigned sum = 0;
static unsigned buf[HEXSIZE];

/*  Hex file open routine.  If a hex file is already open, a warning	*/
/*  occurs.  If the hex file doesn't open correctly, a fatal error	*/
/*  occurs.  If no hex file is open, all calls to hputc(), hseek(), and	*/
/*  hclose() have no effect.						*/

void hopen(nam)
char *nam;
{
    FILE *fopen();
    void fatal_error(), warning();

    if (hex) warning(TWOHEX);
    else if (!(hex = fopen(nam,"w"))) fatal_error(HEXOPEN);
    return;
}

/*  Hex file write routine.  The data byte is appended to the current	*/
/*  record.  If the record fills up, it gets written to disk.  If the	*/
/*  disk fills up, a fatal error occurs.				*/

void hputc(c)
unsigned c;
{
    void record();

    if (hex) {
	buf[cnt++] = c;
	if (cnt == HEXSIZE) record(0);
    }
    return;
}

/*  Hex file address set routine.  The specified address becomes the	*/
/*  load address of the next record.  If a record is currently open,	*/
/*  it gets written to disk.  If the disk fills up, a fatal error	*/
/*  occurs.								*/

void hseek(a)
unsigned a;
{
    void record();

    if (hex) {
	if (cnt) record(0);
	addr = a;
    }
    return;
}

/*  Hex file close routine.  Any open record is written to disk, the	*/
/*  EOF record is added, and file is closed.  If the disk fills up, a	*/
/*  fatal error occurs.							*/

void hclose()
{
    void fatal_error(), record();

    if (hex) {
	if (cnt) record(0);
	record(1);
	if (fclose(hex) == EOF) fatal_error(DSKFULL);
    }
    return;
}

static void record(typ)
unsigned typ;
{
    SCRATCH unsigned i;
    void fatal_error(), putb();

    putc(':',hex);  putb(cnt);  putb(high(addr));
    putb(low(addr));  putb(typ);
    for (i = 0; i < cnt; ++i) putb(buf[i]);
    putb(low(-sum));  putc('\n',hex);

    addr += cnt;  cnt = 0;

    if (ferror(hex)) fatal_error(DSKFULL);
    return;
}

static void putb(b)
unsigned b;
{
    static char digit[] = "0123456789ABCDEF";

    putc(digit[b >> 4],hex);  putc(digit[b & 0x0f],hex);
    sum += b;  return;
}

/*  Error handler routine.  If the current error code is non-blank,	*/
/*  the error code is filled in and the	number of lines with errors	*/
/*  is adjusted.							*/

void error(code)
char code;
{
    if (errcode == ' ') { errcode = code;  ++errors; }
    return;
}

/*  Fatal error handler routine.  A message gets printed on the stderr	*/
/*  device, and the program bombs.					*/

void fatal_error(msg)
char *msg;
{
    printf("Fatal Error -- %s\n",msg);
    exit(-1);
}

/*  Non-fatal error handler routine.  A message gets printed on the	*/
/*  stderr device, and the routine returns.				*/

void warning(msg)
char *msg;
{
    printf("Warning -- %s\n",msg);
    return;
}
