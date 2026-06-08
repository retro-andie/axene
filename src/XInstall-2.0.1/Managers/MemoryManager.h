/*
** MemoryManager.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Managers/
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
** Started on  Sun Jun 12 01:56:40 1994 Stéphane Boisson
** Last update Sun Jun 11 19:50:36 2000 Emmanuel Paris
*/

#ifndef __XC_MEMORY_H__
#define __XC_MEMORY_H__

#include "xcalibur.h"
#include <sys/types.h>

#define Xc_SIZE_UNIT sizeof(long)
#define Xc_ROUND_SIZE(size) ((((size) + Xc_SIZE_UNIT - 1) / Xc_SIZE_UNIT) * Xc_SIZE_UNIT)


/*--- Item handle structure ---*/
typedef struct
{
  char *description;
  void **next_free;
  void **chunk_chain;
  size_t slot_size;
  size_t slots_in_chunk;
  size_t chunk_size;
  void **temp;
} ItemHandle;


/*--- Spool handle structure ---*/
typedef struct spool_handle_s SpoolHandle;
struct spool_handle_s
{
  SpoolHandle *next;
  char *description;
  size_t size;
  size_t pos;
  unsigned int flags;
  char *data;
};


/*--- Stack handle structure ---*/
typedef struct stack_handle_s StackHandle;
struct stack_handle_s
{
  char *description;
  size_t size;
  ItemHandle *items;
  void **head;
  void **tail;
  void **temp;
  int depth;
};

extern void *Xc_TraceMemAlloc ___PROTO((char *description, size_t size,
					char *, int));
extern void Xc_TraceMemFree ___PROTO((void *ptr, char *, int));
extern void *Xc_NormalMemAlloc ___PROTO((char *description, size_t size));
extern void *Xc_NormalMemRealloc ___PROTO((char *desc, void *pt, size_t size));
extern void *Xc_TraceMemRealloc ___PROTO((char *description,
					  void *ptr,
					  size_t size,
					  char *filename, int linenumber));
extern ItemHandle *Xc_TraceItemInit ___PROTO((char *description,
					      size_t slots_per_chunk,
					      size_t object_size,
					      char *filename, int linenumber));
extern void Xc_TraceItemDestroy ___PROTO((ItemHandle *handle,
					  char *filename, int linenumber));
extern void *Xc_TraceItemAlloc ___PROTO((ItemHandle *handle,
					 char *filename, int linenumber));
extern void Xc_TraceItemFree ___PROTO((ItemHandle *handle, void *pointer,
				       char *filename, int linenumber));

extern ItemHandle *Xc_NormalItemInit ___PROTO((char *description,
					      size_t slots_per_chunk,
					      size_t object_size));
extern void Xc_NormalItemDestroy ___PROTO((ItemHandle *handle));
extern void *Xc_NormalItemAlloc ___PROTO((ItemHandle *handle));

extern void Xc_TraceMemCheck ___PROTO((char *filename, int linenumber));
extern void Xc_TraceMemDump ___PROTO((char *filename, int linenumber));
extern char *Xc_NormalStrDup ___PROTO((char *description, char *string));
extern char *Xc_TraceStrDup ___PROTO((char *description, char *string,
				      char *, int));

extern void free ___PROTO((void *));

/*--- Spool protoypes ---*/
extern SpoolHandle *Xc_TraceCreateSpool ___PROTO((char *desc, size_t size,
						  int flags, char *filename,
						  int linenumber));
extern void *Xc_TraceSpoolAlloc ___PROTO((SpoolHandle *handle, size_t size,
					   char *filename, int linenumber));
extern SpoolHandle *Xc_NormalCreateSpool ___PROTO((char *desc, size_t size,
						  int flags));
extern void *Xc_NormalSpoolAlloc ___PROTO((SpoolHandle *handle, size_t size));

/*--- Stack prototypes ---*/
StackHandle *Xc_NormalStackCreate ___PROTO((char *desc, size_t size, int nb));
void *Xc_NormalStackTopPop ___PROTO((StackHandle *handle));
void *Xc_NormalStackBottomPop ___PROTO((StackHandle *handle));
StackHandle *Xc_TraceStackCreate ___PROTO((char *desc, size_t size, int count,
					   char *file, int line));
void *Xc_TraceStackTopPop ___PROTO((StackHandle *handle, char *file, int lin));
void *Xc_TraceStackBottomPop ___PROTO((StackHandle *handle, char *file,
				       int line));


#define Xc_getItemSize(handle) ((handle)->slot_size)
#define Xc_mdump() Xc_TraceMemDump(__FILE__, __LINE__)
#define Xc_mcheck() Xc_TraceMemCheck(__FILE__, __LINE__)

#ifndef NMEMDEBUG

#define Xc_malloc(desc, size) Xc_TraceMemAlloc((desc),(size),__FILE__,__LINE__)
#define Xc_free(ptr) Xc_TraceMemFree((ptr),__FILE__,__LINE__)
#define Xc_realloc(desc, ptr, size) Xc_TraceMemRealloc((desc),(ptr),(size),__FILE__,__LINE__)
#define Xc_createItem(desc,base,size) Xc_TraceItemInit((desc),(base),(size),__FILE__,__LINE__)
#define Xc_destroyItem(handle) Xc_TraceItemDestroy((handle),__FILE__,__LINE__)
#define Xc_new(handle) (void *)Xc_TraceItemAlloc((handle),__FILE__,__LINE__)
#define Xc_delete(handle,ptr) Xc_TraceItemFree((handle),(ptr),__FILE__,__LINE__)
#define Xc_strdup(desc, string) Xc_TraceStrDup((desc),(string),__FILE__,__LINE__)

/*--- Spool ---*/
#define Xc_createSpool(desc, size, flags) \
  Xc_TraceCreateSpool((desc), (size), (flags), __FILE__, __LINE__)
#define Xc_smalloc(handle, sz) ((((handle)->pos + (sz)) > (handle)->size)?\
Xc_TraceSpoolAlloc((handle), (sz), __FILE__, __LINE__) : \
(((handle)->pos += Xc_ROUND_SIZE(sz)), \
 (void *)((handle)->data + (handle)->pos - Xc_ROUND_SIZE(sz))))

/*--- Stack ---*/
#define Xc_stackCreate(desc, size, count) \
 Xc_TraceStackCreate((desc), (size), (count), __FILE__, __LINE__)
#define Xc_stackTopPop(handle) \
 Xc_TraceStackTopPop((handle), __FILE__, __LINE__)
#define Xc_stackBottomPop(handle) \
 Xc_TraceStackBottomPop((handle), __FILE__, __LINE__)


#else /* NMEMDEBUG */

#define Xc_malloc(desc, size) Xc_NormalMemAlloc((desc),(size))
#define Xc_realloc(desc, ptr, size) Xc_NormalMemRealloc((desc),(ptr),(size))
#define Xc_free(ptr) free(ptr)
#define Xc_createItem(desc,base,size) Xc_NormalItemInit((desc),(base),(size))
#define Xc_destroyItem(handle) Xc_NormalItemDestroy(handle)

#define Xc_new(handle) \
((void *)((handle)->next_free == NULL)? \
 Xc_NormalItemAlloc(handle) \
 : ((handle)->temp = (handle)->next_free, \
    (handle)->next_free = ((void **) *((handle)->next_free)), \
    (void *) ((handle)->temp)))

#define Xc_delete(handle, ptr) \
do { \
    register void **temp= (void **)(ptr); \
    *temp = (handle)->next_free; \
    (handle)->next_free = temp; \
} while(0)

#define Xc_strdup(desc, string) Xc_NormalStrDup((desc), (string))

/*--- Spool ---*/
#define Xc_createSpool(desc, size, flags) \
  Xc_NormalCreateSpool((desc), (size), (flags))
#define Xc_smalloc(handle, sz) ((((handle)->pos + (sz)) > (handle)->size)?\
Xc_NormalSpoolAlloc((handle), (sz)) : \
(((handle)->pos += Xc_ROUND_SIZE(sz)), \
 (void *)((handle)->data + (handle)->pos - Xc_ROUND_SIZE(sz))))

/*--- Stack ---*/
#define Xc_stackCreate(desc, size, count) \
 Xc_NormalStackCreate((desc), (size), (count))
#define Xc_stackTopPop(handle) Xc_NormalStackTopPop(handle)
#define Xc_stackBottomPop(handle) Xc_NormalStackBottomPop(handle)

#endif /* NMEMDEBUG */


/*--- Spool ---*/
#define Xc_destroySpool(handle) \
do{ register SpoolHandle *ptr, *ptr2; \
  for(ptr = (handle); (ptr2 = ptr->next) != NULL; ptr = ptr2) Xc_free(ptr); \
  Xc_free(ptr); \
} while(0)


/*--- Stack ---*/
#define Xc_stackMalloc(handle) \
((((handle)->temp = Xc_new((handle)->items)) == NULL)? \
 NULL : (void *)((handle)->temp + 1))

#define Xc_stackFree(handle, ptr) \
Xc_delete((handle)->items, (void **)(ptr) - 1)

#define Xc_stackDestroy(handle) \
do{Xc_destroyItem((handle)->items); Xc_free(handle);}while(0) 

#define Xc_stackTopPush(handle, data) \
((handle)->depth++, *(handle)->head = (void *)((void **)(data) - 1), \
 (handle)->head = (void **)(data) - 1, \
 ((void **)(data))[-1] = NULL )\

#define Xc_stackBottomPush(handle, data) \
((handle)->depth++, ((void **)(data))[-1] = (void *)(handle)->tail, \
 (handle)->tail = (void **)(data) - 1)

#define Xc_stackDepth(handle) ((handle)->depth)

#endif

