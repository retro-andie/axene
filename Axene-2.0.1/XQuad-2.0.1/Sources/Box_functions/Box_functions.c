/*
** Box_functions.c for XQuad in Box_functions/
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
** Started on  Fri Aug 11 15:48:33 1995 Robin Castanier
** Last update Tue Aug 10 00:17:53 1999 Emmanuel Paris
*/

#include "Box_functions.h"
#include <Xm/DrawingA.h>

void *cons_Box_functions();
void dest_Box_functions();
void *copy_Box_functions();
static void cb_item_list ___NPROTO((c_ItemList *This,
				    void *data, int reason, ...));
static int sort_alpha ___PROTO((def_func *a, def_func *b));
static boolean funcs_filter ___PROTO((def_func *function));
static void create_string_params ___PROTO((c_Box_functions *This));
static void cb_action ___PROTO((c_CustomDialog	*cb_dialog,
				c_Box_functions	*This,
				int		cb_reason));
static void cb_toggle_type ___PROTO((Widget w, c_Box_functions *This, 
				     XmToggleButtonCallbackStruct *cbs));

static char *pszFuncTypeNames[8] =
{
  "tbBFFuncTypeAll",
  "tbBFFuncTypeNume",
  "tbBFFuncTypeStat",
  "tbBFFuncTypeBusi",
  "tbBFFuncTypeDate",
  "tbBFFuncTypeShee",
  "tbBFFuncTypeText",
  "tbBFFuncTypeLogi",
};

static int FuncTypeNum[8] = {
  Xq_FT_ALL, Xq_FT_NUMERIC, Xq_FT_STATISTICAL, Xq_FT_BUSINESS,
  Xq_FT_DATE_TIME, Xq_FT_SPREADSHEET, Xq_FT_TEXT, Xq_FT_LOGICAL
}; 

static int FuncTypeSel = Xq_FT_ALL;

sf_Box_functions fc_Box_functions =
{
  cons_Box_functions,
  dest_Box_functions,
  copy_Box_functions
};

void *cons_Box_functions(w_Parent, pszTitle, Parser)
Widget		w_Parent;
char		*pszTitle;
c_Parser	*Parser;
{
  c_Box_functions	*ObjTmp;
  int			i;

  if((ObjTmp = (c_Box_functions *)Xc_malloc("Box_functions",
					    sizeof(c_Box_functions))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }

  ObjTmp->f = &fc_Box_functions;
  ObjTmp->Parser = Parser;

  ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent, 
							 pszTitle,
							 450, 410,
							 XcCD_VERTICAL
							 | XcCD_VRESIZE);

  ObjTmp->Ltitre = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Dialog),
					   MANAGER(ObjTmp->Dialog),
					   "lBoxFunctionsTitle");
  
  ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->Dialog),
				       MANAGER(ObjTmp->Dialog),
				       "gBFgrid", FALSE);
  
  ObjTmp->ILfunctions = (c_ItemList *)NEW(c_ItemList)(WIDGET(ObjTmp->Grid),
						      MANAGER(ObjTmp->Dialog),
						      "ilBoxFunction", 250, 0);
  F(ObjTmp->ILfunctions).set(ObjTmp->ILfunctions, XcIL_BROWSE_MODE,
			     TRUE, offsetof(def_func, name),
			     0, (void *)sort_alpha, (void *)funcs_filter);
  F(ObjTmp->ILfunctions).setCallback(ObjTmp->ILfunctions,
				     (XtPointer)cb_item_list,
				     ObjTmp);

  ObjTmp->wSep = XtVaCreateWidget("", xmDrawingAreaWidgetClass,
				  WIDGET(ObjTmp->Dialog), 
				  NULL);
  ObjTmp->tfFunction = 
    (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Dialog),
				      MANAGER(ObjTmp->Dialog),
				      "tfBFPreview",
				      TF_STRING,
				      0.0, 1000.0,
				      "test", NULL, 7);
  
  XtVaSetValues(WIDGET(ObjTmp->tfFunction),
		XmNeditable, False,
		XmNcursorPositionVisible, False,
		XmNtraversalOn, False,
		XmNselectionArrayCount, 0,
		NULL);
    
  F(ObjTmp->ILfunctions).addItems(ObjTmp->ILfunctions, FALSE,
				  XcIL_LINKED_LIST, -1,
				  Parser->func,
				  offsetof(def_func, Next));
  F(ObjTmp->ILfunctions).selectItem(ObjTmp->ILfunctions, 
				    *ObjTmp->ILfunctions->item_base);
  
  ObjTmp->FrameType = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					      MANAGER(ObjTmp->Dialog), 
					      "fBFFuncType", 0);
  ObjTmp->GridType = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->FrameType),
					   MANAGER(ObjTmp->Dialog),
					   "gBFgridType", FALSE);

  
  for (i = 0; i < 8; i++)
    {
      ObjTmp->atbFunctionTypes[i] = 
	(c_ToggleButton * )NEW(c_ToggleButton)(WIDGET(ObjTmp->GridType),
					       MANAGER(ObjTmp->Dialog),
					       pszFuncTypeNames[i]);
      if (((i == 0) && (FuncTypeSel == Xq_FT_ALL)) ||
	  ((FuncTypeSel != Xq_FT_ALL) && 
	   (FuncTypeSel & FuncTypeNum[i]) == FuncTypeNum[i]))
	XmToggleButtonGadgetSetState(WIDGET(ObjTmp->atbFunctionTypes[i]),
				     True, False);
      
      F(ObjTmp->atbFunctionTypes[i]).Add_Callback(ObjTmp->atbFunctionTypes[i], 
						  XmNvalueChangedCallback,
						  cb_toggle_type, ObjTmp);
    }
  
  F(ObjTmp->GridType).
    addColumn(ObjTmp->GridType, XcG_MAX_SIZE,
	      WIDGET(ObjTmp->atbFunctionTypes[0]), XcG_MAX_SIZE,
	      WIDGET(ObjTmp->atbFunctionTypes[1]), XcG_MAX_SIZE,
	      WIDGET(ObjTmp->atbFunctionTypes[2]), XcG_MAX_SIZE,
	      WIDGET(ObjTmp->atbFunctionTypes[3]), XcG_MAX_SIZE,
	      WIDGET(ObjTmp->atbFunctionTypes[4]), XcG_MAX_SIZE,
	      WIDGET(ObjTmp->atbFunctionTypes[5]), XcG_MAX_SIZE,
	      WIDGET(ObjTmp->atbFunctionTypes[6]), XcG_MAX_SIZE,
	      WIDGET(ObjTmp->atbFunctionTypes[7]), XcG_MAX_SIZE,
	      NULL);
  
  

  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 250,
			    WIDGET(ObjTmp->ILfunctions), XcG_MAX_SIZE,
			    NULL);
  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, 7, NULL);
  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->FrameType), XcG_MAX_SIZE,
			    NULL);
  

  F(ObjTmp->Dialog).blockChild((ObjTmp->Dialog), WIDGET(ObjTmp->Ltitre), 30);
  F(ObjTmp->Dialog).blockChild(ObjTmp->Dialog, ObjTmp->wSep, 10);
  F(ObjTmp->Dialog).blockChild((ObjTmp->Dialog), 
			       WIDGET(ObjTmp->tfFunction), 30);
  
  
  F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog,
				     XcCD_OK | XcCD_CANCEL,
				     XcCD_OK,
				     TRUE, (void (*)())cb_action, ObjTmp);
  
  F(ObjTmp->ILfunctions).setState(ObjTmp->ILfunctions, TRUE,
				  XcIL_VA_LIST, 1, Parser->func);
  ObjTmp->edited_item = Parser->func;

  create_string_params(ObjTmp);
  
  F(ObjTmp->tfFunction).set(ObjTmp->tfFunction, ObjTmp->szBaratin);
  
  F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);

  Xc_TRACE(("Object Box_functions build"));
  return ObjTmp;
}

void dest_Box_functions(This)
c_Box_functions *This;
{
  int i;

  for (i = 0; i < 8; i++)
    DELETE(c_ToggleButton)(This->atbFunctionTypes[i]);
  DELETE(c_Grid)(This->GridType);
  DELETE(c_Frame)(This->FrameType);
  DELETE(c_ItemList)(This->ILfunctions);
  DELETE(c_Grid)(This->Grid);
  DELETE(c_WTextField)(This->tfFunction);
  DELETE(c_Label)(This->Ltitre);
  XtDestroyWidget(This->wSep);
  DELETE(c_CustomDialog)(This->Dialog);
  Xc_free(This);
  Xc_TRACE(("Object Box_functions destroyed"));
}

void *copy_Box_functions(This)
c_Box_functions *This;
{
  Xc_TRACE(("Copy not implemented"));
  return NULL;
}

/* ----------------------------------------------------------------- ** 
** cb_item_list - ItemList selection callback                        ** 
** ----------------------------------------------------------------- */
static void cb_item_list(list, This)
c_ItemList	*list;
c_Box_functions	*This;
{
  def_func     	**items;
  int 		count; 
  
  Xc_TRACE(("change item list selection"));
  if(F(list).getSelection(list, (void ***)&items, &count) && count)
    {
      void *item;
      
      item = *items;
      Xc_free(items);

      if (This->edited_item == item) return;
      This->edited_item = item;
      
      create_string_params(This);
      
      F(This->tfFunction).set(This->tfFunction, This->szBaratin);
    }
}



/* ----------------------------------------------------------------- ** 
** sort_alpha - Alpha sort for files                                 ** 
** ----------------------------------------------------------------- */
static int sort_alpha(a, b)
def_func *a;
def_func *b;
{
  return strcmp(a->name, b->name);
}

/* ----------------------------------------------------------------- ** 
** sort_alpha - Alpha sort for files                                 ** 
** ----------------------------------------------------------------- */
static boolean funcs_filter(function)
def_func *function;
{
  return ((function->type & FuncTypeSel) != 0);
}

/* ----------------------------------------------------------------- ** 
** create_string_params - Create string for functions parameters     **
** ----------------------------------------------------------------- */
static void create_string_params(This)
c_Box_functions	*This;
{
  def_func **items;
  int i;
  
  if(F(This->ILfunctions).getSelection(This->ILfunctions,
				       (void ***)&items, &i) && i)
    {
      Xc_TRACE(("item (%s) = %d", items[0]->name, items[0]->nbr_param));
      strcpy(This->szBaratin, XcIL_ITEM_NAME(This->ILfunctions, 
					     This->edited_item));
      if (items[0]->nbr_param != -1)
	{
	  strcat(This->szBaratin, "()");
	}
      Xc_free(items);
    }
}

/* ----------------------------------------------------------------- ** 
** cb_action - CallBack on exit                                      ** 
** ----------------------------------------------------------------- */
static void cb_action(cb_dialog, This, cb_reason)
c_CustomDialog	*cb_dialog;
c_Box_functions	*This;
int		cb_reason;
{
  int insert_pos;
  XmTextPosition left, right;
  def_func **items;
  int i;
  char *string;
  
  switch(cb_reason)
    {
    case XcCD_OK:
      if (F(This->ILfunctions).getSelection(This->ILfunctions,
					(void ***)&items, &i) && i)
	{	  
	  create_string_params(This);
	  if (XmTextFieldGetSelectionPosition(WIDGET(GlobEditBar->Edit),
					 &left, &right) == True)
	    XmTextFieldRemove(WIDGET(GlobEditBar->Edit));
	  insert_pos = 
	    XmTextFieldGetInsertionPosition(WIDGET(GlobEditBar->Edit));

	  string = (char *)Xc_malloc("string", strlen(This->szBaratin) + 2);
	  if (!insert_pos) 
	    {
	      string[0] = '='; 
	      string[1] = '\0';
	    }
	  else 
	    string[0] = '\0';
	  strcat(string, This->szBaratin);
	  XmTextFieldInsert(WIDGET(GlobEditBar->Edit), insert_pos, string);
	  Xc_free(string);
	  
	  if (items[0]->nbr_param != -1)
	    {
	      insert_pos = 
		XmTextFieldGetInsertionPosition(WIDGET(GlobEditBar->Edit));
	      XmTextFieldSetInsertionPosition(WIDGET(GlobEditBar->Edit), 
					      insert_pos -1);
	    }
	  
	  Xc_free(items);
	}
      break;
    default:
      Xc_TRACE(("Cancel Box_functions"));
    }
  F(cb_dialog).unmap(cb_dialog);
}
/* ----------------------------------------------------------------- ** 
** cb_toggle_type- when clicking on on of toggles                    ** 
** ----------------------------------------------------------------- */
static void cb_toggle_type(w, This, cbs)
Widget w;
c_Box_functions *This;
XmToggleButtonCallbackStruct *cbs;
{
  boolean ctrl;
  int	i, but_num;

  ctrl = FALSE;
  if (cbs->event->type == ButtonPress || cbs->event->type == ButtonRelease)
    {
      if ((cbs->event->xbutton.state & ControlMask)!=0)
	ctrl = TRUE;
    }
  if (cbs->event->type == KeyPress || cbs->event->type == KeyRelease)
     {
      if ((cbs->event->xkey.state & ControlMask)!=0)
	ctrl = TRUE;
    }

  i = 0;
  while(i < 8 && WIDGET(This->atbFunctionTypes[i]) != w)
    i++;
  
  if (i >= 8)
    return;
  but_num = i;

  if (ctrl && (i != 0) && (FuncTypeSel != Xq_FT_ALL))
    {
      if (FuncTypeSel == FuncTypeNum[but_num])
	XmToggleButtonGadgetSetState(WIDGET(This->atbFunctionTypes[but_num]),
				     True, False);
      else
	{
	  FuncTypeSel ^= FuncTypeNum[but_num];
	  F(This->ILfunctions).deleteItems(This->ILfunctions, 
					   XcIL_LINKED_LIST, -1,
					   This->Parser->func,
					   offsetof(def_func, Next));
	  F(This->ILfunctions).addItems(This->ILfunctions, FALSE,
					XcIL_LINKED_LIST, -1,
					This->Parser->func,
					offsetof(def_func, Next));
	  F(This->ILfunctions).selectItem(This->ILfunctions, 
					  *This->ILfunctions->item_base);
	  cb_item_list(This->ILfunctions, This);
	}
    }
  else
    {
      if ( FuncTypeSel == Xq_FT_ALL)
	{
	  if (i != 0) 
	    XmToggleButtonGadgetSetState(WIDGET(This->atbFunctionTypes[0]),
					 False, False);
	  else
	    XmToggleButtonGadgetSetState(WIDGET(This->atbFunctionTypes[0]),
					   True, False);
	}
      else
	{
	  for(i = 1; i < 8; i++)
	    {
	      if (FuncTypeSel & FuncTypeNum[i])
		{
		  if (i != but_num) 
		    XmToggleButtonGadgetSetState
		      (WIDGET(This->atbFunctionTypes[i]), False, False);
		  else
		    XmToggleButtonGadgetSetState
		      (WIDGET(This->atbFunctionTypes[i]), True, False);
		}
	    }
	}
      FuncTypeSel =  FuncTypeNum[but_num];
      F(This->ILfunctions).deleteItems(This->ILfunctions, 
				       XcIL_LINKED_LIST, -1,
				       This->Parser->func,
				       offsetof(def_func, Next));
      F(This->ILfunctions).addItems(This->ILfunctions, FALSE,
				    XcIL_LINKED_LIST, -1,
				    This->Parser->func,
				    offsetof(def_func, Next));
      F(This->ILfunctions).selectItem(This->ILfunctions, 
				      *This->ILfunctions->item_base);
      cb_item_list(This->ILfunctions, This);
    }
}







