/*
** main.c for XMayday in Main/
** Main Part of XMayday
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
** Started on  Mon Jul 17 15:05:00 1995 Antoine Buat
** Last update Thu Feb  3 16:33:25 2000 Emmanuel Paris
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
#include "CustomDialog.h"
#include "AlertBox.h"
#include "RemoteServer.h"
#include "Resource.h"
#include "DocIcon.h"
#include "Box_about.h"
#include "Box_print.h"
#include "Box_cache.h"

#include "RegisterHelp.h"
#include "Colormap.h"
#include "Cursor.h"
#include "ImageManager.h"
#include "ImageCache.h"

#include "check.h"
#include "config.h"
#include "printer.h"

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

c_MainInterface  *MainInterface;
c_Interface	 *GlobInterface;
c_Interface	 *CurrentInterface;
Widget wMainWindow;

/* ------------- Motif Objects ----------------*/
c_RowColumn      *MainDocument;

c_RemoteServer	*GlobRemoteServer;
c_RegisterHelp	*GlobRegHelp;
c_Colormap 	*GlobColormap;
c_DocIcon	*GlobDocIcon;
c_ImageCache	*GlobImgCache;
c_ImageManager	*GlobImgMng;
c_GCursor  	*GlobCursor = NULL;
c_Resource	*GlobResources;
printer_t	*GlobPrinterBase = NULL;
char		*Globprog_name = NULL;

print_param_t GlobPrintParam = 
{
 NULL, FALSE, FALSE, TRUE, FALSE, 2, 1, &Xc_Table_Page_Format[1]
};

static void SensitivizePrevious();
static void SensitivizeNext();
static void SensitivizeRefresh();
static void SensitivizeIndex();
static void SensitivizeICreation();
static void SensitivizeClose();

void MenuFileNewXMayday();
void MenuFileOpenIndex();
void MenuFileOpen();
void MenuFilePrint();
void MenuFileClose();
void MenuFileQuit();
void QuitApplication();

void ReloadAll();
void RefreshAll();
void DisplayIndex();
void DisplayBottomBar();
static void SendRefreshRequest();

void MenuHelpCache();
void MenuHelpAbout();
void MenuHelpCallHelp();
void MenuHelpTrackHelp();
void SelectFunction();

static void Signal_Error_Handler();
static void IO_Error_Handler();

void XcHandleEvent ___PROTO((XEvent *event));
void XcFlushEvents ___PROTO((Widget w, void (*func) ___PROTO((XEvent *))));

#define SET_WAIT_MODE_STATIC2 \
if (Interface && Interface != CurrentInterface) \
{ \
   F(GlobCursor).enable_wait_mode(GlobCursor, \
				  XtWindow(WIDGET(Interface->MainInterface))); \
				   CurrentInterface = Interface; \
} \
SET_WAIT_MODE_STATIC

int main(argc, argv)
int  argc;
char **argv;
{
 Display  *display;
 XtAppContext	  app;
 XSetWindowAttributes	set_attrib;
 XWindowAttributes	get_attrib;
  
 Globprog_name = SearchFile(argv[0]);
 Xc_TRACE(("Full path filename: %s", Globprog_name));

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
 if (xshm_flag == FUZZY)
 {
  xshm_flag = XShmQueryExtension(display)? TRUE : FALSE;
  Xc_TRACE(("XShm extension %s", xshm_flag? "ON" : "OFF"));
  if (strncmp(ServerVendor(display), "Metro", 5) == 0)
   xshm_flag = FALSE;
 }

#ifdef HAVE_XSHAPE
 {
  int event_base, error_base;/* just used as dummy parameters */
  xshape_flag = XShapeQueryExtension(display, &event_base, &error_base);
  Xc_TRACE(("XShape extension %s", xshape_flag? "ON" : "OFF"));
 }
#endif

 GlobImgMng = (c_ImageManager *)NEW(c_ImageManager)(XErrH, display, 
						    xshm_flag);
#else
 GlobImgMng = (c_ImageManager *)NEW(c_ImageManager)(XErrH, display, FALSE);
#endif
  
 GlobDocIcon = (c_DocIcon *)NEW(c_DocIcon)(display);
  
 GlobRemoteServer = (c_RemoteServer *)
  NEW(c_RemoteServer)(display, XtWindow(MainInterface->w_Hidden));
  
 GlobRegHelp = (c_RegisterHelp *)NEW(c_RegisterHelp)(display);
  
 GlobCursor = (c_GCursor *)NEW(c_GCursor)(display);
    
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
 SET_WAIT_MODE_STATIC;
  
 XGetWindowAttributes(display, XtWindow(WIDGET(MainInterface)), &get_attrib);
 set_attrib.event_mask = get_attrib.all_event_masks | StructureNotifyMask;
 XChangeWindowAttributes(display, XtWindow(WIDGET(MainInterface)),
			 CWEventMask, &set_attrib);
 F(GlobColormap).generate_colormap(GlobColormap);
 GlobImgCache = (c_ImageCache *)NEW(c_ImageCache)(GlobColormap);
  
 Xc_mcheck();
 if(!ReadConfigFile())
 {
  if(!ReadUserConfigFile()) QuitApplication();
 }
 else
  ReadUserConfigFile();
  
 F(GlobImgCache).init(GlobImgCache);
  
 GlobPrintParam.printer = GlobPrinterBase;
  
 F(GlobInterface->HelpManager).read_cmd_line(GlobInterface->HelpManager, 
					     argc, argv);
  
 if (GlobInterface->HelpManager->index_exist)
  DisplayIndex(WIDGET(GlobInterface->HelpWIndex), GlobInterface);
  
 F(GlobInterface->HelpWHelp).refresh(GlobInterface->HelpWHelp);
 UNSET_WAIT_MODE_STATIC;
  
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
 
 if (event->type == PropertyNotify)
 {
  if (F(GlobRemoteServer).eventlook(GlobRemoteServer, event))
   return;
 }
  
 if (Interface->HelpManager->help_exist)
 {
  char *title, stitle[64], wtitle[80];
    
  title = HTMLGetTitle(WIDGET(Interface->HelpWHelp));
  if (!title)
   sprintf(stitle, "%.50s (untitled)", Interface->HelpWHelp->url_filename); 
  else
  {
   sprintf(stitle, "%.60s", title); 
   if (strlen(title) > 60) strcat(stitle, "...");
  }
  if (!Xstrcmp(Interface->AppTitle, stitle))
  {
   Arg	argsw[1];
      
   sprintf(wtitle, "XMayday: %s", stitle);
   XtSetArg(argsw[0], XmNtitle, wtitle);
   XtSetValues(Interface->MainInterface->w_Main, argsw, 1);
   strcpy(Interface->AppTitle, stitle);
  } 
 } 
  
 if (Interface->HelpManager->index_exist || 
     Interface->HelpManager->anchor_current)
 {
  SensitivizeRefresh(Interface, True);
 }
 else
 {
  SensitivizeRefresh(Interface, False);
 }
  
 if (Interface->HelpManager->index_exist)
 {
  SensitivizeIndex(Interface, True);
 }
 else
 {
  SensitivizeIndex(Interface, False);
 }
  
 if (!Interface->HelpManager->anchor_current)
 {
  SensitivizePrevious(Interface, False);
  SensitivizeNext(Interface, False);
  SensitivizeICreation(Interface, False);
 }
 else
 {
  if (Interface->HelpManager->anchor_current->Prev)
  {
   SensitivizePrevious(Interface, True);
  }
  else
  {
   SensitivizePrevious(Interface, False);
  }
  if (Interface->HelpManager->anchor_current->Next)
  {
   SensitivizeNext(Interface, True);
  }
  else
  {
   SensitivizeNext(Interface, False);
  }
  SensitivizeICreation(Interface, True);
 }
  
 if (Interface->Next || Interface->Prev)
 {
  SensitivizeClose(Interface, True);
 }
 else
 {
  SensitivizeClose(Interface, False);
 }
  
 if (Interface->IndexExist != Interface->HelpManager->index_exist && 
     Interface->ButtonMaskIndex == FALSE && 
     Interface->HelpManager->index_exist)
  DisplayIndex(WIDGET(Interface->HelpWIndex), Interface);
 Interface->IndexExist = Interface->HelpManager->index_exist;
  
 if (event->type == ClientMessage)
 {
  if ((int)event->xclient.data.l[0] == (int)0xfadafada)
  {
   Xc_TRACE(("Map Notify"));
   F(Interface->HelpWIndex).refresh(Interface->HelpWIndex);
  }
  else if ((int)event->xclient.data.l[0] == (int)0xdeaddddd)
  {
   Xc_TRACE(("Close Window Notify"));
   if (!Interface->Prev && !Interface->Next)
    QuitApplication();
   else
    MenuFileClose(WIDGET(Interface->MainInterface), Interface);
  }
 }
  
 XtDispatchEvent(event);
}

void MenuFileNewXMayday(w, MInterface)
Widget w;
c_Interface *MInterface;
{
 c_Interface *Interface;
  
 Interface = (c_Interface *)COPY(c_Interface)(MInterface);
 if (MInterface->HelpManager->help_exist)
  F(Interface->HelpManager).open_help(Interface->HelpManager, 
				      MInterface->HelpWHelp->url_filename);
 if (MInterface->HelpManager->index_exist)
 {
  F(Interface->HelpManager).open_index(Interface->HelpManager, 
				       MInterface->HelpWIndex->url_filename);
  DisplayIndex(WIDGET(Interface->HelpWIndex), Interface);
 }
}

void MenuFileOpenIndex(w, Interface)
Widget w;
c_Interface *Interface;
{
 char *filename;
  
 Xc_TRACE(("Open Index file"));
  
 SET_WAIT_MODE_STATIC2;
 if((Interface->OpenIndexSelector == NULL) &&
    ((Interface->OpenIndexSelector = CreateOpenIndexSelector
      (Interface->MainInterface->w_Main)) == NULL))
 {
  UNSET_WAIT_MODE;
  return;
 }
 if((filename = F(Interface->OpenIndexSelector).getFile
     (Interface->OpenIndexSelector)) == NULL)
 {
  UNSET_WAIT_MODE;
  return;
 }
  
 F(Interface->HelpManager).open_index(Interface->HelpManager, filename);
  
 if (Interface->ButtonMaskIndex == FALSE)
  DisplayIndex(WIDGET(Interface->HelpWIndex), Interface);
  
 Xc_free(filename);
 UNSET_WAIT_MODE;
}

void MenuFileOpen(w, Interface)
Widget w;
c_Interface *Interface;
{
 char *filename;
  
 Xc_TRACE(("Open File"));
  
 SET_WAIT_MODE_STATIC2;
 if((Interface->OpenFileSelector == NULL) &&
    ((Interface->OpenFileSelector = CreateOpenFileSelector
      (Interface->MainInterface->w_Main)) == NULL))
 {
  UNSET_WAIT_MODE;
  return;
 }
 if((filename = F(Interface->OpenFileSelector).getFile
     (Interface->OpenFileSelector)) == NULL)
 {
  UNSET_WAIT_MODE;
  return;
 }
  
 F(Interface->HelpManager).open_help(Interface->HelpManager, filename);
  
 Xc_free(filename);
 UNSET_WAIT_MODE;
}

/*---------------------------------------------------------------------*/
void MenuFilePrint(w, Interface)
Widget w;
c_Interface *Interface;
{
 c_Box_print *box;
 printer_t   *printer;
 char *ptr;
 int rc, type;
 FILE *handle;
 boolean is_ok;
  
 Xc_TRACE(("Print File Box"));
  
 SET_WAIT_MODE_STATIC2;
 box = (c_Box_print *)NEW(c_Box_print)(Interface->MainInterface->w_Main, 
				       "BoxPrint", &GlobPrintParam);
  
 rc = F(box->Dialog).waitForUnmap(box->Dialog);
 DELETE(c_Box_print)(box);
 if(rc != XcCD_OK) { UNSET_WAIT_MODE; return; }
  
 printer = GlobPrintParam.printer;
 type = (GlobPrintParam.exporttype == 2 ? 
	 2 + GlobPrintParam.fonttype : GlobPrintParam.exporttype);
  
 ptr = HTMLGetText(WIDGET(Interface->HelpWHelp), type, 
		   Interface->HelpWHelp->url_filename + 1 +
		   Xstrnchr(Interface->HelpWHelp->url_filename, '/'), NULL);
 if (ptr == NULL)
 { 
  UNSET_WAIT_MODE;
  return;
 }
  
 if(printer->command == NULL)
 {
  is_ok = TRUE;
  if (Interface->PrintDocumentSelector != NULL)
   F(Interface->PrintDocumentSelector).addPattern(Interface->PrintDocumentSelector, TRUE, (GlobPrintParam.exporttype == 2) ? "*.ps" : "*.txt");
  else
   if((Interface->PrintDocumentSelector = CreatePrintDocumentSelector
       (Interface->MainInterface->w_Main, 
	GlobPrintParam.exporttype == 2 ? "*.ps" : "*.txt")) == NULL)
    is_ok = FALSE;
  if (is_ok)
  {
   char *filename;
      
   filename =
    F(Interface->PrintDocumentSelector).getFile
     (Interface->PrintDocumentSelector);
   if(filename != NULL)
   {
    handle = fopen( filename, "w");
    Xc_free(filename);
   }
   else
   {
    UNSET_WAIT_MODE;
    return;
   }
  }
  else
  {
   UNSET_WAIT_MODE;
   return;
  }
 }
 else 
 {
  handle = popen(printer->command, "w");
 }
  
 if (handle != NULL)
 {
  char *data;
  int	i, packet, last_packet, length;
    
  data = ptr;
  length = strlen(ptr);
  packet = length / 4096;
  last_packet = length % 4096;
    
  for(i = 0; i < packet; i++)
  {
   if (fwrite(data, 1, 4096, handle) != 4096)
   {
    Xc_WARNING(("Broken pipe"));
    last_packet = 0;
    break;
   }
   data += 4096;
  }
  if (last_packet && 
      (int)fwrite(data, 1, last_packet, handle) != last_packet)
  {
   Xc_WARNING(("Broken pipe"));
  }
    
  if (printer->command)
   pclose(handle);
  else
   fclose(handle);
 }
 free(ptr);
  
 UNSET_WAIT_MODE;
}

void MenuFileClose(w, Interface)
Widget w;
c_Interface *Interface;
{
 if (!Interface->Prev && !Interface->Next)
 {
  MenuFileQuit(w, Interface);
 }
 else
 {
  if (Interface == GlobInterface)
   GlobInterface = Interface->Next;
  
  DELETE(c_Interface)(Interface, FALSE);
 }
}

void MenuFileQuit(w, Interface)
Widget w;
c_Interface *Interface;
{
 char		*message;
  
 wMainWindow = WIDGET(Interface->MainInterface);
 message = F(GlobResources).getString(GlobResources, XcR_quitMessage);
 if (Xc_YesNoAlert(message))
  QuitApplication();
 else
  return;
  
}

void QuitApplication()
{
 Xc_HISTORY(("Quit XMayday"));

 WriteUserConfigFile();

 DELETE(c_Interface)(GlobInterface, TRUE);
  
#ifdef  SIGNALH
 DELETE(c_SignalHandler)(SignalH);
#endif
 if (Globprog_name) Xc_free(Globprog_name);
 
 DELETE(c_RegisterHelp)(GlobRegHelp);
 DELETE(c_RemoteServer)(GlobRemoteServer);
 DELETE(c_DocIcon)(GlobDocIcon);
 DELETE(c_Colormap)(GlobColormap);
 DELETE(c_ImageCache)(GlobImgCache);
 DELETE(c_ImageManager)(GlobImgMng);
 DELETE(c_GCursor)(GlobCursor);
 DELETE(c_Resource)(GlobResources);
 DELETE(c_XErrorHandler)(XErrH);
  
#ifndef NMEMDEBUG
 Xc_mdump();
#endif

 Xc_exit(0);
}

void SelectFunction(w, funct)
Widget w;
int funct;
{
 c_Interface *Interface;
  
 Interface = F(GlobInterface).get_by_widget(GlobInterface, w);
 F(Interface->HelpManager).select_function(Interface->HelpManager, funct);
}

void ReloadAll(w, Interface)
Widget w;
c_Interface *Interface;
{
 if (Interface->ButtonMaskIndex)
  F(Interface->HelpWIndex).reload(Interface->HelpWIndex);
 F(Interface->HelpWHelp).reload(Interface->HelpWHelp);
}

void RefreshAll(w, Interface)
Widget w;
c_Interface *Interface;
{
 if (Interface->ButtonMaskIndex)
  F(Interface->HelpWIndex).refresh(Interface->HelpWIndex);
 F(Interface->HelpWHelp).refresh(Interface->HelpWHelp);
}

void DisplayBottomBar(w, Interface)
Widget w;
c_Interface *Interface;
{
 if (Interface->ButtonMaskBottomBar == TRUE)
 {
  XtUnmanageChild(WIDGET(Interface->BottomBar));
  F(Interface->MainInterface).attach_bottom
   (Interface->MainInterface, Interface->fPaned, NULL);
      
  F(Interface->Menu[MENU_DISPLAY]).ToggleManage
   (Interface->Menu[MENU_DISPLAY], MD_MASK_BOTTOMBAR,
    MD_UNMASK_BOTTOMBAR);
  Interface->ButtonMaskBottomBar = FALSE;
 }
 else
 {
  XtManageChild(WIDGET(Interface->BottomBar));
  F(Interface->MainInterface).attach_bottom
   (Interface->MainInterface, Interface->fPaned, 
    WIDGET(Interface->BottomBar));

  F(Interface->Menu[MENU_DISPLAY]).ToggleManage
   (Interface->Menu[MENU_DISPLAY], MD_UNMASK_BOTTOMBAR,
    MD_MASK_BOTTOMBAR);
  Interface->ButtonMaskBottomBar = TRUE;
 }
}

void DisplayIndex(w, Interface)
Widget w;
c_Interface *Interface;
{
 if (Interface->ButtonMaskIndex == TRUE)
 {
  XtUnmanageChild(WIDGET(Interface->HelpWIndex));
  F(Interface->Menu[MENU_DISPLAY]).ToggleManage
   (Interface->Menu[MENU_DISPLAY], MD_MASK_INDEX, MD_UNMASK_INDEX);
  Interface->ButtonMaskIndex = FALSE;
 }
 else
 {
  XtManageChild(WIDGET(Interface->HelpWIndex));
  F(Interface->Menu[MENU_DISPLAY]).ToggleManage
   (Interface->Menu[MENU_DISPLAY], MD_UNMASK_INDEX, MD_MASK_INDEX);
  Interface->ButtonMaskIndex = TRUE;
  SendRefreshRequest(WIDGET(Interface->MainInterface));
 }
}

static void SendRefreshRequest(widget)
Widget widget;
{
 XEvent event;
  
 event.type = ClientMessage;
 event.xclient.send_event = True;
 event.xclient.display = XtDisplay(widget);
 event.xclient.window = XtWindow(widget);
 event.xclient.format = 32;
 event.xclient.data.l[0] = 0xfadafada;
 XSendEvent(XtDisplay(widget), XtWindow(widget), 
	    True, 0, &event);
}

/*---------------------------------------------------------------------*/
void MenuHelpCache(w, Interface)
Widget	w;
c_Interface	*Interface;
{
 c_Box_cache *box;
  
 SET_WAIT_MODE_STATIC2;
 box = (c_Box_cache *)NEW(c_Box_cache)(Interface->MainInterface->w_Main,
				       "BoxCache");
 F(box->Dialog).waitForUnmap(box->Dialog);
 DELETE(c_Box_cache)(box);
 UNSET_WAIT_MODE;
}
/*---------------------------------------------------------------------*/
void MenuHelpAbout(w, Interface)
Widget	w;
c_Interface	*Interface;
{
 c_Box_about *box;

 SET_WAIT_MODE_STATIC2;
 box = (c_Box_about *)NEW(c_Box_about)(Interface->MainInterface->w_Main,
				       "BoxAbout");
 F(box->Dialog).waitForUnmap(box->Dialog);
 DELETE(c_Box_about)(box);
 UNSET_WAIT_MODE;
}

void MenuHelpCallHelp(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 F(GlobRegHelp).call_help_for_widget(GlobRegHelp, w);
}

void MenuHelpTrackHelp(w, Interface)
Widget	w;
c_Interface	*Interface;
{
 F(GlobRegHelp).tracking_help(GlobRegHelp, Interface->MainInterface->w_Main);
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
** Sensitivizes                                                      ** 
** ----------------------------------------------------------------- */

void SensitivizePrevious(Interface, flag)
c_Interface *Interface;
Boolean flag;
{
 if (flag == (Boolean)Interface->PreviousAlreadyGrised)
 {
  Interface->PreviousAlreadyGrised ^= TRUE;
  XtSetSensitive(WIDGET(Interface->Icon[I_NAVIGATE_PREV]), flag);
  XtSetSensitive(WIDGET(Interface->Icon[I_NAVIGATE_INDEX]), flag);
  F(Interface->Menu[MENU_NAVIGATE]).ButtonFlag
   (Interface->Menu[MENU_NAVIGATE], MN_GOTO_PREV, flag);
  F(Interface->Menu[MENU_NAVIGATE]).ButtonFlag
   (Interface->Menu[MENU_NAVIGATE], MN_GOTO_INDEX, flag);
 }
}

void SensitivizeNext(Interface, flag)
c_Interface *Interface;
Boolean flag;
{
 if (flag == (Boolean)Interface->NextAlreadyGrised)
 {
  Interface->NextAlreadyGrised ^= TRUE;
  XtSetSensitive(WIDGET(Interface->Icon[I_NAVIGATE_NEXT]), flag);
  F(Interface->Menu[MENU_NAVIGATE]).ButtonFlag
   (Interface->Menu[MENU_NAVIGATE], MN_GOTO_NEXT, flag);
 }
}

void SensitivizeRefresh(Interface, flag)
c_Interface *Interface;
Boolean flag;
{
 if (flag == (Boolean)Interface->RefreshAlreadyGrised)
 {
  Interface->RefreshAlreadyGrised ^= TRUE;
  F(Interface->Menu[MENU_DISPLAY]).ButtonFlag
   (Interface->Menu[MENU_DISPLAY], MD_REDISPLAY, flag);
  F(Interface->Menu[MENU_DISPLAY]).ButtonFlag
   (Interface->Menu[MENU_DISPLAY], MD_RELOAD, flag);
 }
}

void SensitivizeIndex(Interface, flag)
c_Interface *Interface;
Boolean flag;
{
 if (flag == (Boolean)Interface->IndexAlreadyGrised)
 {
  Interface->IndexAlreadyGrised ^= TRUE;
  F(Interface->Menu[MENU_DISPLAY]).ButtonFlag
   (Interface->Menu[MENU_DISPLAY], MD_UNMASK_INDEX, flag);
 }
}

void SensitivizeICreation(Interface, flag)
c_Interface *Interface;
Boolean flag;
{
 if (flag == (Boolean)Interface->ICreationAlreadyGrised)
 {
  Interface->ICreationAlreadyGrised ^= TRUE;
  F(Interface->Menu[MENU_DISPLAY]).ButtonFlag
   (Interface->Menu[MENU_DISPLAY], MD_COPY_INDEX, flag);
  XtSetSensitive(WIDGET(Interface->Icon[I_MOVEI]), flag);
  F(Interface->Menu[MENU_FILE]).ButtonFlag
   (Interface->Menu[MENU_FILE], MF_PRINT, flag);
  XtSetSensitive(WIDGET(Interface->Icon[I_PRINT]), flag);
 }
}


void SensitivizeClose(Interface, flag)
c_Interface *Interface;
Boolean flag;
{
 if (flag == (Boolean)Interface->CloseAlreadyGrised)
 {
  Interface->CloseAlreadyGrised ^= TRUE;
  F(Interface->Menu[MENU_FILE]).ButtonFlag
   (Interface->Menu[MENU_FILE], MF_CLOSE, flag);
 }
}











