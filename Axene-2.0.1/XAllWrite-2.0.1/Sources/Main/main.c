/*
** main.c for XAllWrite in Main/
** Main Part of XAllWrite (Axene's Word Processor)
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
** Started on  Tue Mar  4 17:32:17 1997 Emmanuel Paris
** Last update Fri Feb  4 11:15:41 2000 Emmanuel Paris
*/

#define __XC_MAIN__

#define SIGNALH
#define IOERRORH
#include "xcalibur.h"

#include "WInterface.h"
#define _PRIVATE_PAGEFORMAT_
#include "PageFormat.h"

#include "Readkey.h"
#include "check.h"
#include "config.h"

/*--- Errors Handlers Headers ---*/
#include "XErrorHandler.h"
#include "SignalHandler.h"

/*--- Global Object Headers ---*/
#include "Colormap.h"
#include "Cursor.h"
#include "Drag_And_Drop.h"
#include "TrashCan.h"
#include "ImageManager.h"
#include "Resource.h"
#include "Keyboard.h"
#include "Undo.h"
#include "HelpLine.h"
#include "RegisterHelp.h"
#include "RemoteServer.h"
#include "DocIcon.h"
#include "ClipBuffer.h"
#include "DocumentList.h"
#include "Document.h"
#include "VectorFont.h"
#include "Color.h"
#include "Import.h"
#include "TextStyle.h"
#include "TextRuler.h"
#include "Text.h"
#include "ImageCache.h"
#include "WinManager.h"
#include "FontManager.h"
#include "PostScript.h"

#ifdef HAVE_XSHM
#include <X11/extensions/XShm.h>
extern Bool XShmQueryExtension();
boolean xshm_flag;
#endif

#ifdef HAVE_XSHAPE
#include <X11/extensions/shape.h>
boolean xshape_flag;
#endif

/*--- Interface Objects Headers ---*/

#include "MainInterface.h"
#include "TimeIndicator.h"
#include "AlertBox.h"
#include "SpecialFileSelector.h"
#include "ManageWidget.h"
#include "RowColumn.h"
#include "MenuBar.h"
#include "PulldownMenu.h"
#include "Form.h"
#include "Icon.h"
#include "ItemMenu.h"
#include "ColorList.h"
#include "SpinBox.h"
#include "WTextField.h"
#include "DrawingArea.h"
#include "Box_color.h"
#include "Box_attrib.h"
#include "Box_styles.h"
#include "Box_imprimer.h"
#include "Box_about.h"
#include "Box_legal.h"
#include "Box_legal2.h"
#include "Box_registration.h"
#include "Box_cache.h"

/*--- Pixmaps inclusion ---*/
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
#include "../Pixmaps/undo.zpm"
#include "../Pixmaps/redo.zpm"

#include "../Pixmaps/fonts.zpm"
#include "../Pixmaps/texte.style.gras.zpm"
#include "../Pixmaps/texte.style.italique.zpm"
#include "../Pixmaps/texte.style.underline.zpm"
#include "../Pixmaps/texte.style.shadow.zpm"
#include "../Pixmaps/texte.style.subscript.zpm"
#include "../Pixmaps/texte.style.supscript.zpm"
#include "../Pixmaps/texte.style.smallcaps.zpm"

#include "../Pixmaps/ruler.zpm"
#include "../Pixmaps/style.gauche.zpm"
#include "../Pixmaps/style.centre.zpm"
#include "../Pixmaps/style.droite.zpm"
#include "../Pixmaps/style.justifie.zpm"
#include "../Pixmaps/style.indent.inc.zpm"
#include "../Pixmaps/style.indent.dec.zpm"
#include "../Pixmaps/style.interline.inc.zpm"
#include "../Pixmaps/style.interline.dec.zpm"
#include "../Pixmaps/style.interparagraph.inc.zpm"
#include "../Pixmaps/style.interparagraph.dec.zpm"
#include "../Pixmaps/special.char.zpm"
#include "../Pixmaps/euro.zpm"

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

#include "../Pixmaps/cadre.zpm"
#include "../Pixmaps/cadre.selectionner.zpm"
#include "../Pixmaps/cadre.creer.zpm"
#include "../Pixmaps/cadre.creer.lignes.zpm"
#include "../Pixmaps/cadre.creer.ovale.zpm"
#include "../Pixmaps/cadre.inserer.point.zpm"
#include "../Pixmaps/cadre.detruire.point.zpm"
#include "../Pixmaps/cadre.bouger.point.zpm"
#include "../Pixmaps/cadre.eloigner.zpm"
#include "../Pixmaps/cadre.approcher.zpm"
#include "../Pixmaps/cadre.premierplan.zpm"
#include "../Pixmaps/cadre.arriereplan.zpm"
#include "../Pixmaps/textflow_around.zpm"
#include "../Pixmaps/textflow_left.zpm"
#include "../Pixmaps/textflow_right.zpm"
#include "../Pixmaps/textflow_skip.zpm"
#include "../Pixmaps/textflow_under.zpm"
#include "../Pixmaps/textflow_over.zpm"
#include "../Pixmaps/textflow_inhole.zpm"
#include "../Pixmaps/textflow_outhole.zpm"
#include "../Pixmaps/cadre.lock.zpm"

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

/*--- Error Handlers Variables ---*/

c_XErrorHandler	*XErrH;
int IOErrorCount = 0;

#ifdef SIGNALH
c_SignalHandler	*SignalH;
jmp_buf SigErrorEnv;
#endif
int SigErrorCount = 1;

/*--- Global Objects Variables ---*/

c_Colormap 	*GlobColormap;
c_GCursor  	*GlobCursor = NULL;
c_ClipBuffer	*GlobClipBuffer;
c_DragAndDrop	*GlobDragAndDrop;
c_Trash		*GlobTrash;
c_ImageManager	*GlobImgMng;
c_ImageCache	*GlobImgCache;
c_FontManager	*GlobFntMng;
c_Keyboard      *GlobKeyboard;
c_Resource	*GlobResources;
c_Undo		*GlobUndo;
c_HelpLine      *GlobHelpLine;
c_RegisterHelp	*GlobRegHelp;
c_RemoteServer	*GlobRemoteServer;
c_DocIcon	*GlobDocIcon;
c_VectorFont	*GlobFontBase = NULL;
c_Color		*GlobColorBase = NULL;
c_TextRuler	*GlobTextRulerBase = NULL;
c_TextStyle	*GlobTextStyleBase = NULL;
c_Text		*GlobTextClipboard = NULL;
printer_t	*GlobPrinterBase = NULL;
c_Import	*GlobImport;
c_DocumentList	*DocList;
c_WinManager	*Gwm;
char		*Globprog_name = NULL;

char	*t1imager_buffer = NULL;
char	*defaultDocumentName;
char	*defaultFrameName;

boolean	motif_focus = FALSE, tffs_focus = FALSE;	
boolean	handle_event_allowed = TRUE;

print_param_t	GlobPrintParam = 
{
 NULL,
 1, 1, 10000, 0, TRUE, TRUE, FALSE, FALSE, FALSE
};

static c_TextStyle *tmp_style = NULL;

static family_tree tmp_family =
{ 
 NULL, "             Fonts List             ", 
 NULL, FALSE, FALSE, FALSE, FALSE
};

static c_TextStyle *saved_style;
static family_tree *saved_family;
coord_t FontSizeValue = COORD_ZERO;

/*--- Interface Objects Variables ---*/

c_MainInterface  *MainInterface1;
Widget wMainWindow;

c_TimeIndicator  *TimeIndicator;
#define TI_NB_LEDS 17

c_ManageWidget *Manager1;
c_ManageWidget *Manager2;

c_DrawingArea  *MainDocument;
c_MenuBar      *Bar1;
c_PulldownMenu *MenuFil;
c_PulldownMenu *MenuEdi;
c_PulldownMenu *MenuDis;
c_PulldownMenu *MenuIns;
c_PulldownMenu *MenuFor;
c_PulldownMenu *MenuWin;
c_PulldownMenu *MenuHel;

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
 MEdi_UNDO = 1,
 MEdi_REDO,
 MEdi_CUT,
 MEdi_COPY,
 MEdi_PASTE,
 MEdi_DELETE,
 MEdi_SELECTALL,
 MEdi_EDITCOLORS,
 MEdi_PREFERENCES,
 MEdi_IMAGECACHE,
 
 MEdi_SEP1,
 MEdi_SEP2,
 MEdi_SEP3,
 MEdi_SEP4,

 MEdi_SUBMENU1
};

enum
{
 MDis_REDRAWALL = 1,
 MDis_REALSIZE,
 MDis_SCREENSIZE,
 MDis_FULLWIDTH,
 MDis_SHOWFRAMESBORDER,
 MDis_SHOWSPECCHARS,
 MDis_SHOWBOTTOMBAR,
 
 MDis_SEP1
};

enum
{
 MIns_PAGEBREAK = 1,
 MIns_EUROSIGN,
 MIns_CHAR,
 MIns_CREATERECTANGLE,
 MIns_CREATEPOLYGON,
 MIns_CREATEELIPSE,

 MIns_SEP1,
 MIns_SEP2
};

enum
{
 MFor_EDITSTYLES = 1,
/*MFor_EDITRULERS,*/

 MFor_FONTS,
 MFor_BOLD,
 MFor_ITALIC,
 MFor_UNDERLINE,
 MFor_STRIKEOUT,
 MFor_SHADOW,
 MFor_SMALLCAPS,
 MFor_BIGCAPS,
 MFor_SUBSCRIPT,
 MFor_SUPSCRIPT,

 MFor_EDITFRAME,
 MFor_POINTINSERT,
 MFor_POINTDELETE,
 MFor_POINTMOVE,
 MFor_MOVEDOWN,
 MFor_MOVEUP,
 MFor_BACKGROUND,
 MFor_FOREGROUND,
 MFor_LOCK,
 MFor_ATTRIBS,
 MFor_FRAMEDELETE,
 
 MFor_TEXTFLOW,
 MFor_TEXTFLOWAROUND,
 MFor_TEXTFLOWLEFT,
 MFor_TEXTFLOWRIGHT,
 MFor_TEXTFLOWSKIP,
 MFor_TEXTFLOWOUTHOLE,
 MFor_TEXTFLOWUNDER,
 MFor_TEXTFLOWOVER,

 MFor_SEP1,
 MFor_SEP2,
 MFor_SEP3,
 MFor_SEP4,
 MFor_SEP5,
 MFor_SEP6,
 MFor_SEP7,
 MFor_SEP8,

 MFor_SUBMENU1,
 MFor_SUBMENU2,
 MFor_SUBMENU3
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
 
 MHel_SEP1,
 MHel_SEP2,
 MHel_SEP3,
 MHel_SEP4
};

enum
{
 HBAR_MISC, HBAR_FONT, HBAR_RULER, HBAR_IMAGE,
 HBAR_FRAME, HBAR_ZOOM, NB_HBAR
};

c_Form          *vIconBar;
c_RowColumn	*vIconBar_1;
c_RowColumn	*vIconBar_2;
Widget    	w_vIconBar_3;
c_Icon		*vIcon[NB_HBAR];
static int	bar_selected = HBAR_FONT;

c_Icon          *IconVlogo;
c_Form          *vIconBarlogo;

c_Form          *hIconBarForm;
c_RowColumn	*hIconBar[NB_HBAR];

c_ItemMenu *IMstyle;
c_ItemMenu *IMfontFamily;
c_SpinBox  *sbFontSize;
c_ColorList *CLfontColor;
c_ColorList *CLfontBgColor;
c_WTextField *tfFontSize;

c_Icon *IconMis_1, *IconMis_2; /* open, new */
c_Icon *IconMis_s1, *IconMis_3, *IconMis_4; /* save print */
c_Icon *IconMis_s2, *IconMis_5, *IconMis_6, *IconMis_7; /* cut/copy/paste */
c_Icon *IconMis_8, *IconMis_9, *IconMis_10; /* del/undo/redo */

c_Icon *IconFon_s1, *IconFon_s2, *IconFon_s3, *IconFon_s4; /*sep*/
/*     bold        italic      underline   strikeout   subscript*/
c_Icon *IconFon_3, *IconFon_4, *IconFon_5, *IconFon_7; 
/*     supscript   shadow         outline        small_caps   big_caps */
c_Icon *IconFon_8, *IconFon_9, /* *IconFon_10,*/ *IconFon_11;

c_Icon *IconRul_s1, *IconRul_s2, *IconRul_s3, *IconRul_s4;
c_Icon *IconRul_1, *IconRul_2, *IconRul_3, *IconRul_4; /* justify LCRJ */
c_Icon *IconRul_5, *IconRul_6; /* increase/decrease indent */
c_Icon *IconRul_7, *IconRul_8; /* increase/decrease interline */
c_Icon *IconRul_9; /* mask/show special char */
c_Icon *IconRul_10, *IconRul_11; /* increase/decrease interparagraph */
c_Icon *IconRul_12;	/* insert euro char */

c_Icon *IconIma_1, *IconIma_2, *IconIma_3, *IconIma_s1;
c_Icon *IconIma_4, *IconIma_5, *IconIma_s2;
c_Icon *IconIma_6, *IconIma_7, *IconIma_8, *IconIma_9, *IconVec_s1;
c_Icon *IconVec_1;

c_Icon *IconFra_s1, *IconFra_s2, *IconFra_s3, *IconFra_s4;
c_Icon *IconFra_1, *IconFra_2, *IconFra_3, *IconFra_4; /* select & create */
c_Icon *IconFra_5, *IconFra_6, *IconFra_7; /* point move, add & destroy */
c_Icon *IconFra_8, *IconFra_9, *IconFra_10, *IconFra_11; /* depth functions */
c_Icon *IconFra_12; /* lock */
c_Icon *IconFra_13; /* rotate */	
c_Icon *IconFra_14, *IconFra_15, *IconFra_16; /* txtflow around, left, right */
c_Icon *IconFra_17, *IconFra_18, *IconFra_19; /* txtflow skip, under, hole */
c_Icon *IconFra_20; /* txtflow over */

c_Icon *IconZoo_1, *IconZoo_2, *IconZoo_3, *IconZoo_s1, *IconZoo_s2;
c_Icon *IconZoo_4, *IconZoo_5, *IconZoo_6, *IconZoo_7,  *IconZoo_8;
c_Icon *IconZoo_9, *IconZoo_10, *IconZoo_11, *IconZoo_12;

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

typedef struct
{
 Widget	tog_wid;
 Widget	tog_wid2;
 int	func_num;
 int	mode_num;
} tog_tab3_t;

static int		store_function;
static int		store_function_ruler;
static Widget		store_widget_imagevector;
static int		store_mode_textflow;

static tog_tab_t	toggletable[30];
static tog_tab2_t	toggleimagemodetable[10];
static tog_tab2_t	togglevectormodetable[10];
static tog_tab3_t	toggletextflowmodetable[10];

c_Form          *BottomBar;
c_Icon		*IconTra;
c_Icon		*IconBottom;

boolean ButtonLockFrame = TRUE;
boolean ButtonHoleOn = FALSE;
boolean ButtonActivBold = FALSE;
boolean ButtonActivItalic = FALSE;
boolean ButtonActivUnderline = FALSE;
boolean ButtonActivStrikeout = FALSE;
boolean ButtonActivSubscript = FALSE;
boolean ButtonActivSupscript = FALSE;
boolean ButtonActivShadow = FALSE;
boolean ButtonActivSmallCaps = FALSE;
boolean ButtonActivBigCaps = FALSE;
boolean ButtonActivJLeft = FALSE;
boolean ButtonActivJCenter = FALSE;
boolean ButtonActivJRight = FALSE;
boolean ButtonActivJustify = FALSE;
boolean ButtonActivSpecChar = TRUE;
boolean ButtonMaskBottomBar = TRUE;
boolean ButtonMaskVectorText = TRUE;
boolean ButtonMaskFrame = FALSE;

boolean	DocAlreadyGrised;
boolean	FrameToolsAlreadyGrised;
boolean	MultiFrameToolsAlreadyGrised;
boolean LockToolsAlreadyGrised;
boolean STextAlreadyGrised;
boolean SImageAlreadyGrised;
boolean SVectorAlreadyGrised;

/* ----------------------------------------------------------------- ** 
** Functions Declarations                                            ** 
** ----------------------------------------------------------------- */

/*--- Callbacks functions ---*/

void DisplayBottomBar();

static void MenuFileNew();
void MenuFileOpen();
static void MenuFileClose();
static void MenuFileSave();
static void MenuFileSaveas();
static void MenuFileSaveall();

void LoadImage ___PROTO((Widget w));
static void LoadText ___PROTO((Widget w));
void LoadVector ___PROTO((Widget w));

static void MenuFilePrint();
static void MenuFileQuit();

static void MenuEditColors();
static void MenuEditStyles();
static void MenuEditCache();

static void MenuFormatFrameAttribs();

static void MenuWindowArrange();
static void MenuWindowTile();

void MenuHelpCallHelp();
static void MenuHelpTrackHelp();
static void MenuHelpAbout();
static void MenuHelpLegal();
static void MenuHelpRegistration();

static void change_style ___PROTO((c_ItemMenu *This, void *Param,
				   c_TextStyle *style));
static void change_font ___PROTO((c_ItemMenu *This, void *Param,
				  family_tree *family));
static void change_font_color ___PROTO((c_ColorList *This,
					void *Param, c_Color *color));
static void change_font_bgcolor ___PROTO((c_ColorList *This,
					  void *Param, c_Color *color));
static void certify_font_size ___PROTO((c_WTextField *This, void *param));

/*--- WM and DocList Callbacks ---*/
void ModStructure();
void SelectFunction();
static void ToggleSwitch();
static void ToggleSwitchRuler();
static void ToggleImagemodeSwitch();
static void ToggleSetImage();
static void ToggleTextflowSwitch();
void SChangeWindow();
void ChangeDocCallback();
void QuitApplication();

/*--- Interface functions ---*/
static void AttachHBar();
static void SelectHBarCallback();
static void UndoCallback();
static void cb_focus_motif ___PROTO((Widget w, void *none,
				     XmRowColumnCallbackStruct *cbs));
static void cb_unfocus_motif ___PROTO((Widget w, void *none,
				       XmRowColumnCallbackStruct *cbs));
static void cb_focus_tfFontSize ___PROTO((Widget w, c_WTextField *This, 
					  XEvent *event));
static void cb_unfocus_tfFontSize ___PROTO((Widget w, c_WTextField *This, 
					    XEvent *event));

/*--- Event Loop functions ---*/

static void SensitivizeDoc ___PROTO((Boolean flag));
static void SensitivizeLockTools();
static void SensitivizeFrameTools();
static void SensitivizeMultiFrameTools();

static void SensitivizeSVector();
static void SensitivizeSImage();
static void SensitivizeSText();

void XcHandleEvent ___PROTO((XEvent *event));
void XcFlushEvents ___PROTO((Widget w, void (*func) ___PROTO((XEvent *))));

/*--- Error Handlers functions ---*/

void Signal_Error_Handler();
void IO_Error_Handler();

/* ----------------------------------------------------------------- ** 
** Main Function                                                     ** 
** ----------------------------------------------------------------- */

int main(argc, argv)
int  argc;
char **argv;
{
 Display  *display;
 Arg      args[10];
 int      nargs;
 int      i;
 XtAppContext	  app;
 char		*vkey;
 
 GlobUndo = (c_Undo *)NEW(c_Undo)();
 
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
 
 GlobFntMng = (c_FontManager *)NEW(c_FontManager)(display);

 MainDocument = (c_DrawingArea *)NEW(c_DrawingArea)(WIDGET(MainInterface1), 
						    Manager1,
						    "MainDocument", 
						    150, 150);
 
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
   if (check_key_validity(&gkinfo, 54642,
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
 ** MenuBar construction                                              ** 
 ** ----------------------------------------------------------------- */
 
 Bar1 = (c_MenuBar *)NEW(c_MenuBar)(WIDGET(MainInterface1), 
				    Manager1, "Menu");

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
 F(MenuFil).AddCallback(MenuFil, MFil_PRINT, MenuFilePrint, NULL);
 
 F(MenuFil).AddCallback(MenuFil, MFil_IMPORTIMAGE, LoadImage, NULL);
 F(MenuFil).AddCallback(MenuFil, MFil_IMPORTTEXT, LoadText, NULL);
 F(MenuFil).AddCallback(MenuFil, MFil_IMPORTVECTOR, LoadVector, NULL);
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
		   "mUndo", MEdi_UNDO,
		   NOMANAGE, Mxxx_NOTHING,
		   "mRedo", MEdi_REDO,
		   MENUSEPARATOR, MEdi_SEP1,
		   "mCut", MEdi_CUT,
		   "mCopy", MEdi_COPY,
		   "mPaste", MEdi_PASTE,
		   MENUSEPARATOR, MEdi_SEP2,
		   "mDelete", MEdi_DELETE,
		   "mSelectAll", MEdi_SELECTALL,
		   MENUSEPARATOR, MEdi_SEP3,
		   "mColors", MEdi_EDITCOLORS,
		   MENUSEPARATOR, MEdi_SEP4,
		   SUBMENUON, MEdi_SUBMENU1,
		   "mPreferences", MEdi_PREFERENCES,
		   "mCache", MEdi_IMAGECACHE,
		   SUBMENUOFF, Mxxx_NOTHING,
		   NULL);

 F(MenuEdi).AddCallback(MenuEdi, MEdi_CUT, SelectFunction, (void *)CUT_TEXT);
 F(MenuEdi).AddCallback(MenuEdi, MEdi_COPY, SelectFunction, (void *)COPY_TEXT);
 F(MenuEdi).AddCallback(MenuEdi, MEdi_PASTE, SelectFunction, (void *)PASTE_TEXT);
 F(MenuEdi).AddCallback(MenuEdi, MEdi_DELETE, SelectFunction,
			(void *)DELETE_TEXT);
 F(MenuEdi).AddCallback(MenuEdi, MEdi_SELECTALL, SelectFunction, 
			(void *)SELECT_ALL_TEXT);
 F(MenuEdi).AddCallback(MenuEdi, MEdi_EDITCOLORS, MenuEditColors, NULL);
 F(MenuEdi).AddCallback(MenuEdi, MEdi_IMAGECACHE, MenuEditCache, NULL);

 XtAddCallback(WIDGET(MenuEdi), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuEdi), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);

 F(TimeIndicator).advance_indicator(TimeIndicator);

 /* ----------------------------------------------------------------- ** 
 ** Menu DISPLAY                                                      ** 
 ** ----------------------------------------------------------------- */

 MenuDis = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1,
		   "pmDisplay",
                   "mRedraw", MDis_REDRAWALL,
                   "mRealSize", MDis_REALSIZE,
		   "mScreenSize", MDis_SCREENSIZE,
                   "mFullWidth", MDis_FULLWIDTH,
                   MENUSEPARATOR, MDis_SEP1,
		   TOGGLEMODEON, Mxxx_NOTHING,
		   "mShowFrameBorders", MDis_SHOWFRAMESBORDER,
                   "mShowSpecChars", MDis_SHOWSPECCHARS,
                   "mShowInfobar", MDis_SHOWBOTTOMBAR,
		   NULL);

 F(MenuDis).AddCallback(MenuDis, MDis_REDRAWALL, 
                        SelectFunction, (void *)REDRAW_ALL);
 F(MenuDis).AddCallback(MenuDis, MDis_REALSIZE, 
                        SelectFunction, (void *)ZOOM_AT_REAL_SIZE);
 F(MenuDis).AddCallback(MenuDis, MDis_SCREENSIZE,
                        SelectFunction, (void *)ZOOM_FIT_IN_SCREEN);
 F(MenuDis).AddCallback(MenuDis, MDis_FULLWIDTH,
                        SelectFunction, (void *)ZOOM_FIT_IN_WIDTH);
 F(MenuDis).AddCallback(MenuDis, MDis_SHOWFRAMESBORDER,
			SelectFunction, (void *)DISPLAY_FRAME_BORDER);
 F(MenuDis).AddCallback(MenuDis, MDis_SHOWSPECCHARS,
                        SelectFunction, (void *)DISPLAY_SPEC_CHARS);
 F(MenuDis).AddCallback(MenuDis, MDis_SHOWBOTTOMBAR,
			DisplayBottomBar, NULL);

 F(MenuDis).ToggleButton(MenuDis, MDis_SHOWBOTTOMBAR, True);

 XtAddCallback(WIDGET(MenuDis), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuDis), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);
 /* ----------------------------------------------------------------- ** 
 ** Menu INSERT                                                       ** 
 ** ----------------------------------------------------------------- */

 MenuIns = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1,
		   "pmInsert",
		   "mInsertPageBreak", MIns_PAGEBREAK,
                   MENUSEPARATOR, MIns_SEP1,
		   "mInsertEuro", MIns_EUROSIGN,
		   "mInsertChar", MIns_CHAR,
                   MENUSEPARATOR, MIns_SEP2,
		   "mDrawBar", MIns_CREATERECTANGLE,
		   "mDrawPolygon", MIns_CREATEPOLYGON,
		   "mDrawEllipse", MIns_CREATEELIPSE,
		   NULL);

 F(MenuIns).AddCallback(MenuIns, MIns_PAGEBREAK, SelectFunction,
			(void *)INSERT_PAGE_BREAK);
 F(MenuIns).AddCallback(MenuIns, MIns_EUROSIGN, SelectFunction,
			(void *)INSERT_EURO);
 F(MenuIns).AddCallback(MenuIns, MIns_CHAR, SelectFunction,
			(void *)INSERT_SPEC_CHAR);
 F(MenuIns).AddCallback(MenuIns, MIns_CREATERECTANGLE, SelectFunction,
			(void *)CREATE_CADRE_RECTANGLE);
 F(MenuIns).AddCallback(MenuIns, MIns_CREATEPOLYGON, SelectFunction, 
			(void *)CREATE_CADRE_POLYGONAL);
 F(MenuIns).AddCallback(MenuIns, MIns_CREATEELIPSE, SelectFunction, 
			(void *)CREATE_CADRE_ELIPSE);

 XtAddCallback(WIDGET(MenuIns), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuIns), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);

 /* ----------------------------------------------------------------- ** 
 ** Menu FORMAT                                                       ** 
 ** ----------------------------------------------------------------- */


 MenuFor = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1,
		   "pmFormat",
		   "mStyles", MFor_EDITSTYLES,
/*"mRulers", MFor_EDITRULERS,*/
		   MENUSEPARATOR, MFor_SEP1,
		   SUBMENUON, MFor_SUBMENU1,
		   "mFonts", MFor_FONTS,
		   TOGGLEMODEON, Mxxx_NOTHING,
		   "mBold", MFor_BOLD,
		   "mItalic", MFor_ITALIC,
		   "mUnderline", MFor_UNDERLINE,
		   "mStrikeout", MFor_STRIKEOUT,
		   "mShadow", MFor_SHADOW,
		   MENUSEPARATOR, MFor_SEP2,
		   "mSmallCaps", MFor_SMALLCAPS,
		   "mBigCaps", MFor_BIGCAPS,
		   MENUSEPARATOR, MFor_SEP3,
		   "mSubscript", MFor_SUBSCRIPT,
		   "mSupscript", MFor_SUPSCRIPT,
		   TOGGLEMODEOFF, Mxxx_NOTHING,
		   SUBMENUOFF, Mxxx_NOTHING,
		   MENUSEPARATOR,  MFor_SEP4,
		   SUBMENUON, MFor_SUBMENU2,
		   "mFrame", MFor_EDITFRAME,
		   "mPointInsert", MFor_POINTINSERT,
		   "mPointDelete", MFor_POINTDELETE,
		   "mPointMove", MFor_POINTMOVE,
		   MENUSEPARATOR, MFor_SEP5,
		   "mMoveDown", MFor_MOVEDOWN,
		   "mMoveUp", MFor_MOVEUP,
		   "mBackground", MFor_BACKGROUND,
		   "mForeground", MFor_FOREGROUND,
		   MENUSEPARATOR, MFor_SEP6,
		   TOGGLEMODEON, Mxxx_NOTHING,
		   "mLock", MFor_LOCK,
		   TOGGLEMODEOFF, Mxxx_NOTHING,
		   MENUSEPARATOR,  MFor_SEP7,
		   "mAttribs", MFor_ATTRIBS,
		   "mFrameDelete", MFor_FRAMEDELETE,
		   SUBMENUOFF, Mxxx_NOTHING,
		   SUBMENUON, MFor_SUBMENU3,
		   "mTextFlow", MFor_TEXTFLOW,
		   TOGGLEMODEON, Mxxx_NOTHING,
		   "mTFaround", MFor_TEXTFLOWAROUND,
		   "mTFleft", MFor_TEXTFLOWLEFT,
		   "mTFright", MFor_TEXTFLOWRIGHT,
		   "mTFskip", MFor_TEXTFLOWSKIP,
		   "mTFunder", MFor_TEXTFLOWUNDER,
		   "mTFover", MFor_TEXTFLOWOVER,
		   MENUSEPARATOR, MFor_SEP8,
		   "mTFouthole", MFor_TEXTFLOWOUTHOLE,
		   TOGGLEMODEOFF, Mxxx_NOTHING,
		   SUBMENUOFF, Mxxx_NOTHING,
		   NULL);

 F(MenuFor).AddCallback(MenuFor, MFor_EDITSTYLES, MenuEditStyles, NULL);
/*F(MenuFor).ButtonFlag(MenuFor, MFor_EDITRULERS, False);*/

 F(MenuFor).AddCallback(MenuFor, MFor_BOLD, SelectFunction, 
			(void *)SET_BOLD);
 F(MenuFor).AddCallback(MenuFor, MFor_ITALIC, SelectFunction, 
			(void *)SET_ITALIC);
 F(MenuFor).AddCallback(MenuFor, MFor_UNDERLINE, SelectFunction, 
			(void *)SET_UNDERLINE);
 F(MenuFor).AddCallback(MenuFor, MFor_STRIKEOUT, SelectFunction, 
			(void *)SET_STRIKEOUT);
 F(MenuFor).AddCallback(MenuFor, MFor_SHADOW, SelectFunction, 
			(void *)SET_SHADOW);
 F(MenuFor).AddCallback(MenuFor, MFor_SMALLCAPS, SelectFunction, 
			(void *)SET_SMALL_CAPS);
 F(MenuFor).AddCallback(MenuFor, MFor_BIGCAPS, SelectFunction, 
			(void *)SET_BIG_CAPS);
 F(MenuFor).AddCallback(MenuFor, MFor_SUBSCRIPT, SelectFunction, 
			(void *)SET_SUBSCRIPT);
 F(MenuFor).AddCallback(MenuFor, MFor_SUPSCRIPT, SelectFunction, 
			(void *)SET_SUPSCRIPT);

 F(MenuFor).AddCallback(MenuFor, MFor_POINTINSERT, SelectFunction, 
			(void *)INSERT_POINT);
 F(MenuFor).AddCallback(MenuFor, MFor_POINTDELETE, SelectFunction, 
			(void *)DELETE_POINT);
 F(MenuFor).AddCallback(MenuFor, MFor_POINTMOVE, SelectFunction, 
			(void *)MOVE_POINT_MODE);
 F(MenuFor).AddCallback(MenuFor, MFor_MOVEDOWN, SelectFunction, 
			(void *)SET_LOWER_PRIORITY);
 F(MenuFor).AddCallback(MenuFor, MFor_MOVEUP, SelectFunction, 
			(void *)SET_HIGHER_PRIORITY);
 F(MenuFor).AddCallback(MenuFor, MFor_BACKGROUND, SelectFunction,
			(void *)SET_LOWEST_PRIORITY);
 F(MenuFor).AddCallback(MenuFor, MFor_FOREGROUND, SelectFunction, 
			(void *)SET_HIGHEST_PRIORITY);
 F(MenuFor).AddCallback(MenuFor, MFor_LOCK, SelectFunction, 
			(void *)LOCK_CADRE);
 F(MenuFor).AddCallback(MenuFor, MFor_FRAMEDELETE, SelectFunction, 
			(void *)DELETE_SELECTED_CADRE);
 F(MenuFor).AddCallback(MenuFor, MFor_ATTRIBS, MenuFormatFrameAttribs, NULL);
 
 F(MenuFor).AddCallback(MenuFor, MFor_TEXTFLOWAROUND, ToggleTextflowSwitch, 
			(void *)TEXTFLOW_AROUND);
 F(MenuFor).AddCallback(MenuFor, MFor_TEXTFLOWLEFT, ToggleTextflowSwitch, 
			(void *)TEXTFLOW_LEFT);
 F(MenuFor).AddCallback(MenuFor, MFor_TEXTFLOWRIGHT, ToggleTextflowSwitch, 
			(void *)TEXTFLOW_RIGHT);
 F(MenuFor).AddCallback(MenuFor, MFor_TEXTFLOWSKIP, ToggleTextflowSwitch, 
			(void *)TEXTFLOW_SKIP);
 F(MenuFor).AddCallback(MenuFor, MFor_TEXTFLOWOUTHOLE, SelectFunction, 
			(void *)TEXTFLOW_HOLE);
 F(MenuFor).AddCallback(MenuFor, MFor_TEXTFLOWUNDER, ToggleTextflowSwitch, 
			(void *)TEXTFLOW_UNDER);
 F(MenuFor).AddCallback(MenuFor, MFor_TEXTFLOWOVER, ToggleTextflowSwitch, 
			(void *)TEXTFLOW_OVER);
 
 XtAddCallback(WIDGET(MenuFor), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuFor), XmNunmapCallback,
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
 
 XtAddCallback(WIDGET(MenuHel), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuHel), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);

 F(TimeIndicator).advance_indicator(TimeIndicator);

 F(MainInterface1).set_loadingMessage(MainInterface1, "Creating Icon bars");

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

 vIcon[HBAR_FONT] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1,
					  "IconText", display,
					  fonts_xpm, NULL, XcToggle);

 CALLBACK(WIDGET(vIcon[HBAR_FONT]), FUNC_VALUE,
	  SelectHBarCallback, (void *)HBAR_FONT);

 vIcon[HBAR_RULER] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1,
					   "IconRuler", display,
					   ruler_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(vIcon[HBAR_RULER]), FUNC_VALUE,
	  SelectHBarCallback, (void *)HBAR_RULER);
 
 vIcon[HBAR_IMAGE] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1,
					   "IconImage", display,
					   image_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(vIcon[HBAR_IMAGE]), FUNC_VALUE,
	  SelectHBarCallback, (void *)HBAR_IMAGE);
 
 vIcon[HBAR_FRAME] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1,
					   "IconFrame", display,
					   cadre_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(vIcon[HBAR_FRAME]), FUNC_VALUE,
	  SelectHBarCallback, (void *)HBAR_FRAME);
 
 vIcon[HBAR_ZOOM] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1,
					  "IconZoom", display,
					  loupe_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(vIcon[HBAR_ZOOM]), FUNC_VALUE,
	  SelectHBarCallback, (void *)HBAR_ZOOM);
 
 XtSetArg(args[0], XmNset, True);
 XtSetValues(WIDGET(vIcon[bar_selected]), args, 1);

 nargs = 0;
 XtSetArg(args[nargs], XmNshadowType, XmSHADOW_OUT);
 nargs++;
 w_vIconBar_3 = XmCreateFrame(WIDGET(vIconBar), "vIconBar3", args, nargs);
 F(Manager1).Add_child(Manager1, w_vIconBar_3, WIDGET(vIconBar));
 
 vIconBarlogo = (c_Form *)NEW(c_Form)(w_vIconBar_3, Manager1,
				      "vIconBarlogo", 32, 32);

 vIconBar_2 = (c_RowColumn *)NEW(c_RowColumn)(WIDGET(vIconBar), Manager1,
					      "vIconBar2", 32, 0,
					      XmVERTICAL);
 XtVaSetValues(WIDGET(vIconBar_2), XmNbackground, 0, XmNadjustLast, False,
	       XmNmarginWidth, 0, XmNmarginHeight, 0, XmNresizeWidth, False,
	       NULL);

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
	  (void *)DELETE_TEXT);

 IconMis_9 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_9", display,
				   undo_xpm, NULL, XcPush);
/* CALLBACK(WIDGET(IconMis_9), FUNC_ACTIVATE, SelectFunction, 
	  (void *));
*/
 IconMis_10 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_10", display,
				   redo_xpm, NULL, XcPush);
/* CALLBACK(WIDGET(IconMis_10), FUNC_ACTIVATE, SelectFunction, 
	  (void *)); */

 IconMis_s2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				    "IconMis_s2", display,
				    NULL, NULL, XcSeparator);

 IconMis_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_5", display,
				   couper_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconMis_5), FUNC_ACTIVATE, SelectFunction, 
	  (void *)CUT_TEXT);

 IconMis_6 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_6", display,
				   copier_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconMis_6), FUNC_ACTIVATE, SelectFunction, 
	  (void *)COPY_TEXT);

 IconMis_7 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_7", display,
				   coller_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconMis_7), FUNC_ACTIVATE, SelectFunction, 
	  (void *)PASTE_TEXT);


 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Creating Icon bar FONT");

 /* ----------------------------------------------------------------- ** 
 ** Horizontal Icon Bars : HBAR_FONT                                  ** 
 ** ----------------------------------------------------------------- */
 
 IMstyle = NEW(c_ItemMenu)(WIDGET(hIconBar[HBAR_FONT]),
			   Manager2, "imStyle", 0, 0);
  
 XtVaSetValues(WIDGET(IMstyle), XmNmarginHeight, 5, XmNmarginWidth, 3, NULL);
 
 F(IMstyle).set(IMstyle, FALSE, offsetof(c_TextStyle, name), NULL, 
		(boolean (*)())text_style_filter);
 
 tmp_style = (c_TextStyle *)NEW(c_TextStyle)
  (&tmp_style, "             Styles List            ", NULL, 0, NULL);
 F(IMstyle).addItems(IMstyle, XcIM_VA_LIST, 1, tmp_style);

 XtAddCallback(MENUPANE(IMstyle), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(MENUPANE(IMstyle), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);

 IMfontFamily = NEW(c_ItemMenu)(WIDGET(hIconBar[HBAR_FONT]),
				Manager2, "imFontFamily", 0, 0);
  
 XtVaSetValues(WIDGET(IMfontFamily), XmNmarginHeight, 5,
	       XmNmarginWidth, 3, NULL);
 
 F(IMfontFamily).set(IMfontFamily,
		     TRUE, offsetof(family_tree, name),
		     NULL, (boolean (*)())font_family_filter);
 
 F(IMfontFamily).addItems(IMfontFamily, XcIM_VA_LIST, 1, &tmp_family);
 
 XtAddCallback(MENUPANE(IMfontFamily), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(MENUPANE(IMfontFamily), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);
 

 sbFontSize = (c_SpinBox *)NEW(c_SpinBox)(WIDGET(hIconBar[HBAR_FONT]),
					  Manager2, "sbFontSize",
					  XcSB_PLUS_MINUS, TF_INTEGER,
					  1.0, 999.0, "12", 5);

 tfFontSize = WTEXTFIELD(sbFontSize);

 F(sbFontSize).setUnit(sbFontSize, XcUB_LENGTH_POINT, NULL, TRUE, FALSE);
 F(sbFontSize).addCallback(sbFontSize, (void *)certify_font_size,
			   NULL, SB_TEXTFIELD_CALLBACK);
 
 F(sbFontSize).addCallback(sbFontSize, (void *)SelectFunction, 
			   (void *)INC_FONT_SIZE, SB_ARROW_UP_CALLBACK);
 
 F(sbFontSize).addCallback(sbFontSize, (void *)SelectFunction, 
			   (void *)DEC_FONT_SIZE, SB_ARROW_DOWN_CALLBACK); 
 

 /* XtAddEventHandler(WIDGET(tfFontSize), ButtonPressMask, False,
    (void *)cb_focus_tfFontSize, (void *)NULL); */ 
 XtAddCallback(WIDGET(tfFontSize), XmNfocusCallback,
	       (void *)cb_focus_tfFontSize, (void *)NULL);
 
 IconFon_s1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				    "IconFon_s1", display,
				    NULL, NULL, XcSeparator);

 IconFon_3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				   "IconFon_3", display,
				   texte_style_gras_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFon_3), FUNC_VALUE, SelectFunction, (void *)SET_BOLD);
 IconFon_4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				   "IconFon_4", display,
				   texte_style_italique_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFon_4), FUNC_VALUE, SelectFunction, (void *)SET_ITALIC);
 IconFon_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				   "IconFon_5", display,
				   texte_style_underline_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFon_5), FUNC_VALUE, SelectFunction,
	  (void *)SET_UNDERLINE);

 IconFon_9 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				   "IconFon_9", display,
				   texte_style_shadow_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFon_9), FUNC_VALUE, SelectFunction,
	  (void *)SET_SHADOW);

 IconFon_s2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				    "IconFon_s2", display,
				    NULL, NULL, XcSeparator);

 IconFon_7 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				   "IconFon_7", display,
				   texte_style_subscript_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFon_7), FUNC_VALUE, SelectFunction,
	  (void *)SET_SUBSCRIPT);
 IconFon_8 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				   "IconFon_8", display,
				   texte_style_supscript_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFon_8), FUNC_VALUE, SelectFunction,
	  (void *)SET_SUPSCRIPT);

 IconFon_s3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				    "IconFon_s3", display,
				    NULL, NULL, XcSeparator);

 IconFon_11 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				    "IconFon_11", display,
				    texte_style_smallcaps_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFon_11), FUNC_VALUE, SelectFunction,
	  (void *)SET_SMALL_CAPS);

 IconFon_s4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				    "IconFon_s4", display,
				    NULL, NULL, XcSeparator);

 CLfontColor = NEW(c_ColorList)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				"clFontColor", XcCL_FONT);
 CLfontBgColor = NEW(c_ColorList)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				  "clFontBgColor", XcCL_FONTBG);
 
 F(TimeIndicator).advance_indicator(TimeIndicator);

 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Creating Icon bar RULES");

 /* ----------------------------------------------------------------- ** 
 ** Horizontal Icon Bars : HBAR_RULER                                 ** 
 ** ----------------------------------------------------------------- */
  
 IconRul_1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				   "IconRul_1", display,
				   style_gauche_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconRul_1), FUNC_VALUE, ToggleSwitchRuler, (void *)SET_JLEFT);

 IconRul_2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				   "IconRul_2", display,
				   style_centre_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconRul_2), FUNC_VALUE, ToggleSwitchRuler, 
	  (void *)SET_JCENTER);

 IconRul_3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				   "IconRul_3", display,
				   style_droite_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconRul_3), FUNC_VALUE, ToggleSwitchRuler, 
	  (void *)SET_JRIGHT);

 IconRul_4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				   "IconRul_4", display,
				   style_justifie_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconRul_4), FUNC_VALUE, ToggleSwitchRuler, 
	  (void *)SET_JUSTIFY);

 IconRul_s1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				    "IconRul_s1", display,
				    NULL, NULL, XcSeparator);

 IconRul_6 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				   "IconRul_6", display,
				   style_indent_dec_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconRul_6), FUNC_ACTIVATE, SelectFunction, 
	  (void *)DEC_INDENT);

 IconRul_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				   "IconRul_5", display,
				   style_indent_inc_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconRul_5), FUNC_ACTIVATE, SelectFunction, 
	  (void *)INC_INDENT);

 IconRul_s2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				    "IconRul_s2", display,
				    NULL, NULL, XcSeparator);

 IconRul_8 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				   "IconRul_8", display,
				   style_interline_dec_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconRul_8), FUNC_ACTIVATE, SelectFunction, 
	  (void *)DEC_INTERLINE);

 IconRul_7 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				   "IconRul_7", display,
				   style_interline_inc_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconRul_7), FUNC_ACTIVATE, SelectFunction, 
	  (void *)INC_INTERLINE);

 IconRul_s3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				    "IconRul_s3", display,
				    NULL, NULL, XcSeparator);

 IconRul_10 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				    "IconRul_10", display,
				    style_interparagraph_dec_xpm, NULL,XcPush);
 CALLBACK(WIDGET(IconRul_10), FUNC_ACTIVATE, SelectFunction, 
	  (void *)DEC_INTERPARAG);
 
 IconRul_11 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				    "IconRul_11", display,
				    style_interparagraph_inc_xpm, NULL,XcPush);
 CALLBACK(WIDGET(IconRul_11), FUNC_ACTIVATE, SelectFunction, 
	  (void *)INC_INTERPARAG);
 
 IconRul_s4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				    "IconRul_s4", display,
				    NULL, NULL, XcSeparator);
 
 IconRul_9 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				   "IconRul_9", display,
				   special_char_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconRul_9), FUNC_VALUE, SelectFunction, 
	  (void *)DISPLAY_SPEC_CHARS);

 IconRul_12 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_RULER]), Manager2,
				    "IconRul_12", display,
				    euro_xpm, NULL,XcPush);
 CALLBACK(WIDGET(IconRul_12), FUNC_ACTIVATE, SelectFunction, 
	  (void *)INSERT_EURO);
 
 F(TimeIndicator).advance_indicator(TimeIndicator);

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
				      "Creating Icon bar FRAME");

 /* ----------------------------------------------------------------- **
 ** Horizontal Icon Bars : HBAR_FRAME                                 ** 
 ** ----------------------------------------------------------------- */

 IconFra_1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				   "IconFra_1", display,
				   cadre_selectionner_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_1), FUNC_VALUE,
	  ToggleSwitch, (void *)RESIZE_CADRE_MODE);

 IconFra_2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				   "IconFra_2", display,
				   cadre_creer_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_2), FUNC_VALUE, 
	  ToggleSwitch, (void *)CREATE_CADRE_RECTANGLE);

 IconFra_3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				   "IconFra_3", display,
				   cadre_creer_lignes_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_3), FUNC_VALUE,
	  ToggleSwitch, (void *)CREATE_CADRE_POLYGONAL);

 IconFra_4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				   "IconFra_4", display,
				   cadre_creer_ovale_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_4), FUNC_VALUE,
	  ToggleSwitch, (void *)CREATE_CADRE_ELIPSE);

 IconFra_s1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				    "IconFra_s1", display,
				    NULL, NULL, XcSeparator);

 IconFra_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				   "IconFra_5", display,
				   cadre_bouger_point_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_5), FUNC_VALUE, 
	  ToggleSwitch, (void *)MOVE_POINT_MODE);

 IconFra_6 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				   "IconFra_6", display,
				   cadre_inserer_point_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_6), FUNC_VALUE, ToggleSwitch, (void *)INSERT_POINT);

 IconFra_7 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				   "IconFra_7", display,
				   cadre_detruire_point_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_7), FUNC_VALUE, ToggleSwitch, (void *)DELETE_POINT);

 IconFra_s2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				    "IconFra_s2", display,
				    NULL, NULL, XcSeparator);

 IconFra_8 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				   "IconFra_8", display,
				   cadre_premierplan_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconFra_8), FUNC_ACTIVATE, 
	  SelectFunction, (void *)SET_HIGHEST_PRIORITY);

 IconFra_9 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				   "IconFra_9", display,
				   cadre_arriereplan_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconFra_9), FUNC_ACTIVATE,
	  SelectFunction, (void *)SET_LOWEST_PRIORITY);

 IconFra_10 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_10", display,
				    cadre_approcher_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconFra_10), FUNC_ACTIVATE,
	  SelectFunction, (void *)SET_HIGHER_PRIORITY);

 IconFra_11 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_11", display,
				    cadre_eloigner_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconFra_11), FUNC_ACTIVATE,
	  SelectFunction, (void *)SET_LOWER_PRIORITY);

 IconFra_s3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				    "IconFra_s3", display,
				    NULL, NULL, XcSeparator);

 IconFra_14 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_14", display,
				    textflow_around_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_14), FUNC_VALUE,
	  ToggleTextflowSwitch, (void *)TEXTFLOW_AROUND);

 IconFra_15 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_15", display,
				    textflow_left_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_15), FUNC_VALUE,
	  ToggleTextflowSwitch, (void *)TEXTFLOW_LEFT);

 IconFra_16 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_16", display,
				    textflow_right_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_16), FUNC_VALUE,
	  ToggleTextflowSwitch, (void *)TEXTFLOW_RIGHT);

 IconFra_17 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_17", display,
				    textflow_skip_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_17), FUNC_VALUE,
	  ToggleTextflowSwitch, (void *)TEXTFLOW_SKIP);

 IconFra_19 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_19", display,
				    textflow_outhole_xpm, textflow_inhole_xpm,
				    XcToggle);
 CALLBACK(WIDGET(IconFra_19), FUNC_VALUE,
	  SelectFunction, (void *)TEXTFLOW_HOLE);

 IconFra_18 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_18", display,
				    textflow_under_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_18), FUNC_VALUE,
	  ToggleTextflowSwitch, (void *)TEXTFLOW_UNDER);
 IconFra_20 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_20", display,
				    textflow_over_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_20), FUNC_VALUE,
	  ToggleTextflowSwitch, (void *)TEXTFLOW_OVER);

 IconFra_s4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2, 
				    "IconFra_s4", display,
				    NULL, NULL, XcSeparator);

 IconFra_12 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FRAME]), Manager2,
				    "IconFra_12", display,
				    cadre_lock_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFra_12), FUNC_VALUE,
	  SelectFunction, (void *)LOCK_CADRE);
 
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
 
 F(TimeIndicator).advance_indicator(TimeIndicator);

 F(MainInterface1).set_loadingMessage(MainInterface1, "Initialize Icon bars");
 /* ----------------------------------------------------------------- ** 
 ** Initialize Icon Bars                                              ** 
 ** ----------------------------------------------------------------- */
 i = 0;
 toggletable[i].tog_wid = WIDGET(IconFra_1);
 toggletable[i].func_num = RESIZE_CADRE_MODE;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_1);
 toggletable[i].func_num = SELECT_MOVE_EDIT_TEXT;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_2);
 toggletable[i].func_num = CREATE_CADRE_RECTANGLE;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_3);
 toggletable[i].func_num = CREATE_CADRE_POLYGONAL;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_4);
 toggletable[i].func_num = CREATE_CADRE_ELIPSE;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_5);
 toggletable[i].func_num = MOVE_POINT_MODE;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_6);
 toggletable[i].func_num = INSERT_POINT;
 i++;
 toggletable[i].tog_wid = WIDGET(IconFra_7);
 toggletable[i].func_num = DELETE_POINT;
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

 i = 0;
 toggletextflowmodetable[i].tog_wid = WIDGET(IconFra_14);
 toggletextflowmodetable[i].tog_wid2 = 
  MenuFor->w_Buttons[MFor_TEXTFLOWAROUND];
 toggletextflowmodetable[i].func_num = TEXTFLOW_AROUND;
 toggletextflowmodetable[i].mode_num = XcTFM_AROUND;
 i++;
 toggletextflowmodetable[i].tog_wid = WIDGET(IconFra_15);
 toggletextflowmodetable[i].tog_wid2 =
  MenuFor->w_Buttons[MFor_TEXTFLOWLEFT];
 toggletextflowmodetable[i].func_num = TEXTFLOW_LEFT;
 toggletextflowmodetable[i].mode_num = XcTFM_LEFT;
 i++;
 toggletextflowmodetable[i].tog_wid = WIDGET(IconFra_16);
 toggletextflowmodetable[i].tog_wid2 =
  MenuFor->w_Buttons[MFor_TEXTFLOWRIGHT];
 toggletextflowmodetable[i].func_num = TEXTFLOW_RIGHT;
 toggletextflowmodetable[i].mode_num = XcTFM_RIGHT;
 i++;
 toggletextflowmodetable[i].tog_wid = WIDGET(IconFra_17);
 toggletextflowmodetable[i].tog_wid2 =
  MenuFor->w_Buttons[MFor_TEXTFLOWSKIP];
 toggletextflowmodetable[i].func_num = TEXTFLOW_SKIP;
 toggletextflowmodetable[i].mode_num = XcTFM_SKIP;
 i++;
 toggletextflowmodetable[i].tog_wid = WIDGET(IconFra_18);
 toggletextflowmodetable[i].tog_wid2 =
  MenuFor->w_Buttons[MFor_TEXTFLOWUNDER];
 toggletextflowmodetable[i].func_num = TEXTFLOW_UNDER;
 toggletextflowmodetable[i].mode_num = XcTFM_UNDER;
 i++;
 toggletextflowmodetable[i].tog_wid = WIDGET(IconFra_20);
 toggletextflowmodetable[i].tog_wid2 =
  MenuFor->w_Buttons[MFor_TEXTFLOWOVER];
 toggletextflowmodetable[i].func_num = TEXTFLOW_OVER;
 toggletextflowmodetable[i].mode_num = XcTFM_OVER;
 i++;
  
 store_function = -1;
 store_function_ruler = -1;
 store_widget_imagevector = (Widget)0;
 store_mode_textflow = -1;

 F(Manager1).Manage_all(Manager2, NULL); 

 for (i = 0; i < NB_HBAR; i++)
 {
  AttachHBar(hIconBarForm, i);
  XtUnmanageChild(WIDGET(hIconBar[i]));
 }
 
 XtManageChild(WIDGET(hIconBar[bar_selected]));

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

 /* ----------------------------------------------------------------- ** 
 ** MainInterface Form Attachment                                     ** 
 ** ----------------------------------------------------------------- */

 F(MainInterface1).attach_left(MainInterface1, WIDGET(Bar1), NULL);
 F(MainInterface1).attach_right(MainInterface1, WIDGET(Bar1), NULL);
 F(MainInterface1).attach_top(MainInterface1, WIDGET(Bar1), NULL);
 
 F(MainInterface1).attach_left(MainInterface1, WIDGET(hIconBarForm), NULL);
 F(MainInterface1).attach_right(MainInterface1, WIDGET(hIconBarForm), NULL);
 F(MainInterface1).attach_top(MainInterface1, WIDGET(hIconBarForm),
			      WIDGET(Bar1));
 
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
 
 
 F(TimeIndicator).advance_indicator(TimeIndicator);
 
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
 SET_HELPLINE(GlobHelpLine, WIDGET(IconMis_9),helpMiscUndo, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconMis_10),helpMiscRedo, XcR_NONE);

 /*--- RULERS ---*/
 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_RULER]),helpRuler, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconRul_1),helpRulerLeft, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconRul_2),helpRulerCenter, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconRul_3),helpRulerRight, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconRul_4),helpRulerJustify, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconRul_6),helpRulerIndentDec, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconRul_5),helpRulerIndentInc, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconRul_7),helpRulerInterlineInc, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconRul_8),helpRulerInterlineDec, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconRul_10),helpRulerInterparDec, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconRul_11),helpRulerInterparInc, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconRul_9),helpRulerSpecialChar, XcR_NONE);

 /*--- FONTS ---*/
 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_FONT]),helpFont, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFon_3),helpFontBold, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFon_4),helpFontItalic, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFon_5),helpFontUnderline, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFon_9),helpFontShadow, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFon_7),helpFontSubscript, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFon_8),helpFontSupscript, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFon_11),helpFontSmallcaps, XcR_NONE);

 /*--- IMAGES ---*/
 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_IMAGE]),helpPicture, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_1),helpPictureMove, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_2),helpPictureCenter, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_3),helpPictureReset, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_4),helpPictureVFlip, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_5),helpPictureHFlip, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_6),helpPictureNormal, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_7),helpPictureBest, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_8),helpPictureMax, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconIma_9),helpPictureStretch, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconVec_1),helpVectorTexts, XcR_NONE);

 /*--- FRAMES ---*/
 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_FRAME]),helpFrame, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_1),helpFrameSelect, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_2),helpFrameSquare, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_3),helpFramePolygon, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_4),helpFrameCircle, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_5),helpFrameMovePoint, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_6),helpFrameAddPoint, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_7),helpFrameSubPoint, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_8),helpFramePlanFirst, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_9),helpFramePlanLast, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_10),helpFramePlanUp, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_11),helpFramePlanDown, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_14),helpFrameTFaround, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_15),helpFrameTFleft, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_16),helpFrameTFright, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_17),helpFrameTFskip, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_19),helpFrameTFhole, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_18),helpFrameTFunder, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_20),helpFrameTFover, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFra_12),helpFrameLock, XcR_NONE);
  
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

 /* ----------------------------------------------------------------- ** 
 ** Interface construction complete                                   ** 
 ** ----------------------------------------------------------------- */

 if (TimeIndicator) F(TimeIndicator).advance_indicator(TimeIndicator);

 F(MainInterface1).set_loadingMessage(MainInterface1, "Managing Interface");

 F(Manager1).Manage_all(Manager1, NULL);

 F(MainInterface1).map_TopLevel(MainInterface1);
 if (TimeIndicator) F(TimeIndicator).advance_indicator(TimeIndicator);

 {
  XSetWindowAttributes	set_attrib;
  XWindowAttributes	get_attrib;
  
  XGetWindowAttributes(display, XtWindow(WIDGET(MainDocument)), &get_attrib);
  set_attrib.event_mask = get_attrib.all_event_masks | StructureNotifyMask;
  XChangeWindowAttributes(display, XtWindow(WIDGET(MainDocument)),
			  CWEventMask, &set_attrib);
 }
  
 F(MainInterface1).set_loadingMessage(MainInterface1, "Initialize core");

 if (TimeIndicator) F(MainInterface1).set_main_icon(MainInterface1);
 
 GlobKeyboard = (c_Keyboard *)
  NEW(c_Keyboard)(display, XtWindow(WIDGET(MainInterface1)));

 Init_Pager(display);
 Init_XaRegle(display);

 DocList = (c_DocumentList *)NEW(c_DocumentList)
  (MenuWin, MWin_DYNAMICDOCLIST, defaultDocumentName, ChangeDocCallback);
 
 Gwm = (c_WinManager *)NEW(c_WinManager)(display, 
					 XtWindow(WIDGET(MainDocument)));
 Gwm->SelectCallBack = SChangeWindow;
 
 F(GlobCursor).enable_wait_mode(GlobCursor, XtWindow(WIDGET(MainInterface1)));
 F(GlobColormap).generate_colormap(GlobColormap);

 IconVlogo = NULL;

#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
 XcDO_IF_KEY_OK(IconVlogo = (c_Icon *)NEW(c_Icon)
		(WIDGET(vIconBarlogo), NULL, "IconVlogo", display,
		 (gkinfo.valid == Xc_KEY_GOOD ? 
		  small_column_xpm : small_columnu_xpm), NULL, XcLabel))
 else
  IconVlogo = (c_Icon *)NEW(c_Icon)
   (WIDGET(vIconBarlogo), NULL, "IconVlogo", display,
    small_columnu_xpm, NULL, XcLabel);
#else
 IconVlogo = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBarlogo), NULL, 
				   "IconVlogo", display,  
				   small_column_xpm, NULL, XcLabel); 
#endif

 F(vIconBarlogo).attach_bottom(WIDGET(vIconBarlogo), WIDGET(IconVlogo), NULL);

 XtManageChild(WIDGET(IconVlogo));

 AttachHBar(hIconBarForm, bar_selected);

 nargs = 0;
 XtSetArg(args[nargs], XmNbackgroundPixmap, 
	  F(GlobDocIcon).get_pixmap(GlobDocIcon, Icon_BACKGROUND));
 nargs++;
 XtSetValues(WIDGET(MainDocument), args, nargs);

 GlobImgCache = (c_ImageCache *)NEW(c_ImageCache)(GlobColormap);
 GlobClipBuffer = (c_ClipBuffer *)NEW(c_ClipBuffer)(XcCB_INFINITE_LEVEL);
 GlobDragAndDrop = (c_DragAndDrop *)NEW(c_DragAndDrop)(display);
 GlobTrash = (c_Trash *)NEW(c_Trash)(display, XtWindow(WIDGET(IconTra)));
 GlobImport = (c_Import *)NEW(c_Import)();

 /*--- Read Config files ---*/
 
 if (TimeIndicator) F(TimeIndicator).advance_indicator(TimeIndicator);
 
 F(MainInterface1).set_loadingMessage(MainInterface1, "Loading user settings");

 if(!ReadConfigFile())
 {
  if(!ReadUserConfigFile()) 
   QuitApplication();
 }
 else
  ReadUserConfigFile();

 /* to dispatch */
 if (TimeIndicator) F(TimeIndicator).advance_indicator(TimeIndicator);
 if (TimeIndicator) F(TimeIndicator).advance_indicator(TimeIndicator);

 /*--- init undo ---*/
 F(GlobUndo).addWidgets(GlobUndo, MenuEdi->w_Buttons[MEdi_UNDO],
			MenuEdi->w_Buttons[MEdi_REDO], XcU_MENU_ITEM);
 F(GlobUndo).addWidgets(GlobUndo, WIDGET(IconMis_9),  WIDGET(IconMis_10),
			XcU_COUPLE_ICON);
 F(GlobUndo).addCallback(GlobUndo, UndoCallback, NULL);

 /*--- Item Menu for TextStyle ---*/
 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Initialize Interface items");

 F(IMstyle).addItems(IMstyle, XcIM_LINKED_LIST, -1,
		     GlobTextStyleBase, offsetof(c_TextStyle, next));
 F(IMstyle).deleteItems(IMstyle, XcIM_VA_LIST, 1, tmp_style);
 DELETE(c_TextStyle)(tmp_style);
 F(IMstyle).setSelection(IMstyle, GlobTextStyleBase);
 F(IMstyle).calculSize(IMstyle, 250);

 saved_style = GlobTextStyleBase;
 F(IMstyle).setCallback(IMstyle, (void *)change_style, NULL);
 
 /*--- Item Menu for Font Family ---*/
 F(IMfontFamily).addItems(IMfontFamily, XcIM_LINKED_LIST, -1,
			  GlobFontBase->family, offsetof(family_tree, Next));
 F(IMfontFamily).deleteItems(IMfontFamily, XcIM_VA_LIST, 1, &tmp_family);
 F(IMfontFamily).setSelection(IMfontFamily, GlobFontBase->family);
 F(IMfontFamily).calculSize(IMfontFamily, 250);
 
 saved_family = GlobFontBase->family;
 F(IMfontFamily).setCallback(IMfontFamily, (void *)change_font, NULL);

 if (TimeIndicator) F(TimeIndicator).advance_indicator(TimeIndicator);

 /*--- Font Color and Font Background Color ---*/
 F(CLfontColor).setCallback(CLfontColor, (void *)change_font_color, NULL);
 F(CLfontColor).setColorBase(CLfontColor, GlobColorBase, 
			     CL_SKIP_TRANSPARENCY); 
 F(CLfontColor).setColor(CLfontColor, NULL);

 F(CLfontBgColor).setCallback(CLfontBgColor, (void *)change_font_bgcolor, 
			      NULL);
 F(CLfontBgColor).setColorBase(CLfontBgColor, GlobColorBase, 
			       CL_KEEP_TRANSPARENCY); 
 F(CLfontBgColor).setColor(CLfontBgColor, NULL);

 /*--- test if an euro font is not present ---*/
 if (F(GlobFontBase).getFontByName(GlobFontBase, "Euro") == NULL)
 {
  XtUnmanageChild(WIDGET(IconRul_12));
  XtUnmanageChild(MenuIns->w_Buttons[MIns_EUROSIGN]);
 }

 /*--- set traversal to MainDocument drawing area ---*/
 if (!XmProcessTraversal(WIDGET(MainDocument), XmTRAVERSE_CURRENT))
  Xc_TRACE(("XmProcessTraversal failed"));

 XtVaSetValues(WIDGET(tfFontSize), XmNtraversalOn, True, NULL);
 /* ------------------------------------------------------------------- */
 
 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Initialize Image cache");
 if (TimeIndicator) F(TimeIndicator).advance_indicator(TimeIndicator);

 GlobTextClipboard = (c_Text *)NEW(c_Text)(NULL, Xa_CLIPBOARD);

 F(GlobImgCache).init(GlobImgCache);
 GlobPrintParam.printer = GlobPrinterBase;

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

 /* ----------------------------------------------------------------- ** 
 ** Main Loop                                                         ** 
 ** ----------------------------------------------------------------- */
 DocAlreadyGrised = FALSE;
 FrameToolsAlreadyGrised = FALSE;
 MultiFrameToolsAlreadyGrised = FALSE;
 LockToolsAlreadyGrised = FALSE;
 STextAlreadyGrised = FALSE;
 SImageAlreadyGrised = FALSE;
 SVectorAlreadyGrised = FALSE;

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

/* ------------------------------------------------------------------------ **
**                      MenuItem & Icon Callbacks                           ** 
** ------------------------------------------------------------------------ */

/* ----------------------------------------------------------------- ** 
** MenuFileNew                                                       ** 
** ----------------------------------------------------------------- */
static void MenuFileNew()
{
 Display	*display;
 c_Document	*DocTmp;
 char		*doc_name;
 
 /*--- Get page format ---*/
 display = XtDisplay(WIDGET(MainDocument));
 doc_name = F(DocList).Get_NextName(DocList);
 
 /**************************/
 /* Creation d'un document */
 /**************************/
 DocTmp = (c_Document *)NEW(c_Document)();
 
 /*****************/
 /* Init Document */
 /*****************/
 Xc_TRACE(("Init document"));
 F(DocTmp).Init_Document(DocTmp,
			 XtDisplay(WIDGET(MainDocument)),
			 XtWindow(WIDGET(MainDocument)),
			 TRUE, doc_name);
 
 /*******************/
 /* Manage document */
 /*******************/
 F(Gwm).SetWindowName(Gwm, display, DocTmp->window, doc_name);
 F(DocList).Add(DocList, DocTmp, DOCUMENT_WORDP, doc_name);
 Xc_free(doc_name);
 Xc_TRACE(("manage"));
 F(Gwm).ManageWin(Gwm, display, DocTmp->window,
		  F(GlobDocIcon).get_pixmap(GlobDocIcon, Icon_WORDP));
 FLUSH_UNDO;
}

/* ----------------------------------------------------------------- ** 
** MenuFileOpen                                                      ** 
** ----------------------------------------------------------------- */
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
   strcat(filename, ".xa");
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
    F(DocList).Add(DocList, DocTmp, DOCUMENT_WORDP, DocTmp->name);
    
    Xc_TRACE(("Set Window Name"));
    F(Gwm).SetWindowName(Gwm, DocTmp->display,
			 DocTmp->window, DocTmp->name);
    
    F(Gwm).ManageWin(Gwm, DocTmp->display, DocTmp->window,
		     F(GlobDocIcon).get_pixmap(GlobDocIcon,
					       Icon_WORDP));
    FLUSH_UNDO;
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

/* ----------------------------------------------------------------- ** 
** MenuFileClose                                                     ** 
** ----------------------------------------------------------------- */
static void MenuFileClose()
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
  if (Type == DOCUMENT_WORDP && Document->modified)
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
  FLUSH_UNDO;
 }
}

/* ----------------------------------------------------------------- ** 
** MenuFileSaveas                                                    ** 
** ----------------------------------------------------------------- */
static void MenuFileSaveas()
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
  FLUSH_UNDO;
  F(DocList).ChangeDocName(DocList, (void *)Document, Document->name);
  
  if(WRITE_DOCUMENT(Document)(Document, doc) == FALSE)
   Xc_ERROR(("Save error"));
  
  DELETE(c_DocFile)(doc);
 }
}

/* ----------------------------------------------------------------- ** 
** MenuFileSave                                                      ** 
** ----------------------------------------------------------------- */
static void MenuFileSave()
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

/* ----------------------------------------------------------------- ** 
** MenuFileSaveall                                                   ** 
** ----------------------------------------------------------------- */
static void MenuFileSaveall()
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


/* ----------------------------------------------------------------- ** 
** LoadImage                                                         ** 
** ----------------------------------------------------------------- */
void LoadImage(w)
Widget	w;
{
 char		*filename;
 c_Page		*SelectedPage;
 c_Image	*image;
 c_Image	*image2;
 l_Cadre	*lcadre;
 l_Cadre	*svglcadre;
 c_Document	*doc;
 void		*DocTmp;
 int		Type;

 DocTmp = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (DocTmp && Type == DOCUMENT_WORDP)
  SelectedPage =  ((c_Document *)DocTmp)->PageArea->SelectedPage;
 else
  return;
     
 if(!SelectedPage->frmg->SelectedCadre)
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
 doc = ((c_Document *)DocTmp);
 if((image = (c_Image *)NEW(c_Image)(filename, NULL)) != NULL)
 {
  lcadre = svglcadre = (l_Cadre *)sort_asc_list_cadre
   (SelectedPage->frmg->SelectedCadre);
  F(lcadre->cadre).attachobject(lcadre->cadre, image, CADRE_IMAGE_BITMAP);
  F(SelectedPage->frmg).draw_cadre(SelectedPage->frmg, lcadre); 
  lcadre = lcadre->NextCadre;
  while(lcadre)
  {
   image2 = (c_Image *)COPY(c_Image)(image);
   if (image2)
   {
    F(lcadre->cadre).attachobject(lcadre->cadre, image2,
				  CADRE_IMAGE_BITMAP);
    F(SelectedPage->frmg).draw_cadre(SelectedPage->frmg, lcadre); 
   }
   else
    break;
   lcadre = lcadre->NextCadre;
  }
  free_list_cadre(svglcadre, NO_DEL_CADRE);
  doc->modified = TRUE;
 }
 else
 {
  Xc_ErrorAlert(F(GlobResources).getString(GlobResources, 
					   XcR_notBitmapError));
 }
 UNSET_WAIT_MODE;
 Xc_free(filename);
 SET_FUNCTION_DOCUMENT(DocTmp)(DocTmp, doc->PageArea->function_type);

 return;
}

/* ----------------------------------------------------------------- ** 
** LoadText                                                          ** 
** ----------------------------------------------------------------- */
static void LoadText(w)
Widget	w;
{
 void		 *doc;
 int		Type;	
 c_Document	*document;
  
 Xc_TRACE(("Import Text"));
 SET_WAIT_MODE_STATIC;
 if (Gwm->SelectedWindow == NULL)
 {
  F(GlobImport).import(GlobImport, NULL);
 }
 else
 {
  doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
      
  if (Type == DOCUMENT_WORDP && doc != NULL)
  {
   document = (c_Document *)doc;
   F(GlobImport).import(GlobImport, document);
  }
  else
  {
   F(GlobImport).import(GlobImport, NULL);
  }
 }
 UNSET_WAIT_MODE;
}

/* ----------------------------------------------------------------- ** 
** LoadVector                                                        ** 
** ----------------------------------------------------------------- */
void LoadVector(w)
Widget w;
{
 char		*filename;
 c_Page		*SelectedPage;
 l_Cadre	*frame_list;
 c_VectorGraph	*graph, *graph2;
 c_Document	*doc;
 void	 	*DocTmp;
 int		Type;
  
 DocTmp = NULL;
 DocTmp = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (DocTmp && Type == DOCUMENT_WORDP)
  SelectedPage =  ((c_Document *)DocTmp)->PageArea->SelectedPage;
 else
  return;
  
 if(!SelectedPage->frmg->SelectedCadre)
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
 if ((graph = NEW(c_VectorGraph)(&doc->BaseStd)) == NULL)
  return;
  
 /*--- Load ---*/
 SET_WAIT_MODE_ANIM;
 if(F(graph).load(graph, filename))
 {
  frame_list = sort_asc_list_cadre(SelectedPage->frmg->SelectedCadre);
  F(frame_list->cadre).attachobject(frame_list->cadre,
				    graph, CADRE_VECTOR);
  F(SelectedPage->frmg).draw_cadre(SelectedPage->frmg, frame_list); 
  frame_list = frame_list->NextCadre;
  while(frame_list)
  {
   graph2 = (c_VectorGraph *)COPY(c_VectorGraph)(graph, &doc->BaseStd);
   if (graph2)
   {
    F(frame_list->cadre).attachobject(frame_list->cadre,
				      graph2, CADRE_VECTOR);
    F(SelectedPage->frmg).draw_cadre(SelectedPage->frmg, frame_list); 
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
 SET_FUNCTION_DOCUMENT(DocTmp)(DocTmp, doc->PageArea->function_type);
 return;
}

/* ----------------------------------------------------------------- ** 
** MenuFilePrint                                                     ** 
** ----------------------------------------------------------------- */
static void MenuFilePrint(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 void		*DocSelected;
 c_PostScript	*print;
 c_Box_imprimer	*box;
 int		Type;	
 printer_t	*printer;
 FILE		*handle;

 DocSelected = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 
 if (Type == DOCUMENT_WORDP)
 {
  int rc;
  
  GlobPrintParam.NbPage = ((c_Document *)DocSelected)->PageArea->NbrPage;
  
  if (GlobPrintParam.EndPage > GlobPrintParam.NbPage)
   GlobPrintParam.EndPage = GlobPrintParam.NbPage;

  if (GlobPrintParam.StartPage > GlobPrintParam.NbPage || 
      GlobPrintParam.StartPage > GlobPrintParam.EndPage)
   GlobPrintParam.StartPage = 1;
  
  GlobPrintParam.AllPages = (GlobPrintParam.StartPage == 1) &&
   (GlobPrintParam.EndPage == GlobPrintParam.NbPage);
  
  SET_WAIT_MODE_STATIC;
  box = (c_Box_imprimer *)NEW(c_Box_imprimer)(MainInterface1->w_Main, 
					      "BoxPrint",
					      &GlobPrintParam);
  rc = F(box->Dialog).waitForUnmap(box->Dialog);
  DELETE(c_Box_imprimer)(box);
  if(rc != XcCD_OK) { UNSET_WAIT_MODE; return;}
  
  printer = GlobPrintParam.printer;
  
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

/* ----------------------------------------------------------------- ** 
** MenuFileQuit                                                      ** 
** ----------------------------------------------------------------- */
static void MenuFileQuit()
{
 char		*message;
 
 MenuFileSaveall();
 message = F(GlobResources).getString(GlobResources, XcR_quitMessage);
 if (Xc_YesNoAlert(message))
  QuitApplication();
 else
  return;
}

/* ----------------------------------------------------------------- ** 
** MenuEditColors                                                    ** 
** ----------------------------------------------------------------- */
static void MenuEditColors(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_color	*box;
 c_Document	*doc;
 int		Type, rc;
 
 doc = (c_Document *)F(DocList).Find(DocList, 
				     Gwm->SelectedWindow->MainWin, &Type);
 if (Type == DOCUMENT_WORDP)
 {
  SET_WAIT_MODE_STATIC;
  box = (c_Box_color *)NEW(c_Box_color)
   (MainInterface1->w_Main, "BoxColors",
    &(doc->BaseStd.color_base));
  rc = F(box->PBox->Dialog).waitForUnmap(box->PBox->Dialog);
  DELETE(c_Box_color)(box);
  
  if (rc == XcCD_OK)
  {
   F(doc->PageArea).RecalibrateAll(doc->PageArea);
   doc->modified = TRUE;
   
   F(CLfontColor).setColorBase(CLfontColor, 
			       doc->BaseStd.color_base,
			       CL_SKIP_TRANSPARENCY);
   F(CLfontBgColor).setColorBase(CLfontBgColor, 
				 doc->BaseStd.color_base,
				 CL_KEEP_TRANSPARENCY);
  }

  UNSET_WAIT_MODE;
 }
}

/*---------------------------------------------------------------------*/
static void MenuEditCache(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_cache *box;
  
 box = (c_Box_cache *)NEW(c_Box_cache)(MainInterface1->w_Main,
				       "BoxCache");
 F(box->Dialog).waitForUnmap(box->Dialog);
 DELETE(c_Box_cache)(box);
}

/* ----------------------------------------------------------------- ** 
** MenuEditStyles                                                    ** 
** ----------------------------------------------------------------- */
static void MenuEditStyles(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 void		*doc;
 c_Document	*document;
 c_Box_styles  *box;
 c_TextStyle	*style;
 int		Type, rc;	
  
 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (Type == DOCUMENT_WORDP)
 {
  document = (c_Document *)doc;
  style = document->BaseStd.text_base->style;
  
  SET_WAIT_MODE_STATIC;
  box = (c_Box_styles *)NEW(c_Box_styles)(MainInterface1->w_Main, 
					  "BoxFonts",
					  &(document->BaseStd),
					  &style);
  rc = F(box->PBox->Dialog).waitForUnmap(box->PBox->Dialog);
  DELETE(c_Box_styles)(box);

  switch(rc)
  {
  case XcCD_DESTROY:
  case XcCD_CANCEL:
   break;
  case XcCD_OK:
  case XcCD_APPLY:
   document->modified = TRUE;
   
   if (rc == XcCD_OK)
    F(document->PageArea).RecalibrateAll(document->PageArea);
   else	  /* XcCD_APPLY */
    if (style)
    {
     c_Page *page = document->PageArea->SelectedPage;
     F(page).SetFontStyle(page, style);
    }

   F(IMstyle).deleteAllItems(IMstyle);
   F(IMstyle).addItems(IMstyle, XcIM_LINKED_LIST, -1,
		       document->BaseStd.text_style_base,
		       offsetof(c_TextStyle, next));
   break;
  }

  UNSET_WAIT_MODE;
 }
}

/* ----------------------------------------------------------------- ** 
** MenuFormatFrameAttribs                                            ** 
** ----------------------------------------------------------------- */

static void MenuFormatFrameAttribs(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_attrib	*box;
 void		*doc;
 int		Type;	

 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (Type == DOCUMENT_WORDP)
 {
  SET_WAIT_MODE_STATIC;
  box = (c_Box_attrib *)NEW(c_Box_attrib)
   (MainInterface1->w_Main, "BoxAttribs",
    ((c_Document *)doc)->BaseStd.color_base,
    ((c_Document *)doc)->PageArea->ActivePage->frmg->SelectedCadre);
  if (F(box->PBox->Dialog).waitForUnmap(box->PBox->Dialog) ==  XcCD_OK)
   ((c_Document *)doc)->modified = TRUE;
  DELETE(c_Box_attrib)(box);
  UNSET_WAIT_MODE;
 }
}



/*---------------------------------------------------------------------*/
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
 FLUSH_UNDO;
}


/* ----------------------------------------------------------------- ** 
** MenuWindowArrange                                                 ** 
** ----------------------------------------------------------------- */
static void MenuWindowArrange(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 Display	*display;

 display = XtDisplay(WIDGET(MainInterface1));
 F(Gwm).ArrangeWindow(Gwm, display, XtWindow(WIDGET(MainDocument)));
}

/* ----------------------------------------------------------------- ** 
** MenuWindowTile                                                    ** 
** ----------------------------------------------------------------- */
static void MenuWindowTile(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 Display	*display;

 display = XtDisplay(WIDGET(MainInterface1));
 F(Gwm).TileWindow(Gwm, display, XtWindow(WIDGET(MainDocument)));
}


/* ----------------------------------------------------------------- ** 
** MenuHelpCallHelp                                                  ** 
** ----------------------------------------------------------------- */
void MenuHelpCallHelp(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 F(GlobRegHelp).call_help_for_widget(GlobRegHelp, w);
}

/* ----------------------------------------------------------------- ** 
** MenuHelpTrackHelp                                                 ** 
** ----------------------------------------------------------------- */
static void MenuHelpTrackHelp(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 F(GlobRegHelp).tracking_help(GlobRegHelp, MainInterface1->w_Main);
}


/* ----------------------------------------------------------------- ** 
** MenuHelpAbout                                                     ** 
** ----------------------------------------------------------------- */
static void MenuHelpAbout(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_about *boxabout;

 boxabout = (c_Box_about *)NEW(c_Box_about)(MainInterface1->w_Main,
					    "BoxAbout");
 F(boxabout->Dialog).waitForUnmap(boxabout->Dialog);
 DELETE(c_Box_about)(boxabout);
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

/* ----------------------------------------------------------------- ** 
** MenuHelpRegistration                                              ** 
** ----------------------------------------------------------------- */
static void MenuHelpRegistration(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_registration *box;
 
 box = (c_Box_registration *)NEW(c_Box_registration)(MainInterface1->w_Main,
						     "BoxRegistration");
 F(box->Dialog).waitForUnmap(box->Dialog);
 DELETE(c_Box_registration)(box);
}

/* ----------------------------------------------------------------- ** 
** Change style -> change actual text selection stylen               ** 
** ----------------------------------------------------------------- */
static void change_style(This, Param, style)
c_ItemMenu	*This;
void		*Param;
c_TextStyle	*style;
{
 c_Document	*document;
 c_Page		*page;
 int Type;

 document = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (!document || Type != DOCUMENT_WORDP)
  return;
  
 page = document->PageArea->SelectedPage;
 F(page).SetFontStyle(page, style);
}
/* ----------------------------------------------------------------- ** 
** Change font -> create style and set it to selection               ** 
** ----------------------------------------------------------------- */
static void change_font(This, Param, family)
c_ItemMenu	*This;
void		*Param;
family_tree	*family;
{
 c_Document	*document;
 c_Page		*page;
 int Type;

 document = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (!document || Type != DOCUMENT_WORDP)
  return;
  
 page = document->PageArea->SelectedPage;
 F(page).SetFontFamily(page, family);
}

/* ----------------------------------------------------------------- ** 
** Change font color -> create style and set it to selection         ** 
** ----------------------------------------------------------------- */
static void change_font_color(This, Param, color)
c_ColorList	*This;
void		*Param;
c_Color		*color;
{
 c_Document	*document;
 c_Page		*page;
 int Type;

 document = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (!document || Type != DOCUMENT_WORDP)
  return;
  
 page = document->PageArea->SelectedPage;
 F(page).SetFontColor(page, color);
}

/* ----------------------------------------------------------------- ** 
** Change font bgcolor -> create style and set it to selection       ** 
** ----------------------------------------------------------------- */
static void change_font_bgcolor(This, Param, color)
c_ColorList	*This;
void		*Param;
c_Color		*color;
{
 c_Document	*document;
 c_Page		*page;
 int Type;

 document = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (!document || Type != DOCUMENT_WORDP)
  return;
  
 page = document->PageArea->SelectedPage;
 F(page).SetFontBgColor(page, color);
}

/* ----------------------------------------------------------------- ** 
** Certify fonte size -> change fonte size		             ** 
** ----------------------------------------------------------------- */
static void certify_font_size(This, param)
c_WTextField *This;
void *param;
{
 c_Document	*document;
 c_Page		*page;
 coord_t	num;
 int		Type;

 Xc_HISTORY(("certify fonte size"));

 document = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (!document || Type != DOCUMENT_WORDP)
  return;

 num = ((coord_t)F(This).get_value_in_unit(This)) & COORD_FONTSIZE_MASK;
 F(This).set_value_in_unit(This, SCALE_TO_POINTS(num));
 
 page = document->PageArea->SelectedPage;
 F(page).SetFontSize(page, num);
 
 /*XtSetArg(argsw[0], XmNtraversalOn, False);*/
 /*XtSetValues(WIDGET(This), argsw, 1);*/

 motif_focus = tffs_focus = FALSE;

 if (!XmProcessTraversal(WIDGET(MainDocument), XmTRAVERSE_CURRENT))
  Xc_TRACE(("XmProcessTraversal failed"));
}

/* ----------------------------------------------------------------- ** 
** ModStructure                                                      ** 
** ----------------------------------------------------------------- */
void ModStructure(display, Event, listegwm)
Display		*display; 
XEvent		*Event;
listmanaged	*listegwm;
{
 void	*DocTmp;
  
 DocTmp = F(DocList).Find(DocList, listegwm->MainWin, NULL);
 RESIZE_DOCUMENT(DocTmp)(DocTmp, Event,
			 (listegwm->Type == D_MAXIMIZED ? TRUE : FALSE));
}

/* ----------------------------------------------------------------- ** 
** SelectFunction                                                    ** 
** ----------------------------------------------------------------- */
void SelectFunction(w, funct)
Widget	w;
int	funct;
{
 XEvent Noevent;

 void	*DocSelected;
  
 DocSelected = F(DocList).Find(DocList,
			       Gwm->SelectedWindow->MainWin, NULL);
 if (DocSelected)
  SET_FUNCTION_DOCUMENT(DocSelected)(DocSelected, funct);

 Noevent.type = ClientMessage;
 Noevent.xnoexpose.send_event = True;
 Noevent.xnoexpose.display = XtDisplay(wMainWindow);
 Noevent.xnoexpose.drawable = 1;
  
 XcHandleEvent(&Noevent);
}

/* ----------------------------------------------------------------- ** 
** ToggleSwitch                                                      ** 
** ----------------------------------------------------------------- */
static void ToggleSwitch(w, funct)
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
** ToggleSwitchRuler                                                 ** 
** ----------------------------------------------------------------- */
static void ToggleSwitchRuler(w, funct)
Widget	w;
int	funct;
{
 if (store_function_ruler == funct)
 {
  XmToggleButtonSetState(w, True, False);
  return;
 }
 
 store_function_ruler = funct;
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

/* ----------------------------------------------------------------- ** 
** ToggleTextflowSwitch                                              ** 
** ----------------------------------------------------------------- */
static void ToggleTextflowSwitch(w, funct)
Widget	w;
int	funct;
{
 int i, j;

 j = 0;
 while ((toggletextflowmodetable[j].func_num != funct)
	&& (toggletextflowmodetable[j].tog_wid != 0))
  j++;
  
 if (store_mode_textflow != toggletextflowmodetable[j].mode_num)
 {
  if (toggletextflowmodetable[j].tog_wid != 0)
   XmToggleButtonSetState(toggletextflowmodetable[j].tog_wid,
			  True, False);
  if (toggletextflowmodetable[j].tog_wid2 != 0)
   XmToggleButtonSetState(toggletextflowmodetable[j].tog_wid2,
			  True, False);
  i = 0;
  while ((toggletextflowmodetable[i].mode_num != store_mode_textflow)
	 && (toggletextflowmodetable[i].tog_wid != 0))
   i++;
  if (toggletextflowmodetable[i].tog_wid != 0)
   XmToggleButtonSetState(toggletextflowmodetable[i].tog_wid,
			  False, False);
  if (toggletextflowmodetable[i].tog_wid2 != 0)
   XmToggleButtonSetState(toggletextflowmodetable[i].tog_wid2,
			  False, False);
  store_mode_textflow = toggletextflowmodetable[j].mode_num;

  SelectFunction(w, funct);
 }
 else
 {
  XmToggleButtonSetState(w, True, False);
 }
}


/* ----------------------------------------------------------------- ** 
** SChangeWindow                                                     ** 
** ----------------------------------------------------------------- */
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
  if (Type1 == DOCUMENT_WORDP)
  {
   ABORT(((c_Document *)DocTmp1)->PageArea);
   INIT(((c_Document *)DocTmp1)->PageArea);
   F(((c_Document *)DocTmp1)->PageArea).UnsetCursor
    (((c_Document *)DocTmp1)->PageArea);
  }
 }
 DocTmp2 = F(DocList).Find(DocList, listNew->MainWin, &Type2);
 if (Type2 == DOCUMENT_WORDP)
 {
  ABORT(((c_Document *)DocTmp2)->PageArea);
  INIT(((c_Document *)DocTmp2)->PageArea);
  F(((c_Document *)DocTmp2)->PageArea).ResetCursor
   (((c_Document *)DocTmp2)->PageArea);
 }
}

/* ----------------------------------------------------------------- ** 
** ChangeDocCallback                                                 ** 
** ----------------------------------------------------------------- */
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
** QuitApplication - Quit XAllWrite, free all remaining objects      ** 
** ----------------------------------------------------------------- */

void QuitApplication()
{
 int i;
 Xc_HISTORY(("Quit XAllWrite"));
 
 handle_event_allowed = FALSE;

 F(MainInterface1).unmap_TopLevel(MainInterface1);
 WriteUserConfigFile();

#ifdef  SIGNALH
 DELETE(c_SignalHandler)(SignalH);
#endif

 DELETE(c_DocumentList)(DocList);

 if (GlobTextClipboard) DELETE(c_Text)(GlobTextClipboard);
 while(GlobTextRulerBase != NULL) DELETE(c_TextRuler)(GlobTextRulerBase);
 while(GlobTextStyleBase != NULL) DELETE(c_TextStyle)(GlobTextStyleBase);
 while(GlobColorBase != NULL) DELETE(c_Color)(GlobColorBase);
 while(GlobFontBase != NULL) DELETE(c_VectorFont)(GlobFontBase);

 DELETE(c_XErrorHandler)(XErrH);
 DELETE(c_WinManager)(Gwm, XtDisplay(WIDGET(MainInterface1)));
 DELETE(c_ClipBuffer)(GlobClipBuffer);
 DELETE(c_Colormap)(GlobColormap);
 DELETE(c_FontManager)(GlobFntMng);
 DELETE(c_ImageCache)(GlobImgCache);
 DELETE(c_ImageManager)(GlobImgMng);
 DELETE(c_Trash)(GlobTrash);
 DELETE(c_DragAndDrop)(GlobDragAndDrop);
 DELETE(c_Keyboard)(GlobKeyboard);
 DELETE(c_RegisterHelp)(GlobRegHelp);
 DELETE(c_DocIcon)(GlobDocIcon);
 
 if (t1imager_buffer) Xc_free(t1imager_buffer);
 if (Globprog_name) Xc_free(Globprog_name);
 Reset_XaRegle(XtDisplay(WIDGET(MainInterface1)));

 /*--- File selectors ---*/
 if(OpenDocumentSelector != NULL)
  DELETE(c_FileSelector)(OpenDocumentSelector);
 if(CloseDocumentSelector != NULL)
  DELETE(c_FileSelector)(CloseDocumentSelector);
 if(PrintDocumentSelector != NULL)
  DELETE(c_FileSelector)(PrintDocumentSelector);
 if(ImportImageSelector != NULL) 
  DELETE(c_FileSelector)(ImportImageSelector);
 if(ImportDocumentSelector != NULL)
  DestroyImportDocumentSelector();
 if(ImportVectorSelector != NULL)
  DELETE(c_FileSelector)(ImportVectorSelector);
 
 DELETE(c_Import)(GlobImport);

 /* ----------------------------------------------------------------- ** 
 ** interface                                                         ** 
 ** ----------------------------------------------------------------- */

 DELETE(c_Icon)(IconMis_1); DELETE(c_Icon)(IconMis_2);
 DELETE(c_Icon)(IconMis_3); DELETE(c_Icon)(IconMis_4); 
 DELETE(c_Icon)(IconMis_s1); DELETE(c_Icon)(IconMis_8);
 DELETE(c_Icon)(IconMis_9); DELETE(c_Icon)(IconMis_10);
 DELETE(c_Icon)(IconMis_s2); DELETE(c_Icon)(IconMis_5); 
 DELETE(c_Icon)(IconMis_6); DELETE(c_Icon)(IconMis_7);  

 DELETE(c_Icon)(IconFon_s1); DELETE(c_Icon)(IconFon_s2);
 DELETE(c_Icon)(IconFon_s3); DELETE(c_Icon)(IconFon_s4);
 DELETE(c_Icon)(IconFon_3); DELETE(c_Icon)(IconFon_4);
 DELETE(c_Icon)(IconFon_5);
 DELETE(c_Icon)(IconFon_7); DELETE(c_Icon)(IconFon_8);
 DELETE(c_Icon)(IconFon_9); /*	DELETE(c_Icon)(IconFon_10); */
 DELETE(c_Icon)(IconFon_11); 
 DELETE(c_ColorList)(CLfontColor); DELETE(c_ColorList)(CLfontBgColor);
 DELETE(c_ItemMenu)(IMfontFamily); DELETE(c_ItemMenu)(IMstyle);
 DELETE(c_SpinBox)(sbFontSize);

 DELETE(c_Icon)(IconRul_s1); DELETE(c_Icon)(IconRul_s2);
 DELETE(c_Icon)(IconRul_s3); DELETE(c_Icon)(IconRul_s4);
 DELETE(c_Icon)(IconRul_1); DELETE(c_Icon)(IconRul_2);
 DELETE(c_Icon)(IconRul_3); DELETE(c_Icon)(IconRul_4);
 DELETE(c_Icon)(IconRul_5); DELETE(c_Icon)(IconRul_6);
 DELETE(c_Icon)(IconRul_7); DELETE(c_Icon)(IconRul_8);
 DELETE(c_Icon)(IconRul_10); DELETE(c_Icon)(IconRul_11);
 DELETE(c_Icon)(IconRul_9); DELETE(c_Icon)(IconRul_12);

 DELETE(c_Icon)(IconIma_1); DELETE(c_Icon)(IconIma_2);
 DELETE(c_Icon)(IconIma_3); DELETE(c_Icon)(IconIma_s1);
 DELETE(c_Icon)(IconIma_4); DELETE(c_Icon)(IconIma_5); 
 DELETE(c_Icon)(IconIma_s2); DELETE(c_Icon)(IconIma_6);
 DELETE(c_Icon)(IconIma_7); DELETE(c_Icon)(IconIma_8);
 DELETE(c_Icon)(IconIma_9);

 DELETE(c_Icon)(IconVec_s1); DELETE(c_Icon)(IconVec_1);  

 DELETE(c_Icon)(IconFra_s1); DELETE(c_Icon)(IconFra_s2);
 DELETE(c_Icon)(IconFra_s3); DELETE(c_Icon)(IconFra_s4);
 DELETE(c_Icon)(IconFra_1); DELETE(c_Icon)(IconFra_2);
 DELETE(c_Icon)(IconFra_3); DELETE(c_Icon)(IconFra_4);
 DELETE(c_Icon)(IconFra_5); DELETE(c_Icon)(IconFra_6);
 DELETE(c_Icon)(IconFra_7); DELETE(c_Icon)(IconFra_8);
 DELETE(c_Icon)(IconFra_9); DELETE(c_Icon)(IconFra_10);
 DELETE(c_Icon)(IconFra_11); DELETE(c_Icon)(IconFra_12);
 DELETE(c_Icon)(IconFra_14); DELETE(c_Icon)(IconFra_15);
 DELETE(c_Icon)(IconFra_16); DELETE(c_Icon)(IconFra_17);
 DELETE(c_Icon)(IconFra_18); DELETE(c_Icon)(IconFra_19);
 DELETE(c_Icon)(IconFra_20);

 DELETE(c_Icon)(IconZoo_1); DELETE(c_Icon)(IconZoo_2);
 DELETE(c_Icon)(IconZoo_3); DELETE(c_Icon)(IconZoo_s1);
 DELETE(c_Icon)(IconZoo_s2); DELETE(c_Icon)(IconZoo_4);
 DELETE(c_Icon)(IconZoo_5); DELETE(c_Icon)(IconZoo_6);
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
 
 DELETE(c_Icon)(IconBottom);
 DELETE(c_Form)(BottomBar);
 
 DELETE(c_PulldownMenu)(MenuFil);
 DELETE(c_PulldownMenu)(MenuEdi);
 DELETE(c_PulldownMenu)(MenuDis);
 DELETE(c_PulldownMenu)(MenuIns);
 DELETE(c_PulldownMenu)(MenuFor);
 DELETE(c_PulldownMenu)(MenuWin);
 DELETE(c_PulldownMenu)(MenuHel);
 DELETE(c_MenuBar)(Bar1);
 
 DELETE(c_MainInterface)(MainInterface1);

 DELETE(c_Undo)(GlobUndo); GlobUndo = NULL;
 DELETE(c_HelpLine)(GlobHelpLine);
 DELETE(c_RemoteServer)(GlobRemoteServer);
 DELETE(c_GCursor)(GlobCursor);
 DELETE(c_Resource)(GlobResources);
 
#ifndef NMEMDEBUG
 Xc_mdump();
#endif

 Xc_exit(0);
}

/* ----------------------------------------------------------------- ** 
** AttachHBar - attach and manage a horizontal icon bar              ** 
** ----------------------------------------------------------------- */
static void AttachHBar(hIBarForm, num)
c_Form		*hIBarForm;
int		num;
{
 c_RowColumn	*hIBar = hIconBar[num];
 char		nbhb[3];

 F(hIBarForm).attach_left(WIDGET(hIBarForm), WIDGET(hIBar), NULL);
 F(hIBarForm).attach_top(WIDGET(hIBarForm), WIDGET(hIBar), NULL);
 F(hIBarForm).attach_bottom(WIDGET(hIBarForm), WIDGET(hIBar), NULL);
 F(hIBarForm).attach_right(WIDGET(hIBarForm), WIDGET(hIBar), NULL);

 /* store in hIBarForm window name, the num of hIconBar (for undo) */
 sprintf(nbhb, "%d", num + 1);
 XStoreName(XtDisplay(WIDGET(hIBarForm)), XtWindow(WIDGET(hIBarForm)),
	    nbhb);

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
  AttachHBar(hIconBarForm, num);
  XtUnmanageChild(WIDGET(hIconBar[bar_selected]));
  bar_selected = num;
 }
}

/* ----------------------------------------------------------------- ** 
** UndoCallback - call by GlobUndo when the Interface must be        ** 
**                re-state                                           ** 
** ----------------------------------------------------------------- */
static void UndoCallback()
{
 int i, hbar;
 listmanaged	*listegwm;
 Display	*display;
 c_Document	*Document;

 Xc_TRACE(("Re-state interface"));
 
 display = XtDisplay(WIDGET(MainInterface1));
 F(Gwm).UpdateWindow(Gwm, display, XtWindow(WIDGET(MainDocument)));
 listegwm = Gwm->ListManaged;
 while(listegwm)
 {
  Document = F(DocList).Find(DocList, listegwm->MainWin, NULL);
  
  if (listegwm->Type != D_ICONED)
  {
   F(Document).Update_Document(Document);
   if (listegwm->Type == D_MAXIMIZED)
   {
    Window none;
    int dummy;
    XEvent event;
    
    XGetGeometry(display, listegwm->MainWin, &none,
		 &(event.xconfigure.x), &(event.xconfigure.y), 
		 &(event.xconfigure.width), &(event.xconfigure.height), 
		 &dummy, &dummy);
    
    RESIZE_DOCUMENT(Document)(Document, &event, TRUE);
   }
  }
  
  listegwm = listegwm->NextManaged;
 }

 /*--- inverse all button state ---*/
 ButtonLockFrame ^= TRUE;
 ButtonHoleOn ^= TRUE;
 ButtonActivBold ^= TRUE;
 ButtonActivItalic ^= TRUE;
 ButtonActivUnderline ^= TRUE;
 ButtonActivStrikeout ^= TRUE;
 ButtonActivSubscript ^= TRUE;
 ButtonActivSupscript ^= TRUE;
 ButtonActivShadow ^= TRUE;
 ButtonActivSmallCaps ^= TRUE;
 ButtonActivBigCaps ^= TRUE;
 ButtonActivJLeft ^= TRUE;
 ButtonActivJCenter ^= TRUE;
 ButtonActivJRight ^= TRUE;
 ButtonActivJustify ^= TRUE;
 ButtonActivSpecChar ^= TRUE;
 ButtonMaskBottomBar ^= TRUE;
 ButtonMaskVectorText ^= TRUE;
 ButtonMaskFrame ^= TRUE;
 
 DocAlreadyGrised ^= TRUE;
 FrameToolsAlreadyGrised ^= TRUE;
 MultiFrameToolsAlreadyGrised ^= TRUE;
 LockToolsAlreadyGrised ^= TRUE;
 STextAlreadyGrised ^= TRUE;
 SImageAlreadyGrised ^= TRUE;
 SVectorAlreadyGrised ^= TRUE;

 store_function = -1;
 store_function_ruler = -1;
 store_widget_imagevector = (Widget)0;
 store_mode_textflow = -1;
 
 FontSizeValue = COORD_ZERO;
 {
  void *item;
  item = IMstyle->selection->item; IMstyle->selection = NULL;
  F(IMstyle).setSelection(IMstyle, item);
  item = IMfontFamily->selection->item; IMfontFamily->selection = NULL;
  F(IMfontFamily).setSelection(IMfontFamily, item);
 }
 F(CLfontColor).setColor(CLfontColor, NULL);
 F(CLfontBgColor).setColor(CLfontBgColor, NULL);

 {
  char *p;
  
  if (XFetchName(XtDisplay(WIDGET(hIconBarForm)), 
		 XtWindow(WIDGET(hIconBarForm)), &p) && 
      (hbar = (atoi(p) - 1)) >= 0)
   XFree(p);
  else 
   hbar = bar_selected;
 }
 for(i = 0; i < NB_HBAR; i++)
 {
  XmToggleButtonSetState(WIDGET(vIcon[i]), True, True);
 }
 XmToggleButtonSetState(WIDGET(vIcon[hbar]), True, True);

 F(GlobHelpLine).change_function_message(GlobHelpLine, XcR_NONE, XcR_DEFAULT); 
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
 GlobKeyboard->event_enable = FALSE;
}

static void cb_unfocus_motif(w, none, cbs)
Widget 		w;
void		*none;
XmRowColumnCallbackStruct *cbs;
{
 Xc_TRACE(("UNFOCUS MOTIF"));
 motif_focus = FALSE;
 GlobKeyboard->event_enable = TRUE;
}

static void cb_focus_tfFontSize(w, This, event)
Widget w;            
c_WTextField *This;
XEvent	*event;
{
 Xc_TRACE(("FOCUS MOTIF (for tfFontSize)"));
 
 motif_focus = tffs_focus = TRUE;
 GlobKeyboard->event_enable = FALSE;
}

static void cb_unfocus_tfFontSize(w, This, event)
Widget w;            
c_WTextField *This;
XEvent	*event;
{
 Xc_TRACE(("UNFOCUS MOTIF (for tfFontSize)"));
 
 XtCallCallbacks(w, XmNlosingFocusCallback, This);
 
 motif_focus = tffs_focus = FALSE;
 GlobKeyboard->event_enable = TRUE;
}

/* ----------------------------------------------------------------- ** 
** SensitivizeLockTools: au moins un cadre doit etre select         ** 
** ----------------------------------------------------------------- */

void SensitivizeLockTools(flag)
Boolean flag;
{
 if (flag == (Boolean)LockToolsAlreadyGrised)
 {
  LockToolsAlreadyGrised = !LockToolsAlreadyGrised;
  
  if (!flag)
  {
   if (!ButtonLockFrame)
   {
    ButtonLockFrame = TRUE;
    XmToggleButtonSetState(WIDGET(IconFra_12), False, False);
   }
  }

  F(MenuFil).ButtonFlag(MenuFil, MFil_IMPORTIMAGE, flag);
  F(MenuFil).ButtonFlag(MenuFil, MFil_IMPORTVECTOR, flag);
  F(MenuFor).ButtonFlag(MenuFor, MFor_EDITFRAME, flag);
  XtSetSensitive(WIDGET(IconFra_12), flag);

  XtSetSensitive(WIDGET(IconFra_8), flag);
  XtSetSensitive(WIDGET(IconFra_9), flag);
  XtSetSensitive(WIDGET(IconFra_10), flag);
  XtSetSensitive(WIDGET(IconFra_11), flag);

  F(MenuFor).ButtonFlag(MenuFor, MFor_TEXTFLOW, flag);
  XtSetSensitive(WIDGET(IconFra_14), flag);
  XtSetSensitive(WIDGET(IconFra_15), flag);
  XtSetSensitive(WIDGET(IconFra_16), flag);
  XtSetSensitive(WIDGET(IconFra_17), flag);
  XtSetSensitive(WIDGET(IconFra_18), flag);
  XtSetSensitive(WIDGET(IconFra_19), flag);
  XtSetSensitive(WIDGET(IconFra_20), flag);

  F(MenuFor).ButtonFlag(MenuFor, MFor_MOVEDOWN, flag);
  F(MenuFor).ButtonFlag(MenuFor, MFor_MOVEUP, flag);
  F(MenuFor).ButtonFlag(MenuFor, MFor_BACKGROUND, flag);
  F(MenuFor).ButtonFlag(MenuFor, MFor_FOREGROUND, flag);
  F(MenuFor).ButtonFlag(MenuFor, MFor_ATTRIBS, flag);
 }
}

/* ----------------------------------------------------------------- ** 
** SensitivizeFrameTools: au moins un cadre doit etre select non lock    ** 
** ----------------------------------------------------------------- */

void SensitivizeFrameTools(flag)
Boolean flag;
{
 if (flag == (Boolean)FrameToolsAlreadyGrised)
 {
  FrameToolsAlreadyGrised = !FrameToolsAlreadyGrised;

  XtSetSensitive(WIDGET(IconFra_5), flag);
  XtSetSensitive(WIDGET(IconFra_6), flag);
  XtSetSensitive(WIDGET(IconFra_7), flag);

  F(MenuFor).ButtonFlag(MenuFor, MFor_POINTINSERT, flag);
  F(MenuFor).ButtonFlag(MenuFor, MFor_POINTDELETE, flag);
  F(MenuFor).ButtonFlag(MenuFor, MFor_POINTMOVE, flag);

  XtSetSensitive(WIDGET(IconTra), flag);
  F(MenuFor).ButtonFlag(MenuFor, MFor_FRAMEDELETE, flag);

 }
}

/* ----------------------------------------------------------------- ** 
** SensitivizeFrameTools: au moins deux cadre doivent etre select non lock    ** 
** ----------------------------------------------------------------- */

void SensitivizeMultiFrameTools(flag)
Boolean flag;
{
 if (flag == (Boolean)MultiFrameToolsAlreadyGrised)
 {
  MultiFrameToolsAlreadyGrised = !MultiFrameToolsAlreadyGrised;

 }
}

/* ----------------------------------------------------------------- ** 
** SensitivizeDoc: (Un)Sensitivize in case of document existance     ** 
** ----------------------------------------------------------------- */
static void SensitivizeDoc(flag)
Boolean flag;
{
 if (flag == (Boolean)DocAlreadyGrised)
 {
  DocAlreadyGrised = !DocAlreadyGrised;

  if (!flag)
  {
   if (ButtonActivSpecChar)
   {
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWSPECCHARS, False);
    XmToggleButtonSetState(WIDGET(IconRul_9), False, False);
    ButtonActivSpecChar = FALSE;
   }
   F(MenuFil).ButtonFlag(MenuFil, MFil_IMPORTIMAGE, flag);
   F(MenuFil).ButtonFlag(MenuFil, MFil_IMPORTTEXT, !flag);
   F(MenuFil).ButtonFlag(MenuFil, MFil_IMPORTVECTOR, flag);
  }

  XtSetSensitive(MenuEdi->w_Cascade, flag);
  XtSetSensitive(WIDGET(IconMis_5), flag);
  XtSetSensitive(WIDGET(IconMis_6), flag);
  XtSetSensitive(WIDGET(IconMis_7), flag);
  XtSetSensitive(WIDGET(IconMis_8), flag);

  XtSetSensitive(MenuDis->w_Cascade, flag);
  XtSetSensitive(MenuIns->w_Cascade, flag);
  XtSetSensitive(MenuFor->w_Cascade, flag);
  XtSetSensitive(MenuWin->w_Cascade, flag);
      
  F(MenuFil).ButtonFlag(MenuFil, MFil_CLOSE, flag);
  F(MenuFil).ButtonFlag(MenuFil, MFil_SAVE, flag);
  XtSetSensitive(WIDGET(IconMis_3), flag);
  F(MenuFil).ButtonFlag(MenuFil, MFil_SAVEAS, flag);
  F(MenuFil).ButtonFlag(MenuFil, MFil_SAVEALL, flag);
  F(MenuFil).ButtonFlag(MenuFil, MFil_PRINT, flag);
  XtSetSensitive(WIDGET(IconMis_4), flag);

  XtSetSensitive(WIDGET(IconRul_9), flag);
  
  /* frame bar */
  XtSetSensitive(WIDGET(IconFra_1), flag);
  XtSetSensitive(WIDGET(IconFra_2), flag);
  XtSetSensitive(WIDGET(IconFra_3), flag);
  XtSetSensitive(WIDGET(IconFra_4), flag);
   /* zoom bar */
  XtSetSensitive(WIDGET(IconZoo_1), flag);
  XtSetSensitive(WIDGET(IconZoo_2), flag);
  XtSetSensitive(WIDGET(IconZoo_3), flag);
  XtSetSensitive(WIDGET(IconZoo_4), flag);
  XtSetSensitive(WIDGET(IconZoo_5), flag);
  XtSetSensitive(WIDGET(IconZoo_6), flag);
  XtSetSensitive(WIDGET(IconZoo_7), flag);
  XtSetSensitive(WIDGET(IconZoo_8), flag);
  XtSetSensitive(WIDGET(IconZoo_9), flag);
  XtSetSensitive(WIDGET(IconZoo_10), flag);
  XtSetSensitive(WIDGET(IconZoo_11), flag);
  XtSetSensitive(WIDGET(IconZoo_12), flag);
 }
}

static void SensitivizeSText(flag)
Boolean flag;
{
 if (flag == (Boolean)STextAlreadyGrised)
 {
  STextAlreadyGrised = !STextAlreadyGrised;

  if (!flag)			/* TEXT WILL BE GRAY: UNSELECT BUTTONS */
  {
   if (ButtonActivBold)
   {
    XmToggleButtonSetState(WIDGET(IconFon_3), False, False);
    ButtonActivBold = FALSE;
   }
   if (ButtonActivItalic)
   {
    XmToggleButtonSetState(WIDGET(IconFon_4), False, False);
    ButtonActivItalic = FALSE;
   }
   if (ButtonActivUnderline)
   {
    XmToggleButtonSetState(WIDGET(IconFon_5), False, False);
    ButtonActivUnderline = FALSE;
   }
   if (ButtonActivSubscript)
   {
    XmToggleButtonSetState(WIDGET(IconFon_7), False, False);
    ButtonActivSubscript = FALSE;
   }
   if (ButtonActivSupscript)
   {
    XmToggleButtonSetState(WIDGET(IconFon_8), False, False);
    ButtonActivSupscript = FALSE;
   }
   if (ButtonActivShadow)
   {
    XmToggleButtonSetState(WIDGET(IconFon_9), False, False);
    ButtonActivShadow = FALSE;
   }
   if (ButtonActivSmallCaps)
   {
    XmToggleButtonSetState(WIDGET(IconFon_11), False, False);
    ButtonActivSmallCaps = FALSE;
   }
   if (ButtonActivJLeft)
   {
    XmToggleButtonSetState(WIDGET(IconRul_1), False, False);
    ButtonActivJLeft = FALSE;
   }
   if (ButtonActivJCenter)
   {
    XmToggleButtonSetState(WIDGET(IconRul_2), False, False);
    ButtonActivJCenter = FALSE;
   }
   if (ButtonActivJRight)
   {
    XmToggleButtonSetState(WIDGET(IconRul_3), False, False);
    ButtonActivJRight = FALSE;
   }
   if (ButtonActivJustify)
   {
    XmToggleButtonSetState(WIDGET(IconRul_4), False, False);
    ButtonActivJustify = FALSE;
   }
  }
  
  F(MenuFil).ButtonFlag(MenuFil, MFil_IMPORTTEXT, flag);
  F(MenuFor).ButtonFlag(MenuFor, MFor_FONTS, flag);

  F(MenuIns).ButtonFlag(MenuIns, MIns_PAGEBREAK, flag);
  F(MenuIns).ButtonFlag(MenuIns, MIns_EUROSIGN, flag);
  F(MenuIns).ButtonFlag(MenuIns, MIns_CHAR, flag);


  XtSetSensitive(WIDGET(sbFontSize), flag);
  XtSetSensitive(WIDGET(IconFon_3), flag);
  XtSetSensitive(WIDGET(IconFon_4), flag);
  XtSetSensitive(WIDGET(IconFon_5), flag);
  XtSetSensitive(WIDGET(IconFon_7), flag);
  XtSetSensitive(WIDGET(IconFon_8), flag);
  XtSetSensitive(WIDGET(IconFon_9), flag);
  XtSetSensitive(WIDGET(IconFon_11), flag);
  XtSetSensitive(WIDGET(IMfontFamily), flag);
  XtSetSensitive(WIDGET(IMstyle), flag);
  XtSetSensitive(WIDGET(CLfontColor), flag);
  XtSetSensitive(WIDGET(CLfontBgColor), flag);

  /* ruler bar */
  XtSetSensitive(WIDGET(IconRul_1), flag);
  XtSetSensitive(WIDGET(IconRul_2), flag);
  XtSetSensitive(WIDGET(IconRul_3), flag);
  XtSetSensitive(WIDGET(IconRul_4), flag);
  XtSetSensitive(WIDGET(IconRul_5), flag);
  XtSetSensitive(WIDGET(IconRul_6), flag);
  XtSetSensitive(WIDGET(IconRul_7), flag);
  XtSetSensitive(WIDGET(IconRul_8), flag);
  XtSetSensitive(WIDGET(IconRul_10), flag);
  XtSetSensitive(WIDGET(IconRul_11), flag);
  XtSetSensitive(WIDGET(IconRul_12), flag);

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


/* ----------------------------------------------------------------- ** 
** HandleEvent - Process event                                       ** 
** ----------------------------------------------------------------- */
void XcHandleEvent(event)
XEvent *event;
{
 listmanaged	*listegwm;
 void		*DocumentTmp;
 void		*DocumentSel;
 int		gwm_func;
 int		TypeSel;
 int		TypeTmp;
 int     	num;
 boolean	dbl_click;
 int		i;

 if (!handle_event_allowed || motif_focus)
 {
  if (handle_event_allowed && tffs_focus)
  {
   if ((event->type == KeyPress || event->type == KeyRelease))
   {
    event->xkey.window = XtWindow(WIDGET(tfFontSize));
    XtDispatchEvent(event);
    return;
   }
   
   if ((event->type == ButtonPress || event->type == ButtonRelease) && 
       event->xbutton.window != XtWindow(WIDGET(tfFontSize)))
   {
    cb_unfocus_tfFontSize(WIDGET(tfFontSize), tfFontSize, event);
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
 
 /* ------------- DRAG AND DROP MANAGEMENT -------------- */ 
 if (GlobDragAndDrop->active)
 {
  if (!F(GlobDragAndDrop).eventlook_drag(GlobDragAndDrop, event))
   return;
 }
 if ((event->type == ClientMessage) && ((DROP_FLAGS(*event) & 0xffff00)
					== DD_XCLAMATION_INDENTITY))
 {
  Xc_TRACE(("client message received"));
  F(GlobDragAndDrop).eventlook_drop(GlobDragAndDrop, event);
  return;
 }
 /* ------------- END OF DRAG AND DROP MANAGEMENT -------------- */ 

 /* ------------- REMOTE SERVER MANAGEMENT -------------- */ 
 if (event->type == PropertyNotify)
 {
  if (F(GlobRemoteServer).eventlook(GlobRemoteServer, event))
   return;
 }
 if ((event->type == ClientMessage) &&
     (int)event->xclient.data.l[0] == (int)0xdeaddddd)
 {
  Xc_TRACE(("Close Window Notify"));
  QuitApplication();
 }
 /* ------------- END OF REMOTE SERVER MANAGEMENT -------------- */ 

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
 
 if ((event->xany.window == XtWindow(WIDGET(MainDocument))) && 
     (event->type == ConfigureNotify))
 {
  F(Gwm).ArrangeIcon(Gwm, event->xany.display,
		     XtWindow(WIDGET(MainDocument)));
 }

 if (motif_focus && (event->type == KeyPress || event->type == KeyRelease))
 {
#if defined(___ncr_svr4) 
  if (XtWindow(WIDGET(tfFontSize)) == event->xany.window)
   XtDispatchEventToWidget(WIDGET(tfFontSize), event);
  else
   XtDispatchEvent(event);
#else
  XtDispatchEvent(event);
#endif
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
		  event->xconfigure.height);
    XResizeWindow(event->xany.display,
		  listegwm->MainWin,
		  event->xconfigure.width,
		  event->xconfigure.height);
    XMoveWindow(event->xany.display, 
		listegwm->Use->UnMaxWin,
		event->xconfigure.width - BUTTON_UMAXW, 0);
    ModStructure(event->xany.display, event, listegwm);
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
      int x, y;
      Window wnone;

      XTranslateCoordinates(event->xany.display, listegwm->Icon_Name,
			    listegwm->Icon, event->xcrossing.x,
			    event->xcrossing.y, &x, &y, &wnone);
      if ((x >= 0) && (x < ICONW) && (y >= 0) && (y < ICONH))
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
       if (event->xbutton.time - Gwm->last_iclick < DBL_CLK_DELAY)
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
    }/* switch */
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
   
   if ((DocumentTmp == DocumentSel) && (TypeTmp == DOCUMENT_WORDP))
   {
    c_PageArea	*PageArea;
    
    PageArea = ((c_Document *)DocumentTmp)->PageArea;
    if ( store_function != PageArea->function_type)
    {
     /* ----------------------------------------------------------------- ** 
     ** change the toggle button function                                 ** 
     ** ----------------------------------------------------------------- */
     int i;
		  
     i = 0;
     while ((toggletable[i].func_num != store_function) && 
	    (toggletable[i].tog_wid != 0))
      i++;
     if (toggletable[i].tog_wid != 0)
      XmToggleButtonSetState(toggletable[i].tog_wid,
			     False, False);

     store_function = PageArea->function_type;
     i = 0;
     while ((toggletable[i].func_num != store_function) && 
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
  ** end of current document test                                      ** 
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
   ModStructure(event->xany.display, event, listegwm);
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
 
 if ((Gwm->SelectedWindow == NULL) || (TypeSel != DOCUMENT_WORDP))
 {
  SensitivizeSText(False);
  SensitivizeSImage(False);
  SensitivizeSVector(False);
  SensitivizeFrameTools(False);
  SensitivizeMultiFrameTools(False);
  SensitivizeLockTools(False);
  SensitivizeDoc(False);

  F(GlobHelpLine).change_function_message(GlobHelpLine, 
					  XcR_NONE, XcR_DEFAULT);
 }
 else				/* a WP document is selected */
 {
  c_Document	*DocSelected;
  c_Text	*text;
  unsigned int	attributes;
  textruler_just_t justification;

  SensitivizeDoc(True);
  DocSelected = (c_Document *)DocumentSel;
  /*   F(GlobHelpLine).change_function_message
       (GlobHelpLine, 
       ((c_Document *)DocumentSel)->SelectedPage->Page->function_type,
       TypeSel); */
   
  if (DocSelected->PageArea->Redraw->spec_chars == TRUE)
  {
   if (!ButtonActivSpecChar)
   {
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWSPECCHARS, True);
    XmToggleButtonSetState(WIDGET(IconRul_9), True, False);
    ButtonActivSpecChar = TRUE;
   }
  }
  else
  {
   if (ButtonActivSpecChar)
   {
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWSPECCHARS, False);
    XmToggleButtonSetState(WIDGET(IconRul_9), False, False);
    ButtonActivSpecChar = FALSE;
   }
  }
   
  if (DocSelected->PageArea->SelectedPage->frmg->Frame_Border_actif)
  {
   if (!ButtonMaskFrame)
   {
    ButtonMaskFrame = TRUE;
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWFRAMESBORDER, True);
   }
  }
  else
  {
   if (ButtonMaskFrame)
   {
    ButtonMaskFrame = FALSE;
    F(MenuDis).ToggleButton(MenuDis, MDis_SHOWFRAMESBORDER, False);
   }
  }

  if (DocSelected->PageArea->select_type)/* TEXT SELECTION */
  {
   SensitivizeSText(True);
   SensitivizeSImage(False);
   SensitivizeSVector(False);
   SensitivizeFrameTools(False);
   SensitivizeMultiFrameTools(False);
   SensitivizeLockTools(False);

   text = DocSelected->BaseStd.text_base;
   
   /* ----------------------------------------------------------------- ** 
   ** Style Bar Activation                                              ** 
   ** ----------------------------------------------------------------- */
   
   attributes = text->style->attributes;
   
   if ((text->style->inherit_from != NULL) ? 
       (text->style->inherit_from != saved_style) :
       (text->style != saved_style))
   {
    saved_style = (text->style->inherit_from != NULL) ? 
     text->style->inherit_from : text->style;

    F(IMstyle).deleteAllItems(IMstyle);
    F(IMstyle).addItems(IMstyle, XcIM_LINKED_LIST, -1,
			text->BaseStd->text_style_base,
			offsetof(c_TextStyle, next));

    F(IMstyle).setSelection(IMstyle, saved_style);
   }

   if (text->style->font->spec_family != saved_family)
   {
    saved_family = text->style->font->spec_family;
    F(IMfontFamily).setSelection(IMfontFamily, saved_family);
   }
   
   if (text->style->color != F(CLfontColor).getColor(CLfontColor))
   {
    F(CLfontColor).setColorBase(CLfontColor, 
				DocSelected->BaseStd.color_base,
				CL_SKIP_TRANSPARENCY);
    F(CLfontColor).setColor(CLfontColor, text->style->color);
   }
   if (text->style->bgcolor != F(CLfontBgColor).getColor(CLfontBgColor))
   {
    F(CLfontBgColor).setColorBase(CLfontBgColor, 
				  DocSelected->BaseStd.color_base,
				  CL_KEEP_TRANSPARENCY);
    F(CLfontBgColor).setColor(CLfontBgColor, text->style->bgcolor);
   }
   
   if ((attributes & XcTS_B_BOLD) == XcTS_B_BOLD)
   {
    if (!ButtonActivBold)
    {
     XmToggleButtonSetState(WIDGET(IconFon_3), True, False);
     F(MenuFor).ToggleButton(MenuFor, MFor_BOLD, True);
     ButtonActivBold = TRUE;
    }
   }
   else
   {
    if (ButtonActivBold)
    {
     XmToggleButtonSetState(WIDGET(IconFon_3), False, False);
     F(MenuFor).ToggleButton(MenuFor, MFor_BOLD, False);
     ButtonActivBold = FALSE;
    }
   }
   
   if ((attributes & XcTS_B_ITALIC) == XcTS_B_ITALIC)
   {
    if (!ButtonActivItalic)
    {
     XmToggleButtonSetState(WIDGET(IconFon_4), True, False);
     F(MenuFor).ToggleButton(MenuFor, MFor_ITALIC, True);
     ButtonActivItalic = TRUE;
    }
   }
   else
   {
    if (ButtonActivItalic)
    {
     XmToggleButtonSetState(WIDGET(IconFon_4), False, False);
     F(MenuFor).ToggleButton(MenuFor, MFor_ITALIC, False);
     ButtonActivItalic = FALSE;
    }
   }
   
   if ((attributes & XcTS_B_UNDERLINE) == XcTS_B_UNDERLINE)
   {
    if (!ButtonActivUnderline)
    {
     XmToggleButtonSetState(WIDGET(IconFon_5), True, False);
     F(MenuFor).ToggleButton(MenuFor, MFor_UNDERLINE, True);
     ButtonActivUnderline = TRUE;
    }
   }
   else
   {
    if (ButtonActivUnderline)
    {
     XmToggleButtonSetState(WIDGET(IconFon_5), False, False);
     F(MenuFor).ToggleButton(MenuFor, MFor_UNDERLINE, False);
     ButtonActivUnderline = FALSE;
    }
   }
   
   if ((attributes & XcTS_B_STRIKEOUT) == XcTS_B_STRIKEOUT)
   {
    if (!ButtonActivStrikeout)
    {
     F(MenuFor).ToggleButton(MenuFor, MFor_STRIKEOUT, True);
     ButtonActivStrikeout = TRUE;
    }
   }
   else
   {
    if (ButtonActivStrikeout)
    {
     F(MenuFor).ToggleButton(MenuFor, MFor_STRIKEOUT, False);
     ButtonActivStrikeout = FALSE;
    }
   }
 
   if ((attributes & XcTS_B_SUBSCRIPT) == XcTS_B_SUBSCRIPT)
   {
    if (!ButtonActivSubscript)
    {
     XmToggleButtonSetState(WIDGET(IconFon_7), True, False);
     F(MenuFor).ToggleButton(MenuFor, MFor_SUBSCRIPT, True);
     ButtonActivSubscript = TRUE;
    }
   }
   else
   {
    if (ButtonActivSubscript)
    {
     XmToggleButtonSetState(WIDGET(IconFon_7), False, False);
     F(MenuFor).ToggleButton(MenuFor, MFor_SUBSCRIPT, False);
     ButtonActivSubscript = FALSE;
    }
   }
   
   if ((attributes & XcTS_B_SUPSCRIPT) == XcTS_B_SUPSCRIPT)
   {
    if (!ButtonActivSupscript)
    {
     XmToggleButtonSetState(WIDGET(IconFon_8), True, False);
     F(MenuFor).ToggleButton(MenuFor, MFor_SUPSCRIPT, True);
     ButtonActivSupscript = TRUE;
    }
   }
   else
   {
    if (ButtonActivSupscript)
    {
     XmToggleButtonSetState(WIDGET(IconFon_8), False, False);
     F(MenuFor).ToggleButton(MenuFor, MFor_SUPSCRIPT, False);
     ButtonActivSupscript = FALSE;
    }
   }
   
   if ((attributes & XcTS_B_SHADOW) == XcTS_B_SHADOW)
   {
    if (!ButtonActivShadow)
    {
     XmToggleButtonSetState(WIDGET(IconFon_9), True, False);
     F(MenuFor).ToggleButton(MenuFor, MFor_SHADOW, True);
     ButtonActivShadow = TRUE;
    }
   }
   else
   {
    if (ButtonActivShadow)
    {
     XmToggleButtonSetState(WIDGET(IconFon_9), False, False);
     F(MenuFor).ToggleButton(MenuFor, MFor_SHADOW, False);
     ButtonActivShadow = FALSE;
    }
   }
   
   if ((attributes & XcTS_B_SMALL_CAPS) == XcTS_B_SMALL_CAPS)
   {
    if (!ButtonActivSmallCaps)
    {
     XmToggleButtonSetState(WIDGET(IconFon_11), True, False);
     F(MenuFor).ToggleButton(MenuFor, MFor_SMALLCAPS, True);
     ButtonActivSmallCaps = TRUE;
    }
   }
   else
   {
    if (ButtonActivSmallCaps)
    {
     XmToggleButtonSetState(WIDGET(IconFon_11), False, False);
     F(MenuFor).ToggleButton(MenuFor, MFor_SMALLCAPS, False);
     ButtonActivSmallCaps = FALSE;
    }
   }
   
   if ((attributes & XcTS_B_BIG_CAPS) == XcTS_B_BIG_CAPS)
   {
    if (!ButtonActivBigCaps)
    {
     F(MenuFor).ToggleButton(MenuFor, MFor_BIGCAPS, True);
     ButtonActivBigCaps = TRUE;
    }
   }
   else
   {
    if (ButtonActivBigCaps)
    {
     F(MenuFor).ToggleButton(MenuFor, MFor_BIGCAPS, False);
     ButtonActivBigCaps = FALSE;
    }
   }

   if (FontSizeValue != text->style->point_size)
   {
    FontSizeValue = text->style->point_size;
    F(tfFontSize).set_value_in_unit(tfFontSize,
				    SCALE_TO_POINTS(FontSizeValue));
   }
   
   
   /* ----------------------------------------------------------------- ** 
   ** Ruler Bar Activation                                              ** 
   ** ----------------------------------------------------------------- */
   
   justification = text->ruler->justification;
   
   if (justification == XcTR_LEFT)
   {
    if (!ButtonActivJLeft)
    {
     XmToggleButtonSetState(WIDGET(IconRul_1), True, False);
     ButtonActivJLeft = TRUE;
     store_function_ruler = SET_JLEFT;
    }
   }
   else
   {
    if (ButtonActivJLeft)
    {
     XmToggleButtonSetState(WIDGET(IconRul_1), False, False);
     ButtonActivJLeft = FALSE;
    }
   }
   
   if (justification == XcTR_CENTER)
   {
    if (!ButtonActivJCenter)
    {
     XmToggleButtonSetState(WIDGET(IconRul_2), True, False);
     ButtonActivJCenter = TRUE;
     store_function_ruler = SET_JCENTER;
    }
   }
   else
   {
    if (ButtonActivJCenter)
    {
     XmToggleButtonSetState(WIDGET(IconRul_2), False, False);
     ButtonActivJCenter = FALSE;
    }
   }
   
   if (justification == XcTR_RIGHT)
   {
    if (!ButtonActivJRight)
    {
     XmToggleButtonSetState(WIDGET(IconRul_3), True, False);
     ButtonActivJRight = TRUE;
     store_function_ruler = SET_JRIGHT;
    }
   }
   else
   {
    if (ButtonActivJRight)
    {
     XmToggleButtonSetState(WIDGET(IconRul_3), False, False);
     ButtonActivJRight = FALSE;
    }
   }
   
   if (justification == XcTR_JUSTIFIED)
   {
    if (!ButtonActivJustify)
    {
     XmToggleButtonSetState(WIDGET(IconRul_4), True, False);
     ButtonActivJustify = TRUE;
     store_function_ruler = SET_JUSTIFY;
    }
   }
   else
   {
    if (ButtonActivJustify)
    {
     XmToggleButtonSetState(WIDGET(IconRul_4), False, False);
     ButtonActivJustify = FALSE;
    }
   }
  }
  else				/* FRAME SELECTION */
  {
   if (!DocSelected->PageArea->SelectedPage->frmg->SelectedCadre)
   {
    SensitivizeSText(False);
    SensitivizeSImage(False);
    SensitivizeSVector(False);
    SensitivizeFrameTools(False);
    SensitivizeMultiFrameTools(False);
    SensitivizeLockTools(False);
   }
   else				/* AT LEAST 1 FRAME is SELECTED */
   {
    SensitivizeLockTools(True);
   
    if (store_mode_textflow != (int)(DocSelected->PageArea->SelectedPage->frmg
				     ->SelectedCadre->cadre->textflow_mode
				     & XcTFM_HOLE_MASK))
    {
     i = 0;
     while ((toggletextflowmodetable[i].mode_num != store_mode_textflow)
	    && 
	    (toggletextflowmodetable[i].tog_wid != 0))
      i++;
     if (toggletextflowmodetable[i].tog_wid != 0)
     {
      XmToggleButtonSetState
       (toggletextflowmodetable[i].tog_wid,
	False, False);
      XmToggleButtonSetState
       (toggletextflowmodetable[i].tog_wid2,
	False, False);
     }
     store_mode_textflow = DocSelected->PageArea->SelectedPage->frmg
      ->SelectedCadre->cadre->textflow_mode & XcTFM_HOLE_MASK;
     
     i = 0;
     while ((toggletextflowmodetable[i].mode_num != store_mode_textflow)
	    && 
	    (toggletextflowmodetable[i].tog_wid != 0))
      i++;
     if (toggletextflowmodetable[i].tog_wid != 0)
     {
      XmToggleButtonSetState(toggletextflowmodetable[i].tog_wid,
			     True, False);	
      XmToggleButtonSetState(toggletextflowmodetable[i].tog_wid2,
			     True, False);	
     }
    }

    if (DocSelected->PageArea->SelectedPage->frmg
	->SelectedCadre->cadre->textflow_mode > XcTFM_HOLE_MASK)
    {
     if (ButtonHoleOn)
     {
      ButtonHoleOn = FALSE;
      XmToggleButtonSetState(WIDGET(IconFra_19), True, False);
      XmToggleButtonSetState(MenuFor->w_Buttons[MFor_TEXTFLOWOUTHOLE],
			     True, False);
     }
    }
    else
    {
     if (!ButtonHoleOn)
     {
      ButtonHoleOn = TRUE;
      XmToggleButtonSetState(WIDGET(IconFra_19), False, False);
      XmToggleButtonSetState(MenuFor->w_Buttons[MFor_TEXTFLOWOUTHOLE],
			     False, False);
     }
    }

    if (DocSelected->PageArea->SelectedPage->frmg->verrou)/* Frame is locked */
    {
     if (ButtonLockFrame)
     {
      ButtonLockFrame = FALSE;
      F(MenuFor).ToggleButton(MenuFor, MFor_LOCK, True);
      XmToggleButtonSetState(WIDGET(IconFra_12), True, False);
     }
     SensitivizeFrameTools(False);
    }
    else			/* Frame is not locked */
    {
     if (!ButtonLockFrame)
     {
      ButtonLockFrame = TRUE;
      F(MenuFor).ToggleButton(MenuFor, MFor_LOCK, False);
      XmToggleButtonSetState(WIDGET(IconFra_12), False, False);
     }
     SensitivizeFrameTools(True);
    }
   
    if (DocSelected->PageArea->SelectedPage->frmg->
	SelectedCadre->NextCadre)/* AT LEAST 2 FRAMES are SELECTED */
    {
     SensitivizeSText(False);
     SensitivizeSImage(False);
     SensitivizeSVector(False);
    }   
    else			/* ONLY 1 FRAME is SELECTED */
    {

     if (DocSelected->PageArea->SelectedPage->frmg->
	 SelectedCadre->cadre->object_type == CADRE_TEXT)
      /* SELECTED CONTENT is TEXT */
      SensitivizeSText(True);
     else
      SensitivizeSText(False); 
    
     if (DocSelected->PageArea->SelectedPage->frmg->
	 SelectedCadre->cadre->object_type == CADRE_VECTOR)
      /* SELECTED CONTENT is VECTOR */
     {
      ToggleSetImage(CADRE_VECTOR,
		     ((c_VectorGraph *)(DocSelected->PageArea->SelectedPage
					->frmg->SelectedCadre->cadre
					->object))->mode);

      SensitivizeSImage(True);
      SensitivizeSVector(True);
      if (((c_VectorGraph *)(DocSelected->PageArea->SelectedPage->frmg->
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
     {
      SensitivizeSVector(False);
      if (DocSelected->PageArea->SelectedPage->frmg->
	  SelectedCadre->cadre->object_type == CADRE_IMAGE_BITMAP)
       /* SELECTED CONTENT is IMAGE */
      {
       ToggleSetImage(CADRE_IMAGE_BITMAP,
		      ((c_Image *)(DocSelected->PageArea->SelectedPage
				   ->frmg->SelectedCadre->cadre
				   ->object))->zoom_type);
       SensitivizeSImage(True);
      }
      else
       SensitivizeSImage(False);
     }
    }
   }  
  }
 }
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
 
 app = XtWidgetToApplicationContext(w);
 XSync(XtDisplay(w), 0);
 
 while(XtAppPending(app))
 {
  XtAppNextEvent(app, &event);
  func(&event);
 }
}

/* ----------------------------------------------------------------- ** 
** Signal_Error_Handler - Error Handlers for system signals          ** 
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
     sprintf(filename, "Crash_svg_%03d.xa", i++); 
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
  Xc_TRACE(("Argh !!  Error in signal handler"));
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
** IO_Error_Handler - Error Handler for X server IO Error            ** 
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
  GlobColormap = (c_Colormap *)NEW(c_Colormap)(display, (Widget)0,
					       FALSE, 0, NO_FORCE);
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

