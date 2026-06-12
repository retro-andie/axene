/*
** ErrorManager.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Managers/
** Functions for errors and debugging
**
** Copyright (C) 1994-2000 Axene.
** Authors: St�phane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
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
** Started on  Sun Jun 12 02:00:44 1994 St�phane Boisson
** Last update Thu Dec 30 17:51:38 1999 Emmanuel Paris
*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/times.h>
#include <limits.h>
#include <unistd.h>

FILE *Xc_log_file = NULL;
FILE *Xc_history_file = NULL;

/* ----------------------------------------------------------------- **
** Xc_strerror - strerror implementation                             **
** ----------------------------------------------------------------- */
char *Xc_strerror(int errorcode)
{
 return strerror(errorcode);
}


/* ----------------------------------------------------------------- **
** Xc_LogPrintf - Output informations to log-file                    **
** ----------------------------------------------------------------- */
void Xc_LogPrintf(char *format, ...)
{
 va_list ap;

 if (!Xc_log_file) Xc_log_file = stderr;
 va_start(ap, format);
 vfprintf(Xc_log_file, format, ap);
 va_end(ap);
}


/* ----------------------------------------------------------------- **
** Xc_HistoryPrintf - Output informations to history file            **
** ----------------------------------------------------------------- */
void Xc_HistoryPrintf(char *format, ...)
{
 va_list ap;

 if (!Xc_history_file) Xc_history_file = stderr;
 va_start(ap, format);
 vfprintf(Xc_history_file, format, ap);
 va_end(ap);
}


/* ----------------------------------------------------------------- **
** Xc_ReportTime - Output timing infos                               **
** ----------------------------------------------------------------- */
void Xc_ReportTime(report)
int report;
{
#if !defined(___mips) && !defined(___sun4)
 static int first_time = -1;
 static struct tms last, now;
 static clock_t current, prev;

 if(!first_time && report)
 {
  int user, sys, elapsed;

  current = times(&now);
  user = (int)(1000 * (now.tms_utime - last.tms_utime) / sysconf(_SC_CLK_TCK));
  sys = (int)(1000 * (now.tms_stime - last.tms_stime) / sysconf(_SC_CLK_TCK));
  elapsed = (int)(1000 * (current - prev) / sysconf(_SC_CLK_TCK));
  fprintf(Xc_log_file, "  Elapsed Time = %d.%03d\n",
	  elapsed / 1000, elapsed % 1000);
  fprintf(Xc_log_file, "  User Time = %d.%03d\n",
	  user / 1000, user % 1000);
  fprintf(Xc_log_file, "  System Time = %d.%03d\n",
	  sys / 1000, sys % 1000);
 }
 first_time = 0;
 prev = times(&last);
#endif
}


#if 0
/* ----------------------------------------------------------------- ** 
** main - main function for testing purposes                         ** 
** ----------------------------------------------------------------- */
void main(void)
{
 Xc_HISTORY(("ErrorManager test"));
 Xc_TRACE(("polom polom je suis dans %s a la ligne %d", __FILE__, __LINE__));
 Xc_WARNING(("gato %s %d", "test", 13));
 Xc_ASSERT(1==2);
}
#endif
