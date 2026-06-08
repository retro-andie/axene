/*
** Box_author.c for Xclamation in Box_author/
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
** Started on  Tue Feb  1 23:34:28 2000 Antoine Buat
** Last update Mon May  1 17:40:05 2000 Antoine Buat
*/

#include "Box_author.h"
#include "Actions.h"
#include "builtdate.h"
#include "../Pixmaps/logo_soft_big.zpm"
#include "../Pixmaps/logo_editor.zpm"
#include "Cursor.h"
#include <stdlib.h>
#include "Colormap.h"
#include "ImageManager.h"
#include <X11/Xlib.h>

/*--- define ---*/
//#define BA_STAND_DELAY 4000L
#define BA_STAND_DELAY 1000L
#define BA_SELECT_DELAY 2L
#define BA_LINEAR_DELAY 2L
#define BA_RECTANGULAR_DELAY 4L

#ifndef Xc_XMAYDAY
#include "Readkey.h"
#endif

extern c_ImageManager	*GlobImgMng;
extern c_Colormap       *GlobColormap;
void XcHandleEvent();

static void *cons_Box_author();
static void dest_Box_author();
static void *copy_Box_author();
static int BAuthwaitForUnMap();
static void redraw_Image();

sf_Box_author fc_Box_author =
{
 cons_Box_author,
 dest_Box_author,
 copy_Box_author,
 BAuthwaitForUnMap
};

static void localLoad();
static void loadXpmGet();

static char *AB = "Antoine BUAT";
static char *EP = "Emmanuel Paris";
static char *SB = "Stéphane Boisson";
static char *RC = "Robin Castanier";

static void stand_still();
static void change_select();
static void std_copy();
static void left_copy();
static void right_copy();
static void top_copy();
static void bottom_copy();
static void boxin_copy();
static void boxout_copy();
static void genericfunc_coverl();
static void genericfunc_wavel();
static void random_copy();
static void gradient_copy();

// anim. functions
static void *funcTbl[] =
{
 gradient_copy
#if 0
 std_copy,
 left_copy,
 right_copy,
 top_copy,
 bottom_copy,
 boxin_copy,
 boxout_copy,
  genericfunc_coverl,
  genericfunc_wavel,
  random_copy
#endif
};

void *cons_Box_author(w_Parent, pszTitle)
Widget w_Parent;
char   *pszTitle;
{
 c_Box_author *	This;
 Arg		argsw[4];
 XmString       xmsBaratin;
 long		forec;

 if ((This = (c_Box_author *)Xc_malloc("Box_author",
				      sizeof(c_Box_author))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f = &fc_Box_author;
 This->app = XtWidgetToApplicationContext(w_Parent);
 
 This->currentAuthor = -1;
 This->lastAuthor = 0;
 This->paramSave = NULL;
 This->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
						      pszTitle,
						      520, 400,
						      XcCD_HORIZONTAL );
  
 This->w_This = WIDGET(This->Dialog);
 allow_ba_action(This->w_This);
 This->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(This),
				    MANAGER(This->Dialog),
				    "gBoxAuthor", FALSE);
  
 
 This->display = XtDisplay(w_Parent);


 This->MainTitleLabel = (c_Label *)NEW(c_Label)(WIDGET(This->Grid),
					      MANAGER(This->Dialog), 
					      "lMainTitle");
 xmsBaratin = F(GlobResources).getXmString(GlobResources,
					   XcR_authorTitle);
 XtSetArg(argsw[0], XmNlabelString, xmsBaratin);
 XtSetValues(WIDGET(This->MainTitleLabel), argsw, 1);
 XmStringFree(xmsBaratin);

 This->Frame2 = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					MANAGER(This->Dialog), 
					NULL, 0);
 This->Grid2 = (c_Grid *)NEW(c_Grid)(WIDGET(This->Frame2),
				     MANAGER(This->Dialog),
				     "gBAFrame2", FALSE);


 /*--- Flipping icons ---*/

 This->FormIcon = (c_Form *)NEW(c_Form)(WIDGET(This->Grid2),
					MANAGER(This->Dialog),
					"iBAflpIcon");
 XtVaSetValues(WIDGET(This->FormIcon), 
	       XmNnoResize, True, XmNresizePolicy, XmRESIZE_NONE, 
	       XmNautoUnmanage, False, NULL);

 This->Drawing = XmCreateDrawingArea(WIDGET(This->FormIcon),
				     "BGDrawing", NULL, 0);
 This->WDraw = 0; // set at first redraw
 XtAddCallback(This->Drawing, XmNexposeCallback, redraw_Image, 
	       This);

 XtSetArg(argsw[0], XmNrecomputeSize, False);
 XtSetValues(This->Drawing, argsw, 1);
 XtSetArg(argsw[0], XmNbackground, &forec);
 XtGetValues(This->Drawing, argsw, 1);

 F(This->FormIcon).attach_top(This->FormIcon, 
			      This->Drawing, NULL);
 F(This->FormIcon).attach_left(This->FormIcon, 
			       This->Drawing, NULL);
 F(This->FormIcon).attach_right(This->FormIcon, 
				This->Drawing, NULL);
 F(This->FormIcon).attach_bottom(This->FormIcon, 
				 This->Drawing, NULL);

 This->Label1 = (c_Label *)NEW(c_Label)(WIDGET(This->Grid2),
					MANAGER(This->Dialog),
					AB);
  
 This->Label2 = (c_Label *)NEW(c_Label)(WIDGET(This->Grid2),
					MANAGER(This->Dialog),
					EP);

 This->Label3 = (c_Label *)NEW(c_Label)(WIDGET(This->Grid2),
					MANAGER(This->Dialog),
					SB);
  
 This->Label4 = (c_Label *)NEW(c_Label)(WIDGET(This->Grid2),
					MANAGER(This->Dialog),
					RC);

 XtSetArg(argsw[0], XmNrecomputeSize, False);
 XtSetValues(WIDGET(This->Label1), argsw, 1);
 XtSetValues(WIDGET(This->Label2), argsw, 1);
 XtSetValues(WIDGET(This->Label3), argsw, 1);
 XtSetValues(WIDGET(This->Label4), argsw, 1);

 This->funcAuthor[0].wiID = WIDGET(This->Label1);
 This->funcAuthor[0].pszAuthor = AB;
 localLoad(This, &(This->funcAuthor[0].icAuthor), logo_soft_big_xpm, forec, 0);

 This->funcAuthor[1].wiID = WIDGET(This->Label2);
 This->funcAuthor[1].pszAuthor = EP;
 localLoad(This, &(This->funcAuthor[1].icAuthor), logo_editor_xpm, forec, 0);

 This->funcAuthor[2].wiID = WIDGET(This->Label3);
 This->funcAuthor[2].pszAuthor = SB; 
 localLoad(This, &(This->funcAuthor[2].icAuthor), logo_soft_big_xpm, forec, 0);

 This->funcAuthor[3].wiID = WIDGET(This->Label4);
 This->funcAuthor[3].pszAuthor = RC;
 localLoad(This, &(This->funcAuthor[3].icAuthor), logo_editor_xpm, forec, 0);

 XtSetArg(argsw[0], XmNforeground, &(This->lastColor));
 XtGetValues(This->funcAuthor[This->lastAuthor].wiID, argsw, 1);

 F(This->Grid2).addColumn(This->Grid2, 150,
			  WIDGET(This->FormIcon),  XcG_MAX_SIZE,
			  NULL);

 F(This->Grid2).addColumn(This->Grid2, XcG_MAX_SIZE,
			  WIDGET(This->Label1), XcG_MAX_SIZE,
			  WIDGET(This->Label2), XcG_MAX_SIZE,
			  WIDGET(This->Label3), XcG_MAX_SIZE,
			  WIDGET(This->Label4), XcG_MAX_SIZE,
			  NULL);
  
  
 F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE,
			 WIDGET(This->MainTitleLabel), 70,
			 WIDGET(This->Frame2), XcG_MAX_SIZE,
			 NULL);

 F(This->Dialog).createActionArea(This->Dialog, 
				  XcCD_OK,
				  XcCD_OK,
				  FALSE, NULL, NULL);
    
 F(This->Dialog).map(This->Dialog, TRUE);
  
 XtManageChild(This->Drawing);

 Xc_TRACE(("Object Box_author build"));
 return This;
}

void dest_Box_author(This)
c_Box_author *This;
{
 DELETE(c_Label)(This->MainTitleLabel);
 DELETE(c_Label)(This->Label1);
 DELETE(c_Label)(This->Label2);
 DELETE(c_Label)(This->Label3);
 DELETE(c_Label)(This->Label4);
 XtDestroyWidget(This->Drawing);
 DELETE(c_Form)(This->FormIcon);
 DELETE(c_Grid)(This->Grid2);
 DELETE(c_Frame)(This->Frame2);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);

 Xc_free(This);
 Xc_TRACE(("Object Box_author destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copy ctor                                                         ** 
** ----------------------------------------------------------------- */
static void *copy_Box_author(This)
c_Box_author *This;
{
 Xc_TRACE(("Copy not impemented"));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** specific wait fo unmap                                            ** 
** ----------------------------------------------------------------- */
static int BAuthwaitForUnMap(This)
c_Box_author	*This;
{
 int	rc;
 XEvent		event;
 XtAppContext	app;

 while(!This->WDraw)
 {
  app = XtWidgetToApplicationContext(WIDGET(This));
  XSync(This->display, 0);
  XtAppNextEvent(app, &event);
  if(event.type == Expose)
   XcHandleEvent(&event);
  else
   XtDispatchEvent(&event);
 }
 
 rc = F(This->Dialog).waitForUnmap(This->Dialog);
 XtRemoveTimeOut(This->id);

 if (TEST_WAIT_MODE) 
  UNSET_WAIT_MODE;
 return rc;
}

/* ----------------------------------------------------------------- ** 
** redraw image                                                      ** 
** ----------------------------------------------------------------- */
static void redraw_Image(wid, This)
Widget		wid;
c_Box_author	*This;
{
 XGCValues xgcv;
 if (!This->WDraw)
 {
  This->WDraw = XtWindow(This->Drawing);
  printf("first redraw: %p\n", This);
 
  xgcv.graphics_exposures = False;
  xgcv.function = GXcopy;
  This->gc_copy = XCreateGC(This->display, This->WDraw, 
			    GCFunction|GCGraphicsExposures, &xgcv);
  change_select(This);
 }
}

static void localLoad(This, tmpImg, pixmapStr, forec, backc)
c_Box_author *	This;
s_quickImg *	tmpImg;
char **		pixmapStr;
long		forec;
long		backc;
{
 XImage	*	xmask = NULL;
 Window		window;
 GC		gc;

 loadXpmGet(This->display, pixmapStr, forec, backc, &(tmpImg->infoImg), &xmask);
 Xc_free(xmask->data);
 XFree((char *)xmask); 
 
 window = DefaultRootWindow(This->display);
 tmpImg->quickImg = XCreatePixmap(This->display, window,
				  tmpImg->infoImg->width,
				  tmpImg->infoImg->height,
				  tmpImg->infoImg->depth);
 
 gc = XCreateGC(This->display, tmpImg->quickImg, 0, NULL);
 XPutImage(This->display, tmpImg->quickImg, gc, tmpImg->infoImg, 
	   0, 0, 0, 0, 
	   tmpImg->infoImg->width, tmpImg->infoImg->height);
}

static void loadXpmGet(display, pixmapStr, forec, backc, ret_ximage, ret_xmask)
Display *	display;
char **		pixmapStr;
long		forec;
long		backc;
XImage **	ret_xmask;
XImage **	ret_ximage;
{
 XImage *	xmask;
 XImage *	ximage;
 char		*datai;
 char		*datai2;
 char		*datam;
 int		depth;
 int		x,y,ki,km;
 int		ptri;
 char		*ptrm;
 boolean	bit_order;

 XPMload(display, pixmapStr, 
	 &ximage, &xmask, FALSE);
 if (xmask)
 {
  datai = ximage->data;
  datam = xmask->data;
  depth = GlobImgMng->real_depth;
  bit_order = (xmask->bitmap_bit_order==LSBFirst?TRUE:FALSE);
    
  datai2 = (char *)Xc_malloc("dup img2",
			     ximage->height * ximage->bytes_per_line);
  memcpy(datai2, datai, ximage->height*ximage->bytes_per_line);
    
  for(y = 0; y < ximage->height; y++)
  {
   ptrm = datam + y * xmask->bytes_per_line;
   ptri = y * ximage->bytes_per_line;
   ki = km = 0;
   for(x = 0; x<ximage->width; x++)
   {
    if (*ptrm & (1 << (bit_order ? km : 7-km)))
    {
     switch(depth)
     {
     case 1:
      if (forec)
       datai[ptri] &= ~(1 << (bit_order ? ki : 7-ki));
      else
       datai[ptri] |= (1 << (bit_order ? ki : 7-ki));
		  
      if (backc)
       datai2[ptri] &= ~(1 << (bit_order ? ki : 7-ki));
      else
       datai2[ptri] |= (1 << (bit_order ? ki : 7-ki));
		  
      if (++ki&8)
      {
       ki &= 7;
       ptri++;
      }
      break;
     case 32:
      if (GlobImgMng->byte_order_LSBFirst)
      {
       datai[ptri] = (char)(forec);
       datai2[ptri++] = (char)(backc);
       datai[ptri] = (char)(forec >> 8);
       datai2[ptri++] = (char)(backc >> 8);
       datai[ptri] = (char)(forec >> 16);
       datai2[ptri++] = (char)(backc >> 16);
       datai[ptri] = (char)(forec >> 24);
       datai2[ptri++] = (char)(backc >> 24);
      }
      else
      {
       datai[ptri] = (char)(forec >> 24);
       datai2[ptri++] = (char)(backc >> 24);
       datai[ptri] = (char)(forec >> 16);
       datai2[ptri++] = (char)(backc >> 16);
       datai[ptri] = (char)(forec >> 8);
       datai2[ptri++] = (char)(backc >> 8);
       datai[ptri] = (char)(forec);
       datai2[ptri++] = (char)(backc);
      }
      break;
     case 24:
      if (GlobImgMng->byte_order_LSBFirst)
      {
       datai[ptri] = (char)(forec);
       datai2[ptri++] = (char)(backc);
       datai[ptri] = (char)(forec >> 8);
       datai2[ptri++] = (char)(backc >> 8);
       datai[ptri] = (char)(forec >> 16);
       datai2[ptri++] = (char)(backc >> 16);
      }
      else
      {		      
       datai[ptri] = (char)(forec >> 16);
       datai2[ptri++] = (char)(backc >> 16);
       datai[ptri] = (char)(forec >> 8);
       datai2[ptri++] = (char)(backc >> 8);
       datai[ptri] = (char)(forec);
       datai2[ptri++] = (char)(backc);
      }
      break;
     case 16:
      if (GlobImgMng->byte_order_LSBFirst)
      {
       datai[ptri] = (char)(forec);
       datai2[ptri++] = (char)(backc);
       datai[ptri] = (char)(forec >> 8);
       datai2[ptri++] = (char)(backc >> 8);
      }
      else
      {		      
       datai[ptri] = (char)(forec >> 8);
       datai2[ptri++] = (char)(backc >> 8);
       datai[ptri] = (char)(forec);
       datai2[ptri++] = (char)(backc);
      }		  
      break;
     case 8:
      datai[ptri] = (char)(forec);
      datai2[ptri++] = (char)(backc);
      break;
     }
    }
    else
    {
     switch(depth)
     {
     case 1:
      if (++ki&8)
      {
       ki &= 7;
       ptri ++;
      }
      break;
     case 8:
      ptri++;
      break;
     case 15:
     case 16:
      ptri += 2;
      break;
     case 24:
      ptri += 3;
      break;
     case 32:
      ptri += 4;
      break;
     }
    }
    if (++km&8)
    {
     km &= 7;
     ptrm ++;
    }
   }
  }  
    
  switch(depth)
  {
  case 1:
   xmask = XCreateImage(display, 
			DefaultVisual(display, DefaultScreen(display)),
			1, XYBitmap, 0, datai2, ximage->width,
			ximage->height, 8, 0);
   break;
  case 8:
  case 16:
  case 24:
  case 32:
   xmask = XCreateImage(display, 
			DefaultVisual(display, DefaultScreen(display)),
			ximage->depth, ZPixmap, 0, datai2, ximage->width,
			ximage->height, 8, 0);
   break;
  default:
   Xc_WARNING(("Icon image unknow depth: %d",depth));
   break;
  }
 }
 *ret_ximage = ximage;
 *ret_xmask = xmask;
}

/* ----------------------------------------------------------------- ** 
** animation functions                                               ** 
** ----------------------------------------------------------------- */
static void stand_still(This)
c_Box_author	*This;
{
 This->id = XtAppAddTimeOut(This->app, BA_STAND_DELAY, change_select, This);
}

static void change_select(This)
c_Box_author	*This;
{
 char		tmpBuf[100];
 XmString	xmsName;
 Arg		argsw[2];

 if (This->currentAuthor >= 0)
 {
  /*-- select new anim functions --*/
  This->currentAnim = rand() % ( sizeof(funcTbl) / sizeof(void *) ) ;

  /*-- select author label name --*/
  xmsName = XmStringCreateLtoR(This->funcAuthor[This->currentAuthor].pszAuthor,
			       XmSTRING_DEFAULT_CHARSET);
  XtSetArg(argsw[0], XmNlabelString, xmsName);
  XtSetArg(argsw[1], XmNforeground, This->lastColor);
  XtSetValues(This->funcAuthor[This->currentAuthor].wiID, argsw, 2);
 
  This->lastAuthor = This->currentAuthor;
 }
 else
  This->currentAnim = 0;

 if (This->currentAuthor > 2)
  This->currentAuthor=0;
 else
  This->currentAuthor++;

 XtSetArg(argsw[0], XmNforeground, &(This->lastColor));
 XtGetValues(This->funcAuthor[This->currentAuthor].wiID, argsw, 1);
 sprintf(tmpBuf, "-> %s <-", This->funcAuthor[This->currentAuthor].pszAuthor);
 xmsName = XmStringCreateLtoR(tmpBuf, XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[0], XmNlabelString, xmsName);
 XtSetArg(argsw[1], XmNforeground, GlobColormap->Cred);
 XtSetValues(This->funcAuthor[This->currentAuthor].wiID, argsw, 2);

 XmStringFree(xmsName);

 sprintf(tmpBuf, "Random Num = %d - %d", This->currentAnim, sizeof(funcTbl));
 Xc_TRACE((tmpBuf));

 This->id = XtAppAddTimeOut(This->app, BA_SELECT_DELAY, funcTbl[This->currentAnim], This);
}

static void std_copy(This)
c_Box_author	*This;
{
 s_quickImg *curImage = &This->funcAuthor[This->currentAuthor].icAuthor;
 XClearWindow(This->display, This->WDraw);
 XCopyArea(This->display, 
	   curImage->quickImg, This->WDraw,
	   This->gc_copy,
	   0, 0,
	   curImage->infoImg->width, curImage->infoImg->height,
	   0, 0);
 stand_still(This);
}

typedef struct 
{
 int number;
} s_linear;

static void left_copy(This)
c_Box_author	*This;
{
 s_linear *curParm;
 s_quickImg *curImage;
 s_quickImg *oldImage;

 if (!This->paramSave)
 {
  This->paramSave = Xc_malloc("paramSave", sizeof(s_linear));
  curParm = (s_linear *)This->paramSave;
  curParm->number = 0;
 }
 else
  curParm = (s_linear *)This->paramSave;;

 curImage = &This->funcAuthor[This->currentAuthor].icAuthor;
 oldImage = &This->funcAuthor[This->lastAuthor].icAuthor;
 XClearArea(This->display, This->WDraw, curParm->number, 0, 
	    1, oldImage->infoImg->height, False);
 XCopyArea(This->display, 
	   curImage->quickImg, This->WDraw,
	   This->gc_copy,
	   curParm->number, 0,
	   1, curImage->infoImg->height,
	   curParm->number, 0);
 curParm->number++;
 if (curParm->number >= curImage->infoImg->width)
 {
  Xc_free(This->paramSave);
  This->paramSave = NULL;
  std_copy(This);
 }
 else
 {
  This->id = XtAppAddTimeOut(This->app, BA_LINEAR_DELAY, funcTbl[This->currentAnim], This);
 }
}

static void right_copy(This)
c_Box_author	*This;
{
 s_linear *curParm;
 s_quickImg *curImage;
 s_quickImg *oldImage;

 if (!This->paramSave)
 {
  This->paramSave = Xc_malloc("paramSave", sizeof(s_linear));
  curParm = (s_linear *)This->paramSave;
  curParm->number = 0;
 }
 else
  curParm = (s_linear *)This->paramSave;;

 curImage = &This->funcAuthor[This->currentAuthor].icAuthor;
 oldImage = &This->funcAuthor[This->lastAuthor].icAuthor;

 XClearArea(This->display, This->WDraw, 
	    curImage->infoImg->width - (curParm->number+1), 0, 
	    1, oldImage->infoImg->height, False);
 XCopyArea(This->display, 
	   curImage->quickImg, This->WDraw,
	   This->gc_copy,
	   curImage->infoImg->width - (curParm->number+1), 0,
	   1, curImage->infoImg->height,
	   curImage->infoImg->width - (curParm->number+1), 0);
 curParm->number++;
 if (curParm->number >= curImage->infoImg->width)
 {
  Xc_free(This->paramSave);
  This->paramSave = NULL;
  std_copy(This);
 }
 else
 {
  This->id = XtAppAddTimeOut(This->app, BA_LINEAR_DELAY, funcTbl[This->currentAnim], This);
 }
}

static void top_copy(This)
c_Box_author	*This;
{
 s_linear *curParm;
 s_quickImg *curImage;
 s_quickImg *oldImage;

 if (!This->paramSave)
 {
  This->paramSave = Xc_malloc("paramSave", sizeof(s_linear));
  curParm = (s_linear *)This->paramSave;
  curParm->number = 0;
 }
 else
  curParm = (s_linear *)This->paramSave;;

 curImage = &This->funcAuthor[This->currentAuthor].icAuthor;
 oldImage = &This->funcAuthor[This->lastAuthor].icAuthor;

 XClearArea(This->display, This->WDraw, 
	    curParm->number, 0, 
	    oldImage->infoImg->width, 1, False);
 XCopyArea(This->display, 
	   curImage->quickImg, This->WDraw,
	   This->gc_copy,
	   0, curParm->number,
	   curImage->infoImg->width, 1,
	   0, curParm->number);
 curParm->number++;
 if (curParm->number >= curImage->infoImg->height)
 {
  Xc_free(This->paramSave);
  This->paramSave = NULL;
  std_copy(This);
 }
 else
 {
  This->id = XtAppAddTimeOut(This->app, BA_LINEAR_DELAY, funcTbl[This->currentAnim], This);
 }
}

static void bottom_copy(This)
c_Box_author	*This;
{
 s_linear *curParm;
 s_quickImg *curImage;
 s_quickImg *oldImage;

 if (!This->paramSave)
 {
  This->paramSave = Xc_malloc("paramSave", sizeof(s_linear));
  curParm = (s_linear *)This->paramSave;
  curParm->number = 0;
 }
 else
  curParm = (s_linear *)This->paramSave;;

 curImage = &This->funcAuthor[This->currentAuthor].icAuthor;
 oldImage = &This->funcAuthor[This->lastAuthor].icAuthor;

 XClearArea(This->display, This->WDraw, 
	    0, curImage->infoImg->height - (curParm->number+1),
	    oldImage->infoImg->width, 1, False);
 XCopyArea(This->display, 
	   curImage->quickImg, This->WDraw,
	   This->gc_copy,
	   0, curImage->infoImg->height - (curParm->number+1),
	   curImage->infoImg->width, 1,
	   0, curImage->infoImg->height - (curParm->number+1));
 curParm->number++;
 if (curParm->number >= curImage->infoImg->height)
 {
  Xc_free(This->paramSave);
  This->paramSave = NULL;
  std_copy(This);
 }
 else
 {
  This->id = XtAppAddTimeOut(This->app, BA_LINEAR_DELAY, funcTbl[This->currentAnim], This);
 }
}

typedef struct 
{
 int currentY;
 int currentX;
 float stepX;
 float stepY;
 float stepErrorX;
 float stepErrorY;
} s_rectangular;

static void boxin_copy(This)
c_Box_author	*This;
{
 s_rectangular *curParm;
 s_quickImg *curImage;
 s_quickImg *oldImage;
 int nextX;
 int nextY;
 int centerX;
 int centerY;
 int lLeft;
 int lRight;
 int lTop;
 int lBottom;

 curImage = &This->funcAuthor[This->currentAuthor].icAuthor;
 oldImage = &This->funcAuthor[This->lastAuthor].icAuthor;

 if (!This->paramSave)
 {
  This->paramSave = Xc_malloc("paramSave", sizeof(s_rectangular));
  curParm = (s_rectangular *)This->paramSave;
  curParm->currentX = 0;
  curParm->currentY = 0;
  curParm->stepErrorX = 0.0;
  curParm->stepErrorY = 0.0;
  if (curImage->infoImg->width < curImage->infoImg->height)
  {
   curParm->stepX = 1;
   curParm->stepY = ((float)curImage->infoImg->height) / 
    (curImage->infoImg->width);
  }
  else
  {
   curParm->stepX = ((float)curImage->infoImg->width) / 
    (curImage->infoImg->height);
   curParm->stepY = 1;
  }
 }
 else
  curParm = (s_rectangular *)This->paramSave;

 nextX = curParm->currentX + (curParm->stepX + curParm->stepErrorX);
 curParm->stepErrorX = (float)curParm->currentX + (curParm->stepX + curParm->stepErrorX) - (float)nextX;
 nextY = curParm->currentY + (curParm->stepY + curParm->stepErrorY);
 curParm->stepErrorY = (float)curParm->currentY + (curParm->stepY + curParm->stepErrorY) - (float)nextY;

 centerX = curImage->infoImg->width / 2;
 centerY = curImage->infoImg->height / 2;

 lLeft = curParm->currentX;
 lRight = curImage->infoImg->width - curParm->currentX;
 lTop = curParm->currentY;
 lBottom = curImage->infoImg->height - curParm->currentY;

 /*--- left ---*/
 XCopyArea(This->display, 
	   curImage->quickImg, This->WDraw,
	   This->gc_copy,
	   lLeft, 0,
	   nextX - curParm->currentX, curImage->infoImg->height,
	   lLeft, 0);

 /*--- top ---*/
 XCopyArea(This->display, 
	   curImage->quickImg, This->WDraw,
	   This->gc_copy,
	   0, lTop,
	   curImage->infoImg->width, nextY - curParm->currentY,
	   0, lTop);

 /*--- right ---*/
 XCopyArea(This->display, 
	   curImage->quickImg, This->WDraw,
	   This->gc_copy,
	   lRight - (nextX - curParm->currentX), 0,
	   nextX - curParm->currentX, curImage->infoImg->height,
	   lRight - (nextX - curParm->currentX), 0);

 /*--- bottom ---*/
 XCopyArea(This->display, 
	   curImage->quickImg, This->WDraw,
	   This->gc_copy,
	   0, lBottom - (nextY - curParm->currentY),
	   curImage->infoImg->width, nextY - curParm->currentY,
	   0, lBottom - (nextY - curParm->currentY));

 curParm->currentX = nextX;
 curParm->currentY = nextY;

 if ((nextX*2 >= curImage->infoImg->width) ||
     (nextY*2 >= curImage->infoImg->height))
 {
  Xc_free(This->paramSave);
  This->paramSave = NULL;
  std_copy(This);
 }
 else
 {
  This->id = XtAppAddTimeOut(This->app, BA_RECTANGULAR_DELAY, funcTbl[This->currentAnim], This);
 }
}

static void boxout_copy(This)
c_Box_author	*This;
{
 s_rectangular *curParm;
 s_quickImg *curImage;
 s_quickImg *oldImage;
 int nextX;
 int nextY;
 int centerX;
 int centerY;
 int lLeft;
 int lRight;
 int lTop;
 int lBottom;

 curImage = &This->funcAuthor[This->currentAuthor].icAuthor;
 oldImage = &This->funcAuthor[This->lastAuthor].icAuthor;

 if (!This->paramSave)
 {
  This->paramSave = Xc_malloc("paramSave", sizeof(s_rectangular));
  curParm = (s_rectangular *)This->paramSave;
  curParm->currentX = 0;
  curParm->currentY = 0;
  curParm->stepErrorX = 0.0;
  curParm->stepErrorY = 0.0;
  if (curImage->infoImg->width < curImage->infoImg->height)
  {
   curParm->stepX = 1;
   curParm->stepY = ((float)curImage->infoImg->height) / 
    (curImage->infoImg->width);
  }
  else
  {
   curParm->stepX = ((float)curImage->infoImg->width) / 
    (curImage->infoImg->height);
   curParm->stepY = 1;
  }
 }
 else
  curParm = (s_rectangular *)This->paramSave;

 nextX = curParm->currentX + (curParm->stepX + curParm->stepErrorX);
 curParm->stepErrorX = (float)curParm->currentX + (curParm->stepX + curParm->stepErrorX) - (float)nextX;
 nextY = curParm->currentY + (curParm->stepY + curParm->stepErrorY);
 curParm->stepErrorY = (float)curParm->currentY + (curParm->stepY + curParm->stepErrorY) - (float)nextY;

 centerX = curImage->infoImg->width / 2;
 centerY = curImage->infoImg->height / 2;

 lLeft = centerX - nextX;
 if (lLeft < 0)
  lLeft = 0;
 lRight = centerX + nextX;
 if (lRight > curImage->infoImg->width)
  lRight = curImage->infoImg->width;
 lTop = centerY - nextY;
 if (lTop < 0)
  lTop = 0;
 lBottom = centerY + nextY;
 if (lBottom > curImage->infoImg->height)
  lBottom = curImage->infoImg->height;

 /*--- left ---*/
 if (nextX - curParm->currentX)
 {
  XCopyArea(This->display, 
	    curImage->quickImg, This->WDraw,
	    This->gc_copy,
	    lLeft, lTop,
	    nextX - curParm->currentX, 2*nextY,
	    lLeft, lTop);
 }
 /*--- top ---*/
 if (nextY - curParm->currentY)
 {
  XCopyArea(This->display, 
	    curImage->quickImg, This->WDraw,
	    This->gc_copy,
	    lLeft, lTop,
	    2*nextX, nextY - curParm->currentY,
	    lLeft, lTop);
 }
 /*--- right ---*/
 if (nextX - curParm->currentX)
 {
  XCopyArea(This->display, 
	    curImage->quickImg, This->WDraw,
	    This->gc_copy,
	    lRight - (nextX - curParm->currentX), lTop,
	    nextX - curParm->currentX, 2*nextY,
	    lRight - (nextX - curParm->currentX), lTop);
 }
 /*--- bottom ---*/
 if (nextY - curParm->currentY)
 {
  XCopyArea(This->display, 
	    curImage->quickImg, This->WDraw,
	    This->gc_copy,
	    lLeft, lBottom - (nextY - curParm->currentY),
	    2*nextX, nextY - curParm->currentY,
	    lLeft, lBottom - (nextY - curParm->currentY));
 }
 curParm->currentX = nextX;
 curParm->currentY = nextY;

 if ((nextX*2 >= curImage->infoImg->width) ||
     (nextY*2 >= curImage->infoImg->height))
 {
  Xc_free(This->paramSave);
  This->paramSave = NULL;
  std_copy(This);
 }
 else
 {
  This->id = XtAppAddTimeOut(This->app, BA_RECTANGULAR_DELAY, funcTbl[This->currentAnim], This);
 }
}

int func_per(x, y, max_x, max_y)
int x;
int y;
int max_x;
int max_y;
{
 double phaseY = (double)y * (2.0 * 3.14 / (double)max_y);
 double phaseX = (double)x * (2.0 * 3.14 / (double)max_x);
 return (sin( 3.0*phaseY  + phaseX )+1) * (max_y / 14);
// return (sin( 3.0*phaseY  + phaseX )+1) * 9 * sin(phaseX);
}

static void genericfunc_coverl(This)
c_Box_author	*This;
{
 s_linear *curParm;
 s_quickImg *curImage;
 s_quickImg *oldImage;
 int y;
 int delta;
 int width;

 if (!This->paramSave)
 {
  This->paramSave = Xc_malloc("paramSave", sizeof(s_linear));
  curParm = (s_linear *)This->paramSave;
  curParm->number = 0;
 }
 else
  curParm = (s_linear *)This->paramSave;;

 curImage = &This->funcAuthor[This->currentAuthor].icAuthor;
 oldImage = &This->funcAuthor[This->lastAuthor].icAuthor;

 for (y = 0; y < curImage->infoImg->height; ++y)
 {
  delta = func_per(curParm->number, y, 
		   curImage->infoImg->width, curImage->infoImg->height);
  width = delta + curParm->number;
  if ( width >= curImage->infoImg->width)
   width = curImage->infoImg->width;
  else
   XCopyArea(This->display, 
	     oldImage->quickImg, This->WDraw,
	     This->gc_copy,
	     width, y,
	     curImage->infoImg->width - width, 1,
	     width, y);

  XCopyArea(This->display, 
	    curImage->quickImg, This->WDraw,
	    This->gc_copy,
	    0, y,
	    width, 1,
	    0, y);
  
 }

 curParm->number++;
 if (curParm->number >= curImage->infoImg->width)
 {
  Xc_free(This->paramSave);
  This->paramSave = NULL;
  std_copy(This);
 }
 else
 {
  This->id = XtAppAddTimeOut(This->app, BA_LINEAR_DELAY, funcTbl[This->currentAnim], This);
 }
}

static void genericfunc_wavel(This)
c_Box_author	*This;
{
 s_linear *curParm;
 s_quickImg *curImage;
 s_quickImg *oldImage;
 int y;
 int delta;
 int width;

 if (!This->paramSave)
 {
  This->paramSave = Xc_malloc("paramSave", sizeof(s_linear));
  curParm = (s_linear *)This->paramSave;
  curParm->number = 0;
 }
 else
  curParm = (s_linear *)This->paramSave;;

 curImage = &This->funcAuthor[This->currentAuthor].icAuthor;
 oldImage = &This->funcAuthor[This->lastAuthor].icAuthor;

 for (y = 0; y < curImage->infoImg->height; ++y)
 {
  delta = func_per(curParm->number, y, 
		   curImage->infoImg->width, curImage->infoImg->height);
  width = delta + curParm->number;
  if ( width >= curImage->infoImg->width)
   width = curImage->infoImg->width;
  else
   XCopyArea(This->display, 
	     oldImage->quickImg, This->WDraw,
	     This->gc_copy,
	     0, y,
	     curImage->infoImg->width - width, 1,
	     width, y);

  XCopyArea(This->display, 
	    curImage->quickImg, This->WDraw,
	    This->gc_copy,
	    curImage->infoImg->width - width, y,
	    width, 1,
	    0, y);
  
 }

 curParm->number++;
 if (curParm->number >= curImage->infoImg->width)
 {
  Xc_free(This->paramSave);
  This->paramSave = NULL;
  std_copy(This);
 }
 else
 {
  This->id = XtAppAddTimeOut(This->app, BA_LINEAR_DELAY, funcTbl[This->currentAnim], This);
 }
}

#define GRIDW 30
#define GRIDH 30
typedef struct 
{
 char grid[GRIDW*GRIDH];
 int number;
} s_random;

static void random_copy(This)
c_Box_author	*This;
{
 s_random *curParm;
 s_quickImg *curImage;
 s_quickImg *oldImage;
 int i, x, y, w, h;
 int num_block = 10;

 if (!This->paramSave)
 {
  This->paramSave = Xc_malloc("paramSave", sizeof(s_random));
  curParm = (s_random *)This->paramSave;
  memset(curParm->grid, 0, GRIDW*GRIDH);
  curParm->number = 0;
 }
 else
  curParm = (s_random *)This->paramSave;;

 curImage = &This->funcAuthor[This->currentAuthor].icAuthor;
 oldImage = &This->funcAuthor[This->lastAuthor].icAuthor;

 while (num_block--)
 {
  i = random() % (GRIDW*GRIDH);
  while(curParm->grid[i])
  {
   i++;
   if (i == GRIDW*GRIDH) i = 0;
  }
  curParm->grid[i] = 1;
  w = (curImage->infoImg->width / GRIDW) + 1;
  h = (curImage->infoImg->height / GRIDH) + 1;
  x = curImage->infoImg->width * (i / GRIDH) / GRIDW;
  y = curImage->infoImg->height * (i % GRIDH) / GRIDH;

  XCopyArea(This->display, 
	    curImage->quickImg, This->WDraw,
	    This->gc_copy, x, y, w, h, x, y);

  curParm->number++;
 }
 if (curParm->number >= GRIDW*GRIDH)
 {
  Xc_free(This->paramSave);
  This->paramSave = NULL;
  std_copy(This);
 }
 else
 {
  This->id = XtAppAddTimeOut(This->app, BA_LINEAR_DELAY, funcTbl[This->currentAnim], This);
 }
}

typedef struct 
{
 int number;
 unsigned char *gradient;
} s_gradient;

static void round_gradient(This, width, height, buffer)
c_Box_author *	This;
unsigned int	width;
unsigned int	height;
unsigned char * buffer;
{
 Window		window;
 GC		gc;
 Pixmap		drawable;
 XImage	*	img;
 unsigned long	i;
 unsigned int	x;
 unsigned int	y;

 window = DefaultRootWindow(This->display);
 drawable = XCreatePixmap(This->display, window,
			  width, height, 16);
 gc = XCreateGC(This->display, drawable, 0, NULL);

 XSetLineAttributes(This->display, gc, (MAX(width, height) / 256) + 1, 
		    LineSolid, CapNotLast, JoinMiter);

 XSetForeground(This->display, gc, 0);
 XFillRectangle(This->display, drawable, gc, 0, 0, width, height);

 for (i=0; i<256; i++)
 {
  XSetForeground(This->display, gc, (i<<16) + (i<<8) + i);
  XDrawArc(This->display, drawable, gc, 
	   (width-(width*i)/256)/2, (height-(height*i)/256)/2, 
	   (width*i)/256, (height*i)/256, 0, 360*64);
 }
 img = XGetImage(This->display, drawable, 0, 0, width, height, 
		 (unsigned long)-1, ZPixmap);

 for(x=0; x < width; x++)
 {
  for(y=0; y < height; y++)
  {
    buffer[y*width + x] = XGetPixel(img, x, y);
    if (buffer[y*width + x] == 0)
     buffer[y*width + x] = 255;
  }
 }
 XDestroyImage(img);
 XFreePixmap(This->display, drawable);
}

static void chronos_gradient(This, width, height, buffer)
c_Box_author *	This;
unsigned int	width;
unsigned int	height;
unsigned char * buffer;
{
 Window		window;
 GC		gc;
 Pixmap		drawable;
 XImage	*	img;
 unsigned long	i;
 unsigned int	x;
 unsigned int	y;
 XPoint		points[3];
 double		pente;
 double		plimit = (double)height/width;
 int		isInX = 1;
 int		quarter = 0;
 int		dir = -1;

 window = DefaultRootWindow(This->display);
 drawable = XCreatePixmap(This->display, window,
			  width, height, 16);
 gc = XCreateGC(This->display, drawable, 0, NULL);

 XSetForeground(This->display, gc, 0);
 XFillRectangle(This->display, drawable, gc, 0, 0, width, height);

 points[0].x = width/2;
 points[0].y = height/2;
 points[2].x = width/2 + points[0].x;
 points[2].y = 0 + points[0].y;

#define LEVELNUM 256
 for (i=0; i<LEVELNUM; i++)
 {
  points[1].x = points[2].x;
  points[1].y = points[2].y;

  pente = tan( dir*i*2*M_PI/LEVELNUM );
  if (fabs(pente) <= plimit)
  {
   if (!isInX)
   {
    isInX = 1;
    quarter+=dir;
    if (quarter < 0)
     quarter = 3;
    if (quarter > 3)
     quarter = 0;
   }
   if (quarter == 2)
   {
    points[2].x = points[0].x - width/2;
    points[2].y = points[0].y + (double)(width/2)*pente;
   }
   else
   {
    points[2].x = points[0].x + width/2;
    points[2].y = points[0].y - (double)(width/2)*pente;
   }
  }
  else
  {
   if (isInX)
   {
    isInX = 0;
    quarter+=dir;
    if (quarter < 0)
     quarter = 3;
    if (quarter > 3)
     quarter = 0;
   }
   if (quarter == 1)
   {
    points[2].y =  points[0].y - height/2;
    points[2].x =  points[0].x + (double)(height/2)/pente;
   }
   else
   {
    points[2].y = points[0].y + height/2;
    points[2].x = points[0].x - (double)(height/2)/pente;
   }
  }

  if ( (abs(points[1].x - points[2].x) > 10) || 
      (abs(points[1].y - points[2].y) > 10)) 
  {
   printf("P1.x=%d, P1.y=%d, P2.x=%d, P2.y=%d\n", 
	  points[1].x, points[1].y,
	  points[2].x, points[2].y);
   printf("i=%ld, pente=%f, height/width=%f\n", 
	  i, pente, (float)height/width);
  }

  XSetForeground(This->display, gc, (i<<16) + (i<<8) + i);
  XFillPolygon(This->display, drawable, gc, 
	       points, 3, Convex, CoordModeOrigin);
 }
 img = XGetImage(This->display, drawable, 0, 0, width, height, 
		 (unsigned long)-1, ZPixmap);

 for(x=0; x < width; x++)
 {
  for(y=0; y < height; y++)
  {
    buffer[y*width + x] = XGetPixel(img, x, y);
    if (buffer[y*width + x] == 0)
     buffer[y*width + x] = 255;
  }
 }
 XDestroyImage(img);
 XFreePixmap(This->display, drawable);
}

static void gradient_copy(This)
c_Box_author	*This;
{
 s_gradient *curParm;
 s_quickImg *curImage;
 s_quickImg *oldImage;
 int x=0;
 int y=0;

 curImage = &This->funcAuthor[This->currentAuthor].icAuthor;
 oldImage = &This->funcAuthor[This->lastAuthor].icAuthor;

 if (!This->paramSave)
 {
  This->paramSave = Xc_malloc("paramSave", sizeof(s_random));
  curParm = (s_gradient *)This->paramSave;
  curParm->number = 0;
  /*-- alloc gradient --*/
  curParm->gradient = (char *)Xc_malloc("gradient", 
					curImage->infoImg->width*
					curImage->infoImg->height);
  memset(curParm->gradient, 0, curImage->infoImg->width*curImage->infoImg->height);
  
  /*-- fill gradient --*/
  chronos_gradient(This, curImage->infoImg->width,
		   curImage->infoImg->height, curParm->gradient);
 }
 else
  curParm = (s_gradient *)This->paramSave;;

 /*-- execute gradient --*/
 for(x=0; x < curImage->infoImg->width; x++)
 {
  for(y=0; y < curImage->infoImg->height; y++)
  {
   if (curParm->gradient[y*curImage->infoImg->width + x] == curParm->number)
   {
    /*-- set this point! --*/
    /*-- find a quicker way to set the point --*/
    XCopyArea(This->display, 
	      curImage->quickImg, This->WDraw,
	      This->gc_copy, x, y, 1, 1, x, y);
   }
  }
 }

 curParm->number++;
 if (curParm->number >= 256)
 {
  Xc_free(curParm->gradient);
  Xc_free(This->paramSave);
  This->paramSave = NULL;
  std_copy(This);
 }
 else
 {
  This->id = XtAppAddTimeOut(This->app, BA_LINEAR_DELAY, funcTbl[This->currentAnim], This);
 }
}
