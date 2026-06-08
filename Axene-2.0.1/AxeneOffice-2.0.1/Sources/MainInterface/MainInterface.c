/*
** MainInterface.c for AxeneOffice in MainInterface/
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
** Started on  Mon Jul 17 19:22:50 1995 Antoine Buat
** Last update Tue Feb  1 11:19:54 2000 Emmanuel Paris
*/

#include "MainInterface.h"
#include "Actions.h"
#ifdef EDITRES_COMPATIBLE
#include <X11/Xmu/Editres.h>
#endif
#include "Xpm.h"
#include "osname.h"
#include "readkey.h"
#include "version.h"
#include "CommandLine.h"
#include "../Pixmaps/axeneoffice_icon.zpm"

extern void XcFlushEvents ___PROTO((Widget w, 
				    void (*func) ___PROTO((XEvent *))));
extern Widget wMainWindow;

static void *cons_MainInterface();
static void dest_MainInterface();
static void *copy_MainInterface();
static void attach_top_MainInterface();
static void attach_bottom_MainInterface();
static void attach_left_MainInterface();
static void attach_right_MainInterface();
static void set_icon_MainInterface();
static void create_TopLevel ___PROTO((c_MainInterface *this,
				      c_ManageWidget *ManageW));
static void map_TopLevel ___PROTO((c_MainInterface *this));
static void unmap_TopLevel ___PROTO((c_MainInterface *this));
static void reopen_display ___PROTO((c_MainInterface *this));

static void shellMap ___PROTO((c_MainInterface *this, Widget w));

#ifdef PREVENT_GADGET_WARNING
static void warnings_null();
#endif

sf_MainInterface fc_MainInterface =
{
 cons_MainInterface,
 dest_MainInterface,
 copy_MainInterface,

 create_TopLevel,
 map_TopLevel,
 unmap_TopLevel,

 attach_top_MainInterface,
 attach_bottom_MainInterface,
 attach_left_MainInterface,
 attach_right_MainInterface,
 set_icon_MainInterface,
 reopen_display
};

static char *fallbacks[] = 
{ 
#include "../Global/AxeneOffice.ad.h"
 NULL};

static char cryptString[] = "i(:^9Qb^9Qb";
static char appName[] = { 'A' - 'i', 'x' - '(', 'e' - ':',
			   'n' - '^', 'e' - '9', 'O' - 'Q',  'f' - 'b', 
			   'f' - '^', 'i' - '9', 'c' - 'Q',  'e' - 'b', '\0'};

static char *helpString[] = { "--help", "-help", "-h", NULL};
static char *versionString[] = { "--version", "-version", "-v", NULL};

static XrmOptionDescRec options[] = 
{
 {"-font", "*fontList", XrmoptionSepArg, NULL},
 {"-iconic", "*TopLevelShell.iconic", XrmoptionNoArg, "True"},
 {"-title", "*title", XrmoptionSepArg, NULL},
 {"-geom", "*TopLevelShell.geometry", XrmoptionSepArg, NULL},
 {"-geometry", "*TopLevelShell.geometry", XrmoptionSepArg, NULL},
 {"-keepFreeColor", "*keepFreeColor", XrmoptionSepArg, NULL},
 {"+ownColormap", "*ownColormapMsg", XrmoptionNoArg, "on"},
 {"-ownColormap", "*ownColormapMsg", XrmoptionNoArg, "off"},
} ;

XtAppContext	  gapp;


static void *cons_MainInterface(argc, argv)
int            argc;
char           **argv;
{
 c_MainInterface *This;
 char		  c;
 Display	  *display;
 boolean	  display_open = FALSE;
 XtAppContext	  app;
 int		  i, j;


 for(i = 0; i < (int)strlen(appName) ; i++)
 {
  appName[i] += cryptString[i];
 }

 for(i = 1; i < argc; i++)
 {
  j = 0;
  while(helpString[j])
  {
   if (Xstrcmp(helpString[j], argv[i]))
   {
#ifdef PL	      
    printf("Axene Office version %d.%d.%d (%s), %s\n", 
	   MAJOR, MINOR, PL, Xc_SYSTEM_NAME, copyrightDisplay);
#else
    printf("Axene Office version %d.%d (%s), %s\n", 
	   MAJOR, MINOR, Xc_SYSTEM_NAME, copyrightDisplay);
#endif
    printf(helpDisplay);
    exit(0);
   }
   j++;
  }
  j = 0;
  while(versionString[j])
  {
   if (Xstrcmp(versionString[j], argv[i]))
   {
#ifdef PL	      
    printf("Axene Office version %d.%d.%d (%s), %s\n\n",
	   MAJOR, MINOR, PL, Xc_SYSTEM_NAME, copyrightDisplay);
#else
    printf("Axene Office version %d.%d (%s), %s\n\n",
	   MAJOR, MINOR, Xc_SYSTEM_NAME, copyrightDisplay);
#endif
    exit(0);
   }
   j++;
  }
 }


 Xc_HISTORY(("constructor"));
 if ((This = (c_MainInterface *)Xc_malloc("MainInterface",
					  sizeof(c_MainInterface))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f = &fc_MainInterface;
  
 XtToolkitInitialize();
 gapp = app = XtCreateApplicationContext();
 XtAppSetFallbackResources(app, fallbacks);
 XtAppAddActions(app, actions, NB_ACTIONS);
  
 This->display_name = NULL;
 display = XtOpenDisplay(app, NULL, NULL, appName,
			 options, XtNumber(options),&argc, argv);
  
 if (display == NULL)
 {
  Xc_WARNING(("Can't open display (maybe no environment variable $DISPLAY set or no command line argument -display given)."));
  printf("Try to open display `:0.0' (y,n,q)? ");
  fflush(stdout);
  do 
  {
   c = xc_read_key_without_echo();
   if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
  } while(c!='y' && c!='n' && c!='q');
  putchar(c);
  putchar('\n');
      
  if (c == 'q')
   Xc_FATAL(("Exiting now. Try to set a valid display name."));
      
  if (c == 'y')
  {
   display = XtOpenDisplay(app, ":0.0", NULL, appName, NULL,
			   0, &argc, argv);
   if (display == NULL)
   {
    Xc_WARNING(("Can not open display `:0.0' (maybe not on a graphic workstation)"));
   }
   else
   {
    display_open = TRUE;
    This->display_name = Xc_strdup("dn", ":0.0");
   }
  }
      
  if (!display_open)
  {
   char	name[200];
   printf("Enter a display name or nothing to exit: ");
   fflush(stdout);
   fgets(name, 200, stdin);
   name[strlen(name) - 1] = '\0';
   if (name[0] != '\0')
   {
    display = XtOpenDisplay(app, name, NULL, appName,
			    NULL, 0, &argc, argv);
    if (display == NULL)
    {
     Xc_FATAL(("Can not open display `%s' (maybe a wrong display name or you don't have rights on this display).", name));
    }
    else
    {
     display_open = TRUE;
     This->display_name = Xc_strdup("dn", name);
    }
   }
   if (!display_open)
    Xc_FATAL(("Exiting now. Try to set a valid display name."));
  }
 }
 if (!This->display_name)
 {
  This->display_name = Xc_strdup("dn", DisplayString(display));
  Xc_TRACE(("%s -display %s", appName, This->display_name));
 }
  
 /*--- Hidden shell ---*/
 This->w_Hidden = XtVaAppCreateShell(NULL, appName, 
				     applicationShellWidgetClass, display, 
				     XmNkeyboardFocusPolicy, XmEXPLICIT,
				     XmNmappedWhenManaged, False,
				     XmNwidth, 1,
				     XmNheight, 1,
				     XmNx, DisplayWidth(display, 0) >> 1,
				     XmNy, DisplayHeight(display, 0) >> 1,
				     NULL);
  
#ifdef EDITRES_COMPATIBLE
 XtAddEventHandler(This->w_Hidden, (EventMask)0, True,
		   _XEditResCheckMessages, NULL);
#endif
  
#ifdef PREVENT_GADGET_WARNING
 XtAppSetErrorHandler(gapp, (XtErrorHandler)warnings_null);
 XtAppSetErrorMsgHandler(gapp, (XtErrorMsgHandler)warnings_null);
 XtAppSetWarningHandler(gapp, (XtErrorHandler)warnings_null);
 XtAppSetWarningMsgHandler(gapp, (XtErrorMsgHandler)warnings_null);
#endif

 XtRealizeWidget(This->w_Hidden);
 wMainWindow = This->w_Hidden;
 Xc_TRACE(("Hidden shell"));
  
 Xc_TRACE(("Object MainInterface build"));
 return This;
}


static void dest_MainInterface(this)
c_MainInterface *this;
{
 if (this->w_Main != (Widget)0)
  XtDestroyWidget(this->w_Main);
 if (this->w_This != (Widget)0)
  XtDestroyWidget(this->w_This);
 if (this->display_name)
  Xc_free(this->display_name);
 Xc_free(this);
 Xc_TRACE(("Object MainInterface destroyed"));
}

static void *copy_MainInterface(this)
c_MainInterface *this;
{
 c_MainInterface	*copy;
  
 if ((copy = (c_MainInterface *)Xc_malloc("MainInterface",
					  sizeof(c_MainInterface))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 memset(copy, 0, sizeof(c_MainInterface));
 copy->f = &fc_MainInterface;
 copy->w_Hidden = this->w_Hidden; 
  
 copy->display_name = Xc_strdup("dn", this->display_name);
 return copy;
}

static void create_TopLevel(this, ManageW)
c_MainInterface *this;
c_ManageWidget *ManageW;
{
 Arg             argsw[10];
 int             nargs;

 /*--- Application shell ---*/
 nargs = 0;
 XtSetArg(argsw[nargs], XmNtransient, True); nargs++;
 XtSetArg(argsw[nargs], XmNoverrideRedirect, False); nargs++;
 XtSetArg(argsw[nargs], XmNallowShellResize, True); nargs++;

 this->w_Main = XtCreatePopupShell(appName, topLevelShellWidgetClass,
				   this->w_Hidden, argsw, nargs);
  
#ifdef EDITRES_COMPATIBLE
 XtAddEventHandler(this->w_Main, (EventMask)0, TRUE,
		   _XEditResCheckMessages, NULL);
#endif

 /*
   nargs = 0;
   XtSetArg(argsw[nargs], XmNresizePolicy, XmRESIZE_ANY); nargs++;
   
   this->w_This = XmCreateForm(this->w_Main, "Application", argsw, nargs);
   F(ManageW).Add_child(ManageW, this->w_This, this->w_Main);
   */

 this->w_This = this->w_Main;

 /* XtVaSetValues(this->w_Main, 
    XmNwidth, width, 
    XmNminWidth, width, 
    XmNmaxWidth, width,
    XmNheight, height, 
    XmNminHeight, height, 
    XmNmaxHeight, height, 
    NULL);
    */

 Xc_TRACE(("Main shell"));
}

static void map_TopLevel(this)
c_MainInterface *this;
{
 XtPopup(this->w_Main, XtGrabNone);
 shellMap(this, this->w_Main);
 /*  wMainWindow = this->w_Main; */
 XcFlushEvents(this->w_Main, (void (*)())XtDispatchEvent);
 XmUpdateDisplay(this->w_Hidden);
}

static void unmap_TopLevel(this)
c_MainInterface *this;
{
 XtPopdown(this->w_Main);
}

static void attach_top_MainInterface(This, son, otherson)
c_MainInterface	*This;
Widget		son;
Widget		otherson;
{  
 Arg	argsw[4];
 int	nargs;
  
 nargs = 0;
 if (otherson == NULL)
 {
  XtSetArg(argsw[nargs], XmNtopAttachment, XmATTACH_FORM);
  nargs++;
 }
 else
 {
  XtSetArg(argsw[nargs], XmNtopAttachment, XmATTACH_WIDGET);
  nargs++;
  XtSetArg(argsw[nargs], XmNtopWidget, otherson);
  nargs++;
 }
 XtSetValues(son, argsw, nargs);
}

static void attach_bottom_MainInterface(This, son, otherson)
c_MainInterface	*This;
Widget		son;
Widget		otherson;
{
 Arg	argsw[4];
 int	nargs;

 nargs = 0;
 if (otherson == NULL)
 {
  XtSetArg(argsw[nargs], XmNbottomAttachment, XmATTACH_FORM);
  nargs++;
 }
 else
 {
  XtSetArg(argsw[nargs], XmNbottomAttachment, XmATTACH_WIDGET);
  nargs++;
  XtSetArg(argsw[nargs], XmNbottomWidget, otherson);
  nargs++;
 }
 XtSetValues(son, argsw, nargs);
}

static void attach_left_MainInterface(This, son, otherson)
c_MainInterface	*This;
Widget		son;
Widget		otherson;
{
 Arg	argsw[4];
 int	nargs;

 nargs = 0;
 if (otherson == NULL)
 {
  XtSetArg(argsw[nargs], XmNleftAttachment, XmATTACH_FORM);
  nargs++;
 }
 else
 {
  XtSetArg(argsw[nargs], XmNleftAttachment, XmATTACH_WIDGET);
  nargs++;
  XtSetArg(argsw[nargs], XmNleftWidget, otherson);
  nargs++;
 }
 XtSetValues(son, argsw, nargs);
}

static void attach_right_MainInterface(This, son, otherson)
c_MainInterface	*This;
Widget son;
Widget otherson;
{
 Arg argsw[4];
 int nargs;

 nargs = 0;
 if (otherson == NULL)
 {
  XtSetArg(argsw[nargs], XmNrightAttachment, XmATTACH_FORM);
  nargs++;
 }
 else
 {
  XtSetArg(argsw[nargs], XmNrightAttachment, XmATTACH_WIDGET);
  nargs++;
  XtSetArg(argsw[nargs], XmNrightWidget, otherson);
  nargs++;
 }
 XtSetValues(son, argsw, nargs);
}

static void set_icon_MainInterface(This)
c_MainInterface	*This;
{
 XImage	*xsource;
 XImage	*xmask;
 Pixmap	psource;
 Pixmap	pmask;
 char		**image;
 Display	*display;
 Window	window;
 GC		gc;
 int		width;
 int		height = 0;
 int		nb_size;
 int		code_ret;
 int		i;
 XIconSize	*size_icon;
 XWMHints	infowmh;

 display = XtDisplay(This->w_Main);
 window = XtWindow(This->w_Main);
 image = axeneoffice_icon_xpm;
 code_ret = XGetIconSizes(display, DefaultRootWindow(display),
			  &size_icon, &nb_size);
 if (!code_ret || !nb_size)
 {
  width = 40;
  height = 40;
 }
 else
 {
  width = 0;
  for(i = 0 ; i < nb_size ; i++)
  {
   if (size_icon->max_width > width)
   {
    width = size_icon->max_width;
    height = size_icon->max_height;
   }
  }
 }
 XPMload(display, image, &xsource, &xmask, TRUE);
 width = MIN(width, xsource->width);
 height = MIN(height, xsource->height);
 psource = XCreatePixmap(display, 
			 DefaultRootWindow(display),
			 width,height, 1);
 gc = XCreateGC(display, psource, 0, 0);
 XPutImage(display, psource, gc, xsource, 0, 0, 0, 0, width, height);
 Xc_free(xsource->data);
 XFree((char*)xsource);
 pmask = XCreatePixmap(display, 
		       DefaultRootWindow(display),
		       width, height, 1);
 XPutImage(display, pmask, gc, xmask, 0, 0, 0, 0, width, height);
 Xc_free(xmask->data);
 XFree((char *)xmask);
 XFreeGC(display,gc);
 infowmh.icon_pixmap = psource;
 infowmh.icon_mask = pmask;
 infowmh.flags = IconPixmapHint | IconMaskHint;
 XSetWMHints(display, window, &infowmh);
 XSetIconName(display, window, appName);
}

static void reopen_display(This)
c_MainInterface *This;
{
 Display	  *display;
 XtAppContext	  app;
 int		  null = 0;
  
 /*  XtDestroyApplicationContext(gapp); */
  
 XtToolkitInitialize();
 app = XtCreateApplicationContext();
 XtAppSetFallbackResources(app, fallbacks);
 XtAppAddActions(app, actions, NB_ACTIONS);
  
 display = XtOpenDisplay(app, This->display_name, NULL, appName,
			 NULL, 0, &null, NULL);
 if (display == NULL)
 {
  Xc_FATAL(("Can't open display `%s' (maybe a wrong display name or you don't have rights on this display).", This->display_name));
 }
  
 /*--- Hidden shell ---*/
 This->w_Hidden = XtVaAppCreateShell(NULL, appName, 
				     applicationShellWidgetClass, display, 
				     XmNkeyboardFocusPolicy, XmEXPLICIT,
				     XmNmappedWhenManaged, False,
				     XmNwidth, 1,
				     XmNheight, 1,
				     XmNx, DisplayWidth(display, 0) >> 1,
				     XmNy, DisplayHeight(display, 0) >> 1,
				     NULL);
  
#ifdef EDITRES_COMPATIBLE
 XtAddEventHandler(This->w_Hidden, (EventMask)0, True,
		   _XEditResCheckMessages, NULL);
#endif
  
 XtRealizeWidget(This->w_Hidden);
 wMainWindow =  This->w_This = This->w_Main = This->w_Hidden; 
}

/*--- Internal functions ---*/

static void shellMap(this, w)
c_MainInterface *this;
Widget w;
{
 XtAppContext app;
  
 app = XtWidgetToApplicationContext(w);
 XSync(XtDisplay(w), 0);
 while(42)
 {
  XEvent event;
      
  XtAppNextEvent(app, &event);
  if((event.xany.type == MapNotify) &&
     (event.xany.window == XtWindow(w))) break;
  XtDispatchEvent(&event);
 }
}

#ifdef PREVENT_GADGET_WARNING
/* ----------------------------------------------------------------- ** 
** warnings_null - to avoid a motif warning. horrible hack..         ** 
** ----------------------------------------------------------------- */
static void warnings_null(name, type, class, sdefault, params, num_params)
String name;
String type;
String class;
String sdefault;
String* params;
Cardinal* num_params;
{
 Xc_TRACE(("Warnings:\nName: %s\nType: %s\nClass: %s", 
           (char *)name, (char *)type, (char *)class));
}
#endif








