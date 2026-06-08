/*
** Set_TextStyle.c for XAllWrite in Page/
** Set Text Style
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
** Started on  Fri Apr 11 17:16:25 1997 Emmanuel Paris
** Last update Wed Dec 16 17:23:58 1998 Robin Castanier
*/

#include "Page.h"
#include "Undo.h"
#include "Resource_WORD.h"

void set_font_bold ___PROTO((c_Page *This));
void set_font_italic ___PROTO((c_Page *This));
void set_font_underline ___PROTO((c_Page *This));
void set_font_strikeout ___PROTO((c_Page *This));
void set_font_shadow ___PROTO((c_Page *This));
void set_font_subscript ___PROTO((c_Page *This));
void set_font_supscript ___PROTO((c_Page *This));
void set_font_outline ___PROTO((c_Page *This));
void set_font_small_caps ___PROTO((c_Page *This));
void set_font_big_caps ___PROTO((c_Page *This));
void set_font_inc_size ___PROTO((c_Page *This));
void set_font_dec_size ___PROTO((c_Page *This));
void set_font_family ___PROTO((c_Page *This, family_tree *family));
void set_font_color ___PROTO((c_Page *This, c_Color *color));
void set_font_bgcolor ___PROTO((c_Page *This, c_Color *color));
void set_font_size ___PROTO((c_Page *This, coord_t size));
void set_font_style ___PROTO((c_Page *This, c_TextStyle *style));

static c_TextStyle *set_font_bold_callback();
static c_TextStyle *set_font_italic_callback();
static c_TextStyle *set_font_underline_callback();
static c_TextStyle *set_font_strikeout_callback();
static c_TextStyle *set_font_shadow_callback();
static c_TextStyle *set_font_subscript_callback();
static c_TextStyle *set_font_supscript_callback();
static c_TextStyle *set_font_outline_callback();
static c_TextStyle *set_font_small_caps_callback();
static c_TextStyle *set_font_big_caps_callback();
static c_TextStyle *set_font_inc_dec_size_callback();
static c_TextStyle *set_font_family_callback();
static c_TextStyle *set_font_color_callback();
static c_TextStyle *set_font_bgcolor_callback();
static c_TextStyle *set_font_size_callback();
static c_TextStyle *set_font_style_callback();

#define NB_CONST_FONT_SIZE	21
static coord_t dft_font_size[NB_CONST_FONT_SIZE] =
{
 SCALE_FROM_POINTS(4), SCALE_FROM_POINTS(6), SCALE_FROM_POINTS(8),
 SCALE_FROM_POINTS(9), SCALE_FROM_POINTS(10), SCALE_FROM_POINTS(12),
 SCALE_FROM_POINTS(14), SCALE_FROM_POINTS(16), SCALE_FROM_POINTS(18),
 SCALE_FROM_POINTS(24),
 SCALE_FROM_POINTS(36), SCALE_FROM_POINTS(64), SCALE_FROM_POINTS(100),
 SCALE_FROM_POINTS(200), SCALE_FROM_POINTS(300), SCALE_FROM_POINTS(400),
 SCALE_FROM_POINTS(500), SCALE_FROM_POINTS(600), SCALE_FROM_POINTS(700),
 SCALE_FROM_POINTS(800), SCALE_FROM_POINTS(900)
};

void set_font_bold(This)
c_Page *This;
{
 c_Text	*Text;
 c_TextStyle   *tstyle;
 int           set_bold;

 Text = This->BaseStd->text_base;
 tstyle = Text->style;

 if ((tstyle->attributes & XcTS_B_BOLD) != 0)
  set_bold = FALSE;
 else
  set_bold = TRUE;
 
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_UBold)) return;
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_bold_callback, This, (void *)set_bold);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
  F(Text).orTextStyle(Text, set_font_bold_callback, This, (void *)set_bold);

 Xc_TRACE(("set font bold"));
}
static c_TextStyle *set_font_bold_callback(page, tstyle, set_bold)
c_Page	    *page;
c_TextStyle *tstyle;
int set_bold;
{
 c_TextStyle   *new_tstyle;
 
 if (set_bold)
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_BOLD_ON,
			     XcTS_INHERIT, tstyle, (unsigned int)XcTS_H_BOLD,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_BOLD_OFF, 
			     XcTS_INHERIT, tstyle, (unsigned int)XcTS_H_BOLD,
			     XcTS_END);
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 return new_tstyle;
}

void set_font_italic(This)
c_Page *This;
{
 c_Text	*Text;
 c_TextStyle   *tstyle;
 int           set_italic;

 Text = This->BaseStd->text_base;
 tstyle = Text->style;

 if ((tstyle->attributes & XcTS_B_ITALIC) != 0)
  set_italic = FALSE;
 else
  set_italic = TRUE;
 
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_UItalic)) return;
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_italic_callback, This,
		      (void *)set_italic);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
  F(Text).orTextStyle(Text, set_font_italic_callback, This,
		      (void *)set_italic);

 Xc_TRACE(("set font italic"));
}

static c_TextStyle *set_font_italic_callback(page, tstyle, set_italic)
c_Page	    *page;
c_TextStyle *tstyle;
int set_italic;
{
 c_TextStyle   *new_tstyle;
 
 if (set_italic)
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_ITALIC_ON,
			     XcTS_INHERIT, tstyle, (unsigned int)XcTS_H_ITALIC,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_ITALIC_OFF, 
			     XcTS_INHERIT, tstyle, (unsigned int)XcTS_H_ITALIC,
			     XcTS_END);
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 return new_tstyle;
}

void set_font_underline(This)
c_Page *This;
{
 c_Text	*Text;
 c_TextStyle   *tstyle;
 int           set_underline;

 Text = This->BaseStd->text_base;
 tstyle = Text->style;

 if ((tstyle->attributes & XcTS_B_UNDERLINE) != 0)
  set_underline = FALSE;
 else
  set_underline = TRUE;
 
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_UUnderline)) return;
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_underline_callback, 
		      This, (void *)set_underline);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
  F(Text).orTextStyle(Text, set_font_underline_callback, 
		      This, (void *)set_underline);

 Xc_TRACE(("set font underline"));
}

static c_TextStyle *set_font_underline_callback(page, tstyle, set_underline)
c_Page	    *page;
c_TextStyle *tstyle;
int set_underline;
{
 c_TextStyle   *new_tstyle;
 
 if (set_underline)
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_UNDERLINE_ON,
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_UNDERLINE,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_UNDERLINE_OFF, 
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_UNDERLINE,
			     XcTS_END);
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 return new_tstyle;
}

void set_font_strikeout(This)
c_Page *This;
{
 c_Text	*Text;
 c_TextStyle   *tstyle;
 int           set_strikeout;

 Text = This->BaseStd->text_base;
 tstyle = Text->style;

 if ((tstyle->attributes & XcTS_B_STRIKEOUT) != 0)
  set_strikeout = FALSE;
 else
  set_strikeout = TRUE;
 
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_UStrikeout)) return;
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_strikeout_callback, 
		      This, (void *)set_strikeout);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
  F(Text).orTextStyle(Text, set_font_strikeout_callback, 
		      This, (void *)set_strikeout);

 Xc_TRACE(("set font strikeout"));
}

static c_TextStyle *set_font_strikeout_callback(page, tstyle, set_strikeout)
c_Page	    *page;
c_TextStyle *tstyle;
int set_strikeout;
{
 c_TextStyle   *new_tstyle;
 
 if (set_strikeout)
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_STRIKEOUT_ON,
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_STRIKEOUT,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_STRIKEOUT_OFF, 
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_STRIKEOUT,
			     XcTS_END);
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 return new_tstyle;
}

void set_font_shadow(This)
c_Page *This;
{
 c_Text	*Text;
 c_TextStyle   *tstyle;
 int           set_shadow;

 Text = This->BaseStd->text_base;
 tstyle = Text->style;

 if ((tstyle->attributes & XcTS_B_SHADOW) != 0)
  set_shadow = FALSE;
 else
  set_shadow = TRUE;
 
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_UShadow)) return;  
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_shadow_callback, 
		      This, (void *)set_shadow);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
  F(Text).orTextStyle(Text, set_font_shadow_callback, 
		      This, (void *)set_shadow);

 Xc_TRACE(("set font shadow"));
}

static c_TextStyle *set_font_shadow_callback(page, tstyle, set_shadow)
c_Page	    *page;
c_TextStyle *tstyle;
int set_shadow;
{
 c_TextStyle   *new_tstyle;
 
 if (set_shadow)
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SHADOW_ON,
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_SHADOW,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SHADOW_OFF, 
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_SHADOW,
			     XcTS_END);
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 return new_tstyle;
}

void set_font_subscript(This)
c_Page *This;
{
 c_Text	*Text;
 c_TextStyle   *tstyle;
 int           set_subscript;

 Text = This->BaseStd->text_base;
 tstyle = Text->style;

 if ((tstyle->attributes & XcTS_B_SUBSCRIPT) != 0)
  set_subscript = FALSE;
 else
  set_subscript = TRUE;
 
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_USubscript)) return;   
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_subscript_callback, 
		      This, (void *)set_subscript);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
  F(Text).orTextStyle(Text, set_font_subscript_callback, 
		      This, (void *)set_subscript);

 Xc_TRACE(("set font subscript"));
}

static c_TextStyle *set_font_subscript_callback(page, tstyle, set_subscript)
c_Page	    *page;
c_TextStyle *tstyle;
int set_subscript;
{
 c_TextStyle   *new_tstyle;
 
 if (set_subscript)
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SUBSCRIPT_ON,
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_SUBSCRIPT,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SUBSCRIPT_OFF, 
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_SUBSCRIPT,
			     XcTS_END);
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 return new_tstyle;
}

void set_font_supscript(This)
c_Page *This;
{
 c_Text	*Text;
 c_TextStyle   *tstyle;
 int           set_supscript;

 Text = This->BaseStd->text_base;
 tstyle = Text->style;

 if ((tstyle->attributes & XcTS_B_SUPSCRIPT) != 0)
  set_supscript = FALSE;
 else
  set_supscript = TRUE;
 
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_USupscript)) return;  
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_supscript_callback, 
		      This, (void *)set_supscript);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
  F(Text).orTextStyle(Text, set_font_supscript_callback, 
		      This, (void *)set_supscript);

 Xc_TRACE(("set font supscript"));
}

static c_TextStyle *set_font_supscript_callback(page, tstyle, set_supscript)
c_Page	    *page;
c_TextStyle *tstyle;
int set_supscript;
{
 c_TextStyle   *new_tstyle;
 
 if (set_supscript)
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SUPSCRIPT_ON,
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_SUPSCRIPT,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SUPSCRIPT_OFF, 
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_SUPSCRIPT,
			     XcTS_END);
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 return new_tstyle;
}

void set_font_outline(This)
c_Page *This;
{
 c_Text	*Text;
 c_TextStyle   *tstyle;
 int           set_outline;

 Text = This->BaseStd->text_base;
 tstyle = Text->style;

 if ((tstyle->attributes & XcTS_B_OUTLINE) != 0)
  set_outline = FALSE;
 else
  set_outline = TRUE;
 
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_outline_callback, 
		      This, (void *)set_outline);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
  F(Text).orTextStyle(Text, set_font_outline_callback, 
		      This, (void *)set_outline);
 
 Xc_TRACE(("set font outline"));
}

static c_TextStyle *set_font_outline_callback(page, tstyle, set_outline)
c_Page	    *page;
c_TextStyle *tstyle;
int set_outline;
{
 c_TextStyle   *new_tstyle;
 
 if (set_outline)
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_OUTLINE_ON,
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_OUTLINE,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_OUTLINE_OFF, 
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_OUTLINE,
			     XcTS_END);
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 return new_tstyle;
}

void set_font_small_caps(This)
c_Page *This;
{
 c_Text	*Text;
 c_TextStyle   *tstyle;
 int           set_small_caps;

 Text = This->BaseStd->text_base;
 tstyle = Text->style;

 if ((tstyle->attributes & XcTS_B_SMALL_CAPS) != 0)
  set_small_caps = FALSE;
 else
  set_small_caps = TRUE;
 
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_USmallcaps)) return;  
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_small_caps_callback, 
		      This, (void *)set_small_caps);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
  F(Text).orTextStyle(Text, set_font_small_caps_callback, 
		      This, (void *)set_small_caps);
 
 Xc_TRACE(("set font small_caps"));
}

static c_TextStyle *set_font_small_caps_callback(page, tstyle, set_small_caps)
c_Page	    *page;
c_TextStyle *tstyle;
int set_small_caps;
{
 c_TextStyle   *new_tstyle;
 
 if (set_small_caps)
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SMALL_CAPS_ON,
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_SMALL_CAPS,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SMALL_CAPS_OFF, 
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_SMALL_CAPS,
			     XcTS_END);
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 return new_tstyle;
}

void set_font_big_caps(This)
c_Page *This;
{
 c_Text	*Text;
 c_TextStyle   *tstyle;
 int           set_big_caps;

 Text = This->BaseStd->text_base;
 tstyle = Text->style;

 if ((tstyle->attributes & XcTS_B_BIG_CAPS) != 0)
  set_big_caps = FALSE;
 else
  set_big_caps = TRUE;
 
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_UBigcaps)) return;  
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_big_caps_callback, 
		      This, (void *)set_big_caps);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
  F(Text).orTextStyle(Text, set_font_big_caps_callback, 
		      This, (void *)set_big_caps);
 Xc_TRACE(("set font big_caps"));
}

static c_TextStyle *set_font_big_caps_callback(page, tstyle, set_big_caps)
c_Page	    *page;
c_TextStyle *tstyle;
int set_big_caps;
{
 c_TextStyle   *new_tstyle;
 
 if (set_big_caps)
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_BIG_CAPS_ON,
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_BIG_CAPS,
			     XcTS_END);
 else
  new_tstyle = F(tstyle).get(tstyle, page->BaseStd,  XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_BIG_CAPS_OFF, 
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_BIG_CAPS,
			     XcTS_END);
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 return new_tstyle;
}

void set_font_inc_size(This)
c_Page *This;
{
 c_Text	*Text;
  
 Text = This->BaseStd->text_base;
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_UFincsize)) return;  
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_inc_dec_size_callback, 
		      This, (void *)TRUE);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
 {
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_inc_dec_size_callback, 
		      This, (void *)TRUE);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 Xc_TRACE(("set font increase size"));
}

void set_font_dec_size(This)
c_Page *This;
{
 c_Text	*Text;
  
 Text = This->BaseStd->text_base;
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_UFdecsize)) return;  
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_inc_dec_size_callback, 
		      This, (void *)FALSE);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
 {
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_inc_dec_size_callback, 
		      This, (void *)FALSE);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 Xc_TRACE(("set font decrease size"));
}

static c_TextStyle *set_font_inc_dec_size_callback(page, tstyle, incr)
c_Page	    *page;
c_TextStyle *tstyle;
boolean	    incr;
{
 c_TextStyle   *new_tstyle;
 coord_t	size;
 int		i;
 
 size = tstyle->point_size;
 if (incr)
 {
  i = 0;
  while(i < NB_CONST_FONT_SIZE)
  {
   if (size < dft_font_size[i])
    break;
   i++;
  }
  if (i == NB_CONST_FONT_SIZE)
   return tstyle;
  size = dft_font_size[i];
 }
 else
 {
  i = NB_CONST_FONT_SIZE -1;
  while(i >= 0)
  {
   if (size > dft_font_size[i])
    break;
   i--;
  }
  if (i < 0)
   return tstyle;
  size = dft_font_size[i];
 }
  
 new_tstyle =  F(tstyle).get(tstyle, page->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SIZE, size,
			     XcTS_INHERIT, tstyle, (unsigned int)XcTS_H_SIZE,
			     XcTS_END);
 Xc_TRACE(("new style name: %s", new_tstyle->name));
 
 return new_tstyle;
}

void set_font_family(This, family)
c_Page		*This;
family_tree	*family;
{
 c_Text	*Text;
 
 Text = This->BaseStd->text_base;
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_UFfamily)) return;  
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_family_callback, This, (void *)family);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
  F(Text).orTextStyle(Text, set_font_family_callback, This, (void *)family);

 Xc_TRACE(("set font family"));
}

static c_TextStyle *set_font_family_callback(page, tstyle, family)
c_Page	    *page;
c_TextStyle *tstyle;
family_tree	*family;
{
 c_TextStyle	*new_tstyle;
 font_list	*fontl;
 boolean	bold;
 boolean	italic; 

 fontl = tstyle->font->specific;
  
 bold = italic = FALSE;
 if (fontl->is_bold)
  bold = TRUE;
 if (fontl->is_italic)
  italic = TRUE;
 if (fontl->is_bold_italic)
  bold = italic = TRUE;
  
 fontl = family->list;
 if (family->can_bold_italic && bold && italic)
 {
  while(fontl && !fontl->is_bold_italic)
   fontl = fontl->Next;
 }
 else
 {
  if (family->can_bold && bold)
  {
   while(fontl && !fontl->is_bold)
    fontl = fontl->Next;
  }
  else
  {
   if (family->can_italic && italic)
   {
    while(fontl && !fontl->is_italic)
     fontl = fontl->Next;
   }
   else
   {
    while(fontl && !fontl->is_normal)
     fontl = fontl->Next;
   }
  }
 }
 if (fontl == NULL)
  return tstyle;
 
 new_tstyle =  F(tstyle).get(tstyle, page->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_FONT, fontl->font, 
			     XcTS_INHERIT, tstyle, (unsigned int)XcTS_H_FONT,
			     XcTS_END);
 
 Xc_TRACE(("new style name: %s", new_tstyle->name));
 
 return new_tstyle;
}

void set_font_color(This, color)
c_Page		*This;
c_Color		*color;
{
 c_Text	*Text;
 
 Text = This->BaseStd->text_base;
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_UFcolor)) return;  
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_color_callback, This, (void *)color);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
  F(Text).orTextStyle(Text, set_font_color_callback, This, (void *)color);
 
 Xc_TRACE(("set font color"));
}

static c_TextStyle *set_font_color_callback(page, tstyle, color)
c_Page	    *page;
c_TextStyle *tstyle;
c_Color	    *color;
{
 c_TextStyle	*new_tstyle;

 new_tstyle =  F(tstyle).get(tstyle, page->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_COLOR, color, 
			     XcTS_INHERIT, tstyle, (unsigned int)XcTS_H_COLOR,
			     XcTS_END);
 Xc_TRACE(("new style name: %s", new_tstyle->name));
 
 return new_tstyle;
}

void set_font_bgcolor(This, color)
c_Page		*This;
c_Color		*color;
{
 c_Text	*Text;
 
 Text = This->BaseStd->text_base;
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_UFbgcolor)) return;  
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_bgcolor_callback, This, (void *)color);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
  F(Text).orTextStyle(Text, set_font_bgcolor_callback, This, (void *)color);
 
 Xc_TRACE(("set font color"));
}

static c_TextStyle *set_font_bgcolor_callback(page, tstyle, color)
c_Page	    *page;
c_TextStyle *tstyle;
c_Color	    *color;
{
 c_TextStyle	*new_tstyle;

 new_tstyle =  F(tstyle).get(tstyle, page->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_BGCOLOR, color, 
			     XcTS_INHERIT, tstyle, 
			     (unsigned int)XcTS_H_BGCOLOR,
			     XcTS_END);
 Xc_TRACE(("new style name: %s", new_tstyle->name));
 
 return new_tstyle;
}

void set_font_size(This, size)
c_Page		*This;
coord_t		size;
{
 c_Text	*Text;
 
  Text = This->BaseStd->text_base;
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (SET_UNDO(XcR_UFsize)) return;  
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_size_callback, This, (void *)&size);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
 {
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_size_callback, This, (void *)&size);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }

 Xc_TRACE(("set font color"));
}

static c_TextStyle *set_font_size_callback(page, tstyle, size)
c_Page	    *page;
c_TextStyle *tstyle;
coord_t	    *size;
{
 c_TextStyle	*new_tstyle;

 new_tstyle =  F(tstyle).get(tstyle, page->BaseStd, XcTS_NAME, NULL,
			     XcTS_HIDE_ON, XcTS_SIZE, *size, 
			     XcTS_INHERIT, tstyle, (unsigned int)XcTS_H_SIZE,
			     XcTS_END);
 Xc_TRACE(("new style name: %s", new_tstyle->name));

 return new_tstyle;
}

void set_font_style(This, style)
c_Page		*This;
c_TextStyle	*style;
{
 c_Text	*Text;
  
 Text = This->BaseStd->text_base;
 if (Text->select_start != Text->select_end ||
     Text->select_start_pos != Text->select_end_pos)
 {
  if (GET_UNDO != XcR_USApply && SET_UNDO(XcR_UFstyle)) return;  
  F(This->Redraw).display_selection(This->Redraw, NULL);
  F(Text).orTextStyle(Text, set_font_style_callback, This, (void *)style);
  F(This->Calibration).updateText(This->Calibration);
  F(This->Redraw).display_selection(This->Redraw, NULL);
 }
 else
  F(Text).orTextStyle(Text, set_font_style_callback, This, (void *)style);
 
 Xc_TRACE(("set font style"));
}

static c_TextStyle *set_font_style_callback(page, tstyle, style)
c_Page	    *page;
c_TextStyle *tstyle;
c_TextStyle *style;
{
 return style;
}
