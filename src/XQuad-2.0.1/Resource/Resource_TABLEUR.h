/*
** Resource_TABLEUR.h for XQuad in Resource/
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
** Last update Fri Feb  4 11:38:26 2000 Emmanuel Paris
*/

#ifndef Resource_TABLEUR_h
#define Resource_TABLEUR_h

#include "version.h"

/*___String Resources___*/

enum 
{
 XcR_memoryError,
 XcR_loadingError,
 XcR_frameAttribs,
 XcR_aboutTitle,
 XcR_aboutComments,
 XcR_aboutEnd,
#ifdef PL
 XcR_aboutReleaseBeta,
#else
 XcR_aboutRelease,
#endif
 XcR_editColors,
 XcR_editStyles,
 XcR_styleNumbers,
 XcR_loadDocument,
 XcR_saveDocument,
 XcR_printDocument,
 XcR_quitMessage,
 XcR_askGraphRow,
 XcR_askGraphCol,
 XcR_askGraphRow2,
 XcR_askGraphCol2,
 
 XcR_importDocument,
 XcR_importDocTextSpaceUnix,
 XcR_importDocTextSpaceDos,
 XcR_importDocTextSpaceMac,
 XcR_importDocTextTabUnix,
 XcR_importDocTextTabDos,
 XcR_importDocTextTabMac,
 XcR_importDocTextCSVUnix,
 XcR_importDocTextCSVDos,
 XcR_importDocTextCSVMac,
 
 XcR_exportDocument,
 XcR_exportDocTextSpaceUnix,
 XcR_exportDocTextSpaceDos,
 XcR_exportDocTextSpaceMac,
 XcR_exportDocTextTabUnix,
 XcR_exportDocTextTabDos,
 XcR_exportDocTextTabMac,
 XcR_exportDocTextCSVUnix,
 XcR_exportDocTextCSVDos,
 XcR_exportDocTextCSVMac,
 
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
 XcR_dateMonthVeryShort,
 
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
 XcR_dateDayVeryShort,
 
 XcR_dateYearBeforeJC,
 XcR_dateYearAfterJC,
 
 /*--- Hours ----*/
 XcR_timeHourAM,
 XcR_timeHourPM,
 
 /*--- Formula Error messages ---*/
 XcR_formulaCircError,
 XcR_formulaValError,
 XcR_formulaNameError,
 XcR_formulaCharError,
 XcR_formulaCalcError,
 XcR_formulaDivzError,
 XcR_formulaParamLessError,
 XcR_formulaParamMoreError,
 XcR_formulaCellRangeError,
 XcR_formulaDIntError,
 XcR_formulaDOneOneError,
 XcR_formulaDOverOneError,
 XcR_formulaDOneOneOpenError,
 XcR_formulaDAlleZeroError,
 XcR_formulaDOverZeroError,
 XcR_formulaDPositiveError,
 XcR_formulaMissingParamError,
 XcR_formulaError,

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

 /*--- HelpLine Outils CALCULS ---*/
 
 XcR_helpCalc,
 XcR_helpPasteEqual,
 XcR_helpCalcSumm,
 XcR_helpToolsFunctions,
 XcR_helpToolsThousands, 
 XcR_helpToolsCurrency, 
 XcR_helpToolsPercent, 
 XcR_helpToolsScientific, 
 XcR_helpToolsEngineer, 
 XcR_helpAddDigit, 
 XcR_helpSubDigit, 
 
 /*--- HelpLine Outils FONTES ---*/
 
 XcR_helpFont, 
 XcR_helpToolsBold,
 XcR_helpToolsItalic, 
 XcR_helpToolsUnderline,
 XcR_helpToolsStrikeout,
 XcR_helpToolsShadow,
 XcR_helpToolsSmallCaps,
 XcR_helpToolsBigCaps,
 XcR_helpFontMinus, 
 XcR_helpFontPlus, 
 
 /*--- HelpLine Outils CELLULES ---*/
 
 XcR_helpCells,
 XcR_helpCellsLeft,
 XcR_helpCellsCenter,
 XcR_helpCellsRight,
 XcR_helpCellsHJustify,
 XcR_helpCellsVJustify,
 XcR_helpCellsTop,
 XcR_helpCellsBottom,
 XcR_helpCellsMultilines,
 XcR_helpCellsAngle90,
 XcR_helpCellsAngle270,
 
 /*--- HelpLine Outils BORDURES ---*/
 
 XcR_helpBorders,
 XcR_helpBordersClean,
 XcR_helpBordersLeft,
 XcR_helpBordersBottom,
 XcR_helpBordersRight,
 XcR_helpBordersTop,
 XcR_helpBordersOutline,
 XcR_helpBackgroundDefault,
 XcR_helpVideoReverse,
 
 /*--- HelpLine Outils GRAPHIQUES ---*/
 
 XcR_helpGraph,
 XcR_helpFrameSelect,
 XcR_helpFrameCreate,
 XcR_helpFramePlanFirst,
 XcR_helpFramePlanLast,
 XcR_helpFramePlanUp,
 XcR_helpFramePlanDown,
 XcR_helpGraphVBar,
 XcR_helpGraphHBar,
 XcR_helpGraphLine,
 XcR_helpGraphArea,
 XcR_helpGraphPie,
 XcR_helpGraphRadar,
 XcR_helpFrameLock,
 XcR_helpGraphAxes,
 XcR_helpGraphLegend,
 XcR_helpGraphTitle,
 XcR_helpGraphSetup,
 XcR_helpGraphAbs,
 XcR_helpGraphNames,
 
 /*--- HelpLine Outils ZOOM ---*/
 
 XcR_helpZoom,
 XcR_helpZoomIn,
 XcR_helpZoomOut,
 XcR_helpZoom50,
 XcR_helpZoom75,
 XcR_helpZoom100,
 XcR_helpZoom200,
 XcR_helpZoom400,
 
 XcR_helpTrash,
 
 XcR_helpEditbarOk,
 XcR_helpEditbarEsc,
 
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
 
 /*--- Standard names ---*/
 XcR_defaultDocumentName,
 XcR_defaultFrameName,
 XcR_defaultSheetName,
 
 XcR_automaticColor,
 XcR_ownColormap,
 XcR_opaqueResize,
 XcR_helpBubbles,
 
 XcR_deadlineDate,
 XcR_deadlineExpired,
 XcR_deadlineExpired2,

 XcR_true,
 XcR_false,
 
 /*--- Functions ---*/
 
 XcR_funcAbs,
 XcR_funcAcos,
 XcR_funcAcosh,
 XcR_funcAdress,
 XcR_funcAlea,
 XcR_funcAleab,
 XcR_funcSln,
 XcR_funcRound,
 XcR_funcAsin,
 XcR_funcAsinh,
 XcR_funcAtan,
 XcR_funcAtan2,
 XcR_funcAtanh,
 XcR_funcBase,
 XcR_funcBesselj,
 XcR_funcBesselk,
 XcR_funcChar,
 XcR_funcCeil,
 XcR_funcFind,
 XcR_funcCnum,
 XcR_funcCol,
 XcR_funcCode,
 XcR_funcCos,
 XcR_funcCosh,
 XcR_funcCount,
 XcR_funcCountn,
 XcR_funcCtxt,
 XcR_funcDegrees,
 XcR_funcRight,
 XcR_funcE,
 XcR_funcInt,
 XcR_funcPurge,
 XcR_funcAnd,
 XcR_funcExact,
 XcR_funcExp,
 XcR_funcFact,
 XcR_funcFalse,
 XcR_funcFloor,
 XcR_funcFrac,
 XcR_funcCurrency,
 XcR_funcLeft,
 XcR_funcIndirect,
 XcR_funcInv,
 XcR_funcRow,
 XcR_funcLn,
 XcR_funcLog,
 XcR_funcLog10,
 XcR_funcUpper,
 XcR_funcMax,
 XcR_funcMin,
 XcR_funcLower,
 XcR_funcMod,
 XcR_funcAvg,
 XcR_funcLength,
 XcR_funcProper,
 XcR_funcPi,
 XcR_funcProd,
 XcR_funcSqrt,
 XcR_funcRandom,
 XcR_funcReplace,
 XcR_funcRepeat,
 XcR_funcSign,
 XcR_funcSin,
 XcR_funcSinh,
 XcR_funcSumm,
 XcR_funcSqr,
 XcR_funcMid,
 XcR_funcSubstitute,
 XcR_funcTrim,
 XcR_funcRadians,
 XcR_funcT,
 XcR_funcTan,
 XcR_funcTanh,
 XcR_funcText,
 XcR_funcTrunc,
 XcR_funcMatch,
 XcR_funcTrue,
 XcR_funcConcat,
 
 XcR_funcDay360,
 XcR_funcToday,
 XcR_funcDate,
 XcR_funcDateval,
 XcR_funcNow,
 XcR_funcTime,
 XcR_funcTimeval,
 XcR_funcYear,
 XcR_funcDay,
 XcR_funcDayweek,
 XcR_funcHour,
 XcR_funcMinute,
 XcR_funcMonth,
 XcR_funcSecond,
 XcR_funcMillisecond,
 
 XcR_funcDb,
 XcR_funcDdb,
 XcR_funcIpmt,
 XcR_funcNper,
 XcR_funcPpmt,
 XcR_funcSyd,
 XcR_funcRate,
 XcR_funcIrr,
 XcR_funcMirr,
 XcR_funcPv,
 XcR_funcNpv,
 XcR_funcFv,
 XcR_funcVdb,
 XcR_funcPmt,

 XcR_funcNot,
 XcR_funcOr,
 XcR_funcIf,
 
 /*--- Functions Aliases ---*/
 
 XcR_funcColAlias1,
 XcR_funcBesselkAlias1,
 XcR_funcBesseljAlias1,
 XcR_funcIntAlias1,
 XcR_funcFactAlias1,
 XcR_funcFracAlias1,
 XcR_funcInvAlias1,
 XcR_funcMaxAlias1,
 XcR_funcMinAlias1,
 XcR_funcModAlias1,
 XcR_funcAvgAlias1,
 XcR_funcProdAlias1,
 XcR_funcSummAlias1,
 
 XcR_BnFmtAll,
 XcR_BnFmtNormal,
 XcR_BnFmtUnit,
 XcR_BnFmtExponantial,
 XcR_BnFmtPercent,
 XcR_BnFmtFrac,
 XcR_BnFmtBoolean,
 XcR_BnFmtDay,
 XcR_BnFmtMonth,
 XcR_BnFmtTime,
 XcR_BnFmtDate,
 XcR_BnFmtDateTime,
 
 XcR_BnColPositive,
 XcR_BnColNegative,
 XcR_BnColTrue,
 XcR_BnColFalse,
 
 XcR_greetings,
 XcR_authors,
 
 XcR_dateEntryMode,
 
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
 
 XcR_STRING_COUNT		/* must be last */
};

/*___Font Resources___*/

enum 
{
 XcR_iconFont,
 XcR_positionFont,
 XcR_classorFont,
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

#endif /* Resource_TABLEUR_h */

#ifdef _Resource_TABLEUR_PRIVATE_

/* ------------------------------------------------------------------- */
static XtResource resources[]=
{
 XcR_STRING(memoryError, "Out of memory for %ld bytes"),
 XcR_STRING(loadingError, "Loading error for `%s'"),
 XcR_STRING(frameAttribs, "Frame Attribs"),
 XcR_STRING(aboutTitle, "XQuad® - SpreadSheet"),
 XcR_STRING(aboutComments, "EMail: contact@axene.org\nBug-report: support@axene.org\nhttp://www.axene.org/\nftp://ftp.axene.org/"),
 XcR_STRING(aboutEnd, "©%d-%d Axene."),
#ifdef PL
 XcR_STRING(aboutReleaseBeta, "XQuad® %d.%d.%d"),
#else
 XcR_STRING(aboutRelease, "XQuad® %d.%d"),
#endif
 XcR_STRING(editColors, "x"),
 XcR_STRING(editStyles, "x"),
 XcR_STRING(styleNumbers, "x"),
 XcR_STRING(loadDocument, "x"),
 XcR_STRING(saveDocument, "x"),
 XcR_STRING(printDocument, "x"),
 XcR_STRING(quitMessage, "x"),
 XcR_STRING(askGraphRow, "x"),
 XcR_STRING(askGraphCol, "x"),
 XcR_STRING(askGraphRow2, "x"),
 XcR_STRING(askGraphCol2, "x"),
 
 XcR_STRING(importDocument, "x"),
 XcR_STRING(importDocTextSpaceUnix, "x"),
 XcR_STRING(importDocTextSpaceDos, "x"),
 XcR_STRING(importDocTextSpaceMac, "x"),
 XcR_STRING(importDocTextTabUnix, "x"),
 XcR_STRING(importDocTextTabDos, "x"),
 XcR_STRING(importDocTextTabMac, "x"),
 XcR_STRING(importDocTextCSVUnix, "x"),
 XcR_STRING(importDocTextCSVDos, "x"),
 XcR_STRING(importDocTextCSVMac, "x"),
 
 XcR_STRING(exportDocument, "x"),
 XcR_STRING(exportDocTextSpaceUnix, "x"),
 XcR_STRING(exportDocTextSpaceDos, "x"),
 XcR_STRING(exportDocTextSpaceMac, "x"),
 XcR_STRING(exportDocTextTabUnix, "x"),
 XcR_STRING(exportDocTextTabDos, "x"),
 XcR_STRING(exportDocTextTabMac, "x"),
 XcR_STRING(exportDocTextCSVUnix, "x"),
 XcR_STRING(exportDocTextCSVDos, "x"),
 XcR_STRING(exportDocTextCSVMac, "x"),
 
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
 XcR_STRING(dateMonthVeryShort, "Ja F Mr Ap My Ju Jy Au S O N D"),  
 
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
 XcR_STRING(dateDayVeryShort, "M Tu W Th F Sa Su"),
  
 XcR_STRING(dateYearBeforeJC, "B.C."),
 XcR_STRING(dateYearAfterJC, "A.D."),
 
 /*--- Hours ----*/
 XcR_STRING(timeHourAM, "AM"),
 XcR_STRING(timeHourPM, "PM"),
 
 /*--- Formula Error messages ---*/
 XcR_STRING(formulaCircError, "!CIRCULAR DEPENDENCY"),
 XcR_STRING(formulaValError, "!INCORRECT TYPE OF VALUE"),
 XcR_STRING(formulaNameError, "!UNKNOWN FUNCTION"),
 XcR_STRING(formulaCharError, "!UNKNOWN CHAR"),
 XcR_STRING(formulaCalcError, "!CALCULATION ERROR"),
 XcR_STRING(formulaDivzError, "!DIVISION BY ZERO"),
 XcR_STRING(formulaParamLessError, "!NOT ENOUGH PARAMETER"),
 XcR_STRING(formulaParamMoreError, "!TOO MANY PARAMETER"),
 XcR_STRING(formulaCellRangeError, "!CELL OUT OF RANGE"),
 XcR_STRING(formulaDIntError, "!INTEGER PARAMETER REQUIRED"),
 XcR_STRING(formulaDOneOneError, "!PARAMETER NOT IN [-1,1]"),
 XcR_STRING(formulaDOverOneError, "!PARAMETER NOT >= 1"),
 XcR_STRING(formulaDOneOneOpenError, "!PARAMETER NOT IN ]-1,1["),
 XcR_STRING(formulaDAlleZeroError, "!PARAMETER NOT IN R/{0}"),
 XcR_STRING(formulaDOverZeroError, "!PARAMETER NOT > 0"),
 XcR_STRING(formulaDPositiveError, "!PARAMETER NOT >= 0"),
 XcR_STRING(formulaMissingParamError, "!MISSING PARENTHESIS"),
 XcR_STRING(formulaError, "!FORMULA ERROR"),
 
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

 /*--- HelpLine Outils CALCULS & TOOLS ---*/
 
 XcR_STRING(helpCalc, "x"),
 XcR_STRING(helpPasteEqual, "x"),
 XcR_STRING(helpCalcSumm, "x"),
 XcR_STRING(helpToolsFunctions, "x"),
 XcR_STRING(helpToolsThousands, "x"),
 XcR_STRING(helpToolsCurrency, "x"),
 XcR_STRING(helpToolsPercent, "x"),
 XcR_STRING(helpToolsScientific, "x"),
 XcR_STRING(helpToolsEngineer, "x"),
 XcR_STRING(helpAddDigit, "x"),
 XcR_STRING(helpSubDigit, "x"),
 
 /*--- HelpLine Outils FONTES ---*/
 
 XcR_STRING(helpFont, "x"),
 XcR_STRING(helpToolsBold, "x"),
 XcR_STRING(helpToolsItalic, "x"),
 XcR_STRING(helpToolsUnderline, "x"),
 XcR_STRING(helpToolsStrikeout, "x"),
 XcR_STRING(helpToolsShadow, "x"),
 XcR_STRING(helpToolsSmallCaps, "x"),
 XcR_STRING(helpToolsBigCaps, "x"),
 XcR_STRING(helpFontMinus, "x"),
 XcR_STRING(helpFontPlus, "x"),
 
 /*--- HelpLine Outils CELLULES ---*/
 
 XcR_STRING(helpCells, "x"),
 XcR_STRING(helpCellsLeft, "x"),
 XcR_STRING(helpCellsCenter, "x"),
 XcR_STRING(helpCellsRight, "x"),
 XcR_STRING(helpCellsHJustify, "x"),
 XcR_STRING(helpCellsVJustify, "x"),
 XcR_STRING(helpCellsTop, "x"),
 XcR_STRING(helpCellsBottom, "x"),
 XcR_STRING(helpCellsMultilines, "x"),
 XcR_STRING(helpCellsAngle90, "x"),
 XcR_STRING(helpCellsAngle270, "x"),
 
 /*--- HelpLine Outils BORDURES ---*/
 
 XcR_STRING(helpBorders, "x"),
 XcR_STRING(helpBordersClean, "x"),
 XcR_STRING(helpBordersLeft, "x"),
 XcR_STRING(helpBordersBottom, "x"),
 XcR_STRING(helpBordersRight, "x"),
 XcR_STRING(helpBordersTop, "x"),
 XcR_STRING(helpBordersOutline, "x"),
 XcR_STRING(helpBackgroundDefault, "x"),
 XcR_STRING(helpVideoReverse, "x"),
 
 /*--- HelpLine Outils GRAPHIQUES ---*/
 
 XcR_STRING(helpGraph, "x"),
 XcR_STRING(helpFrameSelect, "x"),
 XcR_STRING(helpFrameCreate, "x"),
 XcR_STRING(helpFramePlanFirst, "x"),
 XcR_STRING(helpFramePlanLast, "x"),
 XcR_STRING(helpFramePlanUp, "x"),
 XcR_STRING(helpFramePlanDown, "x"),
 XcR_STRING(helpGraphVBar, "x"),
 XcR_STRING(helpGraphHBar, "x"),
 XcR_STRING(helpGraphLine, "x"),
 XcR_STRING(helpGraphArea, "x"),
 XcR_STRING(helpGraphPie, "x"),
 XcR_STRING(helpGraphRadar, "x"),
 XcR_STRING(helpFrameLock, "x"),
 XcR_STRING(helpGraphAxes, "x"),
 XcR_STRING(helpGraphLegend, "x"),
 XcR_STRING(helpGraphTitle, "x"),
 XcR_STRING(helpGraphSetup, "x"),
 XcR_STRING(helpGraphAbs, "x"),
 XcR_STRING(helpGraphNames, "x"),
 
 /*--- HelpLine Outils ZOOM ---*/
 
 XcR_STRING(helpZoom, "x"),
 XcR_STRING(helpZoomIn, "x"),
 XcR_STRING(helpZoomOut, "x"),
 XcR_STRING(helpZoom50, "x"),
 XcR_STRING(helpZoom75, "x"),
 XcR_STRING(helpZoom100, "x"),
 XcR_STRING(helpZoom200, "x"),
 XcR_STRING(helpZoom400, "x"),
 
 XcR_STRING(helpTrash, "x"),
 
 XcR_STRING(helpEditbarOk, "x"),
 XcR_STRING(helpEditbarEsc, "x"),
 
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
 /*--- Error messages ---*/
 XcR_STRING(loadError, "x"),
 
 /*--- Standard names ---*/
 XcR_STRING(defaultDocumentName, "Untitled"),
 XcR_STRING(defaultFrameName, "Frame"),
 XcR_STRING(defaultSheetName, "Sheet"),
 
 XcR_STRING(automaticColor, "Automatic"),
 XcR_STRING(ownColormap, "No"),
 XcR_STRING(opaqueResize, "No"),
 XcR_STRING(helpBubbles, "Classic"),

 XcR_STRING(deadlineDate, "x %d/%d/%d"),
 XcR_STRING(deadlineExpired, "x"),
 XcR_STRING(deadlineExpired2, "x"),
 
 XcR_STRING(true, "TRUE"),
 XcR_STRING(false, "FALSE"),
 
 /*--- Functions ---*/
 
 XcR_STRING(funcAbs, "abs"),
 XcR_STRING(funcAcos, "acos"),
 XcR_STRING(funcAcosh, "acosh"),
 XcR_STRING(funcAdress, "address"),
 XcR_STRING(funcAlea, "alea"),
 XcR_STRING(funcAleab, "aleab"),
 XcR_STRING(funcSln, "sln"),
 XcR_STRING(funcRound, "round"),
 XcR_STRING(funcAsin, "asin"),
 XcR_STRING(funcAsinh, "asinh"),
 XcR_STRING(funcAtan, "atan"),
 XcR_STRING(funcAtan2, "atan2"),
 XcR_STRING(funcAtanh, "atanh"),
 XcR_STRING(funcBase, "base"),
 XcR_STRING(funcBesselj, "besselj"),
 XcR_STRING(funcBesselk, "besselk"),
 XcR_STRING(funcChar, "char"),
 XcR_STRING(funcCeil, "ceil"),
 XcR_STRING(funcFind, "find"),
 XcR_STRING(funcCnum, "cnum"),
 XcR_STRING(funcCol, "col"),
 XcR_STRING(funcCode, "code"),
 XcR_STRING(funcCos, "cos"),
 XcR_STRING(funcCosh, "cosh"),
 XcR_STRING(funcCount, "count"),
 XcR_STRING(funcCountn, "countn"),
 XcR_STRING(funcCtxt, "ctxt"),
 XcR_STRING(funcDegrees, "degrees"),
 XcR_STRING(funcRight, "right"),
 XcR_STRING(funcE, "e"),
 XcR_STRING(funcInt, "int"),
 XcR_STRING(funcPurge, "purge"),
 XcR_STRING(funcAnd, "and"),
 XcR_STRING(funcExact, "exact"),
 XcR_STRING(funcExp, "exp"),
 XcR_STRING(funcFact, "fact"),
 XcR_STRING(funcFalse, "false"),
 XcR_STRING(funcFloor, "floor"),
 XcR_STRING(funcFrac, "frac"),
 XcR_STRING(funcCurrency, "currency"),
 XcR_STRING(funcLeft, "left"),
 XcR_STRING(funcIndirect, "indirect"),
 XcR_STRING(funcInv, "inv"),
 XcR_STRING(funcRow, "row"),
 XcR_STRING(funcLn, "ln"),
 XcR_STRING(funcLog, "log"),
 XcR_STRING(funcLog10, "log10"),
 XcR_STRING(funcUpper, "upper"),
 XcR_STRING(funcMax, "max"),
 XcR_STRING(funcMin, "min"),
 XcR_STRING(funcLower, "lower"),
 XcR_STRING(funcMod, "mod"),
 XcR_STRING(funcAvg, "avg"),
 XcR_STRING(funcLength, "length"),
 XcR_STRING(funcProper, "proper"),
 XcR_STRING(funcPi, "pi"),
 XcR_STRING(funcProd, "prod"),
 XcR_STRING(funcSqrt, "sqrt"),
 XcR_STRING(funcRandom, "random"),
 XcR_STRING(funcReplace, "replace"),
 XcR_STRING(funcRepeat, "repeat"),
 XcR_STRING(funcSign, "sign"),
 XcR_STRING(funcSin, "sin"),
 XcR_STRING(funcSinh, "sinh"),
 XcR_STRING(funcSumm, "sum"),
 XcR_STRING(funcSqr, "sqr"),
 XcR_STRING(funcMid, "mid"),
 XcR_STRING(funcSubstitute, "substitute"),
 XcR_STRING(funcTrim, "trim"),
 XcR_STRING(funcRadians, "radians"),
 XcR_STRING(funcT, "t"),
 XcR_STRING(funcTan, "tan"),
 XcR_STRING(funcTanh, "tanh"),
 XcR_STRING(funcText, "text"),
 XcR_STRING(funcTrunc, "trunc"),
 XcR_STRING(funcMatch, "match"),
 XcR_STRING(funcTrue, "true"),
 XcR_STRING(funcConcat, "concat"),
 
 XcR_STRING(funcDay360, "day360"),
 XcR_STRING(funcToday, "today"),
 XcR_STRING(funcDate, "date"),
 XcR_STRING(funcDateval, "datevalue"),
 XcR_STRING(funcNow, "now"),
 XcR_STRING(funcTime, "time"),
 XcR_STRING(funcTimeval, "timevalue"),
 XcR_STRING(funcYear, "year"),
 XcR_STRING(funcDay, "day"),
 XcR_STRING(funcDayweek, "weekday"),
 XcR_STRING(funcHour, "hour"),
 XcR_STRING(funcMinute, "minute"),
 XcR_STRING(funcMonth, "month"),
 XcR_STRING(funcSecond, "second"),
 XcR_STRING(funcMillisecond, "millisecond"),
 
 XcR_STRING(funcDb, "db"),
 XcR_STRING(funcDdb, "ddb"),
 XcR_STRING(funcIpmt, "ipmt"),
 XcR_STRING(funcNper, "nper"),
 XcR_STRING(funcPpmt, "ppmt"),
 XcR_STRING(funcSyd, "syd"),
 XcR_STRING(funcRate, "rate"),
 XcR_STRING(funcIrr, "irr"),
 XcR_STRING(funcMirr, "mirr"),
 XcR_STRING(funcPv, "pv"),
 XcR_STRING(funcNpv, "npv"),
 XcR_STRING(funcFv, "fv"),
 XcR_STRING(funcVdb, "vdb"),
 XcR_STRING(funcPmt, "pmt"),
 
 XcR_STRING(funcNot, "not"),
 XcR_STRING(funcOr, "or"),
 XcR_STRING(funcIf, "if"),
 
 /*--- Functions Aliases ---*/
 
 XcR_STRING(funcColAlias1, "column"),
 XcR_STRING(funcBesselkAlias1, "kbessel"),
 XcR_STRING(funcBesseljAlias1, "jbessel"),
 XcR_STRING(funcIntAlias1, "integer"),
 XcR_STRING(funcFactAlias1, "factoriel"),
 XcR_STRING(funcFracAlias1, "fraction"),
 XcR_STRING(funcInvAlias1, "invert"),
 XcR_STRING(funcMaxAlias1, "maximum"),
 XcR_STRING(funcMinAlias1, "minimum"),
 XcR_STRING(funcModAlias1, "modulo"),
 XcR_STRING(funcAvgAlias1, "average"),
 XcR_STRING(funcProdAlias1, "product"),
 XcR_STRING(funcSummAlias1, "sum"),
 
 XcR_STRING(BnFmtAll, "All"),
 XcR_STRING(BnFmtNormal, "Normal"),
 XcR_STRING(BnFmtUnit, "Unit"),
 XcR_STRING(BnFmtExponantial, "Scientific"),
 XcR_STRING(BnFmtPercent, "Percent"),
 XcR_STRING(BnFmtFrac, "Fraction"),
 XcR_STRING(BnFmtBoolean, "Boolean"),
 XcR_STRING(BnFmtDay, "Day in week"),
 XcR_STRING(BnFmtMonth, "Month"),
 XcR_STRING(BnFmtTime, "Time"),
 XcR_STRING(BnFmtDate, "Date"),
 XcR_STRING(BnFmtDateTime, "Date + Time"),
 
 XcR_STRING(BnColPositive, "x"),
 XcR_STRING(BnColNegative, "x"),
 XcR_STRING(BnColTrue, "x"),
 XcR_STRING(BnColFalse, "x"),
 
 XcR_STRING(greetings, "x"),
 XcR_STRING(authors, "Stephane BOISSONAntoine BUATRobin CASTANIEREmmanuel PARIS"),

 XcR_STRING(dateEntryMode, "monthfirst"),
 
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
 XcR_STRING(aboutLicenseDate, "x %s (%d/%d/%d)\n%s"),

 XcR_STRING(licenseCommercial, "x"),
 XcR_STRING(licensePersonal, "x"),
 
 XcR_STRING(licenseTextOk, "x"),
 XcR_STRING(licenseTextNo, "x"),
 XcR_STRING(licenseTextTK, "x"),
 
 XcR_STRING(licenseFileName, "LICENSE"),
 XcR_STRING(registrationFileName, "REGISTER"),

 XcR_STRING(mailingList, "x"),
 
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
 
 
 XcR_FONT(iconFont, "8x13"),
 XcR_FONT(positionFont, "-*-helvetica-medium-r-*-*-18-*-*-*-*-*-*-*"),
 XcR_FONT(classorFont, "-*-helvetica-medium-*-*-*-10-*-*-*-*-*-*-*"),
 XcR_FONT(colorlistFont, "-*-helvetica-medium-*-*-*-10-*-*-*-*-*-*-*"),
 XcR_FONT(bubbleFont, "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*"),
 XcR_FONT(loadingMessageFont, "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*"),
 
 XcR_INT(keepFreeColor, 10),
 XcR_INT(opaqueMove, 10),
 
};
/* ------------------------------------------------------------------- */
#endif


