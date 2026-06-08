/*
** Box_AskGraph.c for XQuad in Box_AskGraph/
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
** Started on  Mon Sep 18 16:48:35 1995 Robin Castanier
** Last update Sat Feb  7 17:54:50 1998 Emmanuel Paris
*/

#include "Box_AskGraph.h"
#include "RegisterHelp.h"
#include "../Pixmaps/colonnes.zpm"
#include "../Pixmaps/lignes.zpm"
#include "../Pixmaps/donnees.zpm"
#include "../Pixmaps/etiquettes.zpm"
#include "../Pixmaps/legendes.zpm"

void *cons_Box_AskGraph();
void dest_Box_AskGraph();
void *copy_Box_AskGraph();

static void eventlook_drawing();
static void change_LC1();
static void change_LC0();
static void change_ABS1();
static void change_ABS0();
static void change_ORD1();
static void change_ORD0();
static void cancel_change();

sf_Box_AskGraph fc_Box_AskGraph =
{
 cons_Box_AskGraph,
 dest_Box_AskGraph,
 copy_Box_AskGraph,
 cancel_change
};

void *cons_Box_AskGraph(w_Parent, pszTitle, highg)
Widget		w_Parent;
char		*pszTitle;
c_HighGraph	*highg;
{
 c_Box_AskGraph	*ObjTmp;
 XmString       	xmsBaratinRow;
 XmString       	xmsBaratinCol;
 XmString       	xmsBaratinRow2;
 XmString       	xmsBaratinCol2;
 Arg			argsw[2];
 Arg			argsw2[2];
 Arg			argsw3[2];
 Arg			argsw4[2];
 Display		*display;

 if((ObjTmp = (c_Box_AskGraph *)Xc_malloc("Box_AskGraph",
					  sizeof(c_Box_AskGraph))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }

 ObjTmp->f = &fc_Box_AskGraph;
 ObjTmp->highg = highg;
 ObjTmp->col_row = highg->col_row?0:1;
 ObjTmp->abscisse = highg->abscisse?0:1;
 ObjTmp->ordonnee = highg->ordonnee?0:1;
 ObjTmp->old_col_row = highg->col_row;
 ObjTmp->old_abscisse = highg->abscisse;
 ObjTmp->old_ordonnee = highg->ordonnee;

 display = XtDisplay(w_Parent);
  
 ObjTmp->Dialog = 
  (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent, pszTitle, 
					GRAPH_DRAWING_SIZE + 370,
					GRAPH_DRAWING_SIZE + 75,
					XcCD_HORIZONTAL
					| XcCD_HRESIZE | XcCD_VRESIZE );

 ObjTmp->w_This = WIDGET(ObjTmp->Dialog);

 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				      MANAGER(ObjTmp->Dialog),
				      "gBoxAskGraph", FALSE);

 ObjTmp->Ltitre = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid),
			  MANAGER(ObjTmp->Dialog),
			  "lBoxAskGraphTitle");

 ObjTmp->Frame1 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog),
					  "fAGSerial", 0);
 ObjTmp->GridLC = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame1),
					MANAGER(ObjTmp->Dialog),
					"gAGFirstSerial", FALSE);
 ObjTmp->l_LC = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridLC),
			  MANAGER(ObjTmp->Dialog),
			  "lAGFirstSerial");
 ObjTmp->ri_LC = 
  (c_RadioImage *)NEW(c_RadioImage)(WIDGET(ObjTmp->GridLC),
				    MANAGER(ObjTmp->Dialog),
				    &ObjTmp->col_row,
				    "rbAGColumns",
				    colonnes_xpm,
				    "rbAGRows",
				    lignes_xpm,
				    NULL);

 F(ObjTmp->ri_LC->Toggles[0]).Add_Callback(ObjTmp->ri_LC->Toggles[0],
					   XmNarmCallback,
					   change_LC1, ObjTmp);
 F(ObjTmp->ri_LC->Toggles[1]).Add_Callback(ObjTmp->ri_LC->Toggles[1], 
					   XmNarmCallback,
					   change_LC0, ObjTmp);

 F(ObjTmp->GridLC).addColumn(ObjTmp->GridLC, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->l_LC), XcG_BEST_SIZE,
			     WIDGET(ObjTmp->ri_LC), XcG_MAX_SIZE,
			     NULL);

 ObjTmp->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog),
					  "fAGFirstQuestion", 0);
 ObjTmp->GridFCL = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame2),
					 MANAGER(ObjTmp->Dialog),
					 "gAGFirstQuestion", FALSE);

 ObjTmp->l_FCL = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridFCL),
			  MANAGER(ObjTmp->Dialog),
			  "lAGFirstQuestion");
 ObjTmp->ri_FCL = 
  (c_RadioImage *)NEW(c_RadioImage)(WIDGET(ObjTmp->GridFCL),
				    MANAGER(ObjTmp->Dialog),
				    &ObjTmp->abscisse,
				    "rbAGLabels",
				    etiquettes_xpm,
				    "rbAGDatas",
				    donnees_xpm,
				    NULL);

 F(ObjTmp->ri_FCL->Toggles[0]).Add_Callback(ObjTmp->ri_FCL->Toggles[0],
					    XmNarmCallback,
					    change_ABS1, ObjTmp);
 F(ObjTmp->ri_FCL->Toggles[1]).Add_Callback(ObjTmp->ri_FCL->Toggles[1],
					    XmNarmCallback,
					    change_ABS0, ObjTmp);

 F(ObjTmp->GridFCL).addColumn(ObjTmp->GridFCL, XcG_MAX_SIZE,
			      WIDGET(ObjTmp->l_FCL), XcG_BEST_SIZE,
			      WIDGET(ObjTmp->ri_FCL), XcG_MAX_SIZE,
			      NULL);


 ObjTmp->Frame3 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog),
					  "fAGSecondQuestion", 0);
 ObjTmp->GridFLL = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame3),
					 MANAGER(ObjTmp->Dialog),
					 "gAGSecondQuestion", FALSE);
 ObjTmp->l_FLL = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->GridFLL),
			  MANAGER(ObjTmp->Dialog),
			  "lAGSecondQuestion");

 ObjTmp->ri_FLL = 
  (c_RadioImage *)NEW(c_RadioImage)(WIDGET(ObjTmp->GridFLL),
				    MANAGER(ObjTmp->Dialog),
				    &ObjTmp->ordonnee,
				    "rbAGLegend",
				    legendes_xpm,
				    "rbAGDatas",
				    donnees_xpm,
				    NULL);

 F(ObjTmp->ri_FLL->Toggles[0]).Add_Callback(ObjTmp->ri_FLL->Toggles[0],
					    XmNarmCallback,
					    change_ORD1, ObjTmp);
 F(ObjTmp->ri_FLL->Toggles[1]).Add_Callback(ObjTmp->ri_FLL->Toggles[1], 
					    XmNarmCallback,
					    change_ORD0, ObjTmp);
  
 xmsBaratinRow = F(GlobResources).getXmString(GlobResources,
					      XcR_askGraphRow);
 xmsBaratinCol = F(GlobResources).getXmString(GlobResources,
					      XcR_askGraphCol);
 xmsBaratinRow2 = F(GlobResources).getXmString(GlobResources,
					       XcR_askGraphRow2);
 xmsBaratinCol2 = F(GlobResources).getXmString(GlobResources,
					       XcR_askGraphCol2);
  
 XtSetArg(argsw[0], XmNlabelString, xmsBaratinRow);
 XtSetArg(argsw2[0], XmNlabelString, xmsBaratinCol);
 XtSetArg(argsw3[0], XmNlabelString, xmsBaratinRow2);
 XtSetArg(argsw4[0], XmNlabelString, xmsBaratinCol2);
 if(ObjTmp->col_row)
 {
  XtSetValues(WIDGET(ObjTmp->l_FCL), argsw, 1);
  XtSetValues(WIDGET(ObjTmp->l_FLL), argsw2, 1);
  XtSetValues(ObjTmp->Frame2->w_Label, argsw3, 1);
  XtSetValues(ObjTmp->Frame3->w_Label, argsw4, 1);
 }
 else
 {
  XtSetValues(WIDGET(ObjTmp->l_FCL), argsw2, 1);
  XtSetValues(WIDGET(ObjTmp->l_FLL), argsw, 1);
  XtSetValues(ObjTmp->Frame2->w_Label, argsw4, 1);
  XtSetValues(ObjTmp->Frame3->w_Label, argsw3, 1);
 }
 XmStringFree(xmsBaratinRow);
 XmStringFree(xmsBaratinCol);
 XmStringFree(xmsBaratinRow2);
 XmStringFree(xmsBaratinCol2);


 ObjTmp->Frame4 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog),
					  NULL, 0);
 XtSetArg(argsw[0], XmNshadowThickness, 0);
 XtSetArg(argsw[1], XmNbackground, 
	  WhitePixel(display, DefaultScreen(display)));
 ObjTmp->drawing = XmCreateDrawingArea(WIDGET(ObjTmp->Frame4),
				       "daBoxAskGraph", argsw, 2);
 Xc_REGISTER_HELP(ObjTmp->drawing);
 XtAddCallback(ObjTmp->drawing, XmNexposeCallback,
	       (XtCallbackProc)eventlook_drawing, ObjTmp);

 F(MANAGER(ObjTmp->Dialog)).Add_child(MANAGER(ObjTmp->Dialog),
				      ObjTmp->drawing,
				      WIDGET(ObjTmp->Frame4));

 F(ObjTmp->GridFLL).addColumn(ObjTmp->GridFLL, XcG_MAX_SIZE,
			      WIDGET(ObjTmp->l_FLL), XcG_BEST_SIZE,
			      WIDGET(ObjTmp->ri_FLL), XcG_MAX_SIZE,
			      NULL);

 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Ltitre), 30,
			   WIDGET(ObjTmp->Frame1), XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Frame2), XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Frame3), XcG_MAX_SIZE,
			   NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 10, NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, GRAPH_DRAWING_SIZE,
			   WIDGET(ObjTmp->Frame4), GRAPH_DRAWING_SIZE,
			   NULL);

 F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, 
				    XcCD_OK | XcCD_CANCEL,
				    XcCD_OK,
				    TRUE, NULL, NULL);
  
 F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);

 Xc_TRACE(("Object Box_AskGraph build"));
  
 {
  scale_t	factor;
  scale_t	scale;
    
  F(highg).set_X_info(highg, XtDisplay(ObjTmp->drawing),
		      XtWindow(ObjTmp->drawing));
  HG_CHANGE_XINFO(highg->HG_object)(highg->HG_object);
  F(highg->matrix).identity(highg->matrix, FALSE);
  highg->scale = COORD_DEFAULT * 2;
  scale = SCALE_FROM_COEF(1.0 / highg->scale);
  factor = ((scale_t)(GRAPH_DRAWING_SIZE * highg->scale)) / BG_AREA_WIDTH;
  F(highg->matrix).scale(highg->matrix, FALSE, scale, scale);
  F(highg->matrix).scale(highg->matrix, FALSE, factor, factor);
 }
 Xc_TRACE(("Object Box_AskGraph build"));
 return ObjTmp;
}

void dest_Box_AskGraph(This)
c_Box_AskGraph *This;
{
 XtDestroyWidget(This->drawing);
 DELETE(c_RadioImage)(This->ri_LC);
 DELETE(c_RadioImage)(This->ri_FCL);
 DELETE(c_RadioImage)(This->ri_FLL);
 DELETE(c_Label)(This->Ltitre);
 DELETE(c_Label)(This->l_LC);
 DELETE(c_Label)(This->l_FCL);
 DELETE(c_Label)(This->l_FLL);
 DELETE(c_Grid)(This->GridLC);
 DELETE(c_Grid)(This->GridFCL);
 DELETE(c_Grid)(This->GridFLL);
 DELETE(c_Frame)(This->Frame1);
 DELETE(c_Frame)(This->Frame2);
 DELETE(c_Frame)(This->Frame3);
 DELETE(c_Frame)(This->Frame4);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);
  
 Xc_free(This);
 Xc_TRACE(("Object Box_AskGraph destroyed"));
}

void *copy_Box_AskGraph(This)
c_Box_AskGraph *This;
{
 Xc_TRACE(("Copy not implemented"));
 return NULL;
}

static void eventlook_drawing(w, This, cbs)
Widget				w;
c_Box_AskGraph			*This;
XmDrawingAreaCallbackStruct	*cbs;
{
 XEvent	*event;
  
 event = cbs->event;
 switch(event->type)
 {
 case Expose:
 case GraphicsExpose:
  HG_DISPLAY_REDRAW(This->highg->HG_object)(This->highg->HG_object, FALSE);
  break;
 default:
  Xc_TRACE(("Unknown event"));
  break;
 }
}


static void change_LC1(w, This)
Widget		w;
c_Box_AskGraph	*This;
{
 Arg		 argsw[2];
 XmString       xmsBaratin;

 Xc_HISTORY(("LC1"));
 if (!This->highg->col_row)
 {
  xmsBaratin = F(GlobResources).getXmString(GlobResources, 
					    XcR_askGraphCol);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(WIDGET(This->l_FCL), argsw, 1);
  XmStringFree(xmsBaratin);
      
  xmsBaratin = F(GlobResources).getXmString(GlobResources, 
					    XcR_askGraphCol2);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(This->Frame2->w_Label, argsw, 1);
  XmStringFree(xmsBaratin);
      
  xmsBaratin = F(GlobResources).getXmString(GlobResources,
					    XcR_askGraphRow);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(WIDGET(This->l_FLL), argsw, 1);
  XmStringFree(xmsBaratin);
      
  xmsBaratin = F(GlobResources).getXmString(GlobResources,
					    XcR_askGraphRow2);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(This->Frame3->w_Label, argsw, 1);
  XmStringFree(xmsBaratin);
      
  This->highg->col_row = TRUE;
      
  XClearWindow(This->highg->X_info.display,
	       This->highg->X_info.window);
  F(This->highg).calcul_and_draw(This->highg);
 }
}

static void change_LC0(w, This)
Widget		w;
c_Box_AskGraph	*This;
{
 Arg		 argsw[2];
 XmString       xmsBaratin;

 Xc_HISTORY(("LC0"));
  
 if (This->highg->col_row)
 {
  xmsBaratin = F(GlobResources).getXmString(GlobResources,
					    XcR_askGraphRow);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(WIDGET(This->l_FCL), argsw, 1);
  XmStringFree(xmsBaratin);
      
  xmsBaratin = F(GlobResources).getXmString(GlobResources,
					    XcR_askGraphRow2);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(This->Frame2->w_Label, argsw, 1);
  XmStringFree(xmsBaratin);
    
  xmsBaratin = F(GlobResources).getXmString(GlobResources,
					    XcR_askGraphCol);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(WIDGET(This->l_FLL), argsw, 1);
  XmStringFree(xmsBaratin);
      
  xmsBaratin = F(GlobResources).getXmString(GlobResources,
					    XcR_askGraphCol2);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(This->Frame3->w_Label, argsw, 1);
  XmStringFree(xmsBaratin);
      
  This->highg->col_row = FALSE;
      
  XClearWindow(This->highg->X_info.display,
	       This->highg->X_info.window);
  F(This->highg).calcul_and_draw(This->highg);
 }
}

static void change_ABS1(w, This)
Widget		w;
c_Box_AskGraph	*This;
{
 Xc_HISTORY(("ABS1"));
 if (!This->highg->abscisse)
 {
  This->highg->abscisse = TRUE;
      
  XClearWindow(This->highg->X_info.display,
	       This->highg->X_info.window);
  F(This->highg).calcul_and_draw(This->highg);
 }
}

static void change_ABS0(w, This)
Widget		w;
c_Box_AskGraph	*This;
{
 Xc_HISTORY(("ABS0"));
 if (This->highg->abscisse)
 {
  This->highg->abscisse = FALSE;
      
  XClearWindow(This->highg->X_info.display,
	       This->highg->X_info.window);
  F(This->highg).calcul_and_draw(This->highg);
 }
}

static void change_ORD1(w, This)
Widget		w;
c_Box_AskGraph	*This;
{
 Xc_HISTORY(("ORD1"));
 if (!This->highg->ordonnee)
 {
  This->highg->ordonnee = TRUE;
      
  XClearWindow(This->highg->X_info.display,
	       This->highg->X_info.window);
  F(This->highg).calcul_and_draw(This->highg);
 }
}

static void change_ORD0(w, This)
Widget		w;
c_Box_AskGraph	*This;
{
 Xc_HISTORY(("ORD0"));
 if (This->highg->ordonnee)
 {
  This->highg->ordonnee = FALSE;
      
  XClearWindow(This->highg->X_info.display,
	       This->highg->X_info.window);
  F(This->highg).calcul_and_draw(This->highg);
 }
}

static void cancel_change(This)
c_Box_AskGraph	*This;
{
 This->highg->col_row = This->old_col_row;
 This->highg->abscisse = This->old_abscisse;
 This->highg->ordonnee = This->old_ordonnee;
 F(This->highg).calcul_and_draw(This->highg);
}
