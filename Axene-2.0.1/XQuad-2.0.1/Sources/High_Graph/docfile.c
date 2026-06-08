/*
** docfile.c for XQuad in High_Graph/
** Loading & saving methods for High_Graph class
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
** Started on  Mon Aug 28 18:45:59 1995 Stéphane Boisson
** Last update Fri Oct  2 17:04:55 1998 Robin Castanier
*/

#include "High_GraphP.h"

/*--- Keywords for loading & saving ---*/
static char *keywords[] = 
{
 "NONE", "BARS", "HISTOGRAM", "SECTOR", "SURFACE", "CURVE", "RADAR", 
 "LEGEND", "AXE"
};

/*--- Prototypes ---*/
static boolean cb_area ___PROTO((c_DocFile *doc, char *keyword,
				 long param, region_t **regionp));
static boolean cb_class ___PROTO((c_DocFile *doc, char *keyword,
				  long param, HG_class_t *classp));
static boolean cb_bitfield ___PROTO((c_DocFile *doc, char *keyword,
				     long param, long *bitfieldp));


/* ----------------------------------------------------------------- ** 
** writeVector - Standard frame write method                         ** 
** ----------------------------------------------------------------- */
boolean writeHighGraph(this, doc)
c_HighGraph *this;
c_DocFile *doc;
{
 long bitfield;
 char *ptr;
 region_t	*region;
  
 Xc_HISTORY(("write"));

 if(!F(doc).writeKeyword(doc, XcDF_GRAPH_KEYWORD, -1L) ||
    !F(doc).startSequence(doc)) return FALSE;
  
 region = this->orig_region;
 while(region)
 {
  /*--- Region ---*/
  if((ptr = regtostr(region)) == NULL) return FALSE;
  if(!F(doc).writeKeyword(doc, XcDF_AREA_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).write(doc, ptr, strlen(ptr)) ||
     !F(doc).endSequence(doc)) {Xc_free(ptr); return FALSE;}
  Xc_free(ptr);
  region = region->NextRegion;
 }

 /*--- Class ---*/
 Xc_ASSERT(this->class >= 0);
 Xc_ASSERT(this->class < (sizeof(keywords)/sizeof(char *)));
 if(!F(doc).writeKeyword(doc, keywords[(int)this->class], -1))
  return FALSE;

 /*--- Bitfield ---*/
 bitfield = this->col_row? 1 : 0;
 bitfield <<= 1; bitfield |= this->abscisse? 1 : 0;
 bitfield <<= 1; bitfield |= this->ordonnee? 1 : 0;
 bitfield <<= 1; bitfield |= this->title? 1 : 0;
 bitfield <<= 1; bitfield |= this->legend? 1 : 0;
 bitfield <<= 1; bitfield |= this->axe? 1 : 0;
 bitfield <<= 1; bitfield |= this->d_abscisse? 1 : 0;
 bitfield <<= 1; bitfield |= this->d_ordonnee? 1 : 0;
 bitfield <<= 1; bitfield |= this->d_title? 1 : 0;
 bitfield <<= 1; bitfield |= this->d_legend? 1 : 0;
 bitfield <<= 1; bitfield |= this->graduation_y? 1 : 0;
 bitfield <<= 1; bitfield |= this->multiple_region? 1 : 0;
 bitfield <<= 1; bitfield |= this->region_direction? 1 : 0;
 Xc_ASSERT(bitfield >= 0L);
 if(!F(doc).writeKeyword(doc, XcDF_BITFIELD_KEYWORD, bitfield))
  return FALSE;
  
 return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** vectorRead - loading callback                                     ** 
** ----------------------------------------------------------------- */
boolean readHighGraph(doc, keyword, param, vector)
c_DocFile *doc;
char *keyword;
long param;
c_VectorGraph *vector;
{
 c_HighGraph *this;
 region_t *region, *region2;
 HG_class_t class;
 long bitfield;
 int i;

 Xc_TRACE(("read"));

 region = NULL;
 bitfield = 0L;
 class = HG_NONE;

 /*--- Class keywords ---*/
 for(i = 0; i < (int)(sizeof(keywords) / sizeof(char *)); i++)
  if(!F(doc).addCallbacks(doc, keywords[i], cb_class, &class, NULL))
   return FALSE;
  
 if(!F(doc).addCallbacks(doc,
			 XcDF_AREA_KEYWORD, cb_area, &region,
			 XcDF_BITFIELD_KEYWORD, cb_bitfield, &bitfield,
			 NULL)) return FALSE;

 if(F(doc).expectKeyword(doc, NULL, TRUE) && region)
 {
  this = NEW(c_HighGraph)(NULL, vector->frame->BaseStd,
			  region, class, 0, vector);
  while(region)
  {
   region2 = region->NextRegion;
   Xc_free(region); 
   region = region2;
  }
  if(this != NULL)
  {
   this->region_direction = (bitfield & 1)? TRUE : FALSE; bitfield >>=1;
   this->multiple_region = (bitfield & 1)? TRUE : FALSE; bitfield >>= 1;
   this->graduation_y = (bitfield & 1)? TRUE : FALSE; bitfield >>= 1;
   this->d_legend = (bitfield & 1)? TRUE : FALSE; bitfield >>= 1;
   this->d_title = (bitfield & 1)? TRUE : FALSE; bitfield >>= 1;
   this->d_ordonnee = (bitfield & 1)? TRUE : FALSE; bitfield >>= 1;
   this->d_abscisse = (bitfield & 1)? TRUE : FALSE; bitfield >>= 1;
   this->axe = (bitfield & 1)? TRUE : FALSE; bitfield >>= 1;
   this->legend = (bitfield & 1)? TRUE : FALSE; bitfield >>= 1;
   this->title = (bitfield & 1)? TRUE : FALSE; bitfield >>= 1;
   this->ordonnee = (bitfield & 1)? TRUE : FALSE; bitfield >>= 1;
   this->abscisse = (bitfield & 1)? TRUE : FALSE; bitfield >>= 1;
   this->col_row = (bitfield & 1)? TRUE : FALSE; bitfield >>= 1;
   return TRUE;
  }
 }
  
 while(region)
 {
  region2 = region->NextRegion;
  Xc_free(region); 
  region = region2;
 }
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_bitfield - Bitfiled loading callback                           ** 
** ----------------------------------------------------------------- */
static boolean cb_bitfield(doc, keyword, param, ptr)
c_DocFile *doc;
char *keyword;
long param;
long *ptr;
{
 Xc_TRACE(("cb_bitfield(%ld)", param));
 *ptr = param;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_class - HG class loading callback                              ** 
** ----------------------------------------------------------------- */
static boolean cb_class(doc, keyword, param, ptr)
c_DocFile *doc;
char *keyword;
long param;
HG_class_t *ptr;
{
 int i;

 Xc_TRACE(("cb_class(`%s')", keyword));

 for(i = 0; i < (int)(sizeof(keywords) / sizeof(char *)); i++)
  if(!strcmp(keywords[i], keyword))
  {
   *ptr = (HG_class_t)i;
   return TRUE;
  }
 
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** cb_area - Loading callback for region                             ** 
** ----------------------------------------------------------------- */
static boolean cb_area(doc, keyword, param, regionp)
c_DocFile *doc;
char *keyword;
long param;
region_t **regionp;
{
 char *str;
 region_t	*region;
  
 Xc_TRACE(("cb_area"));

 if(!F(doc).readString(doc, (unsigned char **)(&str)) || (str == NULL))
  return FALSE;
 Xc_TRACE(("area: `%s'", str));
  
 if(*regionp != NULL)
 {
  region = *regionp;
  while(region->NextRegion)
   region = region->NextRegion;
  region->NextRegion = strtoreg(str);
  if (region->NextRegion)
   region->NextRegion->NextRegion = NULL;
 }
 else
 {
  *regionp = strtoreg(str);
  if (*regionp)
   (*regionp)->NextRegion = NULL;
 }
 Xc_free(str);
  
 return TRUE;
}


