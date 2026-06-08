/*
** stat.c for XInstall in GenCDKey/
** do a stat on input file and print status.
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
** Started on  Mon Jun 30 12:05:23 1997 Emmanuel Paris
** Last update Mon Jun 30 14:05:08 1997 Emmanuel Paris
*/

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

int main(argc, argv)
int argc;
char **argv;
{
 struct stat buf;
 int ret;

 if (argc != 2)
 {
  printf("Usage: %s filename\n", argv[0]);
  return 1;
 }

 ret = stat(argv[1], &buf);
 
 if (ret)
 {
  if (errno == ENOENT)
   printf("file %s doesn't exist\n", argv[1]);
  else
   printf("stat encounter an unknow error\n");
  return 1;
 }

 printf("stat on file %s:\n", argv[1]);
 printf("\tdev_t         st_dev     = %d\n", (int)buf.st_dev);
 printf("\tino_t         st_ino     = %d\n", (int)buf.st_ino);
 printf("\tumode_t       st_mode    = %o\n", (int)buf.st_mode);
 printf("\tnlink_t       st_nlink   = %d\n", (int)buf.st_nlink);
 printf("\tuid_t         st_uid     = %d\n", (int)buf.st_uid);
 printf("\tgid_t         st_gid     = %d\n", (int)buf.st_gid);
 printf("\tdev_t         st_rdev    = %d\n", (int)buf.st_rdev);
 printf("\toff_t         st_size    = %d\n", (int)buf.st_size);
 printf("\tunsigned long st_blksize = %ld\n", (long)buf.st_blksize);
 printf("\tunsigned long st_blocks  = %ld\n", (long)buf.st_blocks);
 printf("\ttime_t        st_atime   = %d\n", (int)buf.st_atime);
 printf("\ttime_t        st_mtime   = %d\n", (int)buf.st_mtime);
 printf("\ttime_t        st_ctime   = %d\n", (int)buf.st_ctime);

 return 0;
}
