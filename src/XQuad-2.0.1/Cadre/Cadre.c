/*
** Cadre.c for Xclamation, XQuad and XAllWrite in Cadre/
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
** Started on  Mon Jul 17 14:08:22 1995 Emmanuel Paris
** Last update Thu Jun  3 20:04:26 1999 Emmanuel Paris
*/

#include "CadreP.h"
#include "ObjectStd.h"

/* #define NMEMDEBUG */

void *cons_Cadre();
void dest_Cadre();
void *copy_Cadre();
void Caddpoint();
void Caddspoint();
void Cinsertpoint();
void Cinsertspoint();
error Cdeletepoint();
l_Point *Cgetpoint();
l_Point *Cgetpreviouspoint();
l_Point *Cgetnextpoint();
void Cviewpoint();

void free_list_point();
l_Point *duplicate_list_point();
void trace_list_point();
void rotate_list_point();
void scale_list_point();
void get_limit_list_point();

void CfusionCadre();

void CattachObject();
void CdetachObject();

boolean Cisincadre();
void Cset_X_info();
void Creinit_X_info();
void CcalculOrigine();
void CcalculScaling();
void CcalculForme();

void Csetlinewidth();
static boolean setBackgroundColor ___PROTO((c_Cadre *this, c_Color *color));
static boolean setOutlineColor ___PROTO((c_Cadre *this, c_Color *color));
void Cset_contour_action();
void Cdrawcontour();
void Cdrawcadre();
void Cdrawselect();
void Cdrawsommet();
void Cdrawhandle();

void FreeLigne();
Ligne *CalculLigne();
void TraceLigne();

extern boolean printCadre();
extern boolean printCadrePath();
extern boolean readCadre();
static boolean writeCadre ___PROTO((c_Cadre *this, c_DocFile *doc,
				    coord_t dx, coord_t dy));
static void hook_color ___PROTO((c_Cadre *this, int reason,
				 c_Color *old, c_Color *new));
static void configure_request ___PROTO((c_Cadre *This, boolean init));

sf_Cadre fc_Cadre =
{
 cons_Cadre,
 dest_Cadre,
 copy_Cadre,
 Caddpoint,
 Caddspoint,
 Cinsertpoint,
 Cinsertspoint,
 Cdeletepoint,
 Cgetpoint,
 Cgetpreviouspoint,
 Cgetnextpoint,
 Cviewpoint,
 CfusionCadre,
 CattachObject,
 CdetachObject,
 CcalculOrigine,
 CcalculScaling,
 CcalculForme,
 Cisincadre,
 Cset_X_info,
 Creinit_X_info,
 Csetlinewidth,
 setBackgroundColor,
 setOutlineColor,
 Cset_contour_action,
 Cdrawcontour,
 Cdrawcadre,
 Cdrawsommet,
 Cdrawhandle,
 hook_color,
 configure_request,
 printCadre, printCadrePath,
 readCadre, writeCadre,
};

void *cons_Cadre(BaseStd)
BaseStd_t	*BaseStd;	
{
 c_Cadre *ObjTmp;

 Xc_HISTORY(("constructor"));
  
 if((ObjTmp = Xc_malloc("Cadre", sizeof (c_Cadre))) == NULL) return NULL;
  
 memset(ObjTmp, 0, sizeof(c_Cadre));
 ObjTmp->f = &fc_Cadre;
  
 ObjTmp->BaseStd = BaseStd;
 ObjTmp->border_color =F(BaseStd->color_base).getColor(&(BaseStd->color_base),
						       XcC_NAME,"Black",
						       XcC_GRAY,
						       XcC_SCALE_COLOR(0.0),
						       XcC_TRANSPARENCY, 0,
						       XcC_END);
 if(XcH_REGISTER(ObjTmp->border_color, hook_color, ObjTmp) == FALSE)
 {
  Xc_free(ObjTmp);
  return NULL;
 }
  
 ObjTmp->inside_color =F(BaseStd->color_base).getColor(&(BaseStd->color_base),
						       XcC_NAME, "White",
						       XcC_GRAY,
						       XcC_SCALE_COLOR(1.0), 
						       XcC_TRANSPARENCY, 0,
						       XcC_END);
 if(XcH_REGISTER(ObjTmp->inside_color, hook_color, ObjTmp) == FALSE)
 {
  XcH_UNREGISTER(ObjTmp->border_color, hook_color, ObjTmp);
  Xc_free(ObjTmp);
  return NULL;
 }
  
 Xc_TRACE(("bord: %d interieur: %d",
	   ObjTmp->border_color->cell.pixel,
	   ObjTmp->inside_color->cell.pixel));
  
 ObjTmp->thickness = 0;
 ObjTmp->sOrig_X = ObjTmp->sOrig_Y = ObjTmp->sWidth = ObjTmp->sHeight = 0;
 ObjTmp->sScale = COORD_DEFAULT;
 ObjTmp->sFactor = SCALE_FROM_PERCENT(100);
 ObjTmp->sAngle = 0;
 ObjTmp->ListPoint = NULL;
 ObjTmp->Forme_calc = FALSE;
 ObjTmp->Forme = CreateNullPolygone();
 ObjTmp->X_info_calc = FALSE;
 ObjTmp->X_info.display = NULL;
 ObjTmp->X_info.window = 0;
 ObjTmp->X_info.gc_contour = 0;
 ObjTmp->X_info.gc_inside = 0;
 ObjTmp->X_info.ecran = 0;
 ObjTmp->X_info.contour_color = 0;
 ObjTmp->X_info.inside_color = 0;
 ObjTmp->object_type = CADRE_EMPTY;
 ObjTmp->object = NULL;
 ObjTmp->dirty_flag = FALSE;
 ObjTmp->in_work = FALSE;
 ObjTmp->verrou = FALSE;
 ObjTmp->selected = FALSE;
 ObjTmp->data_to_object = NULL;
 ObjTmp->sConfig_X = 0;
 ObjTmp->sConfig_Y = 0;
 ObjTmp->textflow_mode = XcTFM_AROUND;

 Xc_TRACE(("Object Cadre builded"));
 return ObjTmp;
}

void dest_Cadre(ObjectToDelete)
c_Cadre		*ObjectToDelete;
{
 l_Point *point;
 l_Point *nextp;

 Xc_HISTORY(("destructor"));

 XcH_UNREGISTER(ObjectToDelete->border_color, hook_color, ObjectToDelete);
 XcH_UNREGISTER(ObjectToDelete->inside_color, hook_color, ObjectToDelete);

 point = ObjectToDelete->ListPoint;
 while(point)
 {
  nextp = point->NextPoint;
  Xc_free(point);
  point = nextp;
 }
 FreePolygone(ObjectToDelete->Forme, DEL_POLYGONE);
 if (ObjectToDelete->X_info_calc)
 {
  XFreeGC(ObjectToDelete->X_info.display, ObjectToDelete->X_info.gc_contour);
  XFreeGC(ObjectToDelete->X_info.display, ObjectToDelete->X_info.gc_inside);
  ObjectToDelete->X_info.gc_contour = 0;
  ObjectToDelete->X_info.gc_inside = 0;
  ObjectToDelete->X_info_calc = FALSE;
 }

 if(ObjectToDelete->object_type != CADRE_EMPTY)
  DISPLAY_DELETE(ObjectToDelete->object)(ObjectToDelete->object);
 Xc_free(ObjectToDelete);

 Xc_TRACE(("destroyed"));  
}


/* ----------------------------------------------------------------- ** 
** copy_Cadre - Copy method                                          ** 
** ----------------------------------------------------------------- */
void *copy_Cadre(ObjectToCopy, BaseStd)
c_Cadre		*ObjectToCopy;
BaseStd_t	*BaseStd;
{
 c_Cadre *ObjTmp;
 c_Color *color;
  
 Xc_HISTORY(("copy"));

 if((ObjTmp = (c_Cadre *)Xc_malloc("Cadre", sizeof(c_Cadre))) == NULL)
  return NULL;
 memcpy(ObjTmp, ObjectToCopy, sizeof (c_Cadre));
 ObjTmp->f= &fc_Cadre;
  
  
 /*--- Color copies ---*/
 color = ObjectToCopy->border_color;
 if((ObjTmp->border_color = F(color).virtualCopy(color, BaseStd)) == NULL)
 {
  Xc_free(ObjTmp);
  return NULL;
 }
  
 color = ObjectToCopy->inside_color;
 if((ObjTmp->inside_color = F(color).virtualCopy(color, BaseStd)) == NULL)
 {
  Xc_free(ObjTmp);
  return NULL;
 }
  
  
 /*--- Color hooks ---*/
 if(XcH_REGISTER(ObjTmp->border_color, hook_color, ObjTmp) == FALSE)
 {
  Xc_free(ObjTmp);
  return NULL;
 }
 if(XcH_REGISTER(ObjTmp->inside_color, hook_color, ObjTmp) == FALSE)
 {
  XcH_UNREGISTER(ObjTmp->border_color, hook_color, ObjTmp);
  Xc_free(ObjTmp);
  return NULL;
 }
  
 ObjTmp->BaseStd = BaseStd;
 ObjTmp->ListPoint = duplicate_list_point(ObjectToCopy->ListPoint);
 ObjTmp->Forme = DuplicatePolygone(ObjectToCopy->Forme);
 ObjTmp->X_info.gc_contour = 0;
 ObjTmp->X_info.gc_inside = 0;
 ObjTmp->X_info_calc = FALSE;
 F(ObjTmp).reinit_X_info(ObjTmp, ObjectToCopy->X_info.display,
			 ObjectToCopy->X_info.window);
 ObjTmp->selected = FALSE;
  
 /*--- Copy content ---*/
 if(ObjectToCopy->object_type != CADRE_EMPTY)
 {
  ObjTmp->object = DISPLAY_COPY(ObjectToCopy->object)(ObjectToCopy->object,
						      BaseStd);
  if(ObjTmp->object != NULL)
  {
   if(DISPLAY_INIT(ObjTmp->object)(ObjTmp->object, ObjTmp)!=XC_NO_ERROR) 
   {
    CdetachObject(ObjTmp);
   }
   else
    DISPLAY_CONFIGURE(ObjTmp->object)(ObjTmp->object, ObjTmp,
				      ObjTmp->Forme);
  }
  else
   ObjTmp->object_type = CADRE_EMPTY;
 }
  
 Xc_TRACE(("copied"));
 return ObjTmp;
}


void Caddpoint(Object1, X1, Y1, Cont)
c_Cadre		*Object1;
int		X1;
int		Y1;
boolean         Cont;
{
 l_Point	**PointTmp;
 l_Point	*PointNew;

 PointNew=(l_Point *)Xc_malloc("Point",sizeof (l_Point));
 PointNew->X = X1;
 PointNew->Y = Y1;
 PointNew->cont = Cont;
 PointNew->sX = X1*Object1->sScale;
 PointNew->sY = Y1*Object1->sScale;
 PointNew->NextPoint = NULL;

 PointTmp= &D(Object1).ListPoint;
 while (*PointTmp!=NULL)
 {
  PointTmp= &((*PointTmp)->NextPoint);
 }
 *PointTmp = PointNew;
 Object1->Forme_calc = FALSE;
}

void Caddspoint(Object1, X1, Y1, Cont)
c_Cadre		*Object1;
coord_t		X1;
coord_t		Y1;
boolean         Cont;
{
 l_Point	**PointTmp;
 l_Point	*PointNew;

 PointNew=(l_Point *)Xc_malloc("Point",sizeof (l_Point));
 PointNew->X = X1/Object1->sScale;
 PointNew->Y = Y1/Object1->sScale;
 PointNew->sX = X1;
 PointNew->sY = Y1;
 PointNew->cont = Cont;
 PointNew->NextPoint = NULL;

 PointTmp= &D(Object1).ListPoint;
 while (*PointTmp!=NULL)
 {
  PointTmp= &((*PointTmp)->NextPoint);
 }
 *PointTmp = PointNew;
 Object1->Forme_calc = FALSE;
}

void Cinsertpoint(Object1, X1, Y1, Cont, lpoint)
c_Cadre		*Object1;
int		X1;
int		Y1;
boolean         Cont;
l_Point		*lpoint;
{
 l_Point	*lpoint2;
 l_Point	*newlpoint;
  
 lpoint2 = Object1->ListPoint;
 while (lpoint2 && lpoint2!=lpoint)
 {
  lpoint2 = lpoint2->NextPoint;
 }
 if (lpoint2!=lpoint)
 {
  Xc_WARNING(("Can't create at this position"));
 }
 else
 {
  if ((newlpoint=(l_Point *)Xc_malloc("Inserted point",
				      sizeof (l_Point)))==NULL)
   Xc_FATAL(("Can't add new point: memory allocation error"));
  newlpoint->X = X1;
  newlpoint->Y = Y1;
  newlpoint->cont = Cont;
  newlpoint->sX = X1*Object1->sScale;
  newlpoint->sY = Y1*Object1->sScale;
  newlpoint->NextPoint = lpoint2->NextPoint;
  lpoint2->NextPoint = newlpoint;
  Object1->Forme_calc = FALSE;
 }
}
void Cinsertspoint(Object1, X1, Y1, Cont, lpoint)
c_Cadre		*Object1;
coord_t		X1;
coord_t		Y1;
boolean         Cont;
l_Point		*lpoint;
{
 l_Point	*lpoint2;
 l_Point	*newlpoint;
  
 lpoint2 = Object1->ListPoint;
 while (lpoint2 && lpoint2!=lpoint)
 {
  lpoint2 = lpoint2->NextPoint;
 }
 if (lpoint2!=lpoint)
 {
  Xc_WARNING(("Can't create at this position"));
 }
 else
 {
  if ((newlpoint=(l_Point *)Xc_malloc("Inserted point",
				      sizeof (l_Point)))==NULL)
   Xc_FATAL(("Can't add new point: memory allocation error"));
  newlpoint->sX = X1;
  newlpoint->sY = Y1;
  newlpoint->cont = Cont;
  newlpoint->X = X1/Object1->sScale;
  newlpoint->Y = Y1/Object1->sScale;
  newlpoint->NextPoint = lpoint2->NextPoint;
  lpoint2->NextPoint = newlpoint;
  Object1->Forme_calc = FALSE;
 }
}

error Cdeletepoint(Object1, lpoint)
c_Cadre		*Object1;
l_Point		*lpoint;
{
 l_Point	*PointTmp;
 l_Point	*PointNew;

 PointNew = NULL;
 PointTmp= Object1->ListPoint;
 while (PointTmp && PointTmp!=lpoint)
 {
  PointNew = PointTmp;
  PointTmp= PointTmp->NextPoint;
 }
 if (!PointTmp)
 {
  Xc_WARNING(("Can't delete at this position"));
  return XC_NO_POINT_DELETED;
 }
 else
 {
  if (!PointTmp->cont && PointTmp->NextPoint)
  {
   PointTmp->NextPoint->cont = FALSE;
  }
  if (!PointNew)
  {
   Object1->ListPoint = PointTmp->NextPoint;
  }
  else
  {
   PointNew->NextPoint = PointTmp->NextPoint;
  }
  Xc_free(PointTmp);
  Object1->Forme_calc = FALSE;
  if (!Object1->ListPoint)
  {
   return XC_CADRE_MUST_BE_DELETED;
  }
  return XC_NO_ERROR;
 }
}

l_Point *Cgetpoint(Object1, pos)
c_Cadre		*Object1;
int		pos;
{
 l_Point	**PointTmp;
 int		num;
  
  
 PointTmp= &D(Object1).ListPoint;
 num = 0;
 while (*PointTmp!=NULL && num<pos)
 {
  PointTmp= &((*PointTmp)->NextPoint);
  num++;
 }
 if (num!=pos)
 {
  Xc_WARNING(("Can't get point at this position"));
  return NULL;
 }
 else
 {
  return *PointTmp;
 }
}


void Cviewpoint(Object1)
c_Cadre		*Object1;
{
 l_Point	*PointTmp;
  
 PointTmp = D(Object1).ListPoint;
 Xc_TRACE(("liste:\n"));
 while (PointTmp!=NULL)
 {
  Xc_TRACE(("X: %d , Y: %d cont: %s\n",PointTmp->X, PointTmp->Y,
	    PointTmp->cont?"oui":"non"));
  PointTmp = PointTmp->NextPoint;
 }
}

l_Point *Cgetnextpoint(cadre, point)
c_Cadre	*cadre;
l_Point	*point;
{
 l_Point	*lpoint;
 l_Point	*retpoint;
 l_Point	*s_point;
  
 s_point = lpoint = cadre->ListPoint;
 while(lpoint && (lpoint->X!=point->X || lpoint->Y!=point->Y))
 {
  if (lpoint->cont==FALSE)
   s_point = lpoint;
  lpoint = lpoint->NextPoint;
 }
 if (lpoint)
 {
  if ( !lpoint->NextPoint || lpoint->NextPoint->cont==FALSE)
   retpoint = s_point;
  else
   retpoint = lpoint->NextPoint;
 }
 else
  retpoint = NULL;
 return retpoint;
}

l_Point *Cgetpreviouspoint(cadre, point)
c_Cadre	*cadre;
l_Point	*point;
{
 l_Point	*lpoint;
 l_Point	*retpoint;
 l_Point	*s_point;
  
 s_point = NULL;
 lpoint = cadre->ListPoint;
 while(lpoint && (lpoint->X!=point->X || lpoint->Y!=point->Y))
 {
  s_point = lpoint;
  lpoint = lpoint->NextPoint;
 }
 if (lpoint)
 {
  if (lpoint->cont==TRUE && s_point)
  {
   retpoint = s_point;
  }
  else
  {
   do
   {
    s_point = lpoint;
    lpoint = lpoint->NextPoint;
   }
   while(lpoint && lpoint->cont==TRUE);
   retpoint = s_point;
  }
 }
 else
  retpoint = NULL;
 return retpoint;
}

void free_list_point(listp)
l_Point	*listp;
{
 l_Point	*listtmp;
  
 while(listp)
 {
  listtmp = listp->NextPoint;
  Xc_free(listp);
  listp = listtmp;
 }
}

l_Point *duplicate_list_point(listp)
l_Point	*listp;
{
 l_Point	*svgpnt;
 l_Point	*tmppnt = NULL;
 l_Point	*srcpnt;
 boolean	prems;
  
 prems = TRUE;
 srcpnt = listp;
 svgpnt = NULL;
 while(srcpnt)
 {
  if (prems)
  {
   svgpnt = (l_Point *)Xc_malloc("dup lstpnt",sizeof(l_Point));
   tmppnt = svgpnt;
   prems = FALSE;
  }
  else
  {
   tmppnt->NextPoint=(l_Point *)Xc_malloc("dup lstpnt",sizeof(l_Point));
   tmppnt = tmppnt->NextPoint;
  }
  memcpy(tmppnt, srcpnt, sizeof(l_Point));
  srcpnt = srcpnt->NextPoint;
 }
 if (svgpnt)
  tmppnt->NextPoint = NULL;
 return svgpnt;
}

void trace_list_point(listp, display, window, gc, px, py, last)
l_Point	*listp;
Display	*display;
Window	window;
GC		gc;    
int	px;
int	py;
boolean	last;
{
 l_Point	*point_s;
 l_Point	*point_d;
 l_Point	*sv_pnt;
 int		count_seg;
  
 count_seg = 0;
 point_d = sv_pnt = listp;
 point_s = listp->NextPoint;
 while(point_s)
 {
  if (point_s->cont)
  {
   if (count_seg!=MAX_SEGMENT)
   {
    segments[count_seg].x1 = point_s->X+px;
    segments[count_seg].y1 = point_s->Y+py;
    segments[count_seg].x2 = point_d->X+px;
    segments[count_seg++].y2 = point_d->Y+py;
   }
   else
   {
    XDrawSegments(display, window, gc, segments, MAX_SEGMENT);
    count_seg = 0;
   }
   point_d = point_s;
  }
  else
  {
   if (sv_pnt->NextPoint!=point_d)
   {
    if (point_d!=sv_pnt)
     if (count_seg!=MAX_SEGMENT)
     {
      segments[count_seg].x1 = point_d->X+px;
      segments[count_seg].y1 = point_d->Y+py;
      segments[count_seg].x2 = sv_pnt->X+px;
      segments[count_seg++].y2 = sv_pnt->Y+py;
     }
     else
     {
      XDrawSegments(display, window, gc, segments, MAX_SEGMENT);
      count_seg = 0;
     }
    else
     XDrawPoint(display, window, gc, point_d->X+px, point_d->Y+py);
   }
   sv_pnt = point_d = point_s;
  }
  point_s = point_s->NextPoint;
 }
 if (last)
 {
  if (sv_pnt->NextPoint!=point_d)
  {
   if (point_d!=sv_pnt)
    if (count_seg!=MAX_SEGMENT)
    {
     segments[count_seg].x1 = point_d->X+px;
     segments[count_seg].y1 = point_d->Y+py;
     segments[count_seg].x2 = sv_pnt->X+px;
     segments[count_seg++].y2 = sv_pnt->Y+py;
    }
    else
    {
     XDrawSegments(display, window, gc, segments, MAX_SEGMENT);
     count_seg = 0;
    }
   else
    XDrawPoint(display, window, gc, point_d->X+px, point_d->Y+py);
  }
 }
 if (count_seg)
 {
  XDrawSegments(display, window, gc, segments, count_seg);
 }
}

void rotate_list_point(listp, orig, scale, angle, xcent, ycent)
l_Point	*listp;
l_Point	*orig;
coord_t	scale;
angle_t	angle;
coord_t	xcent;
coord_t	ycent;
{
 l_Point  	*tmplist;
 real		cs, ss;
 coord_t	dx, dy;
  
 cs = cos(SCALE_TO_RADIANS(angle));
 ss = sin(SCALE_TO_RADIANS(angle));
 tmplist = listp;
 while(tmplist)
 {
  dx = xcent-orig->sX;
  dy = ycent-orig->sY;
  tmplist->sX = xcent-(coord_t)(dx*cs+dy*ss);
  tmplist->sY = ycent-(coord_t)(dy*cs-dx*ss);
  tmplist->X = tmplist->sX/scale;
  tmplist->Y = tmplist->sY/scale;
  orig = orig->NextPoint;
  tmplist = tmplist->NextPoint;
 }
}

void scale_list_point(listp, orig, scale, xfactor, yfactor, xcent, ycent)
l_Point	*listp;
l_Point	*orig;
coord_t	scale;
real	xfactor;
real	yfactor;
coord_t	xcent;
coord_t	ycent;
{
 l_Point	*tmplist;
 coord_t	xnew, ynew;
  
 tmplist = listp;
 while(tmplist)
 {
  xnew = orig->sX-xcent;
  ynew = orig->sY-ycent;
  tmplist->sX = xcent+xnew*xfactor;
  tmplist->sY = ycent+ynew*yfactor;
  tmplist->X = tmplist->sX/scale;
  tmplist->Y = tmplist->sY/scale;
  orig = orig->NextPoint;
  tmplist = tmplist->NextPoint;
 }
}

void get_limit_list_point(lpoint, x_min, y_min, x_max, y_max)
l_Point	*lpoint;
coord_t	*x_min;
coord_t	*y_min;
coord_t	*x_max;
coord_t	*y_max;
{
 coord_t	smin_x;
 coord_t	smin_y;
 coord_t	smax_x;
 coord_t	smax_y;
  
 if (!lpoint)
  return;
 smin_x = lpoint->sX;
 smin_y = lpoint->sY;
 smax_x = lpoint->sX;
 smax_y = lpoint->sY;
 while(lpoint)
 {
  if (lpoint->sX<smin_x)
   smin_x = lpoint->sX;
  if (lpoint->sY<smin_y)
   smin_y = lpoint->sY;
  if (lpoint->sX>smax_x)
   smax_x = lpoint->sX;
  if (lpoint->sY>smax_y)
   smax_y = lpoint->sY;
  lpoint = lpoint->NextPoint;
 }
 *x_min = smin_x;
 *y_min = smin_y;
 *x_max = smax_x;
 *y_max = smax_y;
}

void CfusionCadre(Cadre1, Cadre2)
c_Cadre	*Cadre1;
c_Cadre	*Cadre2;
{
 l_Point	*source;
 l_Point	*copy;
 coord_t	spx, spy;
  
 spx = Cadre2->sOrig_X-Cadre1->sOrig_X;
 spy = Cadre2->sOrig_Y-Cadre1->sOrig_Y;
 source = Cadre1->ListPoint;
 while(source->NextPoint)
  source = source->NextPoint;
 copy = duplicate_list_point(Cadre2->ListPoint);
 source->NextPoint = copy;
 while(copy)
 {
  copy->sX+=spx;
  copy->sY+=spy;
  copy->X = copy->sX/Cadre1->sScale;
  copy->Y = copy->sY/Cadre1->sScale;
  copy = copy->NextPoint;
 }
}

void CattachObject(This, object, type)
c_Cadre	*This;
void	*object;
int	type;
{
 Xc_HISTORY(("attachObject"));
 if(This->object_type != CADRE_EMPTY)
  DISPLAY_DELETE(This->object)(This->object);
 Xc_TRACE(("f object: %d -- %x",*(int *)object,*(int *)object));
 This->object = object;
 This->object_type = type;
 if (DISPLAY_INIT(object)(object, This)!=XC_NO_ERROR) 
 {
  CdetachObject(This);
 }
 else
  DISPLAY_CONFIGURE(object)(object, This, This->Forme);
 if (This->X_info_calc)
  F(This).set_X_info(This, This->X_info.display, This->X_info.window);
}

void CdetachObject(This)
c_Cadre	*This;
{
 Xc_HISTORY(("detachObject"));
 if(This->object_type != CADRE_EMPTY)
  DISPLAY_DELETE(This->object)(This->object);
 This->object_type = CADRE_EMPTY;
 This->object = NULL;
 if (This->X_info_calc)
 {
      
  F(This).set_X_info(This, This->X_info.display, This->X_info.window);
 }
}

void CcalculOrigine(cadre)
c_Cadre	*cadre;
{
 l_Point	*pointl;
 int		min_x;
 int		min_y;
 coord_t	smin_x;
 coord_t	smin_y;
 coord_t	smax_x;
 coord_t	smax_y;
 coord_t	dx, dy;
 coord_t	fx, fy;
 double	cs, ss;
    
 pointl = cadre->ListPoint;
 if (!pointl)
  return;
 min_x = pointl->X;
 min_y = pointl->Y;
 smin_x = pointl->sX;
 smin_y = pointl->sY;
 smax_x = pointl->sX;
 smax_y = pointl->sY;
 while(pointl)
 {
  if (pointl->X<min_x)
   min_x = pointl->X;
  if (pointl->Y<min_y)
   min_y = pointl->Y;
  if (pointl->sX<smin_x)
   smin_x = pointl->sX;
  if (pointl->sY<smin_y)
   smin_y = pointl->sY;
  if (pointl->sX>smax_x)
   smax_x = pointl->sX;
  if (pointl->sY>smax_y)
   smax_y = pointl->sY;
  pointl = pointl->NextPoint;
 }
 pointl = cadre->ListPoint;
 while(pointl)
 {
  pointl->X -= min_x;
  pointl->Y -= min_y;
  pointl->sX -= smin_x;
  pointl->sY -= smin_y;
  pointl = pointl->NextPoint;
 }
 cadre->sOrig_X+=smin_x;
 cadre->sOrig_Y+=smin_y;
 cadre->Forme->Orig_X+=min_x;
 cadre->Forme->Orig_Y+=min_y;
 cadre->Forme_calc = FALSE;
 cadre->sWidth = smax_x-smin_x;
 cadre->sHeight = smax_y-smin_y;
 if (cadre->sAngle)
 { 
  cs = cos(SCALE_TO_RADIANS(cadre->sAngle));
  ss = sin(SCALE_TO_RADIANS(cadre->sAngle));
  pointl = cadre->ListPoint;
  dx = pointl->sX;
  dy = pointl->sY;
  smin_x = smax_x=(coord_t)(dx*cs-dy*ss);
  smin_y = smax_y=(coord_t)(dy*cs+dx*ss);
  pointl = pointl->NextPoint;
  while(pointl)
  {
   dx = pointl->sX;
   dy = pointl->sY;
   fx=(coord_t)(dx*cs-dy*ss);
   fy=(coord_t)(dy*cs+dx*ss);
   if (fx<smin_x)
    smin_x = fx;
   if (fy<smin_y)
    smin_y = fy;
   if (fx>smax_x)
    smax_x = fx;
   if (fy>smax_y)
    smax_y = fy;
   pointl = pointl->NextPoint;
  }
  dx = smin_x;
  dy = smin_y;
  cadre->saOrig_X=(coord_t)(dx*cs+dy*ss);
  cadre->saOrig_Y=(coord_t)(dy*cs-dx*ss);
  cadre->saWidth = smax_x-smin_x;
  cadre->saHeight = smax_y-smin_y;
 }
 else
 {
  cadre->saOrig_X = cadre->saOrig_Y = 0;
  cadre->saWidth = smax_x-smin_x;
  cadre->saHeight = smax_y-smin_y;
 }
}

void CcalculScaling(cadre)
c_Cadre	*cadre;
{
 l_Point	*lpoint;
  
 lpoint = cadre->ListPoint;
 while(lpoint)
 {
  lpoint->X = lpoint->sX/cadre->sScale;
  lpoint->Y = lpoint->sY/cadre->sScale;
  lpoint = lpoint->NextPoint;
 }
}

void CcalculForme(cadre, contour)
c_Cadre      *cadre;
boolean      contour;
{
 /*Xc_TRACE(("Calculating Contour"));*/
 CalculPolygone(cadre->Forme, cadre->ListPoint);
 cadre->Forme_calc = TRUE;
}

boolean Cisincadre(cadre, posX, posY)
c_Cadre *cadre;
int posX;
int posY;
{
 return IsInPolygone(cadre->Forme, posX, posY);
}

void FreeLigne(ligne)
Ligne *ligne;
{
 Xc_free(ligne->abscisse);
 Xc_free(ligne);
}

Ligne *CalculLigne(xs, ys, xd, yd)
int xs;
int ys;
int xd;
int yd;
{
 Ligne *ligne;
 int dx, dy, sdy, t, ix;
 int i, ii;
  
 ligne=(Ligne *)Xc_malloc("Ligne",sizeof(Ligne));
 if (ys<yd)
 {
  ligne->x_orig = xs;
  ligne->y_orig = ys;
  ligne->x_dest = xd;
  ligne->y_dest = yd;
 }
 else
 {
  ligne->x_orig = xd;
  ligne->y_orig = yd;
  ligne->x_dest = xs;
  ligne->y_dest = ys;
  xs = xd;
  xd = ligne->x_dest;
  ys = yd;
  yd = ligne->y_dest;
 }
 dx = xd-xs;
 dy = yd-ys;
 ligne->nb_absc = dy+1;
 ligne->abscisse=(absc *)Xc_malloc("absc",sizeof(absc)*(dy+1));
 /*Xc_TRACE(("buffer: %p  taille: %d",ligne->abscisse, sizeof(absc)*(dy+1)));*/
  
 if (dx==0)
 {
  i = 0;
  for(ii = ys;ii<=yd;ii++)
  {
   ligne->abscisse[i].x_min = xs;
   ligne->abscisse[i++].x_max = xd;
  }
  return ligne;
 }
 if (dy==0)
 {
  ligne->abscisse[0].x_min = MIN(xs, xd);
  ligne->abscisse[0].x_max = MAX(xs, xd);
  return ligne;
 }
 i = 0;
 ix = SGN(dx);
 dx = ABS(dx);
 if (dx>dy)
 {
  sdy = dy;
  dy+=dy;
  t = dy-dx;
  dx+=dx;
  t+=t+dy;
  if (ix== -1)
  {
   ligne->abscisse[i].x_max = xs;
   while(i!=sdy)
   {
    xs--;
    t+=dy-dx*(t>0);
    if (t>0)
    {
     ligne->abscisse[i++].x_min = xs+1;
     ligne->abscisse[i].x_max = xs;
    }
   }
   ligne->abscisse[i].x_min = xd;
   return ligne;
  }
  ligne->abscisse[i].x_min = xs;
  while(i!=sdy)
  {
   xs++;
   t+=dy-dx*(t>0);
   if (t>0)
   {
    ligne->abscisse[i++].x_max = xs-1;
    ligne->abscisse[i].x_min = xs;
   }
  }
  ligne->abscisse[i].x_max = xd;
  return(ligne);
    
 }
 sdy = dy;
 dx+=dx;
 t = dx-dy;
 dy+=dy;
 t+=dx;
 ligne->abscisse[0].x_min = ligne->abscisse[0].x_max = xs;
 for(i = 1;i<sdy;i++)
 {
  t+=dx-dy*(t>0);
  xs+=ix*(t>0);
  ligne->abscisse[i].x_min = ligne->abscisse[i].x_max = xs;
 }
 ligne->abscisse[i].x_min = ligne->abscisse[i].x_max = xd;
 return ligne;
}

void TraceLigne(ligne, xinfo)
Ligne *ligne;
x_info xinfo;
{
 int i, dy;
 int ys;
  
 ys = ligne->y_orig;
 dy = ligne->nb_absc;
 for(i = 0;i<dy;i++)
 {
  XDrawLine(xinfo.display, xinfo.window, xinfo.gc_contour, ligne->abscisse[i].x_min, ys+i, ligne->abscisse[i].x_max, ys+i);
 }
}

void Cset_X_info(This, display, window)
c_Cadre		*This;
Display		*display;
Window		window;
{
 GC	gc;
 XGCValues	val_gc;
  
 if (!This->X_info_calc)
 {
  val_gc.function = GXcopy;
  val_gc.plane_mask= -1;
  val_gc.background = GlobColormap->Cwhite;
  if (This->border_color->transparency)
   This->X_info.contour_color = GlobColormap->Cwhite;
  else
   This->X_info.contour_color = This->border_color->cell.pixel;
  val_gc.foreground = This->X_info.contour_color;
  val_gc.line_width = 0;
  val_gc.line_style = LineSolid;
  val_gc.cap_style = CapButt;
  val_gc.join_style = JoinMiter;
  if (This->X_info.gc_contour && This->X_info.window==window)
  {
   XChangeGC(display, This->X_info.gc_contour,
	     GCFunction|GCPlaneMask|
	     GCForeground|GCBackground|GCLineWidth|
	     GCLineStyle|GCCapStyle|GCJoinStyle,&val_gc);
  }
  else
  {
   if (This->X_info.gc_contour)
    XFreeGC(display, This->X_info.gc_contour);
   gc = XCreateGC(display, window, GCFunction|GCPlaneMask|
		  GCForeground|GCBackground|GCLineWidth|
		  GCLineStyle|GCCapStyle|GCJoinStyle,&val_gc);
   This->X_info.gc_contour = gc;
  }
    
  val_gc.background = GlobColormap->Cblack;
  if (This->inside_color->transparency)
   This->X_info.inside_color = GlobColormap->Cwhite;
  else
   This->X_info.inside_color = This->inside_color->cell.pixel;
  val_gc.foreground = This->X_info.inside_color;
    
  if (This->X_info.gc_inside && This->X_info.window==window)
  {
   XChangeGC(display, This->X_info.gc_inside,
	     GCFunction|GCForeground|
	     GCBackground|GCLineWidth|GCLineStyle|
	     GCCapStyle|GCJoinStyle,&val_gc);
  }
  else
  {
   if (This->X_info.gc_inside)
    XFreeGC(display, This->X_info.gc_inside);
   gc = XCreateGC(display, window, GCFunction|GCForeground|
		  GCBackground|GCLineWidth|GCLineStyle|
		  GCCapStyle|GCJoinStyle,&val_gc);
   This->X_info.gc_inside = gc;
  }

  This->X_info.display = display;
  This->X_info.window = window;
  This->X_info.ecran = DefaultScreen(display);
    
  val_gc.function = GXcopy;
  Xc_TRACE(("Border color: %d  Background color: %d",
	    This->X_info.contour_color,
	    This->X_info.inside_color));
  This->X_info_calc = TRUE;
 }
}

void Creinit_X_info(This, display, window)
c_Cadre	*This;
Display		*display;
Window		window;
{
 GC	gc;
 XGCValues	val_gc;
  
 This->X_info.display = display;
 This->X_info.window = window;
 This->X_info.ecran = DefaultScreen(display);
  
 val_gc.function = GXcopy;
 val_gc.plane_mask= -1;
 val_gc.background = GlobColormap->Cwhite;
 val_gc.foreground = This->X_info.contour_color;
 val_gc.line_width = 0;
 val_gc.line_style = LineSolid;
 val_gc.cap_style = CapButt;
 val_gc.join_style = JoinMiter;
 if (This->X_info.gc_contour)
  XFree((char *)This->X_info.gc_contour);
 /*XFreeGC(display, This->X_info.gc_contour);*/
 gc = XCreateGC(display, window, GCFunction|GCPlaneMask|
		GCForeground|GCBackground|GCLineWidth|
		GCLineStyle|GCCapStyle|GCJoinStyle,&val_gc);
 This->X_info.gc_contour = gc;
  
 val_gc.background = GlobColormap->Cblack;
 val_gc.foreground = This->X_info.inside_color;
 if (This->X_info.gc_inside)
  XFree((char *)This->X_info.gc_inside);
 /*XFreeGC(display, This->X_info.gc_inside);*/
  
 gc = XCreateGC(display, window, GCFunction|GCForeground|
		GCBackground|GCLineWidth|GCLineStyle|
		GCCapStyle|GCJoinStyle,&val_gc);
 This->X_info.gc_inside = gc;
  
 This->X_info_calc = TRUE;
}


/* ----------------------------------------------------------------- ** 
** setBackgroundColor - Set the frame background color               ** 
** ----------------------------------------------------------------- */
static boolean setBackgroundColor(this, color)
c_Cadre *this;
c_Color *color;
{
 Xc_HISTORY(("setBackgroundColor(`%s', `%s')", this->name, color->name));

 Xc_HISTORY(("oldColor(`%s')", this->inside_color->name));
 /*--- Color hook ---*/
 if(this->inside_color != NULL)
  XcH_UNREGISTER(this->inside_color, hook_color, this);
 if(XcH_REGISTER(color, hook_color, this) == FALSE)
 {
  XcH_REGISTER(this->inside_color, hook_color, this);
  return FALSE;
 }

 this->inside_color = color;
 if (color->transparency)
  this->X_info.inside_color = GlobColormap->Cwhite;
 else
  this->X_info.inside_color = color->cell.pixel;
 if(this->X_info_calc != FALSE)
  XSetForeground(this->X_info.display,
		 this->X_info.gc_inside, this->X_info.inside_color);
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** setOutlineColor - Set the frame outline color                     ** 
** ----------------------------------------------------------------- */
static boolean setOutlineColor(this, color)
c_Cadre *this;
c_Color *color;
{
 Xc_HISTORY(("setOutlineColor(`%s', `%s')", this->name, color->name));

 /*--- Color hook ---*/
 if(this->border_color != NULL)
  XcH_UNREGISTER(this->border_color, hook_color, this);
 if(XcH_REGISTER(color, hook_color, this) == FALSE)
 {
  XcH_REGISTER(this->border_color, hook_color, this);
  return FALSE;
 }

 this->border_color = color;
 if (color->transparency)
  this->X_info.contour_color = GlobColormap->Cwhite;
 else
  this->X_info.contour_color = color->cell.pixel;
 if (this->X_info_calc != FALSE)
  XSetForeground(this->X_info.display,
		 this->X_info.gc_contour, this->X_info.contour_color);
 return TRUE;
}


void Csetlinewidth(This, linew)
c_Cadre		*This;
int		linew;
{
 XGCValues	val_gc;
  
 val_gc.line_width = linew;
 XChangeGC(_display, _gc_contour, GCLineWidth,&val_gc);
}

void Cset_contour_action(This, funct)
c_Cadre		*This;
int		funct;
{
 switch(funct)
 {
 case GFX_SET:
  XSetFunction(_display, _gc_contour, GXcopy);
  break;
 case GFX_REVERSE:
  if (_contour_color==0)
   XSetFunction(_display, _gc_contour, GXinvert);
  else
   XSetFunction(_display, _gc_contour, GXxor);
  break;
 case GFX_CLEAR:
  if (_contour_color==0)
   XSetFunction(_display, _gc_contour, GXset);
  else
   XSetFunction(_display, _gc_contour, GXclear);
  break;
 }
}

void Cdrawcontour(This, polyg, nullsize)
c_Cadre		*This;
Polygone	*polyg;
boolean		nullsize;
{
 l_Point *point_s;
 l_Point *point_d;
 l_Point *sv_pnt;
 Ligne *line;
 Ligne_h	*ligne;
 absc	*absci;
 int px, py;
 int pym, pyx, pxo;	
 int min_y, max_y;
 int i, j, k, t;
 int count_point;
  
 px = This->Forme->Orig_X;
 py = This->Forme->Orig_Y;
 if (!polyg->nb_lig || (!nullsize && !This->thickness))
  return;
 pym = polyg->Orig_Y+polyg->y_min;
 pyx = polyg->Orig_Y+polyg->y_max;
 pxo = polyg->Orig_X;
 point_d = sv_pnt = This->ListPoint;
 count_point = 0;
 if (point_d)
 {
  point_s = point_d->NextPoint;
  while(point_s)
  {
   if (point_s->cont)
   {
    line = CalculLigne(point_s->X+px, point_s->Y+py, point_d->X+px, point_d->Y+py);
    min_y = MAX(line->y_orig, pym);
    max_y = MIN(line->y_dest, pyx);
    absci = line->abscisse+min_y-line->y_orig;
    ligne = polyg->ligne+min_y-pym;
    for(i = min_y;i<=max_y;i++)
    {
     for(j = absci->x_min;j<=absci->x_max;j++)
     {
      k = 0;
      t = j-pxo;
      while(k<ligne->nb_seg)
      {
       if (t>=ligne->segment[k].x_min && t<=ligne->segment[k].x_max)
       {
	if (count_point!=MAX_POINT)
	{
	 points[count_point].x = j;
	 points[count_point++].y = i;
	}
	else
	{		  
	 XDrawPoints(_display, _window, _gc_contour, points, MAX_POINT, CoordModeOrigin);
	 count_point = 0;
	}
       }
       k++;
      }
     }
     ligne++;
     absci++;
    }
    FreeLigne(line);
    point_d = point_s;
   }
   else
   {
    line = CalculLigne(point_d->X+px, point_d->Y+py, sv_pnt->X+px, sv_pnt->Y+py);
    min_y = MAX(line->y_orig, pym);
    max_y = MIN(line->y_dest, pyx);
    absci = line->abscisse+min_y-line->y_orig;
    ligne = polyg->ligne+min_y-pym;
    for(i = min_y;i<=max_y;i++)
    {
     for(j = absci->x_min;j<=absci->x_max;j++)
     {
      k = 0;
      t = j-pxo;
      while(k<ligne->nb_seg)
      {
       if (t>=ligne->segment[k].x_min && t<=ligne->segment[k].x_max)
       {
	if (count_point!=MAX_POINT)
	{
	 points[count_point].x = j;
	 points[count_point++].y = i;
	}
	else
	{		  
	 XDrawPoints(_display, _window, _gc_contour, points, MAX_POINT, CoordModeOrigin);
	 count_point = 0;
	}
       }
       k++;
      }
     }
     ligne++;
     absci++;
    }
    FreeLigne(line);
    sv_pnt = point_d = point_s;
   }
   point_s = point_s->NextPoint;
  }
  line = CalculLigne(point_d->X+px, point_d->Y+py, sv_pnt->X+px, sv_pnt->Y+py);
  min_y = MAX(line->y_orig, pym);
  max_y = MIN(line->y_dest, pyx);
  absci = line->abscisse+min_y-line->y_orig;
  ligne = polyg->ligne+min_y-pym;
  for(i = min_y;i<=max_y;i++)
  {
   for(j = absci->x_min;j<=absci->x_max;j++)
   {
    k = 0;
    t = j-pxo;
    while(k<ligne->nb_seg)
    {
     if (t>=ligne->segment[k].x_min && t<=ligne->segment[k].x_max)
     {
      if (count_point!=MAX_POINT)
      {
       points[count_point].x = j;
       points[count_point++].y = i;
      }
      else
      {		  
       XDrawPoints(_display, _window, _gc_contour, points, MAX_POINT, CoordModeOrigin);
       count_point = 0;
      }
     }
     k++;
    }
   }
   ligne++;
   absci++;
  }
  FreeLigne(line);
  if (count_point)
   XDrawPoints(_display, _window, _gc_contour, points, count_point, CoordModeOrigin);
 }
}

void Cdrawcadre(This, polyg)
c_Cadre	*This;
Polygone	*polyg;
{
 XGCValues	values;

 if (This->object_type)
 {
  if (This->dirty_flag)
  {
   DISPLAY_CONFIGURE(This->object)(This->object, This, This->Forme);
   This->dirty_flag = FALSE;
  }
  if (This->inside_color->transparency == 0)
   DrawPolygone(polyg, This->X_info);
    
  This->clip = CalculPolygoneMask(polyg, This->X_info);
  values.clip_x_origin = polyg->Orig_X;
  values.clip_y_origin = polyg->Orig_Y;
  values.clip_mask = This->clip;
  /* pour nuc */
  values.foreground = GlobColormap->Cblack;
  values.background = GlobColormap->Cwhite;
  XChangeGC(This->X_info.display, This->X_info.gc_inside, GCClipMask|
	    GCClipXOrigin|GCClipYOrigin|GCForeground|GCBackground, &values);
  DISPLAY_REDRAW(This->object)(This->object, This, polyg);
  values.clip_mask = (Pixmap)NULL;
  values.foreground = This->X_info.inside_color;
  values.line_width = 0;
  values.line_style = LineSolid;
  values.cap_style = CapButt;
  values.join_style = JoinMiter;
  values.function = GXcopy;
  XChangeGC(This->X_info.display, This->X_info.gc_inside,
	    GCClipMask|GCForeground|GCLineWidth|GCLineStyle|
	    GCCapStyle|GCJoinStyle|GCFunction,&values);
  XFreePixmap(This->X_info.display, This->clip);
 }
 else
 {
  if (This->inside_color->transparency == 0)
   DrawPolygone(polyg, This->X_info);
 }
 Xc_HISTORY(("redraw cadre done"));
}

void Cdrawsommet(This)
c_Cadre 	*This;
{
 int		px, py;
 l_Point	*point_s;
  
 px = This->Forme->Orig_X;
 py = This->Forme->Orig_Y;
  
 F(This).set_contour_action(This, GFX_REVERSE);
 XSetForeground(_display, _gc_contour, GlobColormap->Cblack);
 for(point_s = This->ListPoint; point_s; point_s = point_s->NextPoint) 
 {
  TRACE_HANDLE(_display, _window, _gc_contour, px+point_s->X, py+point_s->Y);
 }
 XSetForeground(_display, _gc_contour, This->X_info.contour_color);
 F(This).set_contour_action(This, GFX_SET);
}

void Cdrawhandle(This)
c_Cadre 	*This;
{
 coord_t	x1, y1, x2, y2, x3, y3, x4, y4;
 int		anglei;
 double	angle;
 XPoint	xpoint[4];
 int x_min, y_min, x_mid, y_mid, x_max, y_max;

 if ((!This->ListPoint->NextPoint) || 
     (!This->ListPoint->NextPoint->NextPoint) ||
     (!This->sWidth) || (!This->sHeight))
 {
  Cdrawsommet(This);
  return;
 }
  
 anglei = ABS((int)(SCALE_TO_DEGREES(This->sAngle+SCALE_FROM_DEGREES(720)))%90);
  
 F(This).set_contour_action(This, GFX_REVERSE);
 XSetForeground(_display, _gc_contour, GlobColormap->Cblack);

 if (anglei<=1 || anglei>=89)
 {
  x_min = This->Forme->Orig_X+This->Forme->x_min;
  y_min = This->Forme->Orig_Y+This->Forme->y_min;
  x_max = x_min+This->Forme->x_max;
  y_max = y_min+This->Forme->y_max;
  x_mid=(x_max+x_min)>>1;
  y_mid=(y_max+y_min)>>1;
  TRACE_RHANDLE(_display, _window, _gc_contour, x_min-RHANDLE_X-1,
		y_min-RHANDLE_Y-1);
  TRACE_RHANDLE(_display, _window, _gc_contour, x_min-RHANDLE_X-1,
		y_max+RHANDLE_Y+1);
  TRACE_RHANDLE(_display, _window, _gc_contour, x_max+RHANDLE_X+1,
		y_min-RHANDLE_Y-1);
  TRACE_RHANDLE(_display, _window, _gc_contour, x_max+RHANDLE_X+1,
		y_max+RHANDLE_Y+1);
  TRACE_RHANDLE(_display, _window, _gc_contour, x_mid, y_min-RHANDLE_Y-1);
  TRACE_RHANDLE(_display, _window, _gc_contour, x_mid, y_max+RHANDLE_Y+1);
  TRACE_RHANDLE(_display, _window, _gc_contour, x_min-RHANDLE_X-1, y_mid);
  TRACE_RHANDLE(_display, _window, _gc_contour, x_max+RHANDLE_X+1, y_mid);

  XSetForeground(_display, _gc_contour, This->X_info.contour_color);
  F(This).set_contour_action(This, GFX_SET);
  return;
 }
  
 angle = SCALE_TO_RADIANS(This->sAngle);
  
 xpoint[1].x= (int)(RHANDLE_DWIDTH*cos(angle)+.5);
 xpoint[1].y= -(int)(RHANDLE_DWIDTH*sin(angle)+.5);
 xpoint[2].x= -(int)(RHANDLE_DHEIGHT*sin(angle)+.5);
 xpoint[2].y= -(int)(RHANDLE_DHEIGHT*cos(angle)+.5);
 xpoint[3].x= -xpoint[1].x;
 xpoint[3].y= -xpoint[1].y;
  
 x1=(This->sOrig_X+This->saOrig_X);
 y1=(This->sOrig_Y+This->saOrig_Y);
 xpoint[0].x=(x1)/This->sScale+xpoint[3].x;
 xpoint[0].y=(y1)/This->sScale+xpoint[3].y;
 XFillPolygon(_display, _window, _gc_contour, xpoint, 4, Convex, 
	      CoordModePrevious);
 x2 = x1+(coord_t)(cos(angle)*(double)This->saWidth+.5);
 y2 = y1-(coord_t)(sin(angle)*(double)This->saWidth+.5);
 xpoint[0].x=(x2)/This->sScale;
 xpoint[0].y=(y2)/This->sScale;
 XFillPolygon(_display, _window, _gc_contour, xpoint, 4, Convex, 
	      CoordModePrevious);
 x3 = x2+(coord_t)(sin(angle)*(double)This->saHeight+.5);
 y3 = y2+(coord_t)(cos(angle)*(double)This->saHeight+.5);
 xpoint[0].x=(x3)/This->sScale-xpoint[2].x;
 xpoint[0].y=(y3)/This->sScale-xpoint[2].y;
 XFillPolygon(_display, _window, _gc_contour, xpoint, 4, Convex, 
	      CoordModePrevious);
 x4 = x3 - (coord_t)(cos(angle) * (double)This->saWidth + .5);
 y4 = y3 + (coord_t)(sin(angle) * (double)This->saWidth + .5);
 xpoint[0].x=(x4)/This->sScale+xpoint[3].x-xpoint[2].x;
 xpoint[0].y=(y4)/This->sScale+xpoint[3].y-xpoint[2].y;
 XFillPolygon(_display, _window, _gc_contour, xpoint, 4, Convex, 
	      CoordModePrevious);
 xpoint[0].x=((x1+x2)/This->sScale+xpoint[3].x)/2;
 xpoint[0].y=((y1+y2)/This->sScale+xpoint[3].y)/2;
 XFillPolygon(_display, _window, _gc_contour, xpoint, 4, Convex, 
	      CoordModePrevious);
 xpoint[0].x=((x2+x3)/This->sScale-xpoint[2].x)/2;
 xpoint[0].y=((y2+y3)/This->sScale-xpoint[2].y)/2;
 XFillPolygon(_display, _window, _gc_contour, xpoint, 4, Convex, 
	      CoordModePrevious);
 xpoint[0].x=((x3+x4)/This->sScale+xpoint[3].x) / 2 - xpoint[2].x;
 xpoint[0].y=((y3+y4)/This->sScale+xpoint[3].y) / 2 - xpoint[2].y;
 XFillPolygon(_display, _window, _gc_contour, xpoint, 4, Convex, 
	      CoordModePrevious);
 xpoint[0].x=((x4+x1)/This->sScale-xpoint[2].x) / 2 + xpoint[3].x;
 xpoint[0].y=((y4+y1)/This->sScale-xpoint[2].y) / 2 + xpoint[3].y;
 XFillPolygon(_display, _window, _gc_contour, xpoint, 4, Convex, 
	      CoordModePrevious);

 XSetForeground(_display, _gc_contour, This->X_info.contour_color);
 F(This).set_contour_action(This, GFX_SET);
}

static void configure_request(This, init)
c_Cadre	*This;
boolean init;
{
 error erreur;
  
 Xc_HISTORY(("configure_request(%sinit)", init? "" : "no"));
 if (init)
 {
  erreur = DISPLAY_INIT(This->object)(This->object, This);
  if (erreur != XC_NO_ERROR) F(This).detachobject(This);
 }
 This->dirty_flag = TRUE;
 if (This->X_info_calc)
 {
  This->data_to_object->redraw_cadre(This->data_to_object->page, This);
 }
}

/* ----------------------------------------------------------------- ** 
** writeCadre - Cadre saving                                         ** 
** ----------------------------------------------------------------- */
static boolean writeCadre(this, doc, dx, dy)
c_Cadre *this;
c_DocFile *doc;
coord_t dx;
coord_t dy;
{
 l_Point *p;
  
 Xc_HISTORY(("write(`%s')", this->name));
  
 if (!F(doc).writeKeyword(doc, XcDF_FRAME_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeKeyword(doc, XcDF_NAME_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).write(doc, this->name, strlen(this->name)) ||
     !F(doc).endSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->sOrig_X + dx)) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->sOrig_Y + dy)) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->sWidth)) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->sHeight)) ||
     !F(doc).writeKeyword(doc, XcDF_COLOR_KEYWORD, this->inside_color->id) ||
     !F(doc).writeReal(doc, SCALE_TO_DEGREES(this->sAngle)) ||
     !F(doc).writeBoolean(doc, this->verrou) || 
     (this->selected && 
      !F(doc).writeKeyword(doc, XcDF_FRAME_SELECTED_KEYWORD, -1L)))
  return FALSE;
 
#if defined(Xc_XALLWRITE)
 if (!F(doc).writeKeyword
     (doc, XcDF_TEXTFLOW_KEYWORD, (long)this->textflow_mode))
  return FALSE;
#endif
 
 if (!F(doc).writeKeyword(doc, XcDF_SHAPE_KEYWORD, -1L) ||
     !F(doc).startSequence(doc) ||
     !F(doc).writeReal(doc, SCALE_TO_POINTS(this->thickness)) ||
     !F(doc).writeKeyword(doc, XcDF_COLOR_KEYWORD, this->border_color->id))
  return FALSE;
 
 /*--- Write shape ---*/
 for(p = this->ListPoint; p != NULL; p = p->NextPoint)
 {
  if(p->cont == FALSE)
  {
   if (!F(doc).writeKeyword(doc, XcDF_POINT_KEYWORD, -1L) ||
       !F(doc).startSequence(doc) ||
       !F(doc).writeReal(doc, SCALE_TO_POINTS(p->sX)) ||
       !F(doc).writeReal(doc, SCALE_TO_POINTS(p->sY)) ||
       !F(doc).endSequence(doc)) return FALSE;
  }
  else
  {
   if (!F(doc).writeKeyword(doc, XcDF_LINE_KEYWORD, -1L) ||
       !F(doc).startSequence(doc) ||
       !F(doc).writeReal(doc, SCALE_TO_POINTS(p->sX)) ||
       !F(doc).writeReal(doc, SCALE_TO_POINTS(p->sY)) ||
       !F(doc).endSequence(doc)) return FALSE;
  }
 }
  
 if (F(doc).endSequence(doc) == FALSE) return FALSE;
  
 if ((this->object_type != CADRE_EMPTY) &&
     (WRITE(this->object)(this->object, doc) == FALSE)) return FALSE;
  
 return F(doc).endSequence(doc);
}


/* ----------------------------------------------------------------- ** 
** hook_color - Color class hook                                     ** 
** ----------------------------------------------------------------- */
static void hook_color(this, reason, old, new)
c_Cadre *this;
int reason;
c_Color *old;
c_Color *new;
{
 Xc_TRACE(("hook_color"));
 
 if (!this->X_info_calc)
 {
  if(this->border_color == old)
   this->border_color = new;

  if(this->inside_color == old)
   this->inside_color = new;
  return;
 }
 
 if (this->border_color == old)
 {
  this->border_color = new;
  if (new->transparency)
  {
   XSetForeground(this->X_info.display, this->X_info.gc_contour, 
		  GlobColormap->Cwhite);
   this->X_info.contour_color = GlobColormap->Cwhite;
  }
  else
  {
   F(new).setGC(new, this->X_info.display, this->X_info.gc_contour);
   this->X_info.contour_color = new->cell.pixel;
  }
 }
  
 if (this->inside_color == old)
 {
  this->inside_color = new;
  if (new->transparency)
  {
   XSetForeground(this->X_info.display, this->X_info.gc_inside, 
		  GlobColormap->Cwhite);
   this->X_info.inside_color = GlobColormap->Cwhite;
  }
  else
  {
   F(new).setGC(new, this->X_info.display, this->X_info.gc_inside);
   this->X_info.inside_color = new->cell.pixel;
  }
 }
 this->data_to_object->redraw_cadre(this->data_to_object->page, this);
}
