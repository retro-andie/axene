/*
** Client.h for Xclamation in Client/
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
** Started on  Mon Jul 17 14:11:08 1995 Marc Blet
** Last update Mon Jul 17 14:11:08 1995 Robin Castanier
*/


#ifndef _Client_h
#define _Client_h

#include "ObjStd.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <setjmp.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <ServDef.h>
#include "Xcaliburd.h"

#define PRG_SERVER_NAME "./Xcaliburd"
#define PRG_CLIENT_NAME "./Clientd"

#define PAO 1
#define TTX 2
#define OCR 3
#define TBL 4
#define FAX 5

#define MAXCOUNT 	10
#define NO_MESSAGE 	0
#define ALL_MESSAGE 	1

#define ERROR 		-1
#define SERVER_OK 	0
#define SERVER_OQP 	1
#define SERVER_OFF	-2

/* define the method for Class Client */
typedef struct
{
  F_STD;
  char *(*SendMsg)();
  void (*ListClient)();
} sf_Client;

/* define the Client Class */
typedef struct
{
  sf_Client	*f;
  int		sock;
  char		Ident;
  char		*Hostname;
  char		*header;
  char		*footer;
  int		fils;
  listclient	*ListClient;
  key_t		KeySegment;
  int		ComSegment;
  char		*AdrSegment;
  char		NumObj;
  void 		(*Recv_Action)();
} c_Client;

extern sf_Client fc_Client;

#endif


