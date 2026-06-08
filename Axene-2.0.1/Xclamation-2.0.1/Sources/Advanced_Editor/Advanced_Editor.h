/*
** Advanced_Editor.h for Xclamation in Advanced_Editor/
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
** Started on  Mon Jul 17 13:53:06 1995 Emmanuel Paris
** Last update Mon Apr 14 14:59:42 1997 Emmanuel Paris
*/

#ifndef _AEditor_
#define _AEditor_

#include "xcalibur.h"
#include "RowColumn.h"
#include "CustomDialog.h"
#include "Form.h"
#include "Icon.h"
#include "Label.h"
#include "ManageWidget.h"
#include "PulldownMenu.h"
#include "MenuBar.h"
#include "Text.h"
#include "ItemMenu.h"
#include "Page.h"
#include "Cursor.h"
#include <Xm/SeparatoG.h>
#include <Xm/Text.h>

/* ----- Defines the maximum count of elements in bottom row-column ----- */

#define XcAE_DEFAULT			1
#define XcAE_SELECTION_REDRAW		1
#define XcAE_NO_SELECTION_REDRAW	2
#define XcAE_NO_MODIFY_VERIFY		4
#define XcAE_NO_MOTION_NOTIFY		8
#define XcAE_INSERT_CONCATENED		16
#define XcAE_INSERT_NON_CONCATENED	32
#define XcAE_NO_ALLOCATION_LIST		128

#define XcAE_NO_REFERENCE	0
#define XcAE_RULER	1
#define XcAE_STYLE	2
#define XcAE_SPECIAL	3
#define XcAE_SPEC_CR	1
#define XcAE_SPEC_NC	2
#define XcAE_SPEC_NP	3

#define XcAE_SRULER_KEYWORD	"[R:]"
#define XcAE_SRULER_KEYWORD_L	4
#define XcAE_RULER_KEYWORD	"\266\n[R:]"
#define XcAE_RULER_KEYWORD_L	6
#define XcAE_RULER_KEYWORD_O	1

#define XcAE_STYLE_KEYWORD	"[S:]"
#define XcAE_STYLE_KEYWORD_L	4
#define XcAE_STYLE_KEYWORD_O	1

#define XcAE_CR_KEYWORD		"[CR]\n"
#define XcAE_CR_KEYWORD_L	5

#define XcAE_NC_KEYWORD		"\n[Frame Break]\n"
#define XcAE_NC_KEYWORD_L	15

#define XcAE_NP_KEYWORD		"\266\n"
#define XcAE_NP_KEYWORD_L	2

#define XcAE_UNDO_MODE		1
#define XcAE_NORMAL_MODE	2

#define XcAE_RULER_NONE_KEYWORD	"None"
#define XcAE_STYLE_NONE_KEYWORD	"None"

#define XcAE_PARAGRAPH_CODE_LENGTH 1

/* ------------ defines the max number of marks settable in text -----------*/
#define XcAE_MAX_MARKS		2

/*------- Comment ... Test ... -------*/

#define XcAE_AF_SIZE	4096
#define XcAE_MAX_TEXT_LENGTH	1000000

/*------- Definitions are over, now define the object structure ------- */

/* ---- defines a few internal structures  ----- */
typedef struct _s_escape_list
{
 struct _s_escape_list	*Next;
 XmTextPosition	StartPos;
 XmTextPosition	EndPos;
 unsigned char		Type;
 union 
 {
  c_TextRuler	*Ruler;
  c_TextStyle	*Style;
  int		Special;
 } Data;
 boolean		Flag;
} XcAE_Escape_List;

typedef struct 
{
 char			*String;
 XmTextPosition	StartPos;  
 XmTextPosition	EndPos;
 XcAE_Escape_List	*List;
} XcAE_Selection;


/* define the method for Class AEditor */
typedef struct
{
 F_STD;
 void		(*Editor_OK)();
 void		(*Editor_Cancel)();
} sf_AEditor;


/* define the AEditor Class */
typedef struct
{
 sf_AEditor		*f;

 /* -------- Beginning of the interface fields ... ---------- */
 Widget		w_This;
 Widget		w_parent;
 c_Cursor		*cursor;
 c_CustomDialog	*Dialog;
 c_Form		*FormOfWin;
 Widget		Editor;		/* pour editer le texte */
 c_PulldownMenu	*Menu1;
 c_PulldownMenu	*Menu2;
 c_MenuBar		*Bar1;	/* Menu deroulant ... */

 c_RowColumn		*Row1;	/* Row pour mettre labels et itemmenus ... */
 c_Label		*RulerLabel;/* Label pour dire 'Rulers :' */
 c_Label		*StyleLabel;/* Label pour dire 'Styles :' */
 c_ItemMenu		*RulerIM;/* ItemMenu pour les regles */
 c_ItemMenu		*StyleIM;/* ItemMenu pour les styles */
 Widget		Sep1;
 Widget		Sep2;
 c_Icon		*IconNewline;
 c_Icon		*IconNextframe;
 c_Icon		*IconStyle;
 c_Icon		*IconRuler;
  

 /* ---------- end of the interface fields  .... ----------- */

 /* ---------- Beginning of the vital fields ... ----------- */
 c_Text		*Text;		/* l'objet texte fourni et a renvoyer */
 boolean		empty_before;
 c_Text		**Text_Base;	/* ptr du 1er texte de la base */
 c_Page		*Page;
 XmTextPosition	Alloc_Free; 
 int			Nbr_Page;
 XmTextPosition	Position_In_Text;
 XmTextPosition	Text_Length;
 XcAE_Escape_List	*List;
 char			*W_Text;/* text in widget ... */
 XcAE_Selection	*Selection;
 XcAE_Selection	*Clip_Buffer;
 boolean		Motion_Flag;
 boolean		Modify_Flag;
 boolean		Set_Text_Flag;
 boolean		is_selection;
 XmTextPosition	old_insert;
 XmTextPosition	Mark[XcAE_MAX_MARKS];
 c_TextRuler		*DefaultRuler;
 c_TextStyle		*DefaultStyle;
  
 c_TextRuler		*FirstDefaultRuler;   
 c_TextStyle		*FirstDefaultStyle;
  
 c_TextRuler		*RulerBase;
 c_TextStyle		*StyleBase;
 /* ------------- End of the vital fields ... ------------- */
} c_AEditor;

extern sf_AEditor fc_AEditor;

#endif










