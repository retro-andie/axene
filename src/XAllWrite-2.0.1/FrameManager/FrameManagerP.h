/*
** FrameManagerP.h for XAllWrite in FrameManager/
** Private header for the FrameManager class
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
** Started on  Mon Jul 24 16:47:53 1995 Stéphane Boisson
** Last update Tue Jul  7 16:53:31 1998 Emmanuel Paris
*/

#ifndef __XC_FRAME_MANAGER_P_H__
#define __XC_FRAME_MANAGER_P_H__

#include "FrameManager.h"
#include "X11/keysym.h"

#if defined(Xc_XQUAD)
boolean printFrameList ___PROTO((c_FrMg *this, c_PostScript *post,
				 bbox_t *bbox, setup_print_t *setup));
#endif
#if defined(Xc_XALLWRITE)
boolean printFrameList ___PROTO((c_FrMg *this, c_PostScript *post,
				 boolean *over_clip));
boolean printFrameOver ___PROTO((c_FrMg *this, c_PostScript *post,
				 void (*callback)(), void *cb_data));
#endif

extern boolean writeFrMg ___PROTO((c_FrMg *this, c_DocFile *doc));
extern boolean readFrMg ___PROTO((c_DocFile *doc, char *keyword,
				  long param, void *feuille));

extern l_Cadre *sort_asc_list_cadre ___PROTO((l_Cadre *lcadre));
extern l_Cadre *sort_desc_list_cadre ___PROTO((l_Cadre *lcadre));

#if defined(Xc_XALLWRITE)
extern boolean SetFunctionObject ___PROTO((c_FrMg *cfrmg, int num_function));
#endif

#endif /* !__XC_FRAME_MANAGER_P_H__ */

