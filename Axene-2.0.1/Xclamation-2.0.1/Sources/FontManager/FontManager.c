/*
** FontManager.c for Xclamation, XQuad and XAllWrite in FontManager/
** Method for FontManager Class
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
** Started on  Thu Oct 13 17:14:05 1994 Antoine Buat
** Last update Fri Oct  9 16:23:05 1998 Emmanuel Paris
*/

#define NHISTORY
#define NTRACE

#include "FontManager.h"

static void *cons_FontManager();
static void dest_FontManager();
static void *copy_FontManager();
static c_RasterFont *new_font();
static void delete_font();
static void check_too_many_font();

sf_FontManager fc_FontManager =
{
 cons_FontManager,
 dest_FontManager,
 copy_FontManager,
 new_font,
 delete_font,
};

/* ----------------------------------------------------------------- ** 
** construction method                                               ** 
** ----------------------------------------------------------------- */
static void *cons_FontManager(display)
Display	*display;
{
 c_FontManager *This;
  
 Xc_HISTORY(("constructor"));
  
 if((This = (c_FontManager *)Xc_malloc("FontManager",
				       sizeof(c_FontManager))) == NULL)
 {
  Xc_FATAL(("Unable to build Font Manager"));
 }
  
 This->f = &fc_FontManager;
 This->display = display;
  
 This->nb_font_keep = 0;
 This->FntMng = NULL;
  
 Xc_TRACE(("  built"));
 return This;
}

/* ----------------------------------------------------------------- ** 
** destruction method                                                ** 
** ----------------------------------------------------------------- */
static void dest_FontManager(This)
c_FontManager *This;
{
 Xc_HISTORY(("destructor"));
  
 while(This->nb_font_keep)
  delete_font(This, NULL);
  
 Xc_free(This);
 Xc_TRACE(("  destroyed"));
}

/* -------------------------------------------------------------------- ** 
** 				copy method 				**
** -------------------------------------------------------------------- */
static void *copy_FontManager(This)
c_FontManager *This;
{
 c_FontManager *ObjTmp;
  
 if ((ObjTmp = (c_FontManager *)Xc_malloc("CFontManager", 
					  sizeof(c_FontManager))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation Signal"));
 }
 memcpy(ObjTmp, This, sizeof(c_FontManager));
 Xc_TRACE(("Object FontManager copied but not working"));
 return ObjTmp;
}

/* -------------------------------------------------------------------- ** 
** 				new font 				**
** -------------------------------------------------------------------- */
static c_RasterFont *new_font(This, vfont, metrics)
c_FontManager	*This;
c_VectorFont	*vfont;
c_CharMetrics	*metrics;
{
 FontManager_t	*entry, *entry2;
  
 entry = This->FntMng;
 while(entry) 
 {
  Xc_TRACE(("entry: %s [%g] #%d", entry->vfont->font_name,
	    SCALE_TO_POINTS(entry->metrics.point_size), entry->used_time));
    
  if((vfont == entry->vfont) &&
     F(metrics).compare(metrics, &entry->metrics))
  {
   Xc_TRACE(("found ($%lx)", (long)entry->rfont));
      
   entry->used_time++;
   return entry->rfont;
  }
  entry = entry->Next;
 }
  
 Xc_TRACE(("no entry"));
  
 entry = (FontManager_t *)Xc_malloc("entry", sizeof(FontManager_t));
 if(entry == NULL) return NULL;
  
 entry->Next = NULL;
 entry->used_time = 1;
 entry->vfont = vfont;
 F(metrics).fast_copy(metrics, &entry->metrics);
 entry->rfont = (c_RasterFont *)NEW(c_RasterFont)(This->display, 
						  vfont, &entry->metrics);
    
 if(entry->rfont == NULL)
 {
  check_too_many_font(This, TRUE);
  entry->rfont = (c_RasterFont *)NEW(c_RasterFont)
   (This->display, vfont, &(entry->metrics));
      
  if(entry->rfont == NULL)
  {
   Xc_free(entry);
   Xc_TRACE(("out of memory"));
   return NULL;
  }
 }
  
 if ((entry2 = This->FntMng) != NULL)
 {
  while(entry2->Next)
   entry2 = entry2->Next;
  entry2->Next = entry;
 }
 else
  This->FntMng = entry;

 This->nb_font_keep++;
  
 check_too_many_font(This, FALSE);
  
 Xc_TRACE(("new-entry: %s [%g] ($%lx)", entry->vfont->font_name,
	   SCALE_TO_POINTS(entry->metrics.point_size),
	   (long)entry->rfont));
  
 return entry->rfont;
}

/* -------------------------------------------------------------------- ** 
** 				delete font 				**
** -------------------------------------------------------------------- */
static void delete_font(This, rfont)
c_FontManager	*This;
c_RasterFont	*rfont;
{
 FontManager_t	*entry;
 FontManager_t *prev;
  
  
 Xc_HISTORY(("delete($%lx)", (long)rfont));

 if(rfont == NULL)
 {
  entry = This->FntMng;
      
  Xc_TRACE(("entry: %s [%g] ($%lx)", entry->vfont->font_name,
	    SCALE_TO_POINTS(entry->metrics.point_size),
	    (long)entry->rfont));

  DELETE(c_RasterFont)(entry->rfont);
  This->FntMng = entry->Next;
  F(&(entry->metrics)).fast_free(&(entry->metrics));
  Xc_free(entry);
  This->nb_font_keep--;
  return;
 }
  
 entry = prev = This->FntMng;
  
 while(entry)
 {
  Xc_TRACE(("entry: %s [%g] ($%lx)", entry->vfont->font_name,
	    SCALE_TO_POINTS(entry->metrics.point_size), (long)entry->rfont));
    
  if(entry->rfont == rfont)
  {
   Xc_TRACE(("found #%d", entry->used_time));
      
   if((--entry->used_time <= 0) && 
      (This->nb_font_keep > FM_NB_FONT_KEEPED) )
   {
    Xc_TRACE(("remove"));
	
    DELETE(c_RasterFont)(entry->rfont);
	
    if (entry == prev)
     This->FntMng = entry->Next;
    else
     prev->Next = entry->Next;
	
    F(&(entry->metrics)).fast_free(&(entry->metrics));
	
    Xc_free(entry);
    This->nb_font_keep--;
	
   }
   return;
  }
  prev = entry;
  entry = entry->Next;
 }
  
 Xc_TRACE(("Trying to delete an unknow rasterized font"));
}


/* -------------------------------------------------------------------- ** 
** 			check too many font (garbage)         		**
** -------------------------------------------------------------------- */
static void check_too_many_font(This, all)
c_FontManager *This;
boolean	      all;
{
 boolean	found = TRUE;
 FontManager_t	*entry;
 FontManager_t *prev;
 int		max_fnt;
  
 if (all)
  max_fnt = 0;
 else
  max_fnt = FM_NB_FONT_KEEPED;
 if (This->nb_font_keep > max_fnt)
 {
  while(This->nb_font_keep > max_fnt && found)
  {
   entry = prev = This->FntMng;
   found = FALSE;
   while(entry)
   {
    if (entry->used_time <= 0)
     break;
    prev = entry;
    entry = entry->Next;
   }
   if (entry)
   {
    found = TRUE;
    Xc_TRACE(("Too many font, deleting font %s.",
	      entry->rfont->font->font_name));
		
    DELETE(c_RasterFont)(entry->rfont);
	
    if (entry == prev)
     This->FntMng = entry->Next;
    else
     prev->Next = entry->Next;
	
    F(&(entry->metrics)).fast_free(&(entry->metrics));
	
    Xc_free(entry);
    This->nb_font_keep--;
   }
  }
 }
}


