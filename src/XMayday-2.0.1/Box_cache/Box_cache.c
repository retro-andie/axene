/*
** Box_cache.c for Xclamation, XAllWrite and XMayday in Box_cache/
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
** Started on  Mon Jul 17 13:54:22 1995 Robin Castanier
** Last update Sun Mar 16 14:09:35 1997 Robin Castanier
*/

#include "Box_cache.h"
#include "ImageCache.h"

extern c_ImageCache *GlobImgCache;

void *cons_Box_cache();
void dest_Box_cache();
void callback_dest_BoxCache();
static void toggle_keep_disk();
static void certify_memory_cache();
static void certify_disk_cache();
static void empty_mem_cache_callback();
static void empty_disk_cache_callback();

sf_Box_cache fc_Box_cache =
{
 cons_Box_cache,
 dest_Box_cache,
};

void *cons_Box_cache(w_Parent, pszTitle)
Widget w_Parent;
char   *pszTitle;
{
 Display        *display;
 c_Box_cache	 *ObjTmp;
 Arg		 argsw[4];
 int		 nargs;
 char           str[10];

 if ((ObjTmp = (c_Box_cache *)Xc_malloc("Box_cache",
					sizeof(c_Box_cache))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_Box_cache;
 
 ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							pszTitle, 350, 390,
							XcCD_HORIZONTAL
							| XcCD_HRESIZE
							| XcCD_VRESIZE);
 ObjTmp->w_This = WIDGET(ObjTmp->Dialog);
 ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				      MANAGER(ObjTmp->Dialog),
				      "gBoxCache", FALSE);
  
 display = XtDisplay(w_Parent);

 ObjTmp->lTitle = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid),
			  MANAGER(ObjTmp->Dialog),
			  "lBoxCacheTitle");

 nargs = 0;

 ObjTmp->Frame1 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fBCmemoryCache", 0);
 ObjTmp->Grid1 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame1),
				       MANAGER(ObjTmp->Dialog),
				       "gBCFrame1", FALSE);

 ObjTmp->lMemTotal1 = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
			  MANAGER(ObjTmp->Dialog),
			  "lMemTotal");

 sprintf(str, "%ld", GlobImgCache->max_size_in_memory >> 10);
 ObjTmp->tfMemTotal = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid1),
				    MANAGER(ObjTmp->Dialog),
				    "tfMemTotal",
				    TF_INTEGER,
				    0.0, 100000.0,
				    str, NULL, 7);
 F(ObjTmp->tfMemTotal).add_callback(ObjTmp->tfMemTotal,
				    certify_memory_cache, ObjTmp);
 ObjTmp->lMemTotal2 = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
			  MANAGER(ObjTmp->Dialog),
			  "lKb");

 ObjTmp->lMemUsed1 = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
			  MANAGER(ObjTmp->Dialog),
			  "lMemUsed");

 sprintf(str, "%ld", GlobImgCache->size_in_memory >> 10);
 ObjTmp->tfMemUsed = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid1),
				    MANAGER(ObjTmp->Dialog),
				    "tfMemUsed",
				    TF_INTEGER,
				    0.0, 1000000.0,
				    str, NULL, 7);
 ObjTmp->lMemUsed2 = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
			  MANAGER(ObjTmp->Dialog),
			  "lKb");

 ObjTmp->lMemFree1 = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
			  MANAGER(ObjTmp->Dialog),
			  "lMemFree");

 sprintf(str, "%ld", (GlobImgCache->max_size_in_memory >> 10) - 
	 (GlobImgCache->size_in_memory >> 10));
 ObjTmp->tfMemFree = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid1),
				    MANAGER(ObjTmp->Dialog),
				    "tfMemFree",
				    TF_INTEGER,
				    0.0, 1000000.0,
				    str, NULL, 7);
 ObjTmp->lMemFree2 = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid1),
			  MANAGER(ObjTmp->Dialog),
			  "lKb");

 ObjTmp->wPush1 = XmCreatePushButton(WIDGET(ObjTmp->Grid1),
				     "pbBCCleanMemCache",
				     NULL, 0);
 CALLBACK(ObjTmp->wPush1, FUNC_ACTIVATE, empty_mem_cache_callback, ObjTmp);
 XtManageChild(ObjTmp->wPush1);



 ObjTmp->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog), 
					  "fBCdiskCache", 0);
 ObjTmp->Grid2 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame2),
				       MANAGER(ObjTmp->Dialog),
				       "gBCFrame2", FALSE);

 ObjTmp->lDiskTotal1 = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
			  MANAGER(ObjTmp->Dialog),
			  "lDiskTotal");
 sprintf(str, "%ld", GlobImgCache->max_size_on_disk >> 10);
 ObjTmp->tfDiskTotal = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid2),
				    MANAGER(ObjTmp->Dialog),
				    "tfDiskTotal",
				    TF_INTEGER,
				    0.0, 1000000.0,
				    str, NULL, 7);
 if (GlobImgCache->disk_cache_available)
  F(ObjTmp->tfDiskTotal).add_callback(ObjTmp->tfDiskTotal,
				      certify_disk_cache, ObjTmp);
 ObjTmp->lDiskTotal2 = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
			  MANAGER(ObjTmp->Dialog),
			  "lKb");

 ObjTmp->lDiskUsed1 = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
			  MANAGER(ObjTmp->Dialog),
			  "lDiskUsed");
 sprintf(str, "%ld", GlobImgCache->size_on_disk >> 10);
 ObjTmp->tfDiskUsed = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid2),
				    MANAGER(ObjTmp->Dialog),
				    "tfDiskUsed",
				    TF_INTEGER,
				    0.0, 1000000.0,
				    str, NULL, 7);
 ObjTmp->lDiskUsed2 = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
			  MANAGER(ObjTmp->Dialog),
			  "lKb");

 ObjTmp->lDiskFree1 = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
			  MANAGER(ObjTmp->Dialog),
			  "lDiskFree");

 if (GlobImgCache->disk_cache_available)
 {
  sprintf(str, "%ld", 
	  (GlobImgCache->max_size_on_disk >> 10)
	  - (GlobImgCache->size_on_disk >> 10));
 }
 else
 {
  str[0] = '0';
  str[1] = '\0';
 }
 ObjTmp->tfDiskFree = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid2),
				    MANAGER(ObjTmp->Dialog),
				    "tfDiskFree",
				    TF_INTEGER,
				    0.0, 1000000.0,
				    str, NULL, 7);
 ObjTmp->lDiskFree2 = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid2),
			  MANAGER(ObjTmp->Dialog),
			  "lKb");

 ObjTmp->wPush2 = XmCreatePushButton(WIDGET(ObjTmp->Grid2),
				     "pbBCCleanDiskCache",
				     NULL, 0);
 if (GlobImgCache->disk_cache_available)
  CALLBACK(ObjTmp->wPush2, FUNC_ACTIVATE, empty_disk_cache_callback, ObjTmp);
 XtManageChild(ObjTmp->wPush2);

 nargs = 0;
 XtSetArg(argsw[nargs], XmNeditable, False); nargs++;
 XtSetArg(argsw[nargs], XmNcursorPositionVisible, False); nargs++;
 XtSetArg(argsw[nargs], XmNtraversalOn, False); nargs++;
 XtSetArg(argsw[nargs], XmNselectionArrayCount, 0); nargs++;
 XtSetValues(WIDGET(ObjTmp->tfMemUsed), argsw, nargs);
 XtSetValues(WIDGET(ObjTmp->tfMemFree), argsw, nargs);
 XtSetValues(WIDGET(ObjTmp->tfDiskUsed), argsw, nargs);
 XtSetValues(WIDGET(ObjTmp->tfDiskFree), argsw, nargs);


 /*
   ObjTmp->Frame3 = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
   MANAGER(ObjTmp->Dialog), 
   "fBCDiskCachePath", 0);
   ObjTmp->Grid3 = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Frame3),
   MANAGER(ObjTmp->Dialog),
   "gBCFrame3", TRUE);
   ObjTmp->tfDiskCachePath = 
   (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Grid3),
   MANAGER(ObjTmp->Dialog),
   "tfDiskCachePath",
   TF_STRING,
   0.0, 100.0,
   "~/.xclamation.cache/", NULL, 7);
   
   ObjTmp->IconBrowse = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->Grid3), 
   MANAGER(ObjTmp->Dialog),
   "IconBrowse", display,
   browse_xpm, NULL, XcPush);
   */

 ObjTmp->tbBCKeepCacheOnDisk = 
  (c_ToggleButton *)NEW(c_ToggleButton)(WIDGET(ObjTmp->Grid),
					MANAGER(ObjTmp->Dialog),
					"tbBCKeepCacheOnDisk", NULL);
 
 if (!GlobImgCache->disk_cache_available)
 {
  XtSetSensitive(WIDGET(ObjTmp->lDiskTotal1), False);
  XtSetSensitive(WIDGET(ObjTmp->tfDiskTotal), False);
  XtSetSensitive(WIDGET(ObjTmp->lDiskTotal2), False);
  XtSetSensitive(WIDGET(ObjTmp->lDiskUsed1), False);
  XtSetSensitive(WIDGET(ObjTmp->tfDiskUsed), False);
  XtSetSensitive(WIDGET(ObjTmp->lDiskUsed2), False);
  XtSetSensitive(WIDGET(ObjTmp->lDiskFree1), False);
  XtSetSensitive(WIDGET(ObjTmp->tfDiskFree), False);
  XtSetSensitive(WIDGET(ObjTmp->lDiskFree2), False);
  XtSetSensitive(ObjTmp->wPush2, False);
  XtSetSensitive(WIDGET(ObjTmp->tbBCKeepCacheOnDisk), False);
 }

 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_MAX_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lMemTotal1), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lMemUsed1), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lMemFree1), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->tfMemTotal), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->tfMemUsed), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->tfMemFree), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lMemTotal2), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lMemUsed2), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lMemFree2), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, 10, NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid1, XcG_BEST_SIZE,
			    ObjTmp->wPush1, XcG_MAX_SIZE,
			    NULL);

  
 F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_MAX_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lDiskTotal1), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lDiskUsed1), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lDiskFree1), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->tfDiskTotal), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->tfDiskUsed), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->tfDiskFree), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lDiskTotal2), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lDiskUsed2), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lDiskFree2), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->Grid1).addColumn(ObjTmp->Grid2, 10, NULL);
 F(ObjTmp->Grid2).addColumn(ObjTmp->Grid2, XcG_BEST_SIZE,
			    ObjTmp->wPush2, XcG_MAX_SIZE,
			    NULL);
 F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			   WIDGET(ObjTmp->lTitle), 30,
			   WIDGET(ObjTmp->Frame1), XcG_MAX_SIZE,
			   WIDGET(ObjTmp->Frame2), XcG_MAX_SIZE,
			   /*	 WIDGET(ObjTmp->Frame3), 75, */
			   XcG_WIDGET_NULL, 5,
			   WIDGET(ObjTmp->tbBCKeepCacheOnDisk), XcG_BEST_SIZE,
			   NULL);

 F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, 
				    XcCD_OK/*| XcCD_CANCEL*/,
				    XcCD_OK,
				    TRUE, NULL, NULL);
  
 F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);
  
 if (GlobImgCache->keep_cache_on_disk)
  F(ObjTmp->tbBCKeepCacheOnDisk).Set(ObjTmp->tbBCKeepCacheOnDisk,
				     True, False);
 F(ObjTmp->tbBCKeepCacheOnDisk).Add_Callback(ObjTmp->tbBCKeepCacheOnDisk, 
					     XmNvalueChangedCallback,
					     toggle_keep_disk, ObjTmp);

 Xc_TRACE(("Object Box_cache build"));
 return ObjTmp;
}

void dest_Box_cache(This)
c_Box_cache *This;
{
 DELETE(c_Label)(This->lMemTotal1);
 DELETE(c_Label)(This->lMemTotal2);
 DELETE(c_Label)(This->lMemUsed1);
 DELETE(c_Label)(This->lMemUsed2);
 DELETE(c_Label)(This->lMemFree1);
 DELETE(c_Label)(This->lMemFree2);
 DELETE(c_Label)(This->lDiskTotal1);
 DELETE(c_Label)(This->lDiskTotal2);
 DELETE(c_Label)(This->lDiskUsed1);
 DELETE(c_Label)(This->lDiskUsed2);
 DELETE(c_Label)(This->lDiskFree1);
 DELETE(c_Label)(This->lDiskFree2);
 DELETE(c_WTextField)(This->tfMemTotal);
 DELETE(c_WTextField)(This->tfMemUsed);
 DELETE(c_WTextField)(This->tfMemFree);
 DELETE(c_WTextField)(This->tfDiskTotal);
 DELETE(c_WTextField)(This->tfDiskUsed);
 DELETE(c_WTextField)(This->tfDiskFree);
 /*DELETE(c_WTextField)(This->tfDiskCachePath);*/
 XtDestroyWidget(This->wPush1);
 XtDestroyWidget(This->wPush2);
 DELETE(c_ToggleButton)(This->tbBCKeepCacheOnDisk);
 /*DELETE(c_Icon)(This->IconBrowse);*/
 DELETE(c_Grid)(This->Grid1);
 DELETE(c_Grid)(This->Grid2);
 /*DELETE(c_Grid)(This->Grid3);*/
 DELETE(c_Frame)(This->Frame1);
 DELETE(c_Frame)(This->Frame2);
 /*DELETE(c_Frame)(This->Frame3);*/
 DELETE(c_Label)(This->lTitle);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);

 Xc_free(This);
 Xc_TRACE(("Object Box_cache destroyed"));
}

void callback_dest_BoxCache(wid, This)
Widget	    wid;
c_Box_cache *This;
{
 DELETE(c_Box_cache)(This);
}

static void toggle_keep_disk(this, Obj)
Widget		 this;
c_Box_cache	*Obj;
{
 GlobImgCache->keep_cache_on_disk = !(GlobImgCache->keep_cache_on_disk);
}


static void certify_memory_cache(this, Obj)
c_WTextField	*this;
c_Box_cache	*Obj;
{
 char	*vstr;
 long	num;
 char  str[10];
  
 if((vstr = F(this).get(this)) != NULL)
 {
  num = atoi(vstr);
  Xc_free(vstr);
  F(GlobImgCache).set_memory_cache_size(GlobImgCache, num << 10);

  sprintf(str, "%ld", GlobImgCache->size_in_memory >> 10);
  F(Obj->tfMemUsed).set(Obj->tfMemUsed, str);      
  sprintf(str, "%ld", (GlobImgCache->max_size_in_memory >> 10) - 
	  (GlobImgCache->size_in_memory >> 10));
  F(Obj->tfMemFree).set(Obj->tfMemFree, str);

  if (GlobImgCache->disk_cache_available)
  {
   sprintf(str, "%ld", GlobImgCache->size_on_disk >> 10);
   F(Obj->tfDiskUsed).set(Obj->tfDiskUsed, str);      
   sprintf(str, "%ld", (GlobImgCache->max_size_on_disk >> 10) - 
	   (GlobImgCache->size_on_disk >> 10));
   F(Obj->tfDiskFree).set(Obj->tfDiskFree, str);
  }
  F(Obj->Dialog).resetTraversal(Obj->Dialog);
 }
}

static void certify_disk_cache(this, Obj)
c_WTextField	*this;
c_Box_cache	*Obj;
{
 char	*vstr;
 long	num;
 char  str[10];
  
 if((vstr = F(this).get(this)) != NULL)
 {
  num = atoi(vstr);
  Xc_free(vstr);
  F(GlobImgCache).set_disk_cache_size(GlobImgCache, num << 10);

  sprintf(str, "%ld", GlobImgCache->size_on_disk >> 10);
  F(Obj->tfDiskUsed).set(Obj->tfDiskUsed, str);      
  sprintf(str, "%ld", (GlobImgCache->max_size_on_disk >> 10) - 
	  (GlobImgCache->size_on_disk >> 10));
  F(Obj->tfDiskFree).set(Obj->tfDiskFree, str);

  F(Obj->Dialog).resetTraversal(Obj->Dialog);
 }
}

static void empty_mem_cache_callback(w, Obj)
Widget      w;
c_Box_cache *Obj;
{
 char  str[10];

 F(GlobImgCache).empty_memory_cache(GlobImgCache);

 sprintf(str, "%ld", GlobImgCache->size_in_memory >> 10);
 F(Obj->tfMemUsed).set(Obj->tfMemUsed, str);      
 sprintf(str, "%ld", (GlobImgCache->max_size_in_memory >> 10) - 
	 (GlobImgCache->size_in_memory >> 10));
 F(Obj->tfMemFree).set(Obj->tfMemFree, str);
}

static void empty_disk_cache_callback(w, Obj)
Widget      w;
c_Box_cache *Obj;
{
 char  str[10];

 F(GlobImgCache).empty_disk_cache(GlobImgCache);

 sprintf(str, "%ld", GlobImgCache->size_on_disk >> 10);
 F(Obj->tfDiskUsed).set(Obj->tfDiskUsed, str);      
 sprintf(str, "%ld", (GlobImgCache->max_size_on_disk >> 10) - 
	 (GlobImgCache->size_on_disk >> 10));
 F(Obj->tfDiskFree).set(Obj->tfDiskFree, str);
}









