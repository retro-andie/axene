/*
** TonguesP.h for XInstall in Tongues/
** Set Application resources for text messages with tongue dependencies.
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Sat Jul  5 16:05:57 1997 Emmanuel Paris
** Last update Sat Jul  5 18:14:15 1997 Emmanuel Paris
*/

#ifndef _TonguesP_h_
#define _TonguesP_h_

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

static char *english_db[] =
{
#include "Tongue.english.h"
 NULL
};

static char *french_db[] =
{
#include "Tongue.french.h"
 NULL
};

static char *german_db[] =
{
#include "Tongue.german.h"
 NULL
};

static char *spanish_db[] =
{
#include "Tongue.spanish.h"
 NULL
};
  
static char **tongues_db[] = {
 english_db, english_db, french_db, german_db, spanish_db
};


#endif /* _Tongues_h_ */
