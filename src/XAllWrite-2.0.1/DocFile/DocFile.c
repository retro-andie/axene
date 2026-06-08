/*
** DocFile.c for Xclamation, XQuad, XAllWrite and XMayday in DocFile/
** Methods for the DocFile class
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
** Started on  Tue Nov  1 17:34:48 1994 Stéphane Boisson
** Last update Sun Jun 11 19:48:33 2000 Emmanuel Paris
*/

#define NTRACE
#define NHISTORY

#include "DocFileP.h"
#include "file.h"
#include <varargs.h>
#include <math.h>

#ifdef __GNUC__
#include <stdlib.h>
#include <unistd.h>
#endif

/*--- Prototypes ---*/
static unsigned char my_getchar ___PROTO((c_DocFile *doc));
static boolean my_putchar ___PROTO((c_DocFile *this, unsigned char c));
static boolean skipSequence ___PROTO((c_DocFile *this, long depth));

/*
  static void show ___PROTO((c_DocFile *this));
  */
static boolean cb_ignore ___PROTO((c_DocFile *this));

static boolean pushContext ___PROTO((c_DocFile *this, docfile_keyword_t *key));
static void popContext ___PROTO((c_DocFile *this));
static boolean grow_context ___PROTO((c_DocFile *doc, long nb));

static unsigned long hashString ___PROTO((char *));
static docfile_keyword_t *getCallback ___PROTO((c_DocFile *this,
						char *keyword));

static void *constructor();
static void destructor();
static void *copy();

static boolean expectKeyword ___PROTO((c_DocFile *doc, char *keyword,
				       boolean persist_flag));
static boolean addCallbacks() /*___PROTO((c_DocFile *doc,
				char *keyword,
				int (*function)(c_DocFile *doc,
						char *keyword,
						long param,
						void *data),
						void *data, ..., NULL)) */ ;
static boolean removeCallbacks() /*___PROTO((c_DocFile *doc,
				   char *keyword, ..., NULL)) */;
static boolean readData ___PROTO((c_DocFile *doc,
				  unsigned char **ptr, long *len));
static boolean readWord ___PROTO((c_DocFile *doc,
				  unsigned char **ptr, long *len));
static boolean readInteger ___PROTO((c_DocFile *doc, long *integer));
static boolean readBoolean ___PROTO((c_DocFile *doc, boolean *flagp));
static boolean readReal ___PROTO((c_DocFile *doc, real *real));
static boolean writeData ___PROTO((c_DocFile *doc,
				   unsigned char *buffer, long len));
static boolean writeInteger ___PROTO((c_DocFile *doc, long integer));
static boolean writeReal ___PROTO((c_DocFile *doc, real real));
static boolean writeBoolean ___PROTO((c_DocFile *doc, boolean flag));
static boolean writeKeyword ___PROTO((c_DocFile *doc,
				      char *keyword, long param));
static boolean writeFilename ___PROTO((c_DocFile *doc, unsigned char *str));
static boolean startSequence ___PROTO((c_DocFile *doc));
static boolean endSequence ___PROTO((c_DocFile *doc));
static boolean readString ___PROTO((c_DocFile *doc, unsigned char **stringp));
static boolean readFilename ___PROTO((c_DocFile *doc, unsigned char **strp));

sf_DocFile fc_DocFile =
{
 constructor, destructor, copy,

 /*___Reading___*/
 expectKeyword, 
 (boolean (*) ___PROTO((c_DocFile *doc, ...))) addCallbacks,
 (boolean (*) ___PROTO((c_DocFile *doc, ...))) removeCallbacks,
 readData, readWord, readInteger, readReal, readBoolean, readString,
 readFilename,

 /*___Writing___*/
 writeData, writeInteger, writeReal, writeBoolean, writeKeyword,
 writeFilename,
 startSequence, endSequence,
};

#define XcDF_FILL_BUFFER(this, error_func) \
while(this->endptr == this->ptr) \
{ \
   long rc; \
   \
   this->ptr = this->buffer; \
   while((rc = read(this->handle, this->buffer, XcDF_BUFFER_SIZE)) <= 0) \
  { \
     if(rc < 0) \
    { \
       if(errno == EINTR) continue; \
       Xc_SYSERROR((this->basename)); \
     } \
     else \
     Xc_ERROR(("Unexpected end of file `%s'",this->basename)); \
     error_func; \
   } \
   \
   this->endptr = this->ptr + rc; \
   break; \
 }

#define XcDF_FILL_BUFFER2(this) \
while(this->endptr == this->ptr) \
{ \
   long rc; \
   \
   this->ptr = this->buffer; \
   while((rc = read(this->handle, this->buffer, XcDF_BUFFER_SIZE)) < 0) \
  { \
     if(errno == EINTR) continue; \
     Xc_SYSERROR((this->basename)); \
     rc = 0L; \
   } \
   \
   this->endptr = this->ptr + rc; \
   break; \
 }


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(mode, filename, handle)
int mode;
char *filename;
int handle;
{
 c_DocFile *p;

 Xc_HISTORY(("constructor(`%s')", filename));

 /*--- Sanity checks ---*/
 if(handle < 0)
 {
  if(filename == NULL)
   return NULL;
  else if(*filename == 0)
   return NULL;
 }

 /*--- Allocation ---*/
 if((p = Xc_malloc("DocFile class", sizeof(c_DocFile))) == NULL)
  return NULL;
 p->f = &fc_DocFile;

 /*--- Set up filename ---*/
 if((p->filename = Xc_strdup("filename", filename)) == NULL)
 {
  Xc_free(p);
  return NULL;
 }

 p->basename = GetBasename(p->filename, FALSE);
 p->depth = 0;
 p->context_len = p->context_pos = 0;
 p->context = NULL;
 p->mode = mode;
 p->close_flag = p->lock_flag = p->error_flag = FALSE;
 p->item = NULL;
 p->base = NULL;
 p->next_char = 0;
 p->last_char = -1;
 p->data_type = XcDF_NONE;
 p->data_ptr = NULL;

 if(handle < 0)
 {
  struct stat infos;

  if((mode == XcDF_SAVE) && !IncFileBackup(filename))
  {
   Xc_free(p->filename);
   Xc_free(p);
   return NULL;
  }

  handle = open(filename,
		(mode == XcDF_LOAD)? O_RDONLY: O_WRONLY|O_CREAT|O_TRUNC,
		0666);
  if((handle < 0) || (fstat(handle, &infos) < 0))
  {
   if(handle >= 0) close(handle);
   Xc_SYSERROR((filename));
   Xc_free(p->filename);
   Xc_free(p);
   return NULL;
  }

  if((infos.st_mode & S_IFMT) == S_IFDIR)
  {
   close(handle);
   errno = EISDIR;
   Xc_SYSERROR((filename));
   Xc_free(p->filename);
   Xc_free(p);
   return NULL;
  }

  p->close_flag = TRUE;
 }
 p->handle = handle;

  
 if(mode == XcDF_LOAD)
 {
  p->ptr = p->endptr = NULL;
  p->item = Xc_createItem("Keywords", 32, sizeof(docfile_keyword_t));
  if((p->item == NULL) || !grow_context(p, 32) || !pushContext(p, NULL))
  {
   DELETE(c_DocFile)(p);
   return NULL;
  }
  F(p).addCallbacks(p, XcDF_COMMENT_KEYWORD, cb_ignore, NULL, NULL);
 }
 else
 {
  p->ptr = p->buffer;
  p->endptr = p->ptr + XcDF_BUFFER_SIZE;
 }

 return p;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_DocFile *this;
{
 Xc_HISTORY(("destructor(`%s')", this->basename));

 if(this->mode == XcDF_SAVE)
 {
  unsigned char *ptr, *endptr;

  /*--- Flush output buffer ---*/
  Xc_TRACE(("flush"));
  endptr = this->ptr;
  ptr = this->buffer;
  while(ptr < endptr)
  {
   unsigned long len;

   len = write(this->handle, ptr, endptr - ptr);
   if(len <= 0)
   {
    if(errno == EINTR) continue;
    Xc_SYSERROR((this->basename));
    break;
   }
   ptr += len;
  }
 }

 if(this->close_flag)
 {
  close(this->handle);
  if((this->mode == XcDF_SAVE) && this->error_flag)
  {
   unlink(this->filename);
   DecFileBackup(this->filename);
  }
 }

 if(this->context != NULL) Xc_free(this->context);
 if(this->item != NULL) Xc_destroyItem(this->item);
 Xc_free(this->filename);
 Xc_free(this);
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this)
c_DocFile *this;
{
 Xc_HISTORY(("copy not implemented"));
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** hashString - Hashing function                                     ** 
** ----------------------------------------------------------------- */
static unsigned long hashString(string)
char *string;
{
 unsigned long h;
 char c;

 h = XcDF_ConvertPrime;
 while((c = *string++) != 0)
  h = (h * XcDF_ConvertPrime) ^ (c - ' ');
 /*return(h % XcDF_HashPrime);*/
 return h;
}



#define my_ungetchar(this, c) \
do \
{ \
   Xc_ASSERT((this)->last_char < 0); \
   if(c < 32) \
  { \
     if(c == XcDF_SEQ_START_CODE) (this)->depth--; \
     else if(c == XcDF_SEQ_END_CODE) (this)->depth++; \
   } \
   (this)->last_char = c; \
 } \
while(0)


/* ----------------------------------------------------------------- ** 
** my_getchar - Get a char from the file                             ** 
** ----------------------------------------------------------------- */
static unsigned char my_getchar(this)
c_DocFile *this;
{
 unsigned char c;
 
 if(this->last_char >= 0)
 {
  c = (unsigned char)this->last_char;
  if(c < 32)
  {
   if(c == XcDF_SEQ_START_CODE)
    this->depth++;
   else if(c == XcDF_SEQ_END_CODE)
    this->depth--;
  }
  this->last_char = -1;
  return c;
 }
 c = this->next_char;
 
 if(c < 32)
 {
  while(c < 32)
  {
   XcDF_FILL_BUFFER(this, return XcDF_EOF_CODE);
   c = *this->ptr++;
  }
  this->next_char = c;
  return XcDF_NEWLINE_CODE;
 }
 
 if(c == '\\')
 {
  XcDF_FILL_BUFFER(this, return XcDF_EOF_CODE);
  c = *this->ptr++;

  XcDF_FILL_BUFFER(this, return XcDF_EOF_CODE);
	      
  if((c == '\\') || (c == '{') || (c == '}') || (c == 32))
  {
   this->next_char = *this->ptr++;
   return c;
  }
  else if((c >= 'a') && (c <= 'z') && (c != 'x'))
  {
   this->next_char = *this->ptr++;
   switch(c)
   {
   case 't': return '\t';
   case 'n': return '\n';
   case 'r': return '\r';
   case 'f': return '\f';
   default: return my_getchar(this);
   }
  }
  else if(c == '0')
  {
   /*--- Octal number ---*/
   unsigned char value;

   value = 0;
   while(((c = *this->ptr++) >= '0') && (c <= '7'))
   {
    XcDF_FILL_BUFFER(this, return XcDF_EOF_CODE);
    value <<= 3;
    value += c - '0';
   }
   this->next_char = c;
   return value;
  }
  else if(c == 'x')
  {
   /*--- Hexadecimal number ---*/
   unsigned char value;
   int i;

   value = 0;
   c = *this->ptr++;
   for(i = 2; i-- ; )
   {
    if((c >= '0') && (c <= '9'))
     value = (value << 4) + (c - '0');
    else if((c >= 'a') && (c <= 'f'))
     value = (value << 4) + (c - 'a' + 10);
    else if((c >= 'A') && (c <= 'F'))
     value = (value << 4) + (c - 'A' + 10);
    else break;

    XcDF_FILL_BUFFER(this, return XcDF_EOF_CODE);
    c = *this->ptr++;
   }
   this->next_char = c;
   return value;
  }
  else
  {
   this->next_char = c;
   return XcDF_CONTROL_CODE;
  }
 }
 else if(c == '{')
 {
  this->depth++;
  c = XcDF_SEQ_START_CODE;
  /*Xc_TRACE(("depth %ld -> %ld", this->depth - 1, this->depth));*/
 }
 else if(c == '}')
 {
  if(this->depth == 0)
  {
   Xc_ERROR(("Unexpected end of block in file `%s'", this->basename));
   return XcDF_EOF_CODE;
  }
  this->depth--;
  c = XcDF_SEQ_END_CODE;
  /*Xc_TRACE(("depth %ld -> %ld", this->depth + 1, this->depth));*/
 }
 else if(c == ' ')
  c = XcDF_SPACE_CODE;
 
 XcDF_FILL_BUFFER2(this);
 this->next_char = (this->ptr == this->endptr)? 0 : *this->ptr++;
 return c;
}


/* ----------------------------------------------------------------- ** 
** grow_context - Grow allocated memory for contexts                 ** 
** ----------------------------------------------------------------- */
static boolean grow_context(this, extra)
c_DocFile *this;
long extra;
{
 docfile_context_t *new;

 Xc_TRACE(("grow_context(`%s', %ld)", this->basename, extra));

 new = Xc_realloc("Context", this->context,
		  (this->context_len + extra) * sizeof(docfile_context_t));
 if(new == NULL) return FALSE;
 this->context = new;
 this->context_len += extra;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** pushContext - Create a new context                                ** 
** ----------------------------------------------------------------- */
static boolean pushContext(this, keyword)
c_DocFile *this;
docfile_keyword_t *keyword;
{
 docfile_context_t *context;

 Xc_TRACE(("pushContext(`%s')", this->basename));

 if((this->context_pos == this->context_len) && !grow_context(this, 16))
  return FALSE;
 context = this->context + this->context_pos++;
 context->current = keyword;
 context->base = NULL;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** popContext - Remove the current context                           ** 
** ----------------------------------------------------------------- */
static void popContext(this)
c_DocFile *this;
{
 docfile_context_t *context;

 Xc_TRACE(("popContext(`%s')", this->basename));
 Xc_ASSERT(this->context_pos > 0);

 context = this->context + --this->context_pos;
 if(context->base != NULL)
 {
  docfile_keyword_t *keyword;

  keyword = this->base;
  while(42)
  {
   docfile_keyword_t *tmp;
	  
   tmp = keyword->next;
   Xc_delete(this->item, keyword);
   if(keyword == context->base)
   {
    this->base = tmp;
    break;
   }
   keyword = tmp;
  }
 }
}


/* ----------------------------------------------------------------- ** 
** skipSequence - Skip the remaining data of the current sequence    ** 
** ----------------------------------------------------------------- */
static boolean skipSequence(this, depth)
c_DocFile *this;
long depth;
{
 Xc_TRACE(("skipSequence(`%s', %ld -> %ld)", this->basename,
	   this->depth, depth));
  
 while(this->depth > depth)
  if(my_getchar(this) == XcDF_EOF_CODE) return FALSE;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** expectKeyword - Scan file for the specified keyword               ** 
** ----------------------------------------------------------------- */
static boolean expectKeyword(this, keyword, persist_flag)
c_DocFile *this;
char *keyword;
boolean persist_flag;
{
 unsigned char buffer[XcDF_KEYWORD_MAX_LEN], c, *ptr;
 docfile_keyword_t *cb;
 long param, depth, start_depth;
 boolean first_flag;

 Xc_HISTORY(("expectKeyword(`%s', `%s')", this->basename,
	     (keyword == NULL)? "Any" : keyword));

 if(this->lock_flag == TRUE) return FALSE;
 first_flag = TRUE;
 start_depth = this->depth;
 do
 {
  do
  {
   long len;
	  
   while((c = my_getchar(this)) != XcDF_CONTROL_CODE)
    if(c == XcDF_EOF_CODE)
     return FALSE;
    else if((c == XcDF_SEQ_END_CODE) && (start_depth > this->depth))
     return(keyword? (persist_flag & ~first_flag) : TRUE);
    else if((start_depth == 0) &&
	    (c != XcDF_NEWLINE_CODE) && (c != XcDF_SPACE_CODE))
     return FALSE;

   for(ptr = buffer, len = XcDF_KEYWORD_MAX_LEN - 1; len-- > 0; )
   {
    c = my_getchar(this);
    if(((c < 'A') || (c > 'Z')) && (c != '_')) break;
    *ptr++ = c;
   }
   *ptr = 0;
   Xc_TRACE(("Found `%s' keyword", buffer));
  }
  while((keyword != NULL) && strcmp(keyword, buffer));

  first_flag = FALSE;

  if((c >= '0') && (c <= '9'))
  {
   param = c - '0';
   while(((c = my_getchar(this)) >= '0') && (c <= '9'))
    param = (param * 10) + (c - '0');
  }
  else
   param = -1;
      
  /*--- Skip trailing blanks ---*/
  while((c == XcDF_NEWLINE_CODE) || (c == XcDF_SPACE_CODE)) 
   c = my_getchar(this);
      
  depth = this->depth;
      
  if(c != XcDF_SEQ_START_CODE)
  { 
   this->lock_flag = TRUE;
   my_ungetchar(this, c);
  }
  else
   this->lock_flag = FALSE;

  Xc_TRACE(("lock_flag %s (depth %ld)",
	    this->lock_flag? "True" : "False", this->depth));
      
  cb = getCallback(this, buffer);
  if(cb != NULL)
  {
   if(!pushContext(this, cb)) return FALSE;
   Xc_TRACE(("Calling `%s' (%ld, %lx)",
	     cb->name, param, cb->callback_data));
	  
   if(!cb->callback(this, cb->name, param, cb->callback_data))
   {
    Xc_TRACE(("loading error!"));
    popContext(this);
    return FALSE;
   }
   popContext(this);
  }
  else if((this->lock_flag == FALSE) && (keyword != NULL))
  {
   Xc_TRACE(("Seeked keyword found"));

   if(!pushContext(this, cb)) return FALSE;
   if(!F(this).expectKeyword(this, NULL, TRUE))
   {
    Xc_TRACE(("loading error!"));
    popContext(this);
    return FALSE;
   }
   popContext(this);
  }

  Xc_TRACE(("back from `%s' (depth = %ld)",
	    (cb == NULL)? "NULL" : cb->name, this->depth));
      
  if((depth <= this->depth) && (this->lock_flag == FALSE))
   if(!skipSequence(this, depth - 1)) return FALSE;
      
  this->lock_flag = FALSE;
      
 }
 while(persist_flag == TRUE);

 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** addCallbacks - Add callbacks to the current context                ** 
** ----------------------------------------------------------------- */
static boolean addCallbacks(this, va_alist)
c_DocFile *this;
va_dcl
{
 docfile_context_t *context;
 char *keyword;
 va_list ap;

 Xc_HISTORY(("addCallbacks(`%s')", this->basename));

 context = this->context + this->context_pos - 1;
 va_start(ap);
 while((keyword = va_arg(ap, char *)) != NULL)
 {
  docfile_keyword_t *cb;
  unsigned long key;

  Xc_ASSERT(strlen(keyword) < XcDF_KEYWORD_MAX_LEN);

  cb = NULL;
  key = hashString(keyword);
  if(context->base != NULL)
  {
   docfile_keyword_t **ptr;

   ptr = &this->base;
   do
    if((key == (*ptr)->key) && !strcmp(keyword, (*ptr)->name))
    {
     Xc_TRACE(("`%s' callback replaced", (*ptr)->name));
     cb = *ptr;
     break;
    }
   while((*ptr != context->base) && ((ptr = &(*ptr)->next) != NULL));
  }

  if(cb == NULL)
  {
   Xc_TRACE(("`%s' callback created", keyword));
   if((cb = (docfile_keyword_t *)Xc_new(this->item)) == NULL) return FALSE;
   cb->next = this->base;
   this->base = cb;
   if(context->base == NULL) context->base = cb;
   cb->key = key;
   strcpy(cb->name, keyword);
  }
  cb->callback = (boolean (*)())va_arg(ap, void *);
  cb->callback_data = va_arg(ap, void *);
 }
 va_end(ap);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** removeCallbacks - Remove callbacks                                ** 
** ----------------------------------------------------------------- */
static boolean removeCallbacks(this, va_alist)
c_DocFile *this;
va_dcl
{
 docfile_context_t *context;
 char *keyword;
 va_list ap;

 Xc_HISTORY(("removeCallbacks(`%s')", this->basename));
 Xc_ASSERT(this->context_pos > 0);

 context = this->context + this->context_pos - 1;
 va_start(ap);
 while((keyword = va_arg(ap, char *)) != NULL)
 {
  unsigned long key;

  Xc_TRACE(("keyword `%s'", keyword));
  key = hashString(keyword);
  if(context->base != NULL)
  {
   docfile_keyword_t **ptr;

   ptr = &this->base;
   do
    if((key == (*ptr)->key) && !strcmp(keyword, (*ptr)->name))
    {
     docfile_keyword_t *tmp;

     Xc_TRACE(("`%s' callback removed", (*ptr)->name));
     tmp = *ptr;
     *ptr = tmp->next;
     Xc_delete(this->item, tmp);
     if(tmp == context->base)
     {
      if(ptr == &this->base)
       context->base = NULL;
      else
      {
       tmp = *ptr;
       ptr = &this->base;
       while((*ptr)->next != tmp) ptr = &(*ptr)->next;
       context->base = (*ptr);
      }
     }
     break;
    }
   while((*ptr != context->base) && ((ptr = &(*ptr)->next) != NULL));
  }
 }
 va_end(ap);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** getCallback - Find the callback for the specified keyword         ** 
** ----------------------------------------------------------------- */
static docfile_keyword_t *getCallback(this, keyword)
c_DocFile *this;
char *keyword;
{
 docfile_keyword_t *ptr;
 unsigned long key;
#ifndef NTRACE
 docfile_context_t *context;
#endif

#ifndef NTRACE
 Xc_ASSERT(this->context != NULL);
 context = this->context + this->context_pos - 1;
#endif
 key = hashString(keyword);
 for(ptr = this->base; ptr != NULL; ptr = ptr->next)
 {
#ifndef NTRACE
  while(context->base == NULL) context--;
#endif
  if((key == ptr->key) && !strcmp(keyword, ptr->name))
  {
   Xc_TRACE(("callback `%s' from `%s' context", ptr->name,
	     context->current? context->current->name : "TopLevel"));
   return ptr;
  }
#ifndef NTRACE
  if(context->base == ptr) context--;
#endif
 }

 Xc_TRACE(("no callback for `%s'", keyword));
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** readData - Read data from the file                                ** 
** ----------------------------------------------------------------- */
static boolean readData(this, pptr, lenp)
c_DocFile *this;
unsigned char **pptr;
long *lenp;
{
 unsigned char *buffer, *ptr, c;
 long left;

 Xc_HISTORY(("read(`%s')", this->basename));

 *pptr = NULL;
 *lenp = -1;
 if(this->lock_flag == TRUE) return TRUE;
  
 do 
 {
  c = my_getchar(this);
  
  if(c == XcDF_CONTROL_CODE)
  {
   my_ungetchar(this, c);
   if(F(this).expectKeyword(this, NULL, FALSE) == FALSE) return FALSE;
   *lenp = 0;
   return TRUE;
  }
  else if(c == XcDF_SEQ_END_CODE)
   return TRUE;
  else if(c == XcDF_EOF_CODE)
   return FALSE;
 } 
 while(c <= XcDF_LAST_CODE);
 
 left = 0;
 buffer = ptr = NULL;
 *lenp = 0;

 do
 {
  if(left <= 1)
  {
   unsigned char *new;
	  
   new = Xc_realloc("read-data", buffer, *lenp + XcDF_DATA_CHUNK_SIZE);
   if(new == NULL)
   {
    my_ungetchar(this, c);
    this->last_char = c;
    break;
   }
   left = XcDF_DATA_CHUNK_SIZE;
   if(new != buffer)
   {
    *pptr = new;
    ptr += new - buffer;
    buffer = new;
   }
   *lenp += XcDF_DATA_CHUNK_SIZE;
   left = XcDF_DATA_CHUNK_SIZE;
  }
  if(c == XcDF_SPACE_CODE) c = 32;
  if(c != XcDF_NEWLINE_CODE)
  {
   *ptr++ = c;
   left--;
  }
 }
 while(((c = my_getchar(this)) != XcDF_EOF_CODE) &&
       (c != XcDF_SEQ_END_CODE) &&
       (c != XcDF_CONTROL_CODE));
  
 /*--- unexpected end of file ---*/
 if(c == XcDF_EOF_CODE)
 {
  if(buffer != NULL) Xc_free(buffer);
  *pptr = NULL;
  *lenp = -1;
  return FALSE;
 }

 /*--- Resize buffer ---*/
 left = ptr - buffer;
 if(left != 0)
 {
  if((ptr = Xc_realloc("read-data", buffer, left + 1)) != NULL)
  {
   *pptr = ptr;
   *lenp = left;
   ptr[left] = 0;
  }
  else
   buffer[left] = 0;
 }
 else
  *lenp = 0;
  
 my_ungetchar(this, c);
      
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** Readword - Read a word from the file                              ** 
** ----------------------------------------------------------------- */
static boolean readWord(this, pptr, lenp)
c_DocFile *this;
unsigned char **pptr;
long *lenp;
{
 unsigned char *buffer, *ptr, c;
 long left;

 Xc_HISTORY(("readWord(`%s')", this->basename));

 *pptr = NULL;
 *lenp = -1;
 if(this->lock_flag == TRUE) return TRUE;
  
 do
 {
  c = my_getchar(this);

  if(c == XcDF_CONTROL_CODE)
  {
   my_ungetchar(this, c);
   if(F(this).expectKeyword(this, NULL, FALSE) == FALSE) return FALSE;
  }
  else if(c == XcDF_SEQ_END_CODE)
   return TRUE;
  else if(c == XcDF_EOF_CODE)
   return FALSE;
 }
 while(c <= 32);

 left = 0;
 buffer = ptr = NULL;
 *lenp = 0;

 do
 {
  if(left <= 1)
  {
   unsigned char *new;
	  
   new = Xc_realloc("read-data", buffer, *lenp + XcDF_DATA_CHUNK_SIZE);
   if(new == NULL)
   {
    my_ungetchar(this, c);
    this->last_char = c;
    break;
   }
   left = XcDF_DATA_CHUNK_SIZE;
   if(new != buffer)
   {
    *pptr = new;
    ptr += new - buffer;
    buffer = new;
   }
   *lenp += XcDF_DATA_CHUNK_SIZE;
   left = XcDF_DATA_CHUNK_SIZE;
  }

  *ptr++ = c;
  left--;
 }
 while((c = my_getchar(this)) > 32);

 while((c == XcDF_NEWLINE_CODE) || (c == XcDF_SPACE_CODE))
  c = my_getchar(this);

 /*--- unexpected end of file ---*/
 if(c == XcDF_EOF_CODE)
 {
  if(buffer != NULL) Xc_free(buffer);
  *pptr = NULL;
  *lenp = -1;
  return FALSE;
 }

 /*--- Resize buffer ---*/
 left = ptr - buffer;
 if((ptr = Xc_realloc("read-data", buffer, left + 1)) != NULL)
 {
  *pptr = ptr;
  *lenp = left;
  ptr[left] = 0;
 }
 else
  buffer[left] = 0;
  
 Xc_TRACE(("word = [%s]", *pptr));

 my_ungetchar(this, c);
      
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** readInteger - read an integer from the file                       ** 
** ----------------------------------------------------------------- */
static boolean readInteger(this, integerp)
c_DocFile *this;
long *integerp;
{
 long tmp;

 Xc_HISTORY(("readInteger(`%s')", this->basename));

 tmp = 0;
 if(this->lock_flag != TRUE)
 {
  unsigned char *ptr;
  long len;

  if(!readWord(this, &ptr, &len)) return FALSE;
  if(len >= 0)
  {
   tmp = atol(ptr);
   Xc_free(ptr);
  }
 }
 *integerp = tmp;
 Xc_TRACE(("Integer: %ld", tmp));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** readReal - read a real from the file                              ** 
** ----------------------------------------------------------------- */
static boolean readReal(this, realp)
c_DocFile *this;
real *realp;
{
 double tmp;

 Xc_HISTORY(("readReal(`%s')", this->basename));

 tmp = 0.0;
 if(this->lock_flag != TRUE)
 {
  unsigned char *ptr;
  long len;
      
  if(!readWord(this, &ptr, &len)) return FALSE;
  if(len >= 0)
  {
   tmp = atof(ptr);
   if(errno == ERANGE)
    Xc_WARNING(("real %s out of range", ptr));
   Xc_free(ptr);
  }
 }
 *realp = (real)tmp;
 Xc_TRACE(("Real: %g", (double)tmp));
  
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** readBoolean - Read an boolean from the file                       ** 
** ----------------------------------------------------------------- */
static boolean readBoolean(this, flagp)
c_DocFile *this;
boolean *flagp;
{
 boolean tmp;

 Xc_HISTORY(("readBoolean(`%s')", this->basename));

 tmp = FALSE;
 if(this->lock_flag != TRUE)
 {
  unsigned char *ptr;
  long len;

  if(!readWord(this, &ptr, &len)) return FALSE;
  if(len >= 0)
  {
   tmp = (strcmp(ptr, "True") == 0)? TRUE : FALSE;
   Xc_free(ptr);
  }
 }
 *flagp = tmp;
 Xc_TRACE(("Boolean: %s", tmp? "True" : "False"));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** my_putchar - Put a char in the file                               ** 
** ----------------------------------------------------------------- */
static boolean my_putchar(this, c)
c_DocFile *this;
unsigned char c;
{
 if(((c >= 32) && (c <= 127)) || (c == '\n'))
 {
  *this->ptr++ = c;
  if(this->ptr < this->endptr)
   return TRUE;
  else
  {
   unsigned char *ptr, *endptr;

   ptr = this->buffer;
   endptr = this->ptr;
   while(ptr < endptr)
   {
    unsigned long len;

    if((len = write(this->handle, ptr, endptr - ptr)) <= 0)
    {
     if(errno == EINTR) continue;
     this->error_flag = TRUE;
     Xc_SYSERROR((this->basename));
     return FALSE;
    }
    ptr += len;
   }
   this->ptr = this->buffer;
  }
  return TRUE;
 }
 else
 {
  char buffer[8], *ptr;

  sprintf(buffer, "\\x%02x", (int)c);
  for(ptr = buffer; *ptr; ptr++)
   if(my_putchar(this, *ptr) == FALSE) return FALSE;
 }
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** writeData - Write data to the file                                ** 
** ----------------------------------------------------------------- */
static boolean writeData(this, buffer, len)
c_DocFile *this;
unsigned char *buffer;
long len;
{
 unsigned char c;
 boolean first_flag;

 Xc_HISTORY(("writeData(`%s')", this->basename));

 if((this->last_char == XcDF_NUMBER) || (this->last_char == XcDF_KEYWORD))
  if(!my_putchar(this, ' ')) return FALSE;
 first_flag = TRUE;

 while(len--)
 {
  if((c = *buffer++) >= 32)
  {
   if((c == '{') ||
      (c == '}') || (c == '\\') || (first_flag && (c == 32)))
    if(!my_putchar(this, '\\')) return FALSE;
   if(!my_putchar(this, c)) return FALSE;
  }
  else if(c == '\n')
  {
   if(!my_putchar(this, '\\')) return FALSE;
   if(!my_putchar(this, 'n')) return FALSE;
   if(!my_putchar(this, '\n')) return FALSE;
  }
  else if(c == '\r')
  {
   if(!my_putchar(this, '\\')) return FALSE;
   if(!my_putchar(this, 'r')) return FALSE;
  }
  else if(c == '\f')
  {
   if(!my_putchar(this, '\\')) return FALSE;
   if(!my_putchar(this, 'f')) return FALSE;
  }
  else if(c == '\t')
  {
   if(!my_putchar(this, '\\')) return FALSE;
   if(!my_putchar(this, 't')) return FALSE;
  }
  else
  {
   /*if(!my_putchar(this, c)) return FALSE;*/
  }
  first_flag = FALSE;
 }
 this->last_char = XcDF_TEXT;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** writeInteger - Write an integer to the file                       ** 
** ----------------------------------------------------------------- */
static boolean writeInteger(this, integer)
c_DocFile *this;
long integer;
{
 unsigned char buffer[16], *ptr;

 Xc_HISTORY(("writeInteger(`%s', %ld)", this->basename, integer));
 Xc_ASSERT(this->last_char != XcDF_TEXT);

 if((this->last_char == XcDF_NUMBER) || (this->last_char == XcDF_KEYWORD))
  if(!my_putchar(this, ' ')) return FALSE;

 sprintf(buffer, "%ld", integer);
 for(ptr = buffer; *ptr; ptr++)
  if(!my_putchar(this, *ptr)) return FALSE;
 this->last_char = XcDF_NUMBER;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** writeReal - Write an real number to the file                      ** 
** ----------------------------------------------------------------- */
static boolean writeReal(this, r)
c_DocFile *this;
real r;
{
 unsigned char buffer[64], *ptr;

 Xc_HISTORY(("writeReal(`%s', %g)", this->basename, (double)r));
 Xc_ASSERT(this->last_char != XcDF_TEXT);

 if((this->last_char == XcDF_NUMBER) || (this->last_char == XcDF_KEYWORD))
  if(!my_putchar(this, ' ')) return FALSE;

 sprintf(buffer, "%g", (double)r);
 for(ptr = buffer; *ptr; ptr++)
 {
  if(!my_putchar(this, *ptr)) 
   return FALSE;
 }
 this->last_char = XcDF_NUMBER;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** writeBoolean - Write a boolean                                    ** 
** ----------------------------------------------------------------- */
static boolean writeBoolean(this, flag)
c_DocFile *this;
boolean flag;
{
 unsigned char *ptr;

 Xc_HISTORY(("writeBoolean(`%s', %s)", this->basename, flag? "True":"False"));
 Xc_ASSERT(this->last_char != XcDF_TEXT);

 if((this->last_char == XcDF_NUMBER) || (this->last_char == XcDF_KEYWORD))
  if(!my_putchar(this, ' ')) return FALSE;

 for(ptr = (unsigned char *)(flag? "True" : "False") ; *ptr; ptr++)
  if(!my_putchar(this, *ptr)) return FALSE;
 this->last_char = XcDF_NUMBER;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** writeKeyword - Write a keyword with his parameter                 ** 
** ----------------------------------------------------------------- */
static boolean writeKeyword(this, keyword, param)
c_DocFile *this;
char *keyword;
long param;
{
 char *ptr;

 Xc_HISTORY(("writeKeyword(`%s', %s)", this->basename, keyword));

 if((this->last_char == XcDF_SEQUENCE_END) && !my_putchar(this, '\n'))
  return FALSE;
 else if((this->last_char == XcDF_NUMBER) && !my_putchar(this, ' '))
  return FALSE;

 if(!my_putchar(this, '\\')) return FALSE;
 for(ptr = keyword; *ptr; ptr++)
 {
  Xc_ASSERT(((*ptr >= 'A') && (*ptr <= 'Z')) || (*ptr == '_'));
  if(!my_putchar(this, *ptr)) return FALSE;
 }

 if(param >= 0)
 {
  char buffer[16];
  sprintf(buffer, "%ld", param);
  for(ptr = buffer; *ptr; ptr++)
   if(!my_putchar(this, *ptr)) return FALSE;
 }
 this->last_char = XcDF_KEYWORD;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** startSequence - Start a sequence (new context)                    ** 
** ----------------------------------------------------------------- */
static boolean startSequence(this)
c_DocFile *this;
{
 Xc_HISTORY(("startSequence(`%s')", this->basename));

 this->depth++;
 this->last_char = XcDF_SEQUENCE_START;
 return my_putchar(this, '{');
}


/* ----------------------------------------------------------------- ** 
** endSequence - End a sequence                                      ** 
** ----------------------------------------------------------------- */
static boolean endSequence(this)
c_DocFile *this;
{
 Xc_HISTORY(("endSequence(`%s')", this->basename));
 Xc_ASSERT(this->depth > 0);
  
 this->depth--;
 this->last_char = XcDF_SEQUENCE_END;
 return my_putchar(this, '}');
}


/* ----------------------------------------------------------------- ** 
** show - Dump contexts                                              ** 
** ----------------------------------------------------------------- */
#if 0
static void show(this)
c_DocFile *this;
{
 Xc_TRACE(("Dump callbacks(`%s')", this->basename));
 if(this->context_pos == 0)
  puts("no context");
 else
 {
  docfile_context_t *context;
  docfile_keyword_t *keyword;

  keyword = this->base;
  context = this->context + this->context_pos - 1;
  do
  {
   if(context->current == NULL)
    puts("TopLevel context");
   else
    printf("`%s' context\n", context->current->name);
   if(context->base == NULL)
    puts("  No callbacks");
   else
   {
    do
     printf("  %-16s %08lx\n", keyword->name, keyword->key);
    while((keyword != context->base) &&
	  ((keyword = keyword->next) != NULL));
    keyword = keyword->next;
   }
  }
  while(context-- != this->context);
 }
}
#endif

/* ----------------------------------------------------------------- ** 
** readString - Method to red a text without interactions            ** 
** ----------------------------------------------------------------- */
static boolean readString(doc, stringp)
c_DocFile *doc;
unsigned char **stringp;
{
 unsigned char *ptr, *buffer;
 long len, buffer_len;

 Xc_HISTORY(("readString(`%s')", doc->filename));

 buffer = NULL;
 buffer_len = 0;
 while(F(doc).read(doc, &ptr, &len) == TRUE)
  if(len < 0)
  {
   if(buffer != NULL) buffer[buffer_len] = 0;
   *stringp = buffer;
   Xc_TRACE(("string: [%s]", buffer? buffer:"(null)"));
   return TRUE;
  }
  else if(len > 0)
  {
   unsigned char *new;

   if((new = Xc_realloc("string", buffer, buffer_len + len + 1)) == NULL)
   {
    Xc_free(ptr);
    break;
   }
   buffer = new;
   memcpy(buffer + buffer_len, ptr, len);
   buffer_len += len;
   Xc_free(ptr);
  }
 if(buffer != NULL) Xc_free(buffer);
 *stringp = NULL;
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** writeFilename - Output a file reference                           ** 
** ----------------------------------------------------------------- */
static boolean writeFilename(this, filename)
c_DocFile *this;
unsigned char *filename;
{
 boolean rc;

 Xc_HISTORY(("writeFilename(`%s')", filename));

 if((filename = (unsigned char *)MakeRelativeFile(this->filename, 
						  filename)) == NULL)
 {
  this->error_flag = TRUE;
  return FALSE;
 }
 rc = F(this).write(this, filename, strlen(filename));
 Xc_free(filename);
 return rc;
}


/* ----------------------------------------------------------------- ** 
** read1Filename - Get a file reference                              ** 
** ----------------------------------------------------------------- */
static boolean readFilename(this, filenamep)
c_DocFile *this;
unsigned char **filenamep;
{
 unsigned char *tmp;

 Xc_HISTORY(("readFilename(`%s')", this->basename));

 *filenamep = NULL;
 if(F(this).readString(this, &tmp) == FALSE) return FALSE;
 if(tmp == NULL) return TRUE;
 *filenamep = (unsigned char *)MakeAbsoluteFile(this->filename, tmp);
 Xc_free(tmp);
 return((*filenamep == NULL)? FALSE : TRUE);
}


/* ----------------------------------------------------------------- ** 
** cb_ignore - Loading callback                                      ** 
** ----------------------------------------------------------------- */
static boolean cb_ignore(this)
c_DocFile *this;
{
 return TRUE;
}
