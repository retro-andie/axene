/*
** Resource_PAO.h for Xclamation in Resource/
** Resource definition for Xclamation
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
** Started on  Thu Mar  2 15:10:31 1995 Emmanuel Paris
** Last update Tue Aug  8 12:46:59 2000 Emmanuel Paris
*/

#ifndef Resource_PAO_h
#define Resource_PAO_h

#include "version.h"

/*___String Resources___*/

enum 
{
 XcR_memoryError,
 XcR_loadingError,
 XcR_editColors,
 XcR_frameAttribs,
 XcR_editStyles,
 XcR_editRulers,
 XcR_aboutTitle,
 XcR_aboutComments,
 XcR_aboutEnd,
#ifdef PL
 XcR_aboutReleaseBeta,
#else
 XcR_aboutRelease,
#endif
 XcR_loadImage,
 XcR_loadText,
 XcR_loadVector,
 XcR_loadDocument,
 XcR_saveDocument,
 XcR_printDocument,
 XcR_quitMessage,
 XcR_saveTextAsc,
 XcR_saveTextXct,
  
 /*--- Dates ----*/
 XcR_dateMonthJan,
 XcR_dateMonthFeb,
 XcR_dateMonthMar,
 XcR_dateMonthApr,
 XcR_dateMonthMay,
 XcR_dateMonthJun,
 XcR_dateMonthJul,
 XcR_dateMonthAug,
 XcR_dateMonthSep,
 XcR_dateMonthOct,
 XcR_dateMonthNov,
 XcR_dateMonthDec,

 XcR_dateMonthJanShort,
 XcR_dateMonthFebShort,
 XcR_dateMonthMarShort,
 XcR_dateMonthAprShort,
 XcR_dateMonthMayShort,
 XcR_dateMonthJunShort,
 XcR_dateMonthJulShort,
 XcR_dateMonthAugShort,
 XcR_dateMonthSepShort,
 XcR_dateMonthOctShort,
 XcR_dateMonthNovShort,
 XcR_dateMonthDecShort,

 XcR_dateDayMon,
 XcR_dateDayTue,
 XcR_dateDayWed,
 XcR_dateDayThu,
 XcR_dateDayFri,
 XcR_dateDaySat,
 XcR_dateDaySun,

 XcR_dateDayMonShort,
 XcR_dateDayTueShort,
 XcR_dateDayWedShort,
 XcR_dateDayThuShort,
 XcR_dateDayFriShort,
 XcR_dateDaySatShort,
 XcR_dateDaySunShort,

 /*--- HelpLine MISC tools ---*/
 XcR_helpMisc,
 XcR_helpMiscNew,
 XcR_helpMiscOpen,
 XcR_helpMiscSave,
 XcR_helpMiscPrint,
 XcR_helpMiscCut,
 XcR_helpMiscCopy,
 XcR_helpMiscPaste,
 XcR_helpMiscDelete,
 /*--- HelpLine Outils CADRE ---*/
 XcR_helpFrame,
 XcR_helpFrameSelect,
 XcR_helpFrameRotate,
 XcR_helpFrameScale,
 XcR_helpFrameSquare,
 XcR_helpFrameCircle,
 XcR_helpFramePolygon,
 XcR_helpFrameAdd,
 XcR_helpFrameSub,
 XcR_helpFrameXor,
 XcR_helpFrameOutline,
 XcR_helpFrameAlign,
 XcR_helpFrameMovePoint,
 XcR_helpFrameAddPoint,
 XcR_helpFrameSubPoint,
 XcR_helpFramePlanFirst,
 XcR_helpFramePlanLast,
 XcR_helpFramePlanUp,
 XcR_helpFramePlanDown,
 XcR_helpFrameLock,
 /*--- HelpLine Outils TEXTE ---*/
 XcR_helpText,
 XcR_helpTextEditor,
 XcR_helpTextLinkEnd,
 XcR_helpTextLinkBegin,
 XcR_helpTextInsertBefore,
 XcR_helpTextInsertAfter,
 XcR_helpTextUnlink,
 /*--- HelpLine Outils IMAGE ---*/
 XcR_helpPicture,
 XcR_helpPictureMove,
 XcR_helpPictureCenter,
 XcR_helpPictureReset,
 XcR_helpPictureVFlip, 
 XcR_helpPictureHFlip,
 XcR_helpPictureMap1,
 XcR_helpPictureMap2,
 XcR_helpPictureBest,
 XcR_helpPictureMax,
 XcR_helpPictureStretch,
 /*--- HelpLine Outils LOUPE ---*/
 XcR_helpZoom,
 XcR_helpZoomIn,
 XcR_helpZoomOut,
 XcR_helpZoomZone,
 XcR_helpZoomWidth,
 XcR_helpZoomFull,
 XcR_helpZoomReal,
 XcR_helpZoom25,
 XcR_helpZoom50,
 XcR_helpZoom75,
 XcR_helpZoom100,
 XcR_helpZoom200,
 XcR_helpZoom400,
 /*--- HelpLine Outils VECTOR ---*/
 XcR_helpVector,
 XcR_helpVectorMove,
 XcR_helpVectoreCenter,
 XcR_helpVectorReset,
 XcR_helpVectorVFlip, 
 XcR_helpVectorHFlip,
 XcR_helpVectorBest,
 XcR_helpVectorMax,
 XcR_helpVectorStretch,
 /*--- HelpLine Outils COMUNICATION ---*/
 XcR_helpComm,

 XcR_helpTrash,

 /*--- HelpLine Fonctions Cadre ---*/
 XcR_helpFFrameSelect,
 XcR_helpFFrameRotate,
 XcR_helpFFrameScale,
 XcR_helpFFrameSquare,
 XcR_helpFFrameCircle,
 XcR_helpFFramePolygon,
 XcR_helpFFrameMovePoint,
 XcR_helpFFrameAddPoint,
 XcR_helpFFrameSubPoint,

 /*--- HelpLine Fonctions Image ---*/
 XcR_helpFPictureMove,

 /*--- HelpLine Fonctions Vector ---*/
 XcR_helpFVectorMove,

 /*--- HelpLine Fonctions Zoom ---*/
 XcR_helpFZoomIn,
 XcR_helpFZoomOut,
 XcR_helpFZoomZone,

 /*--- HelpLine ContextMenu Pager---*/
 XcR_helpCMPgrInsBef,
 XcR_helpCMPgrInsAft,
 XcR_helpCMPgrModify,
 XcR_helpCMPgrDelete,

 /*--- HelpLine ContextMenu Cadre vide---*/
 XcR_helpCMCdeImpImage,
 XcR_helpCMCdeImpTexte,
 XcR_helpCMCdeImpVector,
      
 /*--- HelpLine ContextMenu No cadre---*/
 /* Memes que pour la barre d'icones */
  
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
 XcR_noFrameError,
 XcR_notBitmapError,

 /*--- Standard names ---*/
 XcR_defaultDocumentName,
 XcR_defaultFrameName,
 XcR_defaultPageName,
  
 XcR_ownColormap,
 XcR_opaqueResize,
 XcR_helpBubbles,

 XcR_deadlineDate,
 XcR_deadlineExpired,
 XcR_deadlineExpired2,

 XcR_exportText,
 XcR_exportTextXcl,
 XcR_exportTextASCII,
 XcR_exportTextHTML,

 XcR_greetings,
 XcR_authors,
  
 XcR_setupPrinters,
 XcR_setupPaths,

 XcR_helpVectorTexts,
 XcR_helpNotFound,

 XcR_keyFirst,
 XcR_keySecond,
 XcR_noKeyFirst,
 XcR_noVKeyFirst,
 XcR_noKeySecond,
 XcR_noVKeySecond,
 XcR_stationFullKeyFirst,
 XcR_stationFullKeySecond,
 XcR_serverFullKeyFirst,
 XcR_serverFullKeySecond, 
 XcR_functionNeedKey,
 XcR_autosaveNeedKey,
  
 XcR_aboutLicense,
 XcR_aboutNoLicense,

 XcR_licenseCommercial,
 XcR_licensePersonal,
  
 XcR_licenseTextOk,
 XcR_licenseTextNo,
 XcR_licenseTextTK,
  
 XcR_licenseFileName,
 XcR_registrationFileName,

 XcR_mailingList,
 XcR_font_name,
  
 XcR_styleBold,
 XcR_styleItalic,
 XcR_styleUnderline,
 XcR_styleThickness,
 XcR_styleSimple,
 XcR_styleDouble,
 XcR_styleTriple,
 XcR_styleNoSpace,
 XcR_styleStrikeout,
 XcR_styleShadow,
 XcR_styleWidth,
 XcR_styleSubscript,
 XcR_styleScale,
 XcR_styleSupscript,
 XcR_styleSmallCaps,
 XcR_styleBigCaps,

 XcR_authorTitle,

 XcR_STRING_COUNT		/* must be last */
};

/*___Font Resources___*/

enum 
{
 XcR_pagerFont,
 XcR_iconFont,
 XcR_colorlistFont,
 XcR_bubbleFont,
 XcR_loadingMessageFont,
 XcR_FONT_COUNT			/* must be last */
};

/*___Int Resources___*/
enum 
{
 XcR_keepFreeColor,
 XcR_opaqueMove,
 XcR_INT_COUNT			/* must be last */
};

#endif /* Resource_PAO_h */

#ifdef _Resource_PAO_PRIVATE_

/* ------------------------------------------------------------------- */
static XtResource resources[]=
{
 XcR_STRING(memoryError, "Out of memory for %ld bytes"),
 XcR_STRING(loadingError, "Loading error for `%s'"),
 XcR_STRING(editColors, "x"),
 XcR_STRING(frameAttribs, "x"),
 XcR_STRING(editStyles, "x"),
 XcR_STRING(editRulers, "x"),
 XcR_STRING(aboutTitle, "Xclamation® - Desktop Publishing"),
 XcR_STRING(aboutComments, "EMail: contact@axene.org\nBug-report: support@axene.org\nhttp://www.axene.org/\nftp://ftp.axene.org/"),
 XcR_STRING(aboutEnd, "©%d-%d Axene."),
#ifdef PL
 XcR_STRING(aboutReleaseBeta, "Xclamation® %d.%d.%d"),
#else
 XcR_STRING(aboutRelease, "Xclamation® %d.%d"),
#endif
 XcR_STRING(loadImage, "x"),
 XcR_STRING(loadText, "x"),
 XcR_STRING(loadVector, "x"),
 XcR_STRING(loadDocument, "x"),
 XcR_STRING(saveDocument, "x"),
 XcR_STRING(printDocument, "x"),
 XcR_STRING(quitMessage, "x"),
 XcR_STRING(saveTextAsc, "x"),
 XcR_STRING(saveTextXct, "x"),

 /*--- Dates ----*/
 XcR_STRING(dateMonthJan, "January"),
 XcR_STRING(dateMonthFeb, "February"),
 XcR_STRING(dateMonthMar, "March"),
 XcR_STRING(dateMonthApr, "April"),
 XcR_STRING(dateMonthMay, "May"),
 XcR_STRING(dateMonthJun, "June"),
 XcR_STRING(dateMonthJul, "July"),
 XcR_STRING(dateMonthAug, "August"),
 XcR_STRING(dateMonthSep, "September"),
 XcR_STRING(dateMonthOct, "October"),
 XcR_STRING(dateMonthNov, "November"),
 XcR_STRING(dateMonthDec, "December"),

 XcR_STRING(dateMonthJanShort, "Jan"),
 XcR_STRING(dateMonthFebShort, "Feb"),
 XcR_STRING(dateMonthMarShort, "Mar"),
 XcR_STRING(dateMonthAprShort, "Apr"),
 XcR_STRING(dateMonthMayShort, "May"),
 XcR_STRING(dateMonthJunShort, "Jun"),
 XcR_STRING(dateMonthJulShort, "Jul"),
 XcR_STRING(dateMonthAugShort, "Aug"),
 XcR_STRING(dateMonthSepShort, "Sep"),
 XcR_STRING(dateMonthOctShort, "Oct"),
 XcR_STRING(dateMonthNovShort, "Nov"),
 XcR_STRING(dateMonthDecShort, "Dec"),

 XcR_STRING(dateDayMon, "Monday"),
 XcR_STRING(dateDayTue, "Tuesday"),
 XcR_STRING(dateDayWed, "Wednesday"),
 XcR_STRING(dateDayThu, "Thursday"),
 XcR_STRING(dateDayFri, "Friday"),
 XcR_STRING(dateDaySat, "Saturday"),
 XcR_STRING(dateDaySun, "Sunday"),

 XcR_STRING(dateDayMonShort, "Mon"),
 XcR_STRING(dateDayTueShort, "Tue"),
 XcR_STRING(dateDayWedShort, "Wed"),
 XcR_STRING(dateDayThuShort, "Thu"),
 XcR_STRING(dateDayFriShort, "Fri"),
 XcR_STRING(dateDaySatShort, "Sat"),
 XcR_STRING(dateDaySunShort, "Sun"),

 /*--- HelpLine MISC tools ---*/
 XcR_STRING(helpMisc, "x"),
 XcR_STRING(helpMiscNew, "x"),
 XcR_STRING(helpMiscOpen, "x"),
 XcR_STRING(helpMiscSave, "x"),
 XcR_STRING(helpMiscPrint, "x"),
 XcR_STRING(helpMiscCut, "x"),
 XcR_STRING(helpMiscCopy, "x"),
 XcR_STRING(helpMiscPaste, "x"),
 XcR_STRING(helpMiscDelete, "x"),
 /*--- HelpLine Outils CADRE ---*/
 XcR_STRING(helpFrame, "x"),
 XcR_STRING(helpFrameSelect, "x"),
 XcR_STRING(helpFrameRotate, "x"),
 XcR_STRING(helpFrameScale, "x"),
 XcR_STRING(helpFrameSquare, "x"),
 XcR_STRING(helpFrameCircle, "x"),
 XcR_STRING(helpFramePolygon, "x"),
 XcR_STRING(helpFrameAdd, "x"),
 XcR_STRING(helpFrameSub, "x"),
 XcR_STRING(helpFrameXor, "x"),
 XcR_STRING(helpFrameOutline, "x"),
 XcR_STRING(helpFrameAlign, "x"),
 XcR_STRING(helpFrameMovePoint, "x"),
 XcR_STRING(helpFrameAddPoint, "x"),
 XcR_STRING(helpFrameSubPoint, "x"),
 XcR_STRING(helpFramePlanFirst, "x"),
 XcR_STRING(helpFramePlanLast, "x"),
 XcR_STRING(helpFramePlanUp, "x"),
 XcR_STRING(helpFramePlanDown, "x"),
 XcR_STRING(helpFrameLock, "x"),

 /*--- HelpLine Outils TEXTE ---*/
 XcR_STRING(helpText, "x"),
 XcR_STRING(helpTextEditor, "x"),
 XcR_STRING(helpTextLinkEnd, "x"),
 XcR_STRING(helpTextLinkBegin, "x"),
 XcR_STRING(helpTextInsertBefore, "x"),
 XcR_STRING(helpTextInsertAfter, "x"),
 XcR_STRING(helpTextUnlink, "x"),

 /*--- HelpLine Outils IMAGE ---*/
 XcR_STRING(helpPicture, "x"),
 XcR_STRING(helpPictureMove, "x"),
 XcR_STRING(helpPictureCenter, "x"),
 XcR_STRING(helpPictureReset, "x"),
 XcR_STRING(helpPictureVFlip, "x"),
 XcR_STRING(helpPictureHFlip, "x"),
 XcR_STRING(helpPictureMap1, "x"),
 XcR_STRING(helpPictureMap2, "x"),
 XcR_STRING(helpPictureBest, "x"),
 XcR_STRING(helpPictureMax, "x"),
 XcR_STRING(helpPictureStretch, "x"),

 /*--- HelpLine Outils LOUPE ---*/
 XcR_STRING(helpZoom, "x"),
 XcR_STRING(helpZoomIn, "x"),
 XcR_STRING(helpZoomOut, "x"),
 XcR_STRING(helpZoomZone, "x"),
 XcR_STRING(helpZoomWidth, "x"),
 XcR_STRING(helpZoomFull, "x"),
 XcR_STRING(helpZoomReal, "x"),
 XcR_STRING(helpZoom25, "x"),
 XcR_STRING(helpZoom50, "x"),
 XcR_STRING(helpZoom75, "x"),
 XcR_STRING(helpZoom100, "x"),
 XcR_STRING(helpZoom200, "x"),
 XcR_STRING(helpZoom400, "x"),

 /*--- HelpLine Outils VECTOR ---*/
 XcR_STRING(helpVector, "x"),
 XcR_STRING(helpVectorMove, "x"),
 XcR_STRING(helpVectoreCenter, "x"),
 XcR_STRING(helpVectorReset, "x"),
 XcR_STRING(helpVectorVFlip, "x"),
 XcR_STRING(helpVectorHFlip, "x"),
 XcR_STRING(helpVectorBest, "x"),
 XcR_STRING(helpVectorMax, "x"),
 XcR_STRING(helpVectorStretch, "x"),
 /*--- HelpLine Outils COMUNICATION ---*/
 XcR_STRING(helpComm, "x"),

 XcR_STRING(helpTrash, "x"),

 /*--- HelpLine Fonctions Cadre ---*/
 XcR_STRING(helpFFrameSelect, "x"),
 XcR_STRING(helpFFrameRotate, "x"),
 XcR_STRING(helpFFrameScale, "x"),
 XcR_STRING(helpFFrameSquare, "x"),
 XcR_STRING(helpFFrameCircle, "x"),
 XcR_STRING(helpFFramePolygon, "x"),
 XcR_STRING(helpFFrameMovePoint, "x"),
 XcR_STRING(helpFFrameAddPoint, "x"),
 XcR_STRING(helpFFrameSubPoint, "x"),

 /*--- HelpLine Fonctions Image ---*/
 XcR_STRING(helpFPictureMove, "x"),

 /*--- HelpLine Fonctions Vector ---*/
 XcR_STRING(helpFVectorMove, "x"),

 /*--- HelpLine Fonctions Zoom ---*/
 XcR_STRING(helpFZoomIn, "x"),
 XcR_STRING(helpFZoomOut, "x"),
 XcR_STRING(helpFZoomZone, "x"),

 /*--- HelpLine ContextMenu Pager---*/
 XcR_STRING(helpCMPgrInsBef, "x"),
 XcR_STRING(helpCMPgrInsAft, "x"),
 XcR_STRING(helpCMPgrModify, "x"),
 XcR_STRING(helpCMPgrDelete, "x"),

 /*--- HelpLine ContextMenu Cadre vide---*/
 XcR_STRING(helpCMCdeImpImage, "x"),
 XcR_STRING(helpCMCdeImpTexte, "x"),
 XcR_STRING(helpCMCdeImpVector, "x"),
  
 /*--- Signal Error Resources ---*/
 XcR_STRING(sigErrorINT, "x"),
 XcR_STRING(sigErrorILL, "x"),
 XcR_STRING(sigErrorSEGV, "x"),
 XcR_STRING(sigErrorPIPE, "x"),
 XcR_STRING(sigErrorBUS, "x"),
 XcR_STRING(sigErrorFPE, "x"),
 XcR_STRING(sigErrorIOT, "x"),
 XcR_STRING(sigErrorEMT, "x"),
 XcR_STRING(sigErrorXCPU, "x"),
 XcR_STRING(sigErrorXFSZ, "x"),
 XcR_STRING(sigErrorQuiting, "x"),
 XcR_STRING(sigErrorSorry, "x"),
 XcR_STRING(loadError, "x"),
 XcR_STRING(noFrameError, "x"),
 XcR_STRING(notBitmapError, "x"),

 /*--- Standard names ---*/
 XcR_STRING(defaultDocumentName, "Untitled"),
 XcR_STRING(defaultFrameName, "Frame"),
 XcR_STRING(defaultPageName, "Page"),
  
 XcR_STRING(ownColormap, "No"),
 XcR_STRING(opaqueResize, "No"),
 XcR_STRING(helpBubbles, "Classic"),

 XcR_STRING(deadlineDate, "x %d/%d/%d"),
 XcR_STRING(deadlineExpired, "x"),
 XcR_STRING(deadlineExpired2, "x"),
 
 XcR_STRING(exportText, "Export text"),
 XcR_STRING(exportTextXcl, "Xclamation"),
 XcR_STRING(exportTextASCII, "ASCII"),
 XcR_STRING(exportTextHTML, "HTML"),
  
 XcR_STRING(greetings, "Greetings to Marc BLET"),
 XcR_STRING(authors, "Stephane BOISSONAntoine BUATRobin CASTANIEREmmanuel PARIS"),

 XcR_STRING(setupPrinters, "x"),
 XcR_STRING(setupPaths, "x"),

 XcR_STRING(helpVectorTexts, "x"),
 XcR_STRING(helpNotFound, "x %s"),

  
 XcR_STRING(keyFirst, "x %s"),
 XcR_STRING(keySecond, "x %s"),
 XcR_STRING(noKeyFirst, "x"),
 XcR_STRING(noKeySecond, "x"),
 XcR_STRING(noVKeyFirst, "x"),
 XcR_STRING(noVKeySecond, "x"),
 XcR_STRING(stationFullKeyFirst, "x"),
 XcR_STRING(stationFullKeySecond, "x"),
 XcR_STRING(serverFullKeyFirst, "x"),
 XcR_STRING(serverFullKeySecond, "x"),
  
 XcR_STRING(functionNeedKey, "x"),
 XcR_STRING(autosaveNeedKey, "x"),
 XcR_STRING(aboutLicense, "x %s\n%s"),
 XcR_STRING(aboutNoLicense, "x"),

 XcR_STRING(licenseCommercial, "x"),
 XcR_STRING(licensePersonal, "x"),
   
 XcR_STRING(licenseTextOk, "x"),
 XcR_STRING(licenseTextNo, "x"),
 XcR_STRING(licenseTextTK, "x"),
  
 XcR_STRING(licenseFileName, "LICENSE"),
 XcR_STRING(registrationFileName, "REGISTER"),

 XcR_STRING(mailingList, "x"),
 XcR_STRING(font_name, "x %s"),

 XcR_STRING(styleBold, "x"),
 XcR_STRING(styleItalic, "x"),
 XcR_STRING(styleUnderline, "x"),
 XcR_STRING(styleThickness, "x"),
 XcR_STRING(styleSimple, "x"),
 XcR_STRING(styleDouble, "x"),
 XcR_STRING(styleTriple, "x"),
 XcR_STRING(styleNoSpace, "x"),
 XcR_STRING(styleStrikeout, "x"),
 XcR_STRING(styleShadow, "x"),
 XcR_STRING(styleWidth, "x"),
 XcR_STRING(styleSubscript, "x"),
 XcR_STRING(styleScale, "x"),
 XcR_STRING(styleSupscript, "x"),
 XcR_STRING(styleSmallCaps, "x"),
 XcR_STRING(styleBigCaps, "x"),

 XcR_STRING(authorTitle, "x"),
 
 XcR_FONT(pagerFont, "-*-helvetica-*-*-*-*-10-*-*-*-*-*-*-*"),
 XcR_FONT(iconFont, "8x13"),
 XcR_FONT(colorlistFont, "-*-helvetica-*-*-*-*-10-*-*-*-*-*-*-*"),
 XcR_FONT(bubbleFont, "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*"),
 XcR_FONT(loadingMessageFont, "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*"),

 XcR_INT(keepFreeColor, 10),
 XcR_INT(opaqueMove, 10),

};
/* ------------------------------------------------------------------- */
#endif
