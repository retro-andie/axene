/*
** CustomDialog.c for Xclamation, XQuad, XAllWrite, XMayday and AxeneOffice 
** 	in CustomDialog/
** Methods for the CustomDialog class
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
** Started on  Thu Oct  6 14:22:32 1994 Stéphane Boisson
** Last update Thu Dec 30 17:40:02 1999 Emmanuel Paris
*/

#ifndef NTRACE
#define NTRACE
#endif
#ifndef NHISTORY
#define NHISTORY
#endif

#include "RegisterHelp.h"
#ifndef NO_REGISTER_HELP
# include "../Pixmaps/help.zpm"
#endif
#include "CustomDialog.h"
#include <X11/IntrinsicP.h>
#include <Xm/DialogS.h>
#include <Xm/PanedW.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/SeparatoG.h>
#include <Xm/Protocols.h>
#include <Xm/AtomMgr.h>
#ifdef EDITRES_COMPATIBLE
#include <X11/Xmu/Editres.h>
#endif

static void *constructor();
static void destructor();
static void *copy();
static void create_action_area ___PROTO((c_CustomDialog *this,
					 unsigned int button_list,
					 unsigned int default_button,
					 boolean help_activ,
					 void (*cb)(),
					 void *cb_data));
static void map ___PROTO((c_CustomDialog *this, boolean modal_flag));
static void unmap ___PROTO((c_CustomDialog *this));

static void activate_callback ___PROTO((Widget w, c_CustomDialog *dialog,
					XmAnyCallbackStruct *cbs));
static void close_callback ___PROTO((Widget w, c_CustomDialog *dialog));
static void setMenuBar ___PROTO((c_CustomDialog *this, c_MenuBar *bar));
static void blockChild ___PROTO((c_CustomDialog *this,
				 Widget w,
				 int size));
static void cb_resize ___PROTO((Widget w, XConfigureEvent *event,
				char **argv, int *argc));
static int waitForUserAction ___PROTO((c_CustomDialog *dialog));
static void resetTraversal ___PROTO((c_CustomDialog *this));

extern void XcHandleEvent ___PROTO((XEvent *event));
extern void XcFlushEvents ___PROTO((Widget w,
				    void (*func) ___PROTO((XEvent *))));

static char *button_label[] =
{
 "Save",  "Save as", "Yes", "No", "Apply", "Ok", "Open", "Retry",
 "Stop", "Reset", "Cancel", "Close",
 "Continue", "Quit",  "Help", "Unknown"
};

#define XcCD_TIGHTNESS 20
#define BBOARD_RESIZE "<Configure>: XcCD_resize(%ld)"

extern void MenuHelpCallHelp();

sf_CustomDialog fc_CustomDialog =
{
 constructor, destructor, copy,
 map, unmap,
 create_action_area,
 setMenuBar, blockChild,
 waitForUserAction,
 resetTraversal
};


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(parent, name, width, height, flags)
Widget parent;
char *name;
int width;
int height;
int flags;
{
 Atom WM_DELETE_WINDOW;
 c_CustomDialog *p;
 c_ManageWidget *m;
 XtActionsRec rec;
 char trans[80];
  
 Xc_HISTORY(("constructor(`%s', %dx%d)", name, width, height));
 Xc_TIMING_START(("Timing start"));

 if((m = NEW(c_ManageWidget)()) == NULL) return NULL;
 p = (c_CustomDialog *)Xc_malloc("Custom Dialog", sizeof(c_CustomDialog));
 if(p == NULL)
 {
  DELETE(c_ManageWidget)(m);
  return NULL;
 }
 memset(p, 0, sizeof(c_CustomDialog));

 p->f = &fc_CustomDialog;
 MANAGER(p) = m;

 p->default_button = -1;
 p->width = width;
 p->height = height;
 p->child_count = 0;
 p->total_size = 0;
 p->vertical_flag = (flags & XcCD_HORIZONTAL)? FALSE : TRUE;
 p->w_MenuBar = p->w_ActionArea = p->w_ActionAreaTop = p->w_Separator = NULL;
 p->mapped_flag = FALSE;
 p->help_activ = FALSE;
  
 p->w_Main = XtVaCreatePopupShell(name, xmDialogShellWidgetClass, parent,
				  XmNdeleteResponse, XmDO_NOTHING,
				  XmNmappedWhenManaged, False,
				  XmNinitialResourcesPersistent, False,
				  /*XmNheight, width,*/
				  /*XmNwidth, height,*/
				  /*XmNmwmDecorations, 2+8+16,*/
				  /*XmNmwmFunctions, 4+32,*/
				  NULL);
  
#ifdef EDITRES_COMPATIBLE
 XtAddEventHandler(p->w_Main, (EventMask)0, True,
		   _XEditResCheckMessages, NULL);
#endif 
  
 if(!(flags & XcCD_HRESIZE) && width)
  XtVaSetValues(p->w_Main, XmNmaxWidth, width, NULL);
 if(!(flags & XcCD_VRESIZE) && height)
  XtVaSetValues(p->w_Main, XmNmaxHeight, height, NULL);

 if (width && height)
  XtVaSetValues(p->w_Main, XmNminWidth, width, XmNminHeight, height, NULL);
  
 if(!height)
  WIDGET(p) = XtVaCreateWidget("bboard", xmBulletinBoardWidgetClass,
			       p->w_Main,
			       XmNmarginWidth, 10,
			       XmNmarginHeight, 10,
			       XmNallowShellResize, True,
			       XmNautoUnmanage, False,
			       XmNdefaultPosition, True,
			       XmNresizePolicy, XmRESIZE_NONE,
			       XmNallowOverlap, False,
			       XmNnoResize,
			       (flags & (XcCD_HRESIZE|XcCD_VRESIZE))?
			       False : True,
			       width? XmNwidth : NULL, width,
			       NULL);
 else
  WIDGET(p) = XtVaCreateWidget("bboard", xmBulletinBoardWidgetClass,
			       p->w_Main,
			       XmNmarginWidth, 10,
			       XmNmarginHeight, 10,
			       XmNallowShellResize, True,
			       XmNautoUnmanage, False,
			       XmNdefaultPosition, True,
			       XmNresizePolicy, XmRESIZE_NONE,
			       XmNallowOverlap, False,
			       XmNheight, height,
			       XmNnoResize,
			       (flags & (XcCD_HRESIZE|XcCD_VRESIZE))?
			       False : True,
			       width? XmNwidth : NULL, width,
			       NULL);

 Xc_REGISTER_HELP(WIDGET(p));

 rec.string = "XcCD_resize";
 rec.proc = (XtActionProc)cb_resize;
 XtAppAddActions(XtWidgetToApplicationContext(WIDGET(p)), &rec, 1);
  
 sprintf(trans, BBOARD_RESIZE, (long)p);
 XtOverrideTranslations(WIDGET(p), XtParseTranslationTable(trans));
  
 /*--- Add a callback for WM_DELETE_WINDOW protocol ---*/
 WM_DELETE_WINDOW = XmInternAtom(XtDisplay(p->w_Main),
				 "WM_DELETE_WINDOW", False);
 XmAddWMProtocolCallback(p->w_Main, WM_DELETE_WINDOW,
			 (void *)close_callback, (XtPointer)p);

 Xc_TRACE(("constructor end"));
 return p;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_CustomDialog *this;
{
 int i;

 Xc_HISTORY(("destructor"));

 unmap(this);

 /*--- Destroy buttons ---*/
 for(i = 0; this->button_list != 0; i++, this->button_list >>= 1)
  if(this->button_list & 1) XtDestroyWidget(this->w_button[i]);

 if (this->w_ActionArea)
  XtDestroyWidget(this->w_ActionArea);
 if (this->help_activ)
 {
  DELETE(c_Icon)(this->help);
  XtDestroyWidget(this->w_ActionAreaTop);
 }
 if (this->w_Separator)
  XtDestroyWidget(this->w_Separator);
 XtDestroyWidget(WIDGET(this));
 XtDestroyWidget(this->w_Main);
  
 if (MANAGER(this) != NULL)
  DELETE(c_ManageWidget)(MANAGER(this));
 Xc_free(this);
 Xc_TRACE(("destroyed"));
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(This)
c_CustomDialog *This;
{
 Xc_HISTORY(("copy"));
 Xc_FATAL(("not implemented"));
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** set_action_zone - Create ActionZone in CustomDialog               ** 
** ----------------------------------------------------------------- */
static void create_action_area(p, button_list, default_button, help_activ, cb, cb_data)
c_CustomDialog *p;
unsigned int button_list;
unsigned int default_button;
boolean help_activ;
void (*cb)();
void *cb_data;
{
 unsigned int argc, i, j, pos;
 XtWidgetGeometry geometry;

 Xc_HISTORY(("createActionArea"));
 Xc_ASSERT(button_list != 0);

 p->help_activ = help_activ;
 p->callback = cb;
 p->callback_data = cb_data;

 /*--- Separator ---*/
 p->w_Separator = XtVaCreateWidget("separator", xmSeparatorGadgetClass,
				   p->w_This,
				   XmNorientation, XmHORIZONTAL,
				   XmNshadowThickness, 2,
				   NULL);
 F(MANAGER(p)).Add_child(MANAGER(p), p->w_Separator, p->w_This);
 geometry.request_mode = CWHeight;
 XtQueryGeometry(p->w_Separator, NULL, &geometry);
 p->separator_height = geometry.height;

 /*--- How many actions do we got? ---*/
 for(i = button_list, argc = 0; i != 0; i >>= 1) if(i & 1) argc++;
 p->button_list = button_list;


#ifndef NO_REGISTER_HELP

 /*--- Create the Action Area as a Form ---*/
 if (help_activ)
 {   
  p->w_ActionAreaTop = XtVaCreateWidget("action_area_top", xmFormWidgetClass,
					p->w_This,
					NULL);
  p->help = (c_Icon *)NEW(c_Icon)(p->w_ActionAreaTop,
				  MANAGER(p),
				  "iBoxHelp", XtDisplay(p->w_ActionAreaTop),
				  help_xpm, NULL, XcPush);
  XtVaSetValues(WIDGET(p->help),
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNbottomAttachment, XmATTACH_FORM,
		NULL);
  CALLBACK(WIDGET(p->help), FUNC_ACTIVATE, MenuHelpCallHelp, NULL);
    
  /*--- Create the Action Area as a Form ---*/
  p->w_ActionArea = XtVaCreateWidget("action_area", xmFormWidgetClass,
				     p->w_ActionAreaTop,
				     XmNfractionBase, 
				     XcCD_TIGHTNESS*argc + 1,
				     XmNskipAdjust, True,
				     XmNshadowThickness, 0,
				     XmNtopAttachment, XmATTACH_FORM,
				     XmNleftAttachment, XmATTACH_FORM,
				     XmNrightAttachment, XmATTACH_WIDGET,
				     XmNrightWidget, WIDGET(p->help),
				     XmNbottomAttachment, XmATTACH_FORM,
				     NULL);
 }
 else
 {
#endif
  p->w_ActionAreaTop = 
   p->w_ActionArea = XtVaCreateWidget("action_area", 
				      xmFormWidgetClass,
				      p->w_This,
				      XmNfractionBase, XcCD_TIGHTNESS*argc + 1,
				      XmNskipAdjust, True,
				      XmNshadowThickness, 0,
				      NULL);
    
#ifndef NO_REGISTER_HELP
 }
#endif  
    
 /*--- Create the buttons ---*/
 i = j = 0;
 pos = 1;
 for( ; button_list != 0; j++, button_list >>= 1, default_button >>= 1)
  if(button_list & 1)
  {
   Widget button;

   Xc_TRACE(("add button `%s'", button_label[j]));
   button = XtVaCreateWidget(button_label[j],
			     xmPushButtonWidgetClass, p->w_ActionArea,
			     XmNtopAttachment, XmATTACH_FORM,
			     XmNbottomAttachment, XmATTACH_FORM,
			     XmNleftAttachment, XmATTACH_POSITION,
			     XmNleftPosition, pos,
			     XmNrightAttachment, XmATTACH_POSITION,
			     XmNrightPosition, pos + XcCD_TIGHTNESS - 1,
			     XmNnavigationType, XmTAB_GROUP,
			     XmNhighlightOnEnter, False,
			     NULL);
   p->w_button[j] = button;
   if(default_button == 1) p->default_button = j;

   pos += XcCD_TIGHTNESS;
   F(MANAGER(p)).Add_child(MANAGER(p), button, p->w_ActionArea);
   XtAddCallback(button, XmNactivateCallback,
		 (XtCallbackProc)activate_callback, p);
   if(i++ == 0)
   {
    Dimension h1, h2, h3, h4, h5;

    XtVaGetValues(p->w_ActionArea, XmNmarginHeight, &h1, NULL);
    h1 <<= 1;
    XtVaGetValues(button,
		  XmNheight, &h2, 
		  XmNshadowThickness, &h3,
		  XmNborderWidth, &h4,
		  XmNmarginHeight, &h5,
		  NULL);
    h1 += h2 + ((h3 + h4 + h5) << 1);
    p->action_height = h1;
   }
  }
  
 if (help_activ)
 {
  F(MANAGER(p)).Add_child(MANAGER(p), p->w_ActionArea, p->w_ActionAreaTop);
  F(MANAGER(p)).Add_child(MANAGER(p), p->w_ActionAreaTop, p->w_This);
 }
 else
 {
  F(MANAGER(p)).Add_child(MANAGER(p), p->w_ActionArea, p->w_This);
 }
}


/* ----------------------------------------------------------------- ** 
** map - Manage all widget and display it                            ** 
** ----------------------------------------------------------------- */
static void map(this, modal_flag)
c_CustomDialog *this;
boolean modal_flag;
{
 XConfigureEvent event;

 Xc_HISTORY(("map"));
 Xc_ASSERT(this->w_ActionArea != NULL);

 /*--- Set modality ---*/
 this->modal_flag = modal_flag;
 XtVaSetValues(WIDGET(this),
	       XmNdialogStyle, modal_flag?
	       XmDIALOG_FULL_APPLICATION_MODAL : XmDIALOG_MODELESS,
	       NULL);

 /*--- Manage ---*/
 if(MANAGER(this) != NULL)
 {
  c_ManageWidget *manager;

  manager = MANAGER(this);
  F(manager).Add_child(manager, this->w_This, this->w_Main);
  F(manager).Manage_all(manager, NULL); 
  MANAGER(this) = NULL;
 }

 /*--- Re-arrange widgets ---*/
 {
  XtWidgetGeometry geometry;
  char param[80], *parama[2];
  int	nparam = 1;
    
  geometry.request_mode = CWHeight;
  XtQueryGeometry(WIDGET(this), NULL, &geometry);
  event.width = this->width? this->width : geometry.width;
  event.height = this->height? this->height : geometry.height;
    
  sprintf(param, "%ld", (long)this);
  parama[0] = param;
  parama[1] = NULL;
  cb_resize(WIDGET(this), &event, parama, &nparam);
 }

 /*--- Map ---*/
 Xc_TRACE(("popup"));
 XtPopup(this->w_Main, modal_flag? XtGrabExclusive : XtGrabNone);
 F(this).resetTraversal(this);

 {
  XtAppContext app;
  
  app = XtWidgetToApplicationContext(this->w_Main);
  XSync(XtDisplay(this->w_Main), 0);
  while(42)
  {
   XEvent event;
	
   XtAppNextEvent(app, &event);
   if(event.xany.type == ButtonPress || event.xany.type == KeyPress)
    continue;
   if((event.xany.type == MapNotify) &&
      (event.xany.window == XtWindow(this->w_Main))) break;
   XcHandleEvent(&event);
  }
 }
 Xc_TIMING(("shell visible"));
 this->mapped_flag = TRUE;
}


/* ----------------------------------------------------------------- ** 
** unmap - Unmap the dialog                                          ** 
** ----------------------------------------------------------------- */
static void unmap(this)
c_CustomDialog *this;
{
 XtAppContext app;

 if(this->mapped_flag == FALSE) return;
 XtPopdown(this->w_Main);
  
 app = XtWidgetToApplicationContext(this->w_Main);
 XSync(XtDisplay(this->w_Main), 0);

 /*--- Wait unmap notify ---*/
 while(42)
 {
  XEvent event;
      
  XtAppNextEvent(app, &event);
  if((event.xany.type == UnmapNotify) &&
     (event.xany.window == XtWindow(this->w_Main))) break;
  XcHandleEvent(&event);
 }
 XSync(XtDisplay(this->w_Main), 0);

 /*--- Wait expose events ---*/
 while(42)
 {
  XEvent event;
      
  XtAppNextEvent(app, &event);
  if(event.xany.type != Expose) break;
  XcHandleEvent(&event);
 }

 XcFlushEvents(XtParent(this->w_Main), XcHandleEvent);
 this->mapped_flag = FALSE;
}


/* ----------------------------------------------------------------- ** 
** close_callback - Internal callback for the shell widget         ** 
** ----------------------------------------------------------------- */
static void close_callback(w, dialog)
Widget w;
c_CustomDialog *dialog;
{
 Xc_TRACE(("close_callback"));

 /*--- Callback ---*/
 if(dialog->callback != NULL)
  dialog->callback(dialog, dialog->callback_data, XcCD_DESTROY);
 else
  F(dialog).unmap(dialog);

 dialog->result = XcCD_DESTROY;

 Xc_TRACE(("close_callback done"));
}


/* ----------------------------------------------------------------- ** 
** activate_callback - Internal callback for the ActionArea          ** 
** ----------------------------------------------------------------- */
static void activate_callback(w, dialog, cbs)
Widget w;
c_CustomDialog *dialog;
XmAnyCallbackStruct *cbs;
{
 Xc_TRACE(("activate_callback (reason = %d)", cbs->reason));

 if(cbs->reason == XmCR_HELP)
 {
  Xc_TRACE(("help"));

  dialog->result = XcCD_HELP;
  if(dialog->callback != NULL)
  {
   dialog->callback(dialog, dialog->callback_data, XcCD_HELP);
   return;
  }
 }
 else if(cbs->reason == XmCR_ACTIVATE)
 {
  int i;
      
  for(i = 0; i < XcCD_NB_BUTTON_DEF; i++)
   if(dialog->w_button[i] == w)
   {
    Xc_TRACE(("button `%s'", button_label[i]));
	    
    dialog->result = 1<<i;
    if(dialog->callback != NULL)
    {
     dialog->callback(dialog, dialog->callback_data, 1<<i);
     return;
    }
   }
 }
 F(dialog).unmap(dialog);
}


/* ----------------------------------------------------------------- ** 
** setMenuBar - Set the custom dialog menu bar                       ** 
** ----------------------------------------------------------------- */
static void setMenuBar(this, bar)
c_CustomDialog *this;
c_MenuBar *bar;
{
#if 0
 Dimension h1, h2, h3, h4;
 Xc_HISTORY(("setMenuBar"));
  
 this->w_MenuBar = WIDGET(bar);
  
 XtVaGetValues(WIDGET(bar),
	       XmNheight, &h1, 
	       XmNshadowThickness, &h2,
	       XmNborderWidth, &h3,
	       XmNmarginHeight, &h4,
	       NULL);
  
 Xc_TRACE(("H:%d, T:%d, BW:%d, MH:%d", h1,h2,h3,h4));

 this->bar_height = h1 + ((h2 + h3 + h4) << 1);
#else
 Xc_HISTORY(("setMenuBar"));
  
 this->w_MenuBar = WIDGET(bar);
#endif
}


/* ----------------------------------------------------------------- ** 
** blockChild - Set size to a child                                  ** 
** ----------------------------------------------------------------- */
static void blockChild(this, w, size)
c_CustomDialog *this;
Widget w;
int size;
{
 Xc_HISTORY(("blockChild(%lx, %d)", (long)w, size));
  
 this->child[this->child_count] = w;
 if(size > 0)
  this->size[this->child_count] = size;
 else
 {
  XtWidgetGeometry geometry;
  
  geometry.request_mode = this->vertical_flag? CWHeight:CWWidth;
  XtQueryGeometry(w, NULL, &geometry);
  this->size[this->child_count] =
   this->vertical_flag? geometry.height : geometry.width;
 }
 this->total_size += this->size[this->child_count++];
 Xc_ASSERT(this->child_count < XcCD_MAX_BLOCKED_CHILDREN);
}



/* ----------------------------------------------------------------- ** 
** cb_resize - Bulletin board resize callback                        ** 
** ----------------------------------------------------------------- */
static void cb_resize(bboard, event,  args, nargs)
Widget bboard;
XConfigureEvent *event;
char	*args[];
int	*nargs;
{
 Dimension margin_w, margin_h;
 c_CustomDialog *dialog;
 WidgetList children;
 Cardinal count;
 int i, h, w, y, x;

 Xc_TRACE(("cb_resize(%d %d)", event->width, event->height));
  
 if (*nargs != 1)
  return;
  
 dialog = (c_CustomDialog *)atol(args[0]);
  
 XtVaGetValues((Widget)bboard,
	       XmNchildren, &children,
	       XmNnumChildren, &count,
	       XmNmarginWidth, &margin_w,
	       XmNmarginHeight, &margin_h,
	       NULL);

 if((dialog->width == event->width) && (dialog->height = event->height))
 {
  Xc_TRACE(("size already set"));
  /*return;*/
 }
  
 dialog->width = event->width;
 dialog->height = event->height;

  
 h = (event->height - dialog->action_height - dialog->separator_height
      - (margin_h << 2));
 w = event->width - (margin_w << 1);
 y = margin_h;
 x = margin_w;
 if((w <= 0) || (h <= 0)) return;

 count -= 2;
 XtConfigureWidget(dialog->w_Separator,
		   0,
		   (event->height - dialog->separator_height
		    - dialog->action_height - (margin_h << 1)),
		   event->width,
		   dialog->separator_height, 0);
 XtConfigureWidget(dialog->w_ActionAreaTop,
		   margin_w,
		   event->height - dialog->action_height - margin_h,
		   event->width - (margin_w << 1),
		   dialog->action_height, 0);

 if(dialog->w_MenuBar)
 {
  XtWidgetGeometry geometry;
  
  geometry.request_mode = CWHeight;
  XtQueryGeometry(dialog->w_MenuBar, NULL, &geometry);
  dialog->bar_height = geometry.height;

  h -= dialog->bar_height;
  y += dialog->bar_height;
  if(h <= 0) return;

  count--;
  XtConfigureWidget(dialog->w_MenuBar, 0, 0,
		    event->width, dialog->bar_height, 0);
  XRaiseWindow(XtDisplay(dialog->w_MenuBar), XtWindow(dialog->w_MenuBar));
 }

 XRaiseWindow(XtDisplay(dialog->w_ActionAreaTop),
	      XtWindow(dialog->w_ActionAreaTop));

 if((int)count != dialog->child_count)
 {
  if(dialog->vertical_flag == FALSE)
  {
   w -= dialog->total_size;
   w /= (count - dialog->child_count);
  }
  else
  {
   h -= dialog->total_size;
   h /= (count - dialog->child_count);
  }
 }

 i = 0;
 while(count)
 {
  int j, ww, wh;
  Widget wc;

  wc = children[i++];
  if((wc == dialog->w_ActionAreaTop) || (wc == dialog->w_MenuBar) ||
     (wc == dialog->w_Separator)) continue;
  ww = w;
  wh = h;
  for(j = 0; j < dialog->child_count; j++)
   if(wc == dialog->child[j])
   {
    if(dialog->vertical_flag == FALSE)
     ww = dialog->size[j];
    else
     wh = dialog->size[j];
    break;
   }
  Xc_TRACE(("  child #%lx x=%d y=%d w=%d h=%d b=%d",
	    (long)wc, x, y, ww, wh, 0));
  XtConfigureWidget(wc, x, y, ww, wh, 0);
  if(dialog->vertical_flag == FALSE)
   x += ww;
  else
   y += wh;
  count--;
 }  
 Xc_TRACE(("end cb_resize"));
}


/* ----------------------------------------------------------------- ** 
** resetTraversal - Reset the traversal to the default button        ** 
** ----------------------------------------------------------------- */
static void resetTraversal(this)
c_CustomDialog *this;
{
 Xc_HISTORY(("resetTraversal"));
  
 if(this->default_button >= 0)
 {
  XmProcessTraversal(this->w_button[this->default_button],
		     XmTRAVERSE_CURRENT);
  Xc_TRACE(("XmProcessTraversal done"));
 }
#ifndef NTRACE
 else
  Xc_TRACE(("No default button!"));
#endif
}


/* ----------------------------------------------------------------- ** 
** waitForUserAction - Wait for an action                            ** 
** ----------------------------------------------------------------- */
static int waitForUserAction(this)
c_CustomDialog *this;
{
 Xc_HISTORY(("waitForUserAction"));

 if(this->mapped_flag == TRUE)
 {
  XtAppContext app;

  app = XtWidgetToApplicationContext(WIDGET(this));
  XSync(XtDisplay(WIDGET(this)), 0);

  while(this->mapped_flag)
  {
   XEvent event;
	  
   XtAppNextEvent(app, &event);

   if(this->modal_flag)
   {
    if(event.type == Expose)
     XcHandleEvent(&event);
    else
     XtDispatchEvent(&event);
   }
   else
    XcHandleEvent(&event);
  }
 }
 Xc_TRACE(("waitForUserAction done"));
 return this->result;
}

