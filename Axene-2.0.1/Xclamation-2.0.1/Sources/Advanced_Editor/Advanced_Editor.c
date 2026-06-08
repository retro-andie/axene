/*
** Advanced_Editor.c for Xclamation in Advanced_Editor/
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
** Started on  Mon Jul 17 13:52:07 1995 Emmanuel Paris
** Last update Sun Aug  8 17:43:37 1999 Emmanuel Paris
*/

/*#define NTRACE*/

#include "Advanced_Editor.h"
#include "SpecialFileSelector.h"
#include "ExportTextFileSelector.h"
#include "TextRuler.h"
#include "TextStyle.h"
#include "TextFrame.h"
#include "Color.h"
#include "Keyboard.h"
#include "VectorFont.h"
#include "Export_Text_HTML.h"
#include "../Pixmaps/appliquer.zpm"
#include "../Pixmaps/insert_newline.zpm"
#include "../Pixmaps/insert_nextframe.zpm"
#include <X11/Intrinsic.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/Separator.h>
#include <Xm/List.h>
#include <varargs.h>

extern void XcHandleEvent ___PROTO((XEvent *event));
extern void XcFlushEvents ___PROTO((Widget w,
				    void (*func) ___PROTO((XEvent *))));
static void 	*constructor();
static void 	destructor();
static void 	*copy();
static void	generate_text();

/* --------------------- Declaration of callbacks --------------------- */
/*___CustomDialog Callbacks___*/
static void 	Editor_OK();
static void 	Editor_Cancel();

/*___Text widget Callbacks___*/
static void 	Modify_Verify_Callback();
static void 	Motion_Verify_Callback();

/*___ItemMenu Callbacks___*/
static void	RulerIM_Callback();
static void	StyleIM_Callback();

/*___Escape Insertion___*/
static void	Do_Ruler_Insertion();
static void	Do_Style_Insertion();
static void	Do_Newline_Insertion();
static void	Do_Nextcadre_Insertion();
static void	Do_Newparagraph_Insertion();

/*___Menu Callbacks___*/
static void 	MenuEditCut();
static void 	MenuEditCopy();
static void 	MenuEditPaste();
static void	MenuRulerInsert();
static void	MenuStyleInsert();
static void	MenuNewlineInsert();
static void	MenuNextcadreInsert();
static void	MenuTextSetMark1();
static void	MenuTextSetMark2();
static void	MenuTextGotoMark1();
static void	MenuTextGotoMark2();
static void	MenuTextUnsetMark1();
static void	MenuTextUnsetMark2();
static void	MenuTextExport();
/* ------------------- Declaration of callbacks over ------------------ */

/* ------------------ Declaration of private functions ---------------- */
static void	Allocate_Escape_List();
static void	Clear_Selection();
static void	Insert_Selection();
static void	Delete_Selection();
static void	Make_Selection();
static void	Insert_Reference_Ruler();
static void	Insert_Reference_Style();
static void	Insert_Reference_Newline();
static void	Insert_Reference_Nextcadre();
static void	Insert_Reference_Newparagraph();
static void	Mouline_Texte();
static void	Insert_String();
static void	Delete_String();
static boolean	Is_In_Reference();
static XmTextPosition	Get_Reference_Start();
static XmTextPosition	Get_Reference_End();
static XcAE_Escape_List	*Get_Reference_List();
static void	Make_Selection_Relative();
static void	Set_Default_Ruler();
static void	Set_Default_Style();
static void	cb_handle_event();
static void	cb_insert_char();

/* --------------- Declaration of private functions over -------------- */

/* ---------------- Declaration of external variables ----------------- */
extern c_VectorFont	*GlobFontBase;
extern c_Keyboard	*GlobKeyboard;
/* ------------- Declaration of external variables over --------------- */
sf_AEditor fc_AEditor =
{
 constructor,
 destructor,
 copy,
 Editor_OK,
 Editor_Cancel,
};

/*----------------------------------------------------------------------**
**		Constructor for Advanced Editor Box			**
**----------------------------------------------------------------------*/

static void *constructor(w_Parent, pszTitle, ArgText, The_Page)
Widget	w_Parent;
char 	*pszTitle;
c_Text	*ArgText;
c_Page	*The_Page;
{
 Display	*display;
 c_AEditor      *This;
 Arg		args[7];
 int		nargs;
 c_Text 	*Text;
 c_TextFrame	*text_frame;

 Xc_HISTORY(("Constructor..."));
  
 display = XtDisplay(w_Parent);
  
 if ((This = (c_AEditor *)Xc_malloc("MAEditor", sizeof(c_AEditor))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
  
 if ((Text = ArgText) == NULL)
 {
  Xc_TRACE(("%x", The_Page->BaseStd));
  Text = (c_Text *)NEW(c_Text)("Filler", &(The_Page->BaseStd->text_base),
			       The_Page->BaseStd->text_style_base,
			       The_Page->BaseStd->text_ruler_base);
  text_frame = (c_TextFrame *)NEW(c_TextFrame)();
  F(text_frame).attachText(text_frame, Text);
  F(The_Page->SelectedCadre->cadre).attachobject
   (The_Page->SelectedCadre->cadre, text_frame, CADRE_TEXT);
  This->empty_before = TRUE;
 }
 else
  This->empty_before = FALSE;
  
  
 This->Selection = (XcAE_Selection *)Xc_malloc("selection",
					       sizeof(XcAE_Selection));
 This->Clip_Buffer = (XcAE_Selection *)Xc_malloc("selection",
						 sizeof(XcAE_Selection));
 memset(This->Selection, 0, sizeof(XcAE_Selection));
 memset(This->Clip_Buffer, 0, sizeof(XcAE_Selection));
 Clear_Selection(This->Selection);
 Clear_Selection(This->Clip_Buffer);
  
 This->f = &fc_AEditor;
  
 This->w_parent = w_Parent;
 This->Text = Text;
 This->Page = The_Page;
 This->Text_Base = &(The_Page->BaseStd->text_base);

 This->Dialog = (c_CustomDialog *) 
  NEW(c_CustomDialog)(w_Parent, "AEditor", 800, 500,
		      XcCD_HORIZONTAL | XcCD_HRESIZE | XcCD_VRESIZE);
  
 /* -------- Creation of the top side of the Custom Dialog ------------ */

 This->Bar1 = 
  (c_MenuBar *)NEW(c_MenuBar)(WIDGET(This->Dialog), 
			      MANAGER(This->Dialog), "AEMenuBar");

#define COUPER		1
#define COPIER		2
#define COLLER		3

 This->Menu1 = (c_PulldownMenu *)
  NEW(c_PulldownMenu)(WIDGET(This->Bar1), MANAGER(This->Dialog), 
		      "pmEdit",
		      "mCut", COUPER,
		      "mCopy", COPIER,
		      "mPaste", COLLER,
		      NULL);

 F(This->Menu1).AddCallback(This->Menu1, COUPER,
			    MenuEditCut, This);
 F(This->Menu1).AddCallback(This->Menu1, COPIER,
			    MenuEditCopy, This);
 F(This->Menu1).AddCallback(This->Menu1, COLLER,
			    MenuEditPaste, This);

#define INSERTSTYLE	1
#define INSERTRULER	2
#define SEP1		3
#define SETMARK1	4
#define SETMARK2	5
#define GOTOMARK1	6
#define GOTOMARK2	7
#define UNSETMARK1	8
#define UNSETMARK2	9
#define SEP2		10
#define NEWLINE		11
#define NEXTCADRE	12
#define SEP3		13
#define EXPORT_TEXT	14
  
 This->Menu2 = (c_PulldownMenu *)
  NEW(c_PulldownMenu)(WIDGET(This->Bar1), MANAGER(This->Dialog), 
		      "pmOptions",
		      "mInsertStyle", INSERTSTYLE,
		      "mInsertRuler", INSERTRULER,
		      "mLineFeed", NEWLINE,
		      "mFrameBreak", NEXTCADRE,
		      MENUSEPARATOR, SEP1,
		      "mSetMark1", SETMARK1,
		      "mGoMark1", GOTOMARK1,
		      "mDelMark1", UNSETMARK1,
		      MENUSEPARATOR, SEP2,
		      "mSetMark2", SETMARK2,
		      "mGoMark2", GOTOMARK2,
		      "mDelMark2", UNSETMARK2,
		      MENUSEPARATOR, SEP3,
		      "mTextExport", EXPORT_TEXT,
		      NULL);

 F(This->Menu2).AddCallback(This->Menu2, INSERTSTYLE,
			    MenuStyleInsert, This);
 F(This->Menu2).AddCallback(This->Menu2, INSERTRULER,
			    MenuRulerInsert, This);
 F(This->Menu2).AddCallback(This->Menu2, NEWLINE,
			    MenuNewlineInsert, This);
 F(This->Menu2).AddCallback(This->Menu2, NEXTCADRE,
			    MenuNextcadreInsert, This);
 F(This->Menu2).AddCallback(This->Menu2, SETMARK1,
			    MenuTextSetMark1, This);
 F(This->Menu2).AddCallback(This->Menu2, GOTOMARK1,
			    MenuTextGotoMark1, This);
 F(This->Menu2).AddCallback(This->Menu2, UNSETMARK1,
			    MenuTextUnsetMark1, This);
 F(This->Menu2).AddCallback(This->Menu2, SETMARK2,
			    MenuTextSetMark2, This);
 F(This->Menu2).AddCallback(This->Menu2, GOTOMARK2,
			    MenuTextGotoMark2, This);
 F(This->Menu2).AddCallback(This->Menu2 , UNSETMARK2,
			    MenuTextUnsetMark2, This);
 F(This->Menu2).AddCallback(This->Menu2 , EXPORT_TEXT,
			    MenuTextExport, This);

 F(This->Dialog).setMenuBar(This->Dialog, This->Bar1);
  
 /* --------- Creation of the down side of the Custom Dialog --------- */
  
 This->FormOfWin = (c_Form *)NEW(c_Form)(WIDGET(This->Dialog),
					 MANAGER(This->Dialog),
					 "AEForm");
  
 nargs = 0;
 XtSetArg(args[nargs], XmNeditable, True);
 nargs++;
 XtSetArg(args[nargs], XmNeditMode, XmMULTI_LINE_EDIT);
 nargs++;
 XtSetArg(args[nargs], XmNwordWrap, True);
 nargs++;
 XtSetArg(args[nargs], XmNscrollHorizontal, False);
 nargs++;
 This->Editor = XmCreateScrolledText(WIDGET(This->FormOfWin),
				     "Editor", args, nargs);


 XtAddCallback(This->Editor, XmNmodifyVerifyCallback,
	       Modify_Verify_Callback, This);
  
 XtAddCallback(This->Editor, XmNmotionVerifyCallback,
	       Motion_Verify_Callback, This);
 XtAddEventHandler(This->Editor, 
		   ButtonPressMask | KeyPressMask | KeyReleaseMask | 
		   KeymapStateMask | EnterWindowMask, True,
		   cb_handle_event, (void *)This); 
 This->Row1 = (c_RowColumn *)
  NEW(c_RowColumn)(WIDGET(This->FormOfWin), MANAGER(This->Dialog),
		   "Editor_DownRow", 0, 34, XmHORIZONTAL);
  
 This->IconNewline = (c_Icon *)NEW(c_Icon)(WIDGET(This->Row1),
					   MANAGER(This->Dialog),
					   "IconNewline", display,
					   insert_newline_xpm, 
					   NULL, XcPush);
 CALLBACK(WIDGET(This->IconNewline), FUNC_ACTIVATE, 
	  MenuNewlineInsert, This);
 nargs = 0;
 XtSetArg(args[nargs], XmNtraversalOn, False);
 nargs++;
 XtSetValues(WIDGET(This->IconNewline), args, nargs);

 This->IconNextframe = (c_Icon *)NEW(c_Icon)(WIDGET(This->Row1),
					     MANAGER(This->Dialog),
					     "IconNextframe", display,
					     insert_nextframe_xpm,
					     NULL, XcPush);
 CALLBACK(WIDGET(This->IconNextframe), FUNC_ACTIVATE, 
	  MenuNextcadreInsert, This);
 nargs = 0;
 XtSetArg(args[nargs], XmNtraversalOn, False);
 nargs++;
 XtSetValues(WIDGET(This->IconNextframe), args, nargs);
  
  
 XtSetArg(args[0], XmNorientation, XmVERTICAL);
 This->Sep1 = XmCreateSeparatorGadget(WIDGET(This->Row1), 
				      "AEsep1", args, 1);
 F(MANAGER(This->Dialog)).Add_child(MANAGER(This->Dialog),
				    This->Sep1, WIDGET(This->Row1));
  

 This->RulerLabel = (c_Label *)NEW(c_Label)(WIDGET(This->Row1),
					    MANAGER(This->Dialog),
					    "lAERuler");

 This->RulerIM = (c_ItemMenu *)NEW(c_ItemMenu)(WIDGET(This->Row1),
					       MANAGER(This->Dialog),
					       "imAERuler", 200, 0);
 F(This->RulerIM).setCallback(This->RulerIM, RulerIM_Callback, This);
  
 This->IconRuler = (c_Icon *)NEW(c_Icon)(WIDGET(This->Row1),
					 MANAGER(This->Dialog),
					 "IconRuler", display,
					 appliquer_xpm,
					 NULL, XcPush);
 CALLBACK(WIDGET(This->IconRuler), FUNC_ACTIVATE, 
	  MenuRulerInsert, This);
 nargs = 0;
 XtSetArg(args[nargs], XmNtraversalOn, False);
 nargs++;
 XtSetValues(WIDGET(This->IconRuler), args, nargs);
  
 XtSetArg(args[0], XmNorientation, XmVERTICAL);
 This->Sep2 = XmCreateSeparatorGadget(WIDGET(This->Row1), 
				      "AEsep2", args, 1);
 F(MANAGER(This->Dialog)).Add_child(MANAGER(This->Dialog),
				    This->Sep2, WIDGET(This->Row1));
  
 This->StyleLabel = (c_Label *)NEW(c_Label)(WIDGET(This->Row1),
					    MANAGER(This->Dialog),
					    "lAEStyle");
  
 This->StyleIM = (c_ItemMenu *)NEW(c_ItemMenu)(WIDGET(This->Row1),
					       MANAGER(This->Dialog),
					       "imAEStyle", 200, 0);
 F(This->StyleIM).setCallback(This->StyleIM, StyleIM_Callback, This);
  
 This->IconStyle = (c_Icon *)NEW(c_Icon)(WIDGET(This->Row1),
					 MANAGER(This->Dialog),
					 "IconStyle", display,
					 appliquer_xpm, 
					 NULL, XcPush);
 CALLBACK(WIDGET(This->IconStyle), FUNC_ACTIVATE, 
	  MenuStyleInsert, This);
 nargs = 0;
 XtSetArg(args[nargs], XmNtraversalOn, False);
 nargs++;
 XtSetValues(WIDGET(This->IconStyle), args, nargs);
  
  
 /* --------------- Attachments in the form widget ----------------- */
  
 F(This->FormOfWin).attach_top(WIDGET(This->FormOfWin),
			       XtParent(This->Editor), NULL);
 F(This->FormOfWin).attach_left(WIDGET(This->FormOfWin),
				XtParent(This->Editor), NULL);
 F(This->FormOfWin).attach_right(WIDGET(This->FormOfWin),
				 XtParent(This->Editor), NULL);
 F(This->FormOfWin).attach_bottom(WIDGET(This->FormOfWin),
				  XtParent(This->Editor),
				  WIDGET(This->Row1));

 F(This->FormOfWin).attach_right(WIDGET(This->FormOfWin),
				 WIDGET(This->Row1), NULL);
 F(This->FormOfWin).attach_left(WIDGET(This->FormOfWin),
				WIDGET(This->Row1), NULL);
 F(This->FormOfWin).attach_bottom(WIDGET(This->FormOfWin),
				  WIDGET(This->Row1), NULL);
  
  
 This->w_This = WIDGET(This->Dialog);
  
 F(This->Dialog).createActionArea(This->Dialog,
				  XcCD_OK | XcCD_CANCEL,
				  XcCD_OK, TRUE, NULL, NULL);
  
 XtManageChild(This->Editor);
  
 /* ---------- Down side of the CustomDialog is built ... ------------ */
  
 F(This->Text).getFirstSeq(This->Text, 0,
			   &(This->StyleBase),
			   &(This->RulerBase));
  
 Xc_TRACE(("Ruler List:"));
 if (This->RulerBase == NULL)
  Xc_TRACE(("No ruler..."));
 else
 {
  while (This->RulerBase->next != NULL)
   This->RulerBase = This->RulerBase->next;
  for (nargs = 1 ;
       This->RulerBase->previous != NULL;
       This->RulerBase = This->RulerBase->previous) nargs++;
      
  Xc_TRACE(("There are #%d rulers in rulerbase", nargs));
      
  F(This->RulerIM).set(This->RulerIM, FALSE, 
		       offsetof(c_TextRuler, name), NULL, NULL);
      
  F(This->RulerIM).addItems(This->RulerIM, XcIM_LINKED_LIST, nargs,
			    This->RulerBase,
			    offsetof(c_TextRuler, next));
      
  while (This->RulerBase != NULL)
  {
   Xc_TRACE(("In ruler base: %s",This->RulerBase->name));
   This->RulerBase = This->RulerBase->next;
  }
 }
  
 Xc_TRACE(("Style List:"));
 if (This->StyleBase == NULL)
  Xc_TRACE(("No style..."));
 else
 {
  while (This->StyleBase->next != NULL)
   This->StyleBase = This->StyleBase->next;
  for (nargs = 1 ;
       This->StyleBase->previous != NULL;
       This->StyleBase = This->StyleBase->previous) nargs++;
      
  Xc_TRACE(("There are #%d styles in This->StyleBase", nargs));
      
  F(This->StyleIM).set(This->StyleIM, FALSE, 
		       offsetof(c_TextStyle, name), NULL, NULL);
      
  F(This->StyleIM).addItems(This->StyleIM, XcIM_LINKED_LIST, nargs,
			    This->StyleBase,
			    offsetof(c_TextStyle, next));
      
  while(This->StyleBase != NULL)
  {
   Xc_TRACE(("In style base: %s", This->StyleBase->name));
   This->StyleBase = This->StyleBase->next;
  }
 }
 F(This->RulerIM).calculSize(This->RulerIM, 0);
 F(This->StyleIM).calculSize(This->StyleIM, 0);
  
 This->DefaultRuler = This->RulerBase;
 This->DefaultStyle = This->StyleBase;

 This->Position_In_Text = This->Text_Length = 0;
 This->List = NULL;
 This->W_Text = (char *)Xc_malloc("char", XcAE_AF_SIZE);
 This->W_Text[0] = '\0';
 This->Alloc_Free = XcAE_AF_SIZE - 1;
 This->Nbr_Page = 1;
  
 Allocate_Escape_List(&This->List);
 This->Set_Text_Flag = This->Motion_Flag = FALSE;
 Mouline_Texte(This);
  
  
 This->Set_Text_Flag = This->Motion_Flag = TRUE;
 This->Modify_Flag = FALSE;
 Xc_TRACE(("Going to set string in widget..."));
 XmTextSetString(This->Editor, This->W_Text);
 XmTextSetInsertionPosition(This->Editor, 0);
 This->Position_In_Text = 0;
 Xc_TRACE(("Text set in widget... ok"));

 This->Modify_Flag = This->Motion_Flag = TRUE;

 for (nargs = 0; nargs < XcAE_MAX_MARKS; This->Mark[nargs++] = -1);

 F(This->Dialog).map(This->Dialog, TRUE);
 This->cursor = (c_Cursor *)NEW(c_Cursor)(XtDisplay(This->Editor),
					  XtWindow(This->Editor));
 F(This->cursor).set_cursor(This->cursor, C_EDIT_TEXT);
 XmProcessTraversal(This->Editor, XmTRAVERSE_CURRENT);

 Xc_HISTORY(("Constructor ok"));
 return This;
}

/*----------------------------------------------------------------------**
**		Destructor for Advanced_Editor Box			**
**----------------------------------------------------------------------*/

static void destructor(This)
c_AEditor *This;
{
 XcAE_Escape_List	*The_list;
 XcAE_Escape_List	*The_list2;

 Xc_HISTORY(("Destructor..."));

 DELETE(c_Cursor)(This->cursor);

 XtDestroyWidget(This->Sep1);
 XtDestroyWidget(This->Sep2);

 DELETE(c_Label)(This->StyleLabel);
 DELETE(c_Label)(This->RulerLabel);
 DELETE(c_ItemMenu)(This->StyleIM);
 DELETE(c_ItemMenu)(This->RulerIM);
 DELETE(c_Icon)(This->IconNewline);
 DELETE(c_Icon)(This->IconNextframe);
 DELETE(c_Icon)(This->IconStyle);
 DELETE(c_Icon)(This->IconRuler);
 DELETE(c_RowColumn)(This->Row1);
 XtDestroyWidget(This->Editor);

 DELETE(c_Form)(This->FormOfWin);
 DELETE(c_PulldownMenu)(This->Menu1);
 DELETE(c_PulldownMenu)(This->Menu2);
 DELETE(c_MenuBar)(This->Bar1);
 DELETE(c_CustomDialog)(This->Dialog);
  
 Clear_Selection(This->Selection);
 Clear_Selection(This->Clip_Buffer);
 Xc_free(This->Selection);
 Xc_free(This->Clip_Buffer);
  
 if (This->W_Text)
  Xc_free(This->W_Text); 
  
 /*___Destruction of the Reference List ...___*/

 The_list = This->List;
 while(The_list)
 {
  The_list2 = The_list->Next;
  Xc_free(The_list);
  The_list = The_list2;
 }

 Xc_free(This);

 Xc_HISTORY(("Destructor ok"));
}

/*----------------------------------------------------------------------**
**		Copy method for Advanced_Editor  Box			**
**----------------------------------------------------------------------*/

static void *copy(This)
c_AEditor *This;
{
 Xc_HISTORY(("Copy object useless..."));
 return NULL;
}

/* -------------------------------------------------------------------- **
** -----	Allocate_Escape_List for Advanced_Editor Box  	  -----	**
** -------------------------------------------------------------------- */

static void Allocate_Escape_List(ptr)
XcAE_Escape_List	**ptr;
{
 *ptr = (XcAE_Escape_List *)Xc_malloc("selection", 
				      sizeof(XcAE_Escape_List));
 (*ptr)->Type = XcAE_NO_REFERENCE;
 (*ptr)->Next = NULL;
 (*ptr)->StartPos = (*ptr)->EndPos = 0;
}

/* -------------------------------------------------------------------- **
** -------	Clear Selection and its reference list ...	------- **
** -------------------------------------------------------------------- */
static void	Clear_Selection(Selection)
XcAE_Selection	*Selection;
{
 XcAE_Escape_List	*List;
 XcAE_Escape_List	*List2;

 if (Selection->String != NULL)
  Xc_free(Selection->String);
 Selection->StartPos = -1;
 Selection->EndPos = -1;
 if (Selection->List != NULL)
 {
  List = Selection->List;
  while(List)
  {
   List2 = List->Next;
   Xc_free(List);
   List = List2;
  }
  Selection->List = NULL;
 }
}
/* -------------------------------------------------------------------- **
** -------	Is In Reference : returns TRUE if Position is 	------- **
** -------	in a reference field ...			------- **
** -------------------------------------------------------------------- */

static boolean	Is_In_Reference(This, The_Position)
c_AEditor	*This;
XmTextPosition	The_Position;
{
 XcAE_Escape_List	*List;
  
 List = This->List;
 while(List->Next != NULL)
 {
  if (The_Position >= List->StartPos && The_Position < List->EndPos)
  {
   Xc_TRACE(("Is in reference !!!"));
   return TRUE;
  }
  List = List->Next;
 }
 return FALSE;
}

/* -------------------------------------------------------------------- **
** -------	Get Reference Start : returns start Position  	------- **
** -------	of the reference in wich The_Position is ...	------- **
** -------------------------------------------------------------------- */

static XmTextPosition	Get_Reference_Start(This, The_Position)
c_AEditor	*This;
XmTextPosition	The_Position;
{
 XcAE_Escape_List	*List;

 List = This->List;
 while(List->Next != NULL)
 {
  if (The_Position >= List->StartPos && The_Position < List->EndPos)
  {
   return List->StartPos;
  }
  List = List->Next;
 }
 return -1;
}

/* -------------------------------------------------------------------- **
** -------	Get Reference End : returns end Position  	------- **
** -------	of the reference in wich The_Position is ...	------- **
** -------------------------------------------------------------------- */

static XmTextPosition	Get_Reference_End(This, The_Position)
c_AEditor	*This;
XmTextPosition	The_Position;
{
 XcAE_Escape_List	*List;
  
 List = This->List;
 while(List->Next != NULL)
 {
  if (The_Position >= List->StartPos && The_Position < List->EndPos)
  {
   return List->EndPos;
  }
  List = List->Next;
 }
 return -1;
}

/* -------------------------------------------------------------------- **
** -------	Get Reference List ...			  	------- **
** -------------------------------------------------------------------- */

static XcAE_Escape_List	*Get_Reference_List(This, StartPos, EndPos)
c_AEditor	*This;
int		StartPos;
int		EndPos;
{
 XcAE_Escape_List	*List, *Svg_List;
 XcAE_Escape_List	*Return_List;

 Allocate_Escape_List(&Return_List);
 Svg_List = Return_List;
  
 List = This->List;
 while(List->Next != NULL)
 {
  if ((List->StartPos >= StartPos) &&
      (List->EndPos <= EndPos))
  {
   Return_List->StartPos = List->StartPos;
   Return_List->EndPos = List->EndPos;
   Return_List->Type = List->Type;
   switch(List->Type)
   {
   case XcAE_RULER:
    Return_List->Data.Ruler = List->Data.Ruler;
    break;
   case XcAE_STYLE:
    Return_List->Data.Style = List->Data.Style;
    break;
   case XcAE_SPECIAL:
    Return_List->Data.Special = List->Data.Special;
    break;
   }
   Allocate_Escape_List(&Return_List->Next);
   Return_List = Return_List->Next;
  }
  List = List->Next;
 }
 return Svg_List;
}

/* -------------------------------------------------------------------- **
** -------	Make Reference List Relative : makes reference	------- **
** ------- list positions relatives to 'Starting' ....		------- **
** ------- This is usefull to handle bloc insertion		------- **
** -------------------------------------------------------------------- */

static void Make_Selection_Relative(Selection, Starting)
XcAE_Selection	*Selection;
XmTextPosition 	Starting;
{
 XcAE_Escape_List	*List;
  
 Selection->StartPos -= Starting;
 Selection->EndPos -= Starting;
  
 List = Selection->List;
 while(List->Next != NULL)
 {
  List->StartPos -= Starting;
  List->EndPos -= Starting;
  List = List->Next;
 }
}

/* -------------------------------------------------------------------- **
** ------------- Add Reference Position...			------- **
** -------------------------------------------------------------------- */

static void	Add_Reference_Position(This, Start, Length)
c_AEditor	*This;
XmTextPosition	Start;
XmTextPosition	Length;
{
 XcAE_Escape_List	*List;
 int			Ii;

 List = This->List;
 while(List->Next != NULL)
 {
  if (List->StartPos >= Start)
  {
   List->StartPos += Length;
   List->EndPos += Length;
  }
  List = List->Next;
 }

 /* ---------- Update marks that should be shifted ----------- */

 for (Ii = 0; Ii < XcAE_MAX_MARKS; Ii++)
  if (This->Mark[Ii] != -1)
   if (This->Mark[Ii] >= Start)
    This->Mark[Ii] += Length;

 /* -------------- Update the text length ... ---------------- */
 This->Text_Length += Length;
}

/* -------------------------------------------------------------------- **
** ----------- Insert a String ... 				------- **
** -------------------------------------------------------------------- */

static void	Insert_String(This, The_Position, Ptr, Length)
c_AEditor	*This;
XmTextPosition	The_Position;
char		*Ptr;
XmTextPosition	Length;
{
 XmTextPosition	Length_To_Realloc;
 int	Nbr_Page;

 Length_To_Realloc = 0;
 if (Length > This->Alloc_Free)
 {
  Nbr_Page = (((Length - This->Alloc_Free) / XcAE_AF_SIZE) + 1);
  This->Nbr_Page += Nbr_Page;
  Length_To_Realloc = Nbr_Page * XcAE_AF_SIZE;
  This->Alloc_Free += Length_To_Realloc;
 }
 This->Alloc_Free -= Length;

 if (Length_To_Realloc > 0)
  This->W_Text = Xc_realloc("char", This->W_Text, 
			    This->Nbr_Page * XcAE_AF_SIZE);

 memmove(This->W_Text + The_Position + Length,
	 This->W_Text + The_Position, This->Text_Length + 1 - The_Position);
 memmove(This->W_Text + The_Position, Ptr, Length);

 Add_Reference_Position(This, The_Position, Length);

 if (This->Set_Text_Flag != FALSE)
 {
  char	*ptr2;
      
  ptr2 = (char *)Xc_malloc("strndup", Length + 1);
  memcpy(ptr2, Ptr, Length);
  ptr2[Length] = '\0';
  XmTextInsert(This->Editor, The_Position, ptr2);
  Xc_free(ptr2);
 }
}

/* -------------------------------------------------------------------- **
** ----------- Delete a String ... 				------- **
** -------------------------------------------------------------------- */

static void	Delete_String(This, The_Position, Length)
c_AEditor	*This;
XmTextPosition	The_Position;
XmTextPosition	Length;
{
 XmTextPosition	The_Length;

 memcpy(This->W_Text + The_Position,
	This->W_Text + The_Position + Length,
	This->Text_Length - The_Position + 1 - Length);
 The_Length = This->Text_Length - Length;
 This->Alloc_Free += Length;
  
 Add_Reference_Position(This, The_Position, -Length);

 if (This->Set_Text_Flag != FALSE)
  XmTextReplace(This->Editor, The_Position, The_Position + Length, "");
}

/* -------------------------------------------------------------------- **
** -------- Insert reference Ruler : if this ruler is not yet --------- **
** -------- in the reference ruler list, then it will be added--------- **
** -------------------------------------------------------------------- */
static void	Insert_Reference_Ruler(This, The_Position, Ruler)
c_AEditor	*This;
XmTextPosition	The_Position;
c_TextRuler	*Ruler;
{
 XcAE_Escape_List	*List;
 Xc_HISTORY(("Adding a ruler in reference list...")); 
  
 List = This->List;
 while(List->Next != NULL) List = List->Next;
 List->StartPos = The_Position;
 if (The_Position == 0)
 {
  List->EndPos = XcAE_SRULER_KEYWORD_L + The_Position
   + strlen(Ruler->name);
 }
 else
 {
  List->EndPos = XcAE_RULER_KEYWORD_L + The_Position
   + strlen(Ruler->name);
 }
 List->Type = XcAE_RULER;
 List->Data.Ruler = Ruler;
 List->Flag = FALSE;
 Allocate_Escape_List(&List->Next);
}

/* -------------------------------------------------------------------- **
** -------- Insert reference Style : if this ruler is not yet --------- **
** -------- in the reference style list, then it will be added--------- **
** -------------------------------------------------------------------- */
static void	Insert_Reference_Style(This, The_Position, Style)
c_AEditor	*This;
XmTextPosition	The_Position;
c_TextStyle	*Style;
{
 XcAE_Escape_List *List;
 Xc_HISTORY(("Adding a style in reference list...")); 
  
 List = This->List;
 while(List->Next != NULL) List = List->Next; 
 List->StartPos = The_Position;
 List->EndPos = XcAE_STYLE_KEYWORD_L + The_Position +
  strlen(Style->name);
 List->Type = XcAE_STYLE;
 List->Data.Style = Style;
 List->Flag = FALSE;
 Allocate_Escape_List(&List->Next);
}

/* -------------------------------------------------------------------- **
** -------------------- Insert reference Newline ---------------------- **
** -------------------------------------------------------------------- */
static void	Insert_Reference_Newline(This, The_Position)
c_AEditor	*This;
XmTextPosition	The_Position;
{
 XcAE_Escape_List *List;
 Xc_HISTORY(("Adding a NewLine in reference list...")); 
  
 List = This->List;
 while(List->Next != NULL) List = List->Next;  
 List->StartPos = The_Position;
 List->EndPos = XcAE_CR_KEYWORD_L + The_Position;
 List->Type = XcAE_SPECIAL;
 List->Data.Special = XcAE_SPEC_CR;
 List->Flag = FALSE;
 Allocate_Escape_List(&List->Next);
}

/* -------------------------------------------------------------------- **
** ------------------- Insert reference NextCadre --------------------- **
** -------------------------------------------------------------------- */
static void	Insert_Reference_Nextcadre(This, The_Position)
c_AEditor	*This;
XmTextPosition	The_Position;
{
 XcAE_Escape_List *List;
 Xc_HISTORY(("Adding a NextCadre in reference list...")); 
    
 List = This->List;
 while(List->Next != NULL) List = List->Next; 
 List->StartPos = The_Position;
 List->EndPos = XcAE_NC_KEYWORD_L + The_Position;
 List->Type = XcAE_SPECIAL;
 List->Data.Special = XcAE_SPEC_NC;
 List->Flag = FALSE;
 Allocate_Escape_List(&List->Next);
}

/* -------------------------------------------------------------------- **
** ------------------ Insert reference NewParagraph ------------------- **
** -------------------------------------------------------------------- */
static void	Insert_Reference_Newparagraph(This, The_Position)
c_AEditor	*This;
XmTextPosition	The_Position;
{
 XcAE_Escape_List *List;
 Xc_HISTORY(("Adding a NewParagraph in reference list...")); 
  
 List = This->List;
 while(List->Next != NULL) List = List->Next; 
 List->StartPos = The_Position;
 List->EndPos = XcAE_NP_KEYWORD_L + The_Position;
 List->Type = XcAE_SPECIAL;
 List->Data.Special = XcAE_SPEC_NP;
 List->Flag = FALSE;
 Allocate_Escape_List(&List->Next);
}

/* ------------------------------------------------------------------ **
** ------------- 	Do Ruler Insertion function ...  ------------ **
** ----- Calls the 'Insert_Reference_Ruler', increments the 	----- **
** ----- current position in text.				----- **
** ------------------------------------------------------------------ */
static void	Do_Ruler_Insertion(This, Ruler)
c_AEditor	*This;
c_TextRuler	*Ruler;
{
 XmTextPosition	The_Position;
 XmTextPosition	Length;
 char			*ruler_keyword;
 int			ruler_kw_length;
 char			string[XcMAX_NAME_LENGTH];
  
 The_Position = This->Position_In_Text;
 if (This->Position_In_Text == 0)
 {
  ruler_keyword = XcAE_SRULER_KEYWORD;
  ruler_kw_length = XcAE_SRULER_KEYWORD_L - XcAE_RULER_KEYWORD_O;
 }
 else
 {
  ruler_keyword = XcAE_RULER_KEYWORD;
  ruler_kw_length = XcAE_RULER_KEYWORD_L - XcAE_RULER_KEYWORD_O;
 }
  
 This->Modify_Flag = FALSE;
 memcpy(string, ruler_keyword, ruler_kw_length);
 Length = strlen(Ruler->name);
 memcpy(string + ruler_kw_length, Ruler->name, Length);
 string[ruler_kw_length + Length] = ']';
 Length += ruler_kw_length + XcAE_RULER_KEYWORD_O;

 Insert_String(This, This->Position_In_Text, string, Length);
 This->Position_In_Text += Length;
  
 This->Modify_Flag = TRUE;
 Insert_Reference_Ruler(This, The_Position, Ruler);
 XmTextSetInsertionPosition(This->Editor, This->Position_In_Text);
}

/* ------------------------------------------------------------------ **
** ------------- 	Do Style Insertion function ...  ------------ **
** ----- Calls the 'Insert_Reference_Style', increments the 	----- **
** ----- current position in text.				----- **
** ------------------------------------------------------------------ */
static void	Do_Style_Insertion(This, Style)
c_AEditor	*This;
c_TextStyle	*Style;
{
 XmTextPosition	Length;
 XmTextPosition	The_Position;
  
 The_Position = This->Position_In_Text;
 This->Modify_Flag = FALSE;
 Insert_String(This, This->Position_In_Text,
	       XcAE_STYLE_KEYWORD, XcAE_STYLE_KEYWORD_L);

 This->Position_In_Text = The_Position + 
  XcAE_STYLE_KEYWORD_L - XcAE_STYLE_KEYWORD_O;
 Length = strlen(Style->name);
  
 Insert_String(This, This->Position_In_Text, Style->name, Length);
 This->Position_In_Text = The_Position + XcAE_STYLE_KEYWORD_L + Length;
  
 This->Modify_Flag = TRUE;
 Insert_Reference_Style(This, The_Position, Style);
 XmTextSetInsertionPosition(This->Editor, This->Position_In_Text);
}

/* ------------------------------------------------------------------ **
** ------------- 	Do Newline Insertion function ...  ---------- **
** ----- Calls the 'Insert_Reference_Newline', increments the 	----- **
** ----- current position in text.				----- **
** ------------------------------------------------------------------ */
static void	Do_Newline_Insertion(This)
c_AEditor	*This;
{
 XmTextPosition	The_Position;
  
 The_Position = This->Position_In_Text;
 This->Modify_Flag = FALSE;
 Insert_String(This, This->Position_In_Text, 
	       XcAE_CR_KEYWORD, XcAE_CR_KEYWORD_L);
 This->Position_In_Text += XcAE_CR_KEYWORD_L;

 This->Modify_Flag = TRUE;
 Insert_Reference_Newline(This, The_Position);
 XmTextSetInsertionPosition(This->Editor, This->Position_In_Text);
}

/* ------------------------------------------------------------------ **
** ------------     Do Nextcadre Insertion function ...    ---------- **
** ----- Calls the 'Insert_Reference_Nextcadre', increments the	----- **
** ----- current position in text.				----- **
** ------------------------------------------------------------------ */
static void	Do_Nextcadre_Insertion(This)
c_AEditor	*This;
{
 XmTextPosition	The_Position;
  
 The_Position = This->Position_In_Text;
 This->Modify_Flag = FALSE;
 Insert_String(This, This->Position_In_Text, 
	       XcAE_NC_KEYWORD, XcAE_NC_KEYWORD_L);
 This->Position_In_Text += XcAE_NC_KEYWORD_L;

 This->Modify_Flag = TRUE;
 Insert_Reference_Nextcadre(This, The_Position);
 XmTextSetInsertionPosition(This->Editor, This->Position_In_Text);
}

/* ------------------------------------------------------------------ **
** ----------     Do Newparagraph Insertion function ...    -------- **
** --- Calls the 'Insert_Reference_Newparagraph', increments the --- **
** ----- current position in text.				----- **
** ------------------------------------------------------------------ */
static void	Do_Newparagraph_Insertion(This)
c_AEditor	*This;
{
 XmTextPosition	The_Position;
  
 The_Position = This->Position_In_Text;

 This->Modify_Flag = FALSE;
 Insert_String(This, This->Position_In_Text, 
	       XcAE_NP_KEYWORD, XcAE_NP_KEYWORD_L);
 This->Position_In_Text += XcAE_NP_KEYWORD_L;

 This->Modify_Flag = TRUE;
 Insert_Reference_Newparagraph(This, The_Position);
 XmTextSetInsertionPosition(This->Editor, This->Position_In_Text);
}

/* --------------------------------------------------------------------	**
** ------------   Callback to verify Text Modification   -------------- **
** -------------------------------------------------------------------- */

static void Modify_Verify_Callback(WEditor, This, cbs)
Widget				WEditor;
c_AEditor			*This;
XmTextVerifyCallbackStruct	*cbs;
{
 char			*txt;
 int			txt_length, i,j;
 boolean		Flag;
 XmTextPosition	The_Pos;
 XmTextPosition	Temp1, Temp2;
  
  
 if (This->Modify_Flag == FALSE)
 {
  return;
 }
 Xc_TRACE(("Modify_Verify_Callback"));

 The_Pos = This->Position_In_Text;
  
 if (cbs->text->ptr != NULL && cbs->text->length)
 {
  /* -------- Text insertion .... ---------- */
      
  if (XmTextGetSelectionPosition(This->Editor, &Temp1, &Temp2) == True 
      && cbs->startPos != cbs->endPos) 
  {
   Flag = TRUE;
   This->Position_In_Text = cbs->endPos;
  }
  else
   Flag = FALSE;
  This->Set_Text_Flag = TRUE;
  This->Motion_Flag = This->Modify_Flag = FALSE; 
  txt = cbs->text->ptr;
  txt_length = cbs->text->length;
  cbs->text->length = 0; 
  for(i = j = 0; i < txt_length; i++)
  {
   if (txt[j] == '\r' || txt[j] == '\n')
   {
    if (j)
    {
     Insert_String(This, This->Position_In_Text, txt, j);
     This->Position_In_Text += j;
    }
    Xc_TRACE(("must insert a paragraph"));
    Do_Newparagraph_Insertion(This);
    This->Modify_Flag = FALSE;
    txt += j + 1; j = 0;
   }
   else 
    j++;
  }
  if (j)
  {
   Insert_String(This, This->Position_In_Text, txt, j);
   This->Position_In_Text += j;
  }
  Xc_TRACE(("place curseur to position: %d", This->Position_In_Text));
  This->Motion_Flag = This->Modify_Flag = This->Set_Text_Flag = TRUE;
  XmTextSetInsertionPosition(This->Editor, This->Position_In_Text);
  if (Flag)
  {
   Make_Selection(This, This->Selection, cbs->startPos, cbs->endPos);
   Xc_TRACE(("Replace Selection: left: %d right: %d", 
	     cbs->startPos, cbs->endPos));
   This->Motion_Flag = This->Modify_Flag = This->Set_Text_Flag = FALSE;
   Delete_Selection(This, This->Selection);
   This->Motion_Flag = This->Modify_Flag = This->Set_Text_Flag = TRUE;
   This->Position_In_Text = cbs->startPos;
  }
  return;
 }
  
 /* ------------- Text Deletion ...... ----------- */
  
 if (cbs->startPos == cbs->endPos) return;
 /* ---- Start Pos and End Pos of deletion are same is impossible ----- */
  
 if (XmTextGetSelectionPosition(This->Editor, &Temp1, &Temp2) == False)
 {
  /* ------------- There is no selection .... ------------- */
  if (cbs->newInsert != cbs->startPos)
  {
   Flag = Is_In_Reference(This, The_Pos - 1);
   /*  Position_In_Text -1 bicoz we delete the previous character ...*/
   if (Flag == TRUE)
   {
    cbs->doit = False;
    Make_Selection(This,This->Selection, cbs->startPos, cbs->endPos);
    The_Pos = This->Selection->StartPos;
    This->Motion_Flag = This->Modify_Flag = FALSE;
    This->Set_Text_Flag = TRUE;
    Delete_Selection(This, This->Selection);
    XmTextSetInsertionPosition(This->Editor, The_Pos);
    This->Position_In_Text = The_Pos;
    This->Motion_Flag = This->Modify_Flag = TRUE;
   }
   /* ------- if backspace was not in a reference .... ----------- */
   else
   {
    This->Set_Text_Flag = FALSE;
    This->Motion_Flag = TRUE;
    Delete_String(This, The_Pos - 1, 1);
    This->Set_Text_Flag = TRUE;
    This->Position_In_Text -= 1;
   }
  }
  else
  {
   /* --------- It is not a backspace, it is a delete ... --------- */
   Flag = Is_In_Reference(This, The_Pos);
   /*  Position_In_Text bicoz we delete the character ...*/
   if (Flag == TRUE)
   {
    cbs->doit = False;
    Make_Selection(This,This->Selection, cbs->startPos, cbs->endPos);
    The_Pos = This->Selection->StartPos;
    This->Motion_Flag = This->Modify_Flag = FALSE;
    This->Set_Text_Flag = TRUE;
    Delete_Selection(This, This->Selection);
    XmTextSetInsertionPosition(This->Editor, The_Pos);
    This->Position_In_Text = The_Pos;
    This->Motion_Flag = This->Modify_Flag = TRUE;
   }
   else
   {
    /* ------ Delete was in text, not in a reference ... ------- */
    This->Set_Text_Flag = FALSE;
    Delete_String(This, The_Pos, 1);
    This->Set_Text_Flag = TRUE;
   }
  }
  return;
 }
  
 Make_Selection(This, This->Selection, cbs->startPos, cbs->endPos);
 This->Motion_Flag = This->Modify_Flag = This->Set_Text_Flag = FALSE;

 cbs->startPos = This->Selection->StartPos;
 cbs->endPos = This->Selection->EndPos;

 Delete_Selection(This, This->Selection);
 This->Motion_Flag = This->Modify_Flag =  This->Set_Text_Flag = TRUE;
 XmTextClearSelection(This->Editor, CurrentTime);
 XmTextSetInsertionPosition(This->Editor, cbs->startPos);
}

/* --------------------------------------------------------------------	**
** ------------   Callback to verify Cursor Motion	 -------------- **
** -------------------------------------------------------------------- */

static void Motion_Verify_Callback(WEditor, This, cbs)
Widget				WEditor;
c_AEditor			*This;
XmTextVerifyCallbackStruct	*cbs;
{
 XmTextPosition	left, right;
 long			newInsert, toto;
  
 if (cbs->reason != XmCR_MOVING_INSERT_CURSOR)
  return;
  
 if (This->Motion_Flag == TRUE) 
 {
  Xc_TRACE(("Motion_Verify_Callback: newpos: %d", (int)cbs->newInsert));
    
  Xc_TRACE(("newInsert: %d currInsert: %d selection: %s",  (int)cbs->newInsert, (int)cbs->currInsert, This->is_selection?"TRUE":"FALSE"));
    
  newInsert = cbs->newInsert;
    
  if ((!This->is_selection && cbs->newInsert > cbs->currInsert) ||
      (This->is_selection && (cbs->newInsert > This->old_insert ||
			      (cbs->newInsert == This->old_insert && 
			       cbs->newInsert < cbs->currInsert))))
  {
   This->old_insert = cbs->newInsert;
   if (Is_In_Reference(This, (int)cbs->newInsert-1))
   {
    if ((toto = Get_Reference_End(This, cbs->newInsert)) != -1)
    {
     cbs->currInsert = cbs->newInsert;
     cbs->newInsert = toto;
    }
   }
  }
  else
  {
   This->old_insert = cbs->newInsert;
   if (Is_In_Reference(This, (int)cbs->newInsert))
   {
    if ((toto = Get_Reference_Start(This, cbs->newInsert)) != -1)
    {
     cbs->currInsert = cbs->newInsert;
     cbs->newInsert = toto;
    }
   }
  }
  if (cbs->newInsert != newInsert)
  {
   This->Motion_Flag = FALSE;
   cbs->doit = False;
   if (XmTextGetSelectionPosition(This->Editor, &left, &right) == True)
   {
    Xc_TRACE(("selection: left: %d right: %d", left, right));
    This->is_selection = TRUE;
    if (left == newInsert)
     XmTextSetSelection(This->Editor, cbs->newInsert, 
			right, CurrentTime);
    else
     XmTextSetSelection(This->Editor, left,
			cbs->newInsert, CurrentTime);
   }
   else
    This->is_selection = FALSE;
   XmTextSetInsertionPosition(This->Editor, cbs->newInsert);
   This->Motion_Flag = TRUE;
  }
  This->Position_In_Text = cbs->newInsert;
  Xc_TRACE(("newInsert: %d currInsert: %d",  (int)cbs->newInsert, 
	    (int)cbs->currInsert));
 }
}

static void cb_handle_event(w, This, event)
Widget		w;
c_AEditor	*This;
XEvent		*event;
{
 XmTextPosition	tpos, left, right;
 boolean		Control;
  
 Control = GlobKeyboard->Control;
 F(GlobKeyboard).EventLook(GlobKeyboard, *event);
 /*  Xc_TRACE(("handle event. TYPE: %d BUFFER: %s %c%c", event->type,
     GlobKeyboard->buf_keyb, GlobKeyboard->Control?'C':' ',
     GlobKeyboard->Shift?'S':' ')); */
  
 if (GlobKeyboard->insertBox)
 {
  XcAE_Escape_List *List;
  XmTextPosition The_Position, Best_Position;
  c_TextStyle	*Style;
    
  Best_Position = 10000000;
  The_Position = This->Position_In_Text;
  Style = This->FirstDefaultStyle;
    
  List = This->List;
  while(List)
  {
   if (List->StartPos < The_Position)
   {
    if (List->Type == XcAE_STYLE)
    {
     if  ((The_Position - List->StartPos) < Best_Position)
     {
      Best_Position = The_Position - List->StartPos;
      Style = List->Data.Style;
     }
    }
   }
   else
    break;
   List = List->Next;
  }
    
  F(GlobKeyboard).CreateBoxInsert(GlobKeyboard, This->w_parent, Style,
				  cb_insert_char, This);
  Xc_TRACE(("CURRENT STYLE: %s", Style->name));
    
  GlobKeyboard->insertBox = FALSE;
  return;
 }
  
 if ((event->type == ButtonPress && event->xbutton.button == 1 
      && Control && GlobKeyboard->buf_length))
 {
  Xc_TRACE(("INSERT CUT BUFFER: %s", GlobKeyboard->buf_keyb));
  if (XmTextGetSelectionPosition(This->Editor, &left, &right) == True)
  {
   XmTextRemove(This->Editor);
   tpos = XmTextGetInsertionPosition(This->Editor);
   XmTextInsert(This->Editor, tpos, GlobKeyboard->buf_keyb);
  }
  else
  {
   tpos = XmTextGetInsertionPosition(This->Editor);
   XmTextInsert(This->Editor, tpos, GlobKeyboard->buf_keyb);
   XmTextSetInsertionPosition(This->Editor, 
			      tpos + GlobKeyboard->buf_length);
  }
 }
}

static void cb_insert_char(This, c)
c_AEditor	*This;
char		c;
{
 char str[2];
 XmTextPosition	tpos, left, right;
  
 Xc_TRACE(("INSERT CHAR: %c (0x%x)", c, (unsigned int)c));
 str[0] = c; str[1] = '\0';
 if (XmTextGetSelectionPosition(This->Editor, &left, &right) == True)
 {
  XmTextRemove(This->Editor);
  tpos = XmTextGetInsertionPosition(This->Editor);
  XmTextInsert(This->Editor, tpos, str);
 }
 else
 {
  tpos = XmTextGetInsertionPosition(This->Editor);
  XmTextInsert(This->Editor, tpos, str);
  XmTextSetInsertionPosition(This->Editor, tpos + 1);
 }
}

/* -------------------------------------------------------------------- **
** -------	Insert_Selection ...				------- **
** -------------------------------------------------------------------- */

static void	Insert_Selection(This, Selection)
c_AEditor	*This;
XcAE_Selection	*Selection;
{
 XcAE_Escape_List *List;
 XcAE_Escape_List *Selection_List;
 Xc_HISTORY(("Insert a bloc..."));
  
 if (Selection->StartPos == Selection->EndPos|| Selection->String == NULL)
 {
  Xc_TRACE(("No Selection..."));
  return;
 }
 Insert_String(This, This->Position_In_Text, Selection->String,
	       Selection->EndPos - Selection->StartPos);
  
 List = This->List;
 while(List->Next != NULL) List = List->Next;
  
 Selection_List = Selection->List;
 while(Selection_List->Next != NULL)
 {
  /* ----- copy the Escape_List structure -------- */
      
  List->StartPos = Selection_List->StartPos+This->Position_In_Text;
  List->EndPos = Selection_List->EndPos+This->Position_In_Text;;
  List->Type = Selection_List->Type;
  switch(Selection_List->Type)
  {
  case XcAE_RULER:
   List->Data.Ruler = Selection_List->Data.Ruler;
   break;
  case XcAE_STYLE:
   List->Data.Style = Selection_List->Data.Style;
   break;
  case XcAE_SPECIAL:
   List->Data.Special = Selection_List->Data.Special;
   break;
  }
  Allocate_Escape_List(&List->Next);
  List = List->Next;
  Selection_List = Selection_List->Next;
 }
  
 Xc_HISTORY(("Insert a bloc ok"));
}

/* -------------------------------------------------------------------- **
** -------	Delete Selection ...				------- **
** -------------------------------------------------------------------- */

static void	Delete_Selection(This, Selection)
c_AEditor	*This;
XcAE_Selection	*Selection;
{
 int	Ii;
 XcAE_Escape_List	*List, *Prev_List;
 XcAE_Escape_List	*Selection_List;
 Xc_HISTORY(("Delete a bloc ..."));

 if ( Selection->StartPos == Selection->EndPos || Selection->String == NULL)
 {
  Xc_TRACE(("No Selection..."));
  return;
 }
  
 /* --------- Removes Marks that are in the selection ----------------	*/

 for (Ii = 0; Ii < XcAE_MAX_MARKS; Ii++)
  if ((Selection->StartPos <= This->Mark[Ii]) && 
      (Selection->EndPos > This->Mark[Ii]))
  {
   This->Mark[Ii] = -1;
  }
 /* --------- Removes Marks that were in the selection over ----------	*/

 Selection_List = Selection->List;
 while(Selection_List->Next)
 {
  List = Prev_List = This->List;
  while(List->Next != NULL)
  {
   /* ---- test with each reference in text reference List ---- */
   if (List->StartPos == Selection_List->StartPos)
   {
    Xc_TRACE(("Remove reference... start = %ld, end = %ld",
	      List->StartPos, List->EndPos));
    if (Prev_List == List)
     This->List = List->Next;
    else
     Prev_List->Next = List->Next;
    Xc_free(List);
    break;
   }
   Prev_List = List;
   List = List->Next;
  }
  Selection_List = Selection_List->Next;
 }
  
 Delete_String(This, Selection->StartPos, 
	       Selection->EndPos - Selection->StartPos);
  
 Xc_HISTORY(("Delete a bloc ok"));
}

/* -------------------------------------------------------------------- **
** -------	Make Selection  method for Advanced_Editor Box	-------	**
** -------------------------------------------------------------------- */

static void	Make_Selection(This, Selection, StartPos, EndPos)
c_AEditor	*This;
XcAE_Selection	*Selection;
XmTextPosition	StartPos;
XmTextPosition	EndPos;
{
 XmTextPosition	Buffer;
  
 Xc_HISTORY(("Make Selection... Limits = %ld, %ld", StartPos, EndPos));
  
 Clear_Selection(Selection);
  
 if (Is_In_Reference(This, EndPos-1) == TRUE)
 {
  if ((Buffer = Get_Reference_End(This, EndPos-1)) != -1)
   EndPos = Buffer;
 }

 if (Is_In_Reference(This, StartPos) == TRUE)
 {
  if ((Buffer = Get_Reference_Start(This, StartPos)) != -1)
   StartPos = Buffer;
 }
 Xc_TRACE(("Limits = %ld, %ld", StartPos, EndPos));

 Selection->String = (char *)Xc_malloc("selection", EndPos-StartPos+1);

 memcpy(Selection->String, This->W_Text + StartPos, EndPos - StartPos);
 Selection->String[EndPos - StartPos] = '\0';
  
 Selection->List = Get_Reference_List(This, StartPos, EndPos);
  
 Selection->StartPos = StartPos;
 Selection->EndPos = EndPos;
  
 Xc_HISTORY(("Make Selection ok"));
}

/* -------------------------------------------------------------------- **
** -------	Set Default Ruler : Sets the default ruler 	------- **
** -------	(Internal data & entry in interface (ItemMenu)	------- **
** -------------------------------------------------------------------- */
static void 	Set_Default_Ruler(This, The_Ruler)
c_AEditor	*This;
c_TextRuler	*The_Ruler;
{
 Xc_TRACE(("Default Ruler is now '%s'", The_Ruler->name));

 This->DefaultRuler = The_Ruler;
 F(This->RulerIM).setSelection(This->RulerIM, The_Ruler);
 return;
}

/* -------------------------------------------------------------------- **
** -------	Set Default Style : Sets the default style 	------- **
** -------	(Internal data & entry in interface (ItemMenu)	------- **
** -------------------------------------------------------------------- */

static void 	Set_Default_Style(This, The_Style)
c_AEditor	*This;
c_TextStyle	*The_Style;
{
 Xc_TRACE(("Default Style is now '%s'", The_Style->name));

 This->DefaultStyle = The_Style;
 F(This->StyleIM).setSelection(This->StyleIM, The_Style);
 return;
}

/* -------------------------------------------------------------------- **
** ------------ FONCTION creant la table de correspondances ----------- **
** ------------ entre les positions dans l'objet texte et   ----------- **
** ------------ les positions dans la widget texte ....     ----------- **
** -------------------------------------------------------------------- */

static void	Mouline_Texte(This)
c_AEditor	*This;
{
 text_seq_t	*seq;
 c_TextRuler	*Ruler;
 c_TextStyle	*Style;
 boolean	set_NP = FALSE;

 Xc_HISTORY(("Transforming text..."));

 F(This->Text).getFirstSeq(This->Text, 0, &Style, &Ruler);
  
 Set_Default_Ruler(This, Ruler);
 Set_Default_Style(This, Style);

 This->FirstDefaultRuler = Ruler;
 This->FirstDefaultStyle = Style;
 while ((seq = F(This->Text).getNextSeq(This->Text, -1)) != NULL)
 {
  if (seq->ref != NULL)
  {
   switch (seq->ref->type)
   {
   case XcT_TEXT_RULER :
    Do_Ruler_Insertion(This, seq->ref->data.ruler);
    /*Xc_TRACE(("Insert Ruler"));*/
    set_NP = FALSE;
    break;
   case XcT_TEXT_STYLE :
    if (set_NP)
    {
     Do_Newparagraph_Insertion(This);
     set_NP = FALSE;
     /*Xc_TRACE(("Insert NP"));*/
    }
    Do_Style_Insertion(This, seq->ref->data.style);
    /*Xc_TRACE(("Insert Style"));*/
    break;
   case XcT_INDEX :
   case XcT_SECTION :
   case XcT_HYPERTEXT :
   case XcT_UNKNOWN :
    Xc_TRACE(("Not implemented yet..."));
    break;
   }
  }
  else
  {
   if (set_NP)
   {
    Do_Newparagraph_Insertion(This);
    set_NP = FALSE;
    /*Xc_TRACE(("Insert NP"));*/
   }
  }

  if (seq->len != 0)
  {
   Insert_String(This, This->Position_In_Text, seq->start, seq->len);
   This->Position_In_Text += seq->len;
  }

  switch(seq->special)
  {
  case XcT_END_OF_LINE :
   Do_Newline_Insertion(This);
   /*Xc_TRACE(("Insert CR"));*/
   break;
  case XcT_NOTHING :
   break;
  case XcT_END_OF_TEXT :
   break;
  case XcT_END_OF_PAGE :
   Do_Nextcadre_Insertion(This);
   /*Xc_TRACE(("Insert NF"));*/
   break;
  case XcT_END_OF_PARAGRAPH :
   set_NP = TRUE;
   break;
  }
  if (seq->special == XcT_END_OF_TEXT) break;
 }
 if (set_NP)
 {
  Do_Newparagraph_Insertion(This);
  Xc_TRACE(("Insert NP"));
 }
 Xc_HISTORY(("Make text ok"));
}

/* -------------------------------------------------------------------- **
** -------	Callback of The Ruler Item Menu ...		------- **
** -------------------------------------------------------------------- */
static void 	RulerIM_Callback(w, This, The_Ruler)
Widget		w;
c_AEditor	*This;
c_TextRuler	*The_Ruler;
{
 Xc_TRACE(("in RulerIM callback, ruler: %s", The_Ruler->name));
 Set_Default_Ruler(This, The_Ruler);
 XmProcessTraversal(This->Editor, XmTRAVERSE_CURRENT);
 return;
}

/* -------------------------------------------------------------------- **
** -------	Callback of The Style Item Menu ...		------- **
** -------------------------------------------------------------------- */
static void 	StyleIM_Callback(w, This, The_Style)
Widget		w;
c_AEditor	*This;
c_TextStyle	*The_Style;
{
  Xc_TRACE(("in StyleIM callback, style: %s", The_Style->name));
  Set_Default_Style(This, The_Style);
  XmProcessTraversal(This->Editor, XmTRAVERSE_CURRENT);
}

/* -------------------------------------------------------------------- **
** -----	Callback of MenuEditCut 			-------	**
** -------------------------------------------------------------------- */
static void	MenuEditCut(The_Widget, This)
Widget	       	The_Widget;
c_AEditor	*This;
{
  XmTextPosition	StartPos, EndPos, The_Pos;

  if (XmTextGetSelectionPosition(This->Editor, &StartPos, &EndPos) == False)
    {
      Xc_TRACE(("No Selection"));
      return;
    }
  Make_Selection(This, This->Clip_Buffer, StartPos, EndPos);
  Make_Selection_Relative(This->Clip_Buffer, This->Clip_Buffer->StartPos);
  
  Make_Selection(This, This->Selection, StartPos, EndPos);
  This->Motion_Flag = This->Modify_Flag = FALSE;
  The_Pos = This->Selection->StartPos;
  Delete_Selection(This, This->Selection);
  This->Motion_Flag = This->Modify_Flag = TRUE;
  This->Position_In_Text = The_Pos;
  XmTextSetInsertionPosition(This->Editor, The_Pos);
  XmTextClearSelection(This->Editor, CurrentTime);
  
  Xc_TRACE(("selection: >>%s<<", This->Clip_Buffer->String));
  XRotateBuffers(XtDisplay(This->Editor), 1);
  XStoreBytes(XtDisplay(This->Editor), This->Clip_Buffer->String, 
	      strlen(This->Clip_Buffer->String));
}

/* -------------------------------------------------------------------- **
** -----	Callback of MenuEditCopy 			  -----	**
** -------------------------------------------------------------------- */

static void	MenuEditCopy(The_Widget, This)
Widget	       	The_Widget;
c_AEditor	*This;
{
  XmTextPosition	StartPos, EndPos;
  
  if (XmTextGetSelectionPosition(This->Editor, &StartPos, &EndPos) == False)
    {
      Xc_TRACE(("No Selection"));
      return;
    }
  
  Make_Selection(This, This->Clip_Buffer, StartPos, EndPos);
  Make_Selection_Relative(This->Clip_Buffer, This->Clip_Buffer->StartPos);
  Xc_TRACE(("selection: >>%s<<", This->Clip_Buffer->String));
  
  XRotateBuffers(XtDisplay(This->Editor), 1);
  XStoreBytes(XtDisplay(This->Editor), This->Clip_Buffer->String, 
	      strlen(This->Clip_Buffer->String));
}
 
/* -------------------------------------------------------------------- **
** -----	Callback of MenuEditPaste 			  -----	**
** -------------------------------------------------------------------- */

static void	MenuEditPaste(The_Widget, This)
Widget	       	The_Widget;
c_AEditor	*This;
{
  XmTextPosition	left, right, pos;
  
  if (This->Clip_Buffer->String == NULL)
  {
    Xc_TRACE(("No Cut Buffer"));
    return;
  }
  if (XmTextGetSelectionPosition(This->Editor, &left, &right) == True)
  {
    if (left != right)
    {
      Make_Selection(This, This->Selection, left, right);
      This->Motion_Flag = This->Modify_Flag = FALSE;
      Delete_Selection(This, This->Selection);
      This->Motion_Flag = This->Modify_Flag = TRUE;
      This->Position_In_Text = left;
      XmTextSetInsertionPosition(This->Editor, This->Position_In_Text);
    }
    XmTextClearSelection(This->Editor, CurrentTime);
  }
  pos = This->Position_In_Text + This->Clip_Buffer->EndPos;
  
  Xc_TRACE(("selection: >>%s<<", This->Clip_Buffer->String));
  This->Motion_Flag = This->Modify_Flag = FALSE;
  Insert_Selection(This, This->Clip_Buffer);
  This->Motion_Flag = This->Modify_Flag = TRUE;
  XmTextSetInsertionPosition(This->Editor, pos);
  This->Position_In_Text = pos;
}

/* -------------------------------------------------------------------- **
** -----	Callback of MenuRulerInsert 			  -----	**
** -------------------------------------------------------------------- */

static void	MenuRulerInsert(The_Widget, This)
Widget	       	The_Widget;
c_AEditor	*This;
{
  XmTextPosition	left, right;
  
  if (XmTextGetSelectionPosition(This->Editor, &left, &right) == True)
  {
    if (left != right)
    {
      Make_Selection(This, This->Selection, left, right);
      This->Motion_Flag = This->Modify_Flag = FALSE;
      Delete_Selection(This, This->Selection);
      This->Motion_Flag = This->Modify_Flag = TRUE;
      This->Position_In_Text = left;
      XmTextSetInsertionPosition(This->Editor, This->Position_In_Text);
    }
    XmTextClearSelection(This->Editor, CurrentTime);
  }
  Do_Ruler_Insertion(This, This->DefaultRuler);
  XmProcessTraversal(This->Editor, XmTRAVERSE_CURRENT);
}

/* -------------------------------------------------------------------- **
** -----	Callback of MenuStyleInsert 			  -----	**
** -------------------------------------------------------------------- */

static void	MenuStyleInsert(The_Widget, This)
Widget	       	The_Widget;
c_AEditor	*This;
{
  XmTextPosition	left, right;
  
  if (XmTextGetSelectionPosition(This->Editor, &left, &right) == True)
  {
    if (left != right)
    {
      Make_Selection(This, This->Selection, left, right);
      This->Motion_Flag = This->Modify_Flag = FALSE;
      Delete_Selection(This, This->Selection);
      This->Motion_Flag = This->Modify_Flag = TRUE;
      This->Position_In_Text = left;
      XmTextSetInsertionPosition(This->Editor, This->Position_In_Text);
    }
    XmTextClearSelection(This->Editor, CurrentTime);
  }
  Do_Style_Insertion(This, This->DefaultStyle);
  XmProcessTraversal(This->Editor, XmTRAVERSE_CURRENT);
}

/* -------------------------------------------------------------------- **
** -----	Callback of MenuNewlineInsert 			  -----	**
** -------------------------------------------------------------------- */

static void	MenuNewlineInsert(The_Widget, This)
Widget	       	The_Widget;
c_AEditor	*This;
{
  XmTextPosition	left, right;
  
  if (XmTextGetSelectionPosition(This->Editor, &left, &right) == True)
  {
    if (left != right)
    {
      Make_Selection(This, This->Selection, left, right);
      This->Motion_Flag = This->Modify_Flag = FALSE;
      Delete_Selection(This, This->Selection);
      This->Motion_Flag = This->Modify_Flag = TRUE;
      This->Position_In_Text = left;
      XmTextSetInsertionPosition(This->Editor, This->Position_In_Text);
    }
    XmTextClearSelection(This->Editor, CurrentTime);
  }
  Do_Newline_Insertion(This);
  XmProcessTraversal(This->Editor, XmTRAVERSE_CURRENT);
}

/* -------------------------------------------------------------------- **
** -----	Callback of MenuNextcadreInsert 		  -----	**
** -------------------------------------------------------------------- */

static void	MenuNextcadreInsert(The_Widget, This)
Widget	       	The_Widget;
c_AEditor	*This;
{
  XmTextPosition	left, right;
  
  if (XmTextGetSelectionPosition(This->Editor, &left, &right) == True)
  {
    if (left != right)
    {
      Make_Selection(This, This->Selection, left, right);
      This->Motion_Flag = This->Modify_Flag = FALSE;
      Delete_Selection(This, This->Selection);
      This->Motion_Flag = This->Modify_Flag = TRUE;
      This->Position_In_Text = left;
      XmTextSetInsertionPosition(This->Editor, This->Position_In_Text);
    }
    XmTextClearSelection(This->Editor, CurrentTime);
  }
  Do_Nextcadre_Insertion(This); 
  XmProcessTraversal(This->Editor, XmTRAVERSE_CURRENT);
}

/* -------------------------------------------------------------------- **
** -----	Callback of MenuTextSetMark1 			  -----	**
** -------------------------------------------------------------------- */

static void	MenuTextSetMark1(The_Widget, This)
Widget	       	The_Widget;
c_AEditor	*This;
{
  if (Is_In_Reference(This, This->Position_In_Text) == FALSE)
    This->Mark[0] = This->Position_In_Text;
#ifndef NTRACE
  else
    Xc_TRACE(("Can't set a mark in a reference..."));
#endif
 }

/* -------------------------------------------------------------------- **
** -----	Callback of MenuTextSetMark2 			  -----	**
** -------------------------------------------------------------------- */

static void	MenuTextSetMark2(The_Widget, This)
Widget	       	The_Widget;
c_AEditor	*This;
{
 if (Is_In_Reference(This, This->Position_In_Text) == FALSE)
  This->Mark[1] = This->Position_In_Text;
#ifndef NTRACE
 else
  Xc_TRACE(("Can't set a mark in a reference..."));
#endif
}

/* -------------------------------------------------------------------- **
** -----	Callback of MenuTextGotoMark1 			  -----	**
** -------------------------------------------------------------------- */

static void	MenuTextGotoMark1(The_Widget, This)
Widget	       	The_Widget;
c_AEditor	*This;
{
 if (This->Mark[0] == -1)
 {
  Xc_TRACE(("No mark set (mark may be bad)"));
  return;
 }
 This->Motion_Flag = TRUE;
 XmTextSetInsertionPosition(This->Editor, This->Mark[0]);
}

/* -------------------------------------------------------------------- **
** -----	Callback of MenuTextSetMark2 			  -----	**
** -------------------------------------------------------------------- */

static void	MenuTextGotoMark2(The_Widget, This)
Widget	       	The_Widget;
c_AEditor	*This;
{
 if (This->Mark[1] == -1)
 {
  Xc_TRACE(("No mark set (mark may be bad)"));
  return;
 }
 This->Motion_Flag = TRUE;
 XmTextSetInsertionPosition(This->Editor, This->Mark[1]);
}

/* -------------------------------------------------------------------- **
** -----	Callback of MenuTextUnsetMark1 			  -----	**
** -------------------------------------------------------------------- */

static void	MenuTextUnsetMark1(The_Widget, This)
Widget	       	The_Widget;
c_AEditor	*This;
{
 This->Mark[0] = -1;
}

/* -------------------------------------------------------------------- **
** -----	Callback of MenuTextUnsetMark2 			  -----	**
** -------------------------------------------------------------------- */

static void	MenuTextUnsetMark2(The_Widget, This)
Widget	       	The_Widget;
c_AEditor	*This;
{
 This->Mark[1] = -1;
}

/* -------------------------------------------------------------------- **
** -----	Callback of MenuTextExport			  -----	**
** -------------------------------------------------------------------- */

static void	MenuTextExport(The_Widget, This)
Widget	       	The_Widget;
c_AEditor	*This;
{
 FILE		*file;
 char 		*filename;
 c_Text	*Text;
  
 initialize_export_text(This->Page->BaseStd->text_style_base);
 if (ExportTextSelector == NULL)
 {
  if (CreateExportTextSelector(This->w_parent) == FALSE)
   return;
 }
 else
 {
  ManageExportTextSelector();
 }
 if((filename = F(ExportTextSelector).getFile(ExportTextSelector)) == NULL)
  return;
    
  
 switch(((ids_data_t *)(ExportTextSelector->data))->selected_item->num)
 {
 case IDS_EXPORT_XCL:
  if (strcmp("Filler", This->Text->name) == 0)
   Text = (c_Text *)NEW(c_Text)(GetBasename(filename, FALSE),
				This->Text_Base,
				This->FirstDefaultStyle,
				This->FirstDefaultRuler);
  else
   Text = (c_Text *)NEW(c_Text)(This->Text->name,
				This->Text_Base,
				This->FirstDefaultStyle,
				This->FirstDefaultRuler);
  generate_text(This, Text);
  F(Text).export(Text, This->Page->BaseStd, filename);
  break;
 case IDS_EXPORT_ASCII:
  if (!(file = fopen(filename,"w")))
  {
   Xc_SYSERROR((filename));
   Xc_free(filename);
   return;
  }
  generate_ascii(This, file);
    
  fclose(file);
  break;
 case IDS_EXPORT_HTML:
  if (!(file = fopen(filename,"w")))
  {
   Xc_SYSERROR((filename));
   Xc_free(filename);
   return;
  }
  generate_html(This, file, filename);
    
  fclose(file);
  break;  
 }
  
  
 Xc_free(filename);
}

/* --------------------------------------------------------------------	**
** ---------------------      Editor OK     --------------------------- **
** -------------------------------------------------------------------- */

static void Editor_OK(This)
c_AEditor	*This;
{
 Xc_HISTORY(("Quit Editor... Generating Xcl Text '%s'.", This->Text->name));

 if (strcmp("Filler", This->Text->name) == 0)
  This->Text = (c_Text *)NEW(c_Text)("Filled",
				     This->Text_Base,
				     This->FirstDefaultStyle,
				     This->FirstDefaultRuler);
 else
  This->Text = (c_Text *)NEW(c_Text)(This->Text->name,
				     This->Text_Base,
				     This->FirstDefaultStyle,
				     This->FirstDefaultRuler);
 Xc_TRACE(("Text Creation Ok"));
  
 generate_text(This, This->Text);
  
 F((c_TextFrame *)This->Page->SelectedCadre->cadre->object).attachText
  ((c_TextFrame *)This->Page->SelectedCadre->cadre->object, This->Text);
  
 Xc_HISTORY(("Quit Editor ok Generating Xcl Text"));
}

/* --------------------------------------------------------------------	**
** ---------------------      Editor Cancel     ----------------------- **
** -------------------------------------------------------------------- */

static void Editor_Cancel(This)
c_AEditor	*This;
{
 Xc_HISTORY(("Advanced Editor CANCEL"));
 if (This->empty_before)
  F(This->Page->SelectedCadre->cadre)
   .detachobject(This->Page->SelectedCadre->cadre);
}

/* ----------------------------------------------------------------- ** 
** generate a c_Text object                                          ** 
** ----------------------------------------------------------------- */

static void 	generate_text(This, Text)
c_AEditor	*This;
c_Text		*Text;
{
 XcAE_Escape_List	*List;
 XcAE_Escape_List	*FoundL;
 int			Length;
 XmTextPosition		The_Pos, BestPos;
 boolean		Paragraph;
 
 Length = The_Pos = 0; 
 /* ---- Algo pour regenerer le texte : faire un insert string de ce --- */
 /* ---- qu'il y a entre deux references, inserer la reference, etc -- */ 
 List = This->List;
 while(List->Next != NULL)
 {
  List->Flag = FALSE;
  List = List->Next;
 }
 do
 {
  Paragraph = TRUE;
  FoundL = NULL;
  BestPos = XcAE_MAX_TEXT_LENGTH;
  List = This->List;
  while(List->Next != NULL)
  {
   if (List->Flag == FALSE)
   {
    if ((List->StartPos < BestPos))
    {
     BestPos = List->StartPos;
     FoundL = List;
    }
   }
   List = List->Next;
  }
  if ((FoundL != NULL) && (The_Pos == FoundL->StartPos))
   FoundL->Flag = TRUE;
  
  if ((FoundL == NULL) || (The_Pos != FoundL->StartPos))
  {
   /* cas du texte sans style gere ... */
   if (FoundL == NULL)
    Length = This->Text_Length - The_Pos;
   else
    Length = FoundL->StartPos - The_Pos;
     
   F(Text).insertString(Text, This->W_Text + The_Pos, Length);
   The_Pos += Length;
  }
  else
  {
   Length = FoundL->EndPos - FoundL->StartPos;
   Xc_TRACE(("Insert a reference... length = %ld", Length));
   switch (FoundL->Type)
   {
   case XcAE_RULER :
    if (The_Pos != 0)
     F(Text).insertCode(Text, XcT_PARAGRAPH_CODE);
    F(Text).insertReference(Text, XcT_TEXT_RULER, FoundL->Data.Ruler);
    break;
   case XcAE_STYLE :
    F(Text).insertReference(Text, XcT_TEXT_STYLE, FoundL->Data.Style);
    break;
   case XcAE_SPECIAL:
    switch(FoundL->Data.Special)
    {
    case XcAE_SPEC_CR:
     F(Text).insertCode(Text, XcT_LINE_CODE);
     break;
    case XcAE_SPEC_NC:
     F(Text).insertCode(Text, XcT_PAGE_CODE);
     break;
    case XcAE_SPEC_NP:
     F(Text).insertCode(Text, XcT_PARAGRAPH_CODE);
     Paragraph = FALSE;
     break;
    }
    break;
   default : 
    Xc_WARNING(("Unknown style..."));
    The_Pos -= Length;		/* Pour compenser l'ajout suivant */
   }
   The_Pos += Length;
  }
 }
 while (The_Pos < This->Text_Length);
 if (Paragraph)
  F(Text).insertCode(Text, XcT_PARAGRAPH_CODE);
}



