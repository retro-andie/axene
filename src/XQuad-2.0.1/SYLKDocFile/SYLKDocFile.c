/*
** SYLKDocFile.c for XQuad in SYLKDocFile/
** Methods for the SYLKDocFile class
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Tue Oct 10 17:30:04 1995 Antoine Buat
** Last update Tue Jan 18 16:17:21 2000 Emmanuel Paris
*/

#include "SYLKDocFileP.h"
#include "file.h"
#include <stdarg.h>
#include <math.h>

#ifdef __GNUC__
#include <stdlib.h>
#include <unistd.h>
#endif

/*--- Prototypes ---*/
static void *constructor();
static void destructor();
static void *copy();

static void create_attribs_box ___PROTO((c_SYLKDocFile *This, Widget Parent));
static void manage_attribs_box ___NPROTO((c_SYLKDocFile *This,
					  boolean doc_available));
static void destroy_attribs_box ___PROTO((c_SYLKDocFile *This));
static boolean test ___PROTO((c_SYLKDocFile *This));
static boolean create_new_sheet ___PROTO((c_SYLKDocFile *This));
static error load ___PROTO((c_SYLKDocFile *This, c_Tableau *sheet));

static char readWord ___PROTO((c_SYLKDocFile *this, unsigned char **pptr,
			       long *lenp));
static boolean readInteger ___PROTO((c_SYLKDocFile *doc, long *integer));
static boolean readReal ___PROTO((c_SYLKDocFile *doc, real *real));
static boolean readBoolean ___PROTO((c_SYLKDocFile *doc, boolean *flagp));
static boolean readString ___PROTO((c_SYLKDocFile *doc, 
				    unsigned char **stringp));
static void skipSpace ___PROTO((c_SYLKDocFile *doc));
static void skipLine ___PROTO((c_SYLKDocFile *doc));

sf_SYLKDocFile fc_SYLKDocFile = {
				 constructor,
				 destructor,
				 copy,

				 /*___Standard import method___*/
				 create_attribs_box,
				 manage_attribs_box,
				 destroy_attribs_box,
				 test,
				 create_new_sheet,
				 load,

				 /*___Reading___*/
				 readWord, 
				 readInteger, 
				 readReal,
				 readBoolean,
				 readString,
				 skipSpace
				};

#define XcDF_FILL_BUFFER(this) \
this->ptr = this->buffer; \
this->endptr = this->ptr + read(this->handle, this->buffer, XcDF_BUFFER_SIZE)


static char *name = "Symbolic Link Format xcalibur version 0.1";
static char *copyright = "Microsoft";
static char *id = "SYLK";
static char *extensions[] = { "sylk", "slk" };
static boolean	box_created = FALSE;

/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(param)
void *param;
{
 c_SYLKDocFile *p;

 Xc_HISTORY(("constructor(`SYLK')"));

 /*--- Allocation ---*/
 if((p = Xc_malloc("SYLKDocFile class", sizeof(c_SYLKDocFile))) == NULL)
  return NULL;
 p->f = &fc_SYLKDocFile;

 /*--- Set up info ---*/
 p->name = name;
 p->copyright = copyright;
 p->id = id;
 p->extension_count = 2;
 p->extensions = extensions;

 p->box_created = &box_created;
 return p;
}

/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_SYLKDocFile *this;
{
 Xc_HISTORY(("destructor(`SYLK')"));
 Xc_free(this);
}

/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this)
c_SYLKDocFile *this;
{
 Xc_HISTORY(("copy not implemented"));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** create attribs box                                                ** 
** ----------------------------------------------------------------- */
static void create_attribs_box(This, Parent)
c_SYLKDocFile	*This;
Widget		Parent;
{
}

/* ----------------------------------------------------------------- ** 
** manage attribs box                                                ** 
** ----------------------------------------------------------------- */
static void manage_attribs_box(This, doc_available)
c_SYLKDocFile *This;
boolean	doc_available;
{
}

static void destroy_attribs_box(This)
c_SYLKDocFile *This;
{
}

static boolean test(This)
c_SYLKDocFile *This;
{
 long		len;
 unsigned char	*ptr;

 /*--- first 2 bytes must be ID ---*/ 
 if (!(This->fildes = fopen(This->filename, "r")))
 {
  return FALSE;
 }
 readWord(This, &ptr, &len);
 Xc_TRACE(("read: %s", ptr));
 if (strcmp(ptr, "ID") == 0)
 {
  Xc_TRACE(("file is ok\n"));
  Xc_free(ptr);
  fclose(This->fildes);
  return TRUE;
 }
 else
 {
  Xc_TRACE(("this is not a SYLK file\n"));
  Xc_free(ptr);
  fclose(This->fildes);
  return FALSE;
 }
}

static boolean create_new_sheet(This)
c_SYLKDocFile *This;
{
 return TRUE;
}

static error load(This, sheet)
c_SYLKDocFile *This;
c_Tableau *sheet;
{
 long		len;
 unsigned char	*ptr;
 int		sortie=0;
 char		tmp_c;

 /*--- first 2 bytes must be ID ---*/ 
 This->fildes = fopen(This->filename, "r");
 readWord(This, &ptr, &len);
 Xc_TRACE(("read: %s", ptr));
 if (strcmp(ptr, "ID") == 0)
 {
  Xc_TRACE(("file is ok\n"));
  /*--- saute le nom du constructeur et eventuelle ;N;E ---*/
  skipLine(This); 
  Xc_free(ptr);
 }
 else
 {
  Xc_TRACE(("this is not a SYLK file\n"));
  Xc_free(ptr);
  return FALSE;
 }

 while (1)
 {
  readWord(This, &ptr, &len);
  if (strcmp(ptr, "P") == 0)
  {
   /* to store number format or font format */
   /* will be treated later */
   skipLine(This);
   Xc_free(ptr);
   continue;
  }
  if (strcmp(ptr, "O") == 0)
  {
   /* This is the config for the spreadsheet */
   skipLine(This);
   Xc_free(ptr);
   continue;
  }
  if (strcmp(ptr, "F") == 0)
  {
   Xc_free(ptr);
   sortie = 0;
   while (!sortie)
   {
    /* read char only */
    tmp_c = fgetc(This->fildes);
    switch(tmp_c)
    {
    case 0x0a:
    case 0x0d:
     sortie = 1;
     break;
    case 'C':
     /* select a column */
     break;
    case 'D':
     /* define default style number and position 
	and column width */
     /* warning all in the line apply on this default */
     skipLine(This);
     break;
    case 'F':
     /* define default style number and position */
     break;
    case 'M':
     /* define default row height */
     skipLine(This);
     break;
    case 'P':
     /* to use format number */
     /* will be treated later */
     skipLine(This);
     break;
    case 'R':
     /* select row */
     break;
    case 'S':
     /* define style */
     break;
    case 'W':
     /* define column width */
     /* warning all on this concern this dimension */
     skipLine(This);
     break;
    case 'X':
     /* position cursor X */
     break;
    case 'Y':
     /* position cursor Y */
     break;
    }
   }
   continue;
  }
  if (strcmp(ptr, "B") == 0)
  {
   /* define the bounding box */
   /* don't need */
   skipLine(This);
   Xc_free(ptr);
   continue;
  }
  if (strcmp(ptr, "C") == 0)
  {
   /* poke something in a cell */
   Xc_free(ptr);
   sortie = 0;
   while (!sortie)
   {
    /* read char only */
    tmp_c = fgetc(This->fildes);
    switch(tmp_c)
    {
    case 0x0a:
    case 0x0d:
     sortie = 1;
     break;
    case 'K':
     /* poke a number or a string*/
     break;
    case 'E':
     /* poke a formula */
     break;
    }
   }
   continue;
  }
 }
 fclose(This->fildes);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** Readword - Read a word from the file                              ** 
** ----------------------------------------------------------------- */
#define MAX_WORD_LEN 100
static char readWord(this, pptr, len)
c_SYLKDocFile	*this;
unsigned char	**pptr;
long		*len;
{
 char		sortie;
 char		tmp_ptr[MAX_WORD_LEN];
 int		tmp_len;

 tmp_len = 0;
 skipSpace(this);
 Xc_HISTORY(("readWord(`%s')", this->filename));
 sortie = 0;
 while (!sortie)
 {
  tmp_ptr[tmp_len] = sortie = fgetc(this->fildes);
  switch (sortie)
  {
  case EOF:
  case 0x0a:
  case 0x0d:
  case 0x20:
  case ';':
   break;
  default:
   sortie = 0;
  }
  tmp_len++;
 }
 if (tmp_len)
  *pptr = (unsigned char *)Xc_malloc("tmp SYLKDF ptr", tmp_len+1);
 tmp_ptr[tmp_len] = 0;
 strcpy(*pptr, tmp_ptr);
 return sortie;
}

/* ----------------------------------------------------------------- ** 
** readInteger - read an integer from the file                       ** 
** ----------------------------------------------------------------- */
static boolean readInteger(this, integerp)
c_SYLKDocFile *this;
long *integerp;
{
 long tmp;
 unsigned char *ptr;
 long len;

 Xc_HISTORY(("readInteger(`%s')", this->filename));

 tmp = 0;

 if(!readWord(this, &ptr, &len)) return FALSE;
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
c_SYLKDocFile *this;
real *realp;
{
 double tmp;
 unsigned char *ptr;
 long len;

 Xc_HISTORY(("readReal(`%s')", this->filename));

 tmp = 0.0;
      
 if(!readWord(this, &ptr, &len)) return FALSE;
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
c_SYLKDocFile *this;
boolean *flagp;
{
 boolean tmp;
 unsigned char *ptr;
 long len;

 Xc_HISTORY(("readBoolean(`%s')", this->filename));

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
** readString - Method to read a text without interactions           ** 
** ----------------------------------------------------------------- */
static boolean readString(doc, stringp)
c_SYLKDocFile *doc;
unsigned char **stringp;
{
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** skipSpace - Method to skip space defined by 0x0d or 0x0a or 0x20  ** 
** ----------------------------------------------------------------- */
static void skipSpace (this)
c_SYLKDocFile *this;
{
 int	sortie;

 sortie = 0;
 while (!sortie)
 {
  sortie = fgetc(this->fildes);
  switch (sortie)
  {
  case 0x0a:
  case 0x0d:
  case 0x20:
   sortie = 0;
   break;
  default:
   sortie = 1;
  }
 }
}

/* ----------------------------------------------------------------- ** 
** skipSpace - Method to skip line defined by 0x0d or 0x0a           ** 
** ----------------------------------------------------------------- */
static void skipLine (this)
c_SYLKDocFile *this;
{
 int	sortie;

 sortie = 0;
 while (!sortie)
 {
  sortie = fgetc(this->fildes);
  switch (sortie)
  {
  case 0x0a:
   if ((sortie = fgetc(this->fildes)) != 0x0d)
    ungetc(sortie, this->fildes);
   sortie = 1;
   break;
  case 0x0d:
   if ((sortie = fgetc(this->fildes)) != 0x0a)
    ungetc(sortie, this->fildes);
   sortie = 1;
   break;
  default:
   sortie = 0;
  }
 }
}
