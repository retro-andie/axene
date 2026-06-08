/*
** Tar.h for XInstall in Tar/
** extract file from tar
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Thu Sep 18 15:57:54 1997 Antoine Buat
** Last update Thu Sep 18 20:58:51 1997 Antoine Buat
*/

extern int extract_tar();
extern int extract_tar_memory();

typedef struct _mem_file
{
 char			*name;
 int			size;
 char			*buffer;
 struct _mem_file	*Next;
} mem_file;
