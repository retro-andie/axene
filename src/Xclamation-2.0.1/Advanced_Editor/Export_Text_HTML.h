/*
** Export_Text_HTML.h for Xclamation in Advanced_Editor/
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
** Started on  Mon Jul 17 13:52:07 1995 Marc Blet
** Last update Mon Oct 23 17:17:15 1995 Emmanuel Paris
*/

#ifndef _Export_Text_HTML_h
#define _Export_Text_HTML_h

extern void initialize_export_text ___PROTO((c_TextStyle *tstyle));
extern void generate_ascii ___PROTO((c_AEditor *This, FILE *file));
extern void generate_html ___PROTO((c_AEditor *This, FILE *file,
				    char *filename));

#endif
