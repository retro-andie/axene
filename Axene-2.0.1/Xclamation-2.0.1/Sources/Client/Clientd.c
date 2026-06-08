/*
** Clientd.c for Xclamation in Client/
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
** Started on  Mon Jul 17 14:10:42 1995 Marc Blet
** Last update Mon Jul 17 14:10:42 1995 Robin Castanier
*/

#ifdef mips
#include <bsd/sys/types.h>
#include <bsd/sys/time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <pwd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "Xcaliburd.h"
#include "ServDef.h"

#define MAXREAD 200

int	sock;
char	write_seg_ok;
unsigned char	buffer[MAXREAD];
unsigned char	*AdrSeg;
listclient	*ListClient;

char *GetServerClient();
int readsock();
void shm_alloc();
void handle_usr1();
void handle_usr2();

listobjet	*ListObjet;

int main(argc, argv)
int	argc;
char	**argv;
{
  struct sockaddr_in	nom;
  struct hostent	*hp;
  struct passwd		*identity;
  char 			hostname[40];
  char			minibuf[10];
  fd_set 		fdtoread;
  int			last_desc;
  int			retour;
  key_t			KeySeg;
  int			ComSeg;

#ifdef DEBUG
  printf("Entering Clientd\n");
#endif
  if (argc!=2)
    {
      printf("This program can't be launch alone\n");
      exit(1);
    }
  srand(getpid());
  sscanf(argv[1], "%x", &KeySeg);
#ifdef DEBUG
  printf("Key: %d\n", KeySeg);
#endif
  ComSeg=shmget(KeySeg, SHSEGSIZE, IPC_CREAT|0666);
  AdrSeg=shmat(ComSeg, 0, 0);
  shmctl(ComSeg, IPC_RMID, 0);
  write_seg_ok=1;
  signal(SIGUSR1, handle_usr1);
  signal(SIGUSR2, handle_usr2);
  
  /* creation de la socket select sur la lecture */
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      printf("error: can't create socket\n");
      exit(1);
    }
  hp = (struct hostent *)malloc(sizeof(struct hostent));
  if ((hp = gethostbyname(SERVER_NAME)) == NULL)
    {
      fprintf(stderr,"Nom de machine inexistant\n");
      exit(3);
    }

  /* prepare socket adress */
  bcopy(hp->h_addr, &(nom.sin_addr), hp->h_length);
  nom.sin_family = AF_INET;
  nom.sin_port = htons(PORT);
 
  if (connect(sock, (struct sockaddr *)&nom, sizeof(nom)) == -1)
    {
      perror("error: can't connect\n");
      exit(4);
    }

  identity = getpwuid(getuid());
  gethostname(hostname, 39);

  /*  ObjTmp->ListClient=(listclient *)malloc(sizeof(listclient));
      ObjTmp->ListClient->Login=(char *)malloc(strlen(identity->pw_name)+1);
      strcpy(ObjTmp->ListClient->Login, identity->pw_name);
      ObjTmp->ListClient->Gecos=(char *)malloc(strlen(identity->pw_gecos)+1);
      strcpy(ObjTmp->ListClient->Gecos, identity->pw_name);
      ObjTmp->ListClient->Server=(char *)malloc(strlen(hostname)+1);
      strcpy(ObjTmp->ListClient->Server, hostname);
      ObjTmp->ListClient->Prog=type;
      */

  minibuf[0]='G';
  minibuf[1]='A';
  minibuf[2]='C';
  minibuf[GACIDENT+3]=0;
  minibuf[GACLOGIN+3]=(char)strlen(identity->pw_name)+1;
  minibuf[GACGECOS+3]=(char)strlen(identity->pw_gecos)+1;
  minibuf[GACSERVER+3]=(char)strlen(hostname)+1;
  minibuf[GACPROG+3]=(char)1;
  write(sock, minibuf, SIZEHGAC+3);
  write(sock, identity->pw_name, (char)strlen(identity->pw_name)+1);
  write(sock, identity->pw_gecos, (char)strlen(identity->pw_gecos)+1);
  write(sock, hostname, (char)strlen(hostname)+1);
  
  last_desc=sock+1;
  while (1)
    {
      listobjet	*TmpObjet;

      FD_ZERO(&fdtoread);
      FD_SET(sock, &fdtoread);
      TmpObjet=ListObjet;
      while (TmpObjet!=NULL)
	{
	  FD_SET(TmpObjet->Socket, &fdtoread);
	  TmpObjet=TmpObjet->NextObjet;
	}
      if ((retour=select(last_desc, &fdtoread, NULL, NULL, NULL)))
	{
	  /*todo: essayer de vider juska ce ke l'on trouve un G */
	  readsock(sock, buffer, 3);
	  buffer[3]=0;
#ifdef DEBUG
	  printf("read Mnemo %s?\n",buffer);
#endif
	  if (strcmp("GAC",buffer)==0)
	    {
	      char slogin;
	      char sgecos;
	      char sserver;
	      listclient	**ClientTmp;
	      listclient	*ClientNew;
	      
	      strcpy(AdrSeg, "GAC");
	      readsock(sock, AdrSeg+3, SIZEHGAC);
	      slogin=AdrSeg[GACLOGIN+3];
	      sgecos=AdrSeg[GACGECOS+3];
	      sserver=AdrSeg[GACSERVER+3];
	      readsock(sock, AdrSeg+3+SIZEHGAC, slogin);
	      readsock(sock, AdrSeg+3+SIZEHGAC+slogin, sgecos);
	      readsock(sock, AdrSeg+3+SIZEHGAC+slogin+sgecos, sserver);

	      if ((ClientNew=(listclient *)malloc(sizeof (listclient)))==NULL)
		{
		  printf("Can't add new client in list: memory allocation error\n");
		}
	      ClientNew->Ident=AdrSeg[GACIDENT+3];
	      ClientNew->Login=(char *)malloc(slogin);
	      strcpy(ClientNew->Login, AdrSeg+3+SIZEHGAC);
	      ClientNew->Gecos=(char *)malloc(sgecos);
	      strcpy(ClientNew->Gecos, AdrSeg+3+SIZEHGAC+slogin);
	      ClientNew->Server=(char *)malloc(sserver);
	      strcpy(ClientNew->Server, AdrSeg+3+SIZEHGAC+slogin+sgecos);
	      ClientNew->Prog=AdrSeg[GACPROG+3];
	      ClientTmp = &ListClient;
	      while (*ClientTmp!=NULL)
		{
		  ClientTmp = &((*ClientTmp)->NextClient);
		}
	      *ClientTmp=ClientNew;

	      write_seg_ok=0;
	      kill(getppid(), SIGUSR1);
	      while (!write_seg_ok);
	    }
	  if (strcmp("GDC",buffer)==0)
	    {
	      strcpy(AdrSeg, "GDC");
	      readsock(sock, AdrSeg+3, SIZEHGDC);
	      write_seg_ok=0;
	      kill(getppid(), SIGUSR1);
	      while (!write_seg_ok);
	    }
	  if (strcmp("GSD",buffer)==0)
	    {
	      int			port;
	      char			*server;
	      struct hostent		*hp;
	      struct sockaddr_in	nom;
	      char			*Seg;
	      key_t			Key;
	      listobjet			**ObjetTmp;
	      listobjet			*ObjetNew;
	      long			size;
	      char			tmp;

	      strcpy(AdrSeg, "GSD");
	      readsock(sock, AdrSeg+3, SIZEHGSD);
	      size=AdrSeg[3+GSDSIZEH];
	      size<<=8;
	      size|=AdrSeg[3+GSDSIZEM];
	      size<<=8;
	      size|=AdrSeg[3+GSDSIZEL];

	      switch(AdrSeg[3+GSDTYPE])
		{
		case GSDSHORT:
		  shm_alloc(&Key, &Seg, size);
		  tmp=(char)(Key>>8);
		  AdrSeg[3+GSDPORTH]=tmp;
		  AdrSeg[3+GSDPORTL]=(char)Key&0xFF;
#ifdef DEBUG
		  printf("lire la suite %d octet sur la clef %d\n", size, Key);
#endif
		  read(sock, Seg, size);
		  shmdt(Seg);
		  break;
		case GSDLOCAL:
		  break;
		case GSDNORMAL:
		  break;
		}

	      write_seg_ok=0;
	      kill(getppid(), SIGUSR1);
	      while (!write_seg_ok);
	      /* ici on recoit des objet court */

#ifdef NOTDEF
	      /* alloc memory */
 	      shm_alloc(&Key, &Seg, ***);

	      /* connect to socket on server */
	      port=(unsigned char)AdrSeg[3+GSDPORTH];
	      port<<=8;
	      port+=(unsigned char)AdrSeg[3+GSDPORTL];
	      /* must find the server */

	      server=GetServerClient(ListClient, AdrSeg[3+GSDIDENT]);
#ifdef DEBUG
	      printf("Client Recv on Server: %s Port: %d\n", server, port);
#endif
	      if ((ObjetNew=(listobjet *)malloc(sizeof (listobjet)))==NULL)
		{
		  printf("Can't add new Window: memory allocation error\n");
		  exit(1);
		}
	      if ((ObjetNew->Socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
		  printf("error: can't create socket\n");
		  exit(1);
		}
	      ObjetNew->NextObjet=0;

    	      ObjetTmp = &ListObjet;
	      while (*ObjetTmp!=NULL)
		{
		  ObjetTmp = &((*ObjetTmp)->NextObjet);
		}
	      *ObjetTmp=ObjetNew;

	      hp = (struct hostent *)malloc(sizeof(struct hostent));
	      if ((hp = gethostbyname(server)) == NULL)
		{
		  fprintf(stderr,"Nom de machine inexistant\n");
		  exit(3);
		}
	      free(server);
	      /* prepare socket adress */
	      bcopy(hp->h_addr, &(nom.sin_addr), hp->h_length);
	      nom.sin_family = AF_INET;
	      nom.sin_port = htons(port);
 	      if (connect(ObjetNew->Socket, 
			  (struct sockaddr *)&nom,
			  sizeof(nom)) == -1)
		{
		  perror("error: can't connect: ");
		  exit(4);
		}
#endif

#ifdef DEBUG
	      printf("receiv clientd obj from xcaliburd\n");
#endif
	    }
	}
    }
}

char *GetServerClient(list, ident)
listclient	*list;
char		ident;
{
  listclient	*ClientGet;

  ClientGet=list;
  while ((ClientGet->Ident!=ident) && ClientGet!=NULL)
    {
      ClientGet=ClientGet->NextClient;
    }
  return ClientGet->Server;
}


int readsock(sock, buffer, size)
int	sock;
char	*buffer;
int	size;
{
  int rsize;
  int rd;

  rsize=0;
  while (rsize!=size)
    {
      rd=read(sock, buffer+rsize, size-rsize);
      rsize+=rd;
    }
}

void handle_usr1(sig)
int	sig;
{
  long	size;

#ifdef DEBUG
  printf ("Zouli message y l'est pret sur le segment partager + 256\n");  
#endif
  if (strncmp("GAC", AdrSeg+SHSEGHALF, 3)==0)
    {
      printf("Newclient\n");
    }
  if (strncmp("GDC", AdrSeg+SHSEGHALF, 3)==0)
    {
      printf("Del client\n");
    }
  if (strncmp("GSD", AdrSeg+SHSEGHALF, 3)==0)
    {
      char			*server;
      struct hostent		*hp;
      int			port;
      int			listen_sock;
      struct sockaddr_in	nom;

#ifdef DEBUG
      printf("recv objet to send in clientd\n");
#endif
      memcpy(buffer, AdrSeg+SHSEGHALF, SIZEHGSD+3);
      size=buffer[3+GSDSIZEH];
      size<<=8;
      size|=buffer[3+GSDSIZEM];
      size<<=8;
      size|=buffer[3+GSDSIZEL];
#ifdef DEBUG
      printf("size %ld\n", size);
#endif
      if (buffer[GSDTYPE+3]==GSDSHORT)
	{
	  memcpy(buffer+SIZEHGSD+3, AdrSeg+SHSEGHALF+SIZEHGSD+3, size);
	}
      signal(SIGUSR1, handle_usr1);
      kill(getppid(), SIGUSR2);

#ifdef DEBUG
      printf("This always for %d\n",buffer[3+GSDFOR]);
#endif

      /* todo */
      /* if size small write on the same socket */
      if (buffer[GSDTYPE+3]==GSDSHORT)
	{
	  write(sock, buffer, SIZEHGSD+3+size);
	}
      else
	{
	  /* if same server write on shared segment */
	  /* just send the Key for shared segment */
	  if (buffer[GSDTYPE+3]==GSDLOCAL)
	    {
	      write(sock, buffer, SIZEHGSD+3);
	    }
	  else
	    {
	      /*create socket...*/
	      if ((listen_sock=socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
#ifdef DEBUG
		  printf("error CreateListenSock: can't create socket");
#endif
		  exit(1);
		}
	      do
		{
		  port=random()%8000+1025;
		  bzero((char *)&nom, sizeof(nom));
		  nom.sin_port = htons(port);
		  nom.sin_addr.s_addr = INADDR_ANY;
		  nom.sin_family = AF_INET;
		}  while (bind(listen_sock, (struct sockaddr *)&nom, sizeof(nom)));
	      listen(listen_sock, 3);
#ifdef DEBUG
	      printf("%d port selected\n",port);	      
#endif
	      buffer[3+GSDPORTH]=(port>>8)&0xff;
	      buffer[3+GSDPORTL]=port&0xFF;
	      write(sock, buffer, SIZEHGSD+3);
	    }
	}
    }
}

void handle_usr2(sig)
int	sig;
{
#ifdef DEBUG
  printf ("Ok Bien compris la... bien compris...\n");  
#endif
  write_seg_ok=1;
  signal(SIGUSR2, handle_usr2);
}

void shm_alloc(KeySegment, AdrSegment, Size)
key_t	*KeySegment;
char	**AdrSegment;
long	Size;
{
  struct shmid_ds	sh_buf;
  int			ComSegment;
  do
    {
      *KeySegment=random()%20000;
      if ((ComSegment=shmget(*KeySegment, Size, IPC_CREAT|0666))==-1)
	{
	  printf("error: can't alloc shared memory...\n");
	}
      shmctl(ComSegment, IPC_STAT, &sh_buf);

#ifdef DEBUG
      printf("perm_uid: %d\n",sh_buf.shm_perm.uid);
      printf("perm_gid: %d\n",sh_buf.shm_perm.gid);
      printf("perm_cuid: %d\n",sh_buf.shm_perm.cuid);
      printf("perm_cgid: %d\n",sh_buf.shm_perm.cgid);
      printf("perm_droit: %x\n",sh_buf.shm_perm.mode);
      printf("perm_used: %d\n",sh_buf.shm_perm.seq);
      printf("perm_key: %d\n",sh_buf.shm_perm.key);
      printf("taille: %d\n",sh_buf.shm_segsz);
      /*  printf("id: %d\n",sh_buf.shm_segid);*/
      printf("last user: %d\n",sh_buf.shm_lpid);
      printf("creator: %d\n",sh_buf.shm_cpid);
      printf("nbr attach: %d\n",sh_buf.shm_nattch);
      printf("date last attach: %s\n",ctime(&sh_buf.shm_atime));
      printf("date last dettach: %s\n",ctime(&sh_buf.shm_dtime));
      printf("date last operation: %s\n",ctime(&sh_buf.shm_ctime));
#endif

    } while(sh_buf.shm_cpid!=getpid() && sh_buf.shm_nattch!=0);
  *AdrSegment=shmat(ComSegment, 0, 0);
}
