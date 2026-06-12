/*
** Resource_OFFICE.h for AxeneOffice in Resource/
** Resource definition for Axene Office
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Thu Mar  2 15:10:31 1995 Emmanuel Paris
** Last update Tue Feb  1 11:41:54 2000 Emmanuel Paris
*/

#ifndef Resource_OFFICE_h
#define Resource_OFFICE_h

#include "version.h"

/*___String Resources___*/

enum 
{
 XcR_helpXclamation, 
 XcR_helpXQuad,
 XcR_helpXAllWrite,
 XcR_helpXMayday,
 
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
 
 XcR_ownColormap,
 XcR_helpBubbles,
 XcR_authors,
 
 XcR_STRING_COUNT		/* must be last */
};

/*___Font Resources___*/

enum 
{
 XcR_bubbleFont,
 XcR_FONT_COUNT			/* must be last */
};

/*___Int Resources___*/
enum 
{
 XcR_keepFreeColor,
 XcR_INT_COUNT			/* must be last */
};

#endif /* Resource_OFFICE_h */

#ifdef _Resource_OFFICE_PRIVATE_

/* ------------------------------------------------------------------- */
static XtResource resources[]=
{
 /*--- HelpLine ---*/
 XcR_STRING(helpXclamation, "Xclamation DTP software"),
 XcR_STRING(helpXQuad, "XQuad spreadsheet"),
 XcR_STRING(helpXAllWrite, "XAllWrite word processor"),
 XcR_STRING(helpXMayday, "XMayday documentation browser"),
 
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
 
 XcR_STRING(ownColormap, "No"),
 XcR_STRING(helpBubbles, "Classic"),
 XcR_STRING(authors, "Emmanuel PARIS\\nRobin CASTANIER\\nAntoine BUAT\\nStephane BOISSON"),
 
 XcR_FONT(bubbleFont, "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*"),

 XcR_INT(keepFreeColor, 10),
};
/* ------------------------------------------------------------------- */
#endif



