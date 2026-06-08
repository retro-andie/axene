/*
** Tongues.c for XInstall in Tongues/
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
** Last update Sat Jul  5 21:11:26 1997 Antoine Buat
*/

#include "Tongues.h"
#include "TonguesP.h"

static void *constructor();
static void destructor();
static void *copy();

sf_Tongue fc_Tongue = 
{
 constructor, destructor, copy
};

/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(widget, tongue)
Widget widget;
int tongue;
{
 c_Tongue *this;
 XrmDatabase	db;
 char **ptr;
 
 Xc_HISTORY(("constructor"));

 if ((this = Xc_malloc("Tongues", sizeof(c_Tongue))) == NULL) return NULL;
 memset(this, 0, sizeof(c_Tongue));
 this->f = &fc_Tongue;
 
 db = XrmGetDatabase(XtDisplay(widget));
 ptr = tongues_db[tongue-1];
 while(*ptr)
 {
  XrmPutLineResource(&db, *ptr);
  ptr++;
 }

 Xc_TRACE(("end constructor"));
 return this;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_Tongue *this;
{
 Xc_HISTORY(("destructor"));
 Xc_free(this);
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this)
c_Tongue *this;
{
 Xc_HISTORY(("copy"));
 Xc_FATAL(("not implemented"));
 return NULL;
}

