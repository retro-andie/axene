/*
** Resource_INSTALL.h for XInstall in Resource/
** Definitions for the Resource class
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
** Started on  Thu Dec 15 18:44:46 1994 Stéphane Boisson
** Last update Tue Feb  1 11:54:27 2000 Emmanuel Paris
*/

#ifndef Resource_INSTALL_h
#define Resource_INSTALL_h

#include "version.h"

/*___String Resources___*/

enum 
{
 XcR_memoryError,
 XcR_loadingError,
 XcR_browseDirectorySrc,
 XcR_browseDirectoryDst,

 /*--- Signal Error Resources ---*/
 XcR_sigErrorINT,
 XcR_sigErrorILL,
 XcR_sigErrorSEGV,
 XcR_sigErrorPIPE,
 XcR_sigErrorBUS,
 XcR_sigErrorFPE,
 XcR_sigErrorIOT,
 XcR_sigErrorEMT,
 XcR_sigErrorXCPU,
 XcR_sigErrorXFSZ,
 XcR_sigErrorQuiting,
 XcR_sigErrorSorry,
 XcR_loadError,
 XcR_helpNotFound,
  
 XcR_freeSpace,
 XcR_usedSpace,
 XcR_noPackageFound,
 XcR_onePackageFound,
 XcR_manyPackageFound,
 XcR_noPackageSelected,
 XcR_onePackageSelected,
 XcR_manyPackageSelected,
 XcR_unknown,
 XcR_installComplete,
 XcR_installQuit,
 XcR_bytes,
 XcR_kiloBytes,
 XcR_megaBytes,
 XcR_unknownSize,
 XcR_freeSpace2,
 XcR_createdir,
 XcR_cannotCreateDir,
 XcR_dirNotWritable,
 XcR_keynotvalid,
 XcR_size,
 XcR_notavailable,
 XcR_demo,
 XcR_manual,
 XcR_libraries,
 XcR_quitMessage,
 XcR_installEndMessage,
 XcR_lBKInfo,
 XcR_notEnoughSpace,

 XcR_ownColormap,
  
 XcR_STRING_COUNT		/* must be last */
};

/*___Font Resources___*/

enum 
{
 XcR_installNormal,
 XcR_installItalic,
 XcR_installItalicSmall,
 XcR_FONT_COUNT			/* must be last */
};

/*___Int Resources___*/
enum 
{
 XcR_keepFreeColor,
 XcR_INT_COUNT			/* must be last */
};

#endif /* Resource_INSTALL_h */

#ifdef _Resource_INSTALL_PRIVATE_

/* ------------------------------------------------------------------- */
static XtResource resources[] =
{
 XcR_STRING(memoryError, "Out of memory for %ld bytes"),
 XcR_STRING(loadingError, "Loading error for `%s'"),
 XcR_STRING(browseDirectorySrc, "Browse Directory Src"),
 XcR_STRING(browseDirectoryDst, "Browse Directory Dst"),

 /*--- Signal Error Resources ---*/
 XcR_STRING(sigErrorINT, "XInstall has been interrupted (Interrupt)"),
 XcR_STRING(sigErrorILL, "Illegal instruction"),
 XcR_STRING(sigErrorSEGV, "Segmentation violation (Segmentation fault)"),
 XcR_STRING(sigErrorPIPE, "Connection broken (Broken pipe)"),
 XcR_STRING(sigErrorBUS, "Specification exception (Bus error)"),
 XcR_STRING(sigErrorFPE, "Arithmetic exception (Floating exception)"),
 XcR_STRING(sigErrorIOT, "Internal Error (IOT trap)"),
 XcR_STRING(sigErrorEMT, "EMT instruction (EMT trap)"),
 XcR_STRING(sigErrorXCPU, "Cputime limit exceeded"),
 XcR_STRING(sigErrorXFSZ, "Filesize limit exceeded"),
 XcR_STRING(sigErrorQuiting, "Exit"),
 XcR_STRING(sigErrorSorry, "Impossible to go on"),
 /*--- Error messages ---*/
 XcR_STRING(loadError, "Loading Error"),
 XcR_STRING(helpNotFound, "Help is not available"),
  
 XcR_STRING(freeSpace, "Free Space: %ld KB"),
 XcR_STRING(usedSpace, "Used Space: %ld KB"),
 XcR_STRING(noPackageFound, "No package found"),
 XcR_STRING(onePackageFound, "1 package found"),
 XcR_STRING(manyPackageFound, "%d packages found"),
 XcR_STRING(noPackageSelected, "No package selected"),
 XcR_STRING(onePackageSelected, "1 package selected"),
 XcR_STRING(manyPackageSelected, "%d packages selected"),
 XcR_STRING(unknown, "unknown"),
 XcR_STRING(installComplete, "install complete"),
 XcR_STRING(installQuit, "Exit"),
 XcR_STRING(bytes, "Bytes"),
 XcR_STRING(kiloBytes, "Kb"),
 XcR_STRING(megaBytes, "Mb"),
 XcR_STRING(unknownSize, "(unknown size)"),
 XcR_STRING(freeSpace2, "%ld KB"),
 XcR_STRING(createdir, "x %s"),
 XcR_STRING(cannotCreateDir, "x %s"),
 XcR_STRING(dirNotWritable, "x %s"),
 XcR_STRING(keynotvalid, "The key is not valid"),
 /*--- messages for packages ---*/
 XcR_STRING(size, "size"),
 XcR_STRING(notavailable, "not avaible"),
 XcR_STRING(demo, "Demo"),
 XcR_STRING(manual, "Manual"),
 XcR_STRING(libraries, "Common libraries"),
 XcR_STRING(quitMessage, "x"),
 XcR_STRING(installEndMessage, "x"),
 XcR_STRING(lBKInfo, "x"),
 XcR_STRING(notEnoughSpace, "x"),
  
 XcR_STRING(ownColormap, "No"),

 XcR_FONT(installNormal, "-*-helvetica-medium-r-*-*-14-*-*-*-*-*-iso8859-1"),
 XcR_FONT(installItalic, "-*-helvetica-medium-*-*-*-14-*-*-*-*-*-iso8859-1"),
 XcR_FONT(installItalicSmall, "-*-helvetica-medium-*-*-*-10-*-*-*-*-*-iso8859-1"),
  
 XcR_INT(keepFreeColor, 10),
};
/* ------------------------------------------------------------------- */
#endif

