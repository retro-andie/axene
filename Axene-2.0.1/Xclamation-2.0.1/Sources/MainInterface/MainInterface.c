/*
** MainInterface.c for Xclamation, XQuad and XAllWrite in MainInterface/
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
** Started on  Mon Jul 17 19:22:50 1995 Antoine Buat
** Last update Fri Jan 28 10:12:30 2000 Emmanuel Paris
*/

#include "MainInterface.h"
#include "Actions.h"
#include "Translations.h"
#ifdef EDITRES_COMPATIBLE
#include <X11/Xmu/Editres.h>
#endif
#include "TimeIndicator.h"
#include "Xpm.h"
#include "osname.h"
#include "readkey.h"
#include "version.h"
#include "builtdate.h"
#include "CommandLine.h"
#include "Readkey.h"
#include <time.h>
#include <netdb.h>
#include <sys/utsname.h>
#ifdef Xc_XCLAMATION
#include "../Pixmaps/xcl_icon.zpm"
#endif
#ifdef Xc_XQUAD
#include "../Pixmaps/xquad_icon.zpm"
#endif
#ifdef Xc_XALLWRITE
#include "../Pixmaps/xallwrite_icon.zpm"
#endif

#define STARTUP_WIDTH 500
#define STARTUP_HEIGHT 135

extern void XcFlushEvents ___PROTO((Widget w, 
				    void (*func) ___PROTO((XEvent *))));
extern Widget wMainWindow;
extern boolean Xc_KEY_TRACE;
#ifdef HAVE_XSHM
extern boolean xshm_flag;
#endif
extern boolean	want_xbug_line;
extern boolean	want_xbug_point;

static void *cons_MainInterface();
static void dest_MainInterface();
static void *copy_MainInterface();
static void attach_top_MainInterface();
static void attach_bottom_MainInterface();
static void attach_left_MainInterface();
static void attach_right_MainInterface();
static void set_icon_MainInterface();
static void create_and_map_StartUp ___PROTO((c_MainInterface *this,
					     c_TimeIndicator *TimeIndicator));
static void unmap_StartUp ___PROTO((c_MainInterface *this));
static void create_TopLevel ___PROTO((c_MainInterface *this,
				      c_ManageWidget *ManageW));
static void map_TopLevel ___PROTO((c_MainInterface *this));
static void unmap_TopLevel ___PROTO((c_MainInterface *this));
static void reopen_display ___PROTO((c_MainInterface *this));

static void shellMap ___PROTO((c_MainInterface *this, Widget w));
static void print_copyright();
static void set_title ___PROTO((c_MainInterface *this));
static void set_loadingMessage ___PROTO((c_MainInterface *this,
					 char *message));

#ifdef PREVENT_GADGET_WARNING
static void warnings_null();
#endif

sf_MainInterface fc_MainInterface =
{
 cons_MainInterface,
 dest_MainInterface,
 copy_MainInterface,
 
 create_and_map_StartUp,
 unmap_StartUp,
 
 create_TopLevel,
 map_TopLevel,
 unmap_TopLevel,
 
 attach_top_MainInterface,
 attach_bottom_MainInterface,
 attach_left_MainInterface,
 attach_right_MainInterface,
 set_icon_MainInterface,
 reopen_display,
 
 set_title,
 set_loadingMessage
};

#ifdef Xc_XCLAMATION
static char *fallbacks[] =
{ 
#include "../Global/Xclamation.ad.h"
 NULL
};

static char cryptString[] = "\ti}#8 .|Qb";
static char appName[] =
{ 'X' - '\t', 'c' - 'i', 'l' - '}', 'a' - '#', 'm' - '8', 'a' - ' ',
   't' - '.', 'i' - '|', 'o' - 'Q', 'n' - 'b', '\0'};
#endif

#ifdef Xc_XQUAD
static char *fallbacks[] =
{ 
#include "../Global/XQuad.ad.h"
 NULL
};

static char cryptString[] = "iM:^9";
static char appName[] =
{ 'X' - 'i', 'Q' - 'M', 'u' - ':', 'a' - '^', 'd' - '9', '\0'};
#endif

#ifdef Xc_XALLWRITE
static char *fallbacks[] =
{
#include "../Global/XAllWrite.ad.h"
 NULL
};

static char cryptString[] = "f\t:8%vZ!}";
static char appName[] = { 'X' - 'f', 'A' - '\t', 'l' - ':', 
			   'l' - '8', 'W' - '%', 'r' - 'v', 'i' - 'Z',
			   't' - '!',  'e' - '}', '\0'};
#endif

static char *helpString[] = { "--help", "-help", "-h", NULL};
static char *versionString[] = { "--version", "-version", "-v", NULL};
static char *checkString[] = { "--check", "-check", "-ck", NULL};
static char *sysinfoString[] = { "--sysinfo", "-sys", "-s", NULL};
static char *xbuglineString[] = { "--xbugline", "-xbugline", "-xbl", NULL};
static char *xbugpointString[] = { "--xbugpoint", "-xbugpoint", "-xbp", NULL};
static char *nosharedpixmapString[] = { "--nosharedpixmap", "-noshpxm", 
					 "-nsp", NULL};

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
 {"+opaqueResize", "*opaqueResizeMsg", XrmoptionNoArg, "on"},
 {"-opaqueResize", "*opaqueResizeMsg", XrmoptionNoArg, "off"},
 {"-opaqueMove", "*opaqueMoveMsg", XrmoptionSepArg, NULL},
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

 want_xbug_line = want_xbug_point = FUZZY;
#ifdef HAVE_XSHM
 xshm_flag = FUZZY;
#endif

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
    print_copyright(FALSE);
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
    print_copyright(FALSE);
    putchar('\n');
    exit(0);
   }
   j++;
  }

  j = 0;  
  while(checkString[j])
  {
   if (Xstrcmp(checkString[j], argv[i]))
   {
    Xc_KEY_TRACE = TRUE;
    print_copyright(TRUE);

    /*--- Read key ---*/
    {
     char		*vkey;
     int		client;
#ifdef Xc_XCLAMATION
     client = 0;
#endif
#ifdef Xc_XQUAD
     client = 1;
#endif
#ifdef Xc_XALLWRITE
     client = 2;
#endif

     vkey = NULL;
     read_key_file(&vkey);
     bzero(&gkinfo, sizeof(gkinfo));
     if (vkey)
     {
      if (get_info_from_key(&gkinfo, vkey)!=-1)
      {
       printf("This software ............. : #%d", client);
       if ((gkinfo.soft == 3) || (client == gkinfo.soft))
       {
	time_t		real_time;
	struct tm	*tmKey;
	struct tm	tmBuild;
	time_t		ttKey;

	printf("   [OK]\n");

	tmBuild.tm_sec = 0;
	tmBuild.tm_min = 0;
	tmBuild.tm_hour = 0;
	tmBuild.tm_mday = MAKE_IDATE_DAY;
	tmBuild.tm_mon = MAKE_IDATE_MONTH;
	tmBuild.tm_year = ((MAKE_IDATE_YEAR < 80) ? 
			   (MAKE_IDATE_YEAR + 100) : (MAKE_IDATE_YEAR));
	tmBuild.tm_wday = 0;
	tmBuild.tm_yday = 0;
	tmBuild.tm_isdst = 0;

	tmKey = localtime(&gkinfo.date); 
	/*-- add two year of free upgrade --*/
	tmKey->tm_year += 2;
	ttKey = mktime(tmKey);

	/* TODO get build time */
	real_time = mktime(&tmBuild);
	if (gkinfo.type == 0)
	 printf("This date ................. : %lx < %lx", real_time, ttKey);
	else
	 printf("This date ................. : %lx < infinite",	real_time);

	if (gkinfo.type == 0 && ttKey < real_time)
	{
	 printf("   [ERROR]\n");
	}
	else
	{
	 printf("   [OK]\n");
	 printf("--------------------------------------------------------------------------\nEVERYTHING SEEMS TO BE GOOD - END OF KEY CHECK\nBe Happy\n\n");
	 Xc_free(vkey);
	 exit(0);
	}
       }
       else
	printf("   [ERROR]\n");
      }
      else
       printf("   [ERROR]\n");
     }
     printf("--------------------------------------------------------------------------\nThe key is not Valid, please Email this log to support@axene.org\n\n");
    }
    exit(0);
   }
   j++;
  }

  j = 0;  
  while(sysinfoString[j])
  {
   if (Xstrcmp(sysinfoString[j], argv[i]))
   {
    char		pszTmp[50];
    char		pszUname[100];
    int			hostname_ret;
    int			display_ret;
    unsigned char	**test = NULL;
    char		*pszDisplay;
    struct hostent	*s_rhost = NULL;
    struct hostent	*r_rhost = NULL;
    boolean		bLocalhost = FALSE;
    struct utsname	utsBuffer;

    print_copyright(FALSE);

    display_ret = 1;
    hostname_ret = gethostname(pszTmp, 50);
    r_rhost = Xc_gethostbyname(pszTmp);

    if((pszDisplay = getenv("DISPLAY")) == NULL) 
     display_ret = 0;

    if (display_ret)
    {
     if (display_to_host(&pszDisplay) == -1)
     {
      display_ret = 0;
     }
  
     s_rhost = Xc_gethostbyname(pszDisplay);
     if (s_rhost)
     {
      Xc_TRACE(("Display: %s.", pszDisplay));
      test = (unsigned char **)s_rhost->h_addr_list;
      Xc_TRACE(("addr: %d.%d.%d.%d", 
		(*test)[0],(*test)[1],(*test)[2],(*test)[3]));
     }
     else
     {
      display_ret = 0;
     }
     Xc_free(pszDisplay);
    }

    if (uname(&utsBuffer))
    {
     sprintf(pszUname, "Unknown");
    }
    else
     sprintf(pszUname, "%s %s %s %s %s", utsBuffer.sysname,
	     utsBuffer.nodename, utsBuffer.release,
	     utsBuffer.version, utsBuffer.machine);
    
    if (!hostname_ret)
    {
     if(r_rhost)
      printf("Hostname ........:  \"%s\"\n", pszTmp);
     else
     {
      printf("TROUBLE: The actual Hostname is \"%s\" but it can't be \
resolved.\n         You can add \"%s\" to the /etc/hosts file or we'll use\n\
         hardcoded \"localhost\".\n", pszTmp, pszTmp);
      bLocalhost = TRUE;
     }
    }

    if (bLocalhost)
    {
     r_rhost = Xc_gethostbyname("localhost");
     if (r_rhost)
     {
      printf("Hostname ........:  \"localhost\"\n");
     }
     else 
     {
      printf("TROUBLE: Can't resolve \"localhost\" !!! You definitively should setup\n         your IP network. Add \"127.0.0.1 localhost\" in your /etc/hosts file.\n");
      exit(0);
     }
    }
     
    printf("Uname ...........:  %s\n", pszUname);
    
    if (display_ret)
     printf("Display .........:  %d.%d.%d.%d\n", 
	    (*test)[0],(*test)[1],(*test)[2],(*test)[3]);
    else
     printf("Use Display = Hostname\n");
    

    exit(0);
   }
   j++;
  }
  
  j = 0;
  while(xbugpointString[j])
  {
   if (Xstrcmp(xbugpointString[j] + 1 + (j == 0), argv[i] + 1 + (j == 0)))
   {
    if (*argv[i] == '-' && (j != 0 || *(argv[i] + 1) == '-'))
     want_xbug_point = FALSE;
    else if (*argv[i] == '+' && (j != 0 || *(argv[i] + 1) == '+'))
     want_xbug_point = TRUE;
    else
    {
     j++; continue;
    }
    
    if (i+1 < argc)
    {
     j = i;
     while (j < argc - 1)
     {
      argv[j] = argv[j+1];
      j++;
     }
    }
    argc--;
    argv[argc] = NULL;
    
    i--;
    break;
   }
   j++;
  }

  j = 0;
  while(xbuglineString[j])
  {
   if (Xstrcmp(xbugpointString[j] + 1 + (j == 0), argv[i] + 1 + (j == 0)))
   {
    if (*argv[i] == '-' && (j != 0 || *(argv[i] + 1) == '-'))
     want_xbug_line = FALSE;
    else if (*argv[i] == '+' && (j != 0 || *(argv[i] + 1) == '+'))
     want_xbug_line = TRUE;
    else
    {
     j++; continue;
    }
    
    if (i+1 < argc)
    {
     j = i;
     while (j < argc - 1)
     {
      argv[j] = argv[j+1];
      j++;
     }
    }
    argc--;
    argv[argc] = NULL;
    
    i--;
    break;
   }
   j++;
  }

  j = 0;
  while(nosharedpixmapString[j])
  {
   if (Xstrcmp(nosharedpixmapString[j] + 1 + (j == 0), argv[i] + 1 + (j == 0)))
   {
#ifdef HAVE_XSHM
    if (*argv[i] == '-' && (j != 0 || *(argv[i] + 1) == '-'))
     xshm_flag = FALSE;
    else if (*argv[i] == '+' && (j != 0 || *(argv[i] + 1) == '+'))
     xshm_flag = TRUE;
    else
    {
     j++; continue;
    }
#endif
    
    if (i+1 < argc)
    {
     j = i;
     while (j < argc - 1)
     {
      argv[j] = argv[j+1];
      j++;
     }
    }
    argc--;
    argv[argc] = NULL;
    
    i--;
    break;
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
  
#ifdef Xc_XCLAMATION
 add_translations(display, TSL4_TEXT | TSL4_TEXTFIELD);
#endif

#if defined(Xc_XQUAD) || defined(Xc_XALLWRITE)
 add_translations(display, TSL4_TEXTFIELD);
#endif

 /*--- Hidden shell ---*/
 This->w_Hidden = XtVaAppCreateShell(NULL, appName, 
				     applicationShellWidgetClass, display, 
				     XmNkeyboardFocusPolicy, XmEXPLICIT,
				     XmNmappedWhenManaged, False,
				     XmNwidth, 640,
				     XmNheight, 400,
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

static void dest_MainInterface(This)
c_MainInterface *This;
{
 XtDestroyWidget(This->w_This);
 if (This->display_name)
  Xc_free(This->display_name);
 Xc_free(This);
 Xc_TRACE(("Object MainInterface destroyed"));
}

static void *copy_MainInterface(This)
c_MainInterface *This;
{
 Xc_TRACE(("Copy not implemented"));
 return NULL;
}

static void create_and_map_StartUp(this, TimeIndicator)
c_MainInterface *this;
c_TimeIndicator *TimeIndicator;
{
 Display	  *display;
 Arg             argsw[10];
 int             nargs;
 char            pszStartTitle[50];
 XmString        StartupTitle; 
 int		  width, height;
  
 /*--- Startup shell ---*/
 display = XtDisplay(this->w_Hidden);
 nargs = 0;
 XtSetArg(argsw[nargs], XmNtransient, True); nargs++;
 XtSetArg(argsw[nargs], XmNoverrideRedirect, True); nargs++;
 XtSetArg(argsw[nargs], XmNheight, STARTUP_HEIGHT); nargs++;
 XtSetArg(argsw[nargs], XmNwidth, STARTUP_WIDTH); nargs++;
 XtSetArg(argsw[nargs], XmNx, 
	  (DisplayWidth(display, 0) - STARTUP_WIDTH) >> 1);
 nargs++;
 XtSetArg(argsw[nargs], XmNy, 
	  (DisplayHeight(display, 0) - STARTUP_HEIGHT) >> 1);
 nargs++;
 this->w_Startup = XtCreatePopupShell("startup_screen",
				      transientShellWidgetClass,
				      /*overrideShellWidgetClass,*/
				      this->w_Hidden, argsw, nargs);
#ifdef EDITRES_COMPATIBLE
 XtAddEventHandler(this->w_Startup, (EventMask)0, TRUE,
		   _XEditResCheckMessages, NULL);
#endif
 nargs = 0;
 XtSetArg(argsw[nargs], XmNshadowType, XmSHADOW_OUT);
 nargs++;
 this->w_Frame = XmCreateFrame(this->w_Startup, 
			       "StartFrame", argsw, nargs);

 nargs = 0;
 XtSetArg(argsw[nargs], XmNentryAlignment, XmALIGNMENT_CENTER);
 nargs++;
 XtSetArg(argsw[nargs], XmNorientation, XmVERTICAL);
 nargs++;
 XtSetArg(argsw[nargs], XmNisHomogeneous, False);
 nargs++;
 this->w_RowStartup = XmCreateRowColumn(this->w_Frame,
					"RowStartup", argsw, nargs);
  
#ifdef PL
 sprintf(pszStartTitle, "Axene %s %d.%d.%d", appName, MAJOR, MINOR, PL);
#else
 sprintf(pszStartTitle, "Axene %s %d.%d", appName, MAJOR, MINOR);
#endif
  
 StartupTitle = XmStringCreateSimple(pszStartTitle);
  
 nargs = 0;
 XtSetArg(argsw[nargs], XmNlabelString, StartupTitle);
 nargs++;
 this->w_LabelStartup1 =  XmCreateLabel(this->w_RowStartup,
					"lTitleStartup",
					argsw, nargs);
 XmStringFree( StartupTitle );

 nargs = 0;
 this->w_LabelStartup2 =  XmCreateLabel(this->w_RowStartup,
					"lPleaseWait",
					argsw, nargs);
  
 F(TimeIndicator).get_indicator_geometry(TimeIndicator, &width, &height);
 nargs = 0;
 XtSetArg(argsw[nargs], XmNwidth, STARTUP_WIDTH);
 nargs++;
 XtSetArg(argsw[nargs], XmNheight, height);
 nargs++;
 XtSetArg(argsw[nargs], XmNresizePolicy, XmRESIZE_NONE);
 nargs++;
 this->w_DAreaStartup =  XmCreateDrawingArea(this->w_RowStartup,
					     "TIndicator",
					     argsw, nargs);  

 this->LabelMessage = (c_XLabel *)NEW(c_XLabel)(this->w_RowStartup,
						NULL,
						"lLoadingMessage",
						XlALIGNMENT_CENTER,
						XcR_loadingMessageFont);

 XtVaSetValues(this->LabelMessage->w_This, 
	       XmNrecomputeSize, False,
	       XmNheight, 15,
	       NULL);

 nargs = 0;
 sprintf(pszStartTitle, "©%d-%d Axene.", Xc_COPYRIGHT_BEGIN, Xc_COPYRIGHT_END);
 StartupTitle = XmStringCreateSimple(pszStartTitle);
 XtSetArg(argsw[nargs], XmNlabelString, StartupTitle);
 nargs++;
 this->w_LabelStartup3 =  XmCreateLabel(this->w_RowStartup,
					"lStartupMessage",
					argsw, nargs);
 XmStringFree( StartupTitle );

 XtManageChild(this->w_Frame);
 XtManageChild(this->w_RowStartup);
 XtManageChild(this->w_LabelStartup1);
 XtManageChild(this->w_LabelStartup2);
 XtManageChild(this->w_DAreaStartup);
 XtManageChild(this->LabelMessage->w_This);
 XtManageChild(this->w_LabelStartup3);

 XtManageChild(this->w_Startup);
 XtPopup(this->w_Startup, XtGrabNone);

 F(this->LabelMessage).set_string(this->LabelMessage, "Initialize...");

 F(TimeIndicator).display_indicator(TimeIndicator, 
				    XtWindow(this->w_DAreaStartup),
				    (STARTUP_WIDTH - width) >> 1, 0);
 XtAddCallback(this->w_DAreaStartup, XmNexposeCallback,
	       (XtCallbackProc)F(TimeIndicator).expose_indicator, 
	       TimeIndicator);

 shellMap(this, this->w_Startup);
 XcFlushEvents(this->w_Startup, (void (*)())XtDispatchEvent);
 XmUpdateDisplay(this->w_Hidden);
 Xc_TRACE(("Startup shell"));
}

static void unmap_StartUp(this)
c_MainInterface *this;
{
 XtPopdown(this->w_Startup);
 XtDestroyWidget(this->w_LabelStartup1);
 XtDestroyWidget(this->w_LabelStartup2);
 XtDestroyWidget(this->w_DAreaStartup);
 XtDestroyWidget(this->w_LabelStartup3);
 DELETE(c_XLabel)(this->LabelMessage);
 XtDestroyWidget(this->w_RowStartup);
 XtDestroyWidget(this->w_Frame);
 XtDestroyWidget(this->w_Startup);
 /*DELETE(c_TimeIndicator)(TimeIndicator);*/
 /*TimeIndicator = NULL;*/
 Xc_TRACE(("destroyStartup"));
}

static void create_TopLevel(this, ManageW)
c_MainInterface *this;
c_ManageWidget *ManageW;
{
 Arg             argsw[10];
 int             nargs;

 /*--- Application shell ---*/


 nargs = 0;
 XtSetArg(argsw[nargs], XmNwidth, 630); nargs++;
 XtSetArg(argsw[nargs], XmNheight, 450); nargs++;
 XtSetArg(argsw[nargs], XmNminWidth, 410); nargs++;
 XtSetArg(argsw[nargs], XmNminHeight, 200); nargs++;

 this->w_Main = XtCreatePopupShell(appName, topLevelShellWidgetClass,
				   this->w_Hidden, argsw, nargs);

#ifdef EDITRES_COMPATIBLE
 XtAddEventHandler(this->w_Main, (EventMask)0, TRUE,
		   _XEditResCheckMessages, NULL);
#endif
 nargs = 0;
 XtSetArg(argsw[nargs], XmNresizePolicy, XmRESIZE_GROW); nargs++;
 this->w_This = XmCreateForm(this->w_Main, "Application", argsw, nargs);
  
 F(ManageW).Add_child(ManageW, this->w_This, this->w_Main);
 Xc_TRACE(("Main shell"));
}

static void map_TopLevel(this)
c_MainInterface *this;
{
 XtPopup(this->w_Main, XtGrabNone);
 
 shellMap(this, this->w_Main);
 wMainWindow = this->w_Main;

 XRaiseWindow(XtDisplay(this->w_Startup), XtWindow(this->w_Startup)); 
 XcFlushEvents(this->w_Main, (void (*)())XtDispatchEvent);
 XmUpdateDisplay(this->w_Hidden);
}

static void unmap_TopLevel(this)
c_MainInterface *this;
{
 XtPopdown(this->w_Main);
 XtDestroyWidget(this->w_Main);
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
#ifdef Xc_XCLAMATION
 image = xcl_icon_xpm;
#endif
#ifdef Xc_XQUAD
 image = xquad_icon_xpm;
#endif
#ifdef Xc_XALLWRITE
 image = xallwrite_icon_xpm;
#endif
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
 Boolean iconic;
 XtAppContext app;

 XtVaGetValues(w, XmNiconic, &iconic, NULL);
 if (iconic == True) return;
  
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

/* ----------------------------------------------------------------- ** 
** print_copyright                                                   ** 
** ----------------------------------------------------------------- */
static void print_copyright(built)
boolean built;
{
 printf("%s version %d.%d.%d (%s), %s\n", appName,
	MAJOR, MINOR, PL, Xc_SYSTEM_NAME, copyrightDisplay);
 
#if defined(SPECIAL_EDITION)
 printf("Special Edition: %s, built on %s/%s/%s.\n", SPECIAL_EDITION,
	MAKE_DATE_MONTH, MAKE_DATE_DAY, MAKE_DATE_YEAR);
#else
 if (built)
  printf("Built on %s/%s/%s.\n", MAKE_DATE_MONTH, MAKE_DATE_DAY, 
	 MAKE_DATE_YEAR);
#endif
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

static void set_title(this)
c_MainInterface	*this;
{
 
 if (gkinfo.valid != Xc_KEY_GOOD)
 {
  Arg             argsw[2];
  char            *titletmp;
  char            *title;

  XtVaGetValues(this->w_Main, XmNtitle, &titletmp, NULL);
  
  title = Xc_malloc("Title", sizeof(char) * (strlen(titletmp) + 20));
  strcpy(title, titletmp);
  strcat(title, " (Unregistered)");
  
  XtSetArg(argsw[0], XmNtitle, title);
  XtSetValues(this->w_Main, argsw, 1);
  Xc_free(title);
 }
}

static void set_loadingMessage(this, message)
c_MainInterface	*this;
char *message;
{
 F(this->LabelMessage).set_string(this->LabelMessage, message);
}








