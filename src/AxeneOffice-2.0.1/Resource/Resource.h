/*
** Resource.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Resource/
** Definitions for the Resource class
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
** Started on  Thu Dec 15 18:44:46 1994 St�phane Boisson
** Last update Thu Jan 27 11:57:20 2000 Emmanuel Paris
*/

#ifndef __XC_RESOURCE_H__
#define __XC_RESOURCE_H__

typedef struct sc_Resource c_Resource;

#include "xcalibur.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#ifdef Xc_XQUAD
#include "Resource_TABLEUR.h"
#endif

#ifdef Xc_XCLAMATION
#include "Resource_PAO.h"
#endif

#ifdef Xc_XALLWRITE
#include "Resource_WORD.h"
#endif

#ifdef Xc_XMAYDAY
#include "Resource_HELP.h"
#endif

#ifdef Xc_XINSTALL
#include "Resource_INSTALL.h"
#endif

#ifdef Xc_AXENEOFFICE
#include "Resource_OFFICE.h"
#endif

#ifdef __STDC__
# define XcR_MAKE_STRING(_PARAM) #_PARAM
#else
# define XcR_MAKE_STRING(_PARAM) "_PARAM"
#endif /* __STDC__ */

#define XcR_STRING_OFFSET(number) XtOffsetOf(c_Resource, message[number])
#define XcR_FONT_OFFSET(number) XtOffsetOf(c_Resource, font_name[number])
#define XcR_INT_OFFSET(number) XtOffsetOf(c_Resource, value[number])

#if defined(___GNUC_COMPOSE_MACRO) || defined(___PSEUDO_COMPOSE_MACRO)
# define XcR_STRING(id_string, default_string) \
{XcR_MAKE_STRING(id_string##Msg), "Message", XtRString, sizeof(String), \
  XcR_STRING_OFFSET(XcR_##id_string), XtRString, default_string}
# define XcR_FONT(id_string, default_font) \
{XcR_MAKE_STRING(id_string), "Font", XtRString, sizeof(String), \
  XcR_FONT_OFFSET(XcR_##id_string), XtRString, default_font}
# define XcR_INT(id_string, default_value) \
{XcR_MAKE_STRING(id_string), "Value", XtRInt, sizeof(int), \
  XcR_INT_OFFSET(XcR_##id_string), XtRImmediate, (caddr_t)default_value}
#else
# define XcR_STRING(id_string, default_string) \
{XcR_MAKE_STRING(id_string/**/Msg), "Message", XtRString, sizeof(String), \
  XcR_STRING_OFFSET(XcR_/**/id_string), XtRString, default_string} 
# define XcR_FONT(id_string, default_font) \
{XcR_MAKE_STRING(id_string), "Font", XtRString, sizeof(String), \
  XcR_FONT_OFFSET(XcR_/**/id_string), XtRString, default_font} 
# define XcR_INT(id_string, default_value) \
{XcR_MAKE_STRING(id_string), "Value", XtRInt, sizeof(int), \
  XcR_INT_OFFSET(XcR_/**/id_string), XtRImmediate, (caddr_t) default_value}
#endif /* __GNUC__ */

/*--- Methods definition ---*/
typedef struct 
{
 F_STD;
 char *(*getString) ___PROTO((c_Resource *this, unsigned int number));
 XmString (*getXmString) ___PROTO((c_Resource *this, unsigned int number));
 char *(*getFontName) ___PROTO((c_Resource *this, unsigned int number));
 Font (*getFont) ___PROTO((c_Resource *this, unsigned int number));
 XFontStruct *(*getXFontStruct) ___PROTO((c_Resource *this, 
					  unsigned int number));
 int (*getInt) ___PROTO((c_Resource *this, unsigned int number));
 boolean (*getBoolean) ___PROTO((c_Resource *this, unsigned int number));
 void (*changeString) ___PROTO((c_Resource *this, unsigned int number,
				char *string));
} sf_Resource;


/*--- Class definition ---*/
struct sc_Resource 
{
 sf_Resource *f;
  
 Display *display;
 char *message[XcR_STRING_COUNT];
 char *font_name[XcR_FONT_COUNT];
 Font font[XcR_FONT_COUNT];
 XFontStruct *font_struct[XcR_FONT_COUNT];
 int value[XcR_INT_COUNT];
};

extern sf_Resource fc_Resource;

#endif /* !__XC_RESOURCE_H__ */








