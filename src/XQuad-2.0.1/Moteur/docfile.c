/*
** docfile.c for XQuad in Moteur/
** Loading and saving methods
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
** Started on  Thu Apr 20 12:13:26 1995 Stéphane Boisson
** Last update Tue Jan 18 16:26:28 2000 Emmanuel Paris
*/

/*#define NHISTORY*/
/*#define NTRACE*/

#include "MoteurP.h"

extern int ptom_convert_type[];

/*--- Internal structure ---*/
typedef struct 
{
 c_Moteur *moteur;
 long x;
 long y;
} load_data_t;

typedef struct 
{
 c_Moteur *moteur;
 region_t region;
 coord_t size;
} load_size_t;

typedef struct 
{
 int	type;
 char	*str;
} load_cell_t;

/*--- Local prototypes ---*/
static boolean cb_arealist ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_Moteur *this));
static boolean cb_area ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_Moteur *this));
static boolean cb_widthlist ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_Moteur *this));
static boolean cb_heightlist ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_Moteur *this));
static boolean cb_width ___PROTO((c_DocFile *doc, char *keyword,
				  long param, load_size_t *data));
static boolean cb_height ___PROTO((c_DocFile *doc, char *keyword,
				   long param, load_size_t *data));
static boolean cb_celllist ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_Moteur *this));
static boolean cb_xy ___PROTO((c_DocFile *doc, char *keyword,
			       long param, long *ptr));
static boolean cb_cellstyle ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_CellStyle *base));
static boolean cb_cell ___PROTO((c_DocFile *doc, char *keyword,
				 long param, load_data_t *data));
static boolean cb_string ___PROTO((c_DocFile *doc, char *keyword,
				   long param, char **pstr));
static boolean cb_value ___PROTO((c_DocFile *doc, char *keyword,
				  long param, load_cell_t *lcell));
static boolean cb_percent ___PROTO((c_DocFile *doc, char *keyword,
				    long param, load_cell_t *lcell));
static boolean cb_unit ___PROTO((c_DocFile *doc, char *keyword,
				 long param, load_cell_t *lcell));
static boolean cb_fraction ___PROTO((c_DocFile *doc, char *keyword,
				     long param, load_cell_t *lcell));
static boolean cb_boolean ___PROTO((c_DocFile *doc, char *keyword,
				    long param, load_cell_t *lcell));
static boolean cb_day_in_week ___PROTO((c_DocFile *doc, char *keyword,
					long param, load_cell_t *lcell));
static boolean cb_month ___PROTO((c_DocFile *doc, char *keyword,
				  long param, load_cell_t *lcell));
static boolean cb_date ___PROTO((c_DocFile *doc, char *keyword,
				 long param, load_cell_t *lcell));
static boolean cb_time ___PROTO((c_DocFile *doc, char *keyword,
				 long param, load_cell_t *lcell));
static boolean cb_date_time ___PROTO((c_DocFile *doc, char *keyword,
				      long param, load_cell_t *lcell));
static boolean cb_exponantial ___PROTO((c_DocFile *doc, char *keyword,
					long param, load_cell_t *lcell));


/* ----------------------------------------------------------------- ** 
** writeMoteur - Saving method                                       ** 
** ----------------------------------------------------------------- */
boolean writeMoteur(this, doc)
c_Moteur *this;
c_DocFile *doc;
{
 Xc_TRACE(("write"));

 /*--- Write cell style area ---*/
 if(F(doc).writeKeyword(doc, XcDF_AREALIST_KEYWORD, -1L) &&
    F(doc).startSequence(doc))
 {
  cell_style_list *ptr;

  Xc_TRACE(("writing style areas"));
  for(ptr = this->style_list; ptr != NULL; ptr = ptr->Next)
  {
   char *str;

   if((str = regtostr(&ptr->region)) == NULL) return FALSE;
   Xc_TRACE(("area: `%s'  cstyle: %lx", str, ptr->cstyle));
   if(!F(doc).writeKeyword(doc, XcDF_AREA_KEYWORD, -1L) ||
      !F(doc).startSequence(doc) ||
      !F(doc).write(doc, str, strlen(str)) ||
      !F(doc).writeKeyword(doc, XcDF_CELLSTYLE_KEYWORD, ptr->cstyle->id)
      || !F(doc).endSequence(doc)) {Xc_free(str); return FALSE;}
   Xc_free(str);
  }

  if(!F(doc).endSequence(doc)) return FALSE;
 }
 else
  return FALSE;

 /*--- Write cell widths ---*/
 Xc_TRACE(("writing cell widths"));
 if(F(doc).writeKeyword(doc, XcDF_WIDTHLIST_KEYWORD, -1L) &&
    F(doc).startSequence(doc))
 {
  cell_size_list *ptr;

  ptr = this->col_size;
  Xc_ASSERT(ptr != NULL);
  Xc_ASSERT(ptr->Amin == 0);
  Xc_ASSERT(ptr->Amax >= 0);

  Xc_TRACE(("width[%d; %d] = %g", ptr->Amin, ptr->Amax,
	    SCALE_TO_POINTS(ptr->size)));
  if(!F(doc).writeReal(doc, SCALE_TO_POINTS(ptr->size))) return FALSE;
      
  while((ptr = ptr->Next) != NULL)
  {
   Xc_TRACE(("width[%d; %d] = %g", ptr->Amin, ptr->Amax,
	     SCALE_TO_POINTS(ptr->size)));
   Xc_ASSERT((ptr->Amin >= 0) && (ptr->Amax >= 0));
   Xc_ASSERT(ptr->Amin <= ptr->Amax);

   if(!F(doc).writeKeyword(doc, XcDF_X_KEYWORD, (long)ptr->Amin) ||
      !F(doc).startSequence(doc) ||
      !F(doc).writeReal(doc, SCALE_TO_POINTS(ptr->size)) ||
      !F(doc).endSequence(doc)) return FALSE;
  }
  if(!F(doc).endSequence(doc)) return FALSE;
 }
 else
  return FALSE;

 /*--- Write cell heights ---*/
 Xc_TRACE(("writing cell heights"));
 if(F(doc).writeKeyword(doc, XcDF_HEIGHTLIST_KEYWORD, -1L) &&
    F(doc).startSequence(doc))
 {
  cell_size_list *ptr;
      
  ptr = this->line_size;
  Xc_ASSERT(ptr != NULL);
  Xc_ASSERT(ptr->Amin == 0);
  Xc_ASSERT(ptr->Amax >= 0);

  Xc_TRACE(("height[%d; %d] = %g", ptr->Amin, ptr->Amax,
	    SCALE_TO_POINTS(ptr->size)));
  if(!F(doc).writeReal(doc, SCALE_TO_POINTS(ptr->size))) return FALSE;
      
  while((ptr = ptr->Next) != NULL)
  {
   Xc_TRACE(("height[%d; %d] = %g", ptr->Amin, ptr->Amax,
	     SCALE_TO_POINTS(ptr->size)));
   Xc_ASSERT((ptr->Amin >= 0) && (ptr->Amax >= 0));
   Xc_ASSERT(ptr->Amin <= ptr->Amax);

   if(!F(doc).writeKeyword(doc, XcDF_Y_KEYWORD, (long)ptr->Amin) ||
      !F(doc).startSequence(doc) ||
      !F(doc).writeReal(doc, SCALE_TO_POINTS(ptr->size)) ||
      !F(doc).endSequence(doc)) return FALSE;
  }
  if(!F(doc).endSequence(doc)) return FALSE;
 }
 else
  return FALSE;
  
 /*--- Write cells content ---*/
 if(F(doc).writeKeyword(doc, XcDF_CELLLIST_KEYWORD, -1L) &&
    F(doc).startSequence(doc))
 {
  c_col *column;
  long x, y;

  Xc_TRACE(("writing cells content"));
  x = y = -1;
  for(column = this->tableCL; column != NULL; column = column->Next)
  {
   c_line *line;

   Xc_ASSERT(column->number >= 0);

   /*--- Update X position ---*/
   if(column->number != x)
   {
    x = column->number;
    y=-1;
    if(!F(doc).writeKeyword(doc, XcDF_X_KEYWORD, x + 1))
     return FALSE;
   }

   for(line = column->line; line != NULL; y++, line = line->Next)
   {
    cell *cp;
    char string[Xq_FORMULA_LENGTH], *str;
    char *keyword;
	      
    Xc_ASSERT(line->number >= 0);
	      
    cp = &line->cell;

    /*--- Skip empty cells ---*/
    if(cp->type == Xq_VOID)
    {
     y=-1;
     continue;
    }

    /*--- Update Y position ---*/
    if(line->number != y)
    {
     y = line->number;
     if(!F(doc).writeKeyword(doc, XcDF_Y_KEYWORD, y + 1)) 
      return FALSE;
    }
	      
    Xc_TRACE(("write cell(%ld, %ld)", x, y));
	      
    if(!F(doc).writeKeyword(doc, XcDF_CELL_KEYWORD, -1L) ||
       !F(doc).startSequence(doc)) return FALSE;
	      
    switch(cp->type)
    {
     /*--- Text ---*/
    case Xq_TEXT:
     Xc_TRACE(("Xq_TEXT"));
     if(!F(doc).writeKeyword(doc, XcDF_TEXT_KEYWORD, -1L) ||
	!F(doc).startSequence(doc) ||
	!F(doc).write(doc, cp->text, strlen(cp->text)) ||
	!F(doc).endSequence(doc)) return FALSE;
     break;

     /*--- Formula ---*/
    case Xq_FORMULA:
    case Xq_FORMERR:
     Xc_TRACE(("Xq_FORMULA"));
     switch(F(this).get_string(this, cp, string, Xq_VALUE))
     {
     case Xq_TEXT:
     case Xq_FORMERR:
      if(!F(doc).writeKeyword(doc, XcDF_TEXT_KEYWORD, -1L) ||
	 !F(doc).startSequence(doc) ||
	 !F(doc).write(doc, string, strlen(string)) ||
	 !F(doc).endSequence(doc))
       return FALSE;
      break;
     default:
      if(!F(doc).writeKeyword(doc, XcDF_VALUE_KEYWORD, -1L) ||
	 !F(doc).startSequence(doc) ||
	 !F(doc).write(doc, string, strlen(string)) ||
	 !F(doc).endSequence(doc))
       return FALSE;
      break;
     }
     str = 
      F(this->parser).trans_language(this->parser, 
				     cp->formula, TRUE);
		  
     if(!F(doc).writeKeyword(doc, XcDF_FORMULA_KEYWORD, -1L) ||
	!F(doc).startSequence(doc) ||
	!F(doc).write(doc, str, strlen(str)) ||
	!F(doc).endSequence(doc)) { Xc_free(str); return FALSE; }
     Xc_free(str);
     break;

     /*--- Value ---*/
    case Xq_VALUE:
     Xc_TRACE(("Xq_VALUE"));
     F(this).get_string(this, cp, string, Xq_VALUE);
     if(!F(doc).writeKeyword(doc, XcDF_VALUE_KEYWORD, -1L) ||
	!F(doc).startSequence(doc) ||
	!F(doc).write(doc, string, strlen(string)) ||
	!F(doc).endSequence(doc)) return FALSE;
		  
     switch(ptom_convert_type[(int)cp->ret_type])
     {
     case Xq_PERCENT:
      keyword = XcDF_VT_PERCENT_KEYWORD;
      break;
     case Xq_UNIT:
      keyword = XcDF_VT_UNIT_KEYWORD;
      break;
     case Xq_FRACTION:
      keyword = XcDF_VT_FRACTION_KEYWORD;
      break;
     case Xq_BOOLEAN:
      keyword = XcDF_VT_BOOLEAN_KEYWORD;
      break;
     case Xq_DAY_IN_WEEK:
      keyword = XcDF_VT_DAYINWEEK_KEYWORD;
      break;
     case Xq_MONTH:
      keyword = XcDF_VT_MONTH_KEYWORD;
      break;
     case Xq_DATE:
      keyword = XcDF_VT_DATE_KEYWORD;
      break;
     case Xq_TIME:
      keyword = XcDF_VT_TIME_KEYWORD;
      break;
     case Xq_DATE_TIME:
      keyword = XcDF_VT_DATE_TIME_KEYWORD;
      break;
     case Xq_EXPONANTIAL:
      keyword = XcDF_VT_EXPONANTIAL_KEYWORD;
      break;
     default:
      keyword = NULL;
      break;
     }
     if (keyword && !F(doc).writeKeyword(doc, keyword, -1L))
      return FALSE;
     break;

     /*--- Empty cell ---*/
     /*case Xq_VOID:*/
     /*break;*/

     /*--- Unexpected or unknown code ---*/
    default:
     Xc_BREAK(("Unexpected cell-type #%d", (int)cp->type));
     break;
    }
    if(!F(doc).endSequence(doc)) return FALSE;
   }
  } 
  if(!F(doc).endSequence(doc)) return FALSE;
 }
 else
  return FALSE;
  
 Xc_TRACE(("write ok"));

 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** readMoteur - Loading method                                       ** 
** ----------------------------------------------------------------- */
boolean readMoteur(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Moteur *this;
{
 Xc_TRACE(("read"));

 if(this->BaseStd->cell_style_base->default_cstyle == NULL)
 {
  Xc_ERROR(("CellStyle expected before sheet"));
  return FALSE;
 }

 /*--- Replace old region cellstyle ---*/
 this->style_list->cstyle = this->BaseStd->cell_style_base->default_cstyle;

 if(!F(doc).addCallbacks(doc,
			 XcDF_AREALIST_KEYWORD, cb_arealist, this,
			 XcDF_WIDTHLIST_KEYWORD, cb_widthlist, this,
			 XcDF_HEIGHTLIST_KEYWORD, cb_heightlist, this,
			 XcDF_CELLLIST_KEYWORD, cb_celllist, this,
			 NULL)) return FALSE;

 return F(doc).expectKeyword(doc, NULL, TRUE);
}


/* ----------------------------------------------------------------- ** 
** cb_arealist - Loading callback for area list                      ** 
** ----------------------------------------------------------------- */
static boolean cb_arealist(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Moteur *this;
{
 Xc_TRACE(("cb_arealist"));

 if(!F(doc).addCallbacks(doc, XcDF_AREA_KEYWORD, cb_area, this, NULL))
  return FALSE;
 return F(doc).expectKeyword(doc, XcDF_AREA_KEYWORD, TRUE);
}


/* ----------------------------------------------------------------- ** 
** cb_area - Loading callback for area                               ** 
** ----------------------------------------------------------------- */
static boolean cb_area(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Moteur *this;
{
 CellStyle_t *cstyle;
 region_t *region;
 unsigned char *str;

 Xc_TRACE(("cb_area"));

 if(!F(doc).addCallbacks(doc,
			 XcDF_CELLSTYLE_KEYWORD,
			 cb_cellstyle, this->BaseStd->cell_style_base,
			 NULL)) return FALSE;

 /*--- Get region ---*/
 if(!F(doc).readString(doc, &str) || (str == NULL)) return FALSE;
 Xc_TRACE(("area: `%s'", str));
 region = strtoreg(str);
 Xc_free(str);
#if 0
 if((str = regtostr(region)) == NULL) return FALSE;
 Xc_TRACE(("check area: `%s'", str));
 Xc_free(str);
#endif

 /*--- Get cell style ref ---*/
 if(doc->data_type == XcDF_CELLSTYLE_TYPE)
 {
  cstyle = doc->data_ptr;
  doc->data_type = XcDF_NONE;
 }
 else
 {
  Xc_TRACE(("no cellstyle"));
  cstyle = NULL;
 }

 /*--- Errors ---*/
 if(region == NULL) return FALSE;
 if(cstyle == NULL)
 {
  Xc_free(region);
  return FALSE;
 }

 /*--- Set area style ---*/
 cstyle->count++;
 F(this).set_cell_style2(this, cstyle, region);

 Xc_free(region);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_cellstyle - Loading callback for cellstyle references          ** 
** ----------------------------------------------------------------- */
static boolean cb_cellstyle(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
c_CellStyle *base;
{
 CellStyle_t *ptr;

 Xc_TRACE(("cb_cellstyle(%ld)", param));

 if(param < 0) param = 0;
 for(ptr = base->cell_style; ptr != NULL; ptr = ptr->NextCellStyle)
  if(ptr->id == param)
  {
   doc->data_type = XcDF_CELLSTYLE_TYPE;
   doc->data_ptr = ptr;
   return TRUE;
  }

 Xc_ERROR(("Bad cell style #%ld", param));
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_widthlist - Loading callback for width list                    ** 
** ----------------------------------------------------------------- */
static boolean cb_widthlist(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Moteur *this;
{
 load_size_t data;
 real r;

 Xc_TRACE(("cb_widthlist"));

 if(!F(doc).addCallbacks(doc, XcDF_X_KEYWORD, cb_width, &data, NULL))
  return FALSE;

 data.region.x_min = 0;
 data.moteur = this;

 if(!F(doc).readReal(doc, &r)) return FALSE;
 data.size = SCALE_FROM_POINTS(r);
  
 if(!F(doc).expectKeyword(doc, NULL, TRUE)) return FALSE;

 data.region.x_max = COLUMN_MAX;

 Xc_TRACE(("width[%d; %d] = %g", data.region.x_min, data.region.x_max,
	   SCALE_TO_POINTS(data.size)));
 F(this).set_size_zone(this, Xc_COLUMN, &data.region, data.size);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_width - Loading callback for width                             ** 
** ----------------------------------------------------------------- */
static boolean cb_width(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
load_size_t *data;
{
 real r;

 Xc_TRACE(("cb_width(%ld)", param));

 if(param < 0) return FALSE;
 if(!F(doc).readReal(doc, &r)) return FALSE;

 if((data->region.x_max = param - 1) >= data->region.x_min)
 {
  Xc_TRACE(("width[%d; %d] = %g", data->region.x_min, data->region.x_max,
	    SCALE_TO_POINTS(data->size)));
  F(data->moteur).set_size_zone(data->moteur, Xc_COLUMN,
				&data->region, data->size);
 }
      
 data->size = SCALE_FROM_POINTS(r);
 data->region.x_min = param;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_heightlist - Loading callback for height                       ** 
** ----------------------------------------------------------------- */
static boolean cb_heightlist(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Moteur *this;
{
 load_size_t data;
 real r;

 Xc_TRACE(("cb_heightlist"));

 if(!F(doc).addCallbacks(doc, XcDF_Y_KEYWORD, cb_height, &data, NULL))
  return FALSE;

 data.region.y_min = 0;
 data.moteur = this;

 if(!F(doc).readReal(doc, &r)) return FALSE;
 data.size = SCALE_FROM_POINTS(r);

 if(!F(doc).expectKeyword(doc, NULL, TRUE)) return FALSE;

 data.region.y_max = ROW_MAX;

 Xc_TRACE(("height[%d; %d] = %g", data.region.y_min, data.region.y_max,
	   SCALE_TO_POINTS(data.size)));
 F(this).set_size_zone(this, Xc_ROW, &data.region, data.size);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_height - Loading callback for height                           ** 
** ----------------------------------------------------------------- */
static boolean cb_height(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
load_size_t *data;
{
 real r;

 Xc_TRACE(("cb_height(%ld)", param));

 if(param < 0) return FALSE;
 if(!F(doc).readReal(doc, &r)) return FALSE;
  
 if((data->region.y_max = param - 1) >= data->region.y_min)
 {
  Xc_TRACE(("height[%d; %d] = %g", data->region.y_min, data->region.y_max,
	    SCALE_TO_POINTS(data->size)));
  F(data->moteur).set_size_zone(data->moteur, Xc_ROW,
				&data->region, data->size);
 }

 data->size = SCALE_FROM_POINTS(r);
 data->region.y_min = param;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_celllist - Loading callback for cells                          ** 
** ----------------------------------------------------------------- */
static boolean cb_celllist(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Moteur *this;
{
 load_data_t data;

 Xc_TRACE(("cb_celllist"));

 data.x = data.y = 0;
 data.moteur = this;

 if(!F(doc).addCallbacks(doc,
			 XcDF_X_KEYWORD, cb_xy, &data.x,
			 XcDF_Y_KEYWORD, cb_xy, &data.y,
			 XcDF_CELL_KEYWORD, cb_cell, &data,
			 NULL)) return FALSE;
 return F(doc).expectKeyword(doc, NULL, TRUE);
}


/* ----------------------------------------------------------------- ** 
** cb_xy - Loading callback for X or Y  position                     ** 
** ----------------------------------------------------------------- */
static boolean cb_xy(doc, keyword, param, ptr)
c_DocFile *doc;
char *keyword;
long param;
long *ptr;
{
 Xc_TRACE(("cb_xy(%s == %ld)", keyword, param - 1L));

 *ptr = param - 1;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_cell - Loading callback for a cell content                     ** 
** ----------------------------------------------------------------- */
static boolean cb_cell(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
load_data_t *data;
{
 cell *this;
 char	*text;
 char	*formula;
 load_cell_t	value;
 char	*str;

 Xc_TRACE(("cb_cell"));

 if(data == NULL) 
  return FALSE;
 this = F(data->moteur).add_cell(data->moteur, data->y, data->x);
  
 text = formula = NULL;
 value.type = 0;
 value.str = NULL;
 if(!F(doc).addCallbacks(doc,
			 XcDF_TEXT_KEYWORD, cb_string, &text,
			 XcDF_FORMULA_KEYWORD, cb_string, &formula,
			 XcDF_VALUE_KEYWORD, cb_value, &value, 
			 XcDF_VT_PERCENT_KEYWORD, cb_percent, &value,
			 XcDF_VT_UNIT_KEYWORD, cb_unit, &value,
			 XcDF_VT_FRACTION_KEYWORD, cb_fraction, &value,
			 XcDF_VT_BOOLEAN_KEYWORD, cb_boolean, &value,
			 XcDF_VT_DAYINWEEK_KEYWORD, cb_day_in_week, &value,
			 XcDF_VT_MONTH_KEYWORD, cb_month, &value,
			 XcDF_VT_DATE_KEYWORD, cb_date, &value,
			 XcDF_VT_TIME_KEYWORD, cb_time, &value,
			 XcDF_VT_DATE_TIME_KEYWORD, cb_date_time, &value,
			 XcDF_VT_EXPONANTIAL_KEYWORD, cb_exponantial, &value,
			 NULL) ||
    !F(doc).expectKeyword(doc, NULL, TRUE))
 {
  /*--- Error ---*/
  F(data->moteur).del_cell(data->moteur, data->y, data->x);
  return FALSE;
 }
  
 if (formula)
 {
  Xc_TRACE(("set formula: `%s'", formula));
  str = 
   F(data->moteur->parser).trans_language(data->moteur->parser, 
					  formula, FALSE);

  F(data->moteur).set_string(data->moteur, data->x, data->y, 
			     this, str, NULL);
  if ((this->status == TYPE_NAME_ERROR) && (text || value.str))
  {
   if (text)
   {
    F(data->moteur).set_string(data->moteur, data->x, data->y, 
			       this, text, NULL);
   }
   else
   {
    F(data->moteur).set_string(data->moteur, data->x, data->y, 
			       this, value.str, NULL);
    this->ret_type = value.type;
   }
  }
  if (text) Xc_free(text);
  if (value.str) Xc_free(value.str);
  Xc_free(str);
  Xc_free(formula);
 }
 else
 {
  if (text)
  {
   Xc_TRACE(("text: `%s'", text));
   F(data->moteur).set_string(data->moteur, data->x, data->y, 
			      this, text, NULL);
   Xc_free(text);
  }
  if (value.str)
  {
   Xc_TRACE(("value.str: `%s'", value.str));
   F(data->moteur).set_string(data->moteur, data->x, data->y, 
			      this, value.str, NULL);
   this->ret_type = value.type;
   Xc_free(value.str);
  }
 }
  
 data->y++;
 Xc_TRACE(("cell ok"));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_string - Loading callback for a string                         ** 
** ----------------------------------------------------------------- */
static boolean cb_string(doc, keyword, param, pstr)
c_DocFile *doc;
char *keyword;
long param;
char **pstr;
{
 unsigned char *str;

 Xc_TRACE(("cb_string(%s)", keyword));

 if(!F(doc).readString(doc, &str)) return FALSE;
 Xc_TRACE(("string=`%s'", str));
 *pstr = (char *)str;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_value - Loading callback for a value                           ** 
** ----------------------------------------------------------------- */
static boolean cb_value(doc, keyword, param, lcell)
c_DocFile *doc;
char *keyword;
long param;
load_cell_t *lcell;
{
 unsigned char *str;

 Xc_TRACE(("cb_value"));

 if(!F(doc).readString(doc, &str)) return FALSE;
 Xc_TRACE(("string=`%s'", str));
 if (!lcell->type) lcell->type = TYPE_NUM;
 lcell->str = (char *)str;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_percent - Loading callback for percent type                    ** 
** ----------------------------------------------------------------- */
static boolean cb_percent(doc, keyword, param, lcell)
c_DocFile *doc;
char *keyword;
long param;
load_cell_t *lcell;
{
 Xc_TRACE(("cb_percent"));

 lcell->type = TYPE_PERCENT;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_unit - Loading callback for unit type                          ** 
** ----------------------------------------------------------------- */
static boolean cb_unit(doc, keyword, param, lcell)
c_DocFile *doc;
char *keyword;
long param;
load_cell_t *lcell;
{
 Xc_TRACE(("cb_unit"));

 lcell->type = TYPE_UNIT;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_fraction - Loading callback for fraction type                  ** 
** ----------------------------------------------------------------- */
static boolean cb_fraction(doc, keyword, param, lcell)
c_DocFile *doc;
char *keyword;
long param;
load_cell_t *lcell;
{
 Xc_TRACE(("cb_fraction"));

 lcell->type = TYPE_FRACTION;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_boolean - Loading callback for boolean type                    ** 
** ----------------------------------------------------------------- */
static boolean cb_boolean(doc, keyword, param, lcell)
c_DocFile *doc;
char *keyword;
long param;
load_cell_t *lcell;
{
 Xc_TRACE(("cb_boolean"));

 lcell->type = TYPE_BOOLEAN;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_fraction - Loading callback for fraction type                  ** 
** ----------------------------------------------------------------- */
static boolean cb_day_in_week(doc, keyword, param, lcell)
c_DocFile *doc;
char *keyword;
long param;
load_cell_t *lcell;
{
 Xc_TRACE(("cb_day_in_week"));

 lcell->type = TYPE_DAY_IN_WEEK;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_fraction - Loading callback for fraction type                  ** 
** ----------------------------------------------------------------- */
static boolean cb_month(doc, keyword, param, lcell)
c_DocFile *doc;
char *keyword;
long param;
load_cell_t *lcell;
{
 Xc_TRACE(("cb_month"));

 lcell->type = TYPE_MONTH;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_fraction - Loading callback for fraction type                  ** 
** ----------------------------------------------------------------- */
static boolean cb_date(doc, keyword, param, lcell)
c_DocFile *doc;
char *keyword;
long param;
load_cell_t *lcell;
{
 Xc_TRACE(("cb_date"));

 lcell->type = TYPE_DATE;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_fraction - Loading callback for fraction type                  ** 
** ----------------------------------------------------------------- */
static boolean cb_time(doc, keyword, param, lcell)
c_DocFile *doc;
char *keyword;
long param;
load_cell_t *lcell;
{
 Xc_TRACE(("cb_time"));

 lcell->type = TYPE_TIME;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_fraction - Loading callback for fraction type                  ** 
** ----------------------------------------------------------------- */
static boolean cb_date_time(doc, keyword, param, lcell)
c_DocFile *doc;
char *keyword;
long param;
load_cell_t *lcell;
{
 Xc_TRACE(("cb_date_time"));

 lcell->type = TYPE_DATE_TIME;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_fraction - Loading callback for fraction type                  ** 
** ----------------------------------------------------------------- */
static boolean cb_exponantial(doc, keyword, param, lcell)
c_DocFile *doc;
char *keyword;
long param;
load_cell_t *lcell;
{
 Xc_TRACE(("cb_exponantial"));

 lcell->type = TYPE_EXPONANTIAL;
 return TRUE;
}
