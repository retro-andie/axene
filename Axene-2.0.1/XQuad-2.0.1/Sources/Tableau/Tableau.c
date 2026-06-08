/*
** Tableau.c for XQuad in Tableau/
** Methods for the Tableau class
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
** Started on  Fri Sep 30 15:33:47 1994 Antoine Buat
** Last update Wed Feb  4 00:59:20 1998 Emmanuel Paris
*/

#include "TableauP.h"
#include "BG_ManageBase.h"
#include "Colormap.h"
#include "version.h"
#include "osname.h"
#include <time.h>

extern c_Colormap *GlobColormap;

/*--- Prototypes ---*/
static void *cons_Tableau();
static void dest_Tableau();
static void *copy_Tableau();
static void resize_Tableau ___NPROTO((c_Tableau *This, XEvent *Event,
				      boolean Maximized));
static void eventlook_Tableau ___NPROTO((c_Tableau *This, XEvent *Event,
					 listmanaged *listegwm,
					 boolean DocSelected));
static void set_function_Tableau ___PROTO((c_Tableau *This, int function));

static void init_Tableau ___PROTO((c_Tableau *This, Display *display,
				   Window window, boolean visible, 
				   char *name));
sf_Tableau fc_Tableau =
{
 cons_Tableau, 
 dest_Tableau,
 copy_Tableau,
 resize_Tableau,
 eventlook_Tableau,
 set_function_Tableau,
 readTableau, writeTableau,
 printTableau,
 init_Tableau,
};


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *cons_Tableau()
{
 c_Tableau 	*ObjTmp;
 c_Color   	*color;
 c_FormatNbr	*format;
 c_VectorFont	*vfont;
 c_Color	*black;
 c_TextStyle	*style;

 Xc_HISTORY(("constructor"));

 if((ObjTmp = Xc_malloc("Tableau", sizeof(c_Tableau))) == NULL) return NULL;
 memset(ObjTmp, 0, sizeof(c_Tableau));

 ObjTmp->f = &fc_Tableau;

 ObjTmp->scrollbh_on = FALSE;
 ObjTmp->scrollbv_on = FALSE;
 ObjTmp->modified = FALSE;
 ObjTmp->filename = NULL;
 ObjTmp->PosX = DTAB_POSX;
 ObjTmp->PosY = DTAB_POSY;
 ObjTmp->Width = DTAB_SIZEX;
 ObjTmp->Height = DTAB_SIZEY;

 {
  time_t now = time(NULL);
  char	*lastcr;

  sprintf(ObjTmp->version, "%d.%d.%d", MAJOR, MINOR, PL);
  sprintf(ObjTmp->osname, "%.30s", Xc_SYSTEM_NAME);
  sprintf(ObjTmp->started_on_date, "%.30s", ctime(&now));
  if ((lastcr = strrchr(ObjTmp->started_on_date, '\n')) != NULL)
   *lastcr = '\0';

  strcpy(ObjTmp->last_update_date, ObjTmp->started_on_date);
  strncpy(ObjTmp->creator, Xc_getgecos, XcMAX_NAME_LENGTH);
 }
 
 /*--- Copy color base ---*/
 ObjTmp->BaseStd.color_base = NULL;
 for(color = GlobColorBase; color != NULL; color = color->next)
  if(COPY(c_Color)(color, &ObjTmp->BaseStd.color_base) == NULL) break;

 /*--- Copy Format base ---*/
 ObjTmp->BaseStd.format_base = NULL;
 for(format = GlobFormatBase; format != NULL; format = format->next)
  if(COPY(c_FormatNbr)(format, &ObjTmp->BaseStd) == NULL) break;

 /*--- Copy TextStyle base ---*/
 ObjTmp->BaseStd.text_style_base = NULL;
 for(style = GlobTextStyleBase; style != NULL; style = style->next)
  F(style).virtualCopy(style, &ObjTmp->BaseStd);
    
 ObjTmp->BaseStd.cell_style_base = NULL;

 /*--- Default text style ---*/
 if(ObjTmp->BaseStd.text_style_base == NULL)
 {
  black = ObjTmp->BaseStd.color_base;
  black = F(black).getColor(&(ObjTmp->BaseStd.color_base),
			    XcC_GRAY, XcC_SCALE_COLOR(0.0),
			    XcC_TRANSPARENCY, 0,
			    XcC_END);
  if (!black) black = ObjTmp->BaseStd.color_base;
  vfont = F(GlobFontBase).getFontByName(GlobFontBase, "Helvetica");
  if(!vfont) vfont = GlobFontBase;
  (void)NEW(c_TextStyle)(&ObjTmp->BaseStd.text_style_base, 
			 "Style", vfont,
			 SCALE_FROM_POINTS(12),
			 black);
 }

 ObjTmp->BaseStd.cell_style_base = NEW(c_CellStyle)(&ObjTmp->BaseStd);
  
 Create_BG_Color_Base(&ObjTmp->BaseStd); 
 Create_BG_TextStyle_Base(&ObjTmp->BaseStd); 
  
 ObjTmp->Feuille = NEW(c_Feuille)(&ObjTmp->BaseStd, &(ObjTmp->modified));
 ObjTmp->moteur = NEW(c_Moteur)(&ObjTmp->BaseStd);
 ObjTmp->Feuille->moteur = ObjTmp->moteur;

 return ObjTmp;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void dest_Tableau(this)
c_Tableau *this;
{
 Xc_HISTORY(("destructor"));
  
 DELETE(c_Feuille)(this->Feuille);
 DELETE(c_Moteur)(this->moteur);
  
 /*--- Destroy Basic Graph bases ----*/
 Destroy_BG_TextStyle_Base(&this->BaseStd);
 Destroy_BG_Color_Base(&this->BaseStd);
  
 /*--- Format CellStyle base ---*/
 Xc_TRACE(("delete CellStyle base"));
 if (this->BaseStd.cell_style_base)
  DELETE(c_CellStyle)(this->BaseStd.cell_style_base);

 /*--- Destroy format base ---*/
 Xc_TRACE(("delete format base"));
 while(this->BaseStd.format_base != NULL)
  DELETE(c_FormatNbr)(this->BaseStd.format_base);
  
 /*--- Destroy the document text style list ---*/
 Xc_TRACE(("delete TextStyle base"));
 while(this->BaseStd.text_style_base != NULL)
  DELETE(c_TextStyle)(this->BaseStd.text_style_base);
  
 /*--- Destroy the document color database ---*/
 Xc_TRACE(("delete color base"));
 while(this->BaseStd.color_base != NULL)
  DELETE(c_Color)(this->BaseStd.color_base);
  
 if (this->scrollbh_on)
  DELETE(c_ScrollBarX)(this->ScrollBarV);
 if (this->scrollbv_on)
  DELETE(c_ScrollBarX)(this->ScrollBarH);
 /*  DELETE(c_Pager)(this->Pager); */

 if(this->filename != NULL) Xc_free(this->filename);

 Xc_free(this);
}


/* ----------------------------------------------------------------- ** 
** Copier                                                            ** 
** ----------------------------------------------------------------- */
static void *copy_Tableau(This)
c_Tableau *This;
{
 c_Tableau *ObjTmp;

 Xc_HISTORY(("copy"));

 if((ObjTmp = Xc_malloc("CTableau", sizeof(c_Tableau))) == NULL)
  return NULL;

 memcpy(ObjTmp, This, sizeof(c_Tableau));
 return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** Init_Tableau                                                      ** 
** ----------------------------------------------------------------- */
static void init_Tableau(This,display,window,visible,name)
c_Tableau	*This;
Display		*display;
Window		window;
boolean		visible;
char		*name;
{
 color_cells_t	realcolor;
 long		doc_color;
  
 This->display = display;
 This->visible = visible;
  
 F(GlobColormap).allocate_named_color(GlobColormap, DTAB_COLOR, &realcolor);
 doc_color = realcolor.pixel; 
  
 This->window = 
  XCreateSimpleWindow(This->display, window,
		      This->PosX, This->PosY, This->Width, This->Height,
		      0, doc_color,doc_color);
 XSelectInput(This->display, This->window, StructureNotifyMask |
	      ButtonPressMask);
 if(name != NULL) strncpy(This->name, name, XcMAX_NAME_LENGTH - 1);
  
 /**************************/
 /* Creation de la feuille */
 /**************************/
  
 /*This->Feuille = NEW(c_Feuille)(&This->BaseStd);*/
  
 F(This->BaseStd.cell_style_base).
  init(This->BaseStd.cell_style_base, This->display, This->window, 
       (void *)This->moteur, F(This->Feuille).HookCellStyleCallback,
       This->Feuille);
  
 /***********************/
 /* Creation Scroll Bar */
 /***********************/
 This->ScrollBarV = (c_ScrollBarX *)NEW(c_ScrollBarX)
  (This->display,
   This->window,
   This->Width - SCROLLB_SIZE, 0,
   This->Height - SCROLLB_SIZE,
   SBVERTICAL,
   0);				/* 1); */
 This->ScrollBarH = (c_ScrollBarX *)NEW(c_ScrollBarX)
  (This->display,
   This->window,
   0, This->Height - SCROLLB_SIZE,/* - PAGER_SIZE, */
   This->Width,
   SBHORIZONTAL,
   0);
 This->scrollbh_on = TRUE;
 This->scrollbv_on = TRUE;

 /******************/
 /* Creation Pager */
 /******************/
 /*  This->Pager = (c_Pager *)NEW(c_Pager)
     (This->display,
     This->window,
     0, This->Height - PAGER_SIZE,
     This->Width - SCROLLB_SIZE,
     PAGER_HORIZONTAL); */
  
 F(This->Feuille).init_Feuille
  (This->Feuille, This->display, This->window, TRUE, 0, 0,
   This->Width - SCROLLB_SIZE - 1, 
   This->Height - SCROLLB_SIZE	/*-PAGER_SIZE*/ - 1,
   This->moteur, This->ScrollBarV,
   This->ScrollBarH, NULL	/* This->Pager */ );
}

static void resize_Tableau(This, Event, Maximized)
c_Tableau	*This;
XEvent	*Event;
boolean	Maximized;
{
 This->PosX = Event->xconfigure.x;
 This->PosY = Event->xconfigure.y;
 This->Width = Event->xconfigure.width;
 This->Height = Event->xconfigure.height;
  
 /**********************/
 /* modifie la feuille */
 /**********************/
 F(This->Feuille).Resize(This->Feuille,0,0,
			 Event->xconfigure.width - SCROLLB_SIZE - 1,
			 Event->xconfigure.height - SCROLLB_SIZE 
			 /* - PAGER_SIZE */ - 1);
 /**********************/
 /* modifie le pager   */
 /**********************/
 /*  F(This->Pager).Resize
     (This->Pager,
     0,
     Event->xconfigure.height - PAGER_SIZE ,
     Event->xconfigure.width  - SCROLLB_SIZE); */
 /**************************/
 /* modifie les ascenceurs */
 /**************************/
 if (Maximized)
  F(This->ScrollBarV).Resize
   (This->ScrollBarV, 
    Event->xconfigure.width - SCROLLB_SIZE, SCROLLB_SIZE+4,
    Event->xconfigure.height - 2*SCROLLB_SIZE -4 , 0);/* 1); */
 else
  F(This->ScrollBarV).Resize
   (This->ScrollBarV, 
    Event->xconfigure.width - SCROLLB_SIZE, 0,
    Event->xconfigure.height - SCROLLB_SIZE, 0);/* 1); */
 F(This->ScrollBarH).Resize
  (This->ScrollBarH, 0,
   Event->xconfigure.height - SCROLLB_SIZE/*- PAGER_SIZE */,
   Event->xconfigure.width, 0);
}

static void eventlook_Tableau(This, Event, listegwm, DocSelected)
c_Tableau	*This;
XEvent		*Event;
listmanaged	*listegwm;
boolean		DocSelected;
{ 
 status_t	function_return;
  
 if (DocSelected || 
     (Event->type == Expose) ||
     (Event->type == GraphicsExpose))
 {
  /* ----------------------------------------------------------------- **
  ** analysing scrollbar                                               **
  ** ----------------------------------------------------------------- */
  if (This->scrollbv_on)
   if (F(This->ScrollBarV).EventLook(This->ScrollBarV, Event))
    return;
  
  if (This->scrollbh_on)
   F(This->ScrollBarH).EventLook(This->ScrollBarH, Event);
    
  /* ----------------------------------------------------------------- **
  ** analysing Pager                                                   **
  ** ----------------------------------------------------------------- */
  /*    F(This->Pager).EventLook(This->Pager,
	Event->xany.display, Event); */

  /* ----------------------------------------------------------------- **
  ** analysing feuille                                                 **
  ** ----------------------------------------------------------------- */
    
  function_return = F(This->Feuille).EventLook(This->Feuille, Event);
  This->modified |= (function_return == FUNCTION_FINISHED) | 
   (function_return == FUNCTION_MODIFIED);
  if (function_return != FUNCTION_NOT_FINISHED && 
      function_return != FUNCTION_MODIFIED)
  {
   switch(This->Feuille->function_type)
   {
   case SELECT_MOVE_RESIZE_CELL:
    ABORT(This->Feuille);
    INIT(This->Feuille);   
    break;
   case GRAPH_HISTO_V:
   case GRAPH_HISTO_H:
   case GRAPH_CURVE:
   case GRAPH_SURFACE:
   case GRAPH_SECTOR:
   case GRAPH_RADAR:
    if (function_return == FUNCTION_REINIT)
     F(This->Feuille).SetFunction(This->Feuille,
				  SELECT_MOVE_RESIZE_CELL);
    else
    {
     ABORT(This->Feuille);
     INIT(This->Feuille); 
    }
    break;
   case FRMG_FUNCTION:
    if (This->Feuille->function_data)
    {
     switch(*((int *)This->Feuille->function_data))
     {
     case CREATE_CADRE_RECTANGLE:
      if (function_return == FUNCTION_REINIT)
       F(This->Feuille).SetFunction(This->Feuille,
				    SELECT_MOVE_RESIZE_CELL);
      else
      {
       ABORT(This->Feuille);
       INIT(This->Feuille); 
      }
     }
    }
    break;
   }
  }
 }
 if (!DocSelected)
 {
  /* ----------------------------------------------------------------- ** 
  ** click dans la fenetre                                             ** 
  ** ----------------------------------------------------------------- */
  if (Event->type == ButtonPress)
  {
   if ((Event->xany.window == This->window) ||
       (Event->xany.window == This->Feuille->X_info.window) ||
       (Event->xany.window == This->Feuille->X_info_O.window) ||
       (Event->xany.window == This->Feuille->X_info_C.window) ||
       (Event->xany.window == This->Feuille->X_info_R.window) ||
       (Event->xany.window == This->ScrollBarH->window) ||
       (Event->xany.window == This->ScrollBarV->window) )/* || */
    /* (Event->xany.window == This->Pager->window)) */
   {
    F(Gwm).DiselectWindow(Gwm, Event->xany.display, listegwm);
   }
  }
 }
}

static void set_function_Tableau(This, function)
     c_Tableau	*This;
     int	function;
{
 This->modified |= F(This->Feuille).SetFunction(This->Feuille, function);
 return;
}
