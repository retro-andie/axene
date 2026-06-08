/*
** main.c for AxeneOffice in Main/
** Main Part of AxeneOffice
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
** Started on  Thu May 22 13:53:30 1997 Robin Castanier
** Last update Tue Feb  1 11:46:09 2000 Emmanuel Paris
*/

#define __XC_MAIN__

#include <X11/cursorfont.h>
#include <Xm/Frame.h>
#define SIGNALH
#define IOERRORH
#include "xcalibur.h"
#include "XErrorHandler.h"
#include "SignalHandler.h"

#include "WInterface.h"
#include "Interface.h"
#include "MainInterface.h"
#include "AlertBox.h"
#include "Resource.h"
#include "CustomDialog.h"
#include "DocIcon.h"

#include "Colormap.h"
#include "Cursor.h"
#include "ImageManager.h"
#include "HelpLine.h"
#include "LaunchCmd.h"

#include "file.h"
#include "check.h"

#ifdef HAVE_XSHM
#include <X11/extensions/XShm.h>
extern Bool XShmQueryExtension();
boolean xshm_flag;
#endif

#ifdef HAVE_XSHAPE
#include <X11/extensions/shape.h>
boolean xshape_flag;
#endif

c_XErrorHandler	*XErrH;
int IOErrorCount = 0;

#ifdef SIGNALH
c_SignalHandler	*SignalH;
jmp_buf SigErrorEnv;
#endif
int SigErrorCount = 1;

c_MainInterface *MainInterface;
c_Interface	*GlobInterface;
c_Interface	*CurrentInterface;
c_DocIcon	*GlobDocIcon;
Widget wMainWindow;
XtIntervalId	TimeOutid = (XtIntervalId)0;
Dimension iH=0, iW=0, mH=0, mW=0, S=0;

/* ------------- Motif Objects ----------------*/

c_Colormap 	*GlobColormap;
c_GCursor  	*GlobCursor = NULL;
c_ImageManager  *GlobImgMng;
c_HelpLine	*GlobHelpLine;
c_Resource	*GlobResources;
c_LaunchCmd	*GlobLaunchCmd;
char		*Globprog_name;

static void Signal_Error_Handler();
static void IO_Error_Handler();

void XcHandleEvent ___PROTO((XEvent *event));
void XcFlushEvents ___PROTO((Widget w, void (*func) ___PROTO((XEvent *))));
void cb_quit ___PROTO((Widget w, c_Interface *This));
void cba_quit ___PROTO((Widget w, XEvent *event, char *args[], int *nargs));
void cba_popup ___PROTO((Widget w, XEvent *event, char *args[], int *nargs));
void cb_launch ___PROTO((Widget w, int softnum));
static void raise_win ___PROTO((c_Interface *This));
static void calcul_shape ___PROTO((c_Interface *This));
static void remove_shape ___PROTO((c_Interface *This));

extern void XtResizeWidget ___PROTO((Widget w, Dimension width,
				     Dimension height, Dimension borderwidth));

/* NB_SOFT is defined in Interface.h */
/* launch_info_t is defined in LaunchCmd.h */
static launch_info_t Office[NB_SOFT] = 
{
 { 0, "Xclamation", NULL, FALSE },
 { 1, "XQuad", NULL, FALSE },
 { 2, "XAllWrite", NULL, FALSE },
 { 3, "XMayday", NULL, FALSE }
};

int main(argc, argv)
int  argc;
char **argv;
{
 Display  *display;
 XtAppContext	  app;
 XSetWindowAttributes	set_attrib;
 XWindowAttributes	get_attrib;
 int i;
 
 Globprog_name = SearchFile(argv[0]);
 
 check_configuration();
 
 MainInterface = NEW(c_MainInterface)(argc, argv);

 display = XtDisplay(MainInterface->w_Hidden);
  
 GlobResources = NEW(c_Resource)(MainInterface->w_Hidden);
  
 Xc_TRACE(("resource ownColormap: %s keepFreeColor: %d",
	   F(GlobResources).getBoolean(GlobResources, XcR_ownColormap) ?
	   "On" : "Off",
	   F(GlobResources).getInt(GlobResources, XcR_keepFreeColor)));

 GlobColormap = (c_Colormap *)NEW(c_Colormap)
  (display, MainInterface->w_Hidden,
   F(GlobResources).getBoolean(GlobResources, XcR_ownColormap),
   F(GlobResources).getInt(GlobResources, XcR_keepFreeColor),
   NO_FORCE);
  
 XErrH = (c_XErrorHandler *)NEW(c_XErrorHandler)();
#ifdef IOERRORH
 F(XErrH).Add_Callback(XcXH_IO_ERROR_CODE, IO_Error_Handler, 0); 
#endif
  
#ifdef HAVE_XSHM
 xshm_flag = XShmQueryExtension(display)? TRUE : FALSE;
 Xc_TRACE(("XShm extension %s", xshm_flag? "ON" : "OFF"));
 GlobImgMng = (c_ImageManager *)NEW(c_ImageManager)(XErrH, display, 
						    xshm_flag);
#else
 GlobImgMng = (c_ImageManager *)NEW(c_ImageManager)(XErrH, display, FALSE);
#endif
  
#ifdef HAVE_XSHAPE
 {
  int event_base, error_base;/* just used as dummy parameters */
  xshape_flag = XShapeQueryExtension(display, &event_base, &error_base);
  Xc_TRACE(("XShape extension %s", xshape_flag? "ON" : "OFF"));
 }
#endif

 GlobCursor = (c_GCursor *)NEW(c_GCursor)(display);
 GlobDocIcon = (c_DocIcon *)NEW(c_DocIcon)(display);
 GlobLaunchCmd = (c_LaunchCmd *)NEW(c_LaunchCmd)(display, 
						 MainInterface->display_name);

 GlobHelpLine = (c_HelpLine *)NEW(c_HelpLine)(XcHL_NO_LABEL, display);
 
#ifdef SIGNALH
 SignalH = (c_SignalHandler *)NEW(c_SignalHandler)();
 F(SignalH).Add_Callback(SignalH, SIGINT, Signal_Error_Handler,
			 XcR_sigErrorINT);
 F(SignalH).Add_Callback(SignalH, SIGILL, Signal_Error_Handler,
			 XcR_sigErrorILL);
 F(SignalH).Add_Callback(SignalH, SIGSEGV, Signal_Error_Handler,
			 XcR_sigErrorSEGV);
 F(SignalH).Add_Callback(SignalH, SIGPIPE, Signal_Error_Handler,
			 XcR_sigErrorPIPE);
 F(SignalH).Add_Callback(SignalH, SIGBUS, Signal_Error_Handler,
			 XcR_sigErrorBUS);
 F(SignalH).Add_Callback(SignalH, SIGFPE, Signal_Error_Handler,
			 XcR_sigErrorFPE);
#ifdef SIGABRT
 F(SignalH).Add_Callback(SignalH, SIGABRT, Signal_Error_Handler,
			 XcR_sigErrorIOT);
#else
#ifdef SIGIOT
 F(SignalH).Add_Callback(SignalH, SIGIOT, Signal_Error_Handler,
			 XcR_sigErrorIOT);
#endif
#endif
#ifdef SIGEMT
#ifdef SIGXCPU
 if (SIGEMT != SIGXCPU)
#endif
  F(SignalH).Add_Callback(SignalH, SIGEMT, Signal_Error_Handler,
			  XcR_sigErrorEMT);
#endif
#ifdef SIGXCPU
 F(SignalH).Add_Callback(SignalH, SIGXCPU, Signal_Error_Handler,
			 XcR_sigErrorXCPU);
#endif
#ifdef SIGXFSZ
 F(SignalH).Add_Callback(SignalH, SIGXFSZ, Signal_Error_Handler,
			 XcR_sigErrorXFSZ);
#endif
#endif
  
 GlobInterface = (c_Interface *)NEW(c_Interface)(MainInterface);    
 CurrentInterface = GlobInterface;

 F(GlobCursor).enable_wait_mode(GlobCursor, XtWindow(WIDGET(MainInterface)));
  
 XGetWindowAttributes(display, XtWindow(WIDGET(MainInterface)), &get_attrib);
 set_attrib.event_mask = get_attrib.all_event_masks | StructureNotifyMask;
 XChangeWindowAttributes(display, XtWindow(WIDGET(MainInterface)),
			 CWEventMask, &set_attrib);
 /*F(GlobColormap).generate_colormap(GlobColormap);*/
  
 Xc_mcheck();

 for(i = 0; i < NB_SOFT; i++)
 {
  Office[i].file_name = SearchFile(Office[i].soft_name);
  if (Office[i].file_name != NULL)
  {
   Office[i].exist = TRUE;
  }
  else
  {
   XtSetSensitive(WIDGET(GlobInterface->Icon[i]), False);
   F(GlobInterface->popup).ButtonFlag(GlobInterface->popup,
				      POPUP_XCLAMATION + i, False);
   Office[i].exist = FALSE;
   {
    XSetWindowAttributes attrib;
    attrib.event_mask = EnterWindowMask | LeaveWindowMask |
     ExposureMask | StructureNotifyMask | FocusChangeMask;
    attrib.do_not_propagate_mask = 0;
    XChangeWindowAttributes(XtDisplay(WIDGET(GlobInterface->Icon[i])),
			    XtWindow(WIDGET(GlobInterface->Icon[i])),
			    CWEventMask | CWDontPropagate, &attrib);
   }
  }
 }
 
 SET_HELPLINE(GlobHelpLine, 
	      WIDGET(GlobInterface->Icon[0]),helpXclamation, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, 
	      WIDGET(GlobInterface->Icon[1]),helpXQuad, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, 
	      WIDGET(GlobInterface->Icon[2]),helpXAllWrite, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, 
	      WIDGET(GlobInterface->Icon[3]),helpXMayday, XcR_NONE);

 XtVaGetValues(WIDGET(GlobInterface->Icon[0]), XmNwidth, &iW,
	       XmNheight,  &iH, NULL);
 XtVaGetValues(WIDGET(GlobInterface->rcMain), XmNmarginWidth, &mW,
	       XmNmarginHeight,  &mH, XmNspacing, &S, NULL);

 SigErrorCount = 0;		/* can continue now */
 app = XtWidgetToApplicationContext(wMainWindow);
 while(42)
 {
  XEvent event;
    
#ifdef SIGNALH 
  if (setjmp(SigErrorEnv) == -1)
   Xc_WARNING(("Error Signal detected, trying to continue"));
#endif
    
  XtAppNextEvent(app, &event);
  XcHandleEvent(&event);
 }
 return 0;
}

/* ----------------------------------------------------------------- ** 
** HandleEvent - Process event                                       ** 
** ----------------------------------------------------------------- */
void XcHandleEvent(event)
XEvent *event;
{
 c_Interface *Interface;
    
 Interface = F(GlobInterface).get_by_window(GlobInterface,
					    event->xany.window);
  
 if (!Interface) Interface = GlobInterface;
 wMainWindow = WIDGET(Interface->MainInterface);
 
 XtDispatchEvent(event);
}


void QuitApplication()
{
 int i;
 Xc_HISTORY(("Quit AxeneOffice"));

 if (TimeOutid != (XtIntervalId)0)
  XtRemoveTimeOut(TimeOutid);

 DELETE(c_Interface)(GlobInterface, TRUE);
 
#ifdef  SIGNALH
 DELETE(c_SignalHandler)(SignalH);
#endif

 Xc_free(Globprog_name);
 for(i = 0; i < NB_SOFT; i++)
 {
  if (Office[i].exist)
   Xc_free(Office[i].file_name);
 }

 DELETE(c_HelpLine)(GlobHelpLine);
 DELETE(c_Colormap)(GlobColormap);
 DELETE(c_ImageManager)(GlobImgMng);
 DELETE(c_DocIcon)(GlobDocIcon);
 DELETE(c_LaunchCmd)(GlobLaunchCmd);
 DELETE(c_GCursor)(GlobCursor);
 DELETE(c_Resource)(GlobResources);
 DELETE(c_XErrorHandler)(XErrH);
  
#ifndef NMEMDEBUG
 Xc_mdump();
#endif

 Xc_exit(0);
}

/* ************************************************************************ */
/* **                        ERRORS HANDLERS                             ** */
/* ************************************************************************ */
static void Signal_Error_Handler(resource)
int	resource;
{
 XmString	xms;
  
 if (SigErrorCount == 0)
 {
  SigErrorCount = 1;
  if (IOErrorCount == 1)
  {
   Xc_WARNING(("Signal Error In IOError Handling. FATAL..."));
   Xc_exit (-1);
  }
  Xc_TRACE(("Signal Error Handle %d", resource));

  while(TEST_WAIT_MODE)
  {
   UNSET_WAIT_MODE_ANIM;
   UNSET_WAIT_MODE_STATIC;
  }
    
  SET_WAIT_MODE_STATIC;
  xms = F(GlobResources).getXmString(GlobResources, resource);
    
  switch(Xc_SigErrorAlert(xms, FALSE))
  {
  case XcCD_CONTINUE:
   XmStringFree(xms);
   SigErrorCount = 0;
   Xc_TRACE(("call longjmp..."));
   UNSET_WAIT_MODE_STATIC;
   longjmp(SigErrorEnv, -1);
   break;
  case XcCD_QUIT:
  case XcCD_DESTROY:
   Xc_TRACE(("Quiting on Signal Error"));
   Xc_exit (-1);
   break;
  }
 }
 else
 {
  Xc_TRACE(("Arg error in signal handler"));
  if (SigErrorCount == 1 && IOErrorCount == 0)
  {
   SigErrorCount = 2;
   xms = F(GlobResources).getXmString(GlobResources, XcR_sigErrorSorry);
   Xc_ErrorAlert2(xms);
   XmStringFree(xms);
  }
  Xc_exit (-1);
 }
}

static void IO_Error_Handler(none)
int	none;
{
 Display	*display;

 if (IOErrorCount == 0)
 {
  IOErrorCount = 1;
  Xc_TRACE(("IO Error Handle"));
    
  F(MainInterface).reopen_display(MainInterface);
  display =  XtDisplay(MainInterface->w_Hidden);
    
  GlobResources = NEW(c_Resource)(MainInterface->w_Hidden);
  GlobColormap = (c_Colormap *)NEW(c_Colormap)(display, (Widget)0,
					       FALSE, 0, NO_FORCE);
  GlobCursor = (c_GCursor *)NEW(c_GCursor)(display);
    
  switch(Xc_IOErrorAlert(FALSE))
  {
  case XcCD_QUIT:
  case XcCD_DESTROY:
   Xc_TRACE(("Quiting on IOError"));
   Xc_exit (-1);
   break;
  }
 }
 else
 {
  Xc_TRACE(("IoError come into IoError handler"));
  Xc_WARNING(("Sorry, can't recover from previous XIOError, exiting..."));
  Xc_exit (-1);
 }
}


/* ----------------------------------------------------------------- ** 
** XcFlushEvents - Handle all pendings events                        ** 
** ----------------------------------------------------------------- */

void XcFlushEvents(w, func)
Widget w;
void (*func) ___PROTO((XEvent *));
{
 XtAppContext app;
 XEvent event;

 app = XtWidgetToApplicationContext(w);
 XSync(XtDisplay(w), 0);

 while(XtAppPending(app))
 {
  XtAppNextEvent(app, &event);
  func(&event);
 }
}

/* ----------------------------------------------------------------- ** 
** raise_win - raise main window (correct a kdewm bug)               ** 
** ----------------------------------------------------------------- */
static void raise_win(This)
c_Interface *This;
{
 XMapWindow(XtDisplay(This->MainInterface->w_Main), 
	    XtWindow(This->MainInterface->w_Main)); 
 TimeOutid = (XtIntervalId)0;
}

/* ----------------------------------------------------------------- ** 
** cb_quit - Quit Task Bar                                           ** 
** ----------------------------------------------------------------- */
void cb_quit(w, This)
Widget w;
c_Interface *This;
{
 QuitApplication();
}

/* ----------------------------------------------------------------- ** 
** cba_quit - Quit Task Bar                                          ** 
** ----------------------------------------------------------------- */
void cba_quit(w, event,  args, nargs)
Widget w;
XEvent *event;
char	*args[];
int	*nargs;
{
 QuitApplication();
}

/* ----------------------------------------------------------------- ** 
** cba_popup - Call the popup menu                                   ** 
** ----------------------------------------------------------------- */
void cba_popup(w, event,  args, nargs)
Widget w;
XEvent *event;
char	*args[];
int	*nargs;
{
 c_Interface *This;

 This = F(GlobInterface).get_by_widget(GlobInterface, w);
 F(This->popup).mapAndWaitForMap(This->popup, event);
 
 F(This->popup).waitForUnmap(This->popup);
}

/* ----------------------------------------------------------------- ** 
** cb_override - Toggle override redirect                            ** 
** ----------------------------------------------------------------- */
void cb_override(w, This)
Widget w;
c_Interface *This;
{
 XtAppContext app;
 Widget shell;
 int i;

 shell = This->MainInterface->w_Main;
 
 if (This->override)
 {
  XtVaSetValues(shell, XmNoverrideRedirect, False, NULL);
  for(i = 0; i < This->nbIcon; i++)
   XtVaSetValues(WIDGET(This->Icon[i]), XmNhighlightThickness, 1, NULL);
  XtVaSetValues(WIDGET(This->rcMain), XmNmarginWidth, mW, XmNmarginHeight, mH,
		XmNspacing, S, NULL);
  XtPopdown(shell);
  remove_shape(This);
  XtPopup(shell, XtGrabNone);
  app = XtWidgetToApplicationContext(shell);
  if (TimeOutid == (XtIntervalId)0)
   TimeOutid = XtAppAddTimeOut(app, 1000L, 
			       (XtTimerCallbackProc)raise_win, This);
  F(This->popup).ButtonFlag(This->popup, POPUP_TRANSIENT, True);
 }
 else
 {
  XtVaSetValues(shell, XmNoverrideRedirect, True, NULL); 
  for(i = 0; i < This->nbIcon; i++)
   XtVaSetValues(WIDGET(This->Icon[i]), XmNhighlightThickness, 0, NULL);
  XtVaSetValues(WIDGET(This->rcMain), XmNmarginWidth, mW+1, 
		       XmNmarginHeight, mH+1, XmNspacing, S+2, NULL);

  XtPopdown(shell);
  calcul_shape(This);
  XFlush(XtDisplay(shell));
  XtPopup(shell, XtGrabNone);
  XFlush(XtDisplay(shell));
  app = XtWidgetToApplicationContext(shell);
  if (TimeOutid == (XtIntervalId)0)
   TimeOutid = XtAppAddTimeOut(app, 1000L, 
			       (XtTimerCallbackProc)raise_win, This);
  XSync(XtDisplay(shell), False);
  while(42)
  {
   XEvent event;
   
   XtAppNextEvent(app, &event);
   if((event.xany.type == MapNotify) &&
      (event.xany.window == XtWindow(shell))) break;
   XtDispatchEvent(&event);
  }
  XcFlushEvents(shell, (void (*)())XtDispatchEvent);
  XmUpdateDisplay(This->MainInterface->w_Hidden);
  XMapWindow(XtDisplay(shell), XtWindow(shell));
  F(This->popup).ButtonFlag(This->popup, POPUP_TRANSIENT, False);
 }
 This->override ^= TRUE;
 F(This->popup).ToggleButton(This->popup, POPUP_OVERRIDE, 
			     This->override ? False : True);
}

/* ----------------------------------------------------------------- ** 
** cba_override - Toggle override redirect                           ** 
** ----------------------------------------------------------------- */
void cba_override(w, event, args, nargs)
Widget w;
XEvent *event;
char	*args[];
int	*nargs;
{
 cb_override(w, F(GlobInterface).get_by_widget(GlobInterface, w));
}

/* ----------------------------------------------------------------- ** 
** cb_transient - Toggle transient                                   ** 
** ----------------------------------------------------------------- */
void cb_transient(w, This)
Widget w;
c_Interface *This;
{
 XtAppContext app;
 Widget shell;
    
 shell = This->MainInterface->w_Main;
 
 if (This->transient)
 {
  XtVaSetValues(shell, XmNtransient, False, NULL);
  XtPopdown(shell);
  XtPopup(shell, XtGrabNone);
 }
 else
 {
  XtVaSetValues(shell, XmNtransient, True, NULL);
  XtPopdown(shell);
  XFlush(XtDisplay(shell));
  XtPopup(shell, XtGrabNone);
  XFlush(XtDisplay(shell));
  app = XtWidgetToApplicationContext(shell);
  XSync(XtDisplay(shell), False);
  while(42)
  {
   XEvent event;
   
   XtAppNextEvent(app, &event);
   if((event.xany.type == MapNotify) &&
      (event.xany.window == XtWindow(shell))) break;
   XtDispatchEvent(&event);
  }
  XcFlushEvents(shell, (void (*)())XtDispatchEvent);
  XmUpdateDisplay(This->MainInterface->w_Hidden);
  XMapWindow(XtDisplay(shell), XtWindow(shell));  
 }
 This->transient ^= TRUE;
 F(This->popup).ToggleButton(This->popup, POPUP_TRANSIENT,
			     This->transient ? False : True);
}

/* ----------------------------------------------------------------- ** 
** cba_transient - Toggle transient                                  ** 
** ----------------------------------------------------------------- */
void cba_transient(w, event, args, nargs)
Widget w;
XEvent *event;
char	*args[];
int	*nargs;
{
 cb_transient(w, F(GlobInterface).get_by_widget(GlobInterface, w));
}

/* ----------------------------------------------------------------- ** 
** cb_flip - Flip Horizontal/Vertical layout                         ** 
** ----------------------------------------------------------------- */
void cb_flip(w, This)
Widget w;
c_Interface *This;
{
 Dimension width, height;
 Widget shell;
    
 shell = This->MainInterface->w_Main;
 
 This->flip ^= TRUE;
 Xc_TRACE(("icon width: %d icon height: %d nb icon: %d", 
	   iW, iH, This->nbIcon));
 Xc_TRACE(("margin width: %d margin height: %d spacing: %d", mW, mH, S));
 
 if (This->flip)		/* Flip TO VERTICAL */
 {
  width = iW + 2*mW;
  height = (This->nbIcon * iH) + ((This->nbIcon -1) * S) + 2*mH;
  Xc_TRACE(("set to width: %d height: %d", width, height));
  F(This->popup).ToggleManage(This->popup, 
			      POPUP_FLIP_VERTICAL, POPUP_FLIP_HORIZONTAL);

  XtVaSetValues(shell, XmNminWidth, width,
		XmNmaxWidth, width, XmNwidth, width,
		XmNmaxHeight, height, XmNminHeight, height,
		XmNheight, height, NULL);
 }
 else				/* Flip TO HORIZONTAL */
 {
  height = iH + 2*mH;
  width = (This->nbIcon * iW) + ((This->nbIcon -1) * S) + 2*mW;
  Xc_TRACE(("set to width: %d height: %d", width, height));
  F(This->popup).ToggleManage(This->popup, 
			      POPUP_FLIP_HORIZONTAL, POPUP_FLIP_VERTICAL);

  XtVaSetValues(shell, XmNmaxWidth, width,
		XmNminWidth, width, XmNwidth, width,
		XmNminHeight, height, XmNmaxHeight, height,
		XmNheight, height, NULL);
 }
 
 XtResizeWidget(shell, width, height, (Dimension)0);

 if (This->flip)
  XtVaSetValues(WIDGET(This->rcMain), XmNorientation, XmVERTICAL, NULL);
 else
  XtVaSetValues(WIDGET(This->rcMain), XmNorientation, XmHORIZONTAL, NULL);

 XtPopdown(shell);
 XtPopup(shell, XtGrabNone);

 if (This->override)
  calcul_shape(This);
}

/* ----------------------------------------------------------------- ** 
** cb_launch - Launch one of Axene Softwares                         ** 
** ----------------------------------------------------------------- */
void cb_launch(w, softnum)
Widget w;
int softnum;
{
 F(GlobLaunchCmd).launch_command(GlobLaunchCmd, &Office[softnum]);
}

/* ----------------------------------------------------------------- ** 
** cba_launch - Launch one of Axene Softwares                        ** 
** ----------------------------------------------------------------- */
void cba_launch(w, event, args, nargs)
Widget w;
XEvent *event;
char	*args[];
int	*nargs;
{
 c_Interface *This;
 int i;

 if (*nargs != 1) return;

 This = F(GlobInterface).get_by_widget(GlobInterface, w);

 if (strlen(args[0]) == 1 && (int)(*args[0] - '1') >= 0 && 
     (int)(*args[0] - '1') <= This->nbIcon)
 {
  cb_launch(w, (int)(*args[0] - '1'));
  return;
 }
 
 for(i = 0; i < This->nbIcon; i++)
 {
  if (Office[i].exist && Xstrcmp(args[0], Office[i].soft_name))
  {
   cb_launch(w, i);
   return;
  }
 }
}

/* ----------------------------------------------------------------- ** 
** calcul_shape - Calcul the application shape                       ** 
** ----------------------------------------------------------------- */
static void calcul_shape(This)
c_Interface *This;
{
#ifdef HAVE_XSHAPE
 Pixmap shape_mask ;
 XGCValues xgcv;
 GC shapeGC ;
 Widget shell = This->MainInterface->w_Main;
 Widget rcmain = WIDGET(This->rcMain);
 Display *display = XtDisplay(rcmain);
 Window window = XtWindow(rcmain);
 int i;
 
 if (xshape_flag)
 {
  Dimension width, height;
   
  if (This->flip)		/* VERTICAL */
  {
   width = iW + 2*mW;
   height = (This->nbIcon * iH) + ((This->nbIcon -1) * S) + 2*mH;
  }
  else				/* HORIZONTAL */
  {
   height = iH + 2*mH;
   width = (This->nbIcon * iW) + ((This->nbIcon -1) * S) + 2*mW;
  }
   
  shape_mask = XCreatePixmap(display, window, width, height, 1);
   
  xgcv.function = GXcopy;
  xgcv.foreground = 0;
   
  shapeGC = XCreateGC(display, shape_mask, GCFunction | GCForeground, &xgcv);
  XFillRectangle(display, shape_mask, shapeGC, 0, 0, width, height);

  /* rcmain the bounding/clipping shape : a square for each icon */
  XSetForeground(display, shapeGC, 1);
   
  if (This->flip)		/* VERTICAL*/
  {
   for(i = 0; i < This->nbIcon; i++)
   {
    XFillRectangle(display, shape_mask, shapeGC, 
		   mW+1, mH + i * iH + S * i + 1, iW - 2, iH - 2); 
   }
  }
  else				/* HORIZONTAL */
  {
   for(i = 0; i < This->nbIcon; i++)
   {
    XFillRectangle(display, shape_mask, shapeGC, 
		   mW + i * iW + S * i + 1, mH + 1, iW - 2, iH - 2); 
   }
  }

  /* shape the parent for event managing and the widget for rcmaining */
  XShapeCombineMask(display, XtWindow(shell), 
		     ShapeBounding, 0, 0, shape_mask, ShapeSet);
  XShapeCombineMask(display, window, ShapeClip, 0, 0, shape_mask, ShapeSet);
  XFreePixmap(display, shape_mask);
  /* don't ask me why I use alternatively rcmain and shell as
     a parameter of XtDisplay, it doesn't matter at all */
 }
#endif
}

/* ----------------------------------------------------------------- ** 
** remove_shape - Remove the application shape                       ** 
** ----------------------------------------------------------------- */
static void remove_shape(This)
c_Interface *This;
{
#ifdef HAVE_XSHAPE
 Widget shell = This->MainInterface->w_Main;
 Widget rcmain = WIDGET(This->rcMain);
 Display *display = XtDisplay(rcmain);
 
 if (xshape_flag)
 {
  XShapeCombineMask(display, XtWindow(shell), ShapeBounding,
		    0, 0, None, ShapeSet);
  XShapeCombineMask(display, XtWindow(rcmain), ShapeClip,
		    0, 0, None, ShapeSet);
 }
#endif
}



