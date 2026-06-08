/*
** FileSelector.c for Xclamation, XQuad, XAllWrite and XMayday in FileSelector/
** Methods for the FileSelector class
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
** Started on  Fri Dec  9 15:59:15 1994 Stéphane Boisson
** Last update Tue Oct  6 19:03:58 1998 Antoine Buat
*/

/*#define NHISTORY*/
#define NTRACE

#include "FileSelector.h"
#include <X11/Intrinsic.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include "fnmatch.h"
#include <pwd.h>

/*--- Constants ---*/
#define XcFS_TIMOUT_INTERVAL (unsigned long)8000

/*--- Macros ---*/
#define XcFS_END_UPDATE_PROC(this) XtRemoveTimeOut((this)->timout)
#define XcFS_BEGIN_UPDATE_PROC(this) \
(this)->timout = XtAppAddTimeOut \
(XtWidgetToApplicationContext(WIDGET((this)->dialog)), \
 XcFS_TIMOUT_INTERVAL, (void *)cb_update, (this))


/*--- Prototypes ---*/
/* extern char *getenv ___PROTO((char *name)); */
static void *constructor();
static void destructor();
static void *copy();

static boolean readDirectory ___PROTO((c_FileSelector *this));
static char *getFile ___PROTO((c_FileSelector *this));
static char *getPath ___PROTO((c_FileSelector *this));
static boolean pushFolder ___PROTO((c_FileSelector *this, fs_file_t *folder));
static boolean addPattern ___PROTO((c_FileSelector *this, boolean clear_flag,
				    char *pattern));
static void cb_list ___PROTO((c_ItemList *list,
			      c_FileSelector *this,
			      int reason));
static void cb_popup ___PROTO((c_ItemMenu *popup,
			       c_FileSelector *this,
			       fs_file_t *file));
static void cb_textfield ___PROTO((c_WTextField *field, c_FileSelector *this));

static boolean setPath ___PROTO((c_FileSelector *this, char *path));
static int sort_alpha ___PROTO((fs_file_t *a, fs_file_t *b));
static int menu_sort ___PROTO((fs_file_t *a, fs_file_t *b));
___INLINE static boolean is_interresting ___PROTO((c_FileSelector *this,
						   struct dirent *dir_entry));

___INLINE static boolean is_really_interresting ___PROTO((c_FileSelector *this,
							  fs_file_t *file));
static void cb_update ___PROTO((c_FileSelector *this));

sf_FileSelector fc_FileSelector =
{
 constructor, destructor, copy,
 getFile,
 readDirectory,
 setPath, getPath,
 addPattern
};


/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(parent, title, w, h)
Widget parent;
char *title;
int w;
int h;
{
 c_ManageWidget *manager;
 c_FileSelector *this;
 fs_file_t root;

 Xc_HISTORY(("constructor"));
 if((this = Xc_malloc("FileSelector", sizeof(c_FileSelector))) == NULL)
  return NULL;
 memset(this, 0, sizeof(c_FileSelector));
 this->f = &fc_FileSelector;
 this->first_time = FALSE;
 *(this->old_selection) = '\0';

 this->patterns[0] = NULL;
  
 this->show_invisibles = FALSE;
 this->show_backups = FALSE;
 this->show_all = FALSE;
 this->show_specials = FALSE;
 this->force_extension = TRUE;
  
 this->first_time = TRUE;
 this->path = this->files = NULL;
 this->top_item_position = 1;

 do
 {
  this->dialog = NEW(c_CustomDialog)(parent, title, w, h,
				     XcCD_HRESIZE|XcCD_VRESIZE|
				     XcCD_HORIZONTAL);
  if(this->dialog == NULL) break;
  manager = MANAGER(this->dialog);
  WIDGET(this) = WIDGET(this->dialog);

  /*--- Form ---*/
  this->form = XtVaCreateWidget("FSForm", xmFormWidgetClass,
				WIDGET(this->dialog), NULL);
  if(this->form == NULL) break;
  F(manager).Add_child(manager, this->form, WIDGET(this));
    
  /*--- Label ---*/
  this->w_Title = XtVaCreateWidget("FSTitle", xmLabelGadgetClass,
				   this->form, NULL);
  F(manager).Add_child(manager, this->w_Title, this->form);

  /*--- ItemMenu ---*/
  this->popup = NEW(c_ItemMenu)(this->form, manager, "imDirectory", 0, 0);
  if(this->popup == NULL) break;
  F(this->popup).setCallback(this->popup, (void *)cb_popup, this);
  F(this->popup).set(this->popup, TRUE, offsetof(fs_file_t, name),
		     (void *)menu_sort, NULL);

  /*--- ItemList ---*/
  this->list = NEW(c_ItemList)(this->form, manager, "FSList", 0, 0);
  if(this->list == NULL) break;
  F(this->list).setCallback(this->list, (void *)cb_list, this);
  F(this->list).set(this->list, XcIL_BROWSE_MODE,
		    TRUE, offsetof(fs_file_t, name), 0,
		    (void *)sort_alpha, NULL);

  /*--- TextField ---*/
  this->text_field = NEW(c_WTextField)(this->form, manager, "FSEditor",
				       TF_STRING, 1.0, 255.0,
				       "", NULL, 255);
  if(this->text_field == NULL) break;
  F(this->text_field).add_callback(this->text_field,
				   (void *)cb_textfield, this);


  /*--- Attachements ---*/
  XtVaSetValues(WIDGET(this->text_field),
		XmNtopAttachment, XmATTACH_NONE,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNleftAttachment,  XmATTACH_FORM,
		XmNrightAttachment,  XmATTACH_FORM,
		XmNresizable, False,
		NULL);
  XtVaSetValues(this->w_Title,
		XmNtopAttachment, XmATTACH_FORM,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftAttachment,  XmATTACH_FORM,
		XmNrightAttachment,  XmATTACH_FORM,
		XmNresizable, False,
		NULL);
  XtVaSetValues(WIDGET(this->popup),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, this->w_Title,
		XmNbottomAttachment, XmATTACH_NONE,
		XmNleftAttachment,  XmATTACH_FORM,
		XmNrightAttachment,  XmATTACH_FORM,
		XmNresizable, False,
		NULL);
  XtVaSetValues(WIDGET(this->list),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, WIDGET(this->popup),
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, WIDGET(this->text_field),
		XmNleftAttachment,  XmATTACH_FORM,
		XmNrightAttachment,  XmATTACH_FORM,
		XmNresizable, False,
		NULL);

  if((this->item_h = Xc_createItem("file base", 50, sizeof(fs_file_t))) == NULL)
   break;

  root.name = "/";
  root.type = XcFS_DIR;
  pushFolder(this, &root);

  F(this).setPath(this, NULL);

  XmProcessTraversal(WIDGET(this->text_field), XmTRAVERSE_CURRENT);
    
  Xc_TRACE(("constructor ok"));
  return this;
 } while(0);
 DELETE(c_FileSelector)(this);
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** Destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_FileSelector *this;
{
 char **ptr;

 Xc_HISTORY(("destructor"));

 if(this->text_field != NULL) DELETE(c_WTextField)(this->text_field);
 if(this->list != NULL) DELETE(c_ItemList)(this->list);
 if(this->popup != NULL) DELETE(c_ItemMenu)(this->popup);
 if(this->form) XtDestroyWidget(this->form);
 if(this->item_h != NULL)
 {
  fs_file_t *file;

  for(file = this->files; file != NULL; file = file->next)
   Xc_free(file->name);
  for(file = this->path; file != NULL; file = file->next)
   Xc_free(file->name);
  Xc_destroyItem(this->item_h);
 }

 for(ptr = this->patterns; *ptr != NULL; ptr++) Xc_free(*ptr);

 DELETE(c_CustomDialog)(this->dialog);
 Xc_free(this);
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this)
c_FileSelector *this;
{
 Xc_HISTORY(("copy non implemented"));
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** is_interresting - Test if file is interresting                    ** 
** ----------------------------------------------------------------- */
___INLINE static boolean is_interresting(this, dir_entry)
c_FileSelector *this;
struct dirent *dir_entry;
{
 register char *filename, c;

 Xc_TRACE(("is_interresting(`%s')", dir_entry->d_name));

 filename = dir_entry->d_name;
 if((c = *filename) == 0) return FALSE;

 if(c == '.')
 {
  if(this->show_invisibles == FALSE) return FALSE;
  if((c = filename[1]) == 0)
   return FALSE;
  else if((c == '.') && (filename[2] == 0))
   return FALSE;
      
  if(this->show_backups != FALSE) return TRUE;

  return (filename[NLENGTH(dir_entry) - 1] == '~')? FALSE : TRUE;
 }
  
 if(this->show_backups != FALSE) return TRUE;
 return (filename[NLENGTH(dir_entry) - 1] == '~')? FALSE : TRUE;
}


/* ----------------------------------------------------------------- ** 
** is_really_interresting - Test if file is really interresting      ** 
** ----------------------------------------------------------------- */
___INLINE static boolean is_really_interresting(this, file)
c_FileSelector *this;
fs_file_t *file;
{
 char **ptr, *filename;

 Xc_TRACE(("is_really_interresting(`%s')", file->name));

 if(file->type != XcFS_NORMAL)
 {
  if(!this->show_specials && (file->type != XcFS_DIR)) return FALSE;
  return TRUE;
 }

 if(this->show_all || (this->patterns[0] == NULL)) return TRUE;
 filename = file->name;
 for(ptr = this->patterns; *ptr != NULL; ptr++)
 {
  if(fnmatch(*ptr, filename, FNM_PERIOD) == 0)
   return TRUE;
 }
  
 return FALSE;
}


/* ----------------------------------------------------------------- ** 
** getFile - Get a file from the file selector                       ** 
** ----------------------------------------------------------------- */
static char *getFile(this)
c_FileSelector *this;
{
 int result;
 Arg args[1];

 Xc_HISTORY(("getFile"));

 if(this->first_time == TRUE)
 {
  F(this->dialog).createActionArea(this->dialog,
				   XcCD_OK|XcCD_CANCEL, XcCD_OK,
				   FALSE, NULL, NULL);
  this->first_time = FALSE;
 }

 readDirectory(this);
 
 F(this->dialog).map(this->dialog, TRUE);
 XmProcessTraversal(WIDGET(this->text_field), XmTRAVERSE_CURRENT);

 XtSetArg(args[0], XmNtopItemPosition, this->top_item_position);
 XtSetValues(LIST(this->list), args, 1);

 XcFS_BEGIN_UPDATE_PROC(this);
 result = F(this->dialog).waitForUnmap(this->dialog);
 XcFS_END_UPDATE_PROC(this);

 if(result == XcCD_OK)
 {
  char *path, *file, *new;
  long len;

  XtSetArg(args[0], XmNtopItemPosition, &this->top_item_position);
  XtGetValues(LIST(this->list), args, 1);
  
  if((path = F(this).getPath(this)) != NULL)
  {
   if((file = F(this->text_field).get(this->text_field)) != NULL)
   {
    len = strlen(path) + strlen(file) + 1;
    if((new = Xc_realloc("filename", path, len)) == NULL)
    {
     Xc_free(path);
     Xc_free(file);
     return NULL;
    }
    strcat(new, file);
    Xc_free(file);
    Xc_TRACE(("selection: `%s'", new));
	      
    return new;
   }
   Xc_free(path);
  }
 }
 Xc_TRACE(("no selection"));
 return NULL;
}  


/* ----------------------------------------------------------------- ** 
** setPath - Set the file selector path                              ** 
** ----------------------------------------------------------------- */
static boolean setPath(this, path)
c_FileSelector *this;
char *path;
{
 char *ptr, *base;
 boolean cwd_flag;

 Xc_HISTORY(("setPath(%s)", path? path:"<current>"));

 cwd_flag = FALSE;
 if(path == NULL)
 {
  if((path = GetWorkingDirectory()) == NULL) return FALSE;
  Xc_TRACE(("current path = `%s'", path));
  cwd_flag = TRUE;
 }

 if((*path == '/') || (*path == '~'))
 { 
  fs_file_t *file;
  int count;
      
  for(count = 0, file = this->path; file->next != NULL; file = file->next)
   count++;
  if(count > 0)
  {
   F(this->popup).deleteItems(this->popup, XcIL_LINKED_LIST, count,
			      this->path, offsetof(fs_file_t, next));
   for(file = this->path; count--; )
   {
    fs_file_t *tmp;
	      
    tmp = file;
    file = file->next;
    Xc_free(tmp->name);
    Xc_delete(this->item_h, tmp);
   }
   this->path = file;
  }

  if(*path == '/')
  {
   base = Xc_strdup("path", path + 1);
   if(cwd_flag) Xc_free(path);
   if(base == NULL) return FALSE;
  }
  else
  {
   char *home;
   long len;

   for(ptr = path; path[1] && (path[1] != '/'); path++);
   home = NULL;
   if((ptr != path) &&
      ((ptr = Xc_strndup("username", ptr + 1, path - ptr)) != NULL))
   {
    struct passwd *infos;
	      
    Xc_TRACE(("user `%s' homedir", ptr));
    if((infos = getpwnam(ptr)) != NULL) home = infos->pw_dir;
    Xc_free(ptr);
   }

   if((home == NULL) && (home = getenv("HOME")) == NULL)
    home = "";

   len = strlen(home) + strlen(path);
   base = Xc_malloc("path", len);
   if(cwd_flag) Xc_free(path);
   if(base == NULL) return FALSE;
   strcpy(base, home);
   strcat(base, path + 1);
  }
 }
 else
 {
  base = Xc_strdup("path", path);
  if(cwd_flag) Xc_free(path);
  if(base == NULL) return FALSE;
 }

 path = base;
 while(*path)
 {
  char *dirname;
  boolean rc;
  fs_file_t dir;

  while(*path == '/') path++;
  for(ptr = path; *ptr && (*ptr != '/'); ptr++);
  if(*ptr) *ptr++ = 0;
      
  if((dirname = Xc_malloc("dirname", ptr - path + 2)) == NULL)
  {
   Xc_free(base);
   if(this->dialog->mapped_flag) F(this).readDirectory(this);
   return FALSE;
  }

  sprintf(dirname, "%s/", path);
  dir.name = dirname;
  dir.type = XcFS_DIR;

  rc = pushFolder(this, &dir);
  Xc_free(dirname);
  if(rc == FALSE)
  {
   Xc_free(base);
   if(this->dialog->mapped_flag) F(this).readDirectory(this);
   return FALSE;
  }
	
  path = ptr;
 }
 Xc_free(base);
 if(this->dialog->mapped_flag) F(this).readDirectory(this);

 Xc_TRACE(("setPath ok"));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** getPath - Return the path                                         ** 
** ----------------------------------------------------------------- */
static char *getPath(this)
c_FileSelector *this;
{
 fs_file_t *ptr;
 char *path;
 int count;
 long len;

 Xc_HISTORY(("getPath"));

 for(ptr = this->path, len = 1, count = 0; ptr != NULL; ptr = ptr->next)
 {
  long l;

  l = strlen(ptr->name);
  Xc_ASSERT((l > 0) && (ptr->name[l - 1] == '/'));
  len += l;
  count++;
 }

 if((path = Xc_malloc("path", len)) == NULL) return NULL;

 *path = 0;

 while(count--)
 {
  int i;
      
  for(i = count, ptr = this->path; i--; ptr = ptr->next); 
  strcat(path, ptr->name);
 }

 Xc_TRACE((path));
 return path;
}


/* ----------------------------------------------------------------- ** 
** readDirectory - Read the files                                    ** 
** ----------------------------------------------------------------- */
static boolean readDirectory(this)
c_FileSelector *this;
{
 struct dirent *dir_entry;
 long path_len, file_pos;
 DIR *dir_handle;
 fs_file_t *file;
 char *path;

 Xc_TRACE(("readDirectory"));

 /*--- Remove all files from the list ---*/
 this->selection = NULL;
 F(this->list).deleteItems(this->list, XcIL_POINTER_ARRAY, -1);
 for(file = this->files; file != NULL; )
 {
  fs_file_t *tmp;

  tmp = file;
  file = file->next;
  Xc_free(tmp->name);
  Xc_delete(this->item_h, tmp);
 }
 this->files = NULL;
 this->file_count = 0;
 this->file_total_len = 0;

 F(this->popup).setSelection(this->popup, this->path);

 /*--- Get the current path ---*/
 if((path = F(this).getPath(this)) != NULL)
 {
  struct stat info;

  if(!stat(path, &info))
  {
   this->last_time = info.st_mtime;
  }
 }
 else
  return FALSE;

 if((dir_handle = opendir(path)) == NULL)
 {
  Xc_SYSERROR((path));
  Xc_free(path);
  return FALSE;
 }
  
 path_len = file_pos = strlen(path);
  
 if((file = Xc_new(this->item_h)) != NULL)
  while((dir_entry = readdir(dir_handle)) != NULL)
   if(is_interresting(this, dir_entry) != FALSE)
   {
    long file_len;
	  
    file_len = NLENGTH(dir_entry) + 1;
    if((file_pos + file_len) > path_len)
    {
     char *new;
	      
     if((new = Xc_realloc("path", path, file_pos + file_len)) == NULL)
      continue;
     path_len = file_pos + file_len;
     path = new;
    }
    memcpy(path + file_pos, dir_entry->d_name, file_len);

    if((stat(path, &file->stat) < 0) && (lstat(path, &file->stat) < 0))
     continue;

    if((file->name = Xc_malloc("filename", file_len + 1)) == NULL)
     continue;
    memcpy(file->name, dir_entry->d_name, --file_len);

    switch(file->stat.st_mode & S_IFMT)
    {
    case S_IFREG:
     file->type = XcFS_NORMAL;
     break;
    case S_IFLNK:
     file->type = XcFS_LINK;
     file->name[file_len++] = '@';
     break;
    case S_IFDIR:
     file->type = XcFS_DIR;
     file->name[file_len++] = '/';
     break;
    default:
     file->type = XcFS_SPECIAL;
     file->name[file_len++] = '%';
     break;
    }
    file->name[file_len] = 0;

    if(is_really_interresting(this, file) == FALSE)
    {
     Xc_free(file->name);
    }
    else
    {
     /*--- Add in linked list ---*/
     file->next = this->files;
     this->files = file;
     this->file_count++;
     this->file_total_len += file->stat.st_size;

     if((file = Xc_new(this->item_h)) == NULL) break;
    }
   }

 closedir(dir_handle);
 Xc_free(path);
 if(file != NULL) Xc_delete(this->item_h, file);
 F(this->list).addItems(this->list, FALSE,
			XcIL_LINKED_LIST, this->file_count,
			this->files, offsetof(fs_file_t, next));
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** pushFolder - Push a folder in path                                ** 
** ----------------------------------------------------------------- */
static boolean pushFolder(this, folder)
c_FileSelector *this;
fs_file_t *folder;
{
 fs_file_t *file;

 Xc_TRACE(("pushFolder(`%s')", folder->name));
 Xc_ASSERT(folder->type == XcFS_DIR);

 if((file = Xc_new(this->item_h)) == NULL) return FALSE;
 memcpy(file, folder, sizeof(fs_file_t));
 if((file->name = Xc_strdup("foldername", folder->name)) == NULL)
 {
  Xc_delete(this->item_h, file);
  return FALSE;
 }

 file->next = this->path;
 this->path = file;

 F(this->popup).addItems(this->popup, XcIM_VA_LIST, 1, file);
  
 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_list - ItemList callback function                              ** 
** ----------------------------------------------------------------- */
static void cb_list(list, this, reason)
c_ItemList *list;
c_FileSelector *this;
int reason;
{
 fs_file_t **selection;
 int count;

 Xc_TRACE(("cb_list"));
  
 if(F(list).getSelection(list, (void ***)&selection, &count) && count)
 {
  if((count == 1) && (selection[0]->type == XcFS_DIR))
  {
   if((reason == XcIL_DOUBLE_CLICK) || (this->selection == *selection))
   {
    XcFS_END_UPDATE_PROC(this);
    pushFolder(this, *selection);
    F(this).readDirectory(this);
    /*F(this->text_field).set(this->text_field, "");*/
    XcFS_BEGIN_UPDATE_PROC(this);
   }
  }
  else
  {
   char *string;
	  
   if((string = Xc_strdup("filename", selection[0]->name)) != NULL)
   {
    if((selection[0]->type == XcFS_LINK) ||
       (selection[0]->type == XcFS_SPECIAL))
     string[strlen(string) - 1] = 0;
   }
	  
   F(this->text_field).set(this->text_field, string);
   Xc_free(string);
   if(reason == XcIL_DOUBLE_CLICK)
   {
    F(this->dialog).unmap(this->dialog);
    this->dialog->result = XcCD_OK;
   }
  }
  this->selection = *selection;
  Xc_free(selection);
 }
 XmProcessTraversal(WIDGET(this->text_field), XmTRAVERSE_CURRENT);
}


/* ----------------------------------------------------------------- ** 
** cb_popup - ItemMenu callback function                             ** 
** ----------------------------------------------------------------- */
static void cb_popup(popup, this, file)
c_ItemMenu *popup;
c_FileSelector *this;
fs_file_t *file;
{
 fs_file_t *ptr;
 int count;

 Xc_TRACE(("cb_popup"));

 for(count = 0, ptr = this->path; ptr != file; ptr = ptr->next) count++;
 if(count > 0)
 {
  XcFS_END_UPDATE_PROC(this);
  F(popup).deleteItems(popup, XcIL_LINKED_LIST, count,
		       this->path, offsetof(fs_file_t, next));
  for(ptr = this->path; count--; )
  {
   fs_file_t *tmp;
	  
   tmp = ptr;
   ptr = ptr->next;
   Xc_free(tmp->name);
   Xc_delete(this->item_h, tmp);
  }
  this->path = ptr;
  F(this).readDirectory(this);
  XcFS_BEGIN_UPDATE_PROC(this);
 }
 Xc_TRACE(("cb_popup done"));
}


/* ----------------------------------------------------------------- ** 
** cb_textfield - Textfield callback                                 ** 
** ----------------------------------------------------------------- */
static void cb_textfield(field, this)
c_WTextField *field;
c_FileSelector *this;
{
 boolean change_directory = FALSE;
 char *text;

 Xc_TRACE(("cb_textfield"));

 /*--- filename correction ---*/
 if((text = F(field).get(field)) != NULL)
 {
  boolean flag;
  char *ptr;
  long len;

  if (!strcmp(text, this->old_selection))
  {
   Xc_free(text);
   return;
  }
  else
   strcpy(this->old_selection, text);
      
  len = strlen(text);
  ptr = text + len - 1;
  while((ptr >= text) && (*ptr == 32)) *ptr-- = 0;
  for(ptr = text; *ptr == 32; ++ptr);
  flag = ((ptr != text) || ((long)strlen(ptr) != len))? TRUE : FALSE;

  /*--- Absolute filenames ---*/
  if((*ptr == '~') || (ptr[0] == '.' && ptr[1] == '.') || 
     (strchr(ptr, '/') != NULL))
  {
   XcFS_END_UPDATE_PROC(this);
   if ((IsDirExist(ptr) || !strcmp(ptr, "~"))
       && F(this).setPath(this, ptr))
   {
    ptr = NULL;
    flag = TRUE;
    change_directory = TRUE;
   }
   else
   {
    char *dir;
	      
    if ((dir = GetDirname(ptr, TRUE)) != NULL)
    {
     if(F(this).setPath(this, dir))
     {
      ptr = GetBasename(ptr, FALSE);
      flag = TRUE;
     }
     Xc_free(dir);
     change_directory = TRUE;
    }
   }
   XcFS_BEGIN_UPDATE_PROC(this);
  }
      
  if(flag) F(field).set(field, ptr);
  Xc_free(text);
 }
  
 if(change_directory) return;

 /*--- Force extension ---*/
 if(this->force_extension != FALSE)
 {
  if((text = F(field).get(field)) != NULL)
  {
   char **ptr;

   for(ptr = this->patterns; *ptr != NULL; ptr++)
    if(fnmatch(*ptr, text, FNM_PERIOD) == 0) break;

   if((*ptr == NULL) && *text)
   {
    /*--- Force extension ---*/
    char *ext;

    Xc_TRACE(("forcing extension"));
    if((ext = this->patterns[0]) != NULL)
    {
     while(*ext && (*ext != '.')) ext++;
		  
     if((ext = Xc_strconcat(text, ext, NULL)) != NULL)
     {
      Xc_TRACE(("filename = `%s'", ext));
      F(field).set(field, ext);
      Xc_free(ext);
     }
    }
   }
   Xc_free(text);
  }
 }
 F(this->dialog).resetTraversal(this->dialog);
}


/* ----------------------------------------------------------------- ** 
** menu_sort - Sort popup menu                                       ** 
** ----------------------------------------------------------------- */
static int menu_sort(a, b)
fs_file_t *a;
fs_file_t *b;
{
 for( ; a != NULL; a = a->next)
  if ((a = b) != NULL) return 1;
 return -1;
}


/* ----------------------------------------------------------------- ** 
** sort_alpha - Alpha sort for files                                 ** 
** ----------------------------------------------------------------- */
static int sort_alpha(a, b)
fs_file_t *a;
fs_file_t *b;
{
 if(a->type == b->type) return strcmp(a->name, b->name);
 if(a->type == XcFS_DIR) return -1;
 if(b->type == XcFS_DIR) return 1;
 return strcmp(a->name, b->name);
}


/* ----------------------------------------------------------------- ** 
** addPattern - Add a pattern for filter                             ** 
** ----------------------------------------------------------------- */
static boolean addPattern(this, clear_flag, pattern)
c_FileSelector *this;
boolean clear_flag;
char *pattern;
{
 char **ptr;
 long len;

 Xc_HISTORY(("addPattern(%s, `%s')",
	     clear_flag? "True":"False", pattern? pattern : "<default>"));

 if(clear_flag != FALSE)
 {
  for(ptr = this->patterns; *ptr != NULL; ptr++)
   Xc_free(*ptr);
  ptr = this->patterns;
  *ptr = NULL;
 }
 else
 {
  int i;

  for(ptr = this->patterns, i = 0; *ptr != NULL; ptr++, i++);
  if(i == XcFS_MAX_PATTERN)
  {
   Xc_WARNING(("Too much patterns"));
   return FALSE;
  }
 }
 if(pattern == NULL) return TRUE;

 len = strlen(pattern) + 1;

 if((*ptr = Xc_malloc("pattern", len + 2)) == NULL) return FALSE;
 ptr[1] = NULL;
 memcpy(*ptr, pattern, len);

 if(this->dialog->mapped_flag) F(this).readDirectory(this);

 return TRUE;
}


/* ----------------------------------------------------------------- ** 
** cb_update - Upadte directory list                                 ** 
** ----------------------------------------------------------------- */
static void cb_update(this)
c_FileSelector *this;
{
 char *path;

 Xc_TRACE(("cb_update"));
  
 if((path = F(this).getPath(this)) != NULL)
 {
  struct stat info;

  if(!stat(path, &info))
  {
   if(info.st_mtime > this->last_time) F(this).readDirectory(this);
  }
      
  Xc_free(path);
 }

 XcFS_BEGIN_UPDATE_PROC(this);
}
