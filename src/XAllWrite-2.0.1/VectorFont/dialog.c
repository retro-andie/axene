/*
** dialog.c for Xclamation, XQuad and XAllWrite in VectorFont/
** Dialog box to choose a font among available fonts
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
** Started on  Wed Jul 20 16:41:58 1994 Stéphane Boisson
** Last update Wed Feb  4 01:20:12 1998 Emmanuel Paris
*/

/*
  #define NHISTORY
  #define NTRACE
  */

#include "VectorFont.h"
#include "CustomDialog.h"
#include "ItemList.h"
#include "ToggleButton.h"
#include "Resource.h"
#include "MainInterface.h"
#include "Cursor.h"
#include <Xm/Label.h>
#include <Xm/LabelG.h>

extern Widget wMainWindow;

c_VectorFont *vectorFontDialog ___PROTO((c_VectorFont *base, char *fontname));
static int sort_alpha ___PROTO((c_VectorFont *a, c_VectorFont *b));
static void cb_list ___PROTO((c_ItemList *list, c_CustomDialog *dialog,
			      int reason));
static void cb_keep_alias ___PROTO((Widget this, void *none));
static c_VectorFont *find_closest_fontname ___PROTO((c_VectorFont *base,
						     char *fontname));
static int value_strcmp ___PROTO((char *name1, char *name2));

boolean	keepAlias = TRUE;

/* ----------------------------------------------------------------- ** 
** vectorFontDialog - VectorFont selection dialog                    ** 
** ----------------------------------------------------------------- */
c_VectorFont *vectorFontDialog(base, fontname)
c_VectorFont *base;
char *fontname;
{
 c_ManageWidget *manager;
 c_CustomDialog *dialog;
 c_VectorFont *font, *sfont;
 c_ItemList *list;
 c_ToggleButton *tbKeepAlias;
 Widget w_Title;
 Widget w_Font = (Widget)0;
 XmString string;
 boolean wait_style;
  
 Xc_HISTORY(("vectorFontDialog"));

 /*-- Init ---*/
 font = NULL;
 w_Title = (Widget)0;
 list = NULL;
 tbKeepAlias = NULL;

 wait_style = (GlobCursor->wait_style == WAIT_ANIMATE);
 if (wait_style)
 {
  int i;
    
  UNSET_WAIT_MODE_ANIM;
  i = GlobCursor->wait_stack_animate;
  while(i)
  {
   UNSET_WAIT_MODE_ANIM;
   i--;
  }
 }
  
 SET_WAIT_MODE_STATIC;
 
 /*--- CustomDialog ---*/
 dialog = NEW(c_CustomDialog)(wMainWindow, "font_select", 400, 500,
			      XcCD_VRESIZE | XcCD_HRESIZE);
 while(dialog != NULL)
 {
  manager = MANAGER(dialog);

  /*--- Labels ---*/
  w_Title = XtVaCreateWidget("lFontSelect", xmLabelGadgetClass,
			     WIDGET(dialog), NULL);
  F(manager).Add_child(manager, w_Title, WIDGET(dialog));
      
  string = XmStringCreateLtoR(fontname, XmSTRING_DEFAULT_CHARSET);
  w_Font = XtVaCreateWidget("Font Name", xmLabelGadgetClass,
			    WIDGET(dialog), XmNlabelString, string, NULL);
  XmStringFree(string);
  F(manager).Add_child(manager, w_Font, WIDGET(dialog));
      
  /*--- ItemList ---*/
  list = NEW(c_ItemList)(WIDGET(dialog), manager, "FSList", 0, 0);
  if(list == NULL) break;
  F(list).setCallback(list, (void *)cb_list, dialog);
  F(list).set(list, XcIL_BROWSE_MODE,
	      TRUE, offsetof(c_VectorFont, full_name), 0,
	      (void *)sort_alpha, NULL);
  F(list).addItems(list, FALSE, XcIL_LINKED_LIST, -1,
		   base, offsetof(c_VectorFont, next));
  sfont = find_closest_fontname(base, fontname);
  if (sfont)
   F(list).selectItem(list, sfont);
  else
   F(list).selectItem(list, *(list->item_base));
      
  /*--- ToggleButton ---*/
  tbKeepAlias = (c_ToggleButton *)NEW(c_ToggleButton)
   (WIDGET(dialog), manager, "tbKeepAlias", NULL);
      
  if (keepAlias)
   F(tbKeepAlias).Set(tbKeepAlias, True, False);
  else
   F(tbKeepAlias).Set(tbKeepAlias, False, False);
      
  F(tbKeepAlias).Add_Callback(tbKeepAlias, 
			      XmNvalueChangedCallback,
			      cb_keep_alias, NULL);
      
  F(dialog).blockChild(dialog, w_Font, 0);
  F(dialog).blockChild(dialog, w_Title, 0);
  F(dialog).blockChild(dialog, WIDGET(tbKeepAlias), 0);
      
  /*--- Gestion CustomDialog ---*/
  F(dialog).createActionArea(dialog, XcCD_OK|XcCD_CANCEL, XcCD_OK,
			     TRUE, NULL, NULL);
  F(dialog).map(dialog, TRUE);
  if(F(dialog).waitForUnmap(dialog) == XcCD_OK)
  {
   c_VectorFont **selection;
   int count;
	  
   Xc_TRACE(("ok"));
   if(F(list).getSelection(list, (void ***)&selection, &count))
   {
    font = *selection;
    Xc_free(selection);
   }
  }
      
  break;
 }
  
 Xc_TRACE(("destroy"));
 if(w_Title) XtDestroyWidget(w_Title);
 if(w_Font) XtDestroyWidget(w_Font);
 if(tbKeepAlias) DELETE(c_ToggleButton)(tbKeepAlias);
 if(list != NULL) DELETE(c_ItemList)(list);
 if(dialog != NULL) DELETE(c_CustomDialog)(dialog);
  
  
 UNSET_WAIT_MODE_STATIC;
  
 if (wait_style)
  SET_WAIT_MODE_ANIM;

 Xc_TRACE(("font=%s", font? font->font_name : "<none>"));
 if (font && keepAlias)
 {
  font_alias *alias;
    
  alias = (font_alias *)Xc_malloc("falias", sizeof(font_alias));
  alias->next = font->aliases;
  alias->alias_name = Xc_strdup("nalias", fontname);
  font->aliases = alias;
 }
  
 return font;
}


/* ----------------------------------------------------------------- ** 
** cb_list - ItemList callback function                              ** 
** ----------------------------------------------------------------- */
static void cb_list(list, dialog, reason)
c_ItemList *list;
c_CustomDialog *dialog;
int reason;
{
 Xc_TRACE(("cb_list"));

 if(reason == XcIL_DOUBLE_CLICK) 
 {
  F(dialog).unmap(dialog);
  dialog->result = XcCD_OK;
 }
}

/* ----------------------------------------------------------------- ** 
** cb_keep_alias - ToggleButton callback function                    ** 
** ----------------------------------------------------------------- */
static void cb_keep_alias(this, none)
Widget this;
void *none;
{
 keepAlias ^= TRUE;
}

/* ----------------------------------------------------------------- ** 
** sort_alpha - Alpha sort for files                                 ** 
** ----------------------------------------------------------------- */
static int sort_alpha(a, b)
c_VectorFont *a;
c_VectorFont *b;
{
 return strcmp(a->full_name, b->full_name);
}

/* ----------------------------------------------------------------- ** 
** find_closest_fontname - find in base the closest font by its name ** 
** ----------------------------------------------------------------- */
static c_VectorFont *find_closest_fontname(base, fontname)
c_VectorFont *base;
char *fontname;
{
 c_VectorFont *vfont, *best_vfont;
 int	result, best_result;
  
 if (!base) return NULL;
  
 best_vfont = base;
 best_result = value_strcmp(fontname, best_vfont->full_name);
  
 vfont = base->next;
 while(vfont)
 {
  result = value_strcmp(fontname, vfont->full_name);
  if (result > best_result)
  {
   best_result = result;
   best_vfont = vfont;
  }
  vfont = vfont->next;
 }
 if (!best_result)
  return NULL;
 return best_vfont;
}

/* ----------------------------------------------------------------- ** 
** value_strcmp - return the position of the first different char    ** 
** ----------------------------------------------------------------- */
static int value_strcmp(name1, name2)
char *name1;
char *name2;
{
 int pos = 0;
  
 while(*name1 && *name2 && *name1 == *name2)
 {
  name1++;
  name2++;
  pos++;
 }
  
 if (!*name1 || !*name2)
  pos += 100;
  
 if (!*name1 && !*name2)
  pos = 10000;
  
 return pos;
}







