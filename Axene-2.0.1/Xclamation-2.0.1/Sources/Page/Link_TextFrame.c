/*
** Link_TextFrame.c for Xclamation in Page/
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
** Started on  Mon Jul 17 20:20:35 1995 Emmanuel Paris
** Last update Thu May 29 15:42:52 1997 Emmanuel Paris
*/

#include "Link_TextFrame.h"
#include "TextFrame.h"

extern c_TextFrame  *TextFrameForLink;

void init_Link_TextFrame();
status_t eventlook_Link_TextFrame();
void abort_Link_TextFrame();
void redraw_Link_TextFrame();

static void link_TextFrame_source_done();
static void link_TextFrame_target_done();
static int  get_function_and_change_cursor();
static boolean is_cadre_valide();

void init_Link_TextFrame(cpage)
c_Page	*cpage;
{ 
 Window	root;
 Window	fille;
 int		xrac,yrac;
 int		xm,ym;
 unsigned int	mod_button;
 DATA(Link_TextFrame);
  
 SET_DATA(Link_TextFrame);
  
 switch(cpage->function_type)
 {
 case ADD_LINK_AT_BEGINNING:
 case ADD_LINK_AT_END:
  F(CURSOR).push_and_set_cursor(CURSOR, C_LINK_FRAME_SOURCE);
  break;
 case INSERT_LINK_BEFORE:
  F(CURSOR).push_and_set_cursor(CURSOR, C_LINK_BEFORE);
  break;
 case INSERT_LINK_BEHIND:
  F(CURSOR).push_and_set_cursor(CURSOR, C_LINK_AFTER);
  break;
 }
 XQueryPointer(cpage->X_info.display,cpage->X_info.window,&root,&fille,
	       &xrac,&yrac,&xm,&ym,&mod_button);
 d(function) = get_function_and_change_cursor(cpage, xm, ym, TRUE);
}

status_t eventlook_Link_TextFrame(cpage,event)
c_Page	*cpage;
XEvent	event;
{  
 Display	*display;
 Window	window;
 GC		gc;
 l_Cadre	*lcadre;
 int		xm,ym;
 status_t	retour_func;
 DATA(Link_TextFrame);

 GET_DATA(Link_TextFrame);
 display=cpage->X_info.display;
 window=cpage->X_info.window;
 gc=cpage->X_info.gc_contour;

 retour_func=FUNCTION_NOT_FINISHED;
 switch(event.type)
 {
 case MotionNotify:
  xm = event.xmotion.x;
  ym = event.xmotion.y;
  get_function_and_change_cursor(cpage, xm, ym, TRUE);
  break;
 case ButtonRelease:
  xm=event.xbutton.x;
  ym=event.xbutton.y;
  if (event.xbutton.button==1 && 
      d(function)==LT_SELECT_TARGET) 
  {
   lcadre=(l_Cadre *)F(cpage).get_cadre_by_xy(cpage,xm,ym);
   if (lcadre && is_cadre_valide(cpage,lcadre))
   {
    link_TextFrame_target_done(cpage, lcadre);
    retour_func=FUNCTION_FINISHED;
   }
  }
  break;
 case ButtonPress:
  xm=event.xbutton.x;
  ym=event.xbutton.y;
  d(function)=get_function_and_change_cursor(cpage, xm, ym, TRUE);
  if (event.xbutton.button==1)
  {
   lcadre=(l_Cadre *)F(cpage).get_cadre_by_xy(cpage,xm,ym);
   if (lcadre && is_cadre_valide(cpage,lcadre))
   {
    if (d(function)==LT_SELECT_SOURCE)
    {
     link_TextFrame_source_done(cpage, lcadre);
     d(function)=get_function_and_change_cursor(cpage, xm, ym,
						TRUE);
    }
    else
    {
     link_TextFrame_target_done(cpage, lcadre);
     retour_func=FUNCTION_FINISHED;
    }
   }
  }
  else
  {
   if (d(function)==LT_SELECT_TARGET)
   {
    retour_func=FUNCTION_ABORTED;
   }
   else
    retour_func=FUNCTION_REINIT;
  }
  break;
 }
 return retour_func;
}

void abort_Link_TextFrame(cpage)
c_Page	*cpage;
{ 
 Display	*display;
 Window	window;
 GC		gc;
 DATA(Link_TextFrame);
  
 GET_DATA(Link_TextFrame);
 display=cpage->X_info.display;
 window=cpage->X_info.window;
 gc=cpage->X_info.gc_contour;
  
 if (LinkFunction)
 {
  LinkFunction = 0;
  if (Xstrcmp("Filler",TextFrameForLink->text->name))
   F(TextFrameForLink->frame).detachobject(TextFrameForLink->frame);
 }
  
 FREE_DATA;
 F(CURSOR).pop_cursor(CURSOR);
}


void redraw_Link_TextFrame(cpage)
c_Page	*cpage;
{
 Display	*display;
 Window	window;
 GC		gc;
 DATA(Link_TextFrame);
  
 GET_DATA(Link_TextFrame);

 display=cpage->X_info.display;
 window=cpage->X_info.window;
 gc=cpage->X_info.gc_contour;
  
}
static void link_TextFrame_source_done(cpage, lcadre)
c_Page	*cpage;
l_Cadre	*lcadre;
{
 c_Text	*text;
 c_TextFrame	*text_frame;
 DATA(Link_TextFrame);
   
 GET_DATA(Link_TextFrame);

 LinkFunction = cpage->function_type;
 if (lcadre->cadre->object_type == CADRE_EMPTY)
 {
  text = (c_Text *)NEW(c_Text)("Filler",&(cpage->BaseStd->text_base),
			       cpage->BaseStd->text_style_base,
			       cpage->BaseStd->text_ruler_base);
  text_frame = (c_TextFrame *)NEW(c_TextFrame)();
  F(text_frame).attachText(text_frame, text);
  Xc_TRACE(("cadre: %s",lcadre->name));
  F(lcadre->cadre).attachobject(lcadre->cadre,text_frame,CADRE_TEXT);
  TextFrameForLink=text_frame;
 }
 else
  TextFrameForLink = (c_TextFrame *)(lcadre->cadre->object);
  
 Xc_TRACE(("TextFrameForLink full")); 
}


static void link_TextFrame_target_done(cpage, lcadre)
c_Page	*cpage;
l_Cadre	*lcadre;
{
 c_Text	*text;
 c_TextFrame	*text_frame;
 c_TextFrame	*text_frame_src;
 boolean	flag = FALSE;
 DATA(Link_TextFrame);
  
 GET_DATA(Link_TextFrame);

 LinkFunction = 0;
  
 Xc_TRACE(("cadre: %s",lcadre->name));
 text_frame_src = TextFrameForLink;
 switch(cpage->function_type)
 {
 case ADD_LINK_AT_BEGINNING:
  while(text_frame_src->previous)
   text_frame_src = text_frame_src->previous;
  flag = TRUE;
  break;
 case ADD_LINK_AT_END:
  while(text_frame_src->next) 
   text_frame_src = text_frame_src->next;
  flag = FALSE;
  break;
 case INSERT_LINK_BEFORE:
  flag = TRUE;
  break;
 case INSERT_LINK_BEHIND:
  flag = FALSE;
  break;
 }
 if (cpage->function_type == ADD_LINK_AT_BEGINNING ||
     cpage->function_type == ADD_LINK_AT_END)
 {
  text_frame = (c_TextFrame *)NEW(c_TextFrame)();
  F(text_frame).link(text_frame, text_frame_src, flag);
  F(lcadre->cadre).attachobject(lcadre->cadre, text_frame, CADRE_TEXT);
  F(cpage).undisplay_selected_lcadre(cpage);
  F(cpage).draw_cadre(cpage, lcadre);
  F(cpage).display_selected_lcadre(cpage);
 }
 else
 {
  F(text_frame_src).attachText(text_frame_src, NULL);
  text_frame = ((c_TextFrame *)lcadre->cadre->object);
  if (!text_frame)
  {
   text = (c_Text *)NEW(c_Text)("Filler",&(cpage->BaseStd->text_base),
				cpage->BaseStd->text_style_base,
				cpage->BaseStd->text_ruler_base);
   text_frame = (c_TextFrame *)NEW(c_TextFrame)();
   F(text_frame).attachText(text_frame, text);
   Xc_TRACE(("attach new_textframe to cadre: %s",lcadre->name));
   F(lcadre->cadre).attachobject(lcadre->cadre, text_frame, CADRE_TEXT);
  }
  F(text_frame_src).link(text_frame_src, text_frame, flag);
  DISPLAY_CONFIGURE(text_frame_src)(text_frame_src,
				    text_frame_src->frame,
				    text_frame_src->frame->Forme);
  lcadre = get_lcadre_by_cadre(cpage, text_frame_src->frame);
  if (lcadre)
  {
   F(cpage).undisplay_selected_lcadre(cpage);
   F(cpage).draw_cadre(cpage, lcadre);
   F(cpage).display_selected_lcadre(cpage);
  }
 }
 TextFrameForLink = NULL;
  
 FREE_DATA;
 F(CURSOR).pop_cursor(CURSOR);
}

static int get_function_and_change_cursor(cpage, xm, ym, change)
c_Page	*cpage;
int	xm;
int	ym;
boolean	change;
{
 l_Cadre	*lcadre;

 lcadre=(l_Cadre *)F(cpage).get_cadre_by_xy(cpage,xm,ym);
 if (lcadre)
 {
  if (is_cadre_valide(cpage,lcadre))
  {
   if (!LinkFunction)
   {
    if (change)
     switch(cpage->function_type)
     {
     case ADD_LINK_AT_BEGINNING:
     case ADD_LINK_AT_END:
      F(CURSOR).set_cursor(CURSOR, C_LINK_FRAME_SOURCE);
      break;
     case INSERT_LINK_BEFORE:
      F(CURSOR).set_cursor(CURSOR, C_LINK_BEFORE);
      break;
     case INSERT_LINK_BEHIND:
      F(CURSOR).set_cursor(CURSOR, C_LINK_AFTER);
      break;
     }
   }
   else
   {
    if (change)
     switch(cpage->function_type)
     {
     case INSERT_LINK_BEFORE:
     case INSERT_LINK_BEHIND:
      F(CURSOR).set_cursor(CURSOR, C_LINK_FRAME_SOURCE);
      break;
     case ADD_LINK_AT_BEGINNING:
      F(CURSOR).set_cursor(CURSOR, C_LINK_ATBEGIN);
      break;
     case ADD_LINK_AT_END:
      F(CURSOR).set_cursor(CURSOR, C_LINK_ATEND);
      break;
     }
   }
  }
  else
  {
   if (change)
    F(CURSOR).set_cursor(CURSOR, C_LOCK_CADRE);
  }
 }
 else
  if (change)
   F(CURSOR).set_cursor(CURSOR, C_SELECT_CADRE);
  
 if (LinkFunction)
  return LT_SELECT_TARGET;
 else
  return LT_SELECT_SOURCE;
}

static boolean is_cadre_valide(cpage, lcadre)
c_Page	*cpage;
l_Cadre	*lcadre;
{
 if (cpage->function_type == ADD_LINK_AT_BEGINNING ||
     cpage->function_type == ADD_LINK_AT_END)
 {
  if (!LinkFunction)
  {
   if (lcadre->cadre->object_type!=CADRE_EMPTY &&
       lcadre->cadre->object_type!=CADRE_TEXT)
    return FALSE;
   else
    return TRUE;
  }
  else
  {
   if (lcadre->cadre->object_type!=CADRE_EMPTY)
    return FALSE;
   else
    return TRUE;
  }
 }
 else
 {
  if (!LinkFunction)
  {
   if (lcadre->cadre->object_type!=CADRE_EMPTY)
    return FALSE;
   else
    return TRUE;
  }
  else
  {
   if (lcadre == (l_Cadre *)get_lcadre_by_cadre
       (cpage, TextFrameForLink->frame))
    return FALSE;
   if (lcadre->cadre->object_type!=CADRE_EMPTY  &&
       lcadre->cadre->object_type!=CADRE_TEXT)
    return FALSE;
   else
    return TRUE;
  }
 }
}

