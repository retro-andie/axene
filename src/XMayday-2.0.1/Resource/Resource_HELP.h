/*
** Resource_HELP.h for XMayday in Resource/
** Resource definition for XMayday
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
** Started on  Thu Mar  2 15:10:31 1995 Emmanuel Paris
** Last update Thu Feb  3 16:23:41 2000 Emmanuel Paris
*/

#ifndef Resource_HELP_h
#define Resource_HELP_h

#include "version.h"

/*___String Resources___*/

enum 
{
 XcR_memoryError,
 XcR_loadingError,
 XcR_fileOpen,
 XcR_indexOpen,
 XcR_saveDocument,
 XcR_printDocument,
 XcR_quitMessage,
 XcR_helpNotFound,
 
 /*--- HelpLine ---*/
 XcR_helpGoBack,
 XcR_helpGoForward,
 XcR_helpGoIndex,
 XcR_helpOpen,
 XcR_helpMoveIndex,
 XcR_helpPrint,
 XcR_helpQuit,
 
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
 XcR_notBitmapError,
 
 XcR_ownColormap,
 XcR_helpBubbles,

 XcR_aboutTitle,
 XcR_aboutComments,
 XcR_aboutEnd,
 XcR_aboutRelease,
 XcR_aboutReleaseBeta,
 XcR_authors,
 
 XcR_postscript,
 XcR_plainText,
 XcR_formatedText,
 
 XcR_STRING_COUNT		/* must be last */
};

/*___Font Resources___*/

enum
{
 XcR_iconFont,
 XcR_bubbleFont,
 XcR_FONT_COUNT		/* must be last */
};

/*___Int Resources___*/
enum 
{
 XcR_keepFreeColor,
 XcR_INT_COUNT		/* must be last */
};

#endif /* Resource_HELP_h */

#ifdef _Resource_HELP_PRIVATE_

/* ------------------------------------------------------------------- */
static XtResource resources[]=
{
 XcR_STRING(memoryError, "Out of memory for %ld bytes"),
 XcR_STRING(loadingError, "Loading error for `%s'"),
 XcR_STRING(fileOpen, "Open File"),
 XcR_STRING(indexOpen, "Open Index File"),
 XcR_STRING(saveDocument, "Save document as..."),
 XcR_STRING(printDocument, "Print in file..."),
 XcR_STRING(quitMessage, "Leave XMayday"),
 
 /*--- HelpLine ---*/
 XcR_STRING(helpNotFound, "Can't find help in %s"),

 XcR_STRING(helpGoBack, "Back"),
 XcR_STRING(helpGoForward, "Forward"),
 XcR_STRING(helpGoIndex, "Go to Index"),
 XcR_STRING(helpOpen, "Open"),
 XcR_STRING(helpMoveIndex, "Move Index"),
 XcR_STRING(helpPrint, "Print"),
 XcR_STRING(helpQuit, "Quit"),
 
 /*--- Signal Error Resources ---*/
 XcR_STRING(sigErrorINT, "INT"),
 XcR_STRING(sigErrorILL, "ILL"),
 XcR_STRING(sigErrorSEGV, "SEGV"),
 XcR_STRING(sigErrorPIPE, "PIPE"),
 XcR_STRING(sigErrorBUS, "BUS"),
 XcR_STRING(sigErrorFPE, "FPE"),
 XcR_STRING(sigErrorIOT, "IOT"),
 XcR_STRING(sigErrorEMT, "EMT"),
 XcR_STRING(sigErrorXCPU, "XCPU"),
 XcR_STRING(sigErrorXFSZ, "XFSZ"),
 XcR_STRING(sigErrorQuiting, "Exit"),
 XcR_STRING(sigErrorSorry, "Impossible to go on"),
 XcR_STRING(loadError, "Loading Error"),
 XcR_STRING(notBitmapError, "Unknown Image format"),
 
 XcR_STRING(ownColormap, "No"),
 XcR_STRING(helpBubbles, "Classic"),

 XcR_STRING(aboutTitle, "XMayday"),
 XcR_STRING(aboutComments, "Warning. Illegal Ressources."),
 XcR_STRING(aboutEnd, "Copyright ©%d-%d Axene."),
 XcR_STRING(aboutRelease, "XMayday® Release %d.%d"),
 XcR_STRING(aboutReleaseBeta, "XMayday® Release %d.%d.%d"),
 
 XcR_STRING(authors, "Emmanuel PARIS\nRobin CASTANIER\nAntoine BUAT\nStephane BOISSON"),
 
 XcR_STRING(postscript, "PostScript"),
 XcR_STRING(plainText, "Plain Text"),
 XcR_STRING(formatedText, "Formated Text"),
 
 XcR_FONT(iconFont, "8x13"),
 XcR_FONT(bubbleFont, "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*"),

 XcR_INT(keepFreeColor, 10),
};
/* ------------------------------------------------------------------- */
#endif



