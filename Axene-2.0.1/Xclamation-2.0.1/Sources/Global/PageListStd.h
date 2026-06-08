/*
** PageListStd.h for Xclamation in Global/
** Document Function defines
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
** Started on  Wed Oct 21 19:37:15 1998 Emmanuel Paris
** Last update Thu Oct 22 19:02:24 1998 Emmanuel Paris
*/

#ifndef _PageList_h_
#define _PageList_h_

enum
{
 DOCUMENT_FUNCTION_BEGIN = 0,
 DOC_ZOOM_FIT_IN_WIDTH,
 DOC_ZOOM_FIT_IN_SCREEN,
 DOC_ZOOM_AT_REAL_SIZE,
 DOC_DISPLAY_GRID,
 DOC_DISPLAY_TYPO_MARK,
 DOC_DISPLAY_ALIGN_MARK,
 DOC_DISPLAY_FRAME_BORDER,
 DOC_MAGNETIZE_ALL,
 DOC_INSERT_PAGE_BEFORE,
 DOC_INSERT_PAGE_AFTER,
 DOC_MODIFY_PAGE,
 DOC_DELETE_PAGE,
 DOC_GOTO_PAGE,
 DOC_GOTO_FIRST_PAGE,
 DOC_GOTO_LAST_PAGE,
 DOC_GOTO_PREVIOUS_PAGE,
 DOC_GOTO_NEXT_PAGE,
 DOCUMENT_FUNCTION_END
};

#endif /*_PageList_h_*/
