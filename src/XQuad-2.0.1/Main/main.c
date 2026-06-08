/*
** main.c for XQuad in Main/
** Main part of XQuad (Axene's Spreadsheet)
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
** Started on  Wed Jul 19 14:00:28 1995 Emmanuel Paris
** Last update Fri Feb  4 11:39:53 2000 Emmanuel Paris
*/

#define __XC_MAIN__

#define SIGNALH
#define IOERRORH
#include "xcalibur.h"

#include "XErrorHandler.h"
#include "SignalHandler.h"

#include "WInterface.h"
#define _PRIVATE_PAGEFORMAT_
#include "PageFormat.h"

#include "../Pixmaps/cadre.tablor.zpm"
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

#include "../Pixmaps/cadre.creer.tablor.zpm"
#include "../Pixmaps/cadre.selectionner.tablor.zpm"
#include "../Pixmaps/cadre.eloigner.zpm"
#include "../Pixmaps/cadre.approcher.zpm"
#include "../Pixmaps/cadre.premierplan.zpm"
#include "../Pixmaps/cadre.arriereplan.zpm"
#include "../Pixmaps/cadre.lock.zpm"

#include "../Pixmaps/graph.aires.28.zpm"
#include "../Pixmaps/graph.barres.28.zpm"
#include "../Pixmaps/graph.courbes.28.zpm"
#include "../Pixmaps/graph.histogrammes.28.zpm"
#include "../Pixmaps/graph.secteurs.28.zpm"
#include "../Pixmaps/graph.radar.28.zpm"
#include "../Pixmaps/conf.axes.zpm"
#include "../Pixmaps/conf.titre.zpm"
#include "../Pixmaps/conf.legende.zpm"
#include "../Pixmaps/conf.abscisse.zpm"
#include "../Pixmaps/conf.sstitres.zpm"
#include "../Pixmaps/conf.config.zpm"

#include "../Pixmaps/cell.bords.zpm"
#include "../Pixmaps/cell.bord.clean.zpm"
#include "../Pixmaps/cell.bord.bas.zpm"
#include "../Pixmaps/cell.bord.haut.zpm"
#include "../Pixmaps/cell.bord.gauche.zpm"
#include "../Pixmaps/cell.bord.droite.zpm"
#include "../Pixmaps/cell.bord.contours.zpm"
#include "../Pixmaps/background.default.zpm"
#include "../Pixmaps/cell.inversevideo.zpm"

#include "../Pixmaps/cell.zpm"
#include "../Pixmaps/style.gauche.zpm"
#include "../Pixmaps/style.centre.zpm"
#include "../Pixmaps/style.droite.zpm"
#include "../Pixmaps/style.haut.zpm"
#include "../Pixmaps/style.bas.zpm"
#include "../Pixmaps/style.vjustif.zpm"
#include "../Pixmaps/style.justifie.zpm"
#include "../Pixmaps/style.multilignes.zpm"
#include "../Pixmaps/style.angle90.zpm"
#include "../Pixmaps/style.angle270.zpm"

#include "../Pixmaps/calcul.zpm"
#include "../Pixmaps/equal.zpm"
#include "../Pixmaps/fonction.somme.zpm"
#include "../Pixmaps/style.milliers.zpm"
#include "../Pixmaps/style.monetaire.zpm"
#include "../Pixmaps/style.pourcentage.zpm"
#include "../Pixmaps/style.scientific.zpm"
#include "../Pixmaps/style.engineer.zpm"
#include "../Pixmaps/decimal.less.zpm"
#include "../Pixmaps/decimal.more.zpm"

#include "../Pixmaps/coller.fonction.zpm"

#include "../Pixmaps/fonts.zpm"
#include "../Pixmaps/texte.style.gras.zpm"
#include "../Pixmaps/texte.style.italique.zpm"
#include "../Pixmaps/texte.style.underline.zpm"
#include "../Pixmaps/texte.style.stroke.zpm"
#include "../Pixmaps/texte.style.shadow.zpm"
#include "../Pixmaps/texte.style.smallcaps.zpm"
#include "../Pixmaps/texte.style.bigcaps.zpm"

#include "../Pixmaps/loupe.zpm"
#include "../Pixmaps/loupe.arriere.zpm"
#include "../Pixmaps/loupe.avant.zpm"
#include "../Pixmaps/loupe.50.zpm"
#include "../Pixmaps/loupe.75.zpm"
#include "../Pixmaps/loupe.100.zpm"
#include "../Pixmaps/loupe.200.zpm"
#include "../Pixmaps/loupe.400.zpm"

#include "../Pixmaps/xquad2.zpm"
#include "../Pixmaps/small_column.zpm"
#if defined(Xc_REGISTER_LEVEL) &&  Xc_REGISTER_LEVEL > 0
#include "../Pixmaps/small_columnu.zpm"
#endif

#include "Label.h"
#include "MainInterface.h"
#include "TimeIndicator.h"
#include "MenuBar.h"
#include "PulldownMenu.h"
#include "ManageWidget.h"
#include "RowColumn.h"
#include "PanedWindow.h"
#include "DrawingArea.h"
#include "Frame.h"
#include "Form.h"
#include "WinManager.h"
#include "RegisterHelp.h"
#include "RemoteServer.h"
#include "HelpLine.h"
#include "Resource.h"
#include "Icon.h"
#include "WTextField.h"
#include "ItemMenu.h"
#include "SpinBox.h"
#include "ColorList.h"
#include "EditBar.h"

#include "Box_bordures.h"
#include "Box_fond.h"
#include "Box_about.h"
#include "Box_legal.h"
#include "Box_legal2.h"
#include "Box_registration.h"
#include "Box_color.h"
#include "Box_nombres.h"
#include "Box_colsize.h"
#include "Box_styles.h"
#include "Box_miseenpage.h"
#include "Box_imprimer.h"
#include "Box_functions.h"
#include "Box_gotocell.h"
#include "AlertBox.h"

#include "Colormap.h"
#include "Cursor.h"
#include "Drag_And_Drop.h"
#include "TrashCan.h"

#include "DocumentList.h"
#include "DocIcon.h"
#include "Tableau.h"
#include "ScrollBarX.h"
#include "High_Graph.h"
#include "ImageManager.h"
#include "FontManager.h"
#include "PostScript.h"
#include "VectorFont.h"
#include "Color.h"
#include "Import.h"
#include "Export.h"
#include "DocFile.h"
#include "check.h"
#include "config.h"
#include "SpecialFileSelector.h"
#include "Readkey.h"

c_XErrorHandler	*XErrH;
int IOErrorCount = 0;

#ifdef SIGNALH
c_SignalHandler	*SignalH;
jmp_buf SigErrorEnv;
#endif
int SigErrorCount = 1;

c_WinManager	 *Gwm;

char	*defaultDocumentName;
char	*defaultFrameName;

c_DocumentList	*DocList;

c_MainInterface  *MainInterface1;
Widget wMainWindow;

c_DrawingArea	*MainDocument;

c_Icon		**Icons_ref;

c_MenuBar      *Bar1;
c_PulldownMenu *MenuFic;
c_PulldownMenu *MenuEdi;
c_PulldownMenu *MenuSel;
c_PulldownMenu *MenuAff;
c_PulldownMenu *MenuFor;
c_PulldownMenu *MenuGra;
c_PulldownMenu *MenuFen;
c_PulldownMenu *MenuAid;

c_ManageWidget *Manager1;
c_ManageWidget *Manager2;

enum
{
 HBAR_MISC,  HBAR_CALCUL, HBAR_FONT, HBAR_CELL,
 HBAR_BORDER, HBAR_GRAPH, HBAR_ZOOM,
 NB_HBAR
};

c_Form          *vIconBar;
c_RowColumn	*vIconBar_1;
c_RowColumn	*vIconBar_2;
Widget    	w_vIconBar_3;
c_Icon		*vIcon[NB_HBAR];
int		bar_selected = HBAR_CALCUL;

c_Icon          *IconVlogo;
c_Form          *vIconBarlogo;

c_Form          *hIconBarForm;
c_RowColumn	*hIconBar[NB_HBAR];

c_Form          *BottomBar;

c_Form          *RightBar;
Widget          w_PositionLabel;

c_EditBar	*GlobEditBar;

c_Icon *IconMis_1, *IconMis_2; /* open, new */
c_Icon *IconMis_s1, *IconMis_3, *IconMis_4; /* save print */
c_Icon *IconMis_s2, *IconMis_5, *IconMis_6, *IconMis_7; /* cut/copy/paste */
c_Icon *IconMis_8; /* del*/


c_Icon *IconCal_15, *IconCal_6, *IconCal_7, *IconCal_s3, *IconCal_8;
c_Icon *IconCal_9, *IconCal_10, *IconCal_11, *IconCal_12, *IconCal_s4;
c_Icon *IconCal_13, *IconCal_14;

c_ItemMenu *IMfontFamily;
c_ColorList *CLfontColor;
c_Icon *IconFon_s1;
c_Icon *IconFon_3; /* bold */
c_Icon *IconFon_4; /* italic */
c_Icon *IconFon_5; /* underline */
c_Icon *IconFon_6; /* strikeout */
/*c_Icon *IconFon_7;  subscript*/
/*c_Icon *IconFon_8;  supscript*/
c_Icon *IconFon_9; /* shadow */
/*c_Icon *IconFon_10;  outline*/
c_Icon *IconFon_11; /* small_caps */
c_Icon *IconFon_12; /* big_caps */
c_Icon *IconFon_s2;
c_SpinBox  *sbFontSize;
c_WTextField *tfFontSize;
c_Icon *IconFon_s3;

c_Icon *IconCel_1;
c_Icon *IconCel_2;
c_Icon *IconCel_3;
c_Icon *IconCel_4;
c_Icon *IconCel_5;
/*c_Icon *IconCel_7;*/
c_Icon *IconCel_8;
/*c_Icon *IconCel_9;*/
c_Icon *IconCel_10;
c_Icon *IconCel_11;
/*c_Icon *IconCel_12;*/
c_Icon *IconCel_13;
c_Icon *IconCel_14;
c_Icon *IconCel_s1;
c_Icon *IconCel_s2;
c_Icon *IconCel_s3;
/*c_Icon *IconCel_s4;*/

c_Icon *IconBor_1;
c_Icon *IconBor_s1;
c_Icon *IconBor_2;
c_Icon *IconBor_3;
c_Icon *IconBor_4;
c_Icon *IconBor_5;
c_Icon *IconBor_6;
c_Icon *IconBor_s2;
c_ColorList *CLfillColor;
c_Icon *IconBor_s3;
c_Icon *IconBor_7;
c_Icon *IconBor_8;

c_Icon *IconGra_0;
c_Icon *IconGra_1;
c_Icon *IconGra_s1;
c_Icon *IconGra_2;
c_Icon *IconGra_3;
c_Icon *IconGra_4;
c_Icon *IconGra_5;
c_Icon *IconGra_s2;
c_Icon *IconGra_6;
c_Icon *IconGra_7;
c_Icon *IconGra_8;
c_Icon *IconGra_9;
c_Icon *IconGra_10;
c_Icon *IconGra_11;
c_Icon *IconGra_s3;
c_Icon *IconGra_12;
c_Icon *IconGra_13;
c_Icon *IconGra_14;
c_Icon *IconGra_16;
c_Icon *IconGra_17;
c_Icon *IconGra_15;
c_Icon *IconGra_s4;
c_Icon *IconGra_18;

c_Icon *IconZoo_1;
c_Icon *IconZoo_2;
c_Icon *IconZoo_3;
c_Icon *IconZoo_4;
c_Icon *IconZoo_5;
c_Icon *IconZoo_6;
c_Icon *IconZoo_7;
c_Icon *IconZoo_s1;

c_Icon *IconTra;

c_Icon *IconBottom;

/*c_ItemMenu *IMstyles;*/

c_RowColumn *RightRC;

boolean ButtonLockFrame = FALSE;
boolean ButtonMaskFrame = TRUE;
boolean ButtonStopMagnetisation = FALSE;
boolean ButtonMaskGrid = TRUE;
boolean ButtonMaskCol = TRUE;
boolean ButtonMaskRow = TRUE;
boolean ButtonMaskPageMark = FALSE;
boolean ButtonMaskFormula = FALSE;
boolean ButtonMaskZeroCell = TRUE;
boolean ButtonMaskBottomBar = TRUE;
boolean ButtonDeleteFrame = FALSE;
boolean ButtonMaskAxes = FALSE;
boolean ButtonMaskTitle = TRUE;
boolean ButtonMaskLegend = TRUE;
boolean ButtonMaskAbscisse = TRUE;
boolean ButtonMaskOrdonnee = TRUE;
boolean ButtonActivBold = FALSE;
boolean ButtonActivItalic = FALSE;
boolean ButtonActivUnderline = FALSE;
boolean ButtonActivStrikeout = FALSE;
boolean ButtonActivShadow = FALSE;
boolean ButtonActivSmallCaps = FALSE;
boolean ButtonActivBigCaps = FALSE;
boolean ButtonActivBorderLeft = FALSE;
boolean ButtonActivBorderRight = FALSE;
boolean ButtonActivBorderTop = FALSE;
boolean ButtonActivBorderBottom = FALSE;
boolean ButtonActivHAlignLeft = FALSE;
boolean ButtonActivHAlignRight = FALSE;
boolean ButtonActivHAlignCenter = FALSE;
boolean ButtonActivHAlignJustify = FALSE;
boolean ButtonActivVAlignTop = FALSE;
boolean ButtonActivVAlignBottom = FALSE;
boolean ButtonActivVAlignJustify = FALSE;
boolean ButtonActivMultiLine = FALSE;
boolean ButtonActivAngle90 = FALSE;
boolean ButtonActivAngle270 = FALSE;
boolean ButtonActivThousands = FALSE;
boolean ButtonActivCurrency = FALSE;
boolean ButtonActivPercent = FALSE;
boolean ButtonActivScientific = FALSE;
boolean ButtonActivEngineer = FALSE;

coord_t FontSizeValue = COORD_ZERO;

boolean	DocAlreadyGrised = FALSE;
boolean SelectFramesAlreadyGrised = FALSE;
boolean	FrameToolsAlreadyGrised = FALSE;
boolean	GraphToolsAlreadyGrised = FALSE;
boolean PasteAlreadyGrised = FALSE;
boolean CopyDownAlreadyGrised = FALSE;
boolean CopyRightAlreadyGrised = FALSE;
boolean SimpleSelectAlreadyGrised = FALSE;
boolean CutCopyAlreadyGrised = FALSE;
boolean DeleteAlreadyGrised = FALSE;
boolean RowAlreadyGrised = FALSE;
boolean ColAlreadyGrised = FALSE;
boolean TitleConfigAlreadyGrised = FALSE;
boolean AxesConfigAlreadyGrised = FALSE;
boolean LegendConfigAlreadyGrised = FALSE;
boolean AbscisseConfigAlreadyGrised = FALSE;
boolean OrdonneeConfigAlreadyGrised = FALSE;
boolean GraphFrameToolsAlreadyGrised = FALSE;
boolean BoldAlreadyGrised = FALSE;
boolean ItalicAlreadyGrised = FALSE;

#ifdef HAVE_XSHM
#include <X11/extensions/XShm.h>
extern Bool XShmQueryExtension();
boolean xshm_flag;
#endif

#ifdef HAVE_XSHAPE
#include <X11/extensions/shape.h>
boolean xshape_flag;
#endif

extern void init ___PROTO((long parisize, long maxprime));

boolean	motif_focus = FALSE, tffs_focus = FALSE;	
boolean	handle_event_allowed = TRUE;
Time	last_iclick = 0;

c_TimeIndicator  *TimeIndicator;
#define TI_NB_LEDS 17

c_RegisterHelp	*GlobRegHelp;
c_RemoteServer	*GlobRemoteServer;
c_HelpLine      *GlobHelpLine;
c_Colormap 	*GlobColormap;
c_GCursor  	*GlobCursor = NULL;
c_DragAndDrop	*GlobDragAndDrop;
c_Trash		*GlobTrash;
c_VectorFont	*GlobFontBase = NULL;
c_Color		*GlobColorBase = NULL;
c_Color		*GlobColorAuto = NULL;
c_FormatNbr	*GlobFormatBase = NULL;
c_TextStyle	*GlobTextStyleBase = NULL;
c_Resource	*GlobResources;
c_DocIcon	*GlobDocIcon;
c_ImageManager	*GlobImgMng;
c_FontManager	*GlobFntMng;
printer_t	*GlobPrinterBase = NULL;
c_Import	*GlobImport;
c_Export	*GlobExport;
char		*t1imager_buffer = NULL;
char		*Globprog_name = NULL;

print_param_t	GlobPrintParam = 
{
 NULL,
 1, 1, 10000, 0, TRUE, FALSE, FALSE, TRUE
};


/* store the last toggle function*/
int  store_function;
int  store_function_type;

typedef struct 
{
 Widget	tog_wid;
 int		func_num;
} tog_tab;

typedef struct 
{
 Widget	tog_wid;
 int		func_num;
 int		mode_num;
} tog_tab2;

tog_tab   toggletable[10];
tog_tab2  typetoggletable[10];

void SensitivizeDoc();
void SensitivizeFrameTools();
void SensitivizeGraphTools();
void SensitivizeSelectFrames();
void SensitivizePaste();
void SensitivizeCopyDown();
void SensitivizeCopyRight();
void SensitivizeSimpleSelect();
void SensitivizeCutCopy();
void SensitivizeRow();
void SensitivizeCol();
void SensitivizeTitleConfig();
void SensitivizeAxesConfig();
void SensitivizeLegendConfig();
void SensitivizeAbscisseConfig();
void SensitivizeOrdonneeConfig();
void SensitivizeGraphFrameTools();
void SensitivizeBold();
void SensitivizeItalic();

void DisplayBottomBar();

void Mod_structure();
void MenuFileNew();
void MenuFileOpen();
void MenuFileClose();
void MenuFileSave();
void MenuFileSaveas();
void MenuFileSaveall();
void MenuFileAcquireDoc();
void MenuFileExportVector();
void MenuFileExportText();
void MenuFileExportHTML();
void MenuFilePrintSetup();
void MenuFilePrint();
void MenuFileQuit();
void MenuEditColors();
void NewTableau();
void PastSummFunction();
void QuitApplication();
static void AttachHBar();
static void SelectHBarCallback();

void ToggleSwitch();
void ToggleSwitchType();
void SelectFunction();

void Signal_Error_Handler();
void IO_Error_Handler();

void MenuArrangeWindow();
void MenuTileWindow();
void SChangeWindow();
void ChangeDocCallback();
void XcHandleEvent ___PROTO((XEvent *event));
void XcFlushEvents ___PROTO((Widget w, void (*func) ___PROTO((XEvent *))));

void MenuHelpCallHelp();
void MenuHelpTrackHelp();
void MenuHelpAbout();
static void MenuHelpLegal();
void MenuHelpRegistration();
void MenuFormatNombres();
void MenuSelectFonctions();
void MenuSelectGotoCell();
void MenuFormatMotifs();
void MenuFormatEncadrements();
void MenuFormatLargeurHauteur();
void MenuFormatPolices();

static void change_font ___PROTO((c_ItemMenu *This,
				  void *Param,
				  family_tree *family));
static void change_font_color ___PROTO((c_ColorList *This,
					void *Param, c_Color *color));
static void change_fill_color ___PROTO((c_ColorList *This,
					void *Param, c_Color *color));
static void certify_font_size ___PROTO((c_WTextField *This, void *param));

static void cb_focus_motif ___PROTO((Widget w, void *none,
				     XmRowColumnCallbackStruct *cbs));
static void cb_unfocus_motif ___PROTO((Widget w, void *none,
				       XmRowColumnCallbackStruct *cbs));
static void cb_focus_tfFontSize ___PROTO((Widget w, c_WTextField *This, 
					  XEvent *event));
static void cb_unfocus_tfFontSize ___PROTO((Widget w, c_WTextField *This, 
					    XEvent *event));

family_tree tmp_family = 
{ 
 NULL, "             Fonts List             ", 
 NULL, FALSE, FALSE, FALSE, FALSE
};

static family_tree	*saved_family;

int main(argc, argv)
int  argc;
char **argv;
{
 Display *display;
 Arg     args[10];
 int     nargs;
 int	  i;
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

 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Creating main Interface");
 F(TimeIndicator).advance_indicator(TimeIndicator);
  
 Manager2 = (c_ManageWidget *)NEW(c_ManageWidget)();

 GlobFntMng = (c_FontManager *)NEW(c_FontManager)(display);
  
 GlobEditBar = (c_EditBar *)NEW(c_EditBar)(WIDGET(MainInterface1), Manager1,
					   "ebGlobalEditBar");
  
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
					  "IconMis", display,
					  misc_tools_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(vIcon[HBAR_MISC]), FUNC_VALUE, 
	  SelectHBarCallback, (void *)HBAR_MISC);
 
 vIcon[HBAR_CALCUL] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1, 
					    "IconCal", display,
					    calcul_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(vIcon[HBAR_CALCUL]), FUNC_VALUE, 
	  SelectHBarCallback, (void *)HBAR_CALCUL);
    
 vIcon[HBAR_FONT] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1,
					  "IconFon", display,
					  fonts_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(vIcon[HBAR_FONT]), FUNC_VALUE, 
	  SelectHBarCallback, (void *)HBAR_FONT);

 vIcon[HBAR_CELL] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1,
					  "IconCel", display,
					  cell_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(vIcon[HBAR_CELL]), FUNC_VALUE, 
	  SelectHBarCallback, (void *)HBAR_CELL);
 
 vIcon[HBAR_BORDER] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1,
					    "IconBor", display,
					    cell_bords_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(vIcon[HBAR_BORDER]), FUNC_VALUE,
	  SelectHBarCallback, (void *)HBAR_BORDER);
 
 vIcon[HBAR_GRAPH] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1, 
					   "IconGra", display,
					   cadre_tablor_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(vIcon[HBAR_GRAPH]), FUNC_VALUE, 
	  SelectHBarCallback, (void *)HBAR_GRAPH);
  
 vIcon[HBAR_ZOOM] = (c_Icon *)NEW(c_Icon)(WIDGET(vIconBar_1), Manager1, 
					  "IconZoo", display,
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
					      "vIconBar2", 32, 32,
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


 F(TimeIndicator).advance_indicator(TimeIndicator);
 
 /* ----------------------------------------------------------------- ** 
 ** Horizontal Icon Bars                                              ** 
 ** ----------------------------------------------------------------- */
 hIconBarForm = (c_Form *)NEW(c_Form)(WIDGET(MainInterface1), Manager1,
				  "hIconBar", 32, 32);
 for (i = 0; i < NB_HBAR; i++)
 {
  hIconBar[i] = 
   (c_RowColumn *)NEW(c_RowColumn)(WIDGET(hIconBarForm), NULL,
				   i == HBAR_FONT ? "rchIconBarFont" :
				   "rchIconBar", 0, 34, XmHORIZONTAL);
 }  
  
 store_function = -1;
 store_function_type = -1;

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
   if (check_key_validity(&gkinfo, 31411,
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
	  (void *)DELETE_SELECTION);

 IconMis_s2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				    "IconMis_s2", display,
				    NULL, NULL, XcSeparator);

 IconMis_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_5", display,
				   couper_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconMis_5), FUNC_ACTIVATE, SelectFunction, 
	  (void *)CUT_REGION);

 IconMis_6 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_6", display,
				   copier_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconMis_6), FUNC_ACTIVATE, SelectFunction, 
	  (void *)COPY_REGION);

 IconMis_7 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_MISC]), Manager2,
				   "IconMis_7", display,
				   coller_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconMis_7), FUNC_ACTIVATE, SelectFunction, 
	  (void *)PASTE_REGION);


 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Creating Icon bar CALCUL");


 /* ----------------------------------------------------------------------- */
 /* --                           HBAR_CALCUL                                */
 /* ----------------------------------------------------------------------- */
  
/*
 IconCal_1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				   "IconCal_1", display,
				   document_save_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconCal_1), FUNC_ACTIVATE, MenuFileSave, NULL);
 IconCal_2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				   "IconCal_2", display,
				   document_imprimer_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconCal_2), FUNC_ACTIVATE, MenuFilePrint, NULL);
 IconCal_s1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				    "IconCal_s1", display,
				    NULL, NULL, XcSeparator);
 IconCal_3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				   "IconCal_3", display,
				   couper_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconCal_3), FUNC_ACTIVATE,
	  SelectFunction, (void *)CUT_REGION);
 IconCal_4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				   "IconCal_4", display,
				   copier_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconCal_4), FUNC_ACTIVATE,
	  SelectFunction, (void *)COPY_REGION);
 IconCal_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				   "IconCal_5", display,
				   coller_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconCal_5), FUNC_ACTIVATE, 
	  SelectFunction, (void *)PASTE_REGION);
 IconCal_s2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				    "IconCal_s2", display,
				    NULL, NULL, XcSeparator);
*/
 IconCal_15 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				    "IconCal_15", display,
				    equal_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconCal_15), FUNC_ACTIVATE,
	  F(GlobEditBar).equal_callback, (void *)GlobEditBar);
 IconCal_7 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				   "IconCal_7", display,
				   coller_fonction_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconCal_7), FUNC_ACTIVATE, MenuSelectFonctions, NULL);
 IconCal_6 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				   "IconCal_6", display,
				   fonction_somme_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconCal_6), FUNC_ACTIVATE, PastSummFunction, NULL);

 IconCal_s3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				    "IconCal_s3", display,
				    NULL, NULL, XcSeparator);
 IconCal_8 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				   "IconCal_8", display,
				   style_milliers_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCal_8), FUNC_VALUE, 
	  SelectFunction, (void *)THOUSANDS_FORMAT);
 IconCal_9 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				   "IconCal_9", display,
				   style_monetaire_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCal_9), FUNC_VALUE, 
	  SelectFunction, (void *)CURRENCY_FORMAT);
 IconCal_10 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				    "IconCal_10", display,
				    style_pourcentage_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCal_10), FUNC_VALUE, 
	  SelectFunction, (void *)PERCENT_FORMAT);
 IconCal_11 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				    "IconCal_11", display,
				    style_scientific_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCal_11), FUNC_VALUE, 
	  SelectFunction, (void *)SCIENTIFIC_FORMAT);
 IconCal_12 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				    "IconCal_12", display,
				    style_engineer_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCal_12), FUNC_VALUE, 
	  SelectFunction, (void *)ENGINEER_FORMAT);

 IconCal_s4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				    "IconCal_s4", display,
				    NULL, NULL, XcSeparator);
 IconCal_13 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				    "IconCal_13", display,
				    decimal_more_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconCal_13), FUNC_ACTIVATE, 
	  SelectFunction, (void *)FORMAT_ADD_DIGIT);
 IconCal_14 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CALCUL]), Manager2,
				    "IconCal_14", display,
				    decimal_less_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconCal_14), FUNC_ACTIVATE, 
	  SelectFunction, (void *)FORMAT_SUB_DIGIT);

 F(TimeIndicator).advance_indicator(TimeIndicator);
  
 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Creating Icon bar FONT");

 /* ----------------------------------------------------------------------- */
 /* --                             HBAR_FONT                                */
 /* ----------------------------------------------------------------------- */

 IMfontFamily = NEW(c_ItemMenu)(WIDGET(hIconBar[HBAR_FONT]),
				Manager2,
				"imFontFamily",
				0, 0);
 XtVaSetValues(WIDGET(IMfontFamily), XmNmarginHeight, 5,
	       XmNmarginWidth, 3, NULL);
  
 F(IMfontFamily).set(IMfontFamily,
		     TRUE, offsetof(family_tree, name),
		     NULL, NULL);

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
 IconFon_6 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				   "IconFon_6", display,
				   texte_style_stroke_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFon_6), FUNC_VALUE, SelectFunction,
	  (void *)SET_STRIKEOUT);

 IconFon_9 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				   "IconFon_9", display,
				   texte_style_shadow_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFon_9), FUNC_VALUE, SelectFunction,
	  (void *)SET_SHADOW);

 IconFon_s2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				    "IconFon_s2", display,
				    NULL, NULL, XcSeparator);

 IconFon_11 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				    "IconFon_11", display,
				    texte_style_smallcaps_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFon_11), FUNC_VALUE, SelectFunction,
	  (void *)SET_SMALL_CAPS);
 IconFon_12 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				    "IconFon_12", display,
				    texte_style_bigcaps_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconFon_12), FUNC_VALUE, SelectFunction,
	  (void *)SET_BIG_CAPS);
  

 IconFon_s3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				    "IconFon_s3", display,
				    NULL, NULL, XcSeparator);

 CLfontColor = NEW(c_ColorList)(WIDGET(hIconBar[HBAR_FONT]), Manager2,
				"clFontColor", XcCL_FONT);
  

 F(TimeIndicator).advance_indicator(TimeIndicator);
  
 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Creating Icon bar CELL");
 /* ----------------------------------------------------------------------- */
 /* --                             HBAR_CELL                                */
 /* ----------------------------------------------------------------------- */

 IconCel_1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CELL]), Manager2,
				   "IconCel_1", display,
				   style_gauche_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCel_1), FUNC_VALUE,
	  SelectFunction, (void *)ALIGN_H_LEFT);
 IconCel_2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CELL]), Manager2,
				   "IconCel_2", display,
				   style_centre_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCel_2), FUNC_VALUE,
	  SelectFunction, (void *)ALIGN_H_CENTER);
 IconCel_3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CELL]), Manager2,
				   "IconCel_3", display,
				   style_droite_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCel_3), FUNC_VALUE,
	  SelectFunction, (void *)ALIGN_H_RIGHT);
 IconCel_4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CELL]), Manager2,
				   "IconCel_4", display,
				   style_justifie_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCel_4), FUNC_VALUE,
	  SelectFunction, (void *)JUSTIFY_H);
 IconCel_s1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CELL]), Manager2,
				    "IconCel_s1", display,
				    NULL, NULL, XcSeparator);
 IconCel_8 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CELL]), Manager2,
				   "IconCel_8", display,
				   style_haut_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCel_8), FUNC_VALUE,
	  SelectFunction, (void *)ALIGN_V_TOP);

 IconCel_10 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CELL]), Manager2,
				    "IconCel_10", display,
				    style_bas_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCel_10), FUNC_VALUE,
	  SelectFunction, (void *)ALIGN_V_BOTTOM);
 IconCel_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CELL]), Manager2,
				   "IconCel_5", display,
				   style_vjustif_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCel_5), FUNC_VALUE,
	  SelectFunction, (void *)JUSTIFY_V);
 IconCel_s2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CELL]), Manager2,
				    "IconCel_s2", display,
				    NULL, NULL, XcSeparator);
  
 IconCel_11 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CELL]), Manager2,
				    "IconCel_11", display,
				    style_multilignes_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCel_11), FUNC_VALUE,
	  SelectFunction, (void *)MULTILINES);
 IconCel_s3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CELL]), Manager2,
				    "IconCel_s3", display,
				    NULL, NULL, XcSeparator);

 IconCel_13 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CELL]), Manager2,
				    "IconCel_13", display,
				    style_angle90_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCel_13), FUNC_VALUE,
	  SelectFunction, (void *)ANGLE_90);
 IconCel_14 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_CELL]), Manager2,
				    "IconCel_14", display,
				    style_angle270_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconCel_14), FUNC_VALUE,
	  SelectFunction, (void *)ANGLE_270);
  
 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Creating Icon bar BORDER");
 /* ----------------------------------------------------------------------- */
 /* --                           HBAR_BORDER                                */
 /* ----------------------------------------------------------------------- */
  
 IconBor_1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_BORDER]), Manager2,
				   "IconBor_1", display,
				   cell_bord_clean_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconBor_1), FUNC_ACTIVATE,
	  SelectFunction, (void *)UNSET_ALL_BORDER);
 IconBor_s1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_BORDER]), Manager2,
				    "IconBor_s1", display,
				    NULL, NULL, XcSeparator);
 IconBor_2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_BORDER]), Manager2,
				   "IconBor_2", display,
				   cell_bord_gauche_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconBor_2), FUNC_VALUE,
	  SelectFunction, (void *)SET_BORDER_LEFT);
 IconBor_3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_BORDER]), Manager2,
				   "IconBor_3", display,
				   cell_bord_bas_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconBor_3), FUNC_VALUE,
	  SelectFunction, (void *)SET_BORDER_DOWN);
 IconBor_4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_BORDER]), Manager2,
				   "IconBor_4", display,
				   cell_bord_droite_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconBor_4), FUNC_VALUE,
	  SelectFunction, (void *)SET_BORDER_RIGHT);
 IconBor_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_BORDER]), Manager2,
				   "IconBor_5", display,
				   cell_bord_haut_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconBor_5), FUNC_VALUE,
	  SelectFunction, (void *)SET_BORDER_UP);
 IconBor_6 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_BORDER]), Manager2,
				   "IconBor_6", display,
				   cell_bord_contours_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconBor_6), FUNC_ACTIVATE,
	  SelectFunction, (void *)SET_ALL_BORDER);
 IconBor_s2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_BORDER]), Manager2,
				    "IconBor_s2", display,
				    NULL, NULL, XcSeparator);
 CLfillColor = NEW(c_ColorList)(WIDGET(hIconBar[HBAR_BORDER]), Manager2,
				"clFillColor", XcCL_FILL);
 IconBor_s3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_BORDER]), Manager2,
				    "IconBor_s3", display,
				    NULL, NULL, XcSeparator);
 IconBor_7 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_BORDER]), Manager2,
				   "IconBor_7", display,
				   background_default_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconBor_7), FUNC_ACTIVATE,
	  SelectFunction, (void *)SET_BACKGROUND);
 IconBor_8 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_BORDER]), Manager2,
				   "IconBor_8", display,
				   cell_inversevideo_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconBor_8), FUNC_ACTIVATE,
	  SelectFunction, (void *)INVERSE_COLOR);
  
 F(TimeIndicator).advance_indicator(TimeIndicator);
  
 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Creating Icon bar GRAPH");
 /* ----------------------------------------------------------------------- */
 /* --                            HBAR_GRAPH                                */
 /* ----------------------------------------------------------------------- */
  
 IconGra_0 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				   "IconGra_0", display,
				   cadre_selectionner_tablor_xpm, NULL,
				   XcToggle);
 CALLBACK(WIDGET(IconGra_0), FUNC_VALUE,
	  ToggleSwitch, (void *)SELECT_MOVE_RESIZE_CELL);
 IconGra_1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				   "IconGra_1", display,
				   cadre_creer_tablor_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconGra_1), FUNC_VALUE,
	  ToggleSwitch, (void *)CREATE_CADRE_RECTANGLE);
 IconGra_s1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				    "IconGra_s1", display,
				    NULL, NULL, XcSeparator);
 IconGra_2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2, 
				   "IconGra_2", display,
				   cadre_premierplan_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconGra_2), FUNC_ACTIVATE,
	  SelectFunction, (void *)SET_HIGHEST_PRIORITY);
 IconGra_3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				   "IconGra_3", display,
				   cadre_arriereplan_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconGra_3), FUNC_ACTIVATE,
	  SelectFunction, (void *)SET_LOWEST_PRIORITY);
 IconGra_4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				   "IconGra_4", display,
				   cadre_approcher_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconGra_4), FUNC_ACTIVATE,
	  SelectFunction, (void *)SET_HIGHER_PRIORITY);
 IconGra_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				   "IconGra_5", display,
				   cadre_eloigner_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconGra_5), FUNC_ACTIVATE,
	  SelectFunction, (void *)SET_LOWER_PRIORITY);
 IconGra_s2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				    "IconGra_s2", display,
				    NULL, NULL, XcSeparator);
 IconGra_6 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				   "IconGra_6", display,
				   graph_histogrammes_28_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconGra_6), FUNC_VALUE,
	  ToggleSwitchType, (void *)GRAPH_HISTO_V);
 IconGra_7 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				   "IconGra_7", display,
				   graph_barres_28_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconGra_7), FUNC_VALUE,
	  ToggleSwitchType, (void *)GRAPH_HISTO_H);
 IconGra_8 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				   "IconGra_8", display,
				   graph_courbes_28_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconGra_8), FUNC_VALUE,
	  ToggleSwitchType, (void *)GRAPH_CURVE);
 IconGra_9 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				   "IconGra_9", display,
				   graph_aires_28_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconGra_9), FUNC_VALUE,
	  ToggleSwitchType, (void *)GRAPH_SURFACE);
 IconGra_10 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				    "IconGra_10", display,
				    graph_secteurs_28_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconGra_10), FUNC_VALUE,
	  ToggleSwitchType, (void *)GRAPH_SECTOR);
 IconGra_11 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				    "IconGra_11", display,
				    graph_radar_28_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconGra_11), FUNC_VALUE,
	  ToggleSwitchType, (void *)GRAPH_RADAR);
 IconGra_s3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				    "IconGra_s3", display,
				    NULL, NULL, XcSeparator);
 IconGra_12 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				    "IconGra_12", display,
				    conf_axes_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconGra_12), FUNC_VALUE,
	  SelectFunction, (void *)GRAPH_AXES);
 IconGra_13 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				    "IconGra_13", display,
				    conf_legende_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconGra_13), FUNC_VALUE,
	  SelectFunction, (void *)GRAPH_LEGEND);
 IconGra_16 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				    "IconGra_16", display,
				    conf_abscisse_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconGra_16), FUNC_VALUE,
	  SelectFunction, (void *)GRAPH_ABSCISSE);
 IconGra_17 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				    "IconGra_17", display,
				    conf_sstitres_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconGra_17), FUNC_VALUE,
	  SelectFunction, (void *)GRAPH_ORDONNEE);
 IconGra_14 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				    "IconGra_14", display,
				    conf_titre_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconGra_14), FUNC_VALUE,
	  SelectFunction, (void *)GRAPH_TITLE);
 IconGra_15 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				    "IconGra_15", display,
				    conf_config_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconGra_15), FUNC_ACTIVATE,
	  SelectFunction, (void *)GRAPH_CONFIG);
 IconGra_s4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				    "IconGra_s4", display,
				    NULL, NULL, XcSeparator);
 IconGra_18 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_GRAPH]), Manager2,
				    "IconGra_18", display,
				    cadre_lock_xpm, NULL, XcToggle);
 CALLBACK(WIDGET(IconGra_18), FUNC_VALUE,
	  SelectFunction, (void *)LOCK_CADRE);
  
 F(TimeIndicator).advance_indicator(TimeIndicator);
  
 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Creating Icon bar ZOOM");
 /* ----------------------------------------------------------------------- */
 /* --                            HBAR_ZOOM                                 */
 /* ----------------------------------------------------------------------- */
  
 IconZoo_1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_1", display,
				   loupe_avant_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_1), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_PLUS_SHEET);
 IconZoo_2 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_2", display, 
				   loupe_arriere_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_2), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_MINUS_SHEET);
 IconZoo_s1 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				    "IconZoo_s1", display,
				    NULL, NULL, XcSeparator);
 IconZoo_3 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_3", display,
				   loupe_50_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_3), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_50_SHEET);
 IconZoo_4 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_4", display,
				   loupe_75_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_4), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_75_SHEET);
 IconZoo_5 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_5", display,
				   loupe_100_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_5), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_100_SHEET);
 IconZoo_6 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_6", display,
				   loupe_200_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_6), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_200_SHEET);
 IconZoo_7 = (c_Icon *)NEW(c_Icon)(WIDGET(hIconBar[HBAR_ZOOM]), Manager2, 
				   "IconZoo_7", display,
				   loupe_400_xpm, NULL, XcPush);
 CALLBACK(WIDGET(IconZoo_7), FUNC_ACTIVATE,
	  SelectFunction, (void *)ZOOM_400_SHEET);

 F(MainInterface1).set_loadingMessage(MainInterface1, "Initialize Icon bars");

 F(Manager2).Manage_all(Manager2, NULL); 
 
 /* ----------------------------------------------------------------- ** 
 ** Initialize Icon Bars                                              ** 
 ** ----------------------------------------------------------------- */

 i = 0;
 toggletable[i].tog_wid = WIDGET(IconGra_0);
 toggletable[i].func_num = SELECT_MOVE_RESIZE_CELL;
 i++;
 toggletable[i].tog_wid = WIDGET(IconGra_1);
 toggletable[i].func_num = CREATE_CADRE_RECTANGLE;
 i++;
 toggletable[i].tog_wid = 0;
 toggletable[i].func_num = 0;


 i = 0;
 typetoggletable[i].tog_wid = WIDGET(IconGra_6);
 typetoggletable[i].func_num = GRAPH_HISTO_V;
 typetoggletable[i].mode_num = HG_HISTOGRAMME;
 i++;
 typetoggletable[i].tog_wid = WIDGET(IconGra_7);
 typetoggletable[i].func_num = GRAPH_HISTO_H;
 typetoggletable[i].mode_num = HG_BARS;
 i++;
 typetoggletable[i].tog_wid = WIDGET(IconGra_8);
 typetoggletable[i].func_num = GRAPH_CURVE;
 typetoggletable[i].mode_num = HG_CURVE;
 i++;
 typetoggletable[i].tog_wid = WIDGET(IconGra_9);
 typetoggletable[i].func_num = GRAPH_SURFACE;
 typetoggletable[i].mode_num = HG_SURFACE;
 i++;
 typetoggletable[i].tog_wid = WIDGET(IconGra_10);
 typetoggletable[i].func_num = GRAPH_SECTOR;
 typetoggletable[i].mode_num = HG_SECTOR;
 i++;
 typetoggletable[i].tog_wid = WIDGET(IconGra_11);
 typetoggletable[i].func_num = GRAPH_RADAR;
 typetoggletable[i].mode_num = HG_RADAR;
 i++;
 typetoggletable[i].tog_wid = 0;
 typetoggletable[i].func_num = 0;
 typetoggletable[i].mode_num = HG_NONE;


 /*-------------------------------------------------*/
 /*---------------- initialize IBar ----------------*/
 /*-------------------------------------------------*/
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
 ** Barre du bas                                                      ** 
 ** ----------------------------------------------------------------- */

 BottomBar = (c_Form *)NEW(c_Form)(WIDGET(MainInterface1), Manager1,
				   "BottomBar", 32, 32);

 IconBottom = (c_Icon *)NEW(c_Icon)(WIDGET(BottomBar), Manager1,
				    "IconBottom", display, 
				    xquad2_xpm, NULL, XcLabel);

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


 /*  MainDocument = (c_RowColumn *)NEW(c_RowColumn)(WIDGET(MainInterface1), 
     Manager1,
     "MainDocument", 
     150, 150, XmVERTICAL); */
  
 MainDocument = (c_DrawingArea *)NEW(c_DrawingArea)(WIDGET(MainInterface1), 
						    Manager1,
						    "MainDocument", 
						    150, 150);
  
 Bar1 = (c_MenuBar *)NEW(c_MenuBar)(WIDGET(MainInterface1), 
				    Manager1, "Menu");
  
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

 /*--- CALCULS & TOOLS ---*/

 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_CALCUL]),helpCalc, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCal_6),helpCalcSumm, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCal_7),helpToolsFunctions, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCal_8),helpToolsThousands, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCal_9),helpToolsCurrency, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCal_10),helpToolsPercent, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCal_11),helpToolsScientific, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCal_12),helpToolsEngineer, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCal_13),helpAddDigit, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCal_14),helpSubDigit, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCal_15),helpPasteEqual, XcR_NONE);

 /*--- FONTES ---*/

 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_FONT]),helpFont, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFon_3),helpToolsBold, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFon_4),helpToolsItalic, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFon_5),helpToolsUnderline, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFon_6),helpToolsStrikeout, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFon_9),helpToolsShadow, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFon_11),helpToolsSmallCaps, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconFon_12),helpToolsBigCaps, XcR_NONE);

 /*--- CELLULES ---*/

 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_CELL]),helpCells, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCel_1),helpCellsLeft, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCel_2),helpCellsCenter, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCel_3),helpCellsRight, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCel_4),helpCellsHJustify, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCel_5),helpCellsVJustify, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCel_8),helpCellsTop, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCel_10),helpCellsBottom, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCel_11),helpCellsMultilines, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCel_13),helpCellsAngle90, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconCel_14),helpCellsAngle270, XcR_NONE);

 /*--- BORDURES ---*/

 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_BORDER]),helpBorders, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconBor_1),helpBordersClean, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconBor_2),helpBordersLeft, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconBor_3),helpBordersBottom, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconBor_4),helpBordersRight, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconBor_5),helpBordersTop, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconBor_6),helpBordersOutline, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconBor_7),helpBackgroundDefault, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconBor_8),helpVideoReverse, XcR_NONE);

 /*--- GRAPHIQUES ---*/

 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_GRAPH]),helpGraph, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_0),helpFrameSelect, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_1),helpFrameCreate, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_2),helpFramePlanFirst, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_3),helpFramePlanLast, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_4),helpFramePlanUp, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_5),helpFramePlanDown, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_6),helpGraphVBar, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_7),helpGraphHBar, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_8),helpGraphLine, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_9),helpGraphArea, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_10),helpGraphPie, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_11),helpGraphRadar, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_12),helpGraphAxes, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_13),helpGraphLegend, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_16),helpGraphAbs, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_17),helpGraphNames, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_14),helpGraphTitle, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_15),helpGraphSetup, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconGra_18),helpFrameLock, XcR_NONE);
  
 /*--- ZOOM ---*/

 SET_HELPLINE(GlobHelpLine, WIDGET(vIcon[HBAR_ZOOM]),helpZoom, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_1),helpZoomIn, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_2),helpZoomOut, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_3),helpZoom50, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_4),helpZoom75, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_5),helpZoom100, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_6),helpZoom200, XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(IconZoo_7),helpZoom400, XcR_NONE);
  
 SET_HELPLINE(GlobHelpLine, WIDGET(IconTra),helpTrash, XcR_NONE);

 /*--- EDITBAR ---*/
 SET_HELPLINE(GlobHelpLine, WIDGET(GlobEditBar->Icon_Esc),helpEditbarEsc,
	      XcR_NONE);
 SET_HELPLINE(GlobHelpLine, WIDGET(GlobEditBar->Icon_Ok),helpEditbarOk,
	      XcR_NONE);

 F(MainInterface1).set_loadingMessage(MainInterface1,
				      "Creating Pulldown menus");
 F(TimeIndicator).advance_indicator(TimeIndicator);

 /* ----------------------------------------------------------------- ** 
 ** Menu FICHIER                                                      ** 
 ** ----------------------------------------------------------------- */
#define RIEN            1

#define NOUVEAU         1
#define OUVRIR          2
#define SEP1            3
#define FERMER          4
#define SAUVER          5
#define SAUVER_SOUS     6
#define TOUT_SAUVER     7
#define SEP2            8
#define IMPORTER        9
#define SUBMENU1        10
#define EXPORTER        11
#define EXPORTER_VECTOR 12
#define EXPORTER_TEXT   13
#define EXPORTER_HTML   14
#define SEP3            15
#define MISEENPAGE      16
#define IMPRIMER        17
#define SEP4            18
#define QUITTER         19

 MenuFic = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1, 
		   "pmFile",
		   "mNew", NOUVEAU,
		   "mOpen", OUVRIR,
		   MENUSEPARATOR, SEP1,
		   "mClose", FERMER,
		   "mSave", SAUVER,
		   "mSaveAs", SAUVER_SOUS,
		   "mSaveAll", TOUT_SAUVER,
		   MENUSEPARATOR, SEP2,
		   "mAcquire", IMPORTER,
		   SUBMENUON, SUBMENU1,
		   "mExport", EXPORTER,
		   "mExportVector", EXPORTER_VECTOR,
		   "mExportText", EXPORTER_TEXT,
		   "mExportHTML", EXPORTER_HTML,
		   SUBMENUOFF, RIEN,
		   MENUSEPARATOR, SEP3,
		   "mPrintSetup", MISEENPAGE,
		   "mPrint", IMPRIMER,
		   MENUSEPARATOR, SEP4,
		   "mQuit", QUITTER,
		   NULL);

 F(MenuFic).AddCallback(MenuFic, NOUVEAU, MenuFileNew, NULL);
 F(MenuFic).AddCallback(MenuFic, OUVRIR, MenuFileOpen, NULL);
 F(MenuFic).AddCallback(MenuFic, FERMER, MenuFileClose, NULL);
 F(MenuFic).AddCallback(MenuFic, SAUVER, MenuFileSave, NULL);
 F(MenuFic).AddCallback(MenuFic, SAUVER_SOUS, MenuFileSaveas, NULL);
 F(MenuFic).AddCallback(MenuFic, TOUT_SAUVER, MenuFileSaveall, NULL);

 F(MenuFic).AddCallback(MenuFic, IMPORTER, MenuFileAcquireDoc, NULL);
 F(MenuFic).AddCallback(MenuFic, EXPORTER_VECTOR, MenuFileExportVector, NULL);
 F(MenuFic).AddCallback(MenuFic, EXPORTER_TEXT, MenuFileExportText, NULL);
 F(MenuFic).AddCallback(MenuFic, EXPORTER_HTML, MenuFileExportHTML, NULL);
  
 F(MenuFic).AddCallback(MenuFic, MISEENPAGE, MenuFilePrintSetup, NULL);
 F(MenuFic).AddCallback(MenuFic, IMPRIMER, MenuFilePrint, NULL);
 F(MenuFic).AddCallback(MenuFic, QUITTER, MenuFileQuit, NULL);
 XtAddCallback(WIDGET(MenuFic), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuFic), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);
  
 F(TimeIndicator).advance_indicator(TimeIndicator);

 /* ----------------------------------------------------------------- ** 
 ** Menu EDITION                                                      ** 
 ** ----------------------------------------------------------------- */

#define COUPER 			1
#define COPIER 			2
#define COLLER 			3
#define EFFACER_CADRE		4
#undef SUBMENU1
#define SUBMENU1		5
#define EFFACER 		6
#define EFFACER_TOUT 		7
#undef SEP1
#define SEP1 			8
#define EFFACER_FORMULES	9
#define EFFACER_FORMATS 	10
#undef SEP2
#define SEP2 			11
#define EFFACER_MOTIFS 	        12
#define EFFACER_ENCADR 		13
#define EFFACER_NOMBRES       	14
#define EFFACER_POSITIONS       15
#define EFFACER_POLICES       	16
#undef SEP3
#define SEP3 			17
#define INSERER_LIGNE		18
#define INSERER_COLONNE		19
#define DETRUIRE_LIGNE		20
#define DETRUIRE_COLONNE	21
#undef SEP4
#define SEP4 			22
#define COPIER_BAS		23
#define COPIER_DROITE		24
#define SEP5			25
#define EDIT_COULEURS		26

 MenuEdi = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1,
		   "pmEdit",
		   "mCut", COUPER,
		   "mCopy", COPIER,
		   "mPaste", COLLER,
		   NOMANAGE, RIEN,     
		   "mDeleteFrame", EFFACER_CADRE,
		   SUBMENUON, SUBMENU1,
		   "mDelete", EFFACER,
		   "mDeleteAll", EFFACER_TOUT,
		   MENUSEPARATOR, SEP1,
		   "mDeleteFormula", EFFACER_FORMULES,
		   "mDeleteFormat", EFFACER_FORMATS,
		   MENUSEPARATOR, SEP2,
		   "mDeletePattern", EFFACER_MOTIFS,
		   "mDeleteBorder", EFFACER_ENCADR,
		   "mDeleteStyle", EFFACER_NOMBRES,
		   "mDeletePosition", EFFACER_POSITIONS,
		   "mDeleteFont", EFFACER_POLICES,
		   SUBMENUOFF, RIEN,
		   MENUSEPARATOR, SEP3,
		   "mInsertRow", INSERER_LIGNE,
		   "mDeleteRow", DETRUIRE_LIGNE,
		   "mInsertColumn", INSERER_COLONNE,
		   "mDeleteColumn", DETRUIRE_COLONNE,
		   MENUSEPARATOR, SEP4,
		   "mCopyBottom", COPIER_BAS,
		   "mCopyRight", COPIER_DROITE,
		   MENUSEPARATOR, SEP5,
		   "mColors", EDIT_COULEURS,
		   NULL);
  
 F(MenuEdi).AddCallback(MenuEdi, COUPER, 
			SelectFunction, (void *)CUT_REGION);
 F(MenuEdi).AddCallback(MenuEdi, COPIER, 
			SelectFunction, (void *)COPY_REGION);
 F(MenuEdi).AddCallback(MenuEdi, COLLER, 
			SelectFunction, (void *)PASTE_REGION);
 F(MenuEdi).AddCallback(MenuEdi, EFFACER_CADRE, 
			SelectFunction, (void *)DELETE_OBJECT);
 F(MenuEdi).AddCallback(MenuEdi, EFFACER_TOUT, 
			SelectFunction, (void *)UNSET_ALL_REGION);
 F(MenuEdi).AddCallback(MenuEdi, EFFACER_FORMULES, 
			SelectFunction, (void *)UNSET_FORMULAS);
 F(MenuEdi).AddCallback(MenuEdi, EFFACER_FORMATS, 
			SelectFunction, (void *)UNSET_ALL_CELLSTYLE);
 F(MenuEdi).AddCallback(MenuEdi, EFFACER_MOTIFS, 
			SelectFunction, (void *)UNSET_INSIDE_PATTERN);
 F(MenuEdi).AddCallback(MenuEdi, EFFACER_ENCADR, 
			SelectFunction, (void *)UNSET_ALL_BORDER);
 F(MenuEdi).AddCallback(MenuEdi, EFFACER_NOMBRES, 
			SelectFunction, (void *)UNSET_FORMATNBR);
 F(MenuEdi).AddCallback(MenuEdi, EFFACER_POSITIONS, 
			SelectFunction, (void *)ALIGN_DEFAULT);
 F(MenuEdi).AddCallback(MenuEdi, EFFACER_POLICES, 
			SelectFunction, (void *)UNSET_TEXTSTYLE);
 F(MenuEdi).AddCallback(MenuEdi, INSERER_LIGNE, 
			SelectFunction, (void *)INSERT_ROW);
 F(MenuEdi).AddCallback(MenuEdi, INSERER_COLONNE, 
			SelectFunction, (void *)INSERT_COLUMN);
 F(MenuEdi).AddCallback(MenuEdi, DETRUIRE_LIGNE, 
			SelectFunction, (void *)DELETE_ROW);
 F(MenuEdi).AddCallback(MenuEdi, DETRUIRE_COLONNE, 
			SelectFunction, (void *)DELETE_COLUMN);
 F(MenuEdi).AddCallback(MenuEdi, COPIER_BAS,
			SelectFunction, (void *)COPY_DOWN);
 F(MenuEdi).AddCallback(MenuEdi, COPIER_DROITE,
			SelectFunction, (void *)COPY_RIGHT);
 F(MenuEdi).AddCallback(MenuEdi, EDIT_COULEURS, MenuEditColors, NULL);
 XtAddCallback(WIDGET(MenuEdi), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuEdi), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);
  
 F(TimeIndicator).advance_indicator(TimeIndicator);

 /* ----------------------------------------------------------------- ** 
 ** Menu SELECTION                                                      ** 
 ** ----------------------------------------------------------------- */

#define COLLER_FONCTION		1
#undef SEP1
#define SEP1 			2
#define ALLER_A			3
#define ALLER_A_ACTIVE       	4
#define ALLER_A_DERNIERE       	5
#undef SEP2
#define SEP2 			6
#define SELECTIONNER_DERNIERE 	7
#define TOUT_SELECTIONNER 	8
#define TOUT_SELECTIONNER_CADRES  9

 MenuSel = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1,
		   "pmSelect",
		   "mPasteFunction", COLLER_FONCTION,
		   MENUSEPARATOR, SEP1,
		   "mGotoCell", ALLER_A,
		   "mGotoActiveCell", ALLER_A_ACTIVE,
		   "mGotoLastCell", ALLER_A_DERNIERE,
		   MENUSEPARATOR, SEP2,
		   "mSelectToLastCell", SELECTIONNER_DERNIERE,
		   "mSelectAllCells", TOUT_SELECTIONNER,
		   "mSelectAllFrames", TOUT_SELECTIONNER_CADRES,
		   NULL);

 F(MenuSel).AddCallback(MenuSel, COLLER_FONCTION, MenuSelectFonctions, NULL);
 F(MenuSel).AddCallback(MenuSel, ALLER_A, MenuSelectGotoCell, NULL);
 F(MenuSel).AddCallback(MenuSel, ALLER_A_ACTIVE, 
			SelectFunction, (void *)GOTO_ACTIVE_CELL);
 F(MenuSel).AddCallback(MenuSel, ALLER_A_DERNIERE, 
			SelectFunction, (void *)GOTO_LAST_CELL);
 F(MenuSel).AddCallback(MenuSel, SELECTIONNER_DERNIERE, 
			SelectFunction, (void *)SELECT_TO_LAST_CELL);
 F(MenuSel).AddCallback(MenuSel, TOUT_SELECTIONNER, 
			SelectFunction, (void *)SELECT_ALL_REGION);
 F(MenuSel).AddCallback(MenuSel, TOUT_SELECTIONNER_CADRES, 
			SelectFunction, (void *)SELECT_ALL_CADRE);

 XtAddCallback(WIDGET(MenuSel), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuSel), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);

 /* ----------------------------------------------------------------- ** 
 ** Menu AFFICHAGE                                                    ** 
 ** ----------------------------------------------------------------- */

#define REDESSINE_ALL		1
#undef SEP1
#define SEP1 			2
#define MASK_GRILLE    		3
#define UNMASK_GRILLE   	4
#define MASK_CADRES		5
#define UNMASK_CADRES		6
#undef SEP2
#define SEP2 			7
#define UNMASK_BOTTOMBAR	8
#define MASK_BOTTOMBAR		9
#define MASK_COLUMN_ORIGIN	10
#define UNMASK_COLUMN_ORIGIN	11
#define MASK_ROW_ORIGIN		12
#define UNMASK_ROW_ORIGIN	13
#define MASK_PAGE_MARK		14
#define UNMASK_PAGE_MARK	15
#undef SEP3
#define SEP3 			16
#define MASK_FORMULA		17
#define UNMASK_FORMULA		18
#define MASK_ZERO_CELL		19
#define UNMASK_ZERO_CELL	20
#undef SEP4
#define SEP4 			21
#define STOP_MAGNETISATION 	22
#define MAGNETISATION 		23
  
 MenuAff = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1,
		   "pmDisplay",
		   "mRedraw", REDESSINE_ALL,
		   MENUSEPARATOR, SEP1,
		   "mMaskGrid", MASK_GRILLE,
		   NOMANAGE, RIEN,     
		   "mShowGrid", UNMASK_GRILLE,
		   "mMaskFrameBorders", MASK_CADRES,
		   NOMANAGE, RIEN,
		   "mShowFrameBorders", UNMASK_CADRES,
		   MENUSEPARATOR, SEP2,
		   NOMANAGE, RIEN,
		   "mShowInfobar", UNMASK_BOTTOMBAR,
		   "mMaskInfobar", MASK_BOTTOMBAR,
		   NOMANAGE, RIEN,
		   "mShowColOrigin", UNMASK_COLUMN_ORIGIN,
		   "mMaskColOrigin", MASK_COLUMN_ORIGIN,
		   NOMANAGE, RIEN,
		   "mShowRowOrigin", UNMASK_ROW_ORIGIN,
		   "mMaskRowOrigin", MASK_ROW_ORIGIN,
		   "mShowSeparator", UNMASK_PAGE_MARK,
		   NOMANAGE, RIEN,
		   "mMaskSeparator", MASK_PAGE_MARK,
		   MENUSEPARATOR, SEP3,
		   "mShowFormula", UNMASK_FORMULA,
		   NOMANAGE, RIEN,
		   "mMaskFormula", MASK_FORMULA,
		   NOMANAGE, RIEN,
		   "mShowZeroCell", UNMASK_ZERO_CELL,
		   "mMaskZeroCell", MASK_ZERO_CELL,
		   MENUSEPARATOR, SEP4,
		   "mActivMagnet", MAGNETISATION,
		   NOMANAGE, RIEN,
		   "mStopMagnet", STOP_MAGNETISATION,
		   NULL);

 F(MenuAff).AddCallback(MenuAff, REDESSINE_ALL, 
			SelectFunction, (void *)REDRAW_ALL);
 F(MenuAff).AddCallback(MenuAff, MASK_GRILLE,
			SelectFunction, (void *)SET_UNSET_GRID);
 F(MenuAff).AddCallback(MenuAff, UNMASK_GRILLE,
			SelectFunction, (void *)SET_UNSET_GRID);
 F(MenuAff).AddCallback(MenuAff, MASK_CADRES,
			SelectFunction, (void *)DISPLAY_FRAME_BORDER);
 F(MenuAff).AddCallback(MenuAff, UNMASK_CADRES,
			SelectFunction, (void *)DISPLAY_FRAME_BORDER);
 F(MenuAff).AddCallback(MenuAff, MASK_BOTTOMBAR, 
			DisplayBottomBar, NULL);
 F(MenuAff).AddCallback(MenuAff, UNMASK_BOTTOMBAR,
			DisplayBottomBar, NULL);
 F(MenuAff).AddCallback(MenuAff, MASK_COLUMN_ORIGIN,
			SelectFunction, (void *)DISPLAY_COLUMN_ORIGIN);
 F(MenuAff).AddCallback(MenuAff, UNMASK_COLUMN_ORIGIN,
			SelectFunction, (void *)DISPLAY_COLUMN_ORIGIN);
 F(MenuAff).AddCallback(MenuAff, MASK_ROW_ORIGIN,
			SelectFunction, (void *)DISPLAY_ROW_ORIGIN);
 F(MenuAff).AddCallback(MenuAff, UNMASK_ROW_ORIGIN,
			SelectFunction, (void *)DISPLAY_ROW_ORIGIN);
 F(MenuAff).AddCallback(MenuAff, MASK_PAGE_MARK,
			SelectFunction, (void *)DISPLAY_PAGE_MARK);
 F(MenuAff).AddCallback(MenuAff, UNMASK_PAGE_MARK,
			SelectFunction, (void *)DISPLAY_PAGE_MARK);
 F(MenuAff).AddCallback(MenuAff, MASK_FORMULA,
			SelectFunction, (void *)DISPLAY_FORMULA);
 F(MenuAff).AddCallback(MenuAff, UNMASK_FORMULA,
			SelectFunction, (void *)DISPLAY_FORMULA);
 F(MenuAff).AddCallback(MenuAff, MASK_ZERO_CELL,
			SelectFunction, (void *)DISPLAY_ZERO_CELL);
 F(MenuAff).AddCallback(MenuAff, UNMASK_ZERO_CELL,
			SelectFunction, (void *)DISPLAY_ZERO_CELL);
 F(MenuAff).AddCallback(MenuAff , MAGNETISATION,
			SelectFunction, (void *)MAGNETIZE_ALL);
 F(MenuAff).AddCallback(MenuAff , STOP_MAGNETISATION,
			SelectFunction, (void *)MAGNETIZE_ALL);
 XtAddCallback(WIDGET(MenuAff), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuAff), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);
  
 F(TimeIndicator).advance_indicator(TimeIndicator);

 /* ----------------------------------------------------------------- ** 
 ** Menu FORMAT                                                       ** 
 ** ----------------------------------------------------------------- */

#define NOMBRES		1
#define MOTIFS		2
#define ENCADREMENTS	3
#define POLICES		4
#undef  SEP1
#define SEP1		5
#define COLSIZE		6

 MenuFor = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1,
		   "pmFormat",
		   "mNumbers", NOMBRES,
		   "mPatterns", MOTIFS,
		   "mBorders", ENCADREMENTS,
		   "mFonts", POLICES,
		   MENUSEPARATOR, SEP1,
		   "mRowColSize", COLSIZE,
		   NULL);

 F(MenuFor).AddCallback(MenuFor, NOMBRES, MenuFormatNombres, NULL);
 F(MenuFor).AddCallback(MenuFor, MOTIFS, MenuFormatMotifs, NULL);
 F(MenuFor).AddCallback(MenuFor, ENCADREMENTS, MenuFormatEncadrements, NULL);
 F(MenuFor).AddCallback(MenuFor, COLSIZE, MenuFormatLargeurHauteur, NULL);
 F(MenuFor).AddCallback(MenuFor, POLICES, MenuFormatPolices, NULL);
 XtAddCallback(WIDGET(MenuFor), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuFor), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);

 /* ----------------------------------------------------------------- ** 
 ** Menu GRAPH                                                        ** 
 ** ----------------------------------------------------------------- */
#define G_BARRES	1
#define G_HISTOGRAMMES	2
#define G_AIRES		3
#define G_COURBES	4
#define G_SECTEURS	5
#define G_RADAR		6
#undef  SEP1
#define SEP1		7
#define MASK_LEGENDE	8
#define UNMASK_LEGENDE	9
#define MASK_AXES	10
#define UNMASK_AXES	11
#define MASK_TITRE	12
#define UNMASK_TITRE	13
#define MASK_ABSCISSES	14
#define UNMASK_ABSCISSES 15
#define MASK_SSTITRES	16
#define UNMASK_SSTITRES 17
#undef  SEP2
#define SEP2		18
#define G_CONFIGURATION	19

 MenuGra = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1,
		   "pmGraph",
		   "mGraphHBar", G_BARRES,
		   "mGraphVBar", G_HISTOGRAMMES,
		   "mGraphLayer", G_AIRES,
		   "mGraphLine", G_COURBES,
		   "mGraphPie", G_SECTEURS,
		   "mGraphRadar", G_RADAR,
		   MENUSEPARATOR, SEP1,
		   "mMaskLegend", MASK_LEGENDE,
		   NOMANAGE, RIEN,
		   "mShowLegend", UNMASK_LEGENDE,
		   NOMANAGE, RIEN,
		   "mMaskAxeMarks", MASK_AXES,
		   "mShowAxeMarks", UNMASK_AXES,
		   "mMaskTitle", MASK_TITRE,
		   NOMANAGE, RIEN,
		   "mShowTitle", UNMASK_TITRE,
		   "mMaskFirstSerieLabel", MASK_ABSCISSES,
		   NOMANAGE, RIEN,
		   "mShowFirstSerieLabel", UNMASK_ABSCISSES,
		   "mMaskSerieTitles", MASK_SSTITRES,
		   NOMANAGE, RIEN,
		   "mShowSerieTitles", UNMASK_SSTITRES,
		   MENUSEPARATOR, SEP2,
		   "mGraphSetup", G_CONFIGURATION,
		   NULL); 


 F(MenuGra).AddCallback(MenuGra, G_BARRES,
			SelectFunction, (void *)GRAPH_HISTO_H);
 F(MenuGra).AddCallback(MenuGra, G_HISTOGRAMMES,
			SelectFunction, (void *)GRAPH_HISTO_V);
 F(MenuGra).AddCallback(MenuGra, G_AIRES,
			SelectFunction, (void *)GRAPH_SURFACE);
 F(MenuGra).AddCallback(MenuGra, G_COURBES,
			SelectFunction, (void *)GRAPH_CURVE);
 F(MenuGra).AddCallback(MenuGra, G_SECTEURS,
			SelectFunction, (void *)GRAPH_SECTOR);
 F(MenuGra).AddCallback(MenuGra, G_RADAR,
			SelectFunction, (void *)GRAPH_RADAR);
 F(MenuGra).AddCallback(MenuGra, MASK_LEGENDE,
			SelectFunction, (void *)GRAPH_LEGEND);
 F(MenuGra).AddCallback(MenuGra, UNMASK_LEGENDE,
			SelectFunction, (void *)GRAPH_LEGEND);
 F(MenuGra).AddCallback(MenuGra, MASK_AXES,
			SelectFunction, (void *)GRAPH_AXES);
 F(MenuGra).AddCallback(MenuGra, UNMASK_AXES,
			SelectFunction, (void *)GRAPH_AXES);
 F(MenuGra).AddCallback(MenuGra, MASK_TITRE,
			SelectFunction, (void *)GRAPH_TITLE);
 F(MenuGra).AddCallback(MenuGra, UNMASK_TITRE,
			SelectFunction, (void *)GRAPH_TITLE);
 F(MenuGra).AddCallback(MenuGra, MASK_ABSCISSES,
			SelectFunction, (void *)GRAPH_ABSCISSE);
 F(MenuGra).AddCallback(MenuGra, UNMASK_ABSCISSES,
			SelectFunction, (void *)GRAPH_ABSCISSE);
 F(MenuGra).AddCallback(MenuGra, MASK_SSTITRES,
			SelectFunction, (void *)GRAPH_ORDONNEE);
 F(MenuGra).AddCallback(MenuGra, UNMASK_SSTITRES,
			SelectFunction, (void *)GRAPH_ORDONNEE);
 F(MenuGra).AddCallback(MenuGra, G_CONFIGURATION,
			SelectFunction, (void *)GRAPH_CONFIG);
 XtAddCallback(WIDGET(MenuGra), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuGra), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);


 /* ----------------------------------------------------------------- ** 
 ** Menu FENETRES                                                     ** 
 ** ----------------------------------------------------------------- */
#define CASCADE		1
#define MOSAIQUE	2
#undef SEP1
#define SEP1		3
#define DOC_LIST	4
  
 MenuFen = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1,
		   "pmWindows",
		   "mCascade", CASCADE,
		   "mTile", MOSAIQUE,
		   MENUSEPARATOR, SEP1,
		   RADIOBOXON, 0,
		   DYNAMIC, DOC_LIST,
		   0, DL_DOC_LIST_LENGTH,
		   NULL);


 F(MenuFen).AddCallback(MenuFen, CASCADE, MenuArrangeWindow, NULL);
 F(MenuFen).AddCallback(MenuFen, MOSAIQUE, MenuTileWindow, NULL);
 XtAddCallback(WIDGET(MenuFen), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuFen), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);

 /* ----------------------------------------------------------------- ** 
 ** Menu AIDE                                                         ** 
 ** ----------------------------------------------------------------- */
#define HOVERVIEW	1
#define HCONTENT	2
#define HKEYBOARD	3
#define HMOUSE		4
#undef SEP1
#define SEP1		5
#define HONITEM		6
#undef SEP2
#define SEP2		7
#define USING_HELP	8
#undef SEP3
#define SEP3		9
#define INFOLEGAL	10  
#undef SEP4
#define SEP4		11
#define A_PROPOS	12
#define REGISTRATION	13
  
 MenuAid = (c_PulldownMenu *)NEW
  (c_PulldownMenu)(WIDGET(Bar1), Manager1,
		   "pmHelp",
		   "mOverview", HOVERVIEW,
		   "mContent", HCONTENT,
		   "mKeyboard", HKEYBOARD,
		   "mMouse", HMOUSE,
		   MENUSEPARATOR, SEP1,
		   "mOnItem", HONITEM,
		   MENUSEPARATOR, SEP2,
		   "mUsingHelp", USING_HELP,
		   MENUSEPARATOR, SEP3,
		   "mLegal", INFOLEGAL,
		   "mRegistration", REGISTRATION,
		   MENUSEPARATOR, SEP3,
		   "mAbout", A_PROPOS,
		   NULL); 
    

 XtSetArg(args[0], XmNmenuHelpWidget, MenuAid->w_Cascade);
 XtSetValues(WIDGET(Bar1), args, 1);
  
 F(MenuAid).AddCallback(MenuAid, HOVERVIEW, MenuHelpCallHelp, NULL);
 F(MenuAid).AddCallback(MenuAid, HCONTENT, MenuHelpCallHelp, NULL);
 F(MenuAid).AddCallback(MenuAid, HKEYBOARD, MenuHelpCallHelp, NULL);
 F(MenuAid).AddCallback(MenuAid, HMOUSE, MenuHelpCallHelp, NULL);
 F(MenuAid).AddCallback(MenuAid, HONITEM, MenuHelpTrackHelp, NULL);
 F(MenuAid).AddCallback(MenuAid, USING_HELP, MenuHelpCallHelp, NULL);
 F(MenuAid).AddCallback(MenuAid, INFOLEGAL, MenuHelpLegal, NULL);
 F(MenuAid).AddCallback(MenuAid, REGISTRATION, MenuHelpRegistration, NULL);
 F(MenuAid).AddCallback(MenuAid, A_PROPOS, MenuHelpAbout, NULL);
 XtAddCallback(WIDGET(MenuAid), XmNmapCallback,
	       (XtCallbackProc)cb_focus_motif, NULL);
 XtAddCallback(WIDGET(MenuAid), XmNunmapCallback,
	       (XtCallbackProc)cb_unfocus_motif, NULL);
  
 /* ----------------------------------------------------------------- */
  
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

 F(MainInterface1).attach_right(MainInterface1, WIDGET(BottomBar), NULL); 
 F(MainInterface1).attach_left(MainInterface1, WIDGET(BottomBar), NULL);
 F(MainInterface1).attach_bottom(MainInterface1, WIDGET(BottomBar), NULL);

 F(MainInterface1).attach_right(MainInterface1, 
				WIDGET(GlobEditBar), NULL); 
 F(MainInterface1).attach_left(MainInterface1, 
			       WIDGET(GlobEditBar), WIDGET(vIconBar));
 F(MainInterface1).attach_top(MainInterface1, 
			      WIDGET(GlobEditBar), WIDGET(hIconBarForm));

 F(MainInterface1).attach_top(MainInterface1,
			      WIDGET(MainDocument), WIDGET(GlobEditBar));
 F(MainInterface1).attach_right(MainInterface1, 
				WIDGET(MainDocument), NULL);
 F(MainInterface1).attach_left(MainInterface1, 
			       WIDGET(MainDocument), WIDGET(vIconBar));
 F(MainInterface1).attach_bottom(MainInterface1,
				 WIDGET(MainDocument), WIDGET(BottomBar));
  
 F(TimeIndicator).advance_indicator(TimeIndicator);

 F(Manager1).Manage_all(Manager1, NULL);
 F(MainInterface1).map_TopLevel(MainInterface1);
  
 if (TimeIndicator) F(TimeIndicator).advance_indicator(TimeIndicator);

 XGetWindowAttributes(display, XtWindow(WIDGET(MainDocument)), &get_attrib);
 set_attrib.event_mask = get_attrib.all_event_masks | StructureNotifyMask;
 XChangeWindowAttributes(display, XtWindow(WIDGET(MainDocument)),
			 CWEventMask,
			 &set_attrib);
  
 F(MainInterface1).set_main_icon(MainInterface1);

 F(MainInterface1).set_loadingMessage(MainInterface1, "Initialize core");
 Gwm = (c_WinManager *)NEW(c_WinManager)(display, 
					 XtWindow(WIDGET(MainDocument)));
  
 F(GlobCursor).enable_wait_mode(GlobCursor, XtWindow(WIDGET(MainInterface1)));
    
 Gwm->SelectCallBack = SChangeWindow;

 if (TimeIndicator) F(TimeIndicator).advance_indicator(TimeIndicator);
  
 DocList = (c_DocumentList *)NEW(c_DocumentList)(MenuFen, DOC_LIST, 
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
  
 GlobDragAndDrop = (c_DragAndDrop *)NEW(c_DragAndDrop)(display);
 GlobTrash = (c_Trash *)NEW(c_Trash)(display, XtWindow(WIDGET(IconTra)));
 GlobImport = (c_Import *)NEW(c_Import)();
 GlobExport = (c_Export *)NEW(c_Export)();
  
 if (TimeIndicator) F(TimeIndicator).advance_indicator(TimeIndicator);

 NEW(c_Color)(&GlobColorAuto,  
	      F(GlobResources).getString(GlobResources, XcR_automaticColor));
 F(GlobColorAuto).set(GlobColorAuto, XcC_TRANSPARENCY, XcC_SCALE_COLOR(1.0),
		      XcC_END);
  

 F(MainInterface1).set_loadingMessage(MainInterface1, "Loading user settings");

 if (TimeIndicator) F(TimeIndicator).advance_indicator(TimeIndicator);
 /*init(2000000,2); */
 init(500000,2);		/* take a million bytes of memory for the stack */

 if(!ReadConfigFile())
 {
  if(!ReadUserConfigFile()) QuitApplication();
 }
 else
  ReadUserConfigFile();

 F(MainInterface1).set_loadingMessage(MainInterface1, 
				      "Initialize Interface items");

 F(IMfontFamily).addItems(IMfontFamily,
			  XcIM_LINKED_LIST, -1,
			  GlobFontBase->family,
			  offsetof(family_tree, Next));
 F(IMfontFamily).deleteItems(IMfontFamily,
			     XcIM_VA_LIST, 1,
			     &tmp_family);
 F(IMfontFamily).setSelection(IMfontFamily,
			      GlobFontBase->family);
 F(IMfontFamily).calculSize(IMfontFamily, 250);

 saved_family = GlobFontBase->family;
 F(IMfontFamily).setCallback(IMfontFamily,
			     (void *)change_font, NULL);
  
 F(CLfontColor).setCallback(CLfontColor, (void *)change_font_color, NULL);
 F(CLfontColor).setColorBase(CLfontColor,GlobColorBase, CL_SKIP_TRANSPARENCY); 
 F(CLfontColor).setColor(CLfontColor, NULL);

 F(CLfillColor).setCallback(CLfillColor, (void *)change_fill_color, NULL);
 F(CLfillColor).setColorBase(CLfillColor,GlobColorBase, CL_KEEP_TRANSPARENCY); 
 F(CLfillColor).setColor(CLfillColor, NULL);
  
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
  
 XmProcessTraversal(WIDGET(GlobEditBar->Edit), XmTRAVERSE_CURRENT);

 /*--- Main loop ---*/
 app = XtWidgetToApplicationContext(wMainWindow);
  
 SigErrorCount = 0;		/* can continue now */
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
}
/*---------------------------------------------------------------------*/
/*-------------------------- End of Main Part--------------------------*/
/*---------------------------------------------------------------------*/

/* ----------------------------------------------------------------- ** 
** HandleEvent - Process event                                       ** 
** ----------------------------------------------------------------- */
void XcHandleEvent(event)
XEvent *event;
{
 listmanaged	*listegwm;
 int		gwm_func;
 void		*DocumentTmp;
 void		*DocumentSel = NULL;
 c_Tableau	*DocSelected;	/* boouu! pas propre .... */
 int		TypeSel;
 int		TypeTmp;
 int     	num;
 Boolean	dbl_click;
 Window	wnone;
 int		x_, y_;
 unsigned int	attributes;
  
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
 if (event->type == ButtonPress && motif_focus)
 {
  XtDispatchEvent(event);
  return;
 }
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
     Xc_TRACE(("Leave"));
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
     Xc_TRACE(("Enter"));
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
     if (event->xbutton.button == 1)
     {
      if (event->xbutton.time-last_iclick < DBL_CLK_DELAY)
       dbl_click = TRUE;
      else
       last_iclick = event->xbutton.time;
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

   if ((DocumentTmp == DocumentSel) && (TypeTmp == DOCUMENT_TABLEUR))
   {
    int cur_fonc;
    DocSelected = (c_Tableau *)DocumentSel;
	      
    if((cur_fonc = 
	((c_Tableau *)DocumentTmp)->Feuille->function_type) ==
       FRMG_FUNCTION )
     cur_fonc = 
      ((c_Tableau *)DocumentTmp)->Feuille->frmg->function_type;

    if (store_function != cur_fonc)
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
		  
		  
     store_function = cur_fonc;

     i = 0; while ((toggletable[i].func_num !=
		    store_function) && (toggletable[i].tog_wid != 0))
      i++; if (toggletable[i].tog_wid != 0)
       XmToggleButtonSetState(toggletable[i].tog_wid, True,
			      False); break; }

    if (!DocSelected->Feuille->select_type 
	&& DocSelected->Feuille->frmg->SelectedCadre 
	&& !DocSelected->Feuille->frmg->SelectedCadre->NextCadre 
	&& DocSelected->Feuille->frmg->SelectedCadre->
	cadre->object_type == CADRE_VECTOR 
	&& ((c_VectorGraph *)
	    (DocSelected->Feuille->frmg->SelectedCadre->
	     cadre->object))->highg)
    {
     int i;
		  
     if (store_function_type
	 != (int)((c_VectorGraph *)
		  (DocSelected->Feuille->frmg->SelectedCadre->
		   cadre->object))->highg->class)
     {
      i = 0;
      while ((typetoggletable[i].mode_num 
	      != store_function_type)
	     && 
	     (typetoggletable[i].tog_wid != 0))
       i++;
      if (typetoggletable[i].tog_wid != 0)
       XmToggleButtonSetState
	(typetoggletable[i].tog_wid,
	 False, False);
      store_function_type = 
       ((c_VectorGraph *)
	(DocSelected->Feuille->frmg->SelectedCadre->
	 cadre->object))->highg->class;
      i = 0;
      while ((typetoggletable[i].mode_num
	      != store_function_type)
	     && 
	     (typetoggletable[i].tog_wid != 0))
       i++;
      if (typetoggletable[i].tog_wid != 0)
       XmToggleButtonSetState
	(typetoggletable[i].tog_wid,
	 True, False);	
      break;
     }
    }
    else
    {
     int i;

     i = 0;
     if (store_function_type != HG_NONE)
     {
      while ((typetoggletable[i].mode_num 
	      != store_function_type)
	     && 
	     (typetoggletable[i].tog_wid != 0))
       i++;
      if (typetoggletable[i].tog_wid != 0)
       XmToggleButtonSetState
	(typetoggletable[i].tog_wid,
	 False, False);
      store_function_type = HG_NONE;
     }
    }
   }
  }  
  /* ----------------------------------------------------------------- ** 
  ** End of handling Document event                                    ** 
  ** ----------------------------------------------------------------- */

  if (event->xany.window)
  {
   if (event->type == KeyPress || event->type == KeyRelease)
   {
    if (XtIsSensitive(WIDGET(GlobEditBar)))
     event->xany.window = XtWindow(WIDGET(GlobEditBar));
    else
     event->xany.window = XtWindow(wMainWindow);
   }
  }
  else
   return;
      
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
  
 if (Gwm->SelectedWindow == NULL)
  F(GlobHelpLine).change_function_message(GlobHelpLine, XcR_NONE, XcR_DEFAULT);
 else
 {
  if (TypeSel != DOCUMENT_TABLEUR)
   F(GlobHelpLine).change_function_message(GlobHelpLine, 
					   XcR_NONE, XcR_DEFAULT); 
 }
 
 if ((Gwm->SelectedWindow == NULL) || (TypeSel != DOCUMENT_TABLEUR))
 {
  /*--- Tout a FALSE sauf MENU FICHIER ---*/
  SensitivizeDoc(False);
  SensitivizeCutCopy(False);
  SensitivizePaste(False);
  SensitivizeSimpleSelect(False);
  SensitivizeAxesConfig(False);
  SensitivizeLegendConfig(False);
  SensitivizeAbscisseConfig(False);
  SensitivizeOrdonneeConfig(False);
  SensitivizeTitleConfig(False);
  SensitivizeGraphFrameTools(False);
  SensitivizeFrameTools(False);
  SensitivizeGraphTools(False);
  SensitivizeBold(False);
  SensitivizeItalic(False);


  if (ButtonActivThousands)
  {
   XmToggleButtonSetState(WIDGET(IconCal_8), False, False);
   ButtonActivThousands = FALSE;
  }
  if (ButtonActivCurrency)
  {
   XmToggleButtonSetState(WIDGET(IconCal_9), False, False);
   ButtonActivCurrency = FALSE;
  }
  if (ButtonActivPercent)
  {
   XmToggleButtonSetState(WIDGET(IconCal_10), False, False);
   ButtonActivPercent = FALSE;
  }
  if (ButtonActivScientific)
  {
   XmToggleButtonSetState(WIDGET(IconCal_11), False, False);
   ButtonActivScientific = FALSE;
  }
  if (ButtonActivEngineer)
  {
   XmToggleButtonSetState(WIDGET(IconCal_12), False, False);
   ButtonActivEngineer = FALSE;
  }
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
  if (ButtonActivStrikeout)
  {
   XmToggleButtonSetState(WIDGET(IconFon_6), False, False);
   ButtonActivStrikeout = FALSE;
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
  if (ButtonActivBigCaps)
  {
   XmToggleButtonSetState(WIDGET(IconFon_12), False, False);
   ButtonActivBigCaps = FALSE;
  }
  if (ButtonActivBorderLeft)
  {
   XmToggleButtonSetState(WIDGET(IconBor_2), False, False);
   ButtonActivBorderLeft = FALSE;
  }
  if (ButtonActivBorderBottom)
  {
   XmToggleButtonSetState(WIDGET(IconBor_3), False, False);
   ButtonActivBorderBottom = FALSE;
  }
  if (ButtonActivBorderRight)
  {
   XmToggleButtonSetState(WIDGET(IconBor_4), False, False);
   ButtonActivBorderRight = FALSE;
  }
  if (ButtonActivBorderTop)
  {
   XmToggleButtonSetState(WIDGET(IconBor_5), False, False);
   ButtonActivBorderTop = FALSE;
  }
  if (ButtonActivHAlignLeft)
  {
   XmToggleButtonSetState(WIDGET(IconCel_1), False, False);
   ButtonActivHAlignLeft = FALSE;
  }
  if (ButtonActivHAlignCenter)
  {
   XmToggleButtonSetState(WIDGET(IconCel_2), False, False);
   ButtonActivHAlignCenter = FALSE;
  }
  if (ButtonActivHAlignRight)
  {
   XmToggleButtonSetState(WIDGET(IconCel_3), False, False);
   ButtonActivHAlignRight = FALSE;
  }
  if (ButtonActivHAlignJustify)
  {
   XmToggleButtonSetState(WIDGET(IconCel_4), False, False);
   ButtonActivHAlignJustify = FALSE;
  }
  if (ButtonActivVAlignTop)
  {
   XmToggleButtonSetState(WIDGET(IconCel_8), False, False);
   ButtonActivVAlignTop = FALSE;
  }
  if (ButtonActivVAlignBottom)
  {
   XmToggleButtonSetState(WIDGET(IconCel_10), False, False);
   ButtonActivVAlignBottom = FALSE;
  }
  if (ButtonActivVAlignJustify)
  {
   XmToggleButtonSetState(WIDGET(IconCel_5), False, False);
   ButtonActivVAlignJustify = FALSE;
  }
  if (ButtonActivMultiLine)
  {
   XmToggleButtonSetState(WIDGET(IconCel_11), False, False);
   ButtonActivMultiLine = FALSE;
  }
  if (ButtonActivAngle90)
  {
   XmToggleButtonSetState(WIDGET(IconCel_13), False, False);
   ButtonActivAngle90 = FALSE;
  }
  if (ButtonActivAngle270)
  {
   XmToggleButtonSetState(WIDGET(IconCel_14), False, False);
   ButtonActivAngle270 = FALSE;
  }
  if (ButtonLockFrame)
  {
   ButtonLockFrame = FALSE;
   XmToggleButtonSetState(WIDGET(IconGra_18), False, False);
  }
  if (!ButtonMaskAxes)
  {
   ButtonMaskAxes = TRUE; 
   F(MenuGra).ToggleManage(MenuGra, MASK_AXES, UNMASK_AXES);
   XmToggleButtonSetState(WIDGET(IconGra_12), False, False);
  }
  if (!ButtonMaskLegend)
  {
   ButtonMaskLegend = TRUE; 
   F(MenuGra).ToggleManage(MenuGra, MASK_LEGENDE, UNMASK_LEGENDE);
   XmToggleButtonSetState(WIDGET(IconGra_13), False, False);
  }
  if (!ButtonMaskTitle)
  {
   ButtonMaskTitle = TRUE; 
   F(MenuGra).ToggleManage(MenuGra, MASK_TITRE, UNMASK_TITRE);
   XmToggleButtonSetState(WIDGET(IconGra_14), False, False);
  }
  if (!ButtonMaskAbscisse)
  {
   ButtonMaskAbscisse = TRUE; 
   F(MenuGra).ToggleManage(MenuGra, MASK_ABSCISSES, UNMASK_ABSCISSES);
   XmToggleButtonSetState(WIDGET(IconGra_16), False, False);
  }
  if (!ButtonMaskOrdonnee)
  {
   ButtonMaskOrdonnee = TRUE; 
   F(MenuGra).ToggleManage(MenuGra, MASK_SSTITRES, UNMASK_SSTITRES);
   XmToggleButtonSetState(WIDGET(IconGra_17), False, False);
  }
 }
 else
 {
  SensitivizeDoc(True);
  DocSelected = (c_Tableau *)DocumentSel;
      
  if (DocSelected->Feuille->frmg->Frame_Border_actif)
  {
   if (!ButtonMaskFrame)
   {
    ButtonMaskFrame = TRUE;
    F(MenuAff).ToggleManage(MenuAff, UNMASK_CADRES, MASK_CADRES);
   }
  }
  else
  {
   if (ButtonMaskFrame)
   {
    ButtonMaskFrame = FALSE;
    F(MenuAff).ToggleManage(MenuAff, MASK_CADRES, UNMASK_CADRES);
   }
  }

  if (DocSelected->Feuille->frmg->magnetic)
  {
   if (!ButtonStopMagnetisation)
   {
    ButtonStopMagnetisation = TRUE;
    F(MenuAff).ToggleManage(MenuAff, MAGNETISATION,
			    STOP_MAGNETISATION);
   }
  }
  else
  {
   if (ButtonStopMagnetisation)
   {
    ButtonStopMagnetisation = FALSE;
    F(MenuAff).ToggleManage(MenuAff, STOP_MAGNETISATION,
			    MAGNETISATION);
   }
  }


  if (DocSelected->Feuille->grid)
  {
   if (!ButtonMaskGrid)
   {
    ButtonMaskGrid = TRUE;
    F(MenuAff).ToggleManage(MenuAff, UNMASK_GRILLE, MASK_GRILLE);
   }
  }
  else
  {
   if (ButtonMaskGrid)
   {
    ButtonMaskGrid = FALSE;
    F(MenuAff).ToggleManage(MenuAff, MASK_GRILLE, UNMASK_GRILLE);
   }
  }

  if (DocSelected->Feuille->Column_Origin_actif)
  {
   if (!ButtonMaskCol)
   {
    ButtonMaskCol = TRUE;
    F(MenuAff).ToggleManage(MenuAff, UNMASK_COLUMN_ORIGIN,
			    MASK_COLUMN_ORIGIN);
   }
  }
  else
  {
   if (ButtonMaskCol)
   {
    ButtonMaskCol = FALSE;
    F(MenuAff).ToggleManage(MenuAff, MASK_COLUMN_ORIGIN, 
			    UNMASK_COLUMN_ORIGIN);
   }
  }

  if (DocSelected->Feuille->Row_Origin_actif)
  {
   if (!ButtonMaskRow)
   {
    ButtonMaskRow = TRUE;
    F(MenuAff).ToggleManage(MenuAff, UNMASK_ROW_ORIGIN, 
			    MASK_ROW_ORIGIN);
   }
  }
  else
  {
   if (ButtonMaskRow)
   {
    ButtonMaskRow = FALSE;
    F(MenuAff).ToggleManage(MenuAff, MASK_ROW_ORIGIN,
			    UNMASK_ROW_ORIGIN);
   }
  }
      
  if (DocSelected->Feuille->page_mark)
  {
   if (!ButtonMaskPageMark)
   {
    ButtonMaskPageMark = TRUE;
    F(MenuAff).ToggleManage(MenuAff, UNMASK_PAGE_MARK,
			    MASK_PAGE_MARK);
   }
  }
  else
  {
   if (ButtonMaskPageMark)
   {
    ButtonMaskPageMark = FALSE;
    F(MenuAff).ToggleManage(MenuAff, MASK_PAGE_MARK, 
			    UNMASK_PAGE_MARK);
   }
  }
      
  if (DocSelected->BaseStd.cell_style_base->display_formula)
  {
   if (!ButtonMaskFormula)
   {
    ButtonMaskFormula = TRUE;
    F(MenuAff).ToggleManage(MenuAff, UNMASK_FORMULA, MASK_FORMULA);
   }
  }
  else
  {
   if (ButtonMaskFormula)
   {
    ButtonMaskFormula = FALSE;
    F(MenuAff).ToggleManage(MenuAff, MASK_FORMULA, UNMASK_FORMULA);
   }
  }
      
  if (DocSelected->BaseStd.cell_style_base->display_zero_cell)
  {
   if (!ButtonMaskZeroCell)
   {
    ButtonMaskZeroCell = TRUE;
    F(MenuAff).ToggleManage(MenuAff, UNMASK_ZERO_CELL, 
			    MASK_ZERO_CELL);
   }
  }
  else
  {
   if (ButtonMaskZeroCell)
   {
    ButtonMaskZeroCell = FALSE;
    F(MenuAff).ToggleManage(MenuAff, MASK_ZERO_CELL, 
			    UNMASK_ZERO_CELL);
   }
  }
      
  if (!DocSelected->Feuille->select_type 
      && DocSelected->Feuille->frmg->verrou )
  {
   if (ButtonLockFrame)
   {
    ButtonLockFrame = FALSE;
    XmToggleButtonSetState(WIDGET(IconGra_18), True, False);
   }
  }
  else
  {
   if (!ButtonLockFrame)
   {
    ButtonLockFrame = TRUE;
    XmToggleButtonSetState(WIDGET(IconGra_18), False, False);
   }
  }


  if (!DocSelected->Feuille->select_type 
      && DocSelected->Feuille->frmg->SelectedCadre 
      && !DocSelected->Feuille->frmg->SelectedCadre->NextCadre 
      && DocSelected->Feuille->frmg->SelectedCadre->cadre->object_type
      == CADRE_VECTOR 
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg->axe
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg->graduation_y)
  {
   if (ButtonMaskAxes)
   {
    ButtonMaskAxes = FALSE;
    F(MenuGra).ToggleManage(MenuGra, UNMASK_AXES, MASK_AXES);
    XmToggleButtonSetState(WIDGET(IconGra_12), True, False);
   }
  }
  else
  {
   if (!ButtonMaskAxes)
   {
    ButtonMaskAxes = TRUE; 
    F(MenuGra).ToggleManage(MenuGra, MASK_AXES, UNMASK_AXES);
    XmToggleButtonSetState(WIDGET(IconGra_12), False, False);
   }
  }


  if (!DocSelected->Feuille->select_type 
      && DocSelected->Feuille->frmg->SelectedCadre 
      && !DocSelected->Feuille->frmg->SelectedCadre->NextCadre 
      && DocSelected->Feuille->frmg->SelectedCadre->cadre->object_type
      == CADRE_VECTOR 
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg->legend
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg->d_legend)
  {
   if (ButtonMaskLegend)
   {
    ButtonMaskLegend = FALSE;
    F(MenuGra).ToggleManage(MenuGra, UNMASK_LEGENDE, MASK_LEGENDE);
    XmToggleButtonSetState(WIDGET(IconGra_13), True, False);
   }
  }
  else
  {
   if (!ButtonMaskLegend)
   {
    ButtonMaskLegend = TRUE; 
    F(MenuGra).ToggleManage(MenuGra, MASK_LEGENDE, UNMASK_LEGENDE);
    XmToggleButtonSetState(WIDGET(IconGra_13), False, False);
   }
  }
      
      

  if (!DocSelected->Feuille->select_type 
      && DocSelected->Feuille->frmg->SelectedCadre 
      && !DocSelected->Feuille->frmg->SelectedCadre->NextCadre 
      && DocSelected->Feuille->frmg->SelectedCadre->cadre->object_type
      == CADRE_VECTOR 
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg->title
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg->d_title)
  {
   if (ButtonMaskTitle)
   {
    ButtonMaskTitle = FALSE;
    F(MenuGra).ToggleManage(MenuGra, UNMASK_TITRE, MASK_TITRE);
    XmToggleButtonSetState(WIDGET(IconGra_14), True, False);
   }
  }
  else
  {
   if (!ButtonMaskTitle)
   {
    ButtonMaskTitle = TRUE; 
    F(MenuGra).ToggleManage(MenuGra, MASK_TITRE, UNMASK_TITRE);
    XmToggleButtonSetState(WIDGET(IconGra_14), False, False);
   }
  }


  if (!DocSelected->Feuille->select_type 
      && DocSelected->Feuille->frmg->SelectedCadre 
      && !DocSelected->Feuille->frmg->SelectedCadre->NextCadre 
      && DocSelected->Feuille->frmg->SelectedCadre->cadre->object_type
      == CADRE_VECTOR 
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg->abscisse
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg->d_abscisse)
  {
   if (ButtonMaskAbscisse)
   {
    ButtonMaskAbscisse = FALSE;
    F(MenuGra).ToggleManage(MenuGra, UNMASK_ABSCISSES,
			    MASK_ABSCISSES);
    XmToggleButtonSetState(WIDGET(IconGra_16), True, False);
   }
  }
  else
  {
   if (!ButtonMaskAbscisse)
   {
    ButtonMaskAbscisse = TRUE; 
    F(MenuGra).ToggleManage(MenuGra, MASK_ABSCISSES,
			    UNMASK_ABSCISSES);
    XmToggleButtonSetState(WIDGET(IconGra_16), False, False);
   }
  }


  if (!DocSelected->Feuille->select_type 
      && DocSelected->Feuille->frmg->SelectedCadre 
      && !DocSelected->Feuille->frmg->SelectedCadre->NextCadre 
      && DocSelected->Feuille->frmg->SelectedCadre->cadre->object_type
      == CADRE_VECTOR 
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg->ordonnee
      && ((c_VectorGraph *)
	  (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg->d_ordonnee
      &&  ((c_VectorGraph *)
	   (DocSelected->Feuille->frmg->SelectedCadre->cadre->object))
      ->highg->class != HG_RADAR)
  {
   if (ButtonMaskOrdonnee)
   {
    ButtonMaskOrdonnee = FALSE;
    F(MenuGra).ToggleManage(MenuGra, UNMASK_SSTITRES, MASK_SSTITRES);
    XmToggleButtonSetState(WIDGET(IconGra_17), True, False);
   }
  }
  else
  {
   if (!ButtonMaskOrdonnee)
   {
    ButtonMaskOrdonnee = TRUE; 
    F(MenuGra).ToggleManage(MenuGra, MASK_SSTITRES, UNMASK_SSTITRES);
    XmToggleButtonSetState(WIDGET(IconGra_17), False, False);
   }
  }

  if (DocSelected->Feuille->select_type)
  {
   CellStyle_t	*cstyle;

   cstyle = F(DocSelected->moteur).
    get_cell_style(DocSelected->moteur, 
		   DocSelected->Feuille->cell_x, 
		   DocSelected->Feuille->cell_y);



   if (FontSizeValue != cstyle->text_style->point_size)
   {
    FontSizeValue = cstyle->text_style->point_size;
    F(tfFontSize).set_value_in_unit(tfFontSize,
				    SCALE_TO_POINTS(FontSizeValue));
   }

   if (cstyle->format->special == XcFt_THOUSANDS)
   {
    if (!ButtonActivThousands)
    {
     XmToggleButtonSetState(WIDGET(IconCal_8), True, False);
     ButtonActivThousands = TRUE;
    }
   }
   else
   {
    if (ButtonActivThousands)
    {
     XmToggleButtonSetState(WIDGET(IconCal_8), False, False);
     ButtonActivThousands = FALSE;
    }
   }
   if (cstyle->format->special == XcFt_CURRENCY)
   {
    if (!ButtonActivCurrency)
    {
     XmToggleButtonSetState(WIDGET(IconCal_9), True, False);
     ButtonActivCurrency = TRUE;
    }
   }
   else
   {
    if (ButtonActivCurrency)
    {
     XmToggleButtonSetState(WIDGET(IconCal_9), False, False);
     ButtonActivCurrency = FALSE;
    }
   }
   if (cstyle->format->special == XcFt_PERCENT)
   {
    if (!ButtonActivPercent)
    {
     XmToggleButtonSetState(WIDGET(IconCal_10), True, False);
     ButtonActivPercent = TRUE;
    }
   }
   else
   {
    if (ButtonActivPercent)
    {
     XmToggleButtonSetState(WIDGET(IconCal_10), False, False);
     ButtonActivPercent = FALSE;
    }
   }
   if (cstyle->format->special == XcFt_SCIENTIFIC)
   {
    if (!ButtonActivScientific)
    {
     XmToggleButtonSetState(WIDGET(IconCal_11), True, False);
     ButtonActivScientific = TRUE;
    }
   }
   else
   {
    if (ButtonActivScientific)
    {
     XmToggleButtonSetState(WIDGET(IconCal_11), False, False);
     ButtonActivScientific = FALSE;
    }
   }
   if (cstyle->format->special == XcFt_ENGINEER)
   {
    if (!ButtonActivEngineer)
    {
     XmToggleButtonSetState(WIDGET(IconCal_12), True, False);
     ButtonActivEngineer = TRUE;
    }
   }
   else
   {
    if (ButtonActivEngineer)
    {
     XmToggleButtonSetState(WIDGET(IconCal_12), False, False);
     ButtonActivEngineer = FALSE;
    }
   }

   /* ---- Cells Borders Icons ---- */
   if (cstyle->left.border_thickness)
   {
    if (!ButtonActivBorderLeft)
    {
     XmToggleButtonSetState(WIDGET(IconBor_2), True, False);
     ButtonActivBorderLeft = TRUE;
    }	 
   }
   else
   {
    if (ButtonActivBorderLeft)
    {
     XmToggleButtonSetState(WIDGET(IconBor_2), False, False);
     ButtonActivBorderLeft = FALSE;
    }
   }
	  

   if (cstyle->down.border_thickness)
   {
    if (!ButtonActivBorderBottom)
    {
     XmToggleButtonSetState(WIDGET(IconBor_3), True, False);
     ButtonActivBorderBottom = TRUE;
    }	 
   }
   else
   {
    if (ButtonActivBorderBottom)
    {
     XmToggleButtonSetState(WIDGET(IconBor_3), False, False);
     ButtonActivBorderBottom = FALSE;
    }
   }
	  

   if (cstyle->right.border_thickness)
   {
    if (!ButtonActivBorderRight)
    {
     XmToggleButtonSetState(WIDGET(IconBor_4), True, False);
     ButtonActivBorderRight = TRUE;
    }	 
   }
   else
   {
    if (ButtonActivBorderRight)
    {
     XmToggleButtonSetState(WIDGET(IconBor_4), False, False);
     ButtonActivBorderRight = FALSE;
    }
   }
	  

   if (cstyle->up.border_thickness)
   {
    if (!ButtonActivBorderTop)
    {
     XmToggleButtonSetState(WIDGET(IconBor_5), True, False);
     ButtonActivBorderTop = TRUE;
    }	 
   }
   else
   {
    if (ButtonActivBorderTop)
    {
     XmToggleButtonSetState(WIDGET(IconBor_5), False, False);
     ButtonActivBorderTop = FALSE;
    }
   }
	  

   if ((cstyle->ruler_type & XqR_H_ALIGN_MASK) == XqR_H_ALIGN_LEFT)
   {
    if (!ButtonActivHAlignLeft)
    {
     XmToggleButtonSetState(WIDGET(IconCel_1), True, False);
     ButtonActivHAlignLeft = TRUE;
    }
   }
   else
   {
    if (ButtonActivHAlignLeft)
    {
     XmToggleButtonSetState(WIDGET(IconCel_1), False, False);
     ButtonActivHAlignLeft = FALSE;
    }
   }
	  
   if ((cstyle->ruler_type & XqR_H_ALIGN_MASK) == XqR_H_ALIGN_CENTER)
   {
    if (!ButtonActivHAlignCenter)
    {
     XmToggleButtonSetState(WIDGET(IconCel_2), True, False);
     ButtonActivHAlignCenter = TRUE;
    }
   }
   else
   {
    if (ButtonActivHAlignCenter)
    {
     XmToggleButtonSetState(WIDGET(IconCel_2), False, False);
     ButtonActivHAlignCenter = FALSE;
    }
   }
   if ((cstyle->ruler_type & XqR_H_ALIGN_MASK) == XqR_H_ALIGN_RIGHT)
   {
    if (!ButtonActivHAlignRight)
    {
     XmToggleButtonSetState(WIDGET(IconCel_3), True, False);
     ButtonActivHAlignRight = TRUE;
    }
   }
   else
   {
    if (ButtonActivHAlignRight)
    {
     XmToggleButtonSetState(WIDGET(IconCel_3), False, False);
     ButtonActivHAlignRight = FALSE;
    }
   }
   if ((cstyle->ruler_type & XqR_H_ALIGN_MASK) == XqR_H_JUSTIFIED)
   {
    if (!ButtonActivHAlignJustify)
    {
     XmToggleButtonSetState(WIDGET(IconCel_4), True, False);
     ButtonActivHAlignJustify = TRUE;
    }
   }
   else
   {
    if (ButtonActivHAlignJustify)
    {
     XmToggleButtonSetState(WIDGET(IconCel_4), False, False);
     ButtonActivHAlignJustify = FALSE;
    }
   }
	  
   if ((cstyle->ruler_type & XqR_V_ALIGN_MASK) == XqR_V_ALIGN_TOP)
   {
    if (!ButtonActivVAlignTop)
    {
     XmToggleButtonSetState(WIDGET(IconCel_8), True, False);
     ButtonActivVAlignTop = TRUE;
    }
   }
   else
   {
    if (ButtonActivVAlignTop)
    {
     XmToggleButtonSetState(WIDGET(IconCel_8), False, False);
     ButtonActivVAlignTop = FALSE;
    }
   }
   if ((cstyle->ruler_type & XqR_V_ALIGN_MASK) == XqR_V_ALIGN_BOTTOM)
   {
    if (!ButtonActivVAlignBottom)
    {
     XmToggleButtonSetState(WIDGET(IconCel_10), True, False);
     ButtonActivVAlignBottom = TRUE;
    }
   }
   else
   {
    if (ButtonActivVAlignBottom)
    {
     XmToggleButtonSetState(WIDGET(IconCel_10), False, False);
     ButtonActivVAlignBottom = FALSE;
    }
   }
   if ((cstyle->ruler_type & XqR_V_ALIGN_MASK) == XqR_V_JUSTIFIED)
   {
    if (!ButtonActivVAlignJustify)
    {
     XmToggleButtonSetState(WIDGET(IconCel_5), True, False);
     ButtonActivVAlignJustify = TRUE;
    }
   }
   else
   {
    if (ButtonActivVAlignJustify)
    {
     XmToggleButtonSetState(WIDGET(IconCel_5), False, False);
     ButtonActivVAlignJustify = FALSE;
    }
   }
   if (cstyle->ruler_type & XqR_MULTILINE)
   {
    if (!ButtonActivMultiLine)
    {
     XmToggleButtonSetState(WIDGET(IconCel_11), True, False);
     ButtonActivMultiLine = TRUE;
    }
   }
   else
   {
    if (ButtonActivMultiLine)
    {
     XmToggleButtonSetState(WIDGET(IconCel_11), False, False);
     ButtonActivMultiLine = FALSE;
    }
   }
	  
   if ((cstyle->ruler_type & XqR_A_MASK) == XqR_A_90)
   {
    if (!ButtonActivAngle90)
    {
     XmToggleButtonSetState(WIDGET(IconCel_13), True, False);
     ButtonActivAngle90 = TRUE;
    }
   }
   else
   {
    if (ButtonActivAngle90)
    {
     XmToggleButtonSetState(WIDGET(IconCel_13), False, False);
     ButtonActivAngle90 = FALSE;
    }
   }
   if ((cstyle->ruler_type & XqR_A_MASK) == XqR_A_270)
   {
    if (!ButtonActivAngle270)
    {
     XmToggleButtonSetState(WIDGET(IconCel_14), True, False);
     ButtonActivAngle270 = TRUE;
    }
   }
   else
   {
    if (ButtonActivAngle270)
    {
     XmToggleButtonSetState(WIDGET(IconCel_14), False, False);
     ButtonActivAngle270 = FALSE;
    }
   }


   /* ---- Fonts Icons ---- */

   attributes = cstyle->text_style->attributes;

   if ((attributes & XcTS_B_BOLD) == XcTS_B_BOLD)
   {
    if (!ButtonActivBold)
    {
     XmToggleButtonSetState(WIDGET(IconFon_3), True, False);
     ButtonActivBold = TRUE;
    }
   }
   else
   {
    if (ButtonActivBold)
    {
     XmToggleButtonSetState(WIDGET(IconFon_3), False, False);
     ButtonActivBold = FALSE;
    }
   }

   if ((attributes & XcTS_B_ITALIC) == XcTS_B_ITALIC)
   {
    if (!ButtonActivItalic)
    {
     XmToggleButtonSetState(WIDGET(IconFon_4), True, False);
     ButtonActivItalic = TRUE;
    }
   }
   else
   {
    if (ButtonActivItalic)
    {
     XmToggleButtonSetState(WIDGET(IconFon_4), False, False);
     ButtonActivItalic = FALSE;
    }
   }


   if ((attributes & XcTS_B_UNDERLINE) == XcTS_B_UNDERLINE)
   {
    if (!ButtonActivUnderline)
    {
     XmToggleButtonSetState(WIDGET(IconFon_5), True, False);
     ButtonActivUnderline = TRUE;
    }
   }
   else
   {
    if (ButtonActivUnderline)
    {
     XmToggleButtonSetState(WIDGET(IconFon_5), False, False);
     ButtonActivUnderline = FALSE;
    }
   }

   if ((attributes & XcTS_B_STRIKEOUT) == XcTS_B_STRIKEOUT)
   {
    if (!ButtonActivStrikeout)
    {
     XmToggleButtonSetState(WIDGET(IconFon_6), True, False);
     ButtonActivStrikeout = TRUE;
    }
   }
   else
   {
    if (ButtonActivStrikeout)
    {
     XmToggleButtonSetState(WIDGET(IconFon_6), False, False);
     ButtonActivStrikeout = FALSE;
    }
   }

   if ((attributes & XcTS_B_SHADOW) == XcTS_B_SHADOW)
   {
    if (!ButtonActivShadow)
    {
     XmToggleButtonSetState(WIDGET(IconFon_9), True, False);
     ButtonActivShadow = TRUE;
    }
   }
   else
   {
    if (ButtonActivShadow)
    {
     XmToggleButtonSetState(WIDGET(IconFon_9), False, False);
     ButtonActivShadow = FALSE;
    }
   }

   if ((attributes & XcTS_B_SMALL_CAPS) == XcTS_B_SMALL_CAPS)
   {
    if (!ButtonActivSmallCaps)
    {
     XmToggleButtonSetState(WIDGET(IconFon_11), True, False);
     ButtonActivSmallCaps = TRUE;
    }
   }
   else
   {
    if (ButtonActivSmallCaps)
    {
     XmToggleButtonSetState(WIDGET(IconFon_11), False, False);
     ButtonActivSmallCaps = FALSE;
    }
   }

   if ((attributes & XcTS_B_BIG_CAPS) == XcTS_B_BIG_CAPS)
   {
    if (!ButtonActivBigCaps)
    {
     XmToggleButtonSetState(WIDGET(IconFon_12), True, False);
     ButtonActivBigCaps = TRUE;
    }
   }
   else
   {
    if (ButtonActivBigCaps)
    {
     XmToggleButtonSetState(WIDGET(IconFon_12), False, False);
     ButtonActivBigCaps = FALSE;
    }
   }


   if (cstyle->text_style->font->spec_family != saved_family)
   {
    saved_family = cstyle->text_style->font->spec_family;
    F(IMfontFamily).setSelection(IMfontFamily, saved_family);
   }

   /*	  if (cstyle->text_style->color != 
	  F(CLfontColor).getColor(CLfontColor))
	  {
	  F(CLfontColor).setColorBase(CLfontColor, 
	  DocSelected->BaseStd.color_base,
	  CL_SKIP_TRANSPARENCY);
	  F(CLfontColor).setColor(CLfontColor, cstyle->text_style->color);
	  } */

   /* si la selection est 1 seule cellule */
   if (DocSelected->Feuille->nb_cell_region == 1 &&
       DocSelected->Feuille->cell_region->x_min == 
       DocSelected->Feuille->cell_region->x_max &&
       DocSelected->Feuille->cell_region->y_min == 
       DocSelected->Feuille->cell_region->y_max )
   {
    family_tree	*family;
	      
    family = cstyle->text_style->font->spec_family;
    if (ButtonActivBold)
    {
     if (ButtonActivItalic)
     {
      SensitivizeBold((family->can_normal
		       || family->can_italic) ? True: False);
      SensitivizeItalic((family->can_normal 
			 || family->can_bold) ? True: False);
     }
     else
     {
      SensitivizeBold((family->can_normal 
		       || family->can_bold_italic) 
		      ? True: False);
      SensitivizeItalic((family->can_italic 
			 || family->can_bold_italic) 
			? True: False);
     }
    }
    else
    {
     if (ButtonActivItalic)
     {
      SensitivizeBold((family->can_bold 
		       || family->can_bold_italic) 
		      ? True: False);
      SensitivizeItalic((family->can_normal 
			 || family->can_bold_italic) 
			? True: False);
     }
     else
     {
      SensitivizeBold((family->can_bold 
		       || family->can_bold_italic) 
		      ? True: False);
      SensitivizeItalic((family->can_italic 
			 || family->can_bold_italic) 
			? True: False);
     }
    }
   }
   else
   {
    SensitivizeBold(True);
    SensitivizeItalic(True);
   }

  }
  else 
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
   if (ButtonActivStrikeout)
   {
    XmToggleButtonSetState(WIDGET(IconFon_6), False, False);
    ButtonActivStrikeout = FALSE;
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
   if (ButtonActivBigCaps)
   {
    XmToggleButtonSetState(WIDGET(IconFon_12), False, False);
    ButtonActivBigCaps = FALSE;
   }
   if (ButtonActivBorderLeft)
   {
    XmToggleButtonSetState(WIDGET(IconBor_2), False, False);
    ButtonActivBorderLeft = FALSE;
   }
   if (ButtonActivBorderBottom)
   {
    XmToggleButtonSetState(WIDGET(IconBor_3), False, False);
    ButtonActivBorderBottom = FALSE;
   }
   if (ButtonActivBorderRight)
   {
    XmToggleButtonSetState(WIDGET(IconBor_4), False, False);
    ButtonActivBorderRight = FALSE;
   }
   if (ButtonActivBorderTop)
   {
    XmToggleButtonSetState(WIDGET(IconBor_5), False, False);
    ButtonActivBorderTop = FALSE;
   }
   if (ButtonActivHAlignLeft)
   {
    XmToggleButtonSetState(WIDGET(IconCel_1), False, False);
    ButtonActivHAlignLeft = FALSE;
   }
   if (ButtonActivHAlignCenter)
   {
    XmToggleButtonSetState(WIDGET(IconCel_2), False, False);
    ButtonActivHAlignCenter = FALSE;
   }
   if (ButtonActivHAlignRight)
   {
    XmToggleButtonSetState(WIDGET(IconCel_3), False, False);
    ButtonActivHAlignRight = FALSE;
   }
   if (ButtonActivHAlignJustify)
   {
    XmToggleButtonSetState(WIDGET(IconCel_4), False, False);
    ButtonActivHAlignJustify = FALSE;
   }
   if (ButtonActivVAlignTop)
   {
    XmToggleButtonSetState(WIDGET(IconCel_8), False, False);
    ButtonActivVAlignTop = FALSE;
   }
   if (ButtonActivVAlignBottom)
   {
    XmToggleButtonSetState(WIDGET(IconCel_10), False, False);
    ButtonActivVAlignBottom = FALSE;
   }
   if (ButtonActivVAlignJustify)
   {
    XmToggleButtonSetState(WIDGET(IconCel_5), False, False);
    ButtonActivVAlignJustify = FALSE;
   }
   if (ButtonActivMultiLine)
   {
    XmToggleButtonSetState(WIDGET(IconCel_11), False, False);
    ButtonActivMultiLine = FALSE;
   }
   if (ButtonActivAngle90)
   {
    XmToggleButtonSetState(WIDGET(IconCel_13), False, False);
    ButtonActivAngle90 = FALSE;
   }
   if (ButtonActivAngle270)
   {
    XmToggleButtonSetState(WIDGET(IconCel_14), False, False);
    ButtonActivAngle270 = FALSE;
   }
  }

  /* ----------------------------------------------------------------- ** 
  ** Main Event Loop for sensitivize                                   ** 
  ** ----------------------------------------------------------------- */

  if (!DocSelected->Feuille->frmg->ListCadre)
  {
   SensitivizeSelectFrames(False);
  }
  else
  {
   SensitivizeSelectFrames(True);
  }

      
  /*--- Si la selection est un simple ou multi de CELLULES ---*/
  if (DocSelected->Feuille->select_type)
  {
   SensitivizePaste(True);

   if (ButtonDeleteFrame)
   {
    ButtonDeleteFrame = FALSE;
    F(MenuEdi).ToggleManage(MenuEdi, EFFACER_CADRE, EFFACER);
   }
   /* griser les options de cadres */
   SensitivizeSimpleSelect(True);

   SensitivizeGraphTools(True);
   SensitivizeAxesConfig(False);
   SensitivizeLegendConfig(False);
   SensitivizeAbscisseConfig(False);
   SensitivizeOrdonneeConfig(False);
   SensitivizeTitleConfig(False);
   SensitivizeGraphFrameTools(False);
   SensitivizeFrameTools(False);

   /* activer menu format */
	  
   /* Copier vers ... */
   {
    region_t *region;
    boolean bDroitePossible = FALSE;
    boolean bBasPossible = FALSE;
	    
    region = DocSelected->Feuille->cell_region;
	    
    while (region)
    {
     if (!bDroitePossible)
      bDroitePossible =
       (region->x_min != region->x_max)
	&& (region->x_min != 0 || region->x_max != COLUMN_MAX);
		
     if (!bBasPossible)
      bBasPossible =
       (region->y_min != region->y_max)
	&& (region->y_min != 0 || region->y_max != ROW_MAX);
		
     region = region->NextRegion;
    }
    if (bDroitePossible)
    {
     SensitivizeCopyRight(True);
    }
    else 
    {
     SensitivizeCopyRight(False);
    }
    if (bBasPossible)
    {
     SensitivizeCopyDown(True);
    }
    else 
    {
     SensitivizeCopyDown(False);
    }
   }
	  
	  
   if (DocSelected->Feuille->nb_cell_region == 1)
    /*--- Si il y a une seule region de selectionnee ---*/
   {
    SensitivizeCutCopy(True);

    if (DocSelected->Feuille->nb_col_region)
    {
     if (!DocSelected->Feuille->nb_row_region)
     {
      SensitivizeRow(False);
      SensitivizeCol(True);
     }
     else
     {
      SensitivizeRow(False);
      SensitivizeCol(False);
     }
    }
    else
    {
     if (DocSelected->Feuille->nb_row_region)
     {
      SensitivizeRow(True);
      SensitivizeCol(False);
     }
     else
     {
      SensitivizeRow(True);
      SensitivizeCol(True);
     }
    }
   }
   else
    /*--- Si il y a plusieurs regions de selectionnees ---*/
   {
    SensitivizeCutCopy(False);
    SensitivizeRow(False);
    SensitivizeCol(False);
   }
  }
  else
  {
   /* Si la selection est un ou plusieurs CADRES ---*/
   if (DocSelected->Feuille->frmg->SelectedCadre)
   {
    SensitivizeFrameTools(True);
    if (!ButtonDeleteFrame)
    {
     ButtonDeleteFrame = TRUE;
     F(MenuEdi).ToggleManage(MenuEdi, EFFACER, EFFACER_CADRE);
    }
    SensitivizeSimpleSelect(False);
    SensitivizeCutCopy(False);
    SensitivizePaste(False);
    SensitivizeRow(False);
    SensitivizeCol(False);
    SensitivizeBold(False);
    SensitivizeItalic(False);
    SensitivizeCopyDown(False);
    SensitivizeCopyRight(False);

    /* griser les option de tableau */
	      
    /* Si la selection est MULTICADRE */
    if (DocSelected->Feuille->frmg->SelectedCadre->NextCadre)
    {
     SensitivizeAxesConfig(False);
     SensitivizeLegendConfig(False);
     SensitivizeAbscisseConfig(False);
     SensitivizeOrdonneeConfig(False);
     SensitivizeTitleConfig(False);
     SensitivizeGraphFrameTools(False);
     SensitivizeGraphTools(False);
    }
    else
     /* Si il n'y a k'un seul cadre selectionne */
    {
     /* Si c'est un cadre GRAPHEUR */
     if (DocSelected->Feuille->frmg->SelectedCadre->
	 cadre->object_type == CADRE_VECTOR 
	 && ((c_VectorGraph *)
	     (DocSelected->Feuille->frmg->SelectedCadre
	      ->cadre->object))->highg)
     {
      SensitivizeGraphFrameTools(True);
      SensitivizeGraphTools(True);
		      
      /* Si il y a des axes */
      if (((c_VectorGraph *)
	   (DocSelected->Feuille->frmg->SelectedCadre
	    ->cadre->object))->highg->axe)  
      {
       SensitivizeAxesConfig(True);
      }
      else
      {
       SensitivizeAxesConfig(False);
      }
		      
      /* Si il y a une legende */
      if (((c_VectorGraph *)
	   (DocSelected->Feuille->frmg->SelectedCadre
	    ->cadre->object))->highg->legend)  
      {
       SensitivizeLegendConfig(True);
      }
      else
      {
       SensitivizeLegendConfig(False);
      }
		      
      /* Si il y a un titre */
      if (((c_VectorGraph *)
	   (DocSelected->Feuille->frmg->SelectedCadre
	    ->cadre->object))->highg->title)  
      {
       SensitivizeTitleConfig(True);
      }
      else
      {
       SensitivizeTitleConfig(False);   
      }
      /* Si il y a une abscisse */
      if (((c_VectorGraph *)
	   (DocSelected->Feuille->frmg->SelectedCadre
	    ->cadre->object))->highg->abscisse)  
      {
       SensitivizeAbscisseConfig(True);
      }
      else
      {
       SensitivizeAbscisseConfig(False);   
      }
      /* Si il y a une ordonnee */
      if (((c_VectorGraph *)
	   (DocSelected->Feuille->frmg->SelectedCadre
	    ->cadre->object))->highg->ordonnee &&
	  ((c_VectorGraph *)
	   (DocSelected->Feuille->frmg->SelectedCadre
	    ->cadre->object))->highg->class != HG_RADAR)
      {
       SensitivizeOrdonneeConfig(True);
      }
      else
      {
       SensitivizeOrdonneeConfig(False);   
      }
     }
     else
      /* Si c'est pas un cadre GRAPHEUR */
     {
      SensitivizeAxesConfig(False);
      SensitivizeLegendConfig(False);
      SensitivizeAbscisseConfig(False);
      SensitivizeOrdonneeConfig(False);
      SensitivizeTitleConfig(False);
      SensitivizeGraphFrameTools(False);
      SensitivizeGraphTools(False);
     }
    }
   }  
  }
 }
 XtDispatchEvent(event);
}
/*---------------------------------------------------------------------*/

void SensitivizeCopyDown(flag)
Boolean flag;
{
 if (flag == (Boolean)CopyDownAlreadyGrised)
 {
  CopyDownAlreadyGrised = !CopyDownAlreadyGrised;
      
  F(MenuEdi).ButtonFlag(MenuEdi, COPIER_BAS, flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeCopyRight(flag)
Boolean flag;
{
 if (flag == (Boolean)CopyRightAlreadyGrised)
 {
  CopyRightAlreadyGrised = !CopyRightAlreadyGrised;
      
  F(MenuEdi).ButtonFlag(MenuEdi, COPIER_DROITE, flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizePaste(flag)
Boolean flag;
{
 if (flag == (Boolean)PasteAlreadyGrised)
 {
  PasteAlreadyGrised = !PasteAlreadyGrised;
      
  F(MenuEdi).ButtonFlag(MenuEdi, COLLER, flag);
  XtSetSensitive(WIDGET(IconMis_7), flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeSimpleSelect(flag)
Boolean flag;
{
 if (flag == (Boolean)SimpleSelectAlreadyGrised)
 {
  SimpleSelectAlreadyGrised = !SimpleSelectAlreadyGrised;
  XtSetSensitive(WIDGET(GlobEditBar->Position), flag);
  XtSetSensitive(WIDGET(GlobEditBar->Edit), flag);
  XtSetSensitive(WIDGET(GlobEditBar->Icon_Ok), flag);
  XtSetSensitive(WIDGET(GlobEditBar->Icon_Esc), flag);
  if (flag)
  {
   XmProcessTraversal(WIDGET(GlobEditBar->Edit), XmTRAVERSE_CURRENT);
  }

  XtSetSensitive(MenuFor->w_Cascade, flag);
  F(MenuSel).ButtonFlag(MenuSel, COLLER_FONCTION, flag);

  XtSetSensitive(WIDGET(IconCal_6), flag);
  XtSetSensitive(WIDGET(IconCal_15), flag);
  XtSetSensitive(WIDGET(IconCal_7), flag);
  XtSetSensitive(WIDGET(IconCal_8), flag);
  XtSetSensitive(WIDGET(IconCal_9), flag);
  XtSetSensitive(WIDGET(IconCal_10), flag);
  XtSetSensitive(WIDGET(IconCal_11), flag);
  XtSetSensitive(WIDGET(IconCal_12), flag);
  XtSetSensitive(WIDGET(IconCal_13), flag);
  XtSetSensitive(WIDGET(IconCal_14), flag);

  XtSetSensitive(WIDGET(IconFon_5), flag);
  XtSetSensitive(WIDGET(IconFon_6), flag);
  XtSetSensitive(WIDGET(IconFon_9), flag);
  XtSetSensitive(WIDGET(IconFon_11), flag);
  XtSetSensitive(WIDGET(IconFon_12), flag);
  XtSetSensitive(WIDGET(IMfontFamily), flag);
  XtSetSensitive(WIDGET(CLfontColor), flag);
  XtSetSensitive(WIDGET(sbFontSize), flag);

  XtSetSensitive(WIDGET(IconCel_1), flag);
  XtSetSensitive(WIDGET(IconCel_2), flag);
  XtSetSensitive(WIDGET(IconCel_3), flag);
  XtSetSensitive(WIDGET(IconCel_4), flag);
  XtSetSensitive(WIDGET(IconCel_5), flag);
  /*XtSetSensitive(WIDGET(IconCel_7), flag);*/
  XtSetSensitive(WIDGET(IconCel_8), flag);
  /*XtSetSensitive(WIDGET(IconCel_9), flag);*/
  XtSetSensitive(WIDGET(IconCel_10), flag);
  XtSetSensitive(WIDGET(IconCel_11), flag);
  /*XtSetSensitive(WIDGET(IconCel_12), flag);*/
  XtSetSensitive(WIDGET(CLfillColor), flag);
  XtSetSensitive(WIDGET(IconCel_13), flag);
  XtSetSensitive(WIDGET(IconCel_14), flag);

  XtSetSensitive(WIDGET(IconBor_1), flag);
  XtSetSensitive(WIDGET(IconBor_2), flag);
  XtSetSensitive(WIDGET(IconBor_3), flag);
  XtSetSensitive(WIDGET(IconBor_4), flag);
  XtSetSensitive(WIDGET(IconBor_5), flag);
  XtSetSensitive(WIDGET(IconBor_6), flag);
  XtSetSensitive(WIDGET(IconBor_7), flag);
  XtSetSensitive(WIDGET(IconBor_8), flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeCutCopy(flag)
Boolean flag;
{
 if (flag == (Boolean)CutCopyAlreadyGrised)
 {
  CutCopyAlreadyGrised = !CutCopyAlreadyGrised;
  F(MenuEdi).ButtonFlag(MenuEdi, COPIER, flag);
  F(MenuEdi).ButtonFlag(MenuEdi, COUPER, flag);
  XtSetSensitive(WIDGET(IconMis_5), flag);
  XtSetSensitive(WIDGET(IconMis_6), flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeRow(flag)
Boolean flag;
{
 if (flag == (Boolean)RowAlreadyGrised)
 {
  RowAlreadyGrised = !RowAlreadyGrised;
  F(MenuEdi).ButtonFlag(MenuEdi, INSERER_LIGNE, flag);
  F(MenuEdi).ButtonFlag(MenuEdi, DETRUIRE_LIGNE, flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeCol(flag)
Boolean flag;
{
 if (flag == (Boolean)ColAlreadyGrised)
 {
  ColAlreadyGrised = !ColAlreadyGrised;
  F(MenuEdi).ButtonFlag(MenuEdi, INSERER_COLONNE, flag);
  F(MenuEdi).ButtonFlag(MenuEdi, DETRUIRE_COLONNE, flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeSelectFrames(flag)
Boolean flag;
{
 if (flag == (Boolean)SelectFramesAlreadyGrised)
 {
  SelectFramesAlreadyGrised = !SelectFramesAlreadyGrised;
  F(MenuSel).ButtonFlag(MenuSel, TOUT_SELECTIONNER_CADRES, flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeFrameTools(flag)
Boolean flag;
{
 if (flag == (Boolean)FrameToolsAlreadyGrised)
 {
  FrameToolsAlreadyGrised = !FrameToolsAlreadyGrised;
  XtSetSensitive(WIDGET(IconGra_2), flag);
  XtSetSensitive(WIDGET(IconGra_3), flag);
  XtSetSensitive(WIDGET(IconGra_4), flag);
  XtSetSensitive(WIDGET(IconGra_5), flag);
  XtSetSensitive(WIDGET(IconGra_18), flag);
  XtSetSensitive(WIDGET(IconTra), flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeGraphTools(flag)
Boolean flag;
{
 if (flag == (Boolean)GraphToolsAlreadyGrised)
 {
  GraphToolsAlreadyGrised = !GraphToolsAlreadyGrised;
  XtSetSensitive(WIDGET(IconGra_6), flag);
  XtSetSensitive(WIDGET(IconGra_7), flag);
  XtSetSensitive(WIDGET(IconGra_8), flag);
  XtSetSensitive(WIDGET(IconGra_9), flag);
  XtSetSensitive(WIDGET(IconGra_10), flag);
  XtSetSensitive(WIDGET(IconGra_11), flag);
  XtSetSensitive(MenuGra->w_Cascade, flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeTitleConfig(flag)
Boolean flag;
{
 if (flag == (Boolean)TitleConfigAlreadyGrised)
 {
  TitleConfigAlreadyGrised = !TitleConfigAlreadyGrised;
  F(MenuGra).ButtonFlag(MenuGra, MASK_TITRE, flag);
  F(MenuGra).ButtonFlag(MenuGra, UNMASK_TITRE, flag);
  XtSetSensitive(WIDGET(IconGra_14), flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeAxesConfig(flag)
Boolean flag;
{
 if (flag == (Boolean)AxesConfigAlreadyGrised)
 {
  AxesConfigAlreadyGrised = !AxesConfigAlreadyGrised;
  F(MenuGra).ButtonFlag(MenuGra, MASK_AXES, flag);
  F(MenuGra).ButtonFlag(MenuGra, UNMASK_AXES, flag);
  XtSetSensitive(WIDGET(IconGra_12), flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeLegendConfig(flag)
Boolean flag;
{
 if (flag == (Boolean)LegendConfigAlreadyGrised)
 {
  LegendConfigAlreadyGrised = !LegendConfigAlreadyGrised;
  F(MenuGra).ButtonFlag(MenuGra, MASK_LEGENDE, flag);
  F(MenuGra).ButtonFlag(MenuGra, UNMASK_LEGENDE, flag);
  XtSetSensitive(WIDGET(IconGra_13), flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeAbscisseConfig(flag)
Boolean flag;
{
 if (flag == (Boolean)AbscisseConfigAlreadyGrised)
 {
  AbscisseConfigAlreadyGrised = !AbscisseConfigAlreadyGrised;
  F(MenuGra).ButtonFlag(MenuGra, MASK_ABSCISSES, flag);
  F(MenuGra).ButtonFlag(MenuGra, UNMASK_ABSCISSES, flag);
  XtSetSensitive(WIDGET(IconGra_16), flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeOrdonneeConfig(flag)
Boolean flag;
{
 if (flag == (Boolean)OrdonneeConfigAlreadyGrised)
 {
  OrdonneeConfigAlreadyGrised = !OrdonneeConfigAlreadyGrised;
  F(MenuGra).ButtonFlag(MenuGra, MASK_SSTITRES, flag);
  F(MenuGra).ButtonFlag(MenuGra, UNMASK_SSTITRES, flag);
  XtSetSensitive(WIDGET(IconGra_17), flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeGraphFrameTools(flag)
Boolean flag;
{
 if (flag == (Boolean)GraphFrameToolsAlreadyGrised)
 {
  GraphFrameToolsAlreadyGrised = !GraphFrameToolsAlreadyGrised;
  F(MenuGra).ButtonFlag(MenuGra, G_CONFIGURATION, flag);
  XtSetSensitive(WIDGET(IconGra_15), flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeBold(flag)
Boolean flag;
{
 if (flag == (Boolean)BoldAlreadyGrised)
 {
  BoldAlreadyGrised = !BoldAlreadyGrised;
  XtSetSensitive(WIDGET(IconFon_3), flag);
 }
}
void SensitivizeItalic(flag)
Boolean flag;
{
 if (flag == (Boolean)ItalicAlreadyGrised)
 {
  ItalicAlreadyGrised = !ItalicAlreadyGrised;
  XtSetSensitive(WIDGET(IconFon_4), flag);
 }
}
/*---------------------------------------------------------------------*/
void SensitivizeDoc(flag)
Boolean flag;
{
 if (flag == (Boolean)DocAlreadyGrised)
 {
  DocAlreadyGrised = !DocAlreadyGrised;
  XtSetSensitive(MenuEdi->w_Cascade, flag);
  XtSetSensitive(MenuSel->w_Cascade, flag);
  XtSetSensitive(MenuAff->w_Cascade, flag);
  XtSetSensitive(MenuFen->w_Cascade, flag);
      
  XtSetSensitive(WIDGET(IconMis_3), flag);
  XtSetSensitive(WIDGET(IconMis_4), flag);
      
  F(MenuFic).ButtonFlag(MenuFic, FERMER, flag);
  F(MenuFic).ButtonFlag(MenuFic, MISEENPAGE, flag);
  F(MenuFic).ButtonFlag(MenuFic, SAUVER, flag);
  F(MenuFic).ButtonFlag(MenuFic, SAUVER_SOUS, flag);
  F(MenuFic).ButtonFlag(MenuFic, TOUT_SAUVER, flag);
  F(MenuFic).ButtonFlag(MenuFic, EXPORTER, flag);
  F(MenuFic).ButtonFlag(MenuFic, IMPRIMER, flag);

  XtSetSensitive(WIDGET(IconGra_0), flag);
  XtSetSensitive(WIDGET(IconGra_1), flag);
  /* Loupes */
  XtSetSensitive(WIDGET(IconZoo_1), flag);
  XtSetSensitive(WIDGET(IconZoo_2), flag);
  XtSetSensitive(WIDGET(IconZoo_3), flag);
  XtSetSensitive(WIDGET(IconZoo_4), flag);
  XtSetSensitive(WIDGET(IconZoo_5), flag);
  XtSetSensitive(WIDGET(IconZoo_6), flag);
  XtSetSensitive(WIDGET(IconZoo_7), flag);
 }
}

/*---------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------*/
void DisplayBottomBar()
{
 if (ButtonMaskBottomBar == TRUE)
 {
  XtUnmanageChild(WIDGET(BottomBar));
  F(MainInterface1).attach_bottom(MainInterface1, WIDGET(vIconBar), NULL);
  F(MainInterface1).attach_bottom(MainInterface1, WIDGET(MainDocument),
				  NULL);
  
  F(MenuAff).ToggleManage(MenuAff, MASK_BOTTOMBAR, UNMASK_BOTTOMBAR);
  ButtonMaskBottomBar = FALSE;
 }
 else
 {
  XtManageChild(WIDGET(BottomBar));
  F(MainInterface1).attach_bottom(MainInterface1, WIDGET(vIconBar), 
				  WIDGET(BottomBar));
  F(MainInterface1).attach_bottom(MainInterface1, WIDGET(MainDocument),
				  WIDGET(BottomBar));
  
  F(MenuAff).ToggleManage(MenuAff, UNMASK_BOTTOMBAR, MASK_BOTTOMBAR);
  ButtonMaskBottomBar = TRUE;
 }
}


/*************************************/
/* This reference the managed Window */
/*************************************/

void MenuFileNew()
{
 Display	*display;
 c_Tableau	*TabTmp;
 char		*doc_name;

 display = XtDisplay(WIDGET(MainDocument));
 doc_name = F(DocList).Get_NextName(DocList);

 /**************************/
 /* Creation d'un document */
 /**************************/
 TabTmp = (c_Tableau *)NEW(c_Tableau)();
  
 /****************/
 /* Init Tableau */
 /****************/
 F(TabTmp).Init_Tableau(TabTmp,
			XtDisplay(WIDGET(MainDocument)),
			XtWindow(WIDGET(MainDocument)),
			TRUE, doc_name);
 /*******************/
 /* Manage document */
 /*******************/
 Xc_TRACE(("add"));
 F(Gwm).SetWindowName(Gwm, display, TabTmp->window, doc_name);
 F(DocList).Add(DocList, TabTmp, DOCUMENT_TABLEUR, doc_name);
 Xc_free(doc_name);
 Xc_TRACE(("manage"));
 F(Gwm).ManageWin(Gwm, display, TabTmp->window,
		  F(GlobDocIcon).get_pixmap(GlobDocIcon, Icon_TABLEUR));
}


/* ----------------------------------------------------------------- ** 
** MenuFileOpen - Open new document                                  ** 
** ----------------------------------------------------------------- */
void MenuFileOpen(w, filen)
Widget	w;     
char	*filen;     
{
 char *filename;
 c_DocFile *doc;
  
 if (filen)
 {
  if (!IsFileExist(filen))
  {
   filename = (char *)Xc_malloc("filen", strlen(filen) + 4);
	  
   strcpy(filename, filen);
   strcat(filename, ".xq");
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
  Xc_TRACE(("open"));
  if ((OpenDocumentSelector == NULL) &&
      (CreateOpenDocumentSelector(MainInterface1->w_Main) == FALSE))
   return;
  filename = F(OpenDocumentSelector).getFile(OpenDocumentSelector);
 }
 if (filename == NULL) return;
  
 if ((doc = NEW(c_DocFile)(XcDF_LOAD, filename, -1)) != NULL)
 {
  F(doc).addCallbacks(doc,
		      XcDF_DOCUMENT_KEYWORD,
		      FUNCTION(c_Tableau, read_document), filename,
		      NULL);
  SET_WAIT_MODE_ANIM;
  if (F(doc).expectKeyword(doc, XcDF_DOCUMENT_KEYWORD, FALSE) == TRUE)
  {
   if (doc->data_type == XcDF_DOCUMENT_TYPE)
   {
    c_Tableau *sheet;

    Xc_TRACE(("Document loaded"));
	      
    /*--- Do not free filename ---*/
    sheet = doc->data_ptr;
    sheet->filename = filename;

    /*--- Initialize document ---*/
    UNSET_WAIT_MODE_ANIM;
    SET_WAIT_MODE_STATIC;
    F(sheet).Init_Tableau(sheet,
			  XtDisplay(WIDGET(MainDocument)),
			  XtWindow(WIDGET(MainDocument)),
			  TRUE, NULL);

    /*--- Manage Doument ---*/
    F(DocList).Add(DocList, sheet, DOCUMENT_TABLEUR, sheet->name);
	      
    Xc_TRACE(("Set Window Name"));
    F(Gwm).SetWindowName(Gwm, sheet->display,
			 sheet->window, sheet->name);

    Xc_TRACE(("Manage window"));
    F(Gwm).ManageWin(Gwm, sheet->display, sheet->window,
		     F(GlobDocIcon).get_pixmap(GlobDocIcon,
					       Icon_TABLEUR));
	      
    DELETE(c_DocFile)(doc);

    UNSET_WAIT_MODE_STATIC;
    Xc_TRACE(("ok"));
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
   UNSET_WAIT_MODE_STATIC;  Xc_free(error_msg);
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
 Xc_TRACE(("error"));
}

void _SaveModif(Document)
c_Tableau	*Document;
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
 c_Tableau	*Document;
 int		Type;

 Xc_TRACE(("close window"));
 display = XtDisplay(WIDGET(MainInterface1));
 listegwm = Gwm->SelectedWindow;
 if (Gwm->SelectedWindow != NULL)
 {
  Document = F(DocList).Find(DocList,
			     Gwm->SelectedWindow->MainWin, &Type);
  if (Type == DOCUMENT_TABLEUR && Document->modified)
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
   F(GlobEditBar).clear_bar(GlobEditBar);
  }
  SET_WAIT_MODE_STATIC;
  F(DocList).DelId(DocList, listegwm->MainWin);
  UNSET_WAIT_MODE_STATIC;
  F(Gwm).CloseWindow(Gwm, listegwm, display);
 }
}


/* ----------------------------------------------------------------- ** 
** MenuFileSave - Save document (ask filename if unknown)            ** 
** ----------------------------------------------------------------- */
void MenuFileSave()
{
 c_DocFile *doc;
 c_Tableau *sheet;

 Xc_TRACE(("save"));

 sheet = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, NULL);
 if (sheet->filename == NULL)
 {
  MenuFileSaveas();
  return;
 }

 SET_WAIT_MODE_ANIM;
 if ((doc = NEW(c_DocFile)(XcDF_SAVE, sheet->filename, -1)) != NULL)
 {
  if (WRITE_DOCUMENT(sheet)(sheet, doc) == FALSE)
   Xc_ERROR(("save error"));
  DELETE(c_DocFile)(doc);
 }
 UNSET_WAIT_MODE_ANIM;
}


/* ----------------------------------------------------------------- ** 
** MenuFileSaveas - Ask filename and save document                   ** 
** ----------------------------------------------------------------- */
void MenuFileSaveas()
{
 char *filename;
 c_DocFile *doc;

 Xc_TRACE(("save as"));
  
 if((CloseDocumentSelector == NULL) &&
    (CreateCloseDocumentSelector(MainInterface1->w_Main) == FALSE)) return;
 filename = F(CloseDocumentSelector).getFile(CloseDocumentSelector);
 if(filename == NULL) return;

 SET_WAIT_MODE_ANIM;
 if((doc = NEW(c_DocFile)(XcDF_SAVE, filename, -1)) != NULL)
 {
  c_Tableau *sheet;

  sheet = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, NULL);
      
  if(sheet->filename != NULL) Xc_free(sheet->filename);

  /*--- DO NOT FREE `filename' !!! ---*/
  sheet->filename = filename;
      
  if (strlen(GetBasename(filename, FALSE))-3>XcMAX_NAME_LENGTH-1)
  {
   memcpy(sheet->name, GetBasename(filename, FALSE),
	  XcMAX_NAME_LENGTH - 1);
   sheet->name[XcMAX_NAME_LENGTH - 1] = '\0';
  }
  else
  {
   memcpy(sheet->name, GetBasename(filename, FALSE),
	  strlen(GetBasename(filename, FALSE) - 3));
   sheet->name[strlen(GetBasename(filename, FALSE)) - 3] = '\0';
  }
  F(Gwm).SetWindowName(Gwm, sheet->display, sheet->window, sheet->name);
  F(DocList).ChangeDocName(DocList, (void *)sheet, sheet->name);
      
  if (WRITE_DOCUMENT(sheet)(sheet, doc) == FALSE)
   Xc_ERROR(("Save error"));

  DELETE(c_DocFile)(doc);
 }
 UNSET_WAIT_MODE_ANIM;
}

/*---------------------------------------------------------------------*/
void MenuFileSaveall()
{
 c_Tableau    *Document;
 listdocument *tmplist = NULL;

 Xc_TRACE(("save all"));

 tmplist = DocList->ListDocument;
 while (tmplist)
 {
  Xc_TRACE(("saving: %s", tmplist->Name));
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

/*---------------------------------------------------------------------*/
char *extract_normal_string(cs)
XmString cs;
{

 XmStringContext context;
 XmStringCharSet charset;
 XmStringDirection direction;
 Boolean separator;
 static char *primitive_string;

 XmStringInitContext(&context, cs);
 XmStringGetNextSegment(context, &primitive_string,
			&charset, &direction, &separator);
 XmStringFreeContext (context);
 return ((char *)primitive_string);
}

/* ----------------------------------------------------------------- ** 
** MenuFileAcquireDoc - Acquire document                             ** 
** ----------------------------------------------------------------- */
void MenuFileAcquireDoc()
{
 void		 *doc;
 int		Type;	
 c_Tableau	*document;
  
 Xc_TRACE(("Import Document"));
 SET_WAIT_MODE_STATIC;
 if (Gwm->SelectedWindow == NULL)
 {
  F(GlobImport).import(GlobImport, NULL);
 }
 else
 {
  doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
      
  if (Type == DOCUMENT_TABLEUR && doc != NULL)
  {
   document = (c_Tableau *)doc;
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
** MenuFileExportText - Exort document in Text Format                ** 
** ----------------------------------------------------------------- */
void MenuFileExportText()
{
 void		 *doc;
 int		Type;	
 c_Tableau	*document;
  
 Xc_TRACE(("Export Document in Text Format"));
 SET_WAIT_MODE_STATIC;
 if (Gwm->SelectedWindow == NULL)
 {
  Xc_WARNING(("No document selected"));
 }
 else
 {
  doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
      
  if (Type == DOCUMENT_TABLEUR && doc != NULL)
  {
   document = (c_Tableau *)doc;
   F(GlobExport).export(GlobExport, document);
  }
  else
  {
   F(GlobExport).export(GlobExport, NULL);
  }
 }
 UNSET_WAIT_MODE;
}

extern cell voidcell;
#include "encodings.h"
/* ----------------------------------------------------------------- ** 
** MenuFileExportHTML - Exort document in HTML Format                ** 
** ----------------------------------------------------------------- */
void MenuFileExportHTML()
{
 c_Tableau *sheet;
 char *filename;
 int s_type;

 Xc_TRACE(("Export Document in HTML format"));

 if((ExportHTMLSelector == NULL) &&
    (CreateExportHTMLSelector(MainInterface1->w_Main) == FALSE))
  return;
 filename = F(ExportHTMLSelector).getFile(ExportHTMLSelector);
 if(filename == NULL) return;

 sheet = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &s_type);
 if(s_type == DOCUMENT_TABLEUR)
 {
  FILE	*file;
  c_Moteur	*moteur;
  region_t	region;
  long int	rx;
  long int	ry;
  char	str[Xq_FORMULA_LENGTH];
  int	type;
  cell	*tmp_cell;
  char	*str2;
  boolean	border;
  boolean	header;
  int	value_type;
  char	head_str[10];
  CellStyle_t	*style;
  conv_info_t	conv_info;
  int		pos_title;

  SET_WAIT_MODE_ANIM;

  border=TRUE;
  header=FALSE;
  type = Xq_VALUE;

  if (!(file = fopen(filename,"w+")))
  {
   Xc_WARNING(("file %s doesn't exist or unreadable", filename));
   return;
  }

  pos_title=strlen(filename);
  while(filename[pos_title]!='/' && pos_title)
  {
   pos_title--;
  }
  if (filename[pos_title]=='/')
   pos_title++;

  moteur = sheet->moteur;
  /* clip region */
  F(moteur).getBoundingRegion(moteur, &region);
  /*BORDERWIDTH=100*/
  if (border)
  {
   fputs("<HTML><BODY>\n<!-- Generated by XQuad (c)1995-1997, AXENE. -->\n<HEAD>\n<TITLE>",file);
   fputs(filename+pos_title, file);
   fputs("</TITLE>\n</HEAD>\n<TABLE BORDER BGCOLOR=#FFFFFF><CAPTION ALIGN=bottom><FONT SIZE=-1>",file);
   fputs(filename+pos_title, file);
   fputs("</FONT></CAPTION>\n", file);
  }
  else
   fputs("<HTML><BODY>\n<TABLE BGCOLOR=#FFFFFF>\n", file);

  if (header)
  {
   rx = region.x_min;
   fputs("<TR>\n", file);
   fputs("<TD></TD>\n", file);
   while (rx <= region.x_max)
   {
    fputs("<TH ALIGN=center>", file);
    coltostr(rx, NULL, head_str);
    fputs(head_str, file);
    fputs("</TH>\n", file);
    rx++;
   }
   fputs("</TR>\n", file);
  }

  tmp_cell = NULL;
  ry = region.y_min;
  while (ry <= region.y_max)
  {
   fputs("<TR>\n", file);
   if (header)
   {
    fputs("<TH ALIGN=center>", file);
    sprintf(head_str,"%ld", ry);
    fputs(head_str, file);
    fputs("</TH>\n", file);
   }
   rx = region.x_min;
   while (rx <= region.x_max)
   {
    tmp_cell = F(moteur).get_cell(moteur, rx, ry);
    if (tmp_cell != &voidcell)
    {
     style = F(moteur).get_cell_style(moteur, rx, ry);
     value_type = F(moteur).get_string(moteur, tmp_cell,
				       str, type);
     F(style->format).convert_string(style->format, str,
				     value_type,
				     &conv_info);
     /* translate */
     str2 = convert_to_encoding(str, XcE_HTML);
     fprintf(file, "<TD");
     switch (style->ruler_type & XqR_H_ALIGN_MASK)
     {
     case XqR_DEFAULT:
      if (Xq_IS_VALUE(value_type))
      {
       if (Xq_IS_BOOLEAN(value_type))
	fputs(" ALIGN=center", file);
       else
	fputs(" ALIGN=right", file);
      }
      break;
     case XqR_H_ALIGN_LEFT:
      break;
     case XqR_H_ALIGN_RIGHT:
      fprintf(file, " ALIGN=right");
      break;
     case XqR_H_ALIGN_CENTER:
     case XqR_H_JUSTIFIED:
      fprintf(file, " ALIGN=center");
      break;
     }
     switch (style->ruler_type & XqR_V_ALIGN_MASK)
     {
     case XqR_V_ALIGN_TOP:
      fprintf(file, " VALIGN=top");
      break;
     case XqR_V_ALIGN_BOTTOM:
      fprintf(file, " VALIGN=bottom");
      break;
     case XqR_V_ALIGN_CENTER:
      break;
     }
     fputs(">", file);
     fputs(str2, file);
     Xc_free(str2);
    }
    else
     fputs("<TD>&nbsp;", file);

    fputs("</TD>\n", file);
    rx++;
   }
   ry++;
   fputs("</TR>\n", file);
  }
  fputs("</TABLE>\n</BODY></HTML>", file);
  fclose(file);
  UNSET_WAIT_MODE_ANIM;
 }
 Xc_free(filename);
}

/* ----------------------------------------------------------------- ** 
** MenuFileExportVector - Exort document in Vector Format            ** 
** ----------------------------------------------------------------- */
void MenuFileExportVector()
{
 c_PostScript *print;
 c_Tableau *sheet;
 char *filename;
 int type;

 Xc_TRACE(("Export Document in Vector format"));

 if((ExportVectorSelector == NULL) &&
    (CreateExportVectorSelector(MainInterface1->w_Main) == FALSE))
  return;
 filename = F(ExportVectorSelector).getFile(ExportVectorSelector);
 if(filename == NULL) return;
  
 sheet = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &type);
 if(type == DOCUMENT_TABLEUR)
 {
  SET_WAIT_MODE_ANIM;
  print = NEW(c_PostScript)(NULL, filename, -1);
  if(print != NULL)
  {
   print->print_param = &GlobPrintParam;
   PS_PRINT_DOCUMENT(sheet)(sheet, print);
   DELETE(c_PostScript)(print);
  }
  UNSET_WAIT_MODE_ANIM;
 }
 Xc_free(filename);
}

/*---------------------------------------------------------------------*/
void MenuFilePrintSetup()
{
 void		 *doc;
 c_Box_miseenpage *box;
 int		Type;	
 c_Tableau	*document;
 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (Type == DOCUMENT_TABLEUR)
 {
  document = (c_Tableau *)doc;
  SET_WAIT_MODE_STATIC;
  box = (c_Box_miseenpage *)NEW(c_Box_miseenpage)
   (MainInterface1->w_Main, "BoxPrintSetup", 
    document->Feuille->print_setup);
  switch(F(box->Dialog).waitForUnmap(box->Dialog))
  {
  case XcCD_DESTROY:
  case XcCD_CANCEL:
   DELETE(c_Box_miseenpage)(box);
   break;
  case XcCD_OK:
   DELETE(c_Box_miseenpage)(box);
   if (!document->Feuille->page_mark)
   {
    F(document->Feuille).CalculPageWindow(document->Feuille, FALSE);
    SET_FUNCTION_DOCUMENT(doc)(doc, DISPLAY_PAGE_MARK);
   }
   else
   {
    F(document->Feuille).CalculPageWindow(document->Feuille, TRUE);
    SET_FUNCTION_DOCUMENT(doc)(doc, REDRAW_ALL);
   }
   document->modified = TRUE;
   break;
  }
  UNSET_WAIT_MODE;
 }
}


/* ----------------------------------------------------------------- ** 
** MenuFilePrint - Print dialog & print                              ** 
** ----------------------------------------------------------------- */
void MenuFilePrint()
{
 c_PostScript	*print;
 c_Box_imprimer *box;
 printer_t 	*printer;
 c_Tableau 	*sheet;
 FILE 		*handle;
 void 		*doc;
 int 		Type, rc;
 region_t	max_reg;
 unsigned int	wp, hp;

 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);

 if (Type == DOCUMENT_TABLEUR)
 {
  sheet = doc;
  F(sheet->Feuille).getBoundingRegion(sheet->Feuille, &max_reg);
  F(sheet->Feuille).getPagesCount(sheet->Feuille, &max_reg, &wp, &hp);
  GlobPrintParam.NbPage = wp * hp;
      
  if (GlobPrintParam.AllPages)
   GlobPrintParam.EndPage = GlobPrintParam.NbPage;
  SET_WAIT_MODE_STATIC;
  box = (c_Box_imprimer *)NEW(c_Box_imprimer)(MainInterface1->w_Main, 
					      "BoxPrint",
					      &GlobPrintParam);
      
  rc = F(box->Dialog).waitForUnmap(box->Dialog);
  DELETE(c_Box_imprimer)(box);
  if(rc != XcCD_OK)
  {
   SelectFunction(WIDGET(MainInterface1), REDRAW_ALL);
   UNSET_WAIT_MODE;
   return;
  }
      
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
     SET_WAIT_MODE_ANIM;
     print = NEW(c_PostScript)(&GlobPrintParam, filename, -1);
     if(print != NULL)
     {
      PS_PRINT_DOCUMENT(sheet)(sheet, print);
      DELETE(c_PostScript)(print);
     }
     UNSET_WAIT_MODE_ANIM;
     Xc_free(filename);
    }
   }
  }
  else if((handle = popen(printer->command, "w")) != NULL)
  {
   SET_WAIT_MODE_ANIM;
   print = NEW(c_PostScript)(&GlobPrintParam, printer->name,
			     fileno(handle));
   if(print != NULL)
   {
    PS_PRINT_DOCUMENT(sheet)(sheet, print);
    DELETE(c_PostScript)(print);
   }
   UNSET_WAIT_MODE_ANIM;
   pclose(handle);
  }
 }
 SelectFunction(WIDGET(MainInterface1), REDRAW_ALL);
 UNSET_WAIT_MODE;
}

/*---------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------*/
void QuitApplication()
{
 int	i;

 Xc_TRACE(("Quit XQuad"));
  
 handle_event_allowed = FALSE;
  
 F(MainInterface1).unmap_TopLevel(MainInterface1);
 WriteUserConfigFile();

#ifdef SIGNALH
 DELETE(c_SignalHandler)(SignalH);
#endif
 DELETE(c_DocumentList)(DocList);
  
 DELETE(c_FontManager)(GlobFntMng);

 while(GlobFormatBase != NULL) DELETE(c_FormatNbr)(GlobFormatBase);
 while(GlobTextStyleBase != NULL) DELETE(c_TextStyle)(GlobTextStyleBase);
 while(GlobFontBase != NULL) DELETE(c_VectorFont)(GlobFontBase);
 while(GlobColorBase != NULL) DELETE(c_Color)(GlobColorBase);
 while(GlobColorAuto != NULL) DELETE(c_Color)(GlobColorAuto);
  
 DELETE(c_WinManager)(Gwm, XtDisplay(WIDGET(MainInterface1)));
 DELETE(c_Colormap)(GlobColormap);
 DELETE(c_DocIcon)(GlobDocIcon);
 DELETE(c_ImageManager)(GlobImgMng);
 DELETE(c_Trash)(GlobTrash);
 DELETE(c_DragAndDrop)(GlobDragAndDrop);
 DELETE(c_XErrorHandler)(XErrH);
 DELETE(c_RegisterHelp)(GlobRegHelp);
  
 if(t1imager_buffer) Xc_free(t1imager_buffer);
 if (Globprog_name) Xc_free(Globprog_name);
  
 /*--- File selectors ---*/
 if(OpenDocumentSelector != NULL)
  DELETE(c_FileSelector)(OpenDocumentSelector);
 if(CloseDocumentSelector != NULL)
  DELETE(c_FileSelector)(CloseDocumentSelector);
 if(PrintDocumentSelector != NULL)
  DELETE(c_FileSelector)(PrintDocumentSelector);
 if(ExportVectorSelector != NULL)
  DELETE(c_FileSelector)(ExportVectorSelector);
 if(ImportDocumentSelector != NULL)
  DestroyImportDocumentSelector();
 if(ExportTextSelector != NULL)
  DestroyExportTextSelector();
 if(ExportHTMLSelector != NULL)
  DELETE(c_FileSelector)(ExportHTMLSelector);

 DELETE(c_Import)(GlobImport);
 DELETE(c_Export)(GlobExport);
  
 /* ----------------------------------------------------------------- ** 
 ** interface                                                         ** 
 ** ----------------------------------------------------------------- */

 DELETE(c_Icon)(IconMis_1); DELETE(c_Icon)(IconMis_2);
 DELETE(c_Icon)(IconMis_s1); DELETE(c_Icon)(IconMis_3);
 DELETE(c_Icon)(IconMis_4); DELETE(c_Icon)(IconMis_s2);
 DELETE(c_Icon)(IconMis_5); DELETE(c_Icon)(IconMis_6);
 DELETE(c_Icon)(IconMis_7);  DELETE(c_Icon)(IconMis_8);
/*DELETE(c_Icon)(IconMis_9); DELETE(c_Icon)(IconMis_10);*/

 DELETE(c_Icon)(IconCal_15); DELETE(c_Icon)(IconCal_6);
 DELETE(c_Icon)(IconCal_7); DELETE(c_Icon)(IconCal_s3);
 DELETE(c_Icon)(IconCal_8); DELETE(c_Icon)(IconCal_9);
 DELETE(c_Icon)(IconCal_10); DELETE(c_Icon)(IconCal_11);
 DELETE(c_Icon)(IconCal_12); DELETE(c_Icon)(IconCal_s4);
 DELETE(c_Icon)(IconCal_13); DELETE(c_Icon)(IconCal_14);

 DELETE(c_Icon)(IconFon_s1); DELETE(c_Icon)(IconFon_3);
 DELETE(c_Icon)(IconFon_4); DELETE(c_Icon)(IconFon_5);
 DELETE(c_Icon)(IconFon_6); /*DELETE(c_Icon)(IconFon_7);*/
 /*DELETE(c_Icon)(IconFon_8);*/ 
 DELETE(c_Icon)(IconFon_9);
 /*DELETE(c_Icon)(IconFon_10);*/
 DELETE(c_Icon)(IconFon_11); DELETE(c_Icon)(IconFon_12);
 DELETE(c_Icon)(IconFon_s2); DELETE(c_Icon)(IconFon_s3);
 DELETE(c_ColorList)(CLfontColor);
 DELETE(c_ItemMenu)(IMfontFamily);
 DELETE(c_SpinBox)(sbFontSize);

 DELETE(c_Icon)(IconCel_1); DELETE(c_Icon)(IconCel_2);
 DELETE(c_Icon)(IconCel_3); DELETE(c_Icon)(IconCel_4);
 DELETE(c_Icon)(IconCel_5); DELETE(c_Icon)(IconCel_8);
 DELETE(c_Icon)(IconCel_10); DELETE(c_Icon)(IconCel_11);
 DELETE(c_Icon)(IconCel_13); DELETE(c_Icon)(IconCel_14);
 DELETE(c_Icon)(IconCel_s1); DELETE(c_Icon)(IconCel_s2);
 DELETE(c_Icon)(IconCel_s3);

 DELETE(c_Icon)(IconBor_1); DELETE(c_Icon)(IconBor_s1);
 DELETE(c_Icon)(IconBor_2); DELETE(c_Icon)(IconBor_3);
 DELETE(c_Icon)(IconBor_4); DELETE(c_Icon)(IconBor_5);
 DELETE(c_Icon)(IconBor_6); DELETE(c_Icon)(IconBor_s2);
 DELETE(c_ColorList)(CLfillColor);
 DELETE(c_Icon)(IconBor_7); DELETE(c_Icon)(IconBor_s3);
 DELETE(c_Icon)(IconBor_8);

 DELETE(c_Icon)(IconGra_0); DELETE(c_Icon)(IconGra_1);
 DELETE(c_Icon)(IconGra_2); DELETE(c_Icon)(IconGra_3);
 DELETE(c_Icon)(IconGra_4); DELETE(c_Icon)(IconGra_5);
 DELETE(c_Icon)(IconGra_6); DELETE(c_Icon)(IconGra_7);
 DELETE(c_Icon)(IconGra_8); DELETE(c_Icon)(IconGra_9);
 DELETE(c_Icon)(IconGra_10); DELETE(c_Icon)(IconGra_11);
 DELETE(c_Icon)(IconGra_12); DELETE(c_Icon)(IconGra_13);
 DELETE(c_Icon)(IconGra_14); DELETE(c_Icon)(IconGra_15);
 DELETE(c_Icon)(IconGra_16); DELETE(c_Icon)(IconGra_17);
 DELETE(c_Icon)(IconGra_18); DELETE(c_Icon)(IconGra_s1);
 DELETE(c_Icon)(IconGra_s2); DELETE(c_Icon)(IconGra_s3);
 DELETE(c_Icon)(IconGra_s4);

 DELETE(c_Icon)(IconZoo_1); DELETE(c_Icon)(IconZoo_2);
 DELETE(c_Icon)(IconZoo_3); DELETE(c_Icon)(IconZoo_4);
 DELETE(c_Icon)(IconZoo_5); DELETE(c_Icon)(IconZoo_6);
 DELETE(c_Icon)(IconZoo_7); DELETE(c_Icon)(IconZoo_s1);

 DELETE(c_Icon)(IconTra);

 for (i = 0; i < NB_HBAR; i++)
 {
  DELETE(c_RowColumn)(hIconBar[i]);
  DELETE(c_Icon)(vIcon[i]);
 }
 
 DELETE(c_Form)(hIconBarForm);
 DELETE(c_RowColumn)(vIconBar_1);
 DELETE(c_RowColumn)(vIconBar_2);
 XtDestroyWidget(w_vIconBar_3);
 DELETE(c_Icon)(IconVlogo);
 DELETE(c_Form)(vIconBarlogo);
 DELETE(c_Form)(vIconBar);

 DELETE(c_Icon)(IconBottom);
 DELETE(c_Form)(BottomBar);
  
 DELETE(c_PulldownMenu)(MenuFic);
 DELETE(c_PulldownMenu)(MenuEdi);
 DELETE(c_PulldownMenu)(MenuSel);
 DELETE(c_PulldownMenu)(MenuAff);
 DELETE(c_PulldownMenu)(MenuFor);
 DELETE(c_PulldownMenu)(MenuGra);
 DELETE(c_PulldownMenu)(MenuFen);
 DELETE(c_PulldownMenu)(MenuAid);

 DELETE(c_RowColumn)(MainDocument);

 DELETE(c_EditBar)(GlobEditBar);
 DELETE(c_GCursor)(GlobCursor); /* GlobEditBar contain a cursor ! */

 DELETE(c_MenuBar)(Bar1);

 DELETE(c_HelpLine)(GlobHelpLine);
 DELETE(c_RemoteServer)(GlobRemoteServer);
 DELETE(c_Resource)(GlobResources);
 DELETE(c_MainInterface)(MainInterface1);

#ifndef NMEMDEBUG
 Xc_mdump();
#endif

 Xc_exit(0);
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

/*---------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------*/
void ToggleSwitchType(w, funct)
Widget	w;
int	funct;
{
 int i, j;

 j = 0;
 while ((typetoggletable[j].func_num != funct)
	&& (typetoggletable[j].tog_wid != 0))
  j++;
  
 if (store_function_type != typetoggletable[j].mode_num)
 {
  i = 0;
  while ((typetoggletable[i].mode_num != store_function_type)
	 && (typetoggletable[i].tog_wid != 0))
   i++;
  if (typetoggletable[i].tog_wid != 0)
   XmToggleButtonSetState(typetoggletable[i].tog_wid,
			  False, False);

  store_function_type = typetoggletable[j].mode_num;

 }
 else
 {
  XmToggleButtonSetState(w, True, False);
 }
 SelectFunction(w, funct);
}
/*---------------------------------------------------------------------*/
/* objet des fonctions selected */

static XEvent Noevent;

void SelectFunction(w, funct)
Widget	w;
int	funct;
{
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

/*---------------------------------------------------------------------*/
void MenuArrangeWindow(w)
Widget	w;
{
 Display	*display;

 display = XtDisplay(WIDGET(MainInterface1));
 F(Gwm).ArrangeWindow(Gwm, display, XtWindow(WIDGET(MainDocument)));
}

/*---------------------------------------------------------------------*/
void MenuTileWindow(w)
Widget	w;
{
 Display	*display;

 display = XtDisplay(WIDGET(MainInterface1));
 F(Gwm).TileWindow(Gwm, display, XtWindow(WIDGET(MainDocument)));
}

/*---------------------------------------------------------------------*/
void SChangeWindow(listOld, listNew)
listmanaged	*listOld;
listmanaged	*listNew;
{
 void	*DocTmp1 = NULL;
 void	*DocTmp2;
 int	Type1;
 int	Type2;
 c_Feuille	*feuille;
  
 if (listOld)
 {
  DocTmp1 = F(DocList).Find(DocList, listOld->MainWin, &Type1); 
  if (Type1 == DOCUMENT_TABLEUR)
  {
   F(GlobEditBar).certify(GlobEditBar->Edit, GlobEditBar);    
  }
 }
  
 DocTmp2 = F(DocList).Find(DocList, listNew->MainWin, &Type2);
  
 if (Type2 == DOCUMENT_TABLEUR)
 {
  feuille = ((c_Tableau *)DocTmp2)->Feuille;
  F(GlobEditBar).query_aff(GlobEditBar, feuille->cell_x, feuille->cell_y);
  F(GlobEditBar).refresh_cell(GlobEditBar, feuille);
  if (DocTmp1 != DocTmp2)
  {
   c_Color *color, *color_base;

   color_base = ((c_Tableau *)DocTmp2)->BaseStd.color_base;
   F(CLfontColor).setColorBase(CLfontColor, color_base, CL_SKIP_TRANSPARENCY);
   F(CLfillColor).setColorBase(CLfillColor, color_base, CL_KEEP_TRANSPARENCY);
   
   color = F(color_base).getColor(&color_base, XcC_GRAY,
				  XcC_SCALE_COLOR(0.0),
				  XcC_TRANSPARENCY, 0,
				  XcC_END);
     
   F(CLfontColor).setColor(CLfontColor, color);
   F(CLfillColor).setColor(CLfillColor, color);
  }
 }
}

/*---------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------*/
void MenuHelpAbout(w, cdisplay)
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
/* void MenuGraph(w, cdisplay)
   Widget	w;
   Display	*cdisplay;
   {
    c_Box_SelGraph *box;
    void		*DocSelected;

    SET_WAIT_MODE_STATIC;
    box = (c_Box_SelGraph *)NEW(c_Box_SelGraph)(MainInterface1->w_Main,
						"BoxSelGraph");
    DocSelected = F(DocList).Find(DocList,
				  Gwm->SelectedWindow->MainWin, NULL);
    GlobGraphParam.moteur = ((c_Tableau *)DocSelected)->moteur;
    GlobGraphParam.tableau = (c_Tableau *)DocSelected;
    GlobGraphParam.x = ((c_Tableau *)DocSelected)->Feuille->cell_region->x_min;
    GlobGraphParam.y = ((c_Tableau *)DocSelected)->Feuille->cell_region->y_min;
    GlobGraphParam.width = 
     ((c_Tableau *)DocSelected)->Feuille->cell_region->x_max-GlobGraphParam.x+1;
    GlobGraphParam.height = 
     ((c_Tableau *)DocSelected)->Feuille->cell_region->y_max-GlobGraphParam.y+1;
    F(box->Dialog).waitForUnmap(box->Dialog);
    DELETE(c_Box_SelGraph)(box);
    UNSET_WAIT_MODE;
   }
   */


/*---------------------------------------------------------------------*/
void MenuEditColors(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 c_Box_color	*box;
 void		*doc;
 int		Type;

 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (Type == DOCUMENT_TABLEUR)
 {
  SET_WAIT_MODE_STATIC;
  box = (c_Box_color *)NEW(c_Box_color)
   (MainInterface1->w_Main, "BoxColors",
    &(((c_Tableau *)doc)->BaseStd.color_base));
  if (F(box->PBox->Dialog).waitForUnmap(box->PBox->Dialog) ==  XcCD_OK)
   ((c_Tableau *)doc)->modified = TRUE;
  DELETE(c_Box_color)(box);
  F(CLfontColor).setColorBase(CLfontColor, 
			      ((c_Tableau *)doc)->BaseStd.color_base,
			      CL_SKIP_TRANSPARENCY);
  F(CLfontColor).setColor(CLfontColor, CLfontColor->color_set);
  F(CLfillColor).setColorBase(CLfillColor, 
			      ((c_Tableau *)doc)->BaseStd.color_base,
			      CL_KEEP_TRANSPARENCY);
  F(CLfillColor).setColor(CLfillColor, CLfillColor->color_set);
  UNSET_WAIT_MODE;
 }
}

/*---------------------------------------------------------------------*/
void MenuFormatNombres(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 void *doc;
 c_Tableau	*document;
 c_Box_nombres	*box;
 c_FormatNbr	*format;
 cell		*cellule;
 char		value[Xq_FORMULA_LENGTH];
 char		*preview;
 int		Type, value_type;
  

 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (Type == DOCUMENT_TABLEUR)
 {
  document = (c_Tableau *)doc;
  format = ((CellStyle_t *)
	    F(document->moteur).get_cell_style(document->moteur,
					       document->Feuille->cell_x,
					       document->Feuille->cell_y
					       ))->format;
  cellule = F(document->moteur).get_cell(document->moteur,
					 document->Feuille->cell_x,
					 document->Feuille->cell_y);
  value_type = 
   F(document->moteur).get_string(document->moteur, cellule, value,
				  Xq_VALUE);
      
  if (Xq_IS_VALUE(value_type) && value[0] != '\0')
   preview = value;
  else
   preview = NULL;
      
  SET_WAIT_MODE_STATIC;
  box = (c_Box_nombres *)NEW(c_Box_nombres)(MainInterface1->w_Main,
					    "BoxNumbers",
					    &(document->BaseStd),
					    &format, preview);
      
  switch(F(box->PBox->Dialog).waitForUnmap(box->PBox->Dialog))
  {
  case XcCD_DESTROY:
  case XcCD_CANCEL:
   DELETE(c_Box_nombres)(box);
   break;
  case XcCD_OK:
   DELETE(c_Box_nombres)(box);
   ((c_Tableau *)doc)->modified = TRUE;
   break;
  case XcCD_APPLY:
   DELETE(c_Box_nombres)(box);
   ((c_Tableau *)doc)->modified = TRUE;
   if (format)
    F(document->Feuille).SetCellStyleFormatNbr(document->Feuille,
					       format);
   break;
  }
  UNSET_WAIT_MODE;
 }
}


/*---------------------------------------------------------------------*/
void MenuFormatMotifs(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 void *doc;
 c_Tableau	*document;
 c_Feuille	*feuille;
 c_Box_fond	*box;
 cell_inside_t	inside;
 CellStyle_t	*cstyle;
 int Type;
  
 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (Type == DOCUMENT_TABLEUR)
 {
  document = (c_Tableau *)doc;
  feuille = document->Feuille;
  cstyle = F(feuille->moteur).get_cell_style(feuille->moteur,
					     feuille->cell_x,
					     feuille->cell_y);
  inside.foreground = cstyle->inside_foreground;
  inside.background = cstyle->inside_background;
  inside.pattern = cstyle->inside_stipple_pattern;
  SET_WAIT_MODE_STATIC;
  box = (c_Box_fond *)NEW(c_Box_fond)(MainInterface1->w_Main,
				      "BoxPatterns",
				      &(document->BaseStd),
				      &inside);
    
  switch(F(box->Dialog).waitForUnmap(box->Dialog))
  {
  case XcCD_DESTROY:
  case XcCD_CANCEL:
   DELETE(c_Box_fond)(box);
   break;
  case XcCD_OK:
   DELETE(c_Box_fond)(box);
   ((c_Tableau *)doc)->modified = TRUE;
   F(feuille).SetCellStyleInside(feuille, &inside);
   break;
  }
  UNSET_WAIT_MODE;
 }
}


/*---------------------------------------------------------------------*/
void MenuFormatPolices(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 void		*doc;
 c_Tableau	*document;
 c_Box_styles  *box;
 c_TextStyle	*style;
 int		Type;	
  
 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (Type == DOCUMENT_TABLEUR)
 {
  document = (c_Tableau *)doc;
  style = ((CellStyle_t *)
	   F(document->moteur).get_cell_style(document->moteur,
					      document->Feuille->cell_x,
					      document->Feuille->cell_y
					      ))->text_style;
  SET_WAIT_MODE_STATIC;
  box = (c_Box_styles *)NEW(c_Box_styles)(MainInterface1->w_Main, 
					  "BoxFonts",
					  &(document->BaseStd),
					  &style);
  switch(F(box->PBox->Dialog).waitForUnmap(box->PBox->Dialog))
  {
  case XcCD_DESTROY:
  case XcCD_CANCEL:
   DELETE(c_Box_styles)(box);
   break;
  case XcCD_OK:
   DELETE(c_Box_styles)(box);
   ((c_Tableau *)doc)->modified = TRUE;
   break;
  case XcCD_APPLY:
   DELETE(c_Box_styles)(box);
   ((c_Tableau *)doc)->modified = TRUE;
   if (style)
    F(document->Feuille).SetCellStyleTextStyle(document->Feuille,
					       style);
   break;
  }
  UNSET_WAIT_MODE;
 }
}


/*---------------------------------------------------------------------*/
void MenuSelectFonctions(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 void		*doc;
 c_Tableau	*document;
 c_Box_functions  *box;
 int		Type;	
  
 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (Type == DOCUMENT_TABLEUR)
 {
  document = (c_Tableau *)doc;
  SET_WAIT_MODE_STATIC;
  box = (c_Box_functions *)NEW(c_Box_functions)(MainInterface1->w_Main, 
						"BoxFunctions",
						document->moteur->parser);
  switch(F(box->Dialog).waitForUnmap(box->Dialog))
  {
  case XcCD_DESTROY:
  case XcCD_CANCEL:
   DELETE(c_Box_functions)(box);
   break;
  case XcCD_OK:
   DELETE(c_Box_functions)(box);
   break;
  }
  UNSET_WAIT_MODE;
 }
}

/*---------------------------------------------------------------------*/
void MenuSelectGotoCell(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 void		*doc;
 c_Tableau	*document;
 c_Box_gotocell  *box;
 int		Type;	
 int		column;
 int		row;
  
 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (Type == DOCUMENT_TABLEUR)
 {
  document = (c_Tableau *)doc;
  column = document->Feuille->cell_x;
  row = document->Feuille->cell_y;
  SET_WAIT_MODE_STATIC;
  box = (c_Box_gotocell *)NEW(c_Box_gotocell)(MainInterface1->w_Main, 
					      "BoxGotoCell", 
					      &column, &row);
  switch(F(box->Dialog).waitForUnmap(box->Dialog))
  {
  case XcCD_DESTROY:
  case XcCD_CANCEL:
   DELETE(c_Box_gotocell)(box);
   break;
  case XcCD_OK:
   DELETE(c_Box_gotocell)(box);
   F(document->Feuille).SetActiveCellule(document->Feuille, 
					 column, row, TRUE);
   break;
  }
  UNSET_WAIT_MODE;
 }
}


/*---------------------------------------------------------------------*/
void MenuFormatEncadrements(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 void	*doc;
 c_Tableau	*document;
 BaseStd_t	*base;
 c_Box_bordures *box;
 cell_border_t	borders;
 c_Color	*black;
 c_Color	*white;
 int	Type;

 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (Type == DOCUMENT_TABLEUR)
 {
  document = (c_Tableau *)doc;
  base = &(document->BaseStd);
  black = F(base->color_base).getColor(&(base->color_base),
				       XcC_NAME, "Black",
				       XcC_GRAY, XcC_SCALE_COLOR(0.0),
				       XcC_TRANSPARENCY, 0,
				       XcC_END);
  if (black == NULL) return;
  white = F(base->color_base).getColor(&(base->color_base),
				       XcC_NAME, "White",
				       XcC_GRAY, XcC_SCALE_COLOR(1.0),
				       XcC_TRANSPARENCY, 0,
				       XcC_END);
  if (white == NULL) return;
  borders.outline = borders.left = borders.right = 
   borders.up = borders.down = FALSE;
  memset(&borders.cellb_outline, 0, sizeof(CellBorderStyle_t));
  borders.cellb_outline.border_foreground = black;
  borders.cellb_outline.border_background = white;
  borders.cellb_outline.border_stipple_pattern = PATTERN_NONE;
  borders.cellb_outline.border_dash_pattern = PATTERN_NONE;
  memcpy(&borders.cellb_left, &borders.cellb_outline, 
	 sizeof(CellBorderStyle_t));
  memcpy(&borders.cellb_right, &borders.cellb_left, 
	 sizeof(CellBorderStyle_t));
  memcpy(&borders.cellb_up, &borders.cellb_left, 
	 sizeof(CellBorderStyle_t));
  memcpy(&borders.cellb_down, &borders.cellb_left, 
	 sizeof(CellBorderStyle_t));
      
  SET_WAIT_MODE_STATIC;
  box = (c_Box_bordures *)NEW(c_Box_bordures)(MainInterface1->w_Main,
					      "BoxBorders",
					      &(document->BaseStd),
					      &borders);
  switch (F(box->Dialog).waitForUnmap(box->Dialog))
  {
  case XcCD_DESTROY:
  case XcCD_CANCEL:
   DELETE(c_Box_bordures)(box);
   break;
  case XcCD_OK:
   DELETE(c_Box_bordures)(box);
   ((c_Tableau *)doc)->modified = TRUE;
   F(document->Feuille).SetCellStyleBorders(document->Feuille,
					    &borders);
   break;
  } 
  UNSET_WAIT_MODE;
 }
}

/*---------------------------------------------------------------------*/
void MenuFormatLargeurHauteur(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 void *doc;
 c_Tableau	*document;
 c_Box_colsize	*box;
 colrow_size_t	colrow_size;
 int Type;
  
 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
 if (Type == DOCUMENT_TABLEUR)
 {
  document = (c_Tableau *)doc;
  colrow_size.change_col_size = colrow_size.change_row_size = FALSE;
  colrow_size.col_size = 
   F(document->moteur).get_size(document->moteur,
				Xc_COLUMN,
				document->Feuille->cell_x);
  colrow_size.row_size = 
   F(document->moteur).get_size(document->moteur,
				Xc_ROW,
				document->Feuille->cell_y);
  SET_WAIT_MODE_STATIC;
  box = (c_Box_colsize *)NEW(c_Box_colsize)(MainInterface1->w_Main,
					    "BoxRowColSize", &colrow_size);
      
  switch(F(box->Dialog).waitForUnmap(box->Dialog))
  {
  case XcCD_DESTROY:
  case XcCD_CANCEL:
   DELETE(c_Box_colsize)(box);
   break;
  case XcCD_OK:
   DELETE(c_Box_colsize)(box);
   ((c_Tableau *)doc)->modified = TRUE;
   F(document->Feuille).SetColumnAndRowSize(document->Feuille,
					    &colrow_size);
   break;
  }
  UNSET_WAIT_MODE;
 }
}
/*---------------------------------------------------------------------*/
void PastSummFunction(w, cdisplay)
Widget	w;
Display	*cdisplay;
{
 int insert_pos;
 char string[20];

 insert_pos = XmTextFieldGetInsertionPosition(WIDGET(GlobEditBar->Edit));
  
 if (!insert_pos) 
 {
  string[0] = '='; 
  string[1] = '\0';
 }
 else 
  string[0] = '\0';
  
 strcat(string, F(GlobResources).getString(GlobResources, XcR_funcSumm));
 strcat(string, "()");
 XmTextFieldInsert(WIDGET(GlobEditBar->Edit), insert_pos, string);

 insert_pos = XmTextFieldGetInsertionPosition(WIDGET(GlobEditBar->Edit));
 XmTextFieldSetInsertionPosition(WIDGET(GlobEditBar->Edit), insert_pos -1);
}

/* ----------------------------------------------------------------- ** 
** Change font -> create style and set it to selection               ** 
** ----------------------------------------------------------------- */
static void change_font(This, Param, family)
c_ItemMenu	*This;
void		*Param;
family_tree	*family;
{
 c_Feuille	*Feuille;
 c_Tableau	*doc;
 int Type;

 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (!doc || Type != DOCUMENT_TABLEUR)
  return;
  
 Feuille = doc->Feuille;
 F(Feuille).SetCellStyleFontFamily(Feuille, family);
}

/* ----------------------------------------------------------------- ** 
** Change font color -> create style and set it to selection         ** 
** ----------------------------------------------------------------- */
static void change_font_color(This, Param, color)
c_ColorList	*This;
void		*Param;
c_Color		*color;
{
 c_Feuille	*Feuille;
 c_Tableau	*doc;
 int Type;

 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (!doc || Type != DOCUMENT_TABLEUR)
  return;
  
 Feuille = doc->Feuille;
 F(Feuille).SetCellStyleFontColor(Feuille, color);
}

/* ----------------------------------------------------------------- ** 
** Change fill color -> change cell background color                 ** 
** ----------------------------------------------------------------- */
static void change_fill_color(This, Param, color)
c_ColorList	*This;
void		*Param;
c_Color		*color;
{
 c_Feuille	*Feuille;
 c_Tableau	*doc;
 int Type;

 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (!doc || Type != DOCUMENT_TABLEUR)
  return;
  
 Feuille = doc->Feuille;
 F(Feuille).SetCellStyleFillColor(Feuille, color);
}

/* ----------------------------------------------------------------- ** 
** Certify fonte size -> change fonte size		             ** 
** ----------------------------------------------------------------- */
static void certify_font_size(This, param)
c_WTextField *This;
void *param;
{
 c_Feuille	*Feuille;
 c_Tableau	*doc;
 coord_t	num;
 int		Type;

 Xc_HISTORY(("certify fonte size"));
  
 doc = F(DocList).Find(DocList, Gwm->SelectedWindow->MainWin, &Type);
  
 if (!doc || Type != DOCUMENT_TABLEUR)
  return;
  
 Feuille = doc->Feuille;
 num = ((coord_t)F(This).get_value_in_unit(This)) & COORD_FONTSIZE_MASK;
 F(This).set_value_in_unit(This, SCALE_TO_POINTS(num));
  
 F(Feuille).SetCellStyleFontSize(Feuille, num);
 motif_focus = tffs_focus = FALSE;

 if (!XmProcessTraversal(WIDGET(GlobEditBar->Edit), XmTRAVERSE_CURRENT))
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

static void cb_focus_tfFontSize(w, This, event)
Widget w;            
c_WTextField *This;
XEvent	*event;
{
 Xc_TRACE(("FOCUS MOTIF (for tfFontSize)"));
 
 motif_focus = tffs_focus = TRUE;
}

static void cb_unfocus_tfFontSize(w, This, event)
Widget w;            
c_WTextField *This;
XEvent	*event;
{
 Xc_TRACE(("UNFOCUS MOTIF (for tfFontSize)"));
 
 XtCallCallbacks(w, XmNlosingFocusCallback, This);
 
 motif_focus = tffs_focus = FALSE;
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
   c_Tableau *Document;
   char filename[20];
   int  i = 0;
	  
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
     sprintf(filename, "Crash_svg_%03d.xq", i++); 
     Xc_WARNING(("saving file name: %s", filename));
     doc = NEW(c_DocFile)(XcDF_SAVE, filename, -1);
     Document->filename = filename;
    }
    if(WRITE_DOCUMENT(Document)(Document, doc) == FALSE)
     Xc_WARNING(("save error"));
    DELETE(c_DocFile)(doc);      
    tmplist = tmplist->NextDocument;
   }
   exit (-1);
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
   xms = F(GlobResources).getXmString(GlobResources,
				      XcR_sigErrorQuiting);
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
   xms = F(GlobResources).getXmString(GlobResources,
				      XcR_sigErrorQuiting);
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
