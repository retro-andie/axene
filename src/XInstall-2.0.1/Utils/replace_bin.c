/*
** replace_bin.c for XInstall in Utils/
** programs to insert a binary file in obj
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
** Started on  Wed Aug 27 00:47:29 1997 Antoine Buat
** Last update Tue Jan 13 20:47:10 1998 Emmanuel Paris
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ___sun4
#include <unistd.h>
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef NULL
#define NULL	((void *)0)
#endif

char	buffer[10000];

int main(argc, argv)
int	argc;
char	**argv;
{
 FILE	*obj;
 FILE	*binary;
 int	ren1;
 char   *pattern;
 unsigned char	car1;
 long	pos;

 if (argc != 3)
 {
  printf("Usage %s <file.o> <binary>\n", argv[0]);
  return -1;
 }

 obj = fopen(argv[1], "r+b");
 binary = fopen(argv[2], "rb");
 fseek(obj, 0, SEEK_SET);
 fseek(binary, 0, SEEK_SET);

 pattern = argv[2] + strlen(argv[2]) - 1;
 while(pattern != argv[2] && *pattern != '/') pattern--;
 if (pattern != argv[2]) pattern++;

 /* find string argv[2] in file argv[1] */
 pos = 0;
 while (1)
 {
  ren1 = fgetc(obj);
  if (ren1 == EOF)
   break;
  car1 = (char)ren1;
  if (car1 == *pattern)
  {
   int	i;
/*printf("***%3lx\n", pos);*/
   i = 0;
   while (pattern[i] && (car1 == pattern[i]))
   {
    ren1 = fgetc(obj);
    car1 = (char)ren1;
    i++;
   }
   if (pattern[i] == '\0')
   {
    int num;
    printf("found at pos 0x%3lx", pos);
    fseek(obj, pos, SEEK_SET);
    num = 10000;
    while(num == 10000)
    {
     num = fread(buffer, 1, 10000, binary);
     fwrite(buffer, 1, num, obj);
    }
    break;
   }
   else
    fseek(obj, pos+1, SEEK_SET);
  }
  pos++;
 }
 fclose(obj);
 fclose(binary);
 return 0;
}

