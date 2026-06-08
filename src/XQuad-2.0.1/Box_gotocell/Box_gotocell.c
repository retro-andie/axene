/*
** Box_gotocell.c for XQuad in Box_gotocell/
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
** Started on  Mon Aug 28 13:15:59 1995 Robin Castanier
** Last update Fri Oct  2 17:07:03 1998 Robin Castanier
*/

#include "Box_gotocell.h"

void *cons_Box_gotocell();
void dest_Box_gotocell();
void *copy_Box_gotocell();
static void certify_cell();
static void error_cell();

sf_Box_gotocell fc_Box_gotocell =
{
 cons_Box_gotocell,
 dest_Box_gotocell,
 copy_Box_gotocell
};


void *cons_Box_gotocell(w_Parent, pszTitle, column, row)
Widget		w_Parent;
char		*pszTitle;
int		*column;
int		*row;
{
 c_Box_gotocell	*ObjTmp;
 char			str[9];
  
 if((ObjTmp = (c_Box_gotocell *)Xc_malloc("Box_gotocell",
					  sizeof(c_Box_gotocell))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }

 ObjTmp->f = &fc_Box_gotocell;
 ObjTmp->column = column;
 ObjTmp->row = row;                         
 ObjTmp->col_orig = *column;
 ObjTmp->row_orig = *row;
 
 ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							pszTitle, 300, 107,
							XcCD_HORIZONTAL);
 ObjTmp->w_This = WIDGET(ObjTmp->Dialog);
  
 ObjTmp->LgotoAt = 
  (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Dialog),
			  MANAGER(ObjTmp->Dialog),
			  "lGCGotoCell");
 colrowtostr(*column, *row, str);
 ObjTmp->WTgotoAt = 
  (c_WTextField *)NEW(c_WTextField)(WIDGET(ObjTmp->Dialog),
				    MANAGER(ObjTmp->Dialog),
				    "tfGCGotoAt",
				    TF_STRING,
				    1.0, 8.0,
				    str, NULL, 8);
  
 F(ObjTmp->WTgotoAt).add_callback(ObjTmp->WTgotoAt,
				  certify_cell, ObjTmp);
  
 F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, 
				    XcCD_OK | XcCD_CANCEL, 
				    XcCD_OK,
				    TRUE, NULL, NULL);
  
 F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);
  
 XmProcessTraversal(WIDGET(ObjTmp->WTgotoAt), XmTRAVERSE_CURRENT);
 XmTextFieldSetInsertionPosition(WIDGET(ObjTmp->WTgotoAt), 
				 (XmTextPosition)strlen(str));
 XmTextFieldSetSelection(WIDGET(ObjTmp->WTgotoAt),
			 0, (XmTextPosition)strlen(str), 0);

 Xc_TRACE(("Object Box_gotocell build"));
 return ObjTmp;
}

void dest_Box_gotocell(This)
c_Box_gotocell *This;
{
 DELETE(c_Label)(This->LgotoAt);
 DELETE(c_WTextField)(This->WTgotoAt);
 DELETE(c_CustomDialog)(This->Dialog);

 Xc_free(This);
 Xc_TRACE(("Object Box_gotocell destroyed"));
}

void *copy_Box_gotocell(This)
c_Box_gotocell *This;
{
 Xc_TRACE(("Copy not implemented"));
 return NULL;
}

#define ERROR_CELL { \
		      Xc_free(vstr); \
		      error_cell(Obj); \
		      return; \
		   }

static void certify_cell(this, Obj)
c_WTextField	*this;
c_Box_gotocell	*Obj;
{
 char		*vstr;
 char		str[9];
 int		num;
 int		i;
 
  
 vstr = F(this).get(this);
 if (!vstr) return;
  
 if (strlen(vstr) == 0) ERROR_CELL;
  
 i = 0;
 while(i < (int)strlen(vstr) && ((vstr[i] >= 'a' && vstr[i] <= 'z') || 
				 (vstr[i] >= 'A' && vstr[i] <= 'Z')) )
  i++;
  
 if (i > 2) ERROR_CELL;
  
 if (i == 0)
 {
  while(i < (int)strlen(vstr) && ( (vstr[i] >= '0' && vstr[i] <= '9')) )
   i++;
  if (i != (int)strlen(vstr)) ERROR_CELL;
  num = atoi(vstr) - 1;
  if (num < 0 || num > ROW_MAX) ERROR_CELL;
  *(Obj->row) = num;
  *(Obj->column) = Obj->col_orig;
 }
 else
 {
  if (i == (int)strlen(vstr))
  {
   if (strtocol(vstr, Obj->column))
   {
    coltostr(*(Obj->column), NULL, str);
    *(Obj->row) = Obj->row_orig;
    if (!Xstrcmp(str, vstr))
     F(this).set(this, str); 
   }
   else
    ERROR_CELL;
  }
  else
  {
   if (strtocolrow(vstr, Obj->column, Obj->row))
   {
    colrowtostr(*(Obj->column), *(Obj->row), str);
    if (!Xstrcmp(str, vstr))
     F(this).set(this, str); 
   }
   else
    ERROR_CELL;
  }
 }
  
 Xc_free(vstr);
 F(Obj->Dialog).resetTraversal(Obj->Dialog);
}

static void error_cell(This)
c_Box_gotocell	*This;
{
 char			str[8];
 
 colrowtostr(*(This->column), *(This->row), str);
 F(This->WTgotoAt).set(This->WTgotoAt, str);
 XmTextFieldSetInsertionPosition(WIDGET(This->WTgotoAt), 
				 (XmTextPosition)strlen(str));
 XmTextFieldSetSelection(WIDGET(This->WTgotoAt),
			 0, (XmTextPosition)strlen(str), 0);
 return;
}
