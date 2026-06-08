/*
** load.c for Xclamation in Page/
** Loading stuff for Page class
**
** Copyright (C) 1994-2000 Axene.
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
** Started on  Thu Jan  5 15:51:31 1995 Stéphane Boisson
** Last update Mon Oct  5 13:36:17 1998 Robin Castanier
*/

#include "Page.h"

/*--- Global functions ---*/
boolean readPage ___PROTO((c_DocFile *doc, char *keyword,
			   long *page_number, BaseStd_t *base));


/*--- Internal structure ---*/
typedef struct 
{
 c_Page *page;
 page_format_t *format;
 page_type_t type;
 page_orient_t orientation;

 /*___Custom size___*/
 boolean custom_flag;
 coord_t width;
 coord_t height;
  
 /*___Margins___*/
 coord_t left_margin;
 coord_t right_margin;
 coord_t upper_margin;
 coord_t lower_margin;

 /*___Gridline___*/
 coord_t orig_x;
 coord_t orig_y;
 coord_t delta_x;
 coord_t delta_y;

 /*___Grid___*/
 int column_count;
 int row_count;
 coord_t inter_column;
 coord_t inter_row;
  
 /*___Selected___*/
 boolean selected;
} page_load_t;


/*--- Local functions ---*/
static boolean cb_frame ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_Page *page));
static boolean cb_zoom ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_Page *page));
static boolean cb_origin ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_Page *page));
static boolean cb_bitfield ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_Page *page));
static boolean cb_type ___PROTO((c_DocFile *doc, char *keyword,
				 long param, page_load_t *data));
static boolean cb_format ___PROTO((c_DocFile *doc, char *keyword,
				   long param, page_load_t *data));
static boolean cb_orientation ___PROTO((c_DocFile *doc, char *keyword,
					long param, page_load_t *dat));
static boolean cb_gridline ___PROTO((c_DocFile *doc, char *keyword,
				     long param, page_load_t *dat));
static boolean cb_grid ___PROTO((c_DocFile *doc, char *keyword,
				 long param, page_load_t *dat));
static boolean cb_margins ___PROTO((c_DocFile *doc, char *keyword,
				    long param, page_load_t *dat));
static boolean cb_line ___PROTO((c_DocFile *doc, char *keyword,
				 long param, page_load_t *dat));
static boolean cb_selected ___PROTO((c_DocFile *doc, char *keyword,
				     long param, page_load_t *dat));


/* ----------------------------------------------------------------- ** 
** readPage - Page loading                                           ** 
** ----------------------------------------------------------------- */
boolean readPage(doc, keyword, page_number, base)
c_DocFile	*doc;
char		*keyword;
long		*page_number;
BaseStd_t	*base;
{
 page_load_t param;
 c_Page *page;
 int i;

 Xc_HISTORY(("read"));

 param.format = &Xc_Table_Page_Format[Xc_DEFAULT_FORMAT];
 param.type = Xc_PAGE_LEFT;
 param.orientation = Xc_PORTRAIT;
 param.custom_flag = FALSE;
 param.left_margin = SCALE_FROM_CENTIMETERS(1.5);
 param.right_margin = SCALE_FROM_CENTIMETERS(1.5);
 param.upper_margin = SCALE_FROM_CENTIMETERS(1);
 param.lower_margin = SCALE_FROM_CENTIMETERS(1);
 param.orig_x = param.orig_y = COORD_ZERO;
 param.delta_x = param.delta_y = SCALE_FROM_CENTIMETERS(5.94);
 param.column_count = 1;
 param.row_count = 1;
 param.inter_column = SCALE_FROM_CENTIMETERS(1);
 param.inter_row = SCALE_FROM_CENTIMETERS(1);
 param.selected = FALSE;
 
  
 /*--- Page formats ---*/
 for(i = Xc_Count_Page_Format; i-- > 0; )
  if(!F(doc).addCallbacks(doc, Xc_Table_Page_Format[i].keyword,
			  cb_format, &param, NULL)) return FALSE;

 if((page = NEW(c_Page)(base)) == NULL) return FALSE;
 param.page = page;
 while(42)
 {
      
  if(!F(doc).addCallbacks(doc,
			  XcDF_FRAME_KEYWORD, cb_frame, page,
			  XcDF_PAGE_ZOOM_KEYWORD, cb_zoom, page,
			  XcDF_PAGE_ORIGIN_KEYWORD, cb_origin, page,
			  XcDF_BITFIELD_KEYWORD, cb_bitfield, page,
			  XcDF_GRIDLINE_KEYWORD, cb_gridline, &param,
			  XcDF_GRID_KEYWORD, cb_grid, &param,
			  XcDF_MARGINS_KEYWORD, cb_margins, &param,
			  /*___Page type___*/
			  XcDF_LEFT_KEYWORD, cb_type, &param,
			  XcDF_RIGHT_KEYWORD, cb_type, &param,
			  XcDF_DOUBLE_KEYWORD, cb_type, &param,
			  /*___Page orientation___*/
			  XcDF_PORTRAIT_KEYWORD, cb_orientation, &param,
			  XcDF_LANDSCAPE_KEYWORD, cb_orientation, &param,
			  XcDF_PAGE_SELECTED_KEYWORD, cb_selected, &param,
			  NULL)) break;

  if(F(doc).expectKeyword(doc, NULL, TRUE) == FALSE) break;

  if(param.custom_flag)
  {
   param.format->width = param.width;
   param.format->height = param.height;
  }
      
  F(page).init_Page(page,
		    (unsigned int)*page_number,
		    param.format, param.type, param.orientation,
		    param.left_margin, param.right_margin,
		    param.upper_margin, param.lower_margin,
		    param.column_count, param.row_count,
		    param.inter_column, param.inter_row, 
		    param.orig_x, param.orig_y,
		    param.delta_x, param.delta_y,
		    XPi_LOAD);
      
  doc->data_type = XcDF_PAGE_TYPE;
  doc->data_ptr = page;
      
  *page_number = (long)param.selected;
  Xc_TRACE(("done"));
  return TRUE;
 }
 Xc_TRACE(("loading error"));
 DELETE(c_Page)(page);
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_frame - Loading callback for frame loading                     ** 
** ----------------------------------------------------------------- */
static boolean cb_frame(doc, keyword, param, page)
c_DocFile *doc;
char *keyword;
long param;
c_Page *page;
{
 l_Cadre *lframe;
 c_Cadre *frame;
 error rc;

 Xc_TRACE(("cb_frame"));

 if(FUNCTION(c_Cadre, read)(doc, keyword, page->sFactor, page->BaseStd)
    == FALSE)
  return FALSE;
  
 Xc_ASSERT(doc->data_type == XcDF_CADRE_TYPE);
 doc->data_type = XcDF_NONE;
 
 frame = (c_Cadre *)doc->data_ptr;
  
 frame->Forme->Orig_X = frame->sOrig_X / page->sScale;
 frame->Forme->Orig_Y = frame->sOrig_Y / page->sScale;
  
 F(frame).calculorigine(frame);
 F(frame).calculscaling(frame);
 F(frame).calculforme(frame, CONTOUR);
 rc = F(page).add_cadre(page, frame->name, F(page).get_highest_priority(page),
			frame);
 if (rc != XC_NO_ERROR)
 {
  char *name2;
    
  name2 = F(page).copy_cadre_name(page, frame->name);
  rc = F(page).add_cadre(page, name2, 
			 F(page).get_highest_priority(page), frame);
  return FALSE;
    
 }
  
 if (frame->selected)		/* if frame selected */
 {
  lframe = page->ListCadre;
  if (!lframe) return FALSE;
    
  while(lframe->NextCadre)	/* find last frame of list */
   lframe = lframe->NextCadre;
    
  frame->selected = FALSE;
  F(page).select_cadre(page, lframe, FALSE);
    
  if (frame->verrou) page->verrou = TRUE;
 }
  
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_zoom - Loading callback for page zoom factor loading           ** 
** ----------------------------------------------------------------- */
static boolean cb_zoom(doc, keyword, param, page)
c_DocFile *doc;
char *keyword;
long param;
c_Page *page;
{
 real factor;
 Xc_TRACE(("cb_zoom(`%s')", keyword));
  
 if (!F(doc).readReal(doc, &factor)) return FALSE;
 if (factor != 0.0)
 {
  page->sFactor = SCALE_FROM_COEF(factor);
  page->sScale = (coord_t)(COORD_DEFAULT / SCALE_FROM_COEF(factor));
 }
  
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_origin - Loading callback for page origin loading              ** 
** ----------------------------------------------------------------- */
static boolean cb_origin(doc, keyword, param, page)
c_DocFile *doc;
char *keyword;
long param;
c_Page *page;
{
 real	origx, origy;
  
 Xc_TRACE(("cb_origin(`%s')", keyword));
  
 if (!F(doc).readReal(doc, &origx) || !F(doc).readReal(doc, &origy))
  return FALSE;
  
 page->sOrig_X = SCALE_FROM_POINTS(origx); 
 page->sOrig_Y = SCALE_FROM_POINTS(origy); 
  
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_bitfield - Loading callback for page bitfield loading          ** 
** ----------------------------------------------------------------- */
static boolean cb_bitfield(doc, keyword, param, page)
c_DocFile *doc;
char *keyword;
long param;
c_Page *page;
{
 Xc_TRACE(("cb_bitfield(`0x%x')", param));
  
 page->magnetic = (param & 1) ? TRUE : FALSE; param >>= 1;
 page->Frame_Border_actif = (param & 1) ? TRUE : FALSE; param >>= 1;
 page->Page_Align_Mark_actif = (param & 1) ? TRUE : FALSE; param >>= 1;
 page->Page_Typo_Mark_actif = (param & 1) ? TRUE : FALSE; param >>= 1;
 page->Page_Grid_actif = (param & 1) ? TRUE : FALSE; param >>= 1;
  
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_orientation - Loading callback for orientation                 ** 
** ----------------------------------------------------------------- */
static boolean cb_orientation(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
page_load_t *data;
{
 Xc_TRACE(("cb_orientation(`%s')", keyword));

 data->orientation = (strcmp(keyword, XcDF_LANDSCAPE_KEYWORD)?
		      Xc_PORTRAIT : Xc_LANDSCAPE);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_type - Loading callback for page type                          ** 
** ----------------------------------------------------------------- */
static boolean cb_type(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
page_load_t *data;
{
 Xc_TRACE(("cb_type(`%s')", keyword));

 if(!strcmp(keyword, XcDF_DOUBLE_KEYWORD))
  data->type = Xc_PAGE_DOUBLE;
 else if(!strcmp(keyword, XcDF_RIGHT_KEYWORD))
  data->type = Xc_PAGE_RIGHT;
 else
  data->type = Xc_PAGE_LEFT;
    
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_format - Loading callback for page format                      ** 
** ----------------------------------------------------------------- */
static boolean cb_format(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
page_load_t *data;
{
 int i;

 Xc_TRACE(("cb_format(`%s')", keyword));

 for(i = 0; i < Xc_Count_Page_Format; i++)
  if((param == Xc_Table_Page_Format[i].param) &&
     !strcmp(Xc_Table_Page_Format[i].keyword, keyword))
  {
   data->format = &Xc_Table_Page_Format[i];
   if(i == Xc_CUSTOM_FORMAT)
   {
    real w, h;

    if(!F(doc).readReal(doc, &w)|| !F(doc).readReal(doc, &h))
     return FALSE;
    data->custom_flag = TRUE;
    data->width = SCALE_FROM_POINTS(w);
    data->height = SCALE_FROM_POINTS(h);
    Xc_TRACE(("custom format: %gx%g",
	      SCALE_TO_CENTIMETERS(w), SCALE_TO_CENTIMETERS(h)));
   }
   else
   {
    Xc_TRACE(("format: `%s'", Xc_Table_Page_Format[i].name));
    data->custom_flag = FALSE;
   }

   return TRUE;
  }
 Xc_BREAK(("Format not found"));
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_gridline - Loading callback for page gridline                  ** 
** ----------------------------------------------------------------- */
static boolean cb_gridline(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
page_load_t *data;
{
 real orig_x, orig_y;
 real delta_x, delta_y;
  
 Xc_TRACE(("cb_gridline"));

 if(!F(doc).readReal(doc, &orig_x) || !F(doc).readReal(doc, &orig_y) ||
    !F(doc).readReal(doc, &delta_x) || !F(doc).readReal(doc, &delta_y))
  return FALSE;

 data->orig_x = SCALE_FROM_POINTS(orig_x);
 data->orig_y = SCALE_FROM_POINTS(orig_y);
 data->delta_x = SCALE_FROM_POINTS(delta_x);
 data->delta_y = SCALE_FROM_POINTS(delta_y);

 return F(doc).expectKeyword(doc, NULL, TRUE);
}
/* ----------------------------------------------------------------- ** 
** cb_grid - Loading callback for page grid                          ** 
** ----------------------------------------------------------------- */
static boolean cb_grid(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
page_load_t *data;
{
 real width, height;
 long rows, columns;
  
 Xc_TRACE(("cb_grid"));

 if(!F(doc).addCallbacks(doc,
			 XcDF_VLINE_KEYWORD, cb_line, data,
			 XcDF_HLINE_KEYWORD, cb_line, data,
			 NULL)) return FALSE;

 if(!F(doc).readInteger(doc, &columns)|| !F(doc).readInteger(doc, &rows) ||
    !F(doc).readReal(doc, &width)|| !F(doc).readReal(doc, &height))
  return FALSE;

 data->column_count = (int)columns;
 data->row_count = (int)rows;
 data->inter_column = SCALE_FROM_POINTS(width);
 data->inter_row = SCALE_FROM_POINTS(height);

 return F(doc).expectKeyword(doc, NULL, TRUE);
}


/* ----------------------------------------------------------------- ** 
** cb_margins - Loading callbcak for page margins                    ** 
** ----------------------------------------------------------------- */
static boolean cb_margins(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
page_load_t *data;
{
 real left, right, top, bottom;

 Xc_TRACE(("cb_margins"));

 if(!F(doc).readReal(doc, &left)|| !F(doc).readReal(doc, &right) ||
    !F(doc).readReal(doc, &top)|| !F(doc).readReal(doc, &bottom))
  return FALSE;

 data->left_margin = SCALE_FROM_POINTS(left);
 data->right_margin = SCALE_FROM_POINTS(right);
 data->upper_margin = SCALE_FROM_POINTS(top);
 data->lower_margin = SCALE_FROM_POINTS(bottom);

 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_line - Magnetic lines loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_line(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
page_load_t *data;
{
 real a, b, c;

 Xc_TRACE(("cb_line(%s)", keyword));

 if(!F(doc).readReal(doc, &a) || !F(doc).readReal(doc, &b) ||
    !F(doc).readReal(doc, &c)) return FALSE;

 Xc_TRACE(("%g %g %g", (double)a, (double)b, (double)c));
 F(data->page).add_page_align_mark(data->page,
				   ((*keyword == 'V')?
				    ALIGN_MARK_ABS_VERTICAL :
				    ALIGN_MARK_ABS_HORIZONTAL),
				   SCALE_FROM_POINTS(a),
				   SCALE_FROM_POINTS(b),
				   SCALE_FROM_POINTS(c));
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_selected - Page selected loading callback                      ** 
** ----------------------------------------------------------------- */
static boolean cb_selected(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
page_load_t *data;
{
 Xc_TRACE(("cb_selected(%s)", keyword));

 data->selected = TRUE;
  
 return TRUE;
}
