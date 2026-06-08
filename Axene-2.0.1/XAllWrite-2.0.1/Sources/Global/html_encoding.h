/*
** html_encoding.h for Xclamation, XQuad, XAllWrite and XMayday in Global/
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
** Started on  Mon Jul 17 14:15:33 1995 Emmanuel Paris
** Last update Tue Aug 27 21:15:07 1996 Emmanuel Paris
*/

#ifndef _html_encoding_h_
#define _html_encoding_h_

static char *to_html_encoding[256] = {
  XcS_to_unknow,	
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,	/* &#00;-&#08; Unused */
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  "&#09;",		/*  Horizontal tab */
  "&#10;",		/*  Line feed */
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,	/* &#11;-&#31; Unused */
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  " ",
  "!",
  "&quot;",
  "#",
  "$",
  "%",
  "&amp;",
  "'",
  "(",
  ")",
  "*",
  "+",
  ",",
  "-",
  ".",
  "/",
  "0",
  "1",
  "2",
  "3",
  "4",
  "5",
  "6",
  "7",
  "8",
  "9",
  ":",
  ";",
  "&lt;",
  "=",
  "&gt;",
  "?",
  "@",
  "A",
  "B",
  "C",
  "D",
  "E",
  "F",
  "G",
  "H",
  "I",
  "J",
  "K",
  "L",
  "M",
  "N",
  "O",
  "P",
  "Q",
  "R",
  "S",
  "T",
  "U",
  "V",
  "W",
  "X",
  "Y",
  "Z",
  "[",
  "\\",
  "]",
  "^",
  "_",
  "`",
  "a",
  "b",
  "c",
  "d",
  "e",
  "f",
  "g",
  "h",
  "i",
  "j",
  "k",
  "l",
  "m",
  "n",
  "o",
  "p",
  "q",
  "r",
  "s",
  "t",
  "u",
  "v",
  "w",
  "x",
  "y",
  "z",
  "{",
  "|",
  "}",
  "~",
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,	/* &#127;- &#160; Unused */
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  XcS_to_unknow,
  "&nbsp;",		/* Non breakable space */
  "&#161;",		/* Inverted exclamation */
  "&#162;",		/* Cent sign */
  "&#163;",		/* Pound sterling */
  "&#164;",		/* General currency sign */
  "&#165;",		/* Yen sign */
  "&#166;",		/* Broken vertical bar */
  "&#167;",		/* Section sign */
  "&#168;",		/* Umlaut (dieresis) */
  "&copy;",		/* Copyright */
  "&#170;",		/* Feminine ordinal */
  "&#171;",		/* Left angle quote, guillemotleft */
  "&#172;",		/* Not sign */
  "&#173;",		/* Soft hyphen */
  "&reg;",		/* Registered trademark */
  "&#175;",		/* Macron accent */
  "&#176;",		/* Degree sign */
  "&#177;",		/* Plus or minus */
  "&#178;",		/* Superscript two */
  "&#179;",		/* Superscript three */
  "&#180;",		/* Acute accent */
  "&#181;",		/* Micro sign */
  "&#182;",		/* Paragraph sign */
  "&#183;",		/* Middle dot */
  "&#184;",		/* Cedilla */
  "&#185;",		/* Superscript one */
  "&#186;",		/* Masculine ordinal */
  "&#187;",		/* Right angle quote, guillemotright */
  "&#188;",		/* Fraction one-fourth */
  "&#189;",		/* Fraction one-half */
  "&#190;",		/* Fraction three-fourths */
  "&#191;",		/* Inverted question mark */
  "&Agrave;",		/* Capital A, grave accent */
  "&Aacute;",		/* Capital A, acute accent */
  "&Acirc;",		/* Capital A, circumflex accent */
  "&Atilde;",		/* Capital A, tilde */
  "&Auml;",		/* Capital A, dieresis or umlaut mark */
  "&Aring;",		/* Capital A, ring */
  "&AElig;",		/* Capital AE dipthong (ligature) */
  "&Ccedil;",		/* Capital C, cedilla */
  "&Egrave;",		/* Capital E, grave accent */
  "&Eacute;",		/* Capital E, acute accent */
  "&Ecirc;",		/* Capital E, circumflex accent */
  "&Euml;",		/* Capital E, dieresis or umlaut mark */
  "&Igrave;",		/* Capital I, grave accent */
  "&Iacute;",		/* Capital I, acute accent */
  "&Icirc;",		/* Capital I, circumflex accent */
  "&Iuml;",		/* Capital I, dieresis or umlaut mark */
  "&ETH;",		/* Capital Eth, Icelandic */
  "&Ntilde;",		/* Capital N, tilde */
  "&Ograve;",		/* Capital O, grave accent */
  "&Oacute;",		/* Capital O, acute accent */
  "&Ocirc;",		/* Capital O, circumflex accent */
  "&Otilde;",		/* Capital O, tilde */
  "&Ouml;",		/* Capital O, dieresis or umlaut mark */
  "&#215;",		/* Multiply sign */
  "&Oslash;",		/* Capital O, slash */
  "&Ugrave;",		/* Capital U, grave accent */
  "&Uacute;",		/* Capital U, acute accent */
  "&Ucirc;",		/* Capital U, circumflex accent */
  "&Uuml;",		/* Capital U, dieresis or umlaut mark */
  "&Yacute;",		/* Capital Y, acute accent */
  "&THORN;",		/* Capital THORN, Icelandic */
  "&szlig;",		/* Small sharp s, German (sz ligature) */
  "&agrave;",		/* Small a, grave accent */
  "&aacute;",		/* Small a, acute accent */
  "&acirc;",		/* Small a, circumflex accent */
  "&atilde;",		/* Small a, tilde */
  "&auml;",		/* Small a, dieresis or umlaut mark */
  "&aring;",		/* Small a, ring */
  "&aelig;",		/* Small ae dipthong (ligature) */
  "&ccedil;",		/* Small c, cedilla */
  "&egrave;",		/* Small e, grave accent */
  "&eacute;",		/* Small e, acute accent */
  "&ecirc;",		/* Small e, circumflex accent */
  "&euml;",		/* Small e, dieresis or umlaut mark */
  "&igrave;",		/* Small i, grave accent */
  "&iacute;",		/* Small i, acute accent */
  "&icirc;",		/* Small i, circumflex accent */
  "&iuml;",		/* Small i, dieresis or umlaut mark */
  "&eth;",		/* Small eth, Icelandic */
  "&ntilde;",		/* Small n, tilde */
  "&ograve;",		/* Small o, grave accent */
  "&oacute;",		/* Small o, acute accent */
  "&ocirc;",		/* Small o, circumflex accent */
  "&otilde;",		/* Small o, tilde */
  "&ouml;",		/* Small o, dieresis or umlaut mark */
  "&#247;",		/* Division sign */
  "&oslash;",		/* Small o, slash */
  "&ugrave;",		/* Small u, grave accent */
  "&uacute;",		/* Small u, acute accent */
  "&ucirc;",		/* Small u, circumflex accent */
  "&uuml;",		/* Small u, dieresis or umlaut mark */
  "&yacute;",		/* Small y, acute accent */
  "&thorn;",		/* Small thorn, Icelandic */
  "&yuml;"		/* Small y, dieresis or umlaut mark */
};

#endif









