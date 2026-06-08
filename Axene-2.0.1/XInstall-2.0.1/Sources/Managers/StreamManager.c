/*
** StreamManager.c for XInstall in Managers/
** Xc Stream management.
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
** Started on  Fri Jul  4 18:18:27 1997 Emmanuel Paris
** Last update Tue Jan 13 14:03:45 1998 Robin Castanier
*/

#include "StreamManager.h"

#ifdef XC_STREAM

static int id = 0;
static MemStream *stack = NULL;

MemStream *Xc_StreamNewMemStream(data, size)
char *data;
int size;
{
 MemStream *stream;

 stream = (MemStream *)Xc_malloc("stream", sizeof(MemStream));
 sprintf(stream->id, "*id%04d", id++);
 stream->data = data;
 stream->size = size;
 stream->next = stack;
 stack = stream;
 
 return stream;
}

void Xc_StreamDeleteMemStream(stream)
MemStream *stream;
{
 MemStream *str;

 Xc_ASSERT((stream != NULL));
 
 str = stack;
 if (str == stream)
 {
  stack = stream->next;
  Xc_free(stream);
  return;
 }
 
 while(str && str->next != stream)
  str = str->next;

 if (str)
 {
  str->next = stream->next;
  Xc_free(stream);
 }
 else
 {
  Xc_WARNING(("Internal Stream not found !"));
 }
}


Xc_FILE *Xc_fopen(filename, mode)
char *filename;
char *mode;
{
 Xc_FILE *file;
 MemStream *stream, *str = NULL;

 Xc_ASSERT((filename != NULL));

 stream = (MemStream *)filename;

 str = stack;
 while(str && str != stream)
  str = str->next;

 if (str)
 {
  file = (Xc_FILE *)Xc_malloc("mf", sizeof(Xc_FILE));
  file->type = SM_MEMORY_FILE;
  file->memory.stream = stream;
  file->memory.mode = mode;
  file->memory.read_ptr = stream->data;
  file->memory.remain_size = stream->size;
 }
 else
 {
  file = (Xc_FILE *)Xc_malloc("nf", sizeof(Xc_FILE));
  file->type = SM_NORMAL_FILE;
  file->normal.file = fopen(filename, mode);
 }
 return file;
}

int Xc_fclose(file)
Xc_FILE *file;
{
 if (file->type == SM_NORMAL_FILE)
 {
  FILE *flux;
  
  flux = file->normal.file;
  Xc_free(file);
  return fclose(flux);
 }
 else
 {
  Xc_free(file);
  return 0;
 }
}

int Xc_fread(ptr, size, nmemb, file)
void *ptr;
size_t size;
size_t nmemb;
Xc_FILE *file;
{
 char	*sptr;
 int	remain, ssize;

 Xc_ASSERT((ptr != NULL && file != NULL && nmemb > 0 && size > 0));

 if (file->type == SM_NORMAL_FILE)
  return fread(ptr, size, nmemb, file->normal.file);
 
 sptr = file->memory.read_ptr;
 remain = file->memory.remain_size;
 
 if ((int)(size * nmemb) < remain)
 {
  memcpy(ptr, sptr, size * nmemb);
  sptr += size * nmemb;
  file->memory.read_ptr = sptr;
  file->memory.remain_size -= size * nmemb;
  return nmemb;
 }

 if ((int)size < remain)
  return 0;

 ssize = remain / size;
 memcpy(ptr, sptr, size * ssize);
 sptr += size * ssize;
 file->memory.read_ptr = sptr;
 file->memory.remain_size -= size * ssize;
 return ssize;
}

int Xc_feof(file)
Xc_FILE *file;
{
 if (file->type == SM_NORMAL_FILE)
  return feof(file->normal.file);
 else
  return (file->memory.remain_size != 0) ? 0 : 1;
}

int Xc_fseek(file, offset, whence)
Xc_FILE *file;
long offset;
int whence;
{
 MemStream *stream;
 int	ret;

 if (file->type == SM_NORMAL_FILE)
  return fseek(file->normal.file, offset, whence);
 
 stream = file->memory.stream;
 switch(whence)
 {
 case SEEK_SET:
  ret = offset;
  break;
 case SEEK_CUR:
  ret = stream->size - file->memory.remain_size + offset;
  break;
 case SEEK_END:
 default:
  ret = stream->size + offset;
  break;
 }

 if (ret < 0) ret = 0;
 if (ret > stream->size) ret = stream->size;
 
 file->memory.remain_size = stream->size - ret;
 file->memory.read_ptr = stream->data + ret;
 
 return ret;
}

long Xc_ftell(file)
Xc_FILE *file;
{
 if (file->type == SM_NORMAL_FILE)
  return ftell(file->normal.file);
 
 return (long)(file->memory.stream->size - file->memory.remain_size);
}

int Xc_fgetc(file)
Xc_FILE *file;
{
 int ret;
 
 if (file->type == SM_NORMAL_FILE)
  return fgetc(file->normal.file);
 
 if (file->memory.remain_size == 0)
  return EOF;
 
 ret = (int)*((unsigned char *)file->memory.read_ptr);
 file->memory.read_ptr ++;
 file->memory.remain_size --;
 
 return ret;
}

#endif
