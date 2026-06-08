/*
** Frame.c for Xclamation, XQuad, XAllWrite, XMayday and XInstall in Frame/
** Frame Object
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
** Started on  Mon Jul 10 13:42:58 1995 Robin Castanier
** Last update Sat Aug 14 16:27:14 1999 Emmanuel Paris
*/

#include "Frame.h"
#include "RegisterHelp.h"
#include <Xm/FrameP.h>

void *cons_Frame();
void dest_Frame();
void *copy_Frame();
static void Frame_add_toggle();
static void Frame_toggle_add_callback();
static void Frame_toggle_set();
#ifdef LESSTIF_VERSION
static void cb_resize ___PROTO((CompositeWidget w, XConfigureEvent *event,
				char *args[], int *nargs));

#define FRAME_RESIZE "<Configure>: XcF_resize()"
#endif

sf_Frame fc_Frame =
{
 cons_Frame,
 dest_Frame,
 copy_Frame,
 Frame_add_toggle,
 Frame_toggle_add_callback,
 Frame_toggle_set
};

void *cons_Frame(w_Parent, ManageW, pszTitle)
Widget         w_Parent;
c_ManageWidget *ManageW;
char           *pszTitle;
{
 c_Frame	*This;
 char		*title;

 if ((This = (c_Frame *)Xc_malloc("Frame", sizeof (c_Frame))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f = &fc_Frame;
  
 if (pszTitle)
 {
  if (*pszTitle == '~')
   title = pszTitle + 1;
  else
   title = pszTitle;
 }
 else title = "frame";
 
 WIDGET(This) = XmCreateFrame(w_Parent, title, NULL, 0);
 Xc_REGISTER_HELP(This->w_This);

#ifndef ___alpha_osf_1_0
 if (title == pszTitle)
 {
  Xc_TRACE(("XmNchildType Detected."));
  This->w_Label = XmCreateLabelGadget(WIDGET(This), pszTitle, NULL, 0);
  XtVaSetValues(This->w_Label, XmNchildType, XmFRAME_TITLE_CHILD, NULL);
  Xc_REGISTER_HELP(This->w_Label);

  if(ManageW) 
   F(ManageW).Add_child(ManageW, This->w_Label, WIDGET(This));
  else
   XtManageChild(This->w_Label);
 }
 else
 {
  This->w_Label = NULL;  
 }

#endif
 This->TBtitle = NULL;


#ifdef LESSTIF_VERSION
 {
  XtActionsRec rec;
  
  rec.string = "XcF_resize";
  rec.proc = (void *)cb_resize;
  XtAppAddActions(XtWidgetToApplicationContext(WIDGET(This)), &rec, 1);
 
  XtOverrideTranslations(WIDGET(This), XtParseTranslationTable(FRAME_RESIZE));
 }
#endif
/*(((XmFrameWidget)WIDGET(This))->frame.processing_constraints) = False;*/
 
 if(ManageW) 
  F(ManageW).Add_child(ManageW, This->w_This, w_Parent);

 Xc_TRACE(("Object Frame build"));
 return This;
}

void dest_Frame(This)
c_Frame *This;
{
 Xc_HISTORY(("Deleting Object Frame"));

#ifndef ___alpha_osf_1_0
 if (This->w_Label)
  XtDestroyWidget(This->w_Label); 
#endif
 if (This->TBtitle != NULL)
  DELETE(c_ToggleButton)(This->TBtitle);
 
 XtDestroyWidget(This->w_This);
 Xc_TRACE(("Object Frame destroyed"));
 Xc_free(This);
}

void *copy_Frame(This)
c_Frame *This;
{
 c_Frame *Obj;

 if ((Obj = (c_Frame *)Xc_malloc("CFrame", sizeof(c_Frame))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }

 memcpy(Obj, This, sizeof(c_Frame));
 Xc_TRACE(("Object Frame copied"));
 return This;
}

static void Frame_add_toggle(This, ManageW, Title)
c_Frame		*This;
c_ManageWidget	*ManageW;
char		*Title;
{
 This->TBtitle = (c_ToggleButton *)NEW(c_ToggleButton)
  (WIDGET(This), ManageW, Title);
 XtVaSetValues(WIDGET(This->TBtitle), XmNchildType, XmFRAME_TITLE_CHILD, NULL);
}

static void Frame_toggle_add_callback(This, func, param)
c_Frame		*This;
void		(*func)();
void		*param;
{
 F(This->TBtitle).Add_Callback
  (This->TBtitle, XmNvalueChangedCallback, func, param);
}

static void Frame_toggle_set(This, value, call)
c_Frame		*This;
boolean		value;
boolean		call;
{
 F(This->TBtitle).Set
  (This->TBtitle, value, call);
}


#ifdef LESSTIF_VERSION
static void cb_resize(w, event, args, nargs)
CompositeWidget w;
XConfigureEvent *event;
char	*args[];
int	*nargs;
{
 if (XtIsRealized(w))
 {
  (((XmFrameWidget)w)->frame.processing_constraints) = False; 
 }

 xmFrameClassRec.core_class.resize((Widget)w);
}
#endif
