/*
** memstack.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Managers/
** Stack memory management
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
** Started on  Mon Jan  8 18:39:51 1996 Stéphane Boisson
** Last update Fri Jan 19 16:50:15 1996 Stéphane Boisson
*/

#include "MemoryManagerP.h"


/* ----------------------------------------------------------------- ** 
** Xc_NormalStackCreate - Initialize a stack                         ** 
** ----------------------------------------------------------------- */
StackHandle *Xc_NormalStackCreate(desc, size, count)
char *desc;
size_t size;
int count;
{
  StackHandle *handle;
  size_t real_size;

  if((handle = Xc_NormalMemAlloc(desc, sizeof(StackHandle))) == NULL)
    return NULL;

  real_size = size + sizeof(void *);
  
  if((handle->items = Xc_NormalItemInit(desc, count, real_size)) == NULL)
    {
      free(handle);
      return NULL;
    }

  handle->description = desc;
  handle->size = size;
  handle->head = handle->tail = NULL;
  handle->depth = 0;

  return handle;
}


/* ----------------------------------------------------------------- ** 
** Xc_NormalStackTopPop - Pop item from top                          ** 
** ----------------------------------------------------------------- */
void *Xc_NormalStackTopPop(handle)
StackHandle *handle;
{
  void **ptr;
  void *item;

  if(handle->head == NULL)
    {
      Xc_WARNING(("Stack empty for `%s'", handle->description));
      return NULL;
    }

  item = (void *)handle->head;
  ptr = (void **)&handle->tail;

  while(*ptr != item) ptr = (void **)*ptr;

  *ptr = NULL;
  handle->head = (handle->tail == NULL)? NULL : (void **)ptr;

  handle->depth--;
  return((void *)((void **)item + 1));
}


/* ----------------------------------------------------------------- ** 
** Xc_NormalStackBottomPop - Pop item from bottom                    ** 
** ----------------------------------------------------------------- */
void *Xc_NormalStackBottomPop(handle)
StackHandle *handle;
{
  void **item;

  if(handle->tail == NULL)
    {
      Xc_WARNING(("Stack empty for `%s'", handle->description));
      return NULL;
    }
  
  item = handle->tail;
  if((handle->tail = (void **)*item) == NULL) handle->head = NULL;
  handle->depth--;
  return((void *)(item + 1));
}


#include "memtrace.h"

/* ----------------------------------------------------------------- ** 
** Xc_TraceStackCreate - Initialize a stack                          ** 
** ----------------------------------------------------------------- */
StackHandle *Xc_TraceStackCreate(desc, size, count, filename, linenumber)
char *desc;
size_t size;
int count;
char *filename;
int linenumber;
{
  StackHandle *handle;
  size_t real_size;

  handle = Xc_TraceMemAlloc(desc, sizeof(StackHandle),
			    filename, linenumber);
  if(handle == NULL) return NULL;

  real_size = size + sizeof(void *);
  
  handle->items = Xc_TraceItemInit(desc, count, real_size,
				   filename, linenumber);
  if(handle->items == NULL)
    {
      Xc_TraceMemFree(handle, filename, linenumber);
      return NULL;
    }

  handle->description = desc;
  handle->size = size;
  handle->head = handle->tail = NULL;
  handle->depth = 0;

  return handle;
}


/* ----------------------------------------------------------------- ** 
** Xc_TraceStackTopPop - Pop item from top                           ** 
** ----------------------------------------------------------------- */
void *Xc_TraceStackTopPop(handle, filename, linenumber)
StackHandle *handle;
char *filename;
int linenumber;
{
  void **ptr;
  void *item;

  if(handle->head == NULL)
    {
      Xc_WARNING(("Stack empty for `%s'", handle->description));
      return NULL;
    }

  item = (void *)handle->head;
  ptr = (void **)&handle->tail;

  while(*ptr != item) ptr = (void **)*ptr;

  *ptr = NULL;
  handle->head = (handle->tail == NULL)? NULL : (void **)ptr;

  handle->depth--;

  return((void *)((void **)item + 1));
}


/* ----------------------------------------------------------------- ** 
** Xc_TraceStackBottomPop - Pop item from bottom                     ** 
** ----------------------------------------------------------------- */
void *Xc_TraceStackBottomPop(handle, filename, linenumber)
StackHandle *handle;
char *filename;
int linenumber;
{
  void **item;

  if(handle->tail == NULL)
    {
      Xc_WARNING(("Stack empty for `%s'", handle->description));
      return NULL;
    }
  
  item = handle->tail;
  if((handle->tail = (void **)*item) == NULL) handle->head = NULL;

  handle->depth--;

  return((void *)(item + 1));
}
