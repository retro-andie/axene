/*
** CellStyleInit.h for XQuad in CellStyle/
** define CellStyleInit Object
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
** Started on  Tue Mar 28 13:43:16 1995 Emmanuel Paris
** Last update Tue Jan 18 16:24:26 2000 Emmanuel Paris
*/

#ifndef _CellStyleInit_h_
#define _CellStyleInit_h_

enum 
{
 CSP_SINGLE_LINE,
 CSP_DOUBLE_LINE,
 CSP_101,
 CSP_1001,
 CSP_1101,
 CSP_1011,
 CSP_10,
 CSP_011,
 CSP_110,
 CSP_1100
};

enum 
{
 CSD_DOTTED,
 CSD_DASH_SHORT,
 CSD_DASH,
 CSD_AXE
};

#if defined(_CELLSTYLE_PRIVATE_) || defined(_CELLSTYLE_PATTERN_) \
|| defined(_CELLSTYLE_DASHES_)

#ifdef _CELLSTYLE_PRIVATE_
#include "../Pixmaps/fond01.bmp"
#include "../Pixmaps/fond02.bmp"
#include "../Pixmaps/fond03.bmp"
#include "../Pixmaps/fond04.bmp"
#include "../Pixmaps/fond05.bmp"
#include "../Pixmaps/fond06.bmp"
#include "../Pixmaps/fond07.bmp"
#include "../Pixmaps/fond08.bmp"
#include "../Pixmaps/fond09.bmp"
#include "../Pixmaps/fond10.bmp"
#include "../Pixmaps/fond11.bmp"
#include "../Pixmaps/fond12.bmp"
#include "../Pixmaps/fond13.bmp"
#include "../Pixmaps/fond14.bmp"
#include "../Pixmaps/fond15.bmp"
#include "../Pixmaps/fond16.bmp"
#include "../Pixmaps/fond17.bmp"
#include "../Pixmaps/fond18.bmp"
#include "../Pixmaps/fond19.bmp"
#include "../Pixmaps/fond20.bmp"
#include "../Pixmaps/fond21.bmp"
#include "../Pixmaps/fond22.bmp"
#include "../Pixmaps/fond23.bmp"
#include "../Pixmaps/fond24.bmp"
#include "../Pixmaps/fond25.bmp"
#include "../Pixmaps/fond26.bmp"
#include "../Pixmaps/fond27.bmp"
#include "../Pixmaps/fond28.bmp"
#include "../Pixmaps/fond29.bmp"
#include "../Pixmaps/fond30.bmp"
#include "../Pixmaps/fond31.bmp"
#include "../Pixmaps/fond32.bmp"
#include "../Pixmaps/fond33.bmp"
#include "../Pixmaps/fond34.bmp"
#include "../Pixmaps/fond35.bmp"
#include "../Pixmaps/fond36.bmp"
#include "../Pixmaps/fond37.bmp"
#include "../Pixmaps/fond38.bmp"
#include "../Pixmaps/fond39.bmp"

boolean	CellStyle_init_done = FALSE;

#endif

#if defined(_CELLSTYLE_PRIVATE_) || defined(_CELLSTYLE_DASHES_)
typedef struct 
{
 int	dash_offset;
 char	*dash_list;
 int	dash_length;
} CellDashesStyle_t;

#define NB_CELL_DASHES	4
#endif

#ifdef _CELLSTYLE_PRIVATE_
CellDashesStyle_t	Cell_Dashes[NB_CELL_DASHES] = 
{
 { 0, "\1\1", 2},
 { 0, "\2\2", 2},
 { 0, "\4\1", 2},
 { 0, "\1\1\3\1", 4} };
#endif

#ifdef _CELLSTYLE_DASHES_
extern CellDashesStyle_t	Cell_Dashes[NB_CELL_DASHES];
#endif

#if defined(_CELLSTYLE_PRIVATE_) || defined(_CELLSTYLE_PATTERN_)
#define PNULL	(Pixmap)NULL
#define PATTERN_LINE		FALSE
#define PATTERN_DATA		TRUE

typedef struct 
{
 boolean	*definition;
} CellPatternLineStyle_t;

typedef struct 
{
 unsigned char	*definition;
 Pixmap		pixmap;
} CellPatternDataStyle_t;

typedef union 
{
 CellPatternLineStyle_t	line;
 CellPatternDataStyle_t	data;
} CellPatternDef_u;

typedef struct 
{
 boolean		type;
 int			width;
 int			height;
 CellPatternDef_u	definition;
} CellPatternStyle_t;

#define NB_CELL_PATTERN 49
#endif

#ifdef _CELLSTYLE_PRIVATE_

#define NB_LINE_PATTERN 10
boolean CPLine1Def[] = { TRUE};
boolean CPLine2Def[] = { TRUE, TRUE};
boolean CPLine3Def[] = { TRUE, FALSE, TRUE};
boolean CPLine4Def[] = { TRUE, FALSE, FALSE, TRUE};
boolean CPLine5Def[] = { TRUE, TRUE, FALSE, TRUE};
boolean CPLine6Def[] = { TRUE, FALSE, TRUE, TRUE};
boolean CPLine7Def[] = { TRUE, FALSE };
boolean CPLine8Def[] = { FALSE, TRUE, TRUE };
boolean CPLine9Def[] = { TRUE, TRUE, FALSE };
boolean CPLine10Def[]= { TRUE, TRUE, FALSE, FALSE};

boolean	*CPLineDef[NB_LINE_PATTERN] = 
{
 CPLine1Def, CPLine2Def, CPLine3Def, CPLine4Def, CPLine5Def,
 CPLine6Def, CPLine7Def, CPLine8Def, CPLine9Def, CPLine10Def
};

CellPatternDataStyle_t	CPData[] = 
{
 { fond01_bits, PNULL} , { fond02_bits, PNULL} ,
 { fond03_bits, PNULL} , { fond04_bits, PNULL} ,
 { fond05_bits, PNULL} , { fond06_bits, PNULL} ,
 { fond07_bits, PNULL} , { fond08_bits, PNULL} ,
 { fond09_bits, PNULL} , { fond10_bits, PNULL} ,
 { fond11_bits, PNULL} , { fond12_bits, PNULL} ,
 { fond13_bits, PNULL} , { fond14_bits, PNULL} ,
 { fond15_bits, PNULL} , { fond16_bits, PNULL} ,
 { fond17_bits, PNULL} , { fond18_bits, PNULL} ,
 { fond19_bits, PNULL} , { fond20_bits, PNULL} ,
 { fond21_bits, PNULL} , { fond22_bits, PNULL} ,
 { fond23_bits, PNULL} , { fond24_bits, PNULL} ,
 { fond25_bits, PNULL} , { fond26_bits, PNULL} ,
 { fond27_bits, PNULL} , { fond28_bits, PNULL} ,
 { fond29_bits, PNULL} , { fond30_bits, PNULL} ,
 { fond31_bits, PNULL} , { fond32_bits, PNULL} ,
 { fond33_bits, PNULL} , { fond34_bits, PNULL} ,
 { fond35_bits, PNULL} , { fond36_bits, PNULL} ,
 { fond37_bits, PNULL} , { fond38_bits, PNULL} ,
 { fond39_bits, PNULL} };

CellPatternStyle_t	Cell_Pattern[NB_CELL_PATTERN] = 
{
 { PATTERN_LINE, 4, 1, },
 { PATTERN_LINE, 4, 2, },
 { PATTERN_LINE, 4, 3, },
 { PATTERN_LINE, 4, 4, },
 { PATTERN_LINE, 4, 4, },
 { PATTERN_LINE, 4, 4, },
 { PATTERN_LINE, 4, 2, },
 { PATTERN_LINE, 4, 3, },
 { PATTERN_LINE, 4, 3, },
 { PATTERN_LINE, 4, 4, },
 { PATTERN_DATA, fond01_width, fond01_height, },
 { PATTERN_DATA, fond02_width, fond02_height, },
 { PATTERN_DATA, fond03_width, fond03_height, },
 { PATTERN_DATA, fond04_width, fond04_height, },
 { PATTERN_DATA, fond05_width, fond05_height, },
 { PATTERN_DATA, fond06_width, fond06_height, },
 { PATTERN_DATA, fond07_width, fond07_height, },
 { PATTERN_DATA, fond08_width, fond08_height, },
 { PATTERN_DATA, fond09_width, fond09_height, },
 { PATTERN_DATA, fond10_width, fond10_height, },
 { PATTERN_DATA, fond11_width, fond11_height, },
 { PATTERN_DATA, fond12_width, fond12_height, },
 { PATTERN_DATA, fond13_width, fond13_height, },
 { PATTERN_DATA, fond14_width, fond14_height, },
 { PATTERN_DATA, fond15_width, fond15_height, },
 { PATTERN_DATA, fond16_width, fond16_height, },
 { PATTERN_DATA, fond17_width, fond17_height, },
 { PATTERN_DATA, fond18_width, fond18_height, },
 { PATTERN_DATA, fond19_width, fond19_height, },
 { PATTERN_DATA, fond20_width, fond20_height, },
 { PATTERN_DATA, fond21_width, fond21_height, },
 { PATTERN_DATA, fond22_width, fond22_height, },
 { PATTERN_DATA, fond23_width, fond23_height, },
 { PATTERN_DATA, fond24_width, fond24_height, },
 { PATTERN_DATA, fond25_width, fond25_height, },
 { PATTERN_DATA, fond26_width, fond26_height, },
 { PATTERN_DATA, fond27_width, fond27_height, },
 { PATTERN_DATA, fond28_width, fond28_height, },
 { PATTERN_DATA, fond29_width, fond29_height, },
 { PATTERN_DATA, fond30_width, fond30_height, },
 { PATTERN_DATA, fond31_width, fond31_height, },
 { PATTERN_DATA, fond32_width, fond32_height, },
 { PATTERN_DATA, fond33_width, fond33_height, },
 { PATTERN_DATA, fond34_width, fond34_height, },
 { PATTERN_DATA, fond35_width, fond35_height, },
 { PATTERN_DATA, fond36_width, fond36_height, },
 { PATTERN_DATA, fond37_width, fond37_height, },
 { PATTERN_DATA, fond38_width, fond38_height, },
 { PATTERN_DATA, fond39_width, fond39_height, } };
#endif

#ifdef _CELLSTYLE_PATTERN_

#define CELL_PATTERN_DATA_FIRST 10
extern CellPatternStyle_t Cell_Pattern[NB_CELL_PATTERN];

#endif

#endif

#endif
