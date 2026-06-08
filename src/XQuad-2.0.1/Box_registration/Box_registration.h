/*
** Box_registration.h for Xclamation, XQuad and XAllWrite in Box_registration/
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
** Started on  Wed Feb  7 16:40:33 1996 Robin Castanier
** Last update Wed Aug 11 00:41:08 1999 Emmanuel Paris
*/


#ifndef _Box_registration_h
#define _Box_registration_h

#include "xcalibur.h"
#include "Resource.h"
#include "WInterface.h"
#include "Grid.h"
#include "Frame.h"
#include "CustomDialog.h"
#include "Label.h"
#include "WTextField.h"
#include "RadioBox.h"
#include "ScrolledText.h"
#include "ManageWidget.h"
#include "version.h"
#include <X11/Intrinsic.h>
#include <Xm/DialogS.h>


#define BR_REGISTER_NONE	0
#define BR_REGISTER_PROD1	1
#define BR_REGISTER_PROD2	2
#define BR_REGISTER_PROD3	4

#define BR_SUBSCRIBE_KEYWORD    "subscribe "
#define BR_REG_PROD1            "axene-announce "
#ifdef Xc_XCLAMATION
#define BR_REG_PROD2		"xclamation-bugs "
#endif
#ifdef Xc_XQUAD
#define BR_REG_PROD2            "xquad-bugs "
#endif
#ifdef Xc_XALLWRITE
#define BR_REG_PROD2            "xallwrite-bugs "
#endif
#ifdef Xc_XMAYDAY
#define BR_REG_PROD2		"xmayday-bugs "
#endif
#define BR_REG_PROD3            "axene-users "

/* defint the method for Class Box_registration */
typedef struct
{
  F_STD;
} sf_Box_registration;

/* define the Box_registration Class */
typedef struct
{
  sf_Box_registration	*f;
  Widget	w_This;
  c_CustomDialog *Dialog;
  c_Grid	*Grid;
  c_Label	*lBRegisterTitle;
  c_Frame	*fText;
  c_ScrolledText *sText;
  c_Frame	*fAdress;
  c_Label	*lAdress;
  c_Frame	*fRegistered;
  c_RadioBox	*rbRegistered;
  
  c_Frame	*fCommand;
  c_Grid	*gCommand;
  c_Label	*lMailCommand;
  c_WTextField	*tfMailCommand;
  c_Label	*lReplyAdress;
  c_WTextField	*tfReplyAdress;
  
  int		register_item;
  char		mail_command[101];
  char		reply_adress[101];
} c_Box_registration;

extern sf_Box_registration fc_Box_registration;
extern c_Resource *GlobResources;

#endif











