/*
** CustomDialog.h for Xclamation, XQuad, XAllWrite, XMayday and AxeneOffice 
** 	in CustomDialog/
** Class to create enhanced custom dialogs
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
** Started on  Thu Oct  6 14:18:01 1994 Stéphane Boisson
** Last update Sat Mar  8 18:32:01 1997 Robin Castanier
*/

#ifndef __XC_CUSTOM_DIALOG_H__
#define __XC_CUSTOM_DIALOG_H__

typedef struct sc_CustomDialog c_CustomDialog;

#include "xcalibur.h"
#include "WInterface.h"
#include "ManageWidget.h" 
#include "MenuBar.h"
#include "Icon.h"
#include <X11/Intrinsic.h>

#define XcCD_DESTROY 0
#define XcCD_SAVE 1
#define XcCD_SAVE_AS 2
#define XcCD_YES 4
#define XcCD_NO 8
#define XcCD_APPLY 16
#define XcCD_OK 32
#define XcCD_OPEN 64
#define XcCD_RETRY 128
#define XcCD_STOP 256
#define XcCD_RESET 512
#define XcCD_CANCEL 1024
#define XcCD_CLOSE 2048
#define XcCD_CONTINUE 4096
#define XcCD_QUIT 8192
#define XcCD_HELP 16384

#define XcCD_NB_BUTTON_DEF 15

#define XcCD_MAX_BLOCKED_CHILDREN 8

#define XcCD_HORIZONTAL 1
#define XcCD_VERTICAL 0
#define XcCD_HRESIZE 2
#define XcCD_VRESIZE 4
 
typedef void (*dialog_cb_func_t) ___PROTO((c_CustomDialog *cb_dialog,
					   void *cb_data,
					   int cb_reason));


/*--- Definition of the CutomDialog methods ---*/
typedef struct
{
  F_STD;

  /*--- Activation & Desactivation ---*/
  void (*map) ___PROTO((c_CustomDialog *this, boolean modal_flag));
  void (*unmap) ___PROTO((c_CustomDialog *this));

  /*--- Create ActionArea (must be last child) ---*/
  void (*createActionArea) ___PROTO((c_CustomDialog *this,
				     unsigned int button_list,
				     unsigned int default_button,
				     boolean help_activ,
				     dialog_cb_func_t cb_func,
				     void *callback_data));

  /*--- Children position setting ---*/
  void (*setMenuBar) ___PROTO((c_CustomDialog *this, c_MenuBar *bar));
  void (*blockChild) ___PROTO((c_CustomDialog *this, Widget w, int size));

  int (*waitForUnmap) ___PROTO((c_CustomDialog *dialog));
  void (*resetTraversal) ___PROTO((c_CustomDialog *this));
} sf_CustomDialog;


/*--- Definition of the CustomDialog class ---*/
struct sc_CustomDialog
{
  sf_CustomDialog *f;

  /*___Widgets stuff___*/
  Widget w_This;
  Widget w_Main;
  Widget w_MenuBar;
  Widget w_Separator;
  Widget w_ActionArea;
  c_ManageWidget *manage_widget;
  Widget w_button[XcCD_NB_BUTTON_DEF];
  int default_button;

  int width;
  int height;
  int separator_height;
  int action_height;
  int bar_height;

  int child_count;
  boolean vertical_flag;
  int total_size;
  int size[XcCD_MAX_BLOCKED_CHILDREN];
  Widget child[XcCD_MAX_BLOCKED_CHILDREN];
  
  /*___Custom callback___*/
  dialog_cb_func_t callback;
  void *callback_data;

  /*___Flags___*/
  boolean modal_flag;
  boolean mapped_flag;
  int result;

  int button_list;
  Widget w_ActionAreaTop;
  c_Icon *help;
  boolean help_activ;
};

extern sf_CustomDialog fc_CustomDialog;

#endif /* !__XC_CUSTOM_DIALOG_H__ */


