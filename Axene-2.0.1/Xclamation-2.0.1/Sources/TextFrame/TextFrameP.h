/*
** TextFrameP.h for Xclamation in TextFrame/
** Private header for the TextFrame class
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
** Started on  Tue Oct 11 19:54:41 1994 Stéphane Boisson
** Last update Wed Jun  4 22:16:22 1997 Antoine Buat
*/

#ifndef __XC_TEXT_FRAME_P_H__
#define __XC_TEXT_FRAME_P_H__

#include "TextFrame.h"
#include "ScanLine.h"
#include "CharMetrics.h"

/*--- Frame structures ---*/
typedef struct {
  text_pos_t start_pos;		/* Hline start char position in text   */
  text_pos_t end_pos;		/* Hline end char position in text     */
  coord_t x_min;		/* Hline start coord                   */
  coord_t x_max;		/* Hline end coord                     */
  coord_t gap;			/* Empty space in this hline           */
  unsigned int space_count;	/* Space char count in hline           */
  unsigned int char_count;	/* Char count in hline                 */
} textframe_hline_t;

struct textframe_line_s {
  int column_count;		/* textframe_hline count in this line  */
  coord_t y_pos;		/* Y position of the line start        */
};


/*--- Calibration structures ---*/
typedef struct {
  text_pos_t pos;		/* Char position in text               */
  char_t code;			/* Char code                           */
  coord_t width;		/* Kerned char width                   */
  coord_t width_delta;		/* Real char width - kerned char width */
  coord_t height;		/* Line height at this text position   */
  coord_t descender;		/* Char descender                      */
} textframe_char_t;

typedef struct {
  boolean reduction_flag;
  c_TextRuler *ruler;		/* Current TextRuler                */
  c_TextStyle *style;		/* Current TextStyle                */
  boolean paragraph_flag;	/* Have to do paragraph indentation */
  coord_t y_pos;		/* Current Y position in the frame  */
  coord_t interline;		/* Current line height              */
  coord_t descender;		/* Max descender value of the line  */

  /*___textframe_char management___*/
  textframe_char_t *buffer;
  unsigned long pos;
  unsigned long len;

  /*___For the hline construction___*/
  textframe_line_t *line_buffer;
  textframe_line_t *line_ptr;
  char *line_buffer_end;
  unsigned int line_count;
} textframe_char_line_t;


/*--- Prototypes ---*/
boolean CalibrationStart ___PROTO((textframe_char_line_t *line,
				   unsigned long hline_init));
void CalibrationEnd ___PROTO((textframe_char_line_t *line,
			      textframe_line_t **ptr,
			      unsigned int *count));
boolean CalibrationShift ___PROTO((textframe_char_line_t *line,
				   text_seq_t *seq));
boolean CalibrationReduce ___PROTO((textframe_char_line_t  *line,
				   int count,
				   sl_Segment *segment,
				   text_pos_t end_pos));
void CalibrationReactualize ___PROTO((textframe_char_line_t *line, int start));

boolean tf_releaseResources ___PROTO((c_TextFrame *this));

/*--- Redraw ---*/
void tf_Redraw ___NPROTO((c_TextFrame *this, c_Cadre *frame, Polygone *poly));

/*--- Print ---*/
boolean tf_Print ___NPROTO((c_TextFrame *this, c_PostScript *post));

/*--- Configure ---*/
void tf_Configure ___NPROTO((c_TextFrame *this, c_Cadre *frame,
			     Polygone *pol));


#endif /* !__XC_TEXT_FRAME_P_H__ */
