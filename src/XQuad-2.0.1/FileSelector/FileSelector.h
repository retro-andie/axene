/*
** FileSelector.h for Xclamation, XQuad, XAllWrite and XMayday in FileSelector/
** Definition of the FileSelector class
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
** Started on  Fri Dec  9 16:00:01 1994 Stéphane Boisson
** Last update Wed May  7 16:21:48 1997 Emmanuel Paris
*/

#ifndef __XC_FILE_SELECTOR_H__
#define __XC_FILE_SELECTOR_H__

typedef struct sc_FileSelector c_FileSelector;

#include "xcalibur.h"
#include "file.h"
#include "ManageWidget.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "ItemList.h"
#include "ItemMenu.h"
#include "WTextField.h"

#define XcFS_MAX_PATTERN 16

/*--- Internal structures ---*/
enum filetype_e { XcFS_LINK, XcFS_DIR, XcFS_NORMAL, XcFS_SPECIAL};
typedef struct fs_file_s fs_file_t;

struct fs_file_s
{
 char *name;
 fs_file_t *next;
 struct stat stat;
 enum filetype_e type;
};


/*--- Methods ---*/
typedef struct
{
 F_STD;

 char *(*getFile) ___PROTO((c_FileSelector *this));
 boolean (*readDirectory) ___PROTO((c_FileSelector *this));

 boolean (*setPath) ___PROTO((c_FileSelector *this, char *path));
 char *(*getPath) ___PROTO((c_FileSelector *this));
 boolean (*addPattern) ___PROTO((c_FileSelector *this,
				 boolean clear_flag, char *pattern));
} sf_FileSelector;


/*--- Class ---*/
struct sc_FileSelector
{
 sf_FileSelector *f;

 Widget w_This;
 Widget w_Title;
 c_CustomDialog *dialog;
 c_ItemList *list;
 c_ItemMenu *popup;
 c_WTextField *text_field;
 Widget form;

 boolean first_time;
 ItemHandle *item_h;

 void *data;

 /*___Path stuff___*/
 fs_file_t *path;
 fs_file_t *files;
 long file_count;
 long file_total_len;
 fs_file_t *selection;

 /*___Patterns___*/
 char *patterns[XcFS_MAX_PATTERN + 1];
 char	old_selection[256];
  
 /*___Configuration___*/
 boolean show_invisibles;
 boolean show_backups;
 boolean show_all;
 boolean show_specials;
 boolean force_extension;

 /*___Time out id___*/
 XtIntervalId  timout;
 time_t	last_time;

 /*___top item visible___*/
 int	top_item_position;
};

extern sf_FileSelector fc_FileSelector;

#endif /* !__XC_FILE_SELECTOR_H__ */

