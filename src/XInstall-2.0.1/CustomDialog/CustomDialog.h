/*
** CustomDialog.h for XInstall in CustomDialog/
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
** Last update Sat Oct 25 06:51:39 1997 Robin Castanier
*/

#ifndef __XC_CUSTOM_DIALOG_H__
#define __XC_CUSTOM_DIALOG_H__

typedef struct sc_CustomDialog c_CustomDialog;

#include "xcalibur.h"
#include "WInterface.h"
#include "ManageWidget.h" 
#include "MenuBar.h"
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
#define XcCD_DONTAGREE 4096
#define XcCD_CONTINUE 8192

#define XcCD_QUIT 16384
#define XcCD_HELP 32768

#define XcCD_BACK 65536
#define XcCD_NEXT 131072
#define XcCD_INSTALL 262144
#define XcCD_AGREE 524288


#define XcCD_NB_BUTTON_DEF 20

#define XcCDn_SAVE	0
#define XcCDn_SAVE_AS	1
#define XcCDn_YES	2
#define XcCDn_NO	3
#define XcCDn_APPLY	4
#define XcCDn_OK	5
#define XcCDn_OPEN	6
#define XcCDn_RETRY	7
#define XcCDn_STOP	8
#define XcCDn_RESET	9
#define XcCDn_CANCEL	10
#define XcCDn_CLOSE	11
#define XcCDn_DONTAGREE	12
#define XcCDn_CONTINUE	13
#define XcCDn_QUIT	14
#define XcCDn_HELP	15
#define XcCDn_BACK	16
#define XcCDn_NEXT	17
#define XcCDn_INSTALL	18
#define XcCDn_AGREE	19


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
};

extern sf_CustomDialog fc_CustomDialog;

#endif /* !__XC_CUSTOM_DIALOG_H__ */


