/*
** docfile.c for XAllWrite in FrameManager/
** Loading stuff for Page class
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
** Started on  Thu Jan  5 15:51:31 1995 Stéphane Boisson
** Last update Sat Apr 11 16:18:22 1998 Emmanuel Paris
*/

#include "FrameManagerP.h"
#include "Page.h"

/*--- Local functions ---*/
static boolean cb_frame ___PROTO((c_DocFile *doc, char *keyword,
				  long param, c_Page *page));
static boolean cb_bitfield ___PROTO((c_DocFile *doc, char *keyword,
				     long param, c_FrMg *this));

/* ----------------------------------------------------------------- ** 
** writeFrMg - Write frame list                                      ** 
** ----------------------------------------------------------------- */
boolean writeFrMg(this, doc)
c_FrMg *this;
c_DocFile *doc;
{
 l_Cadre *ptr, *tmp;
 long	 bitfield;
 Xc_HISTORY(("write"));

 if(this->ListCadre == NULL &&  this->Frame_Border_actif == TRUE &&
    this->magnetic == FALSE) return TRUE;
  
 if(!F(doc).writeKeyword(doc, XcDF_FRAMELIST_KEYWORD, -1L) ||
    !F(doc).startSequence(doc)) return FALSE;
  
 bitfield = this->Frame_Border_actif ? 1 : 0;
 bitfield <<= 1; bitfield |= this->magnetic ? 1 : 0;
 if(!F(doc).writeKeyword(doc, XcDF_BITFIELD_KEYWORD, bitfield))
  return FALSE;
  
 if(this->ListCadre != NULL)
 {
  if((tmp = sort_asc_list_cadre(this->ListCadre)) == NULL) return FALSE;
    
  for(ptr = tmp; ptr != NULL; ptr = ptr->NextCadre)
   if(!F(ptr->cadre).write(ptr->cadre, doc, 0, 0))
    return FALSE;
    
  free_list_cadre(tmp, NO_DEL_CADRE);
 }
  
 Xc_TRACE(("done"));
 return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** readFrMg - Read frame list                                        ** 
** ----------------------------------------------------------------- */
boolean readFrMg(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
void *this;
{
 Xc_TRACE(("read"));

 if(!F(doc).addCallbacks(doc, 
			 XcDF_BITFIELD_KEYWORD, cb_bitfield, 
			 ((c_Page *)this)->frmg,
			 XcDF_FRAME_KEYWORD, cb_frame, this, NULL))
  return FALSE;

 return F(doc).expectKeyword(doc, NULL, TRUE);
}


/* ----------------------------------------------------------------- ** 
** cb_frame - Loading callback for frame loading                     ** 
** ----------------------------------------------------------------- */
static boolean cb_frame(doc, keyword, param, page)
c_DocFile *doc;
char *keyword;
long param;
c_Page *page;
{
 c_Cadre *frame;
 l_Cadre *lframe;
 c_FrMg *this;
 error rc;

 Xc_TRACE(("cb_frame"));

 if(FUNCTION(c_Cadre, read)(doc, keyword,
			    page->sFactor, page->BaseStd) == FALSE)
  return FALSE;

 Xc_ASSERT(doc->data_type == XcDF_CADRE_TYPE);
 doc->data_type = XcDF_NONE;
 
 this = page->frmg;
 frame = (c_Cadre *)doc->data_ptr;

 frame->Forme->Orig_X = frame->sOrig_X / this->sScale;
 frame->Forme->Orig_Y = frame->sOrig_Y / this->sScale;

 F(frame).calculorigine(frame);
 F(frame).calculscaling(frame);
 F(frame).calculforme(frame, CONTOUR);

 /*--- Init VectorGraph object ---*/
 if(frame->object_type == CADRE_VECTOR)
 {
/*  c_HighGraph *graph;

  Xc_TRACE(("Init VectorGraph object"));
  if((graph = ((c_VectorGraph *)frame->object)->highg) != NULL)
   F(graph).init(graph, page->moteur);
  Xc_TRACE(("init ok")); */
 }

 Xc_TRACE(("Add frame"));
 rc = F(this).add_cadre(this, frame->name,
			F(this).get_highest_priority(this), frame);
  
 if (rc != XC_NO_ERROR)
 {
  char *name2;
    
  name2 = F(this).copy_cadre_name(this, frame->name);
  rc = F(this).add_cadre(this, name2, 
			 F(this).get_highest_priority(this), frame);
  return FALSE;
    
 }
  
 if (frame->selected)		/* if frame selected */
 {
  lframe = this->ListCadre;
  if (!lframe) return FALSE;
    
  while(lframe->NextCadre)	/* find last frame of list */
   lframe = lframe->NextCadre;
    
  frame->selected = FALSE;
  F(this).select_cadre(this, lframe, FALSE);
    
  if (frame->verrou) this->verrou = TRUE;
 }
  
 Xc_TRACE(("add ok"));
 return((rc == XC_NO_ERROR)? TRUE : FALSE);
}

/* ----------------------------------------------------------------- ** 
** cb_bitfield - Loading callback for page bitfield loading          ** 
** ----------------------------------------------------------------- */
static boolean cb_bitfield(doc, keyword, param, this)
c_DocFile *doc;
char *keyword;
long param;
c_FrMg *this;
{
 Xc_TRACE(("cb_bitfield(`0x%x')", param));
  
 this->magnetic = (param & 1) ? TRUE : FALSE; param >>= 1;
 this->Frame_Border_actif = (param & 1) ? TRUE : FALSE;
  
 return TRUE;
}
