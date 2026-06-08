/*
** RadioImage.c for XQuad in RadioImage/
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
** Started on  Mon Jul 17 13:54:22 1995 Robin Castanier
** Last update Mon Dec  1 16:56:39 1997 Emmanuel Paris
*/

#include "RadioImage.h"

void *cons_RadioImage();
void dest_RadioImage();
void *copy_RadioImage();
void SelectIcon();

sf_RadioImage fc_RadioImage =
{
 cons_RadioImage,
 dest_RadioImage,
 copy_RadioImage
};

void *cons_RadioImage(w_Parent, ManageW, the_pointer, va_alist)
Widget 		w_Parent;
c_ManageWidget	*ManageW;
int		*the_pointer;
va_dcl
{
 va_list	other_arg;
 Display	*display;
 c_RadioImage	*ObjTmp;
 int		nargs;
 Arg		args[10];
 char		*titletmp;
 char		**xpmtmp;
 int		arg_count;
  
 if ((ObjTmp = (c_RadioImage *)Xc_malloc("RadioImage",
					 sizeof(c_RadioImage))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }

 display = XtDisplay(w_Parent);
 ObjTmp->f = &fc_RadioImage;
 ObjTmp->iIconManaged = *the_pointer;
 ObjTmp->p_change = the_pointer;

 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(w_Parent,
				      ManageW,
				      "RadioImage", FALSE);
 ObjTmp->w_This = WIDGET(ObjTmp->Grid);

 nargs = 0;
 XtSetArg(args[nargs], XmNnumColumns, 1); nargs++;
 XtSetArg(args[nargs], XmNorientation, XmHORIZONTAL); nargs++;
 XtSetArg(args[nargs], XmNresizeHeight, False); nargs++;
 XtSetArg(args[nargs], XmNpacking, XmPACK_COLUMN); nargs++;
 ObjTmp->w_Row = XmCreateRowColumn(WIDGET(ObjTmp->Grid),
				   "RCRadioImage", args, nargs);
 F(ManageW).Add_child(ManageW, ObjTmp->w_Row, WIDGET(ObjTmp->Grid));

 va_start(other_arg);
 arg_count = 0;
 while ((titletmp = va_arg(other_arg, char *)))
 {
  xpmtmp = va_arg(other_arg, char**);
  arg_count++;
 }
 va_end(other_arg);

 ObjTmp->iNbItems = arg_count;

 if ((ObjTmp->Icons = (c_Icon **)Xc_malloc("IconTab", sizeof(c_Icon *)
					   * ObjTmp->iNbItems))  == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }

 if ((ObjTmp->Toggles =
      (c_ToggleButton **)Xc_malloc("ToggleTab", sizeof(c_ToggleButton *)
				   * ObjTmp->iNbItems)) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 
 if ((ObjTmp->pstrToCallback =
      (ToCallback *)Xc_malloc("TabToCallback", sizeof(ToCallback)
			      * ObjTmp->iNbItems)) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }

 
 va_start(other_arg);
 arg_count = 0;
 while ((titletmp = va_arg(other_arg, char *)))
 {
  ObjTmp->Toggles[arg_count] = 
   (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid),
					 ManageW, titletmp);
  XtVaSetValues(WIDGET(ObjTmp->Toggles[arg_count]),
		XmNshadowThickness, 0,
		XmNalignment, XmALIGNMENT_BEGINNING,
		NULL);
  ObjTmp->pstrToCallback[arg_count].RadioImage = ObjTmp;
  ObjTmp->pstrToCallback[arg_count].iIconToManage = arg_count;
  F(ObjTmp->Toggles[arg_count])
   .Add_Callback(ObjTmp->Toggles[arg_count],
		 XmNvalueChangedCallback,
		 SelectIcon,
		 &(ObjTmp->pstrToCallback[arg_count]));
  
  xpmtmp = va_arg(other_arg, char**);
  ObjTmp->Icons[arg_count] = (c_Icon *)NEW(c_Icon)(ObjTmp->w_Row, NULL,
						   "RIIcon",
						   display, xpmtmp,
						   NULL, XcLabel);
  arg_count++;
 }
 va_end(other_arg);

 XtManageChild(WIDGET(ObjTmp->Icons[*the_pointer]));
 F(ObjTmp->Toggles[*the_pointer]).Set(ObjTmp->Toggles[*the_pointer],
				      True, False);
 
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 60,
			   ObjTmp->w_Row, XcG_MAX_SIZE,
			   NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Toggles[0]), XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Toggles[1]), XcG_MAX_SIZE,
			   NULL);

 Xc_TRACE(("Object RadioImage build"));
 return ObjTmp;
}

void dest_RadioImage(This)
c_RadioImage *This;
{
 int    iI;

 for (iI = 0 ; iI < This->iNbItems ; iI++)
 {
  DELETE(c_Icon)(This->Icons[iI]);
  DELETE(c_ToggleButton)(This->Toggles[iI]);
 }
 XtDestroyWidget(This->w_Row);
 DELETE(c_Grid)(This->Grid);

 Xc_free(This->Icons);
 Xc_free(This->Toggles);
 Xc_free(This->pstrToCallback);
 Xc_free(This);
 Xc_TRACE(("Object RadioImage destroyed"));
}

void *copy_RadioImage(This)
c_RadioImage *This;
{
 c_RadioImage *ObjTmp;
  
 if ((ObjTmp = (c_RadioImage *)Xc_malloc("CRadioImage", 
					 sizeof(c_RadioImage))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
  
 memcpy(ObjTmp, This, sizeof(c_RadioImage));
 Xc_TRACE(("Object RadioImage copied"));
 return ObjTmp;
}

void SelectIcon(This, Icone, appel)
Widget	    This;
ToCallback  *Icone;
caddr_t     appel;
{
 c_RadioImage *RadioImage;

 RadioImage = Icone->RadioImage;
 Xc_TRACE(("%d",Icone->iIconToManage));
 F(RadioImage->Toggles[RadioImage->iIconManaged])
  .Set(RadioImage->Toggles[RadioImage->iIconManaged], False, False);
 F(RadioImage->Toggles[Icone->iIconToManage])
  .Set(RadioImage->Toggles[Icone->iIconToManage], True, False);
 
 *(RadioImage->p_change) = Icone->iIconToManage;
 if (RadioImage->iIconManaged != Icone->iIconToManage)
 {
  XtUnmanageChild(WIDGET(RadioImage->Icons[RadioImage->iIconManaged]));
  RadioImage->iIconManaged = Icone->iIconToManage;
  XtManageChild(WIDGET(RadioImage->Icons[Icone->iIconToManage]));
 }
}
