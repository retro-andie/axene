/*
** redraw.c for Xclamation and XAllWrite in VectorGraph/
** Redraw methods for VectorGraph class
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
** Started on  Wed Dec 13 16:12:44 1995 Stéphane Boisson
** Last update Fri Dec 31 16:08:37 1999 Emmanuel Paris
*/

#ifdef EFINITIVE_VERSION
#undef EFINITIVE_VERSION
#define NMEMDEBUG
#endif

#define NHISTORY
#define NTRACE

#include "VectorGraphP.h"
#include "FontManager.h"

extern c_FontManager    *GlobFntMng;

#define MASK_WIDTH(poly) ((poly)->x_max - (poly)->x_min + 1)
#define MASK_HEIGHT(poly) ((poly)->y_max - (poly)->y_min + 1)

scale_t	asin_table[1000]= 
{
 -90.000,-86.374,-84.871,-83.717,-82.744,-81.886,-81.110,-80.397,
 -79.732,-79.107,-78.516,-77.953,-77.416,-76.899,-76.403,-75.923,
 -75.459,-75.009,-74.572,-74.146,-73.732,-73.327,-72.932,-72.545,
 -72.167,-71.796,-71.432,-71.075,-70.725,-70.380,-70.041,-69.708,
 -69.380,-69.057,-68.738,-68.424,-68.114,-67.808,-67.507,-67.209,
 -66.914,-66.624,-66.336,-66.052,-65.771,-65.493,-65.218,-64.946,
 -64.676,-64.409,-64.145,-63.883,-63.624,-63.367,-63.112,-62.859,
 -62.609,-62.361,-62.114,-61.870,-61.628,-61.387,-61.149,-60.912,
 -60.677,-60.444,-60.212,-59.982,-59.753,-59.526,-59.301,-59.077,
 -58.854,-58.633,-58.414,-58.195,-57.978,-57.763,-57.548,-57.335,
 -57.123,-56.913,-56.703,-56.495,-56.287,-56.081,-55.876,-55.672,
 -55.469,-55.268,-55.067,-54.867,-54.668,-54.470,-54.273,-54.077,
 -53.882,-53.688,-53.495,-53.303,-53.111,-52.920,-52.730,-52.541,
 -52.353,-52.166,-51.979,-51.793,-51.608,-51.424,-51.240,-51.058,
 -50.875,-50.694,-50.513,-50.333,-50.154,-49.975,-49.797,-49.620,
 -49.443,-49.267,-49.091,-48.917,-48.742,-48.569,-48.396,-48.223,
 -48.051,-47.880,-47.709,-47.539,-47.369,-47.200,-47.032,-46.864,
 -46.696,-46.529,-46.363,-46.197,-46.031,-45.866,-45.702,-45.538,
 -45.374,-45.211,-45.049,-44.887,-44.725,-44.564,-44.403,-44.243,
 -44.083,-43.923,-43.764,-43.606,-43.447,-43.290,-43.132,-42.975,
 -42.819,-42.662,-42.507,-42.351,-42.196,-42.042,-41.887,-41.733,
 -41.580,-41.427,-41.274,-41.121,-40.969,-40.818,-40.666,-40.515,
 -40.364,-40.214,-40.064,-39.914,-39.765,-39.616,-39.467,-39.319,
 -39.171,-39.023,-38.875,-38.728,-38.581,-38.435,-38.288,-38.142,
 -37.997,-37.851,-37.706,-37.561,-37.417,-37.272,-37.128,-36.985,
 -36.841,-36.698,-36.555,-36.412,-36.270,-36.128,-35.986,-35.844,
 -35.703,-35.562,-35.421,-35.280,-35.140,-35.000,-34.860,-34.720,
 -34.581,-34.442,-34.303,-34.164,-34.025,-33.887,-33.749,-33.611,
 -33.474,-33.336,-33.199,-33.062,-32.925,-32.789,-32.652,-32.516,
 -32.380,-32.245,-32.109,-31.974,-31.839,-31.704,-31.569,-31.434,
 -31.300,-31.166,-31.032,-30.898,-30.765,-30.631,-30.498,-30.365,
 -30.232,-30.099,-29.967,-29.835,-29.702,-29.570,-29.439,-29.307,
 -29.176,-29.044,-28.913,-28.782,-28.651,-28.521,-28.390,-28.260,
 -28.130,-28.000,-27.870,-27.740,-27.611,-27.481,-27.352,-27.223,
 -27.094,-26.965,-26.837,-26.708,-26.580,-26.452,-26.324,-26.196,
 -26.068,-25.940,-25.813,-25.686,-25.558,-25.431,-25.304,-25.178,
 -25.051,-24.924,-24.798,-24.672,-24.545,-24.419,-24.294,-24.168,
 -24.042,-23.917,-23.791,-23.666,-23.541,-23.416,-23.291,-23.166,
 -23.041,-22.917,-22.792,-22.668,-22.543,-22.419,-22.295,-22.171,
 -22.048,-21.924,-21.800,-21.677,-21.553,-21.430,-21.307,-21.184,
 -21.061,-20.938,-20.815,-20.693,-20.570,-20.448,-20.325,-20.203,
 -20.081,-19.959,-19.837,-19.715,-19.593,-19.471,-19.350,-19.228,
 -19.107,-18.985,-18.864,-18.743,-18.622,-18.501,-18.380,-18.259,
 -18.138,-18.018,-17.897,-17.777,-17.656,-17.536,-17.416,-17.295,
 -17.175,-17.055,-16.935,-16.815,-16.696,-16.576,-16.456,-16.337,
 -16.217,-16.098,-15.978,-15.859,-15.740,-15.621,-15.502,-15.383,
 -15.264,-15.145,-15.026,-14.907,-14.789,-14.670,-14.552,-14.433,
 -14.315,-14.196,-14.078,-13.960,-13.842,-13.724,-13.605,-13.488,
 -13.370,-13.252,-13.134,-13.016,-12.898,-12.781,-12.663,-12.546,
 -12.428,-12.311,-12.193,-12.076,-11.959,-11.842,-11.724,-11.607,
 -11.490,-11.373,-11.256,-11.139,-11.022,-10.905,-10.789,-10.672,
 -10.555,-10.439,-10.322,-10.205,-10.089,-9.972,-9.856,-9.740,
 -9.623,-9.507,-9.391,-9.274,-9.158,-9.042,-8.926,-8.810,
 -8.694,-8.578,-8.462,-8.346,-8.230,-8.114,-7.998,-7.882,
 -7.766,-7.651,-7.535,-7.419,-7.304,-7.188,-7.072,-6.957,
 -6.841,-6.726,-6.610,-6.495,-6.379,-6.264,-6.149,-6.033,
 -5.918,-5.803,-5.687,-5.572,-5.457,-5.342,-5.226,-5.111,
 -4.996,-4.881,-4.766,-4.651,-4.536,-4.421,-4.306,-4.191,
 -4.076,-3.961,-3.846,-3.731,-3.616,-3.501,-3.386,-3.271,
 -3.156,-3.041,-2.926,-2.811,-2.697,-2.582,-2.467,-2.352,
 -2.237,-2.123,-2.008,-1.893,-1.778,-1.663,-1.549,-1.434,
 -1.319,-1.205,-1.090,-0.975,-0.860,-0.746,-0.631,-0.516,
 -0.401,-0.287,-0.172,-0.057,0.057,0.172,0.287,0.401,
 0.516,0.631,0.746,0.860,0.975,1.090,1.205,1.319,
 1.434,1.549,1.663,1.778,1.893,2.008,2.123,2.237,
 2.352,2.467,2.582,2.697,2.811,2.926,3.041,3.156,
 3.271,3.386,3.501,3.616,3.731,3.846,3.961,4.076,
 4.191,4.306,4.421,4.536,4.651,4.766,4.881,4.996,
 5.111,5.226,5.342,5.457,5.572,5.687,5.803,5.918,
 6.033,6.149,6.264,6.379,6.495,6.610,6.726,6.841,
 6.957,7.072,7.188,7.304,7.419,7.535,7.651,7.766,
 7.882,7.998,8.114,8.230,8.346,8.462,8.578,8.694,
 8.810,8.926,9.042,9.158,9.274,9.391,9.507,9.623,
 9.740,9.856,9.972,10.089,10.205,10.322,10.439,10.555,
 10.672,10.789,10.905,11.022,11.139,11.256,11.373,11.490,
 11.607,11.724,11.842,11.959,12.076,12.193,12.311,12.428,
 12.546,12.663,12.781,12.898,13.016,13.134,13.252,13.370,
 13.488,13.605,13.724,13.842,13.960,14.078,14.196,14.315,
 14.433,14.552,14.670,14.789,14.907,15.026,15.145,15.264,
 15.383,15.502,15.621,15.740,15.859,15.978,16.098,16.217,
 16.337,16.456,16.576,16.696,16.815,16.935,17.055,17.175,
 17.295,17.416,17.536,17.656,17.777,17.897,18.018,18.138,
 18.259,18.380,18.501,18.622,18.743,18.864,18.985,19.107,
 19.228,19.350,19.471,19.593,19.715,19.837,19.959,20.081,
 20.203,20.325,20.448,20.570,20.693,20.815,20.938,21.061,
 21.184,21.307,21.430,21.553,21.677,21.800,21.924,22.048,
 22.171,22.295,22.419,22.543,22.668,22.792,22.917,23.041,
 23.166,23.291,23.416,23.541,23.666,23.791,23.917,24.042,
 24.168,24.294,24.419,24.545,24.672,24.798,24.924,25.051,
 25.178,25.304,25.431,25.558,25.686,25.813,25.940,26.068,
 26.196,26.324,26.452,26.580,26.708,26.837,26.965,27.094,
 27.223,27.352,27.481,27.611,27.740,27.870,28.000,28.130,
 28.260,28.390,28.521,28.651,28.782,28.913,29.044,29.176,
 29.307,29.439,29.570,29.702,29.835,29.967,30.099,30.232,
 30.365,30.498,30.631,30.765,30.898,31.032,31.166,31.300,
 31.434,31.569,31.704,31.839,31.974,32.109,32.245,32.380,
 32.516,32.652,32.789,32.925,33.062,33.199,33.336,33.474,
 33.611,33.749,33.887,34.025,34.164,34.303,34.442,34.581,
 34.720,34.860,35.000,35.140,35.280,35.421,35.562,35.703,
 35.844,35.986,36.128,36.270,36.412,36.555,36.698,36.841,
 36.985,37.128,37.272,37.417,37.561,37.706,37.851,37.997,
 38.142,38.288,38.435,38.581,38.728,38.875,39.023,39.171,
 39.319,39.467,39.616,39.765,39.914,40.064,40.214,40.364,
 40.515,40.666,40.818,40.969,41.121,41.274,41.427,41.580,
 41.733,41.887,42.042,42.196,42.351,42.507,42.662,42.819,
 42.975,43.132,43.290,43.447,43.606,43.764,43.923,44.083,
 44.243,44.403,44.564,44.725,44.887,45.049,45.211,45.374,
 45.538,45.702,45.866,46.031,46.197,46.363,46.529,46.696,
 46.864,47.032,47.200,47.369,47.539,47.709,47.880,48.051,
 48.223,48.396,48.569,48.742,48.917,49.091,49.267,49.443,
 49.620,49.797,49.975,50.154,50.333,50.513,50.694,50.875,
 51.058,51.240,51.424,51.608,51.793,51.979,52.166,52.353,
 52.541,52.730,52.920,53.111,53.303,53.495,53.688,53.882,
 54.077,54.273,54.470,54.668,54.867,55.067,55.268,55.469,
 55.672,55.876,56.081,56.287,56.495,56.703,56.913,57.123,
 57.335,57.548,57.763,57.978,58.195,58.414,58.633,58.854,
 59.077,59.301,59.526,59.753,59.982,60.212,60.444,60.677,
 60.912,61.149,61.387,61.628,61.870,62.114,62.361,62.609,
 62.859,63.112,63.367,63.624,63.883,64.145,64.409,64.676,
 64.946,65.218,65.493,65.771,66.052,66.336,66.624,66.914,
 67.209,67.507,67.808,68.114,68.424,68.738,69.057,69.380,
 69.708,70.041,70.380,70.725,71.075,71.432,71.796,72.167,
 72.545,72.932,73.327,73.732,74.146,74.572,75.009,75.459,
 75.923,76.403,76.899,77.416,77.953,78.516,79.107,79.732,
 80.397,81.110,81.886,82.744,83.717,84.871,86.374,90.000
};

/* ----------------------------------------------------------------- ** 
** vectorRedraw - Frame standard redraw method                       ** 
** ----------------------------------------------------------------- */
void vectorRedraw(this, frame, poly)
c_VectorGraph	*this;
c_Cadre		*frame;
Polygone	*poly;
{
 coord_t text_leading = COORD_ZERO;
 coord_t text_rise, text_ws, text_cs, text_size;
 vector_state_t *gstate, *ogstate;
 vector_item_t *item, *last_item;
 c_VectorFont *text_font = NULL;
 c_Matrix *text_matrix;
 scale_t text_scale;
 StackHandle *gstack;
 XGCValues gcdata;
 int text_mode, dx, dy;
 double scale;
 GC mask_gc;
 scale_t	special;
 double		osinang;
 double		oaddang;
 int		ocosign;
 
 Xc_HISTORY(("redraw"));
 XcM_DUMP_MATRIX("matrix", F(this->matrix).get(this->matrix, 0));
 osinang = 1;
 ocosign = 0;
 oaddang = 0;
 
#ifndef NTRACE
 XSynchronize(frame->X_info.display, True);
#endif
 
#if 0
 /*--- Build redraw area bounding box ---*/
 area.llx = poly->Orig_X * frame->sScale - frame->sOrig_X;
 area.lly = poly->Orig_Y * frame->sScale - frame->sOrig_Y;
 area.urx = (poly->x_max + poly->Orig_X) * frame->sScale - frame->sOrig_X;
 area.ury = (poly->y_max + poly->Orig_Y) * frame->sScale - frame->sOrig_Y;
#endif
 dx = (frame->sOrig_X + frame->saOrig_X) / frame->sScale;
 dy = (frame->sOrig_Y + frame->saOrig_Y) / frame->sScale;
 
 /*--- Text state ---*/
 text_size = text_rise = COORD_ZERO;
 text_ws = text_cs = COORD_ZERO;
 text_mode = 0;
 text_scale = SCALE_ONE;
 
 /*--- Text matrix ---*/
 if((text_matrix = NEW(c_Matrix)()) == NULL) return;
 F(text_matrix).translate(text_matrix, FALSE,
			  SCALE_FROM_POINTS(dx), SCALE_FROM_POINTS(dy));
 F(text_matrix).scale(text_matrix, FALSE,
		      SCALE_FROM_COEF(COORD_SCALEVALUE),
		      SCALE_FROM_COEF(COORD_SCALEVALUE));
 
 /* concat with this->matrix !!! */
 F(text_matrix).concat(text_matrix, FALSE,
		       F(this->matrix).get(this->matrix, 0));
 
 special = sqrt((this->matrix->base->M[0][0]*this->matrix->base->M[0][0]) +
		(this->matrix->base->M[0][1]*this->matrix->base->M[0][1])) *
		 1024;
 
 if(!F(text_matrix).push(text_matrix))
 {
  DELETE(c_Matrix)(text_matrix);
  return;
 }
 XcM_DUMP_MATRIX("text-matrix", F(text_matrix).get(text_matrix, 0));
 
 /*--- Create mask GC ---*/
 gcdata.foreground = 1;
 gcdata.background = 0;
 mask_gc = XCreateGC(frame->X_info.display, frame->clip,
		     GCForeground|GCBackground, &gcdata);
 
 /*--- Setup graphic context ---*/
 gstack = Xc_stackCreate("gstate stack", sizeof(vector_state_t), 4);
 if(gstack == NULL) return;
 gstate = Xc_stackMalloc(gstack);
 
 gstate->gc = frame->X_info.gc_inside;
 gstate->current_color = gstate->stroke_color = gstate->fill_color = NULL;
 gstate->clip_flag = FALSE;
 gstate->mask = frame->clip;
 
 vectorPathCacheInit(&gstate->path_cache, NULL);
 
 scale = SCALE_TO_COEF(this->scale);
 item = this->base_head, last_item = NULL;
 while((item != NULL) && (gstate != NULL))
 {
  if (this->realtime_draw)
  {
   XEvent event;
   
   if (XCheckTypedEvent(frame->X_info.display, 
			MotionNotify, &event) == True)
   {
    XPutBackEvent(frame->X_info.display, &event);
    break;
   }
  }

  /*--- Item specific print ---*/
  switch(item->type)
  {
   /*--- Context ---*/
  case XcVG_CONTEXT_TYPE:
   Xc_ASSERT(item->data.children.tail != NULL);
   if(last_item != item->data.children.tail)
   {
    /*--- Push ---*/
    Xc_TRACE(("  save(%d)", (int)Xc_stackDepth(gstack)));
    Xc_ASSERT(item->data.children.head != NULL);

    /*--- Push new gstate ---*/
    Xc_stackBottomPush(gstack, gstate);
    ogstate = gstate;
    if((gstate = Xc_stackMalloc(gstack)) == NULL) break;

    /*--- Copy gstate info ---*/
    memcpy(gstate, ogstate, sizeof(vector_state_t));
    gstate->clip_flag = FALSE;
    gstate->gc = XCreateGC(frame->X_info.display,
			   frame->X_info.window, 0L, &gcdata);
    XCopyGC(frame->X_info.display, ogstate->gc, -1L, gstate->gc);
    vectorPathCacheInit(&gstate->path_cache, &ogstate->path_cache);

    last_item = item;
    item = item->data.children.head;
    continue;
   }

   /*--- Pop ---*/
   Xc_TRACE(("  restore(%d)", (int)Xc_stackDepth(gstack) - 1));
	  
   if(gstate->clip_flag)
    XFreePixmap(frame->X_info.display, gstate->mask);
   XFreeGC(frame->X_info.display, gstate->gc);
   vectorPathCacheFree(&gstate->path_cache);
	  
   gstate = Xc_stackBottomPop(gstack);
   break;
	  
   /*--- Path decription ---*/
  case XcVG_PATH_TYPE:
   Xc_TRACE(("  path"));
   vectorPathFlatten(&item->data.path, this->matrix,
		     &gstate->path_cache);
   break;
	  
   /*--- Render path ---*/
  case XcVG_RENDER_TYPE:
   {
    Xc_TRACE(("  render=%d (%d;%d)", item->data.render_mode, dx, dy));
	    
    /*--- Fill path ---*/
    if((item->data.render_mode & XcVG_FILL) &&
       (gstate->fill_color != NULL))
    {
     if(gstate->current_color != gstate->fill_color)
     {
      F(gstate->fill_color).setGC(gstate->fill_color,
				  frame->X_info.display,
				  gstate->gc);
      gstate->current_color = gstate->fill_color;
     }

     vectorPathCacheFill(&gstate->path_cache, dx, dy,
			 frame->X_info.display,
			 frame->X_info.window, gstate->gc);
    }
	    
    /*--- Stroke path ---*/
    if((item->data.render_mode & XcVG_STROKE) &&
       (gstate->stroke_color != NULL))
    {
     if(gstate->current_color != gstate->stroke_color)
     {
      F(gstate->stroke_color).setGC(gstate->stroke_color,
				    frame->X_info.display,
				    gstate->gc); 
      gstate->current_color = gstate->stroke_color;
     }

     vectorPathCacheStroke(&gstate->path_cache, dx, dy,
			   frame->X_info.display,
			   frame->X_info.window, gstate->gc);
    }
    vectorPathCacheReset(&gstate->path_cache);

    break;
   }

   /*--- Clip ---*/
  case XcVG_CLIP_TYPE:
   {
    Pixmap mask;
    int dx, dy;

    Xc_TRACE(("  clip"));
    dx = (frame->sOrig_X + frame->saOrig_X) / frame->sScale;
    dy = (frame->sOrig_Y + frame->saOrig_Y) / frame->sScale;

    XSetFunction(frame->X_info.display, mask_gc, GXcopy);

    if(gstate->clip_flag == FALSE)
    {
     /*--- Copy previous mask ---*/
     Xc_TRACE(("   copy mask"));
     mask = XCreatePixmap(frame->X_info.display,
			  frame->X_info.window,
			  MASK_WIDTH(poly), MASK_HEIGHT(poly), 1);
     XCopyArea(frame->X_info.display, gstate->mask, mask, mask_gc,
	       0, 0, MASK_WIDTH(poly), MASK_HEIGHT(poly), 0, 0);
		
     gstate->mask = mask;
     gstate->clip_flag = TRUE;
    }
	    
    mask = XCreatePixmap(frame->X_info.display,
			 frame->X_info.window,
			 MASK_WIDTH(poly), MASK_HEIGHT(poly), 1);
    XSetForeground(frame->X_info.display, mask_gc, 0L);
    XFillRectangle(frame->X_info.display, mask, mask_gc, 0, 0,
		   MASK_WIDTH(poly), MASK_HEIGHT(poly));
    XSetForeground(frame->X_info.display, mask_gc, 1L);
    vectorPathCacheFill(&gstate->path_cache,
			dx - poly->Orig_X, dy - poly->Orig_Y,
			frame->X_info.display, mask, mask_gc);


    XSetFunction(frame->X_info.display, mask_gc, GXand);
    XCopyArea(frame->X_info.display, mask, gstate->mask, mask_gc,
	      0, 0, MASK_WIDTH(poly), MASK_HEIGHT(poly), 0, 0);
    gcdata.clip_mask = gstate->mask;
    XChangeGC(frame->X_info.display, gstate->gc, GCClipMask, &gcdata);

#if 0
    {
     extern c_Colormap *GlobColormap;
     GC tmp_gc;

     gcdata.foreground = GlobColormap->Cblack;
     gcdata.background = GlobColormap->Cwhite;
     gcdata.subwindow_mode = IncludeInferiors;

     tmp_gc = XCreateGC(frame->X_info.display,
			DefaultRootWindow(frame->X_info.display),
			GCForeground|GCBackground|GCSubwindowMode,
			&gcdata);
     XCopyPlane(frame->X_info.display, gstate->mask,
		DefaultRootWindow(frame->X_info.display),
		tmp_gc, 0, 0, MASK_WIDTH(poly), MASK_HEIGHT(poly),
		0, 0, 1L);
     XFreeGC(frame->X_info.display, tmp_gc);
    }
#endif
    XFreePixmap(frame->X_info.display, mask);
    break;
   }

   /*--- Stroke color ---*/
  case XcVG_STROKE_COLOR_TYPE:
   Xc_TRACE(("  stroke color=`%s'", item->data.color->name));
   gstate->stroke_color = item->data.color;
   break;
	  
   /*--- Fill color ---*/
  case XcVG_FILL_COLOR_TYPE:
   Xc_TRACE(("  fill color=`%s'", item->data.color->name));
   gstate->fill_color = item->data.color;

   break;
	  
   /*--- Line width ---*/
  case XcVG_LINE_WIDTH_TYPE:
   Xc_TRACE(("  line width=%g",SCALE_TO_POINTS(item->data.line_width)));
   if((gcdata.line_width = item->data.line_width * scale) <= 1)
    gcdata.line_width = 0;
   XChangeGC(frame->X_info.display, gstate->gc, GCLineWidth, &gcdata);
   break;
	  
   /*--- Line join style ---*/
  case XcVG_LINE_JOIN_TYPE:
   Xc_TRACE(("  line join=%d", item->data.line_join));
   if(item->data.line_join == 1)
    gcdata.join_style = JoinRound;
   else if(item->data.line_join == 2)
    gcdata.join_style = JoinBevel;
   else
    gcdata.join_style = JoinMiter;
   XChangeGC(frame->X_info.display, gstate->gc, GCJoinStyle, &gcdata);
   break;
	  
   /*--- Line cap style ---*/
  case XcVG_LINE_CAP_TYPE:
   Xc_TRACE(("  line cap=%d", item->data.line_cap));
   if(item->data.line_cap == 1)
    gcdata.cap_style = CapRound;
   else if(item->data.line_cap == 2)
    gcdata.cap_style = CapProjecting;
   else
    gcdata.cap_style = CapButt;
   XChangeGC(frame->X_info.display, gstate->gc, GCCapStyle, &gcdata);
   break;
	  
   /*--- Flatness ---*/
  case XcVG_FLATNESS_TYPE:
   Xc_TRACE(("  flatness"));
   /*### Nothing ###*/
   break;


   /*--- Miter limit ---*/
  case XcVG_MITER_LIMIT_TYPE:
   Xc_TRACE(("  miter limit"));
   /*### Nothing ###*/
   break;

   /*--- Line dash style ---*/
  case XcVG_LINE_DASH_TYPE:
   {
    char dashes[8];
    int i;

    Xc_TRACE(("  line dash"));
    if(item->data.line_dash.length)
    {
     for(i = 0; i < (int)item->data.line_dash.length; i++)
     {
      dashes[MIN(i, 8)] =
       MAX(1, (char)(item->data.line_dash.pattern[i] * scale));
      Xc_TRACE(("   %d", dashes[MIN(i, 8)]));
     }
     XSetDashes(frame->X_info.display, gstate->gc,
		(int)(item->data.line_dash.phase * scale),
		dashes, item->data.line_dash.length);
     gcdata.line_style = LineOnOffDash;
     XChangeGC(frame->X_info.display, gstate->gc, GCLineStyle,
	       &gcdata);
    }
    else
    {
     gcdata.line_style = LineSolid;
     XChangeGC(frame->X_info.display, gstate->gc, GCLineStyle,
	       &gcdata);
    }
    break;
   }

   /*--- Text matrix ---*/
  case XcVG_TEXT_MATRIX_TYPE:
   XcM_DUMP_MATRIX("  text-matrix", &item->data.matrix);
   F(text_matrix).pop(text_matrix);
   F(text_matrix).concat(text_matrix, TRUE, &item->data.matrix);
   break;

   /*--- Text matrix translation ---*/
  case XcVG_TEXT_TRANSLATE_TYPE:
   Xc_TRACE(("  text-translate(%g;%g)",
	     SCALE_TO_POINTS(item->data.vector.dx),
	     SCALE_TO_POINTS(item->data.vector.dy)));
   F(text_matrix).translate(text_matrix, FALSE, item->data.vector.dx,
			    item->data.vector.dy);
   break;

   /*--- Text newline ---*/
  case XcVG_TEXT_NEWLINE_TYPE:
   Xc_TRACE(("  text-newline"));
   F(text_matrix).translate(text_matrix, FALSE,
			    COORD_ZERO, text_leading);
   break;

   /*--- Text rise ---*/
  case XcVG_TEXT_RISE_TYPE:
   Xc_TRACE(("  text-rise(%g)", SCALE_TO_POINTS(item->data.offset)));
   text_rise = item->data.offset;
   break;

   /*--- Text render mode ---*/
  case XcVF_TEXT_MODE_TYPE:
   Xc_TRACE(("  text-mode(%d)", item->data.render_mode));
   text_mode = item->data.render_mode;
   break;

   /*--- Text font ---*/
  case XcVG_TEXT_FONT_TYPE:
   Xc_TRACE(("  text-font(%s, %g)", item->data.font.data->font_name,
	     SCALE_TO_POINTS(item->data.font.size)));
   text_font = item->data.font.data;
   text_size = item->data.font.size;
   break;

   /*--- text nealine leading ---*/
  case XcVG_TEXT_LEADING_TYPE:
   Xc_TRACE(("  text-leading(%g)", SCALE_TO_POINTS(item->data.offset)));
   text_rise = item->data.offset;
   break;

   /*--- Text word spacing ---*/
  case XcVG_TEXT_WORD_SPACING_TYPE:
   Xc_TRACE(("  text-word-spacing(%g)",
	     SCALE_TO_POINTS(item->data.offset)));
   text_ws = item->data.offset;
   break;

   /*--- Text char spacing ---*/
  case XcVG_TEXT_CHAR_SPACING_TYPE:
   Xc_TRACE(("  text-char-spacing(%g)",
	     SCALE_TO_POINTS(item->data.offset)));
   text_cs = item->data.offset;
   break;

   /*--- Text horizontal scale ---*/
  case XcVG_TEXT_HSCALE_TYPE:
   Xc_TRACE(("  text-hscale(%g%%)",
	     SCALE_TO_PERCENT(item->data.scale)));
   text_scale = item->data.scale;
   break;

   /*--- Text kerning ---*/
  case XcVG_TEXT_KERNING_TYPE:
   Xc_TRACE(("  text-kerning"));
   break;

   /*--- Text rendering ---*/
  case XcVG_TEXT_RENDER_TYPE:
   Xc_TRACE(("  text-render"));
   if(this->drawtext_flag == FALSE) break;
   if(gstate->fill_color != NULL)
   {
    c_CharMetrics *metrics;
    matrix_t *m;
    real	test;
    scale_t	addang;
    double	sinang;
    int		cosign;

    if(gstate->current_color != gstate->fill_color)
    {
     F(gstate->fill_color).setGC(gstate->fill_color,
				 frame->X_info.display,
				 gstate->gc);
     gstate->current_color = gstate->fill_color;
    }
	      
    F(text_matrix).translate(text_matrix, TRUE,
			     COORD_ZERO, text_rise);
    F(text_matrix).scale(text_matrix, FALSE,
			 SCALE_ONE, -SCALE_ONE);
    
    m = F(text_matrix).get(text_matrix, 0);
    
    XcM_DUMP_MATRIX("    matrix", m);
    test = m->M[0][1];
    m->M[0][1] = m->M[1][0];
    m->M[1][0] = test;    
    metrics = F(text_font->metrics).makefont(text_font->metrics, m,
					     text_size, text_scale);
    if(metrics != NULL)
    {
     c_RasterFont *font;
		  
     font = F(GlobFntMng).new_font(GlobFntMng,
				   text_font, metrics);
     if((metrics->raster_font = font) != NULL)
     {
      GC gc_mask;
		      
#ifdef Xc_XCLAMATION
      gc_mask = (GC)0;
		      
      F(font).setGC(frame->X_info.display, frame->X_info.window,
		    &gc_mask, gstate->gc);
#else
      gc_mask = XCreateGC(frame->X_info.display, frame->X_info.window,
			  0, NULL);
      F(font).setGC(frame->X_info.display, frame->X_info.window,
		    gc_mask, gstate->gc);
#endif
      sinang = m->M[0][1]/special;

      if (m->M[0][0] < 0)
       cosign = 1;
      else
       cosign = 0;

      if ((sinang == osinang) && (cosign == ocosign))
       addang = oaddang;
      else
      {
       if (sinang >= 1)
	addang = SCALE_FROM_DEGREES(90);
       else
	if (sinang <= -1)
	 addang = -SCALE_FROM_DEGREES(90);
	else
	 addang = asin_table[(int)((sinang+1)*500)];
       /*addang = SCALE_FROM_RADIANS(asin(sinang));*/
       
       if (cosign)
       {
	if (addang > 0)
	 addang = SCALE_FROM_DEGREES(180.0) - addang;
	else
	 addang = -SCALE_FROM_DEGREES(180.0) - addang;
       }
#if 0
       if (addang < 0)
	addang += SCALE_FROM_DEGREES(360.0);
#endif
       ocosign = cosign;
       osinang = sinang;
       oaddang = addang;
      }


      font->metrics->angle += addang;

      F(font).drawString(font, frame->X_info.display,
			 frame->X_info.window,
			 gc_mask, gstate->gc,
			 &m->v, (coord_t)text_cs*special, 
			 (coord_t)text_ws*special,
			 item->data.text.string,
			 item->data.text.length, NULL);

      font->metrics->angle -= addang;

      if(gc_mask != (GC)0) XFreeGC(frame->X_info.display,
				   gc_mask);
		      
      XSetFunction(frame->X_info.display,
		   gstate->gc, GXcopy);

      F(GlobFntMng).delete_font(GlobFntMng, font);
     }
     F(text_font->metrics).unmakefont(text_font->metrics, m,
				      text_size, text_scale);
    }
	      
    F(text_matrix).pop(text_matrix);
   }
   break;

  default:
   Xc_BREAK(("Unknown item type"));
  }
      
  last_item = item;
  item = item->next;
 }  
  
 XFreeGC(frame->X_info.display, mask_gc);
 vectorPathCacheFree(&gstate->path_cache);
 Xc_stackDestroy(gstack);
 DELETE(c_Matrix)(text_matrix);
 Xc_TRACE(("redraw done"));

#ifndef NTRACE
 XSynchronize(frame->X_info.display, False);
#endif
}
