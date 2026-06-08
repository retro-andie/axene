/*
** VectorFont.c for Xclamation, XQuad and XAllWrite in VectorFont/
** Methods for the VectorFont class
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
** Started on  Wed Jul 20 16:41:58 1994 Stéphane Boisson
** Last update Tue Jan 18 16:02:45 2000 Emmanuel Paris
*/

#define NTRACE

#include <string.h>
#include "VectorFont.h"
#include "AFM.h"
#include "file.h"
#include "FontManager.h"

extern c_FontManager    *GlobFntMng;

#define XcVF_FONTSDIR "fonts"
#define XcVF_METRICSDIR "metrics"

static ItemHandle *i_handle = NULL;
static long i_references = 0;

/*--- Prototypes ---*/
static void *constructor();
static void destructor();
static void *copier();
static boolean loadMetrics ___PROTO((c_VectorFont *this));
static boolean readFont ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_VectorFont **base));
static boolean writeFont ___PROTO((c_VectorFont *class, c_DocFile *doc));
static boolean writeFontList ___PROTO((c_VectorFont *this, c_DocFile *doc));
static boolean readFontList ___PROTO((c_DocFile *doc, char *keyword,
				      long param, c_VectorFont **base));
static c_VectorFont *getFontByName ___PROTO((c_VectorFont *this, char *name));
static boolean cb_fullname ___PROTO((c_DocFile *doc, char *keyword,
				     long param, unsigned char **namep));
static boolean cb_family ___PROTO((c_DocFile *doc, char *keyword,
				   long param, unsigned char **namep));
static boolean cb_alias ___PROTO((c_DocFile *doc, char *keyword,
				   long param, font_alias **aliases));
static boolean cb_flag ___PROTO((c_DocFile *doc, char *keyword,
				 long param, boolean *flag));
static boolean ps_print ___PROTO((c_VectorFont *this, c_PostScript *post));

static c_VectorFont *getFontByFamilyAndAttribs ___PROTO((c_VectorFont *this,
							 char *family_name,
							 boolean bold,
							 boolean italic));
static boolean isBase13 ___PROTO((c_VectorFont *this));

c_VectorFont *vectorFontDialog ___PROTO((c_VectorFont *base, char *fontname));
boolean font_family_filter ___PROTO((family_tree *family));

#ifndef NDEBUG
static void dump ___PROTO((c_VectorFont *this));
#endif


sf_VectorFont fc_VectorFont =
{
 constructor, destructor, copier,
 loadMetrics,
 getFontByName,
 readFont, writeFont, readFontList, writeFontList,
 ps_print,
 getFontByFamilyAndAttribs,
 vectorFontDialog,
#ifndef NDEBUG
 dump
#endif
};


/*--- Font types ---*/
static struct 
{
 char *extension;
 enum fonttype_e type;
} FontBase[] =
{
 {".pfa", XcFT_POSTSCRIPT_TYPE1},
 {".pfb", XcFT_POSTSCRIPT_TYPE1},
 {".pfc", XcFT_POSTSCRIPT_TYPE1},
 {NULL, XcFT_UNKNOW},
};


/* ----------------------------------------------------------------- ** 
** constructor - VectorFont class constructor                        ** 
** ----------------------------------------------------------------- */
static void *constructor(base, filename, name)
c_VectorFont **base;
char *filename;
char *name;
{
 char *filename_ext;
 c_VectorFont *p;
 int i;

 Xc_HISTORY(("constructor(`%s')", name));

 if(i_handle == NULL)
  i_handle = Xc_createItem("VectorFont base", 16, sizeof(c_VectorFont));
 if(i_handle == NULL) return NULL;

 if((p = Xc_new(i_handle)) == NULL) return NULL;
 i_references++;
 memset(p, 0, sizeof(c_VectorFont));
 p->f = &fc_VectorFont;

 /*--- Insert in linked list ---*/
 p->next = *base;
 p->previous = NULL;
 if(p->next != NULL) p->next->previous = p;
 *base = p;
 p->base = base;

 p->resident_flag = FALSE;
 p->encoding = XcFE_UNKNOW;

 if((p->name = Xc_strdup("FontName", name)) == NULL)
 {
  DELETE(c_VectorFont)(p);
  return NULL;
 }

 if((p->font_name = Xc_strdup("FontName", name)) == NULL)
 {
  DELETE(c_VectorFont)(p);
  return NULL;
 }

 if((p->filename = TranslateFilename(XcVF_FONTSDIR, filename)) == NULL)
 {
  DELETE(c_VectorFont)(p);
  return NULL;
 }

 if(!IsFileExist(p->filename))
 {
  Xc_SYSERROR((p->filename));
  DELETE(c_VectorFont)(p);
  return NULL;
 }
 p->garbage = NULL;

 p->type = XcFT_UNKNOW;
 if((filename_ext = strrchr(filename, '.')) != NULL)
  for(i = 0; FontBase[i].type != XcFT_UNKNOW; i++)
   if(!strcmp(filename_ext, FontBase[i].extension))
   {
    p->type = FontBase[i].type;
    break;
   }
  
 if(p->type == XcFT_UNKNOW)
 {
  Xc_ERROR(("Unknow filename extension for `%s'", filename));
  DELETE(c_VectorFont)(p);
  return NULL;
 }

 switch(p->type)
 {
 case XcFT_POSTSCRIPT_TYPE1:
  {
   char *afmfile;
   boolean rc;
   int len;

   len = filename_ext - filename;
   if((afmfile = Xc_malloc("AFM Filename", len + 5)) == NULL)
   {
    DELETE(c_VectorFont)(p);
    return NULL;
   }

   memcpy(afmfile, filename, len);
   rc = FALSE;

   strcpy(afmfile + len, ".AFM");
   p->metrics_filename = TranslateFilename(XcVF_METRICSDIR, afmfile);
   if(p->metrics_filename != NULL)
   {
    if(!IsFileExist(p->metrics_filename))
    {
     Xc_free(p->metrics_filename);
     strcpy(afmfile + len, ".afm");
     p->metrics_filename = TranslateFilename(XcVF_METRICSDIR,
					     afmfile);
     if((p->metrics_filename != NULL) &&
	IsFileExist(p->metrics_filename)) rc = TRUE;
    }
    else
     rc = TRUE;
   }
   if(rc == FALSE)
   {
    Xc_SYSERROR((afmfile));
    Xc_free(afmfile);
    DELETE(c_VectorFont)(p);
    return NULL;
   }
	
   Xc_free(afmfile);
   break;
  }
      
 default:
  Xc_ERROR(("`%s': Font format non implemented", filename));
  DELETE(c_VectorFont)(p);
  return NULL;
 }


 Xc_TRACE(("constructor ok"));
 return(p);
}


/* ----------------------------------------------------------------- ** 
** destructor - VectorFont class destructor                          ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_VectorFont *this;
{
 Xc_HISTORY(("destructor(`%s')", this->font_name));
 Xc_ASSERT(i_references > 0);

 /*--- Remove from the linked list ---*/
 if(this->next != NULL)
  this->next->previous = this->previous;
 if(this->previous != NULL)
  this->previous->next = this->next;
 else
 {
  Xc_ASSERT(*this->base == this);
  *this->base = this->next;
 }

 if(this->resident_flag) DELETE(c_CharMetrics)(this->metrics);
  
 if (this->spec_family)
 {
  family_tree	**tmp_family;
  font_list		**tmp_font;

  tmp_font = &(this->spec_family->list);
  while (*tmp_font && (*tmp_font != this->specific))
  {
   tmp_font = &((*tmp_font)->Next);
  }
  if (*tmp_font)
  {
   *tmp_font = this->specific->Next;
   Xc_free(this->specific);
  }
  if (this->spec_family->list == NULL)
  {
   if (this->spec_family != this->family)
   {
    tmp_family = &(this->family);
    while (*tmp_family && (*tmp_family != this->spec_family))
    {
     tmp_family = &((*tmp_family)->Next);
    }
    if (*tmp_family)
    {
     *tmp_family = this->spec_family->Next;
     Xc_free(this->spec_family->name);
     Xc_free(this->spec_family);
    }
   }
   if ((this->family->Next == NULL)
       && (this->family->list == NULL))
   {
    Xc_free(this->family->name);
    Xc_free(this->family);
    this->family = NULL;
   }
  }
 }
  
 if (this->aliases)
 {
  font_alias *alias, *alias2;
    
  alias = this->aliases;
  while(alias)
  {
   alias2 = alias->next;
   if (alias->alias_name)
    Xc_free(alias->alias_name);
   Xc_free(alias);
   alias = alias2;
  }
 }
  
#define XCVF_FREE(font,menber) if(font->menber != NULL) Xc_free(font->menber)
 XCVF_FREE(this, filename);
 XCVF_FREE(this, metrics_filename);
 XCVF_FREE(this, name);
 XCVF_FREE(this, font_name);
 XCVF_FREE(this, full_name);
 XCVF_FREE(this, family_name);
 XCVF_FREE(this, weight);
 XCVF_FREE(this, version);
 XCVF_FREE(this, notice);
 XCVF_FREE(this, character_set);
#undef XCVF_FREE

 if (this->garbage)
 {
  garbage_encode *garb1, *garb2;

  garb1 = this->garbage;
  while(garb1)
  {
   garb2 = garb1->next;
   Xc_free(garb1->encode_name);
   Xc_free(garb1);
   garb1 = garb2;
  }
 }

 if(this->hook != NULL) DELETE(c_Hook)(this->hook);

 Xc_delete(i_handle, this);
 if(--i_references == 0)
 {
  Xc_destroyItem(i_handle);
  i_handle = NULL;
 }
}


/* ----------------------------------------------------------------- ** 
** copier - VectorFont method copy method                            ** 
** ----------------------------------------------------------------- */
static void *copier(this)
c_VectorFont *this;
{
 Xc_HISTORY(("copy(`%s') not implemented", this->font_name));
 return NULL;
}



#ifndef NDEBUG
/* ----------------------------------------------------------------- ** 
** dump - Debugging purposes                                         ** 
** ----------------------------------------------------------------- */
static void dump(this)
c_VectorFont *this;
{
 c_CharMetrics *ptr;

 Xc_HISTORY(("dump"));
 ptr = this->metrics;
 printf("font: `%s'\n", this->full_name);
 Xc_ASSERT(ptr->pure == TRUE);
 Xc_ASSERT(ptr->counter == 1);
 printf("  Base: %g points, %g%% expansion, %g degrees, %g%%\n",
	SCALE_TO_POINTS(ptr->point_size),
	SCALE_TO_PERCENT(ptr->width),
	SCALE_TO_DEGREES(ptr->angle),
	SCALE_TO_PERCENT(ptr->scale));
 ptr = ptr->next;
 while(ptr != NULL)
 {
  Xc_ASSERT(ptr->pure == TRUE);
  printf("    Pure copy x %d: %g points, %g%% expansion, %g degrees, %g%%\n",
	 ptr->counter, SCALE_TO_POINTS(ptr->point_size),
	 SCALE_TO_PERCENT(ptr->width),
	 SCALE_TO_DEGREES(ptr->angle),
	 SCALE_TO_PERCENT(ptr->scale));
  ptr = ptr->next;
  while((ptr != NULL) && (ptr->pure == FALSE))
  {
   printf("      Impure copy x %d: %g points, %g%% expansion, %g degrees, %g%%\n",
	  ptr->counter, SCALE_TO_POINTS(ptr->point_size),
	  SCALE_TO_PERCENT(ptr->width),
	  SCALE_TO_DEGREES(ptr->angle),
	  SCALE_TO_PERCENT(ptr->scale));
   ptr = ptr->next;
  }
 }
}	 
#endif  


/* ----------------------------------------------------------------- ** 
** loadMetrics - Make sure that the font metrics are loaded          ** 
** ----------------------------------------------------------------- */
static boolean loadMetrics(this)
c_VectorFont *this;
{
 c_CharMetrics *metrics;

 Xc_HISTORY(("loadMetrics(`%s')", this->font_name));

 if(this->resident_flag) return TRUE;

 if((this->hook = NEW(c_Hook)(this, 1)) == NULL)
 {
  DELETE(c_VectorFont)(this);
  Xc_TRACE(("not loaded"));
  return FALSE;
 }

 if((this->metrics = NEW(c_CharMetrics)()) == NULL) return FALSE;

 switch(this->type)
 {
 case XcFT_POSTSCRIPT_TYPE1:
  {
   if(this->metrics_filename == NULL) return FALSE;
	
   if(AFM_parse(this->metrics_filename, this) == FALSE)
   {
    Xc_WARNING(("loadMetrics error"));
    if(this->metrics != NULL) DELETE(c_CharMetrics)(this->metrics);
    this->metrics = NULL;
    return FALSE;
   }
   break;
  }

 default:
  Xc_ERROR(("`%s': Font format non implemented", this->filename));
  return FALSE;
 }

 if((metrics = F(this->metrics).pack(this->metrics)) == NULL)
 {
  DELETE(c_CharMetrics)(this->metrics);
  this->metrics = NULL;
  Xc_TRACE(("not loaded"));
  return FALSE;
 }

 this->resident_flag = TRUE;
 this->metrics = metrics;

 Xc_TRACE(("loaded"));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** readFont - VectorFont loading callback                            ** 
** ----------------------------------------------------------------- */
static boolean readFont(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
c_VectorFont **base;
{
 unsigned char *filename, *name, *fullname, *familyname;
 font_alias *aliases;
 boolean download_flag, hide_flag;
 boolean is_normal;
 boolean is_bold;
 boolean is_italic;
 boolean is_bold_italic;
 c_VectorFont *font;
 long len;
 family_tree	**family;
 font_list	**flist;

 Xc_TRACE(("readFont"));
 family = NULL;
 if (*base != NULL)
 {
  family = &((*base)->family);
 }

 familyname = fullname = name = filename = NULL;
 aliases = NULL;
 download_flag = FALSE;
 hide_flag = FALSE;
 is_normal = FALSE;
 is_bold = FALSE;
 is_italic = FALSE;
 is_bold_italic = FALSE;
 if(!F(doc).addCallbacks(doc,
			 XcDF_FULLNAME_KEYWORD, cb_fullname, &fullname,
			 XcDF_FAMILY_KEYWORD, cb_family, &familyname,
			 XcDF_ALIAS_KEYWORD, cb_alias, &aliases,
			 XcDF_DOWNLOAD_KEYWORD, cb_flag, &download_flag,
			 XcDF_NORMAL_KEYWORD, cb_flag, &is_normal,
			 XcDF_BOLD_KEYWORD, cb_flag, &is_bold,
			 XcDF_ITALIC_KEYWORD, cb_flag, &is_italic,
			 XcDF_BOLD_ITALIC_KEYWORD, cb_flag, &is_bold_italic,
			 XcDF_HIDE_KEYWORD, cb_flag, &hide_flag,
			 NULL)) return FALSE;

 do
 {
  boolean flag;

  do
  {
   flag = F(doc).readWord(doc, &name, &len);
  }
  while(len == 0);
  if(!flag || (len < 0)) break; 
      
  if(!F(doc).readString(doc, &filename)) break;
    
  font = NEW(c_VectorFont)(base, filename, name);
  if(font == NULL)
  {
   Xc_WARNING(("`%s' font not loaded", name));
   if(fullname != NULL) Xc_free(fullname);
   if(familyname != NULL) Xc_free(familyname);
  }
  else
  {
   font->full_name = (char *)fullname;
   font->download_flag = download_flag;
   font->hide_flag = hide_flag;
   font->aliases = aliases;
   font->family = NULL;
   font->specific = NULL;
   font->spec_family = NULL;

   if (!familyname)
    familyname = (unsigned char *)Xc_strdup("FamilyName", fullname);
   Xc_TRACE(("%s", familyname));

   if (!family)
   {
    family = &(font->family);
   }
   else
   {
    font->family = *family;
   }
	
   while (*family)
   {
    if (strcmp((*family)->name, familyname) == 0)
     break;
    family = &((*family)->Next);
   }
   if (!*family)
   {
    *family = Xc_malloc("family", sizeof(family_tree));
    (*family)->Next = NULL;
    (*family)->name = (char *)familyname;
    (*family)->list = NULL;
    (*family)->can_normal = 0;
    (*family)->can_bold = 0;
    (*family)->can_italic = 0;
    (*family)->can_bold_italic = 0;
   }
   else
   {
    Xc_free(familyname);
   }
   font->spec_family = *family;
   flist = &((*family)->list);
   while (*flist)
   {
    flist = &((*flist)->Next);
   }
   *flist = Xc_malloc("font_list_part", sizeof(font_list));
   (*flist)->Next = NULL;
   (*flist)->font = font;
   if ((is_normal | is_bold | is_italic | is_bold_italic) == FALSE)
    is_normal = TRUE;
   (*flist)->is_normal = is_normal;
   (*flist)->is_bold = is_bold;
   (*flist)->is_italic = is_italic;
   (*flist)->is_bold_italic = is_bold_italic;
   font->specific = *flist;
	
   (*family)->can_normal |= is_normal;
   (*family)->can_bold |= is_bold;
   (*family)->can_italic |= is_italic;
   (*family)->can_bold_italic |= is_bold_italic;
  }

  Xc_free(name);
  Xc_free(filename);
  return TRUE;
 } while(0);

 if(fullname != NULL) Xc_free(fullname);
 if(filename != NULL) Xc_free(filename);
 if(name != NULL) Xc_free(name);
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** writeFont - VectorFont saving method                              ** 
** ----------------------------------------------------------------- */
static boolean writeFont(this, doc)
c_VectorFont *this;
c_DocFile *doc;
{
 char *filename;

 Xc_HISTORY(("writeFont"));

 filename = UntranslateFilename(XcVF_FONTSDIR, this->filename);
 if(!F(doc).writeKeyword(doc, XcDF_FONT_KEYWORD, this->id) ||
    !F(doc).startSequence(doc) ||
    !F(doc).write(doc, this->font_name, strlen(this->font_name)))
  return FALSE;

 /*--- Write font full name ---*/
 if (this->full_name != NULL)
  if (!F(doc).writeKeyword(doc, XcDF_FULLNAME_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).write(doc, this->full_name, strlen(this->full_name)) ||
     !F(doc).endSequence(doc)) return FALSE;

 if (this->aliases != NULL)
 {
  font_alias *alias;

  for(alias = this->aliases; alias != NULL; alias = alias->next)
   if (!F(doc).writeKeyword(doc, XcDF_ALIAS_KEYWORD, -1L) ||
       !F(doc).startSequence(doc) ||
       !F(doc).write(doc, alias->alias_name, strlen(alias->alias_name)) ||
       !F(doc).endSequence(doc)) return FALSE;
 }
 
 /*--- Write font download flag ---*/
 if(this->download_flag != FALSE)
  if(!F(doc).writeKeyword(doc, XcDF_DOWNLOAD_KEYWORD, -1L)) return FALSE;

 /*--- Write font hide flag ---*/
 if(this->hide_flag != FALSE)
  if(!F(doc).writeKeyword(doc, XcDF_HIDE_KEYWORD, -1L)) return FALSE;

 return(F(doc).write(doc, filename, strlen(filename)) &&
	F(doc).endSequence(doc));
}


/* ----------------------------------------------------------------- ** 
** readFontList - VectorFont list loading method                     ** 
** ----------------------------------------------------------------- */
static boolean readFontList(doc, keyword, param, base)
c_DocFile *doc;
char *keyword;
long param;
c_VectorFont **base;
{
 Xc_TRACE(("readFontList"));

 if(!F(doc).addCallbacks(doc, XcDF_FONT_KEYWORD, readFont, base, NULL))
  return FALSE;
 return F(doc).expectKeyword(doc, XcDF_FONT_KEYWORD, TRUE);
}


/* ----------------------------------------------------------------- ** 
** writeFontList - VectorFont list saving method                     ** 
** ----------------------------------------------------------------- */
static boolean writeFontList(this, doc)
c_VectorFont *this;
c_DocFile *doc;
{
 c_VectorFont *ptr;

 Xc_HISTORY(("writeFontList"));
 if(!F(doc).writeKeyword(doc, XcDF_FONTLIST_KEYWORD, -1) ||
    !F(doc).startSequence(doc)) return FALSE;
 for(ptr = *this->base; ptr != NULL; ptr = ptr->next)
  if(!F(ptr).write(ptr, doc)) return FALSE;
 return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** cb_fontname - Fontname loading callback                           ** 
** ----------------------------------------------------------------- */
static boolean cb_fullname(doc, keyword, param, namep)
c_DocFile *doc;
char *keyword;
long param;
unsigned char **namep;
{
 unsigned char *name;

 Xc_TRACE(("cb_fullname"));

 if(!F(doc).readString(doc, &name)) return FALSE;
 if(*namep != NULL) Xc_free(*namep);
 *namep = name;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_family - Family name loading callback                        ** 
** ----------------------------------------------------------------- */
static boolean cb_family(doc, keyword, param, namep)
c_DocFile *doc;
char *keyword;
long param;
unsigned char **namep;
{
 unsigned char *name;

 Xc_TRACE(("cb_family"));

 if(!F(doc).readString(doc, &name)) return FALSE;
 if(*namep != NULL) Xc_free(*namep);
 *namep = name;
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_alias - Alias name loading callback                            ** 
** ----------------------------------------------------------------- */
static boolean cb_alias(doc, keyword, param, aliases)
c_DocFile *doc;
char *keyword;
long param;
font_alias **aliases;
{
 unsigned char *name;
 font_alias *alias;
 Xc_TRACE(("cb_alias"));

 if(!F(doc).readString(doc, &name)) return FALSE;
 
 alias = (font_alias *)Xc_malloc("falias", sizeof(font_alias));
 alias->next = *aliases;
 alias->alias_name = (char *)name;
 *aliases = alias;

 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** cb_flag - Loading callback for all flags                          ** 
** ----------------------------------------------------------------- */
static boolean cb_flag(doc, keyword, param, flag)
c_DocFile *doc;
char *keyword;
long param;
boolean *flag;
{
 Xc_TRACE(("cb_flag(%s)", keyword));

 *flag = TRUE;
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** getFontByName - Get a font from a name                            ** 
** ----------------------------------------------------------------- */
static c_VectorFont *getFontByName(this, name)
c_VectorFont *this;
char *name;
{
 c_VectorFont *base;
 font_alias	*alias;
 base = *this->base;

 /*--- Search for fontname ---*/
 for(this = base; this != NULL; this = this->next)
 {
  if(!strcmp(name, this->name) || !strcmp(name, this->full_name))
   return this;
  for(alias = this->aliases; alias != NULL; alias = alias->next)
   if(!strcmp(name, alias->alias_name))
    return this;
 }
  
 /*--- Search for _fontname ---*/
 while(*name == '_')
  for(this = base, ++name; this != NULL; this = this->next)
   if(!strcmp(name, this->name)) return this;
  
 Xc_TRACE(("`%s' font not found", name));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** getFontByFamilyAndAttribs - Get a font from a family name         ** 
** ----------------------------------------------------------------- */
static c_VectorFont *getFontByFamilyAndAttribs(this, family_name, bold, italic)
c_VectorFont *this;
char *family_name;
boolean bold;
boolean italic;
{
 family_tree *f_tree;
 font_list   *f_list;
 int value, best_value;
 c_VectorFont *best_font;
  
 f_tree = this->family;
 while(f_tree)
 {
  if (Xstrcmp(f_tree->name, family_name))
   break;
  f_tree = f_tree->Next;
 }
 if (!f_tree)
 {			/* if family_name not found take Helvetica instead */
  f_tree = this->family;
  while(f_tree)
  {
   if (Xstrcmp(f_tree->name, "Helvetica"))
    break;
   f_tree = f_tree->Next;
  }
  if (!f_tree) 
   f_tree = this->family; 
 }
  
 best_font = this; best_value = -10;
 f_list = f_tree->list;
 while(f_list)
 {
  value = 0;
  if (f_list->is_normal)
  {
   if (!bold) value++;
   if (!italic) value++;
  }
  if (f_list->is_bold)
  {
   if (bold) value++;
   if (!italic) value++;
  }
  if (f_list->is_italic)
  {
   if (!bold) value++;
   if (italic) value++;
  }
  if (f_list->is_bold_italic)
  {
   if (bold) value++;
   if (italic) value++;
  }
  if (value > best_value)
  {
   best_font = f_list->font;
   best_value = value;
  }
  f_list = f_list->Next;    
 }
 return best_font;
}

/* ----------------------------------------------------------------- ** 
** ps_print - Setup font for printing                                ** 
** ----------------------------------------------------------------- */
static boolean ps_print(this, post)
c_VectorFont *this;
c_PostScript *post;
{
 boolean encode;

 Xc_HISTORY(("ps_print(`%s')", this->font_name));

 encode = (this->encoding == XcFE_LATIN1)? TRUE : FALSE;
 if(!post->ai_mode && this->download_flag && !isBase13(this))
  F(post).includeResource(post, XcPS_FONT_RESOURCE, this);

 if(post->ai_mode)
  return(F(post).putLine(post, "%AI3_BeginEncoding: _$s $s",
			 this->font_name, this->font_name) &&
	 F(post).putLine(post, "[/_$s/$s 0 0 $d FE",
			 this->font_name, this->font_name, encode? 1:0) && 
	 F(post).putLine(post, "%AI3_EndEncoding AdobeType"));
 else
  return F(post).putLine(post, "$s/F$d/$s FE", encode? "XcEncoding":"null",
			 this->id, this->font_name); 
}


/* ----------------------------------------------------------------- ** 
** isBase13 - Test if font is one of base13 fonts                    ** 
** ----------------------------------------------------------------- */
static boolean isBase13(this)
c_VectorFont *this;
{
 if(!strcmp(this->font_name, "Courier") ||
    !strcmp(this->font_name, "Courier-Oblique") ||
    !strcmp(this->font_name, "Courier-Bold") ||
    !strcmp(this->font_name, "Courier-BoldOblique") ||
    !strcmp(this->font_name, "Helvetica") ||
    !strcmp(this->font_name, "Helvetica-Oblique") ||
    !strcmp(this->font_name, "Helvetica-Bold") ||
    !strcmp(this->font_name, "Helvetica-BoldOblique") ||
    !strcmp(this->font_name, "Times-Roman") ||
    !strcmp(this->font_name, "Times-Italic") ||
    !strcmp(this->font_name, "Times-Bold") ||
    !strcmp(this->font_name, "Times-BoldItalic") ||
    !strcmp(this->font_name, "Symbol")) return TRUE;
 return FALSE;
}

/* ----------------------------------------------------------------- ** 
** font_family_filter: design to be use as filter in ItemMenu or     **
** ItemList objects.                                                 ** 
** ----------------------------------------------------------------- */
boolean font_family_filter(family)
family_tree *family;
{
 font_list *list;
 
 list = family->list;
 while(list)
 {
  if (list->font->hide_flag) return FALSE;
  list = list->Next;
 }
 return TRUE;
}
