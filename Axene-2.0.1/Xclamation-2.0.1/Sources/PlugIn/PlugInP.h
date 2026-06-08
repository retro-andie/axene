/*
** PlugInP.h for Xclamation in PlugIn/
** Private header for PlugIn class
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
** Started on  Thu Oct 12 11:40:25 1995 Stéphane Boisson
** Last update Mon Oct 16 17:33:47 1995 Stéphane Boisson
*/

#ifndef __XC_PLUG_IN_P_H__
#define __XC_PLUG_IN_P_H__

#include "PlugIn.h"

/*--- Header structure ---*/
typedef struct plugin_header_s plugin_header_t;
struct plugin_header_s {
  plugin_header_t *next;  /* MUST BE FIRST */
  char *key;
  char *value;
};

typedef struct plugin_header_list_s plugin_header_list_t;
struct plugin_header_list_s {
  plugin_header_list_t *next;  /* MUST BE FIRST */
  unsigned int major;
  unsigned int minor;
  plugin_header_t *header;
};

#endif /* __XC_PLUG_IN_P_H__ */
