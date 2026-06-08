/*
** main.c for XInstall in Main/
** Main part of Install
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
** Started on  Thu Jan 16 00:39:43 1997 Antoine Buat
** Last update Tue Feb  1 11:45:14 2000 Emmanuel Paris
*/

#define __XC_MAIN__

/*--- Define ---*/
#define SIGNALH
#define IOERRORH

#define EXIST		1
#define READABLE	2
#define WRITABLE	4

#define P_ERROR		-1
#define P_CREATE	1
#define P_EXIST		2

/*--- Include ---*/
#include "xcalibur.h"

#include "deadline.h"    /* obsolete */

#include "file.h"
#include "XErrorHandler.h"
#include "SignalHandler.h"
#include "CustomDialog.h"
#include "AlertBox.h"
#include "Box_language.h"
#include "Box_license.h"
#include "Box_key.h"
#include "Box_directory.h"
#include "Box_select.h"
#include "Box_install.h"
#include "Box_keygen.h"

#include "Resource.h"
#include "ImageManager.h"
#include "Colormap.h"
#include "Cursor.h"
#include "MainInterface.h"
#include "DocIcon.h"
#include "Tongues.h"

#include <Xm/DialogS.h>

#ifdef HAVE_XSHM
#include <X11/extensions/XShm.h>
#endif

#ifdef EDITRES_COMPATIBLE
#include <X11/Xmu/Editres.h>
#endif
#include <sys/stat.h>
#include <pwd.h>

enum 
{
 xi_LANGAGE, xi_LICENSE, xi_KEY, xi_DIRECTORY, xi_SELECT, xi_INSTALL,
 xi_KEYGEN, xi_LAST
};

#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
static int cdinst[] = 
{
 xi_LANGAGE, xi_LICENSE, xi_KEY, xi_DIRECTORY, xi_SELECT, xi_INSTALL, 
 xi_KEYGEN, xi_LAST
};

static int liveinst[] = 
{
 xi_LANGAGE, xi_LICENSE, xi_KEY, xi_KEYGEN, xi_LAST
};

static int keyginst[] = 
{
 xi_LANGAGE, xi_LICENSE, xi_KEY, xi_DIRECTORY, xi_KEYGEN, xi_LAST
};
#else
static int demoinst[] = 
{
 xi_LANGAGE, xi_LICENSE, xi_DIRECTORY, xi_SELECT, xi_INSTALL, xi_LAST
};
#endif

#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
static int *install_seq = cdinst;
#else
static int *install_seq = demoinst;
#endif

/*--- Global static declaration ---*/
static char *pictureString[] = { "--picture", "-picture", "-p", NULL};
static char *srcString[] = { "--srcpath", "-srcpath", "-s", NULL};
static char *dstString[] = { "--dstpath", "-dstpath", "-d", NULL};
#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
static char *liveString[] = { "--live", "-live", "-cd", NULL};
static char *keygString[] = { "--keygen", "-key", "-k", NULL};
#endif
static char *langString[] = { "--language", "-language", "-lang", "-l", NULL};
static char *chooselString[] = { "-cl", "+cl", NULL};

/*--- Global declaration ---*/
c_XErrorHandler	*XErrH;
int IOErrorCount = 0;

#ifdef SIGNALH
c_SignalHandler	*SignalH;
jmp_buf SigErrorEnv;
#endif
int SigErrorCount = 1;

#ifdef HAVE_XSHM
boolean xshm_flag;
#endif

c_MainInterface		*MainInterface1;
c_ManageWidget		*Manager1;
c_Resource		*GlobResources;
c_Tongue		*GlobTongue = NULL;
c_Colormap		*GlobColormap;
c_ImageManager		*GlobImgMng;
c_GCursor		*GlobCursor;
c_DocIcon		*GlobDocIcon;

char			*GlobSrcOpt = NULL;
char			*GlobDstOpt = NULL;
char			*GlobPicOpt = NULL;
boolean			GlobLiveOpt = FALSE;
boolean			GlobKeyOpt = FALSE;
boolean			GlobLangOpt = FALSE;

Widget			wMainWindow;

char			*DstList[]=
{
 "/opt/Axene",
 "/usr/local/Axene/",
 "~/Axene/",
 NULL
};

char			*SrcList[]=
{
 "./Archives/",
 "./Packages/",
 "./packages/",
 "/cdrom/install/",
 "./",
 NULL
};
/*
  "/home/xcalibur/install/programme/Main/",
  "/export/home/xcalibur/Example/install",
  */

char			DstDir[1000];
char			SrcDir[1000];
icon_list		*pack_list;

/*--- type click_zone is defined in Box_language.h ---*/
click_zone lang_zone[6] = 
{
 { 157,  38, 145,  86, Xi_USENGLISH, "English.us" },
 { 350,  66, 145,  86,   Xi_ENGLISH, "English.uk" },
 {  50, 145, 145, 105,    Xi_FRENCH, "French" },
 { 231, 259, 145,  95,    Xi_GERMAN, "German" },
 { 400, 185, 145, 105,   Xi_SPANISH, "Spanish" },
 {  10,   0,  10,  10,            0, "None" }
};

char	*key1 = NULL;
char	*key2 = NULL;
char	*company = NULL;
char	*fname = NULL;
char	cdkey[40];
int	selected_tongue = Xi_USENGLISH;

int	seed;
int	soft;
int	version;
char	*Globprog_name;


/*--- function prototype ---*/
void XcHandleEvent ___PROTO((XEvent *event));
void Signal_Error_Handler();
void IO_Error_Handler();
void my_getopt();
void find_valid_path();
int is_valid_path();
void expand_path();

#ifdef HAVE_XSHM
extern Bool XShmQueryExtension();
#endif

static c_Box_language		*boxL;
static c_Box_license		*boxLc;
static c_Box_key		*boxK;
static c_Box_directory		*boxD;
static c_Box_select		*boxS;
static c_Box_install		*boxI;
static c_Box_keygen		*boxKG;
static int			step;


/*--- Main ---*/
int main(argc, argv)
int	argc;
char	**argv;
{
 Display	*display;
 XtAppContext  app;
 char		*env_dir;
 int		rc;
 Widget		w_box;
 Widget		w_bul;

 Globprog_name = SearchFile(argv[0]);
 if ((env_dir = getenv(Xc_ENVIRONNEMENT)) == NULL)
  if ((env_dir = getenv(Xc_ENVIRONNEMENT_GLOBAL)) == NULL)
   Xsetenv(Xc_ENVIRONNEMENT,"/");

 my_getopt(argv, argc);

 if (GlobSrcOpt)
  if (is_valid_path(GlobSrcOpt, EXIST|READABLE)>0)
   expand_path(SrcDir, GlobSrcOpt);
  else
   find_valid_path(SrcDir, SrcList, EXIST|READABLE);
 else
  find_valid_path(SrcDir, SrcList, EXIST|READABLE);

 if (GlobDstOpt)
  if (is_valid_path(GlobDstOpt, WRITABLE)>0)
   expand_path(DstDir, GlobDstOpt);
  else
   find_valid_path(DstDir, DstList, WRITABLE);
 else
  find_valid_path(DstDir, DstList, WRITABLE);

 MainInterface1 = NEW(c_MainInterface)(argc, argv);
 display = XtDisplay(MainInterface1->w_Hidden);

 GlobResources = NEW(c_Resource)(MainInterface1->w_Hidden);

 Xc_TRACE(("resource ownColormap: %s keepFreeColor: %d",
	   F(GlobResources).getBoolean(GlobResources, XcR_ownColormap) ?
	   "On" : "Off",
	   F(GlobResources).getInt(GlobResources, XcR_keepFreeColor)));

 GlobColormap = (c_Colormap *)NEW(c_Colormap)
  (display, MainInterface1->w_Hidden,
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
 GlobCursor = (c_GCursor *)NEW(c_GCursor)(display);
  
 GlobDocIcon = (c_DocIcon *)NEW(c_DocIcon)(display);
  
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
 F(GlobColormap).generate_colormap(GlobColormap);

 Manager1 = (c_ManageWidget *)NEW(c_ManageWidget)();
 F(MainInterface1).create_TopLevel(MainInterface1, Manager1);

 F(Manager1).Manage_all(Manager1, NULL);

 /*F(MainInterface1).map_TopLevel(MainInterface1);*/

 app = XtWidgetToApplicationContext(wMainWindow);
  
 SigErrorCount = 0;		/* can continue now */
 pack_list = NULL;

 wMainWindow = w_box = WIDGET(MainInterface1);

 XtVaSetValues(w_box, XmNminWidth, 635, XmNmaxWidth, 635, XmNminHeight, 470, 
	       XmNwidth, 635, XmNheight, 470, XmNtitle, "Axene XInstall", 
	       NULL);
 XtManageChild(w_box);

 F(GlobDocIcon).registerPixmapConverters(GlobDocIcon);

 w_bul = XtVaCreateWidget("bboard", xmBulletinBoardWidgetClass,
			  w_box,
			  XmNallowShellResize, True,
			  XmNautoUnmanage, False,
			  XmNresizePolicy, XmRESIZE_ANY,
			  XmNallowOverlap, False,
			  XmNwidth, 635, 
			  XmNheight, 470,
			  NULL);

 XtManageChild(w_bul);

 XtPopup(w_box, XtGrabNone);
 /*XtPopup(w_box, XtGrabExclusive);*/
 /*XtPopup(w_box, XtGrabNonexclusive);*/

 F(MainInterface1).set_main_icon(MainInterface1);
 F(GlobCursor).enable_wait_mode(GlobCursor, XtWindow(wMainWindow));
 
  
#if 0
 /*--- Add a callback for WM_DELETE_WINDOW protocol ---*/
 /*
   Atom WM_DELETE_WINDOW;
   WM_DELETE_WINDOW = XmInternAtom(XtDisplay(p->w_Main),
   "WM_DELETE_WINDOW", False);
   XmAddWMProtocolCallback(p->w_Main, WM_DELETE_WINDOW,
   (void *)close_callback, p);
   */
 /*XtOverrideTranslations(WIDGET(p), XtParseTranslationTable(trans));*/
 this->modal_flag = modal_flag;
 XtVaSetValues(WIDGET(this),
	       XmNdialogStyle, modal_flag?
	       XmDIALOG_FULL_APPLICATION_MODAL : XmDIALOG_MODELESS,
	       NULL);
 /*
   XtDestroyWidget(WIDGET(this));
   XtDestroyWidget(this->w_Main);
   */
#endif


 boxL = NULL;
 boxK = NULL;
 boxD = NULL;
 boxS = NULL;
 boxI = NULL;
 boxKG = NULL;

 step = 0;
 while (install_seq[step] != xi_LAST)
 {
  if (setjmp(SigErrorEnv) == -1)
  {
   rc = 0;
   if (boxL)
   {
    rc = F(boxL).waitForUnmap(boxL, &selected_tongue);
    DELETE(c_Box_language)(boxL);
    boxL = NULL;
   }
   if (boxLc)
   {
    rc = F(boxLc).waitForUnmap(boxLc);
    DELETE(c_Box_language)(boxLc);
    boxLc = NULL;
   }
   if (boxK)
   {
    rc = F(boxK).waitForUnmap(boxK);
    DELETE(c_Box_key)(boxK);
    boxK = NULL;
   }
   if (boxD)
   {
    rc = F(boxD->Dialog).waitForUnmap(boxD->Dialog);
    DELETE(c_Box_directory)(boxD);
    boxD = NULL;
   }
   if (boxS)
   {
    rc = F(boxS->Dialog).waitForUnmap(boxS->Dialog);
    DELETE(c_Box_select)(boxS);
    boxS = NULL;
   }
   if (boxI)
   {
    rc = F(boxI->Dialog).waitForUnmap(boxI->Dialog);
    DELETE(c_Box_install)(boxI);
    boxI = NULL;
   }
   if (boxKG)
   {
    rc = F(boxKG).waitForUnmap(boxKG);
    DELETE(c_Box_keygen)(boxKG);
    boxKG = NULL;
   }
   Xc_WARNING(("Error Signal detected, trying to continue"));
  }
  else
  {
   rc = 0;
   switch (install_seq[step])
   {
   case xi_LANGAGE:
    boxL = (c_Box_language *)NEW(c_Box_language)(w_bul, "BoxLanguage", 
						 lang_zone, selected_tongue);
    rc = F(boxL).waitForUnmap(boxL, &selected_tongue);
    if (GlobTongue)
     DELETE(c_Tongue)(GlobTongue);
    GlobTongue = NEW(c_Tongue)(MainInterface1->w_Hidden, selected_tongue);

    if (GlobResources)
     DELETE(c_Resource)(GlobResources);
    GlobResources = NEW(c_Resource)(MainInterface1->w_Hidden);
    DELETE(c_Box_language)(boxL);
    boxL = NULL;
    break;
   case xi_LICENSE:
    if (rc != XcCD_BACK)
    {
     boxLc = (c_Box_license *)NEW(c_Box_license)(w_bul, "BoxLicense", 
						 selected_tongue);
     if (boxLc)
     {
      rc = F(boxLc).waitForUnmap(boxLc);
      DELETE(c_Box_license)(boxLc);
      boxL = NULL;
     }
     else rc = XcCD_NEXT;
    }
    break;
   case xi_KEY:
    boxK = (c_Box_key *)NEW(c_Box_key)(w_bul, "BoxKey");
    rc = F(boxK).waitForUnmap(boxK);
    DELETE(c_Box_key)(boxK);
    boxK = NULL;
    break;
   case xi_DIRECTORY:
    boxD = (c_Box_directory *)NEW(c_Box_directory)
     (w_bul, "BoxDirectory", SrcDir, DstDir);
    rc = F(boxD).waitForUnmap(boxD);
    DELETE(c_Box_directory)(boxD);
    boxD = NULL;
    break;
   case xi_SELECT:
    boxS = (c_Box_select *)NEW(c_Box_select)(w_bul, "BoxSelect", 
					     SrcDir, DstDir, selected_tongue,
					     &pack_list);
    rc = F(boxS).waitForUnmap(boxS);
    DELETE(c_Box_select)(boxS);
    boxS = NULL;
    break;
   case xi_INSTALL:
    boxI = (c_Box_install *)NEW(c_Box_install)(w_bul,
					       "BoxInstall", 
					       SrcDir, DstDir, pack_list);
    rc = F(boxI->Dialog).waitForUnmap(boxI->Dialog);
    DELETE(c_Box_install)(boxI);
    boxI = NULL;
    break;
   case xi_KEYGEN:
    {
     boxKG = (c_Box_keygen *)NEW(c_Box_keygen)(w_bul, "BoxKG", GlobLiveOpt);
     rc = F(boxKG).waitForUnmap(boxKG);
     DELETE(c_Box_keygen)(boxKG);
     boxKG = NULL;
     break;
    }
   }
  }
  switch(rc)
  {
  case XcCD_DONTAGREE:
  case XcCD_CANCEL:
   if (install_seq[step] > xi_LANGAGE && install_seq[step] != xi_INSTALL)
   {
    char *string;
     
    string = F(GlobResources).getString(GlobResources, XcR_quitMessage);
    if (Xc_ExitContinueAlert(string))
     goto end_main;
   }
   else
    goto end_main;
   break;
  case XcCD_INSTALL:
  case XcCD_AGREE:
  case XcCD_QUIT:
  case XcCD_NEXT:
   step++;
   break;
  case XcCD_BACK:
   step--;
   break;
  }
 }

 end_main:

#ifdef  SIGNALH
 DELETE(c_SignalHandler)(SignalH);
#endif
 
 XtDestroyWidget(w_bul); 
 F(MainInterface1).unmap_TopLevel(MainInterface1);

 DELETE(c_Colormap)(GlobColormap);
 DELETE(c_ImageManager)(GlobImgMng);
 DELETE(c_DocIcon)(GlobDocIcon);
 DELETE(c_XErrorHandler)(XErrH);

 if (Globprog_name) Xc_free(Globprog_name);
 if (fname) Xc_free(fname);
 if (company) Xc_free(company);
 if (key1) Xc_free(key1);
 if (key2) Xc_free(key2);
 if (pack_list) FUNCTION(c_Box_select, deletePackList)(&pack_list, FALSE);
 
 DELETE(c_GCursor)(GlobCursor);
 DELETE(c_Tongue)(GlobTongue);
 DELETE(c_MainInterface)(MainInterface1);
 DELETE(c_Resource)(GlobResources);
 
#ifndef NMEMDEBUG
 Xc_mdump();
#endif

 Xc_exit(0);
}

/* ----------------------------------------------------------------- ** 
** HandleEvent - Process event                                       ** 
** ----------------------------------------------------------------- */
void XcHandleEvent(event)
XEvent *event;
{
 XtDispatchEvent(event);
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
 XtInputMask mask;

 app = XtWidgetToApplicationContext(w);
 XSync(XtDisplay(w), 0);
 while((mask = XtAppPending(app)) != 0)
 {
  XtAppNextEvent(app, &event);
  func(&event);
 }
}

/* ----------------------------------------------------------------- ** 
** Signal Error Manager                                              ** 
** ----------------------------------------------------------------- */
void Signal_Error_Handler(resource)
int	resource;
{
 XmString	xms;
  
 if (SigErrorCount == 0)
 {
  SigErrorCount = 1;
  if (IOErrorCount == 1)
  {
   Xc_WARNING(("Signal Error In IOError Handling. FATAL..."));
   exit (-1);
  }
  Xc_TRACE(("Signal Error Handle %d", resource));
      
  xms = F(GlobResources).getXmString(GlobResources, resource);
      
      
  switch(Xc_SigErrorAlert(xms, 0))
  {
  case XcCD_CONTINUE:
   XmStringFree(xms);
   SigErrorCount = 0;
   Xc_TRACE(("call longjmp..."));
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
  Xc_TRACE(("Error in signal handler"));
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

/* ----------------------------------------------------------------- ** 
** X IOError Manager                                                 ** 
** ----------------------------------------------------------------- */
void IO_Error_Handler(none)
int	none;
{
 Display	*display;

 if (IOErrorCount == 0)
 {
  IOErrorCount = 1;
  Xc_TRACE(("IO Error Handle"));
      
  F(MainInterface1).reopen_display(MainInterface1);
  display =  XtDisplay(MainInterface1->w_Hidden);
      
  GlobResources = NEW(c_Resource)(MainInterface1->w_Hidden);
      
  switch(Xc_IOErrorAlert(0))
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
  Xc_WARNING(("Sorry, cannot recover from previous XIOError, exiting..."));
  Xc_exit (-1);
 }
}

/* ----------------------------------------------------------------- ** 
** get options                                                       ** 
** ----------------------------------------------------------------- */
void my_getopt(argv, argc)
char	**argv;
int	argc;
{
 int	i, arg;

 arg = 0;
 while (arg < argc - 1)
 {
  arg++;

  /*--- picture option ---*/
  i = 0;
  while (pictureString[i])
  {
   if (Xstrcmp(pictureString[i], argv[arg]) && arg < argc - 1) break;
   i++;
  }	  
  if (pictureString[i])
  {
   GlobPicOpt = argv[++arg];
   continue;
  }

  /*--- source directory option ---*/
  i = 0;
  while (srcString[i])
  {
   if (Xstrcmp(srcString[i], argv[arg]) && arg < argc - 1) break;
   i++;
  }
  if (srcString[i]) 
  {
   GlobSrcOpt = argv[++arg];
   continue;
  }	  
  
  /*--- destination directory option ---*/
  i = 0;
  while (dstString[i])
  {
   if (Xstrcmp(dstString[i], argv[arg]) && arg < argc - 1) break;
   i++;
  }
  if (dstString[i])
  {
   GlobDstOpt = argv[++arg];
   continue;
  }
  
#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
  /*--- live option ---*/
  i = 0;
  while (liveString[i])
  {
   if (Xstrcmp(liveString[i], argv[arg]))
   {
    install_seq = liveinst;
    GlobLiveOpt = TRUE;
    break;
   }
   i++;
  }

  /*--- key generation option ---*/
  i = 0;
  while (keygString[i])
  {
   if (Xstrcmp(keygString[i], argv[arg]))
   {
    install_seq = keyginst;
    GlobLiveOpt = TRUE;
    GlobKeyOpt = TRUE;
    break;
   }
   i++;
  }
#endif

  /*--- language option ---*/
  i = 0;
  while(langString[i])
  {
   if (Xstrcmp(langString[i], argv[arg]) && arg < argc - 1) break;
   i++;
  }
  if (langString[i])
  {
   char *lang = argv[++arg];
   int j = 0;
 
   while(lang_zone[j].ret_code != 0)
   {
    if (Xstrcmp(lang, lang_zone[j].ret_string))
    {
     selected_tongue = lang_zone[j].ret_code;
     GlobLangOpt = TRUE;
     break;
    }
    j++;
   }
   continue;
  }
  
  /*--- choose language ---*/
  i = 0;
  while (chooselString[i])
  {
   if (Xstrcmp(chooselString[i], argv[arg]))
   {
    if (*argv[arg] == '+')
     GlobLangOpt = FALSE;
    else
     GlobLangOpt = TRUE;
    break;
   }
   i++;
  }
 }
}

/* ----------------------------------------------------------------- ** 
** find valid path in a list                                         ** 
** ----------------------------------------------------------------- */
void find_valid_path(path, path_list, mode)
char	*path;
char	**path_list;
int	mode;
{
 int		i = 0;
 char		tmp_path[1000];
 
 while (path_list[i])
 {
  expand_path(tmp_path, path_list[i]);
  if (is_valid_path(tmp_path, mode) > 0)
  {
   strcpy(path, tmp_path);
   break;
  }
  i++;
 }
}

/* ----------------------------------------------------------------- ** 
** is this path valid for this mode                                  ** 
** ----------------------------------------------------------------- */
int is_valid_path(path, mode)
char	*path;
int	mode;
{
 int		i;
 struct stat	stat_buf;
 struct stat	stat_buf_old;
 uid_t		my_uid;
 gid_t		my_gid;
 int		last_car;

 i=0;
 my_uid = geteuid();
 my_gid = getegid();
 memset(&stat_buf_old, 0, sizeof(struct stat));

 if (path[i]=='/')
 {
  if (stat("/", &stat_buf) == -1)
  {
   Xc_WARNING(("can't stat /"));
   return P_ERROR;
  }
  if (my_uid)
   if (!(stat_buf.st_mode & S_IXUSR) || (stat_buf.st_uid != my_uid))
    if (!(stat_buf.st_mode & S_IXGRP) || (stat_buf.st_gid != my_gid))
     if (!(stat_buf.st_mode & S_IXOTH))
     {
      Xc_WARNING(("can't access /"));
      return P_ERROR;
     }
 }

 while (1)
 {
  while (path[i] && (path[i]!='/'))
   i++;
  if (path[i] == '/')
  {
   last_car = path[i+1];
   path[i+1]=0;
  }
  else
   last_car = 0;

  memcpy(&stat_buf_old, &stat_buf, sizeof(struct stat));
  if (stat(path, &stat_buf) == -1)
  {
   if (mode & EXIST)
    return P_ERROR;
   else
   {
    if (my_uid)
     if (mode & WRITABLE)
      if (!(stat_buf.st_mode & S_IWUSR) || (stat_buf.st_uid != my_uid))
       if (!(stat_buf.st_mode & S_IWGRP) || (stat_buf.st_gid != my_gid))
	if (!(stat_buf.st_mode & S_IWOTH))
	 return P_ERROR;
    path[i+1]=last_car;
    return P_CREATE;
   }
  }
  if (last_car)
  {
   if (my_uid)
    if (!(stat_buf.st_mode & S_IXUSR) || (stat_buf.st_uid != my_uid))
     if (!(stat_buf.st_mode & S_IXGRP) || (stat_buf.st_gid != my_gid))
      if (!(stat_buf.st_mode & S_IXOTH))
      {
       Xc_WARNING(("can't access %s", path));
       return P_ERROR;
      }
   i++;
   path[i]=last_car;
  }
  else
  {
   if (my_uid)
   {
    if (mode & READABLE)
     if (!(stat_buf.st_mode & S_IRUSR) || (stat_buf.st_uid != my_uid))
      if (!(stat_buf.st_mode & S_IRGRP) || (stat_buf.st_gid != my_gid))
       if (!(stat_buf.st_mode & S_IROTH))
	return P_ERROR;
    if (mode & WRITABLE)
     if (!(stat_buf.st_mode & S_IWUSR) || (stat_buf.st_uid != my_uid))
      if (!(stat_buf.st_mode & S_IWGRP) || (stat_buf.st_gid != my_gid))
       if (!(stat_buf.st_mode & S_IWOTH))
	return P_ERROR;
   }
   return P_EXIST;
  }
 }
}

/* ----------------------------------------------------------------- ** 
** expand path                                                       ** 
** ----------------------------------------------------------------- */
void expand_path(path, path_src)
char	*path;
char	*path_src;
{
 int		i=0;
 int		j=0;
 uid_t		my_uid;
 struct passwd	*pass_buf;
 char		tmp_src[1000];

 if (path_src[i]=='~')
 {
  strcpy(tmp_src, path_src);
  i++;
  if (tmp_src[i]=='/')
  {
   my_uid = geteuid();
   pass_buf = getpwuid(my_uid);
   if (pass_buf)
   {
    strcpy(path, pass_buf->pw_dir);
    j = strlen(path);
    if (path[j-1]!='/')
    {
     path[j++]='/';
     path[j]=0;
    }
    i++;
   }
   strcat(path, tmp_src+i);
  }
  else
  {
   while (tmp_src[i]!='/')
    i++;
   tmp_src[i]=0;
   pass_buf = getpwnam(tmp_src);
   if (pass_buf)
   {
    strcpy(path, pass_buf->pw_dir);
    j = strlen(path);
    if (path[j-1]!='/')
    {
     path[j++]='/';
     path[j]=0;
    }
    i++;
   }
   strcat(path, tmp_src+i);
  }
 }
 else
  strcpy(path, path_src);
}
