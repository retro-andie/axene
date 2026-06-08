/*
** Grid.c for Xclamation, XQuad, XAllWrite and XMayday in Grid/
** Methods for the Grid class
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
** Started on  Tue Dec  6 11:40:44 1994 Stéphane Boisson
** Last update Sun Jan 16 15:55:37 2000 Emmanuel Paris
*/

/*#define NTRACE*/
#define NHISTORY

#include "Grid.h"
#include "RegisterHelp.h"
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <Xm/BulletinB.h>
#include <Xm/Frame.h>
#include <varargs.h>

/*#define XcG_INSTANT_MAP*/

#define BBOARD_RESIZE "<Configure>: XcG_resize(%ld)"

static void *constructor();
static void destructor();
static void *copy();
static void addColumn ___NPROTO((c_Grid *this, int size, ...));
static void callResize ___PROTO((c_Grid *this));

static void cb_resize ___PROTO((CompositeWidget w, XConfigureEvent *event,
				char *args[], int *nargs));
static void cb_popup ___PROTO((Widget w, c_Grid *this));

sf_Grid fc_Grid =
{
 constructor, destructor, copy,
 addColumn, callResize
};


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(parent, manager, name, vertical_flag)
Widget parent;
c_ManageWidget *manager;
char *name;
boolean vertical_flag;
{
 XtActionsRec rec;
 c_Grid *this;
 char trans[80];

 Xc_HISTORY(("constructor(`%s')", name));

 if((this = (c_Grid *)Xc_malloc("Grid", sizeof(c_Grid))) == NULL) return NULL;
 memset(this, 0, sizeof(c_Grid));
 this->f = &fc_Grid;
 this->vertical_flag = vertical_flag;
 this->width = this->height = -1;

 WIDGET(this) = XtVaCreateWidget(name, xmBulletinBoardWidgetClass, parent,
				 XmNallowOverlap, False,
				 XmNresizePolicy, XmRESIZE_ANY,
#ifdef XcG_INSTANT_MAP
				 XmNmappedWhenManaged, False,
#else
				 XmNmappedWhenManaged, True,
#endif
				 NULL);
  
#ifndef NTRACE
 {
  Boolean gato;
  XtVaGetValues(WIDGET(this), XmNmappedWhenManaged, &gato, NULL);
  Xc_TRACE(("XmNmappedWhenManaged=%s", gato? "true" : "false"));
 }
#endif
 Xc_REGISTER_HELP(WIDGET(this));
 if(manager) F(manager).Add_child(manager, WIDGET(this), parent);

 rec.string = "XcG_resize";
 rec.proc = (void *)cb_resize;
 XtAppAddActions(XtWidgetToApplicationContext(WIDGET(this)), &rec, 1);

 sprintf(trans, BBOARD_RESIZE, (long)this);
 XtOverrideTranslations(WIDGET(this), XtParseTranslationTable(trans));

 while(XtIsShell(parent) == False) parent = XtParent(parent);
 XtAddCallback(parent, XtNpopupCallback, (XtCallbackProc)cb_popup, this);

 Xc_TRACE(("constructor end"));
 return this;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_Grid *this;
{
 Xc_HISTORY(("destructor"));

 XtDestroyWidget(WIDGET(this));
 Xc_free(this);
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this)
c_Grid *this;
{
 Xc_HISTORY(("copy"));
 Xc_FATAL(("not implemented"));
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** addColumn - Add a column of widgets                               ** 
** ----------------------------------------------------------------- */
static void addColumn(this, size, va_alist)
c_Grid *this;
int size;
va_dcl
{
 Dimension row_width, row_height;
 grid_item_t *column;
 int count;
 va_list ap;

 Xc_HISTORY(("addColumns"));
 Xc_ASSERT(this->column_count < XcG_MAX_COLUMNS);

 column = this->children[this->column_count];
 this->row_count[this->column_count][1] = 0;
 row_width = row_height = 0;

 va_start(ap);
 for(count = 0; (column->w = va_arg(ap, Widget)) != NULL; column++, count++)
 {
  XtWidgetGeometry geometry;
  Dimension width, height;
 
  Xc_ASSERT(count < XcG_MAX_ROWS);

  geometry.request_mode = CWHeight|CWWidth|CWBorderWidth;
  if (column->w == XcG_WIDGET_NULL)
  {
   geometry.height = geometry.width = geometry.border_width = 0;
  }
  else
  {
   XtQueryGeometry(column->w, NULL, &geometry);
   if (XtClass(column->w) == xmFrameWidgetClass)
   {
    Dimension w, h;
    
    XtVaGetValues(column->w, XmNwidth, &w, XmNheight, &h, NULL);

    if (geometry.width < w)
     geometry.width = w;
    if (geometry.height < h)
     geometry.height = h;
   }
  }
  column->border = geometry.border_width;
  height = geometry.height + geometry.border_width;
  width = geometry.width + geometry.border_width;
      
  Xc_TRACE(("widget=%lx %dx%d border=%d", column->w,
	    geometry.width, geometry.height, geometry.border_width));

  column->size = va_arg(ap, int);
  if(this->vertical_flag == FALSE)
  {
   if(row_width < width) row_width = width;
   if(column->size >= 0)
   {
    if(column->size == 0) column->size = height;
    row_height += column->size;
    this->row_count[this->column_count][1]++;
   }
  }
  else
  {
   if(row_height < height) row_height = height;
   if(column->size >= 0)
   {
    if(column->size == 0) column->size = width;
    row_width += column->size;
    this->row_count[this->column_count][1]++;
   }
  }
 }
 va_end(ap);

 if(this->vertical_flag == FALSE)
 {
  if(size >= 0)
  {
   if(size == 0) size = row_width;
   this->column_size += size; 
   this->column_size_count++;
  }
  this->row_sizes[this->column_count][1] = row_height;
 }
 else
 {
  if(size >= 0)
  {
   if(size == 0)  size = row_height;
   this->column_size += size; 
   this->column_size_count++;
  }
  this->row_sizes[this->column_count][1] = row_width;
 }
 this->row_sizes[this->column_count][0] = size; 
 this->row_count[this->column_count++][0] = count;
 Xc_TRACE(("end addColumn"));
}

/* ----------------------------------------------------------------- ** 
** callResize - call manualy cb popup                                ** 
** ----------------------------------------------------------------- */
static void callResize(this)
c_Grid *this;
{
 this->force_resize = TRUE;
 cb_popup(WIDGET(this), this);
}

/* ----------------------------------------------------------------- ** 
** cb_popup - XtPopup callback                                       ** 
** ----------------------------------------------------------------- */
static void cb_popup(wid, this)
Widget wid;
c_Grid *this;
{
 XConfigureEvent event;
 Dimension w, h;

 Xc_TRACE(("cb_popup"));

 XtVaGetValues(WIDGET(this), XmNwidth, &w, XmNheight, &h, NULL);
 event.width = w;
 event.height = h;
 Xc_TRACE(("%dx%d", w, h));

 cb_resize((CompositeWidget)WIDGET(this), &event, (char **)this, NULL);
}


/* ----------------------------------------------------------------- ** 
** cb_resize - Bulletin board resize callback                        ** 
** ----------------------------------------------------------------- */
static void cb_resize(bboard, event, args, nargs)
CompositeWidget bboard;
XConfigureEvent *event;
char	*args[];
int	*nargs;
{
 Dimension margin_w, margin_h, h, w, y, x;
 int i, j;
 grid_item_t *row;
 c_Grid *this;

 Xc_TRACE(("cb_resize(#%lx %dx%d)", (long)bboard,
	   event->width, event->height));
 
 if (nargs == NULL)
  this = (c_Grid *)args;
 else 
 {
  if (*nargs != 1)
   return;
  else
   this = (c_Grid *)atol(args[0]);
 }
 
 XtVaGetValues((Widget)bboard,
	       XmNmarginWidth, &margin_w,
	       XmNmarginHeight, &margin_h,
	       NULL);

 if((this->width != event->width) || (this->height != event->height) ||
    this->force_resize)
 {
  this->force_resize = FALSE;
  this->width = event->width;
  this->height = event->height;
 }
 else
 {
  Xc_TRACE(("size already set"));

#ifdef XcG_INSTANT_MAP
  /*--- Map widget ---*/
  Xc_TRACE(("map grid"));
  if(XtIsManaged((Widget)bboard) && XtIsRealized((Widget)bboard))
  {
   Xc_TRACE((" XtSetMappedWhenManaged"));
	  
   XtSetMappedWhenManaged((Widget)bboard, True);
  }
#endif
  Xc_TRACE(("cb_resize(#%lx) done", (long)bboard));
  return;
 }

 if((event->height <= margin_h) || (event->width <= margin_w))
 {
  Xc_TRACE(("space too tight"));
  /*return;*/
 }

 h = event->height - margin_h;
 w = event->width - margin_w;

 if(this->vertical_flag == FALSE)
 {
  x = margin_w >> 1;
  if(w <= this->column_size)
  {
   Xc_TRACE(("space too tight"));
   return;
  }
  w -= this->column_size;
  if(this->column_count != this->column_size_count)
   w /= this->column_count - this->column_size_count;

  /*--- Rows ---*/
  for(i = 0; i < this->column_count; i++)
  {
   int rh, rw;
	  
   rh = (this->row_sizes[i][1] < h)? h - this->row_sizes[i][1] : 0;
   rw = (this->row_sizes[i][0] <= 0)? w : this->row_sizes[i][0];

   Xc_TRACE(("row #%d/%d h=%d", i + 1, this->column_count, rh));

   if(this->row_count[i][0] != this->row_count[i][1])
    rh /= this->row_count[i][0] - this->row_count[i][1];
   row = this->children[i];
   y = margin_h >> 1;
	  
   /*--- Columns ---*/
   for(j = this->row_count[i][0]; j--; row++)
   {
    int h, w;

    w = (rw > row->border)? rw : row->border + 1;
    h = (row->size <= 0)? rh : row->size;
    if(h <= row->border) h = row->border + 1;
	      
    Xc_TRACE((" child #%d/%d x=%d y=%d w=%d h=%d b=%d",
	      this->row_count[i][0] - j, this->row_count[i][0], x, y,
	      w - row->border, h - row->border, row->border));
    if (row->w != XcG_WIDGET_NULL)
     XtConfigureWidget(row->w, x, y, w - row->border,
		       h - row->border, row->border);
    y += h;
   }
   x += rw;
  }
 }
 else
 {
  y = margin_h >> 1;
  if(h <= this->column_size)
  {
   Xc_TRACE(("space too tight"));
   /*return;*/
  }

  h -= this->column_size;
  if(this->column_count != this->column_size_count)
   h /= this->column_count - this->column_size_count;

  /*--- Columns ---*/
  for(i = 0; i < this->column_count; i++)
  {
   int rh, rw;
	  
   rh = (this->row_sizes[i][0] <= 0)? h : this->row_sizes[i][0];
   rw = (this->row_sizes[i][1] < w)? w - this->row_sizes[i][1] : 0;

   Xc_TRACE(("column #%d/%d w=%d", i + 1, this->column_count, rw));

   if(this->row_count[i][0] != this->row_count[i][1])
    rw /= this->row_count[i][0] - this->row_count[i][1];
   row = this->children[i];
   x = margin_w >> 1;
	  
   /*--- Rows ---*/
   for(j = this->row_count[i][0]; j--; row++)
   {
    int w, h;

    h = (rh > row->border)? rh : row->border + 1;
    w = (row->size <= 0)? rw : row->size;
    if(w <= row->border) w = row->border + 1;

    Xc_TRACE((" child #%d/%d x=%d y=%d w=%d h=%d b=%d", 
	      this->row_count[i][0] - j, this->row_count[i][0], x, y,
	      w - row->border, h - row->border, row->border));
    if (row->w != XcG_WIDGET_NULL)
     XtConfigureWidget(row->w, x, y,
		       w - row->border, h - row->border,
		       row->border);
    x += w;
   }
   y += rh;
  }
 }


#ifdef XcG_INSTANT_MAP
 /*--- Map widget ---*/
 Xc_TRACE(("map grid"));
 if(XtIsManaged((Widget)bboard) && XtIsRealized((Widget)bboard))
 {
  Xc_TRACE((" XtSetMappedWhenManaged"));

  /*XtMapWidget((Widget)bboard);*/
  XtSetMappedWhenManaged((Widget)bboard, True);
 }
#endif

#ifndef NTRACE
 {
  Dimension w, h;
    
  XtVaGetValues((Widget)bboard, XmNwidth, &w, XmNheight, &h, NULL);
  Xc_TRACE(("check #%lx %dx%d", (long)bboard, w, h));
 }
#endif

 Xc_TRACE(("cb_resize(#%lx) done", (long)bboard));
} 
