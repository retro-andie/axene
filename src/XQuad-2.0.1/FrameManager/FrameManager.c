/*
** FrameManager.c for XQuad in FrameManager/
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
** Started on  Mon Jul 17 19:58:07 1995 Emmanuel Paris
** Last update Wed Aug  5 17:57:43 1998 Emmanuel Paris
*/

#include "FrameManagerP.h"

void *cons_FrMg();
void dest_FrMg();
void *copy_FrMg();

void resize_frmg();
boolean set_function();
status_t eventlook_frmg();
void set_X_info();
error add_Cadre();
void delete_Cadre();
void list_Cadre();
void destroy_cadre();
void redisplay_cadre();

char	*copy_cadre_name();
l_Cadre *get_new_cadre();
l_Cadre *get_copy_cadre();
l_Cadre *get_cadre_by_xy();
l_Cadre *get_cadre_by_name();
l_Cadre *get_cadre_and_point_by_xy();
l_Cadre *get_cadre_and_point_by_line();
l_Point *get_handle_by_xy();

void calcul_select_handle();
void change_frmg_origin();
void change_frmg_scaling();
void init_recoordinate_point();
void recoordinate_point();
void attract_point();
void move_cadre_to_xy();
void redraw_rectangle_frmg();
void draw_cadre_contour();
void draw_cadre();
static void start_draw_cache ___PROTO((c_FrMg *This));
static void add_to_draw_cache ___PROTO((c_FrMg *This, l_Cadre *lcadre, 
					Polygone *polygone));
static void flush_draw_cache ___PROTO((c_FrMg *This));	

void select_cadre();
void deselect_cadre();
void select_all_cadre();
void deselect_all_cadre();

void RectangleDisplaySelectedlCadre();
void DisplaySelectedlCadre();
void UndisplaySelectedlCadre();
void DisplaySelectedCadre();
void UndisplaySelectedCadre();
void draw_handle();

void free_list_cadre();
void get_center_list_cadre();
void get_limit_list_cadre();
void add_cadre_in_list();
void delete_cadre_in_list();
l_Cadre *duplicate_list_cadre();
l_Cadre *list_cadre_inters();
void list_cadre_low_high_priority();
l_Cadre *get_lcadre_by_cadre();

static boolean get_bbox_list_cadre ___PROTO((c_FrMg *This, bbox_t *bboxp,
					     l_Cadre *lcadre));

void  object_redraw_cadre();
void  object_redraw_rectangle();
Polygone *object_get_clip_polygone();

sf_FrMg fc_FrMg =
{
 cons_FrMg,
 dest_FrMg,
 copy_FrMg,
 resize_frmg,
 set_function,
 eventlook_frmg,
 set_X_info,
 add_Cadre,
 delete_Cadre,
 list_Cadre,
 destroy_cadre,
 redisplay_cadre,
 set_priority_Cadre,
 get_priority_Cadre,
 lower_priority,
 higher_priority,
 lowest_priority,
 highest_priority,
 delete_selected_cadre,
 delete_selected_object,
 set_lower_priority,
 set_higher_priority,
 set_lowest_priority,
 set_highest_priority,
 get_highest_priority,
 get_lowest_priority,
 lock_cadre,
 unlock_cadre,
 copy_cadre_name,
 get_new_cadre,
 get_copy_cadre,
 get_cadre_by_xy,
 get_cadre_by_name,
 get_cadre_and_point_by_xy,
 get_cadre_and_point_by_line,
 get_handle_by_xy,
 free_list_cadre,
 duplicate_list_cadre,
 get_center_list_cadre,
 calcul_select_handle,
 change_frmg_origin,
 change_frmg_scaling,
 init_recoordinate_point,
 recoordinate_point,
 attract_point,
 move_cadre_to_xy,
 redraw_rectangle_frmg,
 draw_cadre_contour,
 draw_cadre,
 start_draw_cache,
 flush_draw_cache,
 select_cadre,
 deselect_cadre,
 select_all_cadre,
 deselect_all_cadre,
 DisplaySelectedlCadre,
 UndisplaySelectedlCadre,
 get_bbox_list_cadre,
 printFrameList,
 readFrMg,
 writeFrMg,
 RectangleDisplaySelectedlCadre
};

void *cons_FrMg(BaseStd, callback_data, select_cadre_callback, 
		deselect_cadre_callback, redraw_polygone_callback, 
		modify_cadre_callback, attract_point_callback)
BaseStd_t	*BaseStd;
void		*callback_data;
void		(*select_cadre_callback)();
void		(*deselect_cadre_callback)();
void		(*redraw_polygone_callback)();
void		(*modify_cadre_callback)();
void		(*attract_point_callback)();
{
 c_FrMg *This;

 if ( (This=(c_FrMg *)Xc_malloc("FrMg", sizeof(c_FrMg))) == NULL)
  return NULL;
 memset(This, 0, sizeof(c_FrMg));
 This->f= &fc_FrMg;
  
 This->X_info_calc = FALSE;
 This->BaseStd = BaseStd;
 This->sScale = COORD_DEFAULT;
 This->sFactor = SCALE_FROM_PERCENT(100);
 This->sOrig_X = This->sOrig_Y=0;
 This->Orig_X = This->Orig_Y=0;
 This->PosX = This->PosY = This->Width = This->Height = 0;
 This->magnetic = FALSE;
 This->Frame_Border_actif = TRUE;
 This->verrou = FALSE;
 This->select_type = FALSE;
 This->select_group = TRUE;
 This->select_sX_min = This->select_sY_min =
  This->select_sX_max = This->select_sY_max = 0;
 This->SelectedCadre = NULL; 
 This->ListCadre = NULL;
 This->nb_cadre = 0;
 This->function = NULL;
 This->function_data = NULL;
 This->data_to_object.page = (void *)This;
 This->data_to_object.redraw_cadre = object_redraw_cadre;
 This->data_to_object.redraw_rectangle = object_redraw_rectangle;
 This->data_to_object.get_clip_polygone = object_get_clip_polygone;
 This->redraw_cache_actif = FALSE;
 This->redraw_cache = NULL;
 This->callback_data = callback_data;
 This->select_cadre_callback = select_cadre_callback;
 This->deselect_cadre_callback = deselect_cadre_callback;
 This->redraw_polygone_callback = redraw_polygone_callback;
 This->modify_cadre_callback = modify_cadre_callback;
 This->attract_point_callback = attract_point_callback;
  
 Xc_TRACE(("done"));
 return This;
}

void dest_FrMg(This)
c_FrMg *This;
{
 l_Cadre *lcadre;
 Xc_HISTORY(("destructor"));

 F(This).deselect_all_cadre(This, TRUE);
 if(This->X_info_calc)
 {
  XFreeGC(This->X_info.display, This->X_info.gc_contour);
  XFreeGC(This->X_info.display, This->X_info.gc_inside);
  This->X_info_calc = FALSE;
 }

 lcadre = This->ListCadre;
 This->ListCadre = NULL;
 This->nb_cadre = 0;
 free_list_cadre(lcadre, DEL_CADRE);
  
 Xc_free(This);
 Xc_TRACE(("destroyed"));
}

void *copy_FrMg(ObjectToCopy)
c_FrMg *ObjectToCopy;
{
 c_FrMg *ObjTmp;

 if ((ObjTmp=(c_FrMg *)Xc_malloc("FrMg",sizeof (c_FrMg)))==NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp,ObjectToCopy,sizeof (c_FrMg));
 Xc_TRACE(("Object FrMg copied"));
 return ObjTmp;
}

/* --------------------------------------------------------------------- */
/* ------------------  Direct connected functions  --------------------- */
/* --------------------------------------------------------------------- */


void resize_frmg(This,posx,posy,new_width,new_height)
c_FrMg	*This;
int	posx;
int	posy;
int	new_width;
int	new_height;
{
 This->PosX = posx;
 This->PosY = posy;
 This->Width = new_width;
 This->Height = new_height;
}

boolean set_function(This,num_function)
c_FrMg	*This;
int	num_function;
{
 l_Cadre	*lcadre;
 boolean	ret_code = FALSE;
  
 switch(num_function)
 {
 case SELECT_MOVE_RESIZE_CADRE:
  /*    This->function = &fc_Select_Move_Resize_Cadre;
	if (This->select_type)
	This->function_type=MOVE_POINT_MODE;
	else
	This->function_type=RESIZE_CADRE_MODE; */
  break;
 case CREATE_CADRE_RECTANGLE:
  This->function = &fc_Create_Cadre_Rectangle;
  This->function_type=num_function;
  break;
 case DELETE_SELECTED_CADRE:
  if (!This->verrou && This->SelectedCadre)
  {
   F(This).delete_selected_cadre(This);
   ret_code = TRUE;
  }
  break;
 case SET_LOWER_PRIORITY:
  if (!This->verrou && This->SelectedCadre)
  {
   F(This).set_lower_priority(This);
   ret_code = TRUE;
  }
  break;
 case SET_HIGHER_PRIORITY:
  if (!This->verrou && This->SelectedCadre)
  {
   F(This).set_higher_priority(This);
   ret_code = TRUE;
  }
  break;
 case SET_LOWEST_PRIORITY:
  if (!This->verrou && This->SelectedCadre)
  {
   F(This).set_lowest_priority(This);
   ret_code = TRUE;
  }
  break;
 case SET_HIGHEST_PRIORITY:
  if (!This->verrou && This->SelectedCadre)
  {
   F(This).set_highest_priority(This);
   ret_code = TRUE;
  }
  break;
 case SELECT_ALL_CADRE:
  F(This).deselect_all_cadre(This,TRUE);
  F(This).select_all_cadre(This);
  lcadre=This->SelectedCadre;
  while(lcadre)
  {
   This->verrou|=lcadre->cadre->verrou;
   lcadre=lcadre->NextCadre;
  }
  break;
 case LOCK_CADRE:
  F(This).lock_cadre(This);
  ret_code = TRUE;
  break;
 case UNLOCK_CADRE:
  F(This).unlock_cadre(This);
  ret_code = TRUE;
  break;
 case DELETE_OBJECT:
  if (!This->verrou && This->SelectedCadre)
  {
   F(This).delete_selected_object(This);
   ret_code = TRUE;
  }
  break;
 case DISPLAY_FRAME_BORDER:
  This->Frame_Border_actif ^= TRUE;
  ret_code = TRUE;
  break;
 case MAGNETIZE_ALL:
  This->magnetic ^= TRUE;
  ret_code = TRUE;
  break;
 default:  
  break;
 }
  
 return ret_code;
}

status_t eventlook_frmg(This, event)
c_FrMg	*This;
XEvent	*event;
{
 XEvent		none;
 status_t	status;
 KeySym        symb;
 
 status=FUNCTION_NOT_FINISHED;
 if (event->xany.window == This->X_info.window ||
     event->type == KeyPress || event->type == KeyRelease)
 {
  switch(event->type)
  {
  case Expose:
  case GraphicsExpose:
   Xc_TRACE(("%s rect x:%d y:%d <-> x:%d y:%d",
	     (event->type==Expose?"Expose":"Gfx Expose"),		
	     event->xexpose.x,event->xexpose.y,
	     event->xexpose.x+event->xexpose.width-1,
	     event->xexpose.y+event->xexpose.height-1));
   F(This).redraw_rectangle(This, event->xexpose.x, event->xexpose.y,
			    event->xexpose.x+event->xexpose.width-1,
			    event->xexpose.y+event->xexpose.height-1); 
   break;
  case KeyPress:
   if (XEventsQueued(_display, QueuedAfterReading))
   {
    if (XCheckTypedEvent(_display, KeyPress, &none))
     return status;
   }
   symb = XKeycodeToKeysym(event->xany.display,
			   event->xkey.keycode, 0);
	
   switch (symb)
   { 
   case XK_BackSpace:
    if (set_function(This, DELETE_OBJECT))
    {
     event->type=LASTEvent;
     return FUNCTION_MODIFIED;
    }
    break;
   case XK_Delete:
    if (set_function(This, DELETE_SELECTED_CADRE))
    {
     event->type=LASTEvent;
     return FUNCTION_MODIFIED;
    }
    break;
   case XK_Undo:
   case XK_Escape:
    return FUNCTION_ABORTED;
    break;
   }
   break;
  default:
   status = EVENTLOOK(This, *event);
   break;
  }
 }
 return status;
}

void set_X_info(This, display, window, cursor)
c_FrMg		*This;
Display		*display;
Window		window;
c_Cursor	*cursor;
{
 GC	gc;
 XGCValues	val_gc;
 l_Cadre	*lcadre;
  
 if (!This->X_info_calc)
 {
  This->X_info.display=display;
  This->X_info.window=window;
  This->X_info.ecran=DefaultScreen(display);
  This->cursor = cursor;
    
  if (GlobColormap->Cblack)
   val_gc.function=GXxor;
  else
   val_gc.function=GXinvert;
  val_gc.plane_mask= -1;
  val_gc.background=GlobColormap->Cwhite;
  val_gc.foreground=GlobColormap->Cblack;
  val_gc.line_width=0;
  val_gc.line_style=LineSolid;
  val_gc.cap_style=CapNotLast;
  val_gc.join_style=JoinMiter;
  gc=XCreateGC(display,window,GCFunction|GCPlaneMask|GCForeground|
	       GCBackground|GCLineWidth|GCLineStyle|GCCapStyle|
	       GCJoinStyle,&val_gc);
  This->X_info.gc_contour=gc;
  
  val_gc.function=GXcopy;
  val_gc.cap_style=CapButt;
  val_gc.foreground=GlobColormap->Cwhite;
  val_gc.background=GlobColormap->Cblack;
  val_gc.graphics_exposures=True;
  gc=XCreateGC(display,window,GCFunction|GCForeground|GCBackground|
	       GCLineWidth|GCLineStyle|GCCapStyle|GCJoinStyle|
	       GCGraphicsExposures,&val_gc);
  This->X_info.gc_inside=gc;
  This->X_info_calc=TRUE;
 }
 lcadre=This->ListCadre;
 while(lcadre)
 {
  F(lcadre->cadre).set_X_info(lcadre->cadre,display,window);
  lcadre=lcadre->NextCadre;
 }
}

error add_Cadre(This, name, priority, frame)
c_FrMg 	*This;
char	*name;
int	priority;
c_Cadre	*frame;
{
 l_Cadre *lcadre, **link;

 Xc_HISTORY(("Adding frame `%s' to Frame Manager", name));

 for(link = &This->ListCadre; *link != NULL; link = &((*link)->NextCadre))
  if(Xstrcmp(name, (*link)->name))
  {
   Xc_TRACE(("Cadre name already exists"));
   return XC_NAME_ALREADY_EXIST;
  }

 if((lcadre = Xc_malloc("Frame in list", sizeof(l_Cadre))) == NULL)
  return XC_MEMORY_ALLOCATION;

 if(frame == NULL)
  if((frame = (c_Cadre *)NEW(c_Cadre)(This->BaseStd)) == NULL)
  {
   Xc_free(lcadre);
   return XC_MEMORY_ALLOCATION;
  }
  
 if(frame->name != name) memcpy(frame->name, name, XcMAX_NAME_LENGTH);

 This->nb_cadre++;
 *link = lcadre;

 lcadre->cadre = frame;
 lcadre->name = frame->name;
 lcadre->priority = priority;
 lcadre->NextCadre = NULL;
    
 frame->sScale = This->sScale;
 frame->sFactor = This->sFactor;
 frame->data_to_object = &(This->data_to_object);

 Xc_TRACE(("Cadre `%s' added with no error", lcadre->name));
 return XC_NO_ERROR;
}

void delete_Cadre(This,lcadre,delcadre)
c_FrMg	*This;
l_Cadre	*lcadre;
boolean	delcadre;
{
 l_Cadre	*tmp;
 l_Cadre	*cadre;
  
 Xc_HISTORY(("deleting cadre %s", lcadre->name));

 cadre=tmp=This->ListCadre;
 while(tmp->cadre!=lcadre->cadre)
 {
  cadre=tmp;
  tmp=tmp->NextCadre;
 }
 F(This).deselect_cadre(This,lcadre,FALSE);
 if (cadre!=tmp)
  cadre->NextCadre=tmp->NextCadre;
 else
  This->ListCadre=tmp->NextCadre;
 Xc_free(tmp);
 This->nb_cadre--;
  
 if (delcadre==DEL_CADRE)
  DELETE(c_Cadre)(lcadre->cadre);
}

void list_Cadre(This)
c_FrMg	*This;
{
 int   i;
 l_Cadre       *tmp;

 Xc_TRACE(("Listing des cadres:"));
 tmp=This->ListCadre;
 for(i=0;i<This->nb_cadre;i++)
 {
  Xc_TRACE(("%d) name: %s / priority: %d", i, tmp->name, tmp->priority));
  tmp=tmp->NextCadre;
 }
}

void destroy_cadre(This,lcadre,delcadre)
c_FrMg	*This;
l_Cadre	*lcadre;
boolean	delcadre;
{
 l_Cadre *lowlist;
 l_Cadre *lowsort;
 l_Cadre *highlist;
 l_Cadre *tmplist;
 l_Cadre *tmplist2;
 l_Cadre *transp;
 c_Cadre *this_cadre;
 c_Cadre **lowcadre = NULL;
 Polygone *forme;
 Polygone *lowforme;
 Polygone *ftransp;
 Polygone *oldforme = NULL;
 int	count_cadre;
 int	i;
  
 this_cadre=lcadre->cadre;
 list_cadre_low_high_priority(This,lcadre,&lowlist,&highlist,INTERS_CADRE);
 forme=DuplicatePolygone(lcadre->cadre->Forme);
 tmplist=highlist;
 transp=NULL;
 while(tmplist)
 { 
  if (tmplist->cadre->inside_color->transparency)
   add_cadre_in_list(&transp,tmplist);
  else
   SubtractPolygone(&forme,tmplist->cadre->Forme);
  tmplist=tmplist->NextCadre;
 }
 if (transp)
  oldforme=DuplicatePolygone(lcadre->cadre->Forme);
 lowsort=sort_desc_list_cadre(lowlist);
 tmplist=lowsort;
 count_cadre=0;
 while(tmplist)
 {
  count_cadre++;
  tmplist=tmplist->NextCadre;
 }
 if (count_cadre)
 {
  lowcadre=(c_Cadre **)Xc_malloc("mv lcadre",sizeof(c_Cadre *)*count_cadre);
  tmplist=lowsort;
  for(i=0;i<count_cadre;i++)
  {
   lowcadre[i]=(c_Cadre *)Xc_malloc("mv cadre",sizeof(c_Cadre));
   memcpy(lowcadre[i],tmplist->cadre,sizeof(c_Cadre));
   lowforme=IntersectPolygone(forme,lowcadre[i]->Forme);
   if (!lowcadre[i]->inside_color->transparency)
    SubtractPolygone(&forme,lowforme);
   lowcadre[i]->Forme=lowforme;
   tmplist=tmplist->NextCadre;
  }
 }
  
 /*DrawPolygone(forme,This->X_info);*/
  
 free_list_cadre(lowlist,NO_DEL_CADRE);
 free_list_cadre(highlist,NO_DEL_CADRE);

 if (count_cadre)
 {
  tmplist=lowlist=sort_asc_list_cadre(lowsort);
  for(i=count_cadre-1;i>=0;i--)
  {
   Xc_TRACE(("cadre pos: %d name: %s",i,lowcadre[i]->name));

   F(tmplist->cadre).drawcadre(tmplist->cadre,lowcadre[i]->Forme);
   F(tmplist->cadre).drawcontour(tmplist->cadre,lowcadre[i]->Forme,
				 This->Frame_Border_actif);
   FreePolygone(lowcadre[i]->Forme,DEL_POLYGONE);
   Xc_free(lowcadre[i]);
   tmplist=tmplist->NextCadre;
  }
  Xc_free(lowcadre);
 }
 free_list_cadre(lowlist,NO_DEL_CADRE);
 free_list_cadre(lowsort,NO_DEL_CADRE);
  
 tmplist=transp;
 if (tmplist)
 {
  do
  {
   list_cadre_low_high_priority(This,lcadre,&lowlist,
				&highlist,INTERS_CADRE);
   ftransp=IntersectPolygone(tmplist->cadre->Forme,oldforme);
   tmplist2=highlist;
   while(tmplist2)
   {
    if (!tmplist2->cadre->inside_color->transparency)
     SubtractPolygone(&ftransp,tmplist2->cadre->Forme);
    tmplist2=tmplist2->NextCadre;
   }
   F(tmplist->cadre).drawcadre(tmplist->cadre,ftransp);
   F(tmplist->cadre).drawcontour(tmplist->cadre,ftransp,
				 This->Frame_Border_actif);
	  
   free_list_cadre(lowlist,NO_DEL_CADRE);
   free_list_cadre(highlist,NO_DEL_CADRE);
   FreePolygone(ftransp,DEL_POLYGONE);
   tmplist=tmplist->NextCadre;
  }
  while(tmplist);
  FreePolygone(oldforme,DEL_POLYGONE);
  free_list_cadre(transp,NO_DEL_CADRE);
 }
 delete_Cadre(This,lcadre,delcadre);
  
 This->modify_cadre_callback(This->callback_data);
 This->redraw_polygone_callback(This->callback_data, forme);
 FreePolygone(forme,DEL_POLYGONE);
}

void redisplay_cadre(This,lcadre)
c_FrMg	*This;
l_Cadre	*lcadre;
{
 l_Cadre	*lowlist;
 l_Cadre	*lowsort;
 l_Cadre	*highlist;
 l_Cadre	*tmplist;
 l_Cadre	*tmplist2;
 l_Cadre	*transp;
 c_Cadre	*this_cadre;
 c_Cadre	**lowcadre = NULL;
 Polygone	*forme;
 Polygone	*lowforme;
 Polygone	*ftransp;
 Polygone	*oldforme;
 int		count_cadre;
 int		i;
 coord_t	x_orig;
 coord_t	y_orig;
  
 SET_WAIT_MODE_STATIC;
 this_cadre=lcadre->cadre;
 list_cadre_low_high_priority(This,lcadre,&lowlist,&highlist,INTERS_CADRE);
 forme=DuplicatePolygone(this_cadre->Forme);
 tmplist=highlist;
 transp=NULL;
 while(tmplist)
 {
  if (tmplist->cadre->inside_color->transparency)
   add_cadre_in_list(&transp,tmplist);
  else
   SubtractPolygone(&forme,tmplist->cadre->Forme);
  tmplist=tmplist->NextCadre;
 }
 if (transp)
  oldforme=DuplicatePolygone(this_cadre->Forme);
 lowsort=sort_desc_list_cadre(lowlist);
 tmplist=lowsort;
 count_cadre=0;
 while(tmplist)
 {
  count_cadre++;
  tmplist=tmplist->NextCadre;
 }
 if (count_cadre)
 {
  lowcadre=(c_Cadre **)Xc_malloc("mv lcadre",sizeof(c_Cadre *)*count_cadre);
  tmplist=lowsort;
  for(i=0;i<count_cadre;i++)
  {
   lowcadre[i]=(c_Cadre *)Xc_malloc("mv cadre",sizeof(c_Cadre));
   memcpy(lowcadre[i],tmplist->cadre,sizeof(c_Cadre));
   lowforme=IntersectPolygone(forme,lowcadre[i]->Forme);
   if (!lowcadre[i]->inside_color->transparency)
    SubtractPolygone(&forme,lowforme);
   lowcadre[i]->Forme=lowforme;
   tmplist=tmplist->NextCadre;
  }
 }
 x_orig = this_cadre->sOrig_X + this_cadre->saOrig_X;
 y_orig = this_cadre->sOrig_Y + this_cadre->saOrig_Y;
 F(this_cadre).calculorigine(this_cadre);
 F(this_cadre).calculforme(this_cadre, CONTOUR);
 F(This).calcul_select_handle(This);

 if (this_cadre->object_type)
 {
  if (!this_cadre->sAngle)
  {
   this_cadre->sConfig_X = this_cadre->sOrig_X +
    this_cadre->saOrig_X - x_orig;
   this_cadre->sConfig_Y = this_cadre->sOrig_Y +
    this_cadre->saOrig_Y - y_orig;
  }
  else
  {
   this_cadre->sConfig_X = 0;
   this_cadre->sConfig_Y = 0;
  }	  
  DISPLAY_CONFIGURE(this_cadre->object)(this_cadre->object, this_cadre,
					this_cadre->Forme);
  this_cadre->sConfig_X = 0;
  this_cadre->sConfig_Y = 0;
 }
  
 This->modify_cadre_callback(This->callback_data);
  
 if (!this_cadre->inside_color->transparency)
  SubtractPolygone(&forme, this_cadre->Forme);
  
 /*DrawPolygone(forme,This->X_info);*/
  
 This->redraw_polygone_callback(This->callback_data, forme);
  
 FreePolygone(forme, DEL_POLYGONE);
 free_list_cadre(lowlist, NO_DEL_CADRE);
 free_list_cadre(highlist, NO_DEL_CADRE);
  
 if (count_cadre)
 {
  tmplist = lowlist = sort_asc_list_cadre(lowsort);
  for(i=count_cadre-1;i>=0;i--)
  {
   Xc_TRACE(("cadre pos: %d name: %s",i,lowcadre[i]->name));

   if (!this_cadre->inside_color->transparency)
    SubtractPolygone(&(lowcadre[i]->Forme),this_cadre->Forme);
   F(tmplist->cadre).drawcadre(tmplist->cadre,lowcadre[i]->Forme);
   F(tmplist->cadre).drawcontour(tmplist->cadre,lowcadre[i]->Forme,
				 This->Frame_Border_actif);
   FreePolygone(lowcadre[i]->Forme,DEL_POLYGONE);
   Xc_free(lowcadre[i]);
   tmplist=tmplist->NextCadre;
  }
  Xc_free(lowcadre);
 }
 free_list_cadre(lowlist,NO_DEL_CADRE);
 free_list_cadre(lowsort,NO_DEL_CADRE);

 tmplist=transp;
 if (tmplist)
 {
  SubtractPolygone(&oldforme,this_cadre->Forme);
  do
  {
   list_cadre_low_high_priority(This, tmplist, &lowlist,
				&highlist, INTERS_CADRE);
   ftransp=IntersectPolygone(tmplist->cadre->Forme, oldforme);
   tmplist2=highlist;
   while(tmplist2)
   {
    if (!tmplist2->cadre->inside_color->transparency)
     SubtractPolygone(&ftransp, tmplist2->cadre->Forme);
    tmplist2 = tmplist2->NextCadre;
   }
   F(tmplist->cadre).drawcadre(tmplist->cadre, ftransp);
   F(tmplist->cadre).drawcontour(tmplist->cadre, ftransp,
				 This->Frame_Border_actif);
	  
   free_list_cadre(lowlist, NO_DEL_CADRE);
   free_list_cadre(highlist, NO_DEL_CADRE);
   FreePolygone(ftransp, DEL_POLYGONE);
   tmplist = tmplist->NextCadre;
  }
  while(tmplist);
  FreePolygone(oldforme, DEL_POLYGONE);
  free_list_cadre(transp, NO_DEL_CADRE);
 }
 draw_cadre(This, lcadre);
 UNSET_WAIT_MODE;
}

/* --------------------------------------------------------------------- */
/* --------------------  Get frames' functions  ------------------------ */
/* --------------------------------------------------------------------- */

char *copy_cadre_name(This, name_org)
c_FrMg	*This;
char	*name_org;
{
 char	*name;
 char *ptr;
 int	pnt_name;
 int	i;
  
 Xc_TRACE(("copy_cadre_name(`%s')", name_org));

 name = (char *)Xc_malloc("cadre name", XcMAX_NAME_LENGTH);
 pnt_name = 0;
 if ((i = Xstrnchr(name_org, '.')) >= 0)
 {
  for(ptr = name_org + i + 1; *ptr ; ptr++)
   if(!isdigit(*ptr))
   {
    i = strlen(name_org);
    break;
   }
 }
 else
  i = strlen(name_org);
  
 memcpy(name, name_org, i);
 do 
 {
  sprintf(name + i, ".%d", pnt_name++);
 } 
 while(F(This).get_cadre_by_name(This, name));

 return name;
}

l_Cadre *get_new_cadre(This)
c_FrMg	*This;
{ 
 l_Cadre *lcadre;
 c_Cadre *frame;
 int   priority;
 int	pnt_name;

 if ( (frame = (c_Cadre *)NEW(c_Cadre)(This->BaseStd)) == NULL)
  return NULL;
 priority = F(This).get_highest_priority(This);

 pnt_name = This->nb_cadre;
 do
 {
  sprintf(frame->name, "%s %d", defaultFrameName, pnt_name++);
 }
 while(F(This).add_cadre(This, frame->name,
			 priority, frame) == XC_NAME_ALREADY_EXIST);

 lcadre = F(This).get_cadre_by_name(This, frame->name);
 return lcadre;
}

l_Cadre *get_copy_cadre(This, lcadre, cadre)
c_FrMg	*This;
l_Cadre	*lcadre;
c_Cadre	*cadre;
{ 
 char	*name;
 int   priority;
  
 name = F(This).copy_cadre_name(This, lcadre->name);
 priority = F(This).get_highest_priority(This);
 F(This).add_cadre(This, name, priority, cadre);
 lcadre = F(This).get_cadre_by_name(This, name);
 Xc_free(name);
 return lcadre;
}

l_Cadre *get_cadre_by_xy(This,X,Y)
c_FrMg *This;
int X;
int Y;
{
 l_Cadre *list_cadre;
 l_Cadre *ret_cadre;
 l_Point *lpoint;
 l_Point *lstpnt;
 l_Point tmppoint;
 int ret_dist;
 int nb_point;
 int px,py;
  
 list_cadre=This->ListCadre;
 ret_cadre=NULL;
 while(list_cadre)
 {
  if (F(list_cadre->cadre).isincadre(list_cadre->cadre,X,Y))
  {
   if (ret_cadre)
   {
    if (ret_cadre->priority<list_cadre->priority)
     ret_cadre=list_cadre;
   }
   else
    ret_cadre=list_cadre;
  }
  else
  {
   tmppoint.X = X-list_cadre->cadre->Forme->Orig_X;
   tmppoint.Y = Y-list_cadre->cadre->Forme->Orig_Y;
   lpoint = lstpnt = list_cadre->cadre->ListPoint;
   nb_point = 0;
   while(lpoint)
   {
    if (lpoint->NextPoint && lpoint->NextPoint->cont)
     nb_point++;
    else
    {
     if (nb_point<2)
     {
      if (nb_point == 0)
      {
       px=list_cadre->cadre->Forme->Orig_X;
       py=list_cadre->cadre->Forme->Orig_Y;
       if (lpoint->X+px<X+HANDLE_X_ACT && 
	   lpoint->X+px>X-HANDLE_X_ACT &&
	   lpoint->Y+py<Y+HANDLE_Y_ACT && 
	   lpoint->Y+py>Y-HANDLE_Y_ACT )
       {
	if (ret_cadre)
	{
	 if (ret_cadre->priority<list_cadre->priority)
	  ret_cadre=list_cadre;
	}
	else
	 ret_cadre=list_cadre;
	break;
       }
      }
      else
      {
       ret_dist = distance_line_point( lpoint, lstpnt, &tmppoint);
       if (ret_dist >= 0 && ret_dist < 36)
       {
	if (ret_cadre)
	{
	 if (ret_cadre->priority<list_cadre->priority)
	  ret_cadre=list_cadre;
	}
	else
	 ret_cadre=list_cadre;
	break;
       }
      }
     }
     nb_point = 0;
    }
    lstpnt = lpoint;
    lpoint = lpoint->NextPoint;
   }
  }
  list_cadre=list_cadre->NextCadre;
 }
 return ret_cadre;
}

l_Cadre *get_cadre_by_name(This,name)
c_FrMg *This;
char	*name;
{
 int i;
 l_Cadre *tmp;
  
 tmp=This->ListCadre;
 i=0;
 while((i<This->nb_cadre)&&(!Xstrcmp(name, tmp->name)))
 {
  i++;
  tmp=tmp->NextCadre;
 }
 if (i<This->nb_cadre)
 {
  return tmp;
 }
 return NULL;
}

l_Cadre *get_cadre_and_point_by_xy(This,listsel,lstpnt,X,Y)
c_FrMg	*This;
l_Cadre	*listsel;
l_Point	***lstpnt;
int	X;
int	Y;
{
 boolean	found;
 l_Cadre	*tmplcadre;
 l_Cadre	*retcadre;
 l_Point	*tmplpoint;
 l_Cadre	*foundlcadre = NULL;
 l_Point	*foundlpoint = NULL;
 l_Point	*foundprevlpoint = NULL;
 l_Point	*foundnextlpoint = NULL;
 l_Point	*savelpoint;
 l_Point	*nextlpoint;
 l_Point	*prevlpoint;
 l_Point	**listpnt;
 int		px,py;

 if (!listsel)
 {
  listsel=This->SelectedCadre;
  if (!listsel)
  {
   return NULL;
  }
 }
 found=FALSE;
  
 tmplcadre=listsel;
 while(!found && tmplcadre)
 {
  px=tmplcadre->cadre->Forme->Orig_X;
  py=tmplcadre->cadre->Forme->Orig_Y;
  foundprevlpoint=NULL;
  foundnextlpoint=NULL;
  savelpoint=prevlpoint=NULL;
  tmplpoint=tmplcadre->cadre->ListPoint;
  while(!found && tmplpoint)
  {
   if (!tmplpoint->cont)
   {
    savelpoint=tmplpoint;
   }    
   if (tmplpoint->X+px<X+HANDLE_X_ACT && 
       tmplpoint->X+px>X-HANDLE_X_ACT &&
       tmplpoint->Y+py<Y+HANDLE_Y_ACT && 
       tmplpoint->Y+py>Y-HANDLE_Y_ACT )
   {
    found=TRUE;
    foundlcadre=tmplcadre;
    foundlpoint=tmplpoint;
    if (!tmplpoint->NextPoint || !tmplpoint->NextPoint->cont)
     foundnextlpoint=savelpoint;
    else
     foundnextlpoint=tmplpoint->NextPoint;
    if (!tmplpoint->cont)
    {
     nextlpoint=tmplpoint;
     while (nextlpoint->NextPoint && nextlpoint->NextPoint->cont)
     { 
      nextlpoint=nextlpoint->NextPoint;
     }
     foundprevlpoint=nextlpoint;
    }
    else
     foundprevlpoint=prevlpoint;
   }
   prevlpoint=tmplpoint;
   tmplpoint=tmplpoint->NextPoint;
  }
  tmplcadre=tmplcadre->NextCadre;
 }
 if (!found)
 {
  return NULL;
 }
 listpnt=(l_Point **)Xc_malloc("point 1",sizeof(l_Point *)*3);
 listpnt[0]=foundlpoint;
 if (!foundprevlpoint || foundprevlpoint==foundlpoint)
  listpnt[1]=NULL;
 else
  listpnt[1]=foundprevlpoint;
 if (!foundnextlpoint || foundprevlpoint==foundnextlpoint)
  listpnt[2]=NULL;
 else
  listpnt[2]=foundnextlpoint;
  
 retcadre = get_cadre_by_name(This, foundlcadre->name);
 *lstpnt=listpnt;
  
 return retcadre;
}

l_Cadre *get_cadre_and_point_by_line(This,listsel,lstpnt,X,Y)
c_FrMg	*This;
l_Cadre	*listsel;
l_Point	***lstpnt;
int	X;
int	Y;
{
 l_Cadre	*slistsel;
 l_Cadre	*retcadre;
 l_Cadre	*found_cadre = NULL;
 l_Point	*lpoint;
 l_Point	*spoint;
 l_Point	*sspoint;
 l_Point	*A;
 l_Point	*B;
 l_Point	M;
 l_Point	*found_A = NULL;
 l_Point	*found_B = NULL;
 l_Point	**listpnt;
 boolean	found;
 int		ret_dist;
 int		min_dist;
  
 if (!listsel)
 {
  listsel=This->SelectedCadre;
  if (!listsel)
  {
   return NULL;
  }
 }
 min_dist=36;
 found=FALSE;
 slistsel=listsel;
 while(slistsel) 
 {
  M.X=X-slistsel->cadre->Forme->Orig_X;
  M.Y=Y-slistsel->cadre->Forme->Orig_Y;
    
  sspoint=spoint=lpoint=slistsel->cadre->ListPoint;
  lpoint=lpoint->NextPoint;
  while(1)
  {
   if (!lpoint || lpoint->cont==FALSE)
   {
    A=spoint;
    B=sspoint;
    if (A!=B)
    {
     ret_dist=distance_line_point(A,B,&M);
     if (ret_dist>=0)
     {
      if (ret_dist<min_dist)
      {
       found_A=A;
       found_B=B;
       found_cadre=slistsel;
       min_dist=ret_dist;
       found=TRUE;
      }
     }
    }
    if (lpoint)
    {
     sspoint=lpoint;
    }
    else
    {
     break;
    }
   }
   else
   {
    A=spoint;
    B=lpoint;
    ret_dist=distance_line_point(A,B,&M);
    if (ret_dist>=0)
    {
     if (ret_dist<min_dist)
     {
      found_A=A;
      found_B=B;
      found_cadre=slistsel;
      min_dist=ret_dist;
      found=TRUE;
     }
    }
   }
   spoint=lpoint;
   lpoint=lpoint->NextPoint;
  }
  slistsel=slistsel->NextCadre;
 }
 if (found)
 {
  listpnt=(l_Point **)Xc_malloc("pnt line",sizeof(l_Point *)*2);
  listpnt[0]=found_A;
  listpnt[1]=found_B;
  *lstpnt=listpnt;
  retcadre=found_cadre;
 }
 else
 {
  retcadre=NULL;
  *lstpnt=NULL;
 }
 return retcadre;
}

/* ----------------------------------------------------------------- ** 
** GET HANDLE SUBROUTINES                                            ** 
** ----------------------------------------------------------------- */

typedef struct {
		int		x_point;
		int		y_point;
		coord_t	x_src;
		coord_t	y_src;
		coord_t	x_dst;
		coord_t	y_dst;
		angle_t	angle;
	       } handle_t;

int Is_in_handle();
int Convert_angle();

l_Point	*get_handle_by_xy(This,X,Y,angle,rlcadre)
c_FrMg	*This;
int	X;
int	Y;
int	*angle;      
l_Cadre	**rlcadre;
{
 int		i,pos;
 int		x_min,y_min;
 int		x_max,y_max;
 int		x_mid,y_mid;
 int		iclx,icly,ichx,ichy;
 coord_t	px,py;
 coord_t	x1,y1,x2,y2,x3,y3,x4,y4;
 l_Point	*foundlpoint;
 handle_t	handles[8];
 l_Cadre	*lcadre;
 l_Cadre	duplcadre;
 l_Cadre	*retlcadre;
 c_Cadre	*cadre;
 double	sAngle;
  
 if (!This->SelectedCadre)
  return NULL;

 if (This->select_group)
 {
  cadre=This->SelectedCadre->cadre;
  if ((!This->SelectedCadre->NextCadre) && 
      ((!cadre->ListPoint->NextPoint) || 
       (!cadre->ListPoint->NextPoint->NextPoint) ||
       (!cadre->sWidth) || (!cadre->sHeight)))
  {
   if (!angle)
    return NULL;
   lcadre=F(This).get_cadre_and_point_by_xy(This,This->SelectedCadre,
					    &foundlpoint,X,Y);
   if (lcadre)
   {
    *angle=9;
    return foundlpoint;
   }
   else
   {
    lcadre=F(This).get_cadre_and_point_by_line(This,
					       This->SelectedCadre,
					       &foundlpoint,X,Y);
    if (lcadre) 
    {
     *angle=10;
     return foundlpoint;
    }
   }
   return NULL;
  }
  if (This->select_type)
   return NULL;
      
  x_min=This->select_sX_min/This->sScale;
  y_min=This->select_sY_min/This->sScale;
  x_max=This->select_sX_max/This->sScale;
  y_max=This->select_sY_max/This->sScale;
  x_mid=(x_max+x_min)>>1;
  y_mid=(y_max+y_min)>>1;
      
  handles[0].x_point=handles[6].x_point=handles[7].x_point=x_min-RHANDLE_X-1;
  handles[0].x_src=handles[6].x_src=handles[7].x_src=This->select_sX_min;
  handles[0].x_dst=handles[6].x_dst=handles[7].x_dst=This->select_sX_max;
      
  handles[2].x_point=handles[3].x_point=handles[4].x_point=x_max+RHANDLE_X+1;
  handles[2].x_src=handles[3].x_src=handles[4].x_src=This->select_sX_max;
  handles[2].x_dst=handles[3].x_dst=handles[4].x_dst=This->select_sX_min;

  handles[1].x_point=handles[5].x_point=x_mid;
  handles[1].x_src=handles[5].x_dst=handles[1].x_src=handles[5].x_dst=
   (This->select_sX_min+This->select_sX_max)/2;

  handles[0].y_point=handles[1].y_point=handles[2].y_point=	y_min-RHANDLE_Y-1;
  handles[0].y_src=handles[1].y_src=handles[2].y_src=This->select_sY_min;
  handles[0].y_dst=handles[1].y_dst=handles[2].y_dst=This->select_sY_max;

  handles[4].y_point=handles[5].y_point=handles[6].y_point=y_max+RHANDLE_Y+1;
  handles[4].y_src=handles[5].y_src=handles[6].y_src=This->select_sY_max;
  handles[4].y_dst=handles[5].y_dst=handles[6].y_dst=This->select_sY_min;

  handles[3].y_point=handles[7].y_point=y_mid;
  handles[3].y_src=handles[7].y_dst=handles[3].y_src=handles[7].y_dst=
   (This->select_sY_min+This->select_sY_max)/2;

  handles[0].angle=handles[4].angle=SCALE_FROM_DEGREES(135);
  handles[1].angle=handles[5].angle=SCALE_FROM_DEGREES(90);
  handles[2].angle=handles[6].angle=SCALE_FROM_DEGREES(45);
  handles[3].angle=handles[7].angle=SCALE_FROM_DEGREES(0);

  pos=Is_in_handle(handles,X,Y);
  if (pos==-1)
  {
   return NULL;
  }
  else
  {
   foundlpoint=(l_Point *)Xc_malloc("pnt in handle",sizeof(l_Point)*2);
   foundlpoint[0].sX=handles[pos].x_src;
   foundlpoint[0].sY=handles[pos].y_src;
   foundlpoint[0].X=foundlpoint[0].sY/This->sScale;
   foundlpoint[0].Y=foundlpoint[0].sY/This->sScale;
   foundlpoint[0].cont=(pos==1?FALSE:(pos==5?FALSE:TRUE));
   foundlpoint[1].sX=handles[pos].x_dst;
   foundlpoint[1].sY=handles[pos].y_dst;
   foundlpoint[1].X=foundlpoint[1].sX/This->sScale;
   foundlpoint[1].Y=foundlpoint[1].sY/This->sScale;
   foundlpoint[1].cont=(pos==3?FALSE:(pos==7?FALSE:TRUE));
   if (angle)
    *angle=Convert_angle(handles[pos].angle);
   if (rlcadre)
    *rlcadre=NULL;
   return foundlpoint;	  
  }
 }
 else
 {
  lcadre=This->SelectedCadre;
  duplcadre.NextCadre=NULL;
  duplcadre.name=lcadre->name;
  while(lcadre)
  {
   cadre=lcadre->cadre;
   if ((!cadre->ListPoint->NextPoint) || 
       (!cadre->ListPoint->NextPoint->NextPoint) ||
       (!cadre->sWidth) || (!cadre->sHeight))
   {
    if (!angle)
     return NULL;
    duplcadre.cadre=cadre;
    retlcadre=F(This).get_cadre_and_point_by_xy(This,&duplcadre,
						&foundlpoint,X,Y);
    if (retlcadre)
    {
     if (retlcadre->cadre->ListPoint->NextPoint)
      *angle=9; 
     return foundlpoint;
    }
    else
    {
     retlcadre=F(This).get_cadre_and_point_by_line(This,
						   &duplcadre,
						   &foundlpoint,X,Y);
     if (retlcadre) 
     {
      *angle=10;
      return foundlpoint;
     }
    }
   }
   else 
   {
    if (This->select_type)
    {
     lcadre=lcadre->NextCadre;
     continue;
    }
	    
    sAngle=SCALE_TO_RADIANS(cadre->sAngle);
	    
    iclx=(int)((-(RHANDLE_X+1)*cos(sAngle))/2+.5);
    icly=(int)(( (RHANDLE_X+1)*sin(sAngle))/2+.5);
    ichx=(int)((-(RHANDLE_Y+1)*sin(sAngle))/2+.5);
    ichy=(int)((-(RHANDLE_Y+1)*cos(sAngle))/2+.5);
	    
    px=cadre->sOrig_X;
    py=cadre->sOrig_Y;
    x1=cadre->saOrig_X;
    y1=cadre->saOrig_Y;
    x2=x1+(coord_t)(cos(sAngle)*(double)cadre->saWidth+.5);
    y2=y1-(coord_t)(sin(sAngle)*(double)cadre->saWidth+.5);
    x3=x2+(coord_t)(sin(sAngle)*(double)cadre->saHeight+.5);
    y3=y2+(coord_t)(cos(sAngle)*(double)cadre->saHeight+.5);
    x4=x3-(coord_t)(cos(sAngle)*(double)cadre->saWidth+.5);
    y4=y3+(coord_t)(sin(sAngle)*(double)cadre->saWidth+.5);
	    
    handles[0].x_point=(x1+px)/cadre->sScale+iclx+ichx;
    handles[0].y_point=(y1+py)/cadre->sScale+icly+ichy;
    handles[4].x_point=(x3+px)/cadre->sScale-iclx-ichx;
    handles[4].y_point=(y3+py)/cadre->sScale-icly-ichy;
    handles[0].x_src=handles[4].x_dst=x1+px;
    handles[0].y_src=handles[4].y_dst=y1+py;
    handles[0].x_dst=handles[4].x_src=x3+px;
    handles[0].y_dst=handles[4].y_src=y3+py;
	    
    handles[2].x_point=(x2+px)/cadre->sScale-iclx+ichx;
    handles[2].y_point=(y2+py)/cadre->sScale-icly+ichy;
    handles[6].x_point=(x4+px)/cadre->sScale+iclx-ichx;
    handles[6].y_point=(y4+py)/cadre->sScale+icly-ichy;
    handles[2].x_src=handles[6].x_dst=x2+px;
    handles[2].y_src=handles[6].y_dst=y2+py;
    handles[2].x_dst=handles[6].x_src=x4+px;
    handles[2].y_dst=handles[6].y_src=y4+py;
	    
    handles[1].x_point=((x1+x2)/2+px)/cadre->sScale+ichx;
    handles[1].y_point=((y1+y2)/2+py)/cadre->sScale+ichy;
    handles[5].x_point=((x3+x4)/2+px)/cadre->sScale-ichx;
    handles[5].y_point=((y3+y4)/2+py)/cadre->sScale-ichy;
    handles[1].x_src=handles[5].x_dst=(x1+x2)/2+px;
    handles[1].y_src=handles[5].y_dst=(y1+y2)/2+py;
    handles[1].x_dst=handles[5].x_src=(x3+x4)/2+px;
    handles[1].y_dst=handles[5].y_src=(y3+y4)/2+py;
	    	    
    handles[3].x_point=((x2+x3)/2+px)/cadre->sScale-iclx;
    handles[3].y_point=((y2+y3)/2+py)/cadre->sScale-icly;
    handles[7].x_point=((x4+x1)/2+px)/cadre->sScale+iclx;
    handles[7].y_point=((y4+y1)/2+py)/cadre->sScale+icly;
    handles[3].x_src=handles[7].x_dst=(x2+x3)/2+px;
    handles[3].y_src=handles[7].y_dst=(y2+y3)/2+py;
    handles[3].x_dst=handles[7].x_src=(x4+x1)/2+px;
    handles[3].y_dst=handles[7].y_src=(y4+y1)/2+py;
	    
    handles[0].angle=handles[4].angle=SCALE_FROM_DEGREES(135);
    handles[1].angle=handles[5].angle=SCALE_FROM_DEGREES(90);
    handles[2].angle=handles[6].angle=SCALE_FROM_DEGREES(45);
    handles[3].angle=handles[7].angle=SCALE_FROM_DEGREES(0);
	    
    for(i=0;i<8;i++)
    {
     handles[i].angle += cadre->sAngle;
    }
	    
    pos=Is_in_handle(handles,X,Y);
    if (pos!=-1)
    {
     foundlpoint=(l_Point *)Xc_malloc("pnt in handle",
				      sizeof(l_Point)*2);
     foundlpoint[0].sX=handles[pos].x_src;
     foundlpoint[0].sY=handles[pos].y_src;
     foundlpoint[0].X=foundlpoint[0].sY/cadre->sScale;
     foundlpoint[0].Y=foundlpoint[0].sY/cadre->sScale;
     foundlpoint[0].cont=(pos==1?FALSE:(pos==5?FALSE:TRUE));
     foundlpoint[1].sX=handles[pos].x_dst;
     foundlpoint[1].sY=handles[pos].y_dst;
     foundlpoint[1].X=foundlpoint[1].sX/cadre->sScale;
     foundlpoint[1].Y=foundlpoint[1].sY/cadre->sScale;
     foundlpoint[1].cont=(pos==3?FALSE:(pos==7?FALSE:TRUE));
     if (angle)
      *angle=Convert_angle(handles[pos].angle);
     if (rlcadre)
      *rlcadre=lcadre;
     return foundlpoint;	  
    }
   }
   lcadre=lcadre->NextCadre;
  }
 }
 return NULL;
}

int Is_in_handle(handles,X,Y)
handle_t *handles;
int	 X;
int	 Y;
{
 int	i;
  
 for(i=0;i<8;i++)
 {
  if (handles[i].x_point >= X-RHANDLE_X-RHANDLE_X_ACT &&
      handles[i].x_point <= X+RHANDLE_X+RHANDLE_X_ACT &&
      handles[i].y_point >= Y-RHANDLE_Y-RHANDLE_Y_ACT &&
      handles[i].y_point <= Y+RHANDLE_Y+RHANDLE_Y_ACT)
   return i;
 }
 return -1;
}

int Convert_angle(anglet)
angle_t	anglet;
{
 static int conv[9]={5,4,3,2,1,8,7,6,5};
 int angle;
  
 angle=(int)SCALE_TO_DEGREES(anglet)%180;
 if (angle<0)
  angle+=180;
 angle=((angle<<2)+45)/90;
 return conv[angle];
}

/* --------------------------------------------------------------------- */
/* - Miscellaneous functions on drawing, moving & recoordinate frames -- */
/* --------------------------------------------------------------------- */

void calcul_select_handle(This)
c_FrMg	*This;
{
 coord_t	x_min,y_min,x_max,y_max;
  
 get_limit_list_cadre(This->SelectedCadre,&x_min,&y_min,&x_max,&y_max);
 This->select_sX_min=x_min;
 This->select_sY_min=y_min;
 This->select_sX_max=x_max;
 This->select_sY_max=y_max;
}

void change_frmg_origin(This, delta_x, delta_y)
c_FrMg	*This;
int	delta_x;
int	delta_y;
{
 l_Cadre	*lcadre;
 coord_t	delta_sx;
 coord_t	delta_sy;
  
 if (delta_x==0 && delta_y==0)
  return;
 delta_sx = delta_x * This->sScale;
 delta_sy = delta_y * This->sScale;
 This->Orig_X += delta_x;
 This->Orig_Y += delta_y;
 This->sOrig_X = This->Orig_X*This->sScale;
 This->sOrig_Y = This->Orig_Y*This->sScale;
 This->scroll_delta_sX+=delta_sx;
 This->scroll_delta_sY+=delta_sy;
 This->scroll_delta_X+=delta_x;
 This->scroll_delta_Y+=delta_y;
 This->select_sX_min+=delta_sx;
 This->select_sY_min+=delta_sy;
 This->select_sX_max+=delta_sx;
 This->select_sY_max+=delta_sy;
  
 lcadre=This->ListCadre;
 while(lcadre)
 {
  lcadre->cadre->Forme->Orig_X+=delta_x;
  lcadre->cadre->Forme->Orig_Y+=delta_y;
  lcadre->cadre->sOrig_X+=delta_sx;
  lcadre->cadre->sOrig_Y+=delta_sy;
  lcadre=lcadre->NextCadre;
 }
}

void change_frmg_scaling(This, new_factor, x, y)
c_FrMg	*This;
scale_t	new_factor;
int	x;
int	y;
{
 l_Cadre	*lcadre;
 c_Cadre	*cadre;
 coord_t	new_scale;
 coord_t	old_scale;
 coord_t	old_sOrig_X;
 coord_t	old_sOrig_Y;
 coord_t	sx, sy;
  
 new_scale=(coord_t)( ( (double)COORD_DEFAULT*
		       (double)SCALE_FROM_PERCENT(100) )/
		     (double)new_factor );
  
 old_scale = This->sScale;
 old_sOrig_X = This->sOrig_X;
 old_sOrig_Y = This->sOrig_Y;
  
 This->sFactor = new_factor;
 This->sScale = new_scale;

 This->sOrig_X += x*new_scale;
 This->sOrig_Y += y*new_scale;
 This->Orig_X = This->sOrig_X / new_scale;
 This->Orig_Y = This->sOrig_Y / new_scale;
  
 lcadre = This->ListCadre;
 while(lcadre)
 {
  cadre = lcadre->cadre;
  cadre->sScale = new_scale;
  cadre->sFactor = new_factor;
  sx = (coord_t)(cadre->sOrig_X+old_sOrig_X);
  sy = (coord_t)(cadre->sOrig_Y+old_sOrig_Y);
  sx -= This->sOrig_X;
  sy -= This->sOrig_Y;
  cadre->sOrig_X = sx;
  cadre->sOrig_Y = sy;
  cadre->Forme->Orig_X = sx/new_scale;
  cadre->Forme->Orig_Y = sy/new_scale;
  F(cadre).calculscaling(cadre);
  F(cadre).calculorigine(cadre);
  F(cadre).calculforme(cadre,CONTOUR);
  if (cadre->object_type != CADRE_EMPTY)
  {
   if (DISPLAY_INIT(cadre->object)(cadre->object,cadre)!=XC_NO_ERROR)
    F(cadre).detachobject(cadre);
  }
  lcadre=lcadre->NextCadre;
 }
 F(This).calcul_select_handle(This);
}

void init_recoordinate_point(This)
c_FrMg	*This;
{
 This->scroll_delta_sX=0;
 This->scroll_delta_sY=0;
 This->scroll_delta_X=0;
 This->scroll_delta_Y=0;
}

void recoordinate_point(This, sX, sY, x, y, init)
c_FrMg	*This;
coord_t	*sX;
coord_t	*sY;
int	*x;
int	*y;
boolean	init;
{
 if (sX)
  *sX+=This->scroll_delta_sX;
 if (sY)
  *sY+=This->scroll_delta_sY;
 if (x)
  *x+=This->scroll_delta_X;
 if (y)
  *y+=This->scroll_delta_Y;
 if (init)
 {
  This->scroll_delta_sX=0;
  This->scroll_delta_sY=0;
  This->scroll_delta_X=0;
  This->scroll_delta_Y=0;
 }
}      

void attract_point(This,sx,sy,x,y)
c_FrMg	*This;
coord_t	*sx;
coord_t	*sy;
int	*x;
int	*y;
{
 if (This->magnetic)
  This->attract_point_callback(This->callback_data, sx, sy, x, y);
}

void move_cadre_to_xy(This,lcadre,sx,sy)
c_FrMg	*This;
l_Cadre	*lcadre;
coord_t	sx;
coord_t	sy;
{
 l_Cadre	*lowlist;
 l_Cadre	*lowsort;
 l_Cadre	*highlist;
 l_Cadre	*tmplist;
 l_Cadre	*tmplist2;
 l_Cadre	*transp;
 c_Cadre	*this_cadre;
 c_Cadre	**lowcadre = NULL;
 Polygone	*forme;
 Polygone	*lowforme;
 Polygone	*ftransp;
 Polygone	*oldforme;
 int		count_cadre;
 int		i;
  
 SET_WAIT_MODE_STATIC;
 this_cadre=lcadre->cadre;
 list_cadre_low_high_priority(This,lcadre,&lowlist,&highlist,INTERS_CADRE);
 forme=DuplicatePolygone(this_cadre->Forme);
 tmplist=highlist;
 transp=NULL;
 while(tmplist)
 {
  if (tmplist->cadre->inside_color->transparency)
   add_cadre_in_list(&transp,tmplist);
  else
   SubtractPolygone(&forme,tmplist->cadre->Forme);
  tmplist=tmplist->NextCadre;
 }
 if (transp)
  oldforme=DuplicatePolygone(this_cadre->Forme);
 lowsort=sort_desc_list_cadre(lowlist);
 tmplist=lowsort;
 count_cadre=0;
 while(tmplist)
 {
  count_cadre++;
  tmplist=tmplist->NextCadre;
 }
 if (count_cadre)
 {
  lowcadre=(c_Cadre **)Xc_malloc("mv lcadre",sizeof(c_Cadre *)*count_cadre);
  tmplist=lowsort;
  for(i=0;i<count_cadre;i++)
  {
   lowcadre[i]=(c_Cadre *)Xc_malloc("mv cadre",sizeof(c_Cadre));
   memcpy(lowcadre[i],tmplist->cadre,sizeof(c_Cadre));
   lowforme=IntersectPolygone(forme,lowcadre[i]->Forme);
   if (!lowcadre[i]->inside_color->transparency)
    SubtractPolygone(&forme,lowforme);
   lowcadre[i]->Forme=lowforme;
   tmplist=tmplist->NextCadre;
  }
 }
 this_cadre->Forme->Orig_X=sx/this_cadre->sScale;
 this_cadre->Forme->Orig_Y=sy/this_cadre->sScale;
 this_cadre->sOrig_X=sx;
 this_cadre->sOrig_Y=sy;
 F(This).calcul_select_handle(This);
  
 This->modify_cadre_callback(This->callback_data);
  
 if (!this_cadre->inside_color->transparency)
  SubtractPolygone(&forme,this_cadre->Forme);
  
 /*DrawPolygone(forme,This->X_info);*/
 This->redraw_polygone_callback(This->callback_data, forme);
  
 FreePolygone(forme,DEL_POLYGONE);
 free_list_cadre(lowlist,NO_DEL_CADRE);
 free_list_cadre(highlist,NO_DEL_CADRE);
  
 if (count_cadre)
 {
  tmplist=lowlist=sort_asc_list_cadre(lowsort);
  for(i=count_cadre-1;i>=0;i--)
  {
   Xc_TRACE(("cadre pos: %d name: %s",i,lowcadre[i]->name));
      
   if (!this_cadre->inside_color->transparency)
    SubtractPolygone(&(lowcadre[i]->Forme),this_cadre->Forme);
   F(tmplist->cadre).drawcadre(tmplist->cadre,lowcadre[i]->Forme);
   F(tmplist->cadre).drawcontour(tmplist->cadre,lowcadre[i]->Forme,
				 This->Frame_Border_actif);
   FreePolygone(lowcadre[i]->Forme,DEL_POLYGONE);
   Xc_free(lowcadre[i]);
   tmplist=tmplist->NextCadre;
  }
  Xc_free(lowcadre);
 }
 free_list_cadre(lowlist,NO_DEL_CADRE);
 free_list_cadre(lowsort,NO_DEL_CADRE);

 tmplist=transp;
 if (tmplist)
 {
  SubtractPolygone(&oldforme,this_cadre->Forme);
  do
  {
   list_cadre_low_high_priority(This, tmplist, &lowlist,
				&highlist, INTERS_CADRE);
   ftransp=IntersectPolygone(tmplist->cadre->Forme,oldforme);
   tmplist2=highlist;
   while(tmplist2)
   {      
    if (!tmplist2->cadre->inside_color->transparency)
     SubtractPolygone(&ftransp,tmplist2->cadre->Forme);
    tmplist2=tmplist2->NextCadre;
   }
   F(tmplist->cadre).drawcadre(tmplist->cadre,ftransp);
   F(tmplist->cadre).drawcontour(tmplist->cadre,ftransp,
				 This->Frame_Border_actif);
	
   free_list_cadre(lowlist,NO_DEL_CADRE);
   free_list_cadre(highlist,NO_DEL_CADRE);
   FreePolygone(ftransp,DEL_POLYGONE);
   tmplist=tmplist->NextCadre;
  }
  while(tmplist);
  FreePolygone(oldforme,DEL_POLYGONE);
  free_list_cadre(transp,NO_DEL_CADRE);
 }
 draw_cadre(This,lcadre);
 UNSET_WAIT_MODE;
}

void draw_cadre_contour(This,lcadre)
c_FrMg	*This;
l_Cadre	*lcadre;	
{
 l_Cadre   *lowlist;	
 l_Cadre   *highlist;	
 l_Cadre   *tmp_list;
 c_Cadre   *this_cadre;
 Polygone  *forme;
  
 this_cadre=lcadre->cadre;
 list_cadre_low_high_priority(This,lcadre,&lowlist,&highlist,INTERS_CADRE);
 tmp_list=highlist;
 forme=DuplicatePolygone(this_cadre->Forme);
 while(tmp_list)
 {
  SubtractPolygone(&forme,tmp_list->cadre->Forme);
  tmp_list=tmp_list->NextCadre;
 }
 F(this_cadre).drawcontour(this_cadre,forme,
			   This->Frame_Border_actif);
 FreePolygone(forme,DEL_POLYGONE);
  
 free_list_cadre(lowlist,NO_DEL_CADRE);
 free_list_cadre(highlist,NO_DEL_CADRE);
}

void draw_cadre(This,lcadre)
c_FrMg	*This;
l_Cadre	*lcadre;	
{
 l_Cadre   *lowsort;	
 l_Cadre   *lowlist;	
 l_Cadre   *highlist;	
 l_Cadre   *tmp_list;
 l_Cadre   *tmp_list2;
 l_Cadre   *transp;
 c_Cadre   *this_cadre;
 c_Cadre   **lowcadre;
 Polygone  *forme;
 Polygone  *lowforme;
 Polygone  *ftransp;
 Polygone  *highforme;
 int	count_cadre;
 int	i;

 Xc_TRACE(("draw cadre %s", lcadre->name));
 this_cadre = lcadre->cadre;
 list_cadre_low_high_priority(This, lcadre, &lowlist, &highlist,INTERS_CADRE);
 forme = DuplicatePolygone(this_cadre->Forme);
  
 tmp_list = highlist;
 transp = NULL;
 while(tmp_list)
 {
  if (tmp_list->cadre->inside_color->transparency)
   add_cadre_in_list(&transp, tmp_list);
  else
   SubtractPolygone(&forme, tmp_list->cadre->Forme);
  tmp_list = tmp_list->NextCadre;
 }
 highforme = DuplicatePolygone(forme);
  
 if (this_cadre->inside_color->transparency)
 {
  lowsort = sort_desc_list_cadre(lowlist);
  tmp_list = lowsort;
  count_cadre = 0;
  while(tmp_list)
  {
   count_cadre++;
   tmp_list = tmp_list->NextCadre;
  }
  if (count_cadre)
  {
   lowcadre = (c_Cadre **)Xc_malloc("mv lcadre",
				    sizeof(c_Cadre *)*count_cadre);
   tmp_list = lowsort;
   for(i = 0; i < count_cadre; i++)
   {
    lowcadre[i] = (c_Cadre *)Xc_malloc("mv cadre",sizeof(c_Cadre));
    memcpy(lowcadre[i], tmp_list->cadre, sizeof(c_Cadre));
    lowforme = IntersectPolygone(forme, lowcadre[i]->Forme);
    SubtractPolygone(&forme, lowforme);
    lowcadre[i]->Forme = lowforme;
    tmp_list = tmp_list->NextCadre;
   }
   free_list_cadre(lowsort, NO_DEL_CADRE);
   tmp_list = lowsort=sort_asc_list_cadre(lowlist);
   for(i = count_cadre - 1; i >= 0; i--)
   {
    F(tmp_list->cadre).drawcadre(tmp_list->cadre, 
				 lowcadre[i]->Forme);
    F(tmp_list->cadre).drawcontour(tmp_list->cadre,
				   lowcadre[i]->Forme,
				   This->Frame_Border_actif);
    FreePolygone(lowcadre[i]->Forme, DEL_POLYGONE);
    Xc_free(lowcadre[i]);
    tmp_list=tmp_list->NextCadre;
   }
   Xc_free(lowcadre);
   free_list_cadre(lowsort, NO_DEL_CADRE);
  }
 }
  
 /*  DrawPolygone(forme, This->X_info); */
 This->redraw_polygone_callback(This->callback_data, forme);
  
 F(this_cadre).drawcadre(this_cadre, highforme);
 F(this_cadre).drawcontour(this_cadre,highforme,
			   This->Frame_Border_actif);
 FreePolygone(forme, DEL_POLYGONE);
  
 free_list_cadre(lowlist, NO_DEL_CADRE);
 free_list_cadre(highlist, NO_DEL_CADRE);
  
 tmp_list = transp;
 while(tmp_list)
 {
  list_cadre_low_high_priority(This,lcadre, &lowlist,
			       &highlist, INTERS_CADRE);
  ftransp = IntersectPolygone(tmp_list->cadre->Forme, highforme);
  tmp_list2 = highlist;
  while(tmp_list2)
  {
   if (!tmp_list2->cadre->inside_color->transparency)
    SubtractPolygone(&ftransp, tmp_list2->cadre->Forme);
   tmp_list2=tmp_list2->NextCadre;
  }
  F(tmp_list->cadre).drawcadre(tmp_list->cadre, ftransp);
  F(tmp_list->cadre).drawcontour(tmp_list->cadre, ftransp,
				 This->Frame_Border_actif);

  free_list_cadre(lowlist, NO_DEL_CADRE);
  free_list_cadre(highlist, NO_DEL_CADRE);
  FreePolygone(ftransp, DEL_POLYGONE);
  tmp_list = tmp_list->NextCadre;
 }

 if (transp)
  free_list_cadre(transp, NO_DEL_CADRE);
 FreePolygone(highforme, DEL_POLYGONE);
}

void redraw_rectangle_frmg(This, x1, y1, x2, y2)
c_FrMg	*This;
int x1;
int y1;
int x2;
int y2;
{
 Polygone *rectangle;
 Polygone *forme;
 l_Cadre *ListCadre;
 l_Cadre *lcadre;
 l_Cadre *tmpcadre;
 int i,j;
  
 rectangle=CreateRectPolygone(x1,y1,x2,y2);
 lcadre=ListCadre=sort_asc_list_cadre(This->ListCadre);
  
 for(i=0;i<This->nb_cadre;i++)
 {
  forme=IntersectPolygone(ListCadre->cadre->Forme,rectangle);
  if (forme->memory)
  {
   tmpcadre=ListCadre;
   for(j=i+1;j<This->nb_cadre;j++)
   {
    tmpcadre=tmpcadre->NextCadre;
    if (!tmpcadre->cadre->inside_color->transparency)
     SubtractPolygone(&forme,tmpcadre->cadre->Forme);
   }
   if (This->redraw_cache_actif)
   {
    add_to_draw_cache(This, ListCadre, forme);
   }
   else
   {
    F(ListCadre->cadre).drawcadre(ListCadre->cadre,forme);
    if (!ListCadre->cadre->in_work)
     F(ListCadre->cadre).drawcontour(ListCadre->cadre,forme,
				     This->Frame_Border_actif);
   }
  }
  FreePolygone(forme,DEL_POLYGONE);
  ListCadre=ListCadre->NextCadre;
 }
 free_list_cadre(lcadre,NO_DEL_CADRE);
 FreePolygone(rectangle,DEL_POLYGONE);

 /*  RectangleDisplaySelectedlCadre(This,x1,y1,x2-x1+1,y2-y1+1);  */
}

static void start_draw_cache(This)
c_FrMg *This;
{
 This->redraw_cache_actif = TRUE;
 This->redraw_cache = NULL;
 /*  UndisplaySelectedlCadre(This); */
}

static void add_to_draw_cache(This, lcadre, polygone)
c_FrMg		*This;
l_Cadre		*lcadre;
Polygone	*polygone;
{
 draw_cache_t	*cache;
 draw_cache_t	*cache2;
  
 cache2 = This->redraw_cache;
 while(cache2 && (cache2->lcadre->cadre != lcadre->cadre))
 {
  cache2 = cache2->Next;
 }
  
 if (cache2)
 {
  UnionPolygone(&(cache2->polygone), polygone);
 }
 else
 {
  cache = (draw_cache_t *)Xc_malloc("draw cache", sizeof(draw_cache_t));
  cache->lcadre = (l_Cadre *)Xc_malloc("cache lcadre", sizeof(l_Cadre));
  memcpy(cache->lcadre, lcadre, sizeof(l_Cadre));
  cache->polygone = DuplicatePolygone(polygone);
  cache->Next = NULL;
    
  cache2 = This->redraw_cache;
  if (cache2)
  {
   if (cache2->lcadre->priority > lcadre->priority)
   {
    This->redraw_cache = cache;
    cache->Next = cache2;
   }
   else
   {
    while(cache2->Next && 
	  cache2->Next->lcadre->priority < lcadre->priority)
     cache2 = cache2->Next;
    cache->Next = cache2->Next;
    cache2->Next = cache;
   }
  }
  else
  {
   This->redraw_cache = cache;
  }
 }
}

static void flush_draw_cache(This)
c_FrMg *This;
{
 draw_cache_t	*cache;
 draw_cache_t	*cache2;
 c_Cadre	*cadre;
  
 cache = This->redraw_cache;
 while(cache)
 {
  cadre = cache->lcadre->cadre;
  F(cadre).drawcadre(cadre, cache->polygone);
  if (!cadre->in_work)
   F(cadre).drawcontour(cadre, cache->polygone, This->Frame_Border_actif);
  cache2 = cache->Next;
  FreePolygone(cache->polygone, DEL_POLYGONE);
  Xc_free(cache->lcadre);
  Xc_free(cache);
  cache = cache2;
 }
 /*  DisplaySelectedlCadre(This); */
 This->redraw_cache_actif = FALSE;
}

/* --------------------------------------------------------------------- */
/* ---------------  functions of (de)selection Frames  ----------------- */ 
/* --------------------------------------------------------------------- */

void select_cadre(This,lcadre,draw)
c_FrMg	*This;
l_Cadre	*lcadre;
boolean	draw;
{
 l_Cadre	*duplist;
 l_Cadre	*svglist;
 l_Cadre	*newone;
 l_Cadre	*seektend;
 boolean	reaff=FALSE;
 coord_t	x_min,y_min,x_max,y_max;
  
 if (!lcadre->cadre->selected)
 {
  duplist=(l_Cadre *)duplicate_list_cadre(This->SelectedCadre);
  newone=(l_Cadre *)Xc_malloc("sel lcadre",sizeof(l_Cadre));
  memcpy(newone,lcadre,sizeof(l_Cadre));
  newone->NextCadre=NULL;
  lcadre->cadre->selected=TRUE;
  seektend=This->SelectedCadre;
  if (seektend)
  {
   while(seektend->NextCadre)
   {
    seektend=seektend->NextCadre;
   }
   seektend->NextCadre=newone;
  }
  else
  {
   This->SelectedCadre=newone;
  }
  get_limit_list_cadre(This->SelectedCadre,&x_min,&y_min,&x_max,&y_max);
  if (draw)
  {
   if (This->select_type || !This->select_group)
    DisplaySelectedCadre(lcadre,This->select_type);
   else
    if (This->select_sX_min!=x_min || This->select_sY_min!=y_min ||
	This->select_sX_max!=x_max || This->select_sY_max!=y_max)
    {
     if (seektend)
     {
      svglist=This->SelectedCadre;
      This->SelectedCadre=duplist;
      F(This).undisplay_selected_lcadre(This);
      This->SelectedCadre=svglist;
     }
     reaff=TRUE;
    }
  }
  This->select_sX_min=x_min;
  This->select_sY_min=y_min;
  This->select_sX_max=x_max;
  This->select_sY_max=y_max;
  if (reaff)
   F(This).display_selected_lcadre(This);
  free_list_cadre(duplist,NO_DEL_CADRE);
  if (This->SelectedCadre)
   This->select_cadre_callback(This->callback_data);
 }
}

void deselect_cadre(This, lcadre, draw)
c_FrMg	*This;
l_Cadre	*lcadre;
boolean	draw;
{
 l_Cadre	*seeklcd;
 l_Cadre	*preclcd;
 boolean	found=FALSE;
 boolean	reaff=FALSE;
 coord_t	x_min,y_min,x_max,y_max;
  
 preclcd=seeklcd=This->SelectedCadre;
 while(seeklcd)
 {
  if (seeklcd->cadre==lcadre->cadre)
  {
   found=TRUE;
   break;
  }
  preclcd=seeklcd;
  seeklcd=seeklcd->NextCadre;
 }
 if (draw)
 {
  if (This->select_type || !This->select_group)
   UndisplaySelectedCadre(lcadre,This->select_type);
  else
  {
   F(This).undisplay_selected_lcadre(This);
   reaff=TRUE;
  }
 }
 if (found)
 {
  if (preclcd==seeklcd)
  {
   This->SelectedCadre=seeklcd->NextCadre;
  }
  else
  {
   preclcd->NextCadre=seeklcd->NextCadre;
  }
  Xc_free(seeklcd);
    
  get_limit_list_cadre(This->SelectedCadre,&x_min,&y_min,&x_max,&y_max);
  This->select_sX_min=x_min;
  This->select_sY_min=y_min;
  This->select_sX_max=x_max;
  This->select_sY_max=y_max;
  if (reaff)
   F(This).display_selected_lcadre(This);
  lcadre->cadre->selected=FALSE;
  /*    if (!This->SelectedCadre)
	This->deselect_cadre_callback(This->callback_data); */
 }
}

void select_all_cadre(This)
c_FrMg	*This;
{
 l_Cadre	*lcadre;
 coord_t	x_min,y_min,x_max,y_max;
  
 UndisplaySelectedlCadre(This);
 This->SelectedCadre=duplicate_list_cadre(This->ListCadre);
 lcadre=This->SelectedCadre;
 while(lcadre)
 {
  lcadre->cadre->selected=TRUE;
  lcadre=lcadre->NextCadre;
 }
 get_limit_list_cadre(This->SelectedCadre,&x_min,&y_min,&x_max,&y_max);
 This->select_sX_min=x_min;
 This->select_sY_min=y_min;
 This->select_sX_max=x_max;
 This->select_sY_max=y_max;
 DisplaySelectedlCadre(This);
 if (This->SelectedCadre)
  This->select_cadre_callback(This->callback_data);
}

void deselect_all_cadre(This, draw)
c_FrMg	*This;
boolean	draw;
{
 l_Cadre	*lcadre;
  
 if (draw)
  UndisplaySelectedlCadre(This);
 lcadre=This->SelectedCadre;
 while(lcadre)
 {
  lcadre->cadre->selected=FALSE;
  lcadre=lcadre->NextCadre;
 }
 free_list_cadre(This->SelectedCadre,NO_DEL_CADRE);
 This->SelectedCadre=NULL;
 This->select_sX_min=This->select_sY_min=
  This->select_sX_max=This->select_sY_max=0;
 /*  if (!This->SelectedCadre)
     This->deselect_cadre_callback(This->callback_data); */
}

/* --------------------------------------------------------------------- */
/* -----------------------  Display functions  ------------------------- */
/* --------------------------------------------------------------------- */

void RectangleDisplaySelectedlCadre(This, x, y, w, h)
c_FrMg	*This;
unsigned short	x;
unsigned short	y;
unsigned short	w;
unsigned short	h;
{
 l_Cadre *tmpcadre;
 c_Cadre *cadre;
 XRectangle	rect;
  
 rect.x=x;
 rect.y=y;
 rect.width=w;
 rect.height=h;
 XSetClipRectangles(_display, _gc_contour, 0, 0, &rect, 1, Unsorted);
 if (This->select_type || !This->select_group)
 {
  tmpcadre=This->SelectedCadre;
  while(tmpcadre)
  {
   if (!tmpcadre->cadre->in_work)
   {
    XCopyGC(_display,_gc_contour,(GCClipMask|GCClipXOrigin|GCClipYOrigin),
	    tmpcadre->cadre->X_info.gc_contour);
    if (This->select_type)
     F(tmpcadre->cadre).draw_sommet(tmpcadre->cadre);
    else
     F(tmpcadre->cadre).draw_handle(tmpcadre->cadre);
    XSetClipMask(_display,tmpcadre->cadre->X_info.gc_contour,None);
   }
   tmpcadre=tmpcadre->NextCadre;
  }
 }
 else
  if (This->SelectedCadre && !This->SelectedCadre->cadre->in_work)
  {
   cadre=This->SelectedCadre->cadre;
   if ((!This->SelectedCadre->NextCadre) && 
       ((!cadre->ListPoint->NextPoint) || 
	(!cadre->ListPoint->NextPoint->NextPoint) ||
	(!cadre->sWidth) || (!cadre->sHeight)))
    F(cadre).draw_sommet(cadre);
   else
    draw_handle(This);
  }
 if(This->function_data)
  REDRAW(This);
 XSetClipMask(_display, _gc_contour, None);
}

void DisplaySelectedlCadre(This)
c_FrMg	*This;
{
 l_Cadre *tmpcadre;
 c_Cadre *cadre;
  
 if (This->select_type || !This->select_group)
 {
  tmpcadre=This->SelectedCadre;
  while(tmpcadre)
  {
   if (This->select_type)
    F(tmpcadre->cadre).draw_sommet(tmpcadre->cadre);
   else
    F(tmpcadre->cadre).draw_handle(tmpcadre->cadre);
   tmpcadre=tmpcadre->NextCadre;
  }
 }
 else
  if (This->SelectedCadre && !This->SelectedCadre->cadre->in_work)
  {
   cadre=This->SelectedCadre->cadre;
   if ((!This->SelectedCadre->NextCadre) && 
       ((!cadre->ListPoint->NextPoint) || 
	(!cadre->ListPoint->NextPoint->NextPoint) ||
	(!cadre->sWidth) || (!cadre->sHeight)))
   {
    F(cadre).draw_sommet(cadre);
   }
   else
   {
    draw_handle(This);
   }
  }
}

void UndisplaySelectedlCadre(This)
c_FrMg	*This;
{
 l_Cadre *tmpcadre;
 c_Cadre *cadre;
  
 if (This->select_type || !This->select_group)
 {
  tmpcadre=This->SelectedCadre;
  while(tmpcadre)
  {
   if (This->select_type)
    F(tmpcadre->cadre).draw_sommet(tmpcadre->cadre);
   else
    F(tmpcadre->cadre).draw_handle(tmpcadre->cadre);
   tmpcadre=tmpcadre->NextCadre;
  }
 }
 else
  if (This->SelectedCadre && !This->SelectedCadre->cadre->in_work)
  {
   cadre=This->SelectedCadre->cadre;
   if ((!This->SelectedCadre->NextCadre) && 
       ((!cadre->ListPoint->NextPoint) || 
	(!cadre->ListPoint->NextPoint->NextPoint) ||
	(!cadre->sWidth) || (!cadre->sHeight)))
   {
    F(cadre).draw_sommet(cadre);
   }
   else
   {
    draw_handle(This);
   }
  }
}

void DisplaySelectedCadre(This,witch)
l_Cadre	*This;
boolean	witch;
{
 if (This->cadre->selected)
 {
  if (witch)
   F(This->cadre).draw_sommet(This->cadre);
  else
   F(This->cadre).draw_handle(This->cadre);
 }
}

void UndisplaySelectedCadre(This,witch)
l_Cadre	*This;
boolean	witch;
{
 if (This->cadre->selected)
 {
  if (witch)
   F(This->cadre).draw_sommet(This->cadre);
  else
   F(This->cadre).draw_handle(This->cadre);
 }
}

void draw_handle(This)
c_FrMg 	*This;
{
 int x_min,y_min,x_mid,y_mid,x_max,y_max;
  
 x_min=This->select_sX_min/This->sScale;
 y_min=This->select_sY_min/This->sScale;
 x_max=This->select_sX_max/This->sScale;
 y_max=This->select_sY_max/This->sScale;
 x_mid=(x_max+x_min)>>1;
 y_mid=(y_max+y_min)>>1;
 TRACE_RHANDLE(_display,_window,_gc_contour,x_min-RHANDLE_X-1,
	       y_min-RHANDLE_Y-1);
 TRACE_RHANDLE(_display,_window,_gc_contour,x_min-RHANDLE_X-1,
	       y_max+RHANDLE_Y+1);
 TRACE_RHANDLE(_display,_window,_gc_contour,x_max+RHANDLE_X+1,
	       y_min-RHANDLE_Y-1);
 TRACE_RHANDLE(_display,_window,_gc_contour,x_max+RHANDLE_X+1,
	       y_max+RHANDLE_Y+1);
 TRACE_RHANDLE(_display,_window,_gc_contour,x_mid,y_min-RHANDLE_Y-1);
 TRACE_RHANDLE(_display,_window,_gc_contour,x_mid,y_max+RHANDLE_Y+1);
 TRACE_RHANDLE(_display,_window,_gc_contour,x_min-RHANDLE_X-1,y_mid);
 TRACE_RHANDLE(_display,_window,_gc_contour,x_max+RHANDLE_X+1,y_mid);
}

/* --------------------------------------------------------------------- */
/* ----------------------  functions on l_Cadre  ----------------------- */
/* --------------------------------------------------------------------- */

void free_list_cadre(lcadre,iscadre)
l_Cadre	*lcadre;
boolean	iscadre;
{
 l_Cadre *tmp;
 l_Cadre *tmp2;

 tmp=lcadre;
 while(tmp)
 {
  tmp2=tmp->NextCadre;
  if (iscadre==DEL_CADRE)
  {
   DELETE(c_Cadre)(tmp->cadre);
  }
  Xc_free(tmp);
  tmp=tmp2;
 }
}

void get_center_list_cadre(lcadre,xcent,ycent)
l_Cadre	*lcadre;
coord_t	*xcent;
coord_t	*ycent;
{
 coord_t	x_min;
 coord_t	y_min;
 coord_t	x_max;
 coord_t	y_max;
  
 x_min=lcadre->cadre->sOrig_X;
 x_max=x_min+lcadre->cadre->sWidth;
 y_min=lcadre->cadre->sOrig_Y;
 y_max=y_min+lcadre->cadre->sHeight;
 lcadre=lcadre->NextCadre;
 while(lcadre)
 {
  if (x_min>lcadre->cadre->sOrig_X)
   x_min=lcadre->cadre->sOrig_X;
  if (x_max<lcadre->cadre->sOrig_X+lcadre->cadre->sWidth)
   x_max=lcadre->cadre->sOrig_X+lcadre->cadre->sWidth;
  if (y_min>lcadre->cadre->sOrig_Y)
   y_min=lcadre->cadre->sOrig_Y;
  if (y_max<lcadre->cadre->sOrig_Y+lcadre->cadre->sHeight)
   y_max=lcadre->cadre->sOrig_Y+lcadre->cadre->sHeight;
  lcadre=lcadre->NextCadre;
 } 
 *xcent=(x_max+x_min)>>1;
 *ycent=(y_max+y_min)>>1;
}

void get_limit_list_cadre(lcadre,x_min,y_min,x_max,y_max)
l_Cadre	*lcadre;
coord_t	*x_min;
coord_t	*y_min;
coord_t	*x_max;
coord_t	*y_max;
{
 if (!lcadre)
 {
  *x_min= *x_max= *y_min= *y_max=0;
 }
 else
 {
  *x_min=lcadre->cadre->sOrig_X;
  *x_max= *x_min+lcadre->cadre->sWidth;
  *y_min=lcadre->cadre->sOrig_Y;
  *y_max= *y_min+lcadre->cadre->sHeight;
  lcadre=lcadre->NextCadre;
  while(lcadre)
  {
   if (*x_min>lcadre->cadre->sOrig_X)
    *x_min=lcadre->cadre->sOrig_X;
   if (*x_max<lcadre->cadre->sOrig_X+lcadre->cadre->sWidth)
    *x_max=lcadre->cadre->sOrig_X+lcadre->cadre->sWidth;
   if (*y_min>lcadre->cadre->sOrig_Y)
    *y_min=lcadre->cadre->sOrig_Y;
   if (*y_max<lcadre->cadre->sOrig_Y+lcadre->cadre->sHeight)
    *y_max=lcadre->cadre->sOrig_Y+lcadre->cadre->sHeight;
   lcadre=lcadre->NextCadre;
  } 
 }
}

void add_cadre_in_list(lcadre,lcadre2)
l_Cadre	**lcadre;
l_Cadre	*lcadre2;
{
 l_Cadre	*tmplcadre;
 l_Cadre	*ptrlcadre;
  
 if (lcadre2)
 {
  tmplcadre=(l_Cadre *)Xc_malloc("addlcadre",sizeof(l_Cadre));
  memcpy(tmplcadre,lcadre2,sizeof(l_Cadre));
  tmplcadre->NextCadre=NULL;
      
  if (*lcadre)
  {
   ptrlcadre = *lcadre;
   while(ptrlcadre->NextCadre)
    ptrlcadre=ptrlcadre->NextCadre;
   ptrlcadre->NextCadre=tmplcadre;
  }
  else
   *lcadre=tmplcadre;
 }
}

void delete_cadre_in_list(lcadre,cadre)
l_Cadre	**lcadre;
c_Cadre	*cadre;
{
 l_Cadre	*tmp;
 l_Cadre	*previous;
    
 tmp= *lcadre;
 previous=NULL;
 while(tmp)
 {
  if (tmp->cadre==cadre)
  {
   if (previous)
    previous->NextCadre=tmp->NextCadre;
   else
    *lcadre=tmp->NextCadre;
   Xc_free(tmp);
   return;
  }
  previous=tmp;
  tmp=tmp->NextCadre;
 }
}

l_Cadre *list_cadre_inters(This, my_cadre)
c_FrMg *This;
l_Cadre *my_cadre;
{
 l_Cadre *list_cdr;
 l_Cadre *ret_cdr;
 l_Cadre *lcadre;
 boolean prems;
  
 prems=TRUE;
 lcadre=This->ListCadre;
 list_cdr=ret_cdr=NULL;
 while(lcadre)
 {
  if ((lcadre->cadre==my_cadre->cadre) ||
      !is_inters(lcadre->cadre->Forme,my_cadre->cadre->Forme))
  {
   lcadre=lcadre->NextCadre;
  }
  else
  {
   if (prems)
   {
    list_cdr=(l_Cadre *)Xc_malloc("lcadre inters",sizeof(l_Cadre));
    ret_cdr=list_cdr;
    memcpy(list_cdr,lcadre,sizeof(l_Cadre));
    prems=FALSE;
   }
   else
   {
    list_cdr->NextCadre = 
     (l_Cadre *)Xc_malloc("lcadre inters", sizeof(l_Cadre));
    list_cdr=list_cdr->NextCadre;
    memcpy(list_cdr,lcadre,sizeof(l_Cadre));
   }
   lcadre=lcadre->NextCadre;
  }
 }
 if (list_cdr)
 {
  list_cdr->NextCadre=NULL;
  return ret_cdr;
 }
 else
  return NULL;
}

l_Cadre *duplicate_list_cadre(lcadre)
l_Cadre	*lcadre;
{
 l_Cadre	*tmplist;
 l_Cadre	*returnl;
 l_Cadre	*retlist;
 boolean	prems;
  
 prems=TRUE;
 tmplist=lcadre;
 if (tmplist)
 {
  retlist=(l_Cadre *)Xc_malloc("dup lcadre",sizeof(l_Cadre));
  returnl=retlist;
  while(tmplist)
  {
   memcpy(retlist,tmplist,sizeof(l_Cadre));
   tmplist=tmplist->NextCadre;
   if (tmplist)
   {
    retlist->NextCadre=(l_Cadre *)Xc_malloc("dup lcadre",sizeof(l_Cadre));
    retlist=retlist->NextCadre;
   }
   else
    retlist->NextCadre=NULL;
  }
  return returnl;
 }
 else
  return NULL;
}

void list_cadre_low_high_priority(This, lcadre, lowlist, highlist, isall)
c_FrMg	*This;
l_Cadre	*lcadre;
l_Cadre	**lowlist;
l_Cadre	**highlist;
boolean	isall;
{
 l_Cadre *list_cadre;
 l_Cadre *tmp_cadre;
 l_Cadre *lowcadre;
 l_Cadre *highcadre;
 int	  prior;
  
 if (isall==INTERS_CADRE)
  list_cadre=(l_Cadre *)list_cadre_inters(This,lcadre);
 else
 {
  list_cadre=(l_Cadre *)duplicate_list_cadre(This->ListCadre);
  delete_cadre_in_list(&list_cadre,lcadre->cadre);
 }
 tmp_cadre=list_cadre;
 prior=lcadre->priority;
 lowcadre=highcadre=NULL;
 *lowlist= *highlist=NULL;
 while(tmp_cadre)
 {
  if (tmp_cadre->priority<prior)
  {
   if (lowcadre)
   {
    lowcadre->NextCadre=(l_Cadre *)Xc_malloc("low cadre",sizeof(l_Cadre));
    memcpy(lowcadre->NextCadre,tmp_cadre,sizeof(l_Cadre));
    lowcadre=lowcadre->NextCadre;
    lowcadre->NextCadre=NULL;
   }
   else
   {
    lowcadre=(l_Cadre *)Xc_malloc("low cadre",sizeof(l_Cadre));
    memcpy(lowcadre,tmp_cadre,sizeof(l_Cadre));
    *lowlist=lowcadre;
    lowcadre->NextCadre=NULL;
   }
  }
  else
  {
   if (highcadre)
   {
    highcadre->NextCadre = 
     (l_Cadre *)Xc_malloc("high cadre",sizeof(l_Cadre));
    memcpy(highcadre->NextCadre,tmp_cadre,sizeof(l_Cadre));
    highcadre=highcadre->NextCadre;
    highcadre->NextCadre=NULL;
   }
   else
   {
    highcadre=(l_Cadre *)Xc_malloc("high cadre",sizeof(l_Cadre));
    memcpy(highcadre,tmp_cadre,sizeof(l_Cadre));
    *highlist=highcadre;
    highcadre->NextCadre=NULL;
   }
  }
  tmp_cadre=tmp_cadre->NextCadre;
 }
 free_list_cadre(list_cadre,NO_DEL_CADRE);
}

l_Cadre *sort_asc_list_cadre(list)
l_Cadre *list;
{
 l_Cadre *tmplist;
 l_Cadre **tablist;
 l_Cadre *swplist;
 boolean utils;
 int priority = 0;
 int nb_cadre;
 int pivo;
 int i,j;
  
 tmplist=list;
 nb_cadre=0;
 utils=TRUE;
 while(tmplist)
 {
  if (nb_cadre)
  {
   if (priority>tmplist->priority)
    utils=FALSE;
  }
  priority=tmplist->priority;
  nb_cadre++;
  tmplist=tmplist->NextCadre;
 }
 if (utils)
 {
  tmplist=duplicate_list_cadre(list);
  return tmplist;
 }
 tablist=(l_Cadre **)Xc_malloc("sort lcadre",sizeof(l_Cadre *)*nb_cadre);
 tmplist=list;
 i=0;
 while(tmplist)
 {
  tablist[i]=(l_Cadre *)Xc_malloc("sort cadre",sizeof(l_Cadre));
  memcpy(tablist[i++],tmplist,sizeof(l_Cadre));
  tmplist=tmplist->NextCadre;
 }
 for(i=0;i<nb_cadre-1;i++)
 {
  priority=tablist[i]->priority;
  pivo=i;
  for(j=i+1;j<nb_cadre;j++)
  {
   if (tablist[j]->priority<priority)
   {
    priority=tablist[j]->priority;
    pivo=j;
   }
  }
  swplist=tablist[pivo];
  tablist[pivo]=tablist[i];
  tablist[i]=swplist;
 }
 for(i=0;i<nb_cadre-1;i++)
 {
  tablist[i]->NextCadre=tablist[i+1];
 }
 tablist[i]->NextCadre=NULL;
 tmplist=tablist[0];
 Xc_free(tablist);
 return tmplist;
}

l_Cadre *sort_desc_list_cadre(list)
l_Cadre *list;
{
 l_Cadre *tmplist;
 l_Cadre **tablist;
 l_Cadre *swplist;
 boolean utils;
 int priority = 0;
 int nb_cadre;
 int pivo;
 int i,j;
  
 tmplist=list;
 nb_cadre=0;
 utils=TRUE;
 while(tmplist)
 {
  if (nb_cadre)
  {
   if (priority<tmplist->priority)
    utils=FALSE;
  }
  priority=tmplist->priority;
  nb_cadre++;
  tmplist=tmplist->NextCadre;
 }
 if (utils)
 {
  tmplist=duplicate_list_cadre(list);
  return tmplist;
 }
 tablist=(l_Cadre **)Xc_malloc("sort lcadre",sizeof(l_Cadre *)*nb_cadre);
 tmplist=list;
 i=0;
 while(tmplist)
 {
  tablist[i]=(l_Cadre *)Xc_malloc("sort cadre",sizeof(l_Cadre));
  memcpy(tablist[i++],tmplist,sizeof(l_Cadre));
  tmplist=tmplist->NextCadre;
 }
 for(i=0;i<nb_cadre;i++)
 {
  priority=tablist[i]->priority;
  pivo=i;
  for(j=i+1;j<nb_cadre;j++)
  {
   if (tablist[j]->priority>priority)
   {
    priority=tablist[j]->priority;
    pivo=j;
   }
  }
  swplist=tablist[pivo];
  tablist[pivo]=tablist[i];
  tablist[i]=swplist;
 }
 for(i=0;i<nb_cadre-1;i++)
 {
  tablist[i]->NextCadre=tablist[i+1];
 }
 tablist[i]->NextCadre=NULL;
 tmplist=tablist[0];
 Xc_free(tablist);
 return tmplist;
}

l_Cadre *get_lcadre_by_cadre(This,cadre)
c_FrMg	*This;
c_Cadre	*cadre;
{
 l_Cadre	*lcadre;
  
 lcadre=This->ListCadre;
 while(lcadre)
 {
  if (lcadre->cadre==cadre)
   return lcadre;
  lcadre=lcadre->NextCadre;
 }
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** get_bbox_list_cadre - get the bounding box of a list of cadre     ** 
** ----------------------------------------------------------------- */
static boolean get_bbox_list_cadre(This, bboxp, lcadre)
c_FrMg	*This;
bbox_t *bboxp;
l_Cadre	*lcadre;
{
 coord_t  	c_x_min;
 coord_t  	c_x_max;
 coord_t  	c_y_min;
 coord_t  	c_y_max;

 if(lcadre == NULL) lcadre = This->ListCadre;
 if(lcadre == NULL) return FALSE;

 bboxp->llx = lcadre->cadre->sOrig_X;
 bboxp->lly = lcadre->cadre->sOrig_Y;
 bboxp->urx = bboxp->llx + lcadre->cadre->sWidth;
 bboxp->ury = bboxp->lly + lcadre->cadre->sHeight;
  
 for(lcadre = lcadre->NextCadre; lcadre != NULL; lcadre = lcadre->NextCadre)
 {
  c_x_min = lcadre->cadre->sOrig_X;
  c_y_min = lcadre->cadre->sOrig_Y;
  c_x_max = c_x_min + lcadre->cadre->sWidth;
  c_y_max = c_y_min + lcadre->cadre->sHeight;

  if (bboxp->llx > c_x_min) bboxp->llx = c_x_min;
  if (bboxp->lly > c_y_min) bboxp->lly = c_y_min;
  if (bboxp->urx < c_x_max) bboxp->urx = c_x_max;
  if (bboxp->ury < c_y_max) bboxp->ury = c_y_max;
 }

 bboxp->llx -= This->sOrig_X;
 bboxp->lly -= This->sOrig_Y;
 bboxp->urx -= This->sOrig_X;
 bboxp->ury -= This->sOrig_Y;
  
 return TRUE;
}


/* --------------------------------------------------------------------- */
/* -----------------  functions for object calls  ---------------------- */
/* --------------------------------------------------------------------- */

void  object_redraw_cadre(cfrmg, cadre)
c_FrMg	*cfrmg;
c_Cadre	*cadre;
{
 l_Cadre	*lcadre;
  
 F(cfrmg).undisplay_selected_lcadre(cfrmg);
 lcadre=get_lcadre_by_cadre(cfrmg,cadre);
 if (lcadre)
  F(cfrmg).draw_cadre(cfrmg,lcadre);
 F(cfrmg).display_selected_lcadre(cfrmg);
}

void  object_redraw_rectangle(cfrmg,cadre,x1,y1,x2,y2)
c_FrMg	*cfrmg;
c_Cadre	*cadre;
coord_t	x1,y1,x2,y2;
{
 l_Cadre	*lcadre;
  
 lcadre=get_lcadre_by_cadre(cfrmg,cadre);
}

Polygone *object_get_clip_polygone(cfrmg,cadre)
c_FrMg	*cfrmg;
c_Cadre	*cadre;
{
 l_Cadre	*lcadre;
 l_Cadre   *lowlist;	
 l_Cadre   *highlist;	
 l_Cadre   *tmp_list;
 c_Cadre   *this_cadre;
 Polygone  *forme;
  
 lcadre=get_lcadre_by_cadre(cfrmg,cadre);
 this_cadre=lcadre->cadre;
 list_cadre_low_high_priority(cfrmg,lcadre,&lowlist,&highlist,INTERS_CADRE);
 tmp_list=highlist;
 forme=DuplicatePolygone(this_cadre->Forme);
 while(tmp_list)
 {
  if (!tmp_list->cadre->inside_color->transparency)
   SubtractPolygone(&forme,tmp_list->cadre->Forme);
  tmp_list=tmp_list->NextCadre;
 }
 free_list_cadre(lowlist,NO_DEL_CADRE);
 free_list_cadre(highlist,NO_DEL_CADRE);
 return forme;
}

