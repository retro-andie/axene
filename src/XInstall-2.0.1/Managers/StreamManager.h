/*
** StreamManager.h for XInstall in Managers/
** Encapsulate stream functions so that they accept memory buffer
** for input.
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
** Started on  Fri Jul  4 18:11:53 1997 Emmanuel Paris
** Last update Sat Jul  5 14:14:40 1997 Emmanuel Paris
*/

#ifndef __XC_STREAM_H__
#define __XC_STREAM_H__

#include "xcalibur.h"

#ifndef XC_STREAM

#define Xc_FILE FILE

#define Xc_fopen fopen
#define Xc_fclose fclose
#define Xc_fread fread
#define Xc_feof feof
#define Xc_fseek fseek
#define Xc_ftell ftell
#define Xc_fgetc fgetc

#else

#define SM_NORMAL_FILE 0
#define SM_MEMORY_FILE 1

typedef struct _MemStream
{
 char id[8];
 char *data;
 int  size;	
 struct _MemStream *next;
} MemStream;

typedef struct 
{
 int type;
 FILE *file;
} normal_file_t;

typedef struct
{
 int type;
 MemStream *stream;
 char	*mode;
 char	*read_ptr;
 int	remain_size;
} memory_file_t;

typedef union 
{
 int type;		/* SM_NORMAL_FILE or SM_MEMORY_FILE */
 normal_file_t	normal; 
 memory_file_t	memory;
} Xc_FILE;

extern MemStream *Xc_StreamNewMemStream ___PROTO((char *data, int size)); 
extern void Xc_StreamDeleteMemStream ___PROTO((MemStream *stream));

extern Xc_FILE *Xc_fopen ___PROTO((char *filename, char *mode));
extern int Xc_fclose ___PROTO((Xc_FILE *file));
extern int Xc_fread ___PROTO((void *ptr, size_t size, size_t nmemb, 
			      Xc_FILE *file));

extern int Xc_feof ___PROTO((Xc_FILE *file));
extern int Xc_fseek ___PROTO((Xc_FILE *file, long offset, int whence));
extern long Xc_ftell ___PROTO((Xc_FILE *file));
extern int Xc_fgetc ___PROTO((Xc_FILE *file));

#define Xc_NewStream Xc_StreamNewMemStream
#define Xc_DeleteStream Xc_StreamDeleteMemStream

#endif

#endif /* __XC_STREAM_H__ */




