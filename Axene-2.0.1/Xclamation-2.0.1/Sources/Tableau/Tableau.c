/*
** Tableau.c for Xclamation in Tableau/
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
** Started on  Fri Sep 30 15:33:47 1994 Emmanuel Paris
** Last update Thu Oct 24 15:39:52 1996 Emmanuel Paris
*/

#include "Tableau.h"
#include "Colormap.h"

extern c_Colormap *GlobColormap;
extern c_Color *GlobColorBase;

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
static boolean read_Tableau ___PROTO((c_DocFile *doc, char *keyword,
				      long param));
static boolean write_Tableau ___PROTO((c_Tableau *this, c_DocFile *doc));
static boolean ps_print ___PROTO((c_Tableau *This, c_PostScript *post));

static void init_Tableau ___PROTO((c_Tableau *This, Display *display,
				    Window window, boolean visible));
sf_Tableau fc_Tableau =
{
  cons_Tableau, 
  dest_Tableau,
  copy_Tableau,
  resize_Tableau,
  eventlook_Tableau,
  set_function_Tableau,
  read_Tableau,
  write_Tableau,
  ps_print,
  init_Tableau,
};


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *cons_Tableau()
{
  c_Tableau *ObjTmp;
  c_Color *color;

  Xc_HISTORY(("constructor"));
  if ((ObjTmp = (c_Tableau *)Xc_malloc("Tableau",
					sizeof(c_Tableau))) == NULL)
    {
      Xc_FATAL(("Can't create Tableau"));
    }
  ObjTmp->f = &fc_Tableau;
  ObjTmp->scrollbh_on = FALSE;
  ObjTmp->scrollbv_on = FALSE;
  ObjTmp->modified = FALSE;

  ObjTmp->color_base = NULL;
  for(color = GlobColorBase; color != NULL; color = color->next)
    F(color).copy(color, &ObjTmp->color_base);

  ObjTmp->text_ruler_base = NULL;
  ObjTmp->text_style_base = NULL;
 
  return ObjTmp;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void dest_Tableau(this)
c_Tableau *this;
{
  Xc_HISTORY(("destructor"));
  
  /*--- Destroy the Tableau text ruler list ---*/
  while(this->text_ruler_base != NULL)
    DELETE(c_TextRuler)(this->text_ruler_base);
  
  /*--- Destroy the Tableau text style list ---*/
  while(this->text_style_base != NULL)
    DELETE(c_TextStyle)(this->text_style_base);
  
  /*--- Destroy the Tableau color database ---*/
  while(this->color_base != NULL)
    DELETE(c_Color)(this->color_base);
  
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
  if ((ObjTmp = (c_Tableau *)Xc_malloc("CTableau",
					sizeof(c_Tableau))) == NULL)
    {
      Xc_FATAL(("Can't copy Tableau"));
    }
  memcpy(ObjTmp, This, sizeof(c_Tableau));
  return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** Init_Tableau                                                      ** 
** ----------------------------------------------------------------- */
static void init_Tableau(This,display,window,visible)
     c_Tableau *This;
     Display	*display;
     Window	window;
     boolean	visible;
{
  color_cells_t	realcolor;
  long		doc_color;
  
  This->display = display;
  This->visible = visible;
  
  F(GlobColormap).allocate_named_color(GlobColormap, DTAB_COLOR, &realcolor);
  doc_color = realcolor.pixel; 
  
  This->window = 
    XCreateSimpleWindow(This->display, window,
			DTAB_POSX, DTAB_POSY, DTAB_SIZEX, DTAB_SIZEY, 0, 
			doc_color,doc_color);
  XSelectInput(This->display, This->window, StructureNotifyMask |
	       ButtonPressMask);
  
  /***********************/
  /* Creation Scroll Bar */
  /***********************/
  This->ScrollBarV = (c_ScrollBarX *)NEW(c_ScrollBarX)
    (This->display,
     This->window,
     DTAB_SIZEX - SCROLLB_SIZE, 0,
     DTAB_SIZEY,
     DTAB_SIZEY - SCROLLB_SIZE - PAGER_SIZE,
     SBVERTICAL,
     1);
  This->ScrollBarH = (c_ScrollBarX *)NEW(c_ScrollBarX)
    (This->display,
     This->window,
     0, DTAB_SIZEY - SCROLLB_SIZE - PAGER_SIZE,
     DTAB_SIZEX,
     DTAB_SIZEX - SCROLLB_SIZE,
     SBHORIZONTAL,
     0);
  F(This->ScrollBarV).Change_Rsize(This->ScrollBarV, display, 10,
				   DTAB_SIZEY - SCROLLB_SIZE - PAGER_SIZE);
  F(This->ScrollBarH).Change_Rsize(This->ScrollBarH, display, 
				   DTAB_SIZEX - SCROLLB_SIZE, 10);
  This->scrollbh_on = TRUE;
  This->scrollbv_on = TRUE;

  /******************/
  /* Creation Pager */
  /******************/
  This->Pager = (c_Pager *)NEW(c_Pager)
    (This->display,
     This->window,
     0, DTAB_SIZEY - PAGER_SIZE,
     DTAB_SIZEX - SCROLLB_SIZE,
     0,NULL,NULL);
}

static void resize_Tableau(This, Event, Maximized)
c_Tableau	*This;
XEvent	*Event;
boolean	Maximized;
{
  /**********************/
  /* modifie le pager   */
  /**********************/
  F(This->Pager).Resize
    (This->Pager,
     0,
     Event->xconfigure.height - PAGER_SIZE,
     Event->xconfigure.width  - SCROLLB_SIZE);
  /**************************/
  /* modifie les ascenceurs */
  /**************************/
  if (Maximized)
    F(This->ScrollBarV).Resize
      (This->ScrollBarV, 
       Event->xconfigure.width - SCROLLB_SIZE, 0,
       Event->xconfigure.height - SCROLLB_SIZE - PAGER_SIZE,
       Event->xconfigure.height , 1, 0, 0, TRUE);
  else
    F(This->ScrollBarV).Resize
      (This->ScrollBarV, 
       Event->xconfigure.width - SCROLLB_SIZE, 0,
       Event->xconfigure.height - SCROLLB_SIZE - PAGER_SIZE,
       Event->xconfigure.height , 1, 0, 0, TRUE);
  F(This->ScrollBarH).Resize
    (This->ScrollBarH, 0,
     Event->xconfigure.height - SCROLLB_SIZE - PAGER_SIZE,
     Event->xconfigure.width - SCROLLB_SIZE ,
     Event->xconfigure.width, 0, 0, 0, TRUE);
}

static void eventlook_Tableau(This, Event, listegwm, DocSelected)
c_Tableau	*This;
XEvent		*Event;
listmanaged	*listegwm;
boolean		DocSelected;
{
  if (DocSelected || 
      (Event->type == Expose) ||
      (Event->type == GraphicsExpose))
  {
/* ----------------------------------------------------------------- **
** analysing scrollbar                                               **
** ----------------------------------------------------------------- */
    if (This->scrollbv_on)
      F(This->ScrollBarV).EventLook
	(This->ScrollBarV,
	 Event->xany.display,
	 Event, (Window)NULL, 0, 0);
    if (This->scrollbh_on)
      F(This->ScrollBarH).EventLook
	(This->ScrollBarH,
	 Event->xany.display,
	 Event, (Window)NULL, 0, 0);
    
/* ----------------------------------------------------------------- **
** analysing Pager                                                   **
** ----------------------------------------------------------------- */
    F(This->Pager).EventLook(This->Pager,
			     Event->xany.display, Event);
  }
  if (!DocSelected)
  {
/* ----------------------------------------------------------------- ** 
** click dans la fenetre                                             ** 
** ----------------------------------------------------------------- */
    if (Event->type == ButtonPress)
    {
      if ((Event->xany.window == This->window) ||
	  (Event->xany.window == This->ScrollBarH->window) ||
	  (Event->xany.window == This->ScrollBarV->window) ||
	  (Event->xany.window == This->Pager->window))
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
  return;
}


/* ----------------------------------------------------------------- ** 
** generate a ps file                                                ** 
** ----------------------------------------------------------------- */
static boolean ps_print(This, post)
c_Tableau	*This;
c_PostScript	*post;
{
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** read_Tableau - Tableau loading                                    ** 
** ----------------------------------------------------------------- */
static boolean read_Tableau(doc, keyword, param)
c_DocFile	*doc;
char		*keyword;
long		param;
{
  return FALSE;
}


/* ----------------------------------------------------------------- ** 
** write_Tableau - Tableau saving                                    ** 
** ----------------------------------------------------------------- */
static boolean write_Tableau(this, doc)
c_Tableau *this;
c_DocFile *doc;
{
  return FALSE;
}
