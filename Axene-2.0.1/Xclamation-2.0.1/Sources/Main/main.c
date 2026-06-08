/*
** main.c for Xclamation in Main/
** Main Part of Xclamation (Axene DTP software)
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
** Started on  Mon Jul 17 15:05:00 1995 Antoine Buat
** Last update Mon Apr 24 16:53:37 2000 Antoine Buat
*/

#define __XC_MAIN__

#include <X11/cursorfont.h>
#define SIGNALH
#define IOERRORH
#include "xcalibur.h"

#include "XErrorHandler.h"
#include "SignalHandler.h"

#include "WInterface.h"
#define _PRIVATE_PAGEFORMAT_
#include "PageFormat.h"

#include "../Pixmaps/trash.zpm"

#include "../Pixmaps/misc.tools.zpm"
#include "../Pixmaps/document.imprimer.zpm"
#include "../Pixmaps/document.save.zpm"
#include "../Pixmaps/document.open.zpm"
#include "../Pixmaps/document.new.zpm"
#include "../Pixmaps/copier.zpm"
#include "../Pixmaps/couper.zpm"
#include "../Pixmaps/coller.zpm"
#include "../Pixmaps/delete.zpm"

#include "../Pixmaps/cadre.zpm"
#include "../Pixmaps/cadre.creer.zpm"
#include "../Pixmaps/cadre.creer.lignes.zpm"
#include "../Pixmaps/cadre.creer.ovale.zpm"
#include "../Pixmaps/cadre.selectionner.zpm"
#include "../Pixmaps/cadre.fusion.zpm"
#include "../Pixmaps/cadre.add.zpm"
#include "../Pixmaps/cadre.sub.zpm"
#include "../Pixmaps/cadre.decouper.zpm"
#include "../Pixmaps/cadre.aligner.zpm"
#include "../Pixmaps/cadre.inserer.point.zpm"
#include "../Pixmaps/cadre.detruire.point.zpm"
#include "../Pixmaps/cadre.bouger.point.zpm"
#include "../Pixmaps/cadre.eloigner.zpm"
#include "../Pixmaps/cadre.approcher.zpm"
#include "../Pixmaps/cadre.premierplan.zpm"
#include "../Pixmaps/cadre.arriereplan.zpm"
#include "../Pixmaps/cadre.rotate.zpm"
#include "../Pixmaps/cadre.scale.zpm"
#include "../Pixmaps/cadre.lock.zpm"

#include "../Pixmaps/texte.zpm"
#include "../Pixmaps/texte.edit.zpm"
#include "../Pixmaps/liens.lier.avant.zpm"
#include "../Pixmaps/liens.lier.apres.zpm"
#include "../Pixmaps/liens.inserer.avant.zpm"
#include "../Pixmaps/liens.inserer.apres.zpm"
#include "../Pixmaps/liens.extraire.zpm"

#include "../Pixmaps/image.zpm"
#include "../Pixmaps/image.center.zpm"
#include "../Pixmaps/image.deplacer.zpm"
#include "../Pixmaps/image.normal.zpm"
#include "../Pixmaps/image.autoresize.zpm"
#include "../Pixmaps/image.bestaspect.zpm"
#include "../Pixmaps/image.maxaspect.zpm"
#include "../Pixmaps/image.default.zpm"
#include "../Pixmaps/image.flip.vertical.zpm"
#include "../Pixmaps/image.flip.horizontal.zpm"
#include "../Pixmaps/vector.fonts.zpm"

#include "../Pixmaps/loupe.zpm"
#include "../Pixmaps/loupe.arriere.zpm"
#include "../Pixmaps/loupe.avant.zpm"
#include "../Pixmaps/loupe.entier.zpm"
#include "../Pixmaps/loupe.reel.zpm"
#include "../Pixmaps/loupe.largeur.zpm"
#include "../Pixmaps/loupe.zone.zpm"
#include "../Pixmaps/loupe.25.zpm"
#include "../Pixmaps/loupe.50.zpm"
#include "../Pixmaps/loupe.75.zpm"
#include "../Pixmaps/loupe.100.zpm"
#include "../Pixmaps/loupe.200.zpm"
#include "../Pixmaps/loupe.400.zpm"

#include "../Pixmaps/name_emboss.zpm"
#include "../Pixmaps/small_column.zpm"
#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
#include "../Pixmaps/small_columnu.zpm"
#endif

#include "MainInterface.h"
#include "TimeIndicator.h"
#include "ScrolledWindow.h"
#include "MenuBar.h"
#include "PulldownMenu.h"
#include "ManageWidget.h"
#include "SpecialFileSelector.h"
#include "RowColumn.h"
#include "PanedWindow.h"
#include "Frame.h"
#include "Form.h"
#include "SpinBox.h"
#include "WinManager.h"
#include "HelpLine.h"
#include "HelpLine_PAO.h"
#include "RegisterHelp.h"
#include "RemoteServer.h"
#include "Resource.h"

#include "AIDocFile.h"
#include "VectorGraph.h"

#include "Icon.h"
#include "Document.h"
#include "DocIcon.h"
#include "Tableau.h"
#include "DocumentList.h"
#include "Box_about.h"
#include "Box_author.h"
#include "Box_legal.h"
#include "Box_legal2.h"
/*#include "Box_printers.h"*/
#include "Box_cache.h"
#include "Box_gridSetup.h"
#include "Box_document.h"
#include "Box_imprimer.h"
#include "Box_aligner.h"
#include "Box_styles.h"
#include "Box_regles.h"
#include "Box_color.h"
#include "Box_attrib.h"
#include "Box_registration.h"
#include "ScrollBarX.h"

#include "HelpLine.h"

#include "Image.h"
#include "Drag_And_Drop.h"
#include "TrashCan.h"

#include "LinkManager.h"
#include "ClipIcon.h"
#include "ImageManager.h"
#include "ImageCache.h"
#include "Keyboard.h"
#include "FontManager.h"
#include "PostScript.h"
#include "TextFrame.h"
#include "VectorFont.h"
#include "Color.h"
#include "Advanced_Editor.h"
#include "AlertBox.h"
#include "check.h"
#include "config.h"
#include "Readkey.h"

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

char	*defaultDocumentName;
char	*defaultFrameName;

c_WinManager	 *Gwm;

c_DocumentList	*DocList;

c_MainInterface  *MainInterface1;
Widget wMainWindow;

c_DrawingArea	*MainDocument;

c_Icon		**Icons_ref;

Widget           w_FrameOfClipBoard;
/* c_PanedWindow    *ClipBoard; */
c_ScrolledWindow *ClipBoardPrivate;
/* c_ScrolledWindow *ClipBoardPublic; */

c_ManageWidget *Manager1;
c_ManageWidget *Manager2;

/* c_RowColumn    *Draw2; */
c_RowColumn    *Draw3;
c_MenuBar      *Bar1;
c_PulldownMenu *MenuFil;
c_PulldownMenu *MenuEdi;
c_PulldownMenu *MenuPge;
c_PulldownMenu *MenuFra;
c_PulldownMenu *MenuHel;
c_PulldownMenu *MenuDis;
c_PulldownMenu *MenuWin;

#define Mxxx_NOTHING 		1

enum 
{
 MFil_NEW = 1,
 MFil_OPEN,
 MFil_CLOSE,
 MFil_SAVE,
 MFil_SAVEAS,
 MFil_SAVEALL,
 MFil_IMPORT,
 MFil_IMPORTIMAGE,
 MFil_IMPORTTEXT,
 MFil_IMPORTVECTOR,
 MFil_PRINT,
 MFil_QUIT,
  
 MFil_SEP1,
 MFil_SEP2,
 MFil_SEP3,
 MFil_SEP4,

 MFil_SUBMENU1
};

enum
{
 MEdi_CUT = 1,
 MEdi_COPY,
 MEdi_PASTE,
 MEdi_DELETE,
 MEdi_SELECTALL,
 MEdi_EDITCOLORS,
 MEdi_EDITSTYLES,
 MEdi_EDITRULERS,
 MEdi_PREFERENCES,
 MEdi_IMAGECACHE,
 
 MEdi_SEP1,
 MEdi_SEP2,
 MEdi_SEP3,

 MEdi_SUBMENU1
};

enum
{
 MDis_REDRAWALL = 1,
 MDis_REALSIZE,
 MDis_SCREENSIZE,
 MDis_FULLWIDTH,
 MDis_SHOWGRID,
 MDis_SHOWTYPOMARKS,
 MDis_SHOWRULERSMARKS,
 MDis_SHOWFRAMESBORDER,
 MDis_SHOWRULERS,
 MDis_SHOWCLIPBOARD,
 MDis_SHOWBOTTOMBAR,
 MDis_MAGNETON,
 
 MDis_SEP1,
 MDis_SEP2,
 MDis_SEP3
};

enum
{
 MPge_INSERTPAGEBEFORE = 1,
 MPge_INSERTPAGEAFTER,
 MPge_MODIFYPAGE,
 MPge_DELETEPAGE,
 MPge_GRIDSETUP,
 MPge_PAGEZOOM,
 MPge_REALSIZE,
 MPge_SCREENSIZE,
 MPge_FULLWIDTH,
 MPge_ZOOM25,
 MPge_ZOOM50,
 MPge_ZOOM75,
 MPge_ZOOM100,
 MPge_ZOOM200,
 MPge_ZOOM400,
 MPge_PAGEDISPLAY,
 MPge_SHOWGRID,
 MPge_SHOWTYPOMARKS,
 MPge_SHOWRULERSMARKS,
 MPge_SHOWFRAMESBORDER,
 MPge_MAGNETON,
  
 MPge_SEP1,
 MPge_SEP2,
 MPge_SEP3,
 MPge_SEP4,

 MPge_SUBMENU1,
 MPge_SUBMENU2
};

enum
{
 MFra_CREATERECTANGLE = 1,
 MFra_CREATEPOLYGON,
 MFra_CREATEELIPSE,
 MFra_POINTINSERT,
 MFra_POINTDELETE,
 MFra_POINTMOVE,
 MFra_PLANECHANGE,
 MFra_MOVEDOWN,
 MFra_MOVEUP,
 MFra_BACKGROUND,
 MFra_FOREGROUND,
 MFra_FRAMEOPERATION,
 MFra_FRAMEADD,
 MFra_FRAMESUB,
 MFra_FRAMEXOR,
 MFra_FRAMECUT,
 MFra_ALIGN,
 MFra_LOCK,
 MFra_UNLOCK,
 MFra_ROTATE,
 MFra_SCALE,
 MFra_ATTRIBS,
 MFra_FRAMEDELETE,
  
 MFra_SEP1,
 MFra_SEP2,
 MFra_SEP3,
 MFra_SEP4,
 MFra_SEP5,
  
 MFra_SUBMENU1,
 MFra_SUBMENU2
};

enum
{
 MWin_CASCADE = 1,
 MWin_TILE,
 MWin_DYNAMICDOCLIST,
 
 MWin_SEP1
};

enum
{
 MHel_OVERVIEW = 1,
 MHel_CONTENT,
 MHel_KEYBOARD,
 MHel_MOUSE,
 MHel_ONITEM,
 MHel_USINGHELP,
 MHel_LICENSE,
 MHel_MAILINGLIST,
 MHel_ABOUT,
 MHel_AUTHOR,
 
 MHel_SEP1,
 MHel_SEP2,
 MHel_SEP3,
 MHel_SEP4
};

enum
{
 HBAR_MISC, HBAR_FRAME, HBAR_TEXT, HBAR_IMAGE,
 HBAR_ZOOM, NB_HBAR
};

c_Form          *vIconBar;
c_RowColumn	*vIconBar_1;
c_RowColumn	*vIconBar_2;
Widget    	w_vIconBar_3;
c_Icon		*vIcon[NB_HBAR];
static int bar_selected = HBAR_FRAME;

c_Icon          *IconVlogo;
c_Form          *vIconBarlogo;

c_Form          *hIconBarForm;
c_RowColumn	*hIconBar[NB_HBAR];

c_Form          *BottomBar;

c_Icon *IconMis_1, *IconMis_2; /* open, new */
c_Icon *IconMis_s1, *IconMis_3, *IconMis_4; /* save print */
c_Icon *IconMis_s2, *IconMis_5, *IconMis_6, *IconMis_7; /* cut/copy/paste */
c_Icon *IconMis_8;

c_Icon *IconFra_0, *IconFra_1, *IconFra_10, *IconFra_11, *IconFra_12;
c_Icon *IconFra_13, *IconFra_14, /* *IconFra_2, */ *IconFra_s1, *IconFra_3;
c_Icon *IconFra_4, *IconFra_5, *IconFra_s2, *IconFra_s3, *IconFra_s4;
c_Icon *IconFra_6, *IconFra_7, *IconFra_8, *IconFra_9, *IconFra_15;
c_Icon *IconFra_16, *IconFra_17, *IconFra_18, *IconFra_s5, *IconFra_19;

c_Icon *IconTex_1, *IconTex_s1, *IconTex_2, *IconTex_3, *IconTex_4;
c_Icon *IconTex_5, *IconTex_6;

c_Icon *IconIma_1, *IconIma_2, *IconIma_3, *IconIma_s1;
c_Icon *IconIma_4, *IconIma_5, *IconIma_s2;
c_Icon *IconIma_6, *IconIma_7, *IconIma_8, *IconIma_9, *IconVec_s1;
c_Icon *IconVec_1;

c_Icon *IconZoo_1, *IconZoo_2, *IconZoo_s1, *IconZoo_3, *IconZoo_4;
c_Icon *IconZoo_5, *IconZoo_6,  *IconZoo_s2, *IconZoo_s3, *IconZoo_7;
c_Icon *IconZoo_8, *IconZoo_9, *IconZoo_10, *IconZoo_11, *IconZoo_12;
c_SpinBox *sbZoom;

c_Icon *IconTra;

c_Icon *IconBottom;

int flag_cache;
Window cache;

boolean ButtonLockFrame = TRUE;
boolean ButtonMaskGrid = FALSE;
boolean ButtonMaskFrame = FALSE;
boolean ButtonMaskLines = FALSE;
boolean ButtonMaskReperes = FALSE;
boolean ButtonStopMagnetisation = FALSE;
boolean ButtonMaskRegles = FALSE;
boolean ButtonMaskClipboard = FALSE;
boolean ButtonMaskBottomBar = TRUE;
boolean ButtonMaskVectorText = TRUE;
scale_t	PageZoom;

boolean	DocAlreadyGrised;
boolean SelectAllAlreadyGrised;
boolean	FrameToolsAlreadyGrised;
boolean LockToolsAlreadyGrised;
boolean	MultiFrameToolsAlreadyGrised;

boolean SImageAlreadyGrised;
boolean TexteAlreadyGrised;
boolean Texte2AlreadyGrised;
boolean SVectorAlreadyGrised;
boolean PasteAlreadyGrised;
boolean DeleteAlreadyGrised;


c_TimeIndicator  *TimeIndicator;
#define TI_NB_LEDS 17

c_Colormap 	*GlobColormap;
c_GCursor  	*GlobCursor = NULL;
c_Keyboard	*GlobKeyboard;
c_ClipBuffer	*GlobClipBuffer;
c_DragAndDrop	*GlobDragAndDrop;
c_Trash		*GlobTrash;
c_VectorFont	*GlobFontBase = NULL;
c_Color		*GlobColorBase = NULL;
c_TextRuler	*GlobTextRulerBase = NULL;
c_TextStyle	*GlobTextStyleBase = NULL;
c_HelpLine      *GlobHelpLine;
c_RegisterHelp	*GlobRegHelp;
c_RemoteServer	*GlobRemoteServer;
c_Resource	*GlobResources;
c_DocIcon	*GlobDocIcon;
printer_t	*GlobPrinterBase = NULL;
char		*t1imager_buffer = NULL;
char		*Globprog_name = NULL;

int		LinkFunction = 0;
c_TextFrame	*TextFrameForLink = NULL;

boolean	motif_focus = FALSE, tffs_focus = FALSE;	
boolean	handle_event_allowed = TRUE;

new_doc_s GlobNewDocInfo = 
{ 
 1,
 NULL,
 &Xc_Table_Page_Format[Xc_DEFAULT_FORMAT],
 FALSE, FALSE,
 Xc_PORTRAIT,
 SCALE_FROM_MILLIMETERS(17.82), SCALE_FROM_MILLIMETERS(17.82),
 SCALE_FROM_MILLIMETERS(17.82), SCALE_FROM_MILLIMETERS(17.82),
 1, 1,
 SCALE_FROM_MILLIMETERS(4.53), SCALE_FROM_MILLIMETERS(4.53),
 COORD_ZERO, COORD_ZERO, 
 SCALE_FROM_MILLIMETERS(5.94), SCALE_FROM_MILLIMETERS(5.94)
};

print_param_t	GlobPrintParam = 
{
 NULL,
 1, 1, 10000, 0, TRUE, TRUE, FALSE, FALSE, FALSE
};

align_param_t GlobAlignParam =
{
 TRUE, TRUE,
 FALSE, FALSE,
 ALIGN_CENTER, ALIGN_CENTER,
 0,0
};

/*c_Client     			*Low_Client; */
c_LM_Client			*High_Client;
c_ImageCache			*GlobImgCache;
c_ImageManager			*GlobImgMng;
c_FontManager			*GlobFntMng;
c_ClipIcon			*ClipIcon;

/* store the last toggle function*/
static int	store_function;
static Widget	store_widget_imagevector;

typedef struct 
{
 Widget	tog_wid;
 int	func_num;
} tog_tab_t;

typedef struct 
{
 Widget	tog_wid;
 int	func_num;
 int	mode_num;
} tog_tab2_t;

tog_tab_t  toggletable[30];
tog_tab2_t toggleimagemodetable[10];
tog_tab2_t togglevectormodetable[10];

void LoadImage ___PROTO((Widget w));
void LoadVector ___PROTO((Widget w));
void LoadText ___PROTO((Widget w));
void EditText ___PROTO((Widget w));
void SensitivizeDoc();

void SensitivizeLockTools();
void SensitivizeFrameTools();
void SensitivizeMultiFrameTools();

static void SensitivizeSVector();
static void SensitivizeSImage();
void SensitivizeSTexte();
void SensitivizeSTexte2();
void SensitivizeSelectAll();
void SensitivizePaste();
void SensitivizeDelete();

void DisplayClipboard();
void DisplayBottomBar();

void Mod_structure();
void MenuFileNew();
void MenuFileOpen();
void MenuFileClose();
void MenuFileSave();
void MenuFileSaveas();
void MenuFileSaveall();
void MenuFilePrint();
void MenuFileQuit();
void MenuFrameAttribs();
void MenuFrameAlign();
void MenuEditColors();
void MenuEditStyles();
void MenuEditRulers();
void MenuEditPrinters();
void MenuEditCache();
void MenuEditSave();
void MenuPageGridSetup();
void MenuHelpCallHelp();
void MenuHelpTrackHelp();
void MenuHelpAbout();
void MenuHelpAuthor();
static void MenuHelpLegal();
void MenuHelpRegistration();
void DocumentStyle();
void QuitApplication();
static void AttachHBar();
static void SelectHBarCallback();
void ToggleSwitch();
static void ToggleImagemodeSwitch();
static void ToggleSetImage();
void SelectFunction();

static void certify_zoom ___PROTO((c_WTextField *This, void *param));
static void cb_focus_motif ___PROTO((Widget w, void *none,
				     XmRowColumnCallbackStruct *cbs));
static void cb_unfocus_motif ___PROTO((Widget w, void *none,
				       XmRowColumnCallbackStruct *cbs));
static void cb_focus_tfZoom ___PROTO((Widget w, c_WTextField *This, 
				      XEvent *event));
static void cb_unfocus_tfZoom ___PROTO((Widget w, c_WTextField *This, 
					XEvent *event));

void Signal_Error_Handler();
void IO_Error_Handler();

void MenuWindowArrange();
void MenuWindowTile();
void SChangeWindow();
void ChangeDocCallback();
void XcHandleEvent ___PROTO((XEvent *event));
void XcFlushEvents ___PROTO((Widget w, void (*func) ___PROTO((XEvent *))));


int main(argc, argv)
int  argc;
char **argv;
{
 Display  *display;
 Arg      args[10];
 int      nargs;
 int      i;
 XtAppContext	  app;
 XSetWindowAttributes	set_attrib;
 XWindowAttributes	get_attrib;
 char		*vkey;
  
 Globprog_name = SearchFile(argv[0]);
 Xc_TRACE(("Full path filename: %s", Globprog_name));
 
 check_configuration();
  
 MainInterface1 = NEW(c_MainInterface)(argc, argv);

 display = XtDisplay(MainInterface1->w_Hidden);
 GlobResources = NEW(c_Resource)(MainInterface1->w_Hidden);
 defaultDocumentName = F(GlobResources).getString(GlobResources, 
						  XcR_defaultDocumentName);
 defaultFrameName = F(GlobResources).getString(GlobResources, 
					       XcR_defaultFrameName);
  
 GlobRegHelp = NEW(c_RegisterHelp)(display);
  
 Xc_TRACE(("resource keepFreeColor: %d",
	   F(GlobResources).getInt(GlobResources, XcR_keepFreeColor)));
  
 GlobColormap = (c_Colormap *)NEW(c_Colormap)
  (display, F(GlobResources).getInt(GlobResources, XcR_keepFreeColor),
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
  int event_base, error_base;	/* just used as dummy parameters */
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
  NEW(c_RemoteServer)(display, XtWindow(MainInterface1->w_Hidden));

 TimeIndicator = (c_TimeIndicator *)NEW(c_TimeIndicator)(display, TI_NB_LEDS);
 F(MainInterface1).create_and_map_StartUp(MainInterface1, TimeIndicator);
 F(TimeIndicator).advance_indicator(TimeIndicator);
  
 F(MainInterface1).set_loadingMessage(MainInterface1, "Initialize signals");

 Manager1 = (c_ManageWidget *)NEW(c_ManageWidget)();
 F(MainInterface1).create_TopLevel(MainInterface1, Manager1);
  
 F(GlobDocIcon).registerPixmapConverters(GlobDocIcon);
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

 /*Low_Client = (c_Client *)NEW(c_Client)(PAO,*/
 /*XcLM_Low_Level_Receive_Callback);*/
  
 /*  Low_Client = NULL; */
  
 /*if (Low_Client == NULL)*/
 /*Xc_WARNING(("Impossible de contacter le serveur ..."));*/
  
 High_Client = (c_LM_Client *)NEW(c_LM_Client)("Schmu-Schmu", "my_login",
					       "my_group", NULL,
					       /*Low_Client,*/
					       XcLM_USER_READ |
					       XcLM_USER_WRITE |
					       XcLM_USER_DYNAMIC |
					       XcLM_GROUP_READ |
					       XcLM_GROUP_WRITE |
					       XcLM_ALL_READ,
					       XcLM_TEXT_CLASS |
					       XcLM_BITMAP_CLASS,
					       -1);

 /*CCON_Box = NULL;*/
  
  

 GlobFntMng = (c_FontManager *)NEW(c_FontManager)(display);
  

 F(MainInterface1).set_loadingMessage(MainInterface1, "Creating Icon bars");
 F(TimeIndicator).advance_indicator(TimeIndicator);


 /* ----------------------------------------------------------------- ** 
 ** Vertical Icon Bar                                                 ** 
 ** ----------------------------------------------------------------- */

 vIconBar = (c_Form *)NEW(c_Form)(WIDGET(MainInterface1), Manager1,
				  "vIconBar", 34, 32);
 vIconBar_1 = (c_RowColumn *)NEW(c_RowColumn)(WIDGET(vIconBar), Manager1, 
					      "vIconBar1", 34, 0,
					      XmVERTICAL);
 nargs = 0;
 XtSetArg(args[nargs], XmNradioAlwaysOne, True);
 nargs++;
 XtSetArg(args[nargs], XmNradioBehavior, True);
 nargs++;
 XtSetValues(WIDGET(vIconBar_1), args, nargs);


 vIcon[HBAR_MISC] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1, 
					  "IconMisc", display,
					  misc_tools_xpm, NULL, XcToggle);
 
 CALLBACK(WIDGET(vIcon[HBAR_MISC]), FUNC_VALUE,
	  SelectHBarCallback, (void *)HBAR_MISC);
 
 
 vIcon[HBAR_FRAME] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1, 
					   "IconFrame", display,
					   cadre_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(vIcon[HBAR_FRAME]), FUNC_VALUE,
	  SelectHBarCallback, (void *)HBAR_FRAME);
 
 
 vIcon[HBAR_TEXT] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1,
					  "IconText", display,
					  texte_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(vIcon[HBAR_TEXT]), FUNC_VALUE,
	  SelectHBarCallback, (void *)HBAR_TEXT);
  
 vIcon[HBAR_IMAGE] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1, 
					   "IconImage", display,
					   image_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(vIcon[HBAR_IMAGE]), FUNC_VALUE,
	  SelectHBarCallback, (void *)HBAR_IMAGE);
  
 vIcon[HBAR_ZOOM] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1,
					  "IconZoom", display,
					  loupe_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(vIcon[HBAR_ZOOM]), FUNC_VALUE,
	  SelectHBarCallback, (void *)HBAR_ZOOM);


 XtSetArg(args[0], XmNset, True);
 XtSetValues(WIDGET(vIcon[bar_selected]), args, 1);

 nargs = 0;
 XtSetArg(args[nargs], XmNshadowType, XmSHADOW_OUT );
 nargs++;
 w_vIconBar_3 = XmCreateFrame(WIDGET(vIconBar), "vIconBar3", args, nargs);
 F(Manager1).Add_child(Manager1, w_vIconBar_3, WIDGET(vIconBar));

 vIconBarlogo = (c_Form *)NEW(c_Form)(w_vIconBar_3, Manager1,
				      "vIconBarlogo", 32, 32);

 vIconBar_2 = (c_RowColumn *)NEW(c_RowColumn)(WIDGET(vIconBar), Manager1,
					      "vIconBar2", 32, 0,
					      XmVERTICAL);
  

 F(vIconBar).attach_left(WIDGET(vIconBar), WIDGET(vIconBar_1), NULL);
 F(vIconBar).attach_right(WIDGET(vIconBar), WIDGET(vIconBar_1), NULL);
 F(vIconBar).attach_top(WIDGET(vIconBar), WIDGET(vIconBar_1), NULL);
  
 F(vIconBar).attach_top(WIDGET(vIconBar), w_vIconBar_3, WIDGET(vIconBar_1));
 F(vIconBar).attach_bottom(WIDGET(vIconBar), w_vIconBar_3, 
			   WIDGET(vIconBar_2));
  
 F(vIconBar).attach_left(WIDGET(vIconBar), w_vIconBar_3, NULL);
 F(vIconBar).attach_right(WIDGET(vIconBar), w_vIconBar_3, NULL);
  
 F(vIconBar).attach_left(WIDGET(vIconBar), WIDGET(vIconBar_2), NULL);
 F(vIconBar).attach_right(WIDGET(vIconBar), WIDGET(vIconBar_2), NULL);
 F(vIconBar).attach_bottom(WIDGET(vIconBar), WIDGET(vIconBar_2), NULL);
  

 /* ----------------------------------------------------------------- ** 
 ** Horizontal Icon Bars                                              ** 
 ** ----------------------------------------------------------------- */

 hIconBarForm = (c_Form *)NEW(c_Form)(WIDGET(MainInterface1), Manager1,
				      "hIconBar", 32, 32);

 for (i = 0; i < NB_HBAR; i++)
 {
  hIconBar[i] = 
   (c_RowColumn *)NEW(c_RowColumn)(WIDGET(hIconBarForm), NULL,
				   "rchIconBar", 0, 34, XmHORIZONTAL);
 }  

 store_function = -1;
 store_widget_imagevector = (Widget)0;
  
 F(TimeIndicator).advance_indicator(TimeIndicator);
  
 Manager2 = (c_ManageWidget *)NEW(c_ManageWidget)();

 
 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Creating Icon bar MISC");
 /* ----------------------------------------------------------------- ** 
 ** Horizontal Icon Bars : HBAR_MISC                                  ** 
 ** ----------------------------------------------------------------- */

 IconMis_1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_1", display,
				   document_new_xpm, NULL, XcPush);
  CALLBACK(WIDGET(IconMis_1), FUNC_ACTIVATE, MenuFileNew, NULL);


 IconMis_2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_2", display,
				   document_open_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconMis_2), FUNC_ACTIVATE, MenuFileOpen, NULL);

 IconMis_3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_3", display,
				   document_save_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconMis_3), FUNC_ACTIVATE, MenuFileSave, NULL);

 IconMis_4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_4", display,
				   document_imprimer_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconMis_4), FUNC_ACTIVATE, MenuFilePrint, NULL);

 IconMis_s1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				    "IconMis_s1", display,
				    NULL, NULL, XcSeparator);

 IconMis_8 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_8", display,
				   delete_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconMis_8), FUNC_ACTIVATE, SelectFunction, 
	  (void *)DELETE_OBJECT);
 
 IconMis_s2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				    "IconMis_s2", display,
				    NULL, NULL, XcSeparator);

 IconMis_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_5", display,
				   couper_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconMis_5), FUNC_ACTIVATE, SelectFunction, 
	  (void *)CUT_OBJECT);

 IconMis_6 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_6", display,
				   copier_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconMis_6), FUNC_ACTIVATE, SelectFunction, 
	  (void *)COPY_OBJECT);

 IconMis_7 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_7", display,
				   coller_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconMis_7), FUNC_ACTIVATE, SelectFunction, 
	  (void *)PASTE_OBJECT);

  
 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Creating Icon bar FRAME");
 /* ----------------------------------------------------------------- ** 
 ** Horizontal Icon Bars : HBAR_FRAME                                 ** 
 ** ----------------------------------------------------------------- */
 
 IconFra_0 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				   "IconFra_0", display,
				   cadre_selectionner_xpm, NULL, XcToggle);
  
 CALLBACK(WIDGET(IconFra_0), FUNC_VALUE,
	  ToggleSwitch, (void *)RESIZE_CADRE_MODE);
  
 IconFra_14 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_14", display,
				    cadre_rotate_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_14), FUNC_VALUE, ToggleSwitch, (void *)ROTATE_CADRE);
  
 IconFra_13 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_13", display,
				    cadre_scale_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_13), FUNC_VALUE, ToggleSwitch, (void *)SCALE_CADRE);
 IconFra_s1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				    "IconFra_s1", display,
				    NULL, NULL, XcSeparator);
 IconFra_1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				   "IconFra_1", display,
				   cadre_creer_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_1), FUNC_VALUE, 
	  ToggleSwitch, (void *)CREATE_CADRE_RECTANGLE);
 IconFra_10 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				    "IconFra_10", display,
				    cadre_creer_lignes_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_10), FUNC_VALUE,
	  ToggleSwitch, (void *)CREATE_CADRE_POLYGONAL);
 IconFra_12 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				    "IconFra_12", display,
				    cadre_creer_ovale_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_12), FUNC_VALUE,
	  ToggleSwitch, (void *)CREATE_CADRE_ELIPSE);
 IconFra_s2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_s2", display,
				    NULL, NULL, XcSeparator);
 IconFra_15 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_15", display,
				    cadre_add_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconFra_15), FUNC_ACTIVATE,
	  SelectFunction, (void *)ADDITION_CADRE);
 IconFra_16 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_16", display,
				    cadre_sub_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconFra_16), FUNC_ACTIVATE, 
	  SelectFunction,(void *)SUBSTRACT_CADRE);
 IconFra_11 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_11", display,
				    cadre_fusion_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconFra_11), FUNC_ACTIVATE,
	  SelectFunction, (void *)FUSION_CADRE);
 IconFra_17 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_17", display,
				    cadre_decouper_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconFra_17), FUNC_ACTIVATE,
	  SelectFunction, (void *)OUTLINE_CADRE);
 IconFra_18 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_18", display,
				    cadre_aligner_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconFra_18), FUNC_ACTIVATE, MenuFrameAlign, NULL);
 IconFra_s3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_s3", display,
				    NULL, NULL, XcSeparator);
 IconFra_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				   "IconFra_5", display,
				   cadre_bouger_point_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_5), FUNC_VALUE, 
	  ToggleSwitch, (void *)MOVE_POINT_MODE);
 IconFra_3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				   "IconFra_3", display,
				   cadre_inserer_point_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_3), FUNC_VALUE, ToggleSwitch, (void *)INSERT_POINT);
 IconFra_4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				   "IconFra_4", display,
				   cadre_detruire_point_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_4), FUNC_VALUE, ToggleSwitch, (void *)DELETE_POINT);
 IconFra_s4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_s4", display,
				    NULL, NULL, XcSeparator);
 IconFra_9 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				   "IconFra_9", display,
				   cadre_premierplan_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconFra_9), FUNC_ACTIVATE, 
	  SelectFunction, (void *)SET_HIGHEST_PRIORITY);
 IconFra_8 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				   "IconFra_8", display,
				   cadre_arriereplan_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconFra_8), FUNC_ACTIVATE,
	  SelectFunction, (void *)SET_LOWEST_PRIORITY);
 IconFra_7 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				   "IconFra_7", display,
				   cadre_approcher_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconFra_7), FUNC_ACTIVATE,
	  SelectFunction, (void *)SET_HIGHER_PRIORITY);
 IconFra_6 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				   "IconFra_6", display,
				   cadre_eloigner_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconFra_6), FUNC_ACTIVATE,
	  SelectFunction, (void *)SET_LOWER_PRIORITY);
 IconFra_s5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_s5", display,
				    NULL, NULL, XcSeparator);
 IconFra_19 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_19", display,
				    cadre_lock_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_19), FUNC_VALUE,
	  SelectFunction, (void *)LOCK_CADRE);
  
 F(TimeIndicator).advance_indicator(TimeIndicator);
  
 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Creating Icon bar TEXT");
 /* ----------------------------------------------------------------- **
 ** Horizontal Icon Bars : HBAR_TEXT                                  ** 
 ** ----------------------------------------------------------------- */
  
 IconTex_1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_TEXT]), Manager2,
				   "IconTex_1", display,
				   texte_edit_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconTex_1), FUNC_ACTIVATE, (void *)EditText, NULL);
 IconTex_s1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_TEXT]), Manager2,
				    "IconTex_s1", display,
				    NULL, NULL, XcSeparator);
 IconTex_2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_TEXT]), Manager2,
				   "IconTex_2", display,
				   liens_lier_apres_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconTex_2), FUNC_ACTIVATE, SelectFunction,
	  (void *)ADD_LINK_AT_END);
 IconTex_3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_TEXT]), Manager2,
				   "IconTex_3", display,
				   liens_lier_avant_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconTex_3), FUNC_ACTIVATE, SelectFunction,
	  (void *)ADD_LINK_AT_BEGINNING);
 IconTex_4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_TEXT]), Manager2,
				   "IconTex_4", display,
				   liens_inserer_avant_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconTex_4), FUNC_ACTIVATE, SelectFunction,
	  (void *)INSERT_LINK_BEFORE);
 IconTex_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_TEXT]), Manager2,
				   "IconTex_5", display,
				   liens_inserer_apres_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconTex_5), FUNC_ACTIVATE, SelectFunction,
	  (void *)INSERT_LINK_BEHIND);
 IconTex_6 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_TEXT]), Manager2,
				   "IconTex_6", display,
				   liens_extraire_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconTex_6), FUNC_ACTIVATE, SelectFunction,
	  (void *)DELETE_OBJECT);
    
 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Creating Icon bar IMAGE");
 /* ----------------------------------------------------------------------- */
 /* Horizontal Icon Bars : HBAR_IMAGE                                       */
 /* ----------------------------------------------------------------------- */
 
 IconIma_1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_IMAGE]), Manager2, 
				   "IconIma_1", display,
				   image_deplacer_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconIma_1), FUNC_VALUE, ToggleSwitch, (void *)MOVE_IMAGE);

 IconIma_2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_IMAGE]), Manager2, 
				   "IconIma_2", display,
				   image_center_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconIma_2), FUNC_ACTIVATE,
	  SelectFunction, (void *)CENTER_IMAGE);
 IconIma_3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_IMAGE]), Manager2, 
				   "IconIma_3", display,
				   image_default_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconIma_3), FUNC_ACTIVATE, 
	  SelectFunction, (void *)INIT_IMAGE);

 IconIma_s1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_IMAGE]), Manager2,
				    "IconIma_s1", display,
				    NULL, NULL, XcSeparator);
 IconIma_4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_IMAGE]), Manager2, 
				   "IconIma_4", display,
				   image_flip_vertical_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconIma_4), FUNC_ACTIVATE,
	  SelectFunction, (void *)FLIP_VERTICAL_IMAGE);
 IconIma_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_IMAGE]), Manager2, 
				   "IconIma_5", display,
				   image_flip_horizontal_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconIma_5), FUNC_ACTIVATE,
	  SelectFunction, (void *)FLIP_HORIZONTAL_IMAGE);
  
 IconIma_s2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_IMAGE]), Manager2,
				    "IconIma_s2", display,
				    NULL, NULL, XcSeparator);

 IconIma_6 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_IMAGE]), Manager2, 
				   "IconIma_6", display,
				   image_normal_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconIma_6), FUNC_VALUE,
	  ToggleImagemodeSwitch, (void *)NORMAL_ASPECT_IMAGE);
 IconIma_7 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_IMAGE]), Manager2, 
				   "IconIma_7", display,
				   image_bestaspect_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconIma_7), FUNC_VALUE,
	  ToggleImagemodeSwitch, (void *)BEST_ASPECT_IMAGE);
 IconIma_8 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_IMAGE]), Manager2, 
				   "IconIma_8", display,
				   image_maxaspect_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconIma_8), FUNC_VALUE,
	  ToggleImagemodeSwitch, (void *)MAX_ASPECT_IMAGE);

 IconIma_9 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_IMAGE]), Manager2, 
				   "IconIma_9", display,
				   image_autoresize_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconIma_9), FUNC_VALUE,
	  ToggleImagemodeSwitch, (void *)AUTO_RESIZE_IMAGE);
  
 IconVec_s1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_IMAGE]), Manager2,
				    "IconVec_s1", display,
				    NULL, NULL, XcSeparator);
 IconVec_1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_IMAGE]), Manager2, 
				   "IconVec_1", display,
				   vector_fonts_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconVec_1), FUNC_VALUE,
	  SelectFunction, (void *)TOGGLE_TEXT_VECTOR);

 F(TimeIndicator).advance_indicator(TimeIndicator);

 F(MainInterface1).set_loadingMessage(MainInterface1, 
				      "Creating Icon bar ZOOM");
 /* ----------------------------------------------------------------- ** 
 ** Horizontal Icon Bars : HBAR_ZOOM                                  ** 
 ** ----------------------------------------------------------------- */
  
 IconZoo_1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_1", display,
				   loupe_avant_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconZoo_1), FUNC_VALUE,
	  ToggleSwitch, (void *)ZOOM_PLUS_PAGE);
 IconZoo_2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_2", display, 
				   loupe_arriere_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconZoo_2), FUNC_VALUE, 
	  ToggleSwitch, (void *)ZOOM_MINUS_PAGE);
 IconZoo_s1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				    "IconZoo_s1", display,
				    NULL, NULL, XcSeparator);
 IconZoo_3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_3", display,
				   loupe_zone_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconZoo_3), FUNC_VALUE, ToggleSwitch, (void *)ZOOM_IN_ZONE);
 IconZoo_4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_4", display,
				   loupe_largeur_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_4), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_FIT_IN_WIDTH);
 IconZoo_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_5", display,
				   loupe_entier_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_5), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_FIT_IN_SCREEN);
 IconZoo_12 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				    "IconZoo_12", display,
				    loupe_reel_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_12), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_AT_REAL_SIZE);
 IconZoo_s2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2,
				    "IconZoo_s2", display,
				    NULL, NULL, XcSeparator);
 sbZoom = (c_SpinBox *)NEW(c_SpinBox)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2,
				      "sbZoom", XcSB_PLUS_MINUS, 
				      TF_REAL, 10.0, 400.0, "100", 5);
 F(sbZoom).setUnit(sbZoom, XcUB_RATIO_PERCENTAGE, "%.0f", TRUE, FALSE);
 F(sbZoom).addCallback(sbZoom, (void *)certify_zoom, NULL, 
		       SB_TEXTFIELD_CALLBACK);
 F(sbZoom).setValueTable
  (sbZoom, (real)SCALE_FROM_PERCENT(10), (real)SCALE_FROM_PERCENT(25), 
   (real)SCALE_FROM_PERCENT(50), (real)SCALE_FROM_PERCENT(75), 
   (real)SCALE_FROM_PERCENT(100), (real)SCALE_FROM_PERCENT(150), 
   (real)SCALE_FROM_PERCENT(200), (real)SCALE_FROM_PERCENT(300), 
   (real)SCALE_FROM_PERCENT(400), SB_VALUE_TABLE_END);
 F(sbZoom).setAutoRepeatDelay(sbZoom, SB_DEFAULT_AUTO_REPEAT);
 XtAddCallback(WIDGET(WTEXTFIELD(sbZoom)), XmNfocusCallback,
	       (void *)cb_focus_tfZoom, (void *)NULL);
 
 IconZoo_s3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2,
				    "IconZoo_s3", display,
				    NULL, NULL, XcSeparator);
 IconZoo_6 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_6", display,
				   loupe_25_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_6), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_25_PAGE);
 IconZoo_7 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_7", display,
				   loupe_50_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_7), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_50_PAGE);
 IconZoo_8 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_8", display,
				   loupe_75_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_8), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_75_PAGE);
 IconZoo_9 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_9", display,
				   loupe_100_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_9), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_100_PAGE);
 IconZoo_10 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				    "IconZoo_10", display,
				    loupe_200_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_10), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_200_PAGE);
 IconZoo_11 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				    "IconZoo_11", display,
				    loupe_400_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_11), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_400_PAGE);
  
 F(MainInterface1).set_loadingMessage(MainInterface1, "Initialize Icon bars");
 F(TimeIndicator).advance_indicator(TimeIndicator);

 /* ----------------------------------------------------------------- ** 
 ** Initialize Icon Bars                                              ** 
 ** ----------------------------------------------------------------- */
 i = 0;
 toggletable[i].tog_wid = WIDGET(IconFra_0);
 toggletable[i].func_num = RESIZE_CADRE_MODE;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_14);
 toggletable[i].func_num = ROTATE_CADRE;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_13);
 toggletable[i].func_num = SCALE_CADRE;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_1);
 toggletable[i].func_num = CREATE_CADRE_RECTANGLE;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_10);
 toggletable[i].func_num =  CREATE_CADRE_POLYGONAL;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_12);
 toggletable[i].func_num = CREATE_CADRE_ELIPSE;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_5);
 toggletable[i].func_num = MOVE_POINT_MODE;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_3);
 toggletable[i].func_num = INSERT_POINT;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_4);
 toggletable[i].func_num = DELETE_POINT;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_0);
 toggletable[i].func_num = SELECT_MOVE_RESIZE_CADRE;
 i++;
 toggletable[i].tog_wid = WIDGET(IconIma_1);
 toggletable[i].func_num = MOVE_IMAGE;
 i++;
 toggletable[i].tog_wid = WIDGET(IconIma_1);
 toggletable[i].func_num = MOVE_VECTOR;
 i++;
 toggletable[i].tog_wid = WIDGET(IconZoo_1);
 toggletable[i].func_num = ZOOM_PLUS_PAGE;
 i++;
 toggletable[i].tog_wid = WIDGET(IconZoo_2);
 toggletable[i].func_num = ZOOM_MINUS_PAGE;
 i++;
 toggletable[i].tog_wid = WIDGET(IconZoo_3);
 toggletable[i].func_num = ZOOM_IN_ZONE;
 i++;
 toggletable[i].tog_wid = 0;
 toggletable[i].func_num = 0;

 i = 0;
 toggleimagemodetable[i].tog_wid = WIDGET(IconIma_6);
 toggleimagemodetable[i].func_num = NORMAL_ASPECT_IMAGE;
 toggleimagemodetable[i].mode_num = ZOOM_NORMAL_MODE;
 i++;
 toggleimagemodetable[i].tog_wid = WIDGET(IconIma_7);
 toggleimagemodetable[i].func_num = BEST_ASPECT_IMAGE;
 toggleimagemodetable[i].mode_num = ZOOM_BEST_ASPECT_MODE;
 i++;
 toggleimagemodetable[i].tog_wid = WIDGET(IconIma_8);
 toggleimagemodetable[i].func_num = MAX_ASPECT_IMAGE;
 toggleimagemodetable[i].mode_num = ZOOM_MAX_ASPECT_MODE;
 i++;
 toggleimagemodetable[i].tog_wid = WIDGET(IconIma_9);
 toggleimagemodetable[i].func_num = AUTO_RESIZE_IMAGE;
 toggleimagemodetable[i].mode_num = ZOOM_AUTO_RESIZE_MODE;
 i++;
 toggleimagemodetable[i].tog_wid = NULL;
 toggleimagemodetable[i].func_num = -1;
 toggleimagemodetable[i].mode_num = ZOOM_NORMAL_MODE;

 i = 0;
 togglevectormodetable[i].tog_wid = WIDGET(IconIma_6);
 togglevectormodetable[i].func_num = NORMAL_ASPECT_VECTOR;
 togglevectormodetable[i].mode_num = XcVG_NORMAL;
 i++;
 togglevectormodetable[i].tog_wid = WIDGET(IconIma_7);
 togglevectormodetable[i].func_num = BEST_ASPECT_VECTOR;
 togglevectormodetable[i].mode_num = XcVG_BEST_ASPECT;
 i++;
 togglevectormodetable[i].tog_wid = WIDGET(IconIma_8);
 togglevectormodetable[i].func_num = MAX_ASPECT_VECTOR;
 togglevectormodetable[i].mode_num = XcVG_MAX_ASPECT;
 i++;
 togglevectormodetable[i].tog_wid = WIDGET(IconIma_9);
 togglevectormodetable[i].func_num = AUTO_RESIZE_VECTOR;
 togglevectormodetable[i].mode_num = XcVG_AUTOSIZE;
 i++;
 togglevectormodetable[i].tog_wid = NULL;
 togglevectormodetable[i].func_num = -1;
 togglevectormodetable[i].mode_num = XcVG_NORMAL;

 F(Manager1).Manage_all(Manager2, NULL); 

 F(TimeIndicator).advance_indicator(TimeIndicator);
  
 for (i = 0; i < NB_HBAR; i++)
 {
  AttachHBar(hIconBarForm, hIconBar[i]);
  XtUnmanageChild(WIDGET(hIconBar[i]));
 }

 XtManageChild(WIDGET(hIconBar[bar_selected]));

 F(TimeIndicator).advance_indicator(TimeIndicator);
  
  
 /* ----------------------------------------------------------------- ** 
 ** Trashcan Icon                                                     ** 
 ** ----------------------------------------------------------------- */
 IconTra = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_2), Manager1,
				 "IconTra", display, 
				 trash_xpm, NULL, XcPush);


 /* ----------------------------------------------------------------- ** 
 ** Bottom Bar                                                        ** 
 ** ----------------------------------------------------------------- */

 BottomBar = (c_Form *)NEW(c_Form)(WIDGET(MainInterface1), Manager1,
				   "BottomBar", 32, 32);
  
 IconBottom = (c_Icon *)NEW(c_Icon)(WIDGET(BottomBar), Manager1,
				    "IconBottom", display, 
				    name_emboss_xpm, NULL, XcLabel);
  
 GlobHelpLine = (c_HelpLine *)NEW(c_HelpLine)(XcHL_BUILD_LABEL,
					      WIDGET(BottomBar),
					      Manager1,     
					      "Bottom Security");
 F(GlobHelpLine).change_function_message(GlobHelpLine, XcR_NONE, XcR_DEFAULT);

 F(BottomBar).attach_top(WIDGET(BottomBar), WIDGET(IconBottom), NULL);
 F(BottomBar).attach_bottom(WIDGET(BottomBar), WIDGET(IconBottom), NULL);
 F(BottomBar).attach_left(WIDGET(BottomBar), WIDGET(IconBottom), NULL);
  
 F(BottomBar).attach_top(WIDGET(BottomBar), WIDGET(GlobHelpLine), NULL);
 F(BottomBar).attach_bottom(WIDGET(BottomBar), WIDGET(GlobHelpLine), NULL);
 F(BottomBar).attach_left(WIDGET(BottomBar), WIDGET(GlobHelpLine), 
			  WIDGET(IconBottom));
 F(BottomBar).attach_right(WIDGET(BottomBar), WIDGET(GlobHelpLine), NULL);
  
 /* ------------------------------------------------------------------- */
  
 MainDocument = (c_DrawingArea *)NEW(c_DrawingArea)(WIDGET(MainInterface1), 
						Manager1,
						"MainDocument", 
						150, 150);  

 F(TimeIndicator).advance_indicator(TimeIndicator);
  
 nargs = 0;
 XtSetArg(args[nargs], XmNshadowType, XmSHADOW_OUT );
 nargs++;
 w_FrameOfClipBoard = XmCreateFrame(WIDGET(MainInterface1), 
				    "FrameOfClipboard", args, nargs);
  
 F(MainInterface1).set_loadingMessage(MainInterface1, "Creating Helplines");
 /* ----------------------------------------------------------------- ** 
 ** Help Line definition for Icons                                    ** 
 ** ----------------------------------------------------------------- */

 /*--- MISC ---*/
 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_MISC]),helpMisc, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconMis_1),helpMiscNew, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconMis_2),helpMiscOpen, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconMis_3),helpMiscSave, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconMis_4),helpMiscPrint, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconMis_5),helpMiscCut, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconMis_6),helpMiscCopy, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconMis_7),helpMiscPaste, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconMis_8),helpMiscDelete, XcR_NONE);
  
 /*--- CADRES ---*/
 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_FRAME]),helpFrame, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_0),helpFrameSelect, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_14),helpFrameRotate, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_13),helpFrameScale, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_1),helpFrameSquare, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_10),helpFramePolygon, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_12),helpFrameCircle, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_15),helpFrameAdd, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_16),helpFrameSub, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_11),helpFrameXor, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_17),helpFrameOutline, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_18),helpFrameAlign, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_5),helpFrameMovePoint, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_3),helpFrameAddPoint, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_4),helpFrameSubPoint, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_9),helpFramePlanFirst, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_8),helpFramePlanLast, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_7),helpFramePlanUp, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_6),helpFramePlanDown, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_19),helpFrameLock, XcR_NONE);
  
 /*--- TEXTE ---*/
 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_TEXT]),helpText, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconTex_1),helpTextEditor, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconTex_2),helpTextLinkEnd, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconTex_3),helpTextLinkBegin, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconTex_4),helpTextInsertBefore, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconTex_5),helpTextInsertAfter, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconTex_6),helpTextUnlink, XcR_NONE);
  
 /*--- IMAGES ---*/
 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_IMAGE]),helpPicture, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_1),helpPictureMove, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_2),helpPictureCenter, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_3),helpPictureReset, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_4),helpPictureVFlip, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_5),helpPictureHFlip, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_7),helpPictureBest, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_8),helpPictureMax, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_9),helpPictureStretch, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconVec_1),helpVectorTexts, XcR_NONE);

 /*--- ZOOM ---*/
 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_ZOOM]),helpZoom, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_1),helpZoomIn, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_2),helpZoomOut, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_3),helpZoomZone, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_4),helpZoomWidth, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_5),helpZoomFull, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_12),helpZoomReal, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_6),helpZoom25, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_7),helpZoom50, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_8),helpZoom75, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_9),helpZoom100, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_10),helpZoom200, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_11),helpZoom400, XcR_NONE);
  
 SET_HELPLINE(GlobHelpLine, WIDGET(IconTra),helpTrash, XcR_NONE);
  
 /*
   F(Manager1).Add_child(Manager1, w_FrameOfClipBoard, WIDGET(MainInterface1));
   */
  
 /* 
   ClipBoard = (c_PanedWindow *)NEW(c_PanedWindow)(w_FrameOfClipBoard,
   Manager1,
   "Clipboard");
   */
 ClipBoardPrivate =
  (c_ScrolledWindow *)NEW(c_ScrolledWindow)(w_FrameOfClipBoard,
					    Manager1,
					    "ClipboardPrivate");
  
 Draw3 = (c_RowColumn *)NEW(c_RowColumn)(WIDGET(ClipBoardPrivate),
					 Manager1,
					 "PrivateClipArea",
					 75, 5000, XmVERTICAL);
 ClipIcon = (c_ClipIcon *)NEW(c_ClipIcon)(display, 
					  WIDGET(Draw3),
					  High_Client);
  
 Bar1 = (c_MenuBar *)NEW(c_MenuBar)(WIDGET(MainInterface1), 
				    Manager1, "Menu");
  
 F(TimeIndicator).advance_indicator(TimeIndicator);

 /*--- Read key ---*/
 F(MainInterface1).set_loadingMessage(MainInterface1, "Reading key");
 vkey = NULL;
 XcKEY_OK = 0;
 read_key_file(&vkey);
 bzero(&gkinfo, sizeof(gkinfo));
 if (vkey)
 {
  if (get_info_from_key(&gkinfo, vkey) != -1)
  {
   if (check_key_validity(&gkinfo, 100,
			  MainInterface1->display_name))
    XcKEY_OK = 1;
  }
  else
   gkinfo.valid = Xc_KEY_NOT_VALID;

  Xc_free(vkey);
 }
 else
  gkinfo.valid = Xc_KEY_NO_KEY;

#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
 F(MainInterface1).set_title(MainInterface1);
#endif

 F(MainInterface1).set_loadingMessage(MainInterface1, 
				      "Creating Pulldown menus");

 /* ----------------------------------------------------------------- ** 
 ** Menu FILE                                                         ** 
 ** ----------------------------------------------------------------- */
 
 MenuFil = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1, 
		   "pmFile",
		   "mNew", MFil_NEW,
		   "mOpen", MFil_OPEN,
		   MENUSEPARATOR, MFil_SEP1,
		   "mClose", MFil_CLOSE,
		   "mSave", MFil_SAVE,
		   "mSaveAs", MFil_SAVEAS,
		   "mSaveAll", MFil_SAVEALL,
		   MENUSEPARATOR, MFil_SEP2,
		   SUBMENUON, MFil_SUBMENU1,
		   "mImport", MFil_IMPORT,
		   "mImportImage", MFil_IMPORTIMAGE,
		   "mImportText", MFil_IMPORTTEXT,
		   "mImportVector", MFil_IMPORTVECTOR,
		   SUBMENUOFF, Mxxx_NOTHING,
		   MENUSEPARATOR, MFil_SEP3,
		   "mPrint", MFil_PRINT,
		   MENUSEPARATOR, MFil_SEP4,
		   "mQuit", MFil_QUIT,
		   NULL);
    
 F(MenuFil).AddCallback(MenuFil, MFil_NEW, MenuFileNew, NULL);
 F(MenuFil).AddCallback(MenuFil, MFil_OPEN, MenuFileOpen, NULL);
 F(MenuFil).AddCallback(MenuFil, MFil_CLOSE, MenuFileClose, NULL);
 F(MenuFil).AddCallback(MenuFil, MFil_SAVE, MenuFileSave, NULL);
 F(MenuFil).AddCallback(MenuFil, MFil_SAVEAS, MenuFileSaveas, NULL);
 F(MenuFil).AddCallback(MenuFil, MFil_SAVEALL, MenuFileSaveall, NULL);

 F(MenuFil).AddCallback(MenuFil, MFil_IMPORTIMAGE, LoadImage, NULL);
 F(MenuFil).AddCallback(MenuFil, MFil_IMPORTTEXT, LoadText, NULL);
 F(MenuFil).AddCallback(MenuFil, MFil_IMPORTVECTOR, LoadVector, NULL);
 F(MenuFil).AddCallback(MenuFil, MFil_PRINT, MenuFilePrint, NULL);
 F(MenuFil).AddCallback(MenuFil, MFil_QUIT, MenuFileQuit, NULL);
 XtAddCallback(WIDGET(MenuFil), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuFil), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);
 
 F(TimeIndicator).advance_indicator(TimeIndicator);
 
 /* ----------------------------------------------------------------- ** 
 ** Menu EDIT                                                         ** 
 ** ----------------------------------------------------------------- */

 MenuEdi = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1, 
		   "pmEdit",
		   "mCut", MEdi_CUT,
		   "mCopy", MEdi_COPY,
		   "mPaste", MEdi_PASTE,
		   MENUSEPARATOR, MEdi_SEP1,
		   "mDelete", MEdi_DELETE,
		   "mSelectAll", MEdi_SELECTALL,
		   MENUSEPARATOR, MEdi_SEP2,
		   "mColors", MEdi_EDITCOLORS,
		   "mStyles", MEdi_EDITSTYLES,
		   "mRulers", MEdi_EDITRULERS,
		   MENUSEPARATOR, MEdi_SEP3,
		   SUBMENUON, MEdi_SUBMENU1,
		   "mPreferences", MEdi_PREFERENCES,
		   "mCache", MEdi_IMAGECACHE,
		   SUBMENUOFF, Mxxx_NOTHING,
		   NULL);
  
  
 F(MenuEdi).AddCallback(MenuEdi, MEdi_CUT, SelectFunction,
			(void *)CUT_OBJECT);
 F(MenuEdi).AddCallback(MenuEdi, MEdi_COPY, SelectFunction,
			(void *)COPY_OBJECT);
 F(MenuEdi).AddCallback(MenuEdi, MEdi_PASTE, SelectFunction, 
			(void *)PASTE_OBJECT);
 F(MenuEdi).AddCallback(MenuEdi, MEdi_DELETE,
			SelectFunction, (void *)DELETE_OBJECT);
 F(MenuEdi).AddCallback(MenuEdi , MEdi_SELECTALL,
			SelectFunction, (void *)SELECT_ALL_CADRE);
 F(MenuEdi).AddCallback(MenuEdi, MEdi_EDITCOLORS, MenuEditColors, NULL);
 F(MenuEdi).AddCallback(MenuEdi, MEdi_EDITSTYLES, MenuEditStyles, NULL);
 F(MenuEdi).AddCallback(MenuEdi, MEdi_EDITRULERS, MenuEditRulers, NULL);
 F(MenuEdi).AddCallback(MenuEdi, MEdi_IMAGECACHE, MenuEditCache, NULL);
  
 XtAddCallback(WIDGET(MenuEdi), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuEdi), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);

 F(TimeIndicator).advance_indicator(TimeIndicator);

 /* ----------------------------------------------------------------- ** 
 ** Menu DISPLAY                                                      ** 
 ** ----------------------------------------------------------------- */

 PageZoom = (real)SCALE_FROM_PERCENT(100.0);

 MenuDis = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1, 
		   "pmDisplay",
		   "mRedraw", MDis_REDRAWALL,
		   "mRealSize", MDis_REALSIZE,
		   "mScreenSize", MDis_SCREENSIZE,
		   "mFullWidth", MDis_FULLWIDTH,
		   MENUSEPARATOR, MDis_SEP1,
		   TOGGLEMODEON, Mxxx_NOTHING,
		   "mShowRulers", MDis_SHOWRULERS,
		   "mShowClipboard", MDis_SHOWCLIPBOARD,
		   "mShowInfobar", MDis_SHOWBOTTOMBAR,
		   MENUSEPARATOR, MDis_SEP2,
		   "mShowGrid", MDis_SHOWGRID,
		   "mShowHelplines", MDis_SHOWTYPOMARKS,
		   "mShowAlignlines", MDis_SHOWRULERSMARKS,
		   "mShowFrameBorders", MDis_SHOWFRAMESBORDER,
		   MENUSEPARATOR, MDis_SEP3,
		   "mActivMagnet", MDis_MAGNETON,
		   TOGGLEMODEOFF, Mxxx_NOTHING,
		   NULL);
  
 F(MenuDis).AddCallback(MenuDis, MDis_REDRAWALL, 
			SelectFunction, (void *)REDRAW_ALL);
 F(MenuDis).AddCallback(MenuDis, MDis_REALSIZE, 
			SelectFunction, (void *)DOC_ZOOM_AT_REAL_SIZE);
 F(MenuDis).AddCallback(MenuDis, MDis_SCREENSIZE,
			SelectFunction, (void *)DOC_ZOOM_FIT_IN_SCREEN);
 F(MenuDis).AddCallback(MenuDis, MDis_FULLWIDTH,
			SelectFunction, (void *)DOC_ZOOM_FIT_IN_WIDTH);
 F(MenuDis).AddCallback(MenuDis, MDis_SHOWGRID,
			SelectFunction, (void *)DOC_DISPLAY_GRID);
 F(MenuDis).AddCallback(MenuDis, MDis_SHOWTYPOMARKS,
			SelectFunction, (void *)DOC_DISPLAY_TYPO_MARK);
 F(MenuDis).AddCallback(MenuDis, MDis_SHOWRULERSMARKS,
			SelectFunction, (void *)DOC_DISPLAY_ALIGN_MARK);
 F(MenuDis).AddCallback(MenuDis, MDis_SHOWFRAMESBORDER,
			SelectFunction, (void *)DOC_DISPLAY_FRAME_BORDER);
 F(MenuDis).AddCallback(MenuDis, MDis_SHOWRULERS, 
			SelectFunction, (void *)DISPLAY_REGLE);
 F(MenuDis).AddCallback(MenuDis, MDis_SHOWCLIPBOARD,
			DisplayClipboard, NULL);
 F(MenuDis).AddCallback(MenuDis, MDis_SHOWBOTTOMBAR,
			DisplayBottomBar, NULL);
 F(MenuDis).AddCallback(MenuDis, MDis_MAGNETON,
			SelectFunction, (void *)DOC_MAGNETIZE_ALL);
 XtAddCallback(WIDGET(MenuDis), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuDis), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);

 F(MenuDis).ToggleButton(MenuDis, MDis_SHOWBOTTOMBAR, True);

 F(TimeIndicator).advance_indicator(TimeIndicator);
  
 /* ----------------------------------------------------------------- ** 
 ** Menu PAGE                                                         ** 
 ** ----------------------------------------------------------------- */
  
 MenuPge = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1, 
		   "pmPage",
		   "mInsertBefore", MPge_INSERTPAGEBEFORE,
		   "mInsertAfter", MPge_INSERTPAGEAFTER,
		   "mModify", MPge_MODIFYPAGE,
		   "mDelete", MPge_DELETEPAGE,
		   MENUSEPARATOR, MPge_SEP1,
		   "mGridSetup", MPge_GRIDSETUP,
		   MENUSEPARATOR, MPge_SEP2,
		   SUBMENUON, MPge_SUBMENU1,
		   "mPageZoom", MPge_PAGEZOOM,
		   "mRealSize", MPge_REALSIZE,
		   "mScreenSize", MPge_SCREENSIZE,
		   "mFullWidth", MPge_FULLWIDTH,
		   MENUSEPARATOR, MPge_SEP3,
		   "mZoom25", MPge_ZOOM25,
		   "mZoom50", MPge_ZOOM50,
		   "mZoom75", MPge_ZOOM75,
		   "mZoom100", MPge_ZOOM100,
		   "mZoom200", MPge_ZOOM200,
		   "mZoom400", MPge_ZOOM400,
		   SUBMENUOFF, Mxxx_NOTHING,
		   SUBMENUON, MPge_SUBMENU2,
		   "mPageDisplay", MPge_PAGEDISPLAY,
		   TOGGLEMODEON, Mxxx_NOTHING,
		   "mShowGrid", MPge_SHOWGRID,
		   "mShowHelplines", MPge_SHOWTYPOMARKS,
		   "mShowAlignlines", MPge_SHOWRULERSMARKS,
		   "mShowFrameBorders", MPge_SHOWFRAMESBORDER,
		   MENUSEPARATOR, MPge_SEP4,
		   "mActivMagnet", MPge_MAGNETON,
		   TOGGLEMODEOFF, Mxxx_NOTHING,
		   SUBMENUOFF, Mxxx_NOTHING,
		   NULL);
 
 F(MenuPge).AddCallback(MenuPge, MPge_INSERTPAGEBEFORE, SelectFunction, 
			(void *)DOC_INSERT_PAGE_BEFORE);
 F(MenuPge).AddCallback(MenuPge, MPge_INSERTPAGEAFTER, SelectFunction, 
			(void *)DOC_INSERT_PAGE_AFTER);
 F(MenuPge).AddCallback(MenuPge, MPge_MODIFYPAGE, SelectFunction, 
			(void *)DOC_MODIFY_PAGE);
 F(MenuPge).AddCallback(MenuPge, MPge_DELETEPAGE, SelectFunction, 
			(void *)DOC_DELETE_PAGE);
 F(MenuPge).AddCallback(MenuPge, MPge_GRIDSETUP, MenuPageGridSetup, NULL);
 F(MenuPge).AddCallback(MenuPge, MPge_REALSIZE, 
			SelectFunction, (void *)ZOOM_AT_REAL_SIZE);
 F(MenuPge).AddCallback(MenuPge, MPge_SCREENSIZE,
			SelectFunction, (void *)ZOOM_FIT_IN_SCREEN);
 F(MenuPge).AddCallback(MenuPge, MPge_FULLWIDTH,
			SelectFunction, (void *)ZOOM_FIT_IN_WIDTH);
 F(MenuPge).AddCallback(MenuPge, MPge_ZOOM25,
			SelectFunction, (void *)ZOOM_25_PAGE);
 F(MenuPge).AddCallback(MenuPge, MPge_ZOOM50,
			SelectFunction, (void *)ZOOM_50_PAGE);
 F(MenuPge).AddCallback(MenuPge, MPge_ZOOM75,
			SelectFunction, (void *)ZOOM_75_PAGE);
 F(MenuPge).AddCallback(MenuPge, MPge_ZOOM100,
			SelectFunction, (void *)ZOOM_100_PAGE);
 F(MenuPge).AddCallback(MenuPge, MPge_ZOOM200,
			SelectFunction, (void *)ZOOM_200_PAGE);
 F(MenuPge).AddCallback(MenuPge, MPge_ZOOM400,
			SelectFunction, (void *)ZOOM_400_PAGE);
 F(MenuPge).AddCallback(MenuPge, MPge_SHOWGRID,
			SelectFunction, (void *)DISPLAY_GRID);
 F(MenuPge).AddCallback(MenuPge, MPge_SHOWTYPOMARKS,
			SelectFunction, (void *)DISPLAY_TYPO_MARK);
 F(MenuPge).AddCallback(MenuPge, MPge_SHOWRULERSMARKS,
			SelectFunction, (void *)DISPLAY_ALIGN_MARK);
 F(MenuPge).AddCallback(MenuPge, MPge_SHOWFRAMESBORDER,
			SelectFunction, (void *)DISPLAY_FRAME_BORDER);
 F(MenuPge).AddCallback(MenuPge, MPge_MAGNETON,
			SelectFunction, (void *)MAGNETIZE_ALL);
  
 XtAddCallback(WIDGET(MenuPge), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuPge), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);
 
 /* ----------------------------------------------------------------- ** 
 ** Menu FRAME                                                        ** 
 ** ----------------------------------------------------------------- */
 
 MenuFra = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1),Manager1, 
		   "pmFrame",
		   "mDrawBar", MFra_CREATERECTANGLE,
		   "mDrawPolygon", MFra_CREATEPOLYGON,
		   "mDrawEllipse", MFra_CREATEELIPSE,
		   MENUSEPARATOR, MFra_SEP1, 
		   "mPointInsert", MFra_POINTINSERT,
		   "mPointDelete", MFra_POINTDELETE,
		   "mPointMove", MFra_POINTMOVE,
		   MENUSEPARATOR, MFra_SEP2,
		   SUBMENUON, MFra_SUBMENU1,
		   "mPlaneChange", MFra_PLANECHANGE,
		   "mMoveDown", MFra_MOVEDOWN,
		   "mMoveUp", MFra_MOVEUP,
		   "mBackground", MFra_BACKGROUND,
		   "mForeground", MFra_FOREGROUND,
		   SUBMENUOFF, Mxxx_NOTHING,
		   MENUSEPARATOR, MFra_SEP3,
		   SUBMENUON, MFra_SUBMENU2,
		   "mFrameOperation", MFra_FRAMEOPERATION,
		   "mFrameAdd", MFra_FRAMEADD,
		   "mFrameSub", MFra_FRAMESUB,
		   "mFrameXor", MFra_FRAMEXOR,
		   "mFrameCut", MFra_FRAMECUT,
		   SUBMENUOFF, Mxxx_NOTHING,
		   "mAlign", MFra_ALIGN,
		   MENUSEPARATOR, MFra_SEP4,
		   TOGGLEMODEON, Mxxx_NOTHING,
		   "mLock", MFra_LOCK,
		   TOGGLEMODEOFF, Mxxx_NOTHING,
		   NOMANAGE, Mxxx_NOTHING,
		   "mUnlock", MFra_UNLOCK,
		   MENUSEPARATOR, MFra_SEP5,
		   "mRotate", MFra_ROTATE,
		   "mScale", MFra_SCALE,
		   "mAttribs", MFra_ATTRIBS,
		   "mFrameDelete", MFra_FRAMEDELETE,
		   NULL);


 F(MenuFra).AddCallback(MenuFra, MFra_CREATERECTANGLE, SelectFunction,
			(void *)CREATE_CADRE_RECTANGLE);
 F(MenuFra).AddCallback(MenuFra, MFra_CREATEPOLYGON, SelectFunction, 
			(void *)CREATE_CADRE_POLYGONAL);
 F(MenuFra).AddCallback(MenuFra, MFra_CREATEELIPSE, SelectFunction, 
			(void *)CREATE_CADRE_ELIPSE);
 F(MenuFra).AddCallback(MenuFra, MFra_POINTINSERT, SelectFunction, 
			(void *)INSERT_POINT);
 F(MenuFra).AddCallback(MenuFra, MFra_POINTDELETE, SelectFunction, 
			(void *)DELETE_POINT);
 F(MenuFra).AddCallback(MenuFra, MFra_POINTMOVE, SelectFunction, 
			(void *)MOVE_POINT_MODE);
 F(MenuFra).AddCallback(MenuFra, MFra_MOVEDOWN, SelectFunction, 
			(void *)SET_LOWER_PRIORITY);
 F(MenuFra).AddCallback(MenuFra, MFra_MOVEUP, SelectFunction, 
			(void *)SET_HIGHER_PRIORITY);
 F(MenuFra).AddCallback(MenuFra, MFra_BACKGROUND, SelectFunction,
			(void *)SET_LOWEST_PRIORITY);
 F(MenuFra).AddCallback(MenuFra, MFra_FOREGROUND, SelectFunction, 
			(void *)SET_HIGHEST_PRIORITY);
 F(MenuFra).AddCallback(MenuFra, MFra_FRAMEADD, SelectFunction, 
			(void *)ADDITION_CADRE);
 F(MenuFra).AddCallback(MenuFra, MFra_FRAMESUB, SelectFunction, 
			(void *)SUBSTRACT_CADRE);
 F(MenuFra).AddCallback(MenuFra, MFra_FRAMEXOR, SelectFunction, 
			(void *)FUSION_CADRE);
 F(MenuFra).AddCallback(MenuFra, MFra_FRAMECUT, SelectFunction, 
			(void *)OUTLINE_CADRE);
 F(MenuFra).AddCallback(MenuFra, MFra_ALIGN, MenuFrameAlign, NULL);
 F(MenuFra).AddCallback(MenuFra, MFra_LOCK, SelectFunction, 
			(void *)LOCK_CADRE);
 F(MenuFra).AddCallback(MenuFra, MFra_UNLOCK, SelectFunction, 
			(void *)LOCK_CADRE);
 F(MenuFra).AddCallback(MenuFra, MFra_FRAMEDELETE, SelectFunction, 
			(void *)DELETE_SELECTED_CADRE);
 F(MenuFra).AddCallback(MenuFra, MFra_ROTATE, SelectFunction, 
			(void *)ROTATE_CADRE);
 F(MenuFra).AddCallback(MenuFra, MFra_SCALE, SelectFunction, 
			(void *)SCALE_CADRE);
 F(MenuFra).AddCallback(MenuFra, MFra_ATTRIBS, MenuFrameAttribs, NULL);

 XtAddCallback(WIDGET(MenuFra), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuFra), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);


 /* ----------------------------------------------------------------- ** 
 ** Menu WINDOWS                                                      ** 
 ** ----------------------------------------------------------------- */
 
 MenuWin = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1, 
		   "pmWindows",
		   "mCascade", MWin_CASCADE,
		   "mTile", MWin_TILE,
		   MENUSEPARATOR, MWin_SEP1,
		   RADIOBOXON, 0,
		   DYNAMIC, MWin_DYNAMICDOCLIST,
		   0, DL_DOC_LIST_LENGTH,
		   NULL);
  
 F(MenuWin).AddCallback(MenuWin, MWin_CASCADE, MenuWindowArrange, NULL);
 F(MenuWin).AddCallback(MenuWin, MWin_TILE, MenuWindowTile, NULL);

 XtAddCallback(WIDGET(MenuWin), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuWin), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);

 /* ----------------------------------------------------------------- ** 
 ** Menu HELP                                                         ** 
 ** ----------------------------------------------------------------- */

 MenuHel = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1, 
		   "pmHelp",
		   "mOverview", MHel_OVERVIEW,
		   "mContent", MHel_CONTENT,
		   "mKeyboard", MHel_KEYBOARD,
		   "mMouse", MHel_MOUSE,
		   MENUSEPARATOR, MHel_SEP1,
		   "mOnItem", MHel_ONITEM,
		   MENUSEPARATOR, MHel_SEP2,
		   "mUsingHelp", MHel_USINGHELP,
		   MENUSEPARATOR, MHel_SEP3,
		   "mLegal", MHel_LICENSE,
		   "mRegistration", MHel_MAILINGLIST,
		   MENUSEPARATOR, MHel_SEP4,
		   "mAbout", MHel_ABOUT,
		   "mAuthor", MHel_AUTHOR,
		   NULL); 
  
 XtSetArg(args[0], XmNmenuHelpWidget, MenuHel->w_Cascade);
 XtSetValues(WIDGET(Bar1), args, 1);

 F(MenuHel).AddCallback(MenuHel, MHel_OVERVIEW, MenuHelpCallHelp, NULL);
 F(MenuHel).AddCallback(MenuHel, MHel_CONTENT, MenuHelpCallHelp, NULL);
 F(MenuHel).AddCallback(MenuHel, MHel_KEYBOARD, MenuHelpCallHelp, NULL);
 F(MenuHel).AddCallback(MenuHel, MHel_MOUSE, MenuHelpCallHelp, NULL);
 F(MenuHel).AddCallback(MenuHel, MHel_ONITEM, MenuHelpTrackHelp, NULL);
 F(MenuHel).AddCallback(MenuHel, MHel_USINGHELP, MenuHelpCallHelp, NULL);
 F(MenuHel).AddCallback(MenuHel, MHel_LICENSE, MenuHelpLegal, NULL);
 F(MenuHel).AddCallback(MenuHel, MHel_MAILINGLIST, MenuHelpRegistration, NULL);
 F(MenuHel).AddCallback(MenuHel, MHel_ABOUT, MenuHelpAbout, NULL);
 F(MenuHel).AddCallback(MenuHel, MHel_AUTHOR, MenuHelpAuthor, NULL);

 XtAddCallback(WIDGET(MenuHel), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuHel), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);

 /*-------------------------------------------------------------------*/
  
 F(MainInterface1).set_loadingMessage(MainInterface1, "Managing Interface");

 F(MainInterface1).attach_left(MainInterface1, WIDGET(Bar1), NULL);
 F(MainInterface1).attach_right(MainInterface1, WIDGET(Bar1), NULL);
 F(MainInterface1).attach_top(MainInterface1, WIDGET(Bar1), NULL);
  
 F(MainInterface1).attach_left(MainInterface1, WIDGET(hIconBarForm), NULL);
 F(MainInterface1).attach_right(MainInterface1, WIDGET(hIconBarForm), NULL);
 F(MainInterface1).attach_top(MainInterface1, WIDGET(hIconBarForm), WIDGET(Bar1));
  
 F(MainInterface1).attach_top(MainInterface1, 
			      WIDGET(vIconBar), WIDGET(hIconBarForm));
 F(MainInterface1).attach_left(MainInterface1, WIDGET(vIconBar), NULL);
 F(MainInterface1).attach_bottom(MainInterface1, WIDGET(vIconBar), 
				 WIDGET(BottomBar));
  
 F(MainInterface1).attach_right(MainInterface1, 
				WIDGET(BottomBar), 
				NULL);
 F(MainInterface1).attach_left(MainInterface1, 
			       WIDGET(BottomBar), NULL);
 F(MainInterface1).attach_bottom(MainInterface1, WIDGET(BottomBar), NULL);
  
 F(MainInterface1).attach_top(MainInterface1, 
			      WIDGET(MainDocument), WIDGET(hIconBarForm));
 F(MainInterface1).attach_right(MainInterface1, 
				WIDGET(MainDocument), 
				NULL);
 F(MainInterface1).attach_left(MainInterface1, 
			       WIDGET(MainDocument), WIDGET(vIconBar));
  
 F(MainInterface1).attach_bottom(MainInterface1, WIDGET(MainDocument),
				 WIDGET(BottomBar));
  
 F(MainInterface1).attach_top(MainInterface1,
			      w_FrameOfClipBoard, WIDGET(hIconBarForm));
 F(MainInterface1).attach_right(MainInterface1, 
				w_FrameOfClipBoard, NULL);
 F(MainInterface1).attach_bottom(MainInterface1,
				 w_FrameOfClipBoard, NULL);
  
 F(TimeIndicator).advance_indicator(TimeIndicator);
 F(Manager1).Manage_all(Manager1, NULL);
 F(MainInterface1).map_TopLevel(MainInterface1);
 if (TimeIndicator) F(TimeIndicator).advance_indicator(TimeIndicator);
  
 XGetWindowAttributes(display, XtWindow(WIDGET(MainDocument)), &get_attrib);
 set_attrib.event_mask = get_attrib.all_event_masks | StructureNotifyMask;
 XChangeWindowAttributes(display, XtWindow(WIDGET(MainDocument)),
			 CWEventMask,
			 &set_attrib);
  
 F(MainInterface1).set_loadingMessage(MainInterface1, "Initialize core");
 if (TimeIndicator) F(MainInterface1).set_main_icon(MainInterface1);
 
 Gwm = (c_WinManager *)NEW(c_WinManager)(display, 
					 XtWindow(WIDGET(MainDocument)));
 Gwm->SelectCallBack = SChangeWindow;
  
 F(GlobCursor).enable_wait_mode(GlobCursor, XtWindow(WIDGET(MainInterface1)));
 Init_Regle(display);
 Init_Pager(display);
  
 DocList = (c_DocumentList *)NEW(c_DocumentList)(MenuWin, MWin_DYNAMICDOCLIST, 
						 defaultDocumentName,
						 ChangeDocCallback);
  
 F(GlobColormap).generate_colormap(GlobColormap);
  
#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
 XcDO_IF_KEY_OK(IconVlogo = (c_Icon *)NEW(c_Icon)
		(WIDGET(vIconBarlogo), NULL,
		 "IconVlogo", display,
		 (gkinfo.valid == Xc_KEY_GOOD ? 
		  small_column_xpm : small_columnu_xpm), NULL, XcLabel))
 else
  IconVlogo = (c_Icon *)NEW(c_Icon)
   (WIDGET(vIconBarlogo), NULL,
    "IconVlogo", display,
    small_columnu_xpm, NULL, XcLabel);
#else
 IconVlogo = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBarlogo), NULL, 
				   "IconVlogo", display,  
				   small_column_xpm, NULL, XcLabel); 
#endif

 F(vIconBarlogo).attach_bottom(WIDGET(vIconBarlogo), WIDGET(IconVlogo), NULL);

 XtManageChild(WIDGET(IconVlogo));

 nargs = 0;
 XtSetArg(args[nargs], XmNbackgroundPixmap, 
	  F(GlobDocIcon).get_pixmap(GlobDocIcon, Icon_BACKGROUND));
 nargs++;
 XtSetValues(WIDGET(MainDocument), args, nargs);
  
 GlobImgCache = (c_ImageCache *)NEW(c_ImageCache)(GlobColormap);
  
 GlobKeyboard = 
  (c_Keyboard *)NEW(c_Keyboard)(display, XtWindow(WIDGET(MainInterface1)));
				  
 if (TimeIndicator) F(TimeIndicator).advance_indicator(TimeIndicator);
  
 GlobClipBuffer = (c_ClipBuffer *)NEW(c_ClipBuffer)();
 GlobDragAndDrop = (c_DragAndDrop *)NEW(c_DragAndDrop)(display);
 GlobTrash = (c_Trash *)NEW(c_Trash)(display, XtWindow(WIDGET(IconTra)));
  
 /*---------------------- Config File Reading ----------------------*/

 F(MainInterface1).set_loadingMessage(MainInterface1, "Loading user settings");
 if (TimeIndicator) F(TimeIndicator).advance_indicator(TimeIndicator);
  
 Xc_mcheck();
 if(!ReadConfigFile())
 {
  if(!ReadUserConfigFile()) QuitApplication();
 }
 else
  ReadUserConfigFile();
  
 /*--- set traversal to MainDocument drawing area ---*/
 if (!XmProcessTraversal(WIDGET(MainDocument), XmTRAVERSE_CURRENT))
  Xc_TRACE(("XmProcessTraversal failed"));

 XtVaSetValues(WIDGET(WTEXTFIELD(sbZoom)), XmNtraversalOn, True, NULL);

 F(GlobImgCache).init(GlobImgCache);
 GlobPrintParam.printer = GlobPrinterBase;


 /*--- Main loop ---*/
 DocAlreadyGrised = FALSE;
 SelectAllAlreadyGrised = FALSE;
 DeleteAlreadyGrised = FALSE;
 FrameToolsAlreadyGrised = FALSE;
 MultiFrameToolsAlreadyGrised = FALSE;
 LockToolsAlreadyGrised = FALSE;

 SImageAlreadyGrised = FALSE;
 TexteAlreadyGrised = FALSE;
 Texte2AlreadyGrised = FALSE;
 SVectorAlreadyGrised = FALSE;
 PasteAlreadyGrised = FALSE;
  
 F(MainInterface1).unmap_StartUp(MainInterface1); 
 DELETE(c_TimeIndicator)(TimeIndicator);
 TimeIndicator = NULL;

 /* if ((gkinfo.valid != Xc_KEY_GOOD) || !gkinfo.type)
    MenuHelpLegal(WIDGET(MainInterface1), display); */
 
 if (argc > 1 && argv[1] != NULL)
 {
  char	*fullname;
  char	*cwd;
  int	length;
  i = 1;
  while(argv[i])
  {
   /* should expand filename if no start '/' */
   if (argv[i][0] != '/')
   {
    cwd = GetWorkingDirectory();
    length = strlen(cwd)+strlen(argv[i])+4;
    fullname = Xc_malloc("fullname", length);
    sprintf(fullname, "%s/%s", cwd, argv[i]);
    Xc_free(cwd);
    MenuFileOpen(0, fullname);
    Xc_free(fullname);
   }
   else
    MenuFileOpen(0, argv[i]);
   i++;
  }
 }
 else
  MenuFileNew();
    
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
 listmanaged	*listegwm;
 listpage	*listviewpage;
 int		gwm_func;
 void		*DocumentTmp;
 void		*DocumentSel;
 int		TypeSel;
 int		TypeTmp;
 int     	num;
 Boolean	dbl_click;
 Window	wnone;
 int		x_,y_;
  
 if (!handle_event_allowed || motif_focus)
 {
  if (handle_event_allowed && tffs_focus)
  {
   if ((event->type == KeyPress || event->type == KeyRelease))
   {
    event->xkey.window = XtWindow(WIDGET(WTEXTFIELD(sbZoom)));
    XtDispatchEvent(event);
    return;
   }
   
   if ((event->type == ButtonPress || event->type == ButtonRelease) && 
       event->xbutton.window != XtWindow(WIDGET(WTEXTFIELD(sbZoom))))
   {
    cb_unfocus_tfZoom(WIDGET(WTEXTFIELD(sbZoom)), WTEXTFIELD(sbZoom), event);
   }
  }
  else
  {
   XtDispatchEvent(event);
   return;
  }
 }

 if (TEST_WAIT_MODE && 
     (event->type == KeyPress || event->type == ButtonPress))
  return;
  
 if (GlobDragAndDrop->active)
 {
  if (!F(GlobDragAndDrop).eventlook_drag(GlobDragAndDrop, event))
   return;
 }
 if (Gwm->SelectedWindow)
 {
  DocumentSel = F(DocList).Find(DocList,
				Gwm->SelectedWindow->MainWin,
				&TypeSel);
  F(DocList).SetDocSelection(DocList, Gwm->SelectedWindow->MainWin);
 }
 else
 {
  DocumentSel = NULL;
  F(DocList).ClearDocSelection(DocList);
 }
 if (event->type == PropertyNotify)
 {
  if (F(GlobRemoteServer).eventlook(GlobRemoteServer, event))
   return;
 }
 if (event->type == ClientMessage)
 {
  if ((DROP_FLAGS(*event) & 0xffff00) == DD_XCLAMATION_INDENTITY)
  {
   Xc_TRACE(("Client message received"));
   F(GlobDragAndDrop).eventlook_drop(GlobDragAndDrop, event);
  }
  else if ((int)event->xclient.data.l[0] == (int)0xdeaddddd)
  {
   Xc_TRACE(("Close Window Notify"));
   QuitApplication();
  }
 }
 if ((event->xany.window == XtWindow(WIDGET(MainDocument))) && 
     (event->type == ConfigureNotify))
 {
  F(Gwm).ArrangeIcon(Gwm, event->xany.display,
		     XtWindow(WIDGET(MainDocument)));
 }
 if (event->type == ButtonPress && motif_focus)
 {
  XtDispatchEvent(event);
  return;
 }
 /*  F(GlobKeyboard).EventLook(GlobKeyboard, *event); */
 num = Gwm->nbrmanaged;
 listegwm = Gwm->ListManaged;
 while (num--)
 {
  if (listegwm->Type == D_MAXIMIZED)
  {
   /* ----------------------------------------------------------------- ** 
   ** document maximized                                                ** 
   ** ----------------------------------------------------------------- */
   if ((event->xany.window == XtWindow(WIDGET(MainDocument))) && 
       (event->type == ConfigureNotify))
   {
    XResizeWindow(event->xany.display,
		  listegwm->Win,
		  event->xconfigure.width,
		  event->xconfigure.height
		  );
    XResizeWindow(event->xany.display,
		  listegwm->MainWin,
		  event->xconfigure.width,
		  event->xconfigure.height
		  );
    XMoveWindow(event->xany.display, 
		listegwm->Use->UnMaxWin,
		event->xconfigure.width - BUTTON_UMAXW, 0);
    Mod_structure(event->xany.display, event, listegwm);
    break;
   }
  }
  if (listegwm->Type == D_ICONED)
  {
   /* ----------------------------------------------------------------- ** 
   ** document en icone                                                 ** 
   ** ----------------------------------------------------------------- */
   if ((event->xany.window == listegwm->Icon) ||
       (event->xany.window == listegwm->Icon_Name))
   {
    switch(event->type)
    {
    case LeaveNotify:
     if (event->xany.window == listegwm->Icon)
      if ((event->xcrossing.x >= 0) &&
	  (event->xcrossing.x < ICONW) &&
	  (event->xcrossing.y >= 0) &&
	  (event->xcrossing.y < ICONH))
       break;
     if (event->xany.window == listegwm->Icon_Name)
     {
      XTranslateCoordinates(event->xany.display, 
			    listegwm->Icon_Name,
			    listegwm->Icon,
			    event->xcrossing.x,
			    event->xcrossing.y,
			    &x_, &y_,
			    &wnone);
      if ((x_ >= 0) && (x_ < ICONW) &&
	  (y_ >= 0) && (y_ < ICONH))
       break;
     }
     if (listegwm->Icon_Name)
      XUnmapWindow(event->xany.display, listegwm->Icon_Name);
     break;
    case EnterNotify:
     if (listegwm->Icon_Name)
      XMapWindow(event->xany.display, listegwm->Icon_Name);
     break;
    case Expose:
     if (listegwm->Icon)
      F(Gwm).DrawIconDecor(Gwm, listegwm, event->xany.display);
     if (listegwm->Icon_Name)
      F(Gwm).DrawIconNameDecor(Gwm, listegwm,
			       event->xany.display);
     break;
    case ButtonPress:
     dbl_click = FALSE;
     if (Gwm->last_iwin == event->xbutton.window)
     {
      if (event->xbutton.button == 1)
      {
       if (event->xbutton.time-Gwm->last_iclick
	   < DBL_CLK_DELAY)
	dbl_click = TRUE;
       else
	Gwm->last_iclick = event->xbutton.time;
      }
     }
     else
     {
      Gwm->last_iwin = event->xbutton.window;
      Gwm->last_iclick = event->xbutton.time;
     }

     if ((event->xbutton.button == 2) || (dbl_click == TRUE))
     {
      F(Gwm).DiselectWindow(Gwm, event->xany.display,
			    listegwm);
      F(Gwm).RaiseIcon(Gwm, event->xany.display, listegwm);
      return ;
     }
     else
     {
      F(Gwm).MoveIconWindow(Gwm, event->xany.display, 
			    listegwm->Icon,
			    listegwm);
     }
     break;
    }
    break;
   }
  }
  else
  {
   /* ----------------------------------------------------------------- ** 
   ** there is an open window                                           ** 
   ** ----------------------------------------------------------------- */
   DocumentTmp = F(DocList).Find(DocList, listegwm->MainWin, &TypeTmp);
   EVENTLOOK_DOCUMENT(DocumentTmp)(DocumentTmp, event, listegwm,
				   (DocumentTmp == DocumentSel));

   if ((DocumentTmp == DocumentSel) && (TypeTmp == DOCUMENT_PAO))
   {
    listviewpage = ((c_Document *)DocumentTmp)->SelectedPage;

    if ( store_function != listviewpage->Page->function_type)
    {
     /* ----------------------------------------------------------------- ** 
     ** change the toggle button function                                 ** 
     ** ----------------------------------------------------------------- */
     int i;
		  
     i = 0;
     while ((toggletable[i].func_num != store_function)
	    && 
	    (toggletable[i].tog_wid != 0))
      i++;
     if (toggletable[i].tog_wid != 0)
      XmToggleButtonSetState(toggletable[i].tog_wid,
			     False, False);

     store_function = listviewpage->Page->function_type;
     i = 0;
     while ((toggletable[i].func_num != store_function)
	    && 
	    (toggletable[i].tog_wid != 0))
      i++;
     if (toggletable[i].tog_wid != 0)
      XmToggleButtonSetState(toggletable[i].tog_wid,
			     True, False);	
     break;
     
    }
   }
  }
      
  /* ----------------------------------------------------------------- ** 
  ** end of current page test                                          ** 
  ** ----------------------------------------------------------------- */


  /* ----------------------------------------------------------------- ** 
  ** event for window manager                                          ** 
  ** ----------------------------------------------------------------- */
  gwm_func = F(Gwm).eventlook(Gwm, event->xany.display, event, listegwm);
  if (gwm_func == INTERN_EVENT)
   break;
  switch (gwm_func)
  {
  case MAX_DOC:
   F(Gwm).MaxWindow(Gwm, event->xany.display, listegwm,
		    XtWindow(WIDGET(MainDocument)));
   break;
  case MOD_STRUCT:
   Mod_structure(event->xany.display, event, listegwm);
   break;	
  case CLOSE_DOC:
   MenuFileClose();
   return;
  }
      
  listegwm = listegwm->NextManaged;
 }	
  
 /* ----------------------------------------------------------------- ** 
 ** manage insensitive objects                                        ** 
 ** ----------------------------------------------------------------- */

 if ((Gwm->SelectedWindow == NULL) || (TypeSel != DOCUMENT_PAO))
 {
  SensitivizeDoc(False);
  SensitivizeSImage(False);
  SensitivizeSVector(False);
  SensitivizeSTexte(False);
  SensitivizeSTexte2(False);
  SensitivizeLockTools(False);
  SensitivizeSelectAll(False);
  SensitivizeFrameTools(False);
  SensitivizeMultiFrameTools(False);
  SensitivizePaste(False);
  SensitivizeDelete(False);

  F(GlobHelpLine).change_function_message(GlobHelpLine, 
					  XcR_NONE, XcR_DEFAULT);
 }	
 else
 {				/* a DTP document is selected */
  c_Document	*DocSelected;

  SensitivizeDoc(True);
  DocSelected = (c_Document *)DocumentSel;

  F(GlobHelpLine).change_function_message
   (GlobHelpLine, XcR_NONE, 
    cnv_func_for_helpline(DocSelected->SelectedPage->Page->function_type));

  if (DocSelected->SelectedPage->Page->Frame_Border_actif)
  {
   if (!ButtonMaskFrame)
   {
    ButtonMaskFrame = TRUE;
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWFRAMESBORDER, True);
    F(MenuPge).ToggleButton(MenuPge, MPge_SHOWFRAMESBORDER, True);
   }
  }
  else
  {
   if (ButtonMaskFrame)
   {
    ButtonMaskFrame = FALSE;
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWFRAMESBORDER, False);
    F(MenuPge).ToggleButton(MenuPge, MPge_SHOWFRAMESBORDER, False);
   }
  }

  if (DocSelected->SelectedPage->Page->Page_Align_Mark_actif)
  {
   if (!ButtonMaskReperes)
   {
    ButtonMaskReperes = TRUE;
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWRULERSMARKS, True);
    F(MenuPge).ToggleButton(MenuPge, MPge_SHOWRULERSMARKS, True);
   }
  }
  else
  {
   if (ButtonMaskReperes)
   {
    ButtonMaskReperes = FALSE;
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWRULERSMARKS, False);
    F(MenuPge).ToggleButton(MenuPge, MPge_SHOWRULERSMARKS, False);
   }
  }

  if (DocSelected->SelectedPage->Page->magnetic)
  {
   if (!ButtonStopMagnetisation)
   {
    ButtonStopMagnetisation = TRUE;
    F(MenuDis).ToggleButton(MenuDis, MDis_MAGNETON, True);
    F(MenuPge).ToggleButton(MenuPge, MPge_MAGNETON, True);
   }
  }
  else
  {
   if (ButtonStopMagnetisation)
   {
    ButtonStopMagnetisation = FALSE;
    F(MenuDis).ToggleButton(MenuDis, MDis_MAGNETON, False);
    F(MenuPge).ToggleButton(MenuPge, MPge_MAGNETON, False);
   }
  }

  if (DocSelected->SelectedPage->Page->Page_Grid_actif)
  {
   if (!ButtonMaskGrid)
   {
    ButtonMaskGrid = TRUE;
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWGRID, True);
    F(MenuPge).ToggleButton(MenuPge, MPge_SHOWGRID, True);
   }
  }
  else
  {
   if (ButtonMaskGrid)
   {
    ButtonMaskGrid = FALSE;
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWGRID, False);
    F(MenuPge).ToggleButton(MenuPge, MPge_SHOWGRID, False);
   }
  }

  if (DocSelected->SelectedPage->Page->Regle_actif)
  {
   if (!ButtonMaskRegles)
   {
    ButtonMaskRegles = TRUE;
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWRULERS, True);
   }
  }
  else
  {
   if (ButtonMaskRegles)
   {
    ButtonMaskRegles = FALSE;
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWRULERS, False);
   }
  }
  
  if (DocSelected->SelectedPage->Page->Page_Typo_Mark_actif)
  {
   if (!ButtonMaskLines)
   {
    ButtonMaskLines = TRUE;
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWTYPOMARKS, True);
    F(MenuPge).ToggleButton(MenuPge, MPge_SHOWTYPOMARKS, True);
   }
  }
  else
  {
   if (ButtonMaskLines)
   {
    ButtonMaskLines = FALSE;
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWTYPOMARKS, FALSE);
    F(MenuPge).ToggleButton(MenuPge, MPge_SHOWTYPOMARKS, FALSE);
   }
  }

  if (PageZoom != DocSelected->SelectedPage->Page->sFactor)
  {
   PageZoom = DocSelected->SelectedPage->Page->sFactor;
   F(WTEXTFIELD(sbZoom)).set_value_in_unit(WTEXTFIELD(sbZoom), 
					   (real)SCALE_TO_PERCENT(PageZoom));
  }

  if (!GlobClipBuffer->LMobject)
  {
   SensitivizePaste(False);
  }
  else
  {
   SensitivizePaste(True);
  }

  
  /* ----------------------------------------------------------------- ** 
  ** Main Event Loop for sensitivize                                   ** 
  ** ----------------------------------------------------------------- */

  /*--- Select All activation ---*/
  if (!DocSelected->SelectedPage->Page->ListCadre)
  {
   SensitivizeSelectAll(False);
  }
  else
  {
   SensitivizeSelectAll(True);
  }
      
  /*--- Frame Selected case ---*/

  if (!DocSelected->SelectedPage->Page->SelectedCadre)
  {				/* NO FRAME SELECTED */
   SensitivizeFrameTools(False);
   SensitivizeMultiFrameTools(False);
   SensitivizeDelete(False);
   SensitivizeLockTools(False);
   SensitivizeSImage(False);
   SensitivizeSVector(False);
   SensitivizeSTexte(False);
   SensitivizeSTexte2(False);
  }
  else			
  {				/* SOMES FRAMES are SELECTED */
   SensitivizeLockTools(True);

   if (DocSelected->SelectedPage->Page->verrou == FALSE) 
   {				/* FRAMES ARE NOT LOCKED */
    if (!ButtonLockFrame)
    {
     ButtonLockFrame = TRUE;
     F(MenuFra).ToggleButton(MenuFra, MFra_LOCK, False);
     XmToggleButtonSetState(WIDGET(IconFra_19), False, False);
    }
    SensitivizeFrameTools(True);
    SensitivizeDelete(True);
    if (DocSelected->SelectedPage->Page->SelectedCadre->NextCadre)
    {
     SensitivizeMultiFrameTools(True);
    }
    else
    {
     SensitivizeMultiFrameTools(False);
    }
   }
   else				/* FRAMES ARE LOCKED */
   {
    if (ButtonLockFrame)
    {
     ButtonLockFrame = FALSE;
     F(MenuFra).ToggleButton(MenuFra, MFra_LOCK, True);
     XmToggleButtonSetState(WIDGET(IconFra_19), True, False);
    }
    if (DocSelected->SelectedPage->Page->SelectedCadre->NextCadre)
    {
     SensitivizeDelete(False);
    }
    else
    {
     if (DocSelected->SelectedPage->Page->
	 SelectedCadre->cadre->object_type != CADRE_EMPTY)
     {
      SensitivizeDelete(True);
     }
     else
     {
      SensitivizeDelete(False);
     }
    }
    SensitivizeFrameTools(False);
    SensitivizeMultiFrameTools(False);
   }


   if ((DocSelected->SelectedPage->Page->
	SelectedCadre->cadre->object_type == CADRE_TEXT))
   {
    SensitivizeSTexte2(True);
   }
   else
   {
    SensitivizeSTexte2(False);
   }
	  
	  
   if (DocSelected->SelectedPage->Page->SelectedCadre->NextCadre)
   {				/* AT LEAST 2 FRAMES are SELECTED */
    SensitivizeSVector(False);
    SensitivizeSImage(False);

    switch(DocSelected->SelectedPage->Page->
	   SelectedCadre->cadre->object_type)
    {
    case CADRE_IMAGE_BITMAP:
    case CADRE_VECTOR:
     SensitivizeSTexte(False);
     break;
    case CADRE_EMPTY:
    case CADRE_TEXT:
    default:
     SensitivizeSTexte(True);
     break;
    }     
   }
   else
   {				/* ONLY 1 FRAME is SELECTED */
    if (DocSelected->SelectedPage->Page->
	SelectedCadre->cadre->object_type == CADRE_VECTOR)
     /* SELECTED CONTENT is VECTOR */
    {
     ToggleSetImage(CADRE_VECTOR,
		    ((c_VectorGraph *)(DocSelected->SelectedPage->Page->
				       SelectedCadre->cadre->object))->mode);
     
     SensitivizeSImage(True);
     SensitivizeSVector(True);
     SensitivizeSTexte(False);
     if (((c_VectorGraph *)(DocSelected->SelectedPage->Page->
			    SelectedCadre->cadre->object))->drawtext_flag)
      /* VECTOR is displayed WITH TEXT */
     {
      if (ButtonMaskVectorText)
      {
       ButtonMaskVectorText = FALSE;
       XmToggleButtonSetState(WIDGET(IconVec_1), True, False);
      }
     }
     else
     {
      if (!ButtonMaskVectorText)
      {
       ButtonMaskVectorText = TRUE;
       XmToggleButtonSetState(WIDGET(IconVec_1), False, False);
      }
     }
    }
    else
    {				/* SELECTED CONTENT is NOT VECTOR */
     SensitivizeSVector(False);
     if (DocSelected->SelectedPage->Page->
	 SelectedCadre->cadre->object_type == CADRE_IMAGE_BITMAP)
      /* SELECTED CONTENT is IMAGE */
     {
      ToggleSetImage(CADRE_IMAGE_BITMAP,
		     ((c_Image *)(DocSelected->SelectedPage->Page->
				  SelectedCadre->cadre->object))->zoom_type);
      SensitivizeSImage(True);
      SensitivizeSTexte(False);
     }
     else			/* SELECTED CONTENT is TEXT or EMPTY */
     {
      SensitivizeSImage(False);
      SensitivizeSTexte(True);
     }
    }
    
   }
  }
 }
 XtDispatchEvent(event);
}

void SensitivizePaste(flag)
Boolean flag;
{
 if (flag == (Boolean)PasteAlreadyGrised)
 {
  PasteAlreadyGrised = !PasteAlreadyGrised;
      
  F(MenuEdi).ButtonFlag(MenuEdi, MEdi_PASTE, flag);

  XtSetSensitive(WIDGET(IconMis_7), flag);
 }
}


void SensitivizeDelete(flag)
Boolean flag;
{
 if (flag == (Boolean)DeleteAlreadyGrised)
 {
  DeleteAlreadyGrised = !DeleteAlreadyGrised;

  F(MenuEdi).ButtonFlag(MenuEdi, MEdi_DELETE, flag);
  
  XtSetSensitive(WIDGET(IconMis_8), flag);
 }
}


void SensitivizeSTexte(flag)
Boolean flag;
{
 if (flag == (Boolean)TexteAlreadyGrised)
 {
  TexteAlreadyGrised = !TexteAlreadyGrised;

  XtSetSensitive(WIDGET(IconTex_1), flag);
 }
}

void SensitivizeSTexte2(flag)
Boolean flag;
{
 if (flag == (Boolean)Texte2AlreadyGrised)
 {
  Texte2AlreadyGrised = !Texte2AlreadyGrised;

  XtSetSensitive(WIDGET(IconTex_6), flag);
 }
}

static void SensitivizeSImage(flag)
Boolean flag;
{
 if (flag == (Boolean)SImageAlreadyGrised)
 {
  SImageAlreadyGrised = !SImageAlreadyGrised;
  
  XtSetSensitive(WIDGET(IconIma_1), flag);
  XtSetSensitive(WIDGET(IconIma_2), flag);
  XtSetSensitive(WIDGET(IconIma_3), flag);
  XtSetSensitive(WIDGET(IconIma_4), flag);
  XtSetSensitive(WIDGET(IconIma_5), flag);
  XtSetSensitive(WIDGET(IconIma_6), flag);
  XtSetSensitive(WIDGET(IconIma_7), flag); 
  XtSetSensitive(WIDGET(IconIma_8), flag);
  XtSetSensitive(WIDGET(IconIma_9), flag);
 }
}

static void SensitivizeSVector(flag)
Boolean flag;
{
 if (flag == (Boolean)SVectorAlreadyGrised)
 {
  SVectorAlreadyGrised = !SVectorAlreadyGrised;

  if (!flag)
  {
   if (!ButtonMaskVectorText)
   {
    ButtonMaskVectorText = TRUE;
    XmToggleButtonSetState(WIDGET(IconVec_1), False, False);
   }
  }

  XtSetSensitive(WIDGET(IconVec_1), flag);
 }
}

void SensitivizeSelectAll(flag)
Boolean flag;
{
 if (flag == (Boolean)SelectAllAlreadyGrised)
 {
  SelectAllAlreadyGrised = !SelectAllAlreadyGrised;

  F(MenuEdi).ButtonFlag(MenuEdi, MEdi_SELECTALL, flag);
 }
}

void SensitivizeMultiFrameTools(flag)
Boolean flag;
{
 if (flag == (Boolean)MultiFrameToolsAlreadyGrised)
 {
  MultiFrameToolsAlreadyGrised = !MultiFrameToolsAlreadyGrised;

  XtSetSensitive(WIDGET(IconFra_15), flag);
  XtSetSensitive(WIDGET(IconFra_16), flag);
  XtSetSensitive(WIDGET(IconFra_11), flag);
  XtSetSensitive(WIDGET(IconFra_17), flag);
  XtSetSensitive(WIDGET(IconFra_18), flag);
  F(MenuFra).ButtonFlag(MenuFra, MFra_FRAMEOPERATION, flag);
  F(MenuFra).ButtonFlag(MenuFra, MFra_ALIGN, flag);
 }
}

void SensitivizeLockTools(flag)
Boolean flag;
{
 if (flag == (Boolean)LockToolsAlreadyGrised)
 {
  LockToolsAlreadyGrised = !LockToolsAlreadyGrised;

  F(MenuFra).ButtonFlag(MenuFra, MFra_LOCK, flag);
  F(MenuFra).ButtonFlag(MenuFra, MFra_UNLOCK, flag);
  XtSetSensitive(WIDGET(IconFra_19), flag);
 }
}

void SensitivizeFrameTools(flag)
Boolean flag;
{
 if (flag == (Boolean)FrameToolsAlreadyGrised)
 {
  FrameToolsAlreadyGrised = !FrameToolsAlreadyGrised;

  XtSetSensitive(WIDGET(IconFra_14), flag);
  XtSetSensitive(WIDGET(IconFra_13), flag);
  XtSetSensitive(WIDGET(IconFra_9), flag);
  XtSetSensitive(WIDGET(IconFra_8), flag);
  XtSetSensitive(WIDGET(IconFra_7), flag);
  XtSetSensitive(WIDGET(IconFra_6), flag);
  XtSetSensitive(WIDGET(IconFra_5), flag);
  XtSetSensitive(WIDGET(IconFra_4), flag);
  XtSetSensitive(WIDGET(IconFra_3), flag);

  XtSetSensitive(WIDGET(IconTra), flag);
      
  F(MenuEdi).ButtonFlag(MenuEdi, MEdi_CUT, flag);
  F(MenuEdi).ButtonFlag(MenuEdi, MEdi_COPY, flag);

  XtSetSensitive(WIDGET(IconMis_5), flag);
  XtSetSensitive(WIDGET(IconMis_6), flag);
  
  F(MenuFil).ButtonFlag(MenuFil, MFil_IMPORT, flag);
      
  F(MenuFra).ButtonFlag(MenuFra, MFra_POINTINSERT, flag);
  F(MenuFra).ButtonFlag(MenuFra, MFra_POINTDELETE, flag);
  F(MenuFra).ButtonFlag(MenuFra, MFra_POINTMOVE, flag);
  F(MenuFra).ButtonFlag(MenuFra, MFra_PLANECHANGE, flag);
  F(MenuFra).ButtonFlag(MenuFra, MFra_SCALE, flag);
  F(MenuFra).ButtonFlag(MenuFra, MFra_FRAMEDELETE, flag);
  F(MenuFra).ButtonFlag(MenuFra, MFra_ROTATE, flag);
  F(MenuFra).ButtonFlag(MenuFra, MFra_ATTRIBS, flag);
 }
}

void SensitivizeDoc(flag)
Boolean flag;
{
 if (flag == (Boolean)DocAlreadyGrised)
 {
  DocAlreadyGrised = !DocAlreadyGrised;

  XtSetSensitive(MenuEdi->w_Cascade, flag);
  XtSetSensitive(MenuFra->w_Cascade, flag);
  XtSetSensitive(MenuDis->w_Cascade, flag);
  XtSetSensitive(MenuPge->w_Cascade, flag);
  XtSetSensitive(MenuWin->w_Cascade, flag);
      
  F(MenuFil).ButtonFlag(MenuFil, MFil_CLOSE, flag);
  F(MenuFil).ButtonFlag(MenuFil, MFil_SAVE, flag);
  F(MenuFil).ButtonFlag(MenuFil, MFil_SAVEAS, flag);
  F(MenuFil).ButtonFlag(MenuFil, MFil_SAVEALL, flag);
  F(MenuFil).ButtonFlag(MenuFil, MFil_PRINT, flag);

  XtSetSensitive(WIDGET(IconMis_3), flag);
  XtSetSensitive(WIDGET(IconMis_4), flag);

  /* creation et selection de cadre */
  XtSetSensitive(WIDGET(IconFra_0), flag);
  XtSetSensitive(WIDGET(IconFra_1), flag);
  XtSetSensitive(WIDGET(IconFra_10), flag);
  XtSetSensitive(WIDGET(IconFra_12), flag);
  /* barre du texte */
  XtSetSensitive(WIDGET(IconTex_2), flag);
  XtSetSensitive(WIDGET(IconTex_3), flag);
  XtSetSensitive(WIDGET(IconTex_4), flag);
  XtSetSensitive(WIDGET(IconTex_5), flag);
  /* barre des loupes */
  XtSetSensitive(WIDGET(IconZoo_1), flag);
  XtSetSensitive(WIDGET(IconZoo_2), flag);
  XtSetSensitive(WIDGET(IconZoo_3), flag);
  XtSetSensitive(WIDGET(IconZoo_4), flag);
  XtSetSensitive(WIDGET(IconZoo_5), flag);
  XtSetSensitive(WIDGET(IconZoo_6), flag);
  XtSetSensitive(WIDGET(sbZoom), flag);
  XtSetSensitive(WIDGET(IconZoo_7), flag);
  XtSetSensitive(WIDGET(IconZoo_8), flag);
  XtSetSensitive(WIDGET(IconZoo_9), flag);
  XtSetSensitive(WIDGET(IconZoo_10), flag);
  XtSetSensitive(WIDGET(IconZoo_11), flag);
  XtSetSensitive(WIDGET(IconZoo_12), flag);
 }
}


void Mod_structure(display, Event, listegwm)
Display		*display; 
XEvent		*Event;
listmanaged	*listegwm;
{
 void	*DocTmp;
  
 DocTmp = F(DocList).Find(DocList, listegwm->MainWin, NULL);
 RESIZE_DOCUMENT(DocTmp)(DocTmp, Event,
			 (listegwm->Type == D_MAXIMIZED ? TRUE : FALSE));
  
}


void DisplayClipboard()
{
 if (ButtonMaskClipboard == TRUE)
 {
  XtUnmanageChild(w_FrameOfClipBoard);
  F(MainInterface1).attach_right(MainInterface1, 
				 WIDGET(BottomBar), 
				 NULL);
  F(MainInterface1).attach_right(MainInterface1, 
				 WIDGET(MainDocument), 
				 NULL);

  F(MenuDis).ToggleButton(MenuDis, MDis_SHOWCLIPBOARD, False);
  ButtonMaskClipboard = FALSE;
 }
 else
 {
  XtManageChild(w_FrameOfClipBoard);
  F(MainInterface1).attach_right(MainInterface1, 
				 WIDGET(BottomBar), 
				 w_FrameOfClipBoard);
  F(MainInterface1).attach_right(MainInterface1, 
				 WIDGET(MainDocument), 
				 w_FrameOfClipBoard); 

  F(MenuDis).ToggleButton(MenuDis, MDis_SHOWCLIPBOARD, True);
  ButtonMaskClipboard = TRUE;
 }
}


void DisplayBottomBar()
{
 if (ButtonMaskBottomBar == TRUE)
 {
  XtUnmanageChild(WIDGET(BottomBar));
  F(MainInterface1).attach_bottom(MainInterface1, WIDGET(vIconBar), NULL);
  F(MainInterface1).attach_bottom(MainInterface1, WIDGET(MainDocument),
				  NULL);
  

  F(MenuDis).ToggleButton(MenuDis, MDis_SHOWBOTTOMBAR, False);
  ButtonMaskBottomBar = FALSE;
 }
 else
 {
  XtManageChild(WIDGET(BottomBar));
  F(MainInterface1).attach_bottom(MainInterface1, WIDGET(vIconBar), 
				  WIDGET(BottomBar));
  F(MainInterface1).attach_bottom(MainInterface1, WIDGET(MainDocument),
				  WIDGET(BottomBar));
  

  F(MenuDis).ToggleButton(MenuDis, MDis_SHOWBOTTOMBAR, True);
  ButtonMaskBottomBar = TRUE;
 }
}



/*************************************/
/* This reference the managed Window */
/*************************************/
void MenuFileNoKey()
{
 Xc_ErrorAlert(F(GlobResources).getString(GlobResources,
					  XcR_functionNeedKey));
}

void MenuFileNew()
{
 Display	*display;
 c_Document	*DocTmp;
 c_Page	*page;
 c_Box_document	*box;
 int		rc, i;
 page_type_t	type;
 int		num_page;
  
 /*--- Get page format ---*/
 display = XtDisplay(WIDGET(MainDocument));
 if (GlobNewDocInfo.doc_name)
  Xc_free(GlobNewDocInfo.doc_name);
 GlobNewDocInfo.doc_name = F(DocList).Get_NextName(DocList);
 SET_WAIT_MODE_STATIC;
 box = (c_Box_document *)NEW(c_Box_document)(MainInterface1->w_Main,
					     "BoxDocument", DocList,
					     &GlobNewDocInfo);
 rc = F(box->Dialog).waitForUnmap(box->Dialog);

 DELETE(c_Box_document)(box);
  
 switch(rc)
 {
 case XcCD_CANCEL:
 case XcCD_DESTROY:
  Xc_free(GlobNewDocInfo.doc_name);
  GlobNewDocInfo.doc_name = NULL;
  UNSET_WAIT_MODE;
  return;
  break;
 case XcCD_OPEN:
  Xc_free(GlobNewDocInfo.doc_name);
  GlobNewDocInfo.doc_name = NULL;
  UNSET_WAIT_MODE;
  MenuFileOpen(0, NULL);
  return;
  break;
 }
  
 /**************************/
 /* Creation d'un document */
 /**************************/
 DocTmp = (c_Document *)NEW(c_Document)();
 /*****************/
 /* Creation Page */
 /*****************/
 Xc_TRACE(("Add page"));
 num_page = 1;
 for(i = 0; i < (int)GlobNewDocInfo.page_nbr; i++)
 {
  page = (c_Page *)NEW(c_Page)( &((c_Document *)DocTmp)->BaseStd);
  if (GlobNewDocInfo.p_do)
  {
   if (GlobNewDocInfo.p_rv)
   {
    if (i == 0)
     type = Xc_PAGE_RIGHT;
    else
     if (i == (int)GlobNewDocInfo.page_nbr - 1)
      type = Xc_PAGE_LEFT;
     else
      if (++i >= (int)GlobNewDocInfo.page_nbr)
       type = Xc_PAGE_LEFT;
      else
       type = Xc_PAGE_DOUBLE;
   }
   else
   {
    if (++i >= (int)GlobNewDocInfo.page_nbr)
     type = Xc_PAGE_LEFT;
    else
     type = Xc_PAGE_DOUBLE;
   }
  }
  else
  {
   if (GlobNewDocInfo.p_rv)
   {
    type = ((i&1) ? Xc_PAGE_LEFT : Xc_PAGE_RIGHT);
   }
   else
   {
    type = Xc_PAGE_RIGHT;
   }
  }
  F(page).init_Page(page, num_page, GlobNewDocInfo.p_f, type,
		    GlobNewDocInfo.p_o, 
		    GlobNewDocInfo.le_m, GlobNewDocInfo.ri_m,
		    GlobNewDocInfo.up_m, GlobNewDocInfo.lo_m,
		    GlobNewDocInfo.n_c, GlobNewDocInfo.n_r,
		    GlobNewDocInfo.c_s, GlobNewDocInfo.r_s, 
		    GlobNewDocInfo.g_orx, GlobNewDocInfo.g_ory,
		    GlobNewDocInfo.g_dtx, GlobNewDocInfo.g_dty,
		    XPi_SCRATCH);
  if (type == Xc_PAGE_DOUBLE)
   num_page += 2;
  else
   num_page++;
  F(DocTmp).AddPage(DocTmp, page);
 }
 /*****************/
 /* Init Document */
 /*****************/
 Xc_TRACE(("Init document"));
 F(DocTmp).Init_Document(DocTmp,
			 XtDisplay(WIDGET(MainDocument)),
			 XtWindow(WIDGET(MainDocument)),
			 TRUE,
			 GlobNewDocInfo.doc_name);
 if (!Xstrncmp(defaultDocumentName, DocTmp->name, 
	       strlen(defaultDocumentName)))
 {
  char	*cwd;
  char	*filename;
  int	length;
      
  cwd = GetWorkingDirectory();
  length = strlen(cwd)+strlen(DocTmp->name)+5;
  filename = Xc_malloc("file name", length);
  sprintf(filename, "%s/%s.xc", cwd, DocTmp->name);
  Xc_free(cwd);
  DocTmp->filename = filename;
 }
 /*******************/
 /* Manage document */
 /*******************/
 UNSET_WAIT_MODE;
 F(Gwm).SetWindowName(Gwm, display, DocTmp->window, GlobNewDocInfo.doc_name);
 F(DocList).Add(DocList, DocTmp, DOCUMENT_PAO, GlobNewDocInfo.doc_name);
 Xc_free(GlobNewDocInfo.doc_name);
 GlobNewDocInfo.doc_name = NULL;
 F(Gwm).ManageWin(Gwm, display, DocTmp->window,
		  F(GlobDocIcon).get_pixmap(GlobDocIcon, Icon_PAO));
}

void MenuFileOpen(w, filen)
Widget	w;     
char	*filen;
{
 c_DocFile	*doc;
 c_Document	*DocTmp;
 char		*filename;

 Xc_TRACE(("MenuFileOpen"));

 if (filen)
 {
  if (!IsFileExist(filen))
  {
   filename = (char *)Xc_malloc("filen", strlen(filen) + 4);
	  
   strcpy(filename, filen);
   strcat(filename, ".xc");
   if (!IsFileExist(filename))
   {
    Xc_free(filename);
    filename = Xc_strdup("filen", filen);
   }
  }
  else
   filename = Xc_strdup("filen", filen);
  Xc_TRACE(("load %s", filename));
 }
 else
 {
  Xc_TRACE(("Open"));
      
  if((OpenDocumentSelector == NULL) &&
     (CreateOpenDocumentSelector(MainInterface1->w_Main) == FALSE)) return;
  filename = F(OpenDocumentSelector).getFile(OpenDocumentSelector);
  Xc_TRACE(("filename=`%s'", filename? filename : "<null>"));
 }
 if (filename == NULL) return;
  
 if((doc = NEW(c_DocFile)(XcDF_LOAD, filename, -1)) != NULL)
 {
  F(doc).addCallbacks(doc,
		      XcDF_DOCUMENT_KEYWORD,
		      FUNCTION(c_Document, read_document), filename,
		      NULL);
  SET_WAIT_MODE_ANIM;
  if(F(doc).expectKeyword(doc, XcDF_DOCUMENT_KEYWORD, FALSE) == TRUE)
  {
   if (doc->data_type == XcDF_DOCUMENT_TYPE)
   {
    Xc_TRACE(("Document loaded"));
	      
    /*--- Do not free filename ---*/
    DocTmp = doc->data_ptr;
    DocTmp->filename = filename;
	      
    /*--- Initialize document ---*/
    UNSET_WAIT_MODE_ANIM;
    F(DocTmp).Init_Document(DocTmp,
			    XtDisplay(WIDGET(MainDocument)),
			    XtWindow(WIDGET(MainDocument)),
			    TRUE, NULL);
	      
    /*--- Manage Doument ---*/
    F(DocList).Add(DocList, DocTmp, DOCUMENT_PAO, DocTmp->name);
	      
    Xc_TRACE(("Set Window Name"));
    F(Gwm).SetWindowName(Gwm, DocTmp->display,
			 DocTmp->window, DocTmp->name);
	      
    F(Gwm).ManageWin(Gwm, DocTmp->display, DocTmp->window,
		     F(GlobDocIcon).get_pixmap(GlobDocIcon,
					       Icon_PAO));
    DELETE(c_DocFile)(doc);
    return;
   }
  }
  UNSET_WAIT_MODE_ANIM;
  DELETE(c_DocFile)(doc);
  
  if (filen)
  {
   char	*error_msg;
	  
   error_msg = (char *)Xc_malloc("errmsg",
				 strlen(F(GlobResources).getString
					(GlobResources,
					 XcR_loadingError)) +
				 strlen(filen) + 1);
   sprintf(error_msg, F(GlobResources).getString
	   (GlobResources, XcR_loadingError), filen);
   SET_WAIT_MODE_STATIC;
   Xc_ErrorAlert(error_msg);
   UNSET_WAIT_MODE_STATIC;
   Xc_free(error_msg);
  }
  else
  {
   SET_WAIT_MODE_STATIC;
   Xc_ErrorAlert(F(GlobResources).getString(GlobResources,
					    XcR_loadError));
   UNSET_WAIT_MODE_STATIC;
  }
 }
 Xc_free(filename);
 Xc_TRACE(("Error"));
}


void _SaveModif(Document)
c_Document	*Document;
{
 switch(Xc_CloseDocAlert(Document->name, Document->filename))
 {
 case XcCD_SAVE:
  MenuFileSave();
  break;
 case XcCD_SAVE_AS:
  MenuFileSaveas();
  break;
 case XcCD_OK:
  break;
 case XcCD_CANCEL:
 case XcCD_DESTROY:
  return;
 }
}

void MenuFileClose()
{
 listmanaged	*listegwm;
 listmanaged	*Tlistegwm;
 int		Tnum;
 int		AnotherW;
 Display	*display;
 c_Document	*Document;
 int		Type;

 display = XtDisplay(WIDGET(MainInterface1));
 listegwm = Gwm->SelectedWindow;
 if (Gwm->SelectedWindow != NULL)
 {
  Document = F(DocList).Find(DocList,
			     Gwm->SelectedWindow->MainWin, &Type);
  if (Type == DOCUMENT_PAO && Document->modified)
  {
   _SaveModif(Document);
  }
  Tnum = Gwm->nbrmanaged;
  Tlistegwm = Gwm->ListManaged;
  AnotherW = 0;
  while (Tnum--)
  {
   if ((Tlistegwm->Type == D_NORMAL) && 
       (Tlistegwm != Gwm->SelectedWindow))
   {
    AnotherW = 1;
    Tnum = 0;
   }
   else
    Tlistegwm = Tlistegwm->NextManaged;
  }
  if (AnotherW)
  {
   F(Gwm).DiselectWindow(Gwm, display, Tlistegwm);
  }
  else
  {
   Gwm->SelectedWindow = NULL;
  }
  F(DocList).DelId(DocList, listegwm->MainWin);
  F(Gwm).CloseWindow(Gwm, listegwm, display);
 }
}

void MenuFileSaveas()
{
 c_DocFile *doc;
 c_Document *Document;
 char *filename;

 Xc_TRACE(("Save as"));

 if((CloseDocumentSelector == NULL) &&
    (CreateCloseDocumentSelector(MainInterface1->w_Main) == FALSE)) return;
 if((filename = F(CloseDocumentSelector).getFile(CloseDocumentSelector))
    == NULL)
  return;

 if((doc = NEW(c_DocFile)(XcDF_SAVE, filename, -1)) != NULL)
 {
  Document = F(DocList).Find(DocList,
			     Gwm->SelectedWindow->MainWin, NULL);

  if(Document->filename != NULL) Xc_free(Document->filename);

  /*--- DO NOT FREE `filename' !!! ---*/
  Document->filename = filename;
      
  if (Xstrncmp(defaultDocumentName, Document->name, 
	       strlen(defaultDocumentName)))
  {
   if (strlen(GetBasename(filename, FALSE))-3>XcMAX_NAME_LENGTH-1)
   {
    memcpy(Document->name, GetBasename(filename, FALSE),
	   XcMAX_NAME_LENGTH-1);
    Document->name[XcMAX_NAME_LENGTH-1]='\0';
   }
   else
   {
    memcpy(Document->name, GetBasename(filename, FALSE), 
	   strlen(GetBasename(filename, FALSE) - 3));
    Document->name[strlen(GetBasename(filename, FALSE))-3]='\0';
   }
   F(Gwm).SetWindowName(Gwm, Document->display,
			Document->window, Document->name);
   F(DocList).ChangeDocName(DocList, (void *)Document, Document->name);
  }
      
  if(WRITE_DOCUMENT(Document)(Document, doc) == FALSE)
   Xc_ERROR(("Save error"));
      
  DELETE(c_DocFile)(doc);
 }
}

void MenuFileSave()
{
 c_DocFile *doc;
 c_Document *Document;

 Xc_TRACE(("Save"));

 Document = F(DocList).Find(DocList,
			    Gwm->SelectedWindow->MainWin, NULL);
 if(Document->filename == NULL)
 {
  MenuFileSaveas();
  return;
 }

 if((doc = NEW(c_DocFile)(XcDF_SAVE, Document->filename, -1)) != NULL)
 {
  if(WRITE_DOCUMENT(Document)(Document, doc) == FALSE)
   Xc_ERROR(("Save error"));
  DELETE(c_DocFile)(doc);
 }
}

void MenuFileSaveall()
{
 c_Document *Document;
 listdocument *tmplist = NULL;

 Xc_TRACE(("Save all"));

 tmplist = DocList->ListDocument;
 while (tmplist)
 {
  Xc_TRACE(("%s", tmplist->Name));
  Document = tmplist->Document;
  if(!(Document->modified))
  {
   tmplist = tmplist->NextDocument;
   continue;
  }
      
  switch(Xc_SaveDocAlert(Document->name, Document->filename))
  {
  case XcCD_YES:
   MenuFileSave();
   break;
  case XcCD_SAVE_AS:
   MenuFileSaveas();
   break;
  case XcCD_NO:
   break;
  case XcCD_CANCEL:
  case XcCD_DESTROY:
   return;
  }
  tmplist = tmplist->NextDocument;
 }
}

char *extract_normal_string(cs)
XmString cs;
{

 XmStringContext context;
 XmStringCharSet charset;
 XmStringDirection direction;
 Boolean separator;
 static char *primitive_string;

 XmStringInitContext (&context, cs);
 XmStringGetNextSegment (context, &primitive_string,
			 &charset, &direction, &separator);
 XmStringFreeContext (context);
 return ((char *)primitive_string);
}

/* ----------------------------------------------------------------- ** 
** LoadImage- Callback from ImageImportBox                           ** 
** ----------------------------------------------------------------- */
void LoadImage(w)
Widget	w;
{
 char		*filename;
 c_Page	*SelectedPage;
 c_Image	*image;
 c_Image	*image2;
 l_Cadre	*lcadre;
 l_Cadre	*svglcadre;
 void 	*DocTmp;
 int	Type;

 DocTmp = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (DocTmp && Type == DOCUMENT_PAO)
  SelectedPage =  ((c_Document *)DocTmp)->SelectedPage->Page;
 else
  return;
     
 if(!SelectedPage->SelectedCadre)
 {
  Xc_ErrorAlert(F(GlobResources).getString(GlobResources, 
					   XcR_noFrameError));
  return;
 }
  
 if((ImportImageSelector == NULL) &&
    (CreateImportImageSelector(MainInterface1->w_Main) == FALSE)) return;
 if((filename = F(ImportImageSelector).getFile(ImportImageSelector)) == NULL)
  return;

 SET_WAIT_MODE_STATIC;
 if((image = (c_Image *)NEW(c_Image)(filename, NULL)) != NULL)
 {
  lcadre = svglcadre = (l_Cadre *)sort_asc_list_cadre
   (SelectedPage->SelectedCadre);
  F(lcadre->cadre).attachobject(lcadre->cadre, image, CADRE_IMAGE_BITMAP);
  F(SelectedPage).draw_cadre(SelectedPage, lcadre); 
  lcadre = lcadre->NextCadre;
  while(lcadre)
  {
   image2 = (c_Image *)COPY(c_Image)(image);
   if (image2)
   {
    F(lcadre->cadre).attachobject(lcadre->cadre, image2,
				  CADRE_IMAGE_BITMAP);
    F(SelectedPage).draw_cadre(SelectedPage, lcadre); 
   }
   else
    break;
   lcadre = lcadre->NextCadre;
  }
  free_list_cadre(svglcadre, NO_DEL_CADRE);
  ((c_Document *)DocTmp)->modified = TRUE;
 }
 else
 {
  Xc_ErrorAlert(F(GlobResources).getString(GlobResources, 
					   XcR_notBitmapError));
 }
 UNSET_WAIT_MODE;
 Xc_free(filename);
 F(SelectedPage).set_function(SelectedPage, SelectedPage->function_type);
 return;
}

/* ----------------------------------------------------------------- ** 
** LoadVector - Callback from VectorImportBox                        ** 
** ----------------------------------------------------------------- */
void LoadVector(w)
Widget w;
{
 char		*filename;
 c_Page	*SelectedPage;
 l_Cadre	*frame_list;
 c_VectorGraph	*graph, *graph2;
 c_Document	*doc;
 void	 	*DocTmp;
 int		Type;
  
 DocTmp = NULL;
 DocTmp = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (DocTmp && Type == DOCUMENT_PAO)
  SelectedPage =  ((c_Document *)DocTmp)->SelectedPage->Page;
 else
  return;
  
 if(!SelectedPage->SelectedCadre)
 {
  Xc_ErrorAlert(F(GlobResources).getString(GlobResources, 
					   XcR_noFrameError));
  return;
 }
  
 if((ImportVectorSelector == NULL) &&
    (CreateImportVectorSelector(MainInterface1->w_Main) == FALSE)) return;
 if((filename = F(ImportVectorSelector).getFile(ImportVectorSelector))== NULL)
  return;
  
 doc = ((c_Document *)DocTmp);
 if((graph = NEW(c_VectorGraph)(&doc->BaseStd)) == NULL)
  return;
  
 /*--- Load ---*/
 SET_WAIT_MODE_ANIM;
 if(F(graph).load(graph, filename))
 {
  frame_list = sort_asc_list_cadre(SelectedPage->SelectedCadre);
  F(frame_list->cadre).attachobject(frame_list->cadre,
				    graph, CADRE_VECTOR);
  F(SelectedPage).draw_cadre(SelectedPage, frame_list); 
  frame_list = frame_list->NextCadre;
  while(frame_list)
  {
   graph2 = (c_VectorGraph *)COPY(c_VectorGraph)(graph, &doc->BaseStd);
   if (graph2)
   {
    F(frame_list->cadre).attachobject(frame_list->cadre,
				      graph2, CADRE_VECTOR);
    F(SelectedPage).draw_cadre(SelectedPage, frame_list); 
   }
   else
    break;
   frame_list = frame_list->NextCadre;
  }
  free_list_cadre(frame_list, NO_DEL_CADRE);
  doc->modified = TRUE;
 }
 else
 {
  Xc_ErrorAlert(F(GlobResources).getString(GlobResources, 
					   XcR_loadError));
 }
 UNSET_WAIT_MODE_ANIM;
 Xc_free(filename);
 F(SelectedPage).set_function(SelectedPage, SelectedPage->function_type);
 return;
}

/* ----------------------------------------------------------------- ** 
** LoadText - Callback from TextImportBox                            ** 
** ----------------------------------------------------------------- */
void LoadText(w)
Widget w;
{
 c_TextFrame *textframe;
 c_Page *SelectedPage;
 char *filename;
 void 	*DocTmp;
 int	Type;
 boolean erreur = FALSE;
 boolean new_tf = TRUE;

 DocTmp = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (DocTmp && Type == DOCUMENT_PAO)
  SelectedPage =  ((c_Document *)DocTmp)->SelectedPage->Page;
 else
  return;
     
 if(!SelectedPage->SelectedCadre)
 {
  Xc_ErrorAlert(F(GlobResources).getString(GlobResources, 
					   XcR_noFrameError));
  return;
 }
  
 if((ImportTextSelector == NULL) &&
    (CreateImportTextSelector(MainInterface1->w_Main) == FALSE)) return;
 if((filename = F(ImportTextSelector).getFile(ImportTextSelector)) == NULL)
  return;

 if (SelectedPage->SelectedCadre->cadre->object_type == CADRE_TEXT)
 {
  textframe = (c_TextFrame *)(SelectedPage->SelectedCadre->cadre->object);
  while(textframe->previous)
   textframe = textframe->previous;
  F(textframe).attachText(textframe, NULL);
  new_tf = FALSE;
 }
 else
 {
  textframe = NEW(c_TextFrame)();
  if (textframe == NULL) 
   erreur = TRUE;
 }
 if (!erreur)
 {
  c_TextFrame *previous;
  c_Document *doc;
  l_Cadre *ptr;
  boolean rc = FALSE;

  SET_WAIT_MODE_STATIC;
  doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, NULL);
  switch ((long)(ImportTextSelector->data))
  {
  case 0:
   rc = F(textframe).importText(textframe, &doc->BaseStd, filename);
   break;
  case 1:
   rc = F(textframe).importASCII(textframe, &doc->BaseStd, filename,
				 doc->BaseStd.text_style_base,
				 doc->BaseStd.text_ruler_base);
   break;
   /*case 2:*/
   /*rc = F(textframe).importXMW(textframe, &doc->BaseStd, filename);*/
   /*break;*/
  }
  if(rc == FALSE)
  {
   DELETE(c_TextFrame)(textframe);
   UNSET_WAIT_MODE;
   Xc_ErrorAlert(F(GlobResources).getString(GlobResources, 
					    XcR_loadError));
   Xc_free(filename);
   return;
  }
  UNSET_WAIT_MODE;
      
  Xc_TRACE(("Linking"));
  if (new_tf)
  {
   previous = NULL;
   for(ptr = SelectedPage->SelectedCadre; ptr; ptr = ptr->NextCadre)
   {
    if(previous != NULL)
    {
     if((textframe = NEW(c_TextFrame)()) == NULL) break;
     F(textframe).link(textframe, previous, FALSE);
    }
    F(ptr->cadre).attachobject(ptr->cadre, textframe, CADRE_TEXT);
    previous = textframe;
    F(SelectedPage).draw_cadre(SelectedPage, ptr); 
   }
  }
  else
  {
   SelectedPage->SelectedCadre->cadre->object_type = CADRE_EMPTY;
   F(SelectedPage->SelectedCadre->cadre).
    attachobject(SelectedPage->SelectedCadre->cadre, 
		 textframe, CADRE_TEXT);
   F(SelectedPage).draw_cadre(SelectedPage, 
			      SelectedPage->SelectedCadre); 
  }
  ((c_Document *)DocTmp)->modified = TRUE;
 }
 Xc_free(filename);
 F(SelectedPage).set_function(SelectedPage, SelectedPage->function_type);
 return;
}

/* ----------------------------------------------------------------- ** 
** EditText - callback for Edit Text Icon                            ** 
** ----------------------------------------------------------------- */
void EditText(w)
Widget	w;
{
 c_AEditor	*Editor;
 void		*Doc;
 int		Type;
 c_Page		*Page;
 c_Text 	*Tmp;

 Doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);

 if (Type == DOCUMENT_PAO)
 {
  SET_WAIT_MODE_STATIC;
  Page = ((c_Document *)Doc)->SelectedPage->Page;

  if (Page->SelectedCadre)
  {

   if (Page->SelectedCadre->cadre->object_type == CADRE_TEXT)
    Tmp = ((c_TextFrame *)Page->SelectedCadre->cadre->object)->text;
   else
   {
    Tmp = NULL;
   }

   Editor = (c_AEditor *)NEW(c_AEditor)
    (MainInterface1->w_Main, "Editor", Tmp, Page);
   switch(F(Editor->Dialog).waitForUnmap(Editor->Dialog))
   {
   case XcCD_DESTROY:
   case XcCD_CANCEL:
    F(Editor).Editor_Cancel(Editor);
    DELETE(c_AEditor)(Editor);
    break;
   case XcCD_OK:
    F(Editor).Editor_OK(Editor);
    ((c_Document *)Doc)->modified = TRUE;
    DELETE(c_AEditor)(Editor);

    if (!Tmp)
    {
     c_TextFrame *textframe, *previous = NULL;
     l_Cadre *ptr;
     
     textframe = (c_TextFrame *)Page->SelectedCadre->cadre->object;
     for(ptr = Page->SelectedCadre; ptr; ptr = ptr->NextCadre)
     {
      if(previous != NULL)
      {
       if ((textframe = NEW(c_TextFrame)()) == NULL) break;
       F(textframe).link(textframe, previous, FALSE);
       F(ptr->cadre).attachobject(ptr->cadre, textframe, CADRE_TEXT);
      }
      previous = textframe;
      F(Page).draw_cadre(Page, ptr); 
     }
    }
    break;
   }
  }
  UNSET_WAIT_MODE;
 }
}

void MenuFilePrint(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 void		 *DocSelected;
 c_PostScript	 *print;
 c_Box_imprimer *box;
 int		 Type;	
 c_Page	*SelectedPage;
 printer_t	*printer;
 listpage	*list_page;
 FILE *handle;

 DocSelected = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (Type == DOCUMENT_PAO)
 {
  int rc;
  list_page = ((c_Document *)DocSelected)->ListPage;
  while(list_page->NextPage)
   list_page = list_page->NextPage;
  GlobPrintParam.NbPage = list_page->Page->page_number + 
   (list_page->Page->page_type == Xc_PAGE_DOUBLE);
  if (GlobPrintParam.EndPage>GlobPrintParam.NbPage)
   GlobPrintParam.EndPage = GlobPrintParam.NbPage;
  if (GlobPrintParam.StartPage>GlobPrintParam.NbPage || 
      GlobPrintParam.StartPage>GlobPrintParam.EndPage)
   GlobPrintParam.StartPage = 1;
  GlobPrintParam.AllPages = ((GlobPrintParam.StartPage == 1) &&
			     (GlobPrintParam.EndPage 
			      ==GlobPrintParam.NbPage));
  SET_WAIT_MODE_STATIC;
  box = (c_Box_imprimer *)NEW(c_Box_imprimer)(MainInterface1->w_Main, 
					      "BoxPrint",
					      &GlobPrintParam);
  rc = F(box->Dialog).waitForUnmap(box->Dialog);
  DELETE(c_Box_imprimer)(box);
  if(rc != XcCD_OK) { UNSET_WAIT_MODE; return;}
      
  printer = GlobPrintParam.printer;
  SelectedPage =  ((c_Document *)DocSelected)->SelectedPage->Page;
      
  if(printer->command == NULL)
  {
   if((PrintDocumentSelector != NULL) ||
      CreatePrintDocumentSelector(MainInterface1->w_Main))
   {
    char *filename;
	      
    filename =
     F(PrintDocumentSelector).getFile(PrintDocumentSelector);
    if(filename != NULL)
    {
     if((print = NEW(c_PostScript)(&GlobPrintParam,
				   filename, -1)) 
	!= NULL)
     {
      SET_WAIT_MODE_ANIM;
      PS_PRINT_DOCUMENT(DocSelected)(DocSelected, print);
      DELETE(c_PostScript)(print);
     }
     Xc_free(filename);
    }
   }
  }
  else if((handle = popen(printer->command, "w")) != NULL)
  {
   print = NEW(c_PostScript)(&GlobPrintParam, printer->name,
			     fileno(handle));
   if(print != NULL)
   {
    SET_WAIT_MODE_ANIM;
    PS_PRINT_DOCUMENT(DocSelected)(DocSelected, print);
    DELETE(c_PostScript)(print);
   }
   pclose(handle);
  }
 }
 UNSET_WAIT_MODE_STATIC;
 UNSET_WAIT_MODE_ANIM;
}

void MenuFileQuit()
{
 char		*message;
  
 MenuFileSaveall();
 message = F(GlobResources).getString(GlobResources, XcR_quitMessage);
 if (Xc_YesNoAlert(message))
  QuitApplication();
 else
  return;
}

void QuitApplication()
{
 int            i;

 Xc_HISTORY(("Quit Xclamation"));

 handle_event_allowed = FALSE;

 F(MainInterface1).unmap_TopLevel(MainInterface1);
 WriteUserConfigFile();

#ifdef  SIGNALH
 DELETE(c_SignalHandler)(SignalH);
#endif

 DELETE(c_DocumentList)(DocList);

 while(GlobTextRulerBase != NULL) DELETE(c_TextRuler)(GlobTextRulerBase);
 while(GlobTextStyleBase != NULL) DELETE(c_TextStyle)(GlobTextStyleBase);
 while(GlobColorBase != NULL) DELETE(c_Color)(GlobColorBase);
 while(GlobFontBase != NULL) DELETE(c_VectorFont)(GlobFontBase);

 DELETE(c_Trash)(GlobTrash);
 DELETE(c_WinManager)(Gwm, XtDisplay(WIDGET(MainInterface1)));
 DELETE(c_LM_Client)(High_Client);
 DELETE(c_ClipBuffer)(GlobClipBuffer);
 DELETE(c_ClipIcon)(ClipIcon);
 DELETE(c_Colormap)(GlobColormap);
 DELETE(c_Keyboard)(GlobKeyboard);
 DELETE(c_DocIcon)(GlobDocIcon);
 DELETE(c_FontManager)(GlobFntMng);
 DELETE(c_ImageCache)(GlobImgCache);
 DELETE(c_ImageManager)(GlobImgMng);
 DELETE(c_DragAndDrop)(GlobDragAndDrop);
 DELETE(c_XErrorHandler)(XErrH);
 Reset_Regle();
  
 if (t1imager_buffer) Xc_free(t1imager_buffer);
 if (Globprog_name) Xc_free(Globprog_name);
  
 /*--- File selectors ---*/
 if(ImportTextSelector != NULL)
  DestroyImportTextSelector();
 if(ExportTextSelector != NULL)
  DestroyExportTextSelector();
 if(ImportImageSelector != NULL) 
  DELETE(c_FileSelector)(ImportImageSelector);
 if(ImportVectorSelector != NULL)
  DELETE(c_FileSelector)(ImportVectorSelector);
 if(OpenDocumentSelector != NULL)
  DELETE(c_FileSelector)(OpenDocumentSelector);
 if(CloseDocumentSelector != NULL)
  DELETE(c_FileSelector)(CloseDocumentSelector);
 if(PrintDocumentSelector != NULL)
  DELETE(c_FileSelector)(PrintDocumentSelector);

 DELETE(c_Icon)(IconMis_1); DELETE(c_Icon)(IconMis_2);
 DELETE(c_Icon)(IconMis_s1); DELETE(c_Icon)(IconMis_3);
 DELETE(c_Icon)(IconMis_4); DELETE(c_Icon)(IconMis_s2);
 DELETE(c_Icon)(IconMis_5); DELETE(c_Icon)(IconMis_6);
 DELETE(c_Icon)(IconMis_7); DELETE(c_Icon)(IconMis_8);

 DELETE(c_Icon)(IconFra_0); DELETE(c_Icon)(IconFra_1);
 DELETE(c_Icon)(IconFra_10); DELETE(c_Icon)(IconFra_11);
 DELETE(c_Icon)(IconFra_12); DELETE(c_Icon)(IconFra_13);
 DELETE(c_Icon)(IconFra_14); DELETE(c_Icon)(IconFra_s1);
 DELETE(c_Icon)(IconFra_3); DELETE(c_Icon)(IconFra_4);
 DELETE(c_Icon)(IconFra_5); DELETE(c_Icon)(IconFra_s2);
 DELETE(c_Icon)(IconFra_s3); DELETE(c_Icon)(IconFra_s4);
 DELETE(c_Icon)(IconFra_6); DELETE(c_Icon)(IconFra_7);
 DELETE(c_Icon)(IconFra_8); DELETE(c_Icon)(IconFra_9);
 DELETE(c_Icon)(IconFra_15); DELETE(c_Icon)(IconFra_16);
 DELETE(c_Icon)(IconFra_17); DELETE(c_Icon)(IconFra_18);
 DELETE(c_Icon)(IconFra_s5); DELETE(c_Icon)(IconFra_19);

 DELETE(c_Icon)(IconTex_1); DELETE(c_Icon)(IconTex_s1);
 DELETE(c_Icon)(IconTex_2); DELETE(c_Icon)(IconTex_3);
 DELETE(c_Icon)(IconTex_4); DELETE(c_Icon)(IconTex_5);
 DELETE(c_Icon)(IconTex_6);

 DELETE(c_Icon)(IconIma_1); DELETE(c_Icon)(IconIma_2);
 DELETE(c_Icon)(IconIma_3); DELETE(c_Icon)(IconIma_s1);
 DELETE(c_Icon)(IconIma_4); DELETE(c_Icon)(IconIma_5); 
 DELETE(c_Icon)(IconIma_s2); DELETE(c_Icon)(IconIma_6);
 DELETE(c_Icon)(IconIma_7); DELETE(c_Icon)(IconIma_8);
 DELETE(c_Icon)(IconIma_9);

 DELETE(c_Icon)(IconVec_s1); DELETE(c_Icon)(IconVec_1);  

 DELETE(c_Icon)(IconZoo_1); DELETE(c_Icon)(IconZoo_2);
 DELETE(c_Icon)(IconZoo_s1); DELETE(c_Icon)(IconZoo_3); 
 DELETE(c_Icon)(IconZoo_4); DELETE(c_Icon)(IconZoo_5); 
 DELETE(c_Icon)(IconZoo_6); DELETE(c_Icon)(IconZoo_s2); 
 DELETE(c_SpinBox)(sbZoom);
 DELETE(c_Icon)(IconZoo_s3); 
 DELETE(c_Icon)(IconZoo_7); DELETE(c_Icon)(IconZoo_8);
 DELETE(c_Icon)(IconZoo_9); DELETE(c_Icon)(IconZoo_10);
 DELETE(c_Icon)(IconZoo_11); DELETE(c_Icon)(IconZoo_12);

 for (i = 0; i < NB_HBAR; i++)
 {
  DELETE(c_RowColumn)(hIconBar[i]);
  DELETE(c_Icon)(vIcon[i]);
 }

 DELETE(c_Icon)(IconTra);

 DELETE(c_Form)(hIconBarForm);
 DELETE(c_RowColumn)(vIconBar_1);
 DELETE(c_RowColumn)(vIconBar_2);
 XtDestroyWidget(w_vIconBar_3);
 DELETE(c_Icon)(IconVlogo);
 DELETE(c_Form)(vIconBarlogo);
 DELETE(c_Form)(vIconBar);

 DELETE(c_DrawingArea)(MainDocument);
 DELETE(c_RowColumn)(Draw3);
 DELETE(c_ScrolledWindow)(ClipBoardPrivate);
  
 DELETE(c_Icon)(IconBottom);
 DELETE(c_Form)(BottomBar);

 DELETE(c_PulldownMenu)(MenuFil);
 DELETE(c_PulldownMenu)(MenuEdi);
 DELETE(c_PulldownMenu)(MenuPge);
 DELETE(c_PulldownMenu)(MenuFra);
 DELETE(c_PulldownMenu)(MenuDis);
 DELETE(c_PulldownMenu)(MenuWin);
 DELETE(c_PulldownMenu)(MenuHel);

 DELETE(c_MenuBar)(Bar1);

 DELETE(c_MainInterface)(MainInterface1);

 DELETE(c_GCursor)(GlobCursor);
 DELETE(c_HelpLine)(GlobHelpLine);
 DELETE(c_RegisterHelp)(GlobRegHelp);
 DELETE(c_RemoteServer)(GlobRemoteServer);
 DELETE(c_Resource)(GlobResources);

#ifndef NMEMDEBUG
 Xc_mdump();
#endif

 Xc_exit(0);
}

void MenuEditStyles(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_styles  *box;
 void		*doc;
 int		Type;	

 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);

 if (Type == DOCUMENT_PAO)
 {
  SET_WAIT_MODE_STATIC;
  box = 
   (c_Box_styles *)NEW
    (c_Box_styles)(MainInterface1->w_Main, 
		   "BoxStyles",
		   &(((c_Document *)doc)->BaseStd), NULL);
  if (F(box->PBox->Dialog).waitForUnmap(box->PBox->Dialog) ==  XcCD_OK)
   ((c_Document *)doc)->modified = TRUE;
  DELETE(c_Box_styles)(box);
  UNSET_WAIT_MODE;
 }
}

void MenuEditRulers(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_regles  *box;
 void		*doc;
 int		Type;	

 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (Type == DOCUMENT_PAO)
 {
  SET_WAIT_MODE_STATIC;
  box = 
   (c_Box_regles *)NEW
    (c_Box_regles)(MainInterface1->w_Main, 
		   "BoxRulers",
		   &(((c_Document *)doc)->BaseStd.text_ruler_base));
  if (F(box->PBox->Dialog).waitForUnmap(box->PBox->Dialog) ==  XcCD_OK)
   ((c_Document *)doc)->modified = TRUE;
  DELETE(c_Box_regles)(box);
  UNSET_WAIT_MODE;
 }
}


void MenuFrameAttribs(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_attrib	*box;
 void		*doc;
 int		Type;	

 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (Type == DOCUMENT_PAO)
 {
  SET_WAIT_MODE_STATIC;
  box = (c_Box_attrib *)NEW(c_Box_attrib)
   (MainInterface1->w_Main, "BoxAttribs",
    ((c_Document *)doc)->BaseStd.color_base,
    ((c_Document *)doc)->SelectedPage->Page->SelectedCadre);
  if (F(box->PBox->Dialog).waitForUnmap(box->PBox->Dialog) ==  XcCD_OK)
   ((c_Document *)doc)->modified = TRUE;
  DELETE(c_Box_attrib)(box);
  UNSET_WAIT_MODE;
 }
}

void MenuFrameAlign(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_aligner	*box;
 void		*doc;
 int		Type;	
 int		nbr_cad;
 l_Cadre	*list;

 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (Type == DOCUMENT_PAO)
 {
  SET_WAIT_MODE_STATIC;
  list = ((c_Document *)doc)->SelectedPage->Page->SelectedCadre;
  nbr_cad = 0;
  while ((nbr_cad < 3) && list)
  {
   nbr_cad++;
   list = list->NextCadre;
  }
  box = (c_Box_aligner *)NEW(c_Box_aligner)
   (MainInterface1->w_Main, "BoxAlign", &GlobAlignParam, nbr_cad);

  if (F(box->Dialog).waitForUnmap(box->Dialog) == XcCD_OK)
  {
   DELETE(c_Box_aligner)(box);
   F( ((c_Document *)doc)->SelectedPage->Page )
    .align_cadre(((c_Document *)doc)->SelectedPage->Page,
		 &GlobAlignParam);
   ((c_Document *)doc)->modified = TRUE;
  }
  else
   DELETE(c_Box_aligner)(box);
  UNSET_WAIT_MODE;
 }
}


void MenuEditColors(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_color	*box;
 void		*doc;
 int		Type;

 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (Type == DOCUMENT_PAO)
 {
  SET_WAIT_MODE_STATIC;
  box = (c_Box_color *)NEW(c_Box_color)
   (MainInterface1->w_Main, "BoxColors",
    &(((c_Document *)doc)->BaseStd.color_base));
  if (F(box->PBox->Dialog).waitForUnmap(box->PBox->Dialog) ==  XcCD_OK)
   ((c_Document *)doc)->modified = TRUE;
  DELETE(c_Box_color)(box);
  UNSET_WAIT_MODE;
 }
}

void MenuHelpCallHelp(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 F(GlobRegHelp).call_help_for_widget(GlobRegHelp, w);
}

void MenuHelpTrackHelp(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 F(GlobRegHelp).tracking_help(GlobRegHelp, MainInterface1->w_Main);
}

void MenuHelpAbout(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_about *boxabout;

 SET_WAIT_MODE_STATIC;
 boxabout = (c_Box_about *)NEW(c_Box_about)(MainInterface1->w_Main,
					    "BoxAbout");
 F(boxabout->Dialog).waitForUnmap(boxabout->Dialog);
 DELETE(c_Box_about)(boxabout);
 UNSET_WAIT_MODE;
}

void MenuHelpAuthor(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_author *boxauthor;

 SET_WAIT_MODE_STATIC;
 boxauthor = (c_Box_author *)NEW(c_Box_author)(MainInterface1->w_Main,
					    "BoxAuthor");
 F(boxauthor).waitForUnmap(boxauthor);
 DELETE(c_Box_about)(boxauthor);
 UNSET_WAIT_MODE;
}

/* ----------------------------------------------------------------- ** 
** MenuHelpLegal                                                     ** 
** ----------------------------------------------------------------- */
static void MenuHelpLegal(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_legal  *box;
 c_Box_legal2 *box2;
 boolean out = True;

 do
 {
  box = (c_Box_legal *)NEW(c_Box_legal)(MainInterface1->w_Main,
					"BoxLegal");
  
  switch(F(box->Dialog).waitForUnmap(box->Dialog))
  {
  case XcCD_CONTINUE:
  case XcCD_CANCEL:
  case XcCD_DESTROY:
   DELETE(c_Box_legal)(box);
   out = False;
   break;;
  case XcCD_OK:
   DELETE(c_Box_legal)(box);
   box2 = (c_Box_legal2 *)NEW(c_Box_legal2)(MainInterface1->w_Main,
					    "BoxLegal2");
   F(box2->Dialog).waitForUnmap(box2->Dialog);
   DELETE(c_Box_legal2)(box2);
   break;
  }
 } 
 while(out);

}

/*---------------------------------------------------------------------*/
void MenuHelpRegistration(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_registration *box;

 box = (c_Box_registration *)NEW(c_Box_registration)(MainInterface1->w_Main,
						     "BoxRegistration");
 F(box->Dialog).waitForUnmap(box->Dialog);
 DELETE(c_Box_registration)(box);
}
/*---------------------------------------------------------------------*/
void MenuEditPrinters(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 /*  c_Box_printers *box;
     
     box = (c_Box_printers *)NEW(c_Box_printers)(MainInterface1->w_Main,
     "BoxPrinters");
     F(box->PBox->Dialog).waitForUnmap(box->PBox->Dialog);
     DELETE(c_Box_printers)(box);
     */
}
/*---------------------------------------------------------------------*/
void MenuEditCache(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_cache *box;
  
 box = (c_Box_cache *)NEW(c_Box_cache)(MainInterface1->w_Main,
				       "BoxCache");
 F(box->Dialog).waitForUnmap(box->Dialog);
 DELETE(c_Box_cache)(box);
}
/*---------------------------------------------------------------------*/
void MenuPageGridSetup(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_gridSetup *box;
 c_Page	*page;
 void	*Doc;
 int	Type, rc;
 new_doc_s savend;

 Doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 
 if (Type == DOCUMENT_PAO)
 {
  SET_WAIT_MODE_STATIC;

  page =  ((c_Document *)Doc)->SelectedPage->Page;
  
  memcpy(&savend, &GlobNewDocInfo, sizeof(new_doc_s));
  GlobNewDocInfo.g_orx = page->Page_Grid.sOrig_X - page->Page_Contour.sOrig_X;
  GlobNewDocInfo.g_ory = page->Page_Grid.sOrig_Y - page->Page_Contour.sOrig_Y;
  GlobNewDocInfo.g_dtx = page->Page_Grid.sEcart_X;
  GlobNewDocInfo.g_dty = page->Page_Grid.sEcart_Y;
      
  box = (c_Box_gridSetup *)NEW(c_Box_gridSetup)(MainInterface1->w_Main,
						"BoxGridSetup");
  rc = F(box->Dialog).waitForUnmap(box->Dialog);
  DELETE(c_Box_gridSetup)(box);
  
  switch(rc)
  {
  case XcCD_OK:
   F(page).init_Page(page, page->page_number,
		     page->page_format,
		     page->page_type,
		     page->page_orient,
		     page->Page_Typo_Mark->left_margin,
		     page->Page_Typo_Mark->right_margin,
		     page->Page_Typo_Mark->top_margin,
		     page->Page_Typo_Mark->bottom_margin,
		     page->Page_Typo_Mark->column,
		     page->Page_Typo_Mark->row,
		     page->Page_Typo_Mark->gouttiere_x,
		     page->Page_Typo_Mark->gouttiere_y,
		     GlobNewDocInfo.g_orx,
		     GlobNewDocInfo.g_ory,
		     GlobNewDocInfo.g_dtx,
		     GlobNewDocInfo.g_dty,
		     XPi_MODIFY); 
   ((c_Document *)Doc)->modified = TRUE;
   SelectFunction((Widget)0, REDRAW_ALL);
  case XcCD_CANCEL:
  case XcCD_DESTROY:
   memcpy(&GlobNewDocInfo, &savend, sizeof(new_doc_s));
   break;
  }
  UNSET_WAIT_MODE;
 }
}
/*---------------------------------------------------------------------*/
void MenuEditSave(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 /*  c_Box_setupsave *box;
     
     box = (c_Box_setupsave *)NEW(c_Box_setupsave)(MainInterface1->w_Main,
     "BoxSetupSave");
     F(box->Dialog).waitForUnmap(box->Dialog);
     DELETE(c_Box_setupsave)(box);
     */
}

void DocumentStyle(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
}

/* ----------------------------------------------------------------- ** 
** AttachHBar - attach and manage a horizontal icon bar              ** 
** ----------------------------------------------------------------- */
static void AttachHBar(hIBarForm, hIBar)
c_Form		*hIBarForm;
c_RowColumn	*hIBar;
{
 F(hIBarForm).attach_left(WIDGET(hIBarForm), WIDGET(hIBar), NULL);
 F(hIBarForm).attach_top(WIDGET(hIBarForm), WIDGET(hIBar), NULL);
 F(hIBarForm).attach_bottom(WIDGET(hIBarForm), WIDGET(hIBar), NULL);
 F(hIBarForm).attach_right(WIDGET(hIBarForm), WIDGET(hIBar), NULL);

 XtManageChild(WIDGET(hIBar));
}

/* -------------------------------------------------------------------- ** 
** SelectHBarCallback - called when an icon from the VBar is activated  ** 
** -------------------------------------------------------------------- */
static void SelectHBarCallback(w, num, cbs)
Widget	w;
int	num;
XmToggleButtonCallbackStruct	*cbs;
{
 if (cbs->set == False)
  return;
 
 if (bar_selected != num)
 {
  AttachHBar(hIconBarForm, hIconBar[num]);
  XtUnmanageChild(WIDGET(hIconBar[bar_selected]));
  bar_selected = num;
 }
}

void ToggleSwitch(w, funct)
Widget	w;
int	funct;
{
 int i;
  
 if (store_function != funct)
 {
  i = 0;
  while ((toggletable[i].func_num != store_function) && 
	 (toggletable[i].tog_wid != 0))
   i++;
  if (toggletable[i].tog_wid != 0)
   XmToggleButtonSetState(toggletable[i].tog_wid,
			  False, False);
  store_function = funct;
 }
 else
 {
  XmToggleButtonSetState(w, True, False);
 }
 SelectFunction(w, funct);
}


/* ----------------------------------------------------------------- ** 
** ToggleImagemodeSwitch                                             ** 
** ----------------------------------------------------------------- */
static void ToggleImagemodeSwitch(w, funct)
Widget	w;
int	funct;
{
 int i, j;

 tog_tab2_t *toggletabptr;

 j = 0;
 if ((funct > IMAGE_FUNCTION_BEGIN) && (funct < IMAGE_FUNCTION_END)) 
  toggletabptr = (tog_tab2_t *)toggleimagemodetable;
 else
  toggletabptr = (tog_tab2_t *)togglevectormodetable;

 while ((toggletabptr[j].func_num != funct)
	&& (toggletabptr[j].tog_wid != 0))
  j++;
  
 if (store_widget_imagevector != toggletabptr[j].tog_wid)
 {
  i = 0;
  while ((toggletabptr[i].tog_wid != store_widget_imagevector)
	 && (toggletabptr[i].tog_wid != 0))
   i++;
  if (toggletabptr[i].tog_wid != 0)
   XmToggleButtonSetState(toggletabptr[i].tog_wid,
			  False, False);
  store_widget_imagevector = toggletabptr[j].tog_wid;

  SelectFunction(w, funct);
 }
 else
 {
  XmToggleButtonSetState(w, True, False);
 }
}

/* ----------------------------------------------------------------- ** 
** ToggleSetImage                                                    ** 
** ----------------------------------------------------------------- */
static void ToggleSetImage(type, mode_num)
int type;
int mode_num;
{
 int j, i;

 tog_tab2_t *toggletabptr;

 j = 0;
 if (type == CADRE_IMAGE_BITMAP)
  toggletabptr = (tog_tab2_t *)toggleimagemodetable;
 else
  toggletabptr = (tog_tab2_t *)togglevectormodetable;

 while ((toggletabptr[j].mode_num != mode_num)
	&& (toggletabptr[j].tog_wid != 0))
  j++;

 if (toggletabptr[j].tog_wid != store_widget_imagevector)
 {
  XmToggleButtonSetState(toggletabptr[j].tog_wid,
			 True, False);
  i = 0;
  while ((toggletabptr[i].tog_wid != store_widget_imagevector)
	 && (toggletabptr[i].tog_wid != 0))
   i++;
  if (toggletabptr[i].tog_wid != 0)
   XmToggleButtonSetState(toggletabptr[i].tog_wid,
			  False, False);
  store_widget_imagevector = toggletabptr[j].tog_wid;
 }
}


/* objet des fonctions selected */
void SelectFunction(w, funct)
Widget	w;
int	funct;
{

 void	*DocSelected;
  
 DocSelected = F(DocList).Find(DocList,
			       Gwm->SelectedWindow->MainWin, NULL);
 if (DocSelected)
  SET_FUNCTION_DOCUMENT(DocSelected)(DocSelected, funct);
}

/*---------------------------------------------------------------------*/
void MenuWindowArrange(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 Display	*display;

 display = XtDisplay(WIDGET(MainInterface1));
 F(Gwm).ArrangeWindow(Gwm, display, XtWindow(WIDGET(MainDocument)));
}
/*---------------------------------------------------------------------*/
void MenuWindowTile(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 Display	*display;

 display = XtDisplay(WIDGET(MainInterface1));
 F(Gwm).TileWindow(Gwm, display, XtWindow(WIDGET(MainDocument)));
}
/*---------------------------------------------------------------------------*/
void SChangeWindow(listOld, listNew)
listmanaged	*listOld;
listmanaged	*listNew;
{
 void	*DocTmp1;
 void	*DocTmp2;
 int	Type1;
 int	Type2;
  
 if (listOld)
 {
  DocTmp1 = F(DocList).Find(DocList, listOld->MainWin, &Type1);
  if (Type1 == DOCUMENT_PAO)
  {
   ABORT(((c_Document *)DocTmp1)->SelectedPage->Page);
   INIT(((c_Document *)DocTmp1)->SelectedPage->Page);
   F(((c_Document *)DocTmp1)->SelectedPage->Page->cursor).unset_cursor
    (((c_Document *)DocTmp1)->SelectedPage->Page->cursor);
  }
 }
 DocTmp2 = F(DocList).Find(DocList, listNew->MainWin, &Type2);
 if (Type2 == DOCUMENT_PAO)
 {
  ABORT(((c_Document *)DocTmp2)->SelectedPage->Page);
  INIT(((c_Document *)DocTmp2)->SelectedPage->Page);
  F(((c_Document *)DocTmp2)->SelectedPage->Page->cursor).reset_cursor
   (((c_Document *)DocTmp2)->SelectedPage->Page->cursor);
 }
}

void ChangeDocCallback(window)
Window	window;
{
 Display     *display;
 listmanaged *listm;
 boolean	maximize = FALSE;
  
 if (Gwm->SelectedWindow && Gwm->SelectedWindow->Type == D_MAXIMIZED)
  maximize = TRUE;
  
 listm = Gwm->ListManaged;
 while(listm && listm->MainWin != window)
  listm = listm->NextManaged;
 
 if (listm)
 {
  display = XtDisplay(WIDGET(MainInterface1));
    
  F(Gwm).DiselectWindow(Gwm, display, listm);
    
  if (listm->Type == D_ICONED)
   F(Gwm).RaiseIcon(Gwm, display, listm);
    
  if (maximize)
   F(Gwm).MaxWindow(Gwm, display, listm, XtWindow(WIDGET(MainDocument)));
  else
   XRaiseWindow(display, listm->Win);
 }
}

/* ----------------------------------------------------------------- ** 
** Certify zoom factor -> change zoom factore		             ** 
** ----------------------------------------------------------------- */
static void certify_zoom(This, param)
c_WTextField *This;
void *param;
{
 c_Document	*document;
 c_Page		*page;
 scale_t	num;
 int		Type;

 Xc_HISTORY(("certify fonte size"));

 document = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (!document || Type != DOCUMENT_PAO)
  return;

 num = (scale_t)F(This).get_value_in_unit(This);
 F(This).set_value_in_unit(This, SCALE_TO_PERCENT(num));
 
 page = document->SelectedPage->Page;
 F(page).zoom_page_at_percent(page, num);
 
 /*XtSetArg(argsw[0], XmNtraversalOn, False);*/
 /*XtSetValues(WIDGET(This), argsw, 1);*/

 motif_focus = tffs_focus = FALSE;

 if (!XmProcessTraversal(WIDGET(MainDocument), XmTRAVERSE_CURRENT))
  Xc_TRACE(("XmProcessTraversal failed"));
}

/* ----------------------------------------------------------------- ** 
** callbacks for widget that need key focus                          ** 
** ----------------------------------------------------------------- */
static void cb_focus_motif(w, none, cbs)
Widget		w;
void		*none;
XmRowColumnCallbackStruct *cbs;
{
 Xc_TRACE(("FOCUS MOTIF"));
 motif_focus = TRUE;
  
}

static void cb_unfocus_motif(w, none, cbs)
Widget 		w;
void		*none;
XmRowColumnCallbackStruct *cbs;
{
 Xc_TRACE(("UNFOCUS MOTIF"));
 motif_focus = FALSE;
}

static void cb_focus_tfZoom(w, This, event)
Widget w;            
c_WTextField *This;
XEvent	*event;
{
 Xc_TRACE(("FOCUS MOTIF (for tfZoom)"));
 
 motif_focus = tffs_focus = TRUE;
 GlobKeyboard->event_enable = FALSE;
}

static void cb_unfocus_tfZoom(w, This, event)
Widget w;            
c_WTextField *This;
XEvent	*event;
{
 Xc_TRACE(("UNFOCUS MOTIF (for tfZoom)"));
 
 XtCallCallbacks(w, XmNlosingFocusCallback, This);
 
 motif_focus = tffs_focus = FALSE;
 GlobKeyboard->event_enable = TRUE;
}

/* ----------------------------------------------------------------- ** 
** Signal Error Manager                                              ** 
** ----------------------------------------------------------------- */

void Signal_Error_Handler(resource)
int	resource;
{
 XmString	xms;
 boolean	save;
  
 if (SigErrorCount == 0)
 {
  SigErrorCount = 1;
  if (IOErrorCount == 1)
  {
   c_DocFile *doc;
   listdocument *tmplist;
   c_Document *Document;
   char filename[20];
   int  i=0;
	
   Xc_WARNING(("Signal Error In IOError Handling. FATAL..."));
   Xc_WARNING(("Try to saving all"));
   tmplist = DocList->ListDocument;
   while (tmplist)
   {
    Document = tmplist->Document;
    if (Document->filename)
    {
     Xc_WARNING(("saving file name: %s", Document->filename));
     doc = NEW(c_DocFile)(XcDF_SAVE, Document->filename, -1);
    }
    else
    {
     sprintf(filename, "Crash_svg_%03d.xc", i++); 
     Xc_WARNING(("saving file name: %s", filename));
     doc = NEW(c_DocFile)(XcDF_SAVE, filename, -1);
     Document->filename = filename;
    }
    if(WRITE_DOCUMENT(Document)(Document, doc) == FALSE)
     Xc_WARNING(("save error"));
    DELETE(c_DocFile)(doc);      
    tmplist = tmplist->NextDocument;
   }
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
  save = (DocList->ListDocument != NULL);
    
  switch(Xc_SigErrorAlert(xms, save))
  {
  case XcCD_SAVE:
   XmStringFree(xms);
   Xc_TRACE(("Should save if possible"));
   MenuFileSaveall();
   xms = F(GlobResources).getXmString(GlobResources, XcR_sigErrorQuiting);
   Xc_ErrorAlert2(xms);
   XmStringFree(xms);
   Xc_exit (-1);
   break;
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

/* ----------------------------------------------------------------- ** 
** X IOError Manager                                                 ** 
** ----------------------------------------------------------------- */

void IO_Error_Handler(none)
int	none;
{
 XmString	xms;
 boolean	save;
 Display	*display;

 if (IOErrorCount == 0)
 {
  IOErrorCount = 1;
  Xc_TRACE(("IO Error Handle"));
    
  F(MainInterface1).reopen_display(MainInterface1);
  display =  XtDisplay(MainInterface1->w_Hidden);
   
  CloseDocumentSelector = NULL;
  GlobResources = NEW(c_Resource)(MainInterface1->w_Hidden);
  GlobColormap = (c_Colormap *)NEW(c_Colormap)(display, 0, NO_FORCE);
  GlobCursor = (c_GCursor *)NEW(c_GCursor)(display);
  save = (DocList->ListDocument != NULL);
    
  switch(Xc_IOErrorAlert(save))
  {
  case XcCD_SAVE:
   Xc_TRACE(("Should save if possible"));
   MenuFileSaveall();
   xms = F(GlobResources).getXmString(GlobResources, XcR_sigErrorQuiting);
   Xc_ErrorAlert2(xms);
   XmStringFree(xms);
   Xc_exit (-1);
   break;
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

