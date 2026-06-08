/*
** HelpLine.h for Xclamation, XQuad, XAllWrite, XMayday and AxeneOffice in 
** 	HelpLine/
** Object and method definition for HelpLine
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
** Started on  Sometime during     1994 One of the authors
** Last update Sat Jan  8 15:38:56 2000 Emmanuel Paris
*/

#ifndef _HelpLine_h
#define _HelpLine_h

#include "xcalibur.h"
#include "ManageWidget.h"
#include "WInterface.h"
#include "Label.h"
#include <X11/Intrinsic.h>

# ifdef ___PSEUDO_COMPOSE_MACRO
# define SET_HELPLINE(helpline,widget,message,func) \
 F(helpline).enable_helpline(helpline, widget, XcR_##message, func)
#else
# ifdef ___GNUC_COMPOSE_MACRO
# define SET_HELPLINE(helpline,widget,message,func) \
 F(helpline).enable_helpline(helpline, widget, XcR_##message##, func)
# else
# define SET_HELPLINE(helpline,widget,message,func) \
 F(helpline).enable_helpline(helpline, widget, XcR_/**/message/**/, func)
# endif
#endif /* PSEUDO */

# define UNSET_HELPLINE(helpline,widget) \
 F(helpline).DisableHelpLine(helpline, widget)

#define XcR_NONE -1
#define XcR_DEFAULT -2

#if defined(Xc_XCLAMATION)
#define DEFAULT_FUNC "Xclamation. ©1994-2000 Axene."
#elif defined(Xc_XQUAD)
#define DEFAULT_FUNC "XQuad. ©1995-2000 Axene."
#elif defined(Xc_XALLWRITE)
#define DEFAULT_FUNC "XAllWrite. ©1997-2000 Axene."
#elif defined(Xc_XMAYDAY)
#define DEFAULT_FUNC "XMayday. ©1996-2000 Axene."
#else
#define DEFAULT_FUNC "Axene Software. @1994-2000 Axene."
#endif

typedef struct sc_HelpLine c_HelpLine;

typedef struct _helpline_t
{
 c_HelpLine	*This;
 Widget		widget;
 int		message;
 int		function;
 struct _helpline_t	*next;
} helpline_t;

typedef enum
{
 XcHL_NO_LABEL, XcHL_LABEL_GIVEN, XcHL_BUILD_LABEL
} hl_type_t;

typedef enum
{
 XcHL_NO_BUBBLE, XcHL_CLASSIC_BUBBLE, XcHL_FUN_BUBBLE
} bubble_type_t;

/* defint the method for Class HelpLine */
typedef struct
{
 F_STD;
 void (*enable_helpline) ___PROTO((c_HelpLine *This, Widget widget, 
				   int message, int func));
 void (*disable_helpline) ___PROTO((c_HelpLine *This, Widget widget));
 void (*change_function_message) ___PROTO((c_HelpLine *This, int message,
					   int function));
 int  (*get_helpline_message) ___PROTO((c_HelpLine *This, Widget widget));
} sf_HelpLine;

/* define the HelpLine Class */
struct sc_HelpLine
{
 sf_HelpLine	*f;
 Widget		w_This;
 c_Label        *Label;
 ItemHandle	*hl_handle;
 helpline_t	*base;
 int		function;
 int		message;
 boolean	in_window;
 boolean	popup;
 bubble_type_t	bubbles;
 Display	*display;
 Window		window;
 unsigned long	color[5];
 XFontStruct	*font;
};

extern sf_HelpLine fc_HelpLine;

#endif






