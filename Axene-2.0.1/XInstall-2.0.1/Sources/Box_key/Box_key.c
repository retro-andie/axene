/*
** Box_key.c for XInstall in Box_key/
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Thu Jan 16 00:49:53 1997 Antoine Buat
** Last update Sun Jan 16 17:38:58 2000 Emmanuel Paris
*/

/*--- includes --- */
#include "Box_key.h"
#include "TestCDKey.h"
#include "TestIDFile.h"
#include "GenEndKey.h"
#include "AlertBox.h"
#include "../Pixmaps/axene1.h"
#include "Resource.h"
#include "Cursor.h"
#include "DocIcon.h"
#include "../Pixmaps/logo_flip1.zpm"
#include "../Pixmaps/logo_flip2.zpm"
#include "../Pixmaps/logo_flip3.zpm"
#include "../Pixmaps/logo_flip4.zpm"
#include "../Pixmaps/logo_editor.zpm"

/*--- functions declaration --- */
static void *cons_Box_key();
static int BKwaitForUnMap();
static void dest_Box_key();
static void *copy_Box_key();

static void redraw_Image();
static void resize_Image();

static void certify_key1();
static void certify_key2();
static void certify_fname();
static void certify_company();
static void blink_icon();

/*--- externals --- */
extern c_Resource *GlobResources;

extern image_load_t	Load_Gif;
extern char		*GlobPicOpt;
extern c_DocIcon	*GlobDocIcon;
extern c_GCursor	*GlobCursor;

extern char	*key1;
extern char	*key2;
extern char	*company;
extern char	*fname;
extern char	cdkey[];
extern int	soft;
extern boolean  GlobLiveOpt;

#define SENSITIVE_BUTTON (GlobLiveOpt?XcCD_INSTALL:XcCD_NEXT)
#define SENSITIVEn_BUTTON (GlobLiveOpt?XcCDn_INSTALL:XcCDn_NEXT)

/*--- methods --- */
sf_Box_key fc_Box_key =
{
 cons_Box_key,
 dest_Box_key,
 copy_Box_key,
 BKwaitForUnMap
};

/* ----------------------------------------------------------------- ** 
** constructeur                                                      ** 
** ----------------------------------------------------------------- */
static void *cons_Box_key(w_Parent, pszTitle)
Widget		w_Parent;
char		*pszTitle;
{
 c_Box_key	*This;
 Display	*display;
 Dimension	h1, h2;
 Arg		argsw[1];
 Pixmap		pixmap;
 int		i;
 
 if((This = (c_Box_key *)Xc_malloc("Box_key",
				   sizeof(c_Box_key))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 memset(This, 0, sizeof(c_Box_key));

 This->f = &fc_Box_key;
 This->app = XtWidgetToApplicationContext(w_Parent);
  
 XtVaGetValues(w_Parent, XmNwidth, &h1, XmNheight, &h2, NULL); 
 
 SET_WAIT_MODE_STATIC;

 This->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
						      pszTitle, h1, h2,
						      XcCD_HORIZONTAL);
 This->w_This = WIDGET(This->Dialog);
 display = XtDisplay(WIDGET(This->Dialog));

 This->Tf_field = 0;

 This->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(This),
				    MANAGER(This->Dialog),
				    "gBoxKey", FALSE);

 This->FrameIcon = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					   MANAGER(This->Dialog), 
					   NULL, 0);

 if (GlobPicOpt)
  This->Image = (c_Image *)NEW(c_Image)(GlobPicOpt, &Load_Gif);
 else
 {
  This->ImageStream = Xc_NewStream(axene1_gif, axene1_gif_size);
  This->Image = (c_Image *)NEW(c_Image)
   ((char *)This->ImageStream, &Load_Gif);
 }

 if (This->Image)
 {
  /*F(This->Image).CalculImage(This->Image, 589, 194);*/
  F(This->Image).CalculImage(This->Image, 0, 0);
  This->Drawing = XmCreateDrawingArea(WIDGET(This->FrameIcon),
				      "BKDrawingGIF", NULL, 0);
  F(MANAGER(This->Dialog)).Add_child(MANAGER(This->Dialog),
				     This->Drawing,
				     WIDGET(This->FrameIcon));
    
  XtAddCallback(This->Drawing, XmNexposeCallback, redraw_Image, 
		This);
  XtAddCallback(This->Drawing, XmNresizeCallback, resize_Image, 
		This);
 }

 This->FrameInfo = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					   MANAGER(This->Dialog), 
					   NULL, 0);
 This->LInfo = 
  (c_Label *)NEW(c_Label)(WIDGET(This->FrameInfo),
			  MANAGER(This->Dialog),
			  "lBKInfo");

 if (GlobLiveOpt)
 {
  XmString	xms;
   
  xms = XmStringCreateLtoR(F(GlobResources).getString(GlobResources, 
						      XcR_lBKInfo),
			   XmSTRING_DEFAULT_CHARSET);
  XtSetArg(argsw[0], XmNlabelString, xms);
  XtSetValues(WIDGET(This->LInfo), argsw, 1);
  XmStringFree(xms);
 }

 /*--- full name ---*/
 This->FrameAsk = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					  MANAGER(This->Dialog), 
					  NULL, 0);
 This->GridAskP = (c_Grid *)NEW(c_Grid)(WIDGET(This->FrameAsk),
					MANAGER(This->Dialog),
					"gBoxKeyAskP", FALSE);

 /*--- Flipping icons ---*/

 This->FormIcon = (c_Form *)NEW(c_Form)(WIDGET(This->GridAskP),
					MANAGER(This->Dialog),
					"foflpIcon");
 XtVaSetValues(WIDGET(This->FormIcon), 
	       XmNnoResize, True, XmNresizePolicy, XmRESIZE_NONE, 
	       XmNautoUnmanage, False, NULL);
 
 This->IconFlip[0] = (c_Icon *)NEW(c_Icon)(WIDGET(This->FormIcon), 
					   MANAGER(This->Dialog),
					   "iBoxKey0", display, 
					   logo_editor_xpm, NULL, XcLabel);
 This->IconFlip[1] = (c_Icon *)NEW(c_Icon)(WIDGET(This->FormIcon), 
					   NULL,
					   "iBoxKey1", display,
					   logo_flip1_xpm, NULL, XcLabel);
 This->IconFlip[2] = (c_Icon *)NEW(c_Icon)(WIDGET(This->FormIcon), 
					   NULL,
					   "iBoxKey2", display,
					   logo_flip2_xpm, NULL, XcLabel);
 This->IconFlip[3] = (c_Icon *)NEW(c_Icon)(WIDGET(This->FormIcon), 
					   NULL,
					   "iBoxKey3", display,
					   logo_flip3_xpm, NULL, XcLabel);
 This->IconFlip[4] = (c_Icon *)NEW(c_Icon)(WIDGET(This->FormIcon), 
					   NULL,
					   "iBoxKey4", display,
					   logo_flip4_xpm, NULL, XcLabel);

 This->lFlipTime = 2000L;
 This->id = XtAppAddTimeOut(This->app, This->lFlipTime, blink_icon, This);

 XtSetArg(argsw[0], XmNrecomputeSize, False);
 for(i = 0; i < NB_ICON; i++)
 {
  XtSetValues(WIDGET(This->IconFlip[i]), argsw, 1);
  F(This->FormIcon).attach_top(This->FormIcon, 
			       WIDGET(This->IconFlip[i]), NULL);
  F(This->FormIcon).attach_left(This->FormIcon, 
				WIDGET(This->IconFlip[i]), NULL);
  F(This->FormIcon).attach_right(This->FormIcon, 
				 WIDGET(This->IconFlip[i]), NULL);
  F(This->FormIcon).attach_bottom(This->FormIcon, 
				  WIDGET(This->IconFlip[i]), NULL);
 }
 /*--- Grid Ask content ---*/
 This->GridAsk = (c_Grid *)NEW(c_Grid)(WIDGET(This->GridAskP),
				       MANAGER(This->Dialog),
				       "gBoxKeyAsk", TRUE);

 This->LFName = 
  (c_Label *)NEW(c_Label)(WIDGET(This->GridAsk),
			  MANAGER(This->Dialog),
			  "lBKFName");
 This->TFFName = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->GridAsk),
				    MANAGER(This->Dialog),
				    "tfBKFName",
				    TF_STRING,
				    1.0, (float)(48),
				    fname, NULL, 48);
 
 if (fname && strlen(fname))
 {
  int i, count;
  
  for(count = i = 0; i < (int)strlen(fname); i++)
  {
   if ((fname[i] >= 'A' && fname[i] <= 'Z') || 
       (fname[i] >= 'a' && fname[i] <= 'z') || 
       ((unsigned char)fname[i] > 127))
   {
    if (++count == 2) break;
   }
  }
  if (count == 2)
   This->Tf_field |= TFFN;
 }
 
 /*--- company ---*/
 This->LCompany = 
  (c_Label *)NEW(c_Label)(WIDGET(This->GridAsk),
			  MANAGER(This->Dialog),
			  "lBKCompany");
 This->TFCompany = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->GridAsk),
				    MANAGER(This->Dialog),
				    "tfBKCompany",
				    TF_STRING,
				    1.0, (float)(48),
				    company, NULL, 48);

 /*---  key ---*/
 This->LKey = 
  (c_Label *)NEW(c_Label)(WIDGET(This->GridAsk),
			  MANAGER(This->Dialog),
			  "lBKKey");
 This->TFkey1 = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->GridAsk),
				    MANAGER(This->Dialog),
				    "tfKeyPrefix",
				    TF_NUMSTRING,
				    1.0, (float)(3),
				    key1, NULL, 
#ifdef LESSTIF_VERSION
				    5
#else
				    3
#endif
				    );
 if (key1 && strlen(key1))
  This->Tf_field |= TFK1;
 
 This->LKminus = 
  (c_Label *)NEW(c_Label)(WIDGET(This->GridAsk),
			  MANAGER(This->Dialog),
			  "-");
 This->TFkey2 = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->GridAsk),
				    MANAGER(This->Dialog),
				    "tfKey",
				    TF_NUMSTRING,
				    1.0, (float)(10),
				    key2, NULL, 
#ifdef LESSTIF_VERSION
				    15
#else
				    10
#endif
				    );
 if (key1 && strlen(key1))
  This->Tf_field |= TFK2;

 F(This->GridAsk).addColumn(This->GridAsk, XcG_MAX_SIZE, NULL);
 F(This->GridAsk).addColumn(This->GridAsk, XcG_BEST_SIZE,
			    WIDGET(This->LFName), 150,
			    XcG_WIDGET_NULL, 10,
			    WIDGET(This->TFFName), XcG_MAX_SIZE,
			    XcG_WIDGET_NULL, 20,
			    NULL);
 F(This->GridAsk).addColumn(This->GridAsk, 5, NULL);
 F(This->GridAsk).addColumn(This->GridAsk, XcG_BEST_SIZE,
			    WIDGET(This->LCompany), 150,
			    XcG_WIDGET_NULL, 10,
			    WIDGET(This->TFCompany), XcG_MAX_SIZE,
			    XcG_WIDGET_NULL, 20,
			    NULL);
 F(This->GridAsk).addColumn(This->GridAsk, 5, NULL);
 F(This->GridAsk).addColumn(This->GridAsk, XcG_BEST_SIZE,
			    WIDGET(This->LKey), 200, 
			    XcG_WIDGET_NULL, 10,
			    WIDGET(This->TFkey1), XcG_BEST_SIZE,
			    WIDGET(This->LKminus), XcG_BEST_SIZE,
			    WIDGET(This->TFkey2), XcG_BEST_SIZE,
			    XcG_WIDGET_NULL, 20,
			    NULL);
 F(This->GridAsk).addColumn(This->GridAsk, XcG_MAX_SIZE, NULL);

 
 F(This->GridAskP).addColumn(This->GridAskP, 150,
			     WIDGET(This->FormIcon),  XcG_MAX_SIZE,
			     NULL);

 F(This->GridAskP).addColumn(This->GridAskP, XcG_MAX_SIZE,
			     WIDGET(This->GridAsk), XcG_MAX_SIZE,
			     NULL);


 F(This->TFkey1).add_callback(This->TFkey1,
			      (WTextFieldProc)certify_key1, This);
 F(This->TFkey1).add_control_callback(This->TFkey1,
				      (WTextFieldProc)certify_key1, This);
 F(This->TFkey2).add_callback(This->TFkey2,
			      (WTextFieldProc)certify_key2, This);
 F(This->TFkey2).add_control_callback(This->TFkey2,
				      (WTextFieldProc)certify_key2, This);
 F(This->TFFName).add_callback(This->TFFName,
			       (WTextFieldProc)certify_fname, This);
 F(This->TFFName).add_control_callback(This->TFFName,
			       (WTextFieldProc)certify_fname, This);
 F(This->TFCompany).add_callback(This->TFCompany,
				 (WTextFieldProc)certify_company, This);

 F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE,
			 WIDGET(This->FrameIcon), XcG_MAX_SIZE,
			 XcG_WIDGET_NULL, 7,
			 WIDGET(This->FrameInfo), 50,
			 XcG_WIDGET_NULL, 7,
			 WIDGET(This->FrameAsk), 150,
			 NULL);

 F(This->Dialog).createActionArea(This->Dialog, 
				  XcCD_CANCEL | XcCD_BACK | SENSITIVE_BUTTON,
				  SENSITIVE_BUTTON,
				  NULL, NULL);

 This->isSensitive = TRUE;

 pixmap = F(GlobDocIcon).get_pixmap(GlobDocIcon, OrchidPattern);
 if (pixmap != (Pixmap)0)
 {
  XtSetArg(argsw[0], XmNbackgroundPixmap, pixmap);
  XtSetValues(WIDGET(This->LInfo), argsw, 1);
 }

 UNSET_WAIT_MODE;
 
 Xc_TRACE(("Object Box_key build"));
 return This;
}

/* ----------------------------------------------------------------- ** 
** specific wait fo unmap                                            ** 
** ----------------------------------------------------------------- */
static int BKwaitForUnMap(This)
c_Box_key *This;
{
 int	rc;

 rc = 0;
 while (!rc)
 {
  int		alert_rc;

  alert_rc = 0;
  if ((This->Tf_field & TFALL) != TFALL && This->isSensitive)
  {
   if (!key1 || !key2 || !Xstrcmp(key1, "000") || 
       !Xstrcmp(key2, "0000000000") || GlobLiveOpt)
   {
    XtSetSensitive(This->Dialog->w_button[SENSITIVEn_BUTTON], False);
    This->isSensitive = FALSE;
   }
  }

  F(This->Dialog).map(This->Dialog, TRUE);
  XmProcessTraversal(WIDGET(This->TFFName), XmTRAVERSE_RIGHT);

  rc = F(This->Dialog).waitForUnmap(This->Dialog);
  if (rc == SENSITIVE_BUTTON)
  {
   boolean	key_validity;
   char		*str_msg;
   int		(*alert_box)();

   sprintf(cdkey, "%s-%s", key1, key2);
   
   if (!GlobLiveOpt)
    if (Xstrcmp(cdkey, "000-0000000000"))
    {
     soft = -1;
     return rc;
    }
   
   str_msg = F(GlobResources).getString(GlobResources, XcR_keynotvalid);

   if (GlobLiveOpt)
    alert_box = Xc_CancelRetryAlert;
   else
    alert_box = Xc_CancelRetryContinueAlert;
    
   key_validity = start_cdkey_check(cdkey);
   if (!key_validity)
    alert_rc = alert_box(str_msg);
   else
   {
    key_validity = check_cdkey_stage1();
    if (!key_validity)
     alert_rc = alert_box(str_msg);
    else
    {
     key_validity = check_cdkey_stage2();
     if (!key_validity)
      alert_rc = alert_box(str_msg);
     else
     {
      key_validity = check_cdkey_stage3();
      if (!key_validity)
       alert_rc = alert_box(str_msg);
      else
      {
       /*--- key_valid !! ---*/
       Xc_TRACE(("Object Box_key build"));
       return rc;
      }
     }
    }
   }
   if (!key_validity) soft = -1;

   switch(alert_rc)
   {
   case XcCD_CANCEL:
    rc = XcCD_CANCEL;
    break;
   case XcCD_CONTINUE:
    rc = XcCD_NEXT;
    break;
   default:
    rc = 0;
   }
  }
 }
 return rc;
} 

/* ----------------------------------------------------------------- ** 
** destructeur                                                       ** 
** ----------------------------------------------------------------- */
static void dest_Box_key(This)
c_Box_key *This;
{
 int i;

 XtRemoveTimeOut(This->id);

 DELETE(c_WTextField)(This->TFkey2);
 DELETE(c_Label)(This->LKminus);
 DELETE(c_WTextField)(This->TFkey1);
 DELETE(c_Label)(This->LKey);
 DELETE(c_WTextField)(This->TFCompany);
 DELETE(c_Label)(This->LCompany);
 DELETE(c_WTextField)(This->TFFName);
 DELETE(c_Label)(This->LFName);
 DELETE(c_Grid)(This->GridAsk);

 XtUnmanageChild(WIDGET(This->IconFlip[This->iPixmap]));
 for (i = 0; i < NB_ICON; i++)
  DELETE(c_Icon)(This->IconFlip[i]);
 
 DELETE(c_Form)(This->FormIcon);
 DELETE(c_Grid)(This->GridAskP);
 DELETE(c_Frame)(This->FrameAsk);
 DELETE(c_Label)(This->LInfo);
 DELETE(c_Frame)(This->FrameInfo);
 if (This->Image)
 {
  XtDestroyWidget(This->Drawing);
  DELETE(c_Image)(This->Image);
  Xc_DeleteStream(This->ImageStream);
 }

 DELETE(c_Frame)(This->FrameIcon);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);
 
 Xc_free(This);
 Xc_TRACE(("Object Box_key destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copieur                                                           ** 
** ----------------------------------------------------------------- */
static void *copy_Box_key(This)
c_Box_key *This;
{
 Xc_TRACE(("Copy not impemented"));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** redraw image                                                      ** 
** ----------------------------------------------------------------- */
static void redraw_Image(wid, This)
Widget		wid;
c_Box_key	*This;
{
 Display	*display;
 Window		window;
 GC		gc;
 Dimension	w, h;

 XtVaGetValues(This->Drawing, XmNwidth, &w, XmNheight, &h, NULL); 
 display = XtDisplay(WIDGET(This->Dialog));
 window = XtWindow(This->Drawing);
 gc = XCreateGC(display, window, 0, 0);
 XSetFunction(display, gc, GXcopy);
 F(This->Image).draw(This->Image, window, gc, w, h);
}

/* ----------------------------------------------------------------- ** 
** resize image                                                      ** 
** ----------------------------------------------------------------- */
static void resize_Image(wid, This)
Widget		wid;
c_Box_key	*This;
{
 Display	*display;
 Window		window;
 GC		gc;
 Dimension	w, h;

 display = XtDisplay(WIDGET(This->Dialog));
 window = XtWindow(This->Drawing);

 XClearWindow(display, window);
 XtVaGetValues(This->Drawing, XmNwidth, &w, XmNheight, &h, NULL); 
 gc = XCreateGC(display, window, 0, 0);
 XSetFunction(display, gc, GXcopy);
 F(This->Image).draw(This->Image, window, gc, w, h);
}

/*--- callbacks ---*/

/* ----------------------------------------------------------------- ** 
** certify key1                                                      ** 
** ----------------------------------------------------------------- */
static void certify_key1(this, Obj, cbs)
c_WTextField	*this;
c_Box_key	*Obj;
XmAnyCallbackStruct *cbs;
{
 char		*vstr;
 boolean	something;
 
 Xc_TRACE(("key1"));

 if (key1)
 {
  Xc_free(key1);
  key1 = NULL;
 }

 something = FALSE;
 if((vstr = F(this).get(this)) != NULL)
 {
  if((int)strlen(vstr) == 3)
   something = TRUE;
 }
 key1 = vstr;
 
 if(something)
 {
  Xc_TRACE(("%s", vstr));
  if ((Obj->Tf_field & TFALL) != TFALL)
  {
   Obj->Tf_field |= TFK1;
   
   if ((Obj->Tf_field & TFALL) == TFALL && !Obj->isSensitive)
   {
    XtSetSensitive(Obj->Dialog->w_button[SENSITIVEn_BUTTON], True);
    Obj->isSensitive = TRUE;
   }
  }
  
  if (cbs->reason != XmCR_MODIFYING_TEXT_VALUE)
   XmProcessTraversal(WIDGET(this), XmTRAVERSE_NEXT_TAB_GROUP);
  else if (((XmTextVerifyCallbackStruct *)cbs)->text->length &&
	   ((XmTextVerifyCallbackStruct *)cbs)->text->ptr != NULL)
   XmProcessTraversal(WIDGET(this), XmTRAVERSE_NEXT_TAB_GROUP);
 }
 else
 {
  if ((Obj->Tf_field & TFALL) == TFALL || Obj->isSensitive)
  {
   XtSetSensitive(Obj->Dialog->w_button[SENSITIVEn_BUTTON], False);
   Obj->isSensitive = FALSE;
  }
  Obj->Tf_field &= ~TFK1;
 }
 if (key1 && key2 && Xstrcmp(key1, "000") && Xstrcmp(key2, "0000000000") &&
     !Obj->isSensitive && !GlobLiveOpt)
 {
  XtSetSensitive(Obj->Dialog->w_button[SENSITIVEn_BUTTON], True);
  Obj->isSensitive = TRUE;
 }
}

/* ----------------------------------------------------------------- ** 
** certify key2                                                      ** 
** ----------------------------------------------------------------- */
static void certify_key2(this, Obj, cbs)
c_WTextField			*this;
c_Box_key			*Obj;
XmTextVerifyCallbackStruct	*cbs;
{
 char	*vstr;
 boolean	something;
 
 Xc_TRACE(("key2"));

 if (key2)
 {
  Xc_free(key2);
  key2 = NULL;
 }

 something = FALSE;
 if((vstr = F(Obj->TFkey2).get(Obj->TFkey2)) != NULL)
 {
  if ((int)strlen(vstr) == 10)
   something = TRUE;
 }
 key2 = vstr;
 
 if(something)
 {
  Xc_TRACE(("%s", vstr));
  Obj->Tf_field |= TFK2;
  
  if ((Obj->Tf_field & TFALL) == TFALL && !Obj->isSensitive)
  {
   XtSetSensitive(Obj->Dialog->w_button[SENSITIVEn_BUTTON], True);
   Obj->isSensitive = TRUE;
  }

  if (cbs->reason != XmCR_MODIFYING_TEXT_VALUE)
  {
   XmProcessTraversal(WIDGET(this), XmTRAVERSE_NEXT_TAB_GROUP);
   XmProcessTraversal(WIDGET(this), XmTRAVERSE_NEXT_TAB_GROUP);
   XmProcessTraversal(WIDGET(this), XmTRAVERSE_NEXT_TAB_GROUP);
  }
 }
 else
 {
  if ((Obj->Tf_field & TFALL) == TFALL || Obj->isSensitive)
  {
   XtSetSensitive(Obj->Dialog->w_button[SENSITIVEn_BUTTON], False);
   Obj->isSensitive = FALSE;
  }
  Obj->Tf_field &= ~TFK2;
 }
 
 if (key1 && key2 && Xstrcmp(key1, "000") && Xstrcmp(key2, "0000000000") &&
     !Obj->isSensitive && !GlobLiveOpt)
 {
  XtSetSensitive(Obj->Dialog->w_button[SENSITIVEn_BUTTON], True);
  Obj->isSensitive = TRUE;
 }
}


/* ----------------------------------------------------------------- ** 
** certify full name                                                 ** 
** ----------------------------------------------------------------- */
static void certify_fname(this, Obj, cbs)
c_WTextField			*this;
c_Box_key			*Obj;
XmTextVerifyCallbackStruct	*cbs;
{
 char	*vstr;
 boolean something;

 Xc_TRACE(("fname"));
 
 if (fname)
 {
  Xc_free(fname);
  fname = NULL;
 }

 something = FALSE;
 if((vstr = F(Obj->TFFName).get(Obj->TFFName)) != NULL)
 {
  if (strlen(vstr))
  {		/* the name must have at least 2 letters */
   int i, count;
   
   count = 0;
   for(i = 0; i < (int)strlen(vstr); i++)
   {
    if ((vstr[i] >= 'A' && vstr[i] <= 'Z') || 
	(vstr[i] >= 'a' && vstr[i] <= 'z') || ((unsigned char)vstr[i] > 127))
    {
     if (++count == 2) break;
    }
   }
   
   if (count == 2)
    something = TRUE;
  }
 }
 fname = vstr;

 if(something)
 {
  Xc_TRACE(("%s", vstr));
  if ((Obj->Tf_field & TFALL) != TFALL)
  {
   Obj->Tf_field |= TFFN;

   if ((Obj->Tf_field & TFALL) == TFALL && !Obj->isSensitive)
   {
    XtSetSensitive(Obj->Dialog->w_button[SENSITIVEn_BUTTON], True);
    Obj->isSensitive = TRUE;
   }
  }  
  if (cbs->reason != XmCR_MODIFYING_TEXT_VALUE)
   XmProcessTraversal(WIDGET(this), XmTRAVERSE_NEXT_TAB_GROUP);
 }
 else
 {
  if ((Obj->Tf_field & TFALL) == TFALL || Obj->isSensitive)
  {
   XtSetSensitive(Obj->Dialog->w_button[SENSITIVEn_BUTTON], False);
   Obj->isSensitive = FALSE;
  }
  Obj->Tf_field &= ~TFFN;
 }
 if (key1 && key2 && Xstrcmp(key1, "000") && Xstrcmp(key2, "0000000000") &&
     !Obj->isSensitive && !GlobLiveOpt)
 {
  XtSetSensitive(Obj->Dialog->w_button[SENSITIVEn_BUTTON], True);
  Obj->isSensitive = TRUE;
 }
}

/* ----------------------------------------------------------------- ** 
** certify company                                                   ** 
** ----------------------------------------------------------------- */
static void certify_company(this, Obj, cbs)
c_WTextField			*this;
c_Box_key			*Obj;
XmTextVerifyCallbackStruct	*cbs;
{
 char	*vstr;
 
 Xc_TRACE(("company"));
 if (company)
 {
  Xc_free(company);
  company = NULL;
 }

 if((vstr = F(Obj->TFCompany).get(Obj->TFCompany)) != NULL)
 {
  if (!strlen(vstr))
   Xc_free(vstr);
  else
   company = vstr;
 }
 
 if (cbs->reason != XmCR_MODIFYING_TEXT_VALUE)
  XmProcessTraversal(WIDGET(this), XmTRAVERSE_NEXT_TAB_GROUP);
}

static void blink_icon(This)
c_Box_key *This;
{
 if (This->lFlipTime > 100)
  This->lFlipTime -= 50L;
 
 This->id = XtAppAddTimeOut(This->app, This->lFlipTime, blink_icon, This);
 
 
 XtUnmanageChild(WIDGET(This->IconFlip[This->iPixmap]));
 
 This->iPixmap = (This->iPixmap + 1 ) % NB_ICON;
 XtManageChild(WIDGET(This->IconFlip[This->iPixmap]));
 
 /*F(This->GridAskP).callResize(This->GridAskP);*/
}
