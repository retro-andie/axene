/*
** LMObject.c for Xclamation in LinkManager/
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
** Started on  Mon Jul 17 19:10:18 1995 Marc Blet
** Last update Sat Oct 26 17:09:17 1996 Emmanuel Paris
*/

#include "ObjStd.h"
#include "LinkManager.h"

/*----------------------------------------------------------------------*/
/*--  LINKMANAGER OBJECT : ALLOCATE, BUILD, DESTROY, UPLOAD, DOWNLOAD  -*/
/*----------------------------------------------------------------------*/


int 	XcLM_Client_Allocate_Object(object)
XcLM_Object	**object;
{
  if ((*object = (XcLM_Object *) Xc_malloc("obj ",sizeof(XcLM_Object))) == NULL)
    {
      Xc_WARNING(("Object allocation error"));
      return(XcLM_MALLOC_ERROR);
    }

  (*object)->Descriptor 		= 	NULL;
  (*object)->Data 			= 	NULL;
  (*object)->Length 			= 	0;
  (*object)->Pointer_In_List 		= 	NULL;
  (*object)->Data_Ptr			=	&((*object)->Data);
  (*object)->Send_Zone_List		=	NULL;
  (*object)->Data_Change_Callback 	= 	NULL;
  (*object)->Desc_Change_Callback 	= 	NULL;
  (*object)->Object_Deleted_Callback 	= 	NULL;
  (*object)->Reference_Number 		=       0;
  return (0);
}


int 	XcLM_Client_Allocate_Object_Zone_List(Zone)
XcLM_Object_Zone_List	**Zone;
{
  if ((*Zone = (XcLM_Object_Zone_List *) Xc_malloc("zone list", sizeof(XcLM_Object_Zone_List))) == NULL)
    {
      Xc_WARNING(("Memory allocation error for 'object zone list'"));
      return (XcLM_MALLOC_ERROR);
    }
  (*Zone)->Prev 	= 	NULL;
  (*Zone)->Next 	= 	NULL;
  (*Zone)->Start	= 	0;
  (*Zone)->Length	=	0;
  
  return (0);
}



int	XcLM_Client_Build_Object(ObjTmp, Client, Name, Attributes, Class, Length, Data, Type)
XcLM_Object			**ObjTmp;
c_LM_Client			*Client;
char				*Name;
XcLM_Object_AttributesMask	Attributes;
XcLM_ClassMask			Class;
XcLM_Object_Length		Length;
char				*Data;
XcLM_Object_Type		Type;
{
  *ObjTmp = NULL;

  if (Name == NULL)
    return (-1);
  if (Client == NULL)
    return (XcLM_NEED_CLIENT_ERROR);
    

  if ((*ObjTmp = (XcLM_Object *) Xc_malloc("obj ",sizeof(XcLM_Object)))==NULL)
    {
      Xc_WARNING(("Object allocation error"));
      *ObjTmp = NULL;
      return(XcLM_MALLOC_ERROR);
    }

  if ((OBJECT_DESCRIPTOR((*ObjTmp)) = (XcLM_Object_Descriptor *) Xc_malloc("objdesc ",sizeof(XcLM_Object_Descriptor))) == NULL)
    {
      Xc_TRACE(("Malloc Error, no enough memory ..."));
      Xc_free(*ObjTmp);
      *ObjTmp = NULL;
      return (XcLM_MALLOC_ERROR);
    }

  if ( ((*ObjTmp)->Data_Change_Callback = (XcLM_Callback *) Xc_malloc("Callback data change", sizeof(XcLM_Callback))) == NULL)
    {
      Xc_WARNING(("Malloc error for Data_change callback"));
      Xc_free(*ObjTmp);
      *ObjTmp = NULL;
      return (XcLM_MALLOC_ERROR);
    }
  (*ObjTmp)->Data_Change_Callback->LMCallback = NULL;
  (*ObjTmp)->Data_Change_Callback->Name = NULL;
  (*ObjTmp)->Data_Change_Callback->Number = 0;

  if ( ((*ObjTmp)->Object_Deleted_Callback = (XcLM_Callback *) Xc_malloc("Callback object deleted", sizeof(XcLM_Callback))) == NULL)
    {
      Xc_WARNING(("Malloc error for Object_Deleted callback"));
      Xc_free((*ObjTmp)->Data_Change_Callback);
      Xc_free(*ObjTmp);
      *ObjTmp = NULL;
      return (XcLM_MALLOC_ERROR);
    }
  (*ObjTmp)->Object_Deleted_Callback->LMCallback = NULL;
  (*ObjTmp)->Object_Deleted_Callback->Name = NULL;
  (*ObjTmp)->Object_Deleted_Callback->Number = 0;

  if ( ((*ObjTmp)->Desc_Change_Callback = (XcLM_Callback *) Xc_malloc("Callback desc change", sizeof(XcLM_Callback))) == NULL)
    {
      Xc_WARNING(("Malloc error for Desc_change callback"));
      Xc_free((*ObjTmp)->Data_Change_Callback);
      Xc_free((*ObjTmp)->Object_Deleted_Callback);
      Xc_free(*ObjTmp);
      *ObjTmp = NULL;
      return (XcLM_MALLOC_ERROR);
    }
  (*ObjTmp)->Desc_Change_Callback->LMCallback = NULL;
  (*ObjTmp)->Desc_Change_Callback->Name = NULL;
  (*ObjTmp)->Desc_Change_Callback->Number = 0;


  if (((*ObjTmp)->Descriptor->Name = (char *) Xc_malloc("objdesc name", (strlen(Name)+1)*sizeof(char))) == NULL)
    {
      Xc_TRACE(("Malloc Error, no enough memory ..."));
      Xc_free((*ObjTmp)->Descriptor);
      Xc_free((*ObjTmp)->Data_Change_Callback);
      Xc_free((*ObjTmp)->Object_Deleted_Callback);
      Xc_free((*ObjTmp)->Desc_Change_Callback);
      Xc_free(*ObjTmp);
      *ObjTmp = NULL;
      return (XcLM_MALLOC_ERROR);
    }

  if (((*ObjTmp)->Descriptor->Owner_Name = (char *) Xc_malloc("objdesc owner", (strlen(Client->Descriptor->Name)+1)*sizeof(char))) == NULL)
    {
      Xc_TRACE(("Malloc Error, no enough memory ..."));
      Xc_free((*ObjTmp)->Descriptor->Name);
      Xc_free((*ObjTmp)->Descriptor);
      Xc_free((*ObjTmp)->Data_Change_Callback);
      Xc_free((*ObjTmp)->Object_Deleted_Callback);
      Xc_free((*ObjTmp)->Desc_Change_Callback);
      Xc_free(*ObjTmp);
      *ObjTmp = NULL;
      return (XcLM_MALLOC_ERROR);
    }
  
  strcpy((*ObjTmp)->Descriptor->Owner_Name, Client->Descriptor->Name);
  strcpy((*ObjTmp)->Descriptor->Name, Name);


  (*ObjTmp)->Length 			= 	Length;
  (*ObjTmp)->Descriptor->Owner_Ident	=	Client->Descriptor->Ident;
  (*ObjTmp)->Descriptor->ClassMask 	= 	Class;
  (*ObjTmp)->Descriptor->AttributesMask = 	Attributes;
  (*ObjTmp)->Descriptor->Private 	= 	(void *) NULL;
  (*ObjTmp)->Descriptor->Type		= 	Type;

  if (Length > 0)
    {
      if (((*ObjTmp)->Data = (char *) Xc_malloc("obj data", Length*sizeof(char))) == NULL)
	{
	  Xc_TRACE(("Malloc Error, no enough memory ..."));
	  Xc_free((*ObjTmp)->Descriptor->Owner_Name);
	  Xc_free((*ObjTmp)->Descriptor->Name);
	  Xc_free((*ObjTmp)->Descriptor);
	  Xc_free((*ObjTmp)->Data_Change_Callback);
	  Xc_free((*ObjTmp)->Object_Deleted_Callback);
	  Xc_free((*ObjTmp)->Desc_Change_Callback);
	  Xc_free(*ObjTmp);
	  *ObjTmp = NULL;
	  return (XcLM_MALLOC_ERROR);
	}
      memcpy((void *)(*ObjTmp)->Data, (void *)Data, Length);
    }
  else
    {
      Xc_TRACE(("Managed Object ..."));
      if (Class && XcLM_LOCAL_MANAGED)
	{
	  Xc_TRACE(("Local Management"));
	  (*ObjTmp)->Data = Data;
	  (*ObjTmp)->Length = -1;
	}
    }
  
  (*ObjTmp)->Reference_Number = 0;
  (*ObjTmp)->Type = Type;

  XcLM_Client_Auto_Register_Object(Client, *ObjTmp);
  
  return (0);
}
  


int 	XcLM_Client_Copy_Object(Dest, Source)
XcLM_Object    	**Dest;
XcLM_Object	*Source;
{
  int	Ii;

  Xc_TRACE(("Copy Object ..."));
  /* le but du jeu est de faire un build_object avec comme params les champs de 'Source' */

  if (*Dest != NULL)
    return (-1);

  Ii = XcLM_Client_Build_Object(&(*Dest), NULL, Source->Descriptor->Name, Source->Descriptor->Owner_Name,
				Source->Descriptor->Owner_Ident, 
				Source->Descriptor->AttributesMask, Source->Descriptor->ClassMask, Source->Length, Source->Data);

  return (Ii);
}

int 	XcLM_Client_Destroy_Object(client, object)
c_LM_Client	*client;
XcLM_Object	**object;
{
  XcLM_Object_List	*List;
  XcLM_Object_List	*ListP;
  XcLM_Object_List	*ListN;

  Xc_TRACE(("Xc_Freeing object '%s'...",(*object)->Descriptor->Name));

  /*
     Xc_TRACE(("object name is ??? -> %s",((XcLM_Object_List *)((*object)->Pointer_In_List))->Object->Descriptor->Name));
     */

  /* setting up list next and previous */

  List = (XcLM_Object_List *)((*object)->Pointer_In_List);
  ListP = List->Prev;
  ListN = List->Next;
  
  (ListN)->Prev = ListP;

  if (ListP != NULL)
    ListP->Next = ListN;

  if ((*object)->Descriptor->ClassMask & XcLM_PRIVATE)
    {
    
      client->Private_Object_List = ListN;

      while (client->Private_Object_List->Prev != NULL)
	client->Private_Object_List = client->Private_Object_List->Prev;

      client->Private_Object_Number--;
    }
  if ((*object)->Descriptor->ClassMask & XcLM_PUBLIC)  
    {
      client->Public_Object_List = ListN;

      while (client->Public_Object_List->Prev != NULL)
	client->Public_Object_List = client->Public_Object_List->Prev;

      client->Public_Object_Number--;      
    }
 
  if ((*object)->Descriptor->ClassMask & XcLM_TRASH)  
    {
      client->Trash_Object_List = ListN;

      while (client->Trash_Object_List->Prev != NULL)
	client->Trash_Object_List = client->Trash_Object_List->Prev;

      client->Trash_Object_Number--;      
    }
 

  /* C'EST ICI QU'IL FAUT QUE J'APPELLE LE CALLBACK DE DESTRUCTION D'OBJET ...
   * ET IL FAUT PROPAGER L'EVENEMENT AUX AUTRES CLIENTS.
   *
   * C'est egalement ici qu'il faut que j'appelle le destructeur de l'objet
   * dans le cas de LOCAL_MANAGED et dans le cas normal egalement
   * si on ne veut pas ke les odnnees soient detruites dans le cas du
   * LOCAL_MANAGED, il ne faut pas enregistrer de callback
   */
  

  XcLM_Client_Destroy_Object_Callbacks(*object);
  Xc_free((*object)->Data_Change_Callback);
  Xc_free((*object)->Desc_Change_Callback);
  Xc_free((*object)->Object_Deleted_Callback);

  if ((*object)->Data != NULL)
    {
      if (!((*object)->Descriptor->ClassMask & XcLM_LOCAL_MANAGED))
	{
	  Xc_free((*object)->Data);
	  Xc_TRACE(("Freeing data, non managed object"));
	}
#ifndef NTRACE
      else
	Xc_TRACE(("LOCAL_MANAGED object : data were freed by callback ..."));
#endif
    }


  Xc_free((*object)->Descriptor->Name);
  Xc_free((*object)->Descriptor->Owner_Name);
  Xc_free((*object)->Descriptor);

  Xc_free(*object);

  *object = NULL;

  Xc_free(List); 

  return (0);
}


int 	XcLM_Client_Remove_Object(client, object)
c_LM_Client	*client;
XcLM_Object	**object;
{
  XcLM_Object_List	*List;
  XcLM_Object_List	*ListP;
  XcLM_Object_List	*ListN;

  Xc_TRACE(("removing object '%s'...",(*object)->Descriptor->Name));

  /*
     Xc_TRACE(("object name is ??? -> %s",((XcLM_Object_List *)((*object)->Pointer_In_List))->Object->Descriptor->Name));
     */

  /* setting up list next and previous */

  List = (XcLM_Object_List *)((*object)->Pointer_In_List);
  ListP = List->Prev;
  ListN = List->Next;
  
  (ListN)->Prev = ListP;

  if (ListP != NULL)
    ListP->Next = ListN;

  if ((*object)->Descriptor->ClassMask & XcLM_PRIVATE)
    {
    
      client->Private_Object_List = ListN;

      while (client->Private_Object_List->Prev != NULL)
	client->Private_Object_List = client->Private_Object_List->Prev;

      client->Private_Object_Number--;
    }
  if ((*object)->Descriptor->ClassMask & XcLM_PUBLIC)  
    {
      client->Public_Object_List = ListN;

      while (client->Public_Object_List->Prev != NULL)
	client->Public_Object_List = client->Public_Object_List->Prev;

      client->Public_Object_Number--;      
    }
  if ((*object)->Descriptor->ClassMask & XcLM_TRASH)  
    {
      client->Trash_Object_List = ListN;

      while (client->Trash_Object_List->Prev != NULL)
	client->Trash_Object_List = client->Trash_Object_List->Prev;

      client->Trash_Object_Number--;      
    }

  Xc_free(List); 

  (*object)->Pointer_In_List = NULL;
  return (0);
}


/* La fonction suivante prend un objet precedemment enlever d'une liste
 * par XcLM_Client_Remove_Object() pour le remettre dans une liste.
 * Cela permet de faire un move d'une liste a une autre
 * le parametre list_type dit dans la liste de quel type on veut mettre
 * l'objet ... cela peut-etre XcLM_TRASH, XCLM_PRIVATE ou XcLM_PUBLIC
 */

int 	XcLM_Client_Move_Object(client, object, listmask)
c_LM_Client		*client;
XcLM_Object		*object;
XcLM_Class		listmask;
{
  XcLM_Client_Remove_Object(client, &object);
  XcLM_Client_Unset_Object_Class
    (client->Descriptor, object->Descriptor, XcLM_PRIVATE);
  XcLM_Client_Unset_Object_Class
    (client->Descriptor, object->Descriptor, XcLM_PUBLIC);
  XcLM_Client_Unset_Object_Class
    (client->Descriptor, object->Descriptor, XcLM_TRASH);
  object->Descriptor->ClassMask |= listmask;
      
  XcLM_Client_Auto_Register_Object(client, object);
 
  return (0);
}


int 	XcLM_Client_Upload_Object(object)
XcLM_Object		*object;
{
  
  if (!(object->Descriptor->ClassMask & XcLM_PRIVATE))
    {
      Xc_TRACE(("Uploading an object -> private clipboard"));
      Xc_TRACE(("No mean : it is already in private clip"));
    }
  else
    {
      Xc_TRACE(("Uploading an object -> public clipboard"));
      Xc_TRACE(("... Sorry, no implementation yet"));
    }

  return (0);
}


int 	XcLM_Client_Download_Object(object)
XcLM_Object		*object;
{
  /* The descriptor should be ok, or the system may halt :-) */

  /* if the class is PRIVATE, then search the object and copy it to the 'object' pointer */

  if (!(object->Descriptor->ClassMask & XcLM_PRIVATE))
    {
      Xc_TRACE(("Download object from private clipboard ..."));
      Xc_TRACE(("Searching object descriptor in private list ..."));
      Xc_TRACE(("And then copying it ..."));
      
      Xc_TRACE(("No implementation yet"));
    }

  return (0);
}


/****************************************************************/

int XcLM_Client_Convert_Object_To_Com(com, obj)
XcLM_ComData	**com;
XcLM_Object	*obj;
{
  int 		Length;
  int 		Length2;
  int 		Length3;
  int		Ii;

  Xc_TRACE(("  Converting Object->Com"));

  Length2 = strlen(obj->Descriptor->Name) + 1; /* the '\0' is 1 char long */
  Length3 = strlen(obj->Descriptor->Owner_Name) + 1 ; /* idem */

  Length = XcLM_COM_HEADER_LENGTH_OBJECT_DESCRIPTOR; /* type field */
  Length += XcLM_COM_HEADER_LENGTH_INT + sizeof(int); /* type + legnth + data for 'owner_ident' */
  Length += XcLM_COM_HEADER_LENGTH_INT + sizeof(XcLM_Object_AttributesMask); /* type + legnth + data for 'AttributesMask' */
  Length += XcLM_COM_HEADER_LENGTH_INT + sizeof(XcLM_ClassMask); /* type + legnth + data for 'ClassMask' */
  Length += XcLM_COM_HEADER_LENGTH_BYTE + Length2; /* type + legnth + data for 'Name' */
  Length += XcLM_COM_HEADER_LENGTH_BYTE + Length3; /* type + legnth + data for 'Owner_Name' */
  
  /* maintenant on connait la taille du descripteur */

  Length += XcLM_COM_HEADER_LENGTH_OBJECT + XcLM_COM_HEADER_LENGTH_BYTE + obj->Length;

  if (*com == NULL)
    {
      XcLM_Client_Allocate_ComData(&(*com));

      if (((*com)->Data = (char *) Xc_malloc("com->data",Length * sizeof(char))) == NULL)
	{
	  Xc_WARNING(("Malloc error for conversion of object descriptor"));
	  return (XcLM_MALLOC_ERROR);
	}
    }

  ((*com)->Data)[(*com)->Offset++] = (char) XcLM_COM_OBJECT;

  if ( (Ii = XcLM_Client_Convert_Object_Descriptor_To_Com(&(*com), obj->Descriptor)) < 0)
    return (Ii);
  
  if ( (Ii = XcLM_Client_Convert_Byte_To_Com(&(*com), obj->Data, obj->Length)) < 0)
    return (Ii);

  /* everything should be ok here */
  return (0);
}



int 	XcLM_Client_Convert_Com_To_Object(com, obj)
XcLM_ComData		*com;
XcLM_Object		**obj;
{
  int		Offset;
  int 		Ii;

  Xc_TRACE(("  Converting Com->Object"));

  if (com == NULL)
    {
      *obj = NULL;
      return (-1);
    }

  if (*obj == NULL)
    {
      if ((Ii = XcLM_Client_Allocate_Object(&(*obj))) < 0)
	return (Ii);
    }
  else
    {
      if ((*obj)->Descriptor != NULL)
	XcLM_Client_Destroy_Object_Descriptor(&((*obj)->Descriptor));
    }

  Offset = com->Offset;

  if ((com->Data)[Offset++] != XcLM_COM_OBJECT)
    {
      Xc_WARNING(("         ralala, mvse structure"));
      *obj = NULL;
      return (-1);
    }

  com->Offset = Offset;

  if ( (Ii = XcLM_Client_Convert_Com_To_Object_Descriptor(com, &((*obj)->Descriptor))) < 0)
    return (Ii);

  if ( (Ii = XcLM_Client_Convert_Com_To_Byte(com, &((*obj)->Data), &((*obj)->Length))) < 0)
    return (Ii);

  return(0);
}



/*---------------------------------------------------------------------------*/
/*-------------------- GESTION DE COMMUNICATION -----------------------------*/
/*---------------------------------------------------------------------------*/

int	XcLM_Client_Unregister_Object(Client_I, Object_I)
c_LM_Client	*Client_I;
XcLM_Object	*Object_I;
{
  int				Ii;
  XcLM_ComData			*Com;
  XcLM_Client_Descriptor	*TempDesc;

  /* this function tell the object's owner that we don't want it at all 
   * this function remove the object of the clipboard object list 
   */
  
  Com = NULL;
  IF_ERROR(XcLM_Client_Convert_Object_Descriptor_To_Com(&Com, Object_I->Descriptor));

  IF_ERROR(XcLM_Client_Allocate_Object_Descriptor(&TempDesc));
  TempDesc->Ident = Object_I->Descriptor->Owner_Ident;
  if ( (Ii = XcLM_Client_Test_Client_Descriptor_List(&(Client_I->Client_Descriptor_List), TempDesc)) < 0)
    {
      Xc_WARNING(("client '%s' is dead !!!",Object_I->Descriptor->Owner_Name));
      return (XcLM_NO_SUCH_OBJECT_ERROR);
    }

  IF_ERROR(XcLM_Client_Send(Client_I, TempDesc, XcLM_REFUSE_OBJECT_DESCRIPTOR, Com));

  return (0);
}



int	XcLM_Client_Unregister_Object_List(Client_I, Object_List)
c_LM_Client		*Client_I;
XcLM_Object_List	*Object_List;
{
  int				Ii;

  /* this function tell the object's owner that we don't want it at all 
   * this function remove the object list of the clipboard object list 
   */
  

  while (PREV(Object_List) != NULL)
    Object_List = PREV(Object_List);
  Ii = 0;
  while (NEXT(Object_List) != NULL)
    {
      Ii |= XcLM_Client_Unregister_Object(Client_I, Object_List->Object);
      Object_List = NEXT(Object_List);
    }

  if (Ii != 0)
    return (Ii);
  return (0);
}


int 	XcLM_Client_Private_Destroy_Object_Callback(the_ptr)
XcLM_Callback		*the_ptr;
{
  Xc_TRACE(("Deleting object callback"));

  the_ptr->LMCallback = NULL;
  if (the_ptr->Name != NULL)
    Xc_free(the_ptr->Name);
  the_ptr->Number = 0;
  
  return (0);
}


int 	XcLM_Client_Destroy_Callback(the_object, the_event)
XcLM_Object		*the_object;
XcLM_Callback_Type	the_event;
{
  XcLM_Callback 	*private_ptr;

  switch(the_event)
    {
    case XcLM_DATA_CHANGE : private_ptr = the_object->Data_Change_Callback;
      break;
    case XcLM_DESC_CHANGE : private_ptr = the_object->Desc_Change_Callback;
      break;
    case XcLM_OBJECT_DELETED : private_ptr = the_object->Object_Deleted_Callback;
      break;
    default : return (XcLM_CALLBACK_EVENT_UNKNOWN);
    }
  
  return (XcLM_Client_Private_Destroy_Object_Callback(private_ptr));
}

int 	XcLM_Client_Destroy_Object_Callbacks(obj)
XcLM_Object		*obj;
{
  Xc_TRACE(("All callbacks of object '%s' will be removed",obj->Descriptor->Name));
  
    
  XcLM_Client_Private_Destroy_Object_Callback(obj->Data_Change_Callback);
  XcLM_Client_Private_Destroy_Object_Callback(obj->Desc_Change_Callback);
  XcLM_Client_Private_Destroy_Object_Callback(obj->Object_Deleted_Callback);

  return (0);
}

int 	XcLM_Client_Add_Object_Callback(the_object, the_event, name, func_ptr, user_ptr)
XcLM_Object		*the_object;
XcLM_Callback_Type	the_event;
char			*name;
void			(*func_ptr)();
void 			*user_ptr;
{
  XcLM_Callback		*private_ptr;
  
  Xc_TRACE(("Adding Callback to an object ..."));

  switch(the_event)
    {
    case XcLM_DATA_CHANGE : private_ptr = the_object->Data_Change_Callback;
      break;
    case XcLM_DESC_CHANGE : private_ptr = the_object->Desc_Change_Callback;
      break;
    case XcLM_OBJECT_DELETED : private_ptr = the_object->Object_Deleted_Callback;
      break;
    default : return (XcLM_CALLBACK_EVENT_UNKNOWN);
    }

  private_ptr->LMCallback = func_ptr;
  private_ptr->User_Data = user_ptr;

  if ( (private_ptr->Name = (char *) Xc_malloc("Callback name", (strlen(name)+1)*sizeof(char))) == NULL)
  {
    Xc_WARNING(("malloc error for callback name of object '%s'",the_object->Descriptor->Name));
    return (XcLM_MALLOC_ERROR);
  }
  strcpy(private_ptr->Name, name);
  private_ptr->Number = 1;

  return (0);
}


int	XcLM_Client_List_Object_Callbacks(the_object)
XcLM_Object		*the_object;
{

  Xc_TRACE(("Listing of '%s' callbacks", the_object->Descriptor->Name));

  Xc_TRACE(("Event 'DATA_CHANGE' :"));
  if (the_object->Data_Change_Callback->Name != NULL)
    {
      Xc_TRACE(("Callback function name = '%s'", the_object->Data_Change_Callback->Name));
    }
#ifndef NTRACE
  else
    Xc_TRACE(("No Callback function"));
#endif

  Xc_TRACE(("Event 'DESC_CHANGE' :"));	if (the_object->Data_Change_Callback == NULL)
	  {
	    if ( (the_object->Data_Change_Callback = (XcLM_Callback *) Xc_malloc("data change callback", sizeof(XcLM_Callback))) == NULL)
	      {
		Xc_WARNING(("Malloc error for data change callback"));
		return (XcLM_MALLOC_ERROR);
	      }
	  }

  if (the_object->Desc_Change_Callback->Name != NULL)
    {
      Xc_TRACE(("Callback function name = '%s'", the_object->Desc_Change_Callback->Name));
    }
#ifndef NTRACE
  else
    Xc_TRACE(("No Callback function"));
#endif
  
  Xc_TRACE(("Event 'OBJECT_DELETED' :"));
  if (the_object->Object_Deleted_Callback->Name != NULL)
    {
      Xc_TRACE(("Callback function name = '%s'", the_object->Object_Deleted_Callback->Name));
    }
#ifndef NTRACE
  else
    Xc_TRACE(("No Callback function"));
#endif
  
  return (0);

}










