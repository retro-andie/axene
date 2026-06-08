/*
** Box_attrib.c for Xclamation, XQuad and XAllWrite in Box_attrib/
** Object and method for Box_attrib Class
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
** Started on  Mon Dec  5 16:33:17 1994 Robin Castanier
** Last update Tue Jan 18 11:22:36 2000 Emmanuel Paris
*/

#include "Box_attrib.h"
#include "Color.h"
#include <Xm/DrawingA.h>
#include <X11/Xlib.h>

void *cons_Box_attrib();
void dest_Box_attrib();
void *copy_Box_attrib();
static void BaChange_Sel();
static void changebackcolor();
static void changebordercolor();
static void certify_text ___PROTO((c_WTextField *this, c_Box_attrib *Obj));
static void draw_view ___PROTO((Widget w, c_Box_attrib *Obj,
			       XmDrawingAreaCallbackStruct *cbs));
static void cb_action ___PROTO((c_CustomDialog	*cb_dialog,
				c_Box_attrib 	*box,
				int		cb_reason));
static void cb_change ___PROTO((c_Box_attrib	*box,
				c_Cadre		*color,
				int		type,
				int		num,
				void 		*data));

sf_Box_attrib fc_Box_attrib =
{
 cons_Box_attrib,
 dest_Box_attrib,
 copy_Box_attrib
};

/* --------- Constructor of the object ... -------------- */

void *cons_Box_attrib(w_Parent, pszTitle, color_base, list_cadre)
Widget	w_Parent;
char	*pszTitle;
c_Color *color_base;
l_Cadre	*list_cadre;
{
 c_Box_attrib	*ObjTmp;
 Arg		args[10];
 int		nargs;
 XmString      strTitre;
 char		str_W[4];
  
 if ((ObjTmp = (c_Box_attrib *)Xc_malloc("Box_attrib",
					 sizeof(c_Box_attrib))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_Box_attrib;
 ObjTmp->selectedcadre = list_cadre;
 ObjTmp->lcadre = list_cadre;
 ObjTmp->list_save = (c_ModifList *)
  NEW(c_ModifList)(ObjTmp, cb_change,
		   4,
		   (int)(offsetof(c_Cadre, border_color)),
		   (int)sizeof(c_Color *),
		   (int)(offsetof(c_Cadre, inside_color)),
		   (int)sizeof(c_Color *),
		   (int)(offsetof(c_Cadre, thickness)),
		   (int)sizeof(coord_t),
		   (int)(offsetof(c_Cadre, name)),
		   (int)XcMAX_NAME_LENGTH);

 ObjTmp->PBox = NEW(c_ASLBox)(w_Parent, pszTitle, 580, 300, TRUE, 
			      offsetof(l_Cadre, name));

 strTitre = F(GlobResources).getXmString(GlobResources, XcR_frameAttribs);
 XtSetArg(args[0], XmNlabelString, strTitre);
 XtSetValues(WIDGET(ObjTmp->PBox->Label), args, 1);
 XmStringFree(strTitre);

 F(ObjTmp->PBox).setCallback(ObjTmp->PBox, BaChange_Sel, ObjTmp);

 WIDGET(ObjTmp) = WIDGET(ObjTmp->PBox);

 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "gBoxAttrib", FALSE);

 ObjTmp->G_param = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Grid),
					 MANAGER(ObjTmp->PBox->Dialog),
					 "gBAparam", TRUE);
 
 ObjTmp->Lback_color = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->G_param),
					       MANAGER(ObjTmp->PBox->Dialog),
					       "lBABackColor");
  
 ObjTmp->menubackcolor = NEW(c_ColorList)(WIDGET(ObjTmp->G_param),
					  MANAGER(ObjTmp->PBox->Dialog),
					  "clBABackColor", XcCL_NORMAL);
  
 F(ObjTmp->menubackcolor).setCallback(ObjTmp->menubackcolor,
				      changebackcolor, ObjTmp);
  
 F(ObjTmp->menubackcolor).setColorBase(ObjTmp->menubackcolor,
				       color_base, CL_KEEP_TRANSPARENCY);
  
 ObjTmp->Lborder_color = (c_Label *)NEW(c_Label)
  (WIDGET(ObjTmp->G_param),
   MANAGER(ObjTmp->PBox->Dialog),
   "lBABorderColor");
 ObjTmp->menubordercolor = NEW(c_ColorList)(WIDGET(ObjTmp->G_param),
					    MANAGER(ObjTmp->PBox->Dialog),
					    "clBABorderColor", XcCL_NORMAL);
 F(ObjTmp->menubordercolor).setCallback(ObjTmp->menubordercolor,
					changebordercolor, ObjTmp);
 F(ObjTmp->menubordercolor).setColorBase(ObjTmp->menubordercolor,
					 color_base, CL_KEEP_TRANSPARENCY);

 F(ObjTmp->menubackcolor).setColor(ObjTmp->menubackcolor, 
				   list_cadre->cadre->inside_color);
 F(ObjTmp->menubordercolor).setColor(ObjTmp->menubordercolor, 
				     list_cadre->cadre->border_color);

 ObjTmp->Lborder_width = (c_Label *)NEW(c_Label)
  (WIDGET(ObjTmp->G_param),
   MANAGER(ObjTmp->PBox->Dialog),
   "lBABorderWidth");
 sprintf(str_W, "%g", SCALE_TO_POINTS(list_cadre->cadre->thickness));
 ObjTmp->Wborder_width = (c_WTextField *)
  NEW(c_WTextField) (WIDGET(ObjTmp->G_param),
		     MANAGER(ObjTmp->PBox->Dialog),
		     "tfBABorderWidth", TF_REAL,
		     0.0, 99.0,
		     str_W, NULL, 8);
 F(ObjTmp->Wborder_width).set_unit(ObjTmp->Wborder_width,
				   XcUB_LENGTH_POINT, NULL, TRUE, FALSE);
 F(ObjTmp->Wborder_width).add_callback(ObjTmp->Wborder_width,
				       (void *)certify_text,
				       ObjTmp);

 ObjTmp->F_color = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					   MANAGER(ObjTmp->PBox->Dialog), 
					   "fBAPreview", 0);

 nargs = 0;
 ObjTmp->drawing = XmCreateDrawingArea(WIDGET(ObjTmp->F_color),
				       "daBoxAttrib", args, nargs);
 XtAddCallback(ObjTmp->drawing, XmNexposeCallback,
	       (XtCallbackProc)draw_view, ObjTmp);
 ObjTmp->gc = XCreateGC(XtDisplay(ObjTmp->drawing),
			RootWindowOfScreen(XtScreen(ObjTmp->drawing)),
			0, NULL);
 F(MANAGER(ObjTmp->PBox->Dialog)).Add_child(MANAGER(ObjTmp->PBox->Dialog),
					    ObjTmp->drawing,
					    WIDGET(ObjTmp->F_color));
  
 F(ObjTmp->G_param).addColumn(ObjTmp->G_param, XcG_MAX_SIZE, NULL);
 F(ObjTmp->G_param).addColumn(ObjTmp->G_param, 30,
			      WIDGET(ObjTmp->Lback_color), XcG_MAX_SIZE,
			      WIDGET(ObjTmp->menubackcolor), 122,
			      NULL);
 F(ObjTmp->G_param).addColumn(ObjTmp->G_param, XcG_MAX_SIZE, NULL);
 F(ObjTmp->G_param).addColumn(ObjTmp->G_param, 30,
			      WIDGET(ObjTmp->Lborder_color), XcG_MAX_SIZE,
			      WIDGET(ObjTmp->menubordercolor), 122,
			      NULL);
 F(ObjTmp->G_param).addColumn(ObjTmp->G_param, XcG_MAX_SIZE, NULL);
 F(ObjTmp->G_param).addColumn(ObjTmp->G_param, XcG_BEST_SIZE,
			      WIDGET(ObjTmp->Lborder_width), XcG_MAX_SIZE,
			      WIDGET(ObjTmp->Wborder_width), XcG_BEST_SIZE,
			      NULL);
 F(ObjTmp->G_param).addColumn(ObjTmp->G_param, XcG_MAX_SIZE, NULL);

 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->G_param), XcG_MAX_SIZE,
			   XcG_WIDGET_NULL, 10,
			   WIDGET(ObjTmp->F_color), XcG_MAX_SIZE,
			   NULL);

 F(ObjTmp->PBox->ItemList).addItems(ObjTmp->PBox->ItemList, FALSE,
				    XcIL_LINKED_LIST, -1,
				    list_cadre, offsetof(l_Cadre, NextCadre));
 F(ObjTmp->PBox->Dialog).createActionArea(ObjTmp->PBox->Dialog,
					  XcCD_OK | XcCD_CANCEL, XcCD_OK,
					  TRUE,
					  (void (*)())cb_action, ObjTmp);
 F(ObjTmp->PBox).Map(ObjTmp->PBox, TRUE, list_cadre);
 Xc_TRACE(("Object Box_attrib build"));
 return ObjTmp;
}

/* ----------------- Destructor of the object ---------------- */

void dest_Box_attrib(This)
c_Box_attrib *This;
{
 l_Cadre	*lcadre;
  
 XtDestroyWidget(This->drawing);
 DELETE(c_ColorList)(This->menubackcolor);
 DELETE(c_ColorList)(This->menubordercolor);
 DELETE(c_Label)(This->Lback_color);
 DELETE(c_Label)(This->Lborder_color);
 DELETE(c_WTextField)(This->Wborder_width);
 DELETE(c_Label)(This->Lborder_width);
 DELETE(c_Frame)(This->F_color);
 DELETE(c_Grid)(This->G_param);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_ASLBox)(This->PBox);
 DELETE(c_ModifList)(This->list_save);
 XFreeGC(XtDisplay(WIDGET(This)), This->gc);

 lcadre = This->lcadre;
 while(lcadre)
 {
  lcadre->cadre->data_to_object->redraw_cadre
   (lcadre->cadre->data_to_object->page, lcadre->cadre); 
  lcadre = lcadre->NextCadre;
 }
  
 Xc_free(This);
 Xc_TRACE(("Object Box_attrib destroyed"));
}

/* ------------ copy method for the object .... --------------- */

void *copy_Box_attrib(This)
c_Box_attrib *This;
{
 Xc_TRACE(("Copy not implemented"));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** Change_Sel - Callback for reason Selection had change             ** 
** ----------------------------------------------------------------- */
static void BaChange_Sel(this, box_attrib, lcadre)
c_ASLBox	*this;
c_Box_attrib	*box_attrib;
l_Cadre		*lcadre;
{
 c_Color	*tmp_color;
 coord_t	thick;

 box_attrib->selectedcadre = lcadre;

 if (F(box_attrib->list_save).get(box_attrib->list_save,
				  lcadre->cadre,
				  1,
				  &tmp_color) == -1)
  tmp_color = lcadre->cadre->inside_color;
 F(box_attrib->menubackcolor).setColor(box_attrib->menubackcolor, tmp_color);

 if (F(box_attrib->list_save).get(box_attrib->list_save,
				  lcadre->cadre,
				  0,
				  &tmp_color) == -1)
  tmp_color = lcadre->cadre->border_color;
 F(box_attrib->menubordercolor).setColor
  (box_attrib->menubordercolor, tmp_color);

  
 if (F(box_attrib->list_save).get(box_attrib->list_save,
				  lcadre->cadre,
				  2,
				  &thick) == -1)
  thick = lcadre->cadre->thickness;
  
 F(box_attrib->Wborder_width).set_value_in_unit
  (box_attrib->Wborder_width, SCALE_TO_POINTS(thick));
  
 if (XtWindow(box_attrib->drawing))
  XClearArea(XtDisplay(box_attrib->drawing),
	     XtWindow(box_attrib->drawing),
	     0, 0, 0, 0, True);
}

static void changebackcolor(this, box, color)
c_ColorList	*this;
c_Box_attrib	*box;
c_Color		*color;
{
 l_Cadre	**lcadre;
 int		num2, i;
  
 if(F(box->PBox->ItemList).getSelection(box->PBox->ItemList,
					(void ***)&lcadre, &num2))
 {
  for(i=0; i<num2; i++)
  {
   F(box->list_save).set(box->list_save,
			 lcadre[i]->cadre,
			 1,
			 &color);
  }
  if (num2)
   Xc_free(lcadre);
 } 
  
 if (XtWindow(box->drawing))
  XClearArea(XtDisplay(box->drawing),
	     XtWindow(box->drawing),
	     0, 0, 0, 0, True);
}  

static void changebordercolor(this, box, color)
c_ColorList	*this;
c_Box_attrib	*box;
c_Color		*color;
{
 l_Cadre	**lcadre;
 int		num2, i;
  
 if(F(box->PBox->ItemList).getSelection(box->PBox->ItemList,
					(void ***)&lcadre, &num2))
 {
  for(i=0; i<num2; i++)
  {
   F(box->list_save).set(box->list_save,
			 lcadre[i]->cadre,
			 0,
			 &color);
  }
  if (num2)
   Xc_free(lcadre);
 } 
  
 if (XtWindow(box->drawing))
  XClearArea(XtDisplay(box->drawing),
	     XtWindow(box->drawing),
	     0, 0, 0, 0, True);
}


static void certify_text(this, Obj)
c_WTextField	*this;
c_Box_attrib	*Obj;
{
 l_Cadre	**lcadre;
 int		num2, i;
 coord_t	numf;
  
 numf = F(this).get_value_in_unit(this);
  
 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&lcadre, &num2))
 {
  if (num2 == 1 && ((void *)*lcadre) != Obj->PBox->edited_item)
  {
   F(Obj->list_save).set(Obj->list_save,
			 ((l_Cadre *)Obj->PBox->edited_item)->cadre,
			 2,
			 &numf);
  }
  else
  {
   for(i=0; i<num2; i++)
    F(Obj->list_save).set(Obj->list_save,
			  lcadre[i]->cadre,
			  2,
			  &numf);
  }
  if (num2)
   Xc_free(lcadre);
 }
 if (XtWindow(Obj->drawing))
  XClearArea(XtDisplay(Obj->drawing),
	     XtWindow(Obj->drawing),
	     0, 0, 0, 0, True);
 F(Obj->PBox->Dialog).resetTraversal(Obj->PBox->Dialog);
}

static void draw_view(w, Obj, cbs)
Widget				w;
c_Box_attrib			*Obj;
XmDrawingAreaCallbackStruct	*cbs;
{
 Display	*display;
 Window	window;
 GC		gc;
 int		width;
 int		height;
 l_Cadre	**lcadre;
 int		num2;
 Window	wnone;
 int		nonex;
 int		noney;
 int		nonet;
 int		noned;
 int		sx;
 int		sy;
 int		thick;
 int		thick2;
 c_Color	*tmp_color;

 Xc_HISTORY(("Draw view"));
 display = cbs->event->xany.display;
 window = cbs->window;
 gc = Obj->gc;


 if(F(Obj->PBox->ItemList).getSelection(Obj->PBox->ItemList,
					(void ***)&lcadre, &num2) && num2)
 {
  if (F(Obj->list_save).get(Obj->list_save,
			    (*lcadre)->cadre,
			    2,
			    &thick2) == -1)
   thick2 = (*lcadre)->cadre->thickness;
  thick = thick2 / (SCALE_FROM_PERCENT(100)*COORD_DEFAULT);
      
  XGetGeometry(display, window, 
	       &wnone, &nonex, &noney,
	       &width, &height,
	       &nonet, &noned);
  sx = thick/2 + 1;
  width -= 2*sx;
  sy = thick/2 + 1;
  height -= 2*sy;
      
  if (F(Obj->list_save).get(Obj->list_save,
			    (*lcadre)->cadre,
			    1,
			    &tmp_color) == -1)
   tmp_color = (*lcadre)->cadre->inside_color;
  if (tmp_color->transparency)
  {
   XClearWindow(display, window);
  }
  else
  {
   XSetForeground(display, gc, tmp_color->cell.pixel);
   XFillRectangle(display, window, gc, sx, sy, width, height);
  }
  /*---   valeur pour un zoom de 100% ---*/
  if (thick2)
  {
   XSetLineAttributes(display, gc, thick,
		      LineSolid,
		      CapButt, JoinMiter);
   if (F(Obj->list_save).get(Obj->list_save,
			     (*lcadre)->cadre,
			     0,
			     &tmp_color) == -1)
    tmp_color = (*lcadre)->cadre->border_color;
   if (!tmp_color->transparency)
   {
    XSetForeground(display, gc, tmp_color->cell.pixel);
    XDrawRectangle(display, window, gc, sx, sy, width, height);
   }
  }
  Xc_free(lcadre);
 }
}

static void cb_action(cb_dialog, box, cb_reason)
c_CustomDialog	*cb_dialog;
c_Box_attrib	*box;
int		cb_reason;
{
 F(cb_dialog).unmap(cb_dialog);
 if (cb_reason == XcCD_OK)
 {
  F(box->list_save).change_all2(box->list_save);
 }
 else
 {
  Xc_TRACE(("canceled"));
 }
}

static void cb_change(box, frame, type, num, data)
c_Box_attrib	*box;
c_Cadre		*frame;
int		type;
int		num;
void		*data;
{
 switch (num)
 {
 case 0:
  F(frame).setOutlineColor(frame, ((c_Color **)data)[0]);
  break;
 case 1:
  F(frame).setBackgroundColor(frame, ((c_Color **)data)[0]);
  break;
 case 2:
  frame->thickness = ((coord_t *)data)[0];
  break;
 }
}















