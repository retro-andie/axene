/*
** Box_about.c for Xclamation, XQuad, XAllWrite and XMayday in Box_about/
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
** Started on  Mon Jul 17 13:54:22 1995 Robin Castanier
** Last update Sun Jan 16 17:40:08 2000 Emmanuel Paris
*/

#include "Box_about.h"
#include "Actions.h"
#include "builtdate.h"
#include "../Pixmaps/logo_soft_big.zpm"
#include "../Pixmaps/logo_editor.zpm"

#ifndef Xc_XMAYDAY
#include "Readkey.h"
#endif

void *cons_Box_about();
void dest_Box_about();
void callback_dest_BoxAbout();
void blink_about();

sf_Box_about fc_Box_about =
{
 cons_Box_about,
 dest_Box_about,
};

static boolean decrypted = FALSE;
static unsigned char cryptString[] =
"Dl:^_a!\t3d\nffj\tsGg&\n_-M\t\n((;$(roMZkOk(%)";

#ifdef Xc_XCLAMATION
static unsigned char InterDeposit[] = 
{
 'I' - 'D', 'D' - 'l', 'D' - ':', 'N' - '^', 
 '.' - '_', 'F' - 'a',  'R' - '!', '.' - '\t', 
 '0' - '3', '0' - 'd', '1' - '\n', '.' - 'f', 
 '3' - 'f', '6' - 'j', '0' - '\t', '0' - 's', 
 '0' - 'G', '6' - 'g', '.' - '&', '0' - '\n', 
 '1' - '_', '.' - '-', 'R' - 'M', '.' - '\t', 
 'P' - '\n', '.' - '(', '1' - '(', '9' - ';', 
 '9' - '$', '6' - '(', '.' - 'r', '0' - 'o', 
 '0' - 'M', '0' - 'Z', '.' - 'k', '2' - 'O', 
 '0' - 'k', '8' - '(', '0' - '%', '0' - ')',
 '\0'
};
#endif
#ifdef Xc_XQUAD
static unsigned char InterDeposit[] = 
{
 'I' - 'D', 'D' - 'l', 'D' - ':', 'N' - '^', 
 '.' - '_', 'F' - 'a',  'R' - '!', '.' - '\t', 
 '0' - '3', '0' - 'd', '1' - '\n', '.' - 'f', 
 '3' - 'f', '6' - 'j', '0' - '\t', '0' - 's', 
 '0' - 'G', '7' - 'g', '.' - '&', '0' - '\n', 
 '1' - '_', '.' - '-', 'R' - 'M', '.' - '\t', 
 'P' - '\n', '.' - '(', '1' - '(', '9' - ';', 
 '9' - '$', '6' - '(', '.' - 'r', '0' - 'o', 
 '0' - 'M', '0' - 'Z', '.' - 'k', '2' - 'O', 
 '0' - 'k', '8' - '(', '0' - '%', '0' - ')',
 '\0'
};
#endif
#ifdef Xc_XALLWRITE
static unsigned char InterDeposit[] = 
{
 'I' - 'D', 'D' - 'l', 'D' - ':', 'N' - '^', 
 '.' - '_', 'F' - 'a',  'R' - '!', '.' - '\t', 
 '0' - '3', '0' - 'd', '1' - '\n', '.' - 'f', 
 '3' - 'f', '6' - 'j', '0' - '\t', '0' - 's', 
 '1' - 'G', '1' - 'g', '.' - '&', '0' - '\n', 
 '1' - '_', '.' - '-', 'R' - 'M', '.' - '\t', 
 'P' - '\n', '.' - '(', '1' - '(', '9' - ';', 
 '9' - '$', '7' - '(', '.' - 'r', '0' - 'o', 
 '0' - 'M', '0' - 'Z', '.' - 'k', '2' - 'O', 
 '0' - 'k', '8' - '(', '0' - '%', '0' - ')',
 '\0'
};
#endif
#ifdef Xc_XMAYDAY
static unsigned char InterDeposit[] = 
{
 'I' - 'D', 'D' - 'l', 'D' - ':', 'N' - '^', 
 '.' - '_', 'F' - 'a',  'R' - '!', '.' - '\t', 
 '0' - '3', '0' - 'd', '1' - '\n', '.' - 'f', 
 '3' - 'f', '6' - 'j', '0' - '\t', '0' - 's', 
 '1' - 'G', '2' - 'g', '.' - '&', '0' - '\n', 
 '0' - '_', '.' - '-', 'R' - 'M', '.' - '\t', 
 'C' - '\n', '.' - '(', '1' - '(', '9' - ';', 
 '9' - '$', '7' - '(', '.' - 'r', '0' - 'o', 
 '0' - 'M', '0' - 'Z', '.' - 'k', '1' - 'O', 
 '0' - 'k', '8' - '(', '0' - '%', '0' - ')',
 '\0'
};
#endif

void *cons_Box_about(w_Parent, pszTitle)
Widget w_Parent;
char   *pszTitle;
{
 Display        *display;
 c_Box_about	 *This;
 Arg		 argsw[4];
 int		 nargs;
 char           pszVlabel[200];
 XmString       xmsBaratin;
 int  i;

 if ((This = (c_Box_about *)Xc_malloc("Box_about",
				      sizeof(c_Box_about))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f = &fc_Box_about;
 This->app = XtWidgetToApplicationContext(w_Parent);
 
#if defined(Xc_XMAYDAY) || \
 ( defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0 )
 This->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
						      pszTitle,
						      520, 400,
						      XcCD_HORIZONTAL );
#else
 if (gkinfo.valid == Xc_KEY_GOOD)
  This->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
						       pszTitle,
						       520, 400,
						       XcCD_HORIZONTAL );
 else
  This->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
						       pszTitle,
						       520, 320,
						       XcCD_HORIZONTAL );
#endif
  
 This->w_This = WIDGET(This->Dialog);
 allow_ba_action(This->w_This);
 This->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(This),
				    MANAGER(This->Dialog),
				    "gBoxAbout", FALSE);
  
 display = XtDisplay(w_Parent);

 This->VersionLabel = (c_Label *)NEW(c_Label)(WIDGET(This->Grid),
					      MANAGER(This->Dialog), 
					      "lAboutRelease");

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

 sprintf(pszVlabel, "%s   (%s/%s)" , pszVlabel, 
	 MAKE_DATE_MONTH, MAKE_DATE_YEAR);

 xmsBaratin = XmStringCreate(pszVlabel, XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[nargs], XmNlabelString, xmsBaratin);
 nargs++;
 XtSetValues(WIDGET(This->VersionLabel), argsw, nargs);
 XmStringFree(xmsBaratin);
 

 This->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					MANAGER(This->Dialog), 
					NULL, 0);
 This->Grid2 = (c_Grid *)NEW(c_Grid)(WIDGET(This->Frame2),
				     MANAGER(This->Dialog),
				     "gBAFrame2", FALSE);

 /*--- Flipping icons ---*/

 This->FormIcon = (c_Form *)NEW(c_Form)(WIDGET(This->Grid2),
					MANAGER(This->Dialog),
					"foflpIcon");
 XtVaSetValues(WIDGET(This->FormIcon), 
	       XmNnoResize, True, XmNresizePolicy, XmRESIZE_NONE, 
	       XmNautoUnmanage, False, NULL);
 
 This->IconSoft = (c_Icon *)NEW(c_Icon)(WIDGET(This->FormIcon), 
					MANAGER(This->Dialog),
					"iBoxAboutSoft", display, 
					logo_editor_xpm, NULL, XcLabel);
 This->IconEditor = (c_Icon *)NEW(c_Icon)(WIDGET(This->FormIcon), 
					  NULL,
					  "iBoxAboutEditor", display, 
					  logo_soft_big_xpm, NULL,
					  XcLabel);
 This->id = XtAppAddTimeOut(This->app, 4000L, blink_about, This);
  
 XtSetArg(argsw[0], XmNrecomputeSize, False);
 XtSetValues(WIDGET(This->IconSoft), argsw, 1);
 XtSetValues(WIDGET(This->IconEditor), argsw, 1);
 F(This->FormIcon).attach_top(This->FormIcon, 
			      WIDGET(This->IconSoft), NULL);
 F(This->FormIcon).attach_left(This->FormIcon, 
			       WIDGET(This->IconSoft), NULL);
 F(This->FormIcon).attach_right(This->FormIcon, 
				WIDGET(This->IconSoft), NULL);
 F(This->FormIcon).attach_bottom(This->FormIcon, 
				 WIDGET(This->IconSoft), NULL);
 F(This->FormIcon).attach_top(This->FormIcon, 
			      WIDGET(This->IconEditor), NULL);
 F(This->FormIcon).attach_left(This->FormIcon, 
			       WIDGET(This->IconEditor), NULL);
 F(This->FormIcon).attach_right(This->FormIcon, 
				WIDGET(This->IconEditor), NULL);
 F(This->FormIcon).attach_bottom(This->FormIcon, 
				 WIDGET(This->IconEditor), NULL);
 
 This->Label1 = (c_Label *)NEW(c_Label)(WIDGET(This->Grid2),
					MANAGER(This->Dialog),
					"lAboutTitle" );
 xmsBaratin = F(GlobResources).getXmString(GlobResources,
					   XcR_aboutTitle);
 XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
 XtSetValues(WIDGET(This->Label1), argsw, 1);
 XmStringFree(xmsBaratin);
  
 This->Label2 = (c_Label *)NEW(c_Label)(WIDGET(This->Grid2),
					MANAGER(This->Dialog),
					"lAboutComments");
 xmsBaratin = F(GlobResources).getXmString(GlobResources,
					   XcR_aboutComments);
 XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
 XtSetValues(WIDGET(This->Label2), argsw, 1);
 XmStringFree(xmsBaratin);

 This->Label3 = (c_Label *)NEW(c_Label)(WIDGET(This->Grid2),
					MANAGER(This->Dialog),
					"lAboutEnd");
 sprintf(pszVlabel,
	 F(GlobResources).getString(GlobResources, XcR_aboutEnd),
	 Xc_COPYRIGHT_BEGIN, Xc_COPYRIGHT_END);
 xmsBaratin = XmStringCreate(pszVlabel, XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
 XtSetValues(WIDGET(This->Label3), argsw, 1);
 XmStringFree(xmsBaratin);
  
 This->Label4 = (c_Label *)NEW(c_Label)(WIDGET(This->Grid2),
					MANAGER(This->Dialog),
					"lAboutIDDN");
  
#if defined(Xc_XMAYDAY) || \
 ( defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0 )
 This->LicenseLabel = (c_Label *)NEW(c_Label)(WIDGET(This->Grid),
					      MANAGER(This->Dialog), 
					      "lAboutLicense");
#else
 if (gkinfo.valid == Xc_KEY_GOOD)
 {
  This->LicenseLabel = (c_Label *)NEW(c_Label)(WIDGET(This->Grid),
					       MANAGER(This->Dialog), 
					       "lAboutLicense");
 }
 else
  This->LicenseLabel = NULL;
#endif
  
 if (!decrypted)
 {
  for(i = 0; i < (int)strlen(InterDeposit) ; i++)
  {
   InterDeposit[i] += cryptString[i];
  }
  decrypted = TRUE;
 }
 xmsBaratin = XmStringCreateLtoR(InterDeposit, XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
 XtSetValues(WIDGET(This->Label4), argsw, 1);
 XmStringFree(xmsBaratin);
  
#ifndef Xc_XMAYDAY
 if (gkinfo.valid == Xc_KEY_GOOD)
 {

  if (!gkinfo.type)
   sprintf(pszVlabel,
	   F(GlobResources).getString(GlobResources, XcR_licensePersonal),
	   gkinfo.name);
  else
      sprintf(pszVlabel,
	   F(GlobResources).getString(GlobResources, XcR_licenseCommercial),
	   gkinfo.name, gkinfo.society);

  xmsBaratin = XmStringCreateLtoR(pszVlabel, XmSTRING_DEFAULT_CHARSET);
  
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(WIDGET(This->LicenseLabel), argsw, 1);
  XmStringFree(xmsBaratin);
 }
#if defined(Xc_REGISTER_LEVEL) && Xc_REGISTER_LEVEL > 0
 else
 {
  xmsBaratin = F(GlobResources).getXmString(GlobResources,
					    XcR_aboutNoLicense);
  XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
  XtSetValues(WIDGET(This->LicenseLabel), argsw, 1);
  XmStringFree(xmsBaratin);
 } 
#endif

#endif
  
  
 F(This->Grid2).addColumn(This->Grid2, 150,
			  WIDGET(This->FormIcon),  XcG_MAX_SIZE,
			  NULL);

 F(This->Grid2).addColumn(This->Grid2, XcG_MAX_SIZE,
			  WIDGET(This->Label1), XcG_BEST_SIZE,
			  WIDGET(This->Label2), XcG_MAX_SIZE,
			  WIDGET(This->Label3), XcG_BEST_SIZE,
			  WIDGET(This->Label4), XcG_BEST_SIZE,
			  NULL);
  
  
#if defined(Xc_XMAYDAY) || \
 ( defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0 )
 F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE,
			 WIDGET(This->VersionLabel), 40,
			 WIDGET(This->Frame2), XcG_MAX_SIZE,
			 WIDGET(This->LicenseLabel), 70,
			 NULL);
#else
 if (gkinfo.valid == Xc_KEY_GOOD)
  F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE,
			  WIDGET(This->VersionLabel), 40,
			  WIDGET(This->Frame2), XcG_MAX_SIZE,
			  WIDGET(This->LicenseLabel), 70,
			  NULL);
 else
  F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE,
			  WIDGET(This->VersionLabel), 40,
			  WIDGET(This->Frame2), XcG_MAX_SIZE,
			  NULL);  
#endif

 F(This->Dialog).createActionArea(This->Dialog, 
				  XcCD_OK,
				  XcCD_OK,
				  FALSE, NULL, NULL);
    
 F(This->Dialog).map(This->Dialog, TRUE);
  
#ifdef Xc_XMAYDAY
 This->bPixmap = TRUE;
 XtUnmanageChild(WIDGET(This->IconSoft));
 XtManageChild(WIDGET(This->IconEditor));
#endif

 Xc_TRACE(("Object Box_about build"));
 return This;
}

void dest_Box_about(This)
c_Box_about *This;
{
 XtRemoveTimeOut(This->id);
 if (This->LicenseLabel) DELETE(c_Label)(This->LicenseLabel);
 DELETE(c_Label)(This->VersionLabel);
 DELETE(c_Label)(This->Label1);
 DELETE(c_Label)(This->Label2);
 DELETE(c_Label)(This->Label3);
 DELETE(c_Label)(This->Label4);
 DELETE(c_Icon)(This->IconEditor);
 DELETE(c_Icon)(This->IconSoft);
 DELETE(c_Form)(This->FormIcon);
 DELETE(c_Grid)(This->Grid2);
 DELETE(c_Frame)(This->Frame2);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);

 Xc_free(This);
 Xc_TRACE(("Object Box_about destroyed"));
}

void callback_dest_BoxAbout(wid, This)
Widget	    wid;
c_Box_about *This;
{
 DELETE(c_Box_about)(This);
}

void blink_about(This)
c_Box_about *This;
{
 This->id = XtAppAddTimeOut(This->app, 4000L, blink_about, This);
  
 if ((This->bPixmap = !(This->bPixmap)))
 {
  XtUnmanageChild(WIDGET(This->IconSoft));
  XtManageChild(WIDGET(This->IconEditor));
 }
 else
 {
  XtManageChild(WIDGET(This->IconSoft));
  XtUnmanageChild(WIDGET(This->IconEditor));
 }
}














