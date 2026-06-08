/*
** Edit_Functions.c for XAllWrite in Page/
** Miscellaneous functions for the Edit Menu
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
** Started on  Sat Apr 19 15:59:58 1997 Emmanuel Paris
** Last update Sat Jan 16 21:49:57 1999 One of the authors
*/

#include "Page.h"
#include "VectorFont.h"
#include "KeyBoard_Handler.h"
#include "Undo.h"
#include "Keyboard.h"
#include "Resource_WORD.h"

extern c_Text *GlobTextClipboard;
extern c_VectorFont *GlobFontBase;
extern c_Keyboard *GlobKeyboard;

void clipboard_cut_Page(This)
c_Page *This;
{
 c_Text	*Text;
 
 Xc_TRACE(("Cut Text"));
 Text = This->BaseStd->text_base;
 
 if (F(Text).need_undo(Text, 0))
  if (F(Text).need_undo(Text, 3))
  {
   if (SET_UNDO(XcR_UCut)) return;
  }
  else 
   FLUSH_UNDO;
 else		/* nothing to do, no selection */
  return;

 F(Text).clip_copy(Text, GlobTextClipboard);
 
 F(This->Redraw).undisplay_selection(This->Redraw, NULL);
 F(Text).delete(Text);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(This->Calibration).makeCursorVisible(This->Calibration);
}

void clipboard_copy_Page(This)
c_Page *This;
{
 c_Text	*Text;

 Xc_TRACE(("Copy Text"));
 Text = This->BaseStd->text_base;
 F(Text).clip_copy(Text, GlobTextClipboard);
}

void clipboard_paste_Page(This)
c_Page *This;
{
 c_Text	*Text;

 Xc_TRACE(("Paste Text"));
 Text = This->BaseStd->text_base;

 if (SET_UNDO(XcR_UPaste)) return;
 
 F(This->Redraw).undisplay_selection(This->Redraw, NULL);
 F(Text).clip_paste(Text, GlobTextClipboard);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(This->Calibration).makeCursorVisible(This->Calibration);
}

void delete_selection_Page(This)
c_Page *This;
{
 c_Text	*Text;

 Xc_TRACE(("Delete Selection of Text"));
 Text = This->BaseStd->text_base;
 
 if (F(Text).need_undo(Text, 3))
 {
  if (SET_UNDO(XcR_UDelete)) return;
 }
 else FLUSH_UNDO;

 F(This->Redraw).undisplay_selection(This->Redraw, NULL);
 F(Text).delete(Text);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(This->Calibration).makeCursorVisible(This->Calibration);
}

void select_all_Page(This)
c_Page *This;
{
 c_Text	*Text;

 Xc_TRACE(("Select All Text"));
 F(This->Redraw).display_selection(This->Redraw, NULL);
 Text = This->BaseStd->text_base;
 F(Text).select_all(Text);
 F(This->Calibration).calculSelection(This->Calibration, FALSE);
 F(This->Redraw).display_selection(This->Redraw, NULL);
}

void select_text_by_blocks(cpage, type)
c_Page *cpage;
int type;
{
 c_Text		*text;
 
 if (type == XA_ALL)
 {
  select_all_Page(cpage);
  return;
 }

 text = cpage->BaseStd->text_base;
 
 F(cpage->Redraw).start_selection_diff(cpage->Redraw);
 switch(type)
 {
 case XA_WORD:
 case XA_PARAGRAPH:
  if (type == XA_PARAGRAPH || text->select_start_pos)
   F(text).goTo(text, type, XA_LEFT, 1, NULL, 0, FALSE);
  F(text).goTo(text, type, XA_RIGHT, 1, NULL, 0, TRUE);
  F(cpage->Calibration).calculSelection(cpage->Calibration, FALSE);
  break;
 case XA_LINE:
  F(cpage->Calibration).goToBeginOfLine(cpage->Calibration, FALSE);
  F(cpage->Calibration).goToEndOfLine(cpage->Calibration, TRUE);
  break;
 case XA_PAGE:
  F(cpage->Calibration).goToBeginOfPage(cpage->Calibration, FALSE);
  F(cpage->Calibration).goToEndOfPage(cpage->Calibration, TRUE);
  break;
 }
 
 F(cpage->Redraw).draw_selection_diff(cpage->Redraw, NULL);
}

void insert_page_break_Page(This)
c_Page *This;
{
 c_Text	*Text;

 Xc_TRACE(("Insert Page Break"));
 Text = This->BaseStd->text_base;
 F(This->Redraw).undisplay_selection(This->Redraw, NULL);
 F(Text).insert(Text, XA_LINEFEED, NULL);
 F(Text).insert(Text, XA_PAGEBREAK, NULL);
 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(This->Calibration).makeCursorVisible(This->Calibration); 
}

void insert_euro_Page(This)
c_Page *This;
{
 c_VectorFont *font;
 family_tree	*family;
 c_Text	*Text;

 Xc_TRACE(("Insert Euro"));

 font = F(GlobFontBase).getFontByName(GlobFontBase, "Euro");
 if (!font || !font->spec_family) return;
 
 Text = This->BaseStd->text_base;
 F(This->Redraw).undisplay_selection(This->Redraw, NULL);
 
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
  F(Text).delete(Text);
 
 family = Text->style->font->spec_family;
 F(This).SetFontFamily(This, font->spec_family);
 
 F(Text).insert(Text, XA_TEXT, "\200");

 F(This).SetFontFamily(This, family);

 F(This->Calibration).updateText(This->Calibration);
 F(This->Redraw).display_selection(This->Redraw, NULL);
 F(This->Calibration).makeCursorVisible(This->Calibration); 
}

void insert_spec_char_Page(This)
c_Page *This;
{
 XEvent event;

 event.type = LASTEvent;
 GlobKeyboard->insertBox = TRUE;
 KB_ButtonPress_Handler(This, &event);
}
