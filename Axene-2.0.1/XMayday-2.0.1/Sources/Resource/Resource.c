/*
** Resource.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Resource/
** Methods for the ressource class
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
** Started on  Thu Dec 15 18:42:57 1994 Stéphane Boisson
** Last update Fri Jan 28 14:48:24 2000 Emmanuel Paris
*/

#ifndef NTRACE
#define NTRACE
#endif

#include "Resource.h"

#ifdef Xc_XQUAD
#define _Resource_TABLEUR_PRIVATE_
#include "Resource_TABLEUR.h"
#endif

#ifdef Xc_XCLAMATION
#define _Resource_PAO_PRIVATE_
#include "Resource_PAO.h"
#endif

#ifdef Xc_XALLWRITE
#define _Resource_WORD_PRIVATE_
#include "Resource_WORD.h"
#endif

#ifdef Xc_XMAYDAY
#define _Resource_HELP_PRIVATE_
#include "Resource_HELP.h"
#endif

#ifdef Xc_XINSTALL
#define _Resource_INSTALL_PRIVATE_
#include "Resource_INSTALL.h"
#endif

#ifdef Xc_AXENEOFFICE
#define _Resource_OFFICE_PRIVATE_
#include "Resource_OFFICE.h"
#endif

static void *constructor();
static void destructor();
static void *copy();
static char *getString ___PROTO((c_Resource *this, unsigned int number));
static XmString getXmString ___PROTO((c_Resource *this, unsigned int number));
static char *getFontName ___PROTO((c_Resource *this, unsigned int number));
static Font getFont ___PROTO((c_Resource *this, unsigned int number));
static XFontStruct *getXFontStruct ___PROTO((c_Resource *this, 
					     unsigned int number));
static int getInt ___PROTO((c_Resource *this, unsigned int number));
static boolean getBoolean ___PROTO((c_Resource *this, unsigned int number));
static void changeString ___PROTO((c_Resource *this, unsigned int number,
				   char *string));

sf_Resource fc_Resource = 
{
 constructor, destructor, copy,
 getString, getXmString,
 getFontName, getFont, getXFontStruct,
 getInt, getBoolean, changeString
};

/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(widget)
Widget widget;
{
 c_Resource *this;
 int	     i;
  
 Xc_HISTORY(("constructor"));
 Xc_ASSERT(XtNumber(resources) == (XcR_STRING_COUNT + XcR_FONT_COUNT + XcR_INT_COUNT));

 if((this = Xc_malloc("Resources", sizeof(c_Resource))) == NULL) return NULL;
 memset(this, 0, sizeof(c_Resource));
 this->f = &fc_Resource;
 XtGetApplicationResources(widget, this, resources, XtNumber(resources),
			   NULL, 0);
  
 this->display = XtDisplay(widget);
 for(i=0; i < XcR_FONT_COUNT; i++)
 {
  this->font_struct[i] = XLoadQueryFont(this->display, this->font_name[i]);
  if (this->font_struct[i] == 0)
  {
   Xc_WARNING(("Font doesn't exist on this terminal, loading fixed instead"));
   this->font[i] = XLoadFont(this->display, "fixed");
   this->font_struct[i] = XQueryFont(this->display, this->font[i]);
  }
  else
   this->font[i] = XLoadFont(this->display, this->font_name[i]);
 }

 Xc_TRACE(("end constructor"));
 return this;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_Resource *this;
{
 int i;
 Xc_HISTORY(("destructor"));

 for(i=0; i<XcR_FONT_COUNT; i++)
 {
  XFreeFont(this->display, this->font_struct[i]);
 }
 Xc_free(this);
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this)
c_Resource *this;
{
 Xc_HISTORY(("copy"));
 Xc_FATAL(("not implemented"));
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** getString - get a resource string                                 ** 
** ----------------------------------------------------------------- */
static char *getString(this, number)
c_Resource *this;
unsigned int number;
{
 Xc_TRACE(("getString(%d)", number));
 Xc_ASSERT(number < XcR_STRING_COUNT);

 return this->message[number];
}


/* ----------------------------------------------------------------- ** 
** getXmString - get a resource string                               ** 
** ----------------------------------------------------------------- */
static XmString getXmString(this, number)
c_Resource *this;
unsigned int number;
{
 Xc_TRACE(("getXmString(%d)", number));
 Xc_ASSERT(number < XcR_STRING_COUNT);

 return XmStringCreateLtoR(this->message[number], XmSTRING_DEFAULT_CHARSET);
}


/* ----------------------------------------------------------------- ** 
** getFontName - Get a font name                                     ** 
** ----------------------------------------------------------------- */
static char *getFontName(this, number)
c_Resource *this;
unsigned int number;
{
 Xc_TRACE(("getFont(%d)", number));
 Xc_ASSERT(number < XcR_FONT_COUNT);
  
 return this->font_name[number];
}

/* ----------------------------------------------------------------- ** 
** getFont - Get a Font                                              ** 
** ----------------------------------------------------------------- */
static Font getFont(this, number)
c_Resource *this;
unsigned int number;
{
 Xc_TRACE(("getFont(%d)", number));
 Xc_ASSERT(number < XcR_FONT_COUNT);
  
 return this->font[number];
}

/* ----------------------------------------------------------------- ** 
** getXFontStruct - Get a XFontStruct                                ** 
** ----------------------------------------------------------------- */
static XFontStruct *getXFontStruct(this, number)
c_Resource *this;
unsigned int number;
{
 Xc_TRACE(("getFont(%d)", number));
 Xc_ASSERT(number < XcR_FONT_COUNT);
  
 return this->font_struct[number];
}

/* ----------------------------------------------------------------- ** 
** getInt - Get an integer from resource database                    ** 
** ----------------------------------------------------------------- */
static int getInt(this, number)
c_Resource *this;
unsigned int number;
{
 Xc_TRACE(("getInt(%d)", number));
 Xc_ASSERT(number < XcR_INT_COUNT);
  
 return this->value[number];
}

/* ----------------------------------------------------------------- ** 
** getBoolean - Get a boolean from resource database                 ** 
** ----------------------------------------------------------------- */
static boolean getBoolean(this, number)
c_Resource *this;
unsigned int number;
{
 boolean result = FALSE;
 char *string;
 
 Xc_TRACE(("getBoolean(%d)", number));
 Xc_ASSERT(number < XcR_STRING_COUNT);
 
 string = this->message[number];
 if (Xstrcmp(string, "Yes") || Xstrcmp(string, "yes") ||
     Xstrcmp(string, "On") || Xstrcmp(string, "on") || Xstrcmp(string, "1") ||
     Xstrcmp(string, "True") || Xstrcmp(string, "true"))
  result = TRUE;

 return result;
}

/* ----------------------------------------------------------------- ** 
** changeString - change a string from res db. Dn not free old value.** 
** ----------------------------------------------------------------- */
static void changeString(this, number, string)
c_Resource *this;
unsigned int number;
char *string;
{
 Xc_TRACE(("changeString(%d) for: %s", number, string));
 Xc_ASSERT(number < XcR_STRING_COUNT);
  
 this->message[number] = string;
}


