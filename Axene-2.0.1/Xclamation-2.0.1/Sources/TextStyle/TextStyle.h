/*
** TextStyle.h for Xclamation, XQuad and XAllWrite in TextStyle/
** Definitions of the TextStyle class
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
** Started on  Sun Jun 12 02:09:46 1994 Stéphane Boisson
** Last update Fri Aug 14 17:21:20 1998 Emmanuel Paris
*/

#ifndef __XC_TEXT_STYLE_H__
#define __XC_TEXT_STYLE_H__

typedef struct sc_TextStyle c_TextStyle;

/*--- include ---*/
#include "BaseStd.h"
#include "xcalibur.h"
#include "VectorFont.h"
#include "PostScript.h"
#include "Color.h"
#include "TextStyleP.h"
#include "Hook.h"

/*--- define print type  ---*/
#define XcTS_P_GRAPH	1
#define XcTS_P_TEXT	2

/*--- define attributes bitfield  ---*/
#define XcTS_B_WIDTH	  1
#define XcTS_B_BOLD	  2
#define XcTS_B_ITALIC	  4
#define XcTS_B_UNDERLINE  8
#define XcTS_B_STRIKEOUT  16
#define XcTS_B_SUBSCRIPT  32
#define XcTS_B_SUPSCRIPT  64
#define XcTS_B_SHADOW	  128
#define XcTS_B_OUTLINE	  256
#define XcTS_B_SMALL_CAPS 512
#define XcTS_B_BIG_CAPS   1024

/*--- TextStyle to CharMetrics attributes ---*/
#define XcTS_TO_CM(a) \
(((a) & (XcTS_B_OUTLINE|XcTS_B_SMALL_CAPS|XcTS_B_BIG_CAPS)) >> 8)

#define XcTS_GET_WIDTH(style) \
((((style)->attributes & XcTS_H_WIDTH) == 0)? SCALE_ONE : (style)->width.scale)

/*--- define inheritance attributes ---*/
#define XcTS_H_WIDTH	  1
#define XcTS_H_BOLD	  2
#define XcTS_H_ITALIC	  4
#define XcTS_H_UNDERLINE  8
#define XcTS_H_STRIKEOUT  16
#define XcTS_H_SUBSCRIPT  32
#define XcTS_H_SUPSCRIPT  64
#define XcTS_H_SHADOW	  128
#define XcTS_H_OUTLINE	  256
#define XcTS_H_SMALL_CAPS 512
#define XcTS_H_BIG_CAPS   1024
#define XcTS_H_FONT       2048
#define XcTS_H_SIZE       4096
#define XcTS_H_COLOR      8192
#define XcTS_H_BGCOLOR   16384

/*--- Internal structures ---*/
typedef enum
{
 XcTS_END, XcTS_NAME, XcTS_FONT, XcTS_SIZE, 
 XcTS_COLOR, XcTS_BGCOLOR,
 XcTS_HIDE_ON, XcTS_HIDE_OFF, XcTS_ATTRIB,
 XcTS_WIDTH_CONF,
 XcTS_UNDERLINE_CONF, XcTS_STRIKEOUT_CONF, 
 XcTS_SUBSCRIPT_CONF, XcTS_SUPSCRIPT_CONF,
 XcTS_SHADOW_CONF, XcTS_OUTLINE_CONF,
 XcTS_WIDTH_ON, XcTS_WIDTH_OFF,
 XcTS_BOLD_ON, XcTS_BOLD_OFF,
 XcTS_ITALIC_ON, XcTS_ITALIC_OFF, 
 XcTS_UNDERLINE_ON, XcTS_UNDERLINE_OFF, 
 XcTS_STRIKEOUT_ON, XcTS_STRIKEOUT_OFF,
 XcTS_SUBSCRIPT_ON, XcTS_SUBSCRIPT_OFF,
 XcTS_SUPSCRIPT_ON, XcTS_SUPSCRIPT_OFF,
 XcTS_SHADOW_ON, XcTS_SHADOW_OFF,
 XcTS_OUTLINE_ON, XcTS_OUTLINE_OFF,
 XcTS_SMALL_CAPS_ON, XcTS_SMALL_CAPS_OFF,
 XcTS_BIG_CAPS_ON, XcTS_BIG_CAPS_OFF,
 XcTS_INHERIT,
 XcTS_FONT_FAMILY,
} textstyle_set_code_t;

enum textstyle_underline_e 
{
 XcTS_SIMPLE, XcTS_DOUBLE, XcTS_TRIPLE
};

/*--- Define the methods for the TextStyle Class ---*/
typedef struct
{
 F_STD;

 c_TextStyle *(*get) ___PROTO((c_TextStyle *style, BaseStd_t *base, ...));
 void (*set) ___PROTO((c_TextStyle *style, ...));
 boolean (*compare) ___PROTO((c_TextStyle *a, c_TextStyle *b,
			      boolean with_name));
 void (*merge) ___PROTO((c_TextStyle *source, c_TextStyle **target,
			 boolean fusion));
 c_TextStyle *(*virtualCopy) ___PROTO((c_TextStyle *style, BaseStd_t *base));

 /*___Uses function___*/
 void (*markUsed) ___PROTO((c_TextStyle *This));
 void (*markUnused) ___PROTO((c_TextStyle *This));

 /*___Loading & saving___*/
 boolean (*read) ___PROTO((c_DocFile *doc, char *keyword,
			   long param, textstyle_read_t *data));
 boolean (*write) ___PROTO((c_TextStyle *this, c_DocFile *doc));
 boolean (*writeList) ___PROTO((c_TextStyle *this, c_DocFile *doc));

 /*___Printing___*/
 boolean (*ps_print) ___PROTO((c_TextStyle *this, c_PostScript *post));
 boolean (*ps_print_text) ___PROTO((c_TextStyle *this, char *text, int len,
				    c_PostScript *post,
				    coord_t c_x, coord_t c_y,
				    char *fput));
} sf_TextStyle;


/*--- Internal structures ---*/
typedef struct
{
 scale_t scale;			/* Width expansion factor 		     */
} textstyle_width_t;


/*--- Define the TextStyle Class ---*/
struct sc_TextStyle
{
 c_TextStyle *previous;
 c_TextStyle *next;
 c_TextStyle **base;

 sf_TextStyle *f;

 char name[XcMAX_NAME_LENGTH];	/* TextStyle name */
 long id;

 c_VectorFont *font;
 c_CharMetrics *metrics;

 unsigned int attributes;	/* BOLD, ... */
 coord_t point_size;
 c_Color *color;		/* Text color */
 c_Color *bgcolor;		/* Text background color */

 /*int writing_dir;*/		/* not implemented yet... */

 /*___Italic param___*/
 struct
 {
  angle_t angle;		/* Italic angle 			     */
 } italic;

 /*___Width param___*/
 struct
 {
  scale_t  scale;		/* Width horizontal expansion factor */
 } width;

 /*___Underline param___*/
 struct
 {
  scale_t dy;			/* Vertical offset 			     */
  scale_t overstroke;		/* Length to be underlined before/after text */
  scale_t thickness;		/* Thickness of the underlining 	     */
  c_Color *color;		/* Color of the underlining 		     */
  enum textstyle_underline_e type;/* Type of underlining                  */
  boolean overspace;		/* underline space if TRUE		     */
    
  coord_t internal_dy;		/* Reserved... */
  coord_t internal_overstroke;
  coord_t internal_thickness;
 } underline;

 /*___Strikeout___*/
 struct
 {
  scale_t dy;			/* Vertical offset 			     */
  scale_t overstroke;		/* Length to be strikeout before/after text */
  scale_t thickness;		/* Thickness of the strikeout		     */
  c_Color *color;		/* Color of the strikeout		     */
  enum textstyle_underline_e type;/* Type of strikeout                  */
  boolean overspace;		/* underline space if TRUE		     */
    
  coord_t internal_dy;		/* Reserved... */
  coord_t internal_overstroke;
  coord_t internal_thickness;
 } strikeout;
  
 /*___Outline param___*/
 struct
 {
  scale_t thickness;		/* Outline thickness 			     */
  c_Color *color;		/* Outline color			     */

  coord_t internal_thickness;	/* Reserved... */
 } outline;

 /*___Shadow param___*/
 struct
 {
  scale_t dx;			/* Shadow x distance (sign is direction)     */
  scale_t dy;			/* Shadow y distance (sign is direction)     */
  c_Color *color;		/* Shadow color 			     */

  vector_t internal_offset;	/* Reserved... */
 } shadow;

 /*___Subscript___*/
 struct
 {
  scale_t dy;			/* Subscript distance (should be < 0)        */
  scale_t scale;		/* Subscript character point size factor     */

  coord_t internal_dy;		/* Reserved... */
  coord_t internal_point_size;
 } subscript;

 /*___Superscript___*/
 struct
 {
  scale_t dy;			/* Supscript distance (should be > 0)        */
  scale_t scale;		/* Supscript character point size factor     */

  coord_t internal_dy;		/* Reserved... */
  coord_t internal_point_size;
 } supscript;

 /*___Hook___*/
 c_Hook *hook;

 /*___Flags___*/
 boolean lock_flag;
 boolean hide_flag;

 /*___Inherited From___*/
 c_TextStyle	*inherit_from;
 unsigned int	valid_field;	/* bit field of XcTS_H */

 /*___Delayed Destroy___*/
 boolean	is_delayed;
 int		used_count;
};

extern sf_TextStyle fc_TextStyle;
extern boolean text_style_filter ___PROTO((c_TextStyle *style));

#endif /* !__XC_TEXT_STYLE_H__ */
