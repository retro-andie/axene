/*
** XFIGDocFile.c for Xclamation and XAllWrite in XFIGDocFile/
** Methods for thet XFIGDocFile class
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
** Started on  Mon Apr 28 16:41:39 1997 Antoine Buat
** Last update Sat Jan 15 15:08:47 2000 Emmanuel Paris
*/

#if 0
#define TRACE
#define NMEMDEBUG
#undef EFINITIVE_VERSION
#endif

#define FACT	((float)2/3)

/*--- include ---*/
#include "XFIGDocFileP.h"
#include "file.h"

/*--- external ---*/
extern c_VectorFont *GlobFontBase;

/*--- Prototypes ---*/
static void *XFIG_constructor();
static void XFIG_destructor();
static void *XFIG_copy();

static boolean XFIG_readMain();
static int read_int();
static int read_float();
static int read_header();
static int read_record();
static int realize_record();
static int get_polyline();
static int realize_polyline();
static int get_text();
static int realize_text();
static c_VectorFont *load_font();
static int get_spline();
static int realize_spline();
static int get_ellipse();
static int realize_ellipse();
static int get_arc();
static int realize_arc();
static int draw_arrow_head();
static int ctl_spline();
static int itp_spline();
static void arc_tangent();
static int get_compound();
static int ins_object();
static int get_color();
static c_Color *find_color();
static void destroy_objects();

typedef struct
{
 float	c;
 float	m;
 float	y;
 float	k; 
} tcol;

tcol color_table[33]=
{
 {0, 0, 0, 1},
 {0, 0, 0, 1},
 {1, 1, 0, 0},
 {1, 0, 1, 0},
 {1, 0, 0, 0},
 {0, 1, 1, 0},
 {0, 1, 0, 0},
 {0, 0, 1, 0},
 {0, 0, 0, 0},
 {1, 1, 0.5, 0},
 {1, 1, 0.35, 0},
 {1, 1, 0.25, 0},
 {1, 0.3, 0, 0},
 {1, 0.5, 1, 0},
 {1, 0.35, 1, 0},
 {1, 0.25, 1, 0},
 {1, 0.5, 0.5, 0},
 {1, 0.35, 0.35, 0},
 {1, 0.25, 0.25, 0},
 {0.5, 1, 1, 0},
 {0.35, 1, 1, 0},
 {0.25, 1, 1, 0},
 {0.5, 1, 0.5, 0},
 {0.35, 1, 0.35, 0},
 {0.25, 1, 0.25, 0},
 {0.5, 0.8, 1, 0},
 {0.4, 0.7, 1, 0},
 {0.3, 0.6, 1, 0},
 {0, 0.5, 0.5, 0},
 {0, 0.4, 0.4, 0},
 {0, 0.3, 0.3, 0},
 {0, 0.2, 0.2, 0},
 {0, 0.2, 1, 0}
};

typedef struct
{
 char		*name;
 c_VectorFont	*font;
} tfont;

tfont font_table[36]=
{
 {"Times Roman", NULL},
 {"Times Roman", NULL},
 {"Times Italic", NULL},
 {"Times Bold", NULL},
 {"Times Bold Italic", NULL},
 {"AvantGarde Book", NULL},
 {"AvantGarde Book Oblique", NULL},
 {"AvantGarde Demi", NULL},
 {"AvantGarde Demi Oblique", NULL},
 {"Bookman Light", NULL},
 {"Bookman Light Italic", NULL},
 {"Bookman Demi", NULL},
 {"Bookman Demi Oblique", NULL},
 {"Courier", NULL},
 {"Courier Oblique", NULL},
 {"Courier Bold", NULL},
 {"Courier Bold Oblique", NULL},
 {"Helvetica", NULL},
 {"Helvetica Oblique", NULL},
 {"Helvetica Bold", NULL},
 {"Helvetica Bold Oblique", NULL},
 {"Helvetica Narrow", NULL},
 {"Helvetica Narrow Oblique", NULL},
 {"Helvetica Narrow Bold", NULL},
 {"Helvetica Narrow Bold Oblique", NULL},
 {"New Century Schoolbook Roman", NULL},
 {"New Century Schoolbook Italic", NULL},
 {"New Century Schoolbook Bold", NULL},
 {"New Century Schoolbook Bold Italic", NULL},
 {"Palatino Roman", NULL},
 {"Palatino Italic", NULL},
 {"Palatino Bold", NULL},
 {"Palatino Bold Italic", NULL},
 {"Symbol", NULL},
 {"Zapf Chancery Medium Italic", NULL},
 {"Zapf Dingbats", NULL}
};

/*--- fill methods ---*/
sf_XFIGDocFile fc_XFIGDocFile =
{
 XFIG_constructor,
 XFIG_destructor,
 XFIG_copy,
 XFIG_readMain
};

/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *XFIG_constructor(filename, vector)
char		*filename;
c_VectorGraph	*vector;
{
 c_XFIGDocFile	*p;

 Xc_HISTORY(("constructor(`%s')", filename));
 if((p = Xc_malloc("XFIGDocFile class", sizeof(c_XFIGDocFile))) == NULL)
  return NULL;
 p->f = &fc_XFIGDocFile;

 p->filename = Xc_strdup("filename", filename? filename: "-stream-");
 if(p->filename == NULL)
 {
  Xc_free(p);
  return NULL;
 }

 p->fd = fopen(filename, "r");
 if(!p->fd)
 {
  Xc_SYSERROR((filename));
  Xc_free(p->filename);
  Xc_free(p);
  return NULL;
 }
 p->close_flag = TRUE;

 p->vector = vector;
 vector->filename = p->filename;
 p->color_base = &vector->basestd->color_base;
 p->list = NULL;
 p->last_obj = NULL;
 p->pins_obj = &(p->list);
 p->factor80 = 80;
 p->num_col = 0;

 return p;
}

/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void XFIG_destructor(this)
c_XFIGDocFile *this;
{
 Xc_HISTORY(("destructor(`%s')", GetBasename(this->filename, FALSE)));
 if(this->close_flag) 
  fclose(this->fd);
 /*Xc_free(this->filename);*/  
 Xc_free(this);
}

/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *XFIG_copy(this)
c_XFIGDocFile *this;
{
 Xc_HISTORY(("copy not implemented"));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** readMain - Main Read Part                                         ** 
** ----------------------------------------------------------------- */
static boolean XFIG_readMain(this)
c_XFIGDocFile *this;
{
 int	ret;

 fseek(this->fd, 0, SEEK_SET);

 if (read_header(this, this->fd) == -1)
  return FALSE;
 
 ret = 1;
 while (ret != -2)
 {
  ret = read_record(this, this->fd);
  if (ret == -1)
   return FALSE;
 }
 if (realize_record(this) == -1)
  return -1;

 destroy_objects(this);
 
 if(!F(this->vector).end(this->vector)) 
  return FALSE;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** read_int - read integer from FILE                                 ** 
** ----------------------------------------------------------------- */
static int read_int(fd, num)
FILE	*fd;
int	*num;
{
 char	tchar;
 int	mul;

 tchar = fgetc(fd);
 while ((tchar != (char)EOF) && 
	((tchar == ' ') || (tchar == '\t') || (tchar == '\n')))
  tchar = fgetc(fd);
 if (tchar == '-')
 {
  mul = -1;
  tchar = fgetc(fd);
 }
 else
  mul = 1;

 if ((tchar == (char)EOF) || (tchar<'0') || (tchar>'9'))
  return -1;
 
 *num = 0;
 while ((tchar != (char)EOF) && (tchar>='0') && (tchar<='9')) 
 {
  *num *= 10;
  *num += tchar-'0';
  tchar = fgetc(fd);
 }
 if (tchar != (char)EOF)
  ungetc(tchar, fd);
 *num *= mul;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** read_float - read float from FILE                                 ** 
** ----------------------------------------------------------------- */
static int read_float(fd, num)
FILE	*fd;
float	*num;
{
 char	tchar;
 int	hpart;
 int	lpart;
 int	fdec;
 int	mul;

 tchar = fgetc(fd);
 while ((tchar != (char)EOF) && 
	((tchar == ' ') || (tchar == '\t') || (tchar == '\n')))
  tchar = fgetc(fd);
 if (tchar == '-')
 {
  mul = -1;
  tchar = fgetc(fd);
 }
 else
  mul = 1;
 if ((tchar == (char)EOF) || (tchar<'0') || (tchar>'9'))
  return -1;
 
 hpart = 0;
 while ((tchar != (char)EOF) && (tchar>='0') && (tchar<='9')) 
 {
  hpart *= 10;
  hpart += tchar-'0';
  tchar = fgetc(fd);
 }
 if (tchar != '.')
 {
  *num = hpart;
  return 1;
 }
 tchar = fgetc(fd);
 lpart = 0;
 fdec = 1;
 while ((tchar != (char)EOF) && (tchar>='0') && (tchar<='9')) 
 {
  fdec *= 10;
  lpart *= 10;
  lpart += tchar-'0';
  tchar = fgetc(fd);
 }
 if (tchar != (char)EOF)
  ungetc(tchar, fd);
 *num = mul * (hpart + (float)lpart/fdec);
 return 1;
}

/* ----------------------------------------------------------------- ** 
** read_string - read string from FILE                               ** 
** ----------------------------------------------------------------- */
static int read_string(this, fd, str, num)
c_XFIGDocFile	*this;
FILE		*fd;
char		*str;
int		num;
{
 int	i;

 i = 0;
 str[0] = fgetc(fd);
 while ((i < num) && (str[i] != (char)EOF) && (str[i] != '\1'))
 {
  i++;
  str[i] = fgetc(fd);
  if ((this->version > 21) && (str[i] == '\\'))
  {
   char	num[3];
   num[0] = fgetc(fd);
   if (num[0] != '\\')
   {
    num[1] = fgetc(fd);
    num[2] = fgetc(fd);
    str[i] = (num[0]-'0')*64 + (num[1]-'0')*8 + num[2]-'0';
   }
  }
 }
 str[i] = 0;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** read_header -- decode header to see if really a XFIG              ** 
** ----------------------------------------------------------------- */
static int read_header(this, fd)
c_XFIGDocFile	*this;
FILE		*fd;
{
 char	buffer_line[1000];
 double	version;
 char	*ret;
 int	pos;
 int	num;

 ret = fgets(buffer_line, 1000, fd);
 if (!ret || strncmp(buffer_line, XFIGMAGIK, XFIGMAGIK_LEN))
  return -1;
 version = atof(buffer_line+XFIGMAGIK_LEN);
 this->version = version*10;

 while (ret)
 {
  buffer_line[0] = fgetc(fd);
  if (buffer_line[0] == (char)EOF)
  {
   ret = 0;
   break;
  }
  if (buffer_line[0] == '#')
   ret = fgets(buffer_line, 1000, fd);
  else
  {
   ungetc(buffer_line[0], fd);
   ret = (char *)1;
   break;
  }
 }
 if (!ret)
  return -1;
 pos = 0;

 if (this->version > 21)
 {
  fgets(buffer_line, 1000, fd);
  if (!strcmp("Landscape", buffer_line))
   this->orientation = 0;
  else
   this->orientation = 1;

  fgets(buffer_line, 1000, fd);
  if (!strcmp("Center", buffer_line))
   this->justification = 0;
  else
   this->justification = 1;

  fgets(buffer_line, 1000, fd);
  if (!strcmp("Inches", buffer_line))
   this->units = 0;
  else
   this->units = 1;
 }

 if (read_int(fd, &this->factor) == -1)
  return -1;
 if (read_int(fd, &num) == -1)
  return -1;
 if (num==2)
  this->neg_y = -1;
 else
  this->neg_y = 1;
  
 return 1;
}

/* ----------------------------------------------------------------- ** 
** read_record - read a record object                                ** 
** ----------------------------------------------------------------- */
static int read_record(this, fd)
c_XFIGDocFile	*this;
FILE		*fd;
{
 char	buffer_line[1000];
 char	tchar;
 int	type;

 tchar = fgetc(fd);
 if (tchar == (char)EOF)
  return -2;
 if (tchar == '#')
 {
  fgets(buffer_line, 1000, fd);
  return 1;
 }
 ungetc(tchar, fd);
 if (read_int(fd, &type) == -1)
  return -2;
 switch (type)
 {
 case 0:
  if (get_color(this, fd) == -1)
   return -1;
  break;
 case 1:
  if (get_ellipse(this, fd) == -1)
   return -1;
  break;
 case 2:
  if (get_polyline(this, fd) == -1)
   return -1;
  break;
 case 3:
  if (get_spline(this, fd) == -1)
   return -1;
  break;
 case 4:
  if (get_text(this, fd) == -1)
   return -1;
  break;
 case 5:
  if (get_arc(this, fd) == -1)
   return -1;
  break;
 case -6:
  /*--- compound end ---*/
  break;
 case 6:
  if (get_compound(this, fd) == -1)
   return -1;
  break;
 default:
  Xc_WARNING(("unknown object\n"));
 }

 return 1;
}

/* ----------------------------------------------------------------- ** 
** realize record                                                    ** 
** ----------------------------------------------------------------- */
static int realize_record(this)
c_XFIGDocFile	*this;
{
 obj_list	*list;
 list = this->list;
 while (list)
 {
  switch(list->object->type)
  {
  case 1:
   if (realize_ellipse(this, &(list->object->ellipse)) == -1)
    return -1;
   break;
  case 2:
   if (realize_polyline(this, &(list->object->polyline)) == -1)
    return -1;
   break;
  case 3:
   if (realize_spline(this, &(list->object->spline)) == -1)
    return -1;
   break;
  case 4:
   if (realize_text(this, &(list->object->text)) == -1)
    return -1;
   break;
  case 5:
   if (realize_arc(this, &(list->object->arc)) == -1)
    return -1;
   break;   
  }
  list = list->Next;
 }
 return 1;
}


/* ----------------------------------------------------------------- ** 
** get_polyline -- decode polyline func                              ** 
** ----------------------------------------------------------------- */
static int get_polyline(this, fd)
c_XFIGDocFile	*this;
FILE		*fd;
{
 char		pic_name[50];
 fig_polyline	*object;
 t_spoint	**ppoints;
 int		num;
 int		i;
 int		tx;
 int		ty;

 object = (fig_polyline *)Xc_malloc("object", sizeof(fig_obj));
 object->type = 2;
 if (read_int(fd, &(object->sub_type)) == -1)
  return -1;
 if (read_int(fd, &(object->line_style)) == -1)
  return -1;
 if (read_int(fd, &(object->thickness)) == -1)
  return -1;
 if (read_int(fd, &(object->pen_color)) == -1)
  return -1;
 if (this->version > 21)
 {
  if (read_int(fd, &(object->fill_color)) == -1)
   return -1;
 }
 else
  object->fill_color = object->pen_color;
 if (read_int(fd, &(object->depth)) == -1)
  return -1;
 if (read_int(fd, &(object->pen_style)) == -1)
  return -1;
 if (read_int(fd, &(object->area_fill)) == -1)
  return -1;
 if (this->version > 21)
 {
  if (object->area_fill > 20)
   object->area_fill = 0;
  else
   object->area_fill++;
 }
 if (read_float(fd, &(object->style_val)) == -1)
  return -1;
 if (this->version > 21)
 {
  if (read_int(fd, &(object->join_style)) == -1)
   return -1;
  if (read_int(fd, &(object->cap_style)) == -1)
   return -1;
 }
 else
 {
  object->join_style = 0;
  object->cap_style = 0;
 }
 if (read_int(fd, &(object->radius)) == -1)
  return -1;
 if (read_int(fd, &(object->forward_arrow)) == -1)
  return -1;
 if (read_int(fd, &(object->backward_arrow)) == -1)
  return -1;
 if (this->version > 21)
 {
  if (read_int(fd, &(object->npoints)) == -1)
   return -1;
 }
 else
  object->npoints = -1;
 if (object->forward_arrow)
 {
  if (read_int(fd, &(object->fore_arrow.type)) == -1)
   return -1;
  if (read_int(fd, &(object->fore_arrow.style)) == -1)
   return -1;
  if (read_float(fd, &(object->fore_arrow.thickness)) == -1)
   return -1;
  if (read_float(fd, &(object->fore_arrow.width)) == -1)
   return -1;
  if (read_float(fd, &(object->fore_arrow.height)) == -1)
   return -1;
 }
 if (object->backward_arrow)
 {
  if (read_int(fd, &(object->back_arrow.type)) == -1)
   return -1;
  if (read_int(fd, &(object->back_arrow.style)) == -1)
   return -1;
  if (read_float(fd, &(object->back_arrow.thickness)) == -1)
   return -1;
  if (read_float(fd, &(object->back_arrow.width)) == -1)
   return -1;
  if (read_float(fd, &(object->back_arrow.height)) == -1)
   return -1;
 }
 if (object->sub_type == 5)
 {
  if (read_int(fd, &(object->flipped)) == -1)
   return -1;
  fgets(pic_name, 50, fd);
  object->pic_name = Xc_malloc("pic", strlen(pic_name)+1);
  strcpy(object->pic_name, pic_name);
 }
 else
  object->pic_name = NULL;

 i = object->npoints;
 ppoints = &(object->points);
 num = 0;
 while (i)
 {
  if (read_int(fd, &tx) == -1)
   return -1;
  if (read_int(fd, &ty) == -1)
   return -1;
  if ((this->version <= 21) && (tx == 9999) && (ty == 9999))
   break;
  *ppoints = Xc_malloc("points", sizeof(t_spoint));
  (*ppoints)->x = tx;
  (*ppoints)->y = ty;
  (*ppoints)->next = NULL;
  ppoints = &(*ppoints)->next;
  num++;
  i--;
 }
 object->npoints = num;
 if (ins_object(this, object) == -1)
  return -1;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** realize_polyline                                                  ** 
** ----------------------------------------------------------------- */
static int realize_polyline(this, object)
c_XFIGDocFile	*this;
fig_polyline	*object;
{
 c_Color	*ocolor;
 c_Color	*fcolor;
 int		mode;
 t_spoint	*points;
 int		x;
 int		y;
 int		ox;
 int		oy;
 int		num;

 ox = 0;
 oy = 0;
 ocolor = find_color(this, object->pen_color, -1);
 if((ocolor != NULL) && 
    !F(this->vector).setStrokeColor(this->vector, ocolor))
  return -1;

 if (object->area_fill)
 {
  fcolor = find_color(this, object->fill_color, object->area_fill);
  if((fcolor != NULL) && 
     !F(this->vector).setFillColor(this->vector, fcolor))
   return -1;
 }

 /*--- special case for eps file ---*/
 if (object->sub_type == 5)
 {
/*return 1;*/
  /* nothing but continue */
 }

 /*--- special case for arc box ---*/
 if (object->sub_type == 4)
 {
  t_spoint	*points;
  int		l;
  int		t;
  int		r;
  int		b;
  int		x;
  int		radius;

  if (object->thickness)
  {
   if(!F(this->vector).
      setLineWidth(this->vector, 
		   SCALE_FROM_INCHES(object->thickness)/this->factor80))
    return -1;
  }

  points = object->points;
  l = points->x;
  t = points->y;
  points = points->next;
  r = points->x;
  b = points->y;
  points = points->next;
  if (l != r)
  { 
   b = points->y;
  }
  else
  {
   r= points->x;
  }
  if (l > r)
  {
   x = l;
   l = r;
   r = x;
  }
  if (t > b)
  {
   x = t;
   t = b;
   b = x;
  }

  radius = object->radius*this->factor/this->factor80;
  if(!F(this->vector).move(this->vector,
			   SCALE_FROM_INCHES(l)/this->factor,
			   SCALE_FROM_INCHES(this->neg_y * 
					     (t + radius))
			   /this->factor))
   return -1;
  if(!F(this->vector).arc(this->vector,
			  SCALE_FROM_INCHES(l + radius)/this->factor,
			  SCALE_FROM_INCHES(this->neg_y * 
					    (t + radius))/this->factor,
			  SCALE_FROM_INCHES(radius)/this->factor,
			  SCALE_FROM_INCHES(radius)/this->factor,
			  SCALE_FROM_DEGREES(180),
			  SCALE_FROM_DEGREES(90)))
   return -1;

  if(!F(this->vector).line(this->vector,
			   SCALE_FROM_INCHES(r - radius)/this->factor,
			   SCALE_FROM_INCHES(this->neg_y * t)/this->factor))
   return -1;
  if(!F(this->vector).arc(this->vector,
			  SCALE_FROM_INCHES(r - radius)/this->factor,
			  SCALE_FROM_INCHES(this->neg_y * (t + radius))
			  /this->factor,
			  SCALE_FROM_INCHES(radius)/this->factor,
			  SCALE_FROM_INCHES(radius)/this->factor,
			  SCALE_FROM_DEGREES(90),
			  SCALE_FROM_DEGREES(0)))
   return -1;
  if(!F(this->vector).line(this->vector,
			   SCALE_FROM_INCHES(r)/this->factor,
			   SCALE_FROM_INCHES(this->neg_y * 
					     (b - radius))
			   /this->factor))
   return -1;
  if(!F(this->vector).arc(this->vector,
			  SCALE_FROM_INCHES(r - radius)/this->factor,
			  SCALE_FROM_INCHES(this->neg_y * (b - radius))
			  /this->factor,
			  SCALE_FROM_INCHES(radius)/this->factor,
			  SCALE_FROM_INCHES(radius)/this->factor,
			  SCALE_FROM_DEGREES(0),
			  SCALE_FROM_DEGREES(-90)))
   return -1;
  if(!F(this->vector).line(this->vector,
			   SCALE_FROM_INCHES(l + radius)/this->factor,
			   SCALE_FROM_INCHES(this->neg_y * b)/this->factor))
   return -1;
  if(!F(this->vector).arc(this->vector,
			  SCALE_FROM_INCHES(l + radius)/this->factor,
			  SCALE_FROM_INCHES(this->neg_y * (b - radius))
			  /this->factor,
			  SCALE_FROM_INCHES(radius)/this->factor,
			  SCALE_FROM_INCHES(radius)/this->factor,
			  SCALE_FROM_DEGREES(270),
			  SCALE_FROM_DEGREES(180)))
   return -1;
  if(!F(this->vector).closePath(this->vector))
   return -1;
  if (object->thickness)
   mode = XcVG_STROKE;
  else
   mode = 0;
  if (object->area_fill)
   mode |= XcVG_FILL;  
  if (!F(this->vector).render(this->vector, mode))
   return -1;
  return 1;
 }

 num = object->npoints;
 points = object->points;
 x = points->x;
 y = points->y;
 points = points->next;
 num--;

 if (!num)
 {
  object->backward_arrow = 0;
  object->forward_arrow = 0;
 }

 /*--- special case for backward arrow ---*/
 if ((object->sub_type == 1) && (object->backward_arrow) && 
     (object->back_arrow.thickness))
 {
  int		x1;
  int		y1;

  ox = x;
  oy = y;
  x1 = points->x;
  y1 = points->y;
  points = points->next;
  num --;

  if(!F(this->vector).
     setLineWidth(this->vector, 
		  SCALE_FROM_INCHES(object->back_arrow.thickness)
		  /this->factor80))
   return -1;
  draw_arrow_head(this, (float)x1, (float)y1, (float)x, (float)y, 
		  (float)object->back_arrow.height,
		  (float)object->back_arrow.width);
  if (!F(this->vector).render(this->vector, XcVG_STROKE))
   return -1;

  /*--- return to draw normal ---*/
  if(!F(this->vector).
     setLineWidth(this->vector, 
		  SCALE_FROM_INCHES(object->thickness)/this->factor80))
   return -1;
  if(!F(this->vector).move(this->vector,
			   SCALE_FROM_INCHES(x)/this->factor,
			   SCALE_FROM_INCHES(this->neg_y * y)/this->factor))
   return -1;
  if(!F(this->vector).line(this->vector,
			   SCALE_FROM_INCHES(x1)/this->factor,
			   SCALE_FROM_INCHES(this->neg_y * y1)/this->factor))
   return -1;
  x = x1;
  y = y1;
 }
 else
 {
  if(!F(this->vector).
     setLineWidth(this->vector, 
		  SCALE_FROM_INCHES(object->thickness)/this->factor80))
   return -1;
  if(!F(this->vector).move(this->vector,
			   SCALE_FROM_INCHES(x)/this->factor,
			   SCALE_FROM_INCHES(this->neg_y * y)/this->factor))
   return -1;
 }

 /*--- read points ---*/
 while (num)
 {
  ox = x;
  oy = y;
  x = points->x;
  y = points->y;
  points = points->next;
  num --;
  if(!F(this->vector).line(this->vector,
			   SCALE_FROM_INCHES(x)/this->factor,
			   SCALE_FROM_INCHES(this->neg_y * y)/this->factor))
   return -1;
 }

 if (object->sub_type > 1)
  if(!F(this->vector).closePath(this->vector))
   return -1;
 if (object->thickness)
  mode = XcVG_STROKE;
 else
  mode = 0;
 if (object->area_fill)
  mode |= XcVG_FILL;  
 if (!F(this->vector).render(this->vector, mode))
  return -1;

  /*--- special case for foreward arrow ---*/
 if ((object->sub_type == 1) && (object->forward_arrow) && 
     (object->fore_arrow.thickness))
 {
  if(!F(this->vector).
     setLineWidth(this->vector, 
		  SCALE_FROM_INCHES(object->fore_arrow.thickness)
		  /this->factor80))
   return -1;
  draw_arrow_head(this, (float)ox, (float)oy, (float)x, (float)y,
		  (float)object->fore_arrow.height,
		  (float)object->fore_arrow.width);
  if (!F(this->vector).render(this->vector, XcVG_STROKE))
   return -1;
 }
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_text -- decode text func                                      ** 
** ----------------------------------------------------------------- */
static int get_text(this, fd)
c_XFIGDocFile	*this;
FILE		*fd;
{
 char		str[1000];
 fig_text	*object;

 object = (fig_text *)Xc_malloc("object", sizeof(fig_obj));
 object->type = 4;
 if (read_int(fd, &(object->sub_type)) == -1)
  return -1;
 if (this->version > 21)
 {
  if (read_int(fd, &(object->color)) == -1)
   return -1;
  if (read_int(fd, &(object->depth)) == -1)
   return -1;
  if (read_int(fd, &(object->pen_style)) == -1)
   return -1;
 }
 if (read_int(fd, &(object->font)) == -1)
  return -1;
 if (read_float(fd, &(object->font_size)) == -1)
  return -1;

 if (this->version <= 21)
 {
  if (read_int(fd, &(object->pen_style)) == -1)
   return -1;
  if (read_int(fd, &(object->color)) == -1)
   return -1;
  if (read_int(fd, &(object->depth)) == -1)
   return -1;
 }
 if (read_float(fd, &(object->angle)) == -1)
  return -1;
 if (read_int(fd, &(object->font_flags)) == -1)
  return -1;
 if (read_float(fd, &(object->height)) == -1)
  return -1;
 if (read_float(fd, &(object->length)) == -1)
  return -1;
 if (read_int(fd, &(object->x)) == -1)
  return -1;
 if (read_int(fd, &(object->y)) == -1)
  return -1;
 fgetc(fd);			/* skip first space */

 if (read_string(this, fd, str, 1000) == -1)
  return -1;
 object->str_len = strlen(str);
 object->string = Xc_malloc("string", object->str_len + 1);
 strcpy(object->string, str);
 if (ins_object(this, object) == -1)
  return -1;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** realize_text                                                      ** 
** ----------------------------------------------------------------- */
static int realize_text(this, object)
c_XFIGDocFile	*this;
fig_text	*object;
{
 c_VectorFont	*ofont;
 matrix_t	matrix;
 c_Color	*ocolor;
 c_CharMetrics	*cm;
 coord_t	t_length;
 coord_t	t_dx;
 coord_t	t_dy;

 ocolor = find_color(this, object->color, -1);
 if((ocolor != NULL) && 
    !F(this->vector).setFillColor(this->vector, ocolor))
  return -1;

 ofont = load_font(this, object->font);
 if (!ofont)
  return -1;
 /* this scale seem to correspond better to reality */
 if(!F(this->vector).setTextFont
    (this->vector, ofont, SCALE_FROM_POINTS(object->font_size*(float)92/100)))
  return -1;

 XcM_INIT_MATRIX(&matrix,
		 cos(object->angle), 
		 sin(object->angle),
		 -sin(object->angle),
		 cos(object->angle), 
		 0, 0);
 if(!F(this->vector).setTextMatrix(this->vector, &matrix))
  return -1;

 if (object->sub_type > 0)
 {
  cm = COPY(c_CharMetrics)(ofont->metrics,
			   NULL,
			   SCALE_FROM_POINTS(object->font_size*(float)92/100),
			   SCALE_FROM_COEF(1.0),
			   SCALE_FROM_DEGREES(0.0),
			   0, TRUE);
  if(cm != NULL)
  {
   t_length = F(cm).textWidth(cm, object->string, object->str_len);
   DELETE(c_CharMetrics)(cm);
  }  
  else
   t_length = 0;
  if (object->sub_type == 1)
   t_length /= 2;
 }
 else
  t_length = 0;

 t_dx = (SCALE_FROM_INCHES(object->x)/this->factor) 
  - t_length*cos(object->angle);
 t_dy = (SCALE_FROM_INCHES(this->neg_y * object->y)/this->factor) 
  -  t_length*sin(object->angle);
 if(!F(this->vector).
    setTextPoint(this->vector, 
		 (int)(cos(-object->angle)*t_dx + -sin(-object->angle)*t_dy),
		 (int)(sin(-object->angle)*t_dx + cos(-object->angle)*t_dy)))
  return -1;
 
 if(!F(this->vector).setTextMode(this->vector, 0))
  return -1;

 if(!F(this->vector).textRender(this->vector, object->string, object->str_len))
  return -1;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** load_font                                                         ** 
** ----------------------------------------------------------------- */
static c_VectorFont *load_font(this, num)
c_XFIGDocFile	*this;
int		num;
{
 c_VectorFont *font;

 num ++;
 if (num < 0)
  num = 0;
 if (num > 35)
  num = 0;
 if (!font_table[num].font)
 {
  font = F(GlobFontBase).getFontByName(GlobFontBase, font_table[num].name);
  if (!font)
   font_table[num].font = F(GlobFontBase).selectDialog(GlobFontBase, 
						       font_table[num].name);
  else
   font_table[num].font = font;
 }
 return font_table[num].font;
}

/* ----------------------------------------------------------------- ** 
** get_spline -- decode spline func                                  ** 
** ----------------------------------------------------------------- */
static int get_spline(this, fd)
c_XFIGDocFile	*this;
FILE		*fd;
{
 fig_spline	*object;
 t_spoint	**ppoints;
 t_scontrol	**pcontrols;
 int		num;
 int		i;
 int		tx;
 int		ty;

 object = (fig_spline *)Xc_malloc("object", sizeof(fig_obj));
 object->type = 3;
 if (read_int(fd, &(object->sub_type)) == -1)
  return -1;
 if (read_int(fd, &(object->line_style)) == -1)
  return -1;
 if (read_int(fd, &(object->thickness)) == -1)
  return -1;
 if (read_int(fd, &(object->pen_color)) == -1)
  return -1;
 if (this->version > 21)
 {
  if (read_int(fd, &(object->fill_color)) == -1)
   return -1;
 }
 else
  object->fill_color = object->pen_color;
 if (read_int(fd, &(object->depth)) == -1)
  return -1;
 if (read_int(fd, &(object->pen_style)) == -1)
  return -1;
 if (read_int(fd, &(object->area_fill)) == -1)
  return -1;
 if (this->version > 21)
 {
  if (object->area_fill > 20)
   object->area_fill = 0;
  else
   object->area_fill++;
 }
 if (read_float(fd, &(object->style_val)) == -1)
  return -1;
 if (this->version > 21)
 {
  if (read_int(fd, &(object->cap_style)) == -1)
   return -1;
 }
 else
  object->cap_style = 0;
 if (read_int(fd, &(object->forward_arrow)) == -1)
  return -1;
 if (read_int(fd, &(object->backward_arrow)) == -1)
  return -1;
 if (this->version > 21)
 {
  if (read_int(fd, &(object->npoints)) == -1)
   return -1;
 }
 else
  object->npoints = -1;
 if (object->forward_arrow)
 {
  if (read_int(fd, &(object->fore_arrow.type)) == -1)
   return -1;
  if (read_int(fd, &(object->fore_arrow.style)) == -1)
   return -1;
  if (read_float(fd, &(object->fore_arrow.thickness)) == -1)
   return -1;
  if (read_float(fd, &(object->fore_arrow.width)) == -1)
   return -1;
  if (read_float(fd, &(object->fore_arrow.height)) == -1)
   return -1;
 }
 if (object->backward_arrow)
 {
  if (read_int(fd, &(object->back_arrow.type)) == -1)
   return -1;
  if (read_int(fd, &(object->back_arrow.style)) == -1)
   return -1;
  if (read_float(fd, &(object->back_arrow.thickness)) == -1)
   return -1;
  if (read_float(fd, &(object->back_arrow.width)) == -1)
   return -1;
  if (read_float(fd, &(object->back_arrow.height)) == -1)
   return -1;
 }

 i = object->npoints;
 ppoints = &(object->points);
 num = 0;
 while (i)
 {
  if (read_int(fd, &tx) == -1)
   return -1;
  if (read_int(fd, &ty) == -1)
   return -1;
  if ((this->version <= 21) && (tx == 9999) && (ty == 9999))
   break;
  *ppoints = Xc_malloc("points", sizeof(t_spoint));
  (*ppoints)->x = tx;
  (*ppoints)->y = ty;
  (*ppoints)->next = NULL;
  ppoints = &(*ppoints)->next;
  num++;
  i--;
 }
 object->npoints = num;

 if (object->sub_type > 1)
 {
  pcontrols = &(object->controls);
  while (num)
  {
   *pcontrols = Xc_malloc("controls", sizeof(t_scontrol));
   if (read_float(fd, &((*pcontrols)->lx)) == -1)
    return -1;
   if (read_float(fd, &((*pcontrols)->ly)) == -1)
    return -1;
   if (read_float(fd, &((*pcontrols)->rx)) == -1)
    return -1;
   if (read_float(fd, &((*pcontrols)->ry)) == -1)
    return -1;
   (*pcontrols)->next = NULL;
   pcontrols = &(*pcontrols)->next;
   num --;
  }
 }
 else
  object->controls = NULL;
 if (ins_object(this, object) == -1)
  return -1;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** realize_spline                                                    ** 
** ----------------------------------------------------------------- */
static int realize_spline(this, object)
c_XFIGDocFile	*this;
fig_spline	*object;
{
 c_Color	*ocolor;
 c_Color	*fcolor;

 ocolor = find_color(this, object->pen_color, -1);
 if((ocolor != NULL) && 
    !F(this->vector).setStrokeColor(this->vector, ocolor))
  return -1;

 if (object->area_fill)
 {
  fcolor = find_color(this, object->fill_color, object->area_fill);
  if((fcolor != NULL) && 
     !F(this->vector).setFillColor(this->vector, fcolor))
   return -1;
 }

 if (object->sub_type < 2)
  ctl_spline(this, object);
 else
  itp_spline(this, object);
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_ellipse -- decode ellipse func                                ** 
** ----------------------------------------------------------------- */
static int get_ellipse(this, fd)
c_XFIGDocFile	*this;
FILE		*fd;
{
 fig_ellipse	*object;

 object = (fig_ellipse *)Xc_malloc("object", sizeof(fig_obj));
 object->type = 1;
 if (read_int(fd, &(object->sub_type)) == -1)
  return -1;
 if (read_int(fd, &(object->line_style)) == -1)
  return -1;
 if (read_int(fd, &(object->thickness)) == -1)
  return -1;
 if (read_int(fd, &(object->pen_color)) == -1)
  return -1;
 if (this->version > 21)
 {
  if (read_int(fd, &(object->fill_color)) == -1)
   return -1;
 }
 else
  object->fill_color = object->pen_color;
 if (read_int(fd, &(object->depth)) == -1)
  return -1;
 if (read_int(fd, &(object->pen_style)) == -1)
  return -1;
 if (read_int(fd, &(object->area_fill)) == -1)
  return -1;
 if (this->version > 21)
 {
  if (object->area_fill > 20)
   object->area_fill = 0;
  else
   object->area_fill++;
 }
 if (read_float(fd, &(object->style_val)) == -1)
  return -1;
 if (read_int(fd, &(object->direction)) == -1)
  return -1;
 if (read_float(fd, &(object->angle)) == -1)
  return -1;
 if (read_int(fd, &(object->center_x)) == -1)
  return -1;
 if (read_int(fd, &(object->center_y)) == -1)
  return -1;
 if (read_int(fd, &(object->radius_x)) == -1)
  return -1;
 if (read_int(fd, &(object->radius_y)) == -1)
  return -1;
 if (read_int(fd, &(object->start_x)) == -1)
  return -1;
 if (read_int(fd, &(object->start_y)) == -1)
  return -1;
 if (read_int(fd, &(object->end_x)) == -1)
  return -1;
 if (read_int(fd, &(object->end_y)) == -1)
  return -1;
 if (ins_object(this, object) == -1)
  return -1;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** realize_ellipse                                                   ** 
** ----------------------------------------------------------------- */
static int realize_ellipse(this, object)
c_XFIGDocFile	*this;
fig_ellipse	*object;
{
 c_Color	*ocolor;
 c_Color	*fcolor;
 int		mode;

 ocolor = find_color(this, object->pen_color, -1);
 if((ocolor != NULL) && 
    !F(this->vector).setStrokeColor(this->vector, ocolor))
  return -1;

 if (object->area_fill)
 {
  fcolor = find_color(this, object->fill_color, object->area_fill);
  if((fcolor != NULL) && 
     !F(this->vector).setFillColor(this->vector, fcolor))
   return -1;
 }
 if(!F(this->vector).
    setLineWidth(this->vector, 
		 SCALE_FROM_INCHES(object->thickness)/this->factor80))
  return -1;

 if(!F(this->vector).move
    (this->vector,
     SCALE_FROM_INCHES(object->center_x+object->radius_x)/this->factor,
     this->neg_y*SCALE_FROM_INCHES(object->center_y)/this->factor))
  return -1;
 if(!F(this->vector).arc(this->vector,
			 SCALE_FROM_INCHES(object->center_x)/this->factor,
			 this->neg_y*SCALE_FROM_INCHES(object->center_y)
			 /this->factor,
			 SCALE_FROM_INCHES(object->radius_x)/this->factor,
			 SCALE_FROM_INCHES(object->radius_y)/this->factor,
			 SCALE_FROM_DEGREES(0),
			 SCALE_FROM_DEGREES(360)))
  return -1;
 if(!F(this->vector).closePath(this->vector))
  return -1;

 if (object->thickness)
  mode = XcVG_STROKE;
 else
  mode = 0;
 if (object->area_fill)
  mode |= XcVG_FILL;  
 if (!F(this->vector).render(this->vector, mode))
  return -1;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_arc -- decode arc func                                        ** 
** ----------------------------------------------------------------- */
static int get_arc(this, fd)
c_XFIGDocFile	*this;
FILE		*fd;
{
 fig_arc	*object;

 object = (fig_arc *)Xc_malloc("object", sizeof(fig_obj));
 object->type = 5;
 if (read_int(fd, &(object->sub_type)) == -1)
  return -1;
 if (read_int(fd, &(object->line_style)) == -1)
  return -1;
 if (read_int(fd, &(object->thickness)) == -1)
  return -1;
 if (read_int(fd, &(object->pen_color)) == -1)
  return -1;
 if (this->version > 21)
 {
  if (read_int(fd, &(object->fill_color)) == -1)
   return -1;
 }
 else
  object->fill_color = object->pen_color;
 if (read_int(fd, &(object->depth)) == -1)
  return -1;
 if (read_int(fd, &(object->pen_style)) == -1)
  return -1;
 if (read_int(fd, &(object->area_fill)) == -1)
  return -1;
 if (this->version > 21)
 {
  if (object->area_fill > 20)
   object->area_fill = 0;
  else
   object->area_fill++;
 }
 if (read_float(fd, &(object->style_val)) == -1)
  return -1;
 if (this->version > 21)
 {
  if (read_int(fd, &(object->cap_style)) == -1)
   return -1;
 }
 else
  object->cap_style = 0;
 if (read_int(fd, &(object->direction)) == -1)
  return -1;
 if (read_int(fd, &(object->forward_arrow)) == -1)
  return -1;
 if (read_int(fd, &(object->backward_arrow)) == -1)
  return -1;
 if (read_float(fd, &(object->center_x)) == -1)
  return -1;
 if (read_float(fd, &(object->center_y)) == -1)
  return -1;
 if (read_int(fd, &(object->x1)) == -1)
  return -1;
 if (read_int(fd, &(object->y1)) == -1)
  return -1;
 if (read_int(fd, &(object->x2)) == -1)
  return -1;
 if (read_int(fd, &(object->y2)) == -1)
  return -1;
 if (read_int(fd, &(object->x3)) == -1)
  return -1;
 if (read_int(fd, &(object->y3)) == -1)
  return -1;
 if (object->forward_arrow)
 {
  if (read_int(fd, &(object->fore_arrow.type)) == -1)
   return -1;
  if (read_int(fd, &(object->fore_arrow.style)) == -1)
   return -1;
  if (read_float(fd, &(object->fore_arrow.thickness)) == -1)
   return -1;
  if (read_float(fd, &(object->fore_arrow.width)) == -1)
   return -1;
  if (read_float(fd, &(object->fore_arrow.height)) == -1)
   return -1;
 }
 if (object->backward_arrow)
 {
  if (read_int(fd, &(object->back_arrow.type)) == -1)
   return -1;
  if (read_int(fd, &(object->back_arrow.style)) == -1)
   return -1;
  if (read_float(fd, &(object->back_arrow.thickness)) == -1)
   return -1;
  if (read_float(fd, &(object->back_arrow.width)) == -1)
   return -1;
  if (read_float(fd, &(object->back_arrow.height)) == -1)
   return -1;
 }
 if (ins_object(this, object) == -1)
  return -1;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** realize_arc                                                       ** 
** ----------------------------------------------------------------- */
static int realize_arc(this, object)
c_XFIGDocFile	*this;
fig_arc		*object;
{
 c_Color	*ocolor;
 c_Color	*fcolor;
 int		mode;
 float		angle1, angle2, dx, dy, radius, x, y;
 float		cx, cy, sx, sy, ex, ey;

 ocolor = find_color(this, object->pen_color, -1);
 if((ocolor != NULL) && 
    !F(this->vector).setStrokeColor(this->vector, ocolor))
  return -1;

 if (object->area_fill)
 {
  fcolor = find_color(this, object->fill_color, object->area_fill);
  if((fcolor != NULL) && 
     !F(this->vector).setFillColor(this->vector, fcolor))
   return -1;
 }

 cx = object->center_x;
 cy = object->center_y;
 sx = object->x1;
 sy = object->y1;
 ex = object->x3;
 ey = object->y3;

 if (this->neg_y)
  object->direction = !object->direction;

 if (object->thickness)
 {
  if(!F(this->vector).
     setLineWidth(this->vector, 
		  SCALE_FROM_INCHES(object->thickness)/this->factor80))
  return -1;
 }
 dx = cx - sx;
 dy = cy - sy;
 radius = sqrt(dx*dx+dy*dy);
 angle1 = atan2(this->neg_y*(sy-cy), sx-cx) * 180 / M_PI;
 angle2 = atan2(this->neg_y*(ey-cy), ex-cx) * 180 / M_PI;
 /* direction = 1 -> Counterclockwise */
 if (!object->direction)
 {
  if (angle2 < angle1)
   angle2 += 360;
 }
 else
  if (angle1 < angle2)
   angle1 += 360;

 if(!F(this->vector).move(this->vector,
			  SCALE_FROM_INCHES(sx)/this->factor,
			  this->neg_y*SCALE_FROM_INCHES(sy)/this->factor))
  return -1;
  
			  
 if(!F(this->vector).arc(this->vector,
			 SCALE_FROM_INCHES(cx)/this->factor,
			 this->neg_y*SCALE_FROM_INCHES(cy)/this->factor,
			 SCALE_FROM_INCHES(radius)/this->factor,
			 SCALE_FROM_INCHES(radius)/this->factor,
			 SCALE_FROM_DEGREES(angle1),
			 SCALE_FROM_DEGREES(angle2)))
  return -1;

 if (object->thickness)
  mode = XcVG_STROKE;
 else
  mode = 0;
 if (object->area_fill)
  mode |= XcVG_FILL;  
 if (!F(this->vector).render(this->vector, mode))
  return -1;

 if (object->backward_arrow && object->back_arrow.thickness)
 {
  if(!F(this->vector).
     setLineWidth(this->vector, 
		  SCALE_FROM_INCHES(object->back_arrow.thickness)
		  /this->factor80))
    return -1;
  arc_tangent(cx, cy, sx, sy, !object->direction, &x, &y);
  draw_arrow_head(this, x, y, sx, sy, 
		  object->back_arrow.height, object->back_arrow.width);
  if (!F(this->vector).render(this->vector, XcVG_STROKE))
   return -1;
 }
 if (object->forward_arrow && object->fore_arrow.thickness)
 {
  if(!F(this->vector).
     setLineWidth(this->vector, 
		  SCALE_FROM_INCHES(object->fore_arrow.thickness)
		  /this->factor80))
    return -1;
  arc_tangent(cx, cy, ex, ey, object->direction, &x, &y);
  draw_arrow_head(this, x, y, ex, ey, 
		  object->fore_arrow.height, object->fore_arrow.width);
  if (!F(this->vector).render(this->vector, XcVG_STROKE))
   return -1;
 }
 return 1;
}

/* ----------------------------------------------------------------- ** 
** draw arrow head                                                   ** 
** ----------------------------------------------------------------- */
static int draw_arrow_head(this, x1, y1, x2, y2, arrowht, arrowwid)
c_XFIGDocFile	*this;
float	x1, y1, x2, y2, arrowht, arrowwid;
{
 double	x, y, xb, yb, dx, dy, l, sina, cosa;
 double	xc, yc, xd, yd;

 dx = x2 - x1;  dy = y1 - y2;
 l = sqrt(dx*dx+dy*dy);
 if (l == 0) 
 {
  return 1;
 }
 else 
 {
  sina = dy / l;  cosa = dx / l;
 }
 xb = x2*cosa - y2*sina;
 yb = x2*sina + y2*cosa;
 x = xb - arrowht;
 y = yb - arrowwid / 2;
 xc = x*cosa + y*sina;
 yc = -x*sina + y*cosa;
 y = yb + arrowwid / 2;
 xd = x*cosa + y*sina;
 yd = -x*sina + y*cosa;

 if(!F(this->vector).move(this->vector,
			  SCALE_FROM_INCHES(xc)/this->factor,
			  SCALE_FROM_INCHES(this->neg_y*yc)/this->factor))
  return -1;
 if(!F(this->vector).line(this->vector,
			  SCALE_FROM_INCHES(x2)/this->factor,
			  SCALE_FROM_INCHES(this->neg_y*y2)/this->factor))
  return -1;
 if(!F(this->vector).line(this->vector,
			  SCALE_FROM_INCHES(xd)/this->factor,
			  SCALE_FROM_INCHES(this->neg_y*yd)/this->factor))
  return -1;
 return 1;
}



/* ----------------------------------------------------------------- ** 
** control spline                                                    ** 
** ----------------------------------------------------------------- */
static int ctl_spline(this, object)
c_XFIGDocFile	*this;
fig_spline	*object;
{
 float	a, b, c, d, x1, y1, x2, y2, x3, y3;
 int	mode;
 t_spoint	*points;
 int	num;

 num = object->npoints;
 points = object->points;
 x2 = 0;
 y2 = 0;
 x1 = points->x;
 y1 = points->y;
 points = points->next;
 num--;

 c = points->x;
 d = points->y;
 points = points->next;
 num--;

 x3 = a = (x1 + c) / 2;
 y3 = b = (y1 + d) / 2;
 if (object->backward_arrow && object->back_arrow.thickness)
 {
  if(!F(this->vector).
     setLineWidth(this->vector, 
		  SCALE_FROM_INCHES(object->back_arrow.thickness)
		  /this->factor80))
   return -1;
  draw_arrow_head(this, c, d, x1, y1, object->back_arrow.height,
		  object->back_arrow.width);
  if (!F(this->vector).render(this->vector, XcVG_STROKE))
   return -1;
 }
 if(!F(this->vector).
    setLineWidth(this->vector, SCALE_FROM_INCHES(object->thickness)
		 /this->factor80))
  return -1;
 if (!(object->sub_type & 1))
 {
  if(!F(this->vector).move(this->vector,
			   SCALE_FROM_INCHES(x1)/this->factor,
			   SCALE_FROM_INCHES(this->neg_y * y1)/this->factor))
   return -1;
  if(!F(this->vector).line(this->vector,
			   SCALE_FROM_INCHES(x3)/this->factor,
			   SCALE_FROM_INCHES(this->neg_y * y3)/this->factor))
   return -1;
 }
 else 
 {
  if(!F(this->vector).move(this->vector,
			   SCALE_FROM_INCHES(a)/this->factor,
			   SCALE_FROM_INCHES(this->neg_y * b)/this->factor))
   return -1;
 }

 while (num)
 {
  x1 = x3; y1 = y3;
  x2 = c;  y2 = d;

  c = points->x;
  d = points->y;
  points = points->next;
  num--;

  x3 = (x2 + c) / 2;
  y3 = (y2 + d) / 2;

  if(!F(this->vector).line(this->vector,
			   SCALE_FROM_INCHES(x1)/this->factor,
			   SCALE_FROM_INCHES(this->neg_y * y1)/this->factor))
   return -1;

  if(!F(this->vector).curve
     (this->vector,
      SCALE_FROM_INCHES(x1 + (x2 - x1) * FACT)/this->factor,
      this->neg_y*SCALE_FROM_INCHES(y1 + (y2 - y1) * FACT)/this->factor,
      SCALE_FROM_INCHES(x3 + (x2 - x3) * FACT)/this->factor,
      this->neg_y*SCALE_FROM_INCHES(y3 + (y2 - y3) * FACT)/this->factor,
      SCALE_FROM_INCHES(x3)/this->factor,
      this->neg_y*SCALE_FROM_INCHES(y3)/this->factor))
   return -1;
 }
 /*
 * At this point, (x2,y2) and (c,d) are the position of the 
 * next-to-last and last point respectively, in the point list
 */
 if (object->sub_type & 1)
 {
  if(!F(this->vector).curve
     (this->vector,
      SCALE_FROM_INCHES(x3 + (c - x3) * FACT)/this->factor,
      this->neg_y*SCALE_FROM_INCHES(y3 + (d - y3) * FACT)/this->factor,
      SCALE_FROM_INCHES(a + (c - a) * FACT)/this->factor,
      this->neg_y*SCALE_FROM_INCHES(b + (d - b) * FACT)/this->factor,
      SCALE_FROM_INCHES(a)/this->factor,
      this->neg_y*SCALE_FROM_INCHES(b)/this->factor))
   return -1;
  if(!F(this->vector).closePath(this->vector))
   return -1;
 }
 else 
 {
  if(!F(this->vector).line(this->vector,
			   SCALE_FROM_INCHES(c)/this->factor,
			   SCALE_FROM_INCHES(this->neg_y * d)/this->factor))
   return -1;
 }

 if (object->thickness)
  mode = XcVG_STROKE;
 else
  mode = 0;
 if (object->area_fill)
  mode |= XcVG_FILL;  
 if (!F(this->vector).render(this->vector, mode))
  return -1;

 if (object->forward_arrow && object->fore_arrow.thickness)
 {
  if(!F(this->vector).
     setLineWidth(this->vector, 
		  SCALE_FROM_INCHES(object->fore_arrow.thickness)
		  /this->factor80))
   return -1;
  draw_arrow_head(this, x2, y2, c, d, 
		  object->fore_arrow.height,
		  object->fore_arrow.width);
  if (!F(this->vector).render(this->vector, XcVG_STROKE))
   return -1;
 }
 return 1;
}

/* ----------------------------------------------------------------- ** 
** interpolated spline                                               ** 
** ----------------------------------------------------------------- */
static int itp_spline(this, object)
c_XFIGDocFile	*this;
fig_spline	*object;
{
 t_spoint	*p, *q;
 t_scontrol	*a, *b;
 int		mode;

 /* first load all points */

 a = object->controls;
 p = object->points;
 if (object->backward_arrow && object->back_arrow.thickness)
 {
  if(!F(this->vector).
     setLineWidth(this->vector, 
		  SCALE_FROM_INCHES(object->back_arrow.thickness)
		  /this->factor80))
   return -1;
  draw_arrow_head(this, a->rx, a->ry, 
		  (float)p->x, (float)p->y,
		  object->back_arrow.height, object->back_arrow.width);
  if (!F(this->vector).render(this->vector, XcVG_STROKE))
   return -1;
 }
 if(!F(this->vector).
    setLineWidth(this->vector, SCALE_FROM_INCHES(object->thickness)/
		 this->factor80))
  return -1;
 if(!F(this->vector).move(this->vector,
			  SCALE_FROM_INCHES(p->x)/this->factor,
			  SCALE_FROM_INCHES(this->neg_y * p->y)/this->factor))
  return -1;
 for (q = p->next; q != NULL; p = q, q = q->next) 
 {
  b = a->next;
  if(!F(this->vector).curve
     (this->vector,
      SCALE_FROM_INCHES(a->rx)/this->factor,
      this->neg_y*SCALE_FROM_INCHES(a->ry)/this->factor,
      SCALE_FROM_INCHES(b->lx)/this->factor,
      this->neg_y*SCALE_FROM_INCHES(b->ly)/this->factor,
      SCALE_FROM_INCHES(q->x)/this->factor,
      this->neg_y*SCALE_FROM_INCHES(q->y)/this->factor))
   return -1;
  a = b;
 }
 if (object->sub_type & 1)
  if(!F(this->vector).closePath(this->vector))
   return -1;

 if (object->thickness)
  mode = XcVG_STROKE;
 else
  mode = 0;
 if (object->area_fill)
  mode |= XcVG_FILL;  
 if (!F(this->vector).render(this->vector, mode))
  return -1;
 
 if (object->forward_arrow && object->fore_arrow.thickness)
 {
  if(!F(this->vector).
     setLineWidth(this->vector, 
		  SCALE_FROM_INCHES(object->fore_arrow.thickness)
		  /this->factor80))
   return -1;
  draw_arrow_head(this, a->lx, a->ly, 
		  (float)p->x, (float)p->y,
		  object->fore_arrow.height, object->fore_arrow.width);
  if (!F(this->vector).render(this->vector, XcVG_STROKE))
   return -1;
 }
 return 1;
}

/* ----------------------------------------------------------------- ** 
** arc tangente                                                      ** 
** ----------------------------------------------------------------- */
static void arc_tangent(x1, y1, x2, y2, direction, x, y)
float	x1, y1, x2, y2, *x, *y;
int	direction;
{
 if (direction) 
 { /* counter clockwise  */
  *x = x2 + (y2 - y1);
  *y = y2 - (x2 - x1);
 }
 else 
 {
  *x = x2 - (y2 - y1);
  *y = y2 + (x2 - x1);
 }
}

/* ----------------------------------------------------------------- ** 
** get compound - decode compound function                           ** 
** ----------------------------------------------------------------- */
static int get_compound(this, fd)
c_XFIGDocFile	*this;
FILE		*fd;
{
 int	r;
 int	t;
 int	l;
 int	b;

 if (read_int(fd, &r) == -1)
  return -1;
 if (read_int(fd, &t) == -1)
  return -1;
 if (read_int(fd, &l) == -1)
  return -1;
 if (read_int(fd, &b) == -1)
  return -1;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** ins_object                                                        ** 
** ----------------------------------------------------------------- */
static int ins_object(this, object)
c_XFIGDocFile	*this;
fig_obj		*object;
{
 obj_list	*tlist;
 obj_list	*llist;

 if (!this->last_obj || this->last_obj->object->arc.depth >= object->arc.depth)
 {
  *(this->pins_obj) = Xc_malloc("object", sizeof(obj_list));
  if (!*(this->pins_obj))
   return -1;
  (*(this->pins_obj))->object = object;
  (*(this->pins_obj))->Prev = this->last_obj;
  (*(this->pins_obj))->Next = NULL;
  this->last_obj = *(this->pins_obj);
  this->pins_obj = &((*(this->pins_obj))->Next);
 }
 else
 {
  this->pins_obj = &(this->list);
  llist = NULL;
  while (*(this->pins_obj) && 
	 (*(this->pins_obj))->object->arc.depth >= object->arc.depth)
  {
   llist = *(this->pins_obj);
   this->pins_obj = &((*(this->pins_obj))->Next);
  }
  if (*(this->pins_obj))
  {
   tlist = *(this->pins_obj);
   *(this->pins_obj) = Xc_malloc("object", sizeof(obj_list));
   if (!*(this->pins_obj))
    return -1;
   (*(this->pins_obj))->object = object;
   (*(this->pins_obj))->Prev = llist;
   (*(this->pins_obj))->Next = tlist;
   if (tlist)
    tlist->Prev = *(this->pins_obj);
  }
  while (*(this->pins_obj))
  {
   this->pins_obj = &((*(this->pins_obj))->Next);
  }
 }
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get color                                                         ** 
** ----------------------------------------------------------------- */
static int get_color(this, fd)
c_XFIGDocFile	*this;
FILE		*fd;
{
 char	col_name[50];
 int	num;
 int	r;
 int	g;
 int	b;
 int	i;

 if (read_int(fd, &num) == -1)
  return -1;
 fgets(col_name, 50, fd);
 i = 0;
 while (col_name[i] && col_name[i]!='#')
  i++;
 if (!col_name[i])
  return -1;
 i++;
 r = (col_name[i] > 'a' ? col_name[i]-'a'+10 : col_name[i]-'0') * 16 +
  (col_name[i+1] > 'a' ? col_name[i+1]-'a'+10 : col_name[i+1]-'0');
 i+=2;
 g = (col_name[i] > 'a' ? col_name[i]-'a'+10 : col_name[i]-'0') * 16 +
  (col_name[i+1] > 'a' ? col_name[i+1]-'a'+10 : col_name[i+1]-'0');
 i+=2;
 b = (col_name[i] > 'a' ? col_name[i]-'a'+10 : col_name[i]-'0') * 16 +
  (col_name[i+1] > 'a' ? col_name[i+1]-'a'+10 : col_name[i+1]-'0');

 if (this->num_col)
 {
  this->user_col = Xc_realloc("color", this->user_col, 
			      (this->num_col + 1)*sizeof(fig_color));
 }
 else
  this->user_col = Xc_malloc("color", sizeof(fig_color));
 this->user_col[this->num_col].num = num;
 this->user_col[this->num_col].r = r;
 this->user_col[this->num_col].g = g;
 this->user_col[this->num_col].b = b;
 this->num_col++;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** find color                                                        ** 
** ----------------------------------------------------------------- */
static c_Color *find_color(this, color, area)
c_XFIGDocFile	*this;
int		color;
int		area;
{
 int	i;

 if (color < 32)
 {
  color ++;
  if (area < 0)
  {
   return F(*(this->color_base)).getColor
    (this->color_base, XcC_CMYK,
     XcC_SCALE_COLOR(color_table[color].c),
     XcC_SCALE_COLOR(color_table[color].m),
     XcC_SCALE_COLOR(color_table[color].y),
     XcC_SCALE_COLOR(color_table[color].k),
     XcC_END);
  }
  else
  {
   return F(*(this->color_base)).getColor
    (this->color_base, XcC_CMYK,
     XcC_SCALE_COLOR(color_table[color].c),
     XcC_SCALE_COLOR(color_table[color].m),
     XcC_SCALE_COLOR(color_table[color].y),
     XcC_SCALE_COLOR((color > 1) ? 
		     (float)(21-area)/21 : 
		     (float)(area-1)/21),
     XcC_END);
  }
 }
 i=0;
 while ((i<this->num_col) && (this->user_col[i].num!=color))
  i++;
 if (i < this->num_col)
 {
  return F(*(this->color_base)).getColor
   (this->color_base, XcC_RGB,
    XcC_SCALE_COLOR((float)this->user_col[i].r/255),
    XcC_SCALE_COLOR((float)this->user_col[i].g/255),
    XcC_SCALE_COLOR((float)this->user_col[i].b/255),
    XcC_END);
 }
 return F(*(this->color_base)).getColor
  (this->color_base, XcC_CMYK,
   XcC_SCALE_COLOR(0),
   XcC_SCALE_COLOR(0),
   XcC_SCALE_COLOR(0),
   XcC_SCALE_COLOR(0),
   XcC_END); 
}

/* ----------------------------------------------------------------- ** 
** destroy objects                                                   ** 
** ----------------------------------------------------------------- */
static void destroy_objects(this)
c_XFIGDocFile *this;
{
 obj_list	*list;
 obj_list	*tlist;

 if (this->num_col)
 {
  Xc_free(this->user_col);
 }
 list = this->list;
 while (list)
 {
  switch(list->object->type)
  {
  case 1:
   break;
  case 2:
   {
    t_spoint	*points;
    t_spoint	*dpoints;
    if (list->object->polyline.pic_name)
     Xc_free(list->object->polyline.pic_name);
    points = list->object->polyline.points;
    while (points)
    {
     dpoints = points;
     points = points->next;
     Xc_free(dpoints);
    }
   }
   break;
  case 3:
   {
    t_spoint	*points;
    t_spoint	*dpoints;
    t_scontrol	*controls;
    t_scontrol	*dcontrols;
    points = list->object->spline.points;
    while (points)
    {
     dpoints = points;
     points = points->next;
     Xc_free(dpoints);
    }
    controls = list->object->spline.controls;
    while (controls)
    {
     dcontrols = controls;
     controls = controls->next;
     Xc_free(dcontrols);
    }
   }
   break;
  case 4:
   if (list->object->text.string)
    Xc_free(list->object->text.string);
   break;
  case 5:
   break;   
  }
  tlist = list->Next;
  Xc_free(list->object);
  Xc_free(list);
  list = tlist;
 }
}
