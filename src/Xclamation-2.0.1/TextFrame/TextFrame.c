/*
** TextFrame.c for Xclamation in TextFrame/
** Methods for the TextFrame class
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
** Started on  Thu Sep 22 17:39:51 1994 Stéphane Boisson
** Last update Fri Dec 31 16:11:10 1999 Emmanuel Paris
*/

#define NHISTORY
#define NTRACE

#include "TextFrameP.h"
#include "PostScript.h"
#include <string.h>

/*--- Methods prototypes ---*/
static void *constructor();
static void destructor();
static void *copy();
static boolean unlinkText ___PROTO((c_TextFrame *this));
static boolean linkText ___PROTO((c_TextFrame *this,
				  c_TextFrame *anchor,
				  boolean before_flag));
static error changeBase ___NPROTO((c_TextFrame *this, BaseStd_t *base));
static error display_init ___NPROTO((c_TextFrame *this,
				    c_Cadre *cadre));

static boolean writeTextFrame ___NPROTO((c_TextFrame *this, c_DocFile *doc));
static boolean attachText ___PROTO((c_TextFrame *this, c_Text *text));
void hook_text ___PROTO((c_TextFrame *this, int reason,
				c_Text *old, c_Text *new));

/*--- Extern prototypes ---*/
boolean importText ___PROTO((c_TextFrame *this, BaseStd_t *base,
			     char *filename));
boolean importASCII ___PROTO((c_TextFrame *this, BaseStd_t *base,
			      char *filename,
			      c_TextStyle *style, c_TextRuler *ruler));

/* extern boolean importXMW ___PROTO((c_TextFrame *this, c_Document *doc,
				    char *filename)); */
extern boolean readTextFrame ___PROTO((c_DocFile *doc, char *keyword,
				       long param, c_Cadre *frame));

sf_TextFrame fc_TextFrame =
{
 constructor, destructor, copy,
 changeBase, display_init, tf_Redraw, tf_Configure, tf_Print,
 readTextFrame,
 writeTextFrame,
 linkText, unlinkText, attachText, 
 importText,
 importASCII			/* , importXMW */
};

static ItemHandle *i_handle = NULL;
static long i_references = 0;


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor()
{
 c_TextFrame *p;

 Xc_HISTORY(("constructor"));

 if(i_handle == NULL)
  i_handle = Xc_createItem("TextFrame base", 16, sizeof(c_TextFrame));
 if(i_handle == NULL) return NULL;

 if((p = Xc_new(i_handle)) == NULL) return NULL;
 memset(p, 0, sizeof(c_TextFrame));
 p->f = &fc_TextFrame;
 i_references++;

 p->text = NULL;
 p->frame = NULL;
 p->lines = NULL;
 p->line_count = 0;
 p->start_paragraph_flag = TRUE;
 p->end_paragraph_flag = TRUE;
 p->start_pos = 0;
 p->factor = 0;
 p->angle = 0;
 p->drawn_flag = FALSE;
 p->sequence = 0;
 
 p->next = NULL;
 p->previous = NULL;

 return p;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_TextFrame *this;
{
 Xc_HISTORY(("destructor(%lx)", (long)this));
 Xc_ASSERT(this != NULL);
 Xc_ASSERT(i_references > 0);

 F(this).unlink(this);
 if(this->text != NULL) F(this).attachText(this, NULL);

 Xc_delete(i_handle, this);
 if(--i_references == 0)
 {
  Xc_destroyItem(i_handle);
  i_handle = NULL;
 }
 Xc_TRACE(("destroyed"));
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this, base)
c_TextFrame *this;
BaseStd_t *base;
{
 c_TextFrame *new;

 Xc_HISTORY(("copy"));
   
 if((new = NEW(c_TextFrame)()) == NULL) return NULL;

 /*--- Copy text ---*/
 if(this->text != NULL)
 {
  c_Text *text;

  if((text = COPY(c_Text)(this->text, base)) == NULL) return NULL;
  if(!attachText(new, text))
  {
   DELETE(c_Text)(text);
   return NULL;
  }
 }

 return new;
}


/* ----------------------------------------------------------------- ** 
** changeBase - BaseStd change                                       ** 
** ----------------------------------------------------------------- */
static error changeBase(this, base)
c_TextFrame *this;
BaseStd_t *base;
{
 Xc_TRACE(("changeBase"));

 this->frame = NULL;
 if(this->text != NULL)
 {
  if((this->previous == NULL) && (this->next == NULL))
  {
   /*--- The TextFrame is not a chain link ---*/
	
   return F(this->text).changeBase(this->text, base);
  }
  else
  {
   c_Text *text;

   /*--- The TextFrame is a chain link ---*/
   if(((text = COPY(c_Text)(this->text, base)) != NULL) &&
      F(this).unlink(this))
   {
    this->start_pos = this->end_pos = 0;
    this->start_paragraph_flag = this->end_paragraph_flag = TRUE;
    if(attachText(this, text)) return XC_NO_ERROR;
   }

   if(text != NULL) DELETE(c_Text)(text);
   return XC_MEMORY_ALLOCATION;
  }
 }
 return XC_NO_ERROR;
}


/* ----------------------------------------------------------------- ** 
** linkText - Link frame with each others                            ** 
** ----------------------------------------------------------------- */
static boolean linkText(this, anchor, before_flag)
c_TextFrame *this;
c_TextFrame *anchor;
boolean before_flag;
{
 c_TextFrame *previous, *next;

 Xc_HISTORY(("link %lx %s %lx", (long)this,
	     (before_flag? "before" : "after"), (long)anchor));
 Xc_ASSERT(anchor != NULL);
 Xc_ASSERT((this->previous == NULL) && (this->next == NULL));
 Xc_ASSERT(this->text == NULL);
 Xc_ASSERT(this->drawn_flag == FALSE);

 if(before_flag == FALSE)
 {
  previous = anchor;
  next = anchor->next;
 }
 else
 {
  previous = anchor->previous;
  next = anchor;
 }

 /*--- Insert in linked list ---*/
 if((this->previous = previous) != NULL) previous->next = this;
 if((this->next = next) != NULL) next->previous = this;

 this->text = anchor->text;
 
 if(previous != NULL)
 {
  this->start_pos = previous->end_pos;
  this->start_paragraph_flag = previous->end_paragraph_flag;
 }
 else
 {
  this->start_pos = this->next->start_pos;
  this->start_paragraph_flag = this->next->start_paragraph_flag;
 }

 if(this->text != NULL)
 {
  if(this->previous == NULL)
  {
   Xc_ASSERT(anchor->previous == this);
   XcH_UNREGISTER(this->text, hook_text, anchor);
   XcH_REGISTER(this->text, hook_text, this);
  }
  if(this->frame != NULL)
  {
   tf_Configure(this, this->frame, this->frame->Forme);
   
   for( ; this != NULL; this = this->next)
   {	/* test if one frame of the text is viewable */
    register c_Cadre *frame;
    
    frame = this->frame;
    if (frame->X_info_calc)		
    {	/* one viewable frame found */
     frame->data_to_object->redraw_cadre(frame->data_to_object->page,
					 frame);
     /* this call tf_Redraw that propagate the redraw to all viewable frame */
     break;
    }
   }
  }
 }
 Xc_TRACE(("end link"));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** unlinkText - Unlink a frame                                       ** 
** ----------------------------------------------------------------- */
static boolean unlinkText(this)
c_TextFrame *this;
{
 c_TextFrame *ptr;

 Xc_HISTORY(("unlink(%lx)", (long)this));

 /*--- Remove from the linked list ---*/
 if(this->next != NULL)
 {

  if((this->previous == NULL) && (this->text != NULL))
  {
   Xc_TRACE(("unlinking first node"));
   Xc_ASSERT(this->text == this->next->text);

   XcH_UNREGISTER(this->text, hook_text, this);
   if(XcH_REGISTER(this->text, hook_text, this->next) == FALSE)
    return FALSE;
  }

  this->next->previous = this->previous;
  Xc_TRACE(("start=%ld flag=%s", this->start_pos,
	    this->start_paragraph_flag? "True":"False"));
  this->next->start_pos = this->start_pos;
  this->next->start_paragraph_flag = this->start_paragraph_flag;
 }
 else if(this->previous == NULL)
 {
  Xc_TRACE(("end unlink"));
  return TRUE;
 }

 ptr = this->next;
 
 if(this->previous != NULL) this->previous->next = this->next;
 this->next = this->previous = NULL;

 if((this->text != NULL) && (this->drawn_flag != FALSE))
  tf_releaseResources(this);

 this->text = NULL;
 this->drawn_flag = FALSE;
 if(this->lines != NULL) Xc_free(this->lines);

 if((ptr != NULL) && (ptr->frame != NULL))
 {
  tf_Configure(ptr, ptr->frame, ptr->frame->Forme);
  
  for( ; ptr != NULL; ptr = ptr->next)
  {	/* test if one frame of the text is viewable */
    register c_Cadre *frame;
    
    frame = ptr->frame;
    if (frame->X_info_calc)		
    {	/* one viewable frame found */
     frame->data_to_object->redraw_cadre(frame->data_to_object->page,
					 frame);
     /* this call tf_Redraw that propagate the redraw to all viewable frame */
     break;
    }
   }
 }
 Xc_TRACE(("end unlink"));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** display_init - (re-)initialisation of the scale factor            ** 
** ----------------------------------------------------------------- */
static error display_init(this, frame)
c_TextFrame *this;
c_Cadre *frame;
{
 Xc_HISTORY(("display_init"));

 this->frame = frame;
 if(this->text == NULL)
 {
  Xc_TRACE(("no text!"));
  return XC_EMPTY_FRAME;
 }

 if(this->drawn_flag != FALSE) tf_releaseResources(this);
 this->angle = frame->sAngle;
 this->factor = SCALE_FROM_COEF((float)COORD_SCALEVALUE / frame->sScale);

 Xc_TRACE(("display_init done"));
 return XC_NO_ERROR;
}


/* ----------------------------------------------------------------- ** 
** writeTextFrame - Save TextFrame                                   ** 
** ----------------------------------------------------------------- */
static boolean writeTextFrame(this, doc)
c_TextFrame *this;
c_DocFile *doc;
{
 Xc_HISTORY(("write"));

 if(this->text == NULL)
 {
  Xc_WARNING(("Linked textframe without text not saved!"));
  return TRUE;
 }
 return(F(doc).writeKeyword(doc, XcDF_TEXT_KEYWORD, this->text->id) &&
	F(doc).startSequence(doc) &&
	F(doc).writeKeyword(doc, XcDF_LINK_KEYWORD, this->id) &&
	F(doc).endSequence(doc));
}


/* ----------------------------------------------------------------- ** 
** attachText - Set or remove a text for a text frame                ** 
** ----------------------------------------------------------------- */
static boolean attachText(this, text)
c_TextFrame *this;
c_Text *text;
{
 Xc_HISTORY(("attachText(`%s')", text? text->name : "nil"));

 if(text == this->text) return TRUE;

 /*--- Rewind chain ---*/
 while(this->previous != NULL) this = this->previous;

 if((text == NULL) || (this->text != NULL))
 {
  c_TextFrame *ptr;

  if(this->text == NULL) return TRUE;
  Xc_TRACE(("removing text"));

  XcH_UNREGISTER(this->text, hook_text, this);

  for(ptr = this; ptr != NULL; ptr = ptr->next)
   if(ptr->drawn_flag != FALSE) tf_releaseResources(ptr);

  if(XcH_USAGE(this->text) == 0) DELETE(c_Text)(this->text);
    
  for(ptr = this; ptr != NULL; ptr = ptr->next)
  {
   ptr->text = NULL;
   ptr->drawn_flag = FALSE;
   if(ptr->lines != NULL) Xc_free(ptr->lines);
   ptr->line_count = 0;
   ptr->lines = NULL;
  }

  if(text == NULL)
  {
   Xc_TRACE(("removed"));
   return TRUE;
  }
 }
  
 /*--- Add text ---*/
 Xc_TRACE(("add text"));
 if (XcH_REGISTER(text, hook_text, this) == FALSE)
  return FALSE;
 else
 {
  c_TextFrame *ptr;

  for(ptr = this; ptr != NULL; ptr = ptr->next)
  {
   ptr->text = text;
   ptr->start_paragraph_flag = TRUE;
   ptr->start_pos = 0;
  }
 }
 if(this->frame != NULL)
 {
  tf_Configure(this, this->frame, this->frame->Forme);

  for( ; this != NULL; this = this->next)
  {	/* test if one frame of the text is viewable */
   register c_Cadre *frame;
   
   frame = this->frame;
   if (frame->X_info_calc)		
   {	/* one viewable frame found */
    frame->data_to_object->redraw_cadre(frame->data_to_object->page,
					frame);
    /* this call tf_Redraw that propagate the redraw to all viewable frame */
    break;
   }
  }
 }
 Xc_TRACE(("attached"));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** hook_text - Text hook callback                                    ** 
** ----------------------------------------------------------------- */
void hook_text(this, reason, old, new)
c_TextFrame *this;
int reason;
c_Text *old;
c_Text *new;
{
 Xc_TRACE(("hook_text(%lx, %lx, %d)", (long)this, (long)old, reason));
 Xc_ASSERT((this->text == old) && (this->previous == NULL));

 if(reason == XcH_SAVE_PREP)
 {
  /*--- Saving preparation ---*/
  unsigned long id;

  for(id = 0; this != NULL; this = this->next)
   this->id = id++;
 }
 else if(reason == XcH_REDRAW)
 {
  /*--- Redraw chain ---*/
  Xc_ASSERT(this->previous == NULL);

  for( ; this != NULL; this = this->next)
  {	/* test if one frame of the text is viewable */
   register c_Cadre *frame;

   frame = this->frame;
   if(frame->X_info_calc)
   {	/* one viewable frame found */
    frame->data_to_object->redraw_cadre
     (frame->data_to_object->page, frame);
    /* this call tf_Redraw that propagate the redraw to all viewable frame */
    break;
   }
  }
 }
 else if(this->frame != NULL)
 {
  c_TextFrame *ptr;
  int i;

  /* hack to make sure that all linked textframe will be configured */
  for( ptr = this->next, i = -1; ptr != NULL; ptr = ptr->next, i--)
   ptr->start_pos = i;

  tf_Configure(this, this->frame, this->frame->Forme);
 }
}


/* ----------------------------------------------------------------- ** 
** releaseResources - Release text resources                         ** 
** ----------------------------------------------------------------- */
boolean tf_releaseResources(this)
c_TextFrame *this;
{
 text_reference_t *ref;

 Xc_TRACE(("releaseResources"));

 ref = &this->text->ref_base[0];
 while(ref != NULL)
 {
  if(ref->type == XcT_TEXT_STYLE)
  {
   c_CharMetrics *cm;
	  
   /*F(ref->data.style->font).dump(ref->data.style->font);*/
   if((cm = ref->data.style->metrics) == NULL) continue;
   cm = F(cm).findTransform(cm, this->factor,
			    XcTS_GET_WIDTH(ref->data.style), this->angle);
   if((cm != NULL) && F(cm).isLocked(cm, this)) F(cm).unlock(cm, this);
	  
   /*	  F(cm).purgeImpure(cm, this->factor,
	  ref->data.style->width.scale, this->angle);*/
	  
  }
  ref = ref->next;
 }
 Xc_TRACE(("release ok"));
 return TRUE;
}
