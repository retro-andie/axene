/*
** memspool.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Managers/
** Spool allcation functions
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
** Started on  Tue Mar 21 16:48:29 1995 Stéphane Boisson
** Last update Tue Jan 16 13:02:52 1996 Stéphane Boisson
*/

#include "MemoryManagerP.h"

#define HANDLE_SIZE Xc_ROUND_SIZE(sizeof(SpoolHandle))

/* ----------------------------------------------------------------- ** 
** Xc_NormalCreateSpool - Spool creation                             ** 
** ----------------------------------------------------------------- */
SpoolHandle *Xc_NormalCreateSpool(desc, size, flags)
char *desc;
size_t size;
int flags;
{
  SpoolHandle *handle;
  long real_size;

  size = Xc_ROUND_SIZE(size);
  real_size = HANDLE_SIZE + size;
  if((handle = Xc_NormalMemAlloc(desc, real_size)) == NULL) return NULL;

  handle->next = NULL;
  handle->description = desc;
  handle->size = size;
  handle->pos = 0;
  handle->flags = flags;
  handle->data = ((char *)handle) + HANDLE_SIZE;

  return handle;
}


/* ----------------------------------------------------------------- ** 
** Xc_NormalSpoolAlloc - Spool allocation                           ** 
** ----------------------------------------------------------------- */
void *Xc_NormalSpoolAlloc(handle, size)
SpoolHandle *handle;
size_t size;
{
  SpoolHandle **pptr, *ptr;
  long real_size;

  size = Xc_ROUND_SIZE(size);
  for(pptr = &handle; (ptr = *pptr) != NULL; pptr = &((*pptr)->next))
    if((ptr->pos + size) <= ptr->size)
      {
	ptr->pos += size;
	return(ptr->data + ptr->pos - size);
      }

  real_size = HANDLE_SIZE + Xc_ROUND_SIZE(MAX(size, handle->size));
  if((*pptr = Xc_NormalMemAlloc(handle->description, real_size)) == NULL)
    return NULL;
  ptr = *pptr;
  
  ptr->next = NULL;
  ptr->description = handle->description;
  ptr->size = handle->size;
  ptr->pos = handle->pos;
  ptr->flags = handle->flags;
  ptr->data = handle->data;

  handle->size = MAX(size, ptr->size);
  handle->pos = size;
  handle->data = ((char *)ptr) + HANDLE_SIZE;
  
  return handle->data;
}


/* ----------------------------------------------------------------- ** 
** Xc_TraceCreateSpool - Spool creation in debugging mode            ** 
** ----------------------------------------------------------------- */
SpoolHandle *Xc_TraceCreateSpool(desc, size, flags, filename, linenumber)
char *desc;
size_t size;
int flags;
char *filename;
int linenumber;
{
  SpoolHandle *handle;
  long real_size;

  size = Xc_ROUND_SIZE(size);
  real_size = HANDLE_SIZE + size;
  handle = Xc_TraceMemAlloc(desc, real_size, filename, linenumber);
  if(handle == NULL) return NULL;

  handle->next = NULL;
  handle->description = desc;
  handle->size = size;
  handle->pos = 0;
  handle->flags = flags;
  handle->data = ((char *)handle) + HANDLE_SIZE;

  return handle;
}


/* ----------------------------------------------------------------- ** 
** Xc_TraceSpoolAlloc - Spool allocation in debug mode               ** 
** ----------------------------------------------------------------- */
void *Xc_TraceSpoolAlloc(handle, size, filename, linenumber)
SpoolHandle *handle;
size_t size;
char *filename;
int linenumber;
{
  SpoolHandle **pptr, *ptr;
  long real_size;

  size = Xc_ROUND_SIZE(size);
  for(pptr = &handle; (ptr = *pptr) != NULL; pptr = &((*pptr)->next))
    if((ptr->pos + size) <= ptr->size)
      {
	ptr->pos += size;
	return(ptr->data + ptr->pos - size);
      }

  real_size = HANDLE_SIZE + Xc_ROUND_SIZE(MAX(size, handle->size));
  *pptr = Xc_TraceMemAlloc(handle->description, real_size,
			   filename, linenumber);
  if((ptr = *pptr) == NULL) return NULL;
  
  ptr->next = NULL;
  ptr->description = handle->description;
  ptr->size = handle->size;
  ptr->pos = handle->pos;
  ptr->flags = handle->flags;
  ptr->data = handle->data;

  handle->size = MAX(size, ptr->size);
  handle->pos = size;
  handle->data = ((char *)ptr) + HANDLE_SIZE;
  
  return handle->data;
}
