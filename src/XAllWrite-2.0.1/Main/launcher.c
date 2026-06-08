/*
** launcher.c for XAllWrite in Main/
** AxeneSoftware launcher. This program allows an Axene software to be
** attached to his shell.
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
** Started on  Tue Oct  6 14:18:43 1998 Emmanuel Paris
** Last update Tue Oct  6 16:38:55 1998 Emmanuel Paris
*/

#define NO_FILE_FUNC

#include "xcalibur.h"
#include "version.h"
#include "osname.h"
#include "file.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

static void usage();
static boolean IsFileExecutable ___PROTO((char *filename));
static void signal_handler ___PROTO((int sig));
static void (*def_handler[NSIG])();
static char *command_line;


static char *helpString[] = { "--help", "-help", "-h", NULL};
static char *versionString[] = { "--version", "-version", NULL};
static char copyrightDisplay[] = "Copyright 1997-98 Axene.";

int main(argc, argv)
int argc;
char **argv;
{
 pid_t father_pid;
 pid_t pid;
 int i, status;
 
 command_line = (char *)Xc_malloc("cmdl", 5000);
 *command_line = '\0';
 
 if (argc == 1)
 {
  if (IsFileExecutable("main"))
   strcpy(command_line, "main");
  else
  {
   usage(argv[0]);
   return 1;
  }
 }
 else
 {
  i = 0; while(helpString[i] && strcmp(helpString[i], argv[1])) i++;
  if (helpString[i]) 
  { 
   usage(argv[0]); 
   return 0;
  }
  
  i = 0; while(versionString[i] && strcmp(versionString[i], argv[1])) i++;
  if (versionString[i])
  {
#ifdef PL	      
   printf("%s version %d.%d.%d (%s), %s\n", argv[0],
	  MAJOR, MINOR, PL, Xc_SYSTEM_NAME, copyrightDisplay);
#else
   printf("%s version %d.%d (%s), %s\n", argv[0],
	  MAJOR, MINOR, Xc_SYSTEM_NAME, copyrightDisplay);
#endif
   return 0;
  }
  
  if (IsFileExecutable(argv[1]))
   strcpy(command_line, argv[1]);
  else
  {
   if (*argv[1] == '-')
    printf("%s: illegal option -- %s\n\
Try `%s --help' for more information.\n", argv[0], argv[1], argv[0]);
   else
    printf("%s: %s: No such file or directory\n", argv[0], argv[1]);
   return 1;
  }
 }
 /*--- concats extra arguments to command line. ---*/
 if (argc > 2)
 {
  for(i = 2; i < argc; i++)
  {
   strcat(command_line, " ");
   strcat(command_line, argv[i]);
  }
 }

 strcat(command_line, " &");

 if ((father_pid = fork()) == 0)
 {
  system(command_line);
  _exit(0);
 }

 Xc_free(command_line);

 for(i = 1; i < NSIG; i++)
 {
  def_handler[i] = signal(i, signal_handler);
 }
 
 do
 {
  pause();
  pid = wait(&status);
  
  if (WIFSTOPPED(status))
  {
   Xc_TRACE(("signal caught: %d", WSTOPSIG(status)));
  }
  else
  {
   Xc_TRACE(("a child died"));
  }

 } while(1);

 return 0;
}

/* ----------------------------------------------------------------- ** 
** usage - print launcher usage                                      ** 
** ----------------------------------------------------------------- */
static void usage(filename)
char *filename;
{
 printf("usage: %s [-help] [-version] FILE ...\n",  filename);
 printf("Launch an Axene Software\n\
detail:\n\
    -help, --help, -h            give this help\n\
    -version, --version          display version number\n\
    FILE                         file to execute\n\
    ...                          file arguments\n");
}

/* ----------------------------------------------------------------- ** 
** IsFileExecutable - Test if the file exist and is executable       ** 
** ----------------------------------------------------------------- */
static boolean IsFileExecutable(filename)
char *filename;
{
 struct stat bstat;
 
 Xc_HISTORY(("IsFileExecutable(%s)", filename));
 
 if (lstat(filename, &bstat) == -1)
  return FALSE;
 
 if ( (bstat.st_mode & S_IFDIR) == S_IFDIR)
 {
  Xc_TRACE(("dest is a dir: %s", filename));
  return FALSE;
 }
 
#if 0
 if ((S_IXEXEC & bstat.st_mode) == 0)
 {		/* not owner or not owner exec permission */
  Xc_TRACE(("can't exec dest: %s", filename));
  return FALSE;  
 }
#else
 if (geteuid() != bstat.st_uid || (S_IXUSR & bstat.st_mode) == 0)
 {		/* not owner or not owner exec permission */
  if (getegid() != bstat.st_uid || (S_IXGRP & bstat.st_mode) == 0)
  {		/* not group or not group exec permission */
   if ((S_IXOTH & bstat.st_mode) == 0)
   {		/* nor other exec permission */
    Xc_TRACE(("can't exec dest: %s", filename));
    return FALSE;
   }
  }
 }
#endif

 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** signal_handler                                                    ** 
** ----------------------------------------------------------------- */
static int recurse = 0;

static void signal_handler(sig)
int sig;
{
 Xc_TRACE(("Master Process receive a signal process: %d %s",
	   sig, (recurse == sig) ? "recurse": ""));
 
 signal(sig, signal_handler);
 
 if (recurse == sig) 
 {
  recurse = 0;
  return;
 }
 recurse = sig;

 switch(sig)
 {
 case SIGUSR1:
  Xc_TRACE(("Master process exit normally. Return code 0"));
  exit(0);
  break;
 case SIGUSR2:
  Xc_TRACE(("Master process exit abnormally. Return code -1"));
  exit(-1);
  break;
 case SIGTSTP:
 case SIGSTOP: 
  recurse = 0;
  signal(sig, def_handler[sig]);
  killpg(0, sig);
  signal(sig, signal_handler);
  break;
 default:
  killpg(0, sig);
  break;
 }
 
}
