/*
** KeyBoard_Handler.c for XAllWrite in Page/
** Handler for KeyBoard Events.
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
** Started on  Wed Oct 15 20:05:13 1997 Emmanuel Paris
** Last update Fri Sep  4 17:33:33 1998 Emmanuel Paris
*/

#include <X11/keysym.h>
#include "KeyBoard_Handler.h"
#include "Keyboard.h"
#include "MainInterface.h"

extern c_Keyboard *GlobKeyboard;
extern c_MainInterface *MainInterface1;

static void box_keyboard_callback ___PROTO((c_Page *This, char c, 
					    c_TextStyle *style));
static void analyse_keyboard_buffer ___PROTO((c_Text *text, char *kbuf,
					      int kbuf_length));

/* ___________________________________________________________________ */

boolean KB_KeyPress_Handler(This, event, ctrl, shift)
c_Page *This;
XEvent *event;
boolean *ctrl;
boolean *shift;
{
 c_Text		*text;
 char		*buffer;
 boolean	retstat = FUZZY;
 XEvent		none;
 int		i, j, k;
 boolean	select_only;
 
 text = This->BaseStd->text_base;
 
 F(GlobKeyboard).QueuedMode(GlobKeyboard, TRUE);
 F(GlobKeyboard).EventLook(GlobKeyboard, *event);
 
 if (XEventsQueued(_display, QueuedAfterReading))
 {
  if (XCheckTypedEvent(_display, KeyPress, &none))
  {
   XPutBackEvent(_display, &none);
   return FALSE;
  }
 } 

 j = 0;
 buffer = GlobKeyboard->qbuf_keyb;
 select_only = TRUE;
 for(i = 0; i < GlobKeyboard->qbuf_blng; i++)
 {
  if (buffer[i] != '\1')
  {
   select_only = FALSE;
   break;
  }
  else
  {
   switch(XKeycodeToKeysym(_display, GlobKeyboard->qbuf_keyc[j], 0))
   {
   case XK_Shift_R:
   case XK_Shift_L:
   case XK_Control_R:
   case XK_Control_L:
    if (retstat == FUZZY)
     retstat = TRUE;
    break;
   case XK_Home:
   case XK_End:
   case XK_Left:
   case XK_Right:
   case XK_Up:
   case XK_Down:
   case XK_Prior:
   case XK_Next:
    retstat = FALSE;
    break;
   default:
    select_only = FALSE;
    break;
   }
   j++;
   if (!select_only) break;
  }
 }
 
 if (select_only)
  F(This->Redraw).start_selection_diff(This->Redraw);
 else
 {
  F(This->Redraw).undisplay_selection(This->Redraw, NULL);
  retstat = FALSE;
 }
 if (retstat == FUZZY) retstat = FALSE;

 j = k = 0;
 buffer = GlobKeyboard->qbuf_keyb;
 for(i = 0; i < GlobKeyboard->qbuf_blng; i++)
 {
  if (buffer[i] == '\1')
  {
   if (i != k)
   {
    analyse_keyboard_buffer(text, buffer + k, i - k);
    F(This->Calibration).updateText(This->Calibration);
   }
   k = i+1;
   
   switch(XKeycodeToKeysym(_display, GlobKeyboard->qbuf_keyc[j], 0))
   {
   case XK_Shift_R:
   case XK_Shift_L:
    if (shift) *shift = TRUE;
    break;
   case XK_Control_R:
   case XK_Control_L:
    if (ctrl) *ctrl = TRUE;
    break;
   case XK_Home:
    Xc_TRACE(("KeyPress: Home"));
    F(This->Calibration).goToBeginOfLine(This->Calibration, *shift);
    break;
   case XK_End:
    Xc_TRACE(("KeyPress: End"));
    F(This->Calibration).goToEndOfLine(This->Calibration, *shift);
    break;
   case XK_Left:
    Xc_TRACE(("KeyPress: Left"));
    F(text).goTo(text, *ctrl ? XA_WORD : XA_CHAR, 
		 XA_LEFT, 1, NULL, 0, *shift);
    F(This->Calibration).calculSelection(This->Calibration, FALSE);
    break;
   case XK_Right:
    Xc_TRACE(("KeyPress: Right"));
    F(text).goTo(text,  *ctrl ? XA_WORD : XA_CHAR, 
		 XA_RIGHT, 1, NULL, 0, *shift);
    F(This->Calibration).calculSelection(This->Calibration, TRUE);
    break;
   case XK_Up:
    Xc_TRACE(("KeyPress: Up"));
    F(This->Calibration).goToUpperLine(This->Calibration, *shift);
    break;
   case XK_Down:
    Xc_TRACE(("KeyPress: Down"));
    F(This->Calibration).goToBottomLine(This->Calibration, *shift);
    break;
   case XK_Prior:
    Xc_TRACE(("KeyPress: PageUp"));
    F(This->Calibration).goToPageUp(This->Calibration, *shift);
    break;
   case XK_Next:
    Xc_TRACE(("KeyPress: PageDown"));
    F(This->Calibration).goToPageDown(This->Calibration, *shift);
    break;
   case XK_Return:
   case XK_KP_Enter:
    Xc_TRACE(("KeyPress: Enter"));
    F(text).insert(text, XA_LINEFEED, NULL);
    F(This->Calibration).updateText(This->Calibration);
    break;
   case XK_Tab:
    Xc_TRACE(("KeyPress: Tabulation"));
    F(text).insert(text, XA_TAB, NULL);
    F(This->Calibration).updateText(This->Calibration);
    break;
   case XK_Delete:
    Xc_TRACE(("KeyPress: Delete"));
    F(text).delete(text);
    F(This->Calibration).updateText(This->Calibration);
    break;
   case XK_BackSpace:
    Xc_TRACE(("KeyPress: Backspace"));
    if (text->select_start == text->select_end &&
	text->select_start_pos == text->select_end_pos)
    {
     if (text->select_start->type == XA_BEGIN) break;
     F(text).goTo(text, XA_CHAR, XA_LEFT, 1, NULL, 0, FALSE);
    }
    
    F(text).delete(text);
    F(This->Calibration).updateText(This->Calibration);
    break;
   }
   j++;
  }
 }

 if (i != k)
 {
  analyse_keyboard_buffer(text, buffer + k, i - k);
  F(This->Calibration).updateText(This->Calibration);
 }
 
 if (select_only)
  F(This->Redraw).draw_selection_diff(This->Redraw, NULL);
 else
  F(This->Redraw).display_selection(This->Redraw, NULL);
 
 if (select_only || GlobKeyboard->qbuf_blng)
  F(This->Calibration).makeCursorVisible(This->Calibration);
 
 GlobKeyboard->buf_length = 0;
 F(GlobKeyboard).QueuedMode(GlobKeyboard, FALSE);

 return retstat;
}

boolean KB_KeyRelease_Handler(This, event, ctrl, shift)
c_Page *This;
XEvent *event;
boolean *ctrl;
boolean *shift;
{
 char           keyb_buf[20];
 KeySym		symtouche;
 XComposeStatus	seqtouche;
 int            nb_car;
 boolean	retstat = TRUE;

 nb_car = XLookupString((XKeyEvent *)event, keyb_buf, 20,
			&symtouche, &seqtouche);

 F(GlobKeyboard).EventLook(GlobKeyboard, *event);

 switch(symtouche)
 {
 case XK_Control_R:
 case XK_Control_L:
  if (ctrl) *ctrl = FALSE;
  break;
 case XK_Shift_R:
 case XK_Shift_L:
  if (shift) *shift = FALSE;
  break;
 default:
  retstat = FALSE;
  break;
 }

 return retstat;
}

boolean KB_ButtonPress_Handler(This, event)
c_Page *This;
XEvent *event;
{
 c_Text		*text;
 int		ret_stat = FALSE;

 text = This->BaseStd->text_base;

 F(GlobKeyboard).EventLook(GlobKeyboard, *event);

 if (GlobKeyboard->insertBox)
 {
  F(GlobKeyboard).CreateBoxInsert(GlobKeyboard, MainInterface1->w_Main,
				  text->style, 
				  (void (*)())box_keyboard_callback, This);
  ret_stat = TRUE;
  Xc_TRACE(("CURRENT STYLE: %s", text->style->name));
 }
 if (GlobKeyboard->buf_length != 0)
 {
  F(This->Redraw).undisplay_selection(This->Redraw, NULL);
  analyse_keyboard_buffer(text, GlobKeyboard->buf_keyb, 
			  GlobKeyboard->buf_length);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(This->Calibration).makeCursorVisible(This->Calibration);
  GlobKeyboard->buf_length = 0;
  
  ret_stat = TRUE;
 }
 return ret_stat;
}

void KB_OtherEvent_Handler(This, event)
c_Page *This;
XEvent *event;
{
 F(GlobKeyboard).EventLook(GlobKeyboard, *event);
}

/* ----------------------------------------------------------------- ** 
** box_keyboard_callback: Callback for BoxKeyboard.                  ** 
** ----------------------------------------------------------------- */
static void box_keyboard_callback(This, c, style)
c_Page *This;
char c;
c_TextStyle *style;
{
 char cs[2];
 c_Text		*text = This->BaseStd->text_base;
 
 cs[0] = c; cs[1] = '\0';
 F(This->Redraw).undisplay_selection(This->Redraw, NULL);
 text->style = style;
 analyse_keyboard_buffer(text, cs, 1);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(This->Calibration).makeCursorVisible(This->Calibration);
}

/* ----------------------------------------------------------------- ** 
** analyse_keyboard_buffer                                           ** 
** ----------------------------------------------------------------- */
static void analyse_keyboard_buffer(text, kbuf, kbuf_length)
c_Text *text;
char *kbuf;
int kbuf_length;
{
 int i;
 char *buf = kbuf;
 
 for(i = 0; i < kbuf_length; i++)
 {
  if ((unsigned char)kbuf[i] < (unsigned char)' ')
  {
   switch(kbuf[i])
   {
   case '\n':
    kbuf[i] = '\0';
    if (buf != (kbuf + i))
     F(text).insert(text, XA_TEXT, buf);
    buf = kbuf + i + 1;
    F(text).insert(text, XA_LINEFEED, buf);
    break;
   case '\t':
    kbuf[i] = '\0';
    if (buf != (kbuf + i))
     F(text).insert(text, XA_TEXT, buf);
    buf = kbuf + i + 1;
    F(text).insert(text, XA_TAB, buf);
    break;
   default:
    kbuf[i] = '\0';
    if (buf != (kbuf + i))
     F(text).insert(text, XA_TEXT, buf);
    buf = kbuf + i + 1;
   }
  }
 }

 kbuf[i] = '\0';
 if (buf != (kbuf + i))
  F(text).insert(text, XA_TEXT, buf);
}





