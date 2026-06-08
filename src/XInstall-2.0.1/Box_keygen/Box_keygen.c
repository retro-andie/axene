/*
** Box_keygen.c for XInstall in Box_keygen/
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Thu Jan 16 00:49:53 1997 Antoine Buat
** Last update Wed Apr 29 14:55:09 1998 Emmanuel Paris
*/

/*--- includes --- */
#include "Box_keygen.h"
#include "TestCDKey.h"
#include "TestIDFile.h"
#include "GenEndKey.h"
#include "AlertBox.h"
#include "MainInterface.h"
#include "Resource.h"
#include "DocIcon.h"
#include "../Pixmaps/logo_flip1.zpm"
#include "../Pixmaps/logo_flip2.zpm"
#include "../Pixmaps/logo_flip3.zpm"
#include "../Pixmaps/logo_flip4.zpm"

/*--- functions declaration --- */
static void *cons_Box_keygen();
static int BKGwaitForUnMap();
static void dest_Box_keygen();
static void *copy_Box_keygen();


/*--- externals --- */
extern c_Resource *GlobResources;

extern char	DstDir[1000];
extern char	*company;
extern char	*fname;
extern char	cdkey[];
extern c_MainInterface *MainInterface1;
extern c_DocIcon	*GlobDocIcon;

/*--- seed= CryptSeed for Key generation !! ---*/
int seed;

/*--- methods --- */
sf_Box_keygen fc_Box_keygen =
{
 cons_Box_keygen,
 dest_Box_keygen,
 copy_Box_keygen,
 BKGwaitForUnMap
};

/* ----------------------------------------------------------------- ** 
** consturcteur                                                      ** 
** ----------------------------------------------------------------- */
static void *cons_Box_keygen(w_Parent, pszTitle, verbose)
Widget		w_Parent;
char		*pszTitle;
boolean		verbose;
{
 c_Box_keygen	*This;
 boolean	key_validity;
 Display	*display;
 Dimension	h1, h2;
 Pixmap		pixmap;
 Arg		argsw[5];
 int		nargs;
 char		string[100];
 XmString	xms;
  
 if((This = (c_Box_keygen *)Xc_malloc("Box_keygen",
				      sizeof(c_Box_keygen))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 memset(This, 0, sizeof(c_Box_keygen));

 This->f = &fc_Box_keygen;
  
 This->verbose = verbose;
 if (This->verbose)
 {

  XtVaGetValues(w_Parent, XmNwidth, &h1, XmNheight, &h2, NULL); 
 
  This->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
						       pszTitle, h1, h2,
						       XcCD_HORIZONTAL);
  This->w_This = WIDGET(This->Dialog);
  display = XtDisplay(WIDGET(This->Dialog));

  This->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(This),
				     MANAGER(This->Dialog),
				     "gBoxKey", TRUE);
 
    
  This->Ltitre = (c_Label *)NEW(c_Label)(WIDGET(This->Grid),
					   MANAGER(This->Dialog),
					   "lBoxInstallTitle");
  
  This->FrameIcon1 = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					     MANAGER(This->Dialog), 
					     NULL, 0);

  This->Icon1 = (c_Icon *)NEW(c_Icon)(WIDGET(This->FrameIcon1), 
				      MANAGER(This->Dialog),
				      "iBoxKeyGen1", display, 
				      logo_flip1_xpm, NULL, XcLabel);

  This->FrameIcon2 = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					     MANAGER(This->Dialog), 
					     NULL, 0);

  This->Icon2 = (c_Icon *)NEW(c_Icon)(WIDGET(This->FrameIcon2), 
				      MANAGER(This->Dialog),
				      "iBoxKeyGen2", display, 
				      logo_flip2_xpm, NULL, XcLabel);

  This->FrameIcon3 = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					     MANAGER(This->Dialog), 
					     NULL, 0);

  This->Icon3 = (c_Icon *)NEW(c_Icon)(WIDGET(This->FrameIcon3), 
				      MANAGER(This->Dialog),
				      "iBoxKeyGen3", display, 
				      logo_flip3_xpm, NULL, XcLabel);

  This->FrameIcon4 = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					     MANAGER(This->Dialog), 
					     NULL, 0);

  This->Icon4 = (c_Icon *)NEW(c_Icon)(WIDGET(This->FrameIcon4), 
				      MANAGER(This->Dialog),
				      "iBoxKeyGen4", display, 
				      logo_flip4_xpm, NULL, XcLabel);


  This->FrameLabel1 = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					      MANAGER(This->Dialog), 
					      NULL, 0);
  This->Label1 = (c_Label *)NEW(c_Label)(WIDGET(This->FrameLabel1),
					 MANAGER(This->Dialog),
					 "lBKGLabel1");

  This->FrameLabel2 = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					      MANAGER(This->Dialog), 
					      NULL, 0);
  This->Label2 = (c_Label *)NEW(c_Label)(WIDGET(This->FrameLabel2),
					 MANAGER(This->Dialog),
					 "lBKGLabel2");

  This->FrameLabel3 = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					      MANAGER(This->Dialog), 
					      NULL, 0);
  This->Label3 = (c_Label *)NEW(c_Label)(WIDGET(This->FrameLabel3),
					 MANAGER(This->Dialog),
					 "lBKGLabel3");

  This->FrameLabel4 = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					      MANAGER(This->Dialog), 
					      NULL, 0);
  This->Label4 = (c_Label *)NEW(c_Label)(WIDGET(This->FrameLabel4),
					 MANAGER(This->Dialog),
					 "lBKGLabel4");



  nargs = 0;
  pixmap = F(GlobDocIcon).get_pixmap(GlobDocIcon, OrchidPattern);
  if (pixmap != (Pixmap)0)
  {
   XtSetArg(argsw[nargs], XmNbackgroundPixmap, pixmap);
   nargs++;
  }
  XtSetValues(WIDGET(This->Label1), argsw, nargs);
  XtSetValues(WIDGET(This->Label2), argsw, nargs);
  XtSetValues(WIDGET(This->Label3), argsw, nargs);
  XtSetValues(WIDGET(This->Label4), argsw, nargs);
 }

 /*___key generation !___*/
 key_validity = check_cdkey_stage4();
 Xc_TRACE(("Key validity test 4: %s", key_validity ? "passed" : "failed"));

 if (key_validity)
  key_validity = check_cdkey_stage5();
 Xc_TRACE(("Key validity test 5: %s", key_validity ? "passed" : "failed"));
 /*___key generation !___*/

 if (verbose)
 {
  This->FrameInfo = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					    MANAGER(This->Dialog), 
					    NULL, 0);
 }
 
 /*___key generation !___*/
 if (key_validity)
  key_validity = check_cdkey_stage6();
 Xc_TRACE(("Key validity test 6: %s", key_validity ? "passed" : "failed"));
 
 seed = end_cdkey_check();
 /*___key generation !___*/

 if (verbose)
 {
  This->LInfo = 
   (c_Label *)NEW(c_Label)(WIDGET(This->FrameInfo),
			   MANAGER(This->Dialog),
			   "lBKGInfo");
  
  F(This->Grid).addColumn(This->Grid, XcG_BEST_SIZE,
			  WIDGET(This->Ltitre), XcG_MAX_SIZE,
			  NULL);
/*F(This->Grid).addColumn(This->Grid, 30, NULL);*/
  F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE, NULL);
  F(This->Grid).addColumn(This->Grid, 150,
			  WIDGET(This->FrameIcon1), XcG_MAX_SIZE,
			  XcG_WIDGET_NULL, 10,
			  WIDGET(This->FrameIcon2), XcG_MAX_SIZE,
			  XcG_WIDGET_NULL, 10,
			  WIDGET(This->FrameIcon3), XcG_MAX_SIZE,
			  XcG_WIDGET_NULL, 10,
			  WIDGET(This->FrameIcon4), XcG_MAX_SIZE,
			  NULL);
  F(This->Grid).addColumn(This->Grid, 10, NULL);
  F(This->Grid).addColumn(This->Grid, 22,
			  WIDGET(This->FrameLabel1), XcG_MAX_SIZE,
			  XcG_WIDGET_NULL, 10,
			  WIDGET(This->FrameLabel2), XcG_MAX_SIZE,
			  XcG_WIDGET_NULL, 10,
			  WIDGET(This->FrameLabel3), XcG_MAX_SIZE,
			  XcG_WIDGET_NULL, 10,
			  WIDGET(This->FrameLabel4), XcG_MAX_SIZE,
			  NULL);
  F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE, NULL);

/*
  F(This->Grid).addColumn(This->Grid, 10, NULL);
  F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE,
			  WIDGET(This->FrameInfo), XcG_MAX_SIZE,
			  NULL);
*/ 
  
  
  xms = XmStringCreate(F(GlobResources).getString(GlobResources,
						  XcR_installComplete),
		       XmSTRING_DEFAULT_CHARSET);
  XtSetArg(argsw[0], XmNlabelString, xms);
  XtSetValues(WIDGET(This->Ltitre), argsw, 1);
  XmStringFree(xms);
  
 
  switch ((int)cdkey[2] - '0')
  {
  case 0:
   break;
  case 1:
   XtSetSensitive(WIDGET(This->Icon2), False);
   sprintf(string, "XQuad   [%s]",
	   F(GlobResources).getString(GlobResources, XcR_demo));
   xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(argsw[0], XmNlabelString, xms);
   XtSetValues(WIDGET(This->Label2), argsw, 1);
   XmStringFree(xms);
   
   XtSetSensitive(WIDGET(This->Icon3), False);
   sprintf(string, "XAllWrite   [%s]",
	   F(GlobResources).getString(GlobResources, XcR_demo));
   xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(argsw[0], XmNlabelString, xms);
   XtSetValues(WIDGET(This->Label3), argsw, 1);
   XmStringFree(xms);
   break;
  case 2:
   XtSetSensitive(WIDGET(This->Icon1), False);
   sprintf(string, "Xclamation   [%s]",
	   F(GlobResources).getString(GlobResources, XcR_demo));
   xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(argsw[0], XmNlabelString, xms);
   XtSetValues(WIDGET(This->Label1), argsw, 1);
   XmStringFree(xms);

   XtSetSensitive(WIDGET(This->Icon3), False);
   sprintf(string, "XAllWrite   [%s]",
	   F(GlobResources).getString(GlobResources, XcR_demo));
   xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(argsw[0], XmNlabelString, xms);
   XtSetValues(WIDGET(This->Label3), argsw, 1);
   XmStringFree(xms);
   break;
  case 3:
   XtSetSensitive(WIDGET(This->Icon1), False);
   sprintf(string, "Xclamation   [%s]",
	   F(GlobResources).getString(GlobResources, XcR_demo));
   xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(argsw[0], XmNlabelString, xms);
   XtSetValues(WIDGET(This->Label1), argsw, 1);
   XmStringFree(xms);

   XtSetSensitive(WIDGET(This->Icon2), False);
   sprintf(string, "XQuad   [%s]",
	   F(GlobResources).getString(GlobResources, XcR_demo));
   xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(argsw[0], XmNlabelString, xms);
   XtSetValues(WIDGET(This->Label2), argsw, 1);
   XmStringFree(xms);
   break;
  default:
   XtSetSensitive(WIDGET(This->Icon1), False);
   sprintf(string, "Xclamation   [%s]",
	   F(GlobResources).getString(GlobResources, XcR_demo));
   xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(argsw[0], XmNlabelString, xms);
   XtSetValues(WIDGET(This->Label1), argsw, 1);
   XmStringFree(xms);

   XtSetSensitive(WIDGET(This->Icon2), False);
   sprintf(string, "XQuad   [%s]",
	   F(GlobResources).getString(GlobResources, XcR_demo));
   xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(argsw[0], XmNlabelString, xms);
   XtSetValues(WIDGET(This->Label2), argsw, 1);
   XmStringFree(xms);

   XtSetSensitive(WIDGET(This->Icon3), False);
   sprintf(string, "XAllWrite   [%s]",
	   F(GlobResources).getString(GlobResources, XcR_demo));
   xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(argsw[0], XmNlabelString, xms);
   XtSetValues(WIDGET(This->Label3), argsw, 1);
   XmStringFree(xms);
   break;
  }

  xms = XmStringCreate(F(GlobResources).getString(GlobResources,
						  XcR_installComplete),
		       XmSTRING_DEFAULT_CHARSET);
  XtSetArg(argsw[0], XmNlabelString, xms);
  XtSetValues(WIDGET(This->Ltitre), argsw, 1);
  XmStringFree(xms);
  

  F(This->Dialog).createActionArea(This->Dialog, XcCD_QUIT,
				   XcCD_QUIT,
				   NULL, NULL);
 
 }/* end verbose */
 
 if (key_validity)
  key_validity = test_idfile(); 
 Xc_TRACE(("Test ID file: %s", key_validity ? "passed" : "failed"));
 
 if (key_validity)
  gen_end_key(cdkey, fname, company, MainInterface1->display_name, DstDir);
 
 Xc_TRACE(("Object Box_keygen build"));
 return This;
}

/* ----------------------------------------------------------------- ** 
** specific wait fo unmap                                            ** 
** ----------------------------------------------------------------- */
static int BKGwaitForUnMap(This)
c_Box_keygen *This;
{
 int	rc;
 
 if (This->verbose)
 {
  F(This->Dialog).map(This->Dialog, TRUE);
  rc = F(This->Dialog).waitForUnmap(This->Dialog);
 }
 else
 {
  rc = XcCD_NEXT;
 }

 return rc;
} 

/* ----------------------------------------------------------------- ** 
** destructeur                                                       ** 
** ----------------------------------------------------------------- */
static void dest_Box_keygen(This)
c_Box_keygen *This;
{
 if (This->verbose)
 {
  DELETE(c_Label)(This->Ltitre);
  DELETE(c_Label)(This->Label1);
  DELETE(c_Label)(This->Label2);
  DELETE(c_Label)(This->Label3);
  DELETE(c_Label)(This->Label4);
  DELETE(c_Icon)(This->Icon1);
  DELETE(c_Icon)(This->Icon2);
  DELETE(c_Icon)(This->Icon3);
  DELETE(c_Icon)(This->Icon4);
  DELETE(c_Frame)(This->FrameIcon1);
  DELETE(c_Frame)(This->FrameIcon2);
  DELETE(c_Frame)(This->FrameIcon3);
  DELETE(c_Frame)(This->FrameIcon4);
  DELETE(c_Frame)(This->FrameLabel1);
  DELETE(c_Frame)(This->FrameLabel2);
  DELETE(c_Frame)(This->FrameLabel3);
  DELETE(c_Frame)(This->FrameLabel4);

  DELETE(c_Label)(This->LInfo);
  DELETE(c_Frame)(This->FrameInfo);
  DELETE(c_Grid)(This->Grid);
  DELETE(c_CustomDialog)(This->Dialog);
 }
 Xc_free(This);
 Xc_TRACE(("Object Box_keygen destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copieur                                                           ** 
** ----------------------------------------------------------------- */
static void *copy_Box_keygen(This)
c_Box_keygen *This;
{
 Xc_TRACE(("Copy not impemented"));
 return NULL;
}

