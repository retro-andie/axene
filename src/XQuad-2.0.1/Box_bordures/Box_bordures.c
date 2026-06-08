/*
** Box_bordures.c for XQuad in Box_bordures/
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
** Started on  Mon Sep 18 17:09:02 1995 Robin Castanier
** Last update Sat Feb  7 18:27:56 1998 Emmanuel Paris
*/

#define _CELLSTYLE_PATTERN_
#define _CELLSTYLE_DASHES_

#include "Box_bordures.h"
#include "Display_CellStyle.h"
#include "Colormap.h"
#include "../Pixmaps/line.none.zpm"
#include "../Pixmaps/line.normal.zpm"
#include "../Pixmaps/line.doublesize.zpm"
#include "../Pixmaps/line.double.zpm"
#include "../Pixmaps/line.bigborder.zpm"
#include "../Pixmaps/line.megadouble.zpm"
#include "../Pixmaps/line.megainverse.zpm"
#include "../Pixmaps/line.each.zpm"
#include "../Pixmaps/line.doublesizedown.zpm"
#include "../Pixmaps/line.doublesizeup.zpm"
#include "../Pixmaps/line.doubletout.zpm"
#include "../Pixmaps/line.dash.1.zpm"
#include "../Pixmaps/line.dash.2.zpm"
#include "../Pixmaps/line.dash.4.zpm"
#include "../Pixmaps/line.dash.31.zpm"

extern c_Colormap *GlobColormap;

void *cons_Box_bordures();
void dest_Box_bordures();
void *copy_Box_bordures();

static void change_fore_color();
static void change_back_color();
static void change_pattern();
static void change_dashes();
static void refresh_drawing();
static void change_drawing();
static void default_drawing();
static void eventlook_drawing();

sf_Box_bordures fc_Box_bordures =
{
 cons_Box_bordures,
 dest_Box_bordures,
 copy_Box_bordures
};


void *cons_Box_bordures(w_Parent, pszTitle, BaseStd, borders)
Widget		w_Parent;
char		*pszTitle;
BaseStd_t 	*BaseStd;
cell_border_t	*borders;
{
 c_Box_bordures	*ObjTmp;
 Display		*display;
 Window		win;
 color_cells_t		realcolor;
 Arg       	     	args[5];
 int 			nargs;
 c_Color		*color_base;
 int			i, j, none;

 color_base = BaseStd->color_base;
   
 if((ObjTmp = (c_Box_bordures *)Xc_malloc("Box_borders",
					  sizeof(c_Box_bordures))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }

 ObjTmp->f = &fc_Box_bordures;
 ObjTmp->BaseStd = BaseStd;
 ObjTmp->borders = borders;
  
 ObjTmp->foreground = borders->cellb_outline.border_foreground;
 ObjTmp->background = borders->cellb_outline.border_background;
 ObjTmp->pattern = borders->cellb_outline.border_stipple_pattern;
 ObjTmp->dashes = borders->cellb_outline.border_dash_pattern;
  
 display = XtDisplay(w_Parent);
 if (DefaultDepth(display, DefaultScreen(display)) == 1)
 {
  ObjTmp->light = WhitePixel(display,DefaultScreen(display));
  ObjTmp->dark = 
   ObjTmp->normal = BlackPixel(display,DefaultScreen(display));
 }
 else
 {
  F(GlobColormap).allocate_named_color(GlobColormap, BOX_BORD_LIGHT,
				       &realcolor);
  ObjTmp->light = realcolor.pixel;
  F(GlobColormap).allocate_named_color(GlobColormap, BOX_BORD_DARK,
				       &realcolor);
  ObjTmp->dark = realcolor.pixel;
  F(GlobColormap).allocate_named_color(GlobColormap, BOX_BORD_NORM,
				       &realcolor);
  ObjTmp->normal = realcolor.pixel;
 }
 ObjTmp->source = 0;
  
 ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							pszTitle, 550, 280,
							XcCD_VERTICAL
							| XcCD_HRESIZE
							| XcCD_VRESIZE);
 ObjTmp->w_This = WIDGET(ObjTmp->Dialog);

 ObjTmp->Ltitre = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp),
					  MANAGER(ObjTmp->Dialog),
					  "lBoxBordersTitle");

 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				      MANAGER(ObjTmp->Dialog),
				      "gBoxBorders", TRUE);
  
  
 ObjTmp->Frame1 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fBBBorders", 0);
 
 ObjTmp->Grid1 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame1),
				       MANAGER(ObjTmp->Dialog),
				       "gBBBorders", TRUE);

 ObjTmp->Lcontour = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
					    MANAGER(ObjTmp->Dialog),
					    "lBBOutline");
 ObjTmp->Lhaut = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
					 MANAGER(ObjTmp->Dialog),
					 "lBBTop");
 ObjTmp->Lbas = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
					MANAGER(ObjTmp->Dialog),
					"lBBBottom");
 ObjTmp->Lgauche = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
					   MANAGER(ObjTmp->Dialog),
					   "lBBLeft");
 ObjTmp->Ldroite = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
					   MANAGER(ObjTmp->Dialog),
					   "lBBRight");

 ObjTmp->SFrame1 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid1),
					   MANAGER(ObjTmp->Dialog), 
					   NULL, 0);
 ObjTmp->SFrame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid1),
					   MANAGER(ObjTmp->Dialog), 
					   NULL, 0);
 ObjTmp->SFrame3 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid1),
					   MANAGER(ObjTmp->Dialog), 
					   NULL, 0);
 ObjTmp->SFrame4 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid1),
					   MANAGER(ObjTmp->Dialog), 
					   NULL, 0);
 ObjTmp->SFrame5 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid1),
					   MANAGER(ObjTmp->Dialog), 
					   NULL, 0);
  
 nargs = 0;
 XtSetArg(args[nargs], XmNbackground, WhitePixel(display, 
						 DefaultScreen(display)));
 nargs++;

 ObjTmp->drawing1 = XmCreateDrawingArea(WIDGET(ObjTmp->SFrame1),
					"daBBdrawing1", args, nargs);
 XtAddCallback(ObjTmp->drawing1, XmNexposeCallback,
	       (XtCallbackProc)eventlook_drawing, ObjTmp);
 XtAddCallback(ObjTmp->drawing1, XmNinputCallback,
	       (XtCallbackProc)eventlook_drawing, ObjTmp);
 ObjTmp->drawing2 = XmCreateDrawingArea(WIDGET(ObjTmp->SFrame2),
					"daBBdrawing2", args, nargs);
 XtAddCallback(ObjTmp->drawing2, XmNexposeCallback,
	       (XtCallbackProc)eventlook_drawing, ObjTmp);
 XtAddCallback(ObjTmp->drawing2, XmNinputCallback,
	       (XtCallbackProc)eventlook_drawing, ObjTmp);
 ObjTmp->drawing3 = XmCreateDrawingArea(WIDGET(ObjTmp->SFrame3),
					"daBBdrawing3", args, nargs);
 XtAddCallback(ObjTmp->drawing3, XmNexposeCallback,
	       (XtCallbackProc)eventlook_drawing, ObjTmp);
 XtAddCallback(ObjTmp->drawing3, XmNinputCallback,
	       (XtCallbackProc)eventlook_drawing, ObjTmp);
 ObjTmp->drawing4 = XmCreateDrawingArea(WIDGET(ObjTmp->SFrame4),
					"daBBdrawing4", args, nargs);
 XtAddCallback(ObjTmp->drawing4, XmNexposeCallback,
	       (XtCallbackProc)eventlook_drawing, ObjTmp);
 XtAddCallback(ObjTmp->drawing4, XmNinputCallback,
	       (XtCallbackProc)eventlook_drawing, ObjTmp);
 ObjTmp->drawing5 = XmCreateDrawingArea(WIDGET(ObjTmp->SFrame5),
					"daBBdrawing5", args, nargs);
 XtAddCallback(ObjTmp->drawing5, XmNexposeCallback,
	       (XtCallbackProc)eventlook_drawing, ObjTmp);
 XtAddCallback(ObjTmp->drawing5, XmNinputCallback,
	       (XtCallbackProc)eventlook_drawing, ObjTmp);

 F(MANAGER(ObjTmp->Dialog)).Add_child(MANAGER(ObjTmp->Dialog),
				      ObjTmp->drawing1,
				      WIDGET(ObjTmp->SFrame1));
 F(MANAGER(ObjTmp->Dialog)).Add_child(MANAGER(ObjTmp->Dialog),
				      ObjTmp->drawing2,
				      WIDGET(ObjTmp->SFrame2));
 F(MANAGER(ObjTmp->Dialog)).Add_child(MANAGER(ObjTmp->Dialog),
				      ObjTmp->drawing3,
				      WIDGET(ObjTmp->SFrame3));
 F(MANAGER(ObjTmp->Dialog)).Add_child(MANAGER(ObjTmp->Dialog),
				      ObjTmp->drawing4,
				      WIDGET(ObjTmp->SFrame4));
 F(MANAGER(ObjTmp->Dialog)).Add_child(MANAGER(ObjTmp->Dialog),
				      ObjTmp->drawing5,
				      WIDGET(ObjTmp->SFrame5));

 ObjTmp->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fBBLineStyle", 0);

 ObjTmp->Grid2 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame2),
				       MANAGER(ObjTmp->Dialog),
				       "gBBLineStyle", FALSE);

  
 ObjTmp->Lpattern = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
					    MANAGER(ObjTmp->Dialog),
					    "lBBPattern");
 ObjTmp->Ldash = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
					 MANAGER(ObjTmp->Dialog),
					 "lBBDash");
 ObjTmp->LFcolor = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
					   MANAGER(ObjTmp->Dialog),
					   "lBBForeColor");
 ObjTmp->LBcolor = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
					   MANAGER(ObjTmp->Dialog),
					   "lBBBackColor");
  
#define RIEN	1
#define ITEM1	1
#define ITEM2	2
#define ITEM3	3
#define ITEM4	4
#define ITEM5	5
#define ITEM6	6
#define ITEM7	7
#define ITEM8	8
#define ITEM9	9
#define ITEM10	10
#define ITEM11	11


 ObjTmp->OMenu1 = 
  (c_OptionMenu *)NEW(c_OptionMenu)(WIDGET(ObjTmp->Grid2),
				    MANAGER(ObjTmp->Dialog),
				    "omBBOMenu1",
				    ICONMODEON, RIEN,
				    (char *)line_none_xpm, ITEM1,
				    (char *)line_normal_xpm, ITEM2,
				    (char *)line_doublesize_xpm, ITEM3,
				    (char *)line_double_xpm, ITEM4,
				    (char *)line_bigborder_xpm, ITEM5,
				    (char *)line_megadouble_xpm, ITEM6,
				    (char *)line_megainverse_xpm, ITEM7,
				    (char *)line_each_xpm, ITEM8,
				    (char *)line_doublesizedown_xpm, ITEM9,
				    (char *)line_doublesizeup_xpm, ITEM10,
				    (char *)line_doubletout_xpm, ITEM11,
				    ICONMODEOFF, RIEN,
				    NULL);
  
 for(i = ITEM1, j = 0; i<= ITEM11; i++, j++)
 {
  ObjTmp->pattern_item[j].box = ObjTmp;
  ObjTmp->pattern_item[j].item = j-1;
  F(ObjTmp->OMenu1).AddCallback(ObjTmp->OMenu1, i, change_pattern, 
				&(ObjTmp->pattern_item[j]), NULL );
 }
 ObjTmp->OMenu2 = 
  (c_OptionMenu *)NEW(c_OptionMenu)(WIDGET(ObjTmp->Grid2),
				    MANAGER(ObjTmp->Dialog),
				    "omBBOMenu2",
				    ICONMODEON, RIEN,
				    (char *)line_normal_xpm, ITEM1,
				    (char *)line_dash_1_xpm, ITEM2,
				    (char *)line_dash_2_xpm, ITEM3,
				    (char *)line_dash_4_xpm, ITEM4,
				    (char *)line_dash_31_xpm, ITEM5,
				    ICONMODEOFF, RIEN,
				    NULL);
  
 for(i = ITEM1, j = 0; i <= ITEM5; i++, j++)
 {
  ObjTmp->dashes_item[j].box = ObjTmp;
  ObjTmp->dashes_item[j].item = j-1;
  F(ObjTmp->OMenu2).AddCallback(ObjTmp->OMenu2, i, change_dashes, 
				&(ObjTmp->dashes_item[j]), NULL );
 }
 ObjTmp->clForeg = 
  (c_ColorList *)NEW(c_ColorList)(WIDGET(ObjTmp->Grid2),
				  MANAGER(ObjTmp->Dialog),
				  "clForeground", XcCL_NORMAL);
 F(ObjTmp->clForeg).setCallback(ObjTmp->clForeg, 
				change_fore_color, ObjTmp); 
 F(ObjTmp->clForeg).setColorBase(ObjTmp->clForeg,
				 color_base,
				 CL_KEEP_TRANSPARENCY);
  
 ObjTmp->clBackg = 
  (c_ColorList *)NEW(c_ColorList)(WIDGET(ObjTmp->Grid2),
				  MANAGER(ObjTmp->Dialog),
				  "clBackground", XcCL_NORMAL);
 F(ObjTmp->clBackg).setCallback(ObjTmp->clBackg,
				change_back_color, ObjTmp);
 F(ObjTmp->clBackg).setColorBase(ObjTmp->clBackg,
				 color_base,
				 CL_KEEP_TRANSPARENCY);
  
 F(ObjTmp->clForeg).setColor(ObjTmp->clForeg, ObjTmp->foreground);
 F(ObjTmp->clBackg).setColor(ObjTmp->clBackg, ObjTmp->background);
  
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE, NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, 26,
			    WIDGET(ObjTmp->Lcontour), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->SFrame1), 80,
			    NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE, NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, 26,
			    WIDGET(ObjTmp->Lhaut), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->SFrame2), 80,
			    NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE, NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, 26,
			    WIDGET(ObjTmp->Lbas), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->SFrame3), 80,
			    NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE, NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, 26,
			    WIDGET(ObjTmp->Lgauche), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->SFrame4), 80,
			    NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE, NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, 26,
			    WIDGET(ObjTmp->Ldroite), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->SFrame5), 80,
			    NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE, NULL);
  
 /*			     
   F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE,
   WIDGET(ObjTmp->Lcontour), 26,
   WIDGET(ObjTmp->Lhaut), 26,
   WIDGET(ObjTmp->Lbas), 26,
   WIDGET(ObjTmp->Lgauche), 26,
   WIDGET(ObjTmp->Ldroite), 26,
   NULL);
   F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, 80,
   WIDGET(ObjTmp->SFrame1), 26,
   WIDGET(ObjTmp->SFrame2), 26,
   WIDGET(ObjTmp->SFrame3), 26,
   WIDGET(ObjTmp->SFrame4), 26,
   WIDGET(ObjTmp->SFrame5), 26,
   NULL);
   */
  
 F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2,  XcG_MAX_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->Lpattern), 30,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->Ldash), 30,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->LFcolor), 30,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->LBcolor), 30,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, 122,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->OMenu1), 30,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->OMenu2), 30,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->clForeg), 30,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->clBackg), 30,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Frame1), 180,
			   WIDGET(ObjTmp->Frame2), XcG_MAX_SIZE,
			   NULL);

 F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, 
				    XcCD_OK | XcCD_CANCEL, 
				    XcCD_OK,
				    TRUE, NULL, NULL);

 F(ObjTmp->Dialog).blockChild((ObjTmp->Dialog), WIDGET(ObjTmp->Ltitre), 30);
  
 F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);
  
 XGetGeometry(display, XtWindow(ObjTmp->drawing1), &win, &none, &none, 
	      &(ObjTmp->d_width), &(ObjTmp->d_height), &none, &none);
  
 ObjTmp->d_width = 76;
 ObjTmp->d_height = 22;

 refresh_drawing(ObjTmp, 1);  
 refresh_drawing(ObjTmp, 2);  
 refresh_drawing(ObjTmp, 3);  
 refresh_drawing(ObjTmp, 4);  
 refresh_drawing(ObjTmp, 5);  
  
 Xc_TRACE(("Object Box_borders build"));
 return ObjTmp;
}

void dest_Box_bordures(This)
c_Box_bordures *This;
{
 XtDestroyWidget(This->drawing1);
 XtDestroyWidget(This->drawing2);
 XtDestroyWidget(This->drawing3);
 XtDestroyWidget(This->drawing4);
 XtDestroyWidget(This->drawing5);
 DELETE(c_Frame)(This->SFrame1);
 DELETE(c_Frame)(This->SFrame2);
 DELETE(c_Frame)(This->SFrame3);
 DELETE(c_Frame)(This->SFrame4);
 DELETE(c_Frame)(This->SFrame5);
 DELETE(c_Label)(This->Lcontour);
 DELETE(c_Label)(This->Lhaut);
 DELETE(c_Label)(This->Lbas);
 DELETE(c_Label)(This->Lgauche);
 DELETE(c_Label)(This->Ldroite);
 DELETE(c_Grid)(This->Grid1);
 DELETE(c_Frame)(This->Frame1);

 DELETE(c_Label)(This->Lpattern);
 DELETE(c_Label)(This->Ldash);
 DELETE(c_Label)(This->LFcolor);
 DELETE(c_Label)(This->LBcolor);
 DELETE(c_ColorList)(This->clForeg);
 DELETE(c_ColorList)(This->clBackg);
 DELETE(c_OptionMenu)(This->OMenu1);
 DELETE(c_OptionMenu)(This->OMenu2);
 DELETE(c_Grid)(This->Grid2);
 DELETE(c_Frame)(This->Frame2);

 DELETE(c_Grid)(This->Grid);
 DELETE(c_Label)(This->Ltitre);
 DELETE(c_CustomDialog)(This->Dialog);

 Xc_free(This);
 Xc_TRACE(("Object Box_borders destroyed"));
}

void *copy_Box_bordures(This)
c_Box_bordures *This;
{
 Xc_TRACE(("Copy not implemented"));
 return NULL;
}

static void change_fore_color(this, box, color)
c_ColorList	*this;
c_Box_bordures  *box;
c_Color		*color;
{
 if (box->foreground != color)
 {
  box->foreground = color;
  switch(box->source)
  {
  case 1:
   box->borders->cellb_outline.border_foreground = color;
   break;
  case 2:
   box->borders->cellb_up.border_foreground = color;
   break;
  case 3:
   box->borders->cellb_down.border_foreground = color;
   break;
  case 4:
   box->borders->cellb_left.border_foreground = color;
   break;
  case 5:
   box->borders->cellb_right.border_foreground = color;
   break;
  default:
   return;
  }
  refresh_drawing(box, box->source);
 }
}

static void change_back_color(this, box, color)
c_ColorList	*this;
c_Box_bordures	*box;
c_Color		*color;
{
 if (box->background != color)
 {
  box->background = color;
  switch(box->source)
  {
  case 1:
   box->borders->cellb_outline.border_background = color;
   break;
  case 2:
   box->borders->cellb_up.border_background = color;
   break;
  case 3:
   box->borders->cellb_down.border_background = color;
   break;
  case 4:
   box->borders->cellb_left.border_background = color;
   break;
  case 5:
   box->borders->cellb_right.border_background = color;
   break;
  default:
   return;
  }
  refresh_drawing(box, box->source);
 }
}

static void change_pattern(w, item)
Widget	w;
BB_item_t	*item;
{
 int	pattern;
 c_Box_bordures *box;
  
 if (item == NULL)
  return;
  
 pattern = item->item;
 box = item->box;
  
 if (pattern == -1)
  pattern = PATTERN_NONE;
  
 if (box->pattern != pattern)
 {
  box->pattern = pattern;
  switch(box->source)
  {
  case 1:
   box->borders->cellb_outline.border_stipple_pattern = pattern;
   break;
  case 2:
   box->borders->cellb_up.border_stipple_pattern = pattern;
   break;
  case 3:
   box->borders->cellb_down.border_stipple_pattern = pattern;
   break;
  case 4:
   box->borders->cellb_left.border_stipple_pattern = pattern;
   break;
  case 5:
   box->borders->cellb_right.border_stipple_pattern = pattern;
   break;
  default:
   return;
  }
  refresh_drawing(box, box->source);
 }
}

static void change_dashes(w, item)
Widget	w;
BB_item_t	*item;
{
 int	dashes;
 c_Box_bordures *box;
  
 if (item == NULL)
  return;
  
 dashes = item->item;
 box = item->box;
  
 if (dashes == -1)
  dashes = PATTERN_NONE;
  
 if (box->dashes != dashes)
 {
  box->dashes = dashes;
  switch(box->source)
  {
  case 1:
   box->borders->cellb_outline.border_dash_pattern = dashes;
   break;
  case 2:
   box->borders->cellb_up.border_dash_pattern = dashes;
   break;
  case 3:
   box->borders->cellb_down.border_dash_pattern = dashes;
   break;
  case 4:
   box->borders->cellb_left.border_dash_pattern = dashes;
   break;
  case 5:
   box->borders->cellb_right.border_dash_pattern = dashes;
   break;
  default:
   return;
  }
  refresh_drawing(box, box->source);
 }
}

static void refresh_drawing(This, drawing)
c_Box_bordures	*This;
int		drawing;
{
 Display	*display;
 Window	window;
 GC		gc;
 CellBorderStyle_t	*cellb;
 boolean	border;
 int		masq_bin;
 XGCValues	gc_values;
 static XRectangle	rect[4];
 static XSegment	segment[2];
  
 display = XtDisplay(This->w_This);
 switch(drawing)
 {
 case 1:
  window = XtWindow(This->drawing1);
  cellb = &(This->borders->cellb_outline);
  border = This->borders->outline;
  break;
 case 2:
  window = XtWindow(This->drawing2);
  cellb = &(This->borders->cellb_up);
  border = This->borders->up;
  break;
 case 3:
  window = XtWindow(This->drawing3);
  cellb = &(This->borders->cellb_down);
  border = This->borders->down;
  break;
 case 4:
  window = XtWindow(This->drawing4);
  cellb = &(This->borders->cellb_left);
  border = This->borders->left;
  break;
 case 5:
  window = XtWindow(This->drawing5);
  cellb = &(This->borders->cellb_right);
  border = This->borders->right;
  break;
 default:
  return;
 }
  
 if (!border)
 {
  static char trame[8] = { 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa};
  Pixmap	p_trame;
      
  p_trame = XCreateBitmapFromData(display, window, trame, 8, 8);
      
  gc_values.foreground = BlackPixel(display, DefaultScreen(display));
  gc_values.background = WhitePixel(display, DefaultScreen(display));
  gc_values.fill_style = FillOpaqueStippled;
  gc_values.stipple = p_trame;
      
  gc = XCreateGC(display, window, GCForeground | GCBackground 
		 | GCFillStyle | GCStipple, &gc_values);
      
  XFillRectangle(display, window, gc, 0, 0, This->d_width, This->d_height);
  XFreeGC(display, gc);
 }
 else
 {
  XClearWindow(display, window);
      
  if (cellb->border_stipple_pattern != PATTERN_NONE)
  {
   gc = XCreateGC(display, window, 0, NULL);
   masq_bin = GCForeground | GCBackground | GCFillStyle | GCLineStyle |
    GCLineWidth | GCCapStyle | GCJoinStyle | GCTileStipXOrigin | 
     GCTileStipYOrigin;
   gc_values.foreground = cellb->border_foreground->cell.pixel;
   gc_values.background = cellb->border_background->cell.pixel;
   gc_values.cap_style = cellb->border_line_cap_style;
   gc_values.join_style = cellb->border_line_join_style;
   gc_values.ts_x_origin = 0;
   gc_values.ts_y_origin = 0;
   if (cellb->border_stipple_pattern != PATTERN_NONE)
   {
    cellb->border_thickness = 
     Cell_Pattern[cellb->border_stipple_pattern].height 
      * cellb->border_line_thickness;
	      
    if (Cell_Pattern[cellb->border_stipple_pattern].type
	== PATTERN_LINE)
    {
     gc_values.line_width = 0;
     gc_values.fill_style = FillSolid;
    }
    else
    {
     masq_bin |= GCStipple;
     gc_values.stipple = 
      Cell_Pattern[cellb->border_stipple_pattern].
       definition.data.pixmap;
		  
     if (cellb->border_background->transparency)
      gc_values.fill_style = FillStippled;
     else
      gc_values.fill_style = FillOpaqueStippled;
		  
     if (cellb->border_thickness == 1)
      gc_values.line_width = 0;
     else
      gc_values.line_width = cellb->border_thickness;
    }
   }
   else
   {
    cellb->border_thickness = 0;
   }
	  
   if (cellb->border_dash_pattern != PATTERN_NONE)
   {
    if (gc_values.fill_style == FillSolid)
    {
     if (cellb->border_background->transparency)
      gc_values.line_style = LineOnOffDash;
     else
      gc_values.line_style = LineDoubleDash;
    }
    else
     gc_values.line_style = LineOnOffDash;
    XSetDashes(display, gc, 
	       Cell_Dashes[cellb->border_dash_pattern].dash_offset,
	       Cell_Dashes[cellb->border_dash_pattern].dash_list,
	       Cell_Dashes[cellb->border_dash_pattern].dash_length);
   }
   else
    gc_values.line_style = LineSolid;
	  
   XChangeGC(display, gc, masq_bin, &gc_values);
   CSdrawLine(display, window, gc, cellb, NULL, 2, This->d_height/2, 
	      This->d_width-2, This->d_height/2);
   XFreeGC(display, gc);
  }
 }
 if (This->source == drawing)
 {
  gc_values.foreground = This->normal;
  gc_values.line_width = 0;
  gc_values.fill_style = FillSolid;
  gc = XCreateGC(display, window, 
		 GCForeground|GCLineWidth|GCFillStyle, &gc_values);
      
  rect[0].x = rect[1].x = rect[2].x = 0;
  rect[3].x = This->d_width - 2;
  rect[0].y = 0;
  rect[1].y = This->d_height - 2;
  rect[2].y = rect[3].y = 2;
  rect[0].width = rect[1].width = This->d_width;
  rect[0].height = rect[1].height = 2;
  rect[2].width = rect[3].width = 2;
  rect[2].height = rect[3].height = This->d_height;
  XFillRectangles(display, window, gc, rect, 4);
    
  XSetForeground(display, gc, This->dark);
  segment[0].x1 = segment[0].x2 = segment[1].x1 = 2;
  segment[1].x2 = This->d_width - 3;
  segment[0].y2 = segment[1].y1 = segment[1].y2 = 2;
  segment[0].y1 = This->d_height - 3;
  XDrawSegments(display, window, gc, segment, 2);
      
  XSetForeground(display, gc, This->light);
  segment[0].x1 = segment[0].x2 = segment[1].x1 = This->d_width - 3;
  segment[1].x2 = 3;
  segment[0].y2 = segment[1].y1 = segment[1].y2 = This->d_height - 3;
  segment[0].y1 = 3;
  XDrawSegments(display, window, gc, segment, 2);
  XFreeGC(display, gc);
 }
}

static void change_drawing(This, drawing)
c_Box_bordures	*This;
int		drawing;
{
 int	old_drawing;
  
 if (drawing == This->source)
 {
  This->source = 0;
  switch(drawing)
  {
  case 1:
   This->borders->cellb_outline.border_stipple_pattern = PATTERN_NONE;
   This->borders->cellb_outline.border_dash_pattern = PATTERN_NONE;
   break;
  case 2:
   This->borders->cellb_up.border_stipple_pattern = PATTERN_NONE;
   This->borders->cellb_up.border_dash_pattern = PATTERN_NONE;
   break;
  case 3:
   This->borders->cellb_down.border_stipple_pattern = PATTERN_NONE;
   This->borders->cellb_down.border_dash_pattern = PATTERN_NONE;
   break;
  case 4:
   This->borders->cellb_left.border_stipple_pattern = PATTERN_NONE;
   This->borders->cellb_left.border_dash_pattern = PATTERN_NONE;
   break;
  case 5:
   This->borders->cellb_right.border_stipple_pattern = PATTERN_NONE;
   This->borders->cellb_right.border_dash_pattern = PATTERN_NONE;
   break;
  }
  refresh_drawing(This, drawing);
 }
 else
 {
  if (This->source)
  {
   old_drawing = This->source;
   This->source = drawing;
   refresh_drawing(This, old_drawing);
  }
  else
   This->source = drawing;
  switch(drawing)
  {
  case 1:
   This->borders->outline = TRUE;
   This->borders->cellb_outline.border_foreground = This->foreground;
   This->borders->cellb_outline.border_background = This->background;
   This->borders->cellb_outline.border_stipple_pattern = This->pattern;
   This->borders->cellb_outline.border_dash_pattern = This->dashes;
   break;
  case 2:
   This->borders->up = TRUE;
   This->borders->cellb_up.border_foreground = This->foreground;
   This->borders->cellb_up.border_background = This->background;
   This->borders->cellb_up.border_stipple_pattern = This->pattern;
   This->borders->cellb_up.border_dash_pattern = This->dashes;
   break;
  case 3:
   This->borders->down = TRUE;
   This->borders->cellb_down.border_foreground = This->foreground;
   This->borders->cellb_down.border_background = This->background;
   This->borders->cellb_down.border_stipple_pattern = This->pattern;
   This->borders->cellb_down.border_dash_pattern = This->dashes;
   break;
  case 4:
   This->borders->left = TRUE;
   This->borders->cellb_left.border_foreground = This->foreground;
   This->borders->cellb_left.border_background = This->background;
   This->borders->cellb_left.border_stipple_pattern = This->pattern;
   This->borders->cellb_left.border_dash_pattern = This->dashes;
   break;
  case 5:
   This->borders->right = TRUE;
   This->borders->cellb_right.border_foreground = This->foreground;
   This->borders->cellb_right.border_background = This->background;
   This->borders->cellb_right.border_stipple_pattern = This->pattern;
   This->borders->cellb_right.border_dash_pattern = This->dashes;
   break;
  }
  refresh_drawing(This, drawing);
 }
}

static void default_drawing(This, drawing)
c_Box_bordures	*This;
int		drawing;
{
  
 This->source = 0;
 switch(drawing)
 {
 case 1:
  This->borders->outline = FALSE;
  break;
 case 2:
  This->borders->up = FALSE;
  break;
 case 3:
  This->borders->down = FALSE;
  break;
 case 4:
  This->borders->left = FALSE;
  break;
 case 5:
  This->borders->right = FALSE;
  break;
 }
 refresh_drawing(This, drawing);
}

static void eventlook_drawing(w, This, cbs)
Widget				w;
c_Box_bordures			*This;
XmDrawingAreaCallbackStruct	*cbs;
{
 XEvent	*event;
 int		drawing;
  
 if (w==This->drawing1)
  drawing = 1;
 else
 {
  if (w == This->drawing2)
   drawing = 2;
  else
  {
   if (w == This->drawing3)
    drawing = 3;
   else
   {
    if (w == This->drawing4)
     drawing = 4;
    else
    {
     if (w == This->drawing5)
      drawing = 5;
     else
      return;
    }
   }
  }
 }
 event = cbs->event;
 switch(event->type)
 {
 case Expose:
 case GraphicsExpose:
  refresh_drawing(This, drawing);
  break;
 case ButtonPress:
  if (event->xbutton.button == 1)
   change_drawing(This, drawing);
  else
   default_drawing(This, drawing);
  break;
 }
}
