/*
** Document.h for Xclamation in Document/
** Declaration of the Document class
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
** Started on  Fri Sep 30 15:31:18 1994 Antoine Buat
** Last update Fri Oct 30 15:32:32 1998 Emmanuel Paris
*/


#ifndef _Document_h
#define _Document_h

typedef struct listPAG listpage;
typedef struct sc_Document c_Document;

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include "xcalibur.h"
#include "Regle.h"
#include "ScrollBarX.h"
#include "Page.h"
#include "Pager.h"
#include "PostScript.h"
#include "DocumentStd.h"
#include "PageListStd.h"
#include "BaseStd.h"

/*--- Orientation ---*/
#define PORTRAIT	0
#define LANDSCAPE	1

/*--- PageFormat ---*/
#define DOUBLE		0
#define SINGLE		1

/*--- PageSide ---*/
#define RECTO_ONLY	0
#define RECTO_VERSO	1

/*--- define the listpage type ---*/
struct listPAG
{
  struct listPAG	*NextPage;
  c_Page		*Page;
};

/*--- forward declarations ---*/
struct sfs_Document;

/*--- define the Document Class ---*/
struct sc_Document
{
 struct sfs_Document *f;   /* must be in first position      */

 Display	*display;  /* mustn't be move !!!!!!!!!      */
 Window	        window;	   /* this can identify the document */
 boolean	modified;  /* idem.. */
 
 int		PosX;
 int		PosY;
 int            Width;
 int            Height;
 
 /*___Document user info___*/
 char		name[XcMAX_NAME_LENGTH];
 char		creator[XcMAX_NAME_LENGTH];
 char		*filename;
 char		version[10];
 char		osname[31];
 char		started_on_date[31];
 char		last_update_date[31];

 c_Regle	*RegleH;
 c_Regle	*RegleV;
 c_Regle	*RegleC;
 c_Pager	*Pager;
 c_ScrollBarX	*ScrollBarV;
 c_ScrollBarX	*ScrollBarH;

 listpage	*ListPage;

 /*___configuration___*/
 boolean	visible;
 boolean	regle_ok;
 boolean	scrollbh_on;
 boolean	scrollbv_on;

 /*___used for the view___*/
 listpage	*SelectedPage;
 int		NbrPage;

 /*___Document ressources___*/
 BaseStd_t	BaseStd;
};


/*--- define the method for Class Document ---*/
typedef struct sfs_Document
{
  F_STD;
  F_DOCUMENT_STD;
  void (*Init_Document) ___PROTO((c_Document *This, Display *display,
				  Window window, boolean visible,
				  char *name));
  void (*AddPage) ___PROTO((c_Document *This, c_Page *Page));
  void (*DelPage) ___PROTO((c_Document *This, listpage *Lpage));
  void (*InsPage) ___PROTO((c_Document *This, c_Page *Page, listpage *Lpage));

  void (*InsertPage) ___PROTO((c_Document *This, listpage *Lpage, 
			       boolean before));
  boolean (*ModifyPage) ___PROTO((c_Document *This, listpage **Lpage));
  void (*DeletePage) ___PROTO((c_Document *This, listpage *Lpage));
  boolean (*GotoPage) ___PROTO((c_Document *This, listpage **Lpage));
} sf_Document;

extern sf_Document fc_Document;

#define DDOC_SIZEX	360
#define DDOC_SIZEY	200
#define DDOC_POSX	5
#define DDOC_POSY	5
#define DDOC_COLOR	"gray40"

#define XcDF_DISPLAY_RULERS_KEYWORD	"DISPLAY_RULERS"

#endif




