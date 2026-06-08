/*
** Client.c for Xclamation in Client/
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
** Started on  Mon Jul 17 14:10:16 1995 Marc Blet
** Last update Mon Jul 17 14:10:16 1995 Robin Castanier
*/

#include "ObjStd.h"
#include "Client.h"

/*#define DEBUG*/

void *cons_Client();
void handle_usr1();
void handle_usr2();
void dest_Client();
void *copy_Client();
char *SendMsg_Client();
void ListClient_Client();
void add_client();
void del_client();
void recv_object();
void wait_func();
int try_to_connect();
listclient *GetClient();
int shm_alloc();

sf_Client fc_Client =
{
  cons_Client,
  dest_Client,
  copy_Client,
  SendMsg_Client,
  ListClient_Client
};

jmp_buf		jmp;
c_Client	*ThisG;
char		write_seg_ok;

void *cons_Client(type, function)
char	type;
void	(*function)();
{
  c_Client 		*ObjTmp;
  struct sockaddr_in	nom;
  struct hostent	*hp;
  struct passwd		*identity;
  char 			hostname[40];
  char			minibuf[10];
  int			flag_on;
  struct shmid_ds	sh_buf;

  if ((ObjTmp=(c_Client *)malloc(sizeof (c_Client)))==NULL)
    {
      printf("Can't build this object: memory allocation error\n");
      exit(1);
    }
  ObjTmp->f = &fc_Client;
  ThisG = ObjTmp;
  ObjTmp->Recv_Action = function;
  ObjTmp->ListClient = NULL;
  gethostname(hostname, 39);
  ObjTmp->Hostname = (char *)malloc(strlen(hostname)+1);
  strcpy(ObjTmp->Hostname, hostname);
  
  switch(try_to_connect(NO_MESSAGE,2,PORT))
    {
    case SERVER_OK:
      break;
    case SERVER_OFF:
      if (!fork())
	{
	  char *string=PRG_SERVER_NAME;
	  char *(arg[2]);
	  arg[0]=string;
	  arg[1]=NULL;
	  execvp(string,arg);
	}
      printf("Creating server please wait...\n");
      if (try_to_connect(NO_MESSAGE,10,PORT)==SERVER_OK)
	{
	  printf("Creation Server OK.\n");
	  break;
	}
    default:
      printf("Object Client not created can't join Server\n");
      free(ObjTmp);
      return NULL;
    }

  signal(SIGUSR1, handle_usr1);
  signal(SIGUSR2, handle_usr2);

  srand(getpid());
  shm_alloc(&ObjTmp->KeySegment, &ObjTmp->AdrSegment, SHSEGSIZE);
  
  if (!(ObjTmp->fils=fork()))
    {
      char *(arg[3]);

      arg[0]=(char *)malloc(strlen(PRG_CLIENT_NAME)+1);
      strcpy(arg[0],PRG_CLIENT_NAME);
      arg[1]=(char *)malloc(sizeof(key_t)*2+1);
      sprintf(arg[1],"%x",ObjTmp->KeySegment);
      arg[2]=NULL;
      execvp(arg[0],arg);
    }
  ObjTmp->header=(char *)malloc(HEADER_SIZE);
  ObjTmp->footer=(char *)malloc(FOOTER_SIZE);
  strcpy(ObjTmp->header,"GST");
  strcpy(ObjTmp->footer,"GEN");
  printf("Object Client build\n");
  return ObjTmp;
}

void handle_usr1(sig)
int	sig;
{
#ifdef DEBUG
  printf ("Zouli message y l'est pret sur le segment partager\n");  
#endif
  if (strncmp("GAC", ThisG->AdrSegment, 3)==0)
    {
      printf("Newclient\n");
      add_client(ThisG, ThisG->AdrSegment+3);
    }
  if (strncmp("GDC", ThisG->AdrSegment, 3)==0)
    {
      printf("Del client\n");
      del_client(ThisG, ThisG->AdrSegment[3]);
    }
  if (strncmp("GSD", ThisG->AdrSegment, 3)==0)
    {
      printf("recv objet\n");
      recv_object(ThisG, ThisG->AdrSegment+3);
    }

  signal(SIGUSR1, handle_usr1);
  kill(ThisG->fils, SIGUSR2);
}

void handle_usr2(sig)
int	sig;
{
#ifdef DEBUG
  printf ("Ok Bien compris la... bien compris...+ 256\n");  
#endif
  write_seg_ok=1;
  signal(SIGUSR2, handle_usr2);
}

void dest_Client(This)
c_Client *This;
{
  kill(This->fils, SIGTERM);
#ifdef DEBUG
  printf("sending the SIGTERM to %d\n", This->fils);
#endif
  printf("Object Client destroyed\n");
  free(This);
}

void *copy_Client(ObjectToCopy)
c_Client *ObjectToCopy;
{
  c_Client *ObjTmp;

  if ((ObjTmp=(c_Client *)malloc(sizeof (c_Client)))==NULL)
    {
      printf("Can't copy this object: memory allocation error\n");
      exit(1);
    }
  memcpy(ObjTmp,ObjectToCopy,sizeof (c_Client));
  printf("Object Client copied\n");
  return ObjTmp;
}


/* number of clients limited to int && 0000 */
/* a zero in client is all clients */
char *SendMsg_Client(This, size, msg, client)
c_Client *This;
long	size;
char	*msg;
char	client;
{
  char		tmp;
  listclient	*ListTmp;

#ifdef DEBUG
  printf("Ident %d %d\n", client, This->Ident);
#endif
  if (client!=This->Ident)
    {
      ListTmp=GetClient(This, client);

      strcpy(This->AdrSegment+SHSEGHALF, "GSD");
      This->AdrSegment[SHSEGHALF+3+GSDIDENT]=This->Ident;
      This->AdrSegment[SHSEGHALF+3+GSDFOR]=client;
      This->AdrSegment[SHSEGHALF+3+GSDNUMBER]=This->NumObj++;
      tmp=(char)(size >> 16);
      size&=0xFFFF;
      This->AdrSegment[SHSEGHALF+3+GSDSIZEH]=tmp;
      tmp=(char)(size >> 8);
      This->AdrSegment[SHSEGHALF+3+GSDSIZEM]=tmp;
      This->AdrSegment[SHSEGHALF+3+GSDSIZEL]=size&0xFF;
      if (size<SHSEGHALF-SIZEHGSD)
	{
#ifdef DEBUG
	  printf("small size considere to use the main socket\n");
#endif
	  memcpy(This->AdrSegment+SHSEGHALF+3+SIZEHGSD, msg, size);
	  This->AdrSegment[SHSEGHALF+3+GSDTYPE]=GSDSHORT;
	}
      else
	{
	  key_t	KeySegment;
	  char	*AdrSegment;

	  shm_alloc(KeySegment, AdrSegment, size);
	  tmp=(char)(KeySegment>>8);
	  This->AdrSegment[SHSEGHALF+3+GSDPORTH]=tmp;
	  This->AdrSegment[SHSEGHALF+3+GSDPORTL]=KeySegment&0xFF;
	  if (!strcmp(ListTmp->Server, This->Hostname))
	    {
#ifdef DEBUG
	      printf("same server considere to use shared memory\n");
#endif
	      This->AdrSegment[SHSEGHALF+3+GSDTYPE]=GSDLOCAL;
	    }
	  else
	    {
	      This->AdrSegment[SHSEGHALF+3+GSDTYPE]=GSDNORMAL;
	    }
	}
      write_seg_ok=0;
#ifdef DEBUG
      printf("envoi du signal vers clientd\n");
#endif
      kill(This->fils, SIGUSR1);
      while (!write_seg_ok);
    }
  else
    {
      printf("No Use to send to myself\n");
    }
}

void wait_func(nothing)
int nothing;
{
  signal(SIGALRM, SIG_IGN);
  putchar('.');
  fflush(stdout);
  longjmp(jmp,1);
}

int try_to_connect(message, nbr_test, port)
int message;
int nbr_test;
int port;
{
  struct sockaddr_in	nom;
  struct hostent	*hp;
  int			count;
  int			sock_test1;
  int			sortie;
  int			error;
  char			voidbuffer[2];
  int			flag_on=1;
 
  if ((sock_test1=socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror("creation de socket impossible\n");
      return -1;
    }
  if (message)
    printf("Serveur sur la machine: %s\n",SERVER_NAME);
  hp = (struct hostent *)malloc(sizeof(struct hostent));
  if ((hp = gethostbyname(SERVER_NAME)) == NULL)
    {
      printf("error: can't find machine name");
      return -1;
    }
  bzero((char *)&nom, sizeof(nom));
  bcopy(hp->h_addr, &(nom.sin_addr), hp->h_length);
  nom.sin_family = AF_INET;
  nom.sin_port = htons(port);
 
  count=0;
  if (message)
    printf("Try to connect\n");
  if (setjmp(jmp)==1)
    {
      count++;
    }
  signal(SIGALRM, wait_func);
  alarm(1);
  sortie=0;
  error=1;
  while (count < nbr_test  && !sortie)
    { 
      if (connect(sock_test1, 
		  (struct sockaddr *)&nom, 
		  sizeof(nom)) == -1)
	{
	  signal(SIGALRM, SIG_IGN);
	  error=0;
#ifdef DEBUG
	  printf("CloseA %d \n", sock_test1);
#endif
	  close(sock_test1);
	  if ((sock_test1=socket(AF_INET, SOCK_STREAM, 0)) == -1)
	    {
	      printf("error: can't create socket\n");
	      return -1;
	    }
	  bzero((char *)&nom, sizeof(nom));
	  bcopy(hp->h_addr, &(nom.sin_addr), hp->h_length);
	  nom.sin_family = AF_INET;
	  nom.sin_port = htons(port);
	  count++;
	  sleep(1);
	  putchar('o');
	  fflush(stdout);
	}
      else
	{
	  sortie=1;
	}
    }
  signal(SIGALRM, SIG_IGN);
#ifdef DEBUG
  printf("CloseB %d \n", sock_test1);
#endif
  close(sock_test1);
  putchar('\n');
  if (count<nbr_test)
    {
      if (message)
	printf("connected\n");
      return SERVER_OK;
    }

  if (error)
    {
      if (message)
	printf("Serveur occupe...\n");
      else
	return SERVER_OQP;
    }
  else
    {
      if (message)
	printf("Probleme de connection au port..\n");
    }
  return SERVER_OFF;
}

void ListClient_Client(This)
c_Client	*This;
{
  listclient	*ClientTmp;
  
  ClientTmp=This->ListClient;
  while (ClientTmp!=NULL)
    {
      printf("%d: %s\n",ClientTmp->Ident, ClientTmp->Login);
      ClientTmp=ClientTmp->NextClient;
    }
}

void add_client(This, buffer)
c_Client	*This;
char		*buffer;
{
  listclient	**ClientTmp;
  listclient	*ClientTmp2;
  listclient	*ClientNew;
  int		tmp_nbr;

  if ((ClientNew=(listclient *)malloc(sizeof (listclient)))==NULL)
    {
      printf("Can't add new client in list: memory allocation error\n");
      exit(1);
    }

  if (buffer[GACME])
    {
      printf("I am the new user %d\n", buffer[GACIDENT]);
      This->Ident=buffer[GACIDENT];
    }
  ClientNew->Ident=buffer[GACIDENT];
  ClientNew->Login=(char *)malloc(buffer[GACLOGIN]);
  memcpy(ClientNew->Login, buffer+SIZEHGAC, buffer[GACLOGIN]);
  ClientNew->Gecos=(char *)malloc(buffer[GACGECOS]);
  memcpy(ClientNew->Gecos, buffer+SIZEHGAC+buffer[GACLOGIN] ,
	 buffer[GACGECOS]);
  ClientNew->Server=(char *)malloc(buffer[GACSERVER+3]);
  memcpy(ClientNew->Server, buffer+SIZEHGAC+buffer[GACLOGIN]+buffer[GACGECOS],
	 buffer[GACSERVER]);
  ClientNew->Prog=buffer[GACPROG];

#ifdef DEBUG
  printf("Receive: %s\n", ClientNew->Login);
  printf("Receive: %s\n", ClientNew->Gecos);
  printf("Receive: %s\n", ClientNew->Server);
  printf("Receive: %d\n", ClientNew->Prog);
#endif
      
  ClientTmp = &This->ListClient;
  while (*ClientTmp!=NULL)
    {
      ClientTmp = &((*ClientTmp)->NextClient);
    }
  *ClientTmp=ClientNew;
}

void del_client(This, ident)
c_Client	*This;
char	ident;
{
  listclient	**ListTmp;
  listclient	*ClientDel;

#ifdef DEBUG
  printf("detruire client\n");
#endif

  ListTmp = &This->ListClient;
  while (((*ListTmp)->Ident!=ident) && *ListTmp!=NULL)
    {
      ListTmp = &((*ListTmp)->NextClient);
    }
  if (*ListTmp==NULL)
    {
      printf("Can't destroy this client\n");
    }
  else
    {
      ClientDel = *ListTmp;
      *ListTmp=((*ListTmp)->NextClient);
      free(ClientDel->Login);
      free(ClientDel->Gecos);
      free(ClientDel->Server);
      free(ClientDel);
    }
}

listclient *GetClient(This, ident)
c_Client	*This;
char		ident;
{
  listclient	*ClientGet;

  ClientGet=This->ListClient;
  while ((ClientGet->Ident!=ident) && ClientGet!=NULL)
    {
      ClientGet=ClientGet->NextClient;
    }
  return ClientGet;
}

void recv_object(This, buffer)
c_Client	*This;
unsigned char	*buffer;
{
  char	*Seg;
  key_t	Key;
  int	SegId;
  long	size;

  switch (buffer[GSDTYPE])
    {
    case GSDSHORT:
      /* object received */
      break;
    case GSDLOCAL:
      /* object received */
      break;
    case GSDNORMAL:
      /* object received */
      break;
    }
  size=buffer[GSDSIZEH];
  size<<=8;
  size|=buffer[GSDSIZEM];
  size<<=8;
  size|=buffer[GSDSIZEL];
  Key=buffer[GSDPORTH];
  Key<<=8;
  Key|=buffer[GSDPORTL];
  SegId=shmget(Key, size, IPC_CREAT|0666);
  Seg=shmat(SegId, NULL, 0);
#ifdef DEBUG
  printf("On SegKey %d & size:%d\n", Key, size);
#endif
  This->Recv_Action(Seg, size);
}

int shm_alloc(KeySegment, AdrSegment, Size)
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
	  return 0;
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
  *AdrSegment=shmat(ComSegment, NULL, 0);
}
