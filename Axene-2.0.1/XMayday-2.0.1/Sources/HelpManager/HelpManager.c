/*
** HelpManager.c for XMayday in HelpManager/
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
** Started on  Mon Jul 17 17:45:41 1995 Emmanuel Paris
** Last update Sat Jan 29 17:55:36 2000 Emmanuel Paris
*/

#include "HelpManager.h"
#include "WInterface.h"
#include "Resource.h"

extern c_Resource *GlobResources;

static void *cons_HelpManager();
static void dest_HelpManager();
static void *copy_HelpManager();
static boolean read_cmd_line();
static void open_index();
static void open_help();
static void select_function();

static void add_to_anchor_list();
static void destroy_anchor_list();
static void fill_scroll_pos();
static void anchorCallbackForIndex();
static void anchorCallbackForHelp();
static void moveCallback();
static void menuCallback();
static void calculUrlList();
static void changePointInUrlList();

sf_HelpManager fc_HelpManager =
{
 cons_HelpManager,
 dest_HelpManager,
 copy_HelpManager,
 read_cmd_line,
 open_index,
 open_help,
 select_function
};

static void *cons_HelpManager(index, help, menu, button, url)
c_HelpWidget	*index;
c_HelpWidget	*help;
c_PulldownMenu	*menu;
int		button;
Widget		url;
{
 c_HelpManager	*This;
  
 if ((This = (c_HelpManager *)Xc_malloc
      ("HelpManager", sizeof(c_HelpManager))) == NULL)
  Xc_FATAL(("Can't build this object: memory allocation error"));
 memset(This, 0, sizeof(c_HelpManager));
  
 This->f = &fc_HelpManager;
  
 This->anchor_list = This->anchor_current = NULL;
 This->index_widget = index;
 This->help_widget = help;
 This->menu_widget = menu;
 This->menu_button = button;
 This->url_widget = url;
  
 This->index_exist = This->help_exist = FALSE;
  
 XtAddCallback(WIDGET(index), WbNanchorCallback,
	       (void *)anchorCallbackForIndex, (caddr_t)This); 
 XtAddCallback(WIDGET(help), WbNanchorCallback,
	       (void *)anchorCallbackForHelp, (caddr_t)This); 
  
 XtAddCallback(WIDGET(index), WbNpointerMotionCallback,
	       (void *)moveCallback, (caddr_t)This); 
 XtAddCallback(WIDGET(help), WbNpointerMotionCallback,
	       (void *)moveCallback, (caddr_t)This); 
  
 F(menu).SetDynamicFlag(menu, button, TRUE, TRUE, 
			9, "Meta <Key>%1d", "Alt+%1d", PMt_GUESS, 30);
  
 F(menu).SetDynamicList(menu, button, This->url_list);
 F(menu).AddDynamicCallback(menu, button, menuCallback, This);
  
 Xc_TRACE(("Object HelpManager build"));
 return This;
}

static void dest_HelpManager(This)
c_HelpManager *This;
{
 int i;
  
 for(i = 0; i < HM_URL_LIST_LENGTH; i++)
 {
  if (This->url_list[i])
   Xc_free(This->url_list[i]);
 }
  
 destroy_anchor_list(This, This->anchor_list);
  
 Xc_TRACE(("Object HelpManager destroyed"));
 Xc_free(This);
}

static void *copy_HelpManager(This)
c_HelpManager *This;
{
 Xc_WARNING(("Copy object HelpManager useless"));
 return NULL;
}

static boolean read_cmd_line(This, argc, argv)
c_HelpManager	*This;
int		argc;
char		**argv;
{
 int	i, j;
 Dimension height;
 char	*index;
 char	*help;
 static char *cmd_line[] = { "-index", "-ind", "--index", NULL };
  
 index = help = NULL;
  
 for(i = 1; i < argc && argv[i]; i++)
 {
  j = 0;
  while(cmd_line[j] && !Xstrcmp(argv[i], cmd_line[j]))
   j++;
    
  if (cmd_line[j])
  {
   if (i < argc - 1)
   {
    if (!index)
    {
     index = argv[i + 1];
     i++;
    }
    else
     i += 2;
   }
   else
    break;
  }
  else
  {
   if (!help)
    help = argv[i];
   else
   {
    index = argv[i];
    break;
   }
  }
 }
  
 XtVaGetValues(WIDGET(This->help_widget), XmNheight, &height, NULL);
 XtVaSetValues(WIDGET(This->index_widget), XmNheight, height/5, NULL);
  
 Xc_TRACE(("Index: %s", index ? index : "NULL"));
 Xc_TRACE(("Help:  %s", help ? help : "NULL"));
  
 if (index)
  F(This).open_index(This, index);
 if (help)
  F(This).open_help(This, help);
  
 return This->index_exist;
}

static void open_index(This, index)
c_HelpManager *This;
char	*index;
{
 char	*filename;
 char	*local_anchor;
  
 if (!F(This->index_widget).convert_anchor
     (This->index_widget, index, &filename, &local_anchor))
 {
  Xc_TRACE(("Url: %s not valid", index));
  return;
 }
  
 F(This->index_widget).set_url(This->index_widget, filename, local_anchor);
 if (filename)
  Xc_free(filename);
 if (local_anchor)
  Xc_free(local_anchor);
 This->index_exist = TRUE;
}

static void open_help(This, help)
c_HelpManager *This;
char	*help;
{
 char	*filename;
 char	*local_anchor;
  
 if (!F(This->help_widget).convert_anchor
     (This->help_widget, help, &filename, &local_anchor) && 
     !F(This->index_widget).convert_anchor
     (This->index_widget, help, &filename, &local_anchor))
 {
  Xc_TRACE(("Url: %s not valid", help));
  return;
 }
  
 fill_scroll_pos(This);
 if (This->index_exist)
 {
  F(This->index_widget).add_url(This->index_widget, help);
  F(This->index_widget).reload(This->index_widget);
 }
 F(This->help_widget).add_url(This->help_widget, help);
 F(This->help_widget).set_url(This->help_widget,  filename, local_anchor);
 add_to_anchor_list(This, filename, local_anchor);
 This->help_exist = TRUE;
}

static void select_function(This, function)
c_HelpManager *This;
int function;
{
 switch(function)
 {
 case VIEW_PREVIOUS_ANCHOR:
  Xc_TRACE(("VIEW_PREVIOUS_ANCHOR"));
  if (This->help_exist && This->anchor_current->Prev)
  {
   Xc_TRACE(("Go to %s", This->anchor_current->Prev));
   fill_scroll_pos(This);
   This->anchor_current = This->anchor_current->Prev;
   F(This->help_widget).set_url(This->help_widget,
				This->anchor_current->href,
				This->anchor_current->anchor);
   F(This->help_widget).set_scroll_pos(This->help_widget,
				       This->anchor_current->vx,
				       This->anchor_current->vy);
   changePointInUrlList(This);
  }
  break;
 case VIEW_NEXT_ANCHOR:
  Xc_TRACE(("VIEW_NEXT_ANCHOR"));
  if (This->help_exist && This->anchor_current->Next)
  {
   Xc_TRACE(("Go to %s", This->anchor_current->Next));
   fill_scroll_pos(This);
   This->anchor_current = This->anchor_current->Next;
   F(This->help_widget).set_url(This->help_widget,
				This->anchor_current->href,
				This->anchor_current->anchor);
   F(This->help_widget).set_scroll_pos(This->help_widget,
				       This->anchor_current->vx,
				       This->anchor_current->vy);
   changePointInUrlList(This);
  }
  break;
 case VIEW_HOME_PAGE:
  Xc_TRACE(("VIEW_HOME_PAGE"));
  if (This->anchor_list)
  {
   fill_scroll_pos(This);
   This->anchor_current = This->anchor_list;
   F(This->help_widget).set_url(This->help_widget,
				This->anchor_current->href,
				This->anchor_current->anchor);
   F(This->help_widget).set_scroll_pos(This->help_widget,
				       This->anchor_current->vx,
				       This->anchor_current->vy);
   changePointInUrlList(This);
  }
  else
  {
   if (This->index_exist)
   {
    F(This->help_widget).set_url(This->help_widget,
				 This->anchor_current->href,
				 This->anchor_current->anchor);
    if (This->index_widget->url_anchor)
    {
     add_to_anchor_list(This,
			Xc_strdup("urlf",
				  This->index_widget->url_filename),
			Xc_strdup("urla",
				  This->index_widget->url_anchor));
    }
    else
    {
     add_to_anchor_list(This,
			Xc_strdup("urlf",
				  This->index_widget->url_filename),
			NULL);
    }
    F(This->help_widget).set_scroll_pos(This->help_widget,
					This->anchor_current->vx,
					This->anchor_current->vy);
    This->help_exist = TRUE;
   }
  }
  break;
 case CREATE_INDEX:
  if (This->help_exist)
  {
   if (!This->index_exist || 
       (This->index_exist &&
	!Xstrcmp(This->index_widget->url_filename, 
		 This->help_widget->url_filename)))
    F(This).open_index(This, This->help_widget->url_filename);
  }
  break;
 default:
  Xc_WARNING(("Unknow function: %d", function));
 }
}


static void add_to_anchor_list(This, href, anchor)
c_HelpManager *This;
char	*href;
char	*anchor;
{
 anchor_list_t *anlist, *anlist2;
 char	*title;
  
 title = HTMLGetTitle(WIDGET(This->help_widget));
 if (title)
  title = Xc_strdup("title", title);
  
 anlist = Xc_malloc("anlist", sizeof(anchor_list_t));
 if (anlist == NULL)
 {
  anlist = This->anchor_list;
  if (anlist->href) 
   Xc_free(anlist->href);
  if (anlist->anchor) 
   Xc_free(anlist->anchor);
  anlist->href = href;
  anlist->anchor = anchor;
  anlist->title = title;
  This->anchor_list = anlist->Next;
  if (anlist->Next)
   anlist->Next->Prev = NULL;
  anlist->Next = NULL;
  anlist2 = This->anchor_list;
  while(anlist2->Next)
   anlist2 = anlist2->Next;
  anlist2->Next = anlist;
  anlist->Prev = anlist2;
 }
 else
 {
  anlist->href = href;
  anlist->anchor = anchor;
  anlist->title = title;
  anlist->vx = anlist->vy = 0;
  anlist->Next = NULL;
  anlist2 = This->anchor_list;
  if (anlist2)
  {
   while(anlist2->Next)
    anlist2 = anlist2->Next;
    
   anlist2->Next = anlist;
   anlist->Prev = anlist2;
  }
  else
  {
   anlist->Prev = NULL;
   This->anchor_list = anlist;
  }
 }
 This->anchor_current = anlist;
 calculUrlList(This);
}

static void destroy_anchor_list(This, anchor_list)
c_HelpManager *This;
anchor_list_t *anchor_list;
{
 anchor_list_t *anl1, *anl2;
  
 anl1 = anchor_list;
 if (anl1)
 {
  if (anl1->Prev == NULL)
  {
   This->anchor_list = NULL;
   This->help_exist = FALSE;
  }
  else
  {
   anl1->Prev->Next = NULL;
  }
 }
 else
  return;
  
 do
 {
  anl2 = anl1->Next;
  if (anl1->href)
   Xc_free(anl1->href);
  if (anl1->anchor)
   Xc_free(anl1->anchor);
  if (anl1->title)
   Xc_free(anl1->title);
  Xc_free(anl1);
  anl1 = anl2;
 }
 while(anl1);
}

static void fill_scroll_pos(This)
c_HelpManager *This;
{
 if (This->anchor_current)
 {
  F(This->help_widget).get_scroll_pos(This->help_widget,
				      &(This->anchor_current->vx),
				      &(This->anchor_current->vy));
 }
}

/* ********************************************************************** */
/* **                   HELP MANAGER CALLBACKS                         ** */
/* ********************************************************************** */

static void anchorCallbackForIndex(w, This, cbs)
Widget w;
c_HelpManager *This;
WbAnchorCallbackData *cbs;
{
 char	*filename;
 char	*local_anchor;
  
 Xc_TRACE(("anchor callback"));
 Xc_TRACE(("href: %s", cbs->href));
 Xc_TRACE(("text?: %s", cbs->text));
  
 if (!F(This->index_widget).convert_anchor(This->index_widget, cbs->href,
					   &filename, &local_anchor))
 {
  Xc_TRACE(("Url: %s not valid", cbs->href));
  return;
 }
  
 if (!This->help_exist || 
     (This->help_exist &&
      ((This->help_widget->url_anchor != local_anchor || 
	!Xstrcmp(This->help_widget->url_anchor, local_anchor)) ||
       !Xstrcmp(This->help_widget->url_filename, filename))))
 {
  if (This->anchor_current && This->anchor_current->Next)
   destroy_anchor_list(This, This->anchor_current->Next);
  fill_scroll_pos(This);
  if (This->index_exist)
  {
   F(This->index_widget).add_url(This->index_widget, cbs->href);
   F(This->index_widget).reload(This->index_widget);
  }
  F(This->help_widget).add_url(This->help_widget, cbs->href);
  F(This->help_widget).set_url(This->help_widget, filename, local_anchor);
  add_to_anchor_list(This, filename, local_anchor);
 }
 else
 {
  if (This->anchor_current && This->anchor_current->Next)
   destroy_anchor_list(This, This->anchor_current->Next);
  fill_scroll_pos(This); 
  if (This->index_exist)
  {
   F(This->index_widget).add_url(This->index_widget, cbs->href);
   F(This->index_widget).reload(This->index_widget);
  }
  F(This->help_widget).add_url(This->help_widget, cbs->href);
  F(This->help_widget).set_url(This->help_widget, filename, local_anchor);
  if (local_anchor)
   Xc_free(local_anchor);
  Xc_free(filename);
 }
 This->help_exist = TRUE;    
}

static void anchorCallbackForHelp(w, This, cbs)
Widget w;
c_HelpManager *This;
WbAnchorCallbackData *cbs;
{
 char	*filename;
 char	*local_anchor;
  
 Xc_TRACE(("anchor callback"));
 Xc_TRACE(("href: %s", cbs->href));
 Xc_TRACE(("text?: %s", cbs->text));
  
 if (!F(This->help_widget).convert_anchor
     (This->help_widget, cbs->href, &filename, &local_anchor) && 
     !F(This->index_widget).convert_anchor
     (This->index_widget, cbs->href, &filename, &local_anchor))
 {
  Xc_TRACE(("Url: %s not valid", cbs->href));
  return;
 }
  
 if (This->anchor_current->Next)
  destroy_anchor_list(This, This->anchor_current->Next);
 fill_scroll_pos(This);
 if (This->index_exist)
 {
  F(This->index_widget).add_url(This->index_widget, cbs->href);
  F(This->index_widget).reload(This->index_widget);
 }
 {     /* clear URL in bottom bar */
  Arg      	args[1];
  XmString	xms;
  
  xms = XmStringCreate("", XmSTRING_DEFAULT_CHARSET);
  XtSetArg(args[0], XmNlabelString, xms);
  XtSetValues(This->url_widget, args, 1);
  XmStringFree(xms);
 }
 F(This->help_widget).add_url(This->help_widget, cbs->href);
 F(This->help_widget).set_url(This->help_widget, filename, local_anchor);
 add_to_anchor_list(This, filename, local_anchor);
}

static void moveCallback(w, This, cbs)
Widget w;
c_HelpManager *This;
WbpointerMotionCallbackData *cbs;
{
 Arg      	args[1];
 XmString	xms;
  
 if (!cbs->href)
 {
  Xc_TRACE(("move callback out"));
  xms =  XmStringCreate("", XmSTRING_DEFAULT_CHARSET);
 }
 else
 {
  Xc_TRACE(("move callback in %s", cbs->href));
  xms = XmStringCreate(cbs->href, XmSTRING_DEFAULT_CHARSET);
 }
  
 XtSetArg(args[0], XmNlabelString, xms);
 XtSetValues(This->url_widget, args, 1);
 XmStringFree(xms);
}

static void menuCallback(This, w, button)
c_HelpManager *This;
Widget w;
int button;
{
 int	i;
 anchor_list_t	*anl;
  
 Xc_TRACE(("Callback on button: %d", button));
 F(This->menu_widget).ToggleDynamicButton(This->menu_widget, 
					  button + This->menu_button, True);
  
 fill_scroll_pos(This);
  
 if (This->anchor_current)
  anl = This->anchor_current;
 else
  anl = This->anchor_list;
  
 while(anl->Next)
  anl = anl->Next;
  
 for(i = 0; i < button; i++)
 {
  if (!anl->Prev) break;
  anl = anl->Prev;
 }
 This->anchor_current = anl;
 F(This->help_widget).set_url(This->help_widget,
			      This->anchor_current->href,
			      This->anchor_current->anchor);
 F(This->help_widget).set_scroll_pos(This->help_widget,
				     This->anchor_current->vx,
				     This->anchor_current->vy);
}

static void calculUrlList(This)
c_HelpManager *This;
{
 int	i, button;
 char	*title;
 anchor_list_t	*anl;
  
 for(i = 0; i < HM_URL_LIST_LENGTH; i++)
 {
  if (This->url_list[i])
   Xc_free(This->url_list[i]);
 }
 memset(This->url_list, 0, HM_URL_LIST_LENGTH * sizeof(char *));
  
  
 anl = This->anchor_list;
 if (!anl)
  return;
  
 while(anl->Next)
  anl = anl->Next;
  
 i = 0; button = -1;
  
 while(i < HM_URL_LIST_LENGTH && anl)
 {
  if (anl == This->anchor_current)
   button = i;
    
  title = anl->title;
  if (!title)
  {
   title = anl->anchor;
   if (!title)
    title = anl->href;
  }
    
  This->url_list[i] = Xc_strdup("ul", title);
  anl = anl->Prev; i++;
 }
  
 F(This->menu_widget).SetDynamicList(This->menu_widget, This->menu_button,
				     This->url_list);
 if (button != -1)
  F(This->menu_widget).ToggleDynamicButton(This->menu_widget, 
					   button + This->menu_button, True);
}


static void changePointInUrlList(This)
c_HelpManager *This;
{
 int i;
 anchor_list_t *anl;
  
 anl = This->anchor_current;
 if (!anl)
  return;
  
 i = 0;
 while(anl->Next)
 {
  i++;
  anl = anl->Next;
 }
  
 if (i < HM_URL_LIST_LENGTH)
  F(This->menu_widget).ToggleDynamicButton(This->menu_widget, 
					   i + This->menu_button, True);
 else
  F(This->menu_widget).ToggleDynamicButton(This->menu_widget, 
					   -This->menu_button, False);
    
}








