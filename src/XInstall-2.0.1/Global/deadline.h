/*
** deadline.h for XInstall in Global/
** Macro for deadline
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Fri Nov 10 11:50:25 1995 Stéphane Boisson
** Last update Thu Jan  6 17:21:51 2000 Emmanuel Paris
*/

#ifndef __XC_DEADLINE_H__
#define __XC_DEADLINE_H__


/* -------------------------( DEADLINE )------------------------------ */
/* old version
#define XcDEADLINE_YEAR 1996
#define XcDEADLINE_MONTH 10
#define XcDEADLINE_DAY 1
*/
#define XcDL_INITIAL_TIME (unsigned long)0x34db55b0
/* ------------------------------------------------------------------- */


#ifndef NDEADLINE

#include <time.h>
#include <sys/time.h>
/*#include <sys/stat.h>*/

#ifdef ___mips
extern time_t timezone;
#endif


/*--- Prototypes ---*/
void XcDL_updateTimeFromFiles();
void XcDL_updateTimeFromNet();


#define XcDL_GET_TIME _xc_deadline_check
#define XcDL_SET_TIME(value) XcDL_GET_TIME = (value)

#ifdef __XC_MAIN__
time_t	XcDL_GET_TIME = 0;
int	XcDEADLINE_YEAR = 0;
int	XcDEADLINE_MONTH = 0;
int	XcDEADLINE_DAY = 0;
#else
extern time_t	XcDL_GET_TIME;
extern int	XcDEADLINE_YEAR;
extern int	XcDEADLINE_MONTH;
extern int	XcDEADLINE_DAY;
#endif

/*--- Delta betwewn 1/1/1900 and 1/1/1970 ---*/
#define XcDL_DAY_DELTA (((unsigned long)70 * (unsigned long)365) + (unsigned long)17)
#define XcDL_SEC_DELTA (XcDL_DAY_DELTA * (unsigned long)86400)

#define XcDL_DAYS_PER_MONTHS(_dl_dm) \
static short _dl_dm[2][12] = {{0,31,59,90,120,151,181,212,243,273,304,334}, \
			      {0,31,60,91,121,152,182,213,244,274,305,335}}

#define XcDL_SECONDS_TO_DATE(_dl_day, _dl_month, _dl_year, _dl_dm) \
do { \
  register unsigned long _dl_i; \
\
  _dl_day = (XcDL_GET_TIME + XcDL_SEC_DELTA) / (unsigned long)86400; \
  _dl_year = _dl_day / (unsigned long)365; \
  while(_dl_day < (_dl_i = ((_dl_year - 1) >> 2) + ((unsigned long)365 * _dl_year))) \
    _dl_year--; \
  _dl_day -= _dl_i; \
  for(_dl_month = 12; (short)_dl_day < _dl_dm[!(_dl_year & 3)][--_dl_month]; ); \
  _dl_day -= _dl_dm[!(_dl_year & 3)][_dl_month]; \
} while(0)

#define XcDL_DATE_TO_DAYS(_dl_day, _dl_month, _dl_year, _dl_dm) \
(((((_dl_year) - 1) >> 2) + (365L * (_dl_year)) - XcDL_DAY_DELTA) + \
 _dl_dm[!((_dl_year) & 3)][(_dl_month)] + (_dl_day))

#define XcDAYS_TO_DEADLINE(_dl_rc) \
do { \
  unsigned long _dl_year, _dl_month, _dl_day; \
  XcDL_DAYS_PER_MONTHS(_dl_dm); \
\
  XcDL_SECONDS_TO_DATE(_dl_day, _dl_month, _dl_year, _dl_dm); \
  if(XcDL_INITIAL_TIME < (unsigned long)XcDL_GET_TIME) \
    _dl_rc = XcDL_DATE_TO_DAYS(XcDEADLINE_DAY - 1, XcDEADLINE_MONTH - 1, XcDEADLINE_YEAR - 1900, _dl_dm) - XcDL_DATE_TO_DAYS(_dl_day, _dl_month, _dl_year, _dl_dm); \
  else \
    _dl_rc = -1; \
} while(0)

#define XcDO_BEFORE_DEADLINE(_dl_func) \
do { \
  unsigned long _dl_year, _dl_month, _dl_day; \
  XcDL_DAYS_PER_MONTHS(_dl_dm); \
\
  if(XcDL_INITIAL_TIME >= XcDL_GET_TIME) break; \
  XcDL_SECONDS_TO_DATE(_dl_day, _dl_month, _dl_year, _dl_dm); \
  if((_dl_year < (XcDEADLINE_YEAR - 1900)) || \
     ((_dl_year == (XcDEADLINE_YEAR - 1900)) && \
      ((_dl_month < (XcDEADLINE_MONTH - 1)) || \
       ((_dl_month == (XcDEADLINE_MONTH - 1)) && \
	(_dl_day < (XcDEADLINE_DAY - 1)))))) _dl_func; \
} while(0)

#define XcDO_AFTER_DEADLINE(_dl_func) \
do { \
  unsigned long _dl_year, _dl_month, _dl_day; \
  XcDL_DAYS_PER_MONTHS(_dl_dm); \
\
  XcDL_SECONDS_TO_DATE(_dl_day, _dl_month, _dl_year, _dl_dm); \
  if((XcDL_INITIAL_TIME >= XcDL_GET_TIME) || \
     (_dl_year > (XcDEADLINE_YEAR - 1900)) || \
     ((_dl_year == (XcDEADLINE_YEAR - 1900)) && \
      ((_dl_month > (XcDEADLINE_MONTH - 1)) || \
       ((_dl_month == (XcDEADLINE_MONTH - 1)) && \
	(_dl_day >= (XcDEADLINE_DAY - 1)))))) _dl_func; \
} while(0)


/*--- Time updating functions ---*/
#define XcDL_UPDATE_TIME(value) \
  if((value) > XcDL_GET_TIME) XcDL_SET_TIME(value);

#define XcDL_TIME_STAT_UPDATE(_dl_file) \
do { \
  XcDL_UPDATE_TIME((_dl_file)->st_atime); \
  XcDL_UPDATE_TIME((_dl_file)->st_mtime); \
  XcDL_UPDATE_TIME((_dl_file)->st_ctime); \
} while(0)

#define XcDL_TIME_FILE_UPDATE(_dl_filename) \
do { \
  struct stat _dl_file; \
\
  if(stat(_dl_filename, &_dl_file) < 0) break; \
  XcDL_TIME_STAT_UPDATE(&_dl_file); \
} while(0)

#define XcDL_TIME_DIR_UPDATE(pathname) XcDL_updateTimeFromFiles(pathname)
#define XcDL_TIME_NET_UPDATE XcDL_updateTimeFromNet()


#else /* !NDEADLINE */

#define XcDAYS_TO_DEADLINE(_dl_rc) (_dl_rc) = 365
#define XcGO_BEFORE_DEADLINE(_dl_func) _dl_func
#define XcGO_AFTER_DEADLINE(_dl_func) (void)0
#define XcDL_UPDATE_TIME(value) (void)0
#define XcDL_TIME_STAT_UPDATE(_dl_file) (void)0
#define XcDL_TIME_FILE_UPDATE(_dl_filename) (void)0
#define XcDL_TIME_DIR_UPDATE(pathname) (void)0
#define XcDL_TIME_NET_UPDATE (void)0

#endif /* !NDEADLINE */

#endif /* !__XC_DEADLINE_H__ */

