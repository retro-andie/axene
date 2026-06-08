/*
** load.c for Xclamation and XAllWrite in VectorGraph/
** load vector module
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
** Started on  Mon Apr 28 21:02:35 1997 Antoine Buat
** Last update Mon May 19 01:34:02 1997 Antoine Buat
*/

/* should work with module like in image.h in vectorgraph.h */

#include "xcalibur.h"
#include "VectorGraph.h"
#include "AIDocFile.h"
#include "WMFDocFile.h"
#include "XFIGDocFile.h"


static boolean is_ai();
static boolean is_wmf();
static boolean is_xfig();

/* ----------------------------------------------------------------- ** 
** find vector file format and load                                  ** 
** ----------------------------------------------------------------- */
boolean vectorLoad(this, filename)
c_VectorGraph	*this;
char		*filename;
{
 FILE	*fd;

 if (!(fd = fopen(filename, "r")))
 {
  return FALSE;
 }
 if (is_ai(fd))
 {
  c_AIDocFile	*ai_load;
  fclose(fd);
  this->ftype = XcVG_AI;
  ai_load = NEW(c_AIDocFile)(XcAIDF_LOAD, filename, -1, this);
  if((ai_load == NULL) || !F(ai_load).readMain(ai_load))
  {
   if(ai_load != NULL)  
    DELETE(c_AIDocFile)(ai_load);
   return FALSE;
  }
  DELETE(c_AIDocFile)(ai_load);
  return TRUE;
 }
 if (is_wmf(fd))
 {
  c_WMFDocFile	*wmf_load;
  fclose(fd);
  this->ftype = XcVG_WMF;
  wmf_load = NEW(c_WMFDocFile)(filename, this);
  if((wmf_load == NULL) || !F(wmf_load).readMain(wmf_load))
  {
   if(wmf_load != NULL)  
    DELETE(c_WMFDocFile)(wmf_load);
   return FALSE;
  }
  DELETE(c_WMFDocFile)(wmf_load);
  return TRUE;
 }
 if (is_xfig(fd))
 {
  c_XFIGDocFile	*xfig_load;
  fclose(fd);
  this->ftype = XcVG_XFIG;
  xfig_load = NEW(c_XFIGDocFile)(filename, this);
  if((xfig_load == NULL) || !F(xfig_load).readMain(xfig_load))
  {
   if(xfig_load != NULL)  
    DELETE(c_XFIGDocFile)(xfig_load);
   return FALSE;
  }
  DELETE(c_XFIGDocFile)(xfig_load);
  return TRUE;
 }

 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** test if this file is ai compatible                                ** 
** ----------------------------------------------------------------- */
static boolean is_ai(fd)
FILE	*fd;
{
 char	buffer[12];
 
 fseek(fd, 0, SEEK_SET);
 if ((fread(buffer, 1, 12, fd) != 12) ||
     strncmp(buffer, "%!PS-Adobe-", 11))
  return FALSE;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** test if this file is wmf compatible                               ** 
** ----------------------------------------------------------------- */
static boolean is_wmf(fd)
FILE	*fd;
{
 unsigned char	buffer[4];
 
 fseek(fd, 0, SEEK_SET);
 if ((fread(buffer, 1, 4, fd) != 4) ||
     (buffer[0] != 0xd7) || (buffer[1] != 0xcd) ||
     (buffer[2] != 0xc6) || (buffer[3] != 0x9a))
  return FALSE;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** test if this file is xfig compatible                              ** 
** ----------------------------------------------------------------- */
static boolean is_xfig(fd)
FILE	*fd;
{
 unsigned char	buffer[4];
 
 fseek(fd, 0, SEEK_SET);
 if ((fread(buffer, 1, 4, fd) != 4) ||
     strncmp(buffer, "#FIG", 4))
  return FALSE;
 return TRUE;
}
