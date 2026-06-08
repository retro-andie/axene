/*
** ASLBox.c for Xclamation, XQuad and XAllWrite in ASLBox/
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
** Started on  Mon Jul 17 13:50:36 1995 Marc Blet
** Last update Mon Jan 24 23:04:55 2000 Emmanuel Paris
*/

#define NTRACE
#define NHISTORY

#include "ASLBox.h"
#include <X11/Intrinsic.h>
#include <Xm/DrawingA.h>
#include <varargs.h>

static void *constructor();
static void destructor();
static void *copy();
static void map();
static void unmap();
static int  Add_Buttons();
static int  Add_Icons();
static void setCallback ___PROTO((c_ASLBox *This, void (*cb_func)
				  ___PROTO((c_ASLBox *box, void *data,
					    void *item)),
				  void *cb_data));
static void cb_item_list() /* ___PROTO((c_ItemList *This,
			      void *data, int reason, ...)) */;
static void cb_text_field ___PROTO((c_WTextField *text_field,
				    c_ASLBox *This));
static void selectItem ___PROTO((c_ASLBox *This, void *item, boolean cb_flag));
static char *getNewItemName ___PROTO((c_ASLBox *this, void **item));
extern void XcHandleEvent ___PROTO((XEvent *event));

/* --------------------- Declaration of callbacks --------------------- */

static void 	Global_RC_Callback();
static void	Default_Item_Callback();

/* ------------------- Declaration of callbacks over ------------------ */

sf_ASLBox fc_ASLBox =
{
 constructor,
 destructor,
 copy,
 map,
 unmap,
 Add_Buttons,
 Add_Icons,
 setCallback,
 selectItem,
 getNewItemName,
};

/*----------------------------------------------------------------------**
**		Constructor for Avec-Scrolled-List Box			**
**----------------------------------------------------------------------*/

static void *constructor(w_Parent, pszTitle, iWidth, iHeight,
			 name_flag, name_offset)
Widget	w_Parent;
char 	*pszTitle;
int	iWidth;
int     iHeight;
boolean name_flag;
long	name_offset;
{
 Display	*display;
 c_ASLBox	*This;

 Xc_HISTORY(("Constructor"));
  
 display = XtDisplay(w_Parent);
 if((This = (c_ASLBox *)Xc_malloc("ASLBox", sizeof(c_ASLBox))) == NULL)
  return NULL;

 This->f = &fc_ASLBox;
 This->cb_func = NULL;
 This->edited_item = NULL;
  
 This->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent, pszTitle,
						      iWidth, iHeight,
						      XcCD_HORIZONTAL 
						      | XcCD_VRESIZE);
  
 /*--- Form ---*/
 This->Form = (c_Form *)NEW(c_Form)(WIDGET(This->Dialog),
				    MANAGER(This->Dialog),
				    "ASLform");
 
 /*--- Form.Label ---*/
 This->Label = (c_Label *)NEW(c_Label)(WIDGET(This->Form), 
				       MANAGER(This->Dialog), "ASLBoxLabel");
				       
 F(This->Form).attach_top(WIDGET(This->Form), WIDGET(This->Label), NULL);
 F(This->Form).attach_bottom(WIDGET(This->Label), NULL, NULL);
 F(This->Form).attach_left(WIDGET(This->Form), WIDGET(This->Label), NULL);
 F(This->Form).attach_right(WIDGET(This->Form), WIDGET(This->Label), NULL);

 XtVaSetValues(WIDGET(This->Label), XmNresizable, False, NULL);
  
 /*--- Form.ItemList ---*/
 This->ItemList = (c_ItemList *)NEW(c_ItemList)(WIDGET(This->Form),
						MANAGER(This->Dialog),
						"ASLItemL", 0, 0);
 F(This->ItemList).set(This->ItemList, XcIL_EXTENDED_MODE,
		       name_flag, name_offset, 0, NULL, NULL);
 F(This->ItemList).setCallback(This->ItemList, (void *)cb_item_list,
			       This);
 XtVaSetValues(WIDGET(This->ItemList), XmNresizable, False, NULL);
 
 /*--- Form.Editor ---*/
 This->text_field = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->Form),
				    MANAGER(This->Dialog),
				    "ASLeditor",
				    TF_STRING,
				    1.0, (double)XcMAX_NAME_LENGTH - 1,
				    "",
				    NULL,
				    XcMAX_NAME_LENGTH - 1);
 F(This->text_field).add_callback(This->text_field,
				  (void *)cb_text_field, This);
  
 XtVaSetValues(WIDGET(This->text_field), XmNresizable, False, 
	        XmNbottomOffset, 3, NULL);
 
 /*--- Form.Frame ---*/
 This->Frame = (c_Frame *)NEW(c_Frame)(WIDGET(This->Form), 
				       NULL, "~ASLframe");

 F(This->Form).attach_top(WIDGET(This->Frame), NULL, NULL);
 F(This->Form).attach_bottom(WIDGET(This->Form), WIDGET(This->Frame), NULL);
 F(This->Form).attach_left(WIDGET(This->Form), WIDGET(This->Frame), NULL);
 F(This->Form).attach_right(WIDGET(This->Form), WIDGET(This->Frame), NULL);
 
 XtVaSetValues(WIDGET(This->Frame),
	       XmNshadowType, XmSHADOW_IN,
	       XmNshadowThickness, 1, NULL);
  
 This->grid = (c_Grid *)NEW(c_Grid)(WIDGET(This->Frame), NULL, 
				    "gASLcommands", FALSE);
 
 XtVaSetValues(WIDGET(This->grid),
	       XmNmarginWidth, 0, XmNmarginHeight, 0, NULL);
    
 /*--- Attachements ---*/

 F(This->Form).attach_top(WIDGET(This->text_field), NULL, NULL);
 F(This->Form).attach_bottom(WIDGET(This->Form), WIDGET(This->text_field), 
			     NULL);
 F(This->Form).attach_left(WIDGET(This->Form), WIDGET(This->text_field), NULL);
 F(This->Form).attach_right(WIDGET(This->Form), WIDGET(This->text_field), 
			    NULL);

 F(This->Form).attach_bottom(WIDGET(This->Form), WIDGET(This->ItemList),
			     WIDGET(This->text_field));
 F(This->Form).attach_top(WIDGET(This->Form), WIDGET(This->ItemList), 
			  WIDGET(This->Label));
 F(This->Form).attach_left(WIDGET(This->Form), WIDGET(This->ItemList), NULL);
 F(This->Form).attach_right(WIDGET(This->Form), WIDGET(This->ItemList), NULL);
 
  
 WIDGET(This) = WIDGET(This->Dialog);
  
 This->Type 		= XcASL_NOTHING;
 This->Last_Item 	= 0;
  
 This->wSep = XtVaCreateWidget("", xmDrawingAreaWidgetClass,
			       WIDGET(This->Dialog), 
			       NULL);
 F(This->Dialog).blockChild(This->Dialog, This->wSep, 10);
 F(This->Dialog).blockChild(This->Dialog, WIDGET(This->Form), 250);

 Xc_TRACE(("Object ASLBox build"));
 return (This);
}

/*----------------------------------------------------------------------**
**		Destructor for Avec-Scrolled-List Box			**
**----------------------------------------------------------------------*/

static void destructor(This)
c_ASLBox *This;
{
 Xc_HISTORY(("Destructor"));
  
 while (This->Last_Item-- > 0)
 {
  if (This->Type == XcASL_ICON)
   DELETE(c_Icon)(This->Buttons[This->Last_Item].button.The_Icon);
  else
   DELETE(c_PushButton)(This->Buttons[This->Last_Item].button.PushB);
 }

 DELETE(c_Grid)(This->grid);
 DELETE(c_Frame)(This->Frame);
 DELETE(c_WTextField)(This->text_field);
 DELETE(c_ItemList)(This->ItemList);
 DELETE(c_Label)(This->Label);
 DELETE(c_Form)(This->Form);
 XtDestroyWidget(This->wSep);
 DELETE(c_CustomDialog)(This->Dialog);
 Xc_free(This);

 Xc_TRACE(("Object ASLBox destroyed"));
}

/*----------------------------------------------------------------------**
**		Copy method for Avec-Scrolled-List Box			**
**----------------------------------------------------------------------*/

static void *copy(This)
c_ASLBox *This;
{
 c_ASLBox *ObjTmp;

 Xc_HISTORY(("Copy object"));

 if ((ObjTmp = (c_ASLBox *)
      Xc_malloc("ASLBox",
		sizeof(c_ASLBox))) == NULL)
 {
  Xc_FATAL(("Can't copy This object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof(c_ASLBox));

 Xc_TRACE(("Copy object ok"));
 return ObjTmp;
}

/*----------------------------------------------------------------------**
**		Map method for Avec-Scrolled-List Box			**
**----------------------------------------------------------------------*/
static void	map(This, modal, itemtoselect)
c_ASLBox	*This;
boolean		modal;
void		*itemtoselect;
{
 Xc_HISTORY(("map"));

 if(itemtoselect != NULL)
 {
  F(This->ItemList).setState(This->ItemList, TRUE,
			     XcIL_VA_LIST, 1, itemtoselect);
  This->edited_item = itemtoselect;
  F(This->text_field).set(This->text_field,
			  XcIL_ITEM_NAME(This->ItemList, itemtoselect));
  if(This->cb_func != NULL)
   This->cb_func(This, This->cb_data, itemtoselect);
 }
 F(This->Dialog).map(This->Dialog, modal);
}


/*----------------------------------------------------------------------**
**		Unmap method for Avec-Scrolled-List Box			**
**----------------------------------------------------------------------*/
static void	unmap(This)
c_ASLBox	*This;
{
 F(This->Dialog).unmap(This->Dialog);
}

/*----------------------------------------------------------------------**
**		Add Buttons in Row2 (bottom row_column)			**
**----------------------------------------------------------------------*/

static int	Add_Buttons(This, va_alist)
c_ASLBox		*This;
va_dcl
{
 va_list	other_arg;
 char		*TmpStr;

 Xc_HISTORY(("Add buttons..."));
 Xc_TRACE(("Button count = %d", This->Last_Item));

 switch(This->Type)
 {
 case XcASL_NOTHING :
  This->Type = XcASL_PUSHBUTTON;
  Xc_TRACE(("asumming homogeneity of the row-column..."));
  break ;
 case XcASL_ICON :
  Xc_WARNING(("Error: can't add a button with icons !!!"));
  return (-1);
 case XcASL_PUSHBUTTON :
  Xc_TRACE(("There is %d buttons... adding more", This->Last_Item));
  break;
 }

 va_start(other_arg);
 while ((TmpStr = va_arg(other_arg, char *)) != NULL)
 {
  Widget wid;
  Dimension height;
  void (*Cb)();

  Xc_TRACE(("`%s'", TmpStr));
  if (This->Last_Item == XcASL_MAX_COUNT)
  {
   Xc_WARNING(("Item count is more than permit"));
   return (-1);
  }

  This->Buttons[This->Last_Item].button.PushB = (c_PushButton *)NEW
   (c_PushButton)(WIDGET(This->grid), MANAGER(This->Dialog), TmpStr);
  
  wid = WIDGET(This->Buttons[This->Last_Item].button.PushB);
  XtVaGetValues(wid, XmNheight, &height, NULL);
  
  XtVaSetValues(WIDGET(This->Frame), XmNheight, height+2, 
		XmNresizePolicy, XmRESIZE_NONE, NULL);


  if (!(This->Last_Item))
  {
   F(MANAGER(This->Dialog)).Add_child(MANAGER(This->Dialog),
				      WIDGET(This->Frame), WIDGET(This->Form));
   F(MANAGER(This->Dialog)).Add_child(MANAGER(This->Dialog),
				      WIDGET(This->grid), WIDGET(This->Frame));

   F(This->Form).attach_bottom(WIDGET(This->Form), WIDGET(This->text_field),
			       WIDGET(This->Frame));
  }
      
  XtAddCallback(wid, XmNactivateCallback,
		Global_RC_Callback, (void *)This);
      
  F(This->grid).addColumn(This->grid, XcG_MAX_SIZE, wid, 
			  XcG_MAX_SIZE, NULL);

  if ((Cb = va_arg(other_arg, void *)) == NULL)
   Cb = Default_Item_Callback;
  This->Buttons[This->Last_Item].Callback_Function = Cb;
  This->Buttons[This->Last_Item++].Callback_Data =
   va_arg(other_arg, void *);
 }
 va_end(other_arg);
  
  
 Xc_TRACE(("Buttons count = %d",This->Last_Item));
  
 Xc_HISTORY(("Add buttons ok"));
 return (0);
}

/*----------------------------------------------------------------------**
**		Add Icons in Row2 (bottom row_column)			**
**----------------------------------------------------------------------*/

static int	Add_Icons(This, va_alist)
c_ASLBox		*This;
va_dcl
{
 va_list	other_arg;
 char		*TmpStr;
 Display	*display;

 Xc_HISTORY(("Add icons..."));
 Xc_TRACE(("Icon count = %d", This->Last_Item));
  
 switch(This->Type)
 {
 case XcASL_NOTHING :
  This->Type = XcASL_ICON;
  Xc_TRACE(("asumming homogeneity of the row-column..."));
  break ;
 case XcASL_PUSHBUTTON :
  Xc_WARNING(("Error: can't add a icon with buttons !!!"));
  return (-1);
 case XcASL_ICON :
  Xc_TRACE(("There is %d icons... adding more", This->Last_Item));
  break;
 }
  
 display = XtDisplay(WIDGET(This->Dialog));
  
 va_start(other_arg);
 while ((TmpStr = va_arg(other_arg, char *)) != NULL)
 {
  c_Icon	*The_Icon;
  char	**The_Xpm;
  void 	(*Cb)();
  /* ---------- Test if there is more items than permitted ---------- */
      
  if (This->Last_Item == XcASL_MAX_COUNT)
  {
   Xc_WARNING(("Items count is more than permit..."));
   return (-1);
  }
  /* ------- Create the icon. It is managed while constructing ------ */

  The_Xpm = va_arg(other_arg, char **);
      
  Xc_TRACE(("Create Icon"));
  The_Icon = This->Buttons[This->Last_Item].button.The_Icon =
   (c_Icon *)NEW(c_Icon)(WIDGET(This->grid), MANAGER(This->Dialog),
			 TmpStr, display, The_Xpm, NULL, XcPush);
      
  if (!(This->Last_Item))
  {
   F(MANAGER(This->Dialog)).Add_child(MANAGER(This->Dialog),
				      WIDGET(This->Frame), WIDGET(This->Form));
   F(MANAGER(This->Dialog)).Add_child(MANAGER(This->Dialog),
				      WIDGET(This->grid), WIDGET(This->Frame));

   F(This->Form).attach_bottom(WIDGET(This->Form), WIDGET(This->text_field),
			       WIDGET(This->Frame));
  }

  F(MANAGER(This->Dialog)).Add_child(MANAGER(This->Dialog),
				     WIDGET(The_Icon),
				     WIDGET(This->grid));
      
  Xc_TRACE(("AddCallback..."));
  XtAddCallback(WIDGET(The_Icon), XmNactivateCallback,
		Global_RC_Callback, (void *)This);
  Xc_TRACE(("Callback added"));
  if ((Cb = va_arg(other_arg, void *)) == NULL)
   Cb = Default_Item_Callback;
  This->Buttons[This->Last_Item].Callback_Function = Cb;
  This->Buttons[This->Last_Item++].Callback_Data =
   va_arg(other_arg, void *);
 }
 va_end(other_arg);

 Xc_TRACE(("Icons count = %d",This->Last_Item));

 Xc_HISTORY(("Add icons ok"));
 return (0);
}

/*----------------------------------------------------------------------**
**		Callback for the RowColumn object			**
**----------------------------------------------------------------------*/

static void 	Global_RC_Callback(wid, This, cbs)
Widget 			wid;
c_ASLBox		*This;
XmAnyCallbackStruct	*cbs;
{
 if (cbs->reason == XmCR_ACTIVATE)
 {
  int Ii;
  switch (This->Type)
  {
  case XcASL_ICON :
   for (Ii = This->Last_Item - 1; Ii >= 0; Ii--)
    if (WIDGET(This->Buttons[Ii].button.The_Icon) == wid)
    {
     Xc_TRACE(("Icon #%d activated", Ii));
     (This->Buttons[Ii].Callback_Function)
      (This, This->Buttons[Ii].Callback_Data);
     return;
    }
  case XcASL_PUSHBUTTON :
   for (Ii = This->Last_Item - 1; Ii >= 0; Ii--)
    if (WIDGET(This->Buttons[Ii].button.PushB) == wid)
    {
     Xc_TRACE(("Button #%d activated", Ii));
     (This->Buttons[Ii].Callback_Function)
      (This, This->Buttons[Ii].Callback_Data);
     return;
    }
  }
 }
}

/*----------------------------------------------------------------------**
**			Default Callback for Items			**
**----------------------------------------------------------------------*/

static void	Default_Item_Callback(This, Data)
c_ASLBox	*This;
void		*Data;
{
 Xc_TRACE(("Default Item Callback..."));
 return;
}


/* ----------------------------------------------------------------- ** 
** selectItem - Force selection of an item                           ** 
** ----------------------------------------------------------------- */
static void selectItem(This, item, cb_flag)
c_ASLBox *This;
void *item;
boolean cb_flag;
{
 Xc_HISTORY(("selectItem"));

 F(This->ItemList).showItem(This->ItemList, item);
 if(This->edited_item == item) return;
 This->edited_item = item;
 F(This->text_field).set(This->text_field,
			 XcIL_ITEM_NAME(This->ItemList, item));
 if(cb_flag && (This->cb_func != NULL))
  This->cb_func(This, This->cb_data, item);
}

/* ----------------------------------------------------------------- ** 
** getNewItemName - generate a new name	                             ** 
** ----------------------------------------------------------------- */
static char *getNewItemName(this, item)
c_ASLBox *this;
void **item;
{
 void **items;
 int count;
 char	*name = NULL;
  
 Xc_TRACE(("getNewItemName"));

 if(F(this->ItemList).getSelection(this->ItemList, &items, &count) && count)
 {
  void *itemc;
  char *namec;
  int  length, i, j;
      
  itemc = *items;
  Xc_free(items);
      
  namec = XcIL_ITEM_NAME(this->ItemList, itemc);
  length = strlen(namec);
  i = length - 1;
  while(i && namec[i] >= '0' && namec[i] <= '9')
   i--;
  if (i == length || namec[i]!='.')
  {
   if ((name = (char *)Xc_malloc("new name", length + 3)) == NULL)
    return name;
   sprintf(name, "%s.1", namec);
  }
  else
  {
   j = atoi(&(namec[i+1])) + 1;
   if ((name = (char *)Xc_malloc("new name", length + 2)) == NULL)
    return name;
	  
   memcpy(name, namec, sizeof(char)*i);
   name[i] = '\0';
   sprintf(name, "%s.%d", name, j);
  }
      
  if (item)
   *item = itemc;
 }
 return name;
}


/* ----------------------------------------------------------------- ** 
** setCallback - callback for the selection                          ** 
** ----------------------------------------------------------------- */
static void setCallback(This, cb_func, cb_data)
c_ASLBox	*This;
void	(*cb_func) ___PROTO((c_ASLBox *box, void *data, void *item));
void	*cb_data;
{
 Xc_HISTORY(("setCallback"));

 This->cb_func = cb_func;
 This->cb_data = cb_data;
}


/* ----------------------------------------------------------------- ** 
** cb_item_list - ItemList selection callback                        ** 
** ----------------------------------------------------------------- */
static void cb_item_list(list, This)
c_ItemList	*list;
c_ASLBox	*This;
{
 void **items;
 int count;
  
 Xc_TRACE(("change item list selection"));
 if(F(list).getSelection(list, &items, &count) && count)
 {
  void *item;
      
  item = *items;
  Xc_free(items);
  if(This->edited_item == item) return;
  This->edited_item = item;
  F(This->text_field).set(This->text_field,
			  XcIL_ITEM_NAME(This->ItemList, item));
  if(This->cb_func != NULL) This->cb_func(This, This->cb_data, item);
 }
}


/* ----------------------------------------------------------------- ** 
** cb_text_field - TextField callback                                ** 
** ----------------------------------------------------------------- */
static void cb_text_field(text_field, This)
c_WTextField	*text_field;
c_ASLBox	*This;
{
 char *name;
 void **list, *sel_item = NULL;
 int count;
  
 Xc_TRACE(("cb_text_field"));

 if(This->edited_item == NULL) return;
  
 if (F(This->ItemList).getSelection(This->ItemList, &list, &count) && count)
 {
  sel_item = *list;
  Xc_free(list);
 }      
 name = F(text_field).get(text_field);
 strcpy(XcIL_ITEM_NAME(This->ItemList, This->edited_item), name);
 Xc_free(name);
 F(This->ItemList).updateItems(This->ItemList, XcIL_VA_LIST,
			       1, This->edited_item);
 F(This->ItemList).showItem(This->ItemList, (void *)This->edited_item);
 if (sel_item)
  F(This->ItemList).selectItem(This->ItemList, sel_item);
 F(This->Dialog).resetTraversal(This->Dialog);
}

















