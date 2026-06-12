/*
** AIDocFile.c for Xclamation and XAllWrite in AIDocFile/
** Methods for the AIDocFile class
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
** Last update Fri Jan  7 16:19:19 2000 Emmanuel Paris
*/

#define NHISTORY
#define NTRACE

#include "AIDocFileP.h"
#include <stdarg.h>
#include <math.h>

#ifdef __GNUC__
#include <stdlib.h>
#include <unistd.h>
#endif

#include "file.h"

#define XcAI_IS_WHITESPACE(c) ((c) <= 32)

typedef struct ai_stack_item_s ai_stack_item_t;
struct ai_stack_item_s
{
 union {
	real number;
	char *string;
       } data;
 enum {XcAI_NUMBER_ITEM, XcAI_STRING_ITEM, XcAI_NAME_ITEM,
	XcAI_OPEN_ITEM, XcAI_CLOSE_ITEM} type;
};

#define XcAIDF_IS_NUMBER(item) ((item)->type == XcAI_NUMBER_ITEM)
#define XcAIDF_IS_STRING(item) ((item)->type == XcAI_STRING_ITEM)
#define XcAIDF_IS_NAME(item) ((item)->type == XcAI_NAME_ITEM)
#define XcAIDF_IS_ARRAY(item) ((item)->type == XcAI_ARRAY_ITEM)
#define XcAIDF_IS_OPEN(item) ((item)->type == XcAI_OPEN_ITEM)
#define XcAIDF_IS_CLOSE(item) ((item)->type == XcAI_CLOSE_ITEM)

#define XcAIDF_POP(stack, index, count) (stack) + ((index) -= count);\
if((index) < 0) break
#define XcAIDF_TYPE_MISMATCH(rc) rc = -1; break

/*--- Prototypes ---*/
static void *constructor();
static void destructor();
static void *copy();

static unsigned char AI_getchar ___PROTO((c_AIDocFile *this));
static boolean readMain ___PROTO((c_AIDocFile *this));
static int readClassic ___PROTO((c_AIDocFile *this, unsigned char *ptr,
				 long len));
static int readWord ___PROTO((c_AIDocFile *this, unsigned char **pptr,
			      long *lenp));
static boolean readInteger ___PROTO((c_AIDocFile *doc, long *integer));
static boolean readBoolean ___PROTO((c_AIDocFile *doc, boolean *flagp));
static boolean readReal ___PROTO((c_AIDocFile *doc, real *real));
static void skipSpace ___PROTO((c_AIDocFile *doc));
static int seekClassic ___PROTO((c_AIDocFile *doc, int mode, int len));
static boolean searchData ___PROTO((c_AIDocFile *doc, char *data));

static boolean readLine ___PROTO((c_AIDocFile *this, boolean skip_comment));

sf_AIDocFile fc_AIDocFile = {
			     constructor, 
			     destructor, 
			     copy,

			     /*___Reading___*/
			     readMain, 
			     AI_getchar,
			     readClassic,
			     seekClassic,
			     readWord, 
			     readInteger, 
			     readReal, 
			     readBoolean, 
			     skipSpace,
			     searchData
			    };


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(mode, filename, handle, vector)
int		mode;
char		*filename;
int		handle;
c_VectorGraph	*vector;
{
 c_AIDocFile *p;
 int i;

 Xc_HISTORY(("constructor(`%s')", filename));
 if((p = Xc_malloc("AIDocFile class", sizeof(c_AIDocFile))) == NULL)
  return NULL;
 p->f = &fc_AIDocFile;

 p->filename = Xc_strdup("filename", filename? filename: "-stream-");
 if(p->filename == NULL)
 {
  Xc_free(p);
  return NULL;
 }

 p->close_flag = FALSE;

 if(handle < 0)
 {
  handle = open(filename, O_RDONLY, 0666);
  if(handle < 0)
  {
   Xc_SYSERROR((filename));
   Xc_free(p->filename);
   Xc_free(p);
   return NULL;
  }
  p->close_flag = TRUE;
 }
 p->handle = handle;

 p->buffer_index = p->end_index = 0;
 p->line_ptr = NULL;
 p->vector = vector;
 vector->filename = p->filename;
 p->string_level = 0;

 p->color_base = &vector->basestd->color_base;
 p->font_base = NULL;

 for(i = 0; i < 256; i++) p->native_encoding[i] = i;
 p->encoding = p->native_encoding;

 return p;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_AIDocFile *this;
{
 ai_font_record_t *font;

 Xc_HISTORY(("destructor(`%s')", GetBasename(this->filename, FALSE)));

 if(this->close_flag) 
  close(this->handle);
 /*Xc_free(this->filename);*/

 /*--- Free fonts ---*/
 while((font = this->font_base) != NULL)
 {
  this->font_base = font->next;
  Xc_free(font);
 }
  
 Xc_free(this);
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this)
c_AIDocFile *this;
{
 Xc_HISTORY(("copy not implemented"));
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** AI_getchar - Get a char from the file                             ** 
** ----------------------------------------------------------------- */
static unsigned char AI_getchar(this)
c_AIDocFile *this;
{
 long	len;

 if (this->buffer_index >= XcAIDF_BUFFER_SIZE)
 {
  len = read(this->handle, this->buffer_file, XcAIDF_BUFFER_SIZE);
  this->end_index = len;
  this->buffer_index = 0;
 }
 this->buffer_index++;
 return this->buffer_file[this->buffer_index];
}


/* ----------------------------------------------------------------- ** 
** readMain - Main Read Part                                         ** 
** ----------------------------------------------------------------- */
static boolean readMain(this)
c_AIDocFile *this;
{
 boolean exit_flag, clip_flag, text_flag;
 int rc, polarity, render_mode;
 c_Color **cbase, *color;
 ai_stack_item_t *stack;
 c_VectorGraph *vector;
 unsigned char *n_word;
 char_t encoding[256];
 long n_len, index;
 bbox_t crop, bbox;
 c_VectorFont	*last_font;
 coord_t	last_size;
 coord_t	last_dy;
 coord_t	t_length;
 coord_t	tot_length;

 Xc_HISTORY(("readMain(`%s')", GetBasename(this->filename, FALSE)));

 for(rc = 0; rc < 256; ++rc) encoding[rc] = rc;
 /*--- Identifier ---*/
 if((readWord(this, &n_word, &n_len) != XcAIDF_COMMENT) ||
    strncmp(n_word, "!PS-Adobe-", 10))
 {
  if(n_word != NULL) Xc_free(n_word);
  Xc_WARNING(("This is not an Adobe Illustrator file"));
  return FALSE;
 }
  
 Xc_BBOX_INIT(&bbox, COORD_ZERO, COORD_ZERO);
 Xc_BBOX_INIT(&crop, COORD_ZERO, COORD_ZERO);

 Xc_TRACE(("File is ok version `%s'", n_word + 10));
 Xc_free(n_word);
 
 last_font = NULL;
 last_size = 0;
 last_dy = 0;
 t_length = 0;
 tot_length = 0;
 /*--- Header ---*/
 while(42)
 {
  if(readWord(this, &n_word, &n_len) != XcAIDF_COMMENT)
  {
   if(n_word != NULL) Xc_free(n_word);
   Xc_WARNING(("Truncated header"));
   return FALSE;
  }
  if((n_len == 12) && !strcmp(n_word, "%EndComments"))
   break;
  else if(!strncmp(n_word, "%BoundingBox:", 13))
  {
   float llx, lly, urx, ury;

   if(sscanf(n_word + 13, "%*[ \t]%f%*[ \t]%f%*[ \t]%f%*[ \t]%f",
	     &llx, &lly, &urx, &ury) == 4)
   {
    bbox.llx = SCALE_FROM_POINTS(llx);
    bbox.lly = SCALE_FROM_POINTS(lly);
    bbox.urx = SCALE_FROM_POINTS(urx);
    bbox.ury = SCALE_FROM_POINTS(ury);
    Xc_DUMP_BBOX("bbox", &bbox);
   }
  }
  else if(!strncmp(n_word, "AI3_Cropmarks:", 14))
  {
   float llx, lly, urx, ury;

   if(sscanf(n_word + 14, "%*[ \t]%f%*[ \t]%f%*[ \t]%f%*[ \t]%f",
	     &llx, &lly, &urx, &ury) == 4)
   {
    crop.llx = SCALE_FROM_POINTS(llx);
    crop.lly = SCALE_FROM_POINTS(lly);
    crop.urx = SCALE_FROM_POINTS(urx);
    crop.ury = SCALE_FROM_POINTS(ury);
    Xc_DUMP_BBOX("crop", &crop);
   }
  }
  Xc_free(n_word);
 }
 Xc_free(n_word);
  
 {
  boolean setup_flag;
  char *keyword;
    
  keyword = "%%EndProlog";
  setup_flag = FALSE;

  Xc_TRACE(("skip Prolog"));
  while(42)
  {
   if(readLine(this, FALSE) == FALSE) return FALSE;
   /*Xc_TRACE(("Header: %s", this->line));*/
   if(!strcmp(this->line, keyword))
   {
    if(setup_flag == FALSE)
    {
     if(readLine(this, FALSE) == FALSE) return FALSE;
     if(strcmp(this->line, "%%BeginSetup"))
     {
      this->line_ptr = NULL;
      break;
     }
     else
     {
      Xc_TRACE(("skip Setup"));
      setup_flag = TRUE;
      keyword = "%%EndSetup";
     }
    }
    else
    {
     this->line_ptr = NULL;
     break;
    }
   }
   else if(!strncmp(this->line, "%AI3_BeginEncoding:", 19))
   {
    char *dest, *src, *ptr;

    /*--- Font encoding ---*/
    for(ptr = (char *)this->line + 19; *ptr && (*ptr <= 32); ptr++);
    for(dest = ptr; *ptr > 32; ptr++);
    if(*ptr != 0)
    {
     *ptr++ = 0;
     while(*ptr && (*ptr <= 32)) ptr++;
     for(src = ptr; *ptr > 32; ptr++);
     *ptr = 0;

     if(aiEncodeFont(this, src, dest, 0) == NULL)
     {
      return FALSE;
     }
    }
   }
  }
 }

 Xc_TRACE(("start data"));
 index = 0;
 if((stack = Xc_malloc("stack", 30 * sizeof(ai_stack_item_t))) == NULL)
  return FALSE;

 cbase = this->color_base;
 vector = this->vector;

 exit_flag = clip_flag = text_flag = FALSE;
 polarity = render_mode = 0;
  
 while(exit_flag == FALSE)
 {
  ai_stack_item_t *sptr;

  if((rc = readWord(this, &n_word, &n_len)) == XcAIDF_ERROR) break;

  switch(rc)
  {
  case XcAIDF_STRING:
   Xc_TRACE(("string: `%s'", n_word));
   sptr = stack + index++;
   sptr->type = XcAI_STRING_ITEM;
   sptr->data.string = (char *)n_word;
   break;

  case XcAIDF_NAME:
   Xc_TRACE(("name: `%s'", n_word));
   sptr = stack + index++;
   sptr->type = XcAI_NAME_ITEM;
   sptr->data.string = (char *)n_word;
   break;

  case XcAIDF_COMMENT:
   Xc_TRACE(("Comment: `%s'", n_word));
   if(((n_len == 8) && !strcmp(n_word, "%Trailer")) ||
      ((n_len == 12) && !strcmp(n_word, "%PageTrailer")))
    exit_flag = TRUE;
   else if((n_len == 14) && !strcmp(n_word, "AI3_BeginCrops"))
   {
    Xc_TRACE(("Skip crop marks"));
    do {
	if(readLine(this, FALSE) == FALSE)
	{
	 rc = -1;
	 break;
	}
       } while(strcmp(this->line, "%AI3_EndCrops"));
   }
	  
   Xc_free(n_word);
   break;

  case XcAIDF_OPEN_MARK:
   Xc_TRACE(("open mark"));
   sptr = stack + index++;
   sptr->type = XcAI_OPEN_ITEM;
   break;

  case XcAIDF_CLOSE_MARK:
   Xc_TRACE(("close mark"));
   sptr = stack + index++;
   sptr->type = XcAI_CLOSE_ITEM;
   break;
	  
  case XcAIDF_WORD:
   if(n_len == 0)
   {
    if(n_word != NULL) Xc_free(n_word);
    break;
   }

   if(((*n_word >= '0') && (*n_word <= '9')) ||
      ((*n_word == '.') && (n_word[1] >= '0') && (n_word[1] <= '9')) ||
      ((*n_word == '-') &&
       (((n_word[1] >= '0') && (n_word[1] <= '9')) || 
	((n_word[1] == '.') &&
	 (n_word[2] >= '0') && (n_word[2] <= '9'))))) 
   {
    sptr = stack + index++;
    sptr->type = XcAI_NUMBER_ITEM;
    sptr->data.number = (real)atof(n_word);
    Xc_free(n_word);	  
    break;
   }
	  
   if(n_len == 1)
    switch(n_word[0])
    {	
     /*--- Pattern defintion ---*/
    case 'E':
     Xc_TRACE(("Pattern definition"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_CLOSE(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     do {
	 sptr = XcAIDF_POP(stack, index, 1);
	 if(XcAIDF_IS_STRING(sptr) || XcAIDF_IS_NAME(sptr))
	  Xc_free(sptr->data.string);
	} while(!XcAIDF_IS_OPEN(sptr));
     sptr = XcAIDF_POP(stack, index, 5);
     if(!XcAIDF_IS_STRING(sptr) ||
	!XcAIDF_IS_NUMBER(sptr + 1) ||
	!XcAIDF_IS_NUMBER(sptr + 2) ||
	!XcAIDF_IS_NUMBER(sptr + 3) ||
	!XcAIDF_IS_NUMBER(sptr + 4)) {XcAIDF_TYPE_MISMATCH(rc);}
     Xc_free(sptr->data.string);
     break;

     /*--- Pattern layer color definition ---*/
    case '@':
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_STRING(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     Xc_free(sptr->data.string);
     break;

     /*--- Pattern layer tile definition ---*/
    case '&':
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_STRING(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     Xc_free(sptr->data.string);
     break;

     /*--- Guide operator ---*/
    case '*':
     Xc_TRACE(("Guide"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_STRING(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     Xc_free(sptr->data.string);

     if(!F(vector).abortPath(vector))
      goto vector_error;
     break;

     /*--- `Lock' operator ---*/
    case 'A':
     Xc_TRACE(("lock"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     break;
		
     /*--- setdash ---*/
    case 'd':
     {
      coord_t phase, *pattern = NULL;
      boolean flag;
      int plen;

      Xc_TRACE(("setdash"));
      sptr = XcAIDF_POP(stack, index, 2);
      if(!XcAIDF_IS_NUMBER(sptr + 1) ||
	 !XcAIDF_IS_CLOSE(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
      phase = SCALE_FROM_POINTS(sptr[1].data.number);
      plen = index;
      do {
	  sptr = XcAIDF_POP(stack, index, 1);
	 } while(!XcAIDF_IS_OPEN(sptr));
      if(index < 0) break;
      plen -= index + 1;
      if(plen > 0)
      {
       int i;

       pattern = Xc_malloc("dashes", plen * sizeof(coord_t));
       if(pattern == NULL) goto vector_error;
       for(i = 0; i < plen; i++)
	pattern[i]= SCALE_FROM_POINTS(sptr[i + 1].data.number);
      }

      flag = F(vector).setLineDash(vector, plen, phase, pattern);
      if(pattern != NULL) Xc_free(pattern);
      if(!flag) goto vector_error;
      break;
     }
		
     /*--- flatness ---*/
    case 'i':
     Xc_TRACE(("setflatness"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     if(!F(vector).setFlatness(vector, (int)sptr->data.number))
      goto vector_error;
     break;
		
     /*--- Winding order ---*/
    case 'D':
     Xc_TRACE(("Winding order"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     break;
		
     /*--- setlinejoin ---*/
    case 'j':
     Xc_TRACE(("setlinejoin"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     if(!F(vector).setLineJoin(vector, (int)sptr->data.number))
      goto vector_error;
     break;
		
     /*--- Line cap ---*/
    case 'J':
     Xc_TRACE(("setlinecap"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     if(!F(vector).setLineCap(vector, (int)sptr->data.number))
      goto vector_error;
     break;
		
     /*--- Miter limit ---*/
    case 'M':
     Xc_TRACE(("setmiterlimit"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     if(!F(vector).setMiterLimit(vector, (real)sptr->data.number))
      goto vector_error;
     break;
		
     /*--- Line width ---*/
    case 'w':
     Xc_TRACE(("setlinewidth"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     if(!F(vector).setLineWidth(vector, SCALE_FROM_POINTS(sptr->data.number))) goto vector_error;

     break;
		
     /*--- Grayscale fill ---*/
    case 'g':
     Xc_TRACE(("gray fill"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}

     color = F(*cbase).getColor(cbase, XcC_GRAY,
				XcC_SCALE_COLOR(sptr->data.number),
				XcC_END);
     if((color != NULL) && !F(vector).setFillColor(vector, color))
      goto vector_error;
     break;
		
     /*--- Grayscale stroke ---*/
    case 'G':
     Xc_TRACE(("gray stroke"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}

     color = F(*cbase).getColor(cbase, XcC_GRAY,
				XcC_SCALE_COLOR(sptr->data.number),
				XcC_END);
     if((color != NULL) && !F(vector).setStrokeColor(vector, color))
      goto vector_error;
     break;
		
     /*--- CMYK fill ---*/
    case 'k':
     Xc_TRACE(("cmyk fill"));
     sptr = XcAIDF_POP(stack, index, 4);
     if(!XcAIDF_IS_NUMBER(sptr) ||
	!XcAIDF_IS_NUMBER(sptr + 1) ||
	!XcAIDF_IS_NUMBER(sptr + 2) ||
	!XcAIDF_IS_NUMBER(sptr + 3)) {XcAIDF_TYPE_MISMATCH(rc);}
		
     color= F(*cbase).getColor(cbase, XcC_CMYK,
			       XcC_SCALE_COLOR(sptr->data.number),
			       XcC_SCALE_COLOR(sptr[1].data.number),
			       XcC_SCALE_COLOR(sptr[2].data.number),
			       XcC_SCALE_COLOR(sptr[3].data.number),
			       XcC_END);
     if((color != NULL) && !F(vector).setFillColor(vector, color))
      goto vector_error;
     break;
		
     /*--- CMYK stroke ---*/
    case 'K':
     Xc_TRACE(("cmyk stroke"));
     sptr = XcAIDF_POP(stack, index, 4);
     if(!XcAIDF_IS_NUMBER(sptr) ||
	!XcAIDF_IS_NUMBER(sptr + 1) ||
	!XcAIDF_IS_NUMBER(sptr + 2) ||
	!XcAIDF_IS_NUMBER(sptr + 3)) {XcAIDF_TYPE_MISMATCH(rc);} 
		
     color= F(*cbase).getColor(cbase, XcC_CMYK,
			       XcC_SCALE_COLOR(sptr->data.number),
			       XcC_SCALE_COLOR(sptr[1].data.number),
			       XcC_SCALE_COLOR(sptr[2].data.number),
			       XcC_SCALE_COLOR(sptr[3].data.number),
			       XcC_END);
     if((color != NULL) && !F(vector).setStrokeColor(vector, color))
      goto vector_error;
     break;
		
     /*--- Custom color fill ---*/
    case 'x':
     Xc_TRACE(("custom fill"));
     sptr = XcAIDF_POP(stack, index, 6);
     if(!XcAIDF_IS_NUMBER(sptr) ||
	!XcAIDF_IS_NUMBER(sptr + 1) ||
	!XcAIDF_IS_NUMBER(sptr + 2) ||
	!XcAIDF_IS_NUMBER(sptr + 3) ||
	!XcAIDF_IS_STRING(sptr + 4) ||
	!XcAIDF_IS_NUMBER(sptr + 5)) {XcAIDF_TYPE_MISMATCH(rc);}

     color= F(*cbase).getColor(cbase, XcC_CMYK,
			       XcC_SCALE_COLOR(sptr->data.number),
			       XcC_SCALE_COLOR(sptr[1].data.number),
			       XcC_SCALE_COLOR(sptr[2].data.number),
			       XcC_SCALE_COLOR(sptr[3].data.number),
			       XcC_NAME, sptr[4].data.string,
			       XcC_END);
     Xc_free(sptr[4].data.string);
     if((color != NULL) && !F(vector).setFillColor(vector, color))
      goto vector_error;
     break;

     /*--- Custom stroke color ---*/
    case 'X':
     Xc_TRACE(("custom stroke"));
     sptr = XcAIDF_POP(stack, index, 6);
     if(!XcAIDF_IS_NUMBER(sptr) ||
	!XcAIDF_IS_NUMBER(sptr + 1) ||
	!XcAIDF_IS_NUMBER(sptr + 2) ||
	!XcAIDF_IS_NUMBER(sptr + 3) ||
	!XcAIDF_IS_STRING(sptr + 4) ||
	!XcAIDF_IS_NUMBER(sptr + 5)) {XcAIDF_TYPE_MISMATCH(rc);}

     color= F(*cbase).getColor(cbase, XcC_CMYK,
			       XcC_SCALE_COLOR(sptr->data.number),
			       XcC_SCALE_COLOR(sptr[1].data.number),
			       XcC_SCALE_COLOR(sptr[2].data.number),
			       XcC_SCALE_COLOR(sptr[3].data.number),
			       XcC_NAME, sptr[4].data.string,
			       XcC_END);
     Xc_free(sptr[4].data.string);

     if((color != NULL) && !F(vector).setStrokeColor(vector, color))
      goto vector_error;
     break;
	

     /*--- Fill pattern ---*/
    case 'p':
     Xc_TRACE(("fill pattern"));
     sptr = XcAIDF_POP(stack, index, 18);
     if(!XcAIDF_IS_STRING(sptr) ||
	!XcAIDF_IS_NUMBER(sptr + 1) ||
	!XcAIDF_IS_NUMBER(sptr + 2) ||
	!XcAIDF_IS_NUMBER(sptr + 3) ||
	!XcAIDF_IS_NUMBER(sptr + 4) ||
	!XcAIDF_IS_NUMBER(sptr + 5) ||
	!XcAIDF_IS_NUMBER(sptr + 6) ||
	!XcAIDF_IS_NUMBER(sptr + 7) ||
	!XcAIDF_IS_NUMBER(sptr + 8) ||
	!XcAIDF_IS_NUMBER(sptr + 9) ||
	!XcAIDF_IS_OPEN(sptr + 10) ||
	!XcAIDF_IS_NUMBER(sptr + 11) ||
	!XcAIDF_IS_NUMBER(sptr + 12) ||
	!XcAIDF_IS_NUMBER(sptr + 13) ||
	!XcAIDF_IS_NUMBER(sptr + 14) ||
	!XcAIDF_IS_NUMBER(sptr + 15) ||
	!XcAIDF_IS_NUMBER(sptr + 16) ||
	!XcAIDF_IS_CLOSE(sptr + 17)) {XcAIDF_TYPE_MISMATCH(rc);}
     Xc_free(sptr->data.string);
     break;
		
     /*--- Stroke pattern ---*/
    case 'P':
     Xc_TRACE(("stroke pattern"));
     sptr = XcAIDF_POP(stack, index, 18);
     if(!XcAIDF_IS_STRING(sptr) ||
	!XcAIDF_IS_NUMBER(sptr + 1) ||
	!XcAIDF_IS_NUMBER(sptr + 2) ||
	!XcAIDF_IS_NUMBER(sptr + 3) ||
	!XcAIDF_IS_NUMBER(sptr + 4) ||
	!XcAIDF_IS_NUMBER(sptr + 5) ||
	!XcAIDF_IS_NUMBER(sptr + 6) ||
	!XcAIDF_IS_NUMBER(sptr + 7) ||
	!XcAIDF_IS_NUMBER(sptr + 8) ||
	!XcAIDF_IS_NUMBER(sptr + 9) ||
	!XcAIDF_IS_OPEN(sptr + 10) ||
	!XcAIDF_IS_NUMBER(sptr + 11) ||
	!XcAIDF_IS_NUMBER(sptr + 12) ||
	!XcAIDF_IS_NUMBER(sptr + 13) ||
	!XcAIDF_IS_NUMBER(sptr + 14) ||
	!XcAIDF_IS_NUMBER(sptr + 15) ||
	!XcAIDF_IS_NUMBER(sptr + 16) ||
	!XcAIDF_IS_CLOSE(sptr + 17)) {XcAIDF_TYPE_MISMATCH(rc);}
     Xc_free(sptr->data.string);
     break;
		
     /*--- Fill overprint ---*/
    case 'O':
     Xc_TRACE(("fill overprint"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     break;

     /*--- Stroke overprint ---*/
    case 'R':
     Xc_TRACE(("stroke overprint"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     break;
		
     /*--- Start group ---*/
    case 'u':
     Xc_TRACE(("start group"));
     break;

     /*--- End group ---*/
    case 'U':
     Xc_TRACE(("end group"));
     break;

     /*--- Start clip group ---*/
    case 'q':
     Xc_TRACE(("start clip group"));
     if(!polarity && !F(vector).save(vector))
      goto vector_error;
     break;

     /*--- End clip group ---*/
    case 'Q':
     Xc_TRACE(("end clip group"));
     if(!polarity && !F(vector).restore(vector))
      goto vector_error;
     break;

     /*--- Do nothing ---*/
    case 'H':
     Xc_TRACE(("nothing (`H')"));
     break;

     /*--- closepath ---*/
    case 'h':
     Xc_TRACE(("closepath"));
     if(!F(vector).closePath(vector)) goto vector_error;
     break;

     /*--- moveto ---*/
    case 'm':
     Xc_TRACE(("moveto"));
     sptr = XcAIDF_POP(stack, index, 2);
     if(!XcAIDF_IS_NUMBER(sptr) ||
	!XcAIDF_IS_NUMBER(sptr + 1)) {XcAIDF_TYPE_MISMATCH(rc);}
		
     if(!F(vector).move(vector,
			SCALE_FROM_POINTS(sptr->data.number),
			SCALE_FROM_POINTS(sptr[1].data.number)))
      goto vector_error;
     break;
		
     /*--- lineto ---*/
    case 'l':
    case 'L':
     Xc_TRACE(("lineto"));
     sptr = XcAIDF_POP(stack, index, 2);
     if(!XcAIDF_IS_NUMBER(sptr) ||
	!XcAIDF_IS_NUMBER(sptr + 1)) {XcAIDF_TYPE_MISMATCH(rc);}
		
     if(!F(vector).line(vector,
			SCALE_FROM_POINTS(sptr->data.number),
			SCALE_FROM_POINTS(sptr[1].data.number)))
      goto vector_error;
     break;
		
     /*--- curveto ---*/
    case 'C':
    case 'c':
     Xc_TRACE(("curveto"));
     sptr = XcAIDF_POP(stack, index, 6);
     if(!XcAIDF_IS_NUMBER(sptr) || !XcAIDF_IS_NUMBER(sptr + 1) ||
	!XcAIDF_IS_NUMBER(sptr + 2) ||
	!XcAIDF_IS_NUMBER(sptr + 3) ||
	!XcAIDF_IS_NUMBER(sptr + 4) ||
	!XcAIDF_IS_NUMBER(sptr + 5)) {XcAIDF_TYPE_MISMATCH(rc);}
		
     if(!F(vector).curve(vector,
			 SCALE_FROM_POINTS(sptr->data.number),
			 SCALE_FROM_POINTS(sptr[1].data.number),
			 SCALE_FROM_POINTS(sptr[2].data.number),
			 SCALE_FROM_POINTS(sptr[3].data.number),
			 SCALE_FROM_POINTS(sptr[4].data.number),
			 SCALE_FROM_POINTS(sptr[5].data.number)))
      goto vector_error;
     break;
		
     /*--- curveto (control point 1 = current point) ---*/
    case 'V':
    case 'v':
     Xc_TRACE(("vcurveto"));
     sptr = XcAIDF_POP(stack, index, 4);
     if(!XcAIDF_IS_NUMBER(sptr) || !XcAIDF_IS_NUMBER(sptr + 1) ||
	!XcAIDF_IS_NUMBER(sptr + 2) ||
	!XcAIDF_IS_NUMBER(sptr + 3)) {XcAIDF_TYPE_MISMATCH(rc);}
		
     if(!F(vector).curveBegin(vector,
			      SCALE_FROM_POINTS(sptr->data.number),
			      SCALE_FROM_POINTS(sptr[1].data.number),
			      SCALE_FROM_POINTS(sptr[2].data.number),
			      SCALE_FROM_POINTS(sptr[3].data.number)))
      goto vector_error;

     break;

     /*--- curveto (last point = control point 2) ---*/
    case 'Y':
    case 'y':
     Xc_TRACE(("ycurveto"));
     sptr = XcAIDF_POP(stack, index, 4);
     if(!XcAIDF_IS_NUMBER(sptr) || !XcAIDF_IS_NUMBER(sptr + 1) ||
	!XcAIDF_IS_NUMBER(sptr + 2) ||
	!XcAIDF_IS_NUMBER(sptr + 3)) {XcAIDF_TYPE_MISMATCH(rc);}
		
     if(!F(vector).curveEnd(vector,
			    SCALE_FROM_POINTS(sptr->data.number),
			    SCALE_FROM_POINTS(sptr[1].data.number),
			    SCALE_FROM_POINTS(sptr[2].data.number),
			    SCALE_FROM_POINTS(sptr[3].data.number)))
      goto vector_error;

     break;
		

     /*--- closepath and newpath ---*/
    case 'n':
    case 'N':
     if(text_flag == FALSE)
      if(!polarity)
      {
       if(clip_flag)
       {
	if(!F(vector).clip(vector) ||
	   !F(vector).newPath(vector)) goto vector_error;
	clip_flag = FALSE;
       }
       else
	if(!F(vector).newPath(vector)) goto vector_error;
      }
      else
       render_mode = 0;
     else
      if(!F(vector).abortPath(vector)) goto vector_error;
     break;
		
     /*--- Fill path with closepath ---*/
    case 'f':
     if(!F(vector).closePath(vector)) goto vector_error;
     /*--- fill path---*/
    case 'F':
     Xc_TRACE(("fill"));
     if(!polarity)
     {
      if(clip_flag)
      {
       if(!F(vector).save(vector) ||
	  !F(vector).render(vector, XcVG_FILL) ||
	  !F(vector).restore(vector) ||
	  !F(vector).clip(vector) ||
	  !F(vector).newPath(vector)) goto vector_error;
       clip_flag = FALSE;
      }
      else
       if(!F(vector).render(vector, XcVG_FILL))
	goto vector_error;
     }
     else
      render_mode = XcVG_FILL;
     break;
		
     /*--- Closepath and stroke ---*/
    case 's':
     if(!F(vector).closePath(vector)) goto vector_error;
     /*--- Stroke ---*/
    case 'S':
     Xc_TRACE(("stroke"));
     if(!polarity)
     {
      if(clip_flag)
      {
       if(!F(vector).save(vector) ||
	  !F(vector).render(vector, XcVG_STROKE) ||
	  !F(vector).restore(vector) ||
	  !F(vector).clip(vector) ||
	  !F(vector).newPath(vector)) goto vector_error;
       clip_flag = FALSE;
      }
      else
       if(!F(vector).render(vector, XcVG_STROKE))
	goto vector_error;
     }
     else
      render_mode = XcVG_STROKE;
     break;

     /*--- Closepath, fill and stroke ---*/
    case 'b':
     if(!F(vector).closePath(vector)) goto vector_error;
     /*--- Fill and stroke ---*/
    case 'B':
     Xc_TRACE(("fill & stroke"));
     if(!polarity)
     {
      if(clip_flag)
      {
       if(!F(vector).save(vector) ||
	  !F(vector).render(vector, XcVG_FILL|XcVG_STROKE) ||
	  !F(vector).restore(vector) ||
	  !F(vector).clip(vector) ||
	  !F(vector).newPath(vector)) goto vector_error;
       clip_flag = FALSE;
      }
      else
       if(!F(vector).render(vector, XcVG_FILL|XcVG_STROKE))
	goto vector_error;
     }
     else
      render_mode = XcVG_FILL|XcVG_STROKE;
     break;

     /*--- Add path to clipping mask ---*/
    case 'W':
     Xc_TRACE(("clip"));
     clip_flag = TRUE;
     break;

     /*--- Unknown command ---*/
    default:
     Xc_TRACE(("Unknow command `%s'", n_word));
     while(index != 0)
     {
      sptr = stack + --index;
      if(XcAIDF_IS_STRING(sptr)) Xc_free(sptr->data.string);
     }
     break;

     /*--- Error ---*/
    vector_error:
     Xc_ERROR(("Cannot load AI file"));
     Xc_free(stack);
     Xc_free(n_word);
     return FALSE;
    }
   else if((n_len == 2) && (*n_word == '*'))
    switch(n_word[1])
    {
     /*--- Compound path group ---*/
    case 'u':
     Xc_TRACE(("Compound path group"));
     ++polarity;
     break;

     /*--- Compound path ungroup ---*/
    case 'U':
     Xc_TRACE(("Compound path ungroup"));
     if(--polarity == 0)
     {
      if(clip_flag)
      {
       if(!F(vector).save(vector) ||
	  !F(vector).render(vector, render_mode) ||
	  !F(vector).restore(vector) ||
	  !F(vector).clip(vector) ||
	  !F(vector).newPath(vector)) goto compound_error;
       clip_flag = FALSE;
      }
      else
       if(!F(vector).render(vector, render_mode))
	goto compound_error;
     }
     break;

    compound_error:
     Xc_ERROR(("Cannot load AI file"));
     Xc_free(stack);
     Xc_free(n_word);
     return FALSE;
		
     /*--- Wraparound group ---*/
    case 'w':
     break;

     /*--- Wraparound ungroup ---*/
    case 'W':
     break;
    }
   else if((n_len == 2) && (*n_word == 'T'))
    switch(n_word[1])
    {
     /*--- Begin text object ---*/
    case 'o':
     Xc_TRACE(("begin text object"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     text_flag = TRUE;
     break;

     /*--- End text object ---*/
    case 'O':
     Xc_TRACE(("end text object"));
     tot_length = 0;
     t_length = 0;
     text_flag = FALSE;
     break;

     /*--- Begin text path ---*/
    case 'p':
     {
      matrix_t matrix;

      Xc_TRACE(("begin text path"));
      sptr = XcAIDF_POP(stack, index, 7);
      if(!XcAIDF_IS_NUMBER(sptr) ||
	 !XcAIDF_IS_NUMBER(sptr + 1) ||
	 !XcAIDF_IS_NUMBER(sptr + 2) ||
	 !XcAIDF_IS_NUMBER(sptr + 3) ||
	 !XcAIDF_IS_NUMBER(sptr + 4) ||
	 !XcAIDF_IS_NUMBER(sptr + 5) ||
	 !XcAIDF_IS_NUMBER(sptr + 6)) {XcAIDF_TYPE_MISMATCH(rc);}

      XcM_INIT_MATRIX(&matrix,
		      sptr->data.number, sptr[1].data.number,
		      sptr[2].data.number, sptr[3].data.number,
		      SCALE_FROM_POINTS(sptr[4].data.number),
		      SCALE_FROM_POINTS(sptr[5].data.number));
      if(!F(vector).setTextMatrix(vector, &matrix))
       goto text_error;
     }

     /*--- End text path ---*/
    case 'P':
     Xc_TRACE(("end text path"));
     break;

     /*--- Set text matrix ---*/
    case 'm':
     {
      matrix_t matrix;
		  
      Xc_TRACE(("text matrix"));
      sptr = XcAIDF_POP(stack, index, 6);
      if(!XcAIDF_IS_NUMBER(sptr) ||
	 !XcAIDF_IS_NUMBER(sptr + 1) ||
	 !XcAIDF_IS_NUMBER(sptr + 2) ||
	 !XcAIDF_IS_NUMBER(sptr + 3) ||
	 !XcAIDF_IS_NUMBER(sptr + 4) ||
	 !XcAIDF_IS_NUMBER(sptr + 5)) {XcAIDF_TYPE_MISMATCH(rc);}
		  
      XcM_INIT_MATRIX(&matrix,
		      sptr->data.number, sptr[1].data.number,
		      sptr[2].data.number, sptr[3].data.number,
		      SCALE_FROM_POINTS(sptr[4].data.number),
		      SCALE_FROM_POINTS(sptr[5].data.number));
		  
      if(!F(vector).setTextMatrix(vector, &matrix))
       goto text_error;
      break;
     }

     /*--- Translate text matrix ---*/
    case 'd':
     Xc_TRACE(("text matrix translate"));
     sptr = XcAIDF_POP(stack, index, 2);
     if(!XcAIDF_IS_NUMBER(sptr) || !XcAIDF_IS_NUMBER(sptr + 1))
     {XcAIDF_TYPE_MISMATCH(rc);}
     last_dy = SCALE_FROM_POINTS(sptr[1].data.number);
     if(!F(vector).setTextPoint
	(vector, SCALE_FROM_POINTS(sptr->data.number)-tot_length,
	 SCALE_FROM_POINTS(sptr[1].data.number))) goto text_error;
     t_length = 0;
     tot_length = 0;
     break;

     /*--- Set render mode ---*/
    case 'r':
     Xc_TRACE(("text render mode"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     if(!F(vector).setTextMode(vector, (int)sptr->data.number))
      goto text_error;
     break;

     /*--- Set font name and size ---*/
    case 'f':
     {
      c_VectorFont *font;

      Xc_TRACE(("set font"));
      sptr = XcAIDF_POP(stack, index, 2);
      if(!XcAIDF_IS_NAME(sptr) || !XcAIDF_IS_NUMBER(sptr + 1))
      {XcAIDF_TYPE_MISMATCH(rc);}
      font = aiFindFont(this, sptr->data.string);
      Xc_free(sptr->data.string);
      if(font == NULL) goto text_error;
      /*-- uglly method but the object was not well concepted --*/
      last_font = font;
      last_size = SCALE_FROM_POINTS(sptr[1].data.number);

      if(!F(vector).setTextFont
	 (vector, font, SCALE_FROM_POINTS(sptr[1].data.number)))
       goto text_error;
      break;
     }

     /*--- Set leading ---*/
    case 'l':
     Xc_TRACE(("text leading"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     if(!F(vector).setTextLeading
	(vector, SCALE_FROM_POINTS(sptr->data.number)))
      goto text_error;
     break;

     /*--- Set computed word spacing ---*/
    case 'w':
     Xc_TRACE(("text word spacing"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     if(!F(vector).setTextWordSpacing
	(vector, SCALE_FROM_POINTS(sptr->data.number)))
      goto text_error;
     break;

     /*--- Set computed character spacing ---*/
    case 'c':
     Xc_TRACE(("text character spacing"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     if(!F(vector).setTextCharSpacing
	(vector, SCALE_FROM_POINTS(sptr->data.number)))
      goto text_error;
     break;

     /*--- Set super/subscripting ---*/
    case 's':
     Xc_TRACE(("text rise"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     if(!F(vector).setTextRise
	(vector, SCALE_FROM_POINTS(sptr->data.number)))
      goto text_error;
     break;

     /*--- Set horizontal scaling ---*/
    case 'z':
     Xc_TRACE(("text horizontal scaling"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     if(!F(vector).setTextHScale(vector, SCALE_FROM_PERCENT(sptr->data.number)))
      goto text_error;
     break;

     /*--- Non justified text ---*/
    case 'x':
     Xc_TRACE(("non-justified text"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_STRING(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     if(!F(vector).textRender(vector, sptr->data.string,
			      strlen(sptr->data.string)))
     {
      Xc_free(sptr->data.string);
      goto text_error;
     }
     Xc_free(sptr->data.string);
     break;

     /*--- Justified text ---*/
    case 'j':
     {
      c_CharMetrics *cm;

      Xc_TRACE(("justified text"));

      sptr = XcAIDF_POP(stack, index, 1);
      if(!XcAIDF_IS_STRING(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
      if (t_length)
      {
       tot_length += t_length;
       if(!F(vector).setTextPoint(vector, 
				  t_length,
				  0))
	goto text_error;
      }

      if(!F(vector).textRender(vector, sptr->data.string,
			       strlen(sptr->data.string)))
      {
       Xc_free(sptr->data.string);
       goto text_error;
      }

      if (last_font)
      {
       cm = COPY(c_CharMetrics)(last_font->metrics,
				NULL,
				last_size,
				SCALE_FROM_COEF(1.0),
				SCALE_FROM_DEGREES(0.0),
				0, TRUE);
       if(cm != NULL)
       {
	t_length = F(cm).textWidth(cm, sptr->data.string,
				   strlen(sptr->data.string));
	DELETE(c_CharMetrics)(cm);
       }
       else
	t_length = 0;
      }
      Xc_free(sptr->data.string);
      break;
     }

     /*--- Kern ---*/
    case 'k':
     Xc_TRACE(("text kerning"));
     sptr = XcAIDF_POP(stack, index, 2);
     if(!XcAIDF_IS_NUMBER(sptr) || !XcAIDF_IS_NUMBER(sptr + 1))
     {XcAIDF_TYPE_MISMATCH(rc);}
     if(!F(vector).setTextKerning(vector, (real)sptr->data.number))
      goto text_error;
     break;

     /*--- Translate matrix to start of new line ---*/
    case '*':
     Xc_TRACE(("text newline"));
     if(!F(vector).textNewline(vector)) goto text_error;
     break;

     /*--- Print a discretionary hyphen ---*/
    case '-':
     Xc_TRACE(("text hyphen"));
     if(!F(vector).textRender(vector, "-", 1)) goto text_error;
     break;

     /*--- Set standard platform encoding ---*/
    case 'E':
     break;

     /*--- Set encoding vector ---*/
    case 'Z':
     break;

     /*--- Discretionary hyphen ---*/
    case '+':
     Xc_TRACE(("text discretionary hyphen"));
     break;

     /*--- Overflow text ---*/
    case 'X':
     Xc_TRACE(("text overflow"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_STRING(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     Xc_free(sptr->data.string);
     break;

     /*--- Set user tracking ---*/
    case 't':
     Xc_TRACE(("text user tracking"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     break;
		
     /*--- Set word spacing ---*/
    case 'W':
     Xc_TRACE(("text word spacing"));
     sptr = XcAIDF_POP(stack, index, 3);
     if(!XcAIDF_IS_NUMBER(sptr) || !XcAIDF_IS_NUMBER(sptr + 1) ||
	!XcAIDF_IS_NUMBER(sptr + 2)) {XcAIDF_TYPE_MISMATCH(rc);}
     break;

     /*--- Set character spacing ---*/
    case 'C':
     Xc_TRACE(("text character spacing"));
     sptr = XcAIDF_POP(stack, index, 3);
     if(!XcAIDF_IS_NUMBER(sptr) || !XcAIDF_IS_NUMBER(sptr + 1) ||
	!XcAIDF_IS_NUMBER(sptr + 2)) {XcAIDF_TYPE_MISMATCH(rc);}
     break;

     /*--- Non-printing kern ---*/
    case 'K':
     Xc_TRACE(("text non printing kern"));
     sptr = XcAIDF_POP(stack, index, 2);
     if(!XcAIDF_IS_NUMBER(sptr) || !XcAIDF_IS_NUMBER(sptr + 1))
     {XcAIDF_TYPE_MISMATCH(rc);}
     break;
		
     /*--- Set pairwise kerning ---*/
    case 'A':
     Xc_TRACE(("text pairwise kerning"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     break;

     /*--- Set hanging quotes ---*/
    case 'q':
     Xc_TRACE(("text hanging quote"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     break;
		
     /*--- Set indentation ---*/
    case 'i':
     Xc_TRACE(("text indentation"));
     sptr = XcAIDF_POP(stack, index, 3);
     if(!XcAIDF_IS_NUMBER(sptr) || !XcAIDF_IS_NUMBER(sptr + 1) ||
	!XcAIDF_IS_NUMBER(sptr + 2)) {XcAIDF_TYPE_MISMATCH(rc);}
     break;

     /*--- Set alignement ---*/
    case 'a':
     Xc_TRACE(("text alignement"));
     sptr = XcAIDF_POP(stack, index, 1);
     if(!XcAIDF_IS_NUMBER(sptr)) {XcAIDF_TYPE_MISMATCH(rc);}
     break;

     /*--- Reset pattern matrix ---*/
    case 'R':
     Xc_TRACE(("text pattern matrix reset"));
     sptr = XcAIDF_POP(stack, index, 6);
     if(!XcAIDF_IS_NUMBER(sptr) ||
	!XcAIDF_IS_NUMBER(sptr + 1) ||
	!XcAIDF_IS_NUMBER(sptr + 2) ||
	!XcAIDF_IS_NUMBER(sptr + 3) ||
	!XcAIDF_IS_NUMBER(sptr + 4) ||
	!XcAIDF_IS_NUMBER(sptr + 5)) {XcAIDF_TYPE_MISMATCH(rc);}
     break;

     /*--- Unknow text operator ---*/
    default:
     Xc_TRACE(("Unknow text command `%s'", n_word));
     while(index != 0)
     {
      sptr = stack + --index;
      if(XcAIDF_IS_STRING(sptr) || XcAIDF_IS_NAME(sptr))
       Xc_free(sptr->data.string);
     }
     break;

     /*--- Error ---*/
    text_error:
     Xc_ERROR(("Cannot load AI file"));
     Xc_free(stack);
     Xc_free(n_word);
     return FALSE;
    }
   else if((n_len == 8) && !strcmp(n_word, "showpage"))
    exit_flag = TRUE;
   else
   {
    Xc_TRACE(("Unknow command `%s'", n_word));
    while(index != 0)
    {
     sptr = stack + --index;
     if(XcAIDF_IS_STRING(sptr) || XcAIDF_IS_NAME(sptr))
      Xc_free(sptr->data.string);
    }
   }
	  
   if((index < 0) || (rc < 0))
   {
    Xc_free(n_word);	  
    Xc_ERROR(((index < 0)? "Stack underflow" : "Type mismatch"));
    Xc_free(stack);
    return FALSE;
   }
	
   Xc_free(n_word);	  
   break;
  }
 }

 Xc_free(stack);

 if(!F(vector).end(vector)) return FALSE;

 /*--- Crop region ---*/
 if((crop.llx != crop.urx) && (crop.lly != crop.ury))
{
 Xc_DUMP_BBOX("set crop region", &crop);
 this->vector->bbox.llx = crop.llx;
 this->vector->bbox.lly = crop.lly;
 this->vector->bbox.urx = crop.urx;
 this->vector->bbox.ury = crop.ury;
}
 else if(((bbox.llx != bbox.urx) && (bbox.lly != bbox.ury) &&
	  Xc_BBOX_TEST(&bbox, vector->bbox.llx, vector->bbox.lly) &&
	  Xc_BBOX_TEST(&bbox, vector->bbox.urx, vector->bbox.ury)) ||
	 ((this->vector->bbox.llx == this->vector->bbox.urx) &&
	  (this->vector->bbox.lly == this->vector->bbox.ury)))
{
 /*--- Hack for text bounding region ---*/
 Xc_DUMP_BBOX("bbox", &crop);
 this->vector->bbox.llx = bbox.llx;
 this->vector->bbox.lly = bbox.lly;
 this->vector->bbox.urx = bbox.urx;
 this->vector->bbox.ury = bbox.ury;
}
 Xc_DUMP_BBOX("final bbox", &this->vector->bbox);

 return((rc == XcAIDF_ERROR)? FALSE: TRUE);
}


/* ----------------------------------------------------------------- ** 
** readClassic - Read from the file                                  ** 
** ----------------------------------------------------------------- */
static int readClassic(this, ptr, len)
c_AIDocFile	*this;
unsigned char	*ptr;
long		len;
{
 int	len2;

 Xc_HISTORY(("readClassic(`%s')", GetBasename(this->filename, FALSE)));

 if (this->buffer_index >= XcAIDF_BUFFER_SIZE)
 {
  return read(this->handle, ptr, len);
 }
 else
 {
  len2 = XcAIDF_BUFFER_SIZE-this->buffer_index;
  if (len < len2)
   memcpy(ptr, this->buffer_file+this->buffer_index, len);
  else
  {
   memcpy(ptr, this->buffer_file+this->buffer_index, len2);
   len -= len2;
   this->buffer_index = XcAIDF_BUFFER_SIZE;
   return len2+read(this->handle, ptr+len2, len);
  }
  return len2;
 }
}


/* ----------------------------------------------------------------- ** 
** Readword - Read a word from the file                              ** 
** ----------------------------------------------------------------- */
static int readWord(this, pptr, plen)
c_AIDocFile	*this;
unsigned char	**pptr;
long		*plen;
{
 register unsigned char c, *ptr;
 unsigned long len;
 boolean name_flag;

 /*Xc_TRACE(("readWord"));*/

 *pptr = NULL;
 *plen = 0;
 name_flag = FALSE;

 if((ptr = this->line_ptr) == NULL)
 {
  if(readLine(this, FALSE) == FALSE) return XcAIDF_ERROR;
  ptr = this->line_ptr;
 }
  
 /*--- Skip space ---*/
 while(42)
 {
  c = *ptr++;
  if(c == 0)
  {
   if(readLine(this, FALSE) == FALSE) return XcAIDF_ERROR;
   ptr = this->line_ptr;
  }
  else if(c == '/')
  {
   name_flag = TRUE;
   break;
  }
  else if((c > 32) || (c <= XcAIDF_LAST_CODE))
  {
   ptr--;
   break;
  }
 }
  
 if(c < XcAIDF_LAST_CODE)
 {
  unsigned char *start;

  if(c != XcAIDF_STRING_START_CODE) return XcAIDF_ERROR;

  /*Xc_TRACE(("string"));*/

  start = ++ptr;
  while((*ptr != 0) && (*ptr != XcAIDF_STRING_END_CODE)) ptr++;
  len = ptr - start;

  if((*pptr = Xc_malloc("string", len + 1)) == NULL) return XcAIDF_ERROR;
  memcpy(*pptr, start, len);
  (*pptr)[len] = 0;

  /*--- Multiline strings ---*/
  while(*ptr == 0)
  {
   long old_len;
   char *str;

   if(readLine(this, FALSE) == FALSE)
   {
    Xc_free(*pptr); *pptr = NULL;
    return XcAIDF_ERROR;
   }

   ptr = start = this->line_ptr;
   while((*ptr != 0) && (*ptr != XcAIDF_STRING_END_CODE)) ptr++;
   old_len = len;
   len += ptr - start;
   if((str = Xc_realloc("string", *pptr, len + 1)) == NULL)
   {
    Xc_free(*pptr); *pptr = NULL;
    return XcAIDF_ERROR;
   }
   memcpy(str + old_len, start, len - old_len);
   str[len] = 0;
   *pptr = (unsigned char *)str;
  }

  *plen = len;
  this->line_ptr = ptr + 1;
  return XcAIDF_STRING;
 }
 else if(c == '%')
 {
  /*Xc_TRACE(("comment"));*/

  len = strlen(ptr + 1);
  if((*pptr = Xc_malloc("comment", len + 1)) == NULL) return XcAIDF_ERROR;
  *plen = len;
  memcpy(*pptr, ptr + 1, len + 1);
  this->line_ptr = NULL;
  return XcAIDF_COMMENT;
 }
 else if(c == '[')
 {
  this->line_ptr = ptr + 1;
  return XcAIDF_OPEN_MARK;
 }
 else if(c == ']')
 {
  this->line_ptr = ptr + 1;
  return XcAIDF_CLOSE_MARK;
 }
 else
 {
  unsigned char *start;

  start = ptr;
  while((*ptr > 32) && (*ptr != '[') && (*ptr != ']') && (*ptr != '/'))
   ptr++;
  len = ptr - start;

  if((*pptr = Xc_malloc("string", len + 1)) == NULL) return XcAIDF_ERROR;
  memcpy(*pptr, start, len);
  (*pptr)[len] = 0;
  *plen = len;
  this->line_ptr = ptr;

  /*Xc_TRACE(("word: `%s'", *pptr));*/
  return(name_flag? XcAIDF_NAME : XcAIDF_WORD);
 }
}


/* ----------------------------------------------------------------- ** 
** readInteger - read an integer from the file                       ** 
** ----------------------------------------------------------------- */
static boolean readInteger(this, integerp)
c_AIDocFile *this;
long *integerp;
{
 long tmp;
 unsigned char *ptr;
 long len;

 Xc_HISTORY(("readInteger(`%s')", GetBasename(this->filename, FALSE)));

 tmp = 0;
 if(!readWord(this, &ptr, &len)) 
  return FALSE;
 if(len >= 0)
 {
  tmp = atol(ptr);
  Xc_free(ptr);
 }
 *integerp = tmp;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** readReal - read a real from the file                              ** 
** ----------------------------------------------------------------- */
static boolean readReal(this, realp)
c_AIDocFile *this;
real *realp;
{
 double tmp;
 unsigned char *ptr;
 long len;

 Xc_HISTORY(("readReal(`%s')", GetBasename(this->filename, FALSE)));

 tmp = 0.0; 
 if(!readWord(this, &ptr, &len))
  return FALSE;
 if(len >= 0)
 {
  tmp = atof(ptr);
  if(errno == ERANGE)
   Xc_WARNING(("real %s out of range", ptr));
  Xc_free(ptr);
 }
 *realp = (real)tmp;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** readBoolean - Read an boolean from the file                       ** 
** ----------------------------------------------------------------- */
static boolean readBoolean(this, flagp)
c_AIDocFile *this;
boolean *flagp;
{
 boolean tmp;
 unsigned char *ptr;
 long len;

 Xc_HISTORY(("readBoolean(`%s')", GetBasename(this->filename, FALSE)));

 tmp = FALSE;

 if(!readWord(this, &ptr, &len)) return FALSE;
 if(len >= 0)
 {
  tmp = (strcmp(ptr, "True") == 0)? TRUE : FALSE;
  Xc_free(ptr);
 }
 *flagp = tmp;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** skipSpace - Skip space in the file                                ** 
** ----------------------------------------------------------------- */
static void skipSpace (this)
c_AIDocFile *this;
{
 int	sortie;

 Xc_HISTORY(("skipSpace"));

 sortie = 0;
 while (!sortie)
 {
  if (this->buffer_index >= XcAIDF_BUFFER_SIZE)
  {
   read(this->handle, this->buffer_file, XcAIDF_BUFFER_SIZE);
   this->buffer_index = 0;
  }
  switch (this->buffer_file[this->buffer_index])
  {
  case 0x0a:
  case 0x20:
   sortie = 0;
   break;
  default:
   sortie = 1;
  }
  this->buffer_index++;
 }
 this->buffer_index--;
}


/* ----------------------------------------------------------------- ** 
** seekClassic - Seek in the file                                    ** 
** ----------------------------------------------------------------- */
static int seekClassic (this, mode, len)
c_AIDocFile	*this;
int		mode;
int		len;
{
 Xc_HISTORY(("seekClassic"));

 if (mode != SEEK_CUR) 
  lseek(this->handle, mode, len);
 else
  if (this->buffer_index < XcAIDF_BUFFER_SIZE)
  {
   this->buffer_index += len;
   if (this->buffer_index > XcAIDF_BUFFER_SIZE)
    lseek(this->handle, mode, this->buffer_index-XcAIDF_BUFFER_SIZE);
  }
  else
   lseek(this->handle, mode, len);	
 this->buffer_index = XcAIDF_BUFFER_SIZE;
 return len;
}


/* ----------------------------------------------------------------- ** 
** searchData - Search data in the file                              ** 
** ----------------------------------------------------------------- */
static boolean searchData (this, data)
c_AIDocFile	*this;
char		*data;
{
 int	found;
 int	len;

 Xc_HISTORY(("searchData(`%s', `%s')",
	     GetBasename(this->filename, FALSE), data));

 if (this->buffer_index < XcAIDF_BUFFER_SIZE)
 {
  if ((found = strfind(this->buffer_file, 
		       data, this->end_index-this->buffer_index)) != -1)
  {
   this->buffer_index = found;
   return TRUE;
  }
  this->buffer_index = XcAIDF_BUFFER_SIZE;
 }
 while (1)
 {
  len = read(this->handle, this->buffer_file, XcAIDF_BUFFER_SIZE);
  this->end_index = len;
  if ((found = strfind(this->buffer_file, 
		       data, len)) != -1)
  {
   this->buffer_index = found;
   return TRUE;
  }
  if (len != XcAIDF_BUFFER_SIZE)
   return FALSE;
 }
}


/* ----------------------------------------------------------------- ** 
** readLine - read line                                              ** 
** ----------------------------------------------------------------- */
static boolean readLine(this, skip_comment)
c_AIDocFile *this;
boolean skip_comment;
{
 boolean comment_flag = FALSE, first_flag;
 unsigned char *ptr, *code;

 /*Xc_TRACE(("readLine"));*/

 first_flag = TRUE;
 ptr = this->line;
 code = NULL;
 while(42)
 {
  register unsigned char c;
    
  if(this->buffer_index >= this->end_index)
  {
   this->end_index = read(this->handle,
			  this->buffer_file, XcAIDF_BUFFER_SIZE);
   if(this->end_index <= 0)
   {
    if(this->end_index < 0)
     Xc_SYSERROR((GetBasename(this->filename, FALSE)));
    return FALSE;
   }
   this->buffer_index = 0;
  }
    
  c = this->buffer_file[this->buffer_index++];
  if(first_flag)
  {
   if((c != 10) && (c != 13))
   {
    first_flag = FALSE;
    if((c == '%') && skip_comment)
     comment_flag = TRUE;
    else
    {
     comment_flag = FALSE;
     switch(c)
     {
     case '\\':
      code = ptr;
      break;
     case '(':
      *ptr++ = ((this->string_level++)?
		c : XcAIDF_STRING_START_CODE);
      break;
     case ')':
      *ptr++ = ((--this->string_level)?
		c : XcAIDF_STRING_END_CODE);
      break;
     default:
      *ptr++ = c;
      break;
     }
    }
   }
  }
  else
  {
   if((c == 10) || (c == 13))
   {
    if(comment_flag == FALSE)
    {
     break;
    }
    else
    {
     comment_flag = FALSE;
     first_flag = TRUE;
    }
   }
   else if(comment_flag == FALSE)
   {
    if(code == NULL)
     switch(c)
     {
     case '\\':
      code = ptr;
      break;
     case '(':
      *ptr++ = ((this->string_level++)?
		c : XcAIDF_STRING_START_CODE);
      break;
     case ')':
      *ptr++ = ((--this->string_level)?
		c : XcAIDF_STRING_END_CODE);
      break;
     default:
      *ptr++ = c;
      break;
     }
    else
     if((c >= '0') && (c <= '9'))
      *ptr++ = c;
     else if(code == ptr)
     {
      ptr = code;
      switch(c)
      {
      case 'n':
       *ptr++ = '\n';
       break;
      case 'f':
       *ptr++ = '\f';
       break;
      case 'r':
       *ptr++ = '\r';
       break;
      default:
       *ptr++ = c;
       break;
      }
      code = NULL;
     }
     else
     {
      int i;
	    
      *ptr = 0;
      Xc_TRACE(("sscanf(`%s' in `%s')", code, this->line));
      sscanf(code, "%o", &i);
      ptr = code;
      code = NULL;
      *ptr++ = (unsigned char)i;
	    
      switch(c)
      {
      case '\\':
       code = ptr;
       break;
      case '(':
       *ptr++ = ((this->string_level++)?
		 c : XcAIDF_STRING_START_CODE);
       break;
      case ')':
       *ptr++ = ((--this->string_level)?
		 c : XcAIDF_STRING_END_CODE);
       break;
      default:
       *ptr++ = c;
       break;
      }
     }
   }
  }
 }
 this->buffer_index--;
 *ptr = 0;

 Xc_ASSERT(ptr < (this->line + 256));
 this->line_ptr = this->line;
 /*Xc_TRACE(("done: `%s'", this->line));*/
 return TRUE;
}

