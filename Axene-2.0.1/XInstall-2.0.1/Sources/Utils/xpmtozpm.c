/*
** xpmtozpm.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Utils/
** program that convert XPM file to ZPM file (compressed XPM)
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
** Started on  Mon Jul 17 15:05:00 1995 Antoine Buat
** Last update Wed Aug  5 18:14:37 1998 Emmanuel Paris
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NULL
#define NULL (void *)0
#endif

int main(argc, argv)
int argc;
char **argv;
{
 char *filename, *outfilename, *statstr, str[400];
 char *xbuffer, *zbuffer, c;
 FILE *input;
 FILE *output;
 int  i, j, k, l, rsize = 0, zsize = 0;
 int  linecnt = 0;
 int  *linesize;
  
 if (argc == 1 || argc > 2)
 {
  printf("Usage:  %s [xpm_file]\n", argv[0]);
  return -1;
 }
  
 filename = strrchr(argv[1], '.');
 if (filename == NULL || 
     ( filename[1] == 'x' && filename[1] == 'p' && filename[1] == 'm'))
 {
  printf("%s must have .xpm extensions.\n", argv[1]);
  return -1;
 }
  
 outfilename = (char *)malloc(strlen(argv[1]) + 1);
 statstr = (char *)malloc(strlen(argv[1]) + 1);
 for(i = 0; i < (filename - argv[1]); i++)
 {
  outfilename[i] = argv[1][i];
 }
 outfilename[i] = '.';
 outfilename[i+1] = 'z';
 outfilename[i+2] = 'p';
 outfilename[i+3] = 'm';
 outfilename[i+4] = '\0';
  
 strcpy(statstr, argv[1]);
 for(i = 0; i < (int)strlen(statstr); i++)
 {
  if (statstr[i] == '.')
   statstr[i] = '_';
 }
  
 input = fopen(argv[1], "rb");
  
 while(fgets(str, 400, input))
 {
  i = 0;
  while(str[i] && str[i] != 34 && str[i] != '/')
   i++;
    
  if (str[i] == '/') continue;
  
  if (str[i])
  {
   linecnt++;
   rsize++;
   i++;
   while(str[i] && str[i] != 34)
   {
    rsize++;
    i++;
   }
  }
 }
  
 rewind(input);
  
 if (rsize > 65535)
 {
  printf("XPM size too high. Cannot compress.\n");
  return -1;
 }

 if (linecnt >= 5*256)
 {
  printf("XPM height too high. Cannot compress.\n");
  return -1;
 }
 if (linecnt < 5)
 {
  printf("XPM height too low. Cannot compress.\n");
  return -1;
 }

 linesize = (int *)malloc(linecnt * sizeof(int));
 xbuffer = (char *)malloc(rsize);
 zbuffer = (char *)malloc(rsize);
  
 j = l = 0;
 while(fgets(str, 400, input))
 {
  i = 0;
  while(str[i] && str[i] != 34 && str[i] != '/')
   i++;
  
  if (str[i] == '/') continue;
  
  if (str[i])
  {
   i++; k = 1;
   while(str[i] && str[i] != 34)
   {
    xbuffer[j] = str[i];
    j++; k++; i++;
   }
   linesize[l] = k;
   l ++;
  }
 }

 zsize = j;
 j = 0;
 
 for(i = 0; i < zsize; i++)
 {
  c = xbuffer[i];
  if ( i + 1 < zsize && xbuffer[i + 1] == c)
  {
   k = 2;
   i += 2;
   while(i < zsize && xbuffer[i] == c)
   {
    i++;
    k++;
   }
   while(k > 127)
   {
    zbuffer[j++] = 127 | 128;
    zbuffer[j++] = c;
    k -= 127;
   }
   zbuffer[j++] = k | 128;
   zbuffer[j++] = c;
   i--;
  }
  else
  {
   zbuffer[j++] = c;
  }
 }
 zsize = j;
  
 output = fopen(outfilename, "wb");
    
 fprintf(output, "/* ZPM */\nstatic char %s_1[] =\n{\n", statstr);
 fprintf(output, " 'z', 0x%02x, 0x%02x, ", (rsize >> 8) & 0xff,
	 rsize & 0xff);

 if (linecnt > 255)
  fprintf(output, "0x%02x, 0x%02x, ", (linecnt >> 8) & 0xff,
	  linecnt & 0xff);
 else
  fprintf(output, "0x%02x, ", linecnt & 0xff);

 j = linecnt - 2;
 k = linesize[j+1];
 while( linesize[j] == k) j--;
 k = 4;
 for(i = 0; i < j + 2; i++)
 {
  fprintf(output, "0x%02x", linesize[i] & 0xff);
  if ( i != j + 1)
   fprintf(output, ", ");
  k++;
  if (k > 10)
  {
   fprintf(output, "\n ");
   k = 0;
  }
 }
 fprintf(output, ", '\\0' };\n\nstatic char %s_2[] =\n{\n ", statstr);
  
 j = 0;
 for(i = 0; i < zsize; i++)
 {
  fprintf(output, "0x%02x", zbuffer[i] & 0xff);
  if ( i != zsize - 1)
   fprintf(output, ", ");
  j ++;
  if (j > 10)
  {
   fprintf(output, "\n ");
   j = 0;
  }
 }
  
 fprintf(output, " };\n\nstatic char * %s[] =\n{\n %s_1, %s_2\n};\n\n",
	 statstr, statstr, statstr);
 fclose(output);
  
 return 0;
}















