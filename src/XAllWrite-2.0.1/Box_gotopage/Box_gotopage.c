/*
** Box_gotopage.c for Xclamation and XAllWrite in Box_gotopage/
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
** Started on  Fri Oct 23 15:57:35 1998 Emmanuel Paris
** Last update Wed Nov 11 18:39:00 1998 Emmanuel Paris
*/

#include "Box_gotopage.h"

void *cons_Box_gotopage();
void dest_Box_gotopage();
void *copy_Box_gotopage();

static void certify_pagenum ___PROTO((c_WTextField *this,
				      c_Box_gotopage *Obj));

sf_Box_gotopage fc_Box_gotopage =
{
 cons_Box_gotopage,
 dest_Box_gotopage,
 copy_Box_gotopage
};

void *cons_Box_gotopage(w_Parent, pszTitle, nb_page, selpg)
Widget		w_Parent;
char		*pszTitle;
int		nb_page;
int		*selpg;
{
 c_Box_gotopage		*This;
 char			str[10];
 Display		*display;
  
 if((This = (c_Box_gotopage *)Xc_malloc("Box_gotopage",
				      sizeof(c_Box_gotopage))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
  
 This->f = &fc_Box_gotopage;

 This->nb_page = nb_page;
 This->selected_page = *selpg;
 This->ret_selpg = selpg;
  
 This->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
						      pszTitle, 280, 150,
						      XcCD_HORIZONTAL
						      | XcCD_HRESIZE
						      | XcCD_VRESIZE);
 This->w_This = WIDGET(This->Dialog);
 display = XtDisplay(WIDGET(This->Dialog));

 This->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(This),
				      MANAGER(This->Dialog),
				      "gBoxGotoPage", FALSE);

 This->Ltitre = 
  (c_Label *)NEW(c_Label)(WIDGET(This->Grid),
			  MANAGER(This->Dialog),
			  "lBoxGotoPageTitle");

  
 This->FramePage = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					   MANAGER(This->Dialog), 
					   "~fGotoPage", 0);

 This->GridPage = (c_Grid *)NEW(c_Grid)(WIDGET(This->FramePage),
					MANAGER(This->Dialog),
					"gPageNumber", TRUE);
 
 This->LnumPage = 
  (c_Label *)NEW(c_Label)(WIDGET(This->GridPage),
			  MANAGER(This->Dialog),
			  "lPage");
 
 sprintf(str, "%d", *selpg);

 This->SpinBoxPage = (c_SpinBox *)NEW(c_SpinBox)
  (WIDGET(This->GridPage), MANAGER(This->Dialog), "sbPageNumber",
   XcSB_ARROWS_LEFT_RIGHT, TF_INTEGER, 1.0, (real)nb_page, str, 4);
						 
 F(This->SpinBoxPage).addCallback(This->SpinBoxPage, (void *)certify_pagenum,
				  This, SB_TEXTFIELD_CALLBACK);
 F(This->SpinBoxPage).setAutoRepeatDelay(This->SpinBoxPage, 
					 SB_DEFAULT_AUTO_REPEAT);
 F(This->SpinBoxPage).setIncrement(This->SpinBoxPage, (real)1.0);
 
 
 This->LofPage = (c_Label *)NEW(c_Label)(WIDGET(This->GridPage),
					 MANAGER(This->Dialog),
					 "lOfPage");
 
 {
  XmString xms1, xms2, xms;
  
  
  XtVaGetValues(WIDGET(This->LofPage), XmNlabelString, &xms1, NULL);
  
  sprintf(str, " %d", nb_page);
  xms2 = XmStringCreateLtoR(str, XmSTRING_DEFAULT_CHARSET);
  xms = XmStringConcat(xms1, xms2);
  
  XtVaSetValues(WIDGET(This->LofPage), XmNlabelString, xms, NULL);
  XmStringFree(xms1);
  XmStringFree(xms2);
  XmStringFree(xms);  
 }
 
 F(This->GridPage).addColumn(This->GridPage, XcG_BEST_SIZE,
			     WIDGET(This->LnumPage), XcG_MAX_SIZE,
			     WIDGET(This->SpinBoxPage), XcG_BEST_SIZE,
			     WIDGET(This->LofPage), XcG_MAX_SIZE,
			     NULL);
  
 F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE,
			   WIDGET(This->Ltitre), 30,
			   WIDGET(This->FramePage), 45,
			   NULL);

 F(This->Dialog).createActionArea(This->Dialog, 
				    XcCD_OK | XcCD_CANCEL,
				    XcCD_OK,
				    TRUE, NULL, NULL);
  
 F(This->Dialog).map(This->Dialog, TRUE);
 Xc_TRACE(("Object Box_page build"));
 return This;
}

void dest_Box_gotopage(This)
c_Box_gotopage *This;
{
 DELETE(c_Label)(This->LofPage);
 DELETE(c_SpinBox)(This->SpinBoxPage);
 DELETE(c_Label)(This->LnumPage);
 DELETE(c_Grid)(This->GridPage);
 DELETE(c_Frame)(This->FramePage);
 DELETE(c_Label)(This->Ltitre);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);

 *This->ret_selpg = This->selected_page;

 Xc_free(This);
 Xc_TRACE(("Object Box_gotopage destroyed"));
}

void *copy_Box_gotopage(This)
c_Box_gotopage *This;
{
 Xc_TRACE(("Copy not implemented"));
 return NULL;
}


static void certify_pagenum(tf, This)
c_WTextField	*tf;
c_Box_gotopage	*This;
{
 char	*vstr;
 unsigned int	num;

 if((vstr = F(tf).get(tf)) != NULL)
 {
  num = atoi(vstr);
  Xc_free(vstr);
  
  if (num != 0)
   This->selected_page = num;
  
  F(This->Dialog).resetTraversal(This->Dialog);
 }
}
