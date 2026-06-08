/*
** LMClient.c for Xclamation in LinkManager/
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
** Started on  Mon Jul 17 19:12:52 1995 Marc Blet
** Last update Mon Jul 17 19:12:52 1995 Emmanuel Paris
*/

#include "ObjStd.h"
#include "LinkManager.h"

/*
 * Le type REMOTE_MANAGED n'est pas integre a la version 1.0 faute de temps 
*/

/* modifier les procedures d'envoi : passer comme param une liste de
 descripteurs de  clients a la place d'un descripteur de client ...
   cela permet l'envoi en masse 
*/




sf_LM_Client fc_LM_Client =
{
  cons_LM_Client,
  desXcLM_Client,
  copy_LM_Client,
  XcLM_Client_Send,
  XcLM_Client_Receive,
  XcLM_Client_Build_Object,
  XcLM_Client_Destroy_Object,
  XcLM_Client_Copy_Object,
  XcLM_Client_Allocate_Object_Descriptor,
  XcLM_Client_Destroy_Object_Descriptor,
  XcLM_Client_Copy_Object_Descriptor,
  XcLM_Client_Allocate_Object_Descriptor,
  XcLM_Client_Destroy_Client_Descriptor,
  XcLM_Client_Copy_Client_Descriptor,
  XcLM_Client_Get_Client_Class,
  XcLM_Client_Set_Client_Class,
  XcLM_Client_Unset_Client_Class,
  XcLM_Client_Test_Client_Class,
  XcLM_Client_Get_Client_Attributes,
  XcLM_Client_Set_Client_Attributes,
  XcLM_Client_Unset_Client_Attributes,
  XcLM_Client_Test_Client_Attributes,
  XcLM_Client_Get_Object_Class,
  XcLM_Client_Set_Object_Class,
  XcLM_Client_Unset_Object_Class,
  XcLM_Client_Test_Object_Class, 
  XcLM_Client_Get_Object_Attributes,
  XcLM_Client_Set_Object_Attributes,
  XcLM_Client_Unset_Object_Attributes,
  XcLM_Client_Test_Object_Attributes, 
  XcLM_Client_Initialize_Object_List,
  XcLM_Client_Destroy_Object_List,
  XcLM_Client_Remove_Object_List,
  XcLM_Client_Register_Object,
  XcLM_Client_Auto_Register_Object,
  XcLM_Client_List_Object,
  XcLM_Client_Test_Object_List,
  XcLM_Client_Convert_Object_Descriptor_To_Com,
  XcLM_Client_Convert_Com_To_Object_Descriptor,
  XcLM_Client_Convert_String_To_Com,
  XcLM_Client_Convert_Com_To_String,
  XcLM_Client_Convert_Byte_To_Com,
  XcLM_Client_Convert_Com_To_Byte,
  XcLM_Client_Convert_Int_To_Com,
  XcLM_Client_Convert_Com_To_Int,
  XcLM_Client_Convert_Object_Descriptor_List_To_Com,
  XcLM_Client_Convert_Com_To_Object_Descriptor_List,
  XcLM_Client_Convert_Client_Descriptor_To_Com,
  XcLM_Client_Convert_Com_To_Client_Descriptor,
  XcLM_Client_Convert_Client_Descriptor_List_To_Com,
  XcLM_Client_Convert_Com_To_Client_Descriptor_List,
  XcLM_Client_Convert_Object_To_Com,
  XcLM_Client_Convert_Com_To_Object,
  XcLM_Client_Convert_Object_List_To_Com,
  XcLM_Client_Convert_Com_To_Object_List,
  XcLM_Client_Convert_Object_List_To_Object_Descriptor_List,
  XcLM_Client_List_Object_Descriptor,
  XcLM_Client_Test_Object_Descriptor_List,
  XcLM_Client_Destroy_Object_Descriptor_List,
  XcLM_Client_Remove_Object_Descriptor_List,
  XcLM_Client_Destroy_Element_Object_Descriptor_List,
  XcLM_Client_Remove_Element_Object_Descriptor_List,
  XcLM_Client_List_Client_Descriptor,
  XcLM_Client_Test_Client_Descriptor_List,
  XcLM_Client_Destroy_Client_Descriptor_List,
  XcLM_Client_Remove_Client_Descriptor_List,
  XcLM_Client_Destroy_Element_Client_Descriptor_List,
  XcLM_Client_Remove_Element_Client_Descriptor_List,
  XcLM_Client_Allocate_ComData,
  XcLM_Client_Destroy_ComData,
  XcLM_Client_Copy_ComData,
  XcLM_Client_List_Object,
  XcLM_Client_List_Object,
};

/*--------------------------------------------------------------------------*/
/*---------    LINKMANAGER CLIENT : CREATE, COPY, DESTROY ------------------*/
/*--------------------------------------------------------------------------*/

void *cons_LM_Client(Name, OwnerName, GroupName, Low_Client, Attrib, Class, Delay)
char				*Name;
char				*OwnerName;
char				*GroupName;
c_Client			*Low_Client;
XcLM_Object_AttributesMask	Attrib;
XcLM_ClassMask			Class;
XcLM_Refresh_Delay		Delay;
{
  c_LM_Client	*ObjTmp;
  int		Ii;

  if ((ObjTmp=(c_LM_Client *) Xc_malloc("client ", sizeof (c_LM_Client)))==NULL)
    {
      Xc_ERROR(("Can't build this object: memory allocation error"));
      exit(1);
    }
  if ((CLIENT_DESCRIPTOR(ObjTmp)=(XcLM_Client_Descriptor *) Xc_malloc
       ("clidesc", sizeof (XcLM_Client_Descriptor)))==NULL)
    {
      Xc_ERROR(("Can't build this object: memory allocation error"));
      exit(1);
    }
  if ((ObjTmp->Descriptor->Name = (char *) Xc_malloc
       ("clidesc name", (strlen(Name)+1)*sizeof(char))) == NULL)
    {
      Xc_ERROR(("Can't build object : memory allocation error"));
      exit(1);
    }
  if ((ObjTmp->Descriptor->Owner_Name = (char *) Xc_malloc
       ("clidesc owner name", (strlen(OwnerName)+1)*sizeof(char))) == NULL)
    {
      Xc_ERROR(("Can't build object : memory allocation error"));
      exit(1);
    }
  if ((ObjTmp->Descriptor->Group_Name = (char *) Xc_malloc
       ("clidesc group name",(strlen(GroupName)+1)*sizeof(char))) == NULL)
    {
      Xc_ERROR(("Can't build object : memory allocation error"));
      exit(1);
    }
  
  strcpy(ObjTmp->Descriptor->Name, Name);
  strcpy(ObjTmp->Descriptor->Owner_Name, OwnerName);
  strcpy(ObjTmp->Descriptor->Group_Name, GroupName);
  
  ObjTmp->Descriptor->AttributesMask = Attrib;
  ObjTmp->Descriptor->ClassMask = Class;
  ObjTmp->Descriptor->Refresh_Delay = Delay;

  ObjTmp->f = &fc_LM_Client;

  /* now, initializing objects lists (public list & private list) */
  
  Ii = XcLM_Client_Initialize_Object_List(&(ObjTmp->Private_Object_List));
  Ii |= XcLM_Client_Initialize_Object_List(&(ObjTmp->Public_Object_List));
  Ii |= XcLM_Client_Initialize_Object_List(&(ObjTmp->Trash_Object_List));
  Ii |= XcLM_Client_Initialize_Client_Descriptor_List(&(ObjTmp->Client_Descriptor_List));

  Ii |= XcLM_Client_Initialize_Object_List(&(ObjTmp->Receive_Object_List));
  Ii |= XcLM_Client_Initialize_Object_Descriptor_List(&(ObjTmp->Receive_Object_Descriptor_List));
  Ii |= XcLM_Client_Initialize_Client_Descriptor_List(&(ObjTmp->Receive_Client_Descriptor_List));
  
  Ii |= XcLM_Client_Initialize_Object_List(&(ObjTmp->Send_Object_List));
  Ii |= XcLM_Client_Initialize_Object_Descriptor_List(&(ObjTmp->Send_Object_Descriptor_List));
  Ii |= XcLM_Client_Initialize_Client_Descriptor_List(&(ObjTmp->Send_Client_Descriptor_List));
  

  ObjTmp->Receive_ComData = NULL;
  ObjTmp->Send_ComData = NULL;
  ObjTmp->Receive_Object = NULL;
  ObjTmp->Receive_Object_Descriptor = NULL;
  ObjTmp->Receive_Client_Descriptor = NULL;
  ObjTmp->Send_Object = NULL;
  ObjTmp->Send_Object_Descriptor = NULL;
  ObjTmp->Send_Client_Descriptor = NULL;
  ObjTmp->Private_Object_Number = 0;
  ObjTmp->Public_Object_Number = 0;
  ObjTmp->Trash_Object_Quota = 2;	/* max 5 objects in trash */
  ObjTmp->Sender = NULL;

  ObjTmp->Low_Client = Low_Client;


  Xc_TRACE(("Object LinkManagerClient build"));

  if (Low_Client != NULL)
    { 
      Xc_TRACE(("waiting for low level client list ..."));
      
      while (Low_Client->ListClient == NULL);

      Xc_TRACE(("waiting for low_level id"));
      
      while (Low_Client->Ident == 0);
      
      ObjTmp->Descriptor->Ident = (int) (Low_Client->Ident);

      Xc_TRACE(("registering to other clients ..."));
      XcLM_Client_Register_To_Others(ObjTmp);

      /* les deux boucles precedentes servent a attendre ke le serveur ait
	 donne les information aux client (son ident et celui des autres)
	 */
    }
  else
    {
    ObjTmp->Descriptor->Ident = 0;
  }

  return (ObjTmp);
}



/* ----------------  client  destruction  ---------------   */

void desXcLM_Client(ObjectToDelete)
c_LM_Client 	*ObjectToDelete;
{

     Xc_TRACE(("Xc_Freeing private object list ..."));


  XcLM_Client_Destroy_Object_List(ObjectToDelete, 
				  &(ObjectToDelete->Private_Object_List));
  XcLM_Client_Destroy_Object_List(ObjectToDelete, 
				  &(ObjectToDelete->Trash_Object_List));
  XcLM_Client_Destroy_Object_List(ObjectToDelete,
				  &(ObjectToDelete->Public_Object_List));

  XcLM_Client_Destroy_Client_Descriptor_List
    (&(ObjectToDelete->Client_Descriptor_List));

  Xc_TRACE(("Xc_Freeing all communication lists (Receive and Send lists)"));

  XcLM_Client_Destroy_Object_List(ObjectToDelete,
				  &(ObjectToDelete->Receive_Object_List));
  XcLM_Client_Destroy_Object_List(ObjectToDelete,
				  &(ObjectToDelete->Send_Object_List));

  XcLM_Client_Destroy_Object_Descriptor_List
    ( &(ObjectToDelete->Receive_Object_Descriptor_List));
  XcLM_Client_Destroy_Object_Descriptor_List
    ( &(ObjectToDelete->Send_Object_Descriptor_List));

  XcLM_Client_Destroy_Client_Descriptor_List
    (&(ObjectToDelete->Receive_Client_Descriptor_List));

  XcLM_Client_Destroy_Client_Descriptor_List
    ( &(ObjectToDelete->Send_Client_Descriptor_List));

  XcLM_Client_Destroy_Client_Descriptor( &(ObjectToDelete->Sender));

  /* Xc_freeing datas in object itself */

  Xc_TRACE(("Xc_freeing comdatas"));

  if (ObjectToDelete->Receive_ComData != NULL)
    XcLM_Client_Destroy_ComData(&(ObjectToDelete->Receive_ComData));
  
  if (ObjectToDelete->Send_ComData != NULL)
    XcLM_Client_Destroy_ComData(&(ObjectToDelete->Send_ComData));

  Xc_free(CLIENT_DESCRIPTOR(ObjectToDelete)->Name);
  Xc_free(CLIENT_DESCRIPTOR(ObjectToDelete)->Owner_Name);
  Xc_free(CLIENT_DESCRIPTOR(ObjectToDelete)->Group_Name);
  Xc_free(CLIENT_DESCRIPTOR(ObjectToDelete));
  Xc_free(ObjectToDelete);

  Xc_TRACE(("Object LinkManagerClient deleted\n"));
}


void *copy_LM_Client(ObjectToCopy)
c_LM_Client 	*ObjectToCopy;
{
  c_LM_Client *ObjTmp;

  if ((ObjTmp=(c_LM_Client *) Xc_malloc("client in copy", sizeof (c_LM_Client)))==NULL)
    {
      Xc_FATAL(("Can't copy this object: memory allocation error"));
    }
  memcpy(ObjTmp,ObjectToCopy,sizeof (c_LM_Client));

  if ((CLIENT_DESCRIPTOR(ObjTmp)=(XcLM_Client_Descriptor *) Xc_malloc("clidesc in copy", sizeof (XcLM_Client_Descriptor)))==NULL)
    {
      Xc_FATAL(("Can't copy this object: memory allocation error"));
    }
  memcpy(CLIENT_DESCRIPTOR(ObjTmp),CLIENT_DESCRIPTOR(ObjectToCopy),sizeof (XcLM_Client_Descriptor));

  Xc_TRACE(("Object LinkManagerClient copied"));

  return (ObjTmp);
}

/*----------------------------------------------------------------------------------------*/
/*-------------------------- REGISTER MY CLIENT ... --------------------------------------*/
/*----------------------------------------------------------------------------------------*/

int 	XcLM_Client_Register_To_Others(Client)
c_LM_Client	*Client;
{
  XcLM_Client_Descriptor	*TempDesc;
  XcLM_ComData			*Com;
  int				Ii;
  
  if ( (Ii = XcLM_Client_Allocate_Client_Descriptor(&TempDesc)) < 0)
    {
      Xc_WARNING(("can't say i am here ... allocation error ..."));
      return (Ii);
    }
  TempDesc->Ident = 0;
  /* Ident = 0 so object will be send to everyone ... */

  Xc_TRACE(("i convert my descriptor for other clients , my ident is : %d ...", Client->Descriptor->Ident));
  Com = NULL;
  IF_ERROR(XcLM_Client_Convert_Client_Descriptor_To_Com(&Com, Client->Descriptor));

  Xc_TRACE(("I send my descriptor to all other clients ..."));
  IF_ERROR(XcLM_Client_Send(Client, TempDesc, XcLM_RECEIVE_CLIENT_DESCRIPTOR, Com));

  Xc_free(TempDesc);
  XcLM_Client_Destroy_ComData(&Com);
  return (0);
}


/*---------------------------------------------------------------------------------------*/
/*--------------------- COMMUNICATION : PRIVATE SEND ... --------------------------------*/
/*---------------------------------------------------------------------------------------*/

int 	XcLM_Client_Private_Send(Client, Destination, Com)
c_LM_Client		*Client;
XcLM_Client_Descriptor	*Destination;
XcLM_ComData		*Com;
{
  Xc_TRACE(("now  sending datas !!!"));
  
  Xc_TRACE(("longueur %d, destinataire = %d, my_id = %d",Com->Length,
	    Destination->Ident, Client->Low_Client->Ident));
  
  F(Client->Low_Client).SendMsg(Client->Low_Client, 
				(long) Com->Length, (char *) Com->Data,
				(char) Destination->Ident);

  return (0);
}

int 	XcLM_Client_Private_Send_Low(Client, Destination, Com)
c_LM_Client		*Client;
int 			Destination;
XcLM_ComData		*Com;
{
  Xc_TRACE(("now  sending datas !!! (private low level)"));

  Xc_TRACE(("longueur %d",Com->Length));

  F(Client->Low_Client).SendMsg(Client->Low_Client, (long) Com->Length, (char *) (Com->Data),
				(char) Destination);
  return (0);
}

/*-----------------------------------------------------------------------------------*/
/*  COMMUNICATION : REGISTER RECEIVED DATAS, check autorisations, names ...          */
/*-----------------------------------------------------------------------------------*/

int	XcLM_Client_Register_Receive_Object_List(client)
c_LM_Client		*client;
{
  XcLM_Object_List	*List;
  XcLM_Object		*Object_I;
  int			Ii;

  Xc_TRACE(("analysing received object list ..."));

  List = client->Receive_Object_List;

  while (PREV(List) != NULL)
    List = PREV(List);
  
  Xc_TRACE(("pointer liste publique = %lp", client->Public_Object_List));

  while (NEXT(List) != NULL)
    {
      Object_I = List->Object;
      
      /* ici on fait un unset de la classe private de facon 'hard'
	 cad sans passer par les fonctions prevues pour ...
	 c'est pour forcer l'objet recu a etre public
	 */

      Object_I->Descriptor->ClassMask &= ~XcLM_PRIVATE;
      Object_I->Descriptor->ClassMask |= XcLM_PUBLIC;

      if (XcLM_Client_Test_Object_List(&(PUBLIC_LIST(client)), Object_I->Descriptor) == -1)
	{
	  Xc_TRACE(("object is new here, registering it ..."));
	  if ( (Ii = XcLM_Client_Auto_Register_Object(client, Object_I)) < 0)
	    return (Ii);
	}
      else
	{
	  Xc_TRACE(("object name = '%s'", Object_I->Descriptor->Name));
	  Xc_TRACE(("object already exist, tell it to '%s'",client->Sender->Name));
	}
      List = NEXT(List);
    }
  XcLM_Client_Remove_Object_List(client, &(client->Receive_Object_List));
  Xc_TRACE(("listing apres remove objects ..."));
  XcLM_Client_List_Object(client->Receive_Object_List, XcLM_ALL_CLASS);
  return (0);
}


int	XcLM_Client_Register_Receive_Client_Descriptor_List(client)
c_LM_Client		*client;
{
  XcLM_Client_Descriptor_List	*List;
  int				Ii;

  Xc_TRACE(("analysing received client list ..."));

  List = client->Receive_Client_Descriptor_List;

  while (PREV(List) != NULL)
    List = PREV(List);
  
  while (NEXT(List) != NULL)
    {
      {
	client->Receive_Client_Descriptor = List->Descriptor;
	IF_ERROR(XcLM_Client_Register_Receive_Client_Descriptor(client));
	List = NEXT(List);
      }
    }
  Xc_TRACE(("remove receive client descriptor list ..."));

  XcLM_Client_Remove_Client_Descriptor_List(&(client->Receive_Client_Descriptor_List));
  return (0);
}

int	XcLM_Client_Register_Receive_Client_Descriptor(client)
c_LM_Client	*client;
{
  int		Ii;
  char		Cflag;
  XcLM_ComData	*Com;

  Cflag = 0;
  if (XcLM_Client_Test_Client_Descriptor_List(&(client->Client_Descriptor_List), client->Receive_Client_Descriptor) == 0)
    {
      Xc_TRACE(("client already registered, modifying it"));
      IF_ERROR( XcLM_Client_Destroy_Element_Client_Descriptor_List(&(client->Client_Descriptor_List)) );
    }
  else
    {
      Cflag = 1;
      Xc_TRACE(("client '%s' not registered, registering it ...",client->Receive_Client_Descriptor->Name));
    }
  IF_ERROR( XcLM_Client_Register_Client_Descriptor(&(client->Client_Descriptor_List), client->Receive_Client_Descriptor));

  Xc_TRACE(("receive client descriptor was successfull !!!"));
  client->Receive_Client_Descriptor = NULL;

  if (Cflag != 0)
    {
      Xc_TRACE(("i send my descriptor to : %d ...", client->Sender->Ident));
      Com = NULL;
      IF_ERROR(XcLM_Client_Convert_Client_Descriptor_To_Com(&Com, client->Descriptor));
      if ( (Ii = XcLM_Client_Send(client, client->Sender, XcLM_RECEIVE_CLIENT_DESCRIPTOR, Com)) < 0)
	{
	  XcLM_Client_Destroy_ComData(&Com);
	  return (Ii);
	}
      XcLM_Client_Destroy_ComData(&Com);
    }
  return (0);
}

/*------------------------------------------------------------------------------------*/
/*-----------   COMMUNICATION FUNCTION : SEND DATA, RECEIVE DATA   -------------------*/
/*------------------------------------------------------------------------------------*/

int XcLM_Client_Receive(client, com)
c_LM_Client		*client;
XcLM_ComData		*com;
{
  int				Ii;
  int				Code;
 
  com->Offset = 0;

  Code = (int) (com->Data)[com->Offset++];

  if (client->Sender != NULL)
    XcLM_Client_Destroy_Client_Descriptor(&(client->Sender));

  client->Sender = NULL;

  if ( (Ii = XcLM_Client_Convert_Com_To_Client_Descriptor(com, &(client->Sender))) < 0)
    return (Ii);

  Xc_TRACE(("sender name = '%s'", client->Sender->Name));

  switch (Code)
    {
    case XcLM_RECEIVE_CLIENT_DESCRIPTOR_LIST :
      {
	Xc_TRACE(("receiving client descriptor list ..."));
	  
	if ( (Ii = XcLM_Client_Convert_Com_To_Client_Descriptor_List(com, &(client->Receive_Client_Descriptor_List))) < 0)
	  {
	    Xc_TRACE(("error code = %d",Ii));
	    return (Ii);
	  }
	XcLM_Client_List_Client_Descriptor(client->Receive_Client_Descriptor_List);

	IF_ERROR(XcLM_Client_Register_Receive_Client_Descriptor_List(client));

	XcLM_Client_List_Client_Descriptor(client->Client_Descriptor_List, XcLM_ALL_CLASS);

	break;
      }

    case XcLM_RECEIVE_CLIENT_DESCRIPTOR :
      {
	Xc_TRACE(("receiving client descriptor ..."));
	  
	if ( (Ii = XcLM_Client_Convert_Com_To_Client_Descriptor(com, &(client->Receive_Client_Descriptor))) < 0)
	  {
	    Xc_TRACE(("error code = %d",Ii));
	    return (Ii);
	  }
	IF_ERROR(XcLM_Client_Register_Receive_Client_Descriptor(client));
	
	
	break;
      }

    case XcLM_RECEIVE_OBJECT_DESCRIPTOR_LIST :
      {
	Xc_TRACE(("receiving Object descriptor list ..."));
	  
	if ( (Ii = XcLM_Client_Convert_Com_To_Object_Descriptor_List(com, &(client->Receive_Object_Descriptor_List))) < 0)
	  {
	    Xc_TRACE(("error code = %d",Ii));
	    return (Ii);
	  }
/*
	Xc_TRACE(("Listing of received object descriptors ..."));
	XcLM_Client_List_Object_Descriptor(client->Receive_Object_Descriptor_List, XcLM_ALL_CLASS);
*/
	break;
      }

    case XcLM_RECEIVE_OBJECT_LIST :
      {
	Xc_TRACE(("receiving Object list ..."));
	if ( (Ii = XcLM_Client_Convert_Com_To_Object_List(com, &(client->Receive_Object_List))) < 0)
	  {
	    Xc_TRACE(("error code = %d",Ii));
	    return (Ii);
	  }
	if ( (Ii = XcLM_Client_Register_Receive_Object_List(client)) < 0)
	  return (Ii);

	break;
      }

	
    }
  return (0);
}



/*------------------------------------------------------------------------------------*/
/*----------------           REFRESH DELAY : SET, GET          -----------------------*/
/*------------------------------------------------------------------------------------*/

int XcLM_Private_Get_Refresh_Delay(client, delay)
c_LM_Client   		*client;
XcLM_Refresh_Delay	*delay;
{
  /* fonction a mettre en define */
  Xc_TRACE(("Get client refresh delay ..."));

  *delay = CLIENT_DESCRIPTOR(client)->Refresh_Delay;

  return (0);
}


int XcLM_Private_Set_Refresh_Delay(client,delay)
c_LM_Client 		*client;
XcLM_Refresh_Delay 	delay;
{
  /* fonction a mettre en define */
  Xc_TRACE(("Set client refresh delay ..."));

  CLIENT_DESCRIPTOR(client)->Refresh_Delay = delay;

  return (0);
}



/*------------------------------------------------------------------------------------*/
/*--------------          DIVERSES FONCTIONS NON CLASSEES         --------------------*/
/*------------------------------------------------------------------------------------*/


int XcLM_Show_Data(object_desc)
XcLM_Object_Descriptor	*object_desc;
{

  Xc_TRACE(("... Sorry, no implementation yet"));
  return (0);
}


/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------*/


int XcLM_Client_Send(client, destination, code, com)
c_LM_Client			*client;
XcLM_Client_Descriptor		*destination;
XcLM_Client_Function_Codes	code;
XcLM_ComData			*com;
{
  int			Ii;
  int			Offset;
  XcLM_ComData		*com2;
  char			*tempdata;
  listclient		*templist;

  /* toujours envoyer son descripteur avec les donnees, sinon le destinataire ne
     sait pas a qui repondre ...
     */
  if (destination->Name != NULL)
    {
      Xc_TRACE(("sending to '%s'",destination->Name));
    }
  if (com == NULL)
    return (-1);
  
  com2 = NULL;
  Ii = XcLM_Client_Convert_Client_Descriptor_To_Com(&com2, client->Descriptor);

  tempdata = com->Data;
  if (( com->Data = (char *) Xc_malloc("comdata in send", com->Length + com2->Length + XcLM_COM_HEADER_LENGTH_FUNCTION_CODE)) == NULL)
    {
      Xc_WARNING((" malloc error for final ComData in obj_desc_list conversion"));
      return (XcLM_MALLOC_ERROR);
    }
  Offset = 0;
  (com->Data)[Offset++] = (char) code;

  memcpy(com->Data + Offset, com2->Data, com2->Length);
  Offset += com2->Length;

  memcpy(com->Data + Offset, tempdata, com->Length);

  com->Length += XcLM_COM_HEADER_LENGTH_FUNCTION_CODE + com2->Length;
  com->Offset = com->Length;
  
  XcLM_Client_Destroy_ComData(&com2);
  Xc_free(tempdata);

  if (destination->Ident != 0)
    XcLM_Client_Private_Send(client, destination, com);
  else
    {
      if (client->Low_Client == NULL)
	{
	  Xc_WARNING(("send to ??? there is no server !!!"));
	  return (XcLM_NO_SERVER_ERROR);
	}
      templist = client->Low_Client->ListClient;
/*
      Xc_TRACE(("pointer liste temporaire = %lp, my_ident = %d",templist, client->Descriptor->Ident));
*/
      while ( templist != NULL )
	{
	  if (templist->Ident != client->Descriptor->Ident)
	    {
	      /* i don't send objects to myself !!! */
	      Xc_TRACE(("sending to ident = %d, login = %s", templist->Ident, templist->Login));
	      IF_ERROR(XcLM_Client_Private_Send_Low(client, templist->Ident, com));
	    }
	  templist = templist->NextClient;
	}
    }
  return (0);
}

