/*
** TextStyle.c for Xclamation, XQuad and XAllWrite in TextStyle/
** Methods for the TextStyle class
**
** Copyright (C) 1994-2000 Axene.
** Authors: St�phane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
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
** Started on  Sun Jun 12 02:10:50 1994 St�phane Boisson
** Last update Fri Aug 14 17:20:40 1998 Emmanuel Paris
*/

/*#define NTRACE*/

/*--- include ---*/
#include "TextStyle.h"
#include <stdarg.h>
#include "char_translate.h"

/*--- external ---*/
extern c_VectorFont	*GlobFontBase;

/*--- Prototypes ---*/
static void *contructor();
static void destructor();
static void *copy();
static void set_params ___PROTO((c_TextStyle *style, ...));
static boolean compare ___PROTO((c_TextStyle *a, c_TextStyle *b,
				 boolean with_name));
static void merge ___PROTO((c_TextStyle *source, c_TextStyle **target,
			    boolean fusion));
static c_TextStyle *virtualCopy ___PROTO((c_TextStyle *this, BaseStd_t *base));

static void vset ___PROTO((c_TextStyle *style, va_list ap));
static void hook_color ___PROTO((c_TextStyle *this, int reason,
				 c_Color *old, c_Color *new));
static boolean ps_print ___PROTO((c_TextStyle *this, c_PostScript *post));

static void reactualize ___PROTO((c_TextStyle *style));

static c_TextStyle *getStyle ___PROTO((c_TextStyle *style,
				  BaseStd_t *base, ...));

static boolean ps_print_text ___PROTO((c_TextStyle *this, char *text, int len,
				       c_PostScript *post,
				       coord_t c_x, coord_t c_y,
				       char *fput));
static void markUsed ___PROTO((c_TextStyle *This));
static void markUnused ___PROTO((c_TextStyle *This));

/*--- internal functions ---*/

static void bold_on();
static void bold_off();
static void italic_on();
static void italic_off();
static c_Color	*compute_shadow_color();
static void hook_inherit ___PROTO((c_TextStyle *this, int reason,
				   c_TextStyle *old, c_TextStyle *new));
static void big_caps();
static void small_caps();
static void underline();
static void strikeout();
static void put_font();

/*--- fill object method ---*/
sf_TextStyle fc_TextStyle =
{
 contructor, destructor, copy,
 (c_TextStyle *(*) ___PROTO((c_TextStyle *style, 
			     BaseStd_t *base, ...)))getStyle,
 (void (*) ___PROTO((c_TextStyle *style, ...)))set_params,
 compare, merge, virtualCopy,
 markUsed, markUnused,
 readStyle, 
 writeStyle, writeStyleList,
 ps_print, ps_print_text, 
};

/*--- global var ---*/
static ItemHandle *i_handle = NULL;
static long i_references = 0;

/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *contructor(base, name, font, height, color)
c_TextStyle **base;
char *name;
c_VectorFont *font;
coord_t height;
c_Color *color;
{
 c_TextStyle *p, *tstyle;
 c_Color *transpc;

 Xc_HISTORY(("constructor(`%s')", name? name : ""));
 if(i_handle == NULL)
  i_handle = Xc_createItem("TextStyle base", 16, sizeof(c_TextStyle));
 if(i_handle == NULL) return NULL;

 if((p = Xc_new(i_handle)) == NULL) return NULL;
 memset(p, 0, sizeof(c_TextStyle));
 if((p->hook = NEW(c_Hook)(p, 8)) == NULL)
 {
  Xc_delete(i_handle, p);
  return NULL;
 }
 i_references++;
 p->f = &fc_TextStyle;
  
 /*--- Insert in linked list ---*/
 if(*base == NULL)
 {
  p->next = p->previous = NULL;
  *base = p;
 }
 else
 {
  tstyle = *base;
  while(tstyle->next)
   tstyle = tstyle->next;
    
  p->next = NULL;
  p->previous = tstyle;
  tstyle->next = p;
 }
 p->base = base;
 Xc_TRACE(("end link"));

 p->is_delayed = FALSE;

 p->point_size = height? (height & COORD_FONTSIZE_MASK) : COORD_ONE;
 if(name != NULL) 
 {
  strncpy(p->name, name, XcMAX_NAME_LENGTH - 1);
  p->hide_flag = FALSE;
 }
 else
 {
  p->name[0] = '\0';
  p->hide_flag = TRUE;
 }
 p->font = font;
 p->color = color;
 
 if (color)
  transpc = F(color).getColor(color->base, 
			      XcC_TRANSPARENCY, XcC_SCALE_COLOR(1.0), 
			      XcC_GRAY, 0, XcC_END);
 else
  transpc = NULL;

 p->bgcolor = transpc;

 if (font)
 {
  if (font->specific->is_italic || font->specific->is_bold_italic)
   p->attributes |= XcTS_B_ITALIC;
  else
   p->attributes &= ~XcTS_B_ITALIC;

  if (font->specific->is_bold || font->specific->is_bold_italic)
   p->attributes |= XcTS_B_BOLD;
  else
   p->attributes &= ~XcTS_B_BOLD;
 }

 /*___Init y-offset___*/
 p->subscript.dy = SCALE_FROM_COEF(-0.3);
 p->subscript.scale = SCALE_FROM_COEF(0.6);

 /*___Init y-offset___*/
 p->supscript.dy = SCALE_FROM_COEF(0.4);
 p->supscript.scale = SCALE_FROM_COEF(0.6);

 /*___Init italic___*/
 p->italic.angle = SCALE_FROM_DEGREES(0.0);

 /*___Init weight___*/
 p->width.scale = SCALE_FROM_COEF(0.8);

 /*___Init underline___*/
 p->underline.dy = SCALE_FROM_COEF(0.2);
 p->underline.overstroke = SCALE_FROM_COEF(0);
 p->underline.thickness = SCALE_FROM_COEF(0.1);
 p->underline.overspace = FALSE;
 p->underline.color = color;
 p->underline.type = XcTS_SIMPLE;

 /*___Init strikeout___*/
 p->strikeout.dy = SCALE_FROM_COEF(-0.5);
 p->strikeout.overstroke = SCALE_FROM_COEF(0.3);
 p->strikeout.thickness = SCALE_FROM_COEF(0.1);
 p->strikeout.overspace = TRUE;
 p->strikeout.color = color;
 p->strikeout.type = XcTS_SIMPLE;

 /*___Init outline___*/
 p->outline.thickness = SCALE_FROM_COEF(0);
 p->outline.color = color;

 /*___Init shadow___*/
 p->shadow.dx = SCALE_FROM_COEF(0.1);
 p->shadow.dy = SCALE_FROM_COEF(0.1);
 p->shadow.color = color;

 if(((color != NULL) &&
    (!XcH_REGISTER(color, hook_color, p) ||/* register as many time as */
     !XcH_REGISTER(color, hook_color, p) ||/* color is used */
     !XcH_REGISTER(color, hook_color, p) || 
     !XcH_REGISTER(color, hook_color, p) ||
     !XcH_REGISTER(color, hook_color, p))) ||
    ((transpc != NULL) && !XcH_REGISTER(transpc, hook_color, p)))
 {
  DELETE(c_TextStyle)(p);
  return NULL;
 }

 if(font != NULL)
 {
  Xc_TRACE(("Font ok"));
  if(!F(font).load(font))
  {
   DELETE(c_TextStyle)(p);
   return NULL;
  }
  XcH_REGISTER(font, NULL, p);

  p->metrics = COPY(c_CharMetrics)(font->metrics, NULL, p->point_size,
				   SCALE_ONE, SCALE_FROM_DEGREES(0.0),
				   XcTS_TO_CM(p->attributes), TRUE);
  
  if(p->metrics == NULL)
  {
   DELETE(c_TextStyle)(p);
   return NULL;
  }
  reactualize(p);
 }
 else
  p->metrics = NULL;

 p->used_count = 0;
 Xc_TRACE(("constructor end"));
 return p;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_TextStyle *this;
{
 Xc_HISTORY(("destructor(`%s')", this->name));
 Xc_ASSERT(i_references > 0);

 /*--- Remove from the linked list ---*/
 if(this->next != NULL)
  this->next->previous = this->previous;
 if(this->previous != NULL)
  this->previous->next = this->next;
 if(*(this->base) == this)
 {
  Xc_ASSERT(this->previous == NULL);
  *(this->base) = this->next;
 }

 /*--- reinsert to the end ---*/
 if (this->hook->count && !this->is_delayed)
 {
  c_TextStyle *list;
  list = this;
  while (list->next)
  {
   list = list->next;
  }
  list->next = this;
  this->is_delayed = TRUE;
  this->next = NULL;
  return;
 }

 /*--- Color hooks ---*/
 if(this->color != NULL)
  XcH_UNREGISTER(this->color, hook_color, this);

 if(this->bgcolor != NULL)
  XcH_UNREGISTER(this->bgcolor, hook_color, this);

 if(this->font != NULL)
  XcH_UNREGISTER(this->font, NULL, this);

 if(this->underline.color)
  XcH_UNREGISTER(this->underline.color, hook_color, this);

 if(this->strikeout.color)
  XcH_UNREGISTER(this->strikeout.color, hook_color, this);

 if(this->outline.color)
  XcH_UNREGISTER(this->outline.color, hook_color, this);

 if(this->shadow.color)
  XcH_UNREGISTER(this->shadow.color, hook_color, this);
 
 /*--- Style inheritance hook ---*/
 if(this->inherit_from)
  XcH_UNREGISTER(this->inherit_from, hook_inherit, this);

 /*--- Style hook ---*/
 DELETE(c_Hook)(this->hook);

 if(this->metrics != NULL) DELETE(c_CharMetrics)(this->metrics);
 Xc_delete(i_handle, this);
 if(--i_references == 0)
 {
  Xc_destroyItem(i_handle);
  i_handle = NULL;
 } 
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this, base)
c_TextStyle *this;
BaseStd_t *base;
{
 c_TextStyle *p;

 Xc_HISTORY(("copy(`%s')", this->name));
 Xc_ASSERT(base != NULL);

 if((p = Xc_new(i_handle)) == NULL) return NULL;
 memcpy(p, this, sizeof(c_TextStyle));
 if((p->hook = NEW(c_Hook)(p, 8)) == NULL)
 {
  Xc_delete(i_handle, p);
  return NULL;
 }
 i_references++;

 /*--- Insert copy in linked list ---*/
 if(base->text_style_base != NULL) base->text_style_base->previous = p;
 p->next = base->text_style_base;
 p->previous = NULL;
 base->text_style_base = p;
 p->base = &(base->text_style_base);
 p->used_count = 0;

 if(p->font != NULL)
 {
  scale_t tmp_width;
  coord_t tmp_size;

  XcH_REGISTER(p->font, NULL, p);
  tmp_size = p->point_size;
  if(p->attributes & XcTS_B_SUBSCRIPT)
   tmp_size = ROUNDFLOAT(SCALE_TO_COEF(p->subscript.scale) * tmp_size);
  else
   if(p->attributes & XcTS_B_SUPSCRIPT)
    tmp_size = ROUNDFLOAT(SCALE_TO_COEF(p->supscript.scale) * tmp_size);
      
  if(p->attributes & XcTS_B_WIDTH)
   tmp_width = p->width.scale;
  else
   tmp_width = SCALE_FROM_COEF(1.0);

  tmp_size &= COORD_FONTSIZE_MASK;
  p->metrics = COPY(c_CharMetrics)(p->font->metrics, NULL, tmp_size,
				   tmp_width, SCALE_FROM_DEGREES(0.0),
				   XcTS_TO_CM(p->attributes), TRUE);
  if(p->metrics == NULL)
  {
   DELETE(c_TextStyle)(p);
   return NULL;
  }
  reactualize(p);
 }
 else
  p->metrics = NULL;

 /*--- Color hooks ---*/
 p->shadow.color = p->outline.color = 
  p->underline.color = p->strikeout.color = NULL;
 if((p->color = F(this->color).virtualCopy(this->color, base)) == NULL)
 {
  DELETE(c_TextStyle)(p);
  return NULL;
 }
 XcH_REGISTER(p->color, hook_color, p);

 if((p->bgcolor = F(this->bgcolor).virtualCopy(this->bgcolor, base)) == NULL)
 {
  DELETE(c_TextStyle)(p);
  return NULL;
 }
 XcH_REGISTER(p->bgcolor, hook_color, p);
  
 if((p->underline.color = F(this->underline.color).virtualCopy
     (this->underline.color, base)) == NULL)
 {
  DELETE(c_TextStyle)(p);
  return NULL;
 }
 XcH_REGISTER(p->underline.color, hook_color, p);
 
 if((p->strikeout.color = F( this->strikeout.color).virtualCopy
     (this->strikeout.color, base)) == NULL)
 {
  DELETE(c_TextStyle)(p);
  return NULL;
 }
 XcH_REGISTER(p->strikeout.color, hook_color, p);

 if((p->outline.color = F(this->outline.color).virtualCopy
     (this->outline.color, base)) == NULL)
 {
  DELETE(c_TextStyle)(p);
  return NULL;
 }
 XcH_REGISTER(p->outline.color, hook_color, p);
 
 if((p->shadow.color = F(this->shadow.color).virtualCopy
     (this->shadow.color, base)) == NULL)
 {
  DELETE(c_TextStyle)(p);
  return NULL;
 }
 XcH_REGISTER(p->shadow.color, hook_color, p);

 if (p->inherit_from)
 {
  p->inherit_from = virtualCopy(this->inherit_from, base);
  XcH_REGISTER(p->inherit_from, hook_inherit, p);
 }
   
 return p;
}


/* ----------------------------------------------------------------- ** 
** virtualCopy - Copy object if it doesn't already exist             ** 
** ----------------------------------------------------------------- */
static c_TextStyle *virtualCopy(this, base)
c_TextStyle *this;
BaseStd_t *base;
{
 c_TextStyle *ptr;

 Xc_HISTORY(("virtualCopy(`%s')", this->name));
 
 if(this->base == &(base->text_style_base)) return this;
 
 for(ptr = base->text_style_base; ptr != NULL; ptr = ptr->next)
  if(F(this).compare(ptr, this, CMP_WITH_NAME))
  {
   Xc_TRACE(("found in new base"));
   break;
  }
 
 return((ptr == NULL)? COPY(c_TextStyle)(this, base) : ptr);
}


/* ----------------------------------------------------------------- ** 
** set_params - Set parameters for styles                            ** 
** ----------------------------------------------------------------- */
static void set_params(c_TextStyle *style, ...)
{
 va_list ap;

 Xc_HISTORY(("set(`%s')", style->name));

 va_start(ap, style);
 vset(style, ap);
 va_end(ap);
}

/* ----------------------------------------------------------------- ** 
** vset - Set style parameters                                       ** 
** ----------------------------------------------------------------- */
static void vset(style, ap)
c_TextStyle *style;
va_list	ap;
{
 scale_t	width_scale, subscript_scale, supscript_scale;
 unsigned int	attributes_on, attributes_off, old_attributes;
 unsigned int	new_attributes;
 c_VectorFont	*font;
 coord_t	point_size;
 boolean	out_flag;
 c_Color	*color;
 char		*name;
 c_TextStyle	*tstyle;
 family_tree	*family;
 font_list	*fontl;
 boolean	bold;
 boolean	italic; 

 Xc_HISTORY(("set(`%s')", style->name));

 attributes_on = 0;
 attributes_off = 0xffff;
 font = style->font;
 width_scale = style->width.scale;
 point_size = style->point_size;
 subscript_scale = style->subscript.scale;
 supscript_scale = style->supscript.scale;
 out_flag = FALSE;
  
 do
 {
  switch(va_arg(ap, textstyle_set_code_t))
  {
   /*___End of args___*/
  case XcTS_END:
   Xc_TRACE(("XcTS_END"));
   out_flag = TRUE;
   break;

   /*___New name___*/
  case XcTS_NAME:
   Xc_TRACE(("XcTS_NAME"));
   name = va_arg(ap, char *);
   if (name == NULL)
   {
    style->name[0] = '\0';
    style->hide_flag = TRUE;
   }
   else
   {
    strncpy(style->name, name, XcMAX_NAME_LENGTH - 1);
    style->hide_flag = FALSE;
   }
   continue;
	
   /*___New font___*/
  case XcTS_FONT:
   Xc_TRACE(("XcTS_FONT"));
   font = va_arg(ap, c_VectorFont *);
   if (font->specific->is_italic || font->specific->is_bold_italic)
    style->attributes |= XcTS_B_ITALIC;
   else
    style->attributes &= ~XcTS_B_ITALIC;
	  
   if (font->specific->is_bold || font->specific->is_bold_italic)
    style->attributes |= XcTS_B_BOLD;
   else
    style->attributes &= ~XcTS_B_BOLD;
   continue;

  case XcTS_FONT_FAMILY:
   Xc_TRACE(("XcTS_FONT_FAMILY"));
   family = va_arg(ap, family_tree *);

   fontl = style->font->specific;
  
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
   if (fontl)
    font = fontl->font;
   continue;

   /*___New size___*/
  case XcTS_SIZE:
   Xc_TRACE(("XcTS_SIZE"));
   point_size = va_arg(ap, coord_t) & COORD_FONTSIZE_MASK;
   continue;

   /*___New color___*/
  case XcTS_COLOR:
   Xc_TRACE(("XcTS_COLOR"));
   color = va_arg(ap, c_Color *);
   if (!style->bgcolor && color)
   {
    c_Color *transpc;
     
    transpc = F(color).getColor(color->base, 
				XcC_TRANSPARENCY, XcC_SCALE_COLOR(1.0), 
				XcC_GRAY, 0, XcC_END);
    
    if(XcH_REPLACE(style->bgcolor, transpc, hook_color, style))
     style->bgcolor = transpc;
   }
   if (!style->underline.color || style->underline.color == style->color)
   {
    if(XcH_REPLACE(style->underline.color, color, hook_color, style))
     style->underline.color = color;
   }
   if (!style->strikeout.color || style->strikeout.color == style->color)
   {
    if(XcH_REPLACE(style->strikeout.color, color, hook_color, style))
     style->strikeout.color = color;
   }
   if (!style->shadow.color || style->shadow.color == style->color)
   {
    c_Color	*shadow_color;

    shadow_color = compute_shadow_color(color);
    if(XcH_REPLACE(style->shadow.color, shadow_color, hook_color, style))
     style->shadow.color = shadow_color;
   }
   if (!style->outline.color || style->outline.color == style->color)
   {
    if(XcH_REPLACE(style->outline.color, color, hook_color, style))
     style->outline.color = color;
   }
   
   if(XcH_REPLACE(style->color, color, hook_color, style))
    style->color = color;
   continue;

  case XcTS_BGCOLOR:
   Xc_TRACE(("XcTS_BGCOLOR"));
   color = va_arg(ap, c_Color *);
   if(XcH_REPLACE(style->bgcolor, color, hook_color, style))

   style->bgcolor = color;
   continue;

  case XcTS_HIDE_ON:
   Xc_TRACE(("XcTS_HIDE_ON"));
   style->hide_flag = TRUE;
   break;
      
  case XcTS_HIDE_OFF:
   Xc_TRACE(("XcTS_HIDE_OFF"));
   style->hide_flag = FALSE;
   break;
	
  case XcTS_INHERIT:
   Xc_TRACE(("XcTS_INHERIT"));
   tstyle = (c_TextStyle *)va_arg(ap, c_TextStyle *);
   if (!style->inherit_from)
   {
    if (tstyle->inherit_from)
     style->inherit_from = tstyle->inherit_from;
    else
     style->inherit_from = tstyle;
    XcH_REGISTER(style->inherit_from, hook_inherit, style);
   }
   style->valid_field |= (unsigned int)va_arg(ap, unsigned int);
   break;

  case XcTS_ATTRIB:
   old_attributes = style->attributes;
   new_attributes = (unsigned int)va_arg(ap, unsigned int);
	
   if ( (old_attributes & XcTS_B_BOLD) && 
       !(new_attributes & XcTS_B_BOLD))
    bold_off(style, &font);
   if (!(old_attributes & XcTS_B_BOLD) && 
       (new_attributes & XcTS_B_BOLD))
    bold_on(style, &font);

   if ( (old_attributes & XcTS_B_ITALIC) && 
       !(new_attributes & XcTS_B_ITALIC))
    italic_off(style, &font);
   if (!(old_attributes & XcTS_B_ITALIC) && 
       (new_attributes & XcTS_B_ITALIC))
    italic_on(style, &font);

   style->attributes = new_attributes;

   if ( (old_attributes & XcTS_B_WIDTH) && 
       !(style->attributes & XcTS_B_WIDTH))
    attributes_off &= ~XcTS_B_WIDTH;

   if ( !(old_attributes & XcTS_B_WIDTH) && 
       (style->attributes & XcTS_B_WIDTH))
   {
    attributes_on |= XcTS_B_WIDTH;
    width_scale = style->width.scale;
   }

   if ( (old_attributes & XcTS_B_SUBSCRIPT) && 
       !(style->attributes & XcTS_B_SUBSCRIPT))
    attributes_off &= ~XcTS_B_SUBSCRIPT;

   if ( !(old_attributes & XcTS_B_SUBSCRIPT) && 
       (style->attributes & XcTS_B_SUBSCRIPT))
   {
    attributes_on |= XcTS_B_SUBSCRIPT;
    attributes_off &= ~XcTS_B_SUPSCRIPT;
    subscript_scale = style->subscript.scale;
   }

   if ( (old_attributes & XcTS_B_SUPSCRIPT) && 
       !(style->attributes & XcTS_B_SUPSCRIPT))
    attributes_off &= ~XcTS_B_SUPSCRIPT;
      
   if ( !(old_attributes & XcTS_B_SUPSCRIPT) && 
       (style->attributes & XcTS_B_SUPSCRIPT))
   {
    attributes_on |= XcTS_B_SUPSCRIPT;
    attributes_off &= ~XcTS_B_SUBSCRIPT;
    supscript_scale = style->supscript.scale;
   }

   if ( (old_attributes & XcTS_B_SMALL_CAPS) && 
       !(style->attributes & XcTS_B_SMALL_CAPS))
    attributes_off &= ~XcTS_B_SMALL_CAPS;
      
   if ( !(old_attributes & XcTS_B_SMALL_CAPS) && 
       (style->attributes & XcTS_B_SMALL_CAPS))
   {
    attributes_on |= XcTS_B_SMALL_CAPS;
    attributes_off &= ~XcTS_B_BIG_CAPS;
   }

   if ( (old_attributes & XcTS_B_BIG_CAPS) && 
       !(style->attributes & XcTS_B_BIG_CAPS))
    attributes_off &= ~XcTS_B_BIG_CAPS;
      
   if ( !(old_attributes & XcTS_B_BIG_CAPS) && 
       (style->attributes & XcTS_B_BIG_CAPS))
   {
    attributes_on |= XcTS_B_BIG_CAPS;
    attributes_off &= ~XcTS_B_SMALL_CAPS;
   }
   break;
   /*___width args___*/
  case XcTS_WIDTH_CONF:
   Xc_TRACE(("XcTS_WIDTH_CONF"));
   width_scale = (scale_t)va_arg(ap, double);
   continue;
	
   /*___Underline args___*/
  case XcTS_UNDERLINE_CONF:
   Xc_TRACE(("XcTS_UNDERLINE_CONF"));
   style->underline.dy = (scale_t) va_arg(ap, double);
   style->underline.overstroke = (scale_t) va_arg(ap, double);
   style->underline.thickness = (scale_t) va_arg(ap, double);
   style->underline.overspace = (boolean) va_arg(ap, int);
   color = va_arg(ap, c_Color *);
   style->underline.type = va_arg(ap, enum textstyle_underline_e);

   if(XcH_REPLACE(style->underline.color, color, hook_color, style))
    style->underline.color = color;
   continue;

   /*___Strikeout args___*/
  case XcTS_STRIKEOUT_CONF:
   Xc_TRACE(("XcTS_STRIKEOUT_CONF"));
   style->strikeout.dy = (scale_t) va_arg(ap, double);
   style->strikeout.overstroke = (scale_t) va_arg(ap, double);
   style->strikeout.thickness = (scale_t) va_arg(ap, double);
   style->strikeout.overspace = (boolean) va_arg(ap, int);
   color = va_arg(ap, c_Color *);
   style->strikeout.type = va_arg(ap, enum textstyle_underline_e);

   if(XcH_REPLACE(style->strikeout.color, color, hook_color, style))
    style->strikeout.color = color;
   continue;
	
   /*___Subscript args___*/
  case XcTS_SUBSCRIPT_CONF:
   Xc_TRACE(("XcTS_SUBSCRIPT_CONF"));
   style->subscript.dy = (scale_t) va_arg(ap, double);
   subscript_scale = (scale_t) va_arg(ap, double);
   continue;
	
   /*___Supscript args___*/
  case XcTS_SUPSCRIPT_CONF:
   Xc_TRACE(("XcTS_SUPSCRIPT_CONF"));
   style->supscript.dy = (scale_t) va_arg(ap, double);
   supscript_scale = (scale_t) va_arg(ap, double);
   continue;
	
   /*___Shadow args___*/
  case XcTS_SHADOW_CONF:
   Xc_TRACE(("XcTS_SHADOW_CONF"));
   style->shadow.dx = (scale_t)va_arg(ap, double);
   style->shadow.dy = (scale_t)va_arg(ap, double);
   color = va_arg(ap, c_Color *);
   if (!color)			/* automatik color */
    color = compute_shadow_color(style->color);

   if(XcH_REPLACE(style->shadow.color, color, hook_color, style))
    style->shadow.color = color;
   continue;
	
   /*___Outline args___*/
  case XcTS_OUTLINE_CONF:
   Xc_TRACE(("XcTS_OUTLINE_CONF"));
   style->outline.thickness = (scale_t) va_arg(ap, double);
   color = va_arg(ap, c_Color *);
   if(XcH_REPLACE(style->outline.color, color, hook_color, style))
    style->outline.color = color;
   continue;
	
   /* -------------------------------------------------------------------- *
   * --------------------  attribut WIDTH ON / OFF  --------------------- *
   * -------------------------------------------------------------------- */

  case XcTS_WIDTH_ON:
   Xc_TRACE(("XcTS_WIDTH_ON"));
   attributes_on |= XcTS_B_WIDTH;
   continue;
	
  case XcTS_WIDTH_OFF:
   Xc_TRACE(("XcTS_WIDTH_OFF"));
   attributes_off &= ~XcTS_B_WIDTH;
   continue;
		
   /* -------------------------------------------------------------------- *
   * -------------------  attribut BOLD ON / OFF  ----------------------- *
   * -------------------------------------------------------------------- */
	
  case XcTS_BOLD_ON:
   {
    if ((style->attributes & XcTS_B_BOLD) != 0) return;
    bold_on(style, &font);
    continue;
   }
      
  case XcTS_BOLD_OFF:
   {
    if ((style->attributes & XcTS_B_BOLD) == 0) return;
    bold_off(style, &font);
    continue;
   }
	
   /* -------------------------------------------------------------------- *
   * ------------------  attribut ITALIC ON / OFF  ---------------------- *
   * -------------------------------------------------------------------- */
	
  case XcTS_ITALIC_ON:
   {
    if ((style->attributes & XcTS_B_ITALIC) != 0) return;
    italic_on(style, &font);
    continue;
   }
      
  case XcTS_ITALIC_OFF:
   {
    if ((style->attributes & XcTS_B_ITALIC) == 0) return;
    italic_off(style, &font);
    continue;
   }
	
   /* -------------------------------------------------------------------- *
   * ----------------  attribut UNDERLINE ON / OFF  --------------------- *
   * -------------------------------------------------------------------- */
      
  case XcTS_UNDERLINE_ON:
   if ((style->attributes & XcTS_B_UNDERLINE) != 0) continue;
   style->attributes |= XcTS_B_UNDERLINE;
   continue;
	
  case XcTS_UNDERLINE_OFF:
   if ((style->attributes & XcTS_B_UNDERLINE) == 0) continue;
   style->attributes &= ~XcTS_B_UNDERLINE;
   continue;
      
   /* -------------------------------------------------------------------- *
   * ----------------  attribut STRIKEOUT ON / OFF  --------------------- *
   * -------------------------------------------------------------------- */
      
  case XcTS_STRIKEOUT_ON:
   if ((style->attributes & XcTS_B_STRIKEOUT) != 0) continue;
   style->attributes |= XcTS_B_STRIKEOUT;
   continue;
	
  case XcTS_STRIKEOUT_OFF:
   if ((style->attributes & XcTS_B_STRIKEOUT) == 0) continue;
   style->attributes &= ~XcTS_B_STRIKEOUT;
   continue;
	
   /* -------------------------------------------------------------------- *
   * ----------------  attribut SUBSCRIPT ON / OFF  --------------------- *
   * -------------------------------------------------------------------- */
  case XcTS_SUBSCRIPT_ON:
   if ((style->attributes & XcTS_B_SUBSCRIPT) != 0) continue;
   attributes_on |= XcTS_B_SUBSCRIPT;
   attributes_off &= ~XcTS_B_SUPSCRIPT;
   continue;
	
  case XcTS_SUBSCRIPT_OFF:
   if ((style->attributes & XcTS_B_SUBSCRIPT) == 0) continue;
   attributes_off &= ~XcTS_B_SUBSCRIPT;
   continue;
	
   /* -------------------------------------------------------------------- *
   * ----------------  attribut SUPSCRIPT ON / OFF  --------------------- *
   * -------------------------------------------------------------------- */
  case XcTS_SUPSCRIPT_ON:
   if ((style->attributes & XcTS_B_SUPSCRIPT) != 0) continue;
   attributes_on |= XcTS_B_SUPSCRIPT;
   attributes_off &= ~XcTS_B_SUBSCRIPT;
   continue;
	
  case XcTS_SUPSCRIPT_OFF:
   if ((style->attributes & XcTS_B_SUPSCRIPT) == 0) continue;
   attributes_off &= ~XcTS_B_SUPSCRIPT;
   continue;
	
   /* -------------------------------------------------------------------- *
   * ------------------  attribut SHADOW ON / OFF  ---------------------- *
   * -------------------------------------------------------------------- */
	
  case XcTS_SHADOW_ON:
   if ((style->attributes & XcTS_B_SHADOW) != 0) continue;
   style->attributes |= XcTS_B_SHADOW;
   continue;
	
  case XcTS_SHADOW_OFF:
   if ((style->attributes & XcTS_B_SHADOW) == 0) continue;
   style->attributes &= ~XcTS_B_SHADOW;
   continue;

   /* -------------------------------------------------------------------- *
   * ------------------  attribut OUTLINE ON / OFF  --------------------- *
   * -------------------------------------------------------------------- */
	
  case XcTS_OUTLINE_ON:
   continue;
	
  case XcTS_OUTLINE_OFF:
   continue;
	
   /* -------------------------------------------------------------------- *
   * ----------------  attribut SMALL CAPS ON / OFF  -------------------- *
   * -------------------------------------------------------------------- */
  case XcTS_SMALL_CAPS_ON:
   attributes_on |= XcTS_B_SMALL_CAPS;
   attributes_off &= ~XcTS_B_BIG_CAPS;
   continue;
      
  case XcTS_SMALL_CAPS_OFF:
   attributes_off &= ~XcTS_B_SMALL_CAPS;
   continue;
	
   /* -------------------------------------------------------------------- *
   * ------------------  attribut BIG CAPS ON / OFF  -------------------- *
   * -------------------------------------------------------------------- */
  case XcTS_BIG_CAPS_ON:
   attributes_on |= XcTS_B_BIG_CAPS;
   attributes_off &= ~XcTS_B_SMALL_CAPS;
   continue;
      
  case XcTS_BIG_CAPS_OFF:
   attributes_off &= ~XcTS_B_BIG_CAPS;
   continue;
	
  default:
   Xc_BREAK(("`%s': unknow set-style code", style->name));
  }
 } while(!out_flag);
  
 if (style->name[0] == '\0' && style->font && style->color)
 {			/* generate a formated name for unnamed style */
  unsigned int tmp_attributes;
  
  tmp_attributes = (style->attributes | attributes_on) & attributes_off;  
  sprintf(style->name, "%.10s_%c%c%c%c%c%c%c%c_%.4d_%.6s",
	  font->spec_family->name,
	  ((tmp_attributes & XcTS_B_BOLD) != 0) ? 'B' : '-',
	  ((tmp_attributes & XcTS_B_ITALIC) != 0) ? 'I' : '-',
	  ((tmp_attributes & XcTS_B_UNDERLINE) != 0) ? 'U' : '-',
	  ((tmp_attributes & XcTS_B_STRIKEOUT) != 0) ? 'S' : '-',
	  ((tmp_attributes & XcTS_B_SUBSCRIPT) != 0) ? 'b' : 
	  (((tmp_attributes & XcTS_B_SUPSCRIPT) != 0) ? 'p' : '-'),
	  ((tmp_attributes & XcTS_B_SHADOW) != 0) ? 'W' : '-',
	  ((tmp_attributes & XcTS_B_OUTLINE) != 0) ? 'O' : '-',
	  ((tmp_attributes & XcTS_B_SMALL_CAPS) != 0) ? 'k' : 
	  (((tmp_attributes & XcTS_B_BIG_CAPS) != 0) ? 'K' : '-'),
	  (int)SCALE_TO_POINTS(point_size), style->color->name);  
 }

 if ((style->font != font) || (style->point_size != point_size) ||
     (style->width.scale != width_scale) ||
     (style->subscript.scale != subscript_scale) ||
     (style->supscript.scale != supscript_scale) ||
     attributes_on || (attributes_off != 0xffff))
 {
  c_CharMetrics *cm;
  scale_t tmp_width;
  coord_t tmp_size;
  unsigned int tmp_attributes;
    
  tmp_attributes = style->attributes | attributes_on;
  tmp_attributes &= attributes_off;
    
  if (tmp_attributes & XcTS_B_SUBSCRIPT)
   tmp_size = ROUNDFLOAT(SCALE_TO_COEF(subscript_scale) * point_size);
  else
   if (tmp_attributes & XcTS_B_SUPSCRIPT)
    tmp_size = ROUNDFLOAT(SCALE_TO_COEF(supscript_scale) * point_size);
   else
    tmp_size = point_size;
    
  if (tmp_attributes & XcTS_B_WIDTH)
   tmp_width = width_scale;
  else
   tmp_width = SCALE_FROM_COEF(1.0);
    
  tmp_size &= COORD_FONTSIZE_MASK;
    
  if(F(font).load(font))
  {
   cm = COPY(c_CharMetrics)(font->metrics, NULL, tmp_size, tmp_width,
			    SCALE_FROM_DEGREES(0.0), 
			    XcTS_TO_CM(tmp_attributes), TRUE);
   if(cm != NULL)
   {
    if(style->font != NULL)
    {
     DELETE(c_CharMetrics)(style->metrics);
     XcH_UNREGISTER(style->font, NULL, style);
    }
    XcH_REGISTER(font, NULL, style);
	
    style->font = font;
    style->metrics = cm;
    style->point_size = point_size;
    style->width.scale = width_scale;
    style->subscript.scale = subscript_scale;
    style->supscript.scale = supscript_scale;
    style->attributes = tmp_attributes;
    reactualize(style);
    if (style->hook)
     F(style->hook).callback(style->hook,
			     XcH_CONTENT_CHANGED, style);
    if (style->hook)
     F(style->hook).callback(style->hook, XcH_REDRAW, style);
    return;
   }
  }
 }
 reactualize(style);
 if (style->hook)
  F(style->hook).callback(style->hook, XcH_REDRAW, style);
}



/* ----------------------------------------------------------------- ** 
** compare - Compare two TextRuler objects                           ** 
** ----------------------------------------------------------------- */
static boolean compare(a, b, with_name)
c_TextStyle *a;
c_TextStyle *b;
boolean with_name;
{
 if (a == b) return TRUE;
 if ((a == NULL) || (b == NULL)) return FALSE;
 if (with_name && 
     !(a->hide_flag || b->hide_flag || Xstrcmp(a->name, b->name)))
  return FALSE; 
  
 if ((a->font == b->font) &&
     (a->attributes == b->attributes) &&
     (a->point_size == b->point_size) &&
     F(a->color).compare(a->color, b->color, CMP_WITH_NAME) &&
     F(a->bgcolor).compare(a->bgcolor, b->bgcolor, CMP_WITH_NAME))
 {
  if((a->attributes & XcTS_B_WIDTH) &&
     (a->width.scale != b->width.scale)) return FALSE;
    
  if((a->attributes & XcTS_B_ITALIC) &&
     (a->italic.angle != b->italic.angle)) return FALSE;
  
  if((a->attributes & XcTS_B_UNDERLINE) &&
     ((a->underline.dy != b->underline.dy) ||
      (a->underline.overstroke != b->underline.overstroke) ||
      (a->underline.thickness != b->underline.thickness) ||
      (a->underline.overspace != b->underline.overspace) ||
      !F(a->underline.color).compare
      (a->underline.color, b->underline.color, CMP_WITH_NAME)))
   return FALSE;

  if((a->attributes & XcTS_B_STRIKEOUT) &&
     ((a->strikeout.dy != b->strikeout.dy) ||
      (a->strikeout.overstroke != b->strikeout.overstroke) ||
      (a->strikeout.thickness != b->strikeout.thickness) ||
      (a->strikeout.overspace != b->strikeout.overspace) ||
      !F(a->strikeout.color).compare
      (a->strikeout.color, b->strikeout.color, CMP_WITH_NAME)))
   return FALSE;
    
  if((a->attributes & XcTS_B_OUTLINE) &&
     ((a->outline.thickness != b->outline.thickness) ||
      !F(a->outline.color).compare
      (a->outline.color, b->outline.color, CMP_WITH_NAME)))
   return FALSE;
    
  if((a->attributes & XcTS_B_SHADOW) &&
     ((a->shadow.dx != b->shadow.dx) ||
      (a->shadow.dy != b->shadow.dx) ||
      !F(a->shadow.color).compare
      (a->shadow.color, b->shadow.color, CMP_WITH_NAME)))
   return FALSE;
    
  if((a->attributes & XcTS_B_SUBSCRIPT) &&
     ((a->subscript.dy != b->subscript.dy) ||
      (a->subscript.scale != b->subscript.scale))) return FALSE;
    
  if((a->attributes & XcTS_B_SUPSCRIPT) &&
     ((a->supscript.dy != b->supscript.dy) ||
      (a->supscript.scale != b->supscript.scale))) return FALSE;
    
  return TRUE;
 }
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** merge - Merge two ruler list                                      ** 
** ----------------------------------------------------------------- */
static void merge(source, target, fusion_flag)
c_TextStyle *source;
c_TextStyle **target;
boolean fusion_flag;
{
 c_TextStyle *ptr, *base;

 Xc_HISTORY(("merge"));

 source = *source->base;
 base = *target;
 for(ptr = base; ptr != NULL; ptr = ptr->next) ptr->id = -1;
 while(source != NULL)
 {
  if(fusion_flag != FALSE)
  {
   for(ptr = base; ptr != NULL; ptr = ptr->next)
    if(F(source).compare(source, ptr, CMP_WITH_NAME)) break;
   if(ptr != NULL)
   {
    ptr->id = source->id;
    ptr = source;
    source = source->next;
    DELETE(c_TextStyle)(ptr);
    continue;
   }
  }

  ptr = source;
  source = source->next;

  /*--- Remove from source base ---*/
  Xc_ASSERT(ptr->previous == NULL);
  if(ptr->next != NULL) ptr->next->previous = NULL;
  *ptr->base = ptr->next;

  /*--- Add to target base ---*/
  ptr->next = base;
  ptr->base = target;
  if(base != NULL) base->previous = ptr;
  *target = base = ptr;
 }
}


/* ----------------------------------------------------------------- ** 
** reactualize - Recalculate internal variables                      ** 
** ----------------------------------------------------------------- */
static void reactualize(style)
c_TextStyle *style;
{
 register coord_t		point_size;
 complex_char_metrics_t	*m;
 coord_t			space_width;

 Xc_TRACE(("reactualize"));
#define XcTS_SCALE_SIZE(scale) ROUNDFLOAT(SCALE_TO_COEF(scale) * point_size)
#define XcTS_SCALEM_SIZE(scale) (scale * style->metrics->ascender)
  
 point_size = style->point_size;
 if((m = style->metrics->glyph[' ' - Xc_FIRST_CHAR]) != NULL)
  space_width = m->width[0].dx;
 else
  space_width = 0;

 style->underline.internal_dy = 
  XcTS_SCALEM_SIZE(style->underline.dy);
 style->underline.internal_overstroke = 
  style->underline.overstroke*space_width;
 style->underline.internal_thickness = 
  XcTS_SCALEM_SIZE(style->underline.thickness);

 style->strikeout.internal_dy = 
  XcTS_SCALEM_SIZE(style->strikeout.dy);
 style->strikeout.internal_overstroke = 
  style->strikeout.overstroke*space_width;
 style->strikeout.internal_thickness =
  XcTS_SCALEM_SIZE(style->strikeout.thickness);

 /*style->outline.internal_thickness =XcTS_SCALE_SIZE(style->outline.thickness);*/
 style->shadow.internal_offset.dx = XcTS_SCALEM_SIZE(style->shadow.dx);
 style->shadow.internal_offset.dy = XcTS_SCALEM_SIZE(style->shadow.dy);

 style->subscript.internal_dy = XcTS_SCALEM_SIZE(style->subscript.dy);
 style->subscript.internal_point_size=XcTS_SCALE_SIZE(style->subscript.scale);
 style->subscript.internal_point_size &= COORD_FONTSIZE_MASK;

 style->supscript.internal_dy = XcTS_SCALEM_SIZE(style->supscript.dy);
 style->supscript.internal_point_size=XcTS_SCALE_SIZE(style->supscript.scale);
 style->supscript.internal_point_size &= COORD_FONTSIZE_MASK;
}

/* ----------------------------------------------------------------- ** 
** hook_color - Color hook                                           ** 
** ----------------------------------------------------------------- */
static void hook_color(this, reason, old, new)
c_TextStyle *this;
int reason;
c_Color *old;
c_Color *new;
{
 Xc_TRACE(("hook_color"));

 if(reason == XcH_CONTENT_CHANGED)
 {
  F(this->hook).callback(this->hook, XcH_REDRAW, this);
  return;
 }
}


/* ----------------------------------------------------------------- ** 
** ps_print - Change graphic state                                   ** 
** ----------------------------------------------------------------- */
static boolean ps_print(this, post)
c_TextStyle *this;
c_PostScript *post;
{
 postscript_state_t *state;
 coord_t point_size;
 coord_t rise;
 scale_t factor;
 int mode;

 Xc_HISTORY(("ps_print(`%s')", this->name));
 Xc_ASSERT(!post->ai_mode || post->state.text_flag);

 state = &post->state;
  
 if(this->attributes & XcTS_B_SUBSCRIPT)
 {
  rise = this->subscript.internal_dy;
  point_size = this->subscript.internal_point_size;
 }
 else
  if(this->attributes & XcTS_B_SUPSCRIPT)
  {
   rise = this->supscript.internal_dy;
   point_size = this->supscript.internal_point_size;
  }
  else
  {
   point_size = this->point_size;
   rise = 0;
  }
  
 /*--- Rise ---*/
 if(state->text_rise != rise)
 {
  state->text_rise = rise;
  if(!F(post).putLine(post, "$c Ts", 
		      state->text_rise)) return FALSE;
 }

 /*--- Font and point size ---*/
 if((state->font != this->font) || (state->point_size != point_size))
 {
  Xc_ASSERT(this->font->id > 0);
  F(post).addFont(post, this->font);
  if(post->ai_mode?
     !F(post).putLine(post, "/_$s $c Tf",
		      this->font->font_name, point_size) :
     !F(post).putLine(post, "/F$d $c Tf", 
		      this->font->id, point_size))
   return FALSE;
  state->font = this->font;
  state->point_size = point_size;
 }
  
 /*--- Color ---*/
 mode = (this->color->transparency == 0)? 0 : 3;
 if((this->color->transparency == 0) &&
    !F(this->color).ps_print(this->color, post, FALSE)) return FALSE;

 /*--- Horizontal scaling ---*/
 factor = (this->attributes & XcTS_B_WIDTH)? this->width.scale : SCALE_ONE; 
 if(state->horizontal_scale != factor)
 {
  if(!F(post).putLine(post, NORMAL_TEXT, "$f Tz", 
		      (double)SCALE_TO_PERCENT(factor)))
   return FALSE;
  if(this->attributes & XcTS_B_SHADOW)
   if(!F(post).putLine(post, SHADOW_TEXT, "$f Tz", 
		       (double)SCALE_TO_PERCENT(factor)))
    return FALSE;
  state->horizontal_scale = factor;
 }

 /*--- Outline ---*/
#if 0
 if((this->attributes & XcTS_B_OUTLINE) &&
    (this->outline.color != NULL) && (this->outline.color->transparency == 0))
 {
  if(!F(this->color).ps_print(this->outline.color, post, TRUE))
   return FALSE;
  if(state->line_width != this->outline.internal_thickness)
  {
   state->line_width = this->outline.internal_thickness;
   if(!F(post).putLine(post, "$c w", state->line_width)) return FALSE;
  }
  mode = (mode == 0)? 2 : 1;
 }
#endif

 /*--- Render mode ---*/
 if(state->render_mode != mode)
 {
  if(!F(post).putLine(post, "$d Tr", (long)mode)) return FALSE;
  state->render_mode = mode;
 }

 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** getStyle - Get or create style                                    ** 
** ----------------------------------------------------------------- */
static c_TextStyle *getStyle(c_TextStyle *style, BaseStd_t *base, ...)
{
 c_TextStyle *ptr, *tstyle;
 va_list ap;

 Xc_HISTORY(("getStyle"));

 if (style)
 {
  tstyle = (c_TextStyle *)COPY(c_TextStyle)(style, base);
 }
 else
 {
  tstyle =  (c_TextStyle *)NEW(c_TextStyle)
   (&(base->text_style_base), NULL, GlobFontBase,
    SCALE_FROM_POINTS(12.0), base->color_base);
 }

 va_start(ap, base);
 vset(tstyle, ap);
 va_end(ap);

 for(ptr = base->text_style_base; ptr != NULL; ptr = ptr->next)
 {
  if((ptr != tstyle) && F(ptr).compare(ptr, tstyle, CMP_WITHOUT_NAME))
  {
   DELETE(c_TextStyle)(tstyle);
   return ptr; 
  }
 }
 return tstyle;
}

/* ----------------------------------------------------------------- ** 
** ps_print_text - Change graphic state                              ** 
** ----------------------------------------------------------------- */
static boolean ps_print_text(this, text, len, post, c_x, c_y, fput)
c_TextStyle	*this;
char		*text;
int		len;
c_PostScript	*post;
coord_t		c_x;
coord_t		c_y;
char		*fput;
{
 coord_t		point_size;
 coord_t		rise;
 scale_t		factor;
 int			mode;
 
 Xc_HISTORY(("ps_print_text(`%s':%s)", this->name, text));
 Xc_ASSERT(!post->ai_mode || post->state.text_flag);
 
 if(this->attributes & XcTS_B_SUBSCRIPT)
 {
  rise = this->subscript.internal_dy;
  point_size = this->subscript.internal_point_size;
 }
 else
  if(this->attributes & XcTS_B_SUPSCRIPT)
  {
   rise = this->supscript.internal_dy;
   point_size = this->supscript.internal_point_size;
  }
  else
  {
   point_size = this->point_size;
   rise = 0;
  }
 
 /*--- Rise ---*/
 if(post->state2[NORMAL_TEXT].text_rise != rise)
 {
  if(!F(post).putLine2(post, NORMAL_TEXT, "$c Ts", 
		       rise)) return FALSE;
  post->state2[NORMAL_TEXT].text_rise = rise;

  if((this->attributes & XcTS_B_SHADOW) && 
     ((this->shadow.dx != 0) || (this->shadow.dy != 0)) &&
     (post->state2[SHADOW_TEXT].text_rise != rise))
  {
   if(!F(post).putLine2(post, SHADOW_TEXT, "$c Ts", 
			rise)) return FALSE;
   post->state2[SHADOW_TEXT].text_rise = rise;
  }
  else
   post->state2[SHADOW_TEXT].text_rise = rise;
 }
 /*--- Font and point size ---*/
 put_font(post, NORMAL_TEXT, this->font, point_size);

 if((this->attributes & XcTS_B_SHADOW) &&
    ((this->shadow.dx != 0) || (this->shadow.dy != 0)))
  put_font(post, SHADOW_TEXT, this->font, point_size);
 
 /*--- Horizontal scaling ---*/
 factor = (this->attributes & XcTS_B_WIDTH)? this->width.scale : SCALE_ONE; 
 if(post->state2[NORMAL_TEXT].horizontal_scale != factor)
 {
  if(!F(post).putLine2(post, NORMAL_TEXT, "$f Tz", 
		       (double)SCALE_TO_PERCENT(factor)))
   return FALSE;
  post->state2[NORMAL_TEXT].horizontal_scale = factor;
 }
 if((this->attributes & XcTS_B_SHADOW) &&
    ((this->shadow.dx != 0) || (this->shadow.dy != 0)) &&
    (post->state2[SHADOW_TEXT].horizontal_scale != factor))
 {
  if(!F(post).putLine2(post, SHADOW_TEXT, "$f Tz", 
		       (double)SCALE_TO_PERCENT(factor)))
   return FALSE;
  post->state2[SHADOW_TEXT].horizontal_scale = factor;
 }

 /*--- Outline ---*/
#if 0
 if((this->attributes & XcTS_B_OUTLINE) &&
    (this->outline.color != NULL) && (this->outline.color->transparency == 0))
 {
  if(!F(this->color).ps_print(this->outline.color, post, TRUE))
   return FALSE;
  if(state->line_width != this->outline.internal_thickness)
  {
   state->line_width = this->outline.internal_thickness;
   if(!F(post).putLine(post, "$c w", state->line_width)) return FALSE;
  }
  mode = (mode == 0)? 2 : 1;
 }
#endif
 /*mode = (this->color->transparency == 0)? 0 : 3;*/
 mode = 0;

 /*--- Render mode ---*/
 if(post->state2[NORMAL_TEXT].render_mode != mode)
 {
  if(!F(post).putLine2(post, NORMAL_TEXT, "$d Tr", (long)mode)) return FALSE;
  post->state2[NORMAL_TEXT].render_mode = mode;
 }
 if((this->attributes & XcTS_B_SHADOW) &&
    ((this->shadow.dx != 0) || (this->shadow.dy != 0)) &&
    (post->state2[SHADOW_TEXT].render_mode != mode))
 {
  if(!F(post).putLine2(post, SHADOW_TEXT, "$d Tr", (long)mode)) return FALSE;
  post->state2[SHADOW_TEXT].render_mode = mode;
 }
 
 /*--- Shadow ---*/
 if((this->attributes & XcTS_B_SHADOW) &&
    ((this->shadow.dx != 0) || (this->shadow.dy != 0)))
 {
  /*--- Shadow color ---*/
  if(!F(this->shadow.color).ps_print2(this->shadow.color,
				      post, SHADOW_TEXT, FALSE))
   return FALSE;

  if(!F(post).putLine2(post, SHADOW_TEXT, "$c $c Td",
		       this->shadow.internal_offset.dx,
		       -this->shadow.internal_offset.dy))
   return FALSE;

  /*--- Small Caps ---*/
  if((this->attributes & XcTS_B_SMALL_CAPS))
  {
   small_caps(this, text, len, point_size, post, SHADOW_TEXT, fput);
  }
  else
  {
   /*--- Big Caps ---*/
   if((this->attributes & XcTS_B_BIG_CAPS))
    big_caps(this, text, len);
   F(post).putString2(post, SHADOW_TEXT, text, len);
   F(post).putLine2(post, SHADOW_TEXT, fput);
  }
  /*--- Underline ---*/
  if((this->attributes & XcTS_B_UNDERLINE))
  {
   underline(this, text, len, post, SHADOW_TEXT, TRUE, c_x, c_y);
  }
  /*--- Strikeout ---*/
  if((this->attributes & XcTS_B_STRIKEOUT))
  {
   strikeout(this, text, len, post, SHADOW_TEXT, TRUE, c_x, c_y);
  }
  if(!F(post).putLine2(post, SHADOW_TEXT, "$c $c Td",
		       -this->shadow.internal_offset.dx,
		       this->shadow.internal_offset.dy))
   return FALSE;
 }
 /*--- Normal Color ---*/
 if((this->color->transparency == 0) &&
    !F(this->color).ps_print2(this->color, post, NORMAL_TEXT, FALSE)) 
  return FALSE;

 /*--- Small Caps ---*/
 if((this->attributes & XcTS_B_SMALL_CAPS))
 {
  small_caps(this, text, len, point_size, post, NORMAL_TEXT, fput);
 }
 else
 {
  /*--- Big Caps ---*/
  if((this->attributes & XcTS_B_BIG_CAPS))
   big_caps(this, text, len);
  F(post).putString2(post, NORMAL_TEXT, text, len);
  F(post).putLine2(post, NORMAL_TEXT, fput);
 }
 /*--- Underline ---*/
 if((this->attributes & XcTS_B_UNDERLINE))
 {
  underline(this, text, len, post, NORMAL_TEXT, FALSE, c_x, c_y);
 }
 /*--- Strikeout ---*/
 if((this->attributes & XcTS_B_STRIKEOUT))
 {
  strikeout(this, text, len, post, NORMAL_TEXT, FALSE, c_x, c_y);
 }
 
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** markUsed : increment used counter                                 ** 
** ----------------------------------------------------------------- */
static void markUsed(This)
c_TextStyle *This;
{
 This->used_count++;
}

/* ----------------------------------------------------------------- ** 
** markUnused : decrement used counter                               ** 
** ----------------------------------------------------------------- */
static void markUnused(This)
c_TextStyle *This;
{
 This->used_count--;
 if (This->used_count <= 0)
 {
  if (This->hide_flag && !This->lock_flag)
  {
   DELETE(c_TextStyle)(This);
  }
  else
   This->used_count = 0;
 }
}

/* ----------------------------------------------------------------- ** 
** Private functions                                                 ** 
** ----------------------------------------------------------------- */
static void big_caps(style, text, len)
c_TextStyle	*style;
char		*text;
int		len;
{
 int	i;
 i=0;
 while(text[i] && (i < len))
 {
  text[i] = Xc_TO_UPPER(text[i]);
  i++;
 }
}

static void small_caps(style, text, len, point_size, post, type, fput)
c_TextStyle	*style;
char		*text;
int		len;
coord_t		point_size;
c_PostScript	*post;
int		type;
char		*fput;
{
 int	i;
 int	start;
 coord_t point_small;
 char	svg_text[300];
 int	first;

 i=0;
 first = TRUE;
 memcpy(svg_text, text, strlen(text) + 1);
 strcpy(svg_text, text);
 point_small = (float)point_size*(float)0.75;
 if (svg_text[i]!=Xc_TO_UPPER(svg_text[i]))
 {
  put_font(post, type, style->font, point_small);
  start = i;
  while (svg_text[i] && (i < len) &&
	 (svg_text[i]!=Xc_TO_UPPER(svg_text[i])))
  {
   svg_text[i]=Xc_TO_UPPER(svg_text[i]);
   i++;
  }
  F(post).putString2(post, type, svg_text+start, i-start);
  if (svg_text[i] && (i < len))
  {
   if (first)
   {
    F(post).putLine2(post, type, fput);
    first = FALSE;
   }
   else
    F(post).putLine2(post, type, "Tj");
   put_font(post, type, style->font, point_size);
  }
 }
 else
  put_font(post, type, style->font, point_size);
 while (svg_text[i] && (i < len))
 {
  start = i;
  while (svg_text[i] && (i < len) && 
	 (svg_text[i]==Xc_TO_UPPER(svg_text[i])))
   i++;
  F(post).putString2(post, type, svg_text+start, i-start);
  if (!svg_text[i] || (i >= len))
   break;
  if (first)
  {
   F(post).putLine2(post, type, fput);
   first = FALSE;
  }
  else
   F(post).putLine2(post, type, "Tj");
  put_font(post, type, style->font, point_small);
  start = i;
  while (svg_text[i] && (i < len) && 
	 (svg_text[i]!=Xc_TO_UPPER(svg_text[i])))
  {
   svg_text[i]=Xc_TO_UPPER(svg_text[i]);
   i++;
  }
  F(post).putString2(post, type, svg_text+start, i-start);
  if (!svg_text[i] || (i >= len))
   break;
  if (first)
  {
   F(post).putLine2(post, type, fput);
   first = FALSE;
  }
  else
   F(post).putLine2(post, type, "Tj");
  put_font(post, type, style->font, point_size);
 }
 if (first)
 {
  F(post).putLine2(post, type, fput);
  first = FALSE;
 }
 else
  F(post).putLine2(post, type, "Tj");
}

static void underline(style, text, len, post, type, is_shadow, c_x, c_y)
c_TextStyle	*style;
char		*text;
int		len;
c_PostScript	*post;
int		type;
boolean		is_shadow;
coord_t		c_x;
coord_t		c_y;
{
 coord_t	width;
 coord_t	inc_dx;
 coord_t	inc_dy;
 int		utype;

 inc_dx = c_x;
 inc_dy = c_y;
 inc_dy += post->state2[type].text_rise;
 if (is_shadow)
 {
  inc_dx += style->shadow.internal_offset.dx;
  inc_dy += -style->shadow.internal_offset.dy;
  if(!F(style->shadow.color).ps_print2(style->shadow.color, post, type+GRAPH,
				       TRUE))
   return;
 }
 else
 {
  if(!F(style->underline.color).ps_print2(style->underline.color, post, 
					  type+GRAPH, TRUE))
   return;
 }

 F(post).putLine2(post, type+GRAPH, "$c w", 
		  style->underline.internal_thickness);
#if 0
 printf("size: %f\n", 
	SCALE_TO_MILLIMETERS(style->underline.internal_thickness));
#endif
 utype = style->underline.type+1;
 if (utype == 3)
  inc_dy += 2*style->underline.internal_thickness;
 while (utype)
 {
  if (!style->underline.overspace)
  {
   width = F(style->metrics).textWidthSpec
    (style->metrics, text, len,
     post->state.char_space / XcTS_GET_WIDTH(style),
     post->state.word_space / XcTS_GET_WIDTH(style));
   
   F(post).putLine2(post, type + GRAPH, "$c $c m", 
		    F(post->matrix).convertX
		    (post->matrix, inc_dx, 
		     -style->underline.internal_dy + inc_dy),
		    F(post->matrix).convertY
		    (post->matrix, inc_dx, 
		     -style->underline.internal_dy + inc_dy));

   F(post).putLine2(post, type + GRAPH, "$c $c l", 
		    F(post->matrix).convertX
		    (post->matrix, width + inc_dx, 
		     -style->underline.internal_dy + inc_dy),
		    F(post->matrix).convertY
		    (post->matrix, width + inc_dx, 
		     -style->underline.internal_dy + inc_dy));

   F(post).putLine2(post, type + GRAPH, "S");
  }
  else
  {
   int				i;
   int				start;
   coord_t			space_width;
   coord_t			space_fill;
   coord_t			last_pos;
   complex_char_metrics_t	*m;

   if((m = style->metrics->glyph[' ' - Xc_FIRST_CHAR]) != NULL)
    space_width = m->width[0].dx;
   else
    space_width = 0;
   space_fill = style->underline.internal_overstroke;

   i = 0;
   last_pos = inc_dx;
   while (i < len)
   {
    start = i;
    while ((i < len) && !Xc_IS_PUNCTUATION(text[i]))
     i++;

    if (i == start) 
    {
     i++;
     last_pos += space_width + (post->state.word_space/XcTS_GET_WIDTH(style));
     continue;
    }
    
    width = F(style->metrics).textWidth(style->metrics,
					text+start,
					i-start); 
    width += (i - start) * 
     (post->state.char_space / XcTS_GET_WIDTH(style));

    F(post).putLine2(post, type+GRAPH, "$c $c m", 
		     F(post->matrix).convertX
		     (post->matrix, 
		      last_pos-space_fill,
		      -style->underline.internal_dy+inc_dy),
		     F(post->matrix).convertY
		     (post->matrix,
		      last_pos-space_fill,
		      -style->underline.internal_dy+inc_dy));
    F(post).putLine2(post, type+GRAPH, "$c $c l", 
		     F(post->matrix).convertX
		     (post->matrix, 
		      last_pos+width+space_fill,
		      -style->underline.internal_dy+inc_dy),
		     F(post->matrix).convertY
		     (post->matrix,
		      last_pos+width+space_fill,
		      -style->underline.internal_dy+inc_dy));
    F(post).putLine2(post, type+GRAPH, "S");
    last_pos += width+space_width+
     (post->state.word_space / XcTS_GET_WIDTH(style));
    i++;
   }
  }
  inc_dy -= 2*style->underline.internal_thickness;
  utype--;
 }
}

static void strikeout(style, text, len, post, type, is_shadow, c_x, c_y)
c_TextStyle	*style;
char		*text;
int		len;
c_PostScript	*post;
int		type;
boolean		is_shadow;
coord_t		c_x;
coord_t		c_y;
{
 coord_t	width;
 coord_t	inc_dx;
 coord_t	inc_dy;
 int		utype;

 inc_dx = c_x;
 inc_dy = c_y;
 inc_dy += post->state2[type].text_rise;
 if (is_shadow)
 {
  inc_dx += style->shadow.internal_offset.dx;
  inc_dy += -style->shadow.internal_offset.dy;
  if(!F(style->shadow.color).ps_print2(style->shadow.color, post, type+GRAPH,
				       TRUE))
   return;
 }
 else
 {
  if(!F(style->strikeout.color).ps_print2(style->strikeout.color, post,
					  type+GRAPH, TRUE))
   return;
 }

 F(post).putLine2(post, type+GRAPH, "$c w", 
		  style->strikeout.internal_thickness);
 utype = style->strikeout.type+1;
 if (utype == 3)
  inc_dy += 2*style->strikeout.internal_thickness;
 while (utype)
 {
  if (!style->strikeout.overspace)
  {
   width = F(style->metrics).textWidthSpec
    (style->metrics, text, len,
     post->state.char_space / XcTS_GET_WIDTH(style),
     post->state.word_space / XcTS_GET_WIDTH(style));

   F(post).putLine2(post, type+GRAPH, "$c $c m", 
		    F(post->matrix).convertX
		    (post->matrix, 
		     inc_dx, 
		     -style->strikeout.internal_dy+inc_dy),
		    F(post->matrix).convertY
		    (post->matrix, 
		     inc_dx,
		     -style->strikeout.internal_dy+inc_dy));
   F(post).putLine2(post, type+GRAPH, "$c $c l", 
		    F(post->matrix).convertX
		    (post->matrix, 
		     width+inc_dx,
		     -style->strikeout.internal_dy+inc_dy),
		    F(post->matrix).convertY
		    (post->matrix, 
		     width+inc_dx,
		     -style->strikeout.internal_dy+inc_dy));
   F(post).putLine2(post, type+GRAPH, "S");
  }
  else
  {
   int				i;
   int				start;
   coord_t			space_width;
   coord_t			space_fill;
   coord_t			last_pos;
   complex_char_metrics_t	*m;

   if((m = style->metrics->glyph[' ' - Xc_FIRST_CHAR]) != NULL)
    space_width = m->width[0].dx;
   else
    space_width = 0;
   space_fill = style->strikeout.internal_overstroke;

   i = 0;
   last_pos = inc_dx;
   while (i < len)
   {
    start = i;
    while ((i < len) && !Xc_IS_PUNCTUATION(text[i]))
     i++;

    if (i == start) 
    {
     i++;
     last_pos += space_width + (post->state.word_space/XcTS_GET_WIDTH(style));
     continue;
    }

    width = F(style->metrics).textWidth(style->metrics,
					text+start,
					i-start);
    width += (i - start) * 
     (post->state.char_space / XcTS_GET_WIDTH(style));
    F(post).putLine2(post, type+GRAPH, "$c $c m", 
		     F(post->matrix).convertX
		     (post->matrix, 
		      last_pos-space_fill,
		      -style->strikeout.internal_dy+inc_dy),
		     F(post->matrix).convertY
		     (post->matrix, 
		      last_pos-space_fill,
		      -style->strikeout.internal_dy+inc_dy));
    F(post).putLine2(post, type+GRAPH, "$c $c l", 
		     F(post->matrix).convertX
		     (post->matrix, 
		      last_pos+width+space_fill,
		      -style->strikeout.internal_dy+inc_dy),
		     F(post->matrix).convertY
		     (post->matrix, 
		      last_pos+width+space_fill,
		      -style->strikeout.internal_dy+inc_dy));
    F(post).putLine2(post, type+GRAPH, "S");
    last_pos += width+space_width+
     (post->state.word_space / XcTS_GET_WIDTH(style));
    i++;
   }
  }
  inc_dy -= 2*style->strikeout.internal_thickness;
  utype--;
 }
}

static void put_font(post, type, font, size)
c_PostScript	*post;
int		type;
c_VectorFont	*font;
coord_t		size;
{
 postscript_state_t *state;

 state = &(post->state2[type]);
 if((state->font != font) || (state->point_size != size))
 {
  Xc_ASSERT(font->id > 0);
  F(post).addFont(post, font);
  if(post->ai_mode?
     !F(post).putLine2(post, type, "/_$s $c Tf",
		       font->font_name, size) :
     !F(post).putLine2(post, type, "/F$d $c Tf", font->id, size))
   return;
  state->font = font;
  state->point_size = size;
 }
}

/* ----------------------------------------------------------------- ** 
** bold on                                                           ** 
** ----------------------------------------------------------------- */
static void bold_on(style, font)
c_TextStyle *style;
c_VectorFont **font;
{
 family_tree	*family;
 font_list	*fontl;

 family = (*font)->spec_family;
 fontl = family->list;
 if ((*font)->specific->is_italic)
 {
  if (!family->can_bold_italic) return;
  while(fontl && !fontl->is_bold_italic)
   fontl = fontl->Next;
 }
 else
 {
  if (!family->can_bold)
  {
   if (!family->can_bold_italic) return;
   while(fontl && !fontl->is_bold_italic)
    fontl = fontl->Next;
   if (fontl) style->attributes |= XcTS_B_ITALIC;
  }
  else
  {
   while(fontl && !fontl->is_bold)
    fontl = fontl->Next;
  }
 }
 if (!fontl) 
  return;
 style->attributes |= XcTS_B_BOLD;
 *font = fontl->font;
}

/* ----------------------------------------------------------------- ** 
** bold off                                                           ** 
** ----------------------------------------------------------------- */
static void bold_off(style, font)
c_TextStyle *style;
c_VectorFont **font;
{
 family_tree	*family;
 font_list	*fontl;

 family = (*font)->spec_family;
 fontl = family->list;
 if ((*font)->specific->is_bold_italic)
 {
  if (!family->can_italic)
  {
   if (!family->can_normal) return;
   while(fontl && !fontl->is_normal)
    fontl = fontl->Next;
   if (fontl) style->attributes &= ~XcTS_B_ITALIC;
  }
  else
  {
   while(fontl && !fontl->is_italic)
    fontl = fontl->Next;
  }
 }
 else
 {
  if (!family->can_normal) return;
  while(fontl && !fontl->is_normal)
   fontl = fontl->Next;
 }
 if (!fontl) return;
	  
 style->attributes &= ~XcTS_B_BOLD;
 *font = fontl->font;
}

/* ----------------------------------------------------------------- ** 
** italic on                                                         ** 
** ----------------------------------------------------------------- */
static void italic_on(style, font)
c_TextStyle *style;
c_VectorFont **font;
{
 family_tree	*family;
 font_list	*fontl;

 family = (*font)->spec_family;
 fontl = family->list;
 if ((*font)->specific->is_bold)
 {
  if (!family->can_bold_italic) return;
  while(fontl && !fontl->is_bold_italic)
   fontl = fontl->Next;
 }
 else
 {
  if (!family->can_italic) 
  {
   if (!family->can_bold_italic) return;
   while(fontl && !fontl->is_bold_italic)
    fontl = fontl->Next;
   if (fontl) style->attributes |= XcTS_B_BOLD;
  }
  else
  {
   while(fontl && !fontl->is_italic)
    fontl = fontl->Next;
  }
 }
 if (!fontl) return;
	  
 style->attributes |= XcTS_B_ITALIC;
 *font = fontl->font;
}

/* ----------------------------------------------------------------- ** 
** italic off                                                        ** 
** ----------------------------------------------------------------- */
static void italic_off(style, font)
c_TextStyle *style;
c_VectorFont **font;
{
 family_tree	*family;
 font_list	*fontl;

 family = (*font)->spec_family;
 fontl = family->list;
 if ((*font)->specific->is_bold_italic)
 {
  if (!family->can_bold)
  {
   if (!family->can_normal) return;
   while(fontl && !fontl->is_normal)
    fontl = fontl->Next;
   if (fontl) style->attributes &= ~XcTS_B_BOLD;
  }
  else
  {
   while(fontl && !fontl->is_bold)
    fontl = fontl->Next;
  }
 }
 else
 {
  if (!family->can_normal) return;
  while(fontl && !fontl->is_normal)
   fontl = fontl->Next;
 }
 if (!fontl) return;
	  
 style->attributes &= ~XcTS_B_ITALIC;
 *font = fontl->font;
}

/* ----------------------------------------------------------------- ** 
** compute shadow color - return a newly generated color             ** 
** ----------------------------------------------------------------- */
static c_Color	*compute_shadow_color(color)
c_Color *color;
{
 c_Color *shadow_color;
 ycbcr_t *ycbcr;
 real  y;
 
 ycbcr = F(color).getYCbCr(color);
 y = XcC_DESCALE_COLOR(double, ycbcr->y);
 if (y > 0.5) y -= 0.3; else y += 0.3;
 
 shadow_color = F(color).getColor(color->base,
				  XcC_GRAY, XcC_SCALE_COLOR(y),
				  XcC_HIDE_ON, XcC_END);
 return shadow_color;
}

#if 0
/* ----------------------------------------------------------------- ** 
** create_inheritance - create Textstyle inheritance                 ** 
** ----------------------------------------------------------------- */
static void create_inheritance(this, inherited, attributes)
c_TextStyle	*this;
c_TextStyle	*inherited;
unsigned int	attributes;
{
}
#endif

/* ----------------------------------------------------------------- ** 
** hook_inherit - Textstyle inherit hook                             ** 
** ----------------------------------------------------------------- */
static void hook_inherit(this, reason, old, new)
c_TextStyle	*this;
int		reason;
c_TextStyle	*old;
c_TextStyle	*new;
{
 Xc_TRACE(("hook_inherit"));

 if((reason == XcH_CONTENT_CHANGED) || (reason == XcH_REDRAW))
 {
  /*--- new attrib ? ---*/
  if ((~this->valid_field & this->attributes)!= 
      (~this->valid_field & new->attributes))
  {
   F(this).set(this, XcTS_ATTRIB, 
	       (~this->valid_field & new->attributes) |
	       (this->valid_field & this->attributes),
	       XcTS_END);
  }

  /*--- new font ? ---*/
  if (!(this->valid_field & XcTS_H_FONT))
  {
   if (this->font->spec_family!=new->font->spec_family)
    F(this).set(this, XcTS_FONT_FAMILY, 
		new->font->spec_family,
		XcTS_END);
  }
  /*--- new size ? ---*/
  if (!(this->valid_field & XcTS_H_SIZE))
  {
   if (this->point_size!=new->point_size)
    F(this).set(this, XcTS_SIZE, 
		new->point_size,
		XcTS_END);
  }
  /*--- new color ? ---*/
  if (!(this->valid_field & XcTS_H_COLOR))
  {
   if (this->color!=new->color)
    F(this).set(this, XcTS_COLOR, 
		new->color,
		XcTS_END);    
  }
  /*--- new bgcolor ? ---*/
  if (!(this->valid_field & XcTS_H_BGCOLOR))
  {
   if (this->bgcolor != new->bgcolor)
    F(this).set(this, XcTS_BGCOLOR, 
		new->bgcolor,
		XcTS_END);    
  }
  /* set all changed paramter */  
  if (memcmp(&(this->width), &(new->width), sizeof(this->width)))
   F(this).set(this, XcTS_WIDTH_CONF, new->width.scale, XcTS_END);
  if (memcmp(&(this->underline), &(new->underline), 
	     sizeof(this->underline)))
   F(this).set(this, XcTS_UNDERLINE_CONF, 
	       new->underline.dy, 
	       new->underline.overstroke,
	       new->underline.thickness,
	       new->underline.overspace,
	       new->underline.color,
	       new->underline.type,
	       XcTS_END);
  if (memcmp(&(this->strikeout), &(new->strikeout),
	     sizeof(this->strikeout)))
   F(this).set(this, XcTS_STRIKEOUT_CONF, 
	       new->strikeout.dy, 
	       new->strikeout.overstroke,
	       new->strikeout.thickness,
	       new->strikeout.overspace,
	       new->strikeout.color,
	       new->strikeout.type,
	       XcTS_END);
  if (memcmp(&(this->shadow), &(new->shadow),
	     sizeof(this->shadow)))
   F(this).set(this, XcTS_SHADOW_CONF, 
	       new->shadow.dx, 
	       new->shadow.dy, 
	       new->shadow.color,
	       XcTS_END);
  if (memcmp(&(this->subscript), &(new->subscript),
	     sizeof(this->subscript)))
   F(this).set(this, XcTS_SUBSCRIPT_CONF, 
	       new->subscript.dy, 
	       new->subscript.scale, 
	       XcTS_END);
  if (memcmp(&(this->supscript), &(new->supscript),
	     sizeof(this->supscript)))
   F(this).set(this, XcTS_SUPSCRIPT_CONF, 
	       new->supscript.dy, 
	       new->supscript.scale, 
	       XcTS_END);

 }
}

/* ----------------------------------------------------------------- ** 
** text_style_filter: design to be use as filter in ItemMenu or      **
** ItemList objects.                                                 ** 
** ----------------------------------------------------------------- */
boolean text_style_filter(style)
c_TextStyle *style;
{
 return !style->hide_flag;
}
