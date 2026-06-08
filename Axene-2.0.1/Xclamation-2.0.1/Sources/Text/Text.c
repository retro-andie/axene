/*
** Text.c for Xclamation in Text/
** Methods for the Text class
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
** Started on  Sun Jun 12 02:14:52 1994 Stéphane Boisson
** Last update Sun Jul 27 16:23:04 1997 
*/

#define NTRACE
#define NHISTORY

#include <string.h>
#include "Text.h"
#include "Color.h"

#define MIN_BUFFER_SIZE 4096

static ItemHandle *i_handle = NULL, *ref_handle = NULL;
static unsigned long i_references, ref_references = 0;

/*--- Internal codes in buffer ---*/
#define XcT_SOFT_PAGE_CODE ((char_t) 0x0b)
#define XcT_LINE_CODE ((char_t) 0x0d)        /* \r */

#define XcT_END_CODE ((char_t) 0x1b)
#define XcT_RULER_CODE ((char_t) 0x11)
#define XcT_STYLE_CODE ((char_t) 0x12)
#define XcT_INDEX_CODE ((char_t) 0x13)
#define XcT_SECTION_CODE ((char_t) 0x14)
#define XcT_HYPERTEXT_CODE ((char_t) 0x15)

#define XcT_GET_CODE(ptr, var) \
do { var = ((unsigned int) (*ptr++) << 8); \
      var |= (unsigned int) (*ptr++); } while(0)

#define XcT_SET_CODE(ptr, var) \
do { *ptr++ = (char_t)(((unsigned int)var >> 8) & 0xff); \
      *ptr++ = (char_t)((unsigned int)var & 0xff); } while(0)

#define XcT_REF_CODE_LEN 2

#define XcT_POS_TO_PTR(text, offset) ((char_t *)((offset) + (((offset) >= (text)->gap_pos)? ((text)->gap_len + (text)->buffer_start) : (text)->buffer_start)))

#define XcT_PTR_TO_POS(text, ptr) ((text_pos_t)((ptr) - (((ptr) > (text)->gap_start)? ((text)->gap_len + (text)->buffer_start) : (text)->buffer_start)))


/*--- Prototypes ---*/
static void *contructor();
static void destructor();
static void *copy();
static error changeBase ___PROTO((c_Text *this, BaseStd_t *base));
static void insert_string ___PROTO((c_Text *obj,char_t *string,text_pos_t len));
static void insertCode ___PROTO((c_Text *this, char_t code));
static void delete_string ___PROTO((c_Text *obj, text_pos_t len));
static boolean grow_gap ___PROTO((c_Text *obj, text_pos_t offset));
static boolean grow_buffer ___PROTO((c_Text *obj, text_pos_t grow_size));
static void move ___PROTO((c_Text *obj, text_pos_t pos, boolean save));
/*static char_t *pos_to_ptr();*/
/*static text_pos_t ptr_to_pos();*/
static void make_undo ___PROTO((c_Text *obj));
static void show ___PROTO((c_Text *obj));
static void warn ___PROTO((c_Text *obj));
static text_reference_t *get_previous_reference ___PROTO((c_Text *this,
							  text_pos_t pos,
							  text_ref_type_t code));
static void get_first_seq ___PROTO((c_Text *obj, text_pos_t start,
				    c_TextStyle **first_style,
				    c_TextRuler **first_ruler));
static text_seq_t *get_next_seq ___PROTO((c_Text *obj, text_pos_t end_pos));
static text_reference_t *get_reference_data ___PROTO((c_Text *this,
						      text_ref_type_t type,
						      unsigned int ref));

static text_reference_t *get_reference_id ___PROTO((c_Text *this,
						    text_ref_type_t type,
						    void *data));
static void insert_reference ___PROTO((c_Text *this,
				       text_ref_type_t type,
				       void *data));
static boolean readText ___PROTO((c_DocFile *doc, char *keyword,
				  long param, text_read_t *data));
static boolean writeText ___PROTO((c_Text *this, c_DocFile *doc));
static boolean readList ___PROTO((c_DocFile *doc, char *keyword,
				  long param, BaseStd_t *base));
static boolean writeList ___PROTO((c_Text *this, c_DocFile *doc));
static boolean cb_ruler ___PROTO((c_DocFile *doc, char *keyword,
				  long param, text_read_t *data));
static boolean cb_style ___PROTO((c_DocFile *doc, char *keyword,
				  long param, text_read_t *data));
static boolean cb_name ___PROTO((c_DocFile *doc, char *keyword,
				 long param, c_Text *text));
static void hook_style ___PROTO((c_Text *this, int reason,
                                 c_TextStyle *old, c_TextStyle *new));
static void hook_ruler ___PROTO((c_Text *this, int reason,
                                 c_TextRuler *old, c_TextRuler *new));
static boolean exportText ___PROTO((c_Text *this, BaseStd_t *base,
				    char *filename));


sf_Text fc_Text =
{
 contructor, destructor, copy, changeBase,
 readText, writeText, readList, writeList,

 move,
 insert_string, insertCode,
 insert_reference,
 delete_string,
 make_undo,

 get_first_seq, get_next_seq,

 warn,

 exportText,
 show,
};


/* ----------------------------------------------------------------- ** 
** contructor - Constructor                                          ** 
** ----------------------------------------------------------------- */
static void *contructor(name, base, style, ruler)
char *name;
c_Text **base;
c_TextStyle *style;
c_TextRuler *ruler;
{
 c_Text *p;

 Xc_HISTORY(("constructor(`%s', `%s', `%s', %lx)",
	     name? name : "untitled",
	     style? style->name : "no style",
	     ruler? ruler->name: "no ruler", base));

 if(i_handle == NULL)
  i_handle = Xc_createItem("Text base", 16, sizeof(c_Text));
 if(i_handle == NULL) return NULL;

 if((p = Xc_new(i_handle)) == NULL) return NULL;
 memset(p, 0, sizeof(c_Text));
 if((p->hook = NEW(c_Hook)(p, 2)) == NULL)
 {
  Xc_delete(i_handle, p);
  return NULL;
 }
 p->f = &fc_Text;
 i_references++;

 if(name != NULL) strncpy(p->name, name, XcMAX_NAME_LENGTH - 1);

 /*--- Insert in linked list ---*/
 p->base = base;
 if(base != NULL)
 {
  p->next = *base;
  *base = p;
 }
 else
  p->next = NULL;

 /*--- Initialize default TextStyle ref ---*/
 p->ref_base[0].next = &p->ref_base[1];
 p->ref_base[0].type = XcT_TEXT_STYLE;
 p->ref_base[0].data.style = style;
 p->ref_base[0].count = 1;
 p->ref_base[0].code = 0;
 p->first_style = 0;
 if((style != NULL) && !XcH_REGISTER(style, hook_style, p))
 {
  DELETE(c_Text)(p);
  return NULL;
 }

 /*--- Initialize default TextRuler ref ---*/
 p->ref_base[1].next = NULL;
 p->ref_base[1].type = XcT_TEXT_RULER;
 p->ref_base[1].data.ruler = ruler;
 p->ref_base[1].count = 1;
 p->ref_base[1].code = 1;
 p->first_ruler = 1;
 if((ruler != NULL) && !XcH_REGISTER(ruler, hook_ruler, p))
 {
  DELETE(c_Text)(p);
  return NULL;
 }

 p->sequence = 0;
 
 if(grow_buffer(p, MIN_BUFFER_SIZE) == FALSE) DELETE(c_Text)(p);
 return(p);
}


/* ----------------------------------------------------------------- ** 
** destructor - Destructor                                           ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_Text *this;
{
 text_reference_t *ptr;

 Xc_HISTORY(("destructor(`%s')", this->name));
 Xc_ASSERT(i_references > 0);

 if(this->buffer_start != NULL)
  Xc_free(this->buffer_start);
 if(this->seq_buffer != NULL)
  Xc_free(this->seq_buffer);

 DELETE(c_Hook)(this->hook);

 /*--- un-hook references ---*/
 for(ptr = &this->ref_base[0]; ptr != NULL; ptr = ptr->next)
 {
  if(ptr->data.pointer == NULL) continue;
  if(ptr->type == XcT_TEXT_RULER)
   XcH_UNREGISTER(ptr->data.ruler, hook_ruler, this);
  if(ptr->type == XcT_TEXT_STYLE)
   XcH_UNREGISTER(ptr->data.style, hook_style, this);
 }

 /*--- Free references ---*/
 ptr = this->ref_base[1].next;
 if(ptr != NULL)
 {
  while(ptr != NULL)
  {
   text_reference_t *tmp;
	  
   Xc_ASSERT(ref_references > 0);
   tmp = ptr;
   ptr = ptr->next;
   Xc_delete(ref_handle, tmp);
   ref_references--;
  }
  if(ref_references == 0)
  {
   Xc_destroyItem(ref_handle);
   ref_handle = NULL;
  }
 }

 /*--- Remove from linked list ---*/
 if(this->base != NULL)
 {
  c_Text **ptr;

  for(ptr = this->base; *ptr != this; ptr = &((*ptr)->next))
   Xc_ASSERT(*ptr != NULL);
  *ptr = this->next;
 }

 Xc_delete(i_handle, this);
 if(--i_references == 0)
 {
  Xc_destroyItem(i_handle);
  i_handle = NULL;
 }
}


/* ----------------------------------------------------------------- ** 
** copy - Copy method                                                ** 
** ----------------------------------------------------------------- */
static void *copy(this, base)
c_Text *this;
BaseStd_t *base;
{
 text_reference_t *ptr, **link, *target;
 c_Text *p;
 size_t len;
  
 Xc_HISTORY(("copy(`%s')", this->name));

 if((p = Xc_new(i_handle))==NULL)
  return NULL;
 memcpy(p, this, sizeof(c_Text));
 if((p->hook = NEW(c_Hook)(p, 2)) == NULL)
 {
  Xc_delete(i_handle, p);
  return NULL;
 }

 /*--- Link ---*/
 p->base = &(base->text_base);
 p->next = base->text_base;
 base->text_base = p;
 p->ref_base[0].next = &p->ref_base[1];

 p->buffer_start = p->buffer_end = p->gap_start = p->gap_end = NULL;
 p->seq_buffer = NULL; 

 /*--- Copy text ---*/
 len = this->buffer_end - this->buffer_start;
 if((p->buffer_start = Xc_malloc(p->name, len * sizeof(char_t))) == NULL)
 {
  DELETE(c_Hook)(p->hook);
  Xc_delete(i_handle, p);
  return NULL;
 }
 memcpy(p->buffer_start, this->buffer_start, len);
 p->buffer_end = p->buffer_start + len;
 p->gap_start = p->buffer_start + p->gap_pos;
 p->gap_end = p->gap_start + p->gap_len;
 i_references++;

 /*--- Allocate references ---*/
 p->ref_base[0].type = XcT_UNKNOWN;
 p->ref_base[1].type = XcT_UNKNOWN;
 link = &(p->ref_base[1].next);
 for(ptr = this->ref_base[1].next; ptr != NULL; ptr = ptr->next)
 {
  Xc_ASSERT(ptr->data.pointer != NULL);
  if((*link = Xc_new(ref_handle)) == NULL)
  {
   DELETE(c_Text)(p);
   return NULL;
  }
  ref_references++;

  (*link)->type = XcT_UNKNOWN;
  (*link)->data.pointer = ptr->data.pointer;
  (*link)->code = ptr->code;
  (*link)->count = ptr->count;

  link = &((*link)->next);
 }
 *link = NULL;

 /*--- Copy references ---*/
 target = &p->ref_base[0];
 ptr = &this->ref_base[0];
 for( ; target != NULL; ptr = ptr->next)
 {
  switch(ptr->type)
  {
   /*--- TextRuler reference ---*/
  case XcT_TEXT_RULER:
   {
    c_TextRuler *ruler;
	    
    ruler = ptr->data.ruler;
    ruler = F(ruler).virtualCopy(ruler, base);
    if(((target->data.ruler = ruler) == NULL) ||
       !XcH_REGISTER(ruler, hook_ruler, p)) target = NULL;
    break;
   }

   /*--- TextStyle reference ---*/
  case XcT_TEXT_STYLE:
   {
    c_TextStyle *style;
	    
    style = ptr->data.style;
    style = F(style).virtualCopy(style, base);
    if(((target->data.style = style) == NULL) ||
       !XcH_REGISTER(style, hook_style, p)) target = NULL;
    break;
   }

   /*--- Other references ---*/
  default:
   break;
  }

  if(target == NULL)
  {
   DELETE(c_Text)(p);
   return NULL;
  }
  target->type = ptr->type;
  target = target->next;
 }

 return p;
}


/* ----------------------------------------------------------------- ** 
** changeBase - Change reference base                                ** 
** ----------------------------------------------------------------- */
static error changeBase(this, base)
c_Text *this;
BaseStd_t *base;
{
 text_reference_t *ptr;

 Xc_TRACE(("changeBase"));

 /*--- Remove from linked list ---*/
 if(this->base != NULL)
 {
  c_Text **ptr;

  for(ptr = this->base; *ptr != this; ptr = &((*ptr)->next))
   Xc_ASSERT(*ptr != NULL);
  *ptr = this->next;
 }

 /*--- Insert in new base ---*/
 this->base = &base->text_base;
 this->next = base->text_base;
 base->text_base = this;

 /*--- Change references bases ---*/
 for(ptr = &this->ref_base[0]; ptr != NULL; ptr = ptr->next)
 {
  switch(ptr->type)
  {
   /*--- TextRuler reference ---*/
  case XcT_TEXT_RULER:
   {
    c_TextRuler *ruler;
	    
    ruler = ptr->data.ruler;
    if(((ruler = F(ruler).virtualCopy(ruler, base)) != NULL) &&
       XcH_REGISTER(ruler, hook_ruler, this))
    {
     XcH_UNREGISTER(ptr->data.ruler, hook_ruler, this);
     ptr->data.ruler = ruler;
    }
    else
     ptr = NULL;
    break;
   }

   /*--- TextStyle reference ---*/
  case XcT_TEXT_STYLE:
   {
    c_TextStyle *style;
	    
    style = ptr->data.style;
    if(((style = F(style).virtualCopy(style, base)) != NULL) &&
       XcH_REGISTER(style, hook_style, this))
    {
     XcH_UNREGISTER(ptr->data.style, hook_style, this);
     ptr->data.style = style;
    }
    else
     ptr = NULL;
    break;
   }

   /*--- Other references ---*/
  default:
   break;
  }

  if(ptr == NULL) return XC_MEMORY_ALLOCATION;
 }

 return XC_NO_ERROR;
}


/* ----------------------------------------------------------------- ** 
** insert_reference - Insert a reference in the text                 ** 
** ----------------------------------------------------------------- */
static void insert_reference(this, type, data)
c_Text *this;
text_ref_type_t type;
void *data;
{
 char_t buffer[2 + XcT_REF_CODE_LEN], *ptr;
 text_reference_t *ref;

 Xc_HISTORY(("insertReference(`%s')", this->name));

 if((ref = get_reference_id(this, type, data)) == NULL) return;
 ref->count++;
 if(this->pos == 0)
 {
  if(type == XcT_TEXT_RULER)
  {
   text_reference_t *old;

   old = get_reference_data(this, type, this->first_ruler);
   old->count--;
   this->first_ruler = ref->code;
   return;
  }
  if(type == XcT_TEXT_STYLE)
  {
   text_reference_t *old;

   old = get_reference_data(this, type, this->first_style);
   old->count--;
   this->first_style = ref->code;
   return;
  }
 }
 ptr = buffer;
 *ptr++ = (char_t)type + XcT_RULER_CODE;
 XcT_SET_CODE(ptr, ref->code);
 *ptr = XcT_END_CODE;
 insert_string(this, buffer, 2 + XcT_REF_CODE_LEN);
}


/* ----------------------------------------------------------------- ** 
** get_reference_id - Search for the reference in ref-list           ** 
**                    If it doesn't exist yet, create it             ** 
** ----------------------------------------------------------------- */
static text_reference_t *get_reference_id(this, type, data)
c_Text *this;
text_ref_type_t type;
void *data;
{
 unsigned int ref;
 text_reference_t *ptr, *new;
  
 Xc_TRACE(("get_reference_id(`%s')", this->name));
 
 ptr = &this->ref_base[0];
 while(42)
 {
  if((ptr->type == type) && (ptr->data.pointer == data))
   return(ptr);
  if(ptr->next == NULL) break;
  ptr = ptr->next;
 }
 
 Xc_TRACE(("New reference for `%s' in `%s'", ((c_TextRuler *)data)->name,
	   this->name));
 if(ref_handle == NULL) ref_handle = Xc_createItem("TextReferences base", 32,
						   sizeof(text_reference_t));
 if((new = Xc_new(ref_handle)) == NULL) return NULL;
 ref_references++;
 new->type = type;
 new->data.pointer = data;
 new->count = 0;
 new->code = 0;
 
 if(type == XcT_TEXT_RULER)
 {
  if(!XcH_REGISTER((c_TextRuler *)data, hook_ruler, this))
  {
   Xc_delete(ref_handle, new);
   return NULL;
  }
 }
 else if(type == XcT_TEXT_STYLE)
  if(!XcH_REGISTER((c_TextStyle *)data, hook_style, this))
  {
   Xc_delete(ref_handle, new);
   return NULL;
  }
 new->next = this->ref_base[1].next;
 
 
 ref = 1;
 do 
 {
  ref++;
  for(ptr = this->ref_base[1].next; ptr != NULL; ptr = ptr->next)
   if(ptr->code == ref) break;
 }
 while(ptr != NULL);
 
 new->code = ref;
 this->ref_base[1].next = new;
 return(new);
}


/* ----------------------------------------------------------------- ** 
** get_reference_data - Get TextReference from a reference number    ** 
** ----------------------------------------------------------------- */
static text_reference_t *get_reference_data(this, type, ref)
c_Text *this;
text_ref_type_t type;
unsigned int ref;
{
 text_reference_t *ptr;
  
 Xc_TRACE(("get_reference_data(`%s', %d)", this->name, ref));

 ptr = &this->ref_base[0];
 while(42)
 {
  if(ptr->code == ref)
  {
   Xc_ASSERT(ptr->type == type);
   return(ptr);
  }
  if(ptr->next == NULL) break;
  ptr = ptr->next;
 }

 /*--- Take default reference ---*/
 Xc_WARNING(("Reference %d in `%s' doesnt exist", ref, this->name)); 
#if 0
 /* dump ref base */
 ptr = &this->ref_base[0];
 while(ptr)
 {
  switch(ptr->type)
  {
  case XcT_TEXT_RULER:
   printf("code: %d -- ruler name:%s\n", ptr->code, ptr->data.ruler->name);
   break;
  case XcT_TEXT_STYLE:
   printf("code: %d -- style name:%s\n", ptr->code, ptr->data.style->name);
   break;
  default:
   printf("unknown type\n");
  }
  ptr = ptr->next;
 }
#endif

 ptr = &this->ref_base[0];
 while(42)
 {
  if(ptr->type == type)
   return(ptr);
  if(ptr->next == NULL) break;
  ptr = ptr->next;
 }
 return(NULL);
}


/* ----------------------------------------------------------------- ** 
** insert_string - Insert a string at current position               ** 
**                 (with Undo)                                       ** 
** ----------------------------------------------------------------- */
static void insert_string(this, text, len)
c_Text *this;
char_t *text;
text_pos_t len;
{
 text_pos_t shift_len, count;
  
 Xc_HISTORY(("insertString(`%s')", this->name));
 Xc_ASSERT(this->gap_start <= this->gap_end);

#ifndef NTRACE
 {
  long i;

  for(i = 0; i < len; i++)
   if(text[i] >= 32)
    putc((text[i] == 32)? '_' : text[i], stderr);
   else
    fprintf(stderr, "<%x>", (int)text[i]);

  putc('\n', stderr);
 }
#endif

 /*--- Check is enough gap space ---*/
 if((this->gap_len - this->gap_save_len) < len)
  if(grow_buffer(this, len - this->gap_len + this->gap_save_len) == FALSE)
   return;

 if(this->gap_direction == FALSE)
 {
  /*___Gap save part is first___*/
  shift_len = this->pos - this->gap_pos;
  if(shift_len)
   memmove(this->gap_end - len,
	   this->gap_end,
	   shift_len * sizeof(char_t));
  this->gap_end -= len;
  memcpy(this->gap_end + shift_len, text, len * sizeof(char_t));

  /*--- Remove tab codes wathing out for special code ---*/
  for(text = this->gap_end + shift_len, count = len; count--; text++)
   if ((*text >= XcT_RULER_CODE) && (*text <= XcT_RULER_CODE+XcT_UNKNOWN))
   {
    count -= 3;
    text += 3;
   }
   else
    if(*text == XcT_TABULATOR_CODE) 
     *text = XcT_SPACE_CODE;
 }
 else
 {
  /*___Gap save part is last___*/
  shift_len = this->gap_pos - this->pos;
  if(shift_len)
   memmove(this->gap_start - shift_len + len,
	   this->gap_start - shift_len,
	   shift_len * sizeof(char_t));
  memcpy(this->gap_start - shift_len, text, len * sizeof(char_t));

  /*--- Remove tab codes wathing out for special code ---*/
  for(text = this->gap_end - shift_len, count = len; count--; text++)
   if ((*text >= XcT_RULER_CODE) && (*text <= XcT_RULER_CODE+XcT_UNKNOWN))
   {
    count -= 3;
    text += 3;
   }
   else
    if(*text == XcT_TABULATOR_CODE) 
     *text = XcT_SPACE_CODE;

  this->gap_start += len;
  this->gap_pos += len;
 }

 this->gap_len -= len;
 this->pos += len;
 this->gap_no_save += len;
 this->modified_flag = TRUE;

 /*show(this);*/
}


/* ----------------------------------------------------------------- ** 
** delete_string - Delete a string in buffer                         ** 
**                 (with Undo)                                       ** 
** ----------------------------------------------------------------- */
static void delete_string(this, len)
c_Text *this;
text_pos_t len;
{
 text_pos_t shift_len;
  
 Xc_HISTORY(("deleteString(`%s')", this->name));
 Xc_ASSERT((this->gap_end + len) < this->buffer_end);

 if(this->gap_direction == FALSE)
 {
  /*___Gap save part is first___*/
  shift_len = this->pos - this->gap_pos;
  if(this->gap_no_save < (shift_len + len))
  {
   if((this->gap_len < (shift_len + len)) &&
      (grow_buffer(this, shift_len + len - this->gap_len) == FALSE))
   {
    /*___Undo could be disabled here___*/
    Xc_WARNING(("incomplete undo buffer for `%s'", this->name));
    return;
   }
   memcpy(this->gap_start + this->gap_save_len,
	  this->gap_end + shift_len,
	  len * sizeof(char_t));
   this->gap_save_len += len;
   this->gap_no_save += len;
  }
            
  if(shift_len)
   memmove(this->gap_end + len,
	   this->gap_end,
	   shift_len * sizeof(char_t));
  this->gap_end += len;
 }
 else
 {
  /*___Gap save part is last___*/
  while((shift_len = (this->gap_pos - this->pos)) < len)
   if(grow_gap(this, len - shift_len) == FALSE)
   {
    /*___Undo could be disabled here___*/
    Xc_WARNING(("incomplete undo buffer for `%s'", this->name));
    return;
   }
            
  shift_len -= len;
  if(shift_len)
   memmove(this->gap_start - shift_len,
	   this->gap_start - shift_len + len,
	   shift_len * sizeof(char_t));
  this->gap_start -= len;
  this->gap_pos -= len;
 }

 this->gap_len += len;
 this->gap_no_save -= len;
 this->modified_flag = TRUE;
 /*show(this);*/
}


/* --------------------------------------------------------------- ** 
** grow_gap - Move gap to a new pos without changing Undo data     ** 
**            (absolute relative move)                             **
** --------------------------------------------------------------- */
static boolean grow_gap(this, offset)
c_Text *this;
text_pos_t offset;
{
 text_pos_t space;
  
 Xc_TRACE(("grow_gap(`%s', %ld)", this->name, offset));
 
 while((space = (this->gap_len - this->gap_save_len)) < offset)
  if(grow_buffer(this, offset - space) == FALSE)
   return FALSE;
  
 if(this->gap_direction == FALSE)
 {
  /*___Gap save part is first___*/
  Xc_ASSERT((this->gap_start - offset) >= this->buffer_start);
  this->gap_start -= offset;
  this->gap_end -= offset;
  this->gap_pos -= offset;
  memcpy(this->gap_end, this->gap_start, offset * sizeof(char_t));
 }
 else
 {
  /*___Gap save part is last___*/
  Xc_ASSERT((this->gap_end + offset) <= this->buffer_end);
  memcpy(this->gap_start, this->gap_end, offset * sizeof(char_t));
  this->gap_start += offset;
  this->gap_end += offset;
  this->gap_pos += offset;
 }
 this->gap_no_save += offset;
 this->gap_save_len += offset;

 /*show(this);*/
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** grow_buffer - Grow text buffer                                    ** 
** ----------------------------------------------------------------- */
static boolean grow_buffer(this, grow_size)
c_Text *this;
text_pos_t grow_size;
{
 char_t *new;

 Xc_TRACE(("grow_buffer(`%s', %lu)", this->name, grow_size));

 if(grow_size < MIN_BUFFER_SIZE) grow_size = MIN_BUFFER_SIZE;

 /*--- If buffer not allocated yet, then initialize ---*/
 if(this->buffer_start == NULL)
 {
  new = (char_t *) Xc_malloc(this->name, grow_size * sizeof(char_t));
  if(new == NULL) return FALSE;
  this->buffer_start = this->gap_start = new;
  this->buffer_end = this->gap_end = new + grow_size;
  this->pos = this->gap_pos = this->gap_no_save = this->gap_save_len = 0;
  this->gap_len = grow_size;
  this->gap_direction = FALSE;
  this->save_modified_flag = this->modified_flag = FALSE;
      
  /*show(this);*/
  return TRUE;
 }
 else
 {      
  text_pos_t oldlen, newlen;
      
  Xc_ASSERT(this->buffer_start <= this->gap_start);
  Xc_ASSERT(this->gap_start <= this->gap_end);
  Xc_ASSERT(this->gap_end <= this->buffer_end);

  oldlen = this->buffer_end - this->buffer_start;
  newlen = oldlen + (grow_size * sizeof(char_t));

  new = (char_t *)Xc_realloc(this->name,
			     this->buffer_start,
			     (size_t) newlen);
  if(new == NULL) return FALSE;
    
  this->buffer_start = new;
  this->buffer_end = new + newlen;
  this->gap_start = new + this->gap_pos;
  this->gap_len += grow_size;
  this->gap_end = this->gap_start + this->gap_len;
  if(this->gap_end != this->buffer_end)
   memmove(this->gap_end,
	   this->gap_end - grow_size,
	   (this->buffer_end - this->gap_end) * sizeof(char_t));
      
 }  
 Xc_ASSERT(this->buffer_start < this->buffer_end);
 Xc_ASSERT(this->buffer_start <= this->gap_start);
 Xc_ASSERT(this->gap_start <= this->gap_end);
 Xc_ASSERT(this->gap_end <= this->buffer_end);
 /*show(this);*/
 return TRUE;
}


#if 0

/* ----------------------------------------------------------------- ** 
** pos_to_ptr - Offset to pointer                                    ** 
** ----------------------------------------------------------------- */
static char_t *pos_to_ptr(this, offset)
c_Text *this;
text_pos_t offset;
{
 Xc_TRACE(("pos_to_ptr"));
 if(offset < 0) return(this->buffer_start);
 if(offset >= this->gap_pos) offset += this->gap_len;
 return(this->buffer_start + offset);
}
#define XcT_POS_TO_PTR(text, offset) pos_to_ptr(text, offset)
#endif


#if 0
/* ----------------------------------------------------------------- ** 
** ptr_to_pos - Pointer to offset                                    ** 
** ----------------------------------------------------------------- */
static text_pos_t ptr_to_pos(this, ptr)
c_Text *this;
char_t *ptr;
{
 Xc_TRACE(("ptr_to_pos"));
 Xc_ASSERT(this->buffer_start <= ptr);
 Xc_ASSERT(ptr <= this->buffer_end);

 if(ptr > this->gap_start) ptr -= this->gap_len;
 return(ptr - this->buffer_start);
}
#define XcT_PTR_TO_POS(text, ptr) ptr_to_pos(text, ptr)
#endif


/* ----------------------------------------------------------------- ** 
** make_undo - Swap between the current context ans the saved        ** 
**             context.                                              ** 
** ----------------------------------------------------------------- */
static void make_undo(this)
c_Text *this;
{
 text_pos_t tmp;
 boolean tmp_flag;

 Xc_HISTORY(("undo(`%s')", this->name));
 if((this->gap_no_save == 0) && (this->gap_save_len == 0))
  return;
  
 if(this->gap_direction != FALSE)
 {
  this->gap_start -= this->gap_no_save;
  this->gap_pos -= this->gap_no_save;
  this->gap_end -= this->gap_save_len;
 }
 else
 {
  this->gap_start += this->gap_save_len;
  this->gap_pos += this->gap_save_len;
  this->gap_end += this->gap_no_save;
 }
  
 /*--- Swap `no_save_len' and `save_len' ---*/
 tmp = this->gap_save_len;
 this->gap_save_len = this->gap_no_save;
 this->gap_no_save = tmp;
  
 /*--- Swap `save_pos' and `pos' ---*/
 tmp = this->pos;
 this->pos = this->save_pos;
 this->save_pos = tmp;
  
 /*--- Swap `save_modified_flag' and `modified_flag' ---*/
 tmp_flag = this->modified_flag;
 this->modified_flag = this->save_modified_flag;
 this->save_modified_flag = tmp_flag;
  
 this->gap_direction = ~(this->gap_direction);
 this->gap_len = this->gap_end - this->gap_start;
   
 /*show(this);*/
}



/* ----------------------------------------------------------------- ** 
** move - Move the cursor in the text object                         ** 
** ----------------------------------------------------------------- */
static void move(this, pos, save)
c_Text *this;
text_pos_t pos;
boolean save;
{
 text_pos_t len;
  
 Xc_HISTORY(("move(`%s', %ld, %s UNDO)",
	     this->name, pos, save? "WITH":"WITHOUT"));
 Xc_ASSERT((pos >= 0) && (pos < ((this->buffer_end - this->buffer_start) - this->gap_len)));
  
 if(this->pos == pos) return;
 len = this->gap_pos - pos;
 if((save != FALSE) && (this->gap_save_len || this->gap_no_save))
 {
  if(this->gap_direction != FALSE)
  {
   /*___Save zone is at the end of the gap___*/
   if(len > 0)
   {
    if(len > this->gap_no_save)
    {
     if((len > this->gap_len) &&
	(grow_buffer(this, len - this->gap_len) == FALSE))
     {
      Xc_WARNING(("incomplete undo buffer for `%s'",
		  this->name));
      this->pos = pos;
      return;
     }
     memcpy(this->gap_end - len,
	    this->gap_start - len,
	    (len - this->gap_no_save) * sizeof(char_t));
     this->gap_no_save = len;
    }
   }
   else if(grow_gap(this, -len) == FALSE)
   {
    Xc_WARNING(("incomplete undo buffer for `%s'", this->name));
    this->pos = pos;
    return;
   }
  }
  else
  {
   /*--- Save zone is at the beginning of the gap ---*/
   if(len < 0)
   {
    len = -len;
    if(len > this->gap_no_save)
    {
     if((len > this->gap_len) &&
	(grow_buffer(this, len - this->gap_len) == FALSE))
     {
      Xc_WARNING(("incomplete undo buffer for `%s'",
		  this->name));
      this->pos = pos;
      return;
     }
     memcpy(this->gap_start + this->gap_no_save,
	    this->gap_end + this->gap_no_save,
	    (len - this->gap_no_save) * sizeof(char_t));
     this->gap_no_save = len;
    }
   }
   else if(grow_gap(this, len) == FALSE)
   {
    Xc_WARNING(("incomplete undo buffer for `%s'", this->name));
    this->pos = pos;
    return;
   }
  }	
 }
 else
 {
  /*--- Simple position move (re-init save) ---*/
  if(len > 0)
   memmove(this->gap_end - len,
	   this->gap_start - len,
	   len * sizeof(char_t));
  else
   memmove(this->gap_start,
	   this->gap_end,
	   -len * sizeof(char_t));

  this->gap_start -= len;
  this->gap_end -= len;
  this->gap_pos = this->save_pos = pos;
  this->gap_direction = FALSE;
  this->gap_save_len = this->gap_no_save = 0;
  this->save_modified_flag = this->modified_flag;
 }
  
 this->pos = pos;
  
 /*show(this);*/
}



/* ----------------------------------------------------------------- ** 
** show - Debug function which display the text in the object        ** 
** ----------------------------------------------------------------- */
static void show(this)
c_Text *this;
{
 char_t *ptr;
 text_pos_t pos;
  
 Xc_TRACE(("show(%s)", this->name));

 pos = 0;
 ptr = this->buffer_start;
  
 printf("<START>");
 while(ptr != this->buffer_end)
 {
  if(ptr == this->gap_start)
  {
   text_pos_t i;
   char_t *gapp;
	  
   if((pos == this->pos) && (this->gap_direction != FALSE))
    printf("*"), pos++;
	  
   i = this->gap_save_len;
   if(this->gap_direction == FALSE)
    gapp = ptr;
   else
    gapp = ptr + this->gap_len - i;
	  	  
   /*putchar('[');*/
   /*while(i--) putchar(*gapp++);*/
   /*putchar(']');*/
   printf("[]");

   if((pos == this->pos) && (this->gap_direction == FALSE))
    printf("*"), pos++;
	  
   ptr += this->gap_len;
   if(this->gap_len != 0) continue;
  }

  if(pos == this->pos) printf("*");
  pos++;
      
  switch(*ptr)
  {
  case XcT_SOFT_PAGE_CODE:
   printf("<SOFT-PAGE-BREAK>");
   break;
  case XcT_PAGE_CODE:
   printf("<HARD-PAGE-BREAK>");
   break;
  case XcT_PARAGRAPH_CODE:
   printf("<PARAGRAPH-END>");
   break;
  case XcT_LINE_CODE:
   printf("<LINE-END>");
   break;
  case XcT_RULER_CODE:
  case XcT_STYLE_CODE:
  case XcT_INDEX_CODE:
  case XcT_SECTION_CODE:
  case XcT_HYPERTEXT_CODE:
   ptr += 3;
   pos += 3;
   printf("<REF>");
   break;

  default:
   if(*ptr < 32)
    printf("<%d>",(int) *ptr);
   else
    printf("%c",(char) *ptr);
   break;
  }
  ptr++;
 }
 if(pos == this->pos) printf("*");
 puts("<END>");
 printf("save= %ld  no_save= %ld\n", this->gap_save_len, this->gap_no_save);
  
 Xc_mcheck();
}


/* ----------------------------------------------------------------- ** 
** get_previous_reference - Get previous reference from a pos        ** 
** ----------------------------------------------------------------- */
static text_reference_t *get_previous_reference(this, pos, type)
c_Text *this;
text_pos_t pos;
text_ref_type_t type;
{
 char_t *ptr;

 Xc_TRACE(("get_previous_reference(`%s', %ld)", this->name, pos));

 ptr = XcT_POS_TO_PTR(this, pos);
 while(ptr > this->buffer_start)
 {
  if(ptr == this->gap_end)
  {
   ptr = this->gap_start - 1;
   continue;
  }
  if(*--ptr != XcT_END_CODE) continue;
  ptr -= XcT_REF_CODE_LEN + 1;
   
  Xc_ASSERT(ptr >= this->buffer_start);
  Xc_ASSERT((ptr < this->gap_start) || (ptr >= this->gap_end));
  Xc_ASSERT((*ptr == XcT_STYLE_CODE) || (*ptr == XcT_RULER_CODE) || \
	    (*ptr == XcT_INDEX_CODE) || (*ptr == XcT_SECTION_CODE) || \
	     (*ptr == XcT_HYPERTEXT_CODE));

      if(*ptr == (XcT_RULER_CODE + type))
	{
	  unsigned int ref;

	  ptr++;
	  XcT_GET_CODE(ptr, ref);
	  return get_reference_data(this, type, ref);
	}
    }
  
  if(type == XcT_TEXT_RULER)
    return get_reference_data(this, type, this->first_ruler);

  if(type == XcT_TEXT_STYLE)
    return get_reference_data(this, type, this->first_style);

  return NULL;
}


/* ----------------------------------------------------------------- ** 
** get_first_seq - Get first text sequence                           ** 
** ----------------------------------------------------------------- */
static void get_first_seq(this, start, first_style, first_ruler)
c_Text *this;
text_pos_t start;
c_TextStyle **first_style;
c_TextRuler **first_ruler;
{
 text_reference_t *ref;

 Xc_HISTORY(("getFirstSeq(`%s', %ld)", this->name, start));

 if(this->seq_buffer != NULL)
 {
  Xc_free(this->seq_buffer);
  this->seq_buffer = NULL;
 }

 this->seq_pos = start;

 ref = get_previous_reference(this, start, XcT_TEXT_STYLE);
 Xc_ASSERT(ref != NULL);
 Xc_ASSERT(ref->data.style != NULL);
 *first_style = ref->data.style;
 ref = get_previous_reference(this, start, XcT_TEXT_RULER);
 Xc_ASSERT(ref != NULL);
 Xc_ASSERT(ref->data.ruler != NULL);
 *first_ruler = ref->data.ruler;

 /*show(this);*/
}


/* ----------------------------------------------------------------- ** 
** get_next_seq - Get next text sequence                             ** 
** ----------------------------------------------------------------- */
static text_seq_t *get_next_seq(this, end_pos)
c_Text *this;
text_pos_t end_pos;
{
 boolean out_flag, gap_flag, first_flag, back_flag;
 text_seq_t *seq;
 char_t *ptr, *end_ptr;

 Xc_TRACE(("getNextSeq(`%s', %ld) from %ld",
	   this->name, end_pos, this->seq_pos));

 if(this->seq_buffer != NULL)
 {
  Xc_free(this->seq_buffer);
  this->seq_buffer = NULL;
 }

 seq = &this->seq;
 ptr = XcT_POS_TO_PTR(this, this->seq_pos);
 seq->start_pos = this->seq_pos;
 seq->start = ptr;
 seq->limit_reached = FALSE;
 seq->special = XcT_NOTHING;
 seq->space_count = 0;
 seq->ref = NULL;
 out_flag = gap_flag = back_flag = FALSE;
 first_flag = TRUE;

 if(end_pos > this->seq_pos)
  end_ptr = XcT_POS_TO_PTR(this, end_pos);
 else
  end_ptr = this->buffer_end;

 while(out_flag == FALSE)
 {
  if(ptr == this->gap_start)
  {
   gap_flag = (ptr == seq->start)? FALSE : TRUE;
   ptr += this->gap_end - this->gap_start;
   if(gap_flag == FALSE)
   {
    seq->start = ptr;
    seq->start_pos = XcT_PTR_TO_POS(this, ptr);
   }
  }
	
  if(ptr >= this->buffer_end)
  {
   Xc_TRACE(("End of text"));
   seq->special = XcT_END_OF_TEXT;
   seq->limit_reached = TRUE;
   ptr++;
   break;
  }

  switch(*ptr++)
  {
  case XcT_PAGE_CODE:
  case XcT_SOFT_PAGE_CODE:
   Xc_TRACE(("Page break code"));
   seq->special = XcT_END_OF_PAGE;
   out_flag = TRUE;
   break;
	    
  case XcT_PARAGRAPH_CODE:
   Xc_TRACE(("paragraph code"));
   seq->special = XcT_END_OF_PARAGRAPH;
   out_flag = TRUE;
   break;

  case XcT_LINE_CODE:
   Xc_TRACE(("line break"));
   seq->special = XcT_END_OF_LINE;
   out_flag = TRUE;
   break;
      
  case XcT_END_CODE:
   Xc_WARNING(("Unexpected end-ref in `%s'", this->name));
   break;

  case XcT_RULER_CODE:
  case XcT_STYLE_CODE:
  case XcT_INDEX_CODE:
  case XcT_SECTION_CODE:
  case XcT_HYPERTEXT_CODE:
   Xc_TRACE(("reference code"));
   if(first_flag == FALSE)
   {
    out_flag = TRUE;
    back_flag = TRUE;
    continue;
   }
   else
   {
    unsigned int ref;
    text_ref_type_t type;

    type = ptr[-1] - XcT_RULER_CODE;
    XcT_GET_CODE(ptr, ref);
    if(*ptr++ == XcT_END_CODE)
    {
     seq->start = ptr;
     seq->start_pos = XcT_PTR_TO_POS(this, ptr);
     seq->ref = get_reference_data(this, type, ref);
    }
    else
     Xc_ERROR(("Bad reference in `%s'", this->name));
   }
   break;

  case XcT_SPACE_CODE:
   Xc_TRACE(("space code"));
   seq->space_count++;
   break;
  }
  if(ptr > end_ptr)
  {
   Xc_TRACE(("limit reached"));
   seq->limit_reached = TRUE;
   out_flag = TRUE;
   /*if(seq->special == XcT_NOTHING) back_flag = TRUE;*/
  }
  first_flag = FALSE;
 }

 if(back_flag != FALSE)
 {
  ptr--;
  this->seq_pos = XcT_PTR_TO_POS(this, ptr);
 }
 else
 {
  this->seq_pos = XcT_PTR_TO_POS(this, ptr);
  ptr--;
 }

 /*---The last code is at the gap end---*/
 if((gap_flag != FALSE) && (ptr == this->gap_end))
 {
  seq->len = this->gap_start - seq->start;
  return(seq);
 }

 if(gap_flag == FALSE)
 {
  seq->len = ptr - seq->start; 
 }
 else				/* ---Text sequence contain the gap ---*/
 {
  text_pos_t len;

  seq->len = XcT_PTR_TO_POS(this, ptr) - seq->start_pos;
  this->seq_buffer = Xc_malloc("TMP textseq buffer", seq->len);
  len = this->gap_start - seq->start;
  memcpy(this->seq_buffer, seq->start, len * sizeof(char_t));
  memcpy(this->seq_buffer + len,
	 this->gap_end,
	 (ptr - this->gap_end) * sizeof(char_t));
  seq->start = this->seq_buffer;
 }
 seq->start_pos = XcT_PTR_TO_POS(this, seq->start);

 Xc_TRACE(("end pos: %ld / text=[%*s]", this->seq_pos,
	   (seq->len > 1)? seq->len - 1 : 6,
	   (seq->len > 1)? seq->start : "<none>"));
 return(seq);
}

/* ----------------------------------------------------------------- ** 
** readText - Read method                                            ** 
** ----------------------------------------------------------------- */
static boolean readText(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_read_t *data;
{
 c_Text *text;

 Xc_HISTORY(("readText"));

 if(data->ruler_base == NULL)
 {
  Xc_ERROR(("missing rulers"));
  return FALSE;
 }
 if(data->style_base == NULL)
 {
  Xc_ERROR(("missing styles"));
  return FALSE;
 }

 if((text = NEW(c_Text)(NULL, data->text_base, NULL, NULL)) == NULL)
  return FALSE;
 text->id = param;

 data->text = text;
 if(F(doc).addCallbacks(doc,
			XcDF_NAME_KEYWORD, cb_name, text,
			XcDF_RULER_KEYWORD, cb_ruler, data,
			XcDF_STYLE_KEYWORD, cb_style, data,
			NULL))
  while(42)
  {
   unsigned char *ptr;
   long len;
	
   Xc_TRACE(("read"));
   if(!F(doc).read(doc, &ptr, &len)) break;
   Xc_TRACE(("read ok"));
   if(len < 0)
   {
    Xc_TRACE(("load ok"));
    doc->data_type = XcDF_TEXT_TYPE;
    doc->data_ptr = text;
    return TRUE;
   }
   else if(len > 0)
   {
    F(text).insertString(text, ptr, len);
    Xc_free(ptr);
    Xc_TRACE(("string ok"));
   }
  }
 Xc_TRACE(("loading error"));
 DELETE(c_Text)(text);
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** writeText - Write method                                          ** 
** ----------------------------------------------------------------- */
static boolean writeText(this, doc)
c_Text *this;
c_DocFile *doc;
{
 c_TextStyle *style;
 c_TextRuler *ruler;
 text_seq_t *seq;

 Xc_HISTORY(("writeText(`%s')", this->name));

 /*--- Send message to TextFrame classes to be prepared to be saved ---*/
 F(this->hook).callback(this->hook, XcH_SAVE_PREP, this);

 /*--- Save Text ---*/
 F(this).getFirstSeq(this, 0, &style, &ruler);
 if(!F(doc).writeKeyword(doc, XcDF_TEXT_KEYWORD, this->id) ||
    !F(doc).startSequence(doc) ||
    !F(doc).writeKeyword(doc, XcDF_NAME_KEYWORD, -1L) ||
    !F(doc).startSequence(doc) ||
    !F(doc).write(doc, this->name, strlen(this->name)) ||
    !F(doc).endSequence(doc) ||
    !F(doc).writeKeyword(doc, XcDF_STYLE_KEYWORD, style->id) ||
    !F(doc).writeKeyword(doc, XcDF_RULER_KEYWORD, ruler->id)) return FALSE;
  
 do {
     seq = F(this).getNextSeq(this, 0);
     if(seq->ref != NULL)
      switch(seq->ref->type)
      {
      case XcT_TEXT_RULER:
       if(!F(doc).writeKeyword(doc, XcDF_RULER_KEYWORD,
			       seq->ref->data.ruler->id)) return FALSE;
       break;
      case XcT_TEXT_STYLE:
       if(!F(doc).writeKeyword(doc, XcDF_STYLE_KEYWORD,
			       seq->ref->data.style->id)) return FALSE;
       break;
      case XcT_INDEX:
      case XcT_SECTION:
      case XcT_HYPERTEXT:
       break;
      case XcT_UNKNOWN:
       Xc_BREAK(("XcT_UNKNOWN reference found"));
      }
     if((seq->len > 0) && !F(doc).write(doc, seq->start, seq->len))
      return FALSE;
     if((seq->special == XcT_END_OF_PARAGRAPH) &&
	!F(doc).write(doc, "\n", 1)) return FALSE;
     else if((seq->special == XcT_END_OF_PAGE) &&
	     !F(doc).write(doc, "\f", 1)) return FALSE;
     else if((seq->special == XcT_END_OF_LINE) &&
	     !F(doc).write(doc, "\r", 1)) return FALSE;
    } while(seq->special != XcT_END_OF_TEXT);
 return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** cb_ruler - Ruler keyword loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_ruler(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_read_t *data;
{
 c_TextRuler *ptr;

 Xc_TRACE(("cb_ruler(%ld)", param));

 if(param < 0) param = 0;
 for(ptr = data->ruler_base; ptr != NULL; ptr = ptr->next)
  if(ptr->id == param)
  {
   Xc_TRACE(("Found ruler `%s'", ptr->name));

   if(data->text->pos == 0)
   {
    text_reference_t *ref;

    Xc_TRACE(("replace first ruler"));
    ref = get_reference_data(data->text, XcT_TEXT_RULER,
			     data->text->first_ruler);
    if(ref->data.ruler != NULL)
     XcH_UNREGISTER(ref->data.ruler, hook_ruler, data->text);

    if(!XcH_REGISTER(ptr, hook_ruler, data->text))
     return FALSE;

    ref->data.ruler = ptr;
   }
   else
    F(data->text).insertReference(data->text, XcT_TEXT_RULER, ptr);
   return TRUE;
  }

 Xc_ERROR(("Bad ruler #%ld", param));
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_style - Style keyword loading callback                         ** 
** ----------------------------------------------------------------- */
static boolean cb_style(doc, keyword, param, data)
c_DocFile *doc;
char *keyword;
long param;
text_read_t *data;
{
 c_TextStyle *ptr;

 Xc_TRACE(("cb_style(%ld)", param));

 if(param < 0) param = 0;
 for(ptr = data->style_base; ptr != NULL; ptr = ptr->next)
  if(ptr->id == param)
  {
   Xc_TRACE(("Found style `%s'", ptr->name));

   if(data->text->pos == 0)
   {
    text_reference_t *ref;

    Xc_TRACE(("Replace first style"));
    ref = get_reference_data(data->text, XcT_TEXT_STYLE,
			     data->text->first_style);
    if(ref->data.style != NULL)
     XcH_UNREGISTER(ref->data.style, hook_style, data->text);

    if(!XcH_REGISTER(ptr, hook_style, data->text))
     return FALSE;

    ref->data.style = ptr;
   }
   else
    F(data->text).insertReference(data->text, XcT_TEXT_STYLE, ptr);
   return TRUE;
  }

 Xc_ERROR(("Bad style #%ld", param));
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_name - Name keyword loading callback                           ** 
** ----------------------------------------------------------------- */
static boolean cb_name(doc, keyword, param, text)
c_DocFile *doc;
char *keyword;
long param;
c_Text *text;
{
 unsigned char *name;

 Xc_TRACE(("cb_name"));
 if(!F(doc).readString(doc, &name)) return FALSE;
 if(name != NULL)
 {
  strncpy(text->name, name, XcMAX_NAME_LENGTH - 1);
  Xc_TRACE(("name=`%s'", text->name));
  Xc_free(name);
 }
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** writeList - Write all the text from a base                        ** 
** ----------------------------------------------------------------- */
static boolean writeList(this, doc)
c_Text *this;
c_DocFile *doc;
{
 long id;

 Xc_HISTORY(("writeList"));

 this = *this->base;
  
 if(!F(doc).writeKeyword(doc, XcDF_TEXTLIST_KEYWORD, -1L) ||
    !F(doc).startSequence(doc)) return FALSE;

 for(this = *this->base, id = 0; this != NULL; this = this->next)
 {
  this->id = id++;
  if(!F(this).write(this, doc)) return FALSE;
 }

 return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** readList - read all the text from a list                          ** 
** ----------------------------------------------------------------- */
static boolean readList(doc, keyword, dummy, base)
c_DocFile *doc;
char *keyword;
long dummy;
BaseStd_t *base;
{
 text_read_t param;

 Xc_TRACE(("readList"));

 param.text_base = &base->text_base;
 param.style_base = base->text_style_base;
 param.ruler_base = base->text_ruler_base;

 if(!F(doc).addCallbacks(doc, XcDF_TEXT_KEYWORD, readText, &param, NULL))
  return FALSE;
 return F(doc).expectKeyword(doc, XcDF_TEXT_KEYWORD, TRUE);
}


/* ----------------------------------------------------------------- ** 
** insertCode - Insert code at current position (with UNDO)          ** 
** ----------------------------------------------------------------- */
static void insertCode(this, code)
c_Text *this;
char_t code;
{
 Xc_HISTORY(("insertCode(`%s')", this->name));

 F(this).insertString(this, &code, 1);
}


/* ----------------------------------------------------------------- ** 
** warn - Send callback to all Hook cleints                          ** 
** ----------------------------------------------------------------- */
static void warn(this)
c_Text *this;
{
 Xc_HISTORY(("warn"));

 F(this->hook).callback(this->hook, XcH_CONTENT_CHANGED, this);
}


/* ----------------------------------------------------------------- ** 
** hook_style - TextStyle hook                                       ** 
** ----------------------------------------------------------------- */
static void hook_style(this, reason, old, new)
c_Text *this;
int reason;
c_TextStyle *old;
c_TextStyle *new;
{
 Xc_TRACE(("hook_style"));

 F(this->hook).callback(this->hook, reason, this);
}


/* ----------------------------------------------------------------- ** 
** hook_ruler - TextRuler hook                                       ** 
** ----------------------------------------------------------------- */
static void hook_ruler(this, reason, old, new)
c_Text *this;
int reason;
c_TextRuler *old;
c_TextRuler *new;
{
 Xc_TRACE(("hook_ruler"));

 F(this->hook).callback(this->hook, reason, this);
}


/* ----------------------------------------------------------------- ** 
** exportText - Export text                                          ** 
** ----------------------------------------------------------------- */
static boolean exportText(this, base, filename)
c_Text *this;
BaseStd_t *base;
char *filename;
{
 c_TextRuler *ruler;
 c_TextStyle *style;
 c_Color *color;
 c_DocFile *doc;

 Xc_HISTORY(("export(`%s')", this->name));

 if((doc = NEW(c_DocFile)(XcDF_SAVE, filename, -1)) == NULL)
  return FALSE;

 if(!F(doc).writeKeyword(doc, XcDF_DOCUMENT_KEYWORD, -1L) ||
    !F(doc).startSequence(doc)) return FALSE;

 /*--- Write color list ---*/
 Xc_TRACE(("write colors"));
 if((color = base->color_base) != NULL)
  if(F(color).writeList(color, doc) == FALSE) return FALSE;

 /*--- Write text ruler list ---*/
 Xc_TRACE(("text rulers"));
 if((ruler = base->text_ruler_base) != NULL)
  if(F(ruler).writeList(ruler, doc) == FALSE) return FALSE;

 /*--- Write text style list ---*/
 Xc_TRACE(("text styles"));
 if((style = base->text_style_base) != NULL)
  if(F(style).writeList(style, doc) == FALSE) return FALSE;

 if(!F(this).write(this, doc) || !F(doc).endSequence(doc)) return FALSE;

 DELETE(c_DocFile)(doc);
 return TRUE;
}
