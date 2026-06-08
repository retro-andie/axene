/*
** memdebug.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Managers/
** Memory allocation debug version
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
** Started on  Tue Mar 21 13:08:13 1995 Stéphane Boisson
** Last update Mon Jan 12 13:35:18 1998 Robin Castanier
*/

#include "MemoryManagerP.h"
#include "memtrace.h"

#define DEBUGSIZE (8*(sizeof(char)))
#define ALIGNSIZE (sizeof(long) - (DEBUGSIZE >> 1))


/*--- Memory trace structure ---*/
typedef struct memory_trace_s memory_trace_t;
struct memory_trace_s 
{
 memory_trace_t *next;
 void *start;
 size_t size;
 char *description;
 char *filename;
 int linenumber;
 unsigned long pattern;
};


/*--- Local variables ---*/
static memory_trace_t *firstTrace = NULL;
static ItemHandle *debugItem = NULL;
static long debugReferences = 0;


/*--- Local prototypes ---*/
static int CheckTracePattern ___PROTO((memory_trace_t *trace));
static void MakeTracePattern ___PROTO((memory_trace_t *trace));
static char *errorstring[4]= {"OK","UNDERRUN","OVERRUN","UNDERRUN & OVERRUN"};


/* ----------------------------------------------------------------- ** 
** MakeTracePattern - Trace pattern creation                         ** 
** ----------------------------------------------------------------- */
static void MakeTracePattern(trace)
memory_trace_t *trace;
{
 register unsigned char *pointer;
 unsigned long pattern;
 int i;

 pointer = (unsigned char *)(trace->start) - (DEBUGSIZE/2);
 trace->pattern = pattern = (rand() & 0xffff)|((rand()<<16) & 0xffff0000);
 for(i=0; i < (int)(DEBUGSIZE/2); i++)
 {
  register unsigned char key;

  key = pattern & 0xff;
  pattern >>= 8;
  pointer[i] = key;
  pointer[trace->size + i + (DEBUGSIZE/2)] = key ^ 0xff;
 }
}


/* ----------------------------------------------------------------- ** 
** CheckTracePattern - Trace pattern checking                        ** 
** ----------------------------------------------------------------- */
static int CheckTracePattern(trace)
memory_trace_t *trace;
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


/* ----------------------------------------------------------------- ** 
** Xc_mdump - Display info about memoy allocation                    ** 
** ----------------------------------------------------------------- */
void Xc_TraceMemDump(filename, linenumber)
char *filename;
int linenumber;
{
 register memory_trace_t *trace;

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


/* ----------------------------------------------------------------- ** 
** Xc_mcheck - Checking memory allocation                            ** 
** ----------------------------------------------------------------- */
void Xc_TraceMemCheck(filename, linenumber)
char *filename;
int linenumber;
{
 register memory_trace_t *trace;

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


/* ----------------------------------------------------------------- ** 
** Xc_TraceMemAlloc - malloc                                         ** 
** ----------------------------------------------------------------- */
void *Xc_TraceMemAlloc(description, size, filename, linenumber)
char *description;
size_t size;
char *filename;
int linenumber;
{
 register unsigned char *pointer;
 memory_trace_t *trace;

 if(debugItem == NULL)
  debugItem = Xc_createItem("MemoryDebugItem", 64, sizeof(memory_trace_t));

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


/* ----------------------------------------------------------------- ** 
** Xc_TraceMemFree - free                                            ** 
** ----------------------------------------------------------------- */
void Xc_TraceMemFree(pointer, filename, linenumber)
void *pointer;
char *filename;
int linenumber;
{
 memory_trace_t *trace, **last;
 int i;

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


/* ----------------------------------------------------------------- ** 
** Xc_TraceMemeRealloc - realloc                                     ** 
** ----------------------------------------------------------------- */
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
  memory_trace_t *trace, **last;
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


/* ----------------------------------------------------------------- ** 
** Xc_TraceItemInit - Item handle creation                           ** 
** ----------------------------------------------------------------- */
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

 /*--- get default value? ---*/
 nslots = (slots_per_chunk == 0) ? SLOTS_PER_CHUNK : slots_per_chunk;

 /*--- Minimum is pointer size ---*/
 slot_size = (object_size < sizeof(void *)) ? (sizeof(void *)) : object_size;

 /*--- Make sure that the slot_size is a multiple of an int ---*/
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
  
 itemInitList(nslots, slot_size, fp->next_free);
 return(fp);
}


/* ----------------------------------------------------------------- ** 
** Xc_TraceItemDestroy - Item destroy                                ** 
** ----------------------------------------------------------------- */
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


/* ----------------------------------------------------------------- ** 
** Xc_TraceItemAlloc - Add memory to item                            ** 
** ----------------------------------------------------------------- */
void *Xc_TraceItemAlloc(handle, filename, linenumber)
ItemHandle *handle;
char *filename;
int linenumber;
{
 register void **object;
  
 if(handle->next_free == NULL)
 {
  void **chp= (void **) &(handle->chunk_chain);
      
  /*--- Allocate new chunk ---*/
  while(*chp != NULL) chp = *chp;
  *chp = Xc_TraceMemAlloc(handle->description,
			  handle->chunk_size + sizeof(void *),
			  filename, linenumber);
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
** Xc_TraceItemFree - Item free                                      ** 
** ----------------------------------------------------------------- */
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

