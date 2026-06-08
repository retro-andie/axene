/*
** Import_ExcelP.h for XQuad in Import/
** Header for Import_ExcelP Object
**
** Copyright (C) 1995-2000 Axene.
** Authors: Stéphane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
** Email: xcalibur@axene.org
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program; if not, write to the Free Software
**    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Started on  Wed Feb 22 16:59:27 1995 Emmanuel Paris
** Last update Thu Nov  7 16:28:51 1996 Emmanuel Paris
*/

#ifndef _Import_ExcelP_h_
#define _Import_ExcelP_h_

/* Fonction  definitions */

#define XcXL_FORMAT_ERROR	0

#define XcXL_DIMENSIONS		1
#define XcXL_BLANK		2
#define XcXL_INTEGER		3
#define XcXL_NUMBER		4
#define XcXL_LABEL		5
#define XcXL_BOOLERR		6
#define XcXL_FORMULA		7
#define XcXL_STRING		8
#define XcXL_ROW		9
#define XcXL_BOF		10
#define XcXL_EOF		11
#define XcXL_INDEX		12
#define XcXL_CALCCOUNT		13
#define XcXL_CALCMODE		14
#define XcXL_PRECISION		15
#define XcXL_REFMODE		16
#define XcXL_DELTA		17
#define XcXL_ITERATION		18
#define XcXL_PROTECT		19
#define XcXL_PASSWORD		20
#define XcXL_HEADER		21
#define XcXL_FOOTER		22
#define XcXL_EXTERNCOUNT	23
#define XcXL_EXTERNSHEET	24
#define XcXL_NAME		25
#define XcXL_WINDOWPROTECT	26
#define XcXL_VERTIPAGEBREAKS	27
#define XcXL_HORIZPAGEBREAKS	28
#define XcXL_NOTE		29
#define XcXL_SELECTION		30
#define XcXL_FORMAT		31
#define XcXL_FORMATCOUNT	32
#define XcXL_COLUMNDEFAULT	33
#define XcXL_ARRAY		34
#define XcXL_1904		35
#define XcXL_EXTERNNAME		36
#define XcXL_COLWIDTH		37
#define XcXL_DEFAULTROWHEIGHT	38
#define XcXL_LEFTMARGIN		39
#define XcXL_RIGHTMARGIN	40
#define XcXL_TOPMARGIN		41
#define XcXL_BOTTOMMARGIN	42
#define XcXL_PRINTHEADERS	43
#define XcXL_PRINTGRIDLINES	44
#define XcXL_FILEPASS		45
#define XcXL_FONT		46
#define XcXL_FONT2		47
#define XcXL_TABLE		48
#define XcXL_TABLE2		49
#define XcXL_CONTINUE		50
#define XcXL_WINDOW1		51
#define XcXL_WINDOW2		52
#define XcXL_BACKUP		53
#define XcXL_PANE		54
#define XcXL_CODEPAGE		55
#define XcXL_XF			56
#define XcXL_EFONT		57
#define XcXL_PLS		58
#define XcXL_DCON		59
#define XcXL_DCONREF		60
#define XcXL_DCONNAME		61
#define XcXL_DEFCOLWIDTH	62
#define XcXL_BUILTINFMTCOUNT	63
#define XcXL_XCT		64
#define XcXL_CRN		65
#define XcXL_FILESHARING	66
#define XcXL_WRITEACCESS	67
#define XcXL_OBJ		68
#define XcXL_UNCALCED		69
#define XcXL_SAVERECALC		70
#define XcXL_TEMPLATE		71
#define XcXL_INTL		72
#define XcXL_OBJPROTECT		73
#define XcXL_COLINFO		74
#define XcXL_RK			75
#define XcXL_IMDATA		76
#define XcXL_GUTS		77
#define XcXL_WSBOOL		78
#define XcXL_GRIDSET		79
#define XcXL_HCENTER		80
#define XcXL_VCENTER		81
#define XcXL_WRITEPROT		82
#define XcXL_ADDIN		83
#define XcXL_EDG		84
#define XcXL_PUB		85
#define XcXL_LH			86
#define XcXL_COUNTRY		87
#define XcXL_HIDEOBJ		88
#define XcXL_SUB		89
#define XcXL_PALETTE		90
#define XcXL_STYLE		91
#define XcXL_LHRECORD		92
#define XcXL_LHNGRAPH		93
#define XcXL_SOUND		94
#define XcXL_SYNC		95
#define XcXL_LPR		96
#define XcXL_STNDARDWIDTH	97
#define XcXL_FNGROUPNAME	98
#define XcXL_FNGROUPCOUNT	99
#define XcXL_SCL		100
#define XcXL_SETUP		101
#define XcXL_FNPROTO		102
#define XcXL_COORDLIST		103
#define XcXL_GCW		104
#define NB_EXCEL_RECORD		105

#define XcXL_UNKNOW		200
#define XcXL_FILE_TRUNCATED	0xFFFF

#define XcXLF_INTEGER		FALSE
#define XcXLF_REAL		TRUE

typedef struct {
  unsigned short rec_excel2;
  unsigned short rec_excel3;
  unsigned short rec_excel4;
} excel_record_t;

static excel_record_t excel_record[NB_EXCEL_RECORD] = 
{
  /*  	BIFF 2	BIFF 3	BIFF 4	           Fonction */
	{ 0xfff, 0xfff, 0xfff },	/* ERREUR */
	{ 0x000, 0x200, 0x200 },	/* DIMENSIONS */
	{ 0x001, 0x201, 0x201 },	/* BLANK */
	{ 0x002, 0xFFF, 0xFFF },	/* INTEGER */
	{ 0x003, 0x203, 0x203 },	/* NUMBER */
	{ 0x004, 0x204, 0x204 },	/* LABEL */
	{ 0x005, 0x205, 0x205 },	/* BOOLERR */
	{ 0x006, 0x206, 0x406 },	/* FORMULA */
	{ 0x007, 0x207, 0x207 },	/* STRING */
	{ 0x008, 0x208, 0x208 },	/* ROW */
	{ 0x009, 0x209, 0x409 },	/* BOF */
	{ 0x00A, 0x00A, 0x00A },	/* EOF */
	{ 0x00B, 0x20B, 0x40B },	/* INDEX */
	{ 0x00C, 0x00C, 0x00C },	/* CALCCOUNT */
	{ 0x00D, 0x00D, 0x00D },	/* CALCMODE */
	{ 0x00E, 0x00E, 0x00E },	/* PRECISION */
	{ 0x00F, 0x00F, 0x00F },	/* REFMODE */
	{ 0x010, 0x010, 0x010 },	/* DELTA */
	{ 0x011, 0x011, 0x011 },	/* ITERATION */
	{ 0x012, 0x012, 0x012 },	/* PROTECT */
	{ 0x013, 0x013, 0x013 },	/* PASSWORD */
	{ 0x014, 0x014, 0x014 },	/* HEADER */
	{ 0x015, 0x015, 0x015 },	/* FOOTER */
	{ 0x016, 0x016, 0x016 },	/* EXTERNCOUNT */
	{ 0x017, 0x017, 0x017 },	/* EXTERNSHEET */
	{ 0x018, 0x218, 0x218 },	/* NAME */
	{ 0x019, 0x019, 0x019 },	/* WINDOWPROTECT */
	{ 0x01A, 0x01A, 0x01A },	/* VERTICALPAGEBREAKS */
	{ 0x01B, 0x01B, 0x01B },	/* HORIZONTALPAGEBREAKS */
	{ 0x01C, 0x01C, 0x01C },	/* NOTE */
	{ 0x01D, 0x01D, 0x01D },	/* SELECTION */
	{ 0x01E, 0x01E, 0x41E },	/* FORMAT */
	{ 0x01F, 0xFFF, 0xFFF },	/* FORMATCOUNT */
	{ 0x020, 0xFFF, 0xFFF },	/* COLUMNDEFAULT */
	{ 0x021, 0x221, 0x221 },	/* ARRAY */
	{ 0x022, 0x022, 0x022 },	/* 1904 */
	{ 0x023, 0x223, 0x223 },	/* EXTERNNAME */
	{ 0x024, 0xFFF, 0xFFF },	/* COLWIDTH */
	{ 0x025, 0x225, 0x225 },	/* DEFAULTROWHEIGHT */
	{ 0x026, 0x026, 0x026 },	/* LEFTMARGIN */
	{ 0x027, 0x027, 0x027 },	/* RIGHTMARGIN */
	{ 0x028, 0x028, 0x028 },	/* TOPMARGIN */
	{ 0x029, 0x029, 0x029 },	/* BOTTOMMARGIN */
	{ 0x02A, 0x02A, 0x02A },	/* PRINTHEADERS */
	{ 0x02B, 0x02B, 0x02B },	/* PRINTGRIDLINES */
	{ 0x02F, 0x02F, 0x02F },	/* FILEPASS */
	{ 0x031, 0x231, 0x231 },	/* FONT */
	{ 0x032, 0xFFF, 0xFFF },	/* FONT2 */
	{ 0x036, 0x236, 0x236 },	/* TABLE */
	{ 0x037, 0xFFF, 0xFFF },	/* TABLE2 */
	{ 0x03C, 0x03C, 0x03C },	/* CONTINUE */
	{ 0x03D, 0x03D, 0x03D },	/* WINDOW1 */
	{ 0x03E, 0x23E, 0x23E },	/* WINDOW2 */
	{ 0x040, 0x040, 0x040 },	/* BACKUP */
	{ 0x041, 0x041, 0x041 },	/* PANE */
	{ 0x042, 0x042, 0x042 },	/* CODEPAGE */
	{ 0x043, 0x243, 0x443 },	/* XF */
	{ 0x045, 0xFFF, 0xFFF },	/* EFONT */
	{ 0x04D, 0x04D, 0x04D },	/* PLS */
	{ 0x050, 0x050, 0x050 },	/* DCON */
	{ 0x051, 0x051, 0x051 },	/* DCONREF */
	{ 0x052, 0x052, 0x052 },	/* DCONNAME */
	{ 0x055, 0x055, 0x055 },	/* DEFCOLWIDTH */
	{ 0xFFF, 0x056, 0x056 },	/* BUILTINFMTCOUNT */
	{ 0xFFF, 0x059, 0x059 },	/* XCT */
	{ 0xFFF, 0x05A, 0x05A },	/* CRN */
	{ 0xFFF, 0x05B, 0x05B },	/* FILESHARING */
	{ 0xFFF, 0x05C, 0x05C },	/* WRITEACCESS */
	{ 0xFFF, 0x05D, 0x05D },	/* OBJ */
	{ 0xFFF, 0x05E, 0x05E },	/* UNCALCED */
	{ 0xFFF, 0x05F, 0x05F },	/* SAVERECALC */
	{ 0xFFF, 0x060, 0x060 },	/* TEMPLATE */
	{ 0xFFF, 0x061, 0x061 },	/* INTL */
	{ 0xFFF, 0x063, 0x063 },	/* OBJPROTECT */
	{ 0xFFF, 0x07D, 0x07D },	/* COLINFO */
	{ 0xFFF, 0x27E, 0x27E },	/* RK */
	{ 0xFFF, 0x07F, 0x07F },	/* IMDATA */
	{ 0xFFF, 0x080, 0x080 },	/* GUTS */
	{ 0xFFF, 0x081, 0x081 },	/* WSBOOL */
	{ 0xFFF, 0x082, 0x082 },	/* GRIDSET */
	{ 0xFFF, 0x083, 0x083 },	/* HCENTER */
	{ 0xFFF, 0x084, 0x084 },	/* VCENTER */
	{ 0xFFF, 0x086, 0x086 },	/* WRITEPROT */
	{ 0xFFF, 0x087, 0x087 },	/* ADDIN */
	{ 0xFFF, 0x088, 0x088 },	/* EDG */
	{ 0xFFF, 0x089, 0x089 },	/* PUB */
	{ 0xFFF, 0x08B, 0x08B },	/* LH */
	{ 0xFFF, 0x08C, 0x08C },	/* COUNTRY */
	{ 0xFFF, 0x08D, 0x08D },	/* HIDEOBJ */
	{ 0xFFF, 0x091, 0x091 },	/* SUB */
	{ 0xFFF, 0x092, 0x092 },	/* PALETTE */
	{ 0xFFF, 0x093, 0x093 },	/* STYLE */
	{ 0xFFF, 0x094, 0x094 },	/* LHRECORD */
	{ 0xFFF, 0x095, 0x095 },	/* LHNGRAPH */
	{ 0xFFF, 0xFFF, 0x096 },	/* SOUND */
	{ 0xFFF, 0xFFF, 0x097 },	/* SYNC */
	{ 0xFFF, 0xFFF, 0x098 },	/* LPR */
	{ 0xFFF, 0xFFF, 0x099 },	/* STNDARDWIDTH */
	{ 0xFFF, 0xFFF, 0x09A },	/* FNGROUPNAME */
	{ 0xFFF, 0xFFF, 0x09C },	/* FNGROUPCOUNT */
	{ 0xFFF, 0xFFF, 0x0A0 },	/* SCL */
	{ 0xFFF, 0xFFF, 0x0A1 },	/* SETUP */
	{ 0xFFF, 0xFFF, 0x0A2 },	/* FNPROTO */
	{ 0xFFF, 0xFFF, 0x0A9 },	/* COORDLIST */
	{ 0xFFF, 0xFFF, 0x0AB }		/* GCW */
};

#define REC_BIFF2	1
#define REC_BIFF3	2
#define REC_BIFF4	4
#define REC_BIFF234	7
#define REC_BIFF34	6

typedef struct _tstyle_list_t
{
  c_TextStyle	*tstyle;
  struct _tstyle_list_t *Next;
} tstyle_list_t;

typedef struct _format_list_t
{
  c_FormatNbr	*format;
  struct _format_list_t *Next;
} format_list_t;

typedef struct _cstyle_list_t
{
  CellStyle_t	*cstyle;
  struct _cstyle_list_t *Next;
} cstyle_list_t;

typedef struct _name_list_t
{
  char		*name;
  struct _name_list_t *Next;
} name_list_t;

typedef struct {
  boolean	accept;
  int	col;
  int	row;
} xl_string_set_t;

#define TEST_BIT(value, bit) (((value) & (1<<(bit))) != 0)

typedef struct {
  double	red;
  double	green;
  double	blue;
} xl_palette_t;

#define NB_EXCEL2_COLOR		16

static xl_palette_t xl2_palette[NB_EXCEL2_COLOR] = 
{
  { 0.0, 0.0, 0.0},	/* black */
  { 1.0, 1.0, 1.0},	/* white */
  { 1.0, 0.0, 0.0},	/* red */
  { 0.0, 1.0, 0.0},	/* green */
  { 0.0, 0.0, 1.0},	/* blue */
  { 1.0, 1.0, 0.0},	/* yellow */
  { 1.0, 0.0, 1.0},	/* magenta */
  { 0.0, 1.0, 1.0},	/* cyan */
  { 0.3, 0.3, 0.3},	/* dark gray */
  { 0.7, 0.7, 0.7},	/* light gray */
  { 0.5, 0.0, 0.0},	/* dark red */
  { 0.0, 0.5, 0.0},	/* dark green */
  { 0.0, 0.0, 0.5},	/* dark blue */
  { 0.5, 0.5, 0.0},	/* dark yellow */
  { 0.5, 0.0, 0.5},	/* dark magenta */
  { 0.0, 0.5, 0.5},	/* dark cyan */
};

#define NB_EXCEL34_COLOR	24

static xl_palette_t xl34_palette[NB_EXCEL34_COLOR] = 
{
  { 0.0, 0.0, 0.0},	/* black */
  { 1.0, 1.1, 1.1},	/* white */
  { 1.0, 0.0, 0.0},	/* red */
  { 0.0, 1.0, 0.0},	/* green */
  { 0.0, 0.0, 1.0},	/* blue */
  { 1.0, 1.0, 0.0},	/* yellow */
  { 1.0, 0.0, 1.0},	/* magenta */
  { 0.0, 1.0, 1.0},	/* cyan */
  { 0.0, 0.0, 0.0},	/* black */
  { 1.0, 1.0, 1.0},	/* white */
  { 1.0, 0.0, 0.0},	/* red */
  { 0.0, 1.0, 0.0},	/* green */
  { 0.0, 0.0, 1.0},	/* blue */
  { 1.0, 1.0, 0.0},	/* yellow */
  { 1.0, 0.0, 1.0},	/* magenta */
  { 0.0, 1.0, 1.0},	/* cyan */
  { 0.5, 0.0, 0.0},	/* dark red */
  { 0.0, 0.5, 0.0},	/* dark green */
  { 0.0, 0.0, 0.5},	/* dark blue */
  { 0.5, 0.5, 0.0},	/* dark yellow */
  { 0.5, 0.0, 0.5},	/* dark magenta */
  { 0.0, 0.5, 0.5},	/* dark cyan */
  { 0.7, 0.7, 0.7},	/* light gray */
  { 0.3, 0.3, 0.3},	/* dark gray */
};

static int excel_pattern_convertion[64] = {
  PATTERN_NONE, 
  CELL_PATTERN_DATA_FIRST     , CELL_PATTERN_DATA_FIRST +  1, 
  CELL_PATTERN_DATA_FIRST +  2, CELL_PATTERN_DATA_FIRST +  3, 
  CELL_PATTERN_DATA_FIRST +  4, CELL_PATTERN_DATA_FIRST +  5, 
  CELL_PATTERN_DATA_FIRST +  6, CELL_PATTERN_DATA_FIRST +  7, 
  CELL_PATTERN_DATA_FIRST +  8, CELL_PATTERN_DATA_FIRST +  9, 
  CELL_PATTERN_DATA_FIRST + 10, CELL_PATTERN_DATA_FIRST + 11, 
  CELL_PATTERN_DATA_FIRST + 12, CELL_PATTERN_DATA_FIRST + 13, 
  CELL_PATTERN_DATA_FIRST + 14, CELL_PATTERN_DATA_FIRST + 15, 
  CELL_PATTERN_DATA_FIRST + 16, CELL_PATTERN_DATA_FIRST + 17, 
  CELL_PATTERN_DATA_FIRST + 18, CELL_PATTERN_DATA_FIRST + 19, 
  CELL_PATTERN_DATA_FIRST + 20, CELL_PATTERN_DATA_FIRST + 21, 
  CELL_PATTERN_DATA_FIRST + 22, CELL_PATTERN_DATA_FIRST + 23, 
  CELL_PATTERN_DATA_FIRST + 24, CELL_PATTERN_DATA_FIRST + 25, 
  CELL_PATTERN_DATA_FIRST + 26, CELL_PATTERN_DATA_FIRST + 27, 
  CELL_PATTERN_DATA_FIRST + 28, CELL_PATTERN_DATA_FIRST + 29, 
  CELL_PATTERN_DATA_FIRST + 30, CELL_PATTERN_DATA_FIRST + 31, 
  CELL_PATTERN_DATA_FIRST + 32, CELL_PATTERN_DATA_FIRST + 33, 
  CELL_PATTERN_DATA_FIRST + 34, CELL_PATTERN_DATA_FIRST + 35, 
  CELL_PATTERN_DATA_FIRST + 36, CELL_PATTERN_DATA_FIRST + 37, 
  CELL_PATTERN_DATA_FIRST + 38, 
  PATTERN_NONE, PATTERN_NONE, PATTERN_NONE, PATTERN_NONE, PATTERN_NONE,
  PATTERN_NONE, PATTERN_NONE, PATTERN_NONE, PATTERN_NONE, PATTERN_NONE,
  PATTERN_NONE, PATTERN_NONE, PATTERN_NONE, PATTERN_NONE, PATTERN_NONE,
  PATTERN_NONE, PATTERN_NONE, PATTERN_NONE, PATTERN_NONE, PATTERN_NONE,
  PATTERN_NONE, PATTERN_NONE, PATTERN_NONE, PATTERN_NONE  
};

#ifdef ___PSEUDO_COMPOSE_MACRO

#define decode_xf_border( border) \
switch( i & 7) \
{ \
  case 1: \
border##p = CSP_SINGLE_LINE; \
border##d = PATTERN_NONE; \
    break; \
  case 2: \
border##p = CSP_DOUBLE_LINE; \
border##d = PATTERN_NONE; \
    break; \
  case 3: \
border##p = CSP_SINGLE_LINE; \
border##d = CSD_DASH; \
    break; \
  case 4: \
border##p = CSP_SINGLE_LINE; \
border##d = CSD_DASH_SHORT; \
    break; \
  case 5: \
border##p = CSP_1101; \
border##d = PATTERN_NONE; \
    break; \
  case 6: \
border##p = CSP_101; \
border##d = PATTERN_NONE; \
    break; \
  case 7: \
border##p = CSP_SINGLE_LINE; \
border##d = CSD_DOTTED; \
    break; \
  case 0: \
  default: \
border##p = border##d = PATTERN_NONE; \
    break; \
} \
border##c = get_color_in_palette(palette, nb_color, (i >> 3) & 0x1F)
#else

#ifdef ___GNUC_COMPOSE_MACRO

#define decode_xf_border( border) \
switch( i & 7) \
{ \
  case 1: \
##border##p = CSP_SINGLE_LINE; \
##border##d = PATTERN_NONE; \
    break; \
  case 2: \
##border##p = CSP_DOUBLE_LINE; \
##border##d = PATTERN_NONE; \
    break; \
  case 3: \
##border##p = CSP_SINGLE_LINE; \
##border##d = CSD_DASH; \
    break; \
  case 4: \
##border##p = CSP_SINGLE_LINE; \
##border##d = CSD_DASH_SHORT; \
    break; \
  case 5: \
##border##p = CSP_1101; \
##border##d = PATTERN_NONE; \
    break; \
  case 6: \
##border##p = CSP_101; \
##border##d = PATTERN_NONE; \
    break; \
  case 7: \
##border##p = CSP_SINGLE_LINE; \
##border##d = CSD_DOTTED; \
    break; \
  case 0: \
  default: \
##border##p = ##border##d = PATTERN_NONE; \
    break; \
} \
##border##c = get_color_in_palette(palette, nb_color, (i >> 3) & 0x1F)

#else

#define decode_xf_border( border) \
switch( i & 7) \
{ \
  case 0: \
    /**/border/**/p = /**/border/**/d = PATTERN_NONE; \
    break; \
  case 1: \
    /**/border/**/p = CSP_SINGLE_LINE; \
    /**/border/**/d = PATTERN_NONE; \
    break; \
  case 2: \
    /**/border/**/p = CSP_DOUBLE_LINE; \
    /**/border/**/d = PATTERN_NONE; \
    break; \
  case 3: \
    /**/border/**/p = CSP_SINGLE_LINE; \
    /**/border/**/d = CSD_DASH; \
    break; \
  case 4: \
    /**/border/**/p = CSP_SINGLE_LINE; \
    /**/border/**/d = CSD_DASH_SHORT; \
    break; \
  case 5: \
    /**/border/**/p = CSP_1101; \
    /**/border/**/d = PATTERN_NONE; \
    break; \
  case 6: \
    /**/border/**/p = CSP_101; \
    /**/border/**/d = PATTERN_NONE; \
    break; \
  case 7: \
    /**/border/**/p = CSP_SINGLE_LINE; \
    /**/border/**/d = CSD_DOTTED; \
    break; \
} \
/**/border/**/c = get_color_in_palette(palette, nb_color, (i >> 3) & 0x1F)

#endif
#endif /* PSEUDO */

typedef struct {
  unsigned short xl_func_number;
  int	xl_nb_args;
  char	*xq_func_name;
  int	xq_func_res;
} xl_to_xq_func1_t;

typedef struct {
  unsigned short	xl_func_number;
  char	*xq_func_name;
  int	xq_func_res;
} xl_to_xq_func2_t;

#define NB_XL2XQ_FUNC1 58

static xl_to_xq_func1_t xl_to_xq_func1[NB_XL2XQ_FUNC1] =
{
  { 0x18, 1, NULL, XcR_funcAbs},
  { 0x63, 1, NULL, XcR_funcAcos},
  { 0xe9, 1, NULL, XcR_funcAcosh},
  { 0x3f, 0, NULL, XcR_funcAlea},
  { 0x8e, 3, NULL, XcR_funcSln},
  { 0x1b, 2, NULL, XcR_funcRound},
  { 0x62, 1, NULL, XcR_funcAsin},
  { 0xe8, 1, NULL, XcR_funcAsinh},
  { 0x12, 1, NULL, XcR_funcAtan},
  { 0x61, 2, NULL, XcR_funcAtan2},
  { 0xea, 1, NULL, XcR_funcAtanh},
  { 0x6f, 1, NULL, XcR_funcChar},
  { 0x21, 1, NULL, XcR_funcCnum},
  { 0x79, 1, NULL, XcR_funcCode},
  { 0x10, 1, NULL, XcR_funcCos},
  { 0xe6, 1, NULL, XcR_funcCosh},
  { 0x19, 1, NULL, XcR_funcInt},
  { 0xa2, 1, NULL, XcR_funcPurge},
  { 0x75, 2, NULL, XcR_funcExact},
  { 0x15, 1, NULL, XcR_funcExp},
  { 0xb8, 1, NULL, XcR_funcFact},
  { 0x23, 0, NULL, XcR_funcFalse},
  { 0x16, 1, NULL, XcR_funcLn},
  { 0x17, 1, NULL, XcR_funcLog10},
  { 0x71, 1, NULL, XcR_funcUpper},
  { 0x70, 1, NULL, XcR_funcLower},
  { 0x27, 2, NULL, XcR_funcMod},
  { 0x20, 1, NULL, XcR_funcLength},
  { 0x72, 1, NULL, XcR_funcProper},
  { 0x13, 0, NULL, XcR_funcPi},
  { 0x14, 1, NULL, XcR_funcSqrt},
  { 0x77, 4, NULL, XcR_funcReplace},
  { 0x1e, 2, NULL, XcR_funcRepeat},
  { 0x1a, 1, NULL, XcR_funcSign},
  { 0x0f, 1, NULL, XcR_funcSin},
  { 0xe5, 1, NULL, XcR_funcSinh},
  { 0x1f, 3, NULL, XcR_funcMid},
  { 0x76, 1, NULL, XcR_funcTrim},
  { 0x82, 1, NULL, XcR_funcT},
  { 0x11, 1, NULL, XcR_funcTan},
  { 0xe7, 1, NULL, XcR_funcTanh},
  { 0x30, 2, NULL, XcR_funcText},
  { 0x22, 0, NULL, XcR_funcTrue},
  { 0x26, 1, NULL, XcR_funcNot},
  { 0x45, 1, NULL, XcR_funcYear},
  { 0xdd, 0, NULL, XcR_funcToday},
  { 0x41, 3, NULL, XcR_funcDate},
  { 0x8c, 1, NULL, XcR_funcDateval},
  { 0x47, 1, NULL, XcR_funcHour},
  { 0x43, 1, NULL, XcR_funcDay},
  { 0x4a, 1, NULL, XcR_funcNow},
  { 0x48, 1, NULL, XcR_funcMinute},
  { 0x44, 1, NULL, XcR_funcMonth},
  { 0x49, 1, NULL, XcR_funcSecond},
  { 0x42, 3, NULL, XcR_funcTime},
  { 0x8d, 1, NULL, XcR_funcTimeval},
  { 0x8f, 4, NULL, XcR_funcSyd},
  { 0x3d, 3, NULL, XcR_funcMirr}
};

#define NB_XL2XQ_FUNC2 33
static xl_to_xq_func2_t xl_to_xq_func2[NB_XL2XQ_FUNC2] =
{
  { 0xdb, NULL, XcR_funcAdress},
  { 0x52, NULL, XcR_funcFind},
  { 0x09, NULL, XcR_funcCol},
  { 0x0e, NULL, XcR_funcCtxt},
  { 0x74, NULL, XcR_funcRight},
  { 0x24, NULL, XcR_funcAnd},
  { 0x0d, NULL, XcR_funcCurrency},
  { 0x73, NULL, XcR_funcLeft},
  { 0x94, NULL, XcR_funcIndirect},
  { 0x08, NULL, XcR_funcRow},
  { 0x6d, NULL, XcR_funcLog},
  { 0x07, NULL, XcR_funcMax},
  { 0x06, NULL, XcR_funcMin},
  { 0x05, NULL, XcR_funcAvg},
  { 0xb7, NULL, XcR_funcProd},
  { 0x04, NULL, XcR_funcSumm},
  { 0x78, NULL, XcR_funcSubstitute},
  { 0xc5, NULL, XcR_funcTrunc},
  { 0x7c, NULL, XcR_funcMatch},
  { 0x25, NULL, XcR_funcOr},
  { 0xf7, NULL, XcR_funcDb},
  { 0x90, NULL, XcR_funcDdb},
  { 0xa7, NULL, XcR_funcIpmt},
  { 0x3a, NULL, XcR_funcNper},
  { 0xa8, NULL, XcR_funcPpmt},
  { 0x3c, NULL, XcR_funcRate},
  { 0x3e, NULL, XcR_funcIrr},
  { 0x38, NULL, XcR_funcPv},
  { 0x0b, NULL, XcR_funcNpv},
  { 0x39, NULL, XcR_funcFv},
  { 0xde, NULL, XcR_funcVdb},
  { 0x3b, NULL, XcR_funcPmt},
  { 0xff, NULL, -1}
};

static char *xl_operator[] = 
{ "+", "-", "*", "/", "^", "&", "<", "<=", ">", ">=", "=", "!=", " ", ",",
    "", "+", "-", "%"};

#endif /* _Import_ExcelP_h_ */





