/*
** Form.c for Xclamation, XQuad, XAllWrite, XMayday and XInstall in Form/
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
** Started on  Mon Jul 17 15:24:07 1995 Robin Castanier
** Last update Mon Jan 24 22:49:59 2000 Emmanuel Paris
*/

#include "Form.h"
#include "RegisterHelp.h"

void *cons_Form();
void dest_Form();
void *copy_Form();
void attach_top_Form();
void attach_bottom_Form();
void attach_left_Form();
void attach_right_Form();
void align_top_Form();
void align_bottom_Form();
void align_left_Form();
void align_right_Form();

sf_Form fc_Form =
{
 cons_Form,
 dest_Form,
 copy_Form,
 attach_top_Form,
 attach_bottom_Form,
 attach_left_Form,
 attach_right_Form,
 align_top_Form,
 align_bottom_Form,
 align_left_Form,
 align_right_Form
};

void *cons_Form(w_Parent, ManageW, Title )
Widget		w_Parent;
c_ManageWidget	*ManageW;
char		*Title;
{
 c_Form	*ObjTmp;
 Arg		argsw[4];
 int		nargs;

 if ((ObjTmp = (c_Form *)Xc_malloc("Form",
				   sizeof(c_Form))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_Form;

 nargs = 0;
 XtSetArg(argsw[nargs], XmNresizable, False);
 nargs++;
 ObjTmp->w_This = XmCreateForm(w_Parent, Title, argsw, nargs);
 Xc_REGISTER_HELP(ObjTmp->w_This);
  
 if (ManageW)
 {
  F(ManageW).Add_child(ManageW, ObjTmp->w_This, w_Parent);
 }
 Xc_TRACE(("Object Form build"));
 return ObjTmp;
}

void dest_Form(This)
c_Form *This;
{
 XtDestroyWidget(This->w_This);
 Xc_free(This);
 Xc_TRACE(("Object Form destroyed"));
}

void *copy_Form(This)
c_Form *This;
{
 c_Form *ObjTmp;

 if ((ObjTmp = (c_Form *)Xc_malloc("CForm",
				   sizeof (c_Form))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof(c_Form));
 Xc_TRACE(("Object Form copied"));
 return ObjTmp;
}

void attach_top_Form(w_This, son, otherson)
Widget		w_This;
Widget		son;
Widget		otherson;
{  
 Arg			argsw[4];
 int			nargs;

 nargs = 0;
 if (otherson == NULL)
 {
  if (son == NULL)
  {
   XtSetArg(argsw[nargs], XmNtopAttachment, XmATTACH_NONE);
   son = w_This;
  }
  else
   XtSetArg(argsw[nargs], XmNtopAttachment, XmATTACH_FORM);
 }
 else
 {
  XtSetArg(argsw[nargs], XmNtopAttachment, XmATTACH_WIDGET);
  nargs++;
  XtSetArg(argsw[nargs], XmNtopWidget, otherson);
 }
 nargs++;
 XtSetValues(son, argsw, nargs);
}

void attach_bottom_Form(w_This, son, otherson)
Widget		w_This;
Widget		son;
Widget		otherson;
{
 Arg			argsw[4];
 int			nargs;

 nargs = 0;
 if (otherson == NULL)
 {
  if (son == NULL)
  {
   XtSetArg(argsw[nargs], XmNbottomAttachment, XmATTACH_NONE);
   son = w_This;
  }
  else
   XtSetArg(argsw[nargs], XmNbottomAttachment, XmATTACH_FORM);
 }    
 else
 {
  XtSetArg(argsw[nargs], XmNbottomAttachment, XmATTACH_WIDGET);
  nargs++;
  XtSetArg(argsw[nargs], XmNbottomWidget, otherson);
 }
 nargs++;
 XtSetValues(son, argsw, nargs);
}

void attach_left_Form(w_This, son, otherson)
Widget		w_This;
Widget		son;
Widget		otherson;
{
 Arg			argsw[4];
 int			nargs;

 nargs = 0;
 if (otherson == NULL)
 {
  if (son == NULL)
  {
   XtSetArg(argsw[nargs], XmNleftAttachment, XmATTACH_NONE);
   son = w_This;
  }
  else
   XtSetArg(argsw[nargs], XmNleftAttachment, XmATTACH_FORM);
 }
 else
 {
  XtSetArg(argsw[nargs], XmNleftAttachment, XmATTACH_WIDGET);
  nargs++;
  XtSetArg(argsw[nargs], XmNleftWidget, otherson);
 }
 nargs++;
 XtSetValues(son, argsw, nargs);
}

void attach_right_Form(w_This, son, otherson)
Widget		w_This;
Widget		son;
Widget		otherson;
{
 Arg			argsw[4];
 int			nargs;

 nargs = 0;
 if (otherson == NULL)
 {
  if (son == NULL)
  {
   XtSetArg(argsw[nargs], XmNrightAttachment, XmATTACH_NONE);
   son = w_This;
  }
  else
   XtSetArg(argsw[nargs], XmNrightAttachment, XmATTACH_FORM);
 }
 else
 {
  XtSetArg(argsw[nargs], XmNrightAttachment, XmATTACH_WIDGET);
  nargs++;
  XtSetArg(argsw[nargs], XmNrightWidget, otherson);
 }
 nargs++;
 XtSetValues(son, argsw, nargs);
}


void align_top_Form(w_This, son, otherson)
Widget		w_This;
Widget		son;
Widget		otherson;
{  
 Arg			argsw[4];
 int			nargs;

 nargs = 0;
 if (otherson == NULL)
 {
  if (son == NULL)
  {
   XtSetArg(argsw[nargs], XmNtopAttachment, XmATTACH_NONE);
   son = w_This;
  }
  else
   XtSetArg(argsw[nargs], XmNtopAttachment, XmATTACH_FORM);
 }
 else
 {
  XtSetArg(argsw[nargs], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);
  nargs++;
  XtSetArg(argsw[nargs], XmNtopWidget, otherson);
 }
 nargs++;
 XtSetValues(son, argsw, nargs);
}



void align_bottom_Form(w_This, son, otherson)
Widget		w_This;
Widget		son;
Widget		otherson;
{
 Arg			argsw[4];
 int			nargs;

 nargs = 0;
 if (otherson == NULL)
 {
  if (son == NULL)
  {
   XtSetArg(argsw[nargs], XmNbottomAttachment, XmATTACH_NONE);
   son = w_This;
  }
  else
   XtSetArg(argsw[nargs], XmNbottomAttachment, XmATTACH_FORM);
 }
 else
 {
  XtSetArg(argsw[nargs], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);
  nargs++;
  XtSetArg(argsw[nargs], XmNbottomWidget, otherson);
 }
 nargs++;
 XtSetValues(son, argsw, nargs);
}

void align_left_Form(w_This, son, otherson)
Widget		w_This;
Widget		son;
Widget		otherson;
{
 Arg			argsw[4];
 int			nargs;

 nargs = 0;
 if (otherson == NULL)
 {
  if (son == NULL)
  {
   XtSetArg(argsw[nargs], XmNleftAttachment, XmATTACH_NONE);
   son = w_This;
  }
  else
   XtSetArg(argsw[nargs], XmNleftAttachment, XmATTACH_FORM);
 }
 else
 {
  XtSetArg(argsw[nargs], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);
  nargs++;
  XtSetArg(argsw[nargs], XmNleftWidget, otherson);
 }
 nargs++;
 XtSetValues(son, argsw, nargs);
}

void align_right_Form(w_This, son, otherson)
Widget		w_This;
Widget		son;
Widget		otherson;
{
 Arg			argsw[4];
 int			nargs;

 nargs = 0;
 if (otherson == NULL)
 {
  if (son == NULL)
  {
   XtSetArg(argsw[nargs], XmNrightAttachment, XmATTACH_NONE);
   son = w_This;
  }
  else
   XtSetArg(argsw[nargs], XmNrightAttachment, XmATTACH_FORM);
 }
 else
 {
  XtSetArg(argsw[nargs], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET);
  nargs++;
  XtSetArg(argsw[nargs], XmNrightWidget, otherson);
 }
 nargs++;
 XtSetValues(son, argsw, nargs);
}

