/*
** Resource_WORD.h for XAllWrite in Resource/
** Resource definition for XAllWrite
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
** Last update Fri Feb  4 11:09:43 2000 Emmanuel Paris
*/

#ifndef Resource_WORD_h
#define Resource_WORD_h

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
  
 XcR_importDocument,
 XcR_importDocText,

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
 XcR_helpMiscUndo,
 XcR_helpMiscRedo,
 /*--- FONTS ---*/
 XcR_helpFont,
 XcR_helpFontBold,
 XcR_helpFontItalic,
 XcR_helpFontUnderline,
 XcR_helpFontStroke,
 XcR_helpFontShadow,
 XcR_helpFontSubscript,
 XcR_helpFontSupscript,
 XcR_helpFontSmallcaps,
 /*--- HelpLine RULERS tools---*/
 XcR_helpRuler,
 XcR_helpRulerLeft,
 XcR_helpRulerCenter,
 XcR_helpRulerRight,
 XcR_helpRulerJustify,
 XcR_helpRulerIndentDec,
 XcR_helpRulerIndentInc,
 XcR_helpRulerInterlineInc,
 XcR_helpRulerInterlineDec,
 XcR_helpRulerInterparInc,
 XcR_helpRulerInterparDec,
 XcR_helpRulerSpecialChar,
 /*--- HelpLine FRAME tools ---*/
 XcR_helpFrame,
 XcR_helpFrameSelect,
 XcR_helpFrameSquare,
 XcR_helpFrameCircle,
 XcR_helpFramePolygon,
 XcR_helpFrameMovePoint,
 XcR_helpFrameAddPoint,
 XcR_helpFrameSubPoint,
 XcR_helpFramePlanFirst,
 XcR_helpFramePlanLast,
 XcR_helpFramePlanUp,
 XcR_helpFramePlanDown,
 XcR_helpFrameTFaround,
 XcR_helpFrameTFleft,
 XcR_helpFrameTFright,
 XcR_helpFrameTFskip,
 XcR_helpFrameTFhole,
 XcR_helpFrameTFunder,
 XcR_helpFrameTFover,
 XcR_helpFrameLock,
 /*--- HelpLine IMAGE tools---*/
 XcR_helpPicture,
 XcR_helpPictureMove,
 XcR_helpPictureCenter,
 XcR_helpPictureReset,
 XcR_helpPictureVFlip, 
 XcR_helpPictureHFlip,
 XcR_helpPictureNormal,
 XcR_helpPictureBest,
 XcR_helpPictureMax,
 XcR_helpPictureStretch,
 /*--- HelpLine ZOOM tools ---*/
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

 XcR_helpTrash,

 /*--- HelpLine FRAME Functions  ---*/
 XcR_helpFFrameSelect,
 XcR_helpFFrameSquare,
 XcR_helpFFrameCircle,
 XcR_helpFFramePolygon,
 XcR_helpFFrameMovePoint,
 XcR_helpFFrameAddPoint,
 XcR_helpFFrameSubPoint,

 /*--- HelpLine IMAGE Functions ---*/
 XcR_helpFPictureMove,

 /*--- HelpLine ZOOM Functions ---*/
 XcR_helpFZoomIn,
 XcR_helpFZoomOut,
 XcR_helpFZoomZone,
        
 /*--- HelpLine ContextMenu Cadre vide---*/
 XcR_helpCMCdeImpImage,
 XcR_helpCMCdeImpVector,

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
 XcR_pageBreakName,

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
 XcR_aboutLicenseDate,

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
  
 /*--- Undo func name ---*/
 XcR_NoUndo,
 XcR_Undo,
 XcR_Redo,
 XcR_UCut,
 XcR_UPaste,
 XcR_UDelete,
 XcR_UBold,
 XcR_UItalic,
 XcR_UUnderline,
 XcR_UStrikeout,
 XcR_UShadow,
 XcR_USubscript,
 XcR_USupscript,
 XcR_USmallcaps,
 XcR_UBigcaps,
 XcR_UFincsize,
 XcR_UFdecsize,
 XcR_UFfamily,
 XcR_UFcolor,
 XcR_UFbgcolor,
 XcR_UFsize,
 XcR_UFstyle,
 XcR_UCList,
 XcR_USList,
 XcR_USApply,
 
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
 XcR_logoEditor,
 XcR_INT_COUNT			/* must be last */
};

#endif /* Resource_WORD_h */

#ifdef _Resource_WORD_PRIVATE_

/* ------------------------------------------------------------------- */
static XtResource resources[]=
{
 XcR_STRING(memoryError, "Out of memory for %ld bytes"),
 XcR_STRING(loadingError, "Loading error for `%s'"),
 XcR_STRING(editColors, "x"),
 XcR_STRING(frameAttribs, "x"),
 XcR_STRING(editStyles, "x"),
 XcR_STRING(editRulers, "x"),
 XcR_STRING(aboutTitle, "XAllWrite® - Word Processor"),
 XcR_STRING(aboutComments, "EMail: contact@axene.org\nBug-report: support@axene.org\nhttp://www.axene.org/\nftp://ftp.axene.org/"),
 XcR_STRING(aboutEnd, "©%d-%d Axene."),
#ifdef PL
 XcR_STRING(aboutReleaseBeta, "XAllWrite® %d.%d.%d"),
#else
 XcR_STRING(aboutRelease, "XAllWrite® %d.%d"),
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

 XcR_STRING(importDocument, "x"),
 XcR_STRING(importDocText, "x"),

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
 XcR_STRING(helpMiscUndo, "%s"),
 XcR_STRING(helpMiscRedo, "%s"),
 /*--- HelpLine FONTS tools---*/
 XcR_STRING(helpFont, "x"),
 XcR_STRING(helpFontBold, "x"),
 XcR_STRING(helpFontItalic, "x"),
 XcR_STRING(helpFontUnderline, "x"),
 XcR_STRING(helpFontStroke, "x"),
 XcR_STRING(helpFontShadow, "x"),
 XcR_STRING(helpFontSubscript, "x"),
 XcR_STRING(helpFontSupscript, "x"),
 XcR_STRING(helpFontSmallcaps, "x"),
 /*--- HelpLine RULERS tools---*/
 XcR_STRING(helpRuler, "x"),
 XcR_STRING(helpRulerLeft, "x"),
 XcR_STRING(helpRulerCenter, "x"),
 XcR_STRING(helpRulerRight, "x"),
 XcR_STRING(helpRulerJustify, "x"),
 XcR_STRING(helpRulerIndentDec, "x"),
 XcR_STRING(helpRulerIndentInc, "x"),
 XcR_STRING(helpRulerInterlineInc, "x"),
 XcR_STRING(helpRulerInterlineDec, "x"),
 XcR_STRING(helpRulerInterparInc, "x"),
 XcR_STRING(helpRulerInterparDec, "x"),
 XcR_STRING(helpRulerSpecialChar, "x"),
 /*--- HelpLine FRAME tools ---*/
 XcR_STRING(helpFrame, "x"),
 XcR_STRING(helpFrameSelect, "x"),
 XcR_STRING(helpFrameSquare, "x"),
 XcR_STRING(helpFrameCircle, "x"),
 XcR_STRING(helpFramePolygon, "x"),
 XcR_STRING(helpFrameMovePoint, "x"),
 XcR_STRING(helpFrameAddPoint, "x"),
 XcR_STRING(helpFrameSubPoint, "x"),
 XcR_STRING(helpFramePlanFirst, "x"),
 XcR_STRING(helpFramePlanLast, "x"),
 XcR_STRING(helpFramePlanUp, "x"),
 XcR_STRING(helpFramePlanDown, "x"),
 XcR_STRING(helpFrameTFaround, "x"),
 XcR_STRING(helpFrameTFleft, "x"),
 XcR_STRING(helpFrameTFright, "x"),
 XcR_STRING(helpFrameTFskip, "x"),
 XcR_STRING(helpFrameTFhole, "x"),
 XcR_STRING(helpFrameTFunder, "x"),
 XcR_STRING(helpFrameTFover, "x"),
 XcR_STRING(helpFrameLock, "x"),
 /*--- HelpLine IMAGE tools ---*/
 XcR_STRING(helpPicture, "x"),
 XcR_STRING(helpPictureMove, "x"),
 XcR_STRING(helpPictureCenter, "x"),
 XcR_STRING(helpPictureReset, "x"),
 XcR_STRING(helpPictureVFlip, "x"),
 XcR_STRING(helpPictureHFlip, "x"),
 XcR_STRING(helpPictureNormal, "x"),
 XcR_STRING(helpPictureBest, "x"),
 XcR_STRING(helpPictureMax, "x"),
 XcR_STRING(helpPictureStretch, "x"),
 /*--- HelpLine ZOOM tools ---*/
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

 XcR_STRING(helpTrash, "x"),

 /*--- HelpLine Fonctions Cadre ---*/
 XcR_STRING(helpFFrameSelect, "x"),
 XcR_STRING(helpFFrameSquare, "x"),
 XcR_STRING(helpFFrameCircle, "x"),
 XcR_STRING(helpFFramePolygon, "x"),
 XcR_STRING(helpFFrameMovePoint, "x"),
 XcR_STRING(helpFFrameAddPoint, "x"),
 XcR_STRING(helpFFrameSubPoint, "x"),

 /*--- HelpLine Fonctions Image ---*/
 XcR_STRING(helpFPictureMove, "x"),

 /*--- HelpLine Fonctions Zoom ---*/
 XcR_STRING(helpFZoomIn, "x"),
 XcR_STRING(helpFZoomOut, "x"),
 XcR_STRING(helpFZoomZone, "x"),
  
 /*--- HelpLine ContextMenu Cadre vide---*/
 XcR_STRING(helpCMCdeImpImage, "x"),
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
 XcR_STRING(pageBreakName, "page break"),
  
 XcR_STRING(ownColormap, "No"),
 XcR_STRING(opaqueResize, "No"),
 XcR_STRING(helpBubbles, "Classic"),

 XcR_STRING(deadlineDate, "x %d/%d/%d"),
 XcR_STRING(deadlineExpired, "x"),
 XcR_STRING(deadlineExpired2, "x"),
 
 XcR_STRING(exportText, "Export text"),
 XcR_STRING(exportTextXcl, "XAllWrite"),
 XcR_STRING(exportTextASCII, "ASCII"),
 XcR_STRING(exportTextHTML, "HTML"),
  
 XcR_STRING(authors, "Stephane BOISSON, Antoine BUAT, Robin CASTANIER, Emmanuel PARIS"),

 XcR_STRING(setupPrinters, "x"),
 XcR_STRING(setupPaths, "x"),

 XcR_STRING(helpVectorTexts, "x"),
 XcR_STRING(helpNotFound, "x"),

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
 XcR_STRING(aboutLicenseDate, "x %s (%d/%d/%d)\n%s"),
 
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

 XcR_STRING(NoUndo, "Undo"),
 XcR_STRING(Undo, "Undo %s"),
 XcR_STRING(Redo, "Redo %s"),
 XcR_STRING(UCut, "cut"),
 XcR_STRING(UPaste, "paste"),
 XcR_STRING(UDelete, "delete"),
 XcR_STRING(UBold, "bold"),
 XcR_STRING(UItalic, "italic"),
 XcR_STRING(UUnderline, "underline"),
 XcR_STRING(UStrikeout, "strikeout"),
 XcR_STRING(UShadow, "shadow"),
 XcR_STRING(USubscript, "subscript"),
 XcR_STRING(USupscript, "supscript"),
 XcR_STRING(USmallcaps, "small capital"),
 XcR_STRING(UBigcaps, "big capital"),
 XcR_STRING(UFincsize, "increase font size"),
 XcR_STRING(UFdecsize, "decrement font size"),
 XcR_STRING(UFfamily, "font family"),
 XcR_STRING(UFcolor, "font color"),
 XcR_STRING(UFbgcolor, "font background color"),
 XcR_STRING(UFsize, "font size"),
 XcR_STRING(UFstyle, "font style"),
 XcR_STRING(UCList, "color list"),
 XcR_STRING(USList, "style list"),
 XcR_STRING(USApply, "apply style"),

 XcR_FONT(pagerFont, "-*-helvetica-*-*-*-*-10-*-*-*-*-*-*-*"),
 XcR_FONT(iconFont, "8x13"),
 XcR_FONT(colorlistFont, "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*"),
 XcR_FONT(bubbleFont, "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*"),
 XcR_FONT(loadingMessageFont, "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*"),

 XcR_INT(keepFreeColor, 10),
 XcR_INT(opaqueMove, 10),
 XcR_INT(logoEditor, 1),
};
/* ------------------------------------------------------------------- */
#endif
