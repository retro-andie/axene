/*
** Box_legal.c for Xclamation, XQuad and XAllWrite in Box_legal/
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
** Started on  Tue Nov 14 11:48:46 1995 Robin Castanier
** Last update Sun Jan  9 17:08:44 2000 Emmanuel Paris
*/

#include <unistd.h>
#include <netdb.h>

#include "Box_legal.h"
#include "Colormap.h"
#include "Resource.h"
#include "Actions.h"
#include "version.h"
#include "MainInterface.h"
#include "Readkey.h"
#include "../Pixmaps/logo_editor.zpm"
#include "../Pixmaps/logo_soft_big.zpm"

#define NB_DAYS_BEFORE_BLINK 10

extern c_Resource *GlobResources;
extern c_Colormap *GlobColormap;
extern c_MainInterface *MainInterface1;

void *cons_Box_legal();
void dest_Box_legal();
void callback_dest_BoxLegal();
void blink_legal();
int  display_to_host_legal();

sf_Box_legal fc_Box_legal =
{
 cons_Box_legal,
 dest_Box_legal,
};

void *cons_Box_legal(w_Parent, pszTitle)
Widget w_Parent;
char   *pszTitle;
{
 Display        *display;
 c_Box_legal	 *ObjTmp;
 Arg		 argsw[4];
 int		 nargs;
 char           pszVlabel[100];
 XmString       xmsBaratin;
 XmString	xmsInfoLine1 = NULL;
 XmString	xmsInfoLine2 = NULL;
 char		*license_file;
 char		*full_license_file;
  
 if ((ObjTmp = (c_Box_legal *)Xc_malloc("Box_legal",
					sizeof(c_Box_legal))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_Box_legal;
 ObjTmp->app = XtWidgetToApplicationContext(w_Parent);
 ObjTmp->bBlink = FALSE;

#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
 ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							pszTitle, 540, 580,
							XcCD_HORIZONTAL
							| XcCD_VRESIZE);
#else
 if (gkinfo.valid == Xc_KEY_GOOD)
  ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							 pszTitle, 540, 580,
							 XcCD_HORIZONTAL
							 | XcCD_VRESIZE);
 else
  ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							 pszTitle, 540, 480,
							 XcCD_HORIZONTAL
							 | XcCD_VRESIZE);
#endif
  
 ObjTmp->w_This = WIDGET(ObjTmp->Dialog);
 allow_ba_action(ObjTmp->w_This);
 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				      MANAGER(ObjTmp->Dialog),
				      "gBoxLegal", TRUE);
  
 display = XtDisplay(w_Parent);

 nargs = 0;

 ObjTmp->Label1 = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog),
					  "lLegalVersion" );
  
 nargs = 0;

#ifdef PL
 sprintf(pszVlabel,
	 F(GlobResources).getString(GlobResources, 
				    XcR_aboutReleaseBeta),
	 MAJOR, MINOR, PL);
#else
 sprintf(pszVlabel,
	 F(GlobResources).getString(GlobResources, 
				    XcR_aboutRelease),
	 MAJOR, MINOR);
#endif  

#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
 if (gkinfo.valid != Xc_KEY_GOOD)
 {
  strcat(pszVlabel, "\n(UNREGISTERED)");
 }
 else
 {
  strcat(pszVlabel, "\nRegistered");
 }
#else
 if (gkinfo.valid == Xc_KEY_GOOD)
 {
  strcat(pszVlabel, "\nRegistered");
 }
#endif 
  
 xmsBaratin = XmStringCreateLtoR(pszVlabel, XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[nargs], XmNlabelString, xmsBaratin);
 nargs++;
 XtSetValues(WIDGET(ObjTmp->Label1), argsw, nargs);
 XmStringFree(xmsBaratin);
  
 nargs = 0;
 XtSetArg(argsw[nargs], XmNshadowType, XmSHADOW_IN); nargs++;
 XtSetArg(argsw[nargs], XmNmarginWidth, 7); nargs++;
 XtSetArg(argsw[nargs], XmNmarginHeight, 5); nargs++;

 ObjTmp->w_Frame1 = XmCreateFrame(WIDGET(ObjTmp->Grid), "fLicense",
				  argsw, nargs);

 F(MANAGER(ObjTmp->Dialog)).Add_child(MANAGER(ObjTmp->Dialog),
				      ObjTmp->w_Frame1, WIDGET(ObjTmp->Grid));

 ObjTmp->SText = 
  (c_ScrolledWindow *)NEW(c_ScrolledWindow)(ObjTmp->w_Frame1,
					    MANAGER(ObjTmp->Dialog),
					    "stBoxLicense");
 
 ObjTmp->LText = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->SText),
					 MANAGER(ObjTmp->Dialog),
					 "lBoxLicense");
 
 {
  FILE *file;
  long fsize;
  char *text;
  
  
  license_file = F(GlobResources).getString(GlobResources,
					    XcR_licenseFileName);

  full_license_file = TranslateFilename("license", license_file);
  
  if ((file = fopen(full_license_file, "rb")))
  {
   fseek(file, 0L, SEEK_END);
   fsize = ftell(file);
   fseek(file, 0L, SEEK_SET);
   
   text = (char *)Xc_malloc("tl", fsize + 1);
   if (!text || fread( text, fsize, 1, file) != 1)
   {
    printf("fread fault\n");
    return NULL;
   }
   text[fsize] = '\0';
   
   xmsBaratin =  XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET);
   XtVaSetValues(WIDGET(ObjTmp->LText), XmNlabelString, xmsBaratin, NULL);
   XmStringFree(xmsBaratin);
   
   XtSetArg(argsw[0], XmNworkWindow, WIDGET(ObjTmp->LText));
   XtSetValues(WIDGET(ObjTmp->SText), argsw, 1);
   
   Xc_free(text);
   fclose(file);
  }
 }
 
 ObjTmp->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  NULL, 0);
 ObjTmp->Grid2 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame2),
				       MANAGER(ObjTmp->Dialog),
				       "gBLFrame2", TRUE);
 ObjTmp->Grid2_1 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Grid2),
					 MANAGER(ObjTmp->Dialog),
					 "gBLFrame2_1", TRUE);
 
 ObjTmp->IconSoft = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->Grid2), 
					  MANAGER(ObjTmp->Dialog),
					  "iBoxLegalSoft", display, 
					  logo_soft_big_xpm, NULL, XcLabel);
  
 ObjTmp->Icon = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->Grid2_1), 
				      MANAGER(ObjTmp->Dialog),
				      "iBoxLegal", display, 
				      logo_editor_xpm, NULL, XcLabel);

 ObjTmp->LabelAdress = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2_1),
					       MANAGER(ObjTmp->Dialog),
					       "lLegalAdress");
 ObjTmp->LabelCopy = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2_1),
					     MANAGER(ObjTmp->Dialog),
					     "lLegalCopy");
 
#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
 ObjTmp->Frame3 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  NULL, 0);
 ObjTmp->Grid3 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame3),
				       MANAGER(ObjTmp->Dialog),
				       "gBLFrame3", FALSE);

 ObjTmp->lDeadLineDate = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid3),
						 MANAGER(ObjTmp->Dialog), 
						 "lDeadlineDate");
  
 ObjTmp->lDeadLineDay = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid3),
						MANAGER(ObjTmp->Dialog), 
						"lDeadlineDay");
  
 switch(gkinfo.valid)
 {
 case Xc_KEY_GOOD:
  sprintf(pszVlabel,
	  F(GlobResources).getString(GlobResources, XcR_keyFirst),
	  gkinfo.name);
  xmsInfoLine1 = XmStringCreate(pszVlabel, XmSTRING_DEFAULT_CHARSET);
  
  sprintf(pszVlabel,
	  F(GlobResources).getString(GlobResources, XcR_keySecond),
	  gkinfo.society);
  xmsInfoLine2 = XmStringCreate(pszVlabel, XmSTRING_DEFAULT_CHARSET);
  break;

 case Xc_KEY_EXPIRED:
  
  nargs = 0;
  
  XtSetArg(argsw[nargs], XmNforeground,
	   ((GlobColormap->depth == 1) ? 
	    GlobColormap->Cwhite : GlobColormap->Cred));
  nargs++;
  XtSetArg(argsw[nargs], XmNbackground, GlobColormap->Cblack);
  nargs++;
  XtSetValues(WIDGET(ObjTmp->lDeadLineDate), argsw, nargs);
  
  sprintf(pszVlabel,
	  F(GlobResources).getString(GlobResources,
				     XcR_deadlineExpired));
  
  xmsInfoLine1= XmStringCreate(pszVlabel, XmSTRING_DEFAULT_CHARSET);
  
  ObjTmp->bBlink = TRUE;
  nargs = 0;
  ObjTmp->id = XtAppAddTimeOut(ObjTmp->app, 1000L, blink_legal,
			       ObjTmp);
  XtSetArg(argsw[nargs], XmNbackground, GlobColormap->Cblack);
  nargs++;
  XtSetValues(WIDGET(ObjTmp->lDeadLineDay), argsw, nargs);
  
  sprintf(pszVlabel,
	  F(GlobResources).getString(GlobResources,
				     XcR_deadlineExpired2));
  
  xmsInfoLine2 = XmStringCreate(pszVlabel, XmSTRING_DEFAULT_CHARSET);
  
  break;
 case Xc_KEY_NOT_VALID:
  xmsInfoLine1 = 
   F(GlobResources).getXmString(GlobResources, XcR_noVKeyFirst);
  xmsInfoLine2 = 
   F(GlobResources).getXmString(GlobResources, XcR_noVKeySecond);
  break;
 case Xc_KEY_NO_KEY:
  xmsInfoLine1 = 
   F(GlobResources).getXmString(GlobResources, XcR_noKeyFirst);
  xmsInfoLine2 = 
   F(GlobResources).getXmString(GlobResources, XcR_noKeySecond);
  break;
 }
 
 nargs = 0;
 XtSetArg(argsw[nargs], XmNlabelString, xmsInfoLine1);
 nargs++;
 XtSetValues(WIDGET(ObjTmp->lDeadLineDate), argsw, nargs);
 XmStringFree(xmsInfoLine1);
 
 nargs = 0;
 XtSetArg(argsw[nargs], XmNlabelString, xmsInfoLine2);
 nargs++;
 XtSetValues(WIDGET(ObjTmp->lDeadLineDay), argsw, nargs);
 XmStringFree(xmsInfoLine2);
#else /* Xc_REGISTER_LEVEL == 0 */
 if (gkinfo.valid == Xc_KEY_GOOD)
 {
  ObjTmp->Frame3 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					   MANAGER(ObjTmp->Dialog), 
					  NULL, 0);
  ObjTmp->Grid3 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame3),
					MANAGER(ObjTmp->Dialog),
					"gBLFrame3", FALSE);
  
  ObjTmp->lDeadLineDate = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid3),
						  MANAGER(ObjTmp->Dialog), 
						  "lDeadlineDate");
  
  ObjTmp->lDeadLineDay = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid3),
						 MANAGER(ObjTmp->Dialog), 
						 "lDeadlineDay");
  
  sprintf(pszVlabel,
	  F(GlobResources).getString(GlobResources, XcR_keyFirst),
	  gkinfo.name);
   xmsInfoLine1 = XmStringCreate(pszVlabel, XmSTRING_DEFAULT_CHARSET);
  
  sprintf(pszVlabel,
	  F(GlobResources).getString(GlobResources, XcR_keySecond),
	  gkinfo.society);
  xmsInfoLine2 = XmStringCreate(pszVlabel, XmSTRING_DEFAULT_CHARSET);
  
  nargs = 0;
  XtSetArg(argsw[nargs], XmNlabelString, xmsInfoLine1);
  nargs++;
  XtSetValues(WIDGET(ObjTmp->lDeadLineDate), argsw, nargs);
  XmStringFree(xmsInfoLine1);
  
  nargs = 0;
  XtSetArg(argsw[nargs], XmNlabelString, xmsInfoLine2);
  nargs++;
  XtSetValues(WIDGET(ObjTmp->lDeadLineDay), argsw, nargs);
  XmStringFree(xmsInfoLine2);
 }
 else
 {
  ObjTmp->Frame3 = NULL;
  ObjTmp->Grid3 = NULL;
  ObjTmp->lDeadLineDate = ObjTmp->lDeadLineDay = NULL;
 }
  
#endif  
  
 F(ObjTmp->Grid2_1).addColumn(ObjTmp->Grid2_1, XcG_MAX_SIZE,
			      WIDGET(ObjTmp->LabelAdress), XcG_MAX_SIZE,
			      WIDGET(ObjTmp->Icon), 142,
			      NULL);
 F(ObjTmp->Grid2_1).addColumn(ObjTmp->Grid2_1, XcG_BEST_SIZE,
			      WIDGET(ObjTmp->LabelCopy), XcG_MAX_SIZE,
			      NULL);
  
 F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->IconSoft), 120,
			    WIDGET(ObjTmp->Grid2_1), XcG_MAX_SIZE,
			    NULL);

#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
 F(ObjTmp->Grid3).addColumn(ObjTmp->Grid3, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lDeadLineDate), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lDeadLineDay), XcG_MAX_SIZE,
			    NULL);
#else
 if (gkinfo.valid == Xc_KEY_GOOD)
  F(ObjTmp->Grid3).addColumn(ObjTmp->Grid3, XcG_MAX_SIZE,
			     WIDGET(ObjTmp->lDeadLineDate), XcG_MAX_SIZE,
			     WIDGET(ObjTmp->lDeadLineDay), XcG_MAX_SIZE,
			     NULL);
#endif
  
#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 70,
			   WIDGET(ObjTmp->Label1), XcG_MAX_SIZE,
			   NULL);
#else
 if (gkinfo.valid == Xc_KEY_GOOD)
  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 70,
			    WIDGET(ObjTmp->Label1), XcG_MAX_SIZE,
			   NULL);
 else
  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 50,
			   WIDGET(ObjTmp->Label1), XcG_MAX_SIZE,
			    NULL);
#endif
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   ObjTmp->w_Frame1, XcG_MAX_SIZE,
			   NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 20,
			   NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 160,
			   WIDGET(ObjTmp->Frame2), XcG_MAX_SIZE,
			   NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 20,
			   NULL);

#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 70,
			   WIDGET(ObjTmp->Frame3), XcG_MAX_SIZE,
			   NULL);
#else
 if (gkinfo.valid == Xc_KEY_GOOD)
  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 70,
			    WIDGET(ObjTmp->Frame3), XcG_MAX_SIZE,
			    NULL);
#endif

#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
 if (gkinfo.valid != Xc_KEY_GOOD)
 {
  F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, XcCD_OK | XcCD_CANCEL,
				     XcCD_OK, FALSE, NULL, NULL);
 }
 else
 {
  F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, XcCD_CONTINUE,
				     XcCD_CONTINUE, FALSE, NULL, NULL);
 }
#else
 F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, XcCD_CONTINUE,
				    XcCD_CONTINUE, FALSE, NULL, NULL);
#endif
 
 F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);
  
 Xc_TRACE(("Object Box_legal build"));
 return ObjTmp;
}

void dest_Box_legal(This)
c_Box_legal *This;
{
 if (This->bBlink)
  XtRemoveTimeOut(This->id);
 if (This->lDeadLineDate) DELETE(c_Label)(This->lDeadLineDate);
 if (This->lDeadLineDay)  DELETE(c_Label)(This->lDeadLineDay);
 DELETE(c_Label)(This->Label1);

 DELETE(c_ScrolledWindow)(This->LText);
 DELETE(c_Label)(This->SText);
 
 DELETE(c_Label)(This->LabelAdress);
 DELETE(c_Label)(This->LabelCopy);
 DELETE(c_Icon)(This->IconSoft);
 DELETE(c_Icon)(This->Icon);
 DELETE(c_Grid)(This->Grid2);
 if (This->Grid3) DELETE(c_Grid)(This->Grid3);
 XtDestroyWidget(This->w_Frame1);
 DELETE(c_Frame)(This->Frame2);
 if (This->Frame3) DELETE(c_Frame)(This->Frame3);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);

 Xc_free(This);
 Xc_TRACE(("Object Box_legal destroyed"));
}

void callback_dest_BoxLegal(wid, This)
Widget	    wid;
c_Box_legal *This;
{
 DELETE(c_Box_legal)(This);
}

void blink_legal(This)
c_Box_legal *This;
{
 Arg		 argsw[4];

 This->id = XtAppAddTimeOut(This->app, 400L, blink_legal, This);
  
 XtSetArg(argsw[0], XmNforeground,
	  (This->bColor = !(This->bColor)) ? 
	  ((GlobColormap->depth == 1) ? 
	   GlobColormap->Cwhite : GlobColormap->Cred) 
	  : GlobColormap->Cblack);
 XtSetValues(WIDGET(This->lDeadLineDay), argsw, 1);
}

/* ----------------------------------------------------------------- **
** display_to_host_legal                                             **
** ----------------------------------------------------------------- */
int display_to_host_legal(display)
char **display;
{
 char	*tmp;
 int	i;

 if ((*display)[0]==':')
 {
  tmp = Xc_malloc("dspth", 50);
  if (gethostname(tmp, 50) == -1)
  {
   Xc_free(tmp);
   return -1;
  }
 }
 else
 {
  tmp = Xc_malloc("dspth", strlen(*display));
  i = 0;
  while ((*display)[i] != ':')
  {
   tmp[i] = (*display)[i];
   i++;
  }
  tmp[i] = 0;
 }
 *display = tmp;
 return 1;
}












