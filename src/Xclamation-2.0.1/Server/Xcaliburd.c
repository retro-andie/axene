/*
** Xcaliburd.c for Xclamation in Server/
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
** Started on  Tue Jul 18 12:27:53 1995 Antoine Buat
** Last update Tue Jul 18 12:27:53 1995 Antoine Buat
*/


#include <stdio.h>
#ifdef mips
#include <bsd/sys/types.h>
#include <bsd/sys/time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "ServDef.h"

#include "Xcaliburd.h"

/* #define DEBUG */
#define NBR_CLIENTS 9999
#define MAX_BUF 256

void recv_obj();
void send_leave();
void add_client();
char del_client();
int CreateListenSock();


unsigned char		to_write[MAX_BUF];
unsigned char		to_write2[MAX_BUF];
unsigned char		to_read[MAX_BUF];
int		last_desc;
listclient	*ListClient;
int		NbrClient;
char		ident;

main()
{
  int			sock_ecoute;
  int 			sock_service;
  int			sortir;
  struct sockaddr_in	AdrInternet;
  int			lgadr;
  fd_set 		fdtoread;
  int			client_num;
  int			retour;
  int			rez;
  listclient		*ListTmp;

  ident=0;
  /* creation de la socket d'ecoute */
  printf("Server Xcaliburd (c)1994\n");
  sock_ecoute = CreateListenSock(5);
#ifdef DEBUG
  printf("Socket d'ecoute OK on port %d descriptor %d.\n", PORT, sock_ecoute);
#endif

  sortir=0;
  NbrClient=0;
  last_desc=sock_ecoute+1;
  /*  signal(SIGPIPE, SIG_IGN);*/

  while (!sortir)
    {
      FD_ZERO(&fdtoread);
      FD_SET(sock_ecoute, &fdtoread);

      client_num=NbrClient;
      ListTmp=ListClient;
      while (client_num)
	{
	  client_num--;
	  FD_SET(ListTmp->Socket, &fdtoread);
	  ListTmp=ListTmp->NextClient;
	}
      if ((retour=select(last_desc, &fdtoread, NULL, NULL, NULL)))
	{
#ifdef DEBUG
	  printf("%d possibilty recognized by select\n",retour);
#endif
	  if (FD_ISSET(sock_ecoute, &fdtoread))
	    {
	      sock_service = accept(sock_ecoute,
				    (struct sockaddr *)&AdrInternet,
				    &lgadr);
#ifdef DEBUG
	      printf("Client is comming opening descriptor: %d\n",sock_service);
#endif
	      if (NbrClient<NBR_CLIENTS)
		{
		  add_client(sock_service);
		}
	      else
		{
#ifdef DEBUG
		  printf("Trop de Monde\n");
#endif
		  /* ecrit message ... */
		  close(sock_service);
		}
	    }

 	  client_num=0;
	  ListTmp=ListClient;
	  while (client_num<NbrClient)
	    {
#ifdef DEBUG
	      printf("Present: %d\n",ListTmp->Socket);
x#endif
	      if (FD_ISSET(ListTmp->Socket, &fdtoread))
		{
#ifdef DEBUG
		  printf("Read on descriptor %d client n.%d\n",
			 ListTmp->Socket,
			 client_num);
#endif
		  bzero(to_read,MAX_BUF);
		  rez=read(ListTmp->Socket, to_read, 3);
#ifdef DEBUG
		  printf("Read %d bytes\n", rez);
#endif
		  if (rez)
		    {
#ifdef DEBUG
		      printf("Read: %s\n", to_read);
#endif
		      if (!strncmp("GSD", to_read, 3))
			{
#ifdef DEBUG
			  printf("Gonna receive an object...\n");
#endif
			  /* communik with other client */
			  recv_obj(ListTmp);
			}
		    }
		  else
		    {
		      /* send everybody that he leave... */
		      send_leave(ListClient, del_client(client_num));
		    }
		}
	      ListTmp=ListTmp->NextClient;
	      client_num++;
	    }
	}
    }
}

void recv_obj(SourceClient)
listclient	*SourceClient;
{
  listclient	*ClientGet;
  char		ident;
  long		size;
  
  bzero(to_write, MAX_BUF);
  strcpy(to_write,"GSD");
  ident=read(SourceClient->Socket, to_write+3, SIZEHGSD);
  size=to_write[3+GSDSIZEH];
  size<<=8;
  size|=to_write[3+GSDSIZEM];
  size<<=8;
  size|=to_write[3+GSDSIZEL];
#ifdef DEBUG
  printf("read %d\n", ident);
  printf("work\n");
  printf("ident: %d \n", to_write[3+GSDIDENT]);  
  printf("for: %d \n", to_write[3+GSDFOR]);
  printf("number: %d \n", to_write[3+GSDNUMBER]);
  printf("size: %d \n", to_write[3+GSDSIZEH]);
  printf("size: %d \n", to_write[3+GSDSIZEM]);
  printf("size: %d \n", to_write[3+GSDSIZEL]);
  printf("port: %d \n", to_write[3+GSDPORTH]);
  printf("port: %d \n", to_write[3+GSDPORTL]);
#endif
  ident=to_write[3+GSDFOR];
  if (to_write[3+GSDTYPE]==GSDSHORT)
    {
      read(SourceClient->Socket, to_write+3+SIZEHGSD, size);
    }

#ifdef DEBUG
  printf("looking for ident %d\n", ident);
#endif
  ClientGet=ListClient;
  while ((ClientGet->Ident!=ident) && ClientGet!=NULL)
    {
      ClientGet=ClientGet->NextClient;
    }

#ifdef DEBUG
  printf("Writing to %s at %s on descriptor %d\n",
	 ClientGet->Login,
	 ClientGet->Server,
	 ClientGet->Socket);
#endif
  if (to_write[3+GSDTYPE]==GSDSHORT)
    write(ClientGet->Socket, to_write, SIZEHGSD+3+size);
  else
    write(ClientGet->Socket, to_write, SIZEHGSD+3);
}

void send_leave(ClientTmp, ident)
listclient	*ClientTmp;
char		ident;
{
  char	buffer[4];
  if (ident!=-1)
    {
#ifdef DEBUG
      printf("killing %d\n", ident);
#endif
      while (ClientTmp)
	{
#ifdef DEBUG
	  printf("SENDING GDC to %d\n",ClientTmp->Socket);
#endif
	  strcpy(buffer,"GDC");
	  buffer[3+GDCIDENT]=ident;
	  write(ClientTmp->Socket, buffer, 3+SIZEHGDC);
	  ClientTmp=ClientTmp->NextClient;
	}
    }
}

void add_client(sock_service)
int	sock_service;
{
  listclient	**ClientTmp;
  listclient	*ClientTmp2;
  listclient	*ClientNew;
  int		tmp_nbr;

#ifdef DEBUG
  printf("socket accepted as a new client\n");
#endif
  if ((ClientNew=(listclient *)malloc(sizeof (listclient)))==NULL)
    {
      printf("Can't add new client in list: memory allocation error\n");
      exit(1);
    }

  /* read the identity */
  to_read[0]=0;
  read(sock_service, to_read, SIZEHGAC+3); 
  if (strncmp(to_read, "GAC", 3)!=0)
    {
#ifdef DEBUG
      printf("Not log Some Problemo Probably a test Socket: closing...\n");
#endif
      close(sock_service);
    }
  else
    {
#ifdef DEBUG
      printf("%c %c %c %d %d %d %d %d \n", 
	     to_read[0], to_read[1], to_read[2],
	     to_read[3], to_read[4], to_read[5], to_read[6], to_read[7]);
#endif
      ident++;
      ClientNew->Ident=ident;
      ClientNew->Login=(char *)malloc(to_read[GACLOGIN+3]);
      read(sock_service, ClientNew->Login, to_read[GACLOGIN+3]); 
      ClientNew->Gecos=(char *)malloc(to_read[GACGECOS+3]);
      read(sock_service, ClientNew->Gecos, to_read[GACGECOS+3]); 
      ClientNew->Server=(char *)malloc(to_read[GACSERVER+3]);
      read(sock_service, ClientNew->Server, to_read[GACSERVER+3]);
      ClientNew->Prog=to_read[GACPROG+3];
#ifdef DEBUG
      printf("Receive: %s\n", ClientNew->Login);
      printf("Receive: %s\n", ClientNew->Gecos);
      printf("Receive: %s\n", ClientNew->Server);
      printf("Receive: %d\n", ClientNew->Prog);
#endif
      /*********************/

      ClientNew->Socket=sock_service;
      ClientNew->NextClient=NULL;

      /*send list of client*/
      /*********************/
      /*send him other client*/
      /***********************/
      tmp_nbr=NbrClient;
      memcpy(to_write, to_read, SIZEHGAC+3);
      to_write[GACIDENT+3]=ident;

      ClientTmp2=ListClient;
      strcpy(to_write2, "GAC");
      to_write2[GACME+3]=0;
      while (tmp_nbr)
	{
#ifdef DEBUG
	  printf("SENDING GAC to %d\n",ClientTmp2->Socket);
#endif
	  to_write2[GACIDENT+3]=(char)(ClientTmp2->Ident);
	  to_write2[GACLOGIN+3]=(char)strlen(ClientTmp2->Login)+1;
	  to_write2[GACGECOS+3]=(char)strlen(ClientTmp2->Gecos)+1;
	  to_write2[GACSERVER+3]=(char)strlen(ClientTmp2->Server)+1;
	  to_write2[GACPROG+3]=ClientTmp2->Prog;
	  write(sock_service, to_write2, 3+SIZEHGAC);
	  write(sock_service, ClientTmp2->Login, to_write2[GACLOGIN+3]);
	  write(sock_service, ClientTmp2->Gecos, to_write2[GACGECOS+3]);
	  write(sock_service, ClientTmp2->Server, to_write2[GACSERVER+3]);

	  write(ClientTmp2->Socket, to_write, 3+SIZEHGAC);
	  write(ClientTmp2->Socket, ClientNew->Login, to_read[GACLOGIN+3]);
	  write(ClientTmp2->Socket, ClientNew->Gecos, to_read[GACGECOS+3]);
	  write(ClientTmp2->Socket, ClientNew->Server, to_read[GACSERVER+3]);
	  ClientTmp2=ClientTmp2->NextClient;
	  tmp_nbr--;
	}
#ifdef DEBUG
      printf("Send it Noooooow\n");
#endif
      /* to say it's me to me (well in fact give me my ident) */
      to_write[GACME+3]=1;
      write(sock_service, to_write, 3+SIZEHGAC);
      write(sock_service, ClientNew->Login, to_read[GACLOGIN+3]);
      write(sock_service, ClientNew->Gecos, to_read[GACGECOS+3]);
      write(sock_service, ClientNew->Server, to_read[GACSERVER+3]);
      
      ClientTmp = &ListClient;
      NbrClient++;
      while (*ClientTmp!=NULL)
	{
	  ClientTmp = &((*ClientTmp)->NextClient);
	}
      *ClientTmp=ClientNew;
      
      if ((sock_service+1)>last_desc)
	{
	  last_desc=sock_service+1;
	}
    }
}

char del_client(pos)
int	pos;
{
  listclient	**ListTmp;
  listclient	*ClientDel;
  int		client_num;
  char		rette;
  /* arf char-rette elle est bonne nan?? */

#ifdef DEBUG
  printf("detruire client\n");
#endif

  client_num=0;
  ListTmp = &ListClient;
  while (client_num<pos && *ListTmp!=NULL)
    {
      ListTmp = &((*ListTmp)->NextClient);
      client_num++;
    }
  if (client_num!=pos)
    {
      printf("Can't destroy this client\n");
      return -1;
    }
  else
    {
      ClientDel = *ListTmp;
      *ListTmp=((*ListTmp)->NextClient);
      NbrClient--;
      rette=ClientDel->Ident;
      free(ClientDel->Login);
      free(ClientDel->Gecos);
      free(ClientDel->Server);
      close(ClientDel->Socket);
      free(ClientDel);
    }
  return rette;
}

int CreateListenSock(pending)
int	pending;
{
  int 			desc;
  struct sockaddr_in	nom;

  if ((desc=socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
#ifdef DEBUG
      printf("error CreateListenSock: can't create socket");
#endif
      exit(1);
    }

  bzero( (char *)&nom, sizeof(nom));
  nom.sin_port = htons(PORT);
  nom.sin_addr.s_addr = INADDR_ANY;
  nom.sin_family = AF_INET;

  if (bind(desc, (struct sockaddr *)&nom, sizeof(nom)))
    {
#ifdef DEBUG
      printf("error CreateListenSock: can't bind socket");
#endif
      exit(1);
    }
  listen(desc,3);
  return desc;
}




