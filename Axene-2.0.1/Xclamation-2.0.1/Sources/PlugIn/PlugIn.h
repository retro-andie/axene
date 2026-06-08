/*
** PlugIn.h for Xclamation in PlugIn/
** Public header for PlugIn Class
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
** Started on  Thu Oct 12 11:42:34 1995 Stéphane Boisson
** Last update Thu Oct 24 19:19:33 1996 Emmanuel Paris
*/

#ifndef __XC_PLUG_IN_H__
#define __XC_PLUG_IN_H__

typedef struct sc_PlugIn c_PlugIn;

#include "xcalibur.h"

#ifdef minor
#undef minor
#endif
#ifdef major
#undef major
#endif

/*--- Constants ---*/
#define XcPLUGIN_HEADER "XCPI/%u.%u"
#define XcPLUGIN_DIR "plugs-in"
#define XcPLUGIN_COMMAND "plugs-in"
#define XcPLUGIN_VERSION_MAJOR 1
#define XcPLUGIN_VERSION_MINOR 0
#define XcPLUGIN_LINE_LENGTH 255
#define XcPI_MAX_PREFIX 8

/*--- Supported types ---*/
#define XcPLUGIN_IMAGE_IN "image-in"
#define XcPLUGIN_TEXT_IN "text-in"
#define XcPLUGIN_SPREADSHEET_IN "spreadsheet-in"

/*--- Features ---*/
#define XcPLUGIN_FORM_FEATURE (1<<0)

/*--- Macros ---*/
#define XcPLUGIN_MAKE_VERSION(major, minor) \
  ((((unsigned long)(major)) << 16) | ((unsigned long)(minor)))
#define XcPLUGIN_VERSION \
  XcPLUGIN_MAKE_VERSION(XcPLUGIN_VERSION_MAJOR, XcPLUGIN_VERSION_MINOR)

/*--- Keywords ---*/
#define XcPI_END_KEYWORD NULL
#define XcPI_FILENAME_KEYWORD "Filename"
#define XcPI_TYPE_KEYWORD "Type"
#define XcPI_NAME_KEYWORD "Name"
#define XcPI_AUTHOR_KEYWORD "Author"
#define XcPI_COPYRIGHT_KEYWORD "Copyright"
#define XcPI_FILE_PREFIX_KEYWORD "FilePrefix"
#define XcPI_COMMENT_KEYWORD "Comment"
#define XcPI_REQUEST_KEYWORD "Request"

/*--- Requests ---*/
#define XcPI_GET_REQUEST "GET"
#define XcPI_PUT_REQUEST "PUT"
#define XcPI_HEAD_REQUEST "HEAD"
#define XcPI_FORM_REQUEST "FORM"


/*--- PlugIn item structure ---*/
typedef struct plugin_item_s plugin_item_t;
struct plugin_item_s {
  plugin_item_t	*next;

  /*___Identifiers___*/
  char		id[XcMAX_NAME_LENGTH];
  char		*filename;		/* filename==NUL -> internal plug-in */

  /*___Version___*/
  unsigned int	major;
  unsigned int	minor;
  unsigned int	features;

  /*___Informations___*/
  char		*name;
  char		*author;
  char		*copyright;
  char		*comment;

  /*___File prefix___*/
  char		*prefix[XcPI_MAX_PREFIX];
};


/*--- Definition of the PlugIn methods ---*/
typedef struct
{
  F_STD;

  /*___Add & remove plug-in items___*/
  boolean (*add) ___PROTO((c_PlugIn *this, char *path, char *file));
  void (*removeItem) ___PROTO((c_PlugIn *this, plugin_item_t *item));

  /*___Get a plug-in item by name___*/
  plugin_item_t *(*getByName) ___PROTO((c_PlugIn *this, char *name));

  /*___Send request to plug-in item___*/
  int (*request) ___NPROTO((c_PlugIn *this, plugin_item_t *item,
			    char *request, ...));
} sf_PlugIn;


/*--- Definition of the PlugIn class ---*/
struct sc_PlugIn
{
  sf_PlugIn	*f;

  char		type[XcMAX_NAME_LENGTH];
  plugin_item_t	*base;
};

extern sf_PlugIn fc_PlugIn;

#endif /* !__XC_PLUG_IN_H__ */


