/*
** TestIDFile.c for XInstall in TestCDKey/
** test `.id ' file. Compute with crypt seed.
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
** Started on  Mon Jun 30 15:59:09 1997 Emmanuel Paris
** Last update Sat Jun 12 18:22:37 1999 Emmanuel Paris
*/

#include "xcalibur.h"
#include "typeseed.h"
#include "file.h"
#include <sys/stat.h>

#ifndef WITH_ID_PROTECTION
# define ___noid
#endif

#ifndef WITH_CD_PROTECTION
# define ___nocdrom
#endif

extern int seed;
extern char *Globprog_name;

#ifndef ___noid
static char fileid[5] = { 48, 5, 8, 24, 0};
#endif

boolean test_idfile()
{
#ifdef ___noid
 seed ^= 66513;

 return TRUE;
#else
#ifndef ___nocdrom
 struct statfs	infofs;
#endif
 struct stat buf;
 int value = 0, ret;
 char	*idfile;

 idfile = GetDirname(Globprog_name, TRUE);
 /* idfile = Xc_strdup("ff", "/cdrom/Live/AIX-4.1.3/bin");*/

 if (*idfile != '/') *idfile = '\0';
 idfile = (char *)Xc_realloc("idf", idfile, strlen(idfile) + 5);
 
 fileid[0] += '/' - 48;
 fileid[1] += 'i' - 5;
 fileid[2] += 'd' - 8;
 fileid[3] += '_' - 24;
 
 strcat(idfile, fileid);
 Xc_TRACE(("Test file %s", idfile));
 
 value = 682824;

 Xc_TRACE(("Value store in .id file: %d", value));
 
 seed ^= value;
 Xc_TRACE(("Seed value after step 1: %d", seed));

 ret = stat(idfile, &buf);
 Xc_free(idfile);
 if (ret) return FALSE;

 seed += buf.st_size;
 Xc_TRACE(("Seed value after step 2: %d", seed));
 /* if ((buf.st_mode & 0777) != 0666) return FALSE; */
 
 value = (buf.st_uid + buf.st_gid) * buf.st_ino;
 seed ^= value;
 Xc_TRACE(("Seed value after step 3: %d", seed));

#ifndef ___nocdrom
 value = 456123;

 idfile = GetDirname(Globprog_name, TRUE);
 /* idfile = Xc_strdup("ff", "/cdrom/Live/AIX-4.1.3/bin");*/
 
 if (*idfile != '/') *idfile = '\0';
 idfile = (char *)Xc_realloc("idf", idfile, strlen(idfile) + 2);
 strcat(idfile, "/");

 Xc_TRACE(("Test mount point: %s", idfile));
 if (!statfs(idfile, &infofs))
 {
  value += infofs.f_bsize;
  value *= infofs.f_bavail;
 }
 Xc_TRACE(("value: %d (should be 0)", value));
 seed ^= value;
 Xc_free(idfile);
#endif

 value = buf.st_atime - buf.st_mtime;
 value = 2396 ^  Xc_MERGE_CRYPT_SEED;

 Xc_TRACE(("delta time value: %d (should be 2396)", value));
 seed ^= value;
 
 return TRUE;
#endif
}

