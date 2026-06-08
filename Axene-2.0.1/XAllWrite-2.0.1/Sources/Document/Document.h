/*
** Document.h for XAllWrite in Document/
** Declaration of the Document class
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
** Started on  Mon Jul 17 15:05:00 1995 Antoine Buat
** Last update Thu Oct  8 16:26:38 1998 Emmanuel Paris
*/


#ifndef _Document_h
#define _Document_h

typedef struct sc_Document c_Document;

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include "xcalibur.h"
#include "DocFile.h"
#include "PostScript.h"
#include "DocumentStd.h"
#include "PageArea.h"
#include "ScrollBarX.h"
#include "Pager.h"
#include "XaRegle.h"
#include "BaseStd.h"

/*--- forward declarations ---*/
struct sfs_Document;

/*--- define the Document Class ---*/
struct sc_Document
{
 struct sfs_Document *f;   /* must be in first position      */

 Display	*display;  /* mustn't be move !!!!!!!!!      */
 Window	        window;	   /* this can identify the Document */
 boolean	modified;  /* idem */
 
 int                    PosX;
 int                    PosY;
 int                    Width;
 int			Height;
 boolean		Maximized;

 /*___Document user info___*/
 char		name[XcMAX_NAME_LENGTH];
 char		creator[XcMAX_NAME_LENGTH];
 char		*filename;
 char		version[10];
 char		osname[31];
 char		started_on_date[31];
 char		last_update_date[31];

 /*___components part___*/
 c_ScrollBarX		*ScrollBarV;
 c_ScrollBarX		*ScrollBarH;
 c_XaRegle		*RegleV;
 c_XaRegle		*RegleH;
 c_PageArea		*PageArea;
 c_Pager		*Pager;
 
 /*___configuration___*/ 
 boolean		scrollbh_on;
 boolean		scrollbv_on;
 boolean		pager_on;
 boolean		regle_on;
 
 /*___Document ressources___*/
 BaseStd_t	       BaseStd;

 /*___private part___*/
 GC			gc;
};

/*--- define the method for Class Document ---*/
typedef struct sfs_Document
{
 F_STD;
 F_DOCUMENT_STD;
 void (*Init_Document) ___PROTO((c_Document *This, Display *display,
				 Window window, boolean visible, 
				 char *name));
 void (*Update_Document) ___PROTO((c_Document *This));
} sf_Document;

extern sf_Document fc_Document;

#define DTAB_SIZEX	360
#define DTAB_SIZEY	200
#define DTAB_POSX	5
#define DTAB_POSY	5
#define DTAB_COLOR	"gray60"
#define DTAB_SHADOW	"gray40"
#define REGLE_SIZE	30
#define	BUTTON_SIZE	18

#endif




