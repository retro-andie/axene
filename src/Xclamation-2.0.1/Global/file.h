/*
** file.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Global/
** Header file for file functions
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
** Last update Thu Jan  6 16:47:14 2000 Emmanuel Paris
*/

#ifndef __XC_FILE_H__
#define __XC_FILE_H__

/* ------------------------------------------------------------------- */

#if defined(___mips) || defined(___alpha)
# define SYSDIR
#endif

#if !defined(___mips)
# define HAVE_GETCWD
#endif

#if defined(___i386_linux) || defined(___iris4d) || defined(___hpux10) || \
defined(___hpux9) || defined(___rs6000) || defined(___WIN32) || \
defined(___sparc_linux)
# define DIRENT
#endif

/* ------------------------------------------------------------------- */

#ifdef __GNUC__
#include "unistd.h"
#endif

#include <sys/stat.h>

#if defined(HAVE_FCNTL_H) || defined(_POSIX_VERSION)
# include <fcntl.h>
#else
# include <sys/file.h>
#endif

#if defined(DIRENT) || defined(_POSIX_VERSION)
# include <dirent.h>
# define NLENGTH(direct) (strlen((direct)->d_name))
#else			/* not (DIRENT or _POSIX_VERSION) */
# define dirent direct
# define NLENGTH(direct) ((direct)->d_namlen)
# ifdef SYSNDIR
#  include <sys/ndir.h>
# endif			/* SYSNDIR */
# ifdef SYSDIR
#  include <sys/dir.h>
# endif			/* SYSDIR */
# ifdef NDIR
#  include <ndir.h>
# endif			/* NDIR */
#endif			/* DIRENT or _POSIX_VERSION */

#if !defined(_POSIX_VERSION) && !defined(HAVE_GETCWD)
char *getwd ();
#define getcwd(buf, max) getwd (buf)
#else
char *getcwd ();
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
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

#if defined (_POSIX_VERSION)
/* POSIX does not require that the d_ino field be present, and some
   systems do not provide it. */
#define REAL_DIR_ENTRY(dp) 0
#else
#define REAL_DIR_ENTRY(dp) ((dp)->d_ino == 0)
#endif

#if defined(___sco386) || defined(___sco386sV) || defined(___ncr_svr4) ||\
defined(___i386_unixware)
#include <fcntl.h>
#include <dirent.h>
#include <sys/dirent.h>
#undef NLENGTH
#define NLENGTH(direct) (strlen((direct)->d_name))
#endif

#if defined(___freebsd) || defined(___NetBSD) || defined(___alpha)
#include <sys/param.h>
#include <sys/mount.h>
#elif defined(___rs6000) || defined(___rs6000ppc)
#include <sys/statfs.h>
#elif defined(___sun5) || defined(___sun5_x86)
#include <sys/statvfs.h>
#define statfs statvfs
#elif defined(___sco386) || defined(___sco386sV)
#include <sys/statfs.h>
#define f_bavail f_bfree
#define f_bsize f_bsize / 2
#define statfs(ddir,buf) statfs((ddir), (buf), sizeof(struct statfs), 0)
#else
#include <sys/vfs.h>
#endif


#ifndef S_IRUSR
# define S_IRUSR S_IREAD
#endif 

#ifndef S_IWUSR 
# define S_IWUSR S_IWRITE
#endif

#ifndef S_IXUSR
# define S_IXUSR S_IEXEC
#endif

#ifndef O_NONBLOCK
# define O_NONBLOCK O_NDELAY
#endif

/* def NO_FILE_FUNC if you don't want to link with filename.o & fileutils.o */
#ifndef NO_FILE_FUNC

/*--- Backup level structures and variables ---*/
enum backup_type_e {
  XcBF_INIT, XcBF_NONE, XcBF_SIMPLE, XcBF_NUMBERED_EXISTING, XcBF_NUMBERED
};

extern enum backup_type_e backup_type;
extern char *simple_backup_suffix;


/*--- Filename prototypes ---*/
char *MakeAbsoluteFile ___PROTO((char *basefile, char *file));
char *MakeRelativeFile ___PROTO((char *basefile, char *file));
char *StripFilename ___PROTO((char *file, boolean dup_flag));
char *GetBasename ___PROTO((char *file, boolean dup_flag));
char *GetDirname ___PROTO((char *file, boolean dup_flag));
char *GetFullname ___PROTO((char *path, char *file, boolean realloc_flag));
char *UntranslateFilename ___PROTO((char *subdir, char *filename));
char *TranslateFilename ___PROTO((char *subdir, char *filename));
char *SearchFile ___PROTO((char *filename));
			  
/*--- File prototypes ---*/
char *GetWorkingDirectory ___PROTO((void));
boolean IsFileExist ___PROTO((char *filename));
boolean IsFileRegular ___PROTO((char *filename));
boolean IsFileExecutable ___PROTO((char *filename));
boolean IsDirExist ___PROTO((char *filename));
boolean IsDirWritable ___PROTO((char *filename));
boolean IncFileBackup ___PROTO((char *filename));
boolean DecFileBackup ___PROTO((char *filename));
int PipeOpen ___PROTO((char *path, char **argv, int *inp, int *outp));

#endif

#endif /* !__XC_FILE_H__ */


