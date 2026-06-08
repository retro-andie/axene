/*
** Document.c for Xclamation in Document/
** Methods for the Document class
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
** Last update Wed Nov  4 16:18:29 1998 Emmanuel Paris
*/

#include "WInterface.h"
#include "MainInterface.h"
#include "Box_page.h"
#include "Box_gotopage.h"
#include "DocumentP.h"
#include "Colormap.h"
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
static void add_Page ___PROTO((c_Document *This, c_Page *Page));
static void del_Page ___PROTO((c_Document *This, listpage *Lpage));
static void ins_Page ___PROTO((c_Document *This,c_Page *Page,listpage *Lpage));

static void insert_Page ___PROTO((c_Document *This, listpage *Lpage, 
				  boolean before));
static boolean modify_Page ___PROTO((c_Document *This, listpage **Lpage));
static void delete_Page ___PROTO((c_Document *This, listpage *Lpage));
static boolean goto_Page ___PROTO((c_Document *This, listpage **Lpage));
static listpage *get_previous_page ___PROTO((c_Document *This, listpage *pge));


sf_Document fc_Document =
{
 cons_Document, 
 dest_Document,
 copy_Document,
 resize_Document,
 eventlook_Document,
 set_function_Document,
 readDocument,
 writeDocument,
 ps_printDocument,
 init_Document,
 add_Page,
 del_Page,
 ins_Page,
 insert_Page,
 modify_Page,
 delete_Page,
 goto_Page
};


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *cons_Document()
{
 c_Document *ObjTmp;
 c_TextRuler *ruler;
 c_TextStyle *style;
 c_Color *color;

 Xc_HISTORY(("constructor"));

 if((ObjTmp = Xc_malloc("Document", sizeof(c_Document))) == NULL) return NULL;
 memset(ObjTmp, 0, sizeof(c_Document));
 ObjTmp->f = &fc_Document;

 ObjTmp->modified = FALSE;
 ObjTmp->ListPage = NULL;
 ObjTmp->SelectedPage = NULL;
 ObjTmp->NbrPage = 0;
 ObjTmp->regle_ok = FALSE;
 ObjTmp->scrollbh_on = FALSE;
 ObjTmp->scrollbv_on = FALSE;
 ObjTmp->filename = NULL;
 ObjTmp->PosX = DDOC_POSX;
 ObjTmp->PosY = DDOC_POSY;
 ObjTmp->Width = DDOC_SIZEX;
 ObjTmp->Height = DDOC_SIZEY;
 
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

 /*--- Copy global color base ---*/
 ObjTmp->BaseStd.color_base = NULL;
 for(color = GlobColorBase; color != NULL; color = color->next)
  COPY(c_Color)(color, &ObjTmp->BaseStd.color_base);

 /*--- Copy global text ruler base ---*/
 ObjTmp->BaseStd.text_ruler_base = NULL;
 for(ruler = GlobTextRulerBase; ruler != NULL; ruler = ruler->next)
  F(ruler).virtualCopy(ruler, &ObjTmp->BaseStd);
  
 /*--- Copy global text style base ---*/
 ObjTmp->BaseStd.text_style_base = NULL;
 for(style = GlobTextStyleBase; style != NULL; style = style->next)
  F(style).virtualCopy(style, &ObjTmp->BaseStd);

 ObjTmp->BaseStd.text_base = NULL;
 
 /*--- temporaire ---*/
 if(ObjTmp->BaseStd.text_ruler_base == NULL)
  NEW(c_TextRuler)(&ObjTmp->BaseStd.text_ruler_base, "ruler");

 if(ObjTmp->BaseStd.text_style_base == NULL)
 {
  c_VectorFont *font;
  c_Color *color;

  /*--- Get font ---*/
  if(GlobFontBase == NULL)
  {
   DELETE(c_Document)(ObjTmp);
   return NULL;
  }
  font = F(GlobFontBase).getFontByName(GlobFontBase, "Courier");
  if(font == NULL) font = GlobFontBase;
      
  color = F(ObjTmp->BaseStd.color_base).getColor
   (&(ObjTmp->BaseStd.color_base), XcC_NAME,"Black",
    XcC_GRAY, XcC_SCALE_COLOR(0.0), XcC_TRANSPARENCY,0, XcC_END);
      
  NEW(c_TextStyle)(&ObjTmp->BaseStd.text_style_base, "style",
		   font, SCALE_FROM_POINTS(12), color);
 }
 /*--- Temporaire ---*/

 Xc_TRACE(("end constructor"));
 return ObjTmp;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void dest_Document(this)
c_Document *this;
{
 Xc_HISTORY(("destructor"));

 /*--- Destroy pages ---*/
 while(this->ListPage != NULL) F(this).DelPage(this, this->ListPage);

 /*--- Destroy the document text database ---*/
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

 if (this->scrollbh_on)
  DELETE(c_ScrollBarX)(this->ScrollBarV);
 if (this->scrollbv_on)
  DELETE(c_ScrollBarX)(this->ScrollBarH);
 if (this->regle_ok)
 {
  DELETE(c_Regle)(this->RegleH);
  DELETE(c_Regle)(this->RegleV);
  DELETE(c_Regle)(this->RegleC);
  DELETE(c_Pager)(this->Pager);
 }
 if(this->filename != NULL) Xc_free(this->filename);
  
 Xc_free(this);
  
 Xc_TRACE(("destroyed"));
}


/* ----------------------------------------------------------------- ** 
** Copier                                                            ** 
** ----------------------------------------------------------------- */
static void *copy_Document(This)
c_Document *This;
{
 c_Document *ObjTmp;

 Xc_HISTORY(("copy"));
 if ((ObjTmp = (c_Document *)Xc_malloc("CDocument",
				       sizeof(c_Document))) == NULL)
 {
  Xc_FATAL(("Can't copy document"));
 }
 memcpy(ObjTmp, This, sizeof(c_Document));
 return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** Init_Document                                                     ** 
** ----------------------------------------------------------------- */
static void init_Document(This,display,window,visible,name)
c_Document *This;
Display	*display;
Window	window;
boolean	visible;
char	*name;
{
 color_cells_t	realcolor;
 long		doc_color;
  
 Xc_HISTORY(("init"));

 This->display = display;
 This->visible = visible;
 if (This->SelectedPage == NULL)/* if new document, not loaded */
  This->SelectedPage = This->ListPage;
  
 F(GlobColormap).allocate_named_color(GlobColormap, DDOC_COLOR, &realcolor);
 doc_color = realcolor.pixel; 
 This->window = 
  XCreateSimpleWindow(This->display, window,
		      This->PosX, This->PosY, This->Width, This->Height, 0, 
		      doc_color,doc_color);  
 XSelectInput(This->display, This->window, StructureNotifyMask |
	      ButtonPressMask);
 if(name != NULL) strncpy(This->name, name, XcMAX_NAME_LENGTH - 1);
  
 /***********************/
 /* Creation Scroll Bar */
 /***********************/
 This->ScrollBarV = (c_ScrollBarX *)NEW(c_ScrollBarX)
  (This->display,
   This->window,
   This->Width - SCROLLB_SIZE, 0,
   This->Height,
   This->Height - SCROLLB_SIZE - PAGER_SIZE - REGLE_SIZE,
   SBVERTICAL,
   1);
 This->ScrollBarH = (c_ScrollBarX *)NEW(c_ScrollBarX)
  (This->display,
   This->window,
   0, This->Height - SCROLLB_SIZE - PAGER_SIZE,
   This->Width,
   This->Width - SCROLLB_SIZE - REGLE_SIZE,
   SBHORIZONTAL,
   0);
 This->scrollbh_on = TRUE;
 This->scrollbv_on = TRUE;
 /*******************/
 /* Creation Regles */
 /*******************/
 This->RegleH = (c_Regle *)NEW(c_Regle)(display,
					This->window,
					REGLE_SIZE+1, 0,
					This->Width - REGLE_SIZE -
					SCROLLB_SIZE-1,
					REGLE_HORIZONTAL);
 This->RegleV = (c_Regle *)NEW(c_Regle)(display,
					This->window,
					0, REGLE_SIZE+1,
					This->Height - REGLE_SIZE - 
					SCROLLB_SIZE - PAGER_SIZE-1,
					REGLE_VERTICAL);
 This->RegleC = (c_Regle *)NEW(c_Regle)(display,
					This->window,
					0, 0,
					REGLE_SIZE,
					REGLE_CENTER);
 This->regle_ok = TRUE;
 /******************/
 /* Creation Pager */
 /******************/
 This->Pager = (c_Pager *)NEW(c_Pager)
  (This->display,
   This->window,
   0, This->Height - PAGER_SIZE,
   This->Width - SCROLLB_SIZE, This);
 /***************/
 /*  Init Page  */
 /***************/
 /*  F(This->SelectedPage->Page).
     init_Page(This->SelectedPage->Page, 1, Xc_PAGE_A4, Xc_PAGE_DOUBLE,
     Xc_PORTRAIT, SCALE_FROM_MILLIMETERS(17.82),
     SCALE_FROM_MILLIMETERS(17.82), SCALE_FROM_MILLIMETERS(17.82),
     SCALE_FROM_MILLIMETERS(17.82), 2, 3, 
     SCALE_FROM_MILLIMETERS(4.53), SCALE_FROM_MILLIMETERS(4.53)); */
  
 F(This->SelectedPage->Page).
  select_Page(This->SelectedPage->Page,
	      This->display, This->window,
	      REGLE_SIZE, REGLE_SIZE,
	      This->Width - (REGLE_SIZE + SCROLLB_SIZE) - 1, 
	      This->Height - (SCROLLB_SIZE + REGLE_SIZE + PAGER_SIZE) - 1,
	      This->ScrollBarV, This->ScrollBarH, This->RegleV,
	      This->RegleH, This->RegleC, This->Pager);
  
 /* only used while loading */
 if (!This->SelectedPage->Page->Regle_actif)
 {
  This->SelectedPage->Page->Regle_actif = TRUE;
  F(This->SelectedPage->Page)
   .set_function(This->SelectedPage->Page, DISPLAY_REGLE);
 }
  
 Xc_TRACE(("end init"));
}

/* ----------------------------------------------------------------- ** 
** add_Page                                                          ** 
** ----------------------------------------------------------------- */
static void add_Page(This, Page)
c_Document	*This;
c_Page		*Page;
{
 listpage	**PageTmp;
 listpage	*PageNew;

 Xc_HISTORY(("add page"));
 if ((PageNew = (listpage *)Xc_malloc("AddPage",
				      sizeof (listpage))) == NULL)
 {
  Xc_FATAL(("Can't add new Page in list"));
 }

 PageNew->Page = Page;
 PageNew->NextPage = NULL;

 PageTmp = &D(This).ListPage;
 D(This).NbrPage++;
 while (*PageTmp != NULL)
 {
  PageTmp = &((*PageTmp)->NextPage);
 }
 *PageTmp = PageNew;
 Xc_TRACE(("Page adding to document with no error")); 
}


/* ----------------------------------------------------------------- ** 
** del_Page                                                          ** 
** ----------------------------------------------------------------- */
static void del_Page(This, Lpage)
c_Document	*This;
listpage	*Lpage;
{
 listpage	*ListPage;

 if (This->ListPage == Lpage)
 {
  This->ListPage = This->ListPage->NextPage;
 }
 else
 {
  ListPage = This->ListPage;
      
  while(ListPage && ListPage->NextPage != Lpage)
   ListPage = ListPage->NextPage;
      
  if (!ListPage)
  {
   Xc_WARNING(("attempt to del an unknow page"));
   return;
  }
  ListPage->NextPage = Lpage->NextPage;
 }
  
 This->NbrPage = This->NbrPage - 1;
 DELETE(c_Page)(Lpage->Page);
 Xc_free(Lpage);
}


/* ----------------------------------------------------------------- ** 
** ins_Page                                                          ** 
** ----------------------------------------------------------------- */
static void ins_Page(This, Page, Lpage)
c_Document	*This;
c_Page		*Page;
listpage	*Lpage;
{
 listpage	*PageNew;

 if ((PageNew = (listpage *)Xc_malloc("AddPage",
				      sizeof (listpage))) == NULL)
 {
  Xc_FATAL(("Can't add new Page in list"));
 }
 PageNew->Page = Page;

 if (Lpage)
 {
  PageNew->NextPage = Lpage->NextPage;
  Lpage->NextPage = PageNew;
 }
 else
 {
  PageNew->NextPage = This->ListPage;
  This->ListPage = PageNew;
 }
 This->NbrPage = This->NbrPage + 1; 
}

static void resize_Document(This, Event, Maximized)
c_Document	*This;
XEvent	*Event;
boolean	Maximized;
{
 This->PosX = Event->xconfigure.x;
 This->PosY = Event->xconfigure.y;
 This->Width = Event->xconfigure.width;
 This->Height = Event->xconfigure.height;
  
 /**********************/
 /* modifie le pager   */
 /**********************/
 F(This->Pager).Resize
  (This->Pager,
   0,
   Event->xconfigure.height - PAGER_SIZE,
   Event->xconfigure.width 
   - SCROLLB_SIZE*(This->SelectedPage->Page->ScrollBarV_actif &&
		   This->SelectedPage->Page->ScrollBarH_actif) );
 /**************************/
 /* modifie les ascenceurs */
 /**************************/
 if (Maximized)
  F(This->ScrollBarV).Resize
   (This->ScrollBarV, 
    Event->xconfigure.width - SCROLLB_SIZE,
    REGLE_SIZE,
    Event->xconfigure.height
    - This->SelectedPage->Page->Regle_actif * REGLE_SIZE
    - This->SelectedPage->Page->ScrollBarH_actif * SCROLLB_SIZE 
    - This->SelectedPage->Page->Pager_actif * PAGER_SIZE,
    Event->xconfigure.height - REGLE_SIZE
    - (1 - This->SelectedPage->Page->ScrollBarH_actif) * PAGER_SIZE,
    This->SelectedPage->Page->ScrollBarH_actif,
    This->SelectedPage->Page->Orig_X,
    This->SelectedPage->Page->Orig_Y,
    This->SelectedPage->Page->ScrollBarV_actif);
 else
  F(This->ScrollBarV).Resize
   (This->ScrollBarV, 
    Event->xconfigure.width - SCROLLB_SIZE,
    0,
    Event->xconfigure.height
    - This->SelectedPage->Page->Regle_actif * REGLE_SIZE
    - This->SelectedPage->Page->ScrollBarH_actif * SCROLLB_SIZE 
    - This->SelectedPage->Page->Pager_actif * PAGER_SIZE,
    Event->xconfigure.height 
    - (1 - This->SelectedPage->Page->ScrollBarH_actif) * PAGER_SIZE,
    This->SelectedPage->Page->ScrollBarH_actif,
    This->SelectedPage->Page->Orig_X,
    This->SelectedPage->Page->Orig_Y,
    This->SelectedPage->Page->ScrollBarV_actif);
 F(This->ScrollBarH).Resize
  (This->ScrollBarH, 
   0,
   Event->xconfigure.height - SCROLLB_SIZE - PAGER_SIZE,
   Event->xconfigure.width
   - This->SelectedPage->Page->Regle_actif * REGLE_SIZE - SCROLLB_SIZE ,
   Event->xconfigure.width, 0, 
   This->SelectedPage->Page->Orig_X,
   This->SelectedPage->Page->Orig_Y,
   This->SelectedPage->Page->ScrollBarH_actif);

 /**********************/
 /* modifie les regles */
 /**********************/
 F(This->RegleH).Resize
  (This->RegleH,
   REGLE_SIZE + 1, 0, 
   Event->xconfigure.width 
   - SCROLLB_SIZE * This->SelectedPage->Page->ScrollBarV_actif  
   - REGLE_SIZE * This->SelectedPage->Page->Regle_actif - 1);
 F(This->RegleV).Resize
  (This->RegleV,
   0, REGLE_SIZE + 1,
   Event->xconfigure.height 
   - SCROLLB_SIZE * This->SelectedPage->Page->ScrollBarH_actif
   - PAGER_SIZE * This->SelectedPage->Page->Pager_actif
   - REGLE_SIZE * This->SelectedPage->Page->Regle_actif - 1);
 /*******************/
 /* modifie la page */
 /*******************/
 F(This->SelectedPage->Page).resize_Page
  (This->SelectedPage->Page,
   REGLE_SIZE * This->SelectedPage->Page->Regle_actif,
   REGLE_SIZE * This->SelectedPage->Page->Regle_actif,
   Event->xconfigure.width 
   - SCROLLB_SIZE * This->SelectedPage->Page->ScrollBarV_actif
   - REGLE_SIZE   * This->SelectedPage->Page->Regle_actif - 1,
   Event->xconfigure.height 
   - SCROLLB_SIZE * This->SelectedPage->Page->ScrollBarH_actif
   - PAGER_SIZE   * This->SelectedPage->Page->Pager_actif
   - REGLE_SIZE   * This->SelectedPage->Page->Regle_actif - 1 );
}

static void eventlook_Document(This, Event, listegwm, DocSelected)
c_Document	*This;
XEvent		*Event;
listmanaged	*listegwm;
boolean		DocSelected;
{
 status_t	function_return;
 listpage	*listviewpage;
  
 /* ----------------------------------------------------------------- ** 
 ** Drag and drop function                                            ** 
 ** ----------------------------------------------------------------- */
 if ((Event->type == ClientMessage) && 
     (Event->xany.window == 
      This->SelectedPage->Page->X_info.window) &&
     ((DROP_FLAGS(*Event) & 0xffff00)
      == DD_XCLAMATION_INDENTITY))
 {		
  if ((DROP_FLAGS(*Event) & 0xf) == DD_DROP_FOR_TARGET)
  {
   F(Gwm).DiselectWindow(Gwm, Event->xany.display, listegwm);
  }
  return;
 }		
  
 if ((DocSelected) || 
     (Event->type == Expose) ||
     (Event->type == GraphicsExpose))
 {

  /* ----------------------------------------------------------------- **
  ** analysing scrollbar                                               **
  ** ----------------------------------------------------------------- */
  if (This->scrollbv_on)
   if (F(This->ScrollBarV).EventLook(This->ScrollBarV, Event->xany.display,
				     Event,
				     This->SelectedPage->Page->X_info.window,
				     This->SelectedPage->Page->Orig_X,
				     This->SelectedPage->Page->Orig_Y))
    return;
 
  if (This->scrollbh_on)
   F(This->ScrollBarH).EventLook(This->ScrollBarH, Event->xany.display,
				 Event,
				 This->SelectedPage->Page->X_info.window,
				 This->SelectedPage->Page->Orig_X,
				 This->SelectedPage->Page->Orig_Y);

  /* ----------------------------------------------------------------- **
  ** analysing regle                                                   **
  ** ----------------------------------------------------------------- */
  if (This->SelectedPage->Page->Regle_actif)
  {
   This->modified |= F(This->RegleH).EventLook(This->RegleH, Event);
   This->modified |= F(This->RegleV).EventLook(This->RegleV, Event);
   This->modified |= F(This->RegleC).EventLook(This->RegleC, Event);
  }

  /* ----------------------------------------------------------------- **
  ** analysing Pager                                                   **
  ** ----------------------------------------------------------------- */
  if (This->Pager)
  {
   F(This->Pager).EventLook(This->Pager,
			    Event->xany.display, Event);
  }

  /* ----------------------------------------------------------------- ** 
  ** analysing Page                                                    ** 
  ** ----------------------------------------------------------------- */
  listviewpage = This->SelectedPage;
  function_return = F(listviewpage->Page).eventlook
   (listviewpage->Page, Event);
  This->modified |= (function_return == FUNCTION_FINISHED) | 
   (function_return == FUNCTION_MODIFIED);
  if (function_return != FUNCTION_NOT_FINISHED && 
      function_return != FUNCTION_MODIFIED)
  {
   switch(listviewpage->Page->function_type)
   {
   case CREATE_CADRE_RECTANGLE:
   case CREATE_CADRE_ELIPSE:
   case CREATE_CADRE_POLYGONAL:
   case ROTATE_CADRE:
   case SCALE_CADRE:
   case ADD_LINK_AT_BEGINNING:
   case ADD_LINK_AT_END:
   case INSERT_LINK_BEFORE:
   case INSERT_LINK_BEHIND:
   case INSERT_POINT:
   case DELETE_POINT:
   case MOVE_IMAGE:
   case MOVE_VECTOR:
    if (function_return == FUNCTION_REINIT)
     F(listviewpage->Page).
      set_function(listviewpage->Page,
		   SELECT_MOVE_RESIZE_CADRE);
    else
     if (function_return == FUNCTION_FINISHED)
     {
      INIT(listviewpage->Page);
     }
     else
     {
      ABORT(listviewpage->Page);
      INIT(listviewpage->Page); 
     }
    break;
   case MOVE_POINT_MODE:
   case RESIZE_CADRE_MODE:
   case SELECT_MOVE_RESIZE_CADRE:
    ABORT(listviewpage->Page);
    INIT(listviewpage->Page);   
    break;
   case ZOOM_PLUS_PAGE:
   case ZOOM_MINUS_PAGE:
   case ZOOM_IN_ZONE:
    F(listviewpage->Page).
     set_function(listviewpage->Page,
		  SELECT_MOVE_RESIZE_CADRE);
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
       (Event->xany.window == 
	This->SelectedPage->Page->X_info.window) ||
       (Event->xany.window == This->RegleH->window) ||
       (Event->xany.window == This->RegleV->window) ||
       (Event->xany.window == This->RegleC->window) ||
       (Event->xany.window == This->ScrollBarH->window) ||
       (Event->xany.window == This->ScrollBarV->window) ||
       (Event->xany.window == This->Pager->window))
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
 if (!This->SelectedPage->Page)
  return;
 
 if (function >= DOCUMENT_FUNCTION_BEGIN && function <= DOCUMENT_FUNCTION_END)
 {
  listpage *lpage, *lpage2;
  boolean defv = FALSE;
  int func;
  
  lpage = This->SelectedPage;
  switch(function)
  {
  case DOC_ZOOM_FIT_IN_WIDTH:
  case DOC_ZOOM_FIT_IN_SCREEN:
  case DOC_ZOOM_AT_REAL_SIZE:
  case DOC_DISPLAY_GRID:
  case DOC_DISPLAY_TYPO_MARK:
  case DOC_DISPLAY_ALIGN_MARK:
  case DOC_DISPLAY_FRAME_BORDER:
  case DOC_MAGNETIZE_ALL:
   switch(function)
   {
   case DOC_ZOOM_FIT_IN_WIDTH:
    func = ZOOM_FIT_IN_WIDTH;
    break;
   case DOC_ZOOM_FIT_IN_SCREEN:
    func = ZOOM_FIT_IN_SCREEN;
    break;
   case DOC_ZOOM_AT_REAL_SIZE:
    func = ZOOM_AT_REAL_SIZE;
    break;
   case DOC_DISPLAY_GRID:
    func = DISPLAY_GRID;
    defv = lpage->Page->Page_Grid_actif;
    break;
   case DOC_DISPLAY_TYPO_MARK:
    func = DISPLAY_TYPO_MARK;
    defv = lpage->Page->Page_Typo_Mark_actif;
    break;
   case DOC_DISPLAY_ALIGN_MARK:
    func = DISPLAY_ALIGN_MARK;
    defv = lpage->Page->Page_Align_Mark_actif;
    break;
   case DOC_DISPLAY_FRAME_BORDER:
    func = DISPLAY_FRAME_BORDER;
    defv = lpage->Page->Frame_Border_actif;
    break;
   case DOC_MAGNETIZE_ALL:
   default:
    func = MAGNETIZE_ALL;
    defv = lpage->Page->magnetic;
    break;
   }
   lpage = This->ListPage;
   while(lpage)
   {
    switch(function)
    {
    case DOC_ZOOM_FIT_IN_WIDTH:
    case DOC_ZOOM_FIT_IN_SCREEN:
    case DOC_ZOOM_AT_REAL_SIZE:
     if (lpage != This->SelectedPage)
     {
      memcpy(&(lpage->Page->X_info), &(This->SelectedPage->Page->X_info),
	     sizeof(x_info));
      lpage->Page->RegleH = This->SelectedPage->Page->RegleH;
      lpage->Page->RegleV = This->SelectedPage->Page->RegleV;
     }
     This->modified |= F(lpage->Page).set_function(lpage->Page, func);
     break;
    case DOC_DISPLAY_GRID:
     if (lpage->Page->Page_Grid_actif == defv)
      This->modified |= F(lpage->Page).set_function(lpage->Page, func);
     break;
    case DOC_DISPLAY_TYPO_MARK:
     if (lpage->Page->Page_Typo_Mark_actif == defv)
      This->modified |= F(lpage->Page).set_function(lpage->Page, func);
     break;
    case DOC_DISPLAY_ALIGN_MARK:
     if (lpage->Page->Page_Align_Mark_actif == defv)
      This->modified |= F(lpage->Page).set_function(lpage->Page, func);
     break;
    case DOC_DISPLAY_FRAME_BORDER:
     if (lpage->Page->Frame_Border_actif == defv)
      This->modified |= F(lpage->Page).set_function(lpage->Page, func);
     break;
    case DOC_MAGNETIZE_ALL:
     if (lpage->Page->magnetic == defv)
      This->modified |= F(lpage->Page).set_function(lpage->Page, func);
     break;
    }
    lpage = lpage->NextPage;
   }
   break;
  case DOC_INSERT_PAGE_BEFORE:
   insert_Page(This, This->SelectedPage, TRUE);
   F(This->Pager).SelectPage(This->Pager, 
			     get_previous_page(This, This->SelectedPage));
   break;
  case DOC_INSERT_PAGE_AFTER:
   insert_Page(This, This->SelectedPage, FALSE);
   F(This->Pager).SelectPage(This->Pager, This->SelectedPage->NextPage);
   break;
  case DOC_MODIFY_PAGE:
   lpage = This->SelectedPage;
   modify_Page(This, &lpage);
   F(This->Pager).SelectPage(This->Pager, lpage);
   break;
  case DOC_DELETE_PAGE:
   lpage2 = This->SelectedPage;
   if (lpage2->NextPage)
    lpage = lpage2->NextPage;
   else
   {
    if (lpage2 == This->ListPage) break;
    lpage = get_previous_page(This, lpage2);
   }
   F(This->Pager).SelectPage(This->Pager, lpage);
   delete_Page(This, lpage2);
   F(This->Pager).SelectPage(This->Pager, lpage);
   break;
  case DOC_GOTO_PAGE:
   break;
  case DOC_GOTO_FIRST_PAGE:
   F(This->Pager).SelectPage(This->Pager, This->ListPage);
   break;
  case DOC_GOTO_LAST_PAGE:
   lpage = This->ListPage;
   while(lpage->NextPage)
    lpage = lpage->NextPage;
   F(This->Pager).SelectPage(This->Pager, lpage);
   break;
  case DOC_GOTO_PREVIOUS_PAGE:
   if (This->SelectedPage != This->ListPage)
    F(This->Pager).SelectPage(This->Pager, 
			      get_previous_page(This, This->SelectedPage));
   break;
  case DOC_GOTO_NEXT_PAGE:
   if (This->SelectedPage->NextPage)
    F(This->Pager).SelectPage(This->Pager, This->SelectedPage->NextPage);
   break;
  default:
   break;
  }
 }
 else
  This->modified |= F(This->SelectedPage->Page).set_function
   (This->SelectedPage->Page, function);
}

/* ----------------------------------------------------------------- ** 
** function on Pages call by Pager                                   ** 
** ----------------------------------------------------------------- */

static void recalcul_Page_number(This, Lpage, new_number) 
c_Document	*This;
listpage       	*Lpage;
int		new_number;
{
 listpage	*ListPage;
  
 ListPage = Lpage;
 while(ListPage)
 {
  ListPage->Page->page_number = new_number;
  new_number += 1 + (ListPage->Page->page_type == Xc_PAGE_DOUBLE);
  ListPage = ListPage->NextPage;
 }
}

static void insert_Page(This, Lpage, before)
c_Document	*This;
listpage	*Lpage;
boolean		before;
{
 c_Page	*page;
 c_Page	*new_page;
 listpage	*ListPage;

 page = Lpage->Page;

 new_page = (c_Page *)NEW(c_Page)(&This->BaseStd);
 F(new_page).init_Page(new_page, page->page_number,
		       page->page_format, page->page_type,
		       page->page_orient,
		       page->Page_Typo_Mark->left_margin,
		       page->Page_Typo_Mark->right_margin, 
		       page->Page_Typo_Mark->top_margin,
		       page->Page_Typo_Mark->bottom_margin,
		       page->Page_Typo_Mark->column,
		       page->Page_Typo_Mark->row,
		       page->Page_Typo_Mark->gouttiere_x,
		       page->Page_Typo_Mark->gouttiere_y,
		       page->Page_Grid.sOrig_X - page->Page_Contour.sOrig_X,
		       page->Page_Grid.sOrig_Y - page->Page_Contour.sOrig_Y,
		       page->Page_Grid.sEcart_X,
		       page->Page_Grid.sEcart_Y,
		       XPi_SCRATCH);
 if (before)
 {
  ListPage = This->ListPage;
      
  if (ListPage == Lpage)
   ListPage = NULL;
  else
   while(ListPage->NextPage!=Lpage)
    ListPage = ListPage->NextPage;
  F(This).InsPage(This, new_page, ListPage);
  recalcul_Page_number(This, Lpage, page->page_number + 1 +
		       (page->page_type == Xc_PAGE_DOUBLE));
 }
 else
 {
  F(This).InsPage(This, new_page, Lpage);
  recalcul_Page_number(This, Lpage, page->page_number);
 }
  
 This->modified = TRUE;
 Xc_TRACE(("insert_Page"));
}

static boolean modify_Page(This,Lpage)
c_Document	*This;
listpage	**Lpage;
{
 listpage	*list_page;
 c_Page	*page;
 c_Box_page	*box;
 page_type_t	type;
 int		i, selpg = 0;
 /*  new_doc_s	new_doc;
     new_doc_s	def_doc; */
 
 new_doc_s	*new_doc;
  
 new_doc = (new_doc_s *)Xc_malloc("new doc",sizeof(new_doc_s)*This->NbrPage);
 if (!new_doc) return FALSE;
 list_page = This->ListPage;
 for(i=0; i<This->NbrPage; i++)
 {
  page = list_page->Page;
  new_doc[i].page_nbr = page->page_number;
  new_doc[i].p_f = page->page_format;
  switch(page->page_type)
  {
  case Xc_PAGE_DOUBLE:
   new_doc[i].p_do = TRUE;
   new_doc[i].p_rv = FALSE;
   break;
  case Xc_PAGE_LEFT:
   new_doc[i].p_do = FALSE;
   new_doc[i].p_rv = TRUE;
   break;
  case Xc_PAGE_RIGHT:
   new_doc[i].p_do = FALSE;
   new_doc[i].p_rv = FALSE;
   break;
  }
  new_doc[i].p_o = page->page_orient; 
  new_doc[i].le_m = page->Page_Typo_Mark->left_margin;
  new_doc[i].ri_m = page->Page_Typo_Mark->right_margin;
  new_doc[i].up_m = page->Page_Typo_Mark->top_margin;
  new_doc[i].lo_m = page->Page_Typo_Mark->bottom_margin;
  new_doc[i].n_c = page->Page_Typo_Mark->column;
  new_doc[i].n_r = page->Page_Typo_Mark->row;
  new_doc[i].c_s = page->Page_Typo_Mark->gouttiere_x;
  new_doc[i].r_s = page->Page_Typo_Mark->gouttiere_y;
      
  if (list_page->Page == (*Lpage)->Page)
  {
   new_doc[i].doc_name = (char *)1;/* current page */
   selpg = i;
  }
  else
  {
   new_doc[i].doc_name = NULL;
  }
  list_page = list_page->NextPage;
 }
  
 box = (c_Box_page *)NEW(c_Box_page)(MainInterface1->w_Main, 
				     "BoxPage", new_doc,
				     This->NbrPage, selpg);
  
 switch(F(box->Dialog).waitForUnmap(box->Dialog))
 {
 case  XcCD_CANCEL:
 case XcCD_DESTROY:
  DELETE(c_Box_page)(box);
  Xc_free(new_doc);
  return FALSE;
 }
 DELETE(c_Box_page)(box);
  
 list_page = This->ListPage;
 for(i=0; i<This->NbrPage; i++)
 {
  page = list_page->Page;
  Xc_TRACE(("doc_name: %d", (long)(new_doc[i].doc_name)));
  if (new_doc[i].doc_name)
  {
   if ((long)new_doc[i].doc_name & 1)
    *Lpage = list_page;
   if ((long)new_doc[i].doc_name & 2)
   {
    Xc_TRACE(("coucou"));
    if (new_doc[i].p_do)
     type = Xc_PAGE_DOUBLE;
    else
     if (new_doc[i].p_rv)
      type = Xc_PAGE_LEFT;
     else
      type = Xc_PAGE_RIGHT;
    if (!(type == page->page_type &&
	  new_doc[i].p_f == page->page_format &&
	  new_doc[i].p_o == page->page_orient &&
	  new_doc[i].le_m == page->Page_Typo_Mark->left_margin &&
	  new_doc[i].ri_m == page->Page_Typo_Mark->right_margin &&
	  new_doc[i].up_m == page->Page_Typo_Mark->top_margin && 
	  new_doc[i].lo_m == page->Page_Typo_Mark->bottom_margin &&
	  new_doc[i].n_c == page->Page_Typo_Mark->column && 
	  new_doc[i].n_r == page->Page_Typo_Mark->row &&
	  new_doc[i].c_s == page->Page_Typo_Mark->gouttiere_x &&
	  new_doc[i].r_s == page->Page_Typo_Mark->gouttiere_y))
    {
     Xc_TRACE(("modify page %d", page->page_number));
     F(page).init_Page(page, page->page_number,
		       new_doc[i].p_f, type,
		       new_doc[i].p_o, new_doc[i].le_m,
		       new_doc[i].ri_m, new_doc[i].up_m,
		       new_doc[i].lo_m, new_doc[i].n_c,
		       new_doc[i].n_r, new_doc[i].c_s,
		       new_doc[i].r_s,
		       page->Page_Grid.sOrig_X - page->Page_Contour.sOrig_X,
		       page->Page_Grid.sOrig_Y - page->Page_Contour.sOrig_Y,
		       page->Page_Grid.sEcart_X,
		       page->Page_Grid.sEcart_Y,
		       XPi_MODIFY); 
    }
   }
  }
  list_page = list_page->NextPage;
 }
  
 Xc_free(new_doc);
 recalcul_Page_number(This, This->ListPage, 1);
 This->modified = TRUE;
  
 /*  new_doc.page_nbr = page->page_number;
     new_doc.p_f = page->page_format;
     switch(page->page_type)
     {
     case Xc_PAGE_DOUBLE:
     new_doc.p_do = TRUE;
     new_doc.p_rv = FALSE;
     break;
     case Xc_PAGE_LEFT:
     new_doc.p_do = FALSE;
     new_doc.p_rv = TRUE;
     break;
     case Xc_PAGE_RIGHT:
     new_doc.p_do = FALSE;
     new_doc.p_rv = FALSE;
     break;
     }
     new_doc.p_o = page->page_orient; 
     new_doc.le_m = page->Page_Typo_Mark->left_margin;
     new_doc.ri_m = page->Page_Typo_Mark->right_margin;
     new_doc.up_m = page->Page_Typo_Mark->top_margin;
     new_doc.lo_m = page->Page_Typo_Mark->bottom_margin;
     new_doc.n_c = page->Page_Typo_Mark->column;
     new_doc.n_r = page->Page_Typo_Mark->row;
     new_doc.c_s = page->Page_Typo_Mark->gouttiere_x;
     new_doc.r_s = page->Page_Typo_Mark->gouttiere_y;
     
     memcpy(&def_doc, &new_doc, sizeof(new_doc_s));
     box = (c_Box_page *)NEW(c_Box_page)(MainInterface1->w_Main, 
     "BoxPage", &new_doc);
     
     if (F(box->Dialog).waitForUnmap(box->Dialog) == XcCD_CANCEL)
     {
     DELETE(c_Box_page)(box);
     return FALSE;
     }
     DELETE(c_Box_page)(box);
     
     if (!memcmp(&new_doc, &def_doc, sizeof(new_doc_s)))
     return FALSE;
     
     if (new_doc.p_do)
     type = Xc_PAGE_DOUBLE;
     else
     if (new_doc.p_rv)
     type = Xc_PAGE_LEFT;
     else
     type = Xc_PAGE_RIGHT;
     
     F(page).init_Page(page, page->page_number,
     new_doc.p_f, type,
     new_doc.p_o, new_doc.le_m,
     new_doc.ri_m, new_doc.up_m,
     new_doc.lo_m, new_doc.n_c,
     new_doc.n_r, new_doc.c_s,
     new_doc.r_s, XPi_MODIFY); 
     recalcul_Page_number(This, Lpage, page->page_number);
     */
 Xc_TRACE(("modify_Page done"));
 return TRUE;
}

static void delete_Page(This,Lpage)
c_Document	*This;
listpage	*Lpage;
{
 listpage	*NxtLpage;
 int   num_page;
 Xc_TRACE(("delete_Page"));
  
 num_page = Lpage->Page->page_number;
 NxtLpage = Lpage->NextPage;
 F(This).DelPage(This, Lpage);
 recalcul_Page_number(This, NxtLpage, num_page);
 This->modified = TRUE;
}

static boolean goto_Page(This, Lpage)
c_Document *This;
listpage **Lpage;
{
 c_Box_gotopage *box;
 int page, nb_page;
 listpage *lpage;
 Xc_TRACE(("goto_Page"));
 
 if (*Lpage)
  page = (*Lpage)->Page->page_number;
 else
  page = 1;
 
 lpage = This->ListPage;
 while(lpage->NextPage)
  lpage = lpage->NextPage;

 nb_page = lpage->Page->page_number + 
  (lpage->Page->page_type == Xc_PAGE_DOUBLE);

 box = (c_Box_gotopage *)NEW(c_Box_gotopage)(MainInterface1->w_Main, 
					     "BoxGotoPage", nb_page, &page);
 
 switch(F(box->Dialog).waitForUnmap(box->Dialog))
 {
 case  XcCD_CANCEL:
 case XcCD_DESTROY:
  DELETE(c_Box_gotopage)(box);
  return FALSE;
 }
 DELETE(c_Box_gotopage)(box);
 
 if (page > nb_page || page == 0) return FALSE;
 
 lpage = This->ListPage;
 while(lpage)
 {
  if (lpage->Page->page_number == page ||
      (lpage->Page->page_type == Xc_PAGE_DOUBLE && 
       lpage->Page->page_number + 1 == page))
   break;

  lpage = lpage->NextPage;
 }
 
 if (lpage && lpage != *Lpage)
 {
  *Lpage = lpage;
  return TRUE;
 }
 return FALSE;
}

static listpage *get_previous_page(This, list_page_def)
c_Document	*This;
listpage	*list_page_def;
{
 listpage	*list_page;
  
 if (!list_page_def)
  list_page_def = This->SelectedPage;
 list_page = This->ListPage;
 if (list_page == list_page_def)
  return list_page_def;
 while(list_page->NextPage != list_page_def)
  list_page = list_page->NextPage;
 return list_page; 
}
