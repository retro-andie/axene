/*
** Xcaliburd.h for Xclamation in Server/
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
** Started on  Tue Jul 18 12:28:24 1995 Antoine Buat
** Last update Tue Jul 18 12:28:24 1995 Antoine Buat
*/

/* the prog are:
   PAO
   TTX
   OCR
   TBL
   FAX
*/
typedef struct listCLI
{
  char			Ident;
  char			*Login;
  char			*Gecos;
  char			*Server;
  char			Prog;
  int			Socket;
  struct listCLI	*NextClient;
} listclient;


/* Message type:
   GAC	<=>	Add a client
   GDC	<=>	Delete a client
   GMG	<=>	Message arrive (could be any object...)
*/
