/*
** DirSelector.h for XInstall in DirSelector/
** Definition of the DirSelector class
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
** Last update Mon Jan 19 14:02:24 1998 Emmanuel Paris
*/

#ifndef __XC_DIR_SELECTOR_H__
#define __XC_DIR_SELECTOR_H__

typedef struct sc_DirSelector c_DirSelector;

#include "xcalibur.h"
#include "file.h"
#include "ManageWidget.h"
#include "WInterface.h"
#include "CustomDialog.h"
#include "ItemList.h"
#include "ItemMenu.h"
#include "WTextField.h"
#include "Label.h"
#include "Grid.h"

/*--- define ---*/
#define XcDS_MAX_PATTERN 16

#define XcDS_NONE	0
#define XcDS_FREE_SPACE	1
#define XcDS_CREATE_DIR	2


/*--- Internal structures ---*/
enum filetype_e {XcDS_LINK, XcDS_DIR, XcDS_NORMAL, XcDS_SPECIAL};
typedef struct fs_file_s fs_file_t;

struct fs_file_s {
  char *name;
  fs_file_t *next;
  struct stat stat;
  enum filetype_e type;
};


/*--- Methods ---*/
typedef struct
{
  F_STD;

  char *(*getFile) ___PROTO((c_DirSelector *this));
  boolean (*readDirectory) ___PROTO((c_DirSelector *this));

  boolean (*setPath) ___PROTO((c_DirSelector *this, char *path));
  char *(*getPath) ___PROTO((c_DirSelector *this));
  boolean (*addPattern) ___PROTO((c_DirSelector *this,
				  boolean clear_flag, char *pattern));
} sf_DirSelector;


/*--- Class ---*/
struct sc_DirSelector
{
  sf_DirSelector *f;

  Widget w_This;
  Widget w_Title;
  c_CustomDialog *dialog;
  c_ItemList *list;
  c_ItemMenu *popup;
  c_WTextField *text_field;
  c_Label *label;
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
  char *patterns[XcDS_MAX_PATTERN + 1];
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

  Widget	w_box;
  Widget	w_bul;
  int		mode;
  Widget	w_create;
  c_Grid	*Grid;
  boolean	write_dir;
};

extern sf_DirSelector fc_DirSelector;

#endif /* !__XC_DIR_SELECTOR_H__ */

