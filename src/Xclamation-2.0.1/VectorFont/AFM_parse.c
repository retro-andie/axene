/*
** AFM_parse.c for Xclamation, XQuad and XAllWrite in VectorFont/
** Parsing of Adobe Font Metrics files
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
** Started on  Fri Jun 10 14:54:27 1994 Stéphane Boisson
** Last update Mon Aug  3 17:16:25 1998 Emmanuel Paris
*/

#define NTRACE

#include "AFM.h"
#include "FontEncoding.h"
#include <stdio.h>
#include <math.h>
/*#include <stdlib.h>*/

extern double atof ___PROTO((const char *));
extern int atoi ___PROTO((const char *));
extern long atol ___PROTO((const char *));

/*--- Text definitions ---*/
#define IS_NEWLINE(chr) (((chr)=='\n') || ((chr)=='\r'))
#define IS_WHITESPACE(chr) (((chr)==' ') || ((chr)=='\t') || IS_NEWLINE(chr))
#define IS_EOF(char) ((char)==EOF)

/*--- gperf definitions ---*/
AFM_keyword_t unknow_AFM_keyword = {"Unknown", AK_UNKNOW};

/*--- Lexical functions ---*/
typedef struct 
{
 int len;
 char data[257]; /* lines are no longer than 255 char long (cf AFM spec) */
} word_t;

static int getWord ___PROTO((FILE *, word_t *));
static int getLine ___PROTO((FILE *, word_t *));
static int getReal ___PROTO((FILE *, double *));
static int getCoord ___PROTO((FILE *, coord_t *));
static int getChar ___PROTO((FILE *, char_t *));
static int getHex ___PROTO((FILE *, long *));
static int getShort ___PROTO((FILE *, SHORT *));
static int getInteger ___PROTO((FILE *, long *));
static int getBoolean ___PROTO((FILE *, boolean *));
static char *fillStringField ___PROTO((FILE *, char *));

/*--- Parsing functions ---*/
static int AFM_parseCharMetrics ___PROTO((FILE *, c_VectorFont *, SHORT));
static int AFM_parseTrackKern ___PROTO((FILE *, c_VectorFont *, SHORT));
static int AFM_parseKernPairs ___PROTO((FILE *, c_VectorFont *, SHORT));
static int AFM_parseComposites ___PROTO((FILE *, c_VectorFont *, SHORT));
static encoding_name_t *is_Unknown_char ___PROTO((char *str, 
						  unsigned int len));

static char *filename;
static int linenumber;

#define UNEXPECTED_KEYWORD \
do { Xc_ERROR(("%s: unexpected `%s' keyword line %d", filename, word.data, \
	       linenumber)); fclose(input); return FALSE; } while(0)

static XcIsEncodedCharProc isFontEncodedChar;

/* ----------------------------------------------------------------- ** 
** AFM_parse - Fill a VectorFont class with an AFM file              ** 
**             (AFM v4.0)                                            ** 
** ----------------------------------------------------------------- */
boolean AFM_parse(fname, font)
char *fname;
c_VectorFont *font;
{
 FILE *input;
 boolean exit_flag, direction_flag, error_flag;
 word_t word;
 AFM_keyword_t *keyword;
 SHORT writing_dir, tmp_SHORT;
 c_CharMetrics *metrics;
 
 Xc_TRACE(("AFM_parse(%s)", fname));
 input = fopen(fname, "rb");
 if(input == NULL)
 {
  Xc_SYSERROR((fname));
  return FALSE;
 }

 metrics = font->metrics;
 writing_dir = 0;
 linenumber = 1;
 filename = fname;
 isFontEncodedChar = is_Unknown_char;
 exit_flag = direction_flag = error_flag = FALSE;

 while(42)
 {
  if(IS_EOF(getWord(input, &word))) return FALSE;
  keyword = is_AFM_keyword(word.data, word.len);
  if(keyword->type == AK_START_FONT_METRICS)
  {
   double version;
      
   if(IS_EOF(getReal(input, &version))) return FALSE;
   if(version > 5.0)
   {
    Xc_ERROR(("%s: AFM version %lf not implemented.",
	      filename, version));
    fclose(input);
    return FALSE;
   }
   else if (version > 4.0)
   {
    Xc_TRACE(("%s: AFM version %lf not fully implemented.",
	      filename, version));
    error_flag = TRUE;
   }
   break;
  }
  Xc_ERROR(("%s: unexpected %s keyword at first line",
	    filename, keyword->name));
  fclose(input);
  return FALSE;
 }

 while(exit_flag == FALSE)
 {
  if(IS_EOF(getWord(input, &word))) return FALSE;
  keyword = is_AFM_keyword(word.data, word.len);
  switch(keyword->type)
  {

   /*--- Unknow keyword: ignore ---*/
  case AK_UNKNOW:  
   if (!error_flag)
    Xc_WARNING(("%s: unknow keyword `%s' at line %d.",
		filename, word.data, linenumber));
   if(IS_EOF(getLine(input, &word))) return FALSE;
   break;

   /*--- Comment: skip line ---*/
  case AK_COMMENT:
   if(IS_EOF(getLine(input, &word))) return FALSE;
   Xc_TRACE(("comment: %s", word.data));
   break;

   /*--- End of main block ---*/
  case AK_END_FONT_METRICS:
   Xc_TRACE(("END_FONT_METRICS"));
   if(direction_flag != FALSE) UNEXPECTED_KEYWORD;
   exit_flag = TRUE;
   break;

   /*--- Start of character metrics ---*/
  case AK_START_CHAR_METRICS:
   Xc_TRACE(("START_CHAR_METRICS"));
   if(direction_flag != FALSE) UNEXPECTED_KEYWORD;
   if(IS_EOF(getShort(input, &tmp_SHORT))) return FALSE;
   if(IS_EOF(AFM_parseCharMetrics(input, font, tmp_SHORT)))
    return FALSE;
   break;
	
   /*--- Start of Composites subsection ---*/
  case AK_START_COMPOSITES:
   Xc_TRACE(("START_COMPOSITES"));
   if(direction_flag != FALSE) UNEXPECTED_KEYWORD;
   if(IS_EOF(getShort(input, &tmp_SHORT))) return FALSE;
   if(IS_EOF(AFM_parseComposites(input, font, tmp_SHORT))) return FALSE;
   break;

   /*--- Start of Kerning data ---*/
  case AK_START_KERN_DATA:
   Xc_TRACE(("START_KERN_DATA"));
   if(direction_flag != FALSE) UNEXPECTED_KEYWORD;
   break;

   /*--- End of Kerning data ---*/
  case AK_END_KERN_DATA:
   Xc_TRACE(("END_KERN_DATA"));
   break;

   /*--- Start of Track Kern subsection ---*/
  case AK_START_TRACK_KERN:
   Xc_TRACE(("START_TRACK_KERN"));
   if(IS_EOF(getShort(input, &tmp_SHORT))) return FALSE;
   if(IS_EOF(AFM_parseTrackKern(input, font, tmp_SHORT))) return FALSE;
   break;

   /*--- Start of Kern Pair subsections ---*/
  case AK_START_KERN_PAIRS:
  case AK_START_KERN_PAIRS0:
  case AK_START_KERN_PAIRS1:
   Xc_TRACE(("START_KERN_PAIRS"));
   if(IS_EOF(getShort(input, &tmp_SHORT))) return FALSE;
   if(IS_EOF(AFM_parseKernPairs(input, font, tmp_SHORT))) return FALSE;
   break;

   /* ---------------------------------------------------------- */
	
   /*--- PostScript Name (req.) ---*/
  case AK_FONT_NAME:
   Xc_TRACE(("FONT_NAME"));
   if(font->font_name != NULL) Xc_free(font->font_name);
   if((font->font_name = fillStringField(input, keyword->name)) == NULL)
    return FALSE;
   break;

   /*--- Full text name (opt.) ---*/
  case AK_FULL_NAME:
   Xc_TRACE(("FULL_NAME"));
   if(font->full_name != NULL) Xc_free(font->full_name);
   if((font->full_name = fillStringField(input, keyword->name)) == NULL)
    return FALSE;
   break;

   /*--- Name of the typeface family (opt.) ---*/
  case AK_FAMILY_NAME:
   Xc_TRACE(("FAMILY_NAME"));
   if(font->family_name != NULL) Xc_free(font->family_name);
   if((font->family_name = fillStringField(input, keyword->name)) == NULL)
    return FALSE;
   break;

   /*--- Weight of the font (opt.) ---*/
  case AK_WEIGHT:
   Xc_TRACE(("WEIGHT"));
   if(font->weight != NULL) Xc_free(font->weight);
   if((font->weight = fillStringField(input, keyword->name)) == NULL)
    return FALSE;
   break;

   /*--- Font bounding box ---*/
  case AK_FONT_BBOX:
   Xc_TRACE(("BBOX"));
   if(IS_EOF(getCoord(input, &(metrics->font_bbox.llx))))
    return FALSE;
   if(IS_EOF(getCoord(input, &(metrics->font_bbox.lly))))
    return FALSE;
   if(IS_EOF(getCoord(input, &(metrics->font_bbox.urx))))
    return FALSE;
   if(IS_EOF(getCoord(input, &(metrics->font_bbox.ury))))
    return FALSE;
   break;

   /*--- Font program version identifier (opt.) ---*/
  case AK_VERSION:
   Xc_TRACE(("VERSION"));
   if(font->version != NULL) Xc_free(font->version);
   if((font->version = fillStringField(input, keyword->name)) == NULL)
    return FALSE;
   break;

   /*--- Font name trademark or copyright (opt.) ---*/
  case AK_NOTICE:
   Xc_TRACE(("NOTICE"));
   if(font->notice != NULL) Xc_free(font->notice);
   if((font->notice = fillStringField(input, keyword->name)) == NULL)
    return FALSE;
   break;

   /*--- Default encoding vector (opt.) ---*/
  case AK_ENCODING_SCHEME:
   Xc_TRACE(("ENCODING_SCHEME"));
   if(IS_EOF(getLine(input, &word))) return FALSE;
   if(!strcmp(XcFE_ADOBE_STD_SCHEME, word.data))
   {
    Xc_TRACE((XcFE_ADOBE_STD_SCHEME));
    /*isFontEncodedChar = is_AdobeStd_char;*/
    /*font->encoding = XcFE_ADOBE_STD;*/
    isFontEncodedChar = is_Latin1_char;
    font->encoding = XcFE_LATIN1;
   }
   else if((font->family_name != NULL) &&
	   !strcmp(font->family_name, XcFE_SYMBOL_NAME))
   {
    Xc_TRACE((XcFE_SYMBOL_NAME));
    isFontEncodedChar = is_Symbol_char;
    font->encoding = XcFE_SYMBOL;
   }
   else if((font->font_name != NULL) &&
	   !strcmp(font->font_name, XcFE_DINGBATS_NAME))
   {
    Xc_TRACE((XcFE_DINGBATS_NAME));
    isFontEncodedChar = is_Dingbats_char;
    font->encoding = XcFE_DINGBATS;
   }
   else if (!strcmp(XcFE_FONT_SPECIFIC_SCHEME, word.data))
   {
    Xc_TRACE((XcFE_FONT_SPECIFIC_SCHEME));
    isFontEncodedChar = is_Unknown_char;
    font->encoding = XcFE_UNKNOW;
   }
   else
    return FALSE;
   break;

   /*--- Mapping scheme (!base font prog) ---*/
  case AK_MAPPING_SCHEME:
   Xc_TRACE(("MAPPING_SCHEME"));
   if(IS_EOF(getShort(input, &(font->mapping_scheme))))
    return FALSE;
	
   /*--- Byte value for escape char. (only MappingScheme 3) ---*/
  case AK_ESC_CHAR:
   Xc_TRACE(("ESC_CHAR"));
   if(IS_EOF(getChar(input, &(font->esc_char)))) return FALSE;
   break;

   /*--- Character set ---*/
  case AK_CHARACTER_SET:
   Xc_TRACE(("CHARACTER_SET"));
   if(font->character_set != NULL) Xc_free(font->character_set);
   if((font->character_set = fillStringField(input, keyword->name)) == NULL)
    return FALSE;
   break;

   /*--- Number of characters (opt.) ---*/
  case AK_CHARACTERS:
   Xc_TRACE(("CHARACTERS"));
   if(IS_EOF(getInteger(input, &(font->characters)))) return FALSE;
   break;

   /*--- Base font flag (opt.) ---*/
  case AK_IS_BASE_FONT:
   Xc_TRACE(("IS_BASE_FONT"));
   if(IS_EOF(getBoolean(input, &(font->is_base_font))))
    return FALSE;
   break;

   /*--- Vector from origin 0 to origin 1 (opt.)---*/
  case AK_V_VECTOR:
   Xc_TRACE(("V_VECTOR"));
   if(IS_EOF(getCoord(input, &(metrics->vvector.dx)))) return FALSE;
   if(IS_EOF(getCoord(input, &(metrics->vvector.dy)))) return FALSE;
   metrics->is_fixed_v = TRUE;
   break;

   /*--- Flag for constant vvector (opt.) ---*/
  case AK_IS_FIXED_V:
   Xc_TRACE(("IS_FIXED_V"));
   if(IS_EOF(getBoolean(input, &(metrics->is_fixed_v))))
    return FALSE;
   break;

   /*--- y-value of the top of the capital `H' (opt.) ---*/
  case AK_CAP_HEIGHT:
   Xc_TRACE(("CAP_HEIGHT"));
   if(IS_EOF(getCoord(input, &(metrics->cap_height)))) return FALSE;
   break;

   /*--- y-value of the top of the lowercase `x' (opt.) ---*/
  case AK_X_HEIGHT:
   Xc_TRACE(("X_HEIGHT"));
   if(IS_EOF(getCoord(input, &(metrics->x_height)))) return FALSE;
   break;

   /*--- y-value of the top of the lowercase `d' (opt.) ---*/
  case AK_ASCENDER:
   Xc_TRACE(("ASCENDER"));
   if(IS_EOF(getCoord(input, &(metrics->ascender)))) return FALSE;
   break;

   /*--- y-value of the bottom of the lowercase 'p' (opt.) ---*/
  case AK_DESCENDER:
   Xc_TRACE(("DESCENDER"));
   if(IS_EOF(getCoord(input, &(metrics->descender)))) return FALSE;
   break;

   /* -------------------------------------------------------------- */

   /*--- Writing directions to be described ---*/
  case AK_METRICS_SETS:
   Xc_TRACE(("METRICS_SETS"));
   if(IS_EOF(getShort(input, &(metrics->metrics_sets))))
    return FALSE;
   if((metrics->metrics_sets < 0) || (metrics->metrics_sets > 2))
   {
    Xc_ERROR(("%s: illegal writing directions line %d",
	      filename, linenumber));
    fclose(input);
    return FALSE;
   }
   writing_dir = 3;
   break;

   /* -------------------------------------------------------------- */
	
   /*--- Encloses metrics for a writing direction ---*/
  case AK_START_DIRECTION:
   Xc_TRACE(("START_DIRECTION"));
   if(direction_flag != FALSE) UNEXPECTED_KEYWORD;
   if(IS_EOF(getShort(input, &writing_dir))) return FALSE;
   if(((metrics->metrics_sets == 0) && (writing_dir != 0)) ||
      ((metrics->metrics_sets != 0) && (writing_dir != 0) &&
       (writing_dir != 1)))
   {
    Xc_ERROR(("%s: illegal writing direction %d at line %d",
	      filename, writing_dir, linenumber));
    fclose(input);
    return FALSE;
   }
   direction_flag = TRUE;
   break;

  case AK_END_DIRECTION:
   Xc_TRACE(("END_DIRECTION"));
   if(direction_flag == FALSE) UNEXPECTED_KEYWORD;
   direction_flag = FALSE;
   writing_dir = 3;
   break;

   /*--- Underline position ---*/
  case AK_UNDERLINE_POSITION:
   Xc_TRACE(("UNDERLINE_POSITION"));
   if(writing_dir == 3) UNEXPECTED_KEYWORD;
   if(IS_EOF(getCoord(input, &(metrics->underline_position[writing_dir]))))
    return FALSE;
   break;
	
   /*--- Underline thickness ---*/
  case AK_UNDERLINE_THICKNESS:
   Xc_TRACE(("UNDERLINE_THICKNESS"));
   if(writing_dir == 3) UNEXPECTED_KEYWORD;
   if(IS_EOF(getCoord(input, &(metrics->underline_thickness[writing_dir]))))
    return FALSE;
   break;
	
   /*--- Italic angle ---*/
  case AK_ITALIC_ANGLE:
   {
    double tmp;
	 
    Xc_TRACE(("ITALIC_ANGLE"));
    if(writing_dir == 3) UNEXPECTED_KEYWORD;
    if(IS_EOF(getReal(input, &tmp))) return FALSE;
    metrics->italic_angle[writing_dir] = SCALE_FROM_DEGREES(tmp);
    break;
   }
		
   /*--- Constant width vector ---*/
  case AK_CHAR_WIDTH:
   Xc_TRACE(("CHAR_WIDTH"));
   if(writing_dir == 3) UNEXPECTED_KEYWORD;
   if(IS_EOF(getCoord(input, &(metrics->char_width[writing_dir].dx))))
    return FALSE;
   if(IS_EOF(getCoord(input, &(metrics->char_width[writing_dir].dy))))
    return FALSE;
   metrics->is_fixed_pitch[writing_dir] = TRUE;
   break;
	
   /*--- Fixed pitch flag ---*/
  case AK_IS_FIXED_PITCH:
   Xc_TRACE(("IS_FIXED_PITCH"));
   if(writing_dir == 3) UNEXPECTED_KEYWORD;
   if(IS_EOF(getBoolean(input, &(metrics->is_fixed_pitch[writing_dir]))))
    return FALSE;
   break;
	
  default:
   Xc_TRACE(("`%s'", word.data));
   getLine(input, &word);
   break;
  }
 }
 fclose(input);
 return TRUE;
}

static garbage_encode *garbage;
/* ----------------------------------------------------------------- ** 
** AFM_parseCharMetrics - Parse a set of individual character        ** 
**                        metrics (end with EndCharMetrics)          ** 
** ----------------------------------------------------------------- */
static int AFM_parseCharMetrics(input, font, count)
FILE *input;
c_VectorFont *font;
SHORT count;
{
 word_t word, name;
 AFM_keyword_t *keyword;
 long code;
 vector_t w[2], vv;
 bbox_t b;
 int rc=0;

 code = -1;
 name.len = 0; name.data[0] = 0;
 w[0].dx = w[1].dx = w[0].dy = w[1].dy = vv.dx = vv.dy = 0;
 b.llx = b.lly = b.urx = b.ury = 0;
 Xc_TRACE(("AFM_parseCharMetrics %d line %d", count, linenumber));
 while(42)
 {
  if(IS_EOF(getWord(input, &word))) return FALSE;
  keyword = is_AFM_keyword(word.data, word.len);
  switch(keyword->type)
  {
   /*--- Unknow keyword: ignore ---*/
  case AK_UNKNOW:  
   Xc_WARNING(("%s: unknow keyword `%s' at line %d.",
	       filename, word.data, linenumber));
   if(IS_EOF(getLine(input, &word))) return FALSE;
   break;

   /*--- Comment: skip line ---*/
  case AK_COMMENT:
   if(IS_EOF(getLine(input, &word))) return FALSE;
   break;

   /*--- Decimal character code ---*/
  case AK_C:
   if(IS_EOF(getInteger(input, &code))) return EOF;
   break;

   /*--- Hexadecimal character code ---*/
  case AK_CH:
   if(IS_EOF(getHex(input, &code))) return EOF;
   break;
       
   /*--- PostScript character name ---*/
  case AK_N:
   if(IS_EOF(getWord(input, &name))) return EOF;
   if((name.data[0] == ';') && (name.len == 1))
    name.len = 0, name.data[0] = 0;
   break;

   /*--- Character width x for writing dir 0 ---*/
  case AK_W0X:
  case AK_WX:
   if(IS_EOF(getCoord(input, &(w[0].dx)))) return EOF;
   break;

   /*--- Character width x for writing dir 1 ---*/
  case AK_W1X:
   if(IS_EOF(getCoord(input, &(w[1].dx)))) return EOF;
   break;
       
   /*--- Character width y for writing dir 0 ---*/
  case AK_W0Y:
  case AK_WY:
   if(IS_EOF(getCoord(input, &(w[0].dy)))) return EOF;
   break;

   /*--- Character width y for writing dir 1 ---*/
  case AK_W1Y:
   if(IS_EOF(getCoord(input, &(w[1].dy)))) return EOF;
   break;
	
   /*--- Character width  vector for writing dir 0 ---*/
  case AK_W0:
  case AK_W:
   if(IS_EOF(getCoord(input, &(w[0].dx)))) return EOF;
   if(IS_EOF(getCoord(input, &(w[0].dy)))) return EOF;
   break;

   /*--- Character width  vector for writing dir 1 ---*/
  case AK_W1:
   if(IS_EOF(getCoord(input, &(w[1].dx)))) return EOF;
   if(IS_EOF(getCoord(input, &(w[1].dy)))) return EOF;
   break;

   /*--- Vector to origin 0 to origin 1 ---*/
  case AK_VV:
   if(IS_EOF(getCoord(input, &(vv.dx)))) return EOF;
   if(IS_EOF(getCoord(input, &(vv.dy)))) return EOF;
   break;

   /*--- Character bounding box ---*/
  case AK_B:
   if(IS_EOF(getCoord(input, &(b.llx)))) return EOF;
   if(IS_EOF(getCoord(input, &(b.lly)))) return EOF;
   if(IS_EOF(getCoord(input, &(b.urx)))) return EOF;
   if(IS_EOF(getCoord(input, &(b.ury)))) return EOF;
   break;

   /*--- Ligature sequence ---*/
  case AK_L:
   if(IS_EOF(getWord(input, &word))) return EOF;
   if(IS_EOF(getWord(input, &word))) return EOF;
   break;

   /*--- End of Character Metrics subsection ---*/
  case AK_END_CHAR_METRICS:
   if(count > 0)
    Xc_WARNING(("%s: %d missing character metrics", filename, count));
   else if(count < 0)
    Xc_WARNING(("%s: %d extra character metrics", filename, count));
   return ~EOF;

   /*--- Remaining keywords are unexpected ---*/
  default:
   Xc_ERROR(("%s: unexpected %s keyword line %d",
	     filename, word.data, linenumber));
   fclose(input);
   return EOF;
  }

  if((keyword->type == AK_N) && (name.len == 0))
   word.data[0] = ';', word.len = 1;
  else if(IS_EOF(rc = getWord(input, &word)))
   return EOF;
      
  if(!((word.data[0] == ';') && (word.len = 1)))
  {
   Xc_ERROR(("%s: semicolon expected line %d (%s)",
	     filename, linenumber, word.data));
   fclose(input);
   return EOF;
  }
  if(IS_NEWLINE(rc))
  {
   encoding_name_t *ename;

   /*--- If there is no code ---*/
   if(font->encoding == XcFE_LATIN1)
   {
    if((ename = is_Latin1_char(name.data, name.len)) != NULL)
    {
     composite_part_t part;

     F(font->metrics).addEncodedChar(font->metrics,
				     ename->code,
				     ename->name, w, &b);
			  
     F(font->metrics).addChar(font->metrics,
			      ename->code, ename->name,
			      w, &b, &vv);
     part.code = ename->code;
     part.offset.dx = part.offset.dy = 0;
     F(font->metrics).addCompositePart(font->metrics,
				       ename->code, &part);
    }
   }
   else
   {
    if(code != -1)
    {
     garbage = NULL;

     if((ename = isFontEncodedChar(name.data, name.len)) != NULL)
     {
      /*	    if((ename->code != code) && (ename->code != 0))
		    Xc_WARNING(("%s:`%s' line %d expected to be %d",
		    filename, name.data, linenumber,
		    ename->code)); */

      F(font->metrics).addEncodedChar(font->metrics, code,
				      ename->name, w, &b);
      if((font->encoding != XcFE_ADOBE_STD) &&
	 (font->encoding != XcFE_LATIN1))
      {
       composite_part_t part;
			  
       F(font->metrics).addChar(font->metrics,
				code, ename->name,
				w, &b, &vv);
       part.code = code;
       part.offset.dx = part.offset.dy = 0;
       F(font->metrics).addCompositePart(font->metrics,
					 code, &part);
      }
     }

     if (garbage)
     {
      garbage->next = font->garbage;
      font->garbage = garbage;
     }

    }
   }
	  
   /*--- re-init variables ---*/
   count--;
   code = -1;
   name.len = 0; name.data[0] = 0;
   w[0].dx = w[1].dx = w[0].dy = w[1].dy = vv.dx = vv.dy = 0;
   b.llx = b.lly = b.urx = b.ury = 0;
  }
 }
}


/* ----------------------------------------------------------------- ** 
** AFM_parseComposites - Parse a set of individual composite         ** 
**                       characters (end with EndComposites)         ** 
** ----------------------------------------------------------------- */
static int AFM_parseComposites(input, font, count)
FILE *input;
c_VectorFont *font;
SHORT count;
{
 word_t word;
 AFM_keyword_t *keyword;
 encoding_name_t *name;
 int rc;
 SHORT part_count;

 Xc_TRACE(("AFM_parseComposites %d line %d", count, linenumber));

 if(font->encoding == XcFE_LATIN1)
 {
  Xc_TRACE(("skip section"));
  do {
      if(IS_EOF(getWord(input, &word))) return FALSE;
      keyword = is_AFM_keyword(word.data, word.len);
      /*getLine(input, &word);*/
     } while(keyword->type != AK_END_COMPOSITES);
  return ~EOF;
 }

 name = NULL;
 while(42)
 {
  if(IS_EOF(getWord(input, &word))) return FALSE;
  keyword = is_AFM_keyword(word.data, word.len);
  switch(keyword->type)
  {
   /*--- Unknow keyword: ignore ---*/
  case AK_UNKNOW:  
   Xc_WARNING(("%s: unknow keyword `%s' at line %d.",
	       filename, word.data, linenumber));
   if(IS_EOF(getLine(input, &word))) return FALSE;
   break;
	  
   /*--- Comment: skip line ---*/
  case AK_COMMENT:
   if(IS_EOF(getLine(input, &word))) return FALSE;
   break;
	  
   /*--- Start composite char ---*/
  case AK_CC:
   if(IS_EOF(getWord(input, &word)))
    return FALSE;
   else
    if((name = is_Latin1_char(word.data, word.len)) == NULL)
    {
     count--;
     if(IS_EOF(getLine(input, &word)))
      return FALSE;
     else
      continue;
    }
   if(IS_EOF(getShort(input, &part_count))) return EOF;
   break;
	  
   /*--- One of the part of the composite char ---*/
  case AK_PCC:
   {
    composite_part_t part;
    encoding_name_t *partname;
	    
    Xc_ASSERT(name != NULL);
    if(IS_EOF(getWord(input, &word))) return FALSE;
    if((partname = isFontEncodedChar(word.data, word.len)) == NULL)
    {
     Xc_WARNING(("%s: unknow part `%s' fo composite `%s' line %d",
		 filename, word.data, name->name, linenumber));
     if(IS_EOF(getWord(input, &word))) return FALSE;
     if(IS_EOF(getWord(input, &word))) return FALSE;
    }
    else
    {
     part.code = partname->code;
     if(IS_EOF(getCoord(input, &(part.offset.dx)))) return FALSE;
     if(IS_EOF(getCoord(input, &(part.offset.dy)))) return FALSE;
     F(font->metrics).addCompositePart(font->metrics,
				       name->code, &part);
    }
    part_count--;
    break;
   }
	  
   /*--- End of Character Metrics subsection ---*/
  case AK_END_COMPOSITES:
   if(count == 0) return ~EOF;
   Xc_WARNING(("%s: %d missing composites character", filename, count));
	  
   /*--- Remaining keywords are unexpected ---*/
  default:
   Xc_ERROR(("%s: unexpected %s keyword line %d",
	     filename, word.data, linenumber));
   fclose(input);
   return EOF;
  }
      
  if(IS_EOF(rc = getWord(input, &word))) return EOF;
  if(!((word.data[0] == ';') && (word.len == 1)))
  {
   Xc_ERROR(("%s: semicolon expected line %d (%s)",
	     filename, linenumber, word.data));
   fclose(input);
   return EOF;
  }
  if(IS_NEWLINE(rc))
  {
   if(part_count != 0)
   {
    Xc_ERROR(("%s: unexpected number of composites parts line %d",
	      filename, linenumber));
    fclose(input);
    return EOF;
   }

   /*--- Fill the structure ---*/
   name = NULL;
   count--;
  }
 }
}


/* ----------------------------------------------------------------- ** 
** AFM_parseTrackKern - Parse kerning data (handle Track             ** 
**                      kerning datas). End with EndKernData         ** 
** ----------------------------------------------------------------- */
static int AFM_parseTrackKern(input, font, count)
FILE *input;
c_VectorFont *font;
SHORT count;
{
 word_t word;
 AFM_keyword_t *keyword;
 track_kern_t kern;
 double tmp;

 Xc_TRACE(("AFM_parseTrackKern %d line %d", count, linenumber));

 while(42)
 {
  if(IS_EOF(getWord(input, &word))) return FALSE;
  keyword = is_AFM_keyword(word.data, word.len);
  switch(keyword->type)
  {

   /*--- Unknow keyword: ignore ---*/
  case AK_UNKNOW:  
   Xc_WARNING(("%s: unknow keyword `%s' at line %d.",
	       filename, word.data, linenumber));
   if(IS_EOF(getLine(input, &word))) return FALSE;
   break;

   /*--- Track Kerning ---*/
  case AK_TRACK_KERN:
   if(IS_EOF(getInteger(input, &(kern.degree)))) return EOF;
   if(IS_EOF(getReal(input, &tmp))) return EOF;
   kern.min_point_size = SCALE_FROM_POINTS(tmp);
   if(IS_EOF(getReal(input, &tmp))) return EOF;
   kern.min_kern = SCALE_FROM_POINTS(tmp);
   if(IS_EOF(getReal(input, &tmp))) return EOF;
   kern.max_point_size = SCALE_FROM_POINTS(tmp);
   if(IS_EOF(getReal(input, &tmp))) return EOF;
   kern.max_kern = SCALE_FROM_POINTS(tmp);
   F(font->metrics).addTrackKern(font->metrics, &kern);
   count--;
   break;

   /*--- End of Track Kerning subsection ---*/
  case AK_END_TRACK_KERN:
   if(count == 0) return ~EOF;

   /*--- Remaining keywords are unexpected ---*/
  default:
   Xc_ERROR(("%s: unexpected %s keyword line %d",
	     filename, word.data, linenumber));
   fclose(input);
   return EOF;
  }
 }
}


/* ----------------------------------------------------------------- ** 
** AFM_parseKernPairs - Parse a set of kerning pairs (end with       ** 
**                      EndKernPairs)                                ** 
** ----------------------------------------------------------------- */
static int AFM_parseKernPairs(input, font, count)
FILE *input;
c_VectorFont *font;
SHORT count;
{
 word_t word;
 AFM_keyword_t *keyword;
 long tmp;
 char_t code;
 kern_pair_t kern;
 encoding_name_t *name;

 Xc_TRACE(("AFM_parseKernPairs %d line %d", count, linenumber));
 while(42)
 {
  if(IS_EOF(getWord(input, &word))) return FALSE;
  keyword = is_AFM_keyword(word.data, word.len);
  switch(keyword->type)
  {
	
   /*--- Unknow keyword: ignore ---*/
  case AK_UNKNOW:  
   Xc_WARNING(("%s: unknow keyword `%s' at line %d.",
	       filename, word.data, linenumber));
   if(IS_EOF(getLine(input, &word))) return FALSE;
   continue;

   /*--- Comment: skip line ---*/
  case AK_COMMENT:
   if(IS_EOF(getLine(input, &word))) return FALSE;
   continue;

   /*--- Kern vector with name ---*/
  case AK_KP:
   if(IS_EOF(getWord(input, &word))) return EOF;
   if((name = is_Latin1_char(word.data, word.len)) == NULL)
   {
    getLine(input, &word);
    continue;
   }
   code = name->code;
   if(IS_EOF(getWord(input, &word))) return EOF;
   if((name = is_Latin1_char(word.data, word.len)) == NULL)
   {
    getLine(input, &word);
    continue;
   }
   kern.succ = name->code;
   goto kern_vector;

   /*--- Kern vector with hex character code ---*/
  case AK_KPH:
   if(IS_EOF(getHex(input, &tmp))) return EOF;
   code = (char_t)tmp;
   if(IS_EOF(getHex(input, &tmp))) return EOF;
   kern.succ = (char_t)tmp;
  kern_vector:
   count--;
   if(IS_EOF(getCoord(input, &(kern.offset.dx)))) return EOF;
   if(IS_EOF(getCoord(input, &(kern.offset.dy)))) return EOF;
   break;
       
   /*--- Kern x offset with name ---*/
  case AK_KPX:
   count--;
   if(IS_EOF(getWord(input, &word))) return EOF;
   if((name = is_Latin1_char(word.data, word.len)) == NULL)
   {
    getLine(input, &word);
    continue;
   }
   code = name->code;
   if(IS_EOF(getWord(input, &word))) return EOF;
   if((name = is_Latin1_char(word.data, word.len)) == NULL)
   {
    getLine(input, &word);
    continue;
   }
   kern.succ = name->code;
   if(IS_EOF(getCoord(input, &(kern.offset.dx)))) return EOF;
   kern.offset.dy = 0;
   break;
       
   /*--- Kern y offset with name ---*/
  case AK_KPY:
   count--;
   if(IS_EOF(getWord(input, &word))) return EOF;
   if((name = is_Latin1_char(word.data, word.len)) == NULL)
   {
    getLine(input, &word);
    continue;
   }
   code = name->code;
   if(IS_EOF(getWord(input, &word))) return EOF;
   if((name = is_Latin1_char(word.data, word.len)) == NULL)
   {
    getLine(input, &word);
    continue;
   }
   kern.succ = name->code;
   if(IS_EOF(getCoord(input, &(kern.offset.dy)))) return EOF;
   kern.offset.dx = 0;
   break;
       
   /*--- End of Pair-Wise Kerning subsection ---*/
  case AK_END_KERN_PAIRS:
   if(count == 0) return ~EOF;
   Xc_WARNING(("%s: %d missing kerning pairs", filename, count));

   /*--- Remaining keywords are unexpected ---*/
  default:
   Xc_ERROR(("%s: unexpected %s keyword line %d",
	     filename, word.data, linenumber));
   fclose(input);
   return EOF;
  }
  F(font->metrics).addKernPair(font->metrics, code, &kern);
 }
}


/* ----------------------------------------------------------------- ** 
** getWord - Get a word without whitespaces                          ** 
** ----------------------------------------------------------------- */
static int getWord(input, wordp)
FILE *input;
word_t *wordp;
{
 char *ptr;
 int c;

 wordp->len = 0;
 ptr = wordp->data;

 do 
 {
  c = getc(input);
 } while(IS_WHITESPACE(c));
  
 if(!IS_EOF(c))
 {
  do 
  {
   *ptr++ = c;
   if(wordp->len++ == 256)
   {
    Xc_WARNING(("%s: line %d too long.", filename, linenumber));
    do {
	c = getc(input);
       } while(!(IS_NEWLINE(c) || IS_EOF(c)));
    break;
   }
   c = getc(input);
  } while(!(IS_WHITESPACE(c) || IS_EOF(c)));
 }

 *ptr = 0;

 if(IS_EOF(c))
 {
  Xc_WARNING(("%s: unexpected end of file at line %d.",
	      filename, linenumber));
  fclose(input);
  return EOF;
 }

 while(IS_WHITESPACE(c) && !IS_NEWLINE(c)) c = getc(input);

 if(IS_NEWLINE(c))
 {
  linenumber++;
  ungetc(c, input);
 }
 else if(!IS_WHITESPACE(c))
  ungetc(c, input);

 return c;
}


/* ----------------------------------------------------------------- ** 
** getReal - Get a *double* value from a word without whitespaces    ** 
** ----------------------------------------------------------------- */
static int getReal(input, realp)
FILE *input;
double *realp;
{
 word_t word;
 int rc;

 if(IS_EOF(rc = getWord(input, &word))) return EOF;
 *realp = atof(word.data);
 return rc;
}


/* ----------------------------------------------------------------- ** 
** getInteger - Get a *long* integer value from a word               ** 
** ----------------------------------------------------------------- */
static int getInteger(input, integerp)
FILE *input;
long *integerp;
{
 word_t word;
 int rc;

 if(IS_EOF(rc = getWord(input, &word))) return EOF;
 *integerp = atol(word.data);
 return rc;
}


/* ----------------------------------------------------------------- ** 
** getShort - Get a *SHORT* integer from a word                      ** 
** ----------------------------------------------------------------- */
static int getShort(input, integerp)
FILE *input;
SHORT *integerp;
{
 word_t word;
 int rc;

 if(IS_EOF(rc = getWord(input, &word))) return EOF;
 *integerp = (SHORT)atoi(word.data);
 return rc;
}


/* ----------------------------------------------------------------- ** 
** getBoolean - Get a boolean from a word                            ** 
** ----------------------------------------------------------------- */
static int getBoolean(input, booleanp)
FILE *input;
boolean *booleanp;
{
 word_t word;
 int rc;

 if(IS_EOF(rc = getWord(input, &word))) return EOF;
 *booleanp = strcmp(word.data, "true")? FALSE : TRUE;
 return rc;
}


/* ----------------------------------------------------------------- ** 
** getCoord - Get a *coord_t* integer from a word                    ** 
** ----------------------------------------------------------------- */
static int getCoord(input, coordp)
FILE *input;
coord_t *coordp;
{
 word_t word;
 int rc;

 if(IS_EOF(rc = getWord(input, &word))) return EOF;
 *coordp = SCALE_FROM_POINTS(atof(word.data) / 1000.0); 
 return rc;
}


/* ----------------------------------------------------------------- ** 
** getChar - Get a *char_t* from a word                              ** 
** ----------------------------------------------------------------- */
static int getChar(input, charp)
FILE *input;
char_t *charp;
{
 word_t word;
 int rc;

 if(IS_EOF(rc = getWord(input, &word))) return EOF;
 *charp = (char_t) atoi(word.data);
 return rc;
}


/* ----------------------------------------------------------------- ** 
** getHex - Get a *long* from a word (coded in hexa)                 ** 
** ----------------------------------------------------------------- */
static int getHex(input, longp)
FILE *input;
long *longp;
{
 word_t word;
 int rc;

 if(IS_EOF(rc = getWord(input, &word))) return EOF;
 if(sscanf(word.data, "<%lx>", longp) != 1)
 {
  Xc_ERROR(("%s: hex value expected line %d", filename, linenumber));
  fclose(input);
  return EOF;
 }
 return rc;
}


/* ----------------------------------------------------------------- ** 
** getLine - Skip all whitespaces and then get the end of line       ** 
** ----------------------------------------------------------------- */
static int getLine(input, wordp)
FILE *input;
word_t *wordp;
{
 char *ptr;
 int c;

 wordp->len = 0;
 ptr = wordp->data;

 do 
 {
  c = getc(input);
 } while(IS_WHITESPACE(c) && !IS_NEWLINE(c));
 
 if(!IS_EOF(c) && !IS_NEWLINE(c))
  do 
  {
   *ptr++ = c;
   if(wordp->len++ == 256)
   {
    Xc_WARNING(("%s: line %d too long.", filename, linenumber));
    do 
    {
     c = getc(input);
    } while(!(IS_NEWLINE(c) || IS_EOF(c)));
    break;
   }
   c = getc(input);
  } while(!(IS_NEWLINE(c) || IS_EOF(c)));
  
 *ptr = 0;

 if(IS_EOF(c))
 {
  Xc_WARNING(("%s: unexpected end of file at line %d.",
	      filename, linenumber));
  fclose(input);
  return EOF;
 }

 linenumber++;

 return c;
}


/* ----------------------------------------------------------------- ** 
** fillStringField - Fill a field with a Xc_mallocated zone          ** 
**                   from a word                                     ** 
** ----------------------------------------------------------------- */
static char *fillStringField(input, malloc_info)
FILE *input;
char *malloc_info;
{
 int rc;
 word_t word;
 void *ptr;

 rc = getLine(input, &word);
 if(IS_EOF(rc))
  return NULL;
 if((ptr = Xc_malloc(malloc_info, word.len + 1)) == NULL)
 {
  fclose(input);
  return NULL;
 }
 /*  Xc_TRACE(("%s = %s", malloc_info, word.data));*/
 memcpy(ptr, word.data, word.len + 1);
 return ptr;
}


/* ------------------------------------------------------------------- ** 
** is_Unknown_char - Default Font encoding, try guessing glyph code    ** 
** ------------------------------------------------------------------- */
static encoding_name_t unknown_glyph;

static encoding_name_t *is_Unknown_char(str, len)
char *str;
unsigned int len;
{
 char *encode_name;
 Xc_TRACE(("is_Unknown_char(`%s')", str));

 encode_name = Xc_strdup("ecn", str);
 
 garbage = (garbage_encode *)Xc_malloc("ge", sizeof(garbage_encode));
 garbage->encode_name = encode_name;
 garbage->next = NULL;
 
 unknown_glyph.name = encode_name;
 unknown_glyph.code = (char_t)0;

 return &unknown_glyph;
}

