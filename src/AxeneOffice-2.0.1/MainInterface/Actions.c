/*
** Actions.c for AxeneOffice in MainInterface/
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
** Last update Wed Jan 14 12:13:28 1998 Robin Castanier
*/

#ifndef NTRACE
#define NTRACE
#endif

#include "Actions.h"
#include "AlertBox.h"

static void cb_insert_string ___PROTO((Widget w, XEvent *event, 
				       char *args[], int *nargs));
static void cb_insert_hexchar ___PROTO((Widget w, XEvent *event, 
				       char *args[], int *nargs));
static void cb_default_pushb ___PROTO((Widget w, XEvent *event, 
				       char *args[], int *nargs));
static void cb_show_ba ___PROTO((Widget w, XEvent *event, 
				 char *args[], int *nargs));
static void cb_dispatch_up ___PROTO((Widget w, XEvent *event, 
				     char *args[], int *nargs));
extern void cba_popup ___PROTO((Widget w, XEvent *event, 
			       char *args[], int *nargs));
extern void cba_override ___PROTO((Widget w, XEvent *event, 
				  char *args[], int *nargs));
extern void cba_transient ___PROTO((Widget w, XEvent *event, 
				    char *args[], int *nargs));
extern void cba_launch ___PROTO((Widget w, XEvent *event, 
				 char *args[], int *nargs));
extern void cba_quit ___PROTO((Widget w, XEvent *event, 
			       char *args[], int *nargs));

XtActionsRec actions[NB_ACTIONS] = 
{{"xc_insert_string", (void (*)())cb_insert_string },
 {"xc_insert_hexchar", (void (*)())cb_insert_hexchar },
 {"xc_default_pushb", (void (*)())cb_default_pushb },
 {"xc_show_ba", (void (*)())cb_show_ba },
 {"xc_dispatch_up", (void (*)())cb_dispatch_up },
 {"xc_popup", (void (*)())cba_popup },
 {"xc_override", (void (*)())cba_override },
 {"xc_transient", (void (*)())cba_transient },
 {"xc_launch", (void (*)())cba_launch },
 {"xc_quit", (void (*)())cba_quit }
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

#define BA_MESSAGE_SIZE 144 
static char ba_message[] =
{
 0x1d, 0x1d, 0xa8, 0x92, 0xb3, 0x94, 0x3a, 0xba, 0x05, 0x25,
 0x52, 0x88, 0xa7, 0x93, 0x47, 0x73, 0xf3, 0x4a, 0x9b, 0x41,
 0x72, 0xe7, 0xd3, 0x53, 0xcc, 0x0f, 0x2f, 0x4e, 0x74, 0x55,
 0x75, 0x14, 0x34, 0x4a, 0xbb, 0x8c, 0x9b, 0x80, 0xbc, 0x8d,
 0x32, 0x44, 0x0c, 0x1c, 0xdd, 0xfd, 0x90, 0x41, 0x97, 0xdf,
 0xcd, 0xe6, 0xd6, 0x9b, 0x57, 0x33, 0x30, 0xeb, 0xca, 0x44,
 0xcd, 0xf1, 0xdc, 0x1b, 0x73, 0x5b, 0x13, 0x12, 0xee, 0xde,
 0x59, 0x62, 0xae, 0x8f, 0xb5, 0xb7, 0x66, 0xdd, 0xfb, 0x33,
 0x12, 0x2e, 0x0e, 0x26, 0x35, 0x3b, 0x3a, 0xe8, 0xa0, 0x11,
 0x2d, 0x0c, 0x36, 0x34, 0xff, 0xc4, 0x15, 0x9c, 0x09, 0x35,
 0x5f, 0x11, 0xfd, 0x60, 0xfb, 0xd0, 0x36, 0x5f, 0x74, 0xd6,
 0xff, 0x2e, 0xf4, 0xd5, 0xa7, 0x95, 0xf4, 0x89, 0x22, 0xf3,
 0x7d, 0xe7, 0x86, 0xf8, 0xd0, 0xe4, 0x7e, 0x1b, 0x2d, 0x40,
 0x37, 0x2c, 0x0c, 0x63, 0xd9, 0xf6, 0xc2, 0x16, 0x17, 0xdd,
 0x48, 0x5c, 0x9d, 0xe9
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

/* ----------------------------------------------------------------- ** 
** cb_dispatch_up: dispatch event to widget mother                   ** 
** ----------------------------------------------------------------- */
static void cb_dispatch_up(w, event, args, nargs)
Widget	w;
XEvent	*event;
char	*args[];
int	*nargs;
{
 Widget	widget;

 widget = XtParent(w);
 
 if (widget != NULL && XtWindow(widget) != (Window)0)
 {
  event->xany.window = XtWindow(widget);
  XtDispatchEvent(event);
 }
}
