/*
** Document.c for XAllWrite in Document/
** Methods for the Document class
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Fri Mar  7 18:17:47 1997 Emmanuel Paris
** Last update Thu Oct 15 18:52:08 1998 Emmanuel Paris
*/

#include "DocumentP.h"
#include "Colormap.h"
#include "MainInterface.h"
#include "Undo.h"
#include "Resource.h"
#include "version.h"
#include "osname.h"
#include <time.h>

extern c_MainInterface	*MainInterface1;
extern c_Color *GlobColorBase;
extern c_TextRuler *GlobTextRulerBase;
extern c_TextStyle *GlobTextStyleBase;
extern c_Colormap *GlobColormap;
extern c_VectorFont *GlobFontBase;

/*--- Prototypes ---*/
static void *cons_Document();
static void dest_Document();
static void *copy_Document();
static void resize_Document ___NPROTO((c_Document *This, XEvent *Event,
				       boolean Maximized));
static void eventlook_Document ___NPROTO((c_Document *This, XEvent *Event,
					  listmanaged *listegwm,
					  boolean DocSelected));
static void set_function_Document ___PROTO((c_Document *This, int function));

static void init_Document ___PROTO((c_Document *This, Display *display,
				    Window window, boolean visible, 
				    char *name));
static void update_Document ___PROTO((c_Document *This));
static void map_scrollbar_Document ___NPROTO((c_Document *This,
					      boolean scrollbH,
					      boolean scrollbV));

sf_Document fc_Document =
{
 cons_Document, 
 dest_Document,
 copy_Document,
 resize_Document,
 eventlook_Document,
 set_function_Document,
 readDocument, writeDocument,
 printDocument,
 init_Document,
 update_Document
};

static pagearea_cb_t pagearea_cb = 
{
 NULL,
 map_scrollbar_Document
};

/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *cons_Document()
{
 c_Document 	*This;
 c_TextRuler *ruler;
 c_TextStyle *style;
 c_Color *color;
 
 Xc_HISTORY(("constructor"));
 
 if((This = Xc_malloc("Document", sizeof(c_Document))) == NULL) return NULL;
 memset(This, 0, sizeof(c_Document));
 
 This->f = &fc_Document;
 
 This->modified = FALSE;
 This->filename = NULL;
 This->scrollbh_on = FALSE;
 This->scrollbv_on = FALSE;
 This->PosX = DTAB_POSX;
 This->PosY = DTAB_POSY;
 This->Width = DTAB_SIZEX;
 This->Height = DTAB_SIZEY;
 This->Maximized = FALSE;

 {
  time_t now = time(NULL);
  char	*lastcr;

  sprintf(This->version, "%d.%d.%d", MAJOR, MINOR, PL);
  sprintf(This->osname, "%.30s", Xc_SYSTEM_NAME);
  sprintf(This->started_on_date, "%.30s", ctime(&now));
  if ((lastcr = strrchr(This->started_on_date, '\n')) != NULL)
   *lastcr = '\0';

  strcpy(This->last_update_date, This->started_on_date);
  strncpy(This->creator, Xc_getgecos, XcMAX_NAME_LENGTH);
 }

 /*--- Copy global color base ---*/
 This->BaseStd.color_base = NULL;
 for(color = GlobColorBase; color != NULL; color = color->next)
  COPY(c_Color)(color, &This->BaseStd.color_base);
 
 /*--- Copy global text ruler base ---*/
 This->BaseStd.text_ruler_base = NULL;
 for(ruler = GlobTextRulerBase; ruler != NULL; ruler = ruler->next)
  F(ruler).virtualCopy(ruler, &This->BaseStd);
 
 /*--- Copy global text style base ---*/
 This->BaseStd.text_style_base = NULL;
 for(style = GlobTextStyleBase; style != NULL; style = style->next)
  F(style).virtualCopy(style, &This->BaseStd);
 
 This->BaseStd.text_base = (c_Text *)NEW(c_Text)(&This->BaseStd, Xa_BODY);

 /*--- temporaire ---*/
 if(This->BaseStd.text_ruler_base == NULL)
  NEW(c_TextRuler)(&This->BaseStd.text_ruler_base, "ruler");
 
 if(This->BaseStd.text_style_base == NULL)
 {
  c_VectorFont *font;
  c_Color *color;
  
  /*--- Get font ---*/
  if(GlobFontBase == NULL)
  {
   DELETE(c_Document)(This);
   return NULL;
  }
  font = F(GlobFontBase).getFontByName(GlobFontBase, "Courier");
  if(font == NULL) font = GlobFontBase;
  
  color = F(This->BaseStd.color_base).getColor
   (&(This->BaseStd.color_base), XcC_NAME,"Black",
    XcC_GRAY, XcC_SCALE_COLOR(0.0), XcC_TRANSPARENCY,0, XcC_END);
  
  NEW(c_TextStyle)(&This->BaseStd.text_style_base, "style",
		   font, SCALE_FROM_POINTS(12), color);
 }
 /*--- Temporaire ---*/

 pagearea_cb.cbData = (void *)This;
 This->PageArea = (c_PageArea *)NEW(c_PageArea)(&This->BaseStd, &pagearea_cb);
 
 return This;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void dest_Document(this)
c_Document *this;
{
 Xc_HISTORY(("destructor"));
  
 DELETE(c_PageArea)(this->PageArea);

 /*--- Destroy the document text base --- */
 while(this->BaseStd.text_base != NULL)
  DELETE(c_Text)(this->BaseStd.text_base);

 /*--- Destroy the document text ruler list ---*/
 while(this->BaseStd.text_ruler_base != NULL)
  DELETE(c_TextRuler)(this->BaseStd.text_ruler_base);
  
 /*--- Destroy the document text style list ---*/
 while(this->BaseStd.text_style_base != NULL)
  DELETE(c_TextStyle)(this->BaseStd.text_style_base);
  
 /*--- Destroy the document color database ---*/
 while(this->BaseStd.color_base != NULL)
  DELETE(c_Color)(this->BaseStd.color_base);

 if (this->Pager)
  DELETE(c_Pager)(this->Pager);

 if (this->RegleV)
  DELETE(c_XaRegle)(this->RegleV);
 if (this->RegleH)
  DELETE(c_XaRegle)(this->RegleH);

 if (this->ScrollBarV)
  DELETE(c_ScrollBarX)(this->ScrollBarV);
 if (this->ScrollBarH)
  DELETE(c_ScrollBarX)(this->ScrollBarH);

 if (this->gc != (GC)0)
  XFreeGC(this->display, this->gc);
 if (this->window != (Window)0)
  XDestroyWindow(this->display, this->window);
 
 if(this->filename != NULL) Xc_free(this->filename);
 Xc_free(this);
}


/* ----------------------------------------------------------------- ** 
** Copier                                                            ** 
** ----------------------------------------------------------------- */
static void *copy_Document(This)
c_Document *This;
{
 c_Document *ObjTmp;
 
 Xc_HISTORY(("copy"));
 
 if((ObjTmp = Xc_malloc("CDocument", sizeof(c_Document))) == NULL)
  return NULL;
 
 memcpy(ObjTmp, This, sizeof(c_Document));
 return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** Init_Document                                                     ** 
** ----------------------------------------------------------------- */
static void init_Document(This, display, window, visible, name)
c_Document	*This;
Display		*display;
Window		window;
boolean		visible;
char		*name;
{
 color_cells_t	realcolor;
 long		doc_color;
 XGCValues	xgcv;
 
 This->display = display;
 
 F(GlobColormap).allocate_named_color(GlobColormap, DTAB_COLOR, &realcolor);
 doc_color = realcolor.pixel; 
 
 This->window = 
  XCreateSimpleWindow(This->display, window,
		      This->PosX, This->PosY, This->Width, This->Height,
		      0, doc_color,doc_color);
 XSelectInput(This->display, This->window, StructureNotifyMask |
	      ButtonPressMask);
 if(name != NULL) strncpy(This->name, name, XcMAX_NAME_LENGTH - 1);
 
 /*--- Creation Scroll Bar ---*/
 This->ScrollBarV = (c_ScrollBarX *)NEW(c_ScrollBarX)
  (This->display, This->window,
   This->Width - SCROLLB_SIZE, REGLE_SIZE,
   This->Height - REGLE_SIZE,
   This->Height - REGLE_SIZE - SCROLLB_SIZE - PAGER_SIZE,
   SBVERTICAL,
   1);
 This->ScrollBarH = (c_ScrollBarX *)NEW(c_ScrollBarX)
  (This->display, This->window,
   REGLE_SIZE, This->Height - SCROLLB_SIZE - PAGER_SIZE,
   This->Width - REGLE_SIZE,
   This->Width - REGLE_SIZE - SCROLLB_SIZE,
   SBHORIZONTAL,
   0);
 This->scrollbh_on = TRUE;
 This->scrollbv_on = TRUE;

 F(This->ScrollBarV).Change_Rsize(This->ScrollBarV, This->display, 500, 500);
 F(This->ScrollBarH).Change_Rsize(This->ScrollBarH, This->display, 500, 500);

 /*--- Creation Pager ---*/
 This->Pager = (c_Pager *)NEW(c_Pager)
  (This->display, This->window, REGLE_SIZE, This->Height - PAGER_SIZE,
   This->Width - REGLE_SIZE - SCROLLB_SIZE, This);

 This->pager_on = TRUE;

 /*--- Creation Regles ---*/
 This->RegleH = (c_XaRegle *)NEW(c_XaRegle)
  (This->display, This->window, 0, 0,
   This->Width, XaRegle_HORIZONTAL);
 This->RegleV = (c_XaRegle *)NEW(c_XaRegle)
  (This->display, This->window, 0, REGLE_SIZE - 1,
   This->Height - REGLE_SIZE + 1, XaRegle_VERTICAL);
 This->regle_on	= TRUE;
 
 F(This->PageArea).Init
  (This->PageArea, This->display, This->window,
   This->ScrollBarH, This->ScrollBarV, REGLE_SIZE, REGLE_SIZE,
   This->Width - REGLE_SIZE - SCROLLB_SIZE,
   This->Height - REGLE_SIZE - SCROLLB_SIZE - PAGER_SIZE, 
   This->Pager, This->RegleV, This->RegleH);

 XRaiseWindow(This->display, This->ScrollBarH->window);
 F(GlobColormap).allocate_named_color(GlobColormap, DTAB_SHADOW, &realcolor);
 xgcv.foreground = realcolor.pixel; 

 This->gc = XCreateGC(display, This->window, GCForeground, &xgcv);
}

static void update_Document(This)
c_Document *This;
{
 F(This->PageArea).Update(This->PageArea);
 map_scrollbar_Document(This, !This->scrollbh_on, !This->scrollbv_on);
}

static void resize_Document(This, Event, Maximized)
c_Document	*This;
XEvent	*Event;
boolean	Maximized;
{
 /*This->PosX = Event->xconfigure.x;*/
 /*This->PosY = Event->xconfigure.y;*/
 This->Width = Event->xconfigure.width;
 This->Height = Event->xconfigure.height;
 This->Maximized = Maximized;

 /*--- Resize Scrollbars ---*/
 if (Maximized && !This->regle_on)
  F(This->ScrollBarV).Resize
   (This->ScrollBarV, This->Width - SCROLLB_SIZE, BUTTON_SIZE, 
    This->Height - SCROLLB_SIZE * This->scrollbh_on - 
    PAGER_SIZE * This->pager_on, This->Height - BUTTON_SIZE,
    This->scrollbh_on * This->pager_on, 0, 0, TRUE);
 else
  F(This->ScrollBarV).Resize
   (This->ScrollBarV,  This->Width - SCROLLB_SIZE, This->regle_on * REGLE_SIZE,
    This->Height - REGLE_SIZE * This->regle_on - 
    SCROLLB_SIZE * This->scrollbh_on - PAGER_SIZE * This->pager_on,
    This->Height - This->regle_on * REGLE_SIZE,
    This->scrollbh_on * This->pager_on, 0, 0, TRUE);
 
 F(This->ScrollBarH).Resize
  (This->ScrollBarH, This->regle_on * REGLE_SIZE,
   This->Height - SCROLLB_SIZE - PAGER_SIZE * This->pager_on,
   This->Width - This->regle_on * REGLE_SIZE - SCROLLB_SIZE *This->scrollbv_on,
   This->Width - This->regle_on * REGLE_SIZE - 
   SCROLLB_SIZE * (This->scrollbv_on && !This->pager_on),
   0, 0, TRUE);

 /*--- Resize Pager ---*/
 F(This->Pager).Resize
  (This->Pager, This->regle_on * REGLE_SIZE, This->Height - PAGER_SIZE,
   This->Width - This->regle_on * REGLE_SIZE - 
   SCROLLB_SIZE *This->scrollbv_on);
  
 /*--- Resize XaRegle ---*/
 if (This->regle_on)
 {
  F(This->RegleH).Resize
   (This->RegleH, This->Width);
  F(This->RegleV).Resize
   (This->RegleV, This->Height - REGLE_SIZE + 1);
 }

 /*--- Resize PageArea ---*/
 F(This->PageArea).Resize
  (This->PageArea, This->regle_on * REGLE_SIZE, This->regle_on * REGLE_SIZE,
   This->Width - This->regle_on * REGLE_SIZE - 
   This->scrollbv_on * SCROLLB_SIZE,
   This->Height - This->regle_on * REGLE_SIZE - 
   This->scrollbh_on * SCROLLB_SIZE - PAGER_SIZE * This->pager_on);
}

static void eventlook_Document(This, Event, listegwm, DocSelected)
c_Document	*This;
XEvent		*Event;
listmanaged	*listegwm;
boolean		DocSelected;
{ 
 status_t	function_return;
 int		num_func;

 /* ----------------------------------------------------------------- ** 
 ** Drag and drop function                                            ** 
 ** ----------------------------------------------------------------- */
 if ((Event->type == ClientMessage) && 
     F(This->PageArea).IsInSubWindow(This->PageArea, Event->xany.window) &&
     ((DROP_FLAGS(*Event) & 0xffff00)
      == DD_XCLAMATION_INDENTITY))
 {		
  if ((DROP_FLAGS(*Event) & 0xf) == DD_DROP_FOR_TARGET)
  {
   F(Gwm).DiselectWindow(Gwm, Event->xany.display, listegwm);
  }
  return;
 }		

 if (Event->type == ConfigureNotify)
 {
  This->PosX = Event->xconfigure.x;
  This->PosY = Event->xconfigure.y;
  return;
 }

 if (DocSelected || Event->type == Expose || Event->type == GraphicsExpose)
 {
  if (Event->xexpose.x < REGLE_SIZE || Event->xexpose.y < REGLE_SIZE)
  {
   XPoint point[3];
   
   point[0].x = point[1].x = REGLE_SIZE - 1; point[2].x = This->Width;
   point[0].y = This->Height; point[1].y = point[2].y = REGLE_SIZE - 1;
   XDrawLines(This->display, This->window, This->gc,
	      point, 3, CoordModeOrigin);
  }
  
   /*--- analysing scrollbar ---*/
  if (This->scrollbv_on)
   if (F(This->ScrollBarV).EventLook(This->ScrollBarV, Event->xany.display,
				     Event, This->window, 
				     This->PageArea->AreaOrig_X, 
				     This->PageArea->AreaOrig_Y))
    return;
  
  if (This->scrollbh_on)
   F(This->ScrollBarH).EventLook(This->ScrollBarH, Event->xany.display,
				 Event, This->window, 
				 This->PageArea->AreaOrig_X, 
				 This->PageArea->AreaOrig_Y);
  
  /*--- analysing Regles ---*/
  if (This->regle_on)
  {
   if (F(This->RegleV).EventLook(This->RegleV, Event))
    return;
   
   if (F(This->RegleH).EventLook(This->RegleH, Event))
    return;
  }
 
  /*--- analysing Pager ---*/
  if (This->pager_on)
   F(This->Pager).EventLook(This->Pager, Event);

  /*--- analysing PageArea ---*/
  function_return = F(This->PageArea).EventLook(This->PageArea, Event);
  This->modified |= (function_return == FUNCTION_FINISHED) | 
   (function_return == FUNCTION_MODIFIED);
  if (function_return != FUNCTION_NOT_FINISHED && 
      function_return != FUNCTION_MODIFIED)
  {
   switch(This->PageArea->function_type)
   {
   case ZOOM_PLUS_PAGE:
   case ZOOM_MINUS_PAGE:
   case ZOOM_IN_ZONE:
    F(This->PageArea).SetFunction(This->PageArea, SELECT_MOVE_EDIT_TEXT);
    break;
   default:
    if (This->PageArea->ActivePage->function_data)
    {
     switch(This->PageArea->ActivePage->function_type)
     {
     case SELECT_MOVE_EDIT_TEXT:
      ABORT(This->PageArea);
      INIT(This->PageArea); 
      break;
     default:
      num_func = This->PageArea->ActivePage->function_type;
      if (num_func > FRMG_FUNCTION_BEGIN && num_func < FRMG_FUNCTION_END)
      {
       switch(num_func)
       {
       case MOVE_POINT_MODE:
       case RESIZE_CADRE_MODE:
	ABORT(This->PageArea);
	INIT(This->PageArea); 
	break;
       case CREATE_CADRE_RECTANGLE:
       case CREATE_CADRE_ELIPSE:
       case CREATE_CADRE_POLYGONAL:
       case INSERT_POINT:
       case DELETE_POINT:
       case MOVE_IMAGE:
       case MOVE_VECTOR:
	if (function_return == FUNCTION_REINIT)
	 F(This->PageArea).SetFunction(This->PageArea, SELECT_MOVE_EDIT_TEXT);
	else
	{
	 ABORT(This->PageArea);
	 INIT(This->PageArea); 
	}
       }
      }
     }
    }
    break;
   }
  }
 }
 
 if (!DocSelected)
 {
  /*--- click dans la fenetre ---*/
  if (Event->type == ButtonPress)
  {
   if ((Event->xany.window == This->window) ||
       (Event->xany.window == This->ScrollBarH->window) ||
       (Event->xany.window == This->ScrollBarV->window) ||
       (Event->xany.window == This->Pager->window) ||
       (Event->xany.window == This->RegleV->window) ||
       (Event->xany.window == This->RegleH->window) ||
       (Event->xany.window == This->PageArea->window) ||
       F(This->PageArea).IsInSubWindow(This->PageArea, Event->xany.window))
   {
    F(Gwm).DiselectWindow(Gwm, Event->xany.display, listegwm);
   }
  }
 }
}

static void set_function_Document(This, function)
c_Document	*This;
int	function;
{
 This->modified |= F(This->PageArea).SetFunction(This->PageArea, function);
 return;
}

static void map_scrollbar_Document(This, scrollbH, scrollbV)
c_Document *This;
boolean scrollbH;
boolean scrollbV;
{
 int	paNewWidth, paNewHeight;
 
 if (scrollbH == This->scrollbh_on && scrollbV == This->scrollbv_on)
  return;

 paNewWidth = This->PageArea->Width;
 paNewHeight = This->PageArea->Height;

 if (scrollbH != This->scrollbh_on)
 {
  if (scrollbH)			/* map scrollbarH */
  {
   paNewHeight -= SCROLLB_SIZE;
   XMapWindow(This->display, This->ScrollBarH->window);
   if (scrollbV)
   {
    if (This->Maximized && !This->regle_on)
     F(This->ScrollBarV).Resize
      (This->ScrollBarV, This->Width - SCROLLB_SIZE, BUTTON_SIZE, 
       This->Height - SCROLLB_SIZE - 
       PAGER_SIZE * This->pager_on, This->Height - BUTTON_SIZE,
       This->pager_on, 0, 0, TRUE);
    else
     F(This->ScrollBarV).Resize
      (This->ScrollBarV,  This->Width - SCROLLB_SIZE,
       This->regle_on * REGLE_SIZE,
       This->Height - REGLE_SIZE * This->regle_on - 
       SCROLLB_SIZE - PAGER_SIZE * This->pager_on,
       This->Height - This->regle_on * REGLE_SIZE,
       This->pager_on, 0, 0, TRUE);
   }
  }
  else				/* unmap  scrollbar H */
  {
   paNewHeight += SCROLLB_SIZE;
   XUnmapWindow(This->display, This->ScrollBarH->window);
   if (scrollbV)
   {
    if (This->Maximized && !This->regle_on)
     F(This->ScrollBarV).Resize
      (This->ScrollBarV, This->Width - SCROLLB_SIZE, BUTTON_SIZE, 
       This->Height - PAGER_SIZE * This->pager_on, This->Height - BUTTON_SIZE,
       0, 0, 0, TRUE);
    else
     F(This->ScrollBarV).Resize
      (This->ScrollBarV,  This->Width - SCROLLB_SIZE,
       This->regle_on * REGLE_SIZE, This->Height - 
       REGLE_SIZE * This->regle_on - PAGER_SIZE * This->pager_on,
       This->Height - This->regle_on * REGLE_SIZE,
       0, 0, 0, TRUE);
   }
  }
  
  This->scrollbh_on = scrollbH;
 }

 if (scrollbV != This->scrollbv_on)
 {
  if (scrollbV)			/* map scrollbarV */
  {
   paNewWidth -= SCROLLB_SIZE;
   XMapWindow(This->display, This->ScrollBarV->window);
   if (This->scrollbh_on)
   {
    F(This->ScrollBarH).Resize
     (This->ScrollBarH, This->regle_on * REGLE_SIZE,
      This->Height - SCROLLB_SIZE - PAGER_SIZE * This->pager_on,
      This->Width - This->regle_on * REGLE_SIZE - SCROLLB_SIZE,
      This->Width - This->regle_on * REGLE_SIZE - 
      SCROLLB_SIZE * (!This->pager_on),
      0, 0, TRUE);
   }
  }
  else				/* unmap  scrollbar V */
  {
   paNewWidth += SCROLLB_SIZE;
   XUnmapWindow(This->display, This->ScrollBarV->window);
   if (This->scrollbh_on)
   {
    F(This->ScrollBarH).Resize
     (This->ScrollBarH, This->regle_on * REGLE_SIZE,
      This->Height - SCROLLB_SIZE - PAGER_SIZE * This->pager_on,
      This->Width - This->regle_on * REGLE_SIZE,
      This->Width - This->regle_on * REGLE_SIZE,
      0, 0, TRUE);
   }
  }
  This->scrollbv_on = scrollbV;
 }

 F(This->Pager).Resize
  (This->Pager, This->regle_on * REGLE_SIZE, This->Height - PAGER_SIZE,
   This->Width - This->regle_on * REGLE_SIZE - 
   SCROLLB_SIZE *This->scrollbv_on);

 F(This->PageArea).Resize
  (This->PageArea, This->regle_on * REGLE_SIZE, This->regle_on * REGLE_SIZE,
   paNewWidth, paNewHeight);
}










