/*
** ServDef.h for Xclamation in Server/
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
** Started on  Tue Jul 18 12:27:18 1995 Antoine Buat
** Last update Tue Jul 18 12:27:18 1995 Antoine Buat
*/

#define SERVER_NAME "gudul"
#define PORT 6538

#define OBJPACKSIZE	6000

#define SHSEGHALF	256
#define SHSEGSIZE	SHSEGHALF*2

#define SIZEHGAC	6
#define SIZEHGDC	1
#define SIZEHGSD	9

#define GACIDENT	0
#define GACLOGIN	1
#define GACGECOS	2
#define GACSERVER	3
#define GACPROG		4
#define GACME		5

#define GDCIDENT	0

#define GSDIDENT	0
#define GSDFOR		1
#define GSDNUMBER	2
#define GSDSIZEH	3
#define GSDSIZEM	4
#define GSDSIZEL	5
#define GSDTYPE		6
#define GSDPORTH	7
#define GSDPORTL	8

#define GSDSHORT	1
#define GSDLOCAL	2
#define GSDNORMAL	3

#define HEADER_SIZE 18
#define FOOTER_SIZE 18

#define TSEND	1
#define TRECV	2

typedef struct listOBJ
{
  char			Ident;
  char			type;
  int			Socket;
  char			*AdrSeg;
  char			*Buffer;
  long int		Size;
  long int		CSize;
  struct listOBJ	*NextObjet;
} listobjet;
