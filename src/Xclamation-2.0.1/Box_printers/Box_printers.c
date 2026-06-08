/*
** Box_printers.c for Xclamation in Box_printers/
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
** Started on  Mon Mar  4 16:58:30 1996 Robin Castanier
** Last update Wed Jun  4 17:37:16 1997 Emmanuel Paris
*/

#include "../Pixmaps/item.del.zpm"
#include "../Pixmaps/item.new.zpm"
#include "Box_printers.h"

void *cons_Box_printers();
void dest_Box_printers();
void *copy_Box_printers();

static void Add_Item();
static void Del_SItem();
static void Change_Sel();
static void cb_action ___PROTO((c_CustomDialog	*cb_dialog,
				c_Box_printers	*box,
				int		cb_reason));
static void cb_change ___PROTO((c_Box_printers	*box,
/*c_TextStyle	*style,*/
				int		type,
				int		num,
				void 		*data));

extern printer_t *GlobPrinterBase;

sf_Box_printers fc_Box_printers =
{
  cons_Box_printers,
  dest_Box_printers,
  copy_Box_printers
};

void *cons_Box_printers(w_Parent, pszTitle)
Widget		w_Parent;
char		*pszTitle;
{
  c_Box_printers	*ObjTmp;
  Arg		args[5]; 
  XmString	strTitre;
  Display	*display;
 
  if ((ObjTmp = (c_Box_printers *)Xc_malloc("Box_printers",
					  sizeof(c_Box_printers))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_Box_printers;

  /* save bases */

  ObjTmp->PBox = (c_ASLBox *)NEW(c_ASLBox)(w_Parent,
					   pszTitle,
					   640, 300,
					   FALSE, 
					   offsetof(c_TextStyle, name));

  display = XtDisplay(WIDGET(ObjTmp->PBox));

  strTitre = F(GlobResources).getXmString(GlobResources, XcR_setupPrinters);
  XtSetArg(args[0], XmNlabelString, strTitre);
  XtSetValues(ObjTmp->PBox->wLabel, args, 1);
  XmStringFree(strTitre);
  
/*  F(ObjTmp->PBox).setCallback(ObjTmp->PBox, Change_Sel,
			      ObjTmp);
*/

  F(ObjTmp->PBox).Add_Icons(ObjTmp->PBox, "add", item_new_xpm, Add_Item,
			    ObjTmp,
			    NULL);
  F(ObjTmp->PBox).Add_Icons(ObjTmp->PBox, "del",
			    item_del_xpm, Del_SItem,
			    ObjTmp,
			    NULL);

/*  if (!*style_base)
    NEW(c_TextStyle)(style_base, "New_style");
  */

  WIDGET(ObjTmp) = WIDGET(ObjTmp->PBox);
  ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				       MANAGER(ObjTmp->PBox->Dialog),
				       "gBoxPrinters", TRUE);
  
  ObjTmp->Frame = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->PBox->Dialog), 
					  "fBPCommandPrinter", 0);

  ObjTmp->tfBPCommandPrinter = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Frame),
				      MANAGER(ObjTmp->PBox->Dialog),
				      "tfBPCommandPrinter",
				      TF_STRING,
				      0.0, 100.0,
				      "lpr", NULL, 7);
 
  ObjTmp->tbBPDefaultPrinter = 
    (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->PBox->Dialog),
					  "tbBPDefaultPrinter", NULL);
  
/*    F(ObjTmp->PBox->ItemList).set(ObjTmp->PBox->ItemList, -1,
				FALSE, -1L, -1, (void *)-1, 
				(void *)color_filter);
  F(ObjTmp->PBox->ItemList).addItems(ObjTmp->PBox->ItemList, FALSE,
				     XcIL_LINKED_LIST, -1,
				     color_base[0], offsetof(c_Color, next));
*/

  F(ObjTmp->PBox->ItemList).addItems(ObjTmp->PBox->ItemList, FALSE,
				     XcIL_LINKED_LIST, -1,
				     &GlobPrinterBase->name,
				     offsetof(printer_t, next));
  

  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE, NULL);
  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 70,
			    WIDGET(ObjTmp->Frame), XcG_MAX_SIZE,
			    NULL);
  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE, NULL);
  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_BEST_SIZE,
			    WIDGET(ObjTmp->tbBPDefaultPrinter), XcG_MAX_SIZE,
			    NULL);
  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE, NULL);
  
  F(ObjTmp->PBox->Dialog).createActionArea(ObjTmp->PBox->Dialog,
					   XcCD_OK | XcCD_CANCEL, XcCD_OK,
					   TRUE, (void (*)())cb_action, ObjTmp);

  F(ObjTmp->PBox).Map(ObjTmp->PBox, TRUE, NULL);
  Xc_TRACE(("Object Box_printers build"));
  return ObjTmp;
}

void dest_Box_printers(This)
c_Box_printers *This;
{
  DELETE(c_ToggleButton)(This->tbBPDefaultPrinter);
  DELETE(c_WTextField)(This->tfBPCommandPrinter);
  DELETE(c_Frame)(This->Frame);
  DELETE(c_Grid)(This->Grid);
  DELETE(c_ASLBox)(This->PBox);
  Xc_free(This);
  Xc_TRACE(("Object Box_printers destroyed"));
}

void *copy_Box_printers(This)
c_Box_printers *This;
{
  Xc_TRACE(("Copy not implemented"));
  return NULL;
}

/* ----------------------------------------------------------------- **
** Add_Item - Create a new style item                                **
** ----------------------------------------------------------------- */
static void Add_Item(w, This)
Widget		w;
c_Box_printers	*This;
{
/*  c_TextStyle	*tmp_style;
  char		*name;
  c_TextStyle	*style;
  c_Color	*tcolor;
  c_VectorFont	*tfont;
  coord_t	tpoint;
  textstyle_width_t	twidth;
  
  name = F(This->PBox).getNewItemName(This->PBox, (void **)&style);
  if (!name) return;
  
  if (F(This->list_save).get(This->list_save, style, 0, &tcolor) == -1)
    tcolor = style->color;
  if (F(This->list_save).get(This->list_save, style, 1, &tfont) == -1)
    tfont = style->font;
  if (F(This->list_save).get(This->list_save, style, 2, &tpoint) == -1)
    tpoint = style->point_size;
  if (F(This->list_save).get(This->list_save, style, 3, &twidth) == -1)
    memcpy(&twidth, &style->width, sizeof(textstyle_width_t));
  
  tmp_style = NEW(c_TextStyle)(&(This->v_Style_base), name,
			       tfont, tpoint, tcolor);
  F(tmp_style).set(tmp_style, XcTS_WIDTH_ON, twidth.scale, XcTS_END);
  F(This->list_save).add(This->list_save, tmp_style);
  F(This->PBox->ItemList).setState(This->PBox->ItemList, FALSE,
				   XcIL_POINTER_ARRAY, -1);
  F(This->PBox->ItemList).addItems(This->PBox->ItemList, TRUE,
				   XcIL_VA_LIST, 1, tmp_style, NULL);
  F(This->PBox).selectItem(This->PBox, tmp_style, TRUE);
  XmProcessTraversal(WIDGET(This->PBox->text_field), XmTRAVERSE_CURRENT);
  XmTextSetInsertionPosition(WIDGET(This->PBox->text_field), 
			     (XmTextPosition)strlen(name));
  Xc_free(name);
*/
}

/* ----------------------------------------------------------------- ** 
** Del_SItem - Delete the Style Selection                            ** 
** ----------------------------------------------------------------- */
static void Del_SItem(w, This)
Widget		w;
c_Box_printers	*This;
{
/*
  c_TextStyle	**style;
  int		num, i;
  c_TextStyle	*item;
  
  if(F(This->PBox->ItemList).getSelection(This->PBox->ItemList,
					  (void ***)&style, &num))
    {
      item = 
	F(This->PBox->ItemList).getFirstUnselectItem(This->PBox->ItemList);
      for (i = 0; i < num; i++)
	{
	  if (XcH_USAGE(style[i]) == 0 && 
	      This->PBox->ItemList->item_count > 1)
	    {
	      F(This->PBox->ItemList).deleteItems(This->PBox->ItemList,
						  XcIL_POINTER_ARRAY, 1,
						  (void **)&(style[i]));  
	      F(This->list_save).sub(This->list_save, style[i]);
	    }
	}
      if (num)
	Xc_free(style);
      if (item)
	{
	  F(This->PBox).selectItem(This->PBox, item, TRUE);
	  F(This->PBox->ItemList).selectItem(This->PBox->ItemList, item);
	}
    }
*/
}

/* ----------------------------------------------------------------- ** 
** Change_Sel - Callback for reason Selection had change             ** 
** ----------------------------------------------------------------- */
static void Change_Sel(this, Box_printers, style)
c_ASLBox	*this;
c_Box_printers	*Box_printers;
c_TextStyle	*style;
{
/*
  char		str_W[4];
  c_Color	*tcolor;
  c_VectorFont	*tfont;
  coord_t	tpoint;
  textstyle_width_t twidth;

  if (F(Box_printers->list_save).get(Box_printers->list_save,
				   style, 0, &tcolor) == -1)
    tcolor = style->color;
  F(Box_printers->IMcouleur).setSelection
    (Box_printers->IMcouleur, tcolor);

  if (F(Box_printers->list_save).get(Box_printers->list_save,
				   style, 1, &tfont) == -1)
    tfont = style->font;
  F(Box_printers->IMfonte).setSelection
    (Box_printers->IMfonte, tfont);
  if (F(Box_printers->list_save).get(Box_printers->list_save,
				   style, 2, &tpoint) == -1)
    tpoint = style->point_size;
  sprintf(str_W, "%3d", (int)SCALE_TO_POINTS(tpoint));
  F(Box_printers->FTfontsize).set(Box_printers->FTfontsize, str_W);
  if (F(Box_printers->list_save).get(Box_printers->list_save,
				   style, 3, &twidth) == -1)
    twidth = style->width;
  sprintf(str_W, "%3d", (int)SCALE_TO_PERCENT(twidth.scale));
  F(Box_printers->FTcondensation).set(Box_printers->FTcondensation, str_W);
*/
}


static void cb_action(cb_dialog, box, cb_reason)
c_CustomDialog	*cb_dialog;
c_Box_printers	*box;
int		cb_reason;
{
/*
  if (cb_reason == XcCD_OK)
    F(box->list_save).change_all2(box->list_save);
  else
    Xc_TRACE(("canceled"));
*/
  F(cb_dialog).unmap(cb_dialog);

}

static void cb_change(box, /*style,*/ type, num, data)
c_Box_printers	*box;
/*c_TextStyle	*style;*/
int		type;
int		num;
void		*data;
{
/*
  switch (type)
    {
    case NEWSET:
      style = box->modif.style;
    case TOSET:
      Xc_TRACE(("TOSET!"));
      switch(num)
	{
	case 0:
	  box->modif.color = ((c_Color **)data)[0];
	  break;
	case 1:
	  box->modif.font = ((c_VectorFont **)data)[0];
	  break;
	case 2:
	  box->modif.size = ((coord_t *)data)[0];
	  break;
	case 3:
	  F(style).set(style,
		       XcTS_COLOR, box->modif.color,
		       XcTS_FONT, box->modif.font,
		       XcTS_SIZE, box->modif.size,
		       XcTS_WIDTH_ON, ((textstyle_width_t *)data)[0].scale,
		       XcTS_END);
	  break;
	}
      break;
    case TOADD:
      Xc_TRACE(("TOADD!"));
      box->modif.style = (c_TextStyle *)NEW(c_TextStyle)(box->Style_base, 
						  style->name,
						  style->font,
						  style->point_size,
						  style->color);
      break;
    case TOSUB:
      Xc_TRACE(("TOSUB!"));
      DELETE(c_TextStyle)(style);
      break;
    }
*/
}

