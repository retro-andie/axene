/*
** Box_legal2.c for Xclamation, XQuad and XAllWrite in Box_legal2/
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
** Started on  Mon Nov  8 11:48:53 1998 Robin Castanier
** Last update Sun Jan  9 17:14:03 2000 Emmanuel Paris
*/

/*
#include <unistd.h>
*/
#include <netdb.h>

#include "Box_legal2.h"
#include "Resource.h"
#include "file.h"
#include "osname.h"

extern c_Resource *GlobResources;

void *cons_Box_legal2();
void dest_Box_legal2();
void callback_dest_BoxLegal2();

sf_Box_legal2 fc_Box_legal2 =
{
 cons_Box_legal2,
 dest_Box_legal2,
};

void *cons_Box_legal2(w_Parent, pszTitle)
Widget w_Parent;
char   *pszTitle;
{
 Display        *display;
 c_Box_legal2	 *ObjTmp;
 Arg		 argsw[4];
 int		 nargs;
 XmString       xmsBaratin;
 char		*registration_file;
 char		*full_registration_file;
  
 if ((ObjTmp = (c_Box_legal2 *)Xc_malloc("Box_legal2",
					sizeof(c_Box_legal2))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_Box_legal2;

 ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							pszTitle, 540, 580,
							XcCD_HORIZONTAL
							| XcCD_VRESIZE);
  
 ObjTmp->w_This = WIDGET(ObjTmp->Dialog);
 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				      MANAGER(ObjTmp->Dialog),
				      "gBoxLegal2", FALSE);
  
 display = XtDisplay(w_Parent);

 nargs = 0;

 ObjTmp->title = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog),
					  "lLegal2Title" );
  
 nargs = 0;
 XtSetArg(argsw[nargs], XmNshadowType, XmSHADOW_IN); nargs++;
 XtSetArg(argsw[nargs], XmNmarginWidth, 7); nargs++;
 XtSetArg(argsw[nargs], XmNmarginHeight, 5); nargs++;

 ObjTmp->w_Frame1 = XmCreateFrame(WIDGET(ObjTmp->Grid), "fRegister",
				  argsw, nargs);
 
 F(MANAGER(ObjTmp->Dialog)).Add_child(MANAGER(ObjTmp->Dialog),
				      ObjTmp->w_Frame1, WIDGET(ObjTmp->Grid));
 
 ObjTmp->SText = 
  (c_ScrolledWindow *)NEW(c_ScrolledWindow)(ObjTmp->w_Frame1,
					    MANAGER(ObjTmp->Dialog),
					    "stBoxLegal2");
 
 ObjTmp->LText = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->SText),
					 MANAGER(ObjTmp->Dialog),
					 "lBoxLegal2");
 
 {
  FILE *file;
  long fsize;
  char *text;
  
  registration_file = F(GlobResources).getString(GlobResources,
					    XcR_registrationFileName);

  full_registration_file = TranslateFilename("license",
					     registration_file);
  
  if ((file = fopen(full_registration_file, "rb")))
  {
   char osname[20], *ptr;
   int i, len;

   fseek(file, 0L, SEEK_END);
   fsize = ftell(file);
   fseek(file, 0L, SEEK_SET);
   
   text = (char *)Xc_malloc("tl", fsize + 1);
   if (!text || fread( text, fsize, 1, file) != 1)
   {
    Xc_ERROR(("fread fault"));
    return NULL;
   }
   text[fsize] = '\0';
   
   strncpy(osname, Xc_SYSTEM_NAME, 19);
   osname[19] = '\0';
   ptr = strchr(osname, ' ');
   if (ptr) *ptr='\0';
   len = strlen(osname);

   for(i = 0; i < fsize; i++)
   {
    if (text[i] == '%' && text[i+1] == 'm')
    {
     text = (char *)Xc_realloc("tlr", text, fsize + 1 + len - 2);
     fsize += len - 2;
     memmove(text + i + len, text+i+2, fsize - i - len - 2);
     memcpy(text + i, osname, len);
    }
   }

   xmsBaratin =  XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET);
   XtVaSetValues(WIDGET(ObjTmp->LText), XmNlabelString, xmsBaratin, NULL);
   XmStringFree(xmsBaratin);
   
   XtSetArg(argsw[0], XmNworkWindow, WIDGET(ObjTmp->LText));
   XtSetValues(WIDGET(ObjTmp->SText), argsw, 1);
   
   Xc_free(text);
   fclose(file);
  }
 }
 
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->title), 50,
			   ObjTmp->w_Frame1, XcG_MAX_SIZE,
			   XcG_WIDGET_NULL, 15,
			   NULL);


 F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, 
				    XcCD_OK,
				    XcCD_OK,
				    FALSE, NULL, NULL);
 
 F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);
  
 Xc_TRACE(("Object Box_legal2 build"));
 return ObjTmp;
}

void dest_Box_legal2(This)
c_Box_legal2 *This;
{
 DELETE(c_Label)(This->title);

 DELETE(c_ScrolledWindow)(This->LText);
 DELETE(c_Label)(This->SText);
  XtDestroyWidget(This->w_Frame1);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);

 Xc_free(This);
 Xc_TRACE(("Object Box_legal2 destroyed"));
}

void callback_dest_BoxLegal2(wid, This)
Widget	    wid;
c_Box_legal2 *This;
{
 DELETE(c_Box_legal2)(This);
}



