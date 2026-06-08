/*
** Box_license.c for XInstall in Box_license/
** Display the license file.
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
** Started on  Mon Oct 20 19:35:36 1997 Emmanuel Paris
** Last update Sat Jan 15 18:31:53 2000 Emmanuel Paris
*/

/*--- includes --- */
#include "Box_license.h"
#include "Resource.h"
#include "file.h"
#include "DocIcon.h"
#include "Cursor.h"


/*--- functions declaration --- */
static void *cons_Box_license();
static int BLwaitForUnMap();
static void dest_Box_license();
static void *copy_Box_license();

/*--- externals --- */
extern char		*strlanguage[];
extern char		*Globprog_name;
extern c_Resource	*GlobResources;
extern c_DocIcon	*GlobDocIcon;
extern c_GCursor	*GlobCursor;


/*--- methods --- */
sf_Box_license fc_Box_license =
{
 cons_Box_license,
 dest_Box_license,
 copy_Box_license,
 BLwaitForUnMap
};

/* ----------------------------------------------------------------- ** 
** consturcteur                                                      ** 
** ----------------------------------------------------------------- */
static void *cons_Box_license(w_Parent, pszTitle, tongue)
Widget		w_Parent;
char		*pszTitle;
int		tongue;
{
 c_Box_license	*This;
 Display	*display;
 Arg	        args[1];
 Dimension	h1, h2;
 char		*license_file;
 XmString	xms;
 Pixmap		pixmap;

 {
  char *directory, *tmp;
  boolean endofsearch = TRUE;
  int nc;
 
  tmp = strrchr(Globprog_name, '/');
  if (tmp == NULL) nc = strlen(Globprog_name);
  else nc = tmp - Globprog_name + 1;

  directory = (char *)Xc_malloc("dir", nc + 1);
  strncpy(directory, Globprog_name, nc);
  directory[nc] = '\0';

  do
  {
   license_file = (char *)Xc_malloc
    ("lf", nc + 9 + strlen(strlanguage[tongue-1]));
   sprintf(license_file, "%sLICENSE.%s", directory, strlanguage[tongue-1]);
   
   if (!IsFileExist(license_file))
   {
    sprintf(license_file, "%sLICENSE", directory);
    if (!IsFileExist(license_file))
    {
     Xc_free(license_file);
     license_file = NULL;
    }
   }
   
   if (!license_file && endofsearch && nc)
   {
    directory[nc - 1] = '\0';
    tmp = strrchr(directory, '/');
    if (tmp)
    {
     *(tmp + 1) = '\0';
     endofsearch = FALSE;
    }
   }
   else
    endofsearch = TRUE;
  } while(!endofsearch);
  Xc_free(directory);
 }

 if (license_file == NULL) return NULL;
 
 if((This = (c_Box_license *)Xc_malloc("Box_license",
				       sizeof(c_Box_license))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 memset(This, 0, sizeof(c_Box_license));

 This->f = &fc_Box_license;
  
 XtVaGetValues(w_Parent, XmNwidth, &h1, XmNheight, &h2, NULL); 
 
 SET_WAIT_MODE_STATIC;

 This->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							pszTitle, h1, h2,
							XcCD_HORIZONTAL);
 This->w_This = WIDGET(This->Dialog);
 display = XtDisplay(WIDGET(This->Dialog));

 This->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(This),
				      MANAGER(This->Dialog),
				      "gBoxLicense", TRUE);

 This->Ltitre = (c_Label *)NEW(c_Label)(WIDGET(This->Grid),
					MANAGER(This->Dialog),
					"lBoxLicenseTitle");

 This->FText = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
				       MANAGER(This->Dialog),
				       NULL, 0);
 
 This->SText = (c_ScrolledWindow *)NEW(c_ScrolledWindow)(WIDGET(This->FText),
						     MANAGER(This->Dialog),
						     "stBoxLicense");
 
 This->LText = (c_Label *)NEW(c_Label)(WIDGET(This->SText),
				       MANAGER(This->Dialog),
				       "lBoxLicense");
 
 {
  FILE *file;
  long fsize;
  char *text;

  if (!(file = fopen(license_file, "rb")))
  {
   DELETE(c_Box_license)(This);
   return NULL;
  }
  fseek(file, 0L, SEEK_END);
  fsize = ftell(file);
  fseek(file, 0L, SEEK_SET);
  
  text = (char *)Xc_malloc("tl", fsize + 1);
  if (!text || fread( text, fsize, 1, file) != 1)
  {
   DELETE(c_Box_license)(This);
   return NULL;
  }
  text[fsize] = '\0';

  xms =  XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET);
  XtVaSetValues(WIDGET(This->LText), XmNlabelString, xms, NULL);
  XmStringFree(xms);

 XtSetArg(args[0], XmNworkWindow, WIDGET(This->LText));
 XtSetValues(WIDGET(This->SText), args, 1);

/*  XmTextInsert(WIDGET(This->SText), 0, text);
  XmTextSetInsertionPosition(WIDGET(This->SText), 0); */

  Xc_free(text);
  fclose(file);
 }
 
 /*--- Grid add columns ---*/
 F(This->Grid).addColumn(This->Grid, XcG_BEST_SIZE,
			 WIDGET(This->Ltitre), XcG_MAX_SIZE,
			 NULL);
 F(This->Grid).addColumn(This->Grid, 5,
			 NULL);
 F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE,
			 WIDGET(This->FText), XcG_MAX_SIZE,
			 NULL);
 
 F(This->Dialog).createActionArea(This->Dialog, 
				  XcCD_DONTAGREE | XcCD_BACK | XcCD_AGREE,
				  XcCD_AGREE,
				  NULL, NULL);

 F(This->Dialog).map(This->Dialog, TRUE);

 pixmap = F(GlobDocIcon).get_pixmap(GlobDocIcon, OrchidPattern);
 if (pixmap != (Pixmap)0)
 {
  XtSetArg(args[0], XmNbackgroundPixmap, pixmap);
  XtSetValues(WIDGET(This->LText), args, 1);
 }
 
 {
  Widget widget;

  XtVaGetValues(WIDGET(This->SText), XmNverticalScrollBar, &widget, NULL);
  XtVaSetValues(widget, XmNincrement, 26, NULL);
 }

 UNSET_WAIT_MODE;

 Xc_free(license_file);

 Xc_TRACE(("Object Box_license build"));
 return This;
}

/* ----------------------------------------------------------------- ** 
** specific wait fo unmap                                            ** 
** ----------------------------------------------------------------- */
static int BLwaitForUnMap(This)
c_Box_license *This;
{
 int	rc;

 F(This->Dialog).map(This->Dialog, TRUE);

 rc = F(This->Dialog).waitForUnmap(This->Dialog);

 return rc;
} 

/* ----------------------------------------------------------------- ** 
** destructeur                                                       ** 
** ----------------------------------------------------------------- */
static void dest_Box_license(This)
c_Box_license *This;
{
 DELETE(c_Label)(This->LText);
 DELETE(c_ScrolledWindow)(This->SText);
 DELETE(c_Frame)(This->FText);
 DELETE(c_Label)(This->Ltitre);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);
 
 Xc_free(This);
 Xc_TRACE(("Object Box_license destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copieur                                                           ** 
** ----------------------------------------------------------------- */
static void *copy_Box_license(This)
c_Box_license *This;
{
 Xc_TRACE(("Copy not impemented"));
 return NULL;
}
