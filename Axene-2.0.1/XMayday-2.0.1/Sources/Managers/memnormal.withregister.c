/*
** memnormal.withregister.c for XMayday in Managers/
** Memory management with debugging
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
** Started on  Sun Jun 12 01:57:34 1994 Stéphane Boisson
** Last update Thu Feb  8 19:25:23 1996 Emmanuel Paris
*/

#include "MemoryManagerP.h"


/* ----------------------------------------------------------------- ** 
** Xc_NormalMemAlloc - malloc                                        ** 
** ----------------------------------------------------------------- */
void *Xc_NormalMemAlloc(description, size)
char *description;
size_t size;
{
  register void *pointer;

  if((pointer = (void *)malloc(size)) == NULL)
    Xc_ERROR(("Out of memory for '%s'", description));
  return(pointer);
}


/* ----------------------------------------------------------------- ** 
** Xc_NormalMemRealloc - realloc                                     ** 
** ----------------------------------------------------------------- */
void *Xc_NormalMemRealloc(description, ptr, size)
char *description;
void *ptr;
size_t size;
{
  register void *pointer;

 if (ptr)
   {
     if(((pointer = (void *)realloc(ptr, size)) == NULL) && (size != 0))
       Xc_ERROR(("Out of memory for '%s'", description));
   }
 else
   {
     if(((pointer = (void *)malloc(size)) == NULL) && (size != 0))
       Xc_ERROR(("Out of memory for '%s'", description));
   }
  return(pointer);
}


/* ----------------------------------------------------------------- ** 
** Xc_NormalStrDup - strdup                                           ** 
** ----------------------------------------------------------------- */
char *Xc_NormalStrDup(desc, string)
char *desc;
char *string;
{
  int len;
  char *ptr;

  len = strlen(string) + 1;
  ptr = Xc_NormalMemAlloc(desc, len);
  if(ptr != NULL) memcpy(ptr, string, len);
  return ptr;
}


/* ----------------------------------------------------------------- ** 
** Xc_NormalItemInit - Item init                                     ** 
** ----------------------------------------------------------------- */
ItemHandle *Xc_NormalItemInit(description, slots_per_chunk, object_size)
char *description;
size_t slots_per_chunk;
size_t object_size;
{
  register ItemHandle *fp;
  size_t nslots;
  size_t chunk_size;
  size_t slot_size;

  /*--- get default value? ---*/
  nslots = (slots_per_chunk == 0) ? SLOTS_PER_CHUNK : slots_per_chunk;

  /*--- Minimum is pointer size ---*/
  slot_size = (object_size < sizeof(void *)) ? (sizeof(void *)) : object_size;

  /*--- Make sure that the slot_size is a multiple of an int ---*/
  slot_size = (slot_size - 1 + sizeof(int)) & -(sizeof(int));

  chunk_size = nslots * slot_size;

  fp = Xc_malloc(description, chunk_size + sizeof(ItemHandle));
  if(fp == NULL) return(fp);

  fp->description = description;
  fp->chunk_chain = NULL;
  fp->next_free = (void **)((char *)fp + sizeof(ItemHandle));
  fp->slots_in_chunk = nslots;
  fp->slot_size = slot_size;
  fp->chunk_size = chunk_size;
  
  itemInitList(nslots, slot_size, fp->next_free);
  return(fp);
}


/* ----------------------------------------------------------------- ** 
** Xc_NormalItemDestroy - Item destroy                               ** 
** ----------------------------------------------------------------- */
void Xc_NormalItemDestroy(handle)
ItemHandle *handle;
{
  register void **chp, **next_chunk;
  
  chp = handle->chunk_chain;
  Xc_free(handle);
  
  while(chp != NULL)
    {
      next_chunk = *chp;
      Xc_free(chp);
      chp = next_chunk;
    }
}


/* ----------------------------------------------------------------- ** 
** Xc_NormalItemAlloc - Xc_new                                       ** 
** ----------------------------------------------------------------- */
void *Xc_NormalItemAlloc(handle)
ItemHandle *handle;
{
  register void **object;
  
  if(handle->next_free == NULL)
    {
      void **chp= (void **) &(handle->chunk_chain);
      
      /*--- Allocate new chunk ---*/
      while(*chp != NULL) chp = *chp;
      *chp = Xc_malloc(handle->description,
			       handle->chunk_size + sizeof(void *));
      chp = *chp;
      if(chp == NULL) return(chp);
      *chp = NULL;
      
      handle->next_free = (void **)((char *)chp + sizeof(void *));
      itemInitList(handle->slots_in_chunk, handle->slot_size,
		     handle->next_free);
    }
  
  object = handle->next_free;
  handle->next_free = *object;
  
  return(object);
}


/* ----------------------------------------------------------------- ** 
** itemInitList - Linked list init                                   ** 
** ----------------------------------------------------------------- */
void itemInitList(nslots, size, slots)
size_t nslots;
size_t size;
void *slots;
{
  register void **current= (void **)slots;
  register void **next= (void **)((char *)current + size);

  while(--nslots)
    {
      *current = next;
      current = next;
      next = (void **)((char *)next + size);
    }
  *current = NULL;
}

