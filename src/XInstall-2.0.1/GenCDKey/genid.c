/*
** genid.c for XInstall in GenCDKey/
** generate the `.id ' file for cd-rom protection.
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
** Started on  Mon Jun 30 14:06:15 1997 Emmanuel Paris
** Last update Tue Jan 27 02:09:47 1998 Emmanuel Paris
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>

#define SEED_BEFORE 24225
#define SEED_AFTER 670892 
#define SEED_IDINFO 682824 

static char *fileid = "id_";

int main(argc, argv)
int argc;
char **argv;
{
 FILE *file;
 struct utimbuf times;
 int  value;
 char id[7];

 if (argc == 2 && !strncmp(argv[1], "-h", 2))
 {
  printf("Usage: %s\n  Create a `.id ' file in current directory. Do not read it !.\n  This file contains usefull informations for generating end keys.\n", argv[0]);
  return 1;
 }

 if (getuid() != 0 && geteuid() != 0)
 {
  printf("You must be root to execute this process.\n");
  return 1;
 }

 file = fopen(fileid, "wb");
 if (!file)
 {
  perror("Can not write file .id .");
  return 1;
 }
 
 value = SEED_IDINFO;
 id[0] = 'I';
 id[1] = 'D';
 id[2] = '-';
 id[3] = (value & 0xff000000) >> 24;
 id[4] = (value & 0xff0000) >> 16;
 id[5] = (value & 0xff00) >> 8;
 id[6] = (value & 0xff);
 fwrite(id, 7, 1, file);
 
 fclose(file);
 
 if (chown(fileid, (uid_t)0, (gid_t)0) < 0)
 {
  perror("Can not chown file .id .");
  return 1;
 }

 if (chmod(fileid, 0666) < 0)
 {
  perror("Can not chmod file .id .");
  return 1;
 }

 times.modtime = time((time_t *)NULL);
 times.actime = time((time_t *)NULL);
/*times.actime = times.modtime + (((SEED_BEFORE ^ SEED_IDINFO)+7) ^ SEED_AFTER);*/

 if (utime(fileid, &times) < 0)
 {
  perror("Can not utime file .id .");
  return 1;
 }
 
 printf("Ok, file %s writted. Do not read or change it !\n", fileid);
 return 0;
}
