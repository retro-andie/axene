/*
** docfile.c for Xclamation, XQuad and XAllWrite in TextStyle/
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
** Started on  Sun Jun 12 02:10:50 1994 Stéphane Boisson
** Last update Mon Apr 13 17:18:50 1998 Emmanuel Paris
*/

/*#define NTRACE*/

#include "TextStyle.h"

boolean readStyle ___PROTO((c_DocFile *doc, char *keyword,
			    long param, textstyle_read_t *data));
boolean writeStyle ___PROTO((c_TextStyle *this, c_DocFile *doc));
boolean writeStyleList ___PROTO((c_TextStyle *this, c_DocFile *doc));

static boolean cb_bgcolor ___PROTO((c_DocFile *doc, char *keyword,
                                   long param, textstyle_read_t *data));
static boolean cb_italic ___PROTO((c_DocFile *doc, char *keyword,
                                   long param, textstyle_read_t *data));
static boolean cb_underline ___PROTO((c_DocFile *doc, char *keyword,
				      long param, textstyle_read_t *data));
static boolean cb_strikeout ___PROTO((c_DocFile *doc, char *keyword,
				      long param, textstyle_read_t *data));
static boolean cb_subscript ___PROTO((c_DocFile *doc, char *keyword,
				      long param, textstyle_read_t *data));
static boolean cb_supscript ___PROTO((c_DocFile *doc, char *keyword,
				      long param, textstyle_read_t *data));
static boolean cb_outline ___PROTO((c_DocFile *doc, char *keyword,
				    long param, textstyle_read_t *data));
static boolean cb_width ___PROTO((c_DocFile *doc, char *keyword,
				  long param, textstyle_read_t *data));
static boolean cb_shadow ___PROTO((c_DocFile *doc, char *keyword,
                                   long param, textstyle_read_t *data));
static boolean cb_color ___PROTO((c_DocFile *doc, char *keyword,
				  long param, textstyle_read_t *data));
static boolean cb_font ___PROTO((c_DocFile *doc, char *keyword,
				 long param, textstyle_read_t *data));
static boolean cb_name ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_TextStyle *this));
static boolean cb_hide ___PROTO((c_DocFile *doc, char *keyword,
				 long param, textstyle_read_t *data));
static boolean cb_small_caps ___PROTO((c_DocFile *doc, char *keyword,
				       long param, textstyle_read_t *data));
static boolean cb_big_caps ___PROTO((c_DocFile *doc, char *keyword,
				     long param, textstyle_read_t *data));
static boolean cb_inherit ___PROTO((c_DocFile *doc, char *keyword,
				    long param, textstyle_read_t *data));
static boolean cb_style ___PROTO((c_DocFile *doc, char *keyword,
				  long param, textstyle_read_t *data));

/* ----------------------------------------------------------------- ** 
** readStyle - Read method                                           ** 
** ----------------------------------------------------------------- */
boolean readStyle(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 c_TextStyle *style;

 style = data->style = NEW(c_TextStyle)(data->style_base, NULL, NULL,
					SCALE_FROM_POINTS(0), NULL);
 if(style == NULL) return FALSE;
 style->id = param;
 do 
 {
  unsigned char *ptr;
  real r;

  if(!F(doc).addCallbacks(doc,
			  XcDF_BGCOLOR_KEYWORD, cb_bgcolor, data,
			  XcDF_ITALIC_KEYWORD, cb_italic, data,
			  XcDF_UNDERLINE_KEYWORD, cb_underline, data,
			  XcDF_STRIKEOUT_KEYWORD, cb_strikeout, data,
			  XcDF_SUBSCRIPT_KEYWORD, cb_subscript, data,
			  XcDF_SUPSCRIPT_KEYWORD, cb_supscript, data,
			  XcDF_OUTLINE_KEYWORD, cb_outline, data,
			  XcDF_WIDTH_KEYWORD, cb_width, data,
			  XcDF_SHADOW_KEYWORD, cb_shadow, data,
			  XcDF_SMALL_CAPS_KEYWORD, cb_small_caps, data,
			  XcDF_BIG_CAPS_KEYWORD, cb_big_caps, data,
			  XcDF_NAME_KEYWORD, cb_name, style,
			  XcDF_FONT_KEYWORD, cb_font, data,
			  XcDF_COLOR_KEYWORD, cb_color, data,
			  XcDF_STYLE_KEYWORD, cb_style, data,
			  XcDF_HIDE_KEYWORD, cb_hide, data,
			  XcDF_INHERIT_KEYWORD, cb_inherit, data,
			  NULL)) break;
  doc->data_type = XcDF_NONE;
  if(!F(doc).readReal(doc, &r)) break;
  F(style).set(style, XcTS_SIZE, SCALE_FROM_POINTS(r), XcTS_END);
  if(doc->data_type == XcDF_COLOR_TYPE)
  {
   F(style).set(style, XcTS_COLOR, (c_Color *)doc->data_ptr, XcTS_END);
   doc->data_type = XcDF_NONE;
  }

  if(!F(doc).readString(doc, &ptr)) break;

  if(style->font == NULL)
  {
   Xc_ERROR(("TextStyle `%s' do not have a font", style->name));
   break;
  }
  if(style->color == NULL)
  {
   Xc_ERROR(("TextStyle `%s' do not have a color", style->name));
   break;
  }
  return TRUE;
 } while(0);
 DELETE(c_TextStyle)(style);
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** writeStyle - Write method                                         ** 
** ----------------------------------------------------------------- */
boolean writeStyle(this, doc)
c_TextStyle *this;
c_DocFile *doc;
{
 Xc_HISTORY(("write(`%s')", this->name));

 if(!F(doc).writeKeyword(doc, XcDF_STYLE_KEYWORD, this->id) ||
    !F(doc).startSequence(doc) ||
    !F(doc).writeKeyword(doc, XcDF_FONT_KEYWORD, -1L) ||
    !F(doc).startSequence(doc) ||
    !F(doc).write(doc, this->font->name, strlen(this->font->name))||
    !F(doc).endSequence(doc) ||
    !F(doc).writeKeyword(doc, XcDF_COLOR_KEYWORD, this->color->id) ||
    !F(doc).writeReal(doc, SCALE_TO_POINTS(this->point_size)) ||
    !F(doc).writeKeyword(doc, XcDF_NAME_KEYWORD, -1L) ||
    !F(doc).startSequence(doc) ||
    !F(doc).write(doc, this->name, strlen(this->name)) ||
    !F(doc).endSequence(doc)) return FALSE;

 /*--- Bg Color ---*/
 if (this->bgcolor && !this->bgcolor->transparency)
 {
  if (!F(doc).writeKeyword(doc, XcDF_BGCOLOR_KEYWORD, this->bgcolor->id))
   return FALSE;
 } 
 /*--- Width ---*/
 if((!F(doc).writeKeyword(doc, XcDF_WIDTH_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->width.scale)) ||
     !F(doc).writeBoolean(doc, !(this->attributes & XcTS_B_WIDTH)) ||
     !F(doc).endSequence(doc))) return FALSE;

 /*--- Italic ---*/
 if((!F(doc).writeKeyword(doc, XcDF_ITALIC_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_DEGREES(this->italic.angle)) ||
     !F(doc).writeBoolean(doc, !(this->attributes & XcTS_B_ITALIC)) ||
     !F(doc).endSequence(doc))) return FALSE;
  
 /*--- Underline ---*/
 if((!F(doc).writeKeyword(doc, XcDF_UNDERLINE_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->underline.dy)) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->underline.overstroke)) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->underline.thickness)) ||
     !F(doc).writeKeyword(doc,XcDF_COLOR_KEYWORD, this->underline.color->id)||
     !F(doc).writeInteger(doc, this->underline.type) ||
     !F(doc).writeBoolean(doc, this->underline.overspace) ||
     !F(doc).writeBoolean(doc, !(this->attributes & XcTS_B_UNDERLINE)) ||
     !F(doc).endSequence(doc))) return FALSE;

 /*--- Strikeout ---*/
 if((!F(doc).writeKeyword(doc, XcDF_STRIKEOUT_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->strikeout.dy)) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->strikeout.overstroke)) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->strikeout.thickness)) ||
     !F(doc).writeKeyword(doc,XcDF_COLOR_KEYWORD, this->strikeout.color->id)||
     !F(doc).writeInteger(doc, this->strikeout.type) ||
     !F(doc).writeBoolean(doc, this->strikeout.overspace) ||
     !F(doc).writeBoolean(doc, !(this->attributes & XcTS_B_STRIKEOUT)) ||
     !F(doc).endSequence(doc))) return FALSE;
  
 /*--- Outline ---*/
#if 0
 if((!F(doc).writeKeyword(doc, XcDF_OUTLINE_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->outline.thickness)) ||
     !F(doc).writeKeyword(doc,XcDF_COLOR_KEYWORD, this->outline.color->id)||
     !F(doc).writeBoolean(doc, !(this->attributes & XcTS_B_OUTLINE)) ||
     !F(doc).endSequence(doc))) return FALSE;
#endif

 /*--- Shadow with automatik color generation ---*/
 if((!F(doc).writeKeyword(doc, XcDF_SHADOW_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->shadow.dx)) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->shadow.dy)) ||
     (!this->shadow.color->hide_flag &&
      !F(doc).writeKeyword(doc,XcDF_COLOR_KEYWORD, this->shadow.color->id)) ||
     !F(doc).writeBoolean(doc, !(this->attributes & XcTS_B_SHADOW)) ||
     !F(doc).endSequence(doc))) return FALSE;
  
 /*--- Subscript ---*/
 if((!F(doc).writeKeyword(doc, XcDF_SUBSCRIPT_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->subscript.dy)) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->subscript.scale)) ||
     !F(doc).writeBoolean(doc, !(this->attributes & XcTS_B_SUBSCRIPT)) ||
     !F(doc).endSequence(doc))) return FALSE;

 /*--- Superscript ---*/
 if((!F(doc).writeKeyword(doc, XcDF_SUPSCRIPT_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->supscript.dy)) ||
     !F(doc).writeReal(doc, SCALE_TO_COEF(this->supscript.scale)) ||
     !F(doc).writeBoolean(doc, !(this->attributes & XcTS_B_SUPSCRIPT)) ||
     !F(doc).endSequence(doc))) return FALSE;

 /*--- Small caps ---*/
 if((!F(doc).writeKeyword(doc, XcDF_SMALL_CAPS_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeBoolean(doc, !(this->attributes & XcTS_B_SMALL_CAPS)) ||
     !F(doc).endSequence(doc))) return FALSE;

 /*--- Big caps ---*/
 if((!F(doc).writeKeyword(doc, XcDF_BIG_CAPS_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeBoolean(doc, !(this->attributes & XcTS_B_BIG_CAPS)) ||
     !F(doc).endSequence(doc))) return FALSE;

 /*--- inheritance ---*/
 if (this->inherit_from)
 {
  if((!F(doc).writeKeyword(doc, XcDF_INHERIT_KEYWORD, -1L) ||
      !F(doc).startSequence(doc) ||
      !F(doc).writeKeyword(doc, XcDF_STYLE_KEYWORD, this->inherit_from->id) ||
      !F(doc).writeInteger(doc, this->valid_field) ||
      !F(doc).endSequence(doc))) return FALSE;
 }
 
 if(this->hide_flag != FALSE)
  if(!F(doc).writeKeyword(doc, XcDF_HIDE_KEYWORD, -1L)) return FALSE;
  
 return F(doc).endSequence(doc);
}

typedef struct small_list s_small_list;

struct	small_list
{
 c_TextStyle		*ts;
 struct small_list	*next;
};

/* ----------------------------------------------------------------- ** 
** writeStyleList - Write the TextStyle list                         ** 
** ----------------------------------------------------------------- */
boolean writeStyleList(this, doc)
c_TextStyle *this;
c_DocFile *doc;
{
 c_TextStyle *ptr;
 
 s_small_list *list;
 s_small_list **cur_list;
 s_small_list *list_delayed;
 s_small_list **cur_delayed;
 s_small_list *t_list;
 long id;

 Xc_HISTORY(("writeList"));

 if(!F(doc).writeKeyword(doc, XcDF_STYLELIST_KEYWORD, -1L) ||
    !F(doc).startSequence(doc)) return FALSE;

 list = NULL;
 list_delayed = NULL;
 cur_list = &list;
 cur_delayed = &list_delayed;
 ptr = *this->base;
 while (ptr)
 {
  if (ptr->inherit_from)
  {
   *cur_delayed = Xc_malloc("sld", sizeof(s_small_list));
   (*cur_delayed)->ts = ptr;
   cur_delayed = &((*cur_delayed)->next);
   *cur_delayed = NULL;
  }
  else
  {
   *cur_list = Xc_malloc("sld", sizeof(s_small_list));
   (*cur_list)->ts = ptr;
   cur_list = &((*cur_list)->next);
   *cur_list = NULL;
  }
  ptr = ptr->next;
 }

 id = 0;
 while (list)
 {
  t_list = list;  
  list->ts->id = id;
  if(!F(list->ts).write(list->ts, doc)) return FALSE;
  list = list->next;
  Xc_free(t_list);
  id++;
 }

 list = list_delayed;
 while (list)
 {
  t_list = list;  
  list->ts->id = id;
  if(!F(list->ts).write(list->ts, doc)) return FALSE;
  list = list->next;
  Xc_free(t_list);
  id++;
 }
/*
 for(ptr = *this->base, id = 0; ptr != NULL; ptr = ptr->next, id++)
 {
  ptr->id = id;
  if(!F(ptr).write(ptr, doc)) return FALSE;
 }
*/
 return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** cb_name - Name keyword loading callback                           ** 
** ----------------------------------------------------------------- */
static boolean cb_name(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_TextStyle *this;
{
 unsigned char *name;

 Xc_TRACE(("cb_name"));
 if(!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  F(this).set(this, XcTS_NAME, name, XcTS_END);
  Xc_free(name);
 }
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_bgcolor - Background Color loading callback                    ** 
** ----------------------------------------------------------------- */
static boolean cb_bgcolor(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 c_Color *ptr;
 
 Xc_TRACE(("cb_bgcolor(%ld)", param));
  
 if(param < 0) param = 0;
 for(ptr = data->color_base; ptr != NULL; ptr = ptr->next)
 {
  if(ptr->id == param)
  {
   F(data->style).set(data->style,
		      XcTS_BGCOLOR, (c_Color *)ptr, XcTS_END);
   return TRUE;
  }
 }

 Xc_ERROR(("Bad color #%ld", param));
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** cb_italic - Italic loading callback                               ** 
** ----------------------------------------------------------------- */
static boolean cb_italic(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 real		r;
 boolean	is_unset;

 Xc_TRACE(("cb_italic"));
 if(!F(doc).readReal(doc, &r) ||
    !F(doc).readBoolean(doc, &is_unset)) 
  return FALSE;
 if (!is_unset)
 {
  F(data->style).set(data->style,
		     XcTS_ITALIC_ON, SCALE_FROM_DEGREES(r), XcTS_END);
 }
 /* else todo - parameter is not used right now */
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_underline - Underline loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_underline(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 real		a, b, c;
 long		int_a;
 boolean	b_over;
 c_Color	*col;
 boolean	is_unset;
 
 Xc_TRACE(("cb_underline"));
 doc->data_type = XcDF_NONE;
 if(!F(doc).readReal(doc, &a) || !F(doc).readReal(doc, &b) ||
    !F(doc).readReal(doc, &c)) return FALSE;

 if(!F(doc).readInteger(doc, &int_a)) return FALSE;
 if(doc->data_type == XcDF_COLOR_TYPE)
 {
  col = (c_Color *)doc->data_ptr;
  doc->data_type = XcDF_NONE;
  doc->data_ptr = NULL;
 }
 else
  col = NULL;
 if(!F(doc).readBoolean(doc, &b_over) ||
    !F(doc).readBoolean(doc, &is_unset)) return FALSE;
 F(data->style).set(data->style, XcTS_UNDERLINE_CONF,
		    SCALE_FROM_COEF(a), SCALE_FROM_COEF(b),SCALE_FROM_COEF(c),
		    b_over, col, 
		    (enum textstyle_underline_e)int_a, XcTS_END);
 if (!is_unset)
  F(data->style).set(data->style, XcTS_UNDERLINE_ON, XcTS_END);
 doc->data_type = XcDF_NONE;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_strikeout - Underline loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_strikeout(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 real		a, b, c;
 long		int_a;
 boolean	b_over;
 c_Color	*col;
 boolean	is_unset;
 
 Xc_TRACE(("cb_strikeout"));
 doc->data_type = XcDF_NONE;
 if(!F(doc).readReal(doc, &a) || !F(doc).readReal(doc, &b) ||
    !F(doc).readReal(doc, &c)) return FALSE;

 if(!F(doc).readInteger(doc, &int_a)) return FALSE;
 if(doc->data_type == XcDF_COLOR_TYPE)
 {
  col = (c_Color *)doc->data_ptr;
  doc->data_type = XcDF_NONE;
  doc->data_ptr = NULL;
 }
 else
  col = NULL;
 if(!F(doc).readBoolean(doc, &b_over) ||
    !F(doc).readBoolean(doc, &is_unset)) return FALSE;
 F(data->style).set(data->style, XcTS_STRIKEOUT_CONF,
		    SCALE_FROM_COEF(a), SCALE_FROM_COEF(b),SCALE_FROM_COEF(c),
		    b_over, col, 
		    (enum textstyle_underline_e)int_a, XcTS_END);
 if (!is_unset)
  F(data->style).set(data->style, XcTS_STRIKEOUT_ON, XcTS_END);
 doc->data_type = XcDF_NONE;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_subscript - Subscript loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_subscript(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 real		a, b;
 boolean	is_unset;

 Xc_TRACE(("cb_subscript"));
 if(!F(doc).readReal(doc, &a) || !F(doc).readReal(doc, &b) ||
    !F(doc).readBoolean(doc, &is_unset)) return FALSE;
 F(data->style).set(data->style, XcTS_SUBSCRIPT_CONF, 
		    SCALE_FROM_COEF(a), SCALE_FROM_COEF(b),
		    XcTS_END);
 if (!is_unset)
  F(data->style).set(data->style, XcTS_SUBSCRIPT_ON, XcTS_END); 
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_supscript - Subscript loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_supscript(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 real		a, b;
 boolean	is_unset;

 Xc_TRACE(("cb_supscript"));
 if(!F(doc).readReal(doc, &a) || !F(doc).readReal(doc, &b) ||
    !F(doc).readBoolean(doc, &is_unset)) return FALSE;
 F(data->style).set(data->style, XcTS_SUPSCRIPT_CONF, 
		    SCALE_FROM_COEF(a), SCALE_FROM_COEF(b),
		    XcTS_END);
 if (!is_unset)
  F(data->style).set(data->style, XcTS_SUPSCRIPT_ON, XcTS_END); 
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_outline - Outline loading callback                             ** 
** ----------------------------------------------------------------- */
static boolean cb_outline(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
/* todo - not active right now */
#if 0
 real		a;
 c_Color	*col;
 unsigned char	*ptr;
  
 Xc_TRACE(("cb_outline"));
 doc->data_type = XcDF_NONE;
 if(!F(doc).readReal(doc, &a)) return FALSE;
 if(!F(doc).readString(doc, &ptr)) return FALSE;
 if(doc->data_type == XcDF_COLOR_TYPE)
 {
  col = (c_Color *)doc->data_ptr;
  doc->data_type = XcDF_NONE;
  doc->data_ptr = NULL;
 }
 F(data->style).set(data->style, XcTS_OUTLINE_ON,
		    SCALE_FROM_COEF(a), col, XcTS_END);
 doc->data_type = XcDF_NONE;
#endif
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_width - Width loading callback                                 ** 
** ----------------------------------------------------------------- */
static boolean cb_width(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 real		a;
 boolean	is_unset;

 Xc_TRACE(("cb_width"));
 if(!F(doc).readReal(doc, &a) ||
    !F(doc).readBoolean(doc, &is_unset)) return FALSE;
 F(data->style).set(data->style, XcTS_WIDTH_CONF, 
		    SCALE_FROM_COEF(a), XcTS_END);
 if (!is_unset)
  F(data->style).set(data->style, XcTS_WIDTH_ON, XcTS_END); 
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_shadow - Shadow loading callback                               ** 
** ----------------------------------------------------------------- */
static boolean cb_shadow(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 real		a, b;
 boolean	is_unset;
 c_Color	*col;
  
 Xc_TRACE(("cb_shadow"));

 if(!F(doc).readReal(doc, &a) || !F(doc).readReal(doc, &b))
  return FALSE;
 if(!F(doc).readBoolean(doc, &is_unset)) return FALSE;
 if(doc->data_type == XcDF_COLOR_TYPE)
 {
  col = (c_Color *)doc->data_ptr;
  doc->data_type = XcDF_NONE;
  doc->data_ptr = NULL;
 }
 else
  col = NULL;
 F(data->style).set(data->style, XcTS_SHADOW_CONF, 
		    SCALE_FROM_COEF(a),SCALE_FROM_COEF(b), col,
		    XcTS_END);
 if (!is_unset)
  F(data->style).set(data->style, XcTS_SHADOW_ON, XcTS_END);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_color - Color loading callback                                 ** 
** ----------------------------------------------------------------- */
static boolean cb_color(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 c_Color *ptr;
 
 Xc_TRACE(("cb_color(%ld)", param));
  
 if(param < 0) param = 0;
 for(ptr = data->color_base; ptr != NULL; ptr = ptr->next)
 {
  if(ptr->id == param)
  {
   doc->data_type = XcDF_COLOR_TYPE;
   doc->data_ptr = ptr;
   return TRUE;
  }
 }

 Xc_ERROR(("Bad color #%ld", param));
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_font - Font loading callback                                   ** 
** ----------------------------------------------------------------- */
static boolean cb_font(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 unsigned char *name;
 c_VectorFont *font;
 long len;

 do {
     if(!F(doc).readWord(doc, &name, &len)) return FALSE;
    } while(len == 0);
 if(len < 0) return FALSE;

 if((font = F(data->font_base).getFontByName(data->font_base, name)) == NULL)
  font = F(data->font_base).selectDialog(data->font_base, name);

 Xc_free(name);
 if(font == NULL) return FALSE;
 F(data->style).set(data->style, XcTS_FONT, font, XcTS_END);
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_hide - Hide loading callback                                   ** 
** ----------------------------------------------------------------- */
static boolean cb_hide(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 Xc_TRACE(("cb_hide"));
 F(data->style).set(data->style, XcTS_HIDE_ON, XcTS_END);
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_small_caps - Small_Caps Loading callback                       ** 
** ----------------------------------------------------------------- */
static boolean cb_small_caps(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 boolean	is_unset;
 Xc_TRACE(("cb_small_caps"));
 if(!F(doc).readBoolean(doc, &is_unset)) return FALSE;
 if (!is_unset)
  F(data->style).set(data->style, XcTS_SMALL_CAPS_ON, XcTS_END);
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_big_caps - Big_Caps Loading callback                           ** 
** ----------------------------------------------------------------- */
static boolean cb_big_caps(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 boolean	is_unset;
 Xc_TRACE(("cb_big_caps"));
 if(!F(doc).readBoolean(doc, &is_unset)) return FALSE;
 if (!is_unset)
  F(data->style).set(data->style, XcTS_BIG_CAPS_ON, XcTS_END);
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_inherit - Inheritance Loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_inherit(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 long	is_valid;
 Xc_TRACE(("cb_hinerit"));
 if(!F(doc).readInteger(doc, &is_valid)) return FALSE;
 if(doc->data_type == XcDF_STYLE_TYPE)
 {
  F(data->style).set(data->style,
		     XcTS_INHERIT, 
		     (c_TextStyle *)doc->data_ptr, (unsigned int)is_valid,
		     XcTS_END);
  doc->data_type = XcDF_NONE;
  doc->data_ptr = NULL;
 }

 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_style - Style loading callback                                 ** 
** ----------------------------------------------------------------- */
static boolean cb_style(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
textstyle_read_t *data;
{
 c_TextStyle *ptr;
 
 Xc_TRACE(("cb_style(%ld)", param));
  
 if(param < 0) param = 0;
 for(ptr = *(data->style_base); ptr != NULL; ptr = ptr->next)
 {
  if(ptr->id == param)
  {
   doc->data_type = XcDF_STYLE_TYPE;
   doc->data_ptr = ptr;
   return TRUE;
  }
 }

 Xc_ERROR(("Bad style #%ld", param));
 return FALSE;
}
