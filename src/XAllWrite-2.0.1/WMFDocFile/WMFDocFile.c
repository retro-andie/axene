/*
** WMFDocFile.c for XAllWrite in WMFDocFile/
** Methods for thet WMFDocFile class
**
** Copyright (C) 1997-2000 Axene.
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
** Last update Mon Aug 25 14:12:46 1997 Antoine Buat
*/

#if 0
#define TRACE
#define NMEMDEBUG
#undef EFINITIVE_VERSION
#endif

/*--- include ---*/
#include "WMFDocFileP.h"
#include "file.h"

/*--- external ---*/
extern c_VectorFont *GlobFontBase;

/*--- Prototypes ---*/
static void *WMF_constructor();
static void WMF_destructor();
static void *WMF_copy();

static int read_char();
static int read_word();
static int read_short();
static int read_dword();

static boolean WMF_readMain();

static int read_header();
static int read_metaheader();
static int read_metarecord();
#ifndef NTRACE
static char *get_metaname();
#endif
static int get_polygon();
static int get_polyline();
static int get_settextcolor();
static int get_createbrushindirect();
static int get_createpenindirect();
static int get_selectobject();
static int get_deleteobject();
static int get_obj_pos();
static int get_rectangle();
static int get_ellipse();
static int get_pie();
static int get_arc();
static int get_lineto();
static int get_moveto();
static int get_savedc();
static int get_restoredc();
static int get_intersectcliprect();
static int get_createfontindirect();
static int get_exttextout();
static c_VectorFont *wmfFindFont();
static int get_roundrect();
static int get_polyfillmode();
static int get_polypolygon();

/*--- fill methods ---*/
sf_WMFDocFile fc_WMFDocFile =
{
 WMF_constructor,
 WMF_destructor,
 WMF_copy,
 WMF_readMain
};

/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *WMF_constructor(filename, vector)
char		*filename;
c_VectorGraph	*vector;
{
 c_WMFDocFile	*p;

 Xc_HISTORY(("constructor(`%s')", filename));
 if((p = Xc_malloc("WMFDocFile class", sizeof(c_WMFDocFile))) == NULL)
  return NULL;
 p->f = &fc_WMFDocFile;

 p->filename = Xc_strdup("filename", filename? filename: "-stream-");
 if(p->filename == NULL)
 {
  Xc_free(p);
  return NULL;
 }

 p->fd = fopen(filename, "rb");
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
 p->dc_stack = NULL;
 p->obj_list = NULL;
 return p;
}

/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void WMF_destructor(this)
c_WMFDocFile *this;
{
 MDCStack	*pdc;
 MClipZone	*pclip;

 Xc_HISTORY(("destructor(`%s')", GetBasename(this->filename, FALSE)));

 /*--- free dc_stack ---*/
 while (this->dc_stack)
 {
  pdc = this->dc_stack;
  this->dc_stack = pdc->Next;
  while(pdc->clip)
  {
   pclip = pdc->clip;
   pdc->clip = pclip->Next;
   Xc_free(pclip);
  }
  Xc_free(pdc);
 }

 /*--- free obj list ---*/
 if (this->obj_list)
  Xc_free(this->obj_list);

 if(this->close_flag) 
  fclose(this->fd);

 Xc_free(this);
}

/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *WMF_copy(this)
c_WMFDocFile *this;
{
 Xc_HISTORY(("copy not implemented"));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** readMain - Main Read Part                                         ** 
** ----------------------------------------------------------------- */
static boolean WMF_readMain(this)
c_WMFDocFile *this;
{
 int	ret;

 fseek(this->fd, 0, SEEK_SET);
 this->dc_stack = Xc_malloc("dc", sizeof(MDCStack));
 this->dc_stack->clip = NULL;
 this->dc_stack->Next = NULL;

 if (read_header(this, this->fd) == -1)
  return FALSE;
 if (read_metaheader(this, this->fd) == -1)
  return FALSE;
 ret = 1;
 while (ret != -2)
 {
  ret = read_metarecord(this, this->fd);
  if (ret == -1)
   return FALSE;
 }
 if(!F(this->vector).end(this->vector)) 
  return FALSE;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** read_char -- 8 bit signed num                                     ** 
** ----------------------------------------------------------------- */
int read_char(fd, num)
FILE	*fd;
char	*num;
{
  char	buffer[1];

  if (fread(buffer, 1, 1, fd) < 1)
    return -1;
  *num = buffer[0];
  return 0;
}

/* ----------------------------------------------------------------- ** 
** read_word -- 16 bit unsigned num                                  ** 
** ----------------------------------------------------------------- */
static int read_word(fd, num)
FILE	*fd;
int	*num;
{
 unsigned char	buffer[2];

 if (fread(buffer, 1, 2, fd) < 2)
  return -1;
 *num = buffer[0] + (buffer[1]<<8);
 return 0;
}

/* ----------------------------------------------------------------- ** 
** read_short -- 16 bit signed num                                   ** 
** ----------------------------------------------------------------- */
static int read_short(fd, num)
FILE	*fd;
int	*num;
{
 unsigned char	buffer[2];

 if (fread(buffer, 1, 2, fd) < 2)
  return -1;
 *num = buffer[0] + (buffer[1]<<8);
 if (*num > 0x8000)
  *num -= 0x10000;
 return 0;
}

/* ----------------------------------------------------------------- ** 
** read_dword -- 32 bits signed word                                 ** 
** ----------------------------------------------------------------- */
static int read_dword(fd, num)
FILE		*fd;
long int	*num;
{
 unsigned char	buffer[4];

 if (fread(buffer, 1, 4, fd) < 4)
  return -1;
 *num = buffer[0] + (buffer[1]<<8) +
  (buffer[2]<<16) + (buffer[3]<<24);
 return 0;
}

/* ----------------------------------------------------------------- ** 
** read_header -- decode header to see if really a WMF               ** 
** ----------------------------------------------------------------- */
static int read_header(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 long int	magik;
 int		handle;
 int		left;
 int		top;
 int		right;
 int		bottom;
 int		inch;
 long		reserved;
 int		checksum;
 int		my_checksum;

 read_dword(fd, &magik);
 if (magik != WMFMAGIK)
 {
  Xc_TRACE(("not a wmf file %lx != %lx", magik, WMFMAGIK));
  return -1;
 }
 if (read_word(fd, &handle) == -1)
  return -1;
 if (read_short(fd, &left) == -1)
  return -1;
 if (read_short(fd, &top) == -1)
  return -1;
 if (read_short(fd, &right) == -1)
  return -1;
 if (read_short(fd, &bottom) == -1)
  return -1;
 if (read_word(fd, &inch) == -1)
  return -1;
 if (read_dword(fd, &reserved) == -1)
  return -1;
 my_checksum = 0;
 my_checksum ^= magik & 0xffff;
 my_checksum ^= (magik>>16) & 0xffff;
 my_checksum ^= handle & 0xffff;
 my_checksum ^= left & 0xffff;
 my_checksum ^= top & 0xffff;
 my_checksum ^= right & 0xffff;
 my_checksum ^= bottom & 0xffff;
 my_checksum ^= inch & 0xffff;
 my_checksum ^= reserved & 0xffff;
 my_checksum ^= (reserved>>16) & 0xffff;
 if (read_word(fd, &checksum) == -1)
  return -1;
 if (checksum != my_checksum)
  Xc_WARNING(("bad checksum: %x != %x", checksum, my_checksum));
 else
  Xc_TRACE(("checksum ok: %x", checksum));

 this->factor = inch;
 this->vector->bbox.llx = SCALE_FROM_INCHES(left) / this->factor;
 this->vector->bbox.lly = SCALE_FROM_INCHES(-top) / this->factor;
 this->vector->bbox.urx = SCALE_FROM_INCHES(right - left) / this->factor;
 this->vector->bbox.ury = SCALE_FROM_INCHES(-bottom + top) / this->factor;

 return 1;
}

/* ----------------------------------------------------------------- ** 
** read_metaheader -- decode metaheader                              ** 
** ----------------------------------------------------------------- */
static int read_metaheader(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int	type;
 int	hsize;
 int	version;
 long	filesize;
 int	numobj;
 long	maxrecsize;
 int	noparam;

 if (read_word(fd, &type) == -1)
  return -1;
 if (read_word(fd, &hsize) == -1)
  return -1;
 if (read_word(fd, &version) == -1)
  return -1;
 if (read_dword(fd, &filesize) == -1)
  return -1;
 if (read_word(fd, &numobj) == -1)
  return -1;
 this->num_obj = numobj;
 this->obj_list = Xc_malloc("MetaObj", sizeof(MObject) * numobj);
 if (!this->obj_list)
  return -1;
 bzero(this->obj_list, sizeof(MObject) * numobj);
 if (read_dword(fd, &maxrecsize) == -1)
  return -1;
 if (read_word(fd, &noparam) == -1)
  return -1;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** read_metarecord -- decode metarecord                              ** 
** ----------------------------------------------------------------- */
static int read_metarecord(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 long	size;
 int	func;
 int	pos;

 if (read_dword(fd, &size) == -1)
  return -1;
 if (read_word(fd, &func) == -1)
  return -1;
 switch (func)
 {
 case 0:
  return -2;

 case 0x1e:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_savedc(this, fd);
  break;
  
 case 0x0127:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_restoredc(this, fd);
  break;

 case 0xf7:
 case 0x142:
 case 0x1f9:
 case 0x6ff:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  pos = get_obj_pos(this);
  this->obj_list[pos].type = M_OBJ_OTHER;
  if (size*2 > 6)
   fseek(fd, size*2-6, SEEK_CUR);
  break;

 case 0x106:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_polyfillmode(this, fd);
  break;
 case 0x12d:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_selectobject(this, fd);
  break;      
 case 0x1f0:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_deleteobject(this, fd);
  break;      
 case 0x209:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_settextcolor(this, fd);
  break;
 case 0x213:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_lineto(this, fd);
  break;
 case 0x214:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_moveto(this, fd);
  break;
 case 0x2fa:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_createpenindirect(this, fd);
  if (size*2 > 16)
   fseek(fd, size*2-16, SEEK_CUR);
  break;

 case 0x2fb:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_createfontindirect(this, fd, size*2);
  break;

 case 0x2fc:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_createbrushindirect(this, fd);
  break;
 case 0x324:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_polygon(this, fd);
  break;
 case 0x325:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_polyline(this, fd);
  break;
 case 0x0416:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_intersectcliprect(this, fd);
  break;
 case 0x0418:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_ellipse(this, fd);
  break;
 case 0x041b:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_rectangle(this, fd);
  break;
 case 0x0538:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_polypolygon(this, fd);
  break;
 case 0x061c:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_roundrect(this, fd);
  break;
 case 0x0817:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_arc(this, fd);
  break;
 case 0x081a:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_pie(this, fd);
  break;
 case 0x0a32:
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
  get_exttextout(this, fd, size*2);
  break;
 default:
  if (size*2 > 6)
   fseek(fd, size*2-6, SEEK_CUR);
  Xc_TRACE(("size: %4ld function: 0x%x %s", size*2, func, 
	    get_metaname(func)));
 }
 return 1;
}

#ifndef NTRACE
/* ----------------------------------------------------------------- ** 
** get_metaname -- gdi func name for debugging                       ** 
** ----------------------------------------------------------------- */
static char *get_metaname(id)
int	id;
{
 int	i;

 i=0;
 while (mname[i].number!=0)
 {
  if (mname[i].number==id)
   return mname[i].name;
  i++;
 }
 return mname[i].name;
}
#endif

/* ----------------------------------------------------------------- ** 
** get_polygon -- decode polygon gdi func                            ** 
** ----------------------------------------------------------------- */
static int get_polygon(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int	x;
 int	y;
 int	num;
 int	mode;

 if (read_short(fd, &num) == -1)
  return -1;
 Xc_TRACE(("num:%d", num));

 if (read_short(fd, &x) == -1)
  return -1;
 if (read_short(fd, &y) == -1)
  return -1;
 if(!F(this->vector).move(this->vector,
			  SCALE_FROM_INCHES(x)/this->factor,
			  SCALE_FROM_INCHES(-y)/this->factor))
  return -1;

 num--;
 while (num)
 {
  if (read_short(fd, &x) == -1)
   return -1;
  if (read_short(fd, &y) == -1)
   return -1;
  if(!F(this->vector).line(this->vector,
			   SCALE_FROM_INCHES(x)/this->factor,
			   SCALE_FROM_INCHES(-y)/this->factor))
   return -1;
  Xc_TRACE(("x:%d y:%d", x, y));
  num --;
 }
 if(!F(this->vector).closePath(this->vector))
  return -1;
 mode = 0;
 if (this->pen_style != PS_NULL)
  mode |= XcVG_STROKE;
 if (this->brush_style != BS_NULL)
  mode |= XcVG_FILL;
 if (!F(this->vector).render(this->vector, mode))
  return -1;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_polyline -- decode polyline gdi func                          ** 
** ----------------------------------------------------------------- */
static int get_polyline(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int	x;
 int	y;
 int	num;

 if (read_short(fd, &num) == -1)
  return -1;
 Xc_TRACE(("num:%d", num));

 if (read_short(fd, &x) == -1)
  return -1;
 if (read_short(fd, &y) == -1)
  return -1;
 if(!F(this->vector).move(this->vector,
			  SCALE_FROM_INCHES(x)/this->factor,
			  SCALE_FROM_INCHES(-y)/this->factor))
  return -1;

 num--;
 while (num)
 {
  if (read_short(fd, &x) == -1)
   return -1;
  if (read_short(fd, &y) == -1)
   return -1;
  if(!F(this->vector).line(this->vector,
			   SCALE_FROM_INCHES(x)/this->factor,
			   SCALE_FROM_INCHES(-y)/this->factor))
   return -1;
  Xc_TRACE(("x:%d y:%d", x, y));
  num --;
 }
 /* if(!F(this->vector).closePath(this->vector))
    return -1;
    */
 if (!F(this->vector).render(this->vector, XcVG_STROKE))
  return -1;
 return 1;
}


/* ----------------------------------------------------------------- ** 
** get_settextcolor -- get text color                                ** 
** ----------------------------------------------------------------- */
static int get_settextcolor(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 long	num;

 if (read_dword(fd, &num) == -1)
  return -1;
 Xc_TRACE(("color C%d M%d Y%d K%d", (int)(num&0xff), (int)((num>>8)&0xff),
	   (int)((num>>16)&0xff), (int)((num>>24)&0xff)));
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_createbrushindirect -- decode brush creation                  ** 
** ----------------------------------------------------------------- */
static int get_createbrushindirect(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 unsigned int	style;
 long		num;
 unsigned int	hatch;
 int		pos;

 if (read_word(fd, &style) == -1)
  return -1;
 if (read_dword(fd, &num) == -1)
  return -1;
 if (read_word(fd, &hatch) == -1)
  return -1;
 Xc_TRACE(("brush style:%s - color C%d M%d Y%d K%d - hatch:%d", 
	   brush_style[style],
	   (int)(num&0xff), (int)((num>>8)&0xff),
	   (int)((num>>16)&0xff), (int)((num>>24)&0xff),
	   hatch));

 pos = get_obj_pos(this);
 this->obj_list[pos].metabrush.type = M_OBJ_BRUSH;
 this->obj_list[pos].metabrush.style = style;
 this->obj_list[pos].metabrush.color = num;
 this->obj_list[pos].metabrush.hatch = hatch;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_createpenindirect -- decode pen creation                      ** 
** ----------------------------------------------------------------- */
static int get_createpenindirect(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 unsigned int	style;
 long		num;
 int		Px;
 int		ignored;
 int		pos;

 if (read_word(fd, &style) == -1)
  return -1;
 if (read_word(fd, &Px) == -1)
  return -1;
 if (read_word(fd, &ignored) == -1)/* Py for Point */
  return -1;
 if (read_dword(fd, &num) == -1)
  return -1;
 Xc_TRACE(("pen style:%s - Width:%d - color C%d M%d Y%d K%d", 
	   pen_style[style],
	   Px,
	   (int)(num&0xff), (int)((num>>8)&0xff),
	   (int)((num>>16)&0xff), (int)((num>>24)&0xff)));
 pos = get_obj_pos(this);
 this->obj_list[pos].metapen.type = M_OBJ_PEN;
 this->obj_list[pos].metapen.style = style;
 this->obj_list[pos].metapen.color = num;
 this->obj_list[pos].metapen.width = Px;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get select object by number                                       ** 
** ----------------------------------------------------------------- */
static int get_selectobject(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int		num;
 long		col;
 c_Color	*color;
 
 if (read_word(fd, &num) == -1)
  return -1;
 Xc_TRACE(("select object: %d\n", num));
 if (num < this->num_obj)
  switch (this->obj_list[num].type)
  {
  case M_OBJ_PEN:
   this->pen_style = this->obj_list[num].metapen.style;
   col = this->obj_list[num].metapen.color;
   color = F(*(this->color_base)).getColor
    (this->color_base, XcC_CMYK,
     XcC_SCALE_COLOR((float)((255-(col&0xff)))/255),
     XcC_SCALE_COLOR((float)((255-((col>>8)&0xff)))/255),
     XcC_SCALE_COLOR((float)((255-((col>>16)&0xff)))/255),
     XcC_SCALE_COLOR((float)((col>>24)&0xff)/255), 
     XcC_END);
   if((color != NULL) && 
      !F(this->vector).setStrokeColor(this->vector, color))
    return -1;
   if(!F(this->vector).
      setLineWidth(this->vector, 
		   SCALE_FROM_INCHES(this->obj_list[num].metapen.width)/
		   this->factor))
    return -1;
   break;
  case M_OBJ_BRUSH:
   this->brush_style = this->obj_list[num].metabrush.style;
   col = this->obj_list[num].metabrush.color;
   color = F(*(this->color_base)).getColor
    (this->color_base, XcC_CMYK,
     XcC_SCALE_COLOR((float)((255-(col&0xff)))/255),
     XcC_SCALE_COLOR((float)((255-((col>>8)&0xff)))/255),
     XcC_SCALE_COLOR((float)((255-((col>>16)&0xff)))/255),
     XcC_SCALE_COLOR((float)((col>>24)&0xff)/255), 
     XcC_END);
   if((color != NULL) && 
      !F(this->vector).setFillColor(this->vector, color))
    return -1;
   break;
  case M_OBJ_FONT:
   if(!F(this->vector).setTextFont
      (this->vector, 
       this->obj_list[num].metafont.font,
       SCALE_FROM_POINTS(this->obj_list[num].metafont.size/this->factor)))
    return -1;
   break;
  }
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get delete object by number                                       ** 
** ----------------------------------------------------------------- */
static int get_deleteobject(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int		num;

 if (read_word(fd, &num) == -1)
  return -1;
 Xc_TRACE(("delete object: %d\n", num));
 if (num < this->num_obj)
  this->obj_list[num].type = M_OBJ_VOID;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get first free object position                                    ** 
** ----------------------------------------------------------------- */
static int get_obj_pos(this)
c_WMFDocFile	*this;
{
 int	i;

 i=0;
 while (i < this->num_obj)
 {
  if (this->obj_list[i].type == M_OBJ_VOID)
   break;
  i++;
 }
 return i;
}

/* ----------------------------------------------------------------- ** 
** get_rectangle -- decode rectangle gdi func                        ** 
** ----------------------------------------------------------------- */
static int get_rectangle(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int	l;
 int	t;
 int	r;
 int	b;
 int	mode;

 if (read_short(fd, &b) == -1)
  return -1;
 if (read_short(fd, &r) == -1)
  return -1;
 if (read_short(fd, &t) == -1)
  return -1;
 if (read_short(fd, &l) == -1)
  return -1;
 if(!F(this->vector).move(this->vector,
			  SCALE_FROM_INCHES(l)/this->factor,
			  SCALE_FROM_INCHES(-t)/this->factor))
  return -1;
 if(!F(this->vector).line(this->vector,
			  SCALE_FROM_INCHES(r)/this->factor,
			  SCALE_FROM_INCHES(-t)/this->factor))
  return -1;
 if(!F(this->vector).line(this->vector,
			  SCALE_FROM_INCHES(r)/this->factor,
			  SCALE_FROM_INCHES(-b)/this->factor))
  return -1;
 if(!F(this->vector).line(this->vector,
			  SCALE_FROM_INCHES(l)/this->factor,
			  SCALE_FROM_INCHES(-b)/this->factor))
  return -1;
 if(!F(this->vector).closePath(this->vector))
  return -1;

 mode = 0;
 if (this->pen_style != PS_NULL)
  mode |= XcVG_STROKE;
 if (this->brush_style != BS_NULL)
  mode |= XcVG_FILL;
 if (!F(this->vector).render(this->vector, mode))
  return -1;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_ellipse -- decode ellipse gdi func                            ** 
** ----------------------------------------------------------------- */
static int get_ellipse(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int	l;
 int	t;
 int	r;
 int	b;
 int	mode;

 if (read_short(fd, &b) == -1)
  return -1;
 if (read_short(fd, &r) == -1)
  return -1;
 if (read_short(fd, &t) == -1)
  return -1;
 if (read_short(fd, &l) == -1)
  return -1;

 if (r < l)
 {
  mode = l;
  l = r;
  r = mode;
 }

 if (t < b)
 {
  mode = t;
  t = b;
  b = mode;
 }

 if(!F(this->vector).move(this->vector,
			  SCALE_FROM_INCHES(r)/this->factor,
			  SCALE_FROM_INCHES(-(t + (b-t)/2))/this->factor))
  return -1;
 if(!F(this->vector).arc(this->vector,
			 SCALE_FROM_INCHES(l + (r-l)/2)/this->factor,
			 SCALE_FROM_INCHES(-(t + (b-t)/2))/this->factor,
			 SCALE_FROM_INCHES((r-l)/2)/this->factor,
			 SCALE_FROM_INCHES((t-b)/2)/this->factor,
			 SCALE_FROM_DEGREES(0),
			 SCALE_FROM_DEGREES(360)))
  return -1;
 if(!F(this->vector).closePath(this->vector))
  return -1;

 mode = 0;
 if (this->pen_style != PS_NULL)
  mode |= XcVG_STROKE;
 if (this->brush_style != BS_NULL)
  mode |= XcVG_FILL;
 if (!F(this->vector).render(this->vector, mode))
  return -1;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_pie -- decode pie gdi func                                    ** 
** ----------------------------------------------------------------- */
static int get_pie(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int	l;
 int	t;
 int	r;
 int	b;
 int	x1;
 int	y1;
 int	x2;
 int	y2;
 int	mode;
 coord_t	Ang1;
 coord_t	Ang2;
 int	Cx;
 int	Cy;


 if (read_short(fd, &y2) == -1)
  return -1;
 if (read_short(fd, &x2) == -1)
  return -1;
 if (read_short(fd, &y1) == -1)
  return -1;
 if (read_short(fd, &x1) == -1)
  return -1;
 if (read_short(fd, &b) == -1)
  return -1;
 if (read_short(fd, &r) == -1)
  return -1;
 if (read_short(fd, &t) == -1)
  return -1;
 if (read_short(fd, &l) == -1)
  return -1;

 if (r < l)
 {
  mode = l;
  l = r;
  r = mode;
 }

 if (t < b)
 {
  mode = t;
  t = b;
  b = mode;
 }
 
 Cx = l + (r-l)/2;
 Cy = -(t + (b-t)/2);

 Ang1 = SCALE_FROM_RADIANS(atan2((double)(-y1 - Cy), (double)(x1 - Cx))) + 
  SCALE_FROM_DEGREES(0.0);
 Ang2 = SCALE_FROM_RADIANS(atan2((double)(-y2 - Cy), (double)(x2 - Cx))) +
  SCALE_FROM_DEGREES(0.0);
 
 if (Ang1 > Ang2)
  Ang2 += SCALE_FROM_DEGREES(360.0);

 if(!F(this->vector).move(this->vector,
			  SCALE_FROM_INCHES(Cx)/this->factor,
			  SCALE_FROM_INCHES(Cy)/this->factor))
  return -1;
 if(!F(this->vector).arc(this->vector,
			 SCALE_FROM_INCHES(Cx)/this->factor,
			 SCALE_FROM_INCHES(Cy)/this->factor,
			 SCALE_FROM_INCHES((r-l)/2)/this->factor,
			 SCALE_FROM_INCHES((t-b)/2)/this->factor,
			 Ang1, Ang2))
  return -1;
 if(!F(this->vector).closePath(this->vector))
  return -1;

 mode = 0;
 if (this->pen_style != PS_NULL)
  mode |= XcVG_STROKE;
 if (this->brush_style != BS_NULL)
  mode |= XcVG_FILL;
 if (!F(this->vector).render(this->vector, mode))
  return -1;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_arc -- decode arc gdi func                                    ** 
** ----------------------------------------------------------------- */
static int get_arc(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int	l;
 int	t;
 int	r;
 int	b;
 int	x1;
 int	y1;
 int	x2;
 int	y2;
 int	mode;
 coord_t	Ang1;
 coord_t	Ang2;
 int	Cx;
 int	Cy;
 int	Rx;
 int	Ry;

 if (read_short(fd, &y2) == -1)
  return -1;
 if (read_short(fd, &x2) == -1)
  return -1;
 if (read_short(fd, &y1) == -1)
  return -1;
 if (read_short(fd, &x1) == -1)
  return -1;
 if (read_short(fd, &b) == -1)
  return -1;
 if (read_short(fd, &r) == -1)
  return -1;
 if (read_short(fd, &t) == -1)
  return -1;
 if (read_short(fd, &l) == -1)
  return -1;

 if (r < l)
 {
  mode = l;
  l = r;
  r = mode;
 }

 if (t < b)
 {
  mode = t;
  t = b;
  b = mode;
 }
 
 Cx = l + (r-l)/2;
 Cy = -(t + (b-t)/2);

 Ang1 = SCALE_FROM_RADIANS(atan2((double)(-y1 - Cy), (double)(x1 - Cx))) + 
  SCALE_FROM_DEGREES(0.0);
 Ang2 = SCALE_FROM_RADIANS(atan2((double)(-y2 - Cy), (double)(x2 - Cx))) +
  SCALE_FROM_DEGREES(0.0);
 
 if (Ang1 > Ang2)
  Ang2 += SCALE_FROM_DEGREES(360.0);

 Rx = (r-l)/2;
 Ry = (t-b)/2;
 if(!F(this->vector).move(this->vector,
			  SCALE_FROM_INCHES(Cx + 
					    Rx*cos(SCALE_TO_RADIANS(Ang1)))/
			  this->factor,
			  SCALE_FROM_INCHES(Cy + 
					    Ry*sin(SCALE_TO_RADIANS(Ang1)))/
			  this->factor))
  return -1;
 if(!F(this->vector).arc(this->vector,
			 SCALE_FROM_INCHES(Cx)/this->factor,
			 SCALE_FROM_INCHES(Cy)/this->factor,
			 SCALE_FROM_INCHES((r-l)/2)/this->factor,
			 SCALE_FROM_INCHES((t-b)/2)/this->factor,
			 Ang1, Ang2))
  return -1;

 mode = 0;
 if (this->pen_style != PS_NULL)
  mode |= XcVG_STROKE;
 if (!F(this->vector).render(this->vector, mode))
  return -1;
 return 1;
}

#define TOLERANCE 1

int clip(this, px1, py1, px2, py2)
c_WMFDocFile	*this;
int		*px1;
int		*py1;
int		*px2;
int		*py2;
{
 int		type1;
 int		type2;
 int		x1;
 int		y1;
 int		x2;
 int		y2;
 float		penteY;
 float		addY;
 float		penteX;
 float		addX;
 MClipZone	*tclip;
 int		tx1;
 int		ty1;

 x1 = *px1;
 y1 = *py1;
 x2 = *px2;
 y2 = *py2;
 if (x2 - x1)
 {
  penteY = (float)(y2 - y1) / (x2 - x1);
  addY = y1 - penteY*x1;
 }
 else
 {
  penteY = 1;
  addY = 0;
 }

 if (y2 - y1)
 {
  penteX = (float)(x2 - x1) / (y2 - y1);
  addX = x1 - penteX*y1;
 }
 else
 {
  penteX = 1;
  addX = 0;
 }

 tclip = this->dc_stack->clip;
 while (tclip)
 {
  type1 = 0;
  if (tclip->Top == tclip->Bottom)
   return 0;
  if (tclip->Left == tclip->Right)
   return 0;
  if (y1 > tclip->Top)
   type1 |= 8;
  if (y1 < tclip->Bottom)
   type1 |= 4;
  if (x1 > tclip->Right)
   type1 |= 2;
  if (x1 < tclip->Left)
   type1 |= 1;
  type2 = 0;
  if (y2 > tclip->Top)
   type2 |= 8;
  if (y2 < tclip->Bottom)
   type2 |= 4;
  if (x2 > tclip->Right)
   type2 |= 2;
  if (x2 < tclip->Left)
   type2 |= 1;
  if (!(type1 | type2))
  {
   tclip = tclip->Next;
   continue;
  }
  if (type1 & type2)
   return 0;

  /* change point 1 */
  while (1)
  {
   if (type1 & 8)
   {
    ty1 = tclip->Top;
    tx1 = penteX*tclip->Top + addX;
    if ((tx1 >= tclip->Left-TOLERANCE) && (tx1 <= tclip->Right+TOLERANCE) &&
	(ty1 <= tclip->Top+TOLERANCE) && (ty1 >= tclip->Bottom-TOLERANCE))
    {
     x1 = tx1;
     y1 = ty1;
     break;
    }
   }
   else
    if (type1 & 4)
    {
     ty1 = tclip->Bottom;
     tx1 = penteX*tclip->Bottom + addX;
     if ((tx1 >= tclip->Left-TOLERANCE) && (tx1 <= tclip->Right+TOLERANCE) &&
	 (ty1 <= tclip->Top+TOLERANCE) && (ty1 >= tclip->Bottom-TOLERANCE))
     {
      x1 = tx1;
      y1 = ty1;
      break;
     }
     break;
    }
   if (type1 & 2)
   {
    tx1 = tclip->Right;
    ty1 = penteY*tclip->Right + addY;
    if ((tx1 >= tclip->Left-TOLERANCE) && (tx1 <= tclip->Right+TOLERANCE) &&
	(ty1 <= tclip->Top+TOLERANCE) && (ty1 >= tclip->Bottom-TOLERANCE))
    {
     x1 = tx1;
     y1 = ty1;
     break;
    }
   }
   else
    if (type1 & 1)
    {
     tx1 = tclip->Left;
     ty1 = penteY*tclip->Left + addY;
     if ((tx1 >= tclip->Left-TOLERANCE) && (tx1 <= tclip->Right+TOLERANCE) &&
	 (ty1 <= tclip->Top+TOLERANCE) && (ty1 >= tclip->Bottom-TOLERANCE))
     {
      x1 = tx1;
      y1 = ty1;
      break;
     }
    }
   break;
  }

  /* change point 2 */
  while (1)
  {
   if (type2 & 8)
   {
    ty1 = tclip->Top;
    tx1 = penteX*tclip->Top + addX;
    if ((tx1 >= tclip->Left-TOLERANCE) && (tx1 <= tclip->Right+TOLERANCE) &&
	(ty1 <= tclip->Top+TOLERANCE) && (ty1 >= tclip->Bottom-TOLERANCE))
    {
     x2 = tx1;
     y2 = ty1;
     break;
    }
   }
   else
    if (type2 & 4)
    {
     ty1 = tclip->Bottom;
     tx1 = penteX*tclip->Bottom + addX;
     if ((tx1 >= tclip->Left-TOLERANCE) && (tx1 <= tclip->Right+TOLERANCE) &&
	 (ty1 <= tclip->Top+TOLERANCE) && (ty1 >= tclip->Bottom-TOLERANCE))
     {
      x2 = tx1;
      y2 = ty1;
      break;
     }
    }
   if (type2 & 2)
   {
    tx1 = tclip->Right;
    ty1 = penteY*tclip->Right + addY;
    if ((tx1 >= tclip->Left-TOLERANCE) && (tx1 <= tclip->Right+TOLERANCE) &&
	(ty1 <= tclip->Top+TOLERANCE) && (ty1 >= tclip->Bottom-TOLERANCE))
    {
     x2 = tx1;
     y2 = ty1;
     break;
    }
   }
   else
    if (type2 & 1)
    {
     tx1 = tclip->Left;
     ty1 = penteY*tclip->Left + addY;
     if ((tx1 >= tclip->Left-TOLERANCE) && (tx1 <= tclip->Right+TOLERANCE) &&
	 (ty1 <= tclip->Top+TOLERANCE) && (ty1 >= tclip->Bottom-TOLERANCE))
     {
      x2 = tx1;
      y2 = ty1;
      break;
     }
    }
   break;
  }
  tclip = tclip->Next;
 }
 *px1 = x1;
 *py1 = y1;
 *px2 = x2;
 *py2 = y2;
 return 1;
}


/* ----------------------------------------------------------------- ** 
** get_lineto -- decode lineto gdi func                              ** 
** ----------------------------------------------------------------- */
static int get_lineto(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int	x1;
 int	y1;
 int	x2;
 int	y2;
 int	mode;

 if (read_short(fd, &y2) == -1)
  return -1;
 if (read_short(fd, &x2) == -1)
  return -1;

 x1 = this->last_x;
 y1 = this->last_y;
 this->last_x = x2;
 this->last_y = y2;
 /* intersect with all cliprect */
 if (clip(this, &x1, &y1, &x2, &y2))
 {
  if(!F(this->vector).move(this->vector,
			   SCALE_FROM_INCHES(x1)/this->factor,
			   SCALE_FROM_INCHES(-y1)/this->factor))
   return -1;
  if(!F(this->vector).line(this->vector,
			   SCALE_FROM_INCHES(x2)/this->factor,
			   SCALE_FROM_INCHES(-y2)/this->factor))
   return -1;
  mode = 0;
  if (this->pen_style != PS_NULL)
   mode |= XcVG_STROKE;
  if (!F(this->vector).render(this->vector, mode))
   return -1;
 }
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_moveto -- decode moveto gdi func                              ** 
** ----------------------------------------------------------------- */
static int get_moveto(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int	x1;
 int	y1;

 if (read_short(fd, &y1) == -1)
  return -1;
 if (read_short(fd, &x1) == -1)
  return -1;

 this->last_x = x1;
 this->last_y = y1;
 /*
   if(!F(this->vector).move(this->vector,
   SCALE_FROM_INCHES(x1)/this->factor,
   SCALE_FROM_INCHES(-y1)/this->factor))
   return -1;
   */
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_savedc -- decode savedc gdi func                              ** 
** ----------------------------------------------------------------- */
static int get_savedc(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 MDCStack	*new_dc_stack;
 MClipZone	*tclip;
 MClipZone	**ptrclip;

 new_dc_stack = Xc_malloc("dc", sizeof(MDCStack));
 new_dc_stack->Next = this->dc_stack;
 new_dc_stack->clip = NULL;
 tclip = this->dc_stack->clip;
 ptrclip = &(new_dc_stack->clip);
 while (tclip)
 {
  *ptrclip = Xc_malloc("clip", sizeof(MClipZone));
  memcpy(*ptrclip, tclip, sizeof(MClipZone));
  ptrclip = &((*ptrclip)->Next);
  tclip = tclip->Next;
 }
 this->dc_stack = new_dc_stack;
 
#if 0
 if (!F(this->vector).save(this->vector))
  return -1;
#endif

 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_restoredc -- decode restoredc gdi func                        ** 
** ----------------------------------------------------------------- */
static int get_restoredc(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int	num;

 if (read_short(fd, &num) == -1)
  return -1;

 if (num != -1)
 {
  Xc_WARNING(("num: %d\n", num));
 }
 else
 {
  MDCStack	*new_dc_stack;
  MClipZone	*tclip;
  MClipZone	*tclip2;

  tclip = this->dc_stack->clip;
  while (tclip)
  {
   tclip2 = tclip->Next;
   Xc_free(tclip);
   tclip = tclip2;
  }
  new_dc_stack = this->dc_stack->Next;
  Xc_free(this->dc_stack);
  this->dc_stack = new_dc_stack;
 }

#if 0
 if (!F(this->vector).restore(this->vector))
  return -1;
#endif

 return 1;
}


/* ----------------------------------------------------------------- ** 
** get_intersectcliprect -- decode intersectcliprect gdi func        ** 
** ----------------------------------------------------------------- */
static int get_intersectcliprect(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int	l;
 int	t;
 int	r;
 int	b;
 MClipZone	**ptrclip;

 if (read_short(fd, &b) == -1)
  return -1;
 if (read_short(fd, &r) == -1)
  return -1;
 if (read_short(fd, &t) == -1)
  return -1;
 if (read_short(fd, &l) == -1)
  return -1;
 if(!F(this->vector).move(this->vector,
			  SCALE_FROM_INCHES(l)/this->factor,
			  SCALE_FROM_INCHES(-t)/this->factor))
  return -1;
 if(!F(this->vector).line(this->vector,
			  SCALE_FROM_INCHES(r)/this->factor,
			  SCALE_FROM_INCHES(-t)/this->factor))
  return -1;
 if(!F(this->vector).line(this->vector,
			  SCALE_FROM_INCHES(r)/this->factor,
			  SCALE_FROM_INCHES(-b)/this->factor))
  return -1;
 if(!F(this->vector).line(this->vector,
			  SCALE_FROM_INCHES(l)/this->factor,
			  SCALE_FROM_INCHES(-b)/this->factor))
  return -1;
 if(!F(this->vector).closePath(this->vector))
  return -1;

 ptrclip = &(this->dc_stack->clip);
 while (*ptrclip)
 {
  ptrclip = &((*ptrclip)->Next);
 }
 *ptrclip = Xc_malloc("clip", sizeof(MClipZone));

 if (t < b)
 {
  (*ptrclip)->Top = b;
  (*ptrclip)->Bottom = t;
 }
 else
 {
  (*ptrclip)->Top = t;
  (*ptrclip)->Bottom = b;
 }

 if (l < r)
 {
  (*ptrclip)->Left = l;
  (*ptrclip)->Right = r;
 }
 else
 {
  (*ptrclip)->Left = r;
  (*ptrclip)->Right = l;
 }

 (*ptrclip)->Next = NULL;

#if 0
 if (!F(this->vector).clip(this->vector) ||
     !F(this->vector).newPath(this->vector))
  return -1;
#endif
 if (!F(this->vector).newPath(this->vector))
  return -1;

 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_createfontindirect -- decode createfontindirect gdi func      ** 
** ----------------------------------------------------------------- */
static int get_createfontindirect(this, fd, size)
c_WMFDocFile	*this;
FILE		*fd;
long		size;
{
 int	height;
 int	width;
 int	escapement;
 int	orientation;
 int	weight;
 char	italic;
 char	underline;
 char	strikeout;
 char	charset;
 char	outprecision;
 char	clipprecision;
 char	quality;
 char	pitchandfamily;
 char	*name;
 int	pos;
 int	i;
 int	max;
 c_VectorFont *font;

 if (read_word(fd, &height) == -1)
  return -1;
 if (read_word(fd, &width) == -1)
  return -1;
 if (read_word(fd, &escapement) == -1)
  return -1;
 if (read_word(fd, &orientation) == -1)
  return -1;
 if (read_word(fd, &weight) == -1)
  return -1;
 if (read_char(fd, &italic) == -1)
  return -1;
 if (read_char(fd, &underline) == -1)
  return -1;
 if (read_char(fd, &strikeout) == -1)
  return -1;
 if (read_char(fd, &charset) == -1)
  return -1;
 if (read_char(fd, &outprecision) == -1)
  return -1;
 if (read_char(fd, &clipprecision) == -1)
  return -1;
 if (read_char(fd, &quality) == -1)
  return -1;
 if (read_char(fd, &pitchandfamily) == -1)
  return -1;

 /* 18 bytes read */
 size -= (18+6);

 name = Xc_malloc("fontname", 20);
 i = 0;
 max = 20;
 while (1)
 {
  read_char(fd, &(name[i]));
  size --;
  if (!name[i])
   break;
  i++;
  if (i > max)
  {
   name = Xc_realloc("rfontname", name, max+20);
   max += 20;
  }
 }
 if (size)
  fseek(fd, size, SEEK_CUR);

 font = wmfFindFont(this, name);
 Xc_free(name);
 if(font == NULL) 
  return -1;

 pos = get_obj_pos(this);
 this->obj_list[pos].metafont.type = M_OBJ_FONT;
 this->obj_list[pos].metafont.size = height;
 this->obj_list[pos].metafont.font = font;
  

 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_exttextout -- decode exttextout gdi func                      ** 
** ----------------------------------------------------------------- */
static int get_exttextout(this, fd, size)
c_WMFDocFile	*this;
FILE		*fd;
long		size;
{
 int	x;
 int	y;
 int	rsize;
 int	option;
 char	*string;
 matrix_t matrix;

 if (read_short(fd, &x) == -1)
  return -1;
 if (read_short(fd, &y) == -1)
  return -1;
 if (read_word(fd, &rsize) == -1)
  return -1;
 if (read_word(fd, &option) == -1)
  return -1;

 string = Xc_malloc("text", rsize+1);
 if (fread(string, 1, rsize, fd) < 1)
 {
  Xc_free(string);
  return -1;
 }
 string[rsize]=0;

 size -= rsize + 6 + 8;

 if (size)
  fseek(fd, size, SEEK_CUR);

 XcM_INIT_MATRIX(&matrix,
		 1, 0, 0, 1, 0, 0);
 if(!F(this->vector).setTextMatrix(this->vector, &matrix))
 {
  Xc_free(string);
  return -1;
 }

 if(!F(this->vector).setTextPoint(this->vector, 
				  SCALE_FROM_INCHES(x)/this->factor,
				  SCALE_FROM_INCHES(-y)/this->factor))
 {
  Xc_free(string);
  return -1;
 }
 
 if(!F(this->vector).setTextMode(this->vector, 0))
 {
  Xc_free(string);
  return -1;
 }

 if(!F(this->vector).textRender(this->vector, string, strlen(string)))
 {
  Xc_free(string);
  return -1;
 }
 Xc_free(string);
 return 1;
}


/* ----------------------------------------------------------------- ** 
** wmfFindFont - Find VectorFont from name and table                 ** 
** ----------------------------------------------------------------- */
static c_VectorFont *wmfFindFont(this, name)
c_WMFDocFile *this;
char *name;
{
 Xc_TRACE(("wmfFindFont(`%s')", name));

 /* translate name ? */
  
 if(GlobFontBase != NULL)
 {
  c_VectorFont *font;
      
  font = F(GlobFontBase).getFontByName(GlobFontBase, name);
  return((font == NULL)?
	 F(GlobFontBase).selectDialog(GlobFontBase, name) : font);
 }
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** get_roundrect -- decode roundrect gdi func                        ** 
** ----------------------------------------------------------------- */
static int get_roundrect(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int	l;
 int	t;
 int	r;
 int	b;
 int	w;
 int	h;
 int	mode;

 if (read_short(fd, &h) == -1)
  return -1;
 if (read_short(fd, &w) == -1)
  return -1;
 if (read_short(fd, &b) == -1)
  return -1;
 if (read_short(fd, &r) == -1)
  return -1;
 if (read_short(fd, &t) == -1)
  return -1;
 if (read_short(fd, &l) == -1)
  return -1;

 if(!F(this->vector).move(this->vector,
			  SCALE_FROM_INCHES(l)/this->factor,
			  SCALE_FROM_INCHES(-(t + h/2))/this->factor))
  return -1;
 if(!F(this->vector).arc(this->vector,
			 SCALE_FROM_INCHES(l + w/2)/this->factor,
			 SCALE_FROM_INCHES(-(t + h/2))/this->factor,
			 SCALE_FROM_INCHES(w/2)/this->factor,
			 SCALE_FROM_INCHES(h/2)/this->factor,
			 SCALE_FROM_DEGREES(180),
			 SCALE_FROM_DEGREES(90)))
  return -1;
 if(!F(this->vector).line(this->vector,
			  SCALE_FROM_INCHES(r - w/2)/this->factor,
			  SCALE_FROM_INCHES(-t)/this->factor))
  return -1;
 if(!F(this->vector).arc(this->vector,
			 SCALE_FROM_INCHES(r - w/2)/this->factor,
			 SCALE_FROM_INCHES(-(t + h/2))/this->factor,
			 SCALE_FROM_INCHES(w/2)/this->factor,
			 SCALE_FROM_INCHES(h/2)/this->factor,
			 SCALE_FROM_DEGREES(90),
			 SCALE_FROM_DEGREES(0)))
  return -1;

 if(!F(this->vector).line(this->vector,
			  SCALE_FROM_INCHES(r)/this->factor,
			  SCALE_FROM_INCHES(-(b - h/2))/this->factor))
  return -1;
 if(!F(this->vector).arc(this->vector,
			 SCALE_FROM_INCHES(r - w/2)/this->factor,
			 SCALE_FROM_INCHES(-(b - h/2))/this->factor,
			 SCALE_FROM_INCHES(w/2)/this->factor,
			 SCALE_FROM_INCHES(h/2)/this->factor,
			 SCALE_FROM_DEGREES(0),
			 SCALE_FROM_DEGREES(-90)))
  return -1;

 if(!F(this->vector).line(this->vector,
			  SCALE_FROM_INCHES(l + w/2)/this->factor,
			  SCALE_FROM_INCHES(-b)/this->factor))
  return -1;
 
 if(!F(this->vector).arc(this->vector,
			 SCALE_FROM_INCHES(l + w/2)/this->factor,
			 SCALE_FROM_INCHES(-(b - h/2))/this->factor,
			 SCALE_FROM_INCHES(w/2)/this->factor,
			 SCALE_FROM_INCHES(h/2)/this->factor,
			 SCALE_FROM_DEGREES(270),
			 SCALE_FROM_DEGREES(180)))
  return -1;
 if(!F(this->vector).closePath(this->vector))
  return -1;

 mode = 0;
 if (this->pen_style != PS_NULL)
  mode |= XcVG_STROKE;
 if (this->brush_style != BS_NULL)
  mode |= XcVG_FILL;
 if (!F(this->vector).render(this->vector, mode))
  return -1;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** get_polyfillmode -- decode polyfillmode gdi func                  ** 
** ----------------------------------------------------------------- */
static int get_polyfillmode(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
  int		mode;

  if (read_word(fd, &mode) == -1)
   return -1;
  /* always winding for now */
  /*printf("%s\n", polyfill_mode[mode]);*/
  return 1;
}

/* ----------------------------------------------------------------- ** 
** get_polypolygon -- decode polypolygon gdi func                    ** 
** ----------------------------------------------------------------- */
static int get_polypolygon(this, fd)
c_WMFDocFile	*this;
FILE		*fd;
{
 int	x;
 int	y;
 int	num;
 int	mode;
 int	*polyvertex;
 int	i;

 if (read_short(fd, &num) == -1)
  return -1;
 Xc_TRACE(("num:%d", num));

 polyvertex = Xc_malloc("polyv", num*sizeof(int));
 i = 0;
 while (i < num)
 {
  if (read_short(fd, &(polyvertex[i])) == -1)
   return -1;
  i++;
 }

 i = 0;
 while (i < num)
 {

  if (read_short(fd, &x) == -1)
   return -1;
  if (read_short(fd, &y) == -1)
   return -1;
  if(!F(this->vector).move(this->vector,
			   SCALE_FROM_INCHES(x)/this->factor,
			   SCALE_FROM_INCHES(-y)/this->factor))
   return -1;

  polyvertex[i]--;
  while (polyvertex[i])
  {
   if (read_short(fd, &x) == -1)
    return -1;
   if (read_short(fd, &y) == -1)
    return -1;
   if(!F(this->vector).line(this->vector,
			    SCALE_FROM_INCHES(x)/this->factor,
			    SCALE_FROM_INCHES(-y)/this->factor))
    return -1;
   Xc_TRACE(("x:%d y:%d", x, y));
   polyvertex[i] --;
  }
  if(!F(this->vector).closePath(this->vector))
   return -1;
  i++;
 }

 Xc_free(polyvertex);
 mode = 0;
 if (this->pen_style != PS_NULL)
  mode |= XcVG_STROKE;
 if (this->brush_style != BS_NULL)
  mode |= XcVG_FILL;
 if (!F(this->vector).render(this->vector, mode))
  return -1;
 return 1;
}
