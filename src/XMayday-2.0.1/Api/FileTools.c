/*
** FileTools.c for XMayday in Api/
** FileTools.c
**
** Copyright (C) 1996-2000 Axene.
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
** Started on  Sometime during     1996 One of the authors
** Last update Sun Jul 13 15:59:22 1997 One of the authors
*/

#include "XMayday.h"

#include <sys/stat.h>

#if defined(HAVE_FCNTL_H) || defined(_POSIX_VERSION)
# include <fcntl.h>
#else
# include <sys/file.h>
#endif

#if !defined(_POSIX_VERSION) && !defined(HAVE_GETCWD)
char *getwd ();
#define getcwd(buf, max) getwd (buf)
#else
char *getcwd ();
#endif

/* Non-POSIX BSD systems might have gcc's limits.h, which doesn't define
   PATH_MAX but might cause redefinition warnings when sys/param.h is
   later included (as on MORE/BSD 4.3).  */
#if defined(_POSIX_VERSION) || (defined(HAVE_LIMITS_H) && !defined(__GNUC__))
#include <limits.h>
#endif

#ifndef _POSIX_PATH_MAX
#define _POSIX_PATH_MAX 255
#endif

/* Don't include sys/param.h if it already has been.  */
#if !defined(PATH_MAX) && !defined(MAXPATHLEN)
#include <sys/param.h>
#endif

#if !defined(PATH_MAX) && defined(MAXPATHLEN)
#define PATH_MAX MAXPATHLEN
#endif

#ifndef PATH_MAX
#define PATH_MAX _POSIX_PATH_MAX
#endif

/* ----------------------------------------------------------------- ** 
** GetWorkingDirectory - `getcwd'                                    ** 
** ----------------------------------------------------------------- */
char *GetWorkingDirectory()
{
 char *cwd;
 char *ret;
 unsigned int path_max;

 Xc_TRACE(("GetWorkingDirectory"));

 errno = 0;
 path_max = (unsigned int) PATH_MAX;
 path_max += 2;			/* The getcwd docs say to do this. */

 if((cwd = (char *)malloc(path_max)) == NULL) return NULL;

 errno = 0;
 while(((ret = getcwd(cwd, path_max)) == NULL) && (errno == ERANGE))
 {
  char *ptr;

  path_max += 32;
  if((ptr = (char *)realloc(cwd, path_max)) == NULL)
  {
   free(cwd);
   return NULL;
  }
  cwd = ptr;
  errno = 0;
 }

 if(ret == NULL)
 {
  int save_errno = errno;

  free(cwd);
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

 Xc_TRACE(("IsFileExist(%s)", filename));

 if((fd = open(filename, O_RDONLY, 0)) < 0) return FALSE;
 close(fd);
 Xc_TRACE(("file exist!"));
 return TRUE;
}

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
  
 Xc_TRACE(("IsFileExecutable(%s)", filename));
  
 if (stat(filename, &buf) == -1)
  return FALSE;

 if ( (buf.st_mode & FILE_TEST) != FILE_TEST)
  return FALSE;
  
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** SearchFile - search file in all $PATH directories                 ** 
** ----------------------------------------------------------------- */
char *SearchFile(file)
char *file;
{
 char *found;
 char *spath, *path, *home;
 int i, j;
  
 if (IsFileExecutable(file))
 {
  if (*file == '/')
   return (char *)strdup(file);
  path = GetWorkingDirectory();
  found = (char *)malloc(strlen(file) + strlen(path) + 2);
  sprintf(found, "%s/%s", path, file);
  free(path);
  return found;
 }
  
 if ((spath = getenv("PATH")) == NULL)
  return NULL;
  
 path = (char *)strdup(spath);
 i = j = 0;
 while(path[i])
 {
  if (path[i] == ':')
  {
   path[i] = '\0';
   if (path[j] == '~')
   {
    if ((home = getenv("HOME")) == NULL)
    {
     j = i + 1;
     continue;
    }
    found = (char *)malloc(strlen(file) + strlen(home) +
			   strlen(path + j) + 2);
    sprintf(found, "%s%s/%s", home, path + j + 1, file);
   }
   else
   {
    found = (char *)malloc(strlen(file) + strlen(path + j) + 2);
    sprintf(found, "%s/%s", path + j, file);
   }
   if (IsFileExecutable(found))
   {
    free(path);
    return found;
   }
   free(found);
   j = i+1;
  }
  i++;
 }
  
 if (path[j] == '~')
 {
  if ((home = getenv("HOME")) == NULL)
   return NULL;
    
  found = (char *)malloc(strlen(file) + strlen(home) + strlen(path + j) + 2);
  sprintf(found, "%s%s/%s", home, path + j + 1, file);
 }
 else
 {
  found = (char *)malloc(strlen(file) + strlen(path + j) + 2);
  sprintf(found, "%s/%s", path + j, file);
 }
 if (IsFileExecutable(found))
 {
  free(path);
  return found;
 }
 free(found);
  
 free(path);
 return NULL;
}
