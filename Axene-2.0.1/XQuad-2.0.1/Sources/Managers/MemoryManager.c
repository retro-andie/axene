/*
** MemoryManager.c for XQuad in Managers/
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
** Last update Thu Feb  8 19:20:47 1996 Emmanuel Paris
*/

#define NTRACE
#define NDEBUG
#define NMEMDEBUG
#include "MemoryManager.h"
#include "ErrorManager.h"

extern void *malloc();
extern void *realloc();
#ifndef __GNUC__
extern unsigned long rand();
#endif

/* ______Default value for slots_per_chunk______ */
#define SLOTS_PER_CHUNK 100

/****=====================================================================****/
/**** NORMAL ALLOCATION ROUTINES                                          ****/
/****=====================================================================****/

static void init_free_list();

/****=====================================================================****/
/* Xc_malloc - malloc substitution                                           */
/****=====================================================================****/
void *Xc_NormalMemAlloc(description, size)
char *description;
size_t size;
{
  register void *pointer;

  if((pointer = (void *)malloc(size)) == NULL)
    Xc_ERROR(("Out of memory for '%s'", description));
  return(pointer);
}

/****=====================================================================****/
/* Xc_realloc - realloc substitution                                         */
/****=====================================================================****/
void *Xc_NormalMemRealloc(description, ptr, size)
char *description;
void *ptr;
size_t size;
{
  register void *pointer;

  if (ptr)
    if(((pointer = (void *)realloc(ptr, size)) == NULL) && (size != 0))
      Xc_ERROR(("Out of memory for '%s'", description));
  else
    if(((pointer = (void *)malloc(size)) == NULL) && (size != 0))
      Xc_ERROR(("Out of memory for '%s'", description));
  return(pointer);
}

/****=====================================================================****/
/* Xc_createItem - Initialization for Item allocation                        */
/****=====================================================================****/
ItemHandle *Xc_NormalItemInit(description, slots_per_chunk, object_size)
char *description;
size_t slots_per_chunk;
size_t object_size;
{
  register ItemHandle *fp;
  size_t nslots;
  size_t chunk_size;
  size_t slot_size;

  /* get default value? */
  nslots = (slots_per_chunk == 0) ? SLOTS_PER_CHUNK : slots_per_chunk;

  /* Minimum is pointer size */
  slot_size = (object_size < sizeof(void *)) ? (sizeof(void *)) : object_size;

  /* Make sure that the slot_size is a multiple of an int */
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
  
  init_free_list(nslots, slot_size, fp->next_free);
  return(fp);
}

/****=====================================================================****/
/* Xc_destroyItem - Remove Item allocation                                   */
/****=====================================================================****/
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

/****=====================================================================****/
/* Xc_new - Add extra memory chunk for Item allocation                       */
/****=====================================================================****/
void *Xc_NormalItemAlloc(handle)
ItemHandle *handle;
{
  register void **object;
  
  if(handle->next_free == NULL)		/* Allocate new chunk? */
    {
      void **chp= (void **) &(handle->chunk_chain);
      
      while(*chp != NULL) chp = *chp;
      *chp = Xc_malloc(handle->description,
			       handle->chunk_size + sizeof(void *));
      chp = *chp;
      if(chp == NULL) return(chp);
      *chp = NULL;
      
      handle->next_free = (void **)((char *)chp + sizeof(void *));
      init_free_list(handle->slots_in_chunk, handle->slot_size,
		     handle->next_free);
    }
  
  object = handle->next_free;
  handle->next_free = *object;
  
  return(object);
}

/****=====================================================================****/
/* local fonction for Item allocation initialization                         */
/****=====================================================================****/
static void init_free_list(nslots, size, slots)
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


/****=====================================================================****/
/**** DEBUGGING ALLOCATION ROUTINES                                       ****/
/****=====================================================================****/

#undef Xc_FATAL
#define Xc_FATAL(args) \
{ Xc_LogPrintf("FATAL ERROR [%s %d] - ", filename, linenumber); \
    Xc_LogPrintf args; putc('\n', Xc_log_file); Xc_exit(-1); }

#undef Xc_WARNING
#define Xc_WARNING(args) \
{ Xc_LogPrintf("WARNING [%s %d] - ", filename, linenumber); \
    Xc_LogPrintf args; putc('\n', Xc_log_file); }

#undef Xc_ERROR
#define Xc_ERROR(args) \
{ Xc_LogPrintf("ERROR [%s %d] - ", filename, linenumber); \
    Xc_LogPrintf args; putc('\n', Xc_log_file); }

#define DEBUGSIZE (8*(sizeof(char)))
#define ALIGNSIZE (sizeof(long) - (DEBUGSIZE >> 1))

/* ______Memory tracing structure______ */
typedef struct MemoryTrace_s {
  struct MemoryTrace_s *next;
  void *start;
  size_t size;
  char *description;
  char *filename;
  int linenumber;
  unsigned long pattern;
} MemoryTrace;

static MemoryTrace *firstTrace = NULL;
static ItemHandle *debugItem = NULL;
static long debugReferences = 0;
static int CheckTracePattern ___PROTO((MemoryTrace *trace));
static void MakeTracePattern ___PROTO((MemoryTrace *trace));
static char *errorstring[4]= {"OK","UNDERRUN","OVERRUN","UNDERRUN & OVERRUN"};

/*-- Check pattern --*/
static void MakeTracePattern(trace)
MemoryTrace *trace;
{
  register unsigned char *pointer;
  unsigned long pattern;
  int i;

  pointer = (unsigned char *)(trace->start) - (DEBUGSIZE/2);
  trace->pattern = pattern = (rand() & 0xffff)|((rand()<<16) & 0xffff0000);
  for(i=0; i < DEBUGSIZE/2; i++)
    {
      register unsigned char key;

      key = pattern & 0xff;
      pattern >>= 8;
      pointer[i] = key;
      pointer[trace->size + i + (DEBUGSIZE/2)] = key ^ 0xff;
    }
}
  
/*--- Check trace patterns ---*/
static int CheckTracePattern(trace)
MemoryTrace *trace;
{
  int i;
  unsigned long start, end;
  unsigned char *ptr;
  
  ptr = (unsigned char *)(trace->start) - (DEBUGSIZE/2);
  start = end = 0;
  for(i = (DEBUGSIZE/2) - 1; i >= 0; i--)
    {
      start <<= 8;
      start |= ptr[i];
      end <<= 8;
      end |= ptr[trace->size + (DEBUGSIZE/2) + i] ^ 0xff;
      Xc_TRACE(("%lx %lx", start, end));
    }

  Xc_TRACE(("%lx %lx %lx", trace->pattern, start, end));
  if((start != trace->pattern) && (end != trace->pattern))
    return 3;
  else if(start != trace->pattern)
    return 1;
  else if(end != trace->pattern)
    return 2;
  return 0;
}

/****=====================================================================****/
/* Xc_mdump - List info for all allocated blocks                             */
/****=====================================================================****/
void Xc_TraceMemDump(filename, linenumber)
char *filename;
int linenumber;
{
  register MemoryTrace *trace;

  Xc_LogPrintf("\nMEMORY-DUMP [%s %d]\n", filename, linenumber);
  if((trace = firstTrace) == NULL)
    Xc_LogPrintf("no memory allocated.");
  else
    {
      Xc_LogPrintf("+-Start----+-Size-----+-Informations-----------------------------------------+\n");
      while(trace != NULL)
        {
          Xc_LogPrintf("| %08lx | %8lx | `%s' %s  @[%s %d]\n",
                       trace->start, trace->size, trace->description,
		       errorstring[CheckTracePattern(trace)],
		       trace->filename, trace->linenumber);
          trace = trace->next;
        }
      Xc_LogPrintf("+----------+----------+------------------------------------------------------+\n");
    }
  putc('\n', Xc_log_file);
}

/****=====================================================================****/
/* Xc_mcheck - Check all allocated blocks for corruption                     */
/****=====================================================================****/
void Xc_TraceMemCheck(filename, linenumber)
char *filename;
int linenumber;
{
  register MemoryTrace *trace;

  trace = firstTrace;
  while(trace != NULL)
    {
      int i = CheckTracePattern(trace);

      if(i != 0)
	Xc_LogPrintf("MEMORY-CHECK [%s %d] - `%s' %s  @[%s %d]\n",
		     filename, linenumber, trace->description,
		     errorstring[i], trace->filename, trace->linenumber);
      trace = trace->next;
    }
}

/****=====================================================================****/
/* Xc_malloc - malloc substitution                                           */
/****=====================================================================****/
void *Xc_TraceMemAlloc(description, size, filename, linenumber)
char *description;
size_t size;
char *filename;
int linenumber;
{
  register unsigned char *pointer;
  MemoryTrace *trace;

  if(debugItem == NULL)
    debugItem = Xc_createItem("MemoryDebugItem", 64, sizeof(MemoryTrace));

  if(!size)
    Xc_WARNING(("Allocating '%s' with NULL size", description));

  if((pointer = (unsigned char *)malloc(size + DEBUGSIZE + ALIGNSIZE)) == NULL)
    {
      Xc_ERROR(("Out of memory for '%s', size %ld",
		description, size - DEBUGSIZE - ALIGNSIZE));
      return pointer;
    }

  /*-- New trace --*/
  trace = Xc_new(debugItem);

  debugReferences++;
  trace->next = firstTrace;
  firstTrace = trace;
  trace->start = (char *)pointer + (DEBUGSIZE/2) + ALIGNSIZE;
  trace->size = size;
  trace->description = description;
  trace->filename = filename;
  trace->linenumber = linenumber;

  MakeTracePattern(trace);

  return(trace->start);
}


/****=====================================================================****/
/* Xc_free - free substitution                                               */
/****=====================================================================****/
void Xc_TraceMemFree(pointer, filename, linenumber)
void *pointer;
char *filename;
int linenumber;
{
  int i;
  MemoryTrace *trace, **last;

  Xc_ASSERT(debugReferences > 0);

  if(pointer == NULL)
    {
      Xc_WARNING(("Attempting to free a NULL pointer"));
      return;
    }

  last = &firstTrace;
  while(1)
    {
      if(*last == NULL)
	{
	  Xc_WARNING(("Attempting to free an unknown pointer (Double free?)"));
	  return;
	}
      if((*last)->start == pointer) break;
      last = &((*last)->next);
    }
  
  trace = *last;
  i = CheckTracePattern(trace);
  if(i != 0)
    Xc_WARNING(("freed `%s' %s  @[%s %d]",
		trace->description, errorstring[i],
		trace->filename, trace->linenumber));
  
  *last = trace->next;
  free((char *)(trace->start) - (DEBUGSIZE/2) - ALIGNSIZE);
  Xc_delete(debugItem, trace);
  if(--debugReferences == 0)
    {
      Xc_destroyItem(debugItem);
      debugItem = NULL;
    }
}

/****=====================================================================****/
/* Xc_realloc - realloc substitution                                         */
/****=====================================================================****/
void *Xc_TraceMemRealloc(description, ptr, size, filename, linenumber)
char *description;
void *ptr;
size_t size;
char *filename;
int linenumber;
{
  if(size == 0)
    {
      Xc_TraceMemFree(ptr, filename, linenumber);
      return NULL;
    }
  else if(ptr == NULL)
    {
      return(Xc_TraceMemAlloc(description, size, filename, linenumber));
    }
  else
    {
      MemoryTrace *trace, **last;
      int i;
      char *pointer;
      
      last = &firstTrace;
      while(1)
	{
	  if(*last == NULL)
	    {
	      Xc_WARNING(("Attempt to realloc an unknown pointer"));
	      return NULL;
	    }
	  if((*last)->start == ptr) break;
	  last = &((*last)->next);
	}
  
      trace = *last;
      i = CheckTracePattern(trace);
      if(i != 0)
	Xc_WARNING(("realloc `%s' %s  @[%s %d]",
		    trace->description, errorstring[i],
		    trace->filename, trace->linenumber));

      pointer = realloc((char *)ptr - (DEBUGSIZE/2) - ALIGNSIZE,
			size + DEBUGSIZE + ALIGNSIZE);
      if(pointer == NULL)
	{
	  Xc_ERROR(("Cannot grow '%s' from %ld to %ld bytes", description,
		    trace->size, size));
	  return pointer;
	}

      pointer += (DEBUGSIZE/2) + ALIGNSIZE;
      trace->start = pointer;
      trace->size = size;
      trace->description = description;
      trace->filename = filename;
      trace->linenumber = linenumber;
      MakeTracePattern(trace);
      return pointer;
    }
}

/****=====================================================================****/
/* Xc_createItem - Initialization for Item allocation                        */
/****=====================================================================****/
ItemHandle *Xc_TraceItemInit(description, slots_per_chunk, object_size, filename, linenumber)
char *description;
size_t slots_per_chunk;
size_t object_size;
char *filename;
int linenumber;
{
  register ItemHandle *fp;
  size_t nslots;
  size_t chunk_size;
  size_t slot_size;

  /* get default value? */
  nslots = (slots_per_chunk == 0) ? SLOTS_PER_CHUNK : slots_per_chunk;

  /* Minimum is pointer size */
  slot_size = (object_size < sizeof(void *)) ? (sizeof(void *)) : object_size;

  /* Make sure that the slot_size is a multiple of an int */
  slot_size = (slot_size - 1 + sizeof(int)) & -(sizeof(int));

  chunk_size = nslots * slot_size;

  fp = Xc_TraceMemAlloc(description, chunk_size + sizeof(ItemHandle),
			filename, linenumber);
  if(fp == NULL) return(fp);

  fp->description = description;
  fp->chunk_chain = NULL;
  fp->next_free = (void **)((char *)fp + sizeof(ItemHandle));
  fp->slots_in_chunk = nslots;
  fp->slot_size = slot_size;
  fp->chunk_size = chunk_size;
  
  init_free_list(nslots, slot_size, fp->next_free);
  return(fp);
}

/****=====================================================================****/
/* Xc_destroyItem - Remove Item allocation                                   */
/****=====================================================================****/
void Xc_TraceItemDestroy(handle, filename, linenumber)
ItemHandle *handle;
char *filename;
int linenumber;
{
  register void **chp, **next_chunk;
  
  if(handle == NULL)
    {
      Xc_WARNING(("Trying to free items from a null handle"));
      return;
    }
  chp = handle->chunk_chain;
  Xc_TraceMemFree(handle, filename, linenumber);
  
  while(chp != NULL)
    {
      next_chunk = *chp;
      Xc_TraceMemFree(chp, filename, linenumber);
      chp = next_chunk;
    }
}

/****=====================================================================****/
/* Xc_new - Add extra memory chunk for Item allocation                       */
/****=====================================================================****/
void *Xc_TraceItemAlloc(handle, filename, linenumber)
ItemHandle *handle;
char *filename;
int linenumber;
{
  register void **object;
  
  if(handle->next_free == NULL)		/* Allocate new chunk? */
    {
      void **chp= (void **) &(handle->chunk_chain);
      
      while(*chp != NULL) chp = *chp;
      *chp = Xc_TraceMemAlloc(handle->description,
			       handle->chunk_size + sizeof(void *),
			      filename, linenumber);
      chp = *chp;
      if(chp == NULL) return(chp);
      *chp = NULL;
      
      handle->next_free = (void **)((char *)chp + sizeof(void *));
      init_free_list(handle->slots_in_chunk, handle->slot_size,
		     handle->next_free);
    }
  
  object = handle->next_free;
  handle->next_free = *object;
  
  return(object);
}

/****=====================================================================****/
/* Xc_new - Add extra memory chunk for Item allocation                       */
/****=====================================================================****/
void Xc_TraceItemFree(handle, pointer, filename, linenumber)
ItemHandle *handle;
void *pointer;
char *filename;
int linenumber;
{
  *((void **)pointer) = (handle)->next_free;
  (handle)->next_free = ((void **)pointer);
}


/* ----------------------------------------------------------------- ** 
** Xc_TraceStrDup - strdup                                           ** 
** ----------------------------------------------------------------- */
char *Xc_TraceStrDup(desc, string, filename, linenumber)
char *desc;
char *string;
char *filename;
int linenumber;
{
  int len;
  char *ptr;

  len = strlen(string) + 1;
  ptr = Xc_TraceMemAlloc(desc, len, filename, linenumber);
  if(ptr != NULL) memcpy(ptr, string, len);
  return ptr;
}

