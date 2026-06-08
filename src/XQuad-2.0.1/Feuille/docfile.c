/*
** docfile.c for XQuad in Feuille/
** Loading & saving methods Feuille class
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
** Started on  Fri Aug 25 11:57:36 1995 Stéphane Boisson
** Last update Fri Aug 16 00:28:54 1996 Emmanuel Paris
*/

#include "FeuilleP.h"

static boolean cb_setup ___PROTO((c_DocFile *doc, char *keyword,
                                  long param, setup_print_t *setup));
static boolean cb_boolean ___PROTO((c_DocFile *doc, char *keyword,
				    long param, boolean *ptr));
static boolean cb_margins ___PROTO((c_DocFile *doc, char *keyword,
                                    long param, setup_print_t *data));
static boolean cb_format ___PROTO((c_DocFile *doc, char *keyword,
				   long param, setup_print_t *data));
static boolean cb_orientation ___PROTO((c_DocFile *doc, char *keyword,
					long param, setup_print_t *data));
static boolean cb_scale ___PROTO((c_DocFile *doc, char *keyword,
				  long param, setup_print_t *data));
static boolean cb_scalefit ___PROTO((c_DocFile *doc, char *keyword,
				  long param, setup_print_t *data));
static boolean cb_zoom ___PROTO((c_DocFile *doc, char *keyword,
                                 long param, c_Feuille *sheet));
static boolean cb_origin ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_Feuille *sheet));
static boolean cb_active_cell ___PROTO((c_DocFile *doc, char *keyword,
					long param, c_Feuille *sheet));
static boolean cb_arealist ___PROTO((c_DocFile *doc, char *keyword,
                                     long param, c_Feuille *this));
static boolean cb_area ___PROTO((c_DocFile *doc, char *keyword,
                                 long param, c_Feuille *this));
static boolean cb_bitfield ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_Feuille *sheet));


/* ----------------------------------------------------------------- ** 
** readFeuille - Feuille loading                                     ** 
** ----------------------------------------------------------------- */
boolean readFeuille(doc, keyword, param, this)
c_DocFile	*doc;
char		*keyword;
long		param;
c_Feuille	*this;
{  
  Xc_TRACE(("read(%ld)", param));

  if(!F(doc).addCallbacks(doc,
			  XcDF_PRINTSETUP_KEYWORD, cb_setup, this->print_setup,
			  XcDF_SHEET_ZOOM_KEYWORD, cb_zoom, this,
			  XcDF_SHEET_ORIGIN_KEYWORD, cb_origin, this,
			  XcDF_ACTIVE_CELL_KEYWORD, cb_active_cell, this,
			  XcDF_SELECTED_AREAS_KEYWORD, cb_arealist, this,
			  XcDF_BITFIELD_KEYWORD, cb_bitfield, this,
			  XcDF_FRAMELIST_KEYWORD, FUNCTION(c_FrMg, read), this,
                          NULL)) return FALSE;
  return F(this->moteur).read(doc, keyword, param, this->moteur);
}


/* ----------------------------------------------------------------- ** 
** writeFeuille - Feuille saving                                     ** 
** ----------------------------------------------------------------- */
boolean writeFeuille(this, doc)
c_Feuille *this;
c_DocFile *doc;
{
  setup_print_t *ptr;
  region_t	*region;
  long		bitfield;
  char		*str, cell_str[10];
  
  Xc_HISTORY(("write"));

  if(!F(doc).writeKeyword(doc, XcDF_SHEET_KEYWORD, 1L) ||
     !F(doc).startSequence(doc)) return FALSE;

  if((ptr = this->print_setup) != NULL)
    {
      Xc_TRACE(("write PrintSetup"));

      if(!F(doc).writeKeyword(doc, XcDF_PRINTSETUP_KEYWORD, -1L) ||
	 !F(doc).startSequence(doc)) return FALSE;

      /*--- Page printing direction ---*/
      if((ptr->direction_f == TRUE) &&
	 !F(doc).writeKeyword(doc, XcDF_DIRECTION_KEYWORD, -1L)) return FALSE;
      
      /*--- Print grid ---*/
      if((ptr->grid_f == TRUE) &&
	 !F(doc).writeKeyword(doc, XcDF_GRID_KEYWORD, -1L)) return FALSE;

      /*--- Print columns & rows title ---*/
      if((ptr->rowcol_f == TRUE) &&
	 !F(doc).writeKeyword(doc, XcDF_AXIS_KEYWORD, -1L)) return FALSE;

      /*--- Margins ---*/
      if(!F(doc).writeKeyword(doc, XcDF_MARGINS_KEYWORD, -1L) ||
	 !F(doc).startSequence(doc) ||
	 !F(doc).writeReal(doc, SCALE_TO_POINTS(ptr->left_margin)) ||
	 !F(doc).writeReal(doc, SCALE_TO_POINTS(ptr->right_margin)) ||
	 !F(doc).writeReal(doc, SCALE_TO_POINTS(ptr->top_margin)) ||
	 !F(doc).writeReal(doc, SCALE_TO_POINTS(ptr->bottom_margin)) ||
	 !F(doc).endSequence(doc)) return FALSE;
	
      /*--- Page format ---*/
      if(!F(doc).writeKeyword(doc, ptr->page_format->keyword,
			      ptr->page_format->param) ||
	 ((ptr->page_format == &Xc_Table_Page_Format[Xc_CUSTOM_FORMAT]) &&
	  (!F(doc).startSequence(doc) ||
	   !F(doc).writeReal(doc, SCALE_TO_POINTS(ptr->width)) ||
	   !F(doc).writeReal(doc, SCALE_TO_POINTS(ptr->height)) ||
	   !F(doc).endSequence(doc)))) return FALSE;

      /*--- Vertical center ---*/
      if((ptr->vcenter_f == TRUE) &&
	 !F(doc).writeKeyword(doc, XcDF_VCENTER_KEYWORD, -1L)) return FALSE;

      /*--- Vertical center ---*/
      if((ptr->hcenter_f == TRUE) &&
	 !F(doc).writeKeyword(doc, XcDF_HCENTER_KEYWORD, -1L)) return FALSE;

      /*--- Write page orientation ---*/
      if(!F(doc).writeKeyword(doc, (ptr->orientation != Xc_LANDSCAPE)?
			      XcDF_PORTRAIT_KEYWORD : XcDF_LANDSCAPE_KEYWORD,
			      -1L)) return FALSE;

      /*--- Scaling ---*/
      if(ptr->scale_mode == 1)
	{
	  if(!F(doc).writeKeyword(doc, XcDF_SCALE_KEYWORD, -1L) ||
	     !F(doc).startSequence(doc) ||
	     !F(doc).writeReal(doc, SCALE_TO_COEF(ptr->scale)) ||
	     !F(doc).endSequence(doc)) return FALSE;
	}
      else if(ptr->scale_mode == 2)
	{
	  if(!F(doc).writeKeyword(doc, XcDF_SCALEFIT_KEYWORD, -1L) ||
	     !F(doc).startSequence(doc) ||
	     !F(doc).writeInteger(doc, (long)ptr->horizontal_pages) ||
	     !F(doc).writeInteger(doc, (long)ptr->vertical_pages) ||
	     !F(doc).endSequence(doc)) return FALSE;
	}

      if(!F(doc).endSequence(doc)) return FALSE;
    }
  
  /*--- Write sheet zoom ---*/
  if (!F(doc).writeKeyword(doc, XcDF_SHEET_ZOOM_KEYWORD, -1L) ||
      !F(doc).startSequence(doc) ||
      !F(doc).writeReal(doc, SCALE_TO_COEF(this->sFactor)) ||
      !F(doc).endSequence(doc)) return FALSE;
  
  /*--- Write sheet origin ---*/
  if (!F(doc).writeKeyword(doc, XcDF_SHEET_ORIGIN_KEYWORD, -1L) ||
      !F(doc).startSequence(doc) ||
      !F(doc).writeInteger(doc, this->column_orig) ||
      !F(doc).writeInteger(doc, this->row_orig) ||
      !F(doc).writeReal(doc, SCALE_TO_POINTS(this->frmg->sOrig_X)) ||
      !F(doc).writeReal(doc, SCALE_TO_POINTS(this->frmg->sOrig_Y)) ||
      !F(doc).endSequence(doc)) return FALSE;
  
  /*--- Write sheet bitfield ---*/
  bitfield = this->grid ? 1 : 0;
  bitfield <<= 1; bitfield |= this->Column_Origin_actif ? 1 : 0;
  bitfield <<= 1; bitfield |= this->Row_Origin_actif ? 1 : 0;
  bitfield <<= 1; bitfield |= this->page_mark ? 1 : 0;
  bitfield <<= 1; bitfield |= 
    this->BaseStd->cell_style_base->display_formula ? 1 : 0;
  bitfield <<= 1; bitfield |= 
    this->BaseStd->cell_style_base->display_zero_cell ? 1 : 0;
  
  colrowtostr(this->cell_x, this->cell_y, cell_str);
  if (!F(doc).writeKeyword(doc, XcDF_ACTIVE_CELL_KEYWORD, -1L) ||
      !F(doc).startSequence(doc) ||
      !F(doc).write(doc, cell_str, strlen(cell_str)) ||
      !F(doc).endSequence(doc)) return FALSE;
  
  if (!F(doc).writeKeyword(doc, XcDF_SELECTED_AREAS_KEYWORD, -1L) ||
      !F(doc).startSequence(doc)) return FALSE;
  
  region = this->cell_region;
  while(region)
  {
    str = regtostr(region);
    if (!str) return FALSE;
    
    if (!F(doc).writeKeyword(doc, XcDF_AREA_KEYWORD, -1L) ||
	!F(doc).startSequence(doc) ||
	!F(doc).write(doc, str, strlen(str)) ||
	!F(doc).endSequence(doc)) 
    {
      Xc_free(str);
      return FALSE;
    }
    Xc_free(str);
    region = region->NextRegion;
  }
  
  if (!F(doc).endSequence(doc)) return FALSE;
      
  if(!F(doc).writeKeyword(doc, XcDF_BITFIELD_KEYWORD, bitfield))
    return FALSE;  
  
  return(F(this->moteur).write(this->moteur, doc) &&
	 F(this->frmg).write(this->frmg, doc) &&
	 F(doc).endSequence(doc));
}



/* ----------------------------------------------------------------- ** 
** cb_setup - Print setup loading callback                           ** 
** ----------------------------------------------------------------- */
static boolean cb_setup(doc, keyword, param, setup)
c_DocFile	*doc;
char		*keyword;
long		param;
setup_print_t	*setup;
{
  int i;

  Xc_TRACE(("cb_setup"));

  /*--- Init structure ---*/
  setup->direction_f = setup->grid_f = setup->rowcol_f = FALSE;
  setup->left_margin = setup->right_margin = COORD_ZERO;
  setup->top_margin = setup->bottom_margin = COORD_ZERO;
  setup->vcenter_f = setup->hcenter_f = FALSE;
  setup->scale_mode = 0;

  /*--- Page formats ---*/
  for(i = Xc_Count_Page_Format; i-- > 0; )
    if(!F(doc).addCallbacks(doc, Xc_Table_Page_Format[i].keyword,
                            cb_format, setup, NULL)) return FALSE;
  
  if(!F(doc).addCallbacks(doc,
			  XcDF_DIRECTION_KEYWORD,
			  cb_boolean, &setup->direction_f,
			  XcDF_AXIS_KEYWORD, cb_boolean, &setup->rowcol_f,
			  XcDF_GRID_KEYWORD, cb_boolean, &setup->grid_f,
			  XcDF_VCENTER_KEYWORD, cb_boolean, &setup->vcenter_f,
			  XcDF_HCENTER_KEYWORD, cb_boolean, &setup->hcenter_f,
			  XcDF_MARGINS_KEYWORD, cb_margins, setup,
			  /*___Page orientation___*/
			  XcDF_PORTRAIT_KEYWORD, cb_orientation, setup,
			  XcDF_LANDSCAPE_KEYWORD, cb_orientation, setup,
			  /*___Scaling___*/
			  XcDF_SCALEFIT_KEYWORD, cb_scalefit, setup,
			  XcDF_SCALE_KEYWORD, cb_scale, setup,
			  NULL)) return FALSE;
  
  return F(doc).expectKeyword(doc, NULL, TRUE);
}


/* ----------------------------------------------------------------- ** 
** cb_margins - Loading callbcak for page margins                    ** 
** ----------------------------------------------------------------- */
static boolean cb_margins(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
setup_print_t *data;
{
  real left, right, top, bottom;

  Xc_TRACE(("cb_margins"));

  if(!F(doc).readReal(doc, &left)|| !F(doc).readReal(doc, &right) ||
     !F(doc).readReal(doc, &top)|| !F(doc).readReal(doc, &bottom))
    return FALSE;

  data->left_margin = SCALE_FROM_POINTS(left);
  data->right_margin = SCALE_FROM_POINTS(right);
  data->top_margin = SCALE_FROM_POINTS(top);
  data->bottom_margin = SCALE_FROM_POINTS(bottom);

  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_format - Loading callback for page format                      ** 
** ----------------------------------------------------------------- */
static boolean cb_format(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
setup_print_t *data;
{
  int i;

  Xc_TRACE(("cb_format(`%s')", keyword));

  for(i = 0; i < Xc_Count_Page_Format; i++)
    if((param == Xc_Table_Page_Format[i].param) &&
       !strcmp(Xc_Table_Page_Format[i].keyword, keyword))
      {
        data->page_format = &Xc_Table_Page_Format[i];
        if(i == Xc_CUSTOM_FORMAT)
          {
            real w, h;

            if(!F(doc).readReal(doc, &w)|| !F(doc).readReal(doc, &h))
              return FALSE;
            data->width = SCALE_FROM_POINTS(w);
            data->height = SCALE_FROM_POINTS(h);

            Xc_TRACE(("custom format: %gx%g",
                      SCALE_TO_CENTIMETERS(w), SCALE_TO_CENTIMETERS(h)));
          }
        else
          {
            Xc_TRACE(("format: `%s'", Xc_Table_Page_Format[i].name));

	    data->width = data->page_format->width;
	    data->height = data->page_format->height;
          }

        return TRUE;
      }
  Xc_BREAK(("Format not found"));
  return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_boolean - Set a boolean                                        ** 
** ----------------------------------------------------------------- */
static boolean cb_boolean(doc, keyword, param, ptr)
c_DocFile	*doc;
char		*keyword;
long		param;
boolean		*ptr;
{
  Xc_TRACE(("cb_boolean(`%s')", keyword));

  *ptr = TRUE;
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_orientation - Loading callback for orientation                 ** 
** ----------------------------------------------------------------- */
static boolean cb_orientation(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
setup_print_t *data;
{
  Xc_TRACE(("cb_orientation(`%s')", keyword));

  data->orientation = (strcmp(keyword, XcDF_LANDSCAPE_KEYWORD)?
                       Xc_PORTRAIT : Xc_LANDSCAPE);
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_scale - Scale loading callback                                 ** 
** ----------------------------------------------------------------- */
static boolean cb_scale(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
setup_print_t *data;
{
  real r;

  Xc_TRACE(("cb_scale"));

  if(!F(doc).readReal(doc, &r)) return FALSE;
  data->scale = SCALE_FROM_COEF(r);
  data->scale_mode = 1;
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_scalefit - Scale fit loading callback                          ** 
** ----------------------------------------------------------------- */
static boolean cb_scalefit(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
setup_print_t *data;
{
  long w, h;

  Xc_TRACE(("cb_scalefit"));

  if(!F(doc).readInteger(doc, &w) || !F(doc).readInteger(doc, &h))
    return FALSE;
  data->horizontal_pages = (unsigned int)w;
  data->vertical_pages = (unsigned int)h;
  data->scale_mode = 2;
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_zoom - Loading callback for sheet zoom factor loading          ** 
** ----------------------------------------------------------------- */
static boolean cb_zoom(doc, keyword, param, sheet)
c_DocFile *doc;
char *keyword;
long param;
c_Feuille *sheet;
{
  real factor;
  Xc_TRACE(("cb_zoom(`%s')", keyword));
  
  if (!F(doc).readReal(doc, &factor)) return FALSE;
  if (factor != 0.0)
  {
    sheet->sFactor = SCALE_FROM_COEF(factor);
    sheet->sScale = (coord_t)(COORD_DEFAULT / SCALE_FROM_COEF(factor));
  }
  
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_origin - Loading callback for sheet cell origin loading        ** 
** ----------------------------------------------------------------- */
static boolean cb_origin(doc, keyword, param, sheet)
c_DocFile *doc;
char *keyword;
long param;
c_Feuille *sheet;
{
  long col_orig, row_orig;
  real origx, origy;
  
  Xc_TRACE(("cb_origin(`%s')", keyword));
  
  if (!F(doc).readInteger(doc, &col_orig) ||
      !F(doc).readInteger(doc, &row_orig) ||
      !F(doc).readReal(doc, &origx) ||
      !F(doc).readReal(doc, &origy))
    return FALSE;
  
  sheet->column_orig = (int)col_orig;
  sheet->row_orig = (int)row_orig; 
  
  sheet->frmg->sOrig_X = SCALE_FROM_POINTS(origx); 
  sheet->frmg->sOrig_Y = SCALE_FROM_POINTS(origy); 
  
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_active_cell - Loading callback for sheet active cell loading   ** 
** ----------------------------------------------------------------- */
static boolean cb_active_cell(doc, keyword, param, sheet)
c_DocFile *doc;
char *keyword;
long param;
c_Feuille *sheet;
{
  unsigned char *str;
  int column, row;
  
  Xc_TRACE(("cb_active_cell(`%s')", keyword));
  
  if(!F(doc).readString(doc, &str) || (str == NULL)) return FALSE;
  
  strtocolrow(str, &column, &row);
  sheet->cell_x = column;
  sheet->cell_y = row; 
  
  Xc_free(str);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_arealist - Loading callback for area list                      ** 
** ----------------------------------------------------------------- */
static boolean cb_arealist(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_Feuille *this;
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
c_Feuille *this;
{
  region_t *region;
  unsigned char *str;
  int type;
  
  Xc_TRACE(("cb_area"));

  /*--- Get region ---*/
  if(!F(doc).readString(doc, &str) || (str == NULL)) return FALSE;
  Xc_TRACE(("area: `%s'", str));
  region = strtoreg(str);
  
  type = Xq_CELL;
  if (region->x_min == 0 && region->x_max == COLUMN_MAX)
    type = Xq_ROW;
  
  if (region->y_min == 0 && region->y_max == ROW_MAX)
  {
    if (type == Xq_ROW)
      type = Xq_ORIGIN;
    else
      type = Xq_COLUMN;
  }
  
  F(this).SelectRegion(this, region->x_min, region->y_min, 
		       region->x_max, region->y_max, type, FALSE);
  
  Xc_free(str);
  Xc_free(region);
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_bitfield - Loading callback for page bitfield loading          ** 
** ----------------------------------------------------------------- */
static boolean cb_bitfield(doc, keyword, param, sheet)
c_DocFile *doc;
char *keyword;
long param;
c_Feuille *sheet;
{
  Xc_TRACE(("cb_bitfield(`0x%x')", param));
  
  sheet->BaseStd->cell_style_base->display_zero_cell = 
    (param & 1) ? TRUE : FALSE; param >>= 1;
  sheet->BaseStd->cell_style_base->display_formula = 
    (param & 1) ? TRUE : FALSE; param >>= 1;
  sheet->page_mark = (param & 1) ? TRUE : FALSE; param >>= 1;
  sheet->Row_Origin_actif = (param & 1) ? TRUE : FALSE; param >>= 1;
  sheet->Column_Origin_actif = (param & 1) ? TRUE : FALSE; param >>= 1;
  sheet->grid = (param & 1) ? TRUE : FALSE; param >>= 1;
  
  return TRUE;
}
