/*
** Import_Excel.c for XQuad in Import/
** Import_Excel Object
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
** Last update Thu Sep 11 20:17:50 1997 Robin Castanier
*/

#define _CELLSTYLE_PATTERN_
#include "Import_Excel.h"
#include "Import_ExcelP.h"
#include <math.h>

extern c_VectorFont    *GlobFontBase;

static void *cons ___PROTO((int style));
static void dest ___PROTO((c_Import_Excel *This));
static void *copy ___PROTO((c_Import_Excel *This));

static boolean test ___PROTO((c_Import_Excel *This));
static boolean create_new_sheet ___PROTO((c_Import_Excel *This));
static error load ___PROTO((c_Import_Excel * This, c_Tableau *sheet));

static unsigned short read_record();
static unsigned short read_short();
static int read_integer();
static char *read_short_string();
static char *read_long_string();
static double read_8byte_IEEE();
static double transform_int_to_30bits_IEEE();
static void special_value_format();

static c_Color *get_color_in_palette();
static c_TextStyle *get_tstyle_in_list();
static c_FormatNbr *get_format_in_list();
static CellStyle_t *get_cstyle_in_list();
static char *get_name_in_list();
static CellStyle_t *read_cell_attributes();
static int get_erreur_res();
static char *read_formula();

static boolean			box_created = FALSE;
static import_excel_box_t	box;

static char biff2_bof[8] = { 9, 0, 4, 0, 7, 0, 16, 0};
static char biff3_bof[8] = { 9, 2, 6, 0, 0, 0, 16, 0};
static char biff4_bof[8] = { 9, 4, 6, 0, 0, 0, 16, 0};
static char short_string[258];

sf_Import_Std fc_Import_Excel = 
{
  cons,
  dest,
  copy,
  xl_create_attribs_box,
  xl_manage_attribs_box,
  xl_destroy_attribs_box,
  test,
  create_new_sheet,
  load
};

static 	char *Extensions_Excel[] = { "xls" };

static void *cons(style)
int style;
{
  c_Import_Excel	*This;
  
  if ((This = (c_Import_Excel *)Xc_malloc("Import Excel", 
					 sizeof(c_Import_Excel))) == NULL)
  {
    Xc_FATAL(("Can't build this object: memory allocation error"));
  }
  This->f = &fc_Import_Excel;

  switch(style)
    {
    case 2:
      This->name = This->id = "Excel 2";
      This->copyright = "Copyright Axene 1995";
      This->extension_count = 1;
      This->extensions = Extensions_Excel;
      This->version = 2;
      break;
    case 3:
      This->name = This->id = "Excel 3";
      This->copyright = "Copyright Axene 1995";
      This->extension_count = 1;
      This->extensions = Extensions_Excel; 
      This->version = 3;
      break;
    case 4:
      This->name = This->id = "Excel 4";
      This->copyright = "Copyright Axene 1995";
      This->extension_count = 1;
      This->extensions = Extensions_Excel;
      This->version = 4;
      break;
    }
  
  This->filename = NULL;
  This->box_created = &box_created;
  
  This->box = &box;
  return This;
}

static void dest(This)
c_Import_Excel *This;
{
  Xc_free(This);
}

static void *copy(This)
c_Import_Excel *This;
{
  Xc_TRACE(("COPY Import_Excel Object not done: normally not usefull."));
  return NULL;
}


static boolean test(This)
c_Import_Excel *This;
{
  FILE	*file;
  int	taille;
  char	bof[10];
  
  if (!(file = fopen(This->filename, "rb")))
    {
      Xc_WARNING(("File %s doesn't exist or unreadable", This->filename));
      return XC_FILE_NOT_EXIST;
    }
  
  fseek(file, 0L, SEEK_END);
  taille = ftell(file);
  fseek(file, 0L, SEEK_SET);
  
  if (taille < 14)
    return FALSE;
  
  if (fread(bof, 10, 1, file) != 1)
    {
      Xc_WARNING(("Error while reading file: %s", This->filename));
    }
  
  switch(This->version)
    {
    case BIFF2:
      if (memcmp(bof, biff2_bof, 8))
	return FALSE;
      break;
    case BIFF3:
      if (memcmp(bof, biff3_bof, 8))
	return FALSE;
      break;
    case BIFF4:
      if (memcmp(bof, biff4_bof, 8))
	return FALSE;
      break;
    }

  return TRUE;
}

static boolean create_new_sheet(This)
c_Import_Excel *This; 
{
  return TRUE;
}

static error load(This, sheet)
c_Import_Excel	*This;
c_Tableau *sheet;
{
  FILE	*file;
  c_Feuille	*feuille;
  c_Moteur	*moteur;
  c_Color	*colorbase;
  c_TextStyle	*tstylebase;
  c_FormatNbr	*formatbase;
  c_CellStyle	*cellstylebase;
  c_Color *color = NULL;
  c_TextStyle  *tstyle;
  boolean exit;
  char	*rec_data, *data;
  unsigned char *udata;
  unsigned short rec_type;
  CellStyle_t	*xf;
  cell	*cellule;
  int	rec_length, rec_data_length, rec_version;
  error	erreur;
  region_t region;
  int i, j, min_row, max_row, min_col, max_col, row, col;
  coord_t	swidth, sheight;
  encoding_t	encoding;
  int		nb_color;
  boolean	date_1904 = FALSE;
  c_Color	**palette = NULL;
  tstyle_list_t *tstyle_list = NULL;
  format_list_t *format_list = NULL;
  cstyle_list_t *cell_xf_list = NULL;
  cstyle_list_t *style_xf_list = NULL;
  name_list_t	*name_list = NULL;
  static xl_string_set_t string_set = { FALSE, 0, 0};
  
  if (!(file = fopen(This->filename,"rb")))
  {
    Xc_WARNING(("File %s doesn't exist or unreadable", This->filename));
    return XC_FILE_NOT_EXIST;
  }
  fseek(file, 0L, SEEK_SET);
  
  feuille = sheet->Feuille;
  moteur = sheet->moteur;
  colorbase = sheet->BaseStd.color_base;
  tstylebase = sheet->BaseStd.text_style_base;
  formatbase = sheet->BaseStd.format_base;
  cellstylebase = sheet->BaseStd.cell_style_base;
  encoding = XcE_LATIN1;
  if (This->version == BIFF2)
    nb_color = NB_EXCEL2_COLOR;
  else
    nb_color = NB_EXCEL34_COLOR;
  palette = (c_Color **)Xc_malloc("palette", 
				  nb_color * sizeof(c_Color *));
  if (This->version == BIFF2)
  {
    for(i=0; i<nb_color; i++)
    {
      palette[i] = 
	F(colorbase).getColor(&(sheet->BaseStd.color_base), XcC_RGB,
			      XcC_SCALE_COLOR(xl2_palette[i].red),
			      XcC_SCALE_COLOR(xl2_palette[i].green),
			      XcC_SCALE_COLOR(xl2_palette[i].blue),
			      XcC_HIDE_OFF, XcC_END);
    }
  }
  else
  {
    for(i=0; i<nb_color; i++)
    {
      palette[i] = 
	F(colorbase).getColor(&(sheet->BaseStd.color_base), XcC_RGB,
			      XcC_SCALE_COLOR(xl34_palette[i].red),
			      XcC_SCALE_COLOR(xl34_palette[i].green),
			      XcC_SCALE_COLOR(xl34_palette[i].blue),
			      XcC_HIDE_OFF, XcC_END);
    }
  }  
  rec_length = rec_data_length = 20;
  rec_data = (char *)Xc_malloc("rec data", 20);
  
  erreur = XC_NO_ERROR;
  exit = FALSE;
  while(!exit)
  {
    rec_type = read_record(This, file, &rec_data, &rec_data_length, 
			   &rec_length, &rec_version);
    data = rec_data; udata = (unsigned char *)rec_data;
    switch(rec_type)
    {
    case XcXL_DIMENSIONS:
      min_row = read_short(data); data += 2;
      max_row = read_short(data); data += 2;
      min_col = read_short(data); data += 2;
      max_col = read_short(data);
      Xc_TRACE(("sheet dimension: %d-%d %d|%d", min_col, max_col, 
		min_row, max_row));
      break;
      
    case XcXL_BOF:
      Xc_TRACE(("Beginnig Of File - Excel %d", This->version));
      break;
      
    case XcXL_EOF:
      Xc_TRACE(("End Of File"));
      exit = TRUE;
      break;
      
    case XcXL_CODEPAGE:
      switch((unsigned short)read_short(data))
      {
      case 0x01B5:
      	Xc_TRACE(("encoding: DOS - OS/2"));
	encoding = XcE_DOS;
	break;
      case 0x8000:
      	Xc_TRACE(("encoding: Macintosh"));
	encoding = XcE_MAC;
	break;
      case 0x8001:
      case 0x04E4:
	Xc_TRACE(("encoding: ANSI")); 
	encoding = XcE_LATIN1;
	break;
      }
      break;
      
    case XcXL_PRINTHEADERS:
      if (read_short(data))
	feuille->print_setup->rowcol_f = TRUE;
      else
	feuille->print_setup->rowcol_f = FALSE;
      break;
    case XcXL_PRINTGRIDLINES:
      if (read_short(data))
	feuille->print_setup->grid_f = TRUE;
      else
	feuille->print_setup->grid_f = FALSE;
      break;
    case XcXL_HCENTER:
      if (read_short(data))
	feuille->print_setup->hcenter_f = TRUE;
      else
	feuille->print_setup->hcenter_f = FALSE;
      break;
    case XcXL_VCENTER:
      if (read_short(data))
	feuille->print_setup->vcenter_f = TRUE;
      else
	feuille->print_setup->vcenter_f = FALSE;
      break;
    case XcXL_WSBOOL:
      i = read_short(data);
      feuille->page_mark = TEST_BIT(i, 0);
      break;
    case XcXL_LEFTMARGIN:
      {
	double	margin;
	
	margin = read_8byte_IEEE(data);
	feuille->print_setup->left_margin = SCALE_FROM_INCHES(margin);
      }
      break;
    case XcXL_RIGHTMARGIN:
      {
	double	margin;
	
	margin = read_8byte_IEEE(data);
	feuille->print_setup->right_margin = SCALE_FROM_INCHES(margin);
      }
      break;
    case XcXL_BOTTOMMARGIN:
      {
	double	margin;
	
	margin = read_8byte_IEEE(data);
	feuille->print_setup->bottom_margin = SCALE_FROM_INCHES(margin);
      }
      break;
    case XcXL_TOPMARGIN:
      {
	double	margin;
	
	margin = read_8byte_IEEE(data);
	feuille->print_setup->top_margin = SCALE_FROM_INCHES(margin);
      }
      break;
    case XcXL_PLS:
      i = (read_short(data)) & 0xff; data += 2;
      
      switch(i)
      {
      case 0:
	Xc_TRACE(("PLS record for Windows"));	
	j = read_short(data); data += 2;
	if (j == 1)
	  feuille->print_setup->orientation = Xc_PORTRAIT;
	if (j == 2)
	  feuille->print_setup->orientation = Xc_LANDSCAPE;
	break;
      case 1:
	Xc_TRACE(("PLS record for Macintosh"));
      	break;
      case 2:
	Xc_TRACE(("PLS record for OS/2"));
      	break;
      }
      break;
      
    case XcXL_FONT:
    case XcXL_FONT2:
      {
	c_VectorFont *font;
	tstyle_list_t *tstylel;
	coord_t	font_height;
	char	*font_name, name[XcMAX_NAME_LENGTH];
	int	xl_attribs, xq_attribs = 0;
	boolean	bold = FALSE, italic = FALSE, strikeout = FALSE;
	
	font_height = SCALE_FROM_POINTS( read_short(data) / 20.0); data += 2;
	xl_attribs = read_short(data); data += 2;
	if (rec_version == REC_BIFF2)
	{
	  if (TEST_BIT(xl_attribs, 0))  bold = TRUE;
	  if (TEST_BIT(xl_attribs, 1))  italic = TRUE;
	  if (TEST_BIT(xl_attribs, 2)) xq_attribs |= XcTS_B_UNDERLINE;
	  if (TEST_BIT(xl_attribs, 3)) strikeout = TRUE;
	}
	else
	{
	  if (TEST_BIT(xl_attribs, 0)) bold = TRUE;
	  if (TEST_BIT(xl_attribs, 1)) italic = TRUE;
	  if (TEST_BIT(xl_attribs, 2)) xq_attribs |= XcTS_B_UNDERLINE;
	  if (TEST_BIT(xl_attribs, 3)) strikeout = TRUE;
	  if (TEST_BIT(xl_attribs, 4)) xq_attribs |= XcTS_B_OUTLINE;
	  if (TEST_BIT(xl_attribs, 5)) xq_attribs |= XcTS_B_SHADOW;
	  color = get_color_in_palette(palette, nb_color, read_short(data));
	  data += 2;
	}
	
	font_name = read_short_string(&data, encoding);
	font = F(GlobFontBase).getFontByFamilyAndAttribs
	  (GlobFontBase, font_name, bold, italic);
	
	sprintf(name, "Xl_%.15s_%d_%c%c", font_name, 
		(int)SCALE_TO_POINTS(font_height), 
		bold?'B':'-', italic?'I':'-');
	
	if (rec_version == REC_BIFF2)
	{
	  if (!strikeout)
	  {
	    tstyle = F(tstylebase).get
	      (NULL, &(sheet->BaseStd), XcTS_SIZE, font_height, 
	       XcTS_FONT, font, XcTS_ATTRIB, xq_attribs, XcTS_NAME, name,
	       XcTS_END);
	  }
	  else
	  {
	    tstyle = F(tstylebase).get
	      (NULL, &(sheet->BaseStd), XcTS_SIZE, font_height, 
	       XcTS_FONT, font, XcTS_ATTRIB, xq_attribs, XcTS_NAME, name,
	       XcTS_STRIKEOUT_ON, XcTS_END);
	  }
	}
	else
	{
	  if (!strikeout)
	  {
	    tstyle = F(tstylebase).get
	      (NULL, &(sheet->BaseStd), XcTS_SIZE, font_height, 
	       XcTS_FONT, font, XcTS_ATTRIB, xq_attribs, XcTS_NAME, name, 
	       XcTS_COLOR, color, XcTS_END);
	  }
	  else
	  {
	    tstyle = F(tstylebase).get
	      (NULL, &(sheet->BaseStd), XcTS_SIZE, font_height, 
	       XcTS_FONT, font, XcTS_ATTRIB, xq_attribs, XcTS_NAME, name, 
	       XcTS_COLOR, color, XcTS_STRIKEOUT_ON, XcTS_END);	    
	  }
	}
	tstylel = tstyle_list;
	if (tstylel)
	{
	  while(tstylel->Next)
	    tstylel = tstylel->Next;
	  tstylel->Next = 
	    (tstyle_list_t *)Xc_malloc("xl font", sizeof(tstyle_list_t));
	  tstylel = tstylel->Next;
	}
	else
	{
	  tstyle_list = tstylel = 
	    (tstyle_list_t *)Xc_malloc("xl font", sizeof(tstyle_list_t));
	}
	tstylel->tstyle = tstyle;
	tstylel->Next = NULL;
      }
      break;
      case XcXL_EFONT:
      {
	c_Color	*color;
	tstyle_list_t	*tstylel;
	
	tstylel = tstyle_list;
	if (!tstylel)
	{
	  Xc_WARNING(("EFONT record before any FONT record ignored"));
	}
	else
	{
	  while(tstylel->Next)
	    tstylel = tstylel->Next;
	  col = read_short(data);
	  color = get_color_in_palette(palette, nb_color, col);
	  F(tstylebase).set(tstylel->tstyle, XcTS_COLOR, color, XcTS_END);
	}
      }
      break;
    
    case XcXL_FORMAT:
      {
	char	format_name[XcMAX_NAME_LENGTH], *format_record;
	format_list_t *formatl;
	c_FormatNbr *format;
	
	if (rec_version == REC_BIFF4) data += 2;
	format_record = read_short_string(&data, encoding);
	sprintf(format_name, "Xl_%.20s", format_record);
	format = F(formatbase).get(NULL, XcFt_NORMAL, &(sheet->BaseStd),
				   XcF_NAME, format_name, 
				   XcF_PARSE_STRING, format_record, XcF_END);
	formatl = format_list;
	if (formatl)
	{
	  while(formatl->Next)
	    formatl = formatl->Next;
	  formatl->Next = 
	    (format_list_t *)Xc_malloc("xl format", sizeof(format_list_t));
	  formatl = formatl->Next;
	}
	else
	{
	  format_list = formatl = 
	    (format_list_t *)Xc_malloc("xl format", sizeof(format_list_t));
	}
	formatl->format = format;
	formatl->Next = NULL;
      }
      break;
    case XcXL_FORMATCOUNT:
    case XcXL_BUILTINFMTCOUNT:
      Xc_TRACE(("Builtin Format Count: %d", read_short(data)));
      break;
    case XcXL_XF:
      {
	unsigned short flags;
	int	num_font, num_format;
	ruler_t	ruler;
	CellStyle_t  *cstyle;
	c_TextStyle *tstyle;
	c_FormatNbr *format;
	int pattern;
	c_Color	*patternb = NULL, *patternf = NULL;
	int leftp, rightp, topp, bottomp;
	int leftd, rightd, topd, bottomd;
	c_Color	*leftc, *rightc, *topc, *bottomc;
	cstyle_list_t *cstylel;
	boolean	style = FALSE;
	
	switch(rec_version)
	{
	case REC_BIFF2:
	  num_font = *udata; udata++;
	  if ((tstyle = get_tstyle_in_list(tstyle_list, num_font)) == NULL)
	    break;
	  
	  udata++; num_format = (*udata) & 0x1F; udata++;
	  if ((format = get_format_in_list(format_list, num_format)) == NULL)
	    break;
	  
	  i = *udata;
	  switch(i & 7)
	  {
	  case 1:
	    ruler = XqR_H_ALIGN_LEFT;
	    break;
	  case 2:
	    ruler = XqR_H_ALIGN_CENTER;
	    break;
	  case 3:
	    ruler = XqR_H_ALIGN_RIGHT;
	    break;
	  case 4:
	    ruler = XqR_H_JUSTIFIED;
	    break;
	  case 0:
	  default:
	    ruler = XqR_DEFAULT;
	    break;
	  }
	  ruler |= XqR_V_ALIGN_BOTTOM;
	  cstyle = F(cellstylebase).CreateNewCellStyle(cellstylebase, 
						       CELL_STYLE_DEFAULT);
	  SET_CELL_STYLE(cstyle, 
			 XqCS_TEXTSTYLE, tstyle, XqCS_FORMAT, format,
			 XqCS_ALIGNEMENT, ruler, XqCS_END);
	  if (TEST_BIT(i, 3)) 
	    SET_CELL_STYLE
	      (cstyle, XqCS_LEFT_PATTERN, CSP_SINGLE_LINE, XqCS_END);
	  
	  if (TEST_BIT(i, 4))
	    SET_CELL_STYLE
	      (cstyle, XqCS_RIGHT_PATTERN, CSP_SINGLE_LINE, XqCS_END);
	  
	  if (TEST_BIT(i, 5))
	    SET_CELL_STYLE
	      (cstyle, XqCS_TOP_PATTERN, CSP_SINGLE_LINE, XqCS_END);
	  
	  if (TEST_BIT(i, 6))
	    SET_CELL_STYLE
	      (cstyle, XqCS_BOTTOM_PATTERN, CSP_SINGLE_LINE, XqCS_END);
	  
	  if (TEST_BIT(i, 7))	/* own implementation of shadow */
	    SET_CELL_STYLE (cstyle, 
			    XqCS_PATTERN, CELL_PATTERN_DATA_FIRST+2, XqCS_END);
	  
	  cstyle = F(cellstylebase).AddCellStyleToBase(cellstylebase,
						       cstyle);

	  cstylel = cell_xf_list;
	  if (cstylel)
	  {
	    while(cstylel->Next)
	      cstylel = cstylel->Next;
	    cstylel->Next = 
	      (cstyle_list_t *)Xc_malloc("xf", sizeof(cstyle_list_t));
	    cstylel = cstylel->Next;
	  }
	  else
	  {
	    cell_xf_list = cstylel = 
	      (cstyle_list_t *)Xc_malloc("xf", sizeof(cstyle_list_t));
	  }
	  cstylel->cstyle = cstyle;
	  cstylel->Next = NULL;
	  break;
	case REC_BIFF3:
	case REC_BIFF4:
	  num_font = *udata; udata++;
	  if ((tstyle = get_tstyle_in_list(tstyle_list, num_font)) == NULL)
	    break;
	  
	  num_format = (*udata) & 0x1F; udata++;
	  if ((format = get_format_in_list(format_list, num_format)) == NULL)
	    break;
	  
	  flags = read_short(udata); udata += 2;
	  if (TEST_BIT(flags, 2))
	    style = TRUE;
	  
	  i = read_short(udata); udata += 2;
	  switch(i & 7)
	  {
	  case 1:
	    ruler = XqR_H_ALIGN_LEFT;
	    break;
	  case 2:
	  case 6:
	    ruler = XqR_H_ALIGN_CENTER;
	    break;
	  case 3:
	    ruler = XqR_H_ALIGN_RIGHT;
	    break;
	  case 4:
	  case 5:
	    ruler = XqR_H_JUSTIFIED;
	    break;
	  case 0:
	  default:
	    ruler = XqR_DEFAULT;
	    break;
	  }
	  if (TEST_BIT(i, 3))
	    ruler |= XqR_MULTILINE;
	  
	  if (rec_version == REC_BIFF4)
	  {
	    i >>= 4;
	    switch(i & 3)
	    {
	    case 0:
	      ruler |= XqR_V_ALIGN_TOP;
	      break;
	    case 1:
	      ruler |= XqR_V_ALIGN_CENTER;
	      break;
	    case 2:
	      ruler |= XqR_V_ALIGN_BOTTOM;
	      break;
	    case 3:  /* extra-polation */
	      ruler |= XqR_V_JUSTIFIED;
	      break;
	    }
	    i >>= 2;
	    switch(i & 3)
	    {
	    case 0:
	      ruler |= XqR_A_DEFAULT;
	      break;
	    case 1:
	    case 3: /* will be XqR_A_180 when it work */
	      ruler |= XqR_A_90;
	      break;
	    case 2:
	      ruler |= XqR_A_270;
	      break;
	    }
	  }
	  
	  i = read_short(udata); udata += 2;
	  pattern = excel_pattern_convertion[i & 0x3F];
	  if (pattern != PATTERN_NONE)
	  {
	    if (((i >> 6) & 0x1F) >= nb_color)
	      patternf = NULL;
	    else
	      patternf = 
		get_color_in_palette(palette, nb_color, (i >> 6) & 0x1F);
	    if (((i >> 11) & 0x1F) >= nb_color)
	      patternb = NULL;
	    else
	      patternb = 
		get_color_in_palette(palette, nb_color, (i >> 11) & 0x1F);
	  }
	  
	  i = read_short(udata); udata += 2;
	  decode_xf_border(top);
	  i >>= 8;
	  decode_xf_border(left);
	  
	  i = read_short(udata); udata += 2;
	  decode_xf_border(bottom);
	  i >>= 8;
	  decode_xf_border(right);
	  	
	  cstyle = F(cellstylebase).CreateNewCellStyle(cellstylebase, 
						       CELL_STYLE_DEFAULT);
	  
	  SET_CELL_STYLE(cstyle, 
			 XqCS_TEXTSTYLE, tstyle, XqCS_FORMAT, format,
			 XqCS_ALIGNEMENT, ruler, 
			 XqCS_LEFT_FOREGROUND, leftc,
			 XqCS_LEFT_PATTERN, leftp, XqCS_LEFT_DASH, leftd,
			 XqCS_RIGHT_FOREGROUND, rightc,
			 XqCS_RIGHT_PATTERN, rightp, XqCS_RIGHT_DASH, rightd,
			 XqCS_TOP_FOREGROUND, topc,
			 XqCS_TOP_PATTERN, topp, XqCS_TOP_DASH, topd,
			 XqCS_BOTTOM_FOREGROUND, bottomc,
			 XqCS_BOTTOM_PATTERN, bottomp, 
			 XqCS_BOTTOM_DASH, bottomd,
			 XqCS_END);
	  
	  if (pattern != PATTERN_NONE)
	  {
	    if (patternf != NULL)
	    {
	      if (patternb != NULL)
	      {
		SET_CELL_STYLE(cstyle, XqCS_PATTERN, pattern,
			       XqCS_FOREGROUND, patternf,
			       XqCS_BACKGROUND, patternb,
			       XqCS_END);
	      }
	      else
	      {
		SET_CELL_STYLE(cstyle, XqCS_PATTERN, pattern,
			       XqCS_FOREGROUND, patternf,
			       XqCS_END);
	      }
	    }
	    else
	    {
	      if (patternb != NULL)
	      {
		SET_CELL_STYLE(cstyle, XqCS_PATTERN, pattern,
			       XqCS_BACKGROUND, patternb,
			       XqCS_END);
	      }
	      else
	      {
		SET_CELL_STYLE(cstyle, XqCS_PATTERN, pattern, XqCS_END);
	      }
	    }
	  }
	  cstyle = F(cellstylebase).AddCellStyleToBase(cellstylebase,
						       cstyle);
	  
	  cstylel = cell_xf_list;
	  if (cstylel)
	  {
	    while(cstylel->Next)
	      cstylel = cstylel->Next;
	    cstylel->Next = 
	      (cstyle_list_t *)Xc_malloc("xf", sizeof(cstyle_list_t));
	    cstylel = cstylel->Next;
	  }
	  else
	  {
	    cell_xf_list = cstylel = 
	      (cstyle_list_t *)Xc_malloc("xf", sizeof(cstyle_list_t));
	  }
	  cstylel->cstyle = cstyle;
	  cstylel->Next = NULL;
	  if (style)
	  {
	    cstylel = style_xf_list;
	    if (cstylel)
	    {
	      while(cstylel->Next)
		cstylel = cstylel->Next;
	      cstylel->Next = 
		(cstyle_list_t *)Xc_malloc("xf", sizeof(cstyle_list_t));
	      cstylel = cstylel->Next;
	    }
	    else
	    {
	      style_xf_list = cstylel = 
		(cstyle_list_t *)Xc_malloc("xf", sizeof(cstyle_list_t));
	    }
	    cstylel->cstyle = cstyle;
	    cstylel->Next = NULL;
	  }
	  break;
	}
      }
      break;
    case XcXL_NAME:
      {
	int	name_lng, expr_lng;
	char	*name = NULL;
	name_list_t	*namel;
	udata += 3;
	name_lng = *udata; udata++;
	expr_lng = read_short(udata); udata += 2;
	if (expr_lng) 
	{
	  udata += name_lng;
	  name = (char *)Xc_malloc("name", expr_lng + 1);
	  memcpy(name, udata, expr_lng);
	  name[expr_lng] = '\0';
	}
	else
	{
	  if (name_lng)
	  {
	    name = (char *)Xc_malloc("name", name_lng + 1);
	    memcpy(name, udata, name_lng);
	    name[name_lng] = '\0';
	  }
	}
	if (name)
	{
	  namel = name_list;
	  if (namel)
	  {
	    while(namel->Next)
	      namel = namel->Next;
	    namel->Next = 
	      (name_list_t *)Xc_malloc("xf", sizeof(name_list_t));
	    namel = namel->Next;
	  }
	  else
	  {
	    name_list = namel = 
	      (name_list_t *)Xc_malloc("xf", sizeof(name_list_t));
	  }
	  namel->name = name;
	  namel->Next = NULL;
	}
      }
      break;
      
    case XcXL_COLWIDTH:
      region.x_min = *(udata); udata++;
      region.x_max = *(udata); udata++;
      swidth = (read_short(udata) * SCALE_FROM_POINTS(6.0)) / 256;
      F(moteur).set_size_zone(moteur, Xc_COLUMN, &region, swidth);
      break;
    case XcXL_COLUMNDEFAULT:
      region.y_min = 0; region.y_max = ROW_MAX;
      region.x_min = read_short(data); data += 2;
      region.x_max = read_short(data); data += 2;
      xf = read_cell_attributes(&data, cell_xf_list, tstyle_list, 
				format_list, cellstylebase);
      if (xf)
	F(moteur).set_cell_style2(moteur, xf, &region);
      break;
    case XcXL_COLINFO:
      region.y_min = 0; region.y_max = ROW_MAX;
      region.x_min = read_short(data); data += 2;
      region.x_max = read_short(data); data += 2;
      swidth = (read_short(data) * SCALE_FROM_POINTS(6.0)) / 256; data += 2;
      i = read_short(data); data += 2;
      xf = get_cstyle_in_list(cell_xf_list, i);
      if (xf)
	F(moteur).set_cell_style2(moteur, xf, &region);
      F(moteur).set_size_zone(moteur, Xc_COLUMN, &region, swidth);
      break;
    case XcXL_ROW:
      row = read_short(data); data += 2;
      min_col = read_short(data); data += 2;
      max_col = read_short(data); data += 2;
      sheight = SCALE_FROM_POINTS( (read_short(data) & 0x7fff) / 20.0);
      region.y_min = region.y_max = row;
      F(moteur).set_size_zone(moteur, Xc_ROW, &region, sheight);
      break;
    case XcXL_DEFAULTROWHEIGHT:
      region.y_min = 0;
      region.y_max = ROW_MAX;
      
      if (rec_version == REC_BIFF34)
	udata += 2;
      sheight = SCALE_FROM_POINTS( (read_short(udata) & 0x7ff) / 20.0);
      F(moteur).set_size_zone(moteur, Xc_ROW, &region, sheight);
      break;
    case XcXL_DEFCOLWIDTH:
      region.x_min = 0;
      region.x_max = COLUMN_MAX;
      
      swidth = SCALE_FROM_POINTS( read_short(udata) * 6.0);
      F(moteur).set_size_zone(moteur, Xc_COLUMN, &region, swidth);
      break;
    case XcXL_STNDARDWIDTH:
      region.x_min = 0;
      region.x_max = COLUMN_MAX;
      
      swidth = (read_short(data) * SCALE_FROM_POINTS(6.0)) / 256;
      F(moteur).set_size_zone(moteur, Xc_COLUMN, &region, swidth);
      break;
      
    case XcXL_WINDOW1:
      sheet->PosX = read_short(data) / 20; data += 2;
      sheet->PosY = read_short(data) / 20; data += 2;
      sheet->Width = read_short(data) / 20; data += 2;
      sheet->Height = read_short(data) / 20;
      break;
    case XcXL_WINDOW2:
      if (rec_version == REC_BIFF2)
      {
	if (*data)
	  cellstylebase->display_formula = TRUE;
	else
	  cellstylebase->display_formula = FALSE;
	data++;
	if (*data)
	  feuille->grid = TRUE;
	else
	  feuille->grid = FALSE;
	data++;
	if (*data)
	  feuille->Column_Origin_actif = feuille->Row_Origin_actif = TRUE;
	else
	  feuille->Column_Origin_actif = feuille->Row_Origin_actif = FALSE;
	data += 2;
	if (*data)
	  cellstylebase->display_zero_cell = TRUE;
	else
	  cellstylebase->display_zero_cell = FALSE;
	data++;
	row = read_short(data); data += 2;
	col = read_short(data); data += 2;
	feuille->row_orig = row;
	feuille->column_orig = col;
      }
      else
      {
	i = read_short(data); data += 2;
	cellstylebase->display_formula = TEST_BIT(i, 0);
	feuille->grid = TEST_BIT(i, 1);
	feuille->Column_Origin_actif = 
	  feuille->Row_Origin_actif = TEST_BIT(i, 2);
	cellstylebase->display_zero_cell = TEST_BIT(i, 4);
	
	row = read_short(data); data += 2;
	col = read_short(data); data += 2;
	feuille->row_orig = row;
	feuille->column_orig = col;
      }
      break;
    case XcXL_SELECTION:
      if (*udata == 3) /* top left pane */
      {
	udata++;
	feuille->cell_y = read_short(udata); udata += 2;
	feuille->cell_x = read_short(udata); udata += 4;
	j = read_short(udata); udata += 2;
	for(i = 0; i < j; i++)
	{
	  min_row = read_short(udata); udata += 2;
	  max_row = read_short(udata); udata += 2;
	  min_col = *(udata); udata ++;
	  max_col = *(udata); udata ++;
	  F(feuille).SelectRegion(feuille, min_col, min_row, max_col, max_row,
				  Xq_CELL, FALSE);
	}
      }
      break;
    case XcXL_SCL:
      i = read_short(data); data += 2;
      j = read_short(data);
      feuille->sFactor = SCALE_FROM_COEF((double)i / (double)j);
      feuille->sScale = (COORD_DEFAULT * i) / j;
      break;
      
    case XcXL_PALETTE:
      {
	int ncolor;
	unsigned int r, v, b;
	
	ncolor = read_short(udata); udata += 2;
	if (ncolor != 0)
	{
	  for(i=0; i<nb_color; i++)
	    DELETE(c_Color)(palette[i]);
	  Xc_free(palette);
	  
	  nb_color = ncolor;
	  palette = (c_Color **)Xc_malloc("palette",
					ncolor * sizeof(c_Color *));
	  for(i = 0; i < nb_color; i++)
	  {
	    r = XcC_SCALE_COLOR((*udata)/255.0); data ++; 
	    v = XcC_SCALE_COLOR((*udata)/255.0); data ++; 
	    b = XcC_SCALE_COLOR((*udata)/255.0); data ++; 
	    palette[i] = 
	      F(colorbase).getColor(&colorbase, XcC_RGB, r, v, b, 
				    XcC_HIDE_OFF, XcC_END);
	  }
	}
      }
      break;
    
    case XcXL_1904:
      i = read_short(udata);
      date_1904 = (i == 1);
      break;
      
    case XcXL_BLANK:
      row = read_short(data); data += 2;
      col = read_short(data); data += 2;
      region.x_min = region.x_max = col;
      region.y_min = region.y_max = row;
      if (rec_version == REC_BIFF2)
	{
	  xf = read_cell_attributes(&data, cell_xf_list, tstyle_list, 
				    format_list, cellstylebase);
	}
      else
	{
	  i = read_short(data); data += 2;
	  xf = get_cstyle_in_list(cell_xf_list, i);
	}
      if (xf)
	F(moteur).set_cell_style2(moteur, xf, &region);
      break;
    case XcXL_INTEGER:
      {
	char int_string[10];
	row = read_short(data); data += 2;
	col = read_short(data); data += 2;
	region.x_min = region.x_max = col;
	region.y_min = region.y_max = row;
	xf = read_cell_attributes(&data, cell_xf_list, tstyle_list, 
				  format_list, cellstylebase);
	if (xf)
	  F(moteur).set_cell_style2(moteur, xf, &region);
	i = read_short(data);
	special_value_format(xf, &i, XcXLF_INTEGER, date_1904);
	sprintf(int_string, "%d", i);
	cellule = F(moteur).add_cell(moteur, row, col);
	F(moteur).set_string(moteur, col, row, cellule, int_string, NULL);
      }
      break;
    case XcXL_NUMBER:
      {
	char string[30];
	double value;
	
	row = read_short(data); data += 2;
	col = read_short(data); data += 2;
	region.x_min = region.x_max = col;
	region.y_min = region.y_max = row;
	if (rec_version == REC_BIFF2)
	{
	  xf = read_cell_attributes(&data, cell_xf_list, tstyle_list, 
				    format_list, cellstylebase);
	}
	else
	{
	  i = read_short(data); data += 2;
	  xf = get_cstyle_in_list(cell_xf_list, i);
	}
	if (xf)
	  F(moteur).set_cell_style2(moteur, xf, &region);
	value = read_8byte_IEEE(data);
	special_value_format(xf, &value, XcXLF_REAL, date_1904);
	sprintf(string, "%f", value);
	
	cellule = F(moteur).add_cell(moteur, row, col);
	F(moteur).set_string(moteur, col, row, cellule, string, NULL);
      }
      break;
    case XcXL_BOOLERR:
      {
	char *string;
	
	row = read_short(data); data += 2;
	col = read_short(data); data += 2;
	region.x_min = region.x_max = col;
	region.y_min = region.y_max = row;
	if (rec_version == REC_BIFF2)
	{
	  xf = read_cell_attributes(&data, cell_xf_list, tstyle_list, 
				    format_list, cellstylebase);
	}
	else
	{
	  i = read_short(data); data += 2;
	  xf = get_cstyle_in_list(cell_xf_list, i);
	}
	if (xf)
	  F(moteur).set_cell_style2(moteur, xf, &region);
	
	i = *data; data++;
	j = *data;
	
	if (j == 0)
	  {
	    if (i == 0)
	      string = F(GlobResources).getString(GlobResources, XcR_false);
	    else
	      string = F(GlobResources).getString(GlobResources, XcR_true);
	  }
	else
	  {
	    j = get_erreur_res( i & 0xff);
	    string = F(GlobResources).getString(GlobResources, j);
	  }
	cellule = F(moteur).add_cell(moteur, row, col);
	F(moteur).set_string(moteur, col, row, cellule, string, NULL);
      }
      break;
    case XcXL_RK:
      {
	char string[30];
	double value;

	row = read_short(data); data += 2;
	col = read_short(data); data += 2;
	region.x_min = region.x_max = col;
	region.y_min = region.y_max = row;
	i = read_short(data); data += 2;
	xf = get_cstyle_in_list(cell_xf_list, i);
	if (xf)
	  F(moteur).set_cell_style2(moteur, xf, &region);
	
	i = *(data);
	switch(i & 3)
	  {
	  case 0:
	    j = read_integer(data);
	    j >>= 2;
	    value = transform_int_to_30bits_IEEE(j);
	    special_value_format(xf, &value, XcXLF_REAL, date_1904);
	    sprintf(string, "%f", value);
	    
	    cellule = F(moteur).add_cell(moteur, row, col);
	    F(moteur).set_string(moteur, col, row, cellule, string, NULL);
	    break;
	  case 1:
	    j = read_integer(data);
	    j >>= 2;
	    value = transform_int_to_30bits_IEEE(j) * 100;
	    special_value_format(xf, &value, XcXLF_REAL, date_1904);
	    sprintf(string, "%f", value);
	    
	    cellule = F(moteur).add_cell(moteur, row, col);
	    F(moteur).set_string(moteur, col, row, cellule, string, NULL);
	    break;
	  case 2:
	    j = read_integer(data);
	    j >>= 2;
	    special_value_format(xf, &j, XcXLF_INTEGER, date_1904);
	    sprintf(string, "%d", j);

	    cellule = F(moteur).add_cell(moteur, row, col);
	    F(moteur).set_string(moteur, col, row, cellule, string, NULL);
	    break;
	  case 3:
	    j = read_integer(data);
	    j = (j >> 2) * 100;
	    special_value_format(xf, &j, XcXLF_INTEGER, date_1904);
	    sprintf(string, "%d", j);
	    
	    cellule = F(moteur).add_cell(moteur, row, col);
	    F(moteur).set_string(moteur, col, row, cellule, string, NULL);
	    break;
	  }
      }
      break;
    case XcXL_LABEL:
      row = read_short(data); data += 2;
      col = read_short(data); data += 2;
      region.x_min = region.x_max = col;
      region.y_min = region.y_max = row;
      if (rec_version == REC_BIFF2)
	{
	  char *label;

	  xf = read_cell_attributes(&data, cell_xf_list, tstyle_list, 
				    format_list, cellstylebase);
	  if (xf)
	    F(moteur).set_cell_style2(moteur, xf, &region);
	  label = read_short_string(&data, encoding);
	  if (isdigit(*label) || *label == '=') 
	  {
	    memmove(label + 1, label, strlen(label) + 1);
	    *label = '\'';
	  }
	  cellule = F(moteur).add_cell(moteur, row, col);
	  F(moteur).set_string(moteur, col, row, cellule, label, NULL);
	}
      else
	{
	  char *label;
	
	  i = read_short(data); data += 2;
	  xf = get_cstyle_in_list(cell_xf_list, i);
	  if (xf)
	    F(moteur).set_cell_style2(moteur, xf, &region);
	  label = read_long_string(&data, encoding);
	  if (isdigit(*label) || *label == '=') 
	  {
	    memmove(label + 1, label, strlen(label) + 1);
	    *label = '\'';
	  }
	  cellule = F(moteur).add_cell(moteur, row, col);
	  F(moteur).set_string(moteur, col, row, cellule, label, NULL);
	  Xc_free(label);
	}
      break;
    case XcXL_FORMULA:
      {
	char *formula, *xl_formula;
	char ret_value[30];
	boolean formula_valid = FALSE;
	int formula_length;
	
	row = read_short(data); data += 2;
	col = read_short(data); data += 2;
	region.x_min = region.x_max = col;
	region.y_min = region.y_max = row;
	if (rec_version == REC_BIFF2)
	{
	  xf = read_cell_attributes(&data, cell_xf_list, tstyle_list, 
				    format_list, cellstylebase);
	  if (xf)
	    F(moteur).set_cell_style2(moteur, xf, &region);
	}
	else
	{
	  i = read_short(data); data += 2;
	  xf = get_cstyle_in_list(cell_xf_list, i);
	  if (xf)
	    F(moteur).set_cell_style2(moteur, xf, &region);
	}
	if (data[6] == (char)0xff && data[7] == (char)0xff)
	{
	  switch(*data)
	  {
	  case 0:
	    ret_value[0] = '\0';
	    string_set.accept = TRUE;
	    string_set.col = col;
	    string_set.row = row;
	    data += 8;
	    break;
	  case 1:
	    data += 2;
	    sprintf(ret_value, "%s", (*data) ? 
		    F(GlobResources).getString(GlobResources, XcR_true):
		    F(GlobResources).getString(GlobResources, XcR_false));
	    data += 6;
	    break;
	  case 2:
	    data += 2;
	    j = get_erreur_res(*data);
	    strcpy(ret_value, F(GlobResources).getString(GlobResources, j));
	    data += 6;
	    break;
	  }
	}
	else
	{
	  sprintf(ret_value, "%f", read_8byte_IEEE(data));
	  data += 8;
	}
	
	if (rec_version == REC_BIFF2)
	{
	  data ++;  /* skip the recalculation flag */
	  
	  formula_length = *data; data++;
	}
	else
	{
	  data += 2;  /* skip the recalculation flag */
	  formula_length = read_short(data); data += 2;
	}
	
	xl_formula = (char *)Xc_malloc("xl formula", formula_length);
	memcpy(xl_formula, data, formula_length);
	Xc_TRACE(("read formula"));
	formula = read_formula(xl_formula, formula_length, 
			       rec_version, name_list);
	Xc_TRACE(("formula readed"));
	Xc_free(xl_formula);
	
	if (formula)
	{
	  Xc_TRACE(("fill cell: %c%d with: %s", col+'A', row+1, formula));
	  cellule = F(moteur).add_cell(moteur, row, col);
	  F(moteur).set_string(moteur, col, row, cellule, formula, NULL);
	  if (cellule->status == TYPE_NAME_ERROR)
	    F(moteur).del_cell(moteur, row, col);
	  else
	    formula_valid = TRUE;
	  Xc_free(formula);
	}
	
	if (!formula_valid)
	{
	  if (ret_value[0] != '\0')
	  {
	    cellule = F(moteur).add_cell(moteur, row, col);
	    F(moteur).set_string(moteur, col, row, cellule, ret_value, NULL);
	  }
	}
	else
	  string_set.accept = FALSE;
      }
      break;
    case XcXL_STRING:
      if (string_set.accept)
      {
	row = string_set.row;
	col = string_set.col;
	if (rec_version == REC_BIFF2)
	{
	  char *label;
	  
	  label = read_short_string(&data, encoding);
	  if (isdigit(*label) || *label == '=') 
	  {
	    memmove(label + 1, label, strlen(label) + 1);
	    *label = '\'';
	  }
	  cellule = F(moteur).add_cell(moteur, row, col);
	  F(moteur).set_string(moteur, col, row, cellule, label, NULL);
	}
	else
	{
	  char *label;
	
	  label = read_long_string(&data, encoding);
	  if (isdigit(*label) || *label == '=') 
	  {
	    memmove(label + 1, label, strlen(label) + 1);
	    *label = '\'';
	  }
	  cellule = F(moteur).add_cell(moteur, row, col);
	  F(moteur).set_string(moteur, col, row, cellule, label, NULL);
	  Xc_free(label);
	}
	string_set.accept = FALSE;
      }
      break;
      
    case XcXL_FILEPASS:
      Xc_ERROR(("File protected with a password !"));
      exit = TRUE;
      erreur = XC_FILE_FORMAT_ERROR;
      break;
      
    case XcXL_UNKNOW:
      Xc_WARNING(("Unknown record. Maybe Excel version > 4."));
      break;
      
    case XcXL_FORMAT_ERROR:	
      Xc_WARNING(("Format error while reading file: %s", This->filename));
      exit = TRUE;
      erreur = XC_FILE_FORMAT_ERROR;
      break;
      
    case XcXL_FILE_TRUNCATED:	
      Xc_WARNING(("File %s truncated", This->filename));
      exit = TRUE;
      erreur = XC_FILE_TRUNCATED;
      break;
      
    default:
      Xc_TRACE(("Record type %d not managed yet (%x)", 
		rec_type, excel_record[rec_type].rec_excel2));
      break;
    }
  }
  
  if (palette)
    Xc_free(palette);
  
  if (tstyle_list)
  {
    tstyle_list_t	*tstylel;
    while(tstyle_list)
    {
      tstylel = tstyle_list->Next;
      Xc_free(tstyle_list);
      tstyle_list = tstylel;
    }
  }
  
  if (format_list)
  {
    format_list_t *formatl;
    while(format_list)
    {
      formatl = format_list->Next;
      Xc_free(format_list);
      format_list = formatl;
    }
  }
  
  if (cell_xf_list)
  {
    cstyle_list_t *cstylel;
    while(cell_xf_list)
    {
      cstylel = cell_xf_list->Next;
      Xc_free(cell_xf_list);
      cell_xf_list = cstylel;
    }
  }
  
  if (style_xf_list)
  {
    cstyle_list_t *cstylel;
    while(style_xf_list)
    {
      cstylel = style_xf_list->Next;
      Xc_free(style_xf_list);
      style_xf_list = cstylel;
    }
  }
  
  if (name_list)
  {
    name_list_t *namel;
    while(name_list)
    {
      namel = name_list->Next;
      Xc_free(name_list->name);
      Xc_free(name_list);
      name_list = namel;
    }
  }
  
  Xc_free(rec_data);
  fclose(file);
  
  if (erreur)
    return XC_FILE_FORMAT_ERROR;
  return XC_NO_ERROR;
}


static unsigned short read_record(This, file, rec_data, data_length, 
				  length, version)
c_Import_Excel	*This;
FILE		*file;
char		**rec_data;
int		*data_length;
int		*length;
int		*version;
{
  unsigned short rec_type, next_rec_type, rec_length, next_rec_length;
  int	i, rec_version;
  
  if ( fread((char *)&rec_type, 2, 1, file) != 1)
    return XcXL_FILE_TRUNCATED;
  if (Xc_BYTE_ORDER != Xc_LITTLE_ENDIAN)
    rec_type = ((rec_type & 0xff) << 8) + ((rec_type & 0xff00) >> 8);
  
  if ( fread((char *)&rec_length, 2, 1, file) != 1)
    return XcXL_FILE_TRUNCATED;
  if (Xc_BYTE_ORDER != Xc_LITTLE_ENDIAN)
    rec_length = ((rec_length & 0xff) << 8) + ((rec_length & 0xff00) >> 8);
  if (rec_length > 2080)
    return XcXL_FORMAT_ERROR;
  
  i = 0; rec_version = 0;
  while(i < NB_EXCEL_RECORD)
  {
    if (excel_record[i].rec_excel2 == rec_type)
      rec_version = REC_BIFF2;
    if (excel_record[i].rec_excel3 == rec_type)
      rec_version |= REC_BIFF3;
    if (excel_record[i].rec_excel4 == rec_type)
      rec_version |= REC_BIFF4;
    if (rec_version)
      break;
    i++;
  }
  if (rec_version == 0)
  {
    Xc_TRACE(("Unknown record type %x in excel file", rec_type));
    if (rec_type >= 0x500)
      return XcXL_FORMAT_ERROR;
    rec_type = XcXL_UNKNOW;
  }
  
  if (*data_length < rec_length)
  {
    *rec_data = (char *)Xc_realloc("rec data", *rec_data, rec_length);
    *data_length = rec_length;
  }
  if ( rec_length)
    if ( fread(*rec_data, rec_length, 1, file) != 1)
      return XcXL_FILE_TRUNCATED;
  
  *version = rec_version;
  
  if (rec_length == 2080)
  {
    do 
    {
      if ( fread((char *)&next_rec_type, 2, 1, file) != 1)
	return XcXL_FILE_TRUNCATED;
      if (Xc_BYTE_ORDER != Xc_LITTLE_ENDIAN)
	next_rec_type = ((next_rec_type & 0xff) << 8) + 
	  ((next_rec_type & 0xff00) >> 8);
      if (next_rec_type != excel_record[XcXL_CONTINUE].rec_excel2)
	break;
      if ( fread((char *)&next_rec_length, 2, 1, file) != 1)
	return XcXL_FILE_TRUNCATED;
      if (Xc_BYTE_ORDER != Xc_LITTLE_ENDIAN)
	next_rec_length = ((next_rec_length & 0xff) << 8) +
	  ((next_rec_length & 0xff00) >> 8);
      if (next_rec_length > 2080)
	return XcXL_FORMAT_ERROR; 
      if (*data_length < rec_length + next_rec_length)
      {
	*rec_data = (char *)Xc_realloc("rec data", *rec_data, 
				       rec_length + next_rec_length);
	*data_length = rec_length + next_rec_length;
      }
      if ( fread(*rec_data + rec_length, next_rec_length, 1, file) != 1)
	return XcXL_FILE_TRUNCATED;
      rec_length += next_rec_length;
    }
    while(next_rec_type == excel_record[XcXL_CONTINUE].rec_excel2);
    fseek(file, -2L, SEEK_CUR);
  }
  *length = (int)rec_length;
  return i;
}

static unsigned short read_short(data)
unsigned char	*data;
{
  if (Xc_BYTE_ORDER != Xc_LITTLE_ENDIAN)
    return (data[1]<<8) + data[0];
  return *((unsigned short *)data);
}

static int read_integer(data)
char *data;
{
  if (Xc_BYTE_ORDER != Xc_LITTLE_ENDIAN)
    return (data[3] << 24) + (data[2] << 16) + (data[1]<<8) + data[0];
  return *((int *)data);
}

static char *read_short_string(data, encoding)
char **data;
encoding_t encoding;
{
  int i;
  
  i = *((unsigned char *)*data); 
  memcpy(short_string, (*data) + 1, i);
  short_string[i] = '\0';
  convert_encoding(short_string, encoding);
  
  *data = *data + i + 1;
  return short_string;
}

static char *read_long_string(data, encoding)
char **data;
encoding_t encoding;
{
  char	*result;
  int i;
  
  i = read_short(*data); 
  if (i == 0 || i > 100000)
    return Xc_strdup("none", "");
  result = (char *)Xc_malloc("lng str", i+2);
  memcpy(result, (*data) + 2, i);
  result[i] = '\0';
  convert_encoding(result, encoding);
  
  *data = *data + i + 2;
  return result;
}

static double read_8byte_IEEE(data)
unsigned char *data;
{
  boolean	signe;
  int		exposant;
  unsigned long	mantise1, mantise2;
  double   decimale, result;
  unsigned short hword;
  
  hword = (data[7] << 8) + data[6];
  signe = TEST_BIT(hword, 15);
  exposant = ((hword >> 4) & 0x7ff) - 1023;
  
  mantise1 = ((hword & 0xf) << 16) + (data[5] << 8) + data[4];
  mantise2 = (data[3] << 24) + (data[2] << 16) + (data[1] << 8) + data[0];
  decimale = 1.0+ ((ldexp((double)mantise1, 32) + mantise2) / pow(2.0, 52.0));
  if (signe)
    decimale = -decimale;
  result = ldexp(decimale, exposant);
  return result;
}

static double transform_int_to_30bits_IEEE(value)
int	value;
{
  boolean	signe;
  int		exposant;
  unsigned long	mantise;
  double   decimale, result;
  
  signe = TEST_BIT(value, 29);
  exposant = ((value >> 18) & 0x7ff) - 1023;
  
  mantise = (value & 0x3FFFF);
  decimale = 1.0 + (mantise / pow(2.0, 18.0));
  if (signe)
    decimale = -decimale;
  result = ldexp(decimale, exposant);
  return result;
}

static void special_value_format(cstyle, value, type, date_1904)
CellStyle_t	*cstyle;     
void		*value;
boolean		type;
boolean		date_1904;
{
  int	 date_diff;
  int	 *value_int;
  double *value_real;
  
  switch(cstyle->format->type)
  {
  case Xq_DATE:
  case Xq_DATE_TIME:
    if (date_1904)
      date_diff = 35066;
    else
      date_diff = 36526;
    
    if (type == XcXLF_INTEGER)
    {
      value_int = (int *)value;
      if (*value_int <= 60)
	*value_int = *value_int + 1;
      *value_int = *value_int - date_diff;
    }
    else
    {
      value_real = (double *)value;
      if (*value_real <= 60.0)
	*value_real = *value_real + 1.0;
      *value_real = *value_real - (double)date_diff;
    }
    break;
  case Xq_PERCENT:
    if (type == XcXLF_INTEGER)
    {
      value_int = (int *)value;
      *value_int = *value_int / 100;
    }
    else
    {
      value_real = (double *)value;
      *value_real = *value_real / 100.0;
    }
    break;
  }
}

static c_Color *get_color_in_palette(palette, pal_size, num)
c_Color	**palette;
int pal_size;
int num;
{
  if (num >= pal_size)
    return palette[0];
  else
    return palette[num];
}
     
static c_TextStyle *get_tstyle_in_list(tstyle_list, num)
tstyle_list_t *tstyle_list;     
int	num;
{
  int i = 1;
  tstyle_list_t *tstylel;
  
  tstylel = tstyle_list;
  while(tstylel && i < num)
  {
    i++;
    tstylel = tstylel->Next;
  }
  if (!tstylel)
    return NULL;
  return tstylel->tstyle;
}

static c_FormatNbr *get_format_in_list(format_list, num)
format_list_t	*format_list;
int	num;
{
  int i = 0;
  format_list_t	*formatl;
  
  formatl = format_list;
  while(formatl && i < num)
  {
    i++;
    formatl = formatl->Next;
  }
  if (!formatl)
    return NULL;
  return formatl->format;
}

static CellStyle_t *get_cstyle_in_list(cstyle_list, num)
cstyle_list_t	*cstyle_list;     
int	num;
{
  int i = 0;
  cstyle_list_t	*cstylel;
  
  cstylel = cstyle_list;
  while(cstylel && i < num)
    {
      i++;
      cstylel = cstylel->Next;
    }
  if (!cstylel)
    return NULL;
    
  return cstylel->cstyle;
}

static char *get_name_in_list(name_list, num)
name_list_t	*name_list;     
int	num;
{
  int i = 1;
  name_list_t	*namel;
  
  namel = name_list;
  while(namel && i < num)
    {
      i++;
      namel = namel->Next;
    }
  if (!namel)
    return NULL;
  return namel->name;
}

static CellStyle_t *read_cell_attributes(data, cstyle_list, tstyle_list, 
					format_list, cellstylebase)
unsigned char	**data;
cstyle_list_t	*cstyle_list;
tstyle_list_t	*tstyle_list;
format_list_t	*format_list;
c_CellStyle	*cellstylebase;
{
  CellStyle_t *xf, *cstyle;
  c_TextStyle *tstyle;
  c_FormatNbr *format;
  ruler_t     ruler;
  int i;
  
  i = (**data & 0x3F);
  xf = get_cstyle_in_list(cstyle_list, i);
  if (!xf) 
    xf = CELL_STYLE_DEFAULT;
  i = *(*data + 1);
  if ((i >> 6) & 3)
    tstyle = get_tstyle_in_list(tstyle_list, (i >> 6) & 3);
  else
    tstyle = NULL;
  if (i & 0x3f)
    format = get_format_in_list(format_list, (i & 0x3f));
  else
    format = NULL;
  
  i = *(*data + 2);
  
  switch(i & 7)
  {
  case 1:
    ruler = XqR_H_ALIGN_LEFT;
    break;
  case 2:
  case 6:
    ruler = XqR_H_ALIGN_CENTER;
    break;
  case 3:
    ruler = XqR_H_ALIGN_RIGHT;
    break;
  case 4:
  case 5:
    ruler = XqR_H_JUSTIFIED;
    break;
  case 0:
  default:
    ruler = XqR_DEFAULT;
    break;
  }
  ruler |= XqR_V_ALIGN_BOTTOM;
  
  cstyle = F(cellstylebase).CreateNewCellStyle(cellstylebase, xf);
  if (tstyle)
  {
    if (format)
      SET_CELL_STYLE(cstyle, XqCS_TEXTSTYLE, tstyle, XqCS_FORMAT, format,
		     XqCS_ALIGNEMENT, ruler, XqCS_END);
    else
      SET_CELL_STYLE(cstyle, XqCS_TEXTSTYLE, tstyle, 
		     XqCS_ALIGNEMENT, ruler, XqCS_END);
    
  }
  else
  {
    if (format)
      SET_CELL_STYLE(cstyle, XqCS_FORMAT, format,
		     XqCS_ALIGNEMENT, ruler, XqCS_END);
    else
      SET_CELL_STYLE(cstyle, XqCS_ALIGNEMENT, ruler, XqCS_END);
  }
  
  if (TEST_BIT(i, 3)) 
    SET_CELL_STYLE(cstyle, XqCS_LEFT_PATTERN, CSP_SINGLE_LINE, XqCS_END);
  
  if (TEST_BIT(i, 4))
    SET_CELL_STYLE(cstyle, XqCS_RIGHT_PATTERN, CSP_SINGLE_LINE, XqCS_END);
  
  if (TEST_BIT(i, 5))
    SET_CELL_STYLE(cstyle, XqCS_TOP_PATTERN, CSP_SINGLE_LINE, XqCS_END);
  
  if (TEST_BIT(i, 6))
    SET_CELL_STYLE(cstyle, XqCS_BOTTOM_PATTERN, CSP_SINGLE_LINE, XqCS_END);
  
  if (TEST_BIT(i, 7))	/* own implementation of shadow */
    SET_CELL_STYLE (cstyle, XqCS_PATTERN, CELL_PATTERN_DATA_FIRST+2, XqCS_END);
  
  cstyle = F(cellstylebase).AddCellStyleToBase(cellstylebase,
					       cstyle);
  
  *data = *data + 3;
  return cstyle;
}

static int get_erreur_res(code)
char code;     
{
  int res;
  
  switch( code & 0xff)
  {
  case 0x00:
    res = XcR_formulaCharError;
    break;
  case 0x07:
    res = XcR_formulaDivzError;
    break;
  case 0x0F:
    res = XcR_formulaCalcError;
    break;
  case 0x17:
    res = XcR_formulaCircError;
    break;
  case 0x1D:
    res = XcR_formulaNameError;
    break;
  case 0x24:
    res = XcR_formulaValError;
    break;
  case 0x2A:
  default:
    res = XcR_formulaError;
    break;
  }
  return res;
}

#define NEW_FORMULA formula = Xc_malloc("formula", Xq_FORMULA_LENGTH)

static char *read_formula(xl_formula, length, version, name_list)
unsigned char *xl_formula;
int length;
int version;
name_list_t *name_list;
{
  char	*formula, column[6], column2[6], *name, *function;
  char	*form_stack[Xq_FORMULA_LENGTH], form2[Xq_FORMULA_LENGTH];
  int	stack_top = 0;
  int	form_ptr, i;
  boolean erreur = FALSE;
  boolean col_rel, row_rel, col2_rel, row2_rel;
  int col, row, col2, row2, nb_args;
  char	token;
  unsigned short funct;
  double dbl;
  
  form_ptr = 0;
  while(form_ptr < length && !erreur)
  {
    token = xl_formula[form_ptr];
    if (token >= 0x40)
      token -= 0x20;
    form_ptr ++;
    switch(token)
    {
    case 0x16:
      break;
    case 0x17:
      NEW_FORMULA;
      i = (unsigned int)xl_formula[form_ptr++];
      memcpy(form2, xl_formula + form_ptr, i);
      form2[i] = '\0';
      sprintf(formula, "\"%s\"", form2);
      form_ptr += i;
      form_stack[stack_top] = formula;
      stack_top ++;
      break;
    case 0x1c:
      NEW_FORMULA;
      i = get_erreur_res(xl_formula[form_ptr++]);
      strcpy(formula, F(GlobResources).getString(GlobResources, i));
      form_stack[stack_top] = formula;
      stack_top ++;
      break;
    case 0x1d:
      NEW_FORMULA;
      if (xl_formula[form_ptr++] != 0)
	strcpy(formula, F(GlobResources).getString(GlobResources, XcR_true));
      else
	strcpy(formula, F(GlobResources).getString(GlobResources, XcR_false));
      form_stack[stack_top] = formula;
      stack_top ++;
      break;
    case 0x1e:
      NEW_FORMULA;
      i = read_short(xl_formula + form_ptr); form_ptr += 2;
      sprintf(formula, "%ld", (unsigned long)i);
      form_stack[stack_top] = formula;
      stack_top ++;
      break;
    case 0x1f:
      NEW_FORMULA;
      dbl = read_8byte_IEEE(xl_formula + form_ptr); form_ptr += 8;
      sprintf(formula, "%f", dbl);
      form_stack[stack_top] = formula;
      stack_top ++;
      break;
    case 0x24:
    case 0x2c:
      NEW_FORMULA;
      i = read_short(xl_formula + form_ptr); form_ptr += 2;
      row = i & 0x3fff;
      col = xl_formula[form_ptr++];
      row_rel = TEST_BIT(i, 15);
      col_rel = TEST_BIT(i, 14);
      coltostr(col, NULL, column);
      sprintf(formula, "%s%s%s%d", col_rel?"":"$", column, 
	      row_rel?"":"$", row + 1);
      form_stack[stack_top] = formula;
      stack_top ++;
      break;
    case 0x2d:
    case 0x25:
      NEW_FORMULA;
      i = read_short(xl_formula + form_ptr); form_ptr += 2;
      row = i & 0x3fff;
      row_rel = TEST_BIT(i, 15);
      col_rel = TEST_BIT(i, 14);
      i = read_short(xl_formula + form_ptr); form_ptr += 2;
      row2 = i & 0x3fff;
      row2_rel = TEST_BIT(i, 15);
      col2_rel = TEST_BIT(i, 14);
      col = xl_formula[form_ptr++];
      coltostr(col, NULL, column);
      col2 = xl_formula[form_ptr++];
      coltostr(col2, NULL, column2);
      if (col == 0 && col2 == 255)
      {
	sprintf(formula, "%s%d:%s%d", row_rel?"":"$", row + 1, 
		row2_rel?"":"$", row2 + 1);
      }
      else
      {
	if (row == 0 && row2 == 16383)
	{
	  sprintf(formula, "%s%s%s%s", col_rel?"":"$", column, 
		  col2_rel?"":"$", column2);
	}
	else
	{
	  sprintf(formula, "%s%s%s%d:%s%s%s%d", col_rel?"":"$", column, 
		  row_rel?"":"$", row + 1, col2_rel?"":"$", column2, 
		  row2_rel?"":"$", row2 + 1);
	}
      }
      form_stack[stack_top] = formula;
      stack_top ++;
      break;
    case 0x23:
      i = read_short(xl_formula + form_ptr); form_ptr += 2;
      if (version == REC_BIFF2)
	form_ptr += 5;
      else
	form_ptr += 8;
      name = get_name_in_list(name_list, i);
      if (name == NULL) { erreur = TRUE; break; }
      NEW_FORMULA;
      strcpy(formula, name);
      form_stack[stack_top] = formula;
      stack_top ++;
      break;
    case 0x27:
      if (version == REC_BIFF2)
	form_ptr += 4;
      else
	form_ptr += 6;
      break;
    case 0x2a:
      form_ptr += 3;
      break;
    case 0x2b:
      form_ptr += 6;
      break;
      
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0a:
    case 0x0b:
    case 0x0c:    
    case 0x0d:
    case 0x0e:
    case 0x0f:
    case 0x10:
      if (stack_top < 2 || 
	  (strlen(form_stack[stack_top - 2]) + 
	   strlen(form_stack[stack_top - 1]) + 2 >= Xq_FORMULA_LENGTH))
      { erreur = TRUE; break; }
      NEW_FORMULA;
      sprintf(formula, "%s%s%s", form_stack[stack_top - 2], 
	      xl_operator[token - 3], form_stack[stack_top - 1]);
      Xc_free(form_stack[--stack_top]);
      Xc_free(form_stack[stack_top - 1]);
      form_stack[stack_top - 1] = formula;
      break;
    case 0x12:
    case 0x13:
      if (stack_top < 1 || 
	  (strlen(form_stack[stack_top - 1]) + 2 >= Xq_FORMULA_LENGTH))
      { erreur = TRUE; break; }
      NEW_FORMULA;
      sprintf(formula, "%s%s", 
	      xl_operator[token - 3], form_stack[stack_top - 1]);
      Xc_free(form_stack[stack_top - 1]);
      form_stack[stack_top - 1] = formula;
      break;
    case 0x14:
      if (stack_top < 1 || 
	  (strlen(form_stack[stack_top - 1]) + 2 >= Xq_FORMULA_LENGTH))
      { erreur = TRUE; break; }
      NEW_FORMULA;
      sprintf(formula, "%s%s", form_stack[stack_top - 1], 
	      xl_operator[token - 3]);
      Xc_free(form_stack[stack_top - 1]);
      form_stack[stack_top - 1] = formula;
      break;
    case 0x15:
      if ((stack_top < 1) || 
	  (strlen(form_stack[stack_top - 1]) + 2 >= Xq_FORMULA_LENGTH))
      { erreur = TRUE; break; }
      NEW_FORMULA;
      sprintf(formula, "(%s)", form_stack[stack_top - 1]);
      Xc_free(form_stack[stack_top - 1]);
      form_stack[stack_top - 1] = formula;
      break;
      
    case 0x21: 
    case 0x22:
    case 0x38:
      i = 0;
      if (token == 0x21)
      {
	if (version == REC_BIFF2)
	  funct = xl_formula[form_ptr++];
	else
	{ 
	  funct = read_short(xl_formula + form_ptr); form_ptr += 2; 
	}
	while(i < NB_XL2XQ_FUNC1 && funct != xl_to_xq_func1[i].xl_func_number)
	  i++;
	if (i == NB_XL2XQ_FUNC1) { erreur = TRUE; break; }
	if (xl_to_xq_func1[i].xq_func_res != -1)
	  function = F(GlobResources).getString
	    (GlobResources, xl_to_xq_func1[i].xq_func_res);
	else
	  function = xl_to_xq_func1[i].xq_func_name;
	nb_args = xl_to_xq_func1[i].xl_nb_args;
      }
      else
      {
	nb_args = xl_formula[form_ptr++] & 0x7f;
	if (version == REC_BIFF2 || token == 0x38)
	  funct = xl_formula[form_ptr++];
	else
	  { 
	    funct = read_short(xl_formula + form_ptr); form_ptr += 2; 
	  }
	while(i < NB_XL2XQ_FUNC2 && funct != xl_to_xq_func2[i].xl_func_number)
	  i++;
	if (i == NB_XL2XQ_FUNC2) { erreur = TRUE; break; }
	if (xl_to_xq_func2[i].xq_func_res != -1)
	  function = F(GlobResources).getString
	    (GlobResources, xl_to_xq_func2[i].xq_func_res);
	else
	  function = xl_to_xq_func2[i].xq_func_name;
      }
      
      if (nb_args == 0)
      {
	NEW_FORMULA;
	sprintf(formula, "%s()", function);
	form_stack[stack_top] = formula;
	stack_top ++;
	break;
      }

      if (function == NULL)
	{
	  if (stack_top < 1) { erreur = TRUE; break; }
	  sprintf(form2, "%s()", form_stack[stack_top - 1]);
	  strcpy(form_stack[stack_top - 1], form2);
	  break;
	}
      
      if (stack_top < nb_args) { erreur = TRUE; break; }
      formula = form_stack[stack_top - nb_args];
      if (strlen(formula) + strlen(function) + 2 > Xq_FORMULA_LENGTH)
      { erreur = TRUE; break; }
      sprintf(form2, "%s(%s", function, formula);
      strcpy(formula, form2);
      for( i = stack_top - nb_args + 1; i < stack_top; i++)
      {
	if (strlen(formula) + strlen(form_stack[i]) + 2 > Xq_FORMULA_LENGTH)
	{ erreur = TRUE; break; }
	sprintf(form2, "%s;%s", formula, form_stack[i]);
	strcpy(formula, form2);
	Xc_free(form_stack[i]);
      }
      if (strlen(formula) + 2 > Xq_FORMULA_LENGTH)
      { erreur = TRUE; break; }
      strcat(formula, ")");
      stack_top -= nb_args - 1;
      break;
    case 0x19:
      i =  xl_formula[form_ptr++];
      if (TEST_BIT(i, 4))
	{
	  i = 0;
	  while(i < NB_XL2XQ_FUNC2 && xl_to_xq_func2[i].xl_func_number != 0x4)
	    i++;
	  if (i == NB_XL2XQ_FUNC2) { erreur = TRUE; break; }
	  if (xl_to_xq_func2[i].xq_func_res != -1)
	    function = F(GlobResources).getString
	      (GlobResources, xl_to_xq_func2[i].xq_func_res);
	  else
	    function = xl_to_xq_func2[i].xq_func_name;

	  if (stack_top < 1) { erreur = TRUE; break; }
	  formula = form_stack[stack_top - 1];
	  if (strlen(formula) + strlen(function) + 3 > Xq_FORMULA_LENGTH)
	    { erreur = TRUE; break; }
	  sprintf(form2, "%s(%s)", function, formula);
	  strcpy(formula, form2);
	}
      if (version == REC_BIFF2)
	form_ptr++;
      else
	{
	  if (TEST_BIT(i, 6))
	    form_ptr += 3;
	  else
	    form_ptr += 2;
	}
      break;

    default:
      Xc_TRACE(("formula token: %x unknown", token));
      erreur = TRUE;
      break;
    }
  }
  if (erreur || stack_top != 1)
  {
    for(i = 0; i < stack_top; i++)
      Xc_free(form_stack[i]);
    return NULL;
  }
  NEW_FORMULA;
  sprintf(formula, "=%s", form_stack[0]);
  Xc_free(form_stack[0]);
  return formula;
}


