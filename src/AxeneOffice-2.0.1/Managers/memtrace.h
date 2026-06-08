/*
** memtrace.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Managers/
** Modified trace macros for memory allocation debugging
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
** Started on  Fri Mar 24 15:10:25 1995 Stéphane Boisson
** Last update Fri Mar 24 15:13:18 1995 Stéphane Boisson
*/

#ifndef __XC_MEM_TRACE_H__
#define __XC_MEM_TRACE_H__

#undef Xc_FATAL
#define Xc_FATAL(args) \
{ Xc_LogPrintf("FATAL ERROR [%s %d] - ", filename, linenumber); \
    Xc_LogPrintf args; putc('\n', Xc_log_file); Xc_exit(-1); }

#undef Xc_WARNING
#define Xc_WARNING(args) \
{ Xc_LogPrintf("WARNING [%s %d] - ", filename, linenumber); \
    Xc_LogPrintf args; putc('\n', Xc_log_file); }

#undef Xc_ERROR
#define Xc_ERROR(args) \
{ Xc_LogPrintf("ERROR [%s %d] - ", filename, linenumber); \
    Xc_LogPrintf args; putc('\n', Xc_log_file); }

#endif /* !__XC_MEM_TRACE_H__ */
