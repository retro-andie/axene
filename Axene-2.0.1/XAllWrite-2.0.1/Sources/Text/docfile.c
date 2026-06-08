/*
** docfile.c for XAllWrite in Text/
** Text Read and Write functions.
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
** Started on  Tue Nov  4 18:20:13 1997 Emmanuel Paris
** Last update Mon Jul 20 15:32:24 1998 Emmanuel Paris
*/

#include "TextP.h"

boolean readText ___PROTO((c_DocFile *doc, char *keyword,
				  long param, text_read_t *data));
boolean writeText ___PROTO((c_Text *this, c_DocFile *doc));
boolean readTextList ___PROTO((c_DocFile *doc, char *keyword,
				  long param, BaseStd_t *base));
boolean writeTextList ___PROTO((c_Text *this, c_DocFile *doc));

/*--- read callbacks ---*/
static boolean cb_body ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_Text *text));
static boolean cb_clipboard ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_Text *text));
static boolean cb_header ___PROTO((c_DocFile *doc, char *keyword,
				   long param, c_Text *text));
static boolean cb_footer ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_Text *text));
static boolean cb_footnotes ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_Text *text));
static boolean cb_style ___PROTO((c_DocFile *doc, char *keyword,
				  long param, text_read_t *data));
static boolean cb_ruler ___PROTO((c_DocFile *doc, char *keyword,
				  long param, text_read_t *data));
static boolean cb_xa_begin ___PROTO((c_DocFile *doc, char *keyword,
				     long param, text_read_t *data));
static boolean cb_xa_end ___PROTO((c_DocFile *doc, char *keyword,
				   long param, text_read_t *data));
static boolean cb_xa_text ___PROTO((c_DocFile *doc, char *keyword,
				    long param, text_read_t *data));
static boolean cb_xa_linefeed ___PROTO((c_DocFile *doc, char *keyword,
					long param, text_read_t *data));
static boolean cb_xa_tab ___PROTO((c_DocFile *doc, char *keyword,
				   long param, text_read_t *data));
static boolean cb_xa_pgbrk ___PROTO((c_DocFile *doc, char *keyword,
				   long param, text_read_t *data));
static boolean cb_selection ___PROTO((c_DocFile *doc, char *keyword,
				      long param, text_read_t *data));

/*--- external declarations ---*/
extern xa_item_t *create_item ___PROTO((c_Text *This, int type,
					void *data, xa_item_t *item));
extern void purge_changed_items ___PROTO((c_Text *This));

/* ----------------------------------------------------------------- ** 
** readTextList - Text List reading                                  ** 
** ----------------------------------------------------------------- */
boolean readTextList(doc, keyword, dummy, base)
c_DocFile *doc;
char *keyword;
long dummy;
BaseStd_t *base;
{
 text_read_t param;
 
 Xc_TRACE(("readList"));
 
 param.base = base;

 if(!F(doc).addCallbacks(doc, XcDF_TEXT_KEYWORD, readText, &param, NULL))
  return FALSE;
 return F(doc).expectKeyword(doc, XcDF_TEXT_KEYWORD, TRUE);
}

/* ----------------------------------------------------------------- ** 
** readText - Text reading                                           ** 
** ----------------------------------------------------------------- */
boolean readText(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_read_t *data;
{
 c_Text	*text;
 xa_item_t *item;
 int i;

 text = (c_Text *)NEW(c_Text)(data->base, Xa_BODY);
 data->text = text;
 data->style = NULL;
 data->ruler = NULL;
 data->item = text->item_base;
 data->sstart = data->send = 0;

 do
 {
  if(!F(doc).addCallbacks(doc,
			  XcDF_TT_BODY_KEYWORD, cb_body, text,
			  XcDF_TT_CLIPBOARD_KEYWORD, cb_clipboard, text,
			  XcDF_TT_HEADER_KEYWORD, cb_header, text,
			  XcDF_TT_FOOTER_KEYWORD, cb_footer, text,
			  XcDF_TT_FOOTNOTES_KEYWORD, cb_footnotes, text,
			  XcDF_STYLE_KEYWORD, cb_style, data,
			  XcDF_RULER_KEYWORD, cb_ruler, data,
			  XcDF_XA_BEGIN_KEYWORD, cb_xa_begin, data,
			  XcDF_XA_END_KEYWORD, cb_xa_end, data,
			  XcDF_XA_TEXT_KEYWORD, cb_xa_text, data,
			  XcDF_XA_LINEFEED_KEYWORD, cb_xa_linefeed, data,
			  XcDF_XA_TAB_KEYWORD, cb_xa_tab, data,
			  XcDF_XA_PAGEBRK_KEYWORD, cb_xa_pgbrk, data,
			  XcDF_SELECTION_KEYWORD, cb_selection, data,
			  NULL)) break;
  
  if(!F(doc).expectKeyword(doc, NULL, TRUE)) break;

  text->id = param;
  item = text->item_base;
  if (data->sstart > data->send) break;

  for(i = 0; i < data->sstart; i++)
  {
   if (!item) break;
   item = item->any.next;
  }
  
  if (!item) break;
  text->select_start = item;
  if (item->type == XA_TEXT && item->text.text_len < text->select_start_pos)
   break;
  
  for(; i < data->send; i++)
  {
   if (!item) break;
   item = item->any.next;
  }

  if (item->type == XA_TEXT && item->text.text_len < text->select_end_pos)
   break;

  text->start_end = FALSE;
  F(text).goTo(text, XA_GLOBAL, XA_RIGHT, 0, item, text->select_end_pos, TRUE);
  
  return TRUE;
 } while(0);

 DELETE(c_Text)(text);
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** writeTextList - Text List saving                                  ** 
** ----------------------------------------------------------------- */
boolean writeTextList(this, doc)
c_Text *this;
c_DocFile *doc;
{
 int id = 0;

 if(!F(doc).writeKeyword(doc, XcDF_TEXTLIST_KEYWORD, -1L) ||
    !F(doc).startSequence(doc)) return FALSE;
 
 while(this)
 {
  this->id = id++;
  writeText(this, doc);
  this = this->next;
 }
 
 return F(doc).endSequence(doc);
}

/* ----------------------------------------------------------------- ** 
** writeText - Text saving                                           ** 
** ----------------------------------------------------------------- */
boolean writeText(this, doc)
c_Text *this;
c_DocFile *doc;
{
 xa_item_t	*item;
 c_TextStyle	*style;
 c_TextRuler	*ruler;
 int		i, sstart, send;
 char		*ttype;

 if(!F(doc).writeKeyword(doc, XcDF_TEXT_KEYWORD, this->id) ||
    !F(doc).startSequence(doc)) return FALSE;
 
 switch(this->type)
 {
 default:
 case Xa_BODY:
  ttype = XcDF_TT_BODY_KEYWORD;
  break;
 case Xa_CLIPBOARD:
  ttype = XcDF_TT_CLIPBOARD_KEYWORD;
  break;
 case Xa_HEADER:
  ttype = XcDF_TT_HEADER_KEYWORD;
  break;
 case Xa_FOOTER:
  ttype = XcDF_TT_FOOTER_KEYWORD;
  break;
 case Xa_FOOTNOTES:
  ttype = XcDF_TT_FOOTNOTES_KEYWORD;
  break;
 }
 
 if(!F(doc).writeKeyword(doc, ttype, -1L) ||
    !F(doc).startSequence(doc) ||
    !F(doc).write(doc, this->name, strlen(this->name)) ||
    !F(doc).endSequence(doc))
  return FALSE;

 item = this->item_base;
 style = item->any.style;
 ruler = item->linefeed.ruler;
 
 if (!F(doc).writeKeyword(doc, XcDF_STYLE_KEYWORD, style->id) ||
     !F(doc).writeKeyword(doc, XcDF_RULER_KEYWORD, ruler->id)) return FALSE;
 
 i = sstart = send = 0;
 while(item)
 {

  if (item->any.style != style)
  {
   style = item->any.style;
   if (!F(doc).writeKeyword(doc, XcDF_STYLE_KEYWORD, style->id)) return FALSE;
  }
  
  switch(item->type)
  {
  case XA_BEGIN:
   if (!F(doc).writeKeyword(doc, XcDF_XA_BEGIN_KEYWORD, -1L)) return FALSE;
   break;
  case XA_END:
   if (!F(doc).writeKeyword(doc, XcDF_XA_END_KEYWORD, -1L)) return FALSE;
   break;
  case XA_TEXT:
   if (!F(doc).writeKeyword(doc, XcDF_XA_TEXT_KEYWORD, -1L) ||
       !F(doc).startSequence(doc) ||
       !F(doc).write(doc, item->text.text, item->text.text_len) ||
       !F(doc).endSequence(doc)) return FALSE;
   break;
  case XA_LINEFEED:
   if (item->linefeed.ruler != ruler)
   {
    ruler = item->linefeed.ruler;
    if (!F(doc).writeKeyword(doc, XcDF_RULER_KEYWORD, ruler->id))
     return FALSE;
   }
   if (!F(doc).writeKeyword(doc, XcDF_XA_LINEFEED_KEYWORD, -1L))
    return FALSE;   
   break;
  case XA_TAB:
   if (!F(doc).writeKeyword(doc, XcDF_XA_TAB_KEYWORD, -1L)) return FALSE;
   break;
  case XA_PAGEBREAK:
   if (!F(doc).writeKeyword(doc, XcDF_XA_PAGEBRK_KEYWORD, -1L)) return FALSE;
   break;
  default:
   break;
  }
  
  if (item == this->select_start) sstart = i;
  if (item == this->select_end) send = i;
  
  i++;
  item = item->any.next;
 }

 if (!F(doc).writeKeyword(doc, XcDF_SELECTION_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeInteger(doc, sstart) ||
     !F(doc).writeInteger(doc, this->select_start_pos) ||
     !F(doc).writeInteger(doc, send) ||
     !F(doc).writeInteger(doc, this->select_end_pos) ||
     !F(doc).endSequence(doc)) return FALSE;
     
 
 return F(doc).endSequence(doc);
}

/*--- read callbacks ---*/
static boolean cb_body(doc, keyword, param, text)
c_DocFile *doc;
char *keyword;
long param;
c_Text *text;
{
 unsigned char *name;

 Xc_TRACE(("cb_body"));
 text->type = Xa_BODY;
 
 if (!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  strcpy(text->name, name);
  Xc_free(name);
 }
 else
  strcpy(text->name, "None");
 return TRUE;
}

static boolean cb_clipboard(doc, keyword, param, text)
c_DocFile *doc;
char *keyword;
long param;
c_Text *text;
{
 unsigned char *name;

 Xc_TRACE(("cb_clipboard"));
 text->type = Xa_CLIPBOARD;
 
 if (!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  strcpy(text->name, name);
  Xc_free(name);
 }
 else
  strcpy(text->name, "None");
 return TRUE;
}

static boolean cb_header(doc, keyword, param, text)
c_DocFile *doc;
char *keyword;
long param;
c_Text *text;
{
 unsigned char *name;

 Xc_TRACE(("cb_header"));
 text->type = Xa_HEADER;
 
 if (!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  strcpy(text->name, name);
  Xc_free(name);
 }
 else
  strcpy(text->name, "None");
 return TRUE;
}

static boolean cb_footer(doc, keyword, param, text)
c_DocFile *doc;
char *keyword;
long param;
c_Text *text;
{
 unsigned char *name;

 Xc_TRACE(("cb_footer"));
 text->type = Xa_FOOTER;
 
 if (!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  strcpy(text->name, name);
  Xc_free(name);
 }
 else
  strcpy(text->name, "None");
 return TRUE;
}

static boolean cb_footnotes(doc, keyword, param, text)
c_DocFile *doc;
char *keyword;
long param;
c_Text *text;
{
 unsigned char *name;

 Xc_TRACE(("cb_footnotes"));
 text->type = Xa_FOOTNOTES;
 
 if (!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  strcpy(text->name, name);
  Xc_free(name);
 }
 else
  strcpy(text->name, "None");
 return TRUE;
}

static boolean cb_style(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_read_t *data;
{
 c_TextStyle *style;
 
 Xc_TRACE(("cb_style"));
 
 style = data->base->text_style_base;
 while(style->id != param)
  style = style->next;

 if (style)
  data->text->style = data->style = style;
 else
  return FALSE;
 return TRUE;
}

static boolean cb_ruler(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_read_t *data;
{
 c_TextRuler *ruler;
 
 Xc_TRACE(("cb_ruler"));
 
 ruler = data->base->text_ruler_base;
 while(ruler->id != param)
  ruler = ruler->next;

 if (ruler)
  data->text->ruler = data->ruler = ruler;
 else
  return FALSE;

 return TRUE;
}

static boolean cb_xa_begin(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_read_t *data;
{
 Xc_TRACE(("cb_xa_begin"));

 if (data->item != data->text->item_base || 
     !data->style || !data->ruler)
  return FALSE;
 
 F(data->item->linefeed.style).markUnused(data->item->linefeed.style);
 F(data->item->linefeed.ruler).markUnused(data->item->linefeed.ruler);
 data->item->linefeed.style = data->style;
 data->item->linefeed.ruler = data->ruler;
 F(data->style).markUsed(data->style);
 F(data->ruler).markUsed(data->ruler);
 
 return TRUE;
}

static boolean cb_xa_end(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_read_t *data;
{
 xa_item_t *item;
 Xc_TRACE(("cb_xa_end"));

 item = data->item->any.next;
 if (item->type != XA_END) return FALSE;
 
 F(data->item->any.style).markUnused(item->any.style);
 item->any.style = data->style;
 F(data->style).markUsed(data->style);
 
 return TRUE;
}

static boolean cb_xa_text(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_read_t *data;
{
 unsigned char *txt;

 Xc_TRACE(("cb_xa_text"));
 if (!F(doc).readString(doc, &txt)) return FALSE;
 if(txt != NULL)
 {
  data->item = create_item(data->text, XA_TEXT, txt, data->item); 
  Xc_free(txt);
 }
 
 return TRUE;
}

static boolean cb_xa_linefeed(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_read_t *data;
{
 Xc_TRACE(("cb_xa_linefeed"));
 
 data->item = create_item(data->text, XA_LINEFEED, NULL, data->item); 
 return TRUE;
}

static boolean cb_xa_tab(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_read_t *data;
{
 Xc_TRACE(("cb_xa_tab"));
 
 data->item = create_item(data->text, XA_TAB, NULL, data->item); 
 return TRUE;
}

static boolean cb_xa_pgbrk(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_read_t *data;
{
 Xc_TRACE(("cb_xa_pgbrk"));
 
 data->item = create_item(data->text, XA_PAGEBREAK, NULL, data->item); 
 return TRUE;
}

static boolean cb_selection(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_read_t *data;
{
 long val[4];
 
 Xc_TRACE(("cb_selection"));

 if (!F(doc).readInteger(doc, &val[0]) ||
     !F(doc).readInteger(doc, &val[1]) ||
     !F(doc).readInteger(doc, &val[2]) ||
     !F(doc).readInteger(doc, &val[3])) return FALSE;

 data->sstart = (int)val[0];
 data->text->select_start_pos = (int)val[1];
 data->send = (int)val[2];
 data->text->select_end_pos = (int)val[3];
 
 return TRUE;
}
