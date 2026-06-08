/*
** config.h for XMayday in Main/
** Header for Config file functions
**
** Copyright (C) 1996-2000 Axene.
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
** Started on  Sat May 11 14:25:00 1996 Emmanuel Paris
** Last update Sat May 11 14:25:11 1996 Emmanuel Paris
*/

#ifndef __XC_CONFIG_FILE_H__
#define __XC_CONFIG_FILE_H__

#include "xcalibur.h"

#define Xc_USER_CONFIG_FILE ".xmaydayrc"
#define Xc_CONFIG_FILE "xmaydayrc"

#define XcDF_CONFIG_KEYWORD "CONFIG"
#define XcDF_PRINTER_KEYWORD "PRINTER"

/*--- Protoypes ---*/
boolean ReadConfigFile ___PROTO((void));
boolean ReadUserConfigFile ___PROTO((void));
boolean WriteUserConfigFile ___PROTO((void));


#endif /* !___XC_CONFIG_FILE_H__ */
