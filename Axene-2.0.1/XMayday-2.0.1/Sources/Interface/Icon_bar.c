/*
** Icon_bar.c for XMayday in Interface/
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
** Last update Sat Jan 29 17:32:17 2000 Emmanuel Paris
*/

#include "Interface.h"
#include "InterfaceP.h"

#include "../Pixmaps/xmayday.prev.zpm"
#include "../Pixmaps/xmayday.next.zpm"
#include "../Pixmaps/xmayday.indx.zpm"
#include "../Pixmaps/xmayday.prev.arm.zpm"
#include "../Pixmaps/xmayday.next.arm.zpm"
#include "../Pixmaps/xmayday.indx.arm.zpm"

#include "../Pixmaps/document.open.zpm"
#include "../Pixmaps/create.index.zpm"
#include "../Pixmaps/document.imprimer.zpm"
#include "../Pixmaps/hradian.zpm"


/* ------------------------------------------------------------------------ **
** Icon Bar Creation --                                                     ** 
** ------------------------------------------------------------------------ */
void create_icon_bar(This)
c_Interface *This;
{
  Arg      args[2];
  Display  *display;
  
  XtSetArg(args[0], XmNshadowType, XmSHADOW_IN);
  XtSetArg(args[1], XmNshadowThickness, 1);
  This->fIconBar = XmCreateFrame(WIDGET(This->foBottom), "fIconBar", args, 2);
  F(This->Manager).Add_child(This->Manager, This->fIconBar,
			     WIDGET(This->foBottom));

  This->foIconBar = (c_Form *)
    NEW(c_Form)(This->fIconBar, This->Manager, "foIconBar");

  XtSetArg(args[0], XmNshadowType, XmSHADOW_OUT);
  XtSetArg(args[1], XmNshadowThickness, 1);
  This->fuIconBar = XmCreateFrame(WIDGET(This->foIconBar), "fuIconBar", args, 2);
  F(This->Manager).Add_child(This->Manager, This->fuIconBar,
			     WIDGET(This->foIconBar));


  This->IconBar = (c_Form *)
    NEW(c_Form)(This->fuIconBar, This->Manager, "hIconBar");

/*
  This->IconBar = (c_RowColumn *)
    NEW(c_RowColumn)(This->fuIconBar, This->Manager, 
		     "hIconBar", 32, 32, XmHORIZONTAL);
  XtSetArg(args[0], XmNpacking, XmPACK_COLUMN);
  XtSetArg(args[1], XmNadjustLast, True);
  XtSetValues(WIDGET(This->IconBar), args, 2);
*/
  display = XtDisplay(This->fIconBar);

  This->Icon[I_NAVIGATE_PREV] = (c_Icon *)NEW(c_Icon)
    (WIDGET(This->IconBar), This->Manager, "IconNav_prev", display,
     xmayday_prev_xpm, xmayday_prev_arm_xpm, XcPush);
  CALLBACK(WIDGET(This->Icon[I_NAVIGATE_PREV]), FUNC_ACTIVATE, 
	   SelectFunction,(void *)VIEW_PREVIOUS_ANCHOR);
  
  This->Icon[I_NAVIGATE_NEXT] = (c_Icon *)NEW(c_Icon)
    (WIDGET(This->IconBar), This->Manager, "IconNav_next", display,
     xmayday_next_xpm, xmayday_next_arm_xpm, XcPush);
  CALLBACK(WIDGET( This->Icon[I_NAVIGATE_NEXT]), FUNC_ACTIVATE, 
	   SelectFunction,(void *)VIEW_NEXT_ANCHOR);
  
   This->Icon[I_NAVIGATE_INDEX] = (c_Icon *)NEW(c_Icon)
    (WIDGET(This->IconBar), This->Manager, "IconNav_indx", display,
     xmayday_indx_xpm, xmayday_indx_arm_xpm, XcPush);
  CALLBACK(WIDGET(This->Icon[I_NAVIGATE_INDEX]), FUNC_ACTIVATE, 
	   SelectFunction,(void *)VIEW_HOME_PAGE);
  
  This->HelpLine = (c_HelpLine *)NEW(c_HelpLine)(XcHL_LABEL_GIVEN, 
						 WIDGET(This->lHelpLine));
  F(This->HelpLine).change_function_message(This->HelpLine, 
					    XcR_NONE, XcR_DEFAULT);
  SET_HELPLINE(This->HelpLine, 
	       WIDGET(This->Icon[I_NAVIGATE_PREV]),helpGoBack, XcR_NONE);
  SET_HELPLINE(This->HelpLine, 
	       WIDGET(This->Icon[I_NAVIGATE_NEXT]),helpGoForward, XcR_NONE);
  SET_HELPLINE(This->HelpLine, 
	       WIDGET(This->Icon[I_NAVIGATE_INDEX]),helpGoIndex, XcR_NONE);

  This->Icon[I_RADIAN] = (c_Icon *)NEW(c_Icon)
   (WIDGET(This->IconBar), This->Manager, "IconRadian", display,
    hradian_xpm, NULL, XcLabel);

  XtSetArg(args[0], XmNalignment, XmALIGNMENT_END);
  XtSetValues(WIDGET(This->Icon[I_RADIAN]), args, 1);
  
  F(This->IconBar).attach_left(WIDGET(This->IconBar),
			       WIDGET(This->Icon[I_NAVIGATE_NEXT]),
			       WIDGET(This->Icon[I_NAVIGATE_PREV]));
  F(This->IconBar).attach_left(WIDGET(This->IconBar),
			       WIDGET(This->Icon[I_NAVIGATE_INDEX]),
			       WIDGET(This->Icon[I_NAVIGATE_NEXT]));

/*  F(This->IconBar).attach_left(WIDGET(This->IconBar),
				WIDGET(This->Icon[I_RADIAN]),
				WIDGET(This->Icon[I_NAVIGATE_INDEX]));
*/
  F(This->IconBar).attach_right(WIDGET(This->IconBar),
				WIDGET(This->Icon[I_RADIAN]),
				NULL);
  F(This->IconBar).attach_top(WIDGET(This->IconBar),
			      WIDGET(This->Icon[I_RADIAN]),
			      NULL);
  F(This->IconBar).attach_bottom(WIDGET(This->IconBar),
				 WIDGET(This->Icon[I_RADIAN]),
				 NULL);

  This->IconBar2 = (c_RowColumn *)
    NEW(c_RowColumn)(WIDGET(This->foIconBar), This->Manager, 
		     "hIconBar2", 0, 34, XmHORIZONTAL);
  XtSetArg(args[0], XmNradioAlwaysOne, True);
  XtSetArg(args[1], XmNradioBehavior, True);
  XtSetValues(WIDGET(This->IconBar2), args, 2);

  This->Icon[I_OPEN] = (c_Icon *)NEW(c_Icon)
    (WIDGET(This->IconBar2), This->Manager, "IconOpen", display,
     document_open_xpm, NULL, XcPush);
  CALLBACK(WIDGET(This->Icon[I_OPEN]), FUNC_ACTIVATE, 
	   MenuFileOpen, This);

  This->Icon[I_MOVEI] = (c_Icon *)NEW(c_Icon)
    (WIDGET(This->IconBar2), This->Manager, "IconMoveI", display,
     create_index_xpm, NULL, XcPush);
  CALLBACK(WIDGET(This->Icon[I_MOVEI]), FUNC_ACTIVATE, 
	   SelectFunction, (void *)CREATE_INDEX);

  This->Icon[I_PRINT] = (c_Icon *)NEW(c_Icon)
    (WIDGET(This->IconBar2), This->Manager, "IconPrint", display,
     document_imprimer_xpm, NULL, XcPush);
  CALLBACK(WIDGET(This->Icon[I_PRINT]), FUNC_ACTIVATE, 
	   MenuFilePrint, This);

  SET_HELPLINE(This->HelpLine, 
	       WIDGET(This->Icon[I_OPEN]),helpOpen, XcR_NONE);
  SET_HELPLINE(This->HelpLine, 
	       WIDGET(This->Icon[I_MOVEI]),helpMoveIndex, XcR_NONE);
  SET_HELPLINE(This->HelpLine, 
	       WIDGET(This->Icon[I_PRINT]),helpPrint, XcR_NONE);


  F(This->foIconBar).attach_right(WIDGET(This->foIconBar),
				  WIDGET(This->IconBar2), NULL);
  F(This->foIconBar).attach_left(WIDGET(This->foIconBar),
				  This->fuIconBar, NULL);
  F(This->foIconBar).attach_right(WIDGET(This->foIconBar),
				  This->fuIconBar, WIDGET(This->IconBar2));

}

/* ------------------------------------------------------------------------ **
** Icon Bar Destruction --                                                  ** 
** ------------------------------------------------------------------------ */
void destroy_icon_bar(This)
c_Interface *This;
{
  int i;
  
  for(i = 0; i < NB_ICON; i++)
  {
    DELETE(c_Icon)(This->Icon[i]);
  }
  
  DELETE(c_Form)(This->IconBar);
  DELETE(c_RowColumn)(This->IconBar2);
  XtDestroyWidget(This->fuIconBar);
  DELETE(c_Form)(This->foIconBar);
  XtDestroyWidget(This->fIconBar);
  
  DELETE(c_HelpLine)(This->HelpLine);
}








