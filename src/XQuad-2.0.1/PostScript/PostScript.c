/*
** PostScript.c for Xclamation, XQuad and XAllWrite in PostScript/
** Object and method for PostScript
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
** Started on  Fri Nov  4 14:20:24 1994 Antoine Buat
** Last update Thu Jun  3 15:24:11 1999 Emmanuel Paris
*/

/*#define NHISTORY*/
#define NTRACE

#define XcPS_INCLUDE_BUFFER 8096

#include "PostScriptP.h"
#include <stdarg.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include "file.h"
#include "version.h"


/*--- Prototypes ---*/
static void *constructor();
static void destructor();
static void *copy();
static boolean addFont ___PROTO((c_PostScript *This, c_VectorFont *font));
static boolean addColor ___PROTO((c_PostScript *this, c_Color *color));
static boolean putLine ___PROTO((c_PostScript *this, unsigned char *format, ...));
static boolean putLine2 ___PROTO((c_PostScript *this, int type,
				   unsigned char *format, ...));
static boolean deferredWrite ___PROTO((c_PostScript *this, int type));
static boolean endPage ___PROTO((c_PostScript *this));
static boolean beginPage ___PROTO((c_PostScript *this, unsigned int number));
static boolean putString ___PROTO((c_PostScript *this,
				   char_t *start, long len));
static boolean putString2 ___PROTO((c_PostScript *this, int type,
				    char_t *start, long len));
static boolean my_write ___PROTO((c_PostScript *this, void *ptr, long len));
static boolean includeFile ___PROTO((c_PostScript *this, char *filename));
static boolean pushState ___PROTO((c_PostScript *this));
static boolean popState ___PROTO((c_PostScript *this));

/*--- Arrays ---*/
static char *ai_header[] = 
{
 "%%BoundingBox: 0 0 0 0",
 "%%DocumentFonts: (atend)",
 "%%DocumentCustomColors: (atend)",
 "%%DocumentNeededResources: procset Adobe_packedarray 2.0 0",
 "%%+ procset Adobe_cmykcolor 1.1 0",
 "%%+ procset Adobe_cshow 1.1 0",
 "%%+ procset Adobe_customcolor 1.0 0",
 "%%+ procset Adobe_typography_AI3 1.0 1",
 "%%+ procset Adobe_IllustratorA_AI3 1.0 1",
 "%AI3_DocumentPreview: None",
 "%%EndComments",
 "%%BeginProlog",
 "%%IncludeResource: procset Adobe_packedarray 2.0 0",
 "Adobe_packedarray /initialize get exec",
 "%%IncludeResource: procset Adobe_cmykcolor 1.1 0",
 "%%IncludeResource: procset Adobe_cshow 1.1 0",
 "%%IncludeResource: procset Adobe_customcolor 1.0 0",
 "%%IncludeResource: procset Adobe_typography_AI3 1.0 1",
 "%%IncludeResource: procset Adobe_IllustratorA_AI3 1.0 1",
 NULL
};

static char *ps_header[] = 
{
 "%%DocumentFonts: (atend)",
 "%%DocumentCustomColors: (atend)",
 "%%BoundingBox: (atend)",
 "%%Pages: (atend)",
 "%%EndComments",
 "%%BeginProlog",
 NULL
};


/*--- Method ---*/
sf_PostScript fc_PostScript =
{
 constructor, destructor, copy,
 my_write, putLine, putString,
 addFont, addColor,
 beginPage, endPage,
 ps_beginImage, ps_endImage,
 includeFile, ps_includeResource,
 pushState, popState,
 putLine2, deferredWrite, putString2
};


/* ----------------------------------------------------------------- ** 
** constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(print_param, filename, handle)
print_param_t	*print_param;
char		*filename;
int		handle;
{
 char *procset_filename;
 c_PostScript 	*this;
 char *strdate;
 time_t gtime;
 char **ptr;

 Xc_HISTORY(("constructor(`%s', %d)", filename? filename:"<null>", handle));

 if((this = Xc_malloc("PostScript", sizeof(c_PostScript))) == NULL)
  return NULL;
 if((this->matrix = NEW(c_Matrix)()) == NULL)
 {
  Xc_free(this);
  return NULL;
 }      

 this->f = &fc_PostScript;

 if((this->filename = Xc_strdup("PostScript outfile", filename)) == NULL)
 {
  DELETE(c_Matrix)(this->matrix);
  Xc_free(this);
  return NULL;
 }

 this->error_flag = FALSE;
 this->close_flag = (handle != -1)? FALSE : TRUE;
 if(handle < 0) 
 {
  Xc_TRACE(("opening file"));
  if(!IncFileBackup(filename))
  {
   DELETE(c_Matrix)(this->matrix);
   Xc_free(this->filename);
   Xc_free(this);
   return NULL;
  }

  if((handle = open(filename, O_CREAT|O_WRONLY|O_TRUNC, 0666)) < 0)
  {
   Xc_SYSERROR((filename));
   DELETE(c_Matrix)(this->matrix);
   Xc_free(this->filename);
   Xc_free(this);
   return NULL;
  }
 }

 this->fd = handle;

 this->shadow_text = NULL;
 this->normal_text = NULL;
 this->shadow_graph = NULL;
 this->normal_graph = NULL;
 this->page_flag = this->segment_flag = FALSE;
 this->page_count = this->page_number = 0;
 this->doc_bbox.llx = this->doc_bbox.lly = 0;
 this->doc_bbox.urx = this->doc_bbox.ury = 0;
 this->font_list = NULL;
 this->color_list = NULL;
 this->item = NULL;
 this->ai_mode = ((this->print_param = print_param) == NULL)? TRUE : FALSE;
  
 procset_filename = NULL;
 Xc_TRACE(("Writing header"));
 while(42)
 {
  char creator[16];

#if 0
  /*--- Set non blocking ---*/
  if((val = fcntl(handle, F_GETFL, 0)) < 0)
  {
   Xc_SYSERROR(("fcntl F_GETFL"));
   break;
  }
  if(fcntl(handle, F_SETFL, val | O_NONBLOCK) < 0)
  {
   Xc_SYSERROR(("fcntl F_SETFL"));
   break;
  }
#endif

  gtime = time(NULL);
  strdate = ctime(&gtime);
  Xc_ASSERT(strlen(strdate) > 0);
  *(strdate + strlen(strdate) - 1) = 0; 

  sprintf(creator, "XC!amation %d.%2d", (int)MAJOR, (int)MINOR);

  /*--- Global header ---*/
  if(!F(this).putLine(this, "%!PS-Adobe-2.0") ||
     !F(this).putLine(this, "%%Creator: ($s)", creator) ||
     !F(this).putLine(this, "%%Title: ($s)", filename) ||
     !F(this).putLine(this, "%%CreationDate: ($s)", strdate)) break;

  /*--- Specific header ---*/
  ptr = this->ai_mode? ai_header : ps_header;
  while(*ptr != NULL) if(!F(this).putLine(this, *ptr++)) break;

  if(!this->ai_mode)
  {
   /*--- Copy procset ---*/
   procset_filename = TranslateFilename(XcPS_DIR,XcPS_PROCSET_FILENAME);
   if(procset_filename == NULL) break;
   if(F(this).includeFile(this, procset_filename) == FALSE) break;
   Xc_free(procset_filename);
   procset_filename = NULL;
  }

  if(!F(this).putLine(this, "%%EndProlog")) break;

  /*--- Setup ---*/
  if(!F(this).putLine(this, "%%BeginSetup")) break;

  if((this->ai_mode == FALSE) && (print_param->NbCopies > 1) &&
     !F(this).putLine(this, "/#copies $d def",
		      (long)print_param->NbCopies)) break;
      
  Xc_TRACE(("constructed"));
  return this;
 }

 Xc_TRACE(("Error during header"));
 if(procset_filename != NULL) Xc_free(procset_filename);
 DELETE(c_PostScript)(this);
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_PostScript *this;
{
 postscript_font_t *fptr;
 postscript_color_t *cptr;

 Xc_HISTORY(("destructor(%s)", this->error_flag? "Error" : "OK"));
 Xc_ASSERT(this->error_flag || (this->page_flag == FALSE));

 while(!this->error_flag)
 {
  if(!F(this).putLine(this, "%%Trailer") ||
     (this->ai_mode &&
      !F(this).putLine(this, "%%Pages: $d", (long)this->page_count)) ||
     !F(this).putLine(this, "%%BoundingBox: $d $d $d $d",
		      (long)ROUNDFLOAT(SCALE_TO_POINTS(this->doc_bbox.llx)),
		      (long)ROUNDFLOAT(SCALE_TO_POINTS(this->doc_bbox.lly)),
		      (long)ROUNDFLOAT(SCALE_TO_POINTS(this->doc_bbox.urx)),
		      (long)ROUNDFLOAT(SCALE_TO_POINTS(this->doc_bbox.ury))))
   break;
      
  if((fptr = this->font_list) != NULL)
  {
   if(!F(this).putLine(this, "%%DocumentFonts: $s",
		       fptr->font->font_name)) break;
   while((fptr = fptr->next) != NULL)
    if(!F(this).putLine(this, "%%+ $s", fptr->font->font_name)) break;
   if(this->error_flag) break;
  }
  else
   if(!F(this).putLine(this, "%%DocumentFonts:")) break;
      
  if((cptr = this->color_list) != NULL)
  {
   F(this).putLine(this, "%%DocumentCustomColors: ($s)",
		   cptr->color->name);
   while((cptr = cptr->next) != NULL)
    if(!F(this).putLine(this, "%%+ $s", cptr->color->name)) break;
   if(this->error_flag) break;

   for(cptr = this->color_list; cptr; cptr = cptr->next)
   {
    c_Color *color;

    color = cptr->color;
    if((color->mode != XcC_RGB) || (this->ai_mode != FALSE))
    {
     if(!F(this).putLine(this,
			 "%%CMYKCustomColor: $f $f $f $f ($s)",
			 XcC_DESCALE_COLOR(double, color->cmyk.c),
			 XcC_DESCALE_COLOR(double, color->cmyk.m),
			 XcC_DESCALE_COLOR(double, color->cmyk.y),
			 XcC_DESCALE_COLOR(double, color->cmyk.k),
			 color->name)) break;
    }
    else
     if(!F(this).putLine(this, "%%RGBCustomColor: $f $f $f ($s)",
			 XcC_DESCALE_COLOR(double, color->rgb.r),
			 XcC_DESCALE_COLOR(double, color->rgb.g),
			 XcC_DESCALE_COLOR(double, color->rgb.b),
			 color->name)) break;
   }
   if(this->error_flag) break;
  }
  else
   if(!F(this).putLine(this, "%%DocumentCustomColors:")) break;
      
  F(this).putLine(this, "%%EOF");
  break;
 }

 DELETE(c_Matrix)(this->matrix);
 if(this->close_flag == TRUE)
 {
  Xc_TRACE(("closing file"));
  close(this->fd);
  if(this->error_flag)
  {
   Xc_TRACE(("unlink file"));
   unlink(this->filename);
   DecFileBackup(this->filename);
  }
 }

 if(this->item != NULL) Xc_destroyItem(this->item);
 Xc_free(this->filename);
 Xc_free(this);
 Xc_TRACE(("destruction ok"));
}


/* ----------------------------------------------------------------- ** 
** copy method                                                       ** 
** ----------------------------------------------------------------- */
static void *copy(this)
c_PostScript *this;
{
 Xc_HISTORY(("copy: non implemeted"));
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** my_write - Internal write function                                ** 
** ----------------------------------------------------------------- */
static boolean my_write(this, ptr, len)
c_PostScript *this;
void *ptr;
long len;
{
 /*struct timeval timout;*/
 /*fd_set writeset;*/
 /*int i;*/

 Xc_TRACE(("write(`%s')", this->filename));

 /*FD_ZERO(&writeset);*/
 while(len)
 {
  long written;

#if 0
  /*--- Wait 60 seconds ---*/
  FD_SET(this->fd, &writeset);
  timout.tv_sec = 60;
  timout.tv_usec = 0;

  if((i = select(this->fd + 1, NULL, &writeset, NULL, &timout)) <= 0)
  {
   if(errno == EINTR) continue;
   this->error_flag = TRUE;
   if(i < 0)
    Xc_SYSERROR(("select"));
   else
    Xc_ERROR(("%s: Timout", this->filename));
   return FALSE;
  }
#endif

  if((written = write(this->fd, ptr, len)) <= 0)
  {
   if(errno == EINTR) continue;
   this->error_flag = TRUE;
   Xc_SYSERROR((this->filename));
   return FALSE;
  }
  Xc_TRACE(("written=%ld", written));

  ptr = (char *)ptr + written;
  len -= written;
 }

 Xc_TRACE(("write done"));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** putLine - Write a text line                                       ** 
** ----------------------------------------------------------------- */
static boolean putLine(c_PostScript *this, unsigned char *format, ...)
{
 unsigned char *output, c, buffer[512];
 va_list ap;
 long len;

 Xc_TRACE(("putLine(`%s')", format));

 if(this->error_flag) return FALSE;

 va_start(ap, format);
 for(output = buffer; (c = *format++) != '\0'; )
  if(c == '$')
   switch(c = *format++)
   {
   case '$':
    *output++ = c;
    break;
	  
    /*--- String ---*/
   case 's':
    {
     char *string;

     string = va_arg(ap, char *);
     len = strlen(string);
     if((output + len) > (buffer + 510))
     {
      Xc_ERROR(("Line too long: ignored"));
      this->error_flag = TRUE;
      return FALSE;
     }
     memcpy(output, string, len);
     output += len;
     break;
    }

    /*--- Integer ---*/
   case 'd':
    {
     long integer;

     integer = va_arg(ap, long);
     sprintf(output, "%ld", integer);
     output += strlen(output);
     break;
    }

    /*--- Real ---*/
   case 'f':
    {
     double number;

     number = va_arg(ap, double);
     sprintf(output, "%g", number);
     output += strlen(output);
     break;
    }

    /*--- Coord_t ---*/
   case 'c':
    {
     coord_t coord;

     coord = va_arg(ap, coord_t);
     sprintf(output, "%g", (double)SCALE_TO_POINTS(coord));
     output += strlen(output);
     break;
    }

    /*--- Point ---*/
   case 'p':
    {
     vector_t a, b;

     a.dx = va_arg(ap, coord_t);
     a.dy = va_arg(ap, coord_t);
     b.dx = F(this->matrix).convertX(this->matrix, a.dx, a.dy);
     b.dy = F(this->matrix).convertY(this->matrix, a.dx, a.dy);
     sprintf(output, "%g %g", 
	     (double)SCALE_TO_POINTS(b.dx),
	     (double)SCALE_TO_POINTS(b.dy));
     output += strlen(output);
     break;
    }

    /*--- Vector ---*/
   case 'v':
    {
     vector_t a, b;

     a.dx = va_arg(ap, coord_t);
     a.dy = va_arg(ap, coord_t);
     b.dx = F(this->matrix).convertDx(this->matrix, a.dx, a.dy);
     b.dy = F(this->matrix).convertDy(this->matrix, a.dx, a.dy);
     sprintf(output, "%g %g", 
	     (double)SCALE_TO_POINTS(b.dx),
	     (double)SCALE_TO_POINTS(b.dy));
     output += strlen(output);
     break;
    }

    /*--- Matrix ---*/
   case 'm':
    {
     matrix_t *m;

     m = va_arg(ap, matrix_t *);
     sprintf(output, this->ai_mode?
	     "%f %f %f %f %f %f" : "%g %g %g %g %g %g",
	     (double)m->M[0][0], (double)m->M[0][1],
	     (double)m->M[1][0], (double)m->M[1][1],
	     (double)SCALE_TO_POINTS(m->v.dx),
	     (double)SCALE_TO_POINTS(m->v.dy));
     output += strlen(output);
     break;
    }

    /*--- Rectangle (from bbox) ---*/
   case 'r':
    {
     bbox_t *bbox;
     vector_t a, b;

     bbox = va_arg(ap, bbox_t *);
     Xc_ASSERT(this->ai_mode == FALSE);

     a.dx = F(this->matrix).convertX(this->matrix,
				     bbox->llx, bbox->lly);
     a.dy = F(this->matrix).convertY(this->matrix,
				     bbox->llx, bbox->lly);
     b.dx = F(this->matrix).convertDx(this->matrix,
				      bbox->urx - bbox->llx + 1,
				      bbox->ury - bbox->lly + 1);
     b.dy = F(this->matrix).convertDy(this->matrix,
				      bbox->urx - bbox->llx + 1,
				      bbox->ury - bbox->lly + 1);
     sprintf(output, "%g %g %g %g", 
	     (double)SCALE_TO_POINTS(a.dx),
	     (double)SCALE_TO_POINTS(a.dy),
	     (double)SCALE_TO_POINTS(b.dx),
	     (double)SCALE_TO_POINTS(b.dy));
     output += strlen(output);
     break;
    }

    /*--- Array ---*/
   case 'a':
    {
     coord_t *array;
     int count;

     count = va_arg(ap, int);
     array = va_arg(ap, coord_t *);
     *output++ = '[';
     while(count--)
     {
      sprintf(output, "%g", (double)SCALE_TO_POINTS(*array++));
      output += strlen(output);
      if(count) *output++ = 32;
     }
     *output++ = ']';
     break;
    }

   default:
    *output++ = '$';
    *output++ = c;
    break;
   }
  else
   *output++ = ((c > 32) && (c < 128))? c : 32;
  
 if((len = (output - buffer + 1)) >= 256) Xc_WARNING(("Line too long!"));

#ifndef NTRACE
 *output = 0;
 Xc_TRACE(("output: `%s'", buffer));
#endif  

 *output = '\n';

 return my_write(this, buffer, len);
}

/* ----------------------------------------------------------------- ** 
** putLine2 - Deferred Write typed text line                         ** 
** ----------------------------------------------------------------- */
static boolean putLine2(c_PostScript *this, int type, unsigned char *format, ...)
{
 unsigned char	*output, c, buffer[512];
 va_list	ap;
 long		len;
 char		**pbuffer;
 long		*plen;

 Xc_TRACE(("putLine(`%s')", format));

 if(this->error_flag) return FALSE;

 va_start(ap, format);
 for(output = buffer; (c = *format++) != '\0'; )
  if(c == '$')
   switch(c = *format++)
   {
   case '$':
    *output++ = c;
    break;
	  
    /*--- String ---*/
   case 's':
    {
     char *string;

     string = va_arg(ap, char *);
     len = strlen(string);
     if((output + len) > (buffer + 510))
     {
      Xc_ERROR(("Line too long: ignored"));
      this->error_flag = TRUE;
      return FALSE;
     }
     memcpy(output, string, len);
     output += len;
     break;
    }

    /*--- Integer ---*/
   case 'd':
    {
     long integer;

     integer = va_arg(ap, long);
     sprintf(output, "%ld", integer);
     output += strlen(output);
     break;
    }

    /*--- Real ---*/
   case 'f':
    {
     double number;

     number = va_arg(ap, double);
     sprintf(output, "%g", number);
     output += strlen(output);
     break;
    }

    /*--- Coord_t ---*/
   case 'c':
    {
     coord_t coord;

     coord = va_arg(ap, coord_t);
     sprintf(output, "%g", (double)SCALE_TO_POINTS(coord));
     output += strlen(output);
     break;
    }

    /*--- Point ---*/
   case 'p':
    {
     vector_t a, b;

     a.dx = va_arg(ap, coord_t);
     a.dy = va_arg(ap, coord_t);
     b.dx = F(this->matrix).convertX(this->matrix, a.dx, a.dy);
     b.dy = F(this->matrix).convertY(this->matrix, a.dx, a.dy);
     sprintf(output, "%g %g", 
	     (double)SCALE_TO_POINTS(b.dx),
	     (double)SCALE_TO_POINTS(b.dy));
     output += strlen(output);
     break;
    }

    /*--- Vector ---*/
   case 'v':
    {
     vector_t a, b;

     a.dx = va_arg(ap, coord_t);
     a.dy = va_arg(ap, coord_t);
     b.dx = F(this->matrix).convertDx(this->matrix, a.dx, a.dy);
     b.dy = F(this->matrix).convertDy(this->matrix, a.dx, a.dy);
     sprintf(output, "%g %g", 
	     (double)SCALE_TO_POINTS(b.dx),
	     (double)SCALE_TO_POINTS(b.dy));
     output += strlen(output);
     break;
    }

    /*--- Matrix ---*/
   case 'm':
    {
     matrix_t *m;

     m = va_arg(ap, matrix_t *);
     sprintf(output, this->ai_mode?
	     "%f %f %f %f %f %f" : "%g %g %g %g %g %g",
	     (double)m->M[0][0], (double)m->M[0][1],
	     (double)m->M[1][0], (double)m->M[1][1],
	     (double)SCALE_TO_POINTS(m->v.dx),
	     (double)SCALE_TO_POINTS(m->v.dy));
     output += strlen(output);
     break;
    }

    /*--- Rectangle (from bbox) ---*/
   case 'r':
    {
     bbox_t *bbox;
     vector_t a, b;

     bbox = va_arg(ap, bbox_t *);
     Xc_ASSERT(this->ai_mode == FALSE);

     a.dx = F(this->matrix).convertX(this->matrix,
				     bbox->llx, bbox->lly);
     a.dy = F(this->matrix).convertY(this->matrix,
				     bbox->llx, bbox->lly);
     b.dx = F(this->matrix).convertDx(this->matrix,
				      bbox->urx - bbox->llx + 1,
				      bbox->ury - bbox->lly + 1);
     b.dy = F(this->matrix).convertDy(this->matrix,
				      bbox->urx - bbox->llx + 1,
				      bbox->ury - bbox->lly + 1);
     sprintf(output, "%g %g %g %g", 
	     (double)SCALE_TO_POINTS(a.dx),
	     (double)SCALE_TO_POINTS(a.dy),
	     (double)SCALE_TO_POINTS(b.dx),
	     (double)SCALE_TO_POINTS(b.dy));
     output += strlen(output);
     break;
    }

    /*--- Array ---*/
   case 'a':
    {
     coord_t *array;
     int count;

     count = va_arg(ap, int);
     array = va_arg(ap, coord_t *);
     *output++ = '[';
     while(count--)
     {
      sprintf(output, "%g", (double)SCALE_TO_POINTS(*array++));
      output += strlen(output);
      if(count) *output++ = 32;
     }
     *output++ = ']';
     break;
    }

   default:
    *output++ = '$';
    *output++ = c;
    break;
   }
  else
   *output++ = ((c > 32) && (c < 128))? c : 32;
  
 if((len = (output - buffer + 1)) >= 256) Xc_WARNING(("Line too long!"));

#ifndef NTRACE
 *output = 0;
 Xc_TRACE(("output: `%s'", buffer));
#endif  

 *output = '\n';
 *(output+1) = 0;

 switch (type)
 {
 case SHADOW_TEXT:
  pbuffer = &(this->shadow_text);
  plen = &(this->shadow_text_len);
  break;
 case SHADOW_GRAPH:
  pbuffer = &(this->shadow_graph);
  plen = &(this->shadow_graph_len);
  break;
 case NORMAL_TEXT:
  pbuffer = &(this->normal_text);
  plen = &(this->normal_text_len);
  break;
 case NORMAL_GRAPH:
 default:
  pbuffer = &(this->normal_graph);
  plen = &(this->normal_graph_len);
 }
 if (!*pbuffer)
 {
  *pbuffer = Xc_malloc("defer", 512);
  **pbuffer = 0;
  *plen = 0;
 }
 if ((512 - (*plen % 512)) <= len)
 {
  *pbuffer = Xc_realloc("re-defer", *pbuffer, ((*plen / 512)+2)*512);
 }
 strcat(*pbuffer, buffer);
 *plen += len;
 return TRUE;
 /*
   return my_write(this, buffer, len);
   */
}


/* ----------------------------------------------------------------- ** 
** deferredWrite - Deferred Write                                    ** 
** ----------------------------------------------------------------- */
static boolean deferredWrite(this, type)
c_PostScript	*this;
int		type;
{
 char		**pbuffer;
 long		*plen;
 boolean	ret;

 switch (type)
 {
 case SHADOW_TEXT:
  pbuffer = &(this->shadow_text);
  plen = &(this->shadow_text_len);
  break;
 case SHADOW_GRAPH:
  pbuffer = &(this->shadow_graph);
  plen = &(this->shadow_graph_len);
  break;
 case NORMAL_TEXT:
  pbuffer = &(this->normal_text);
  plen = &(this->normal_text_len);
  break;
 case NORMAL_GRAPH:
 default:
  pbuffer = &(this->normal_graph);
  plen = &(this->normal_graph_len);
 }
 if (*pbuffer)
 {
  ret = my_write(this, *pbuffer, *plen);
  Xc_free(*pbuffer);
  *pbuffer = NULL;
  *plen = 0;
 }
 else
  ret = TRUE;
 return ret;
}

/* ----------------------------------------------------------------- ** 
** addColor - Add a color to the color list                          ** 
** ----------------------------------------------------------------- */
static boolean addColor(this, color)
c_PostScript *this;
c_Color *color;
{
 postscript_color_t *color_ptr;

 Xc_HISTORY(("addColor(`%s')", color->name));

 if(this->item == NULL)
 {
  this->item = Xc_createItem("Color list", 10, sizeof(postscript_color_t));
  if(this->item == NULL)
  {
   this->error_flag = TRUE;
   return FALSE;
  }
 }
  
 color_ptr = this->color_list;
 for( ; color_ptr != NULL; color_ptr = color_ptr->next)
  if(color == color_ptr->color)
  {
   color_ptr->page_flag = TRUE;
   return TRUE;
  }

 if((color_ptr = Xc_new(this->item)) == NULL)
 {
  this->error_flag = TRUE;
  return FALSE;
 }
 color_ptr->color = color;
 color_ptr->next = this->color_list;
 color_ptr->page_flag = TRUE;
 this->color_list = color_ptr;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** addFont - Add a font to the font list                             ** 
** ----------------------------------------------------------------- */
static boolean addFont(this, font)
c_PostScript *this;
c_VectorFont *font;
{
 postscript_font_t *font_ptr;

 Xc_HISTORY(("addFont(`%s')", font->font_name));

 if(this->item == NULL)
 {
  this->item = Xc_createItem("Font list", 10, sizeof(postscript_font_t));
  if(this->item == NULL)
  {
   this->error_flag = TRUE;
   return FALSE;
  }
 }
  
 for(font_ptr = this->font_list; font_ptr != NULL; font_ptr = font_ptr->next)
  if(font == font_ptr->font)
  {
   font_ptr->page_flag = TRUE;
   return TRUE;
  }

 if((font_ptr = Xc_new(this->item)) == NULL)
 {
  this->error_flag = TRUE;
  return FALSE;
 }
 font_ptr->font = font;
 font_ptr->next = this->font_list;
 font_ptr->page_flag = TRUE;
 this->font_list = font_ptr;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** beginPage - Start a new page in the document                      ** 
** ----------------------------------------------------------------- */
static boolean beginPage(this, number)
c_PostScript *this;
unsigned int number;
{
 int	i;
 Xc_HISTORY(("beginPage(%d)", number));
 Xc_ASSERT(this->page_flag == FALSE);

 if((this->page_number == 0) && !F(this).putLine(this, "%%EndSetup"))
  return FALSE;

 this->page_flag = TRUE;
 this->page_number = number;
 this->page_bbox.llx = this->page_bbox.lly = 0;
 this->page_bbox.urx = this->page_bbox.ury = 0;

 /*--- Init state ---*/
 this->state.fill_color = this->state.stroke_color = NULL;
 this->state.line_width = -COORD_ONE;
 this->state.line_cap = this->state.line_join = this->state.flatness = -1;
 this->state.dash_phase = this->state.dash_len = -1;
 this->state.font = NULL;
 this->state.point_size = -COORD_ONE;
 this->state.leading = COORD_ZERO;
 this->state.render_mode = -1;
 this->state.text_rise = COORD_ZERO;
 this->state.word_space = this->state.char_space = COORD_ZERO;
 this->state.horizontal_scale = SCALE_ONE;
 this->state.text_flag = this->state.clip_flag = FALSE;
 this->state.next = NULL;

 for (i=0; i<4; i++)
 {
  this->state2[i].fill_color = this->state2[i].stroke_color = NULL;
  this->state2[i].line_width = -COORD_ONE;
  this->state2[i].line_cap = this->state2[i].line_join = 
   this->state2[i].flatness = -1;
  this->state2[i].dash_phase = this->state2[i].dash_len = -1;
  this->state2[i].font = NULL;
  this->state2[i].point_size = -COORD_ONE;
  this->state2[i].leading = COORD_ZERO;
  this->state2[i].render_mode = -1;
  this->state2[i].text_rise = COORD_ZERO;
  this->state2[i].word_space = this->state2[i].char_space = COORD_ZERO;
  this->state2[i].horizontal_scale = SCALE_ONE;
  this->state2[i].text_flag = this->state2[i].clip_flag = FALSE;
  this->state2[i].next = NULL;
 }

 /*--- Save current matrix ---*/
 this->page_matrix = F(this->matrix).get(this->matrix, 0);

 return(this->ai_mode ||
	(F(this).putLine(this, "%%Page: $d $d", (long)number, (long)number) &&
	 F(this).putLine(this, "%%PageFonts: (atend)") &&
	 F(this).putLine(this, "%%PageCustomColors: (atend)") &&
	 F(this).putLine(this, "%%PageBoundingBox: (atend)") &&
	 F(this).putLine(this, "save")));
}


/* ----------------------------------------------------------------- ** 
** endPage - End the current page                                    ** 
** ----------------------------------------------------------------- */
static boolean endPage(this)
c_PostScript *this;
{
 postscript_font_t *font_ptr;
 postscript_color_t *color_ptr;

 Xc_HISTORY(("endPage(%d)", this->page_number));
 Xc_ASSERT(this->page_flag == TRUE);
 Xc_ASSERT(this->state.next == NULL);

 /*--- Test matrix page ---*/
 /*Xc_ASSERT(this->page_matrix == F(this->matrix).get(this->matrix, 0));*/

 this->page_flag = FALSE;
 this->page_count++;

 if(!this->ai_mode)
 {
  if(!F(this).putLine(this, "restore showpage") ||
     !F(this).putLine(this, "%%PageTrailer")) return FALSE;
  
  if((font_ptr = this->font_list) != NULL)
  {
   while((font_ptr != NULL) && (font_ptr->page_flag == FALSE))
    font_ptr = font_ptr->next;
   if(font_ptr != NULL)
   {
    font_ptr->page_flag = FALSE;
    if(!F(this).putLine(this, "%%PageFonts: $s", 
			font_ptr->font->font_name)) return FALSE;
    while((font_ptr = font_ptr->next) != NULL)
     if(font_ptr->page_flag != FALSE)
     {
      if(!F(this).putLine(this, "%%+ $s",
			  font_ptr->font->font_name))
       return FALSE;
      font_ptr->page_flag = FALSE;
     }
   }
  }
  else
   if(!F(this).putLine(this, "%%PageFonts:")) return FALSE;
      
  if((color_ptr = this->color_list) != NULL)
  {
   while((color_ptr != NULL) && (color_ptr->page_flag == FALSE))
    color_ptr = color_ptr->next;
   if(color_ptr != NULL)
   {
    if(!F(this).putLine(this, "%%PageCustomColors: ($s)",
			color_ptr->color->name)) return FALSE;
	      
    while((color_ptr = color_ptr->next) != NULL)
     if(!F(this).putLine(this, "%%+ $s", color_ptr->color->name))
      return FALSE;
	      
    color_ptr = this->color_list;
    for( ; color_ptr != NULL; color_ptr = color_ptr->next)
     if(color_ptr->page_flag != FALSE)
     {
      c_Color *color;
		    
      color_ptr->page_flag = FALSE;
      color = color_ptr->color;
      if((color->mode != XcC_RGB) || (this->ai_mode != FALSE))
      {
       if(!F(this).putLine(this,
			   "%%CMYKCustomColor: $f $f $f $f ($s)",
			   XcC_DESCALE_COLOR(double,
					     color->cmyk.c),
			   XcC_DESCALE_COLOR(double,
					     color->cmyk.m),
			   XcC_DESCALE_COLOR(double,
					     color->cmyk.y),
			   XcC_DESCALE_COLOR(double,
					     color->cmyk.k),
			   color->name)) return FALSE;
      }
      else
       if(!F(this).putLine(this, "%%RGBCustomColor: $f $f $f ($s)",
			   XcC_DESCALE_COLOR(double, color->rgb.r),
			   XcC_DESCALE_COLOR(double, color->rgb.g),
			   XcC_DESCALE_COLOR(double, color->rgb.b),
			   color->name)) return FALSE;
     }
   }
  }
  else
   if(!F(this).putLine(this, "%%PageFonts:")) return FALSE;
      
  if(!F(this).putLine(this, "%%PageBoundingBox: $d $d $d $d",
		      (long)ROUNDFLOAT(SCALE_TO_POINTS(this->page_bbox.llx)),
		      (long)ROUNDFLOAT(SCALE_TO_POINTS(this->page_bbox.lly)),
		      (long)ROUNDFLOAT(SCALE_TO_POINTS(this->page_bbox.urx)),
		      (long)ROUNDFLOAT(SCALE_TO_POINTS(this->page_bbox.ury))))
   return FALSE;
 }
  

 /*--- Update doc bounding box ---*/
 if(this->page_bbox.llx < this->doc_bbox.llx)
  this->doc_bbox.llx = this->page_bbox.llx;
 if(this->page_bbox.lly < this->doc_bbox.lly)
  this->doc_bbox.lly = this->page_bbox.lly;
 if(this->page_bbox.urx > this->doc_bbox.urx)
  this->doc_bbox.urx = this->page_bbox.urx;
 if(this->page_bbox.ury > this->doc_bbox.ury)
  this->doc_bbox.ury = this->page_bbox.ury;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** putString - Output a PostScript string                            ** 
** ----------------------------------------------------------------- */
static boolean putString(this, start, len)
c_PostScript *this;
char_t *start;
long len;
{
 unsigned char buffer[270], *ptr;

 Xc_HISTORY(("putString"));
 if(this->error_flag) return FALSE;

 ptr = buffer;
 *ptr++ = '(';
 while(len--)
 {
  char_t c;

  c = *start++;
  if((c >= 32) && (c <= 127))
  {
   if((c == '(') || (c == ')') || (c == '\\'))
    *ptr++ = '\\';
   *ptr++ = (unsigned char)c;
  }
  else
  {
   *ptr++ = '\\';

   if(c == 0)
    *ptr++ = '0';
   else if(c == '\r')
    *ptr++ = 'r';
   else if(c == '\n')
    *ptr++ = 'n';
   else
   {
    unsigned char tmp[4], *tmp_ptr;
	     
    tmp_ptr = tmp + sizeof(tmp) - 1;
    *tmp_ptr = 0;
    do {
	*(--tmp_ptr) = (c & 7) + '0';
	c >>= 3;
       } while(c != 0);
    while((*ptr++ = *tmp_ptr++) != 0);
    ptr--;
   }
  }
  if (ptr > buffer + 260)
  {
   if (!my_write(this, buffer, (long)(ptr - buffer)))
    return FALSE;
   ptr = buffer;
  }
 }
 *ptr++ = ')';

 len = ptr - buffer;
 return my_write(this, buffer, len);
}

/* ----------------------------------------------------------------- ** 
** putString - Output a PostScript string                            ** 
** ----------------------------------------------------------------- */
static boolean putString2(this, type, start, len)
c_PostScript *this;
int	type;
char_t *start;
long len;
{
 unsigned char buffer[270], *ptr;
 char		**pbuffer;
 long		*plen;

 Xc_HISTORY(("putString"));
 if(this->error_flag) return FALSE;

 ptr = buffer;
 *ptr++ = '(';
 while(len--)
 {
  char_t c;

  c = *start++;
  if((c >= 32) && (c <= 127))
  {
   if((c == '(') || (c == ')') || (c == '\\'))
    *ptr++ = '\\';
   *ptr++ = (unsigned char)c;
  }
  else
  {
   *ptr++ = '\\';

   if(c == 0)
    *ptr++ = '0';
   else if(c == '\r')
    *ptr++ = 'r';
   else if(c == '\n')
    *ptr++ = 'n';
   else
   {
    unsigned char tmp[4], *tmp_ptr;
	     
    tmp_ptr = tmp + sizeof(tmp) - 1;
    *tmp_ptr = 0;
    do {
	*(--tmp_ptr) = (c & 7) + '0';
	c >>= 3;
       } while(c != 0);
    while((*ptr++ = *tmp_ptr++) != 0);
    ptr--;
   }
  }
  if (ptr > buffer + 260)
  {
   if (!my_write(this, buffer, (long)(ptr - buffer)))
    return FALSE;
   ptr = buffer;
  }
 }
 *ptr++ = ')';
 *ptr = 0;

 len = ptr - buffer;
 switch (type)
 {
 case SHADOW_TEXT:
  pbuffer = &(this->shadow_text);
  plen = &(this->shadow_text_len);
  break;
 case SHADOW_GRAPH:
  pbuffer = &(this->shadow_graph);
  plen = &(this->shadow_graph_len);
  break;
 case NORMAL_TEXT:
  pbuffer = &(this->normal_text);
  plen = &(this->normal_text_len);
  break;
 case NORMAL_GRAPH:
 default:
  pbuffer = &(this->normal_graph);
  plen = &(this->normal_graph_len);
 }
 if (!*pbuffer)
 {
  *pbuffer = Xc_malloc("defer", 512);
  **pbuffer = 0;
  *plen = 0;
 }
 if ((512 - (*plen % 512)) <= len)
 {
  *pbuffer = Xc_realloc("re-defer", *pbuffer, ((*plen / 512)+2)*512);
 }
 strcat(*pbuffer, buffer);
 *plen += len;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** includeFile - Include a file                                      ** 
** ----------------------------------------------------------------- */
static boolean includeFile(this, filename)
c_PostScript *this;
char *filename;
{
 char *buffer;
 int handle;

 Xc_HISTORY(("includeFile(`%s')", filename));

 /*--- Open file ---*/
 if((handle = open(filename, O_RDONLY)) < 0)
 {
  Xc_SYSERROR((filename));
  this->error_flag = TRUE;
  return FALSE;
 }

 /*--- Alloc buffer ---*/
 if((buffer = Xc_malloc("buffer", XcPS_INCLUDE_BUFFER)) == NULL)
 {
  close(handle);
  this->error_flag = TRUE;
  return FALSE;
 }

 /*--- Copy loop ---*/
 while(42)
 {
  char *ptr;
  long len;

  /*--- Read ---*/
  if((len = read(handle, buffer, XcPS_INCLUDE_BUFFER)) == 0) break;
  if(len < 0)
  {
   Xc_SYSERROR((filename));
   close(handle);
   Xc_free(buffer);
   this->error_flag = TRUE;
   return FALSE;
  }

  /*--- Write ---*/
  for(ptr = buffer; len > 0; )
  {
   long written;

   if((written = write(this->fd, ptr, len)) <= 0)
   {
    if(written < 0) Xc_SYSERROR((filename));
    close(handle);
    Xc_free(buffer);
    this->error_flag = TRUE;
    return FALSE;
   }
   ptr += written;
   len -= written;
  }
 }
	  
 close(handle);
 Xc_free(buffer);

 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** pushState - Push current graphic state                            ** 
** ----------------------------------------------------------------- */
static boolean pushState(this)
c_PostScript *this;
{
 postscript_state_t *ptr;

 Xc_HISTORY(("pushState"));

 if((ptr = Xc_malloc("Graphic state", sizeof(postscript_state_t))) == NULL)
  return FALSE;
  
 memcpy(ptr, &this->state, sizeof(postscript_state_t));
 this->state.next = ptr;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** popState - Restore last pushed state                              ** 
** ----------------------------------------------------------------- */
static boolean popState(this)
c_PostScript *this;
{
 postscript_state_t *ptr;
 c_Color *stroke_color;
 c_Color *fill_color;
 scale_t horizontal_scale;
  
 Xc_HISTORY(("popState"));
 Xc_ASSERT(this->state.next != NULL);

 ptr = this->state.next;
 stroke_color = this->state.stroke_color;
 fill_color = this->state.fill_color;
 horizontal_scale = this->state.horizontal_scale;
 memcpy(&this->state, ptr, sizeof(postscript_state_t));
 this->state.stroke_color = stroke_color;
 this->state.fill_color = fill_color;
 this->state.horizontal_scale = horizontal_scale;
 Xc_free(ptr);

 Xc_TRACE(("LastState: %s", this->state.next? "No" : "Yes"));
 return TRUE;
}
