/*
** HelpWidget.c for XMayday in HelpWidget/
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
** Started on  Mon Jul 17 15:24:07 1995 Robin Castanier
** Last update Fri Jul 25 18:35:11 1997 Emmanuel Paris
*/

#include "HelpWidget.h"
#include "WInterface.h"
#include "RegisterHelp.h"
#include "AlertBox.h"
#include "Resource.h"
#include "file.h"
#include <sys/file.h>

extern c_Resource *GlobResources;
extern c_Colormap *GlobColormap;

#define XcHW_DOCDIR	"doc"

static void *cons_HelpWidget();
static void dest_HelpWidget();
static void *copy_HelpWidget();
static boolean set_url();
static void add_url();
static boolean convert_anchor();
static void reload();
static void refresh();
static void get_scroll_pos();
static void set_scroll_pos();

static void clear_image_list();
static void moveCallback();
static void linkCallback();

static ImageInfo *ImageResolve();

sf_HelpWidget fc_HelpWidget =
{
 cons_HelpWidget,
 dest_HelpWidget,
 copy_HelpWidget,
 set_url,
 add_url,
 convert_anchor,
 reload,
 refresh,
 get_scroll_pos,
 set_scroll_pos
};

static void *cons_HelpWidget(w_Parent, ManageW, Title )
Widget		w_Parent;
c_ManageWidget	*ManageW;
char		*Title;
{
 c_HelpWidget	*This;
 Arg		argsw[4];
 int		i;
  
 if ((This = (c_HelpWidget *)Xc_malloc("HelpWidget",
				       sizeof(c_HelpWidget))) == NULL)
  Xc_FATAL(("Can't build this object: memory allocation error"));
  
 This->f = &fc_HelpWidget;
 This->image_list = NULL;
  
 if (GlobColormap->nb_color > 0 && GlobColormap->type < CMAP_DIRECTCOLOR)
 {
  This->num_colors = GlobColormap->nb_color;
  This->reds   = Xc_malloc("rs", sizeof(int) * GlobColormap->nb_color);
  This->greens = Xc_malloc("gs", sizeof(int) * GlobColormap->nb_color);
  This->blues  = Xc_malloc("bs", sizeof(int) * GlobColormap->nb_color);
  for(i = 0; i < GlobColormap->nb_color; i++)
  {
   This->reds[i]   = (int)GlobColormap->red[i];
   This->greens[i] = (int)GlobColormap->green[i];
   This->blues[i]  = (int)GlobColormap->blue[i];
  }
 }
 else
 {
  This->num_colors = 0;
  This->reds = This->greens = This->blues = NULL;
 }
 This->url_filename = This->url_anchor = This->link_base = NULL;
 This->link_set = FALSE;
  
 XtSetArg(argsw[0], WbNresolveImageFunction, (void *)ImageResolve);
 XtSetArg(argsw[1], WbNextraData, (XPointer)This);
  
 This->w_This = XtCreateWidget(Title, htmlWidgetClass, w_Parent, argsw, 2);
 Xc_REGISTER_HELP(This->w_This);
  
 XtAddCallback(WIDGET(This), WbNpointerMotionCallback,
	       (void *)moveCallback, (caddr_t)This);  
 XtAddCallback(WIDGET(This), WbNlinkCallback,
	       (void *)linkCallback, (caddr_t)This); 
  
 if (ManageW)
  F(ManageW).Add_child(ManageW, This->w_This, w_Parent);
  
 This->anchor_cursor = NULL;
   
 Xc_TRACE(("Object HelpWidget build"));
 return This;
}

static void dest_HelpWidget(This)
c_HelpWidget *This;
{
 clear_image_list(This);
  
 XtDestroyWidget(This->w_This);
  
 if (This->reds) Xc_free(This->reds);
 if (This->greens) Xc_free(This->greens);
 if (This->blues) Xc_free(This->blues);
  
 if (This->anchor_cursor!=NULL)
  DELETE(c_Cursor)(This->anchor_cursor);
  
 if (This->url_filename)
  Xc_free(This->url_filename);
 if (This->url_anchor)
  Xc_free(This->url_anchor);

 Xc_free(This);
 Xc_TRACE(("Object HelpWidget destroyed"));
}

static void *copy_HelpWidget(This)
c_HelpWidget *This;
{
 Xc_ERROR(("Object HelpWidget not copied, not usefull"));
 return NULL;
}

static boolean set_url(This, filename, anchor)
c_HelpWidget *This;
char	*filename;
char	*anchor;
{
 FILE  *file;
 long  taille;
 char	*buffer = NULL;
 boolean valid;
  
 if (This->url_filename == filename || Xstrcmp(This->url_filename, filename))
 {				/* doesn't need to reload the html url */
  if (This->url_anchor != anchor)
  {
   if (This->url_anchor)
    Xc_free(This->url_anchor);
   if (anchor)
    This->url_anchor = (char *)Xc_strdup("url_anchor", anchor);
   else
    This->url_anchor = NULL;
   
   HTMLSetText(WIDGET(This), HTML_NO_RELOAD, NULL, NULL, 0, anchor, NULL);
   return TRUE;
  }
 }

 if ((file = fopen(filename, "rb")) != NULL)
 {
  fseek(file, 0, SEEK_END);
  taille = ftell(file);
  fseek(file, 0, SEEK_SET);
    
  buffer = (char *)Xc_malloc("html", taille + 1);
    
  if ( buffer && fread( (void *)buffer, taille, 1, file) == 1)
  {
   valid = TRUE;
   buffer[taille] = '\0';
  }
  else
  {
   valid = FALSE;
   if (buffer) Xc_free(buffer);
  }
  fclose(file);
 }
 else
 {
  Xc_WARNING(("file %s doesn't exist or unreadable", filename));
  valid = FALSE;
 }
    
 if (!valid)
 {
  buffer = Xc_malloc("htmldef", strlen(filename) + 100);
  sprintf(buffer, "<HR><B>Sorry file: </B><BR>`<I>%s</I>' not found<HR>",
	  filename);
 }
  
 if (This->url_filename != filename)
 {
  if (This->url_filename)
   Xc_free(This->url_filename);
  This->url_filename = (char *)Xc_strdup("url_filename", filename);
 }
  
 if (This->url_anchor != anchor)
 {
  if (This->url_anchor)
   Xc_free(This->url_anchor);
  if (anchor)
   This->url_anchor = (char *)Xc_strdup("url_anchor", anchor);
  else
   This->url_anchor = NULL;
 }
  
 clear_image_list(This);
 if (!This->anchor_cursor)
  This->anchor_cursor = (c_Cursor *)
   NEW(c_Cursor)(XtDisplay(WIDGET(This)), XtWindow(WIDGET(This)));
 F(This->anchor_cursor).set_cursor(This->anchor_cursor, C_NORMAL);
  
 if (valid && anchor)
  HTMLSetText(WIDGET(This), buffer, NULL, NULL, 0, anchor, NULL); 
 else
  HTMLSetText(WIDGET(This), buffer, NULL, NULL, 0, NULL, NULL); 
  
 Xc_free(buffer);
  
 return valid;
}

static void add_url(This, anchor)
c_HelpWidget	*This;
char		*anchor;
{
 HTMLAddHRef(WIDGET(This), anchor);
}

static boolean convert_anchor(This, href, filename, local_anchor)
c_HelpWidget	*This;
char	*href;
char	**filename;
char	**local_anchor;
{
 char	*filen, *anchor;
 char	*directory, *tmp;
  
 filen = anchor = NULL;
 anchor = rindex(href, '#');
  
 if (anchor)
 {
  if (anchor == href)		/* if href begin with # then take This's URL */
   filen = Xc_strdup("filen", This->url_filename);
  else
   filen = Xc_strndup("filen", href, anchor - href);
 }
 else
  filen = Xc_strdup("filen", href);
 
 if (!filen) 
 {
  if (filename) *filename = NULL;
  return FALSE;
 }
  
 if (local_anchor)
 {
  if (anchor)
   *local_anchor = Xc_strdup("lanchor", anchor + 1);
  else
   *local_anchor = NULL;
 }
  
 if (Xstrcmp(filen, This->url_filename))
 {
  if (filename) 
   *filename = filen;
  else
   Xc_free(filen);
  return TRUE;
 }

 if (This->link_set)
 {
  tmp = Xc_malloc("tmp" , strlen(filen) +
		  strlen(This->link_base) + 1);
  strcpy(tmp, This->link_base);
  strcat(tmp, filen);
  if (IsFileRegular(tmp))
  {
   if (filename) 
    *filename = tmp;
   else 
    Xc_free(tmp);
   Xc_free(filen);
   return TRUE;
  }
  else
   Xc_free(tmp);
 }
  
 if (This->url_filename)
 {  
  directory = (char *)GetDirname(This->url_filename, TRUE);
  tmp = Xc_malloc("tmp", strlen(directory) + strlen(filen) + 2);
  sprintf(tmp, "%s/%s", directory, filen);
  Xc_free(directory);
  if (IsFileRegular(tmp))
  {
   if (filename) 
    *filename = tmp;
   else 
    Xc_free(tmp);
   Xc_free(filen);
   return TRUE;
  }
  Xc_free(tmp);
 }
  
 if (IsFileRegular(filen))
 {
  if (filename) 
   *filename = filen;
  else
   Xc_free(filen);
  return TRUE;
 }

 /* here detect a directory as url, try to add index.html and index.htm */
 if ((anchor && anchor != href && *(anchor-1) == '/') ||
     (!anchor && href[strlen(href)-1] == '/'))
 {
  char *href2;

  if (*href == '/')
  {
   href2 = (char *)Xc_malloc("href2", strlen(href) + 10);
   *href2 = '\0';
  }
  else
  {
   directory = (char *)GetDirname(This->url_filename, TRUE);
   href2 = (char *)Xc_malloc("href2", strlen(directory) + strlen(href) + 11);
   sprintf(href2, "%s/", directory);
   Xc_free(directory);
  }

  if (anchor)
  {
   strncat(href2, href, (anchor - href));
   strcat(href2, "index.html");
   strcat(href2, anchor);
  }
  else
  {
   strcat(href2, href);
   strcat(href2, "index.html");
  }

  if (convert_anchor(This, href2, filename, local_anchor))
  {
   Xc_free(filen);
   return TRUE;
  }

  if (*href == '/')
  {
   *href2 = '\0';
  }
  else
  {
   directory = (char *)GetDirname(This->url_filename, TRUE);
   sprintf(href2, "%s/", directory);
   Xc_free(directory);
  }
 
  if (anchor)
  {
   strncat(href2, href, (anchor - href));
   strcat(href2, "index.htm");
   strcat(href2, anchor);
  }
  else
  {
   strcat(href2, href);
   strcat(href2, "index.htm");
  }

  if (convert_anchor(This, href2, filename, local_anchor))
  {
   Xc_free(filen);
   return TRUE;
  }
 }
  
 if (local_anchor)
 {
  if (*local_anchor)
   Xc_free(*local_anchor);
  *local_anchor = NULL;
 }
 if (filename) *filename = NULL;
 Xc_free(filen);
 return FALSE;
}

static void reload(This)
c_HelpWidget *This;
{
 int	vx, vy;
  
 if (!This->anchor_cursor)
  This->anchor_cursor = (c_Cursor *)
   NEW(c_Cursor)(XtDisplay(WIDGET(This)), XtWindow(WIDGET(This)));
 F(This->anchor_cursor).set_cursor(This->anchor_cursor, C_NORMAL);
 if (This->url_filename)
 {
  F(This).get_scroll_pos(This, &vx, &vy);
  set_url(This, This->url_filename, This->url_anchor);
  F(This).set_scroll_pos(This, vx, vy);
 }
 else
 {
  clear_image_list(This);
  HTMLSetText(WIDGET(This), NULL, NULL, NULL, 0, NULL, NULL); 
 }
}

static void refresh(This)
c_HelpWidget *This;
{
 HTMLViewClearAndRefresh(WIDGET(This));
}

static void get_scroll_pos(This, vx, vy)
c_HelpWidget *This;
int *vx;
int *vy;
{
 HTMLGetScrollPos(WIDGET(This), vx, vy);
 Xc_TRACE(("Get Scroll Pos: vx: %d vy: %d", *vx, *vy));
}

static void set_scroll_pos(This, vx, vy)
c_HelpWidget *This;
int vx;
int vy;
{
 HTMLSetScrollPos(WIDGET(This), vx, vy);
 Xc_TRACE(("Set Scroll Pos: vx: %d vy: %d", vx, vy));
}

static ImageInfo *ImageResolve(w, src, noload, swidth, sheight)
Widget w;
char *src;
int noload;
char *swidth; 
char *sheight;
{
 c_HelpWidget	*This;
 char	*filename;
 Pixmap pixmap;
 hw_image_t *l_image;
 c_Image *Image;
 ImageInfo *ImgInfo;
 int	width, height;
 Arg	argsw[1];
  
 Xc_TRACE(("filename: %s noload: %d width: %s height: %s", src, noload,
	   swidth?swidth:"NULL", sheight?sheight:"NULL"));
  
 if (swidth)
  width = atoi(swidth);
 else
  width = 1;
  
 if (sheight)
  height = atoi(sheight);
 else
  height = 1;
  
 if (width <= 0 || height <= 0)
  return (ImageInfo *)NULL;
  
 XtSetArg(argsw[0], WbNextraData, (XPointer *)&This);
 XtGetValues(w, argsw, 1);
  
 /* test if ImageInfo already created */
 l_image = This->image_list;
 while(l_image)
 {
  ImgInfo = l_image->ImageInfo;
  if (ImgInfo->width == width && ImgInfo->height == height 
      && Xstrcmp(ImgInfo->src, src))
   return ImgInfo;
  l_image = l_image->Next;
 }

 convert_anchor(This, src, &filename, NULL);
 Xc_TRACE(("real path filename: %s", filename));
  
 if (!filename)
  return (ImageInfo *)NULL;
  
 Image = (c_Image *)NEW(c_Image)(filename, NULL);
 if (!Image)
 {
  Xc_ErrorAlert(F(GlobResources).getString
		(GlobResources, XcR_notBitmapError));
  Xc_free(filename);
  return (ImageInfo *)NULL;
 }
  
 if (!swidth)
  width = Image->width;
 if (!sheight)
  height = Image->height;
 pixmap = F(Image).CalculImagePixmap(Image, width, height);
 if (pixmap == (Pixmap)0)
 {
  DELETE(c_Image)(Image);
  Xc_free(filename);
  return (ImageInfo *)NULL;
 }
  
 ImgInfo = (ImageInfo *)Xc_malloc("img info", sizeof(ImageInfo));
 memset(ImgInfo, 0, sizeof(ImageInfo));
  
 ImgInfo->transparent = 0;
 ImgInfo->image_data = Image;
 ImgInfo->clip_data = NULL;
 ImgInfo->width = width;
 ImgInfo->height = height;
 ImgInfo->image = (Pixmap)pixmap;
 ImgInfo->clip = (Pixmap)0;
 ImgInfo->text = NULL;
 ImgInfo->src = Xc_strdup("src", src);
 ImgInfo->internal = 0;
 ImgInfo->num_colors = This->num_colors;
 ImgInfo->reds = This->reds;
 ImgInfo->greens = This->greens;
 ImgInfo->blues = This->blues;
  
 l_image = (hw_image_t *)Xc_malloc("limg", sizeof(hw_image_t));
 l_image->Image = Image;
 l_image->ImageInfo = ImgInfo;
 l_image->Next = This->image_list;
 This->image_list = l_image;
  
 Xc_free(filename);
 return ImgInfo;
}

static void clear_image_list(This)
c_HelpWidget *This;
{
 hw_image_t *limg1, *limg2;
  
 limg1 = This->image_list;
 while(limg1)
 {
  limg2 = limg1->Next;
  DELETE(c_Image)(limg1->Image);
  if (limg1->ImageInfo->src) 
   Xc_free(limg1->ImageInfo->src);
  Xc_free(limg1->ImageInfo);
  Xc_free(limg1);
  limg1 = limg2;
 }
 This->image_list = NULL;
}

static void moveCallback(w, This, cbs)
Widget w;
c_HelpWidget *This;
WbpointerMotionCallbackData *cbs;
{
 if (!This->anchor_cursor)
  This->anchor_cursor = (c_Cursor *)
   NEW(c_Cursor)(XtDisplay(WIDGET(This)), XtWindow(WIDGET(This)));
 if (!cbs->href)
  F(This->anchor_cursor).set_cursor(This->anchor_cursor, C_NORMAL);
 else
  F(This->anchor_cursor).set_cursor(This->anchor_cursor, C_ANCHOR);
}

static void linkCallback(w, This, link_info)
Widget w;
c_HelpWidget *This;
LinkInfo *link_info;
{
 Xc_TRACE(("link callback"));
  
 if (!link_info->href || Xstrncmp(link_info->href, "http", 4)) return;
  
 Xc_TRACE(("base HREF: %s", link_info->href));
 Xc_TRACE(("base ROLE: %s", link_info->role));  
  
 if (This->link_base)
  Xc_free(This->link_base);
  
 if (link_info->href[strlen(link_info->href) - 1] != '/')
 {
  This->link_base = Xc_malloc("link base", strlen(link_info->href)+2);
  strcpy(This->link_base, link_info->href);
  strcat(This->link_base, "/");
 }
 else
  This->link_base = Xc_strdup("link base", link_info->href);
 This->link_set = TRUE;
}



















