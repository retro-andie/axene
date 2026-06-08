/*
** fileutils.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Global/
** File functions
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
** Started on  Mon Jan 16 11:10:17 1995 Stéphane Boisson
** Last update Tue Oct  6 19:03:02 1998 Antoine Buat
*/

#ifndef NHISTORY
#define NHISTORY
#endif
#ifndef NTRACE
#define NTRACE
#endif

#include <ctype.h>
#include <errno.h>
#include "xcalibur.h"
#include "file.h"

enum backup_type_e backup_type = XcBF_INIT;
char *simple_backup_suffix = "~";

static char *find_backup_filename ___PROTO((char *filename, int inc));


/* ----------------------------------------------------------------- ** 
** GetWorkingDirectory - `getcwd'                                    ** 
** ----------------------------------------------------------------- */
char *GetWorkingDirectory()
{
 char *cwd;
 char *ret;
 unsigned int path_max;
 
 Xc_HISTORY(("GetWorkingDirectory"));
 
 errno = 0;
 path_max = (unsigned int) PATH_MAX;
 path_max += 2;  /* The getcwd docs say to do this. */
 
 if((cwd = Xc_malloc("Working dir", path_max)) == NULL) return NULL;
 
 errno = 0;
 while(((ret = getcwd(cwd, path_max)) == NULL) && (errno == ERANGE))
 {
  char *ptr;
  
  path_max += 32;
  if((ptr = Xc_realloc("Working dir", cwd, path_max)) == NULL)
  {
   Xc_free(cwd);
   return NULL;
  }
  cwd = ptr;
  errno = 0;
 }
 
 if(ret == NULL)
 {
  int save_errno = errno;
  
  Xc_free(cwd);
  errno = save_errno;
  Xc_SYSERROR(("Get Current Working Directory"));
  return NULL;
 }
 Xc_TRACE(("CWD=%s", cwd));
 return cwd;
}


/* ----------------------------------------------------------------- ** 
** IsFileExist - Test if the file exist                              ** 
** ----------------------------------------------------------------- */
boolean IsFileExist(filename)
char *filename;
{
 int fd;
 
 Xc_HISTORY(("IsFileExist(%s)", filename));
 
 if((fd = open(filename, O_RDONLY, 0)) < 0) return FALSE;
 close(fd);
 Xc_TRACE(("file exist!"));
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** IsFileRegular - Test if the file exist and is a regular file      ** 
** ----------------------------------------------------------------- */
# define FILE_TEST (S_IFREG | S_IFLNK)
boolean IsFileRegular(filename)
char *filename;
{
 struct stat buf;
 
 Xc_HISTORY(("IsFileRegular(%s)", filename));
 
 if (stat(filename, &buf) == -1)
  return FALSE;
 
 if ( (buf.st_mode & FILE_TEST) == 0)
  return FALSE;
 
 return TRUE;
}
#undef FILE_TEST

/* ----------------------------------------------------------------- ** 
** IsFileExecutable - Test if the file can be execute                ** 
** ----------------------------------------------------------------- */
#ifndef S_IXUSR
# define FILE_TEST (S_IEXEC)
#else
# define FILE_TEST (S_IXUSR)
#endif
boolean IsFileExecutable(filename)
char *filename;
{
 struct stat buf;
 
 Xc_HISTORY(("IsFileExecutable(%s)", filename));
 
 if (stat(filename, &buf) == -1)
  return FALSE;
 
 if ( (buf.st_mode & FILE_TEST) != FILE_TEST)
  return FALSE;
 
 return TRUE;
}
#undef FILE_TEST

/* ----------------------------------------------------------------- ** 
** IsDirExist - Test if the directory exist                          ** 
** ----------------------------------------------------------------- */
#ifndef S_IXUSR
# define DIRECTORY_TEST (S_IFDIR | S_IEXEC)
#else
# define DIRECTORY_TEST (S_IFDIR | S_IXUSR)
#endif
boolean IsDirExist(dirname)
char *dirname;
{
 struct stat buf;
 
 Xc_HISTORY(("IsDirectoryExist(%s)", dirname));
 
 if (lstat(dirname, &buf) == -1)
  return FALSE;
 
 if ( (buf.st_mode & DIRECTORY_TEST) != DIRECTORY_TEST)
  return FALSE;
 
 return TRUE;
}
#undef DIRECTORY_TEST

/* ----------------------------------------------------------------- ** 
** IsDirWritable - Test if the directory exist and is writeable      ** 
** ----------------------------------------------------------------- */
#ifndef S_IWUSR
# define DIRECTORY_TEST (S_IFDIR | S_IWRITE | S_IEXEC)
#else
# define DIRECTORY_TEST (S_IFDIR | S_IWUSR | S_IXUSR)
#endif
boolean IsDirWritable(dirname)
char *dirname;
{
 struct stat buf;
 
 Xc_HISTORY(("IsDirectoryWritable(%s)", dirname));
 
 if (lstat(dirname, &buf) == -1)
  return FALSE;
 
 if ( (buf.st_mode & DIRECTORY_TEST) != DIRECTORY_TEST)
  return FALSE;
 
 return TRUE;
}
#undef DIRECTORY_TEST

/* ----------------------------------------------------------------- ** 
** IncFileBackup - Increment the backup level for the filename       ** 
** ----------------------------------------------------------------- */
boolean IncFileBackup(filename)
char *filename;
{
 register char *backup;
 
 Xc_HISTORY(("IncFileBackup(%s)", filename));
 
 if(backup_type == XcBF_INIT)
 {
  /*--- Init variables ---*/
  if((backup = getenv("SIMPLE_BACKUP_SUFFIX")) != NULL)
   simple_backup_suffix = backup;
  
  backup_type = XcBF_NUMBERED_EXISTING;
  if(((backup = getenv("VERSION_CONTROL")) != NULL) && (*backup != 0))
  {
   if(!strcmp(backup, "never"))
    backup_type = XcBF_NONE;
   else if(!strcmp(backup, "simple"))
    backup_type = XcBF_SIMPLE;
   else if(!strcmp(backup, "numbered") || !strcmp(backup, "t"))
    backup_type = XcBF_NUMBERED;
   else if(strcmp(backup, "nil") && strcmp(backup, "existing"))
    Xc_WARNING(("invalid version control type `%s'", backup));
  }
 }
 
 if(!IsFileExist(filename)) return TRUE;
 if(backup_type == XcBF_NONE) return TRUE;
 
 if((backup = find_backup_filename(filename, 1)) == NULL) return FALSE; 
 if(rename(filename, backup) < 0)
 {
  Xc_SYSERROR(("%s backup", filename));
  Xc_free(backup);
  return FALSE;
 }
 Xc_TRACE(("backup %s -> %s", filename, backup));
 
 Xc_free(backup);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** DecFileBackup - Decrement the backup level for the filename       ** 
** ----------------------------------------------------------------- */
boolean DecFileBackup(filename)
char *filename;
{
 register char *backup;
 
 Xc_HISTORY(("DecFileBackup(%s)", filename, 0));
 Xc_ASSERT(backup_type != XcBF_INIT);
 
 if(backup_type == XcBF_NONE) return TRUE;
 
 if((backup = find_backup_filename(filename, 0)) == NULL) return FALSE; 
 if(IsFileExist(backup) && (rename(backup, filename) < 0))
 {
  Xc_SYSERROR(("%s restore", filename));
  Xc_free(backup);
  return FALSE;
 }
 
 Xc_free(backup);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** find_backup_filename - Make backup filename                       ** 
** ----------------------------------------------------------------- */
static char *find_backup_filename(filename, inc)
char *filename;
int inc;
{
 Xc_TRACE(("find_backup_filename(%s)", filename));
 Xc_ASSERT(backup_type != XcBF_NONE);

 if(backup_type != XcBF_SIMPLE)
 {
  char *basename, *dirname;
  int backup_count;
  DIR *handle;
  
  basename = Xc_strconcat(GetBasename(filename, FALSE), ".~", NULL);
  if(basename == NULL) return FALSE;
  if((dirname = GetDirname(filename, TRUE)) == NULL)
  {
   Xc_free(basename);
   return NULL;
  }

  /*--- Get the highest backup level ---*/
  backup_count = 0;
  if((handle = opendir(dirname)) != 0)
  {
   struct dirent *entry;
   int base_len;

   Xc_TRACE(("basename: `%s'", basename));
   base_len = strlen(basename);
   while((entry = readdir(handle)) != 0)
    if(!REAL_DIR_ENTRY(entry) && ((int)NLENGTH(entry) > base_len) &&
       !memcmp(entry->d_name, basename, base_len) &&
       isdigit(entry->d_name[base_len]))
    {
     char *ptr;
     int version;
     
     Xc_TRACE(("backup file: `%s'", entry->d_name));
     
     version = 0;
     for(ptr = entry->d_name + base_len; isdigit(*ptr); ptr++)
      version = (version * 10) + *ptr - '0';
     
     if((*ptr == '~') && (ptr[1] == 0) && (version > backup_count))
      backup_count = version;
    }
   
   closedir(handle);
  }
  else
   Xc_SYSERROR((dirname));
  
  Xc_TRACE(("last backup version: %d", backup_count));
  Xc_free(basename);
  Xc_free(dirname);

  /*--- Make version ---*/
  if((backup_type == XcBF_NUMBERED) || 
     ((backup_type == XcBF_NUMBERED_EXISTING) && (backup_count > 0)))
  {
   if((backup_count + inc) == 0) return NULL; 
   basename = Xc_malloc("backup filename", strlen(filename) + 16);
   if(basename != NULL)
   {
    sprintf(basename, "%s.~%d~", filename, backup_count + inc);
    return basename;
   }
  }
 }

 Xc_TRACE(("Simple backup"));
 return Xc_strconcat(filename, simple_backup_suffix, NULL);
}


/* ----------------------------------------------------------------- ** 
** Xc_popen - XCalibur flavor popen                                  ** 
** ----------------------------------------------------------------- */
int PipeOpen(path, argv, inp, outp)
char *path;
char **argv;
int *inp;
int *outp;
{
 int pid, in_pfd[2], out_pfd[2];
 
 Xc_HISTORY(("PipeOpen(`%s', %s)", *argv,
	     inp? (outp? "IN & OUT" : "IN") : (outp? "OUT" : "NONE!")));
 Xc_ASSERT((inp != NULL) || (outp != NULL));
 
 /*--- Create pipes ---*/
 if((inp != NULL) && (pipe(in_pfd) < 0))
 {
  Xc_SYSERROR(("pipe"));
  return -1;
 }
 if((outp != NULL) && (pipe(out_pfd) < 0))
 {
  Xc_SYSERROR(("pipe"));
  if(inp != NULL)
  {
   close(in_pfd[0]);
   close(in_pfd[1]);
  }
  return -1;
 }

 /*--- Forking ---*/
 if((pid = fork()) < 0)
 {
  /*--- Fork error ---*/
  Xc_SYSERROR(("fork"));
  if(inp != NULL)
  {
   close(in_pfd[0]);
   close(in_pfd[1]);
  }
  if(outp != NULL)
  {
   close(out_pfd[0]);
   close(out_pfd[1]);
  }
  return -1;
 }
 else if(pid == 0)
 {
  /*--- Child proc ---*/

  if(inp != NULL)
  {
   close(in_pfd[0]);
   dup2(in_pfd[1], 1);		/* stdout */
   close(in_pfd[1]);
  }
  else
   close(1);

  if(outp != NULL)
  {
   close(out_pfd[1]);
   dup2(out_pfd[0], 0);		/* stdin */
   close(out_pfd[0]);
  }
  else
   close(0);

  execv(path, argv);
  _exit(127);
 }

 /*--- Parent child ---*/
 Xc_TRACE(("child pid=%d", pid));
 if(outp != NULL)
 {
  close(out_pfd[0]);
  *outp = out_pfd[1];
 }
 if(inp != NULL)
 {
  close(in_pfd[1]);
  *inp = in_pfd[0];
 }

 return pid;
}
