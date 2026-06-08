/*
** Actions.c for XAllWrite in MainInterface/
** intrinsics actions added
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
** Started on  Tue Sep  5 11:12:17 1995 Emmanuel Paris
** Last update Tue Feb 17 17:29:14 1998 Emmanuel Paris
*/

#define NTRACE

#include "Actions.h"
#include "AlertBox.h"
#include <Xm/ScrolledW.h>
#include <Xm/ScrollBar.h>

static void cb_insert_string ___PROTO((Widget w, XEvent *event, 
				       char *args[], int *nargs));
static void cb_insert_hexchar ___PROTO((Widget w, XEvent *event, 
				       char *args[], int *nargs));
static void cb_default_pushb ___PROTO((Widget w, XEvent *event, 
				       char *args[], int *nargs));
static void cb_dispatch_event ___PROTO((Widget w, XEvent *event, 
				       char *args[], int *nargs));
static void cb_show_ba ___PROTO((Widget w, XEvent *event, 
				 char *args[], int *nargs));

XtActionsRec actions[NB_ACTIONS] = 
{{"xc_insert_string", (void *)cb_insert_string },
 {"xc_insert_hexchar", (void *)cb_insert_hexchar },
 {"xc_default_pushb", (void *)cb_default_pushb },
 {"xc_dispatch_event", (void (*)())cb_dispatch_event },
 {"xc_show_ba", (void *)cb_show_ba }
};

/* ----------------------------------------------------------------- ** 
** cb_insert_string: called by a translation                         ** 
** ----------------------------------------------------------------- */

static void cb_insert_string(w, event, args, nargs)
Widget	w;
XEvent	*event;
char	*args[];
int	*nargs;
{
 XmTextPosition	tpos, left, right;
 char	*dups;
 Arg			argsw[1];
 Boolean		editable;

 if (*nargs != 1)
 {
  Xc_TRACE(("unexpected parameter for translation"));
  return;
 }
 XtSetArg(argsw[0], XmNeditable, &editable);
 XtGetValues(w, argsw, 1);
 if (editable == False)
 {
  Xc_TRACE(("widget not editable."));
  return;
 }
  
 dups = Xc_strdup("ins_str", args[0]);
 if (XmTextGetSelectionPosition(w, &left, &right) == True)
 {
  XmTextRemove(w);
  tpos = XmTextGetInsertionPosition(w);
  XmTextInsert(w, tpos, dups); 
 }
 else
 {
  tpos = XmTextGetInsertionPosition(w);
  XmTextInsert(w, tpos, dups);
  XmTextSetInsertionPosition(w, tpos + strlen(dups));
 }
 Xc_free(dups);
 Xc_TRACE(("insert callback string: %s", args[0]));
}

/* ----------------------------------------------------------------- ** 
** cb_insert_hexchar: called by a translation                        ** 
** ----------------------------------------------------------------- */

static void cb_insert_hexchar(w, event, args, nargs)
Widget	w;
XEvent	*event;
char	*args[];
int	*nargs;
{
 XmTextPosition	tpos, left, right;
 char			c[2];
 char			*error_c;
 Arg			argsw[1];
 Boolean		editable;

 if (*nargs != 1)
 {
  Xc_TRACE(("unexpected parameter for translation"));
  return;
 }
  
 XtSetArg(argsw[0], XmNeditable, &editable);
 XtGetValues(w, argsw, 1);
 if (editable == False)
 {
  Xc_TRACE(("widget not editable."));
  return;
 }
 c[0] = (char)strtol(args[0], &error_c, 16);  
 if (c[0] == 0 || error_c == args[0])
 {
  Xc_TRACE(("unexpected parameter for translation"));
  return;
 }
 c[1] = '\0';
 if (XmTextGetSelectionPosition(w, &left, &right) == True)
 {
  XmTextRemove(w);
  tpos = XmTextGetInsertionPosition(w);
  XmTextInsert(w, tpos, c); 
 }
 else
 {
  tpos = XmTextGetInsertionPosition(w);
  XmTextInsert(w, tpos, c); 
  XmTextSetInsertionPosition(w, tpos + 1);
 }
 Xc_TRACE(("insert callback hexchar: %s", c));
}

/* ----------------------------------------------------------------- ** 
** cb_default_pushb: default push button callback                    ** 
** ----------------------------------------------------------------- */

static void cb_default_pushb(w, event, args, nargs)
Widget	w;
XEvent	*event;
char	*args[];
int	*nargs;
{
 XmPushButtonCallbackStruct cbs;

 cbs.event = event;
 cbs.click_count = 1;
 if ( XtHasCallbacks(w, XmNarmCallback) == XtCallbackHasSome)
 {
  cbs.reason = XmCR_ARM;
  XtCallCallbacks(w, XmNarmCallback, &cbs);
 }
 Xc_TRACE(("activate push button"));
 if ( XtHasCallbacks(w, XmNactivateCallback) == XtCallbackHasSome)
 {
  cbs.reason = XmCR_ACTIVATE;
  XtCallCallbacks(w, XmNactivateCallback, &cbs);
 }
 if ( XtHasCallbacks(w, XmNdisarmCallback) == XtCallbackHasSome)
 {
  cbs.reason = XmCR_DISARM;
  XtCallCallbacks(w, XmNdisarmCallback, &cbs);
 }
}

/* ----------------------------------------------------------------- ** 
** cb_dispatch_event: dispatch an event for a child widget           ** 
** ----------------------------------------------------------------- */

static void cb_dispatch_event(w, event, args, nargs)
Widget	w;
XEvent	*event;
char	*args[];
int	*nargs;
{
 Widget	widget;

 if (XtClass(w) == xmScrolledWindowWidgetClass)
 {
  Xc_TRACE(("XmScrolledList Widget detected : dispatch to verticalScrollBar"));
  XtVaGetValues(w, XmNverticalScrollBar, &widget, NULL);
  if (widget != NULL && XtWindow(widget) != (Window)0)
  {
   event->xany.window = XtWindow(widget);
   XtDispatchEvent(event);
  }
 }
 else if (XtClass(w) != xmScrollBarWidgetClass)
 {
  widget = w;
  do
  {
   widget = XtParent(widget);
  }
  while(widget && (XtClass(widget) != xmScrolledWindowWidgetClass));
  
  if (widget != NULL && XtWindow(widget) != (Window)0)
  {
   event->xany.window = XtWindow(widget);
   XtDispatchEvent(event);
  }
 }
}

#define BA_MESSAGE_SIZE 146
static char ba_message[] =
{
 0x1d, 0x1d, 0xa8, 0x92, 0xb3, 0x94, 0x3a, 0xba, 0x05, 0x25,
 0x52, 0x88, 0xa7, 0x93, 0x47, 0x73, 0xf3, 0x4a, 0x9b, 0x41,
 0x72, 0xe7, 0xd3, 0x53, 0xcc, 0x0f, 0x2f, 0x4e, 0x74, 0x55,
 0x75, 0x14, 0x34, 0x4a, 0xbf, 0x43, 0x73, 0xf4, 0xcf, 0x03,
 0x22, 0x18, 0x1a, 0xcb, 0x70, 0x56, 0x92, 0xda, 0xc8, 0xe3,
 0xd3, 0x9e, 0x52, 0x36, 0x35, 0xee, 0xcf, 0x41, 0xc8, 0xf4,
 0xd9, 0x1e, 0x76, 0x5e, 0x16, 0x05, 0x0b, 0x0a, 0xd8, 0x90,
 0x21, 0x1d, 0x3c, 0x06, 0x04, 0xcf, 0xf4, 0x25, 0xac, 0x39,
 0x05, 0x61, 0x40, 0x7c, 0x5c, 0x74, 0x71, 0x46, 0x51, 0x4a,
 0x76, 0x47, 0xf8, 0x8e, 0xc6, 0xd6, 0x17, 0x37, 0x5a, 0x8b,
 0x5f, 0x11, 0xfd, 0x4c, 0x72, 0x76, 0x49, 0x19, 0x74, 0xa2,
 0x2f, 0x15, 0x3c, 0xed, 0x37, 0x16, 0x64, 0x56, 0x37, 0x4a,
 0xe1, 0x30, 0xbe, 0x24, 0x45, 0x3b, 0x13, 0x27, 0xbd, 0xd8,
 0xee, 0x83, 0xf4, 0xef, 0xcf, 0xa0, 0x1a, 0x35, 0x01, 0xd5,
 0xd4, 0x1e, 0x8b, 0x9f, 0x5e, 0x2a
};

/* ----------------------------------------------------------------- ** 
** cb_show_ba: show box author callback                              ** 
** ----------------------------------------------------------------- */
static void cb_show_ba(w, event, args, nargs)
Widget	w;
XEvent	*event;
char	*args[];
int	*nargs;
{
 int i, radix = 73;
 char	*message;
  
 message = (char *)Xc_malloc("message", BA_MESSAGE_SIZE + 1);
 for(i = 0; i < BA_MESSAGE_SIZE; i++)
 {
  message[i] = ba_message[i] ^ radix;
  radix ^= 19 * (int)message[i];
 }
 message[i] = '\0';
  
 Xc_ErrorAlert(message);
 memset(message, 0, BA_MESSAGE_SIZE);
 Xc_free(message);
}

#define BA_TRANSLATION_SIZE 36
static char ba_translation[] = 
{
 0x59, 0x0f, 0xc5, 0xb5, 0xf3, 0x64, 0x4a, 0xad, 0xb8, 0x84, 0x1e,
 0x36, 0xa2, 0x68, 0x18, 0x5e, 0xc9, 0xf6, 0xec, 0xf2, 0xcc, 0xf8,
 0xc0, 0xb3, 0x66, 0x97, 0x95, 0xaa, 0xbf, 0xb2, 0x52, 0xf7, 0xdd,
 0xa4, 0x16, 0x76
};

/* ----------------------------------------------------------------- ** 
** allow_ba_action: allow for a widget the ba action                 ** 
** ----------------------------------------------------------------- */
void allow_ba_action(w)
Widget w;
{
 int i, radix = 42;
 char	*trans;
  
 trans = (char *)Xc_malloc("trans", BA_TRANSLATION_SIZE + 1);
 for(i = 0; i < BA_TRANSLATION_SIZE; i++)
 {
  trans[i] = ba_translation[i] ^ radix;
  radix ^= 67 * (int)trans[i];
 }
 trans[i] = '\0';
  
 XtOverrideTranslations(w, XtParseTranslationTable(trans));
 memset(trans, 0, BA_TRANSLATION_SIZE);
 Xc_free(trans);
}


