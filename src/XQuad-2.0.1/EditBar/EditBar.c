/*
** EditBar.c for XQuad in EditBar/
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
** Started on  Mon Jul 17 13:54:22 1995 Robin Castanier
** Last update Mon Aug  9 23:46:34 1999 Emmanuel Paris
*/

#include "EditBar.h"
#include "../Pixmaps/editbar_ok.zpm"
#include "../Pixmaps/editbar_esc.zpm"

void *cons_EditBar();
void dest_EditBar();
void *copy_EditBar();
static void EditBar_query_aff();
static void EditBar_drag_aff();
static void EditBar_refresh_cell();
static void EditBar_set_focus();
static void certify_string();
static void EditBar_SelectMove();
static void EditBar_NewSelect();
static void is_it_formula();
static boolean test_formula();
static void EditBar_scroll_set();
static void EditBar_scroll_aff();
static void EditBar_scroll_reset();
static void EditBar_equal_callback();
static void EditBar_clear_bar();

static void focus_callback();
static void cb_focus();
static void cb_escape();
static void cb_ok();

sf_EditBar fc_EditBar =
{
 cons_EditBar,
 dest_EditBar,
 copy_EditBar,
 EditBar_query_aff,
 EditBar_drag_aff,
 EditBar_refresh_cell,
 EditBar_set_focus,
 EditBar_SelectMove,
 EditBar_NewSelect,
 certify_string,
 EditBar_scroll_set,
 EditBar_scroll_aff,
 EditBar_scroll_reset,
 EditBar_equal_callback,
 EditBar_clear_bar
};


void *cons_EditBar(w_Parent, ManageW, Title)
Widget w_Parent;
c_ManageWidget *ManageW;
char   *Title;
{
 Display	*display;
 c_EditBar	*ObjTmp;
 Arg		args[15];
  
 if ((ObjTmp = (c_EditBar *)Xc_malloc("EditBar", sizeof(c_EditBar))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 ObjTmp->f = &fc_EditBar;
 ObjTmp->edit_mode = FALSE;
 ObjTmp->edited = FALSE;
 ObjTmp->string[0] = '\0';
  
 ObjTmp->Form = (c_Form *)NEW(c_Form)(w_Parent, ManageW, Title);
 display = XtDisplay(w_Parent);

 ObjTmp->Position = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Form),
					    ManageW,
					    "lPositionLabel" );

#if (XmVERSION == 1 && XmREVISION < 2) || defined(LESSTIF_VERSION)
 XtSetArg(args[0], XmNheight, 28);
 XtSetValues(WIDGET(ObjTmp->Position), args, 1);
#endif
  
 ObjTmp->FrameButton = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Form),
					       ManageW, 
					       NULL, 0);
 XtSetArg(args[0], XmNshadowType, XmSHADOW_IN);
 XtSetValues(WIDGET(ObjTmp->FrameButton), args, 1);

 ObjTmp->RowButton =
  (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->FrameButton),
			ManageW, 
			"RowButton", FALSE);

 XtSetArg(args[0], XmNmarginWidth, 0);
 XtSetArg(args[1], XmNmarginHeight, 0);
 XtSetArg(args[2], XmNwidth, 13);
 XtSetArg(args[3], XmNheight, 26);
 XtSetValues(WIDGET(ObjTmp->RowButton), args, 4);

 ObjTmp->Icon_Esc = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->RowButton),
					  ManageW,
					  "EBIconEsc", display,
					  editbar_esc_xpm, NULL, XcPush);
 XtAddCallback(WIDGET(ObjTmp->Icon_Esc),
	       XmNactivateCallback, cb_escape, ObjTmp);
 ObjTmp->Icon_Ok = (c_Icon *)NEW(c_Icon)(WIDGET(ObjTmp->RowButton), 
					 ManageW,
					 "EBIconOk", display,
					 editbar_ok_xpm, NULL, XcPush);
 XtAddCallback(WIDGET(ObjTmp->Icon_Ok),
	       XmNactivateCallback, cb_ok, ObjTmp);
  
 XtSetArg(args[0], XmNtraversalOn, False);
 XtSetArg(args[1], XmNhighlightThickness, 0);
 XtSetArg(args[2], XmNmarginHeight, 0);
 XtSetArg(args[3], XmNmarginWidth, 0);
 XtSetValues(WIDGET(ObjTmp->Icon_Esc), args, 4);
 XtSetValues(WIDGET(ObjTmp->Icon_Ok), args, 4);

 F(ObjTmp->RowButton).addColumn(ObjTmp->RowButton, XcG_MAX_SIZE,
				WIDGET(ObjTmp->Icon_Esc), XcG_MAX_SIZE,
				WIDGET(ObjTmp->Icon_Ok), XcG_MAX_SIZE,
				NULL);

 ObjTmp->Edit = (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Form),
						  ManageW,
						  "tfEditBar",
						  TF_STRING,
						  1.0, 
						  (float)(Xq_FORMULA_LENGTH 
							  - 1),
						  NULL, NULL, 10);
 F(ObjTmp->Edit).add_escape_func(ObjTmp->Edit, cb_focus, ObjTmp);
  
 ObjTmp->mode_sel = MODE_MOVE;
  
 ObjTmp->w_This = WIDGET(ObjTmp->Form);

 XtAddCallback(WIDGET(ObjTmp->Edit), XmNmodifyVerifyCallback, 
	       is_it_formula, ObjTmp);
  
 XtAddEventHandler(WIDGET(ObjTmp->Edit), ButtonPress,
		   False, focus_callback, (void *)ObjTmp);

 F(ObjTmp->Edit).remove_lostfocus_callback(ObjTmp->Edit);

 F(ObjTmp->Form).attach_left(WIDGET(ObjTmp->Form), 
			     WIDGET(ObjTmp->Position), NULL);
 F(ObjTmp->Form).attach_top(WIDGET(ObjTmp->Form), 
			    WIDGET(ObjTmp->Position), NULL);
 F(ObjTmp->Form).attach_bottom(WIDGET(ObjTmp->Form), 
			       WIDGET(ObjTmp->Position), NULL);
  
 F(ObjTmp->Form).attach_top(WIDGET(ObjTmp->Form), 
			    WIDGET(ObjTmp->FrameButton),
			    NULL);
 F(ObjTmp->Form).attach_bottom(WIDGET(ObjTmp->Form), 
			       WIDGET(ObjTmp->FrameButton), NULL);
 F(ObjTmp->Form).attach_left(WIDGET(ObjTmp->Form), 
			     WIDGET(ObjTmp->FrameButton),
			     WIDGET(ObjTmp->Position));

 F(ObjTmp->Form).attach_right(WIDGET(ObjTmp->Form), 
			      WIDGET(ObjTmp->Edit), NULL);
 F(ObjTmp->Form).attach_top(WIDGET(ObjTmp->Form), 
			    WIDGET(ObjTmp->Edit), NULL);
 F(ObjTmp->Form).attach_bottom(WIDGET(ObjTmp->Form), 
			       WIDGET(ObjTmp->Edit), NULL);
 F(ObjTmp->Form).attach_left(WIDGET(ObjTmp->Form), 
			     WIDGET(ObjTmp->Edit), 
			     WIDGET(ObjTmp->FrameButton));
  
 F(ObjTmp).clear_bar(ObjTmp);

 Xc_TRACE(("Object EditBar build"));
 return ObjTmp;
}

void dest_EditBar(This)
c_EditBar *This;
{
 DELETE(c_Icon)(This->Icon_Esc);
 DELETE(c_Icon)(This->Icon_Ok);
 DELETE(c_WTextField)(This->Edit);
 DELETE(c_Label)(This->Position);
 DELETE(c_Grid)(This->RowButton);
 DELETE(c_Frame)(This->FrameButton);
 DELETE(c_Form)(This->Form);
 Xc_TRACE(("Object EditBar destroyed"));
 Xc_free(This);
}

void *copy_EditBar(This)
c_EditBar *This;
{
 Xc_TRACE(("Object EditBar copy not implemented"));
 return NULL;
}

static void EditBar_query_aff(this, column, row)
c_EditBar	*this;
int		column;
int		row;
{
 Arg		argsw[1];
 XmString	xms;
  
 colrowtostr(column, row, this->label_str);
 xms = XmStringCreate(this->label_str, XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[0], XmNlabelString, xms);
 XtSetValues(WIDGET(this->Position), argsw, 1);
 XmStringFree(xms);
}

static void EditBar_drag_aff(this, nb_column, nb_row)
c_EditBar	*this;
int		nb_column;
int		nb_row;
{
 Arg		argsw[1];

 if (!nb_row)
  sprintf(this->label_str, "C%d", nb_column);
 else
  if (!nb_column)
   sprintf(this->label_str, "L%d", nb_row);
  else
   sprintf(this->label_str, "L%d x C%d", nb_row, nb_column);
 XtSetArg(argsw[0], XmNlabelString,
	  XmStringCreate(this->label_str, XmSTRING_DEFAULT_CHARSET));
 XtSetValues(WIDGET(this->Position), argsw, 1);
}

static void EditBar_refresh_cell(this, feuille)
c_EditBar	*this;
c_Feuille	*feuille;
{
 char		fstr[Xq_FORMULA_LENGTH + 1];
 int		iLastPos;

 this->sel_feuille = feuille;
 this->sel_cell = F(feuille).GetActiveCellule(feuille,
					      &(this->sel_x), &(this->sel_y));
 F(feuille->moteur).get_string(feuille->moteur, this->sel_cell, 
			       fstr, Xq_FORMULA);
  
 strcpy(this->string, fstr);
 F(this->Edit).set(this->Edit, fstr);
  
 this->edited = FALSE;
  
 if ((iLastPos = XmTextFieldGetLastPosition(WIDGET(this->Edit))) != 0)
  XmTextFieldSetSelection(WIDGET(this->Edit), 0, iLastPos, CurrentTime);
}

static void EditBar_set_focus(this)
c_EditBar	*this;
{
 XSetInputFocus(XtDisplay(this->w_This), XtWindow(this->w_This), 
		RevertToNone, CurrentTime);
}

static void certify_string(this, Obj)
c_WTextField	*this;
c_EditBar	*Obj;
{
 char		*vstr;

 if (Obj->mode_sel != MODE_MOVE)
 {
  F(Obj->sel_feuille).SetFunction(Obj->sel_feuille, 
				  SELECT_MOVE_RESIZE_CELL);
  Obj->mode_sel = MODE_MOVE;
 }
 vstr = F(this).get(this);
 if (!vstr) return;
 if (!Obj->edited && Xstrcmp(vstr, Obj->string))
 {
  Xc_free(vstr);
 }
 else
 {
  Xc_TRACE(("Set String: %s", vstr));
      
  *(Obj->sel_feuille->modified) = TRUE;
  Xc_free(Obj->sel_cell->text); Obj->sel_cell->text = NULL;
  F(Obj->sel_feuille->moteur).set_string(Obj->sel_feuille->moteur,
					 Obj->sel_x, Obj->sel_y,
					 Obj->sel_cell, vstr,
					 Obj->sel_feuille);
  strcpy(Obj->string, vstr);
  Xc_free(vstr);
  F(Obj->sel_feuille).RefreshCellule(Obj->sel_feuille, 
				     Obj->sel_x,
				     Obj->sel_y);
  Obj->edited = FALSE;
 }
 Obj->edit_mode = FALSE;
}

static void EditBar_NewSelect(this, region, type, ctrl)
c_EditBar	*this;
region_t	*region;
int		type;
boolean		ctrl;
{
 XmTextPosition	left, right;
 char		fstr[Xq_FORMULA_LENGTH + 1];
 char		*str;
 int		index;
 char		aff[8];
 char		*tmpptr;

 switch(type)
 {
 case Xq_CELL:
  if (region->x_min == region->x_max &&
      region->y_min == region->y_max)
  {
   colrowtostr(region->x_min, region->y_min, fstr);
  }
  else
  {
   str = regtostr(region);
   strcpy(fstr, str);
   Xc_free(str);
  }
  break;
 case Xq_ROW:
  sprintf(fstr, "%d:%d", region->y_min+1, region->y_max+1);
  break;
 case Xq_COLUMN:
  coltostr(region->x_min, &index, fstr);
  fstr[index] = ':';
  coltostr(region->x_max, NULL, fstr+index+1);
  break;
 }
 this->select_len = strlen(fstr);
 /*  this->Edit->SetT = 1; */
 if (XmTextFieldGetSelectionPosition
     (WIDGET(this->Edit), &left, &right) == True)
 {
  this->select_s = (int)left;
  XmTextFieldRemove(WIDGET(this->Edit));
 }
 else
  this->select_s = XmTextFieldGetInsertionPosition(WIDGET(this->Edit));
  
 if (this->select_s)
 {
  tmpptr = XmTextFieldGetString(WIDGET(this->Edit));
  if ((tmpptr[this->select_s-1] != '=') &&
      (tmpptr[this->select_s-1] != '+') &&
      (tmpptr[this->select_s-1] != '-') &&
      (tmpptr[this->select_s-1] != '*') &&
      (tmpptr[this->select_s-1] != '/') &&
      (tmpptr[this->select_s-1] != '%') &&
      (tmpptr[this->select_s-1] != '$') &&
      (tmpptr[this->select_s-1] != '<') &&
      (tmpptr[this->select_s-1] != '>') &&
      (tmpptr[this->select_s-1] != ';') &&
      (tmpptr[this->select_s-1] != '('))
  {
   if (ctrl == 1)
    aff[0] = ';';
   else
    aff[0] = '+';
   aff[1] = 0;
   XmTextFieldInsert(WIDGET(this->Edit), this->select_s, aff);
   this->select_s++;
  }
  XtFree(tmpptr);
 }
 XmTextFieldInsert(WIDGET(this->Edit), this->select_s, fstr);
}

static void EditBar_SelectMove(this, region, type)
c_EditBar	*this;
region_t	*region;
int		type;
{
 char		fstr[Xq_FORMULA_LENGTH + 1];
 char		*str;
 int		index;

 switch(type)
 {
 case Xq_CELL:
  if (region->x_min == region->x_max &&
      region->y_min == region->y_max)
  {
   colrowtostr(region->x_min, region->y_min, fstr);
  }
  else
  {
   str = regtostr(region);
   strcpy(fstr, str);
   Xc_free(str);
  }
  break;
 case Xq_ROW:
  sprintf(fstr, "%d:%d", region->y_min+1, region->y_max+1);
  break;
 case Xq_COLUMN:
  coltostr(region->x_min, &index, fstr);
  fstr[index] = ':';
  coltostr(region->x_max, NULL, fstr + index + 1);
  break;
 }
 this->Edit->SetT = 1;
 XmTextFieldReplace(WIDGET(this->Edit), this->select_s,
		    this->select_s+this->select_len, fstr);
 this->select_len = strlen(fstr);
  
 str = F(this->Edit).get(this->Edit);
 F(this->sel_feuille->moteur).simple_set_string
  (this->sel_feuille->moteur, this->sel_x, this->sel_y,
   &this->sel_cell, str, this->sel_feuille);
 Xc_free(str);
  
 F(this->sel_feuille).RefreshCellule(this->sel_feuille, 
				     this->sel_x, this->sel_y);
 this->edited = TRUE;
}

static void is_it_formula(widget, This, cbs)
Widget widget;
c_EditBar *This;
XmTextVerifyCallbackStruct *cbs;
{
 char	*ptr, *ptr2;
 int	len, len2;

 if (This->Edit->SetT || This->Edit->cleanSetT)
  return;
  
 ptr = XmTextFieldGetString(widget);
 len = strlen(ptr);
 ptr2 = cbs->text->ptr;
 len2 = cbs->text->length;
  
 {
  char vstr[Xq_FORMULA_LENGTH], *str;
  XEvent event;
    
  *vstr = '\0';
  if (cbs->startPos < cbs->endPos)
  {
   if (cbs->startPos)
    strncat(vstr, ptr, cbs->startPos);
        
   if (cbs->endPos < (int)strlen(ptr))
    strcat(vstr, ptr + cbs->endPos);
  }
  else
  {
   strcpy(vstr, ptr);
  }
    
  str = len2 ? ptr2 : NULL;
  if (str)
  {
   memmove(vstr + cbs->newInsert + len2, vstr + cbs->newInsert, 
	   len - cbs->newInsert + 1);
   memcpy(vstr + cbs->newInsert, ptr2, len2);
  }
    
  if (XCheckMaskEvent(XtDisplay(widget), 
		      KeyPressMask | ButtonPressMask, &event)	== False)
  {
   F(This->sel_feuille->moteur).simple_set_string(This->sel_feuille->moteur,
						  This->sel_x, This->sel_y,
						  &This->sel_cell, vstr,
						  This->sel_feuille);
   F(This->sel_feuille).RefreshCellule(This->sel_feuille,
				       This->sel_x, This->sel_y);
   This->edited = TRUE;
  }
 }
  
 if (test_formula(This, ptr, len))
 {
  XtFree(ptr);
  return;
 }
 XtFree(ptr);
  
 if (ptr2 == NULL)
  return;
  
 test_formula(This, ptr2, len2);
}

static boolean test_formula(This, ptr, len)
c_EditBar *This;
char *ptr;
int  len;
{
 int len1 = 0;
  
 while (len1 < len && ptr[len1] == ' ')
  len1++;
  
 if (len1 != len)
 {
  if (ptr[len1] == '=')
  {
   if (This->mode_sel != MODE_REGION)
   {
    F(This->sel_feuille).SetFunction(This->sel_feuille, 
				     SELECT_REGION_FOR_EDITBAR);
    This->mode_sel = MODE_REGION;
   }
  }
  else
  {
   if (This->mode_sel != MODE_MOVE)
   {
    F(This->sel_feuille).SetFunction(This->sel_feuille, 
				     SELECT_MOVE_RESIZE_CELL);
    This->mode_sel = MODE_MOVE;
   }
  }
  return TRUE;
 }
 return FALSE;
}

static void EditBar_scroll_set(this)
c_EditBar	*this;
{
 Arg		argsw[2];
 XmString	xms;
 Window	parent;
 int		none;
 long		foreg;
 long		backg;
 XGCValues 	xgcv;
 XCharStruct	chstr;
  
 xms = XmStringCreate("", XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[0], XmNlabelString, xms);
 XtSetValues(WIDGET(this->Position), argsw, 1);
 XmStringFree(xms);
 this->display = XtDisplay(WIDGET(this->Position));
 this->window = XtWindow(WIDGET(this->Position));
  
 XtSetArg(argsw[0], XmNbackground, &backg);
 XtSetArg(argsw[1], XmNforeground, &foreg);
 XtGetValues(WIDGET(this->Position), argsw, 2);
 xgcv.foreground = foreg;
 xgcv.background = backg;
 xgcv.font = F(GlobResources).getFont(GlobResources, XcR_positionFont);
 this->gc_scraff = XCreateGC(this->display, this->window, 
			     GCForeground | GCBackground | GCFont,
			     &xgcv);
 XGetGeometry(this->display, this->window, &parent, &none, &none,
	      &this->Width, &this->Height, &none, &none); 
 this->fnt_struct = XQueryFont(this->display, 
			       XGContextFromGC(this->gc_scraff));
 XTextExtents(this->fnt_struct, "Mg", 2, &none, 
	      &this->font_height, &none, &chstr);
} 

static void EditBar_scroll_aff(this, colrow, col_or_row)
c_EditBar	*this;
int		colrow;
boolean		col_or_row;
{
 char		fstr[30];
 int		lng, width;
  
 if (col_or_row)
 {
  fstr[0] = ' ';
  fstr[1] = ' ';
  coltostr(colrow, &lng, fstr+2);
  fstr[lng+2] = ' ';
  fstr[lng+3] = ' ';
  lng += 4;
 }
 else
 {
  sprintf(fstr, "  %d  ", colrow + 1);
  lng = strlen(fstr);
 }
 width = XTextWidth(this->fnt_struct, fstr, lng);
 XDrawImageString(this->display, this->window, this->gc_scraff, 
		  (this->Width - width) >> 1, 
		  ((this->Height + this->font_height) >> 1) - 2,
		  fstr, lng);
 XFlush(this->display);
}

static void EditBar_scroll_reset(this)
c_EditBar	*this;
{
 Arg		argsw[1];
 XmString	xms;
  
 XFreeGC(XtDisplay(WIDGET(this->Position)), this->gc_scraff);
 xms = XmStringCreate(this->label_str, XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[0], XmNlabelString, xms);
 XtSetValues(WIDGET(this->Position), argsw, 1);
 XmStringFree(xms);
}

static void EditBar_equal_callback(w, This)
Widget	w;     
c_EditBar	*This;
{
 char		str[Xq_FORMULA_LENGTH];
 char		*vstr, *sel;
 int		i;
  
 vstr = F(This->Edit).get(This->Edit);
 i = 0;
 while(vstr[i] == ' ')
  i++;
  
 strcpy(This->string, vstr);
 if (vstr[i] != '=')
 {
  memmove(str + 1, vstr + i, strlen(vstr) - i + 1);
  str[0] = '=';
 }
 else
 {
  memmove(str, vstr + i + 1, strlen(vstr) - i);
 }
 Xc_free(vstr);
 if ((sel = XmTextFieldGetSelection(WIDGET(This->Edit))) != NULL)
  XmTextFieldClearSelection(WIDGET(This->Edit), CurrentTime);
 else
  XtFree(sel);

 XmTextFieldSetInsertionPosition(WIDGET(This->Edit), 0);
 XmTextFieldSetString(WIDGET(This->Edit), str);
 XmTextFieldSetInsertionPosition(WIDGET(This->Edit), strlen(str));
 test_formula(This, str, strlen(str));
 This->edit_mode = TRUE;
}

static void focus_callback(widget, This, event)
Widget		widget;
c_EditBar	*This;
XEvent		*event;
{
 char *vstr;
  
 Xc_TRACE(("focus editbar"));
 This->edit_mode = TRUE;
  
 vstr = F(This->Edit).get(This->Edit);
 test_formula(This, vstr, strlen(vstr));
  
 F(This->sel_feuille->moteur).simple_set_string
  (This->sel_feuille->moteur, This->sel_x, This->sel_y,
   &This->sel_cell, vstr, This->sel_feuille);
 Xc_free(vstr);
  
 F(This->sel_feuille).RefreshCellule(This->sel_feuille, 
				     This->sel_x, This->sel_y);
 This->edited = TRUE;
}

static void cb_focus(wt, This, flag)
c_WTextField	*wt;
c_EditBar	*This;
boolean		flag;
{
 char *vstr;
 This->edit_mode = flag;
  
 vstr = F(This->Edit).get(This->Edit);
 if (flag == TRUE)
  test_formula(This, vstr, strlen(vstr));
  
 F(This->sel_feuille->moteur).simple_set_string
  (This->sel_feuille->moteur, This->sel_x, This->sel_y,
   &This->sel_cell, vstr, This->sel_feuille);
 Xc_free(vstr);
  
 F(This->sel_feuille).RefreshCellule(This->sel_feuille, 
				     This->sel_x, This->sel_y);
 This->edited = TRUE;
}


static void cb_escape(This, Obj)
Widget	This;
c_EditBar *Obj;
{
 char		*vstr, *sel;

 Xc_TRACE(("Escape"));
 if (Obj->mode_sel != MODE_MOVE)
 {
  F(Obj->sel_feuille).SetFunction(Obj->sel_feuille, 
				  SELECT_MOVE_RESIZE_CELL);
  Obj->mode_sel = MODE_MOVE;
 }
 vstr = F(Obj->Edit).get(Obj->Edit);
 if (!vstr) return;
 if (Xstrcmp(vstr, Obj->string))
 {
  if ((sel = XmTextFieldGetSelection(WIDGET(Obj->Edit))) != NULL)
  {
   XmTextFieldClearSelection(WIDGET(Obj->Edit), CurrentTime);
   XmTextFieldSetInsertionPosition
    (WIDGET(Obj->Edit), XmTextFieldGetLastPosition(WIDGET(Obj->Edit)));
   test_formula(Obj, vstr, strlen(vstr));
  }
  else
  {
   XtFree(sel);
   XmTextFieldSetSelection
    (WIDGET(Obj->Edit), 0,
     XmTextFieldGetLastPosition(WIDGET(Obj->Edit)), CurrentTime);
  }
 }
 else
 {
  F(Obj->Edit).set(Obj->Edit, Obj->string);
  XmTextFieldSetSelection(WIDGET(Obj->Edit), 0,
			  XmTextFieldGetLastPosition(WIDGET(Obj->Edit)),
			  CurrentTime);
      
  F(Obj->sel_feuille->moteur).simple_set_string
   (Obj->sel_feuille->moteur, Obj->sel_x, Obj->sel_y,
    &Obj->sel_cell, Obj->string, Obj->sel_feuille);
  F(Obj->sel_feuille).RefreshCellule(Obj->sel_feuille, 
				     Obj->sel_x, Obj->sel_y);
  Obj->edited = TRUE;
 }
 Xc_free(vstr);
 Obj->edit_mode = TRUE;
}

static void cb_ok(This, Obj)
Widget	This;
c_EditBar *Obj;
{
 Xc_TRACE(("OK"));
 certify_string(Obj->Edit, Obj);
}

static void EditBar_clear_bar(this)
c_EditBar	*this;
{
 Arg		argsw[2];
 XmString	xms;
  
 Xc_TRACE(("clean_bar"));
 xms = XmStringCreate(" ", XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[0], XmNlabelString, xms);
 XtSetValues(WIDGET(this->Position), argsw, 1);
 XmStringFree(xms);
  
 this->Edit->SetT = 1;
 F(this->Edit).set(this->Edit, "");
 this->edit_mode = this->edited = FALSE;
 this->mode_sel = MODE_MOVE;
 this->string[0] = '\0';
}











