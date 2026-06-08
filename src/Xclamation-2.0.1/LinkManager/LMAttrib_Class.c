/*
** LMAttrib_Class.c for Xclamation in LinkManager/
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
** Started on  Mon Jul 17 19:13:17 1995 Marc Blet
** Last update Mon Jul 17 19:13:17 1995 Emmanuel Paris
*/

#include "ObjStd.h"
#include "LinkManager.h"



/*-----------------------------------------------------------------------*/
/*--------   CLIENT CLASS : GET, SET, UNSET, TEST       -----------------*/
/*-----------------------------------------------------------------------*/

/* il n'y a pas de verification pour les primitives set et unset.
   cela peut paraitre choquant car on peut charger le descripteur d'un autre
   client, le modifier et lui renvoyer ...
   Mais bon, si on veut faire n'importe quoi, autant provoquer des SEG FAULT
   on saura a quoi s'en tenir :-)))

   En fait, il y aura une verification possible grace aux attributs ...
   certains auront pour but la modification concernant le descripteur
   de l'objet et non les donnees de l'objet

*/

int 	XcLM_Client_Get_Client_Class(client_desc, class)
XcLM_Client_Descriptor	*client_desc;
XcLM_ClassMask		*class;
{
  Xc_TRACE(("setting client class ..."));
  *class = client_desc->ClassMask;
  return (0);
}


int 	XcLM_Client_Set_Client_Class(client_desc, class)
XcLM_Client_Descriptor	*client_desc;
XcLM_ClassMask		class;
{
  Xc_TRACE(("setting client class ..."));

  client_desc->ClassMask |= class;
  return (0);
}


int 	XcLM_Client_Unset_Client_Class(client_desc, class)
XcLM_Client_Descriptor	*client_desc;
XcLM_ClassMask		class;
{
  Xc_TRACE(("Unsetting client class ..."));
  client_desc->ClassMask &= ~class;
  return (0);
}

int 	XcLM_Client_Test_Client_Class(client_desc, class)
XcLM_Client_Descriptor		*client_desc;
XcLM_ClassMask		class;
{
  Xc_TRACE(("testing object's class      ..."));

  if (((int)client_desc->ClassMask & (int)class)!=(int)class)
    return(-1);
  else
    return (0);
}

/*------------------------------------------------------------------------------------*/
/*--------------     CLIENT ATTRIBUTES : SET, UNSET, GET, TEST        ----------------*/
/*------------------------------------------------------------------------------------*/

int XcLM_Client_Set_Client_Attributes(client_desc, attributesmask)
XcLM_Client_Descriptor 		*client_desc;
XcLM_Object_AttributesMask 	attributesmask;
{
  Xc_TRACE(("Set client attributes"));

  client_desc->AttributesMask |= attributesmask;

  return (0);
}


int XcLM_Client_Unset_Client_Attributes(client_desc, attributesmask)
XcLM_Client_Descriptor 		*client_desc;
XcLM_Object_AttributesMask 	attributesmask;
{
  Xc_TRACE(("Unset client attributes"));

  client_desc->AttributesMask &= ~attributesmask;

  return (0);
}


int XcLM_Client_Get_Client_Attributes(client_desc, attributesmask)
XcLM_Client_Descriptor 		*client_desc;
XcLM_Object_AttributesMask 	*attributesmask;
{
  Xc_TRACE(("Get client attributes"));

  *attributesmask = client_desc->AttributesMask;

  return (0);
}


int 	XcLM_Client_Test_Client_Attributes(client_desc, attr)
XcLM_Client_Descriptor		*client_desc;
XcLM_Object_AttributesMask	attr;
{
  Xc_TRACE(("testing object's attributes ..."));

  if (((int)client_desc->AttributesMask & (int)attr)!=(int)attr)
    return(-1);
  else
    return (0);
}

/*------------------------------------------------------------------------------------*/
/*--------------          OBJECT CLASS : GET, SET, UNSET, TEST     -------------------*/
/*------------------------------------------------------------------------------------*/

int 	XcLM_Client_Get_Object_Class(client_desc, object_desc, class)
XcLM_Client_Descriptor	*client_desc;
XcLM_Object_Descriptor	*object_desc;
XcLM_ClassMask		*class;
{
  *class = object_desc->ClassMask;
  return (0);
}


int 	XcLM_Client_Set_Object_Class(client_desc, object_desc, class)
XcLM_Client_Descriptor	*client_desc;
XcLM_Object_Descriptor	*object_desc;
XcLM_ClassMask		class;
{
  XcLM_AttributesMask	Attrib;
  
  Xc_TRACE(("setting object class ... "));

  Attrib = object_desc->AttributesMask;
  if ((client_desc->Ident == object_desc->Owner_Ident) || (Attrib & (XcLM_ALL_WRITE |  XcLM_GROUP_WRITE | XcLM_CLASS_WRITE)))
    {
      Xc_TRACE(("Set ok"));
      object_desc->ClassMask |= class;
      return (0);
    }
  else
    return (XcLM_PERMISSION_DENIED);
}


int 	XcLM_Client_Unset_Object_Class(client_desc, object_desc, class)
XcLM_Client_Descriptor	*client_desc;
XcLM_Object_Descriptor	*object_desc;
XcLM_ClassMask		class;
{
  XcLM_AttributesMask	Attrib;

  Xc_TRACE(("Unsetting object class ..."));

  Attrib = object_desc->AttributesMask;
  if ((client_desc->Ident == object_desc->Owner_Ident) || (Attrib & (XcLM_ALL_WRITE |  XcLM_GROUP_WRITE | XcLM_CLASS_WRITE)))
    {
      object_desc->ClassMask &= ~class;
      Xc_TRACE(("unset ok"));
      return (0);
    }
  else
    return (XcLM_PERMISSION_DENIED);
}


int 	XcLM_Client_Test_Object_Class(client_desc, object_desc, class)
XcLM_Client_Descriptor	*client_desc;
XcLM_Object_Descriptor	*object_desc;
XcLM_ClassMask		class;
{
  Xc_TRACE(("testing object's class      ..."));

  if (((int)object_desc->ClassMask & (int)class)!=(int)class)
    return(-1);
  else
    return (0);
}

/*------------------------------------------------------------------------------------*/
/*--------------     OBJECT ATTRIBUTES : SET, UNSET, GET, TEST        ----------------*/
/*------------------------------------------------------------------------------------*/

int XcLM_Client_Set_Object_Attributes(client_desc, object_desc, attributesmask)
XcLM_Client_Descriptor		*client_desc;
XcLM_Object_Descriptor 		*object_desc;
XcLM_Object_AttributesMask 	attributesmask;
{
  XcLM_AttributesMask	Attrib;

  Xc_TRACE(("Set object attributes"));

  Attrib = object_desc->AttributesMask;
  if ((client_desc->Ident == object_desc->Owner_Ident) || (Attrib & (XcLM_ALL_WRITE |  XcLM_GROUP_WRITE | XcLM_CLASS_WRITE)))
    {
      object_desc->AttributesMask |= attributesmask;
      return (0);
    }
  else
    return (XcLM_PERMISSION_DENIED);

}


int XcLM_Client_Unset_Object_Attributes(client_desc, object_desc, attributesmask)
XcLM_Client_Descriptor		*client_desc;
XcLM_Object_Descriptor 		*object_desc;
XcLM_Object_AttributesMask 	attributesmask;
{
  XcLM_AttributesMask	Attrib;

  Xc_TRACE(("Unset object attributes"));

  Attrib = object_desc->AttributesMask;
  if ((client_desc->Ident == object_desc->Owner_Ident) || (Attrib & (XcLM_ALL_WRITE |  XcLM_GROUP_WRITE | XcLM_CLASS_WRITE)))
    {
      object_desc->AttributesMask &= ~attributesmask;
      return (0);
    }
  else
    return (XcLM_PERMISSION_DENIED);
}


int XcLM_Client_Get_Object_Attributes(client_desc, object_desc, attributesmask)
XcLM_Client_Descriptor		*client_desc;
XcLM_Object_Descriptor 		*object_desc;
XcLM_Object_AttributesMask 	*attributesmask;
{
  Xc_TRACE(("Get object attributes"));

  *attributesmask = object_desc->AttributesMask;

  return (0);
}


int 	XcLM_Client_Test_Object_Attributes(client_desc, object_desc, attr)
XcLM_Client_Descriptor		*client_desc;
XcLM_Object_Descriptor		*object_desc;
XcLM_Object_AttributesMask	attr;
{
  Xc_TRACE(("testing object's attributes ..."));

  Xc_TRACE(("object -> %X, data -> %X",object_desc->AttributesMask, attr));

  if (((int)object_desc->AttributesMask & (int)attr)!=(int)attr)
    return(-1);
  else
    return (0);
}

/*------------------------------------------------------------------------------------*/
/*-----       OPERATION DIVERSES SUR OBJET : SET, GET : NAME, OWNER, DATA       ------*/
/*------------------------------------------------------------------------------------*/

int 	XcLM_CLient_Set_Object_Name(client, object, name)
c_LM_Client		*client;
XcLM_Object		*object;
char			*name;
{
  XcLM_AttributesMask	Attrib;

  Attrib = object->Descriptor->AttributesMask;

  if ((client->Descriptor->Ident == object->Descriptor->Owner_Ident) || (Attrib & (XcLM_ALL_WRITE |  XcLM_GROUP_WRITE | XcLM_CLASS_WRITE)))
    {
      Xc_TRACE(("Setting object name"));
      Xc_free(object->Descriptor->Name);

      if ((object->Descriptor->Name = (char *) Xc_malloc("ObjDesc name", sizeof(char) * (strlen(name)+1))) == NULL)
	{
	  Xc_TRACE(("Malloc error, can't set object name"));
	  exit(1);
	}
      strcpy(object->Descriptor->Name, name);

      Xc_TRACE(("il faudra renvoyer le nouveau descripteur aux applications ..."));

      return (0);
    }
  return (XcLM_PERMISSION_DENIED);
}


int 	XcLM_CLient_Set_Object_Owner(client, object, owner)
c_LM_Client		*client;
XcLM_Object		*object;
char			*owner;
{
  XcLM_AttributesMask	Attrib;

  Attrib = object->Descriptor->AttributesMask;

  if ((client->Descriptor->Ident == object->Descriptor->Owner_Ident) || (Attrib & (XcLM_ALL_WRITE |  XcLM_GROUP_WRITE | XcLM_CLASS_WRITE)))
    {
      Xc_TRACE(("Setting object owner name"));
      Xc_free(object->Descriptor->Owner_Name);

      if ((object->Descriptor->Owner_Name = (char *) Xc_malloc("objdesc owner name", sizeof(char) * (strlen(owner)+1))) == NULL)
	{
	  Xc_TRACE(("Malloc error, can't set object owner name"));
	  exit(1);
	}
      strcpy(object->Descriptor->Owner_Name, owner);
      return (0);
    }
  return (XcLM_PERMISSION_DENIED);
}



int 	XcLM_CLient_Get_Object_Name(client, object, name)
c_LM_Client		*client;
XcLM_Object		*object;
char			*name;
{
  /* this function allocates memory for result */

  Xc_TRACE(("Getting object name"));
  
  if ((name = (char *) Xc_malloc ("get object name ptr", sizeof(char) * (strlen(object->Descriptor->Name)+1))) == NULL)
    {
      Xc_TRACE(("Memory Error, 'Get Object Name' aborted"));
      return (XcLM_MALLOC_ERROR);
    }
  
  strcpy(name, object->Descriptor->Name);

  return (0);
}


int 	XcLM_CLient_Get_Object_Owner(client, object, owner)
c_LM_Client		*client;
XcLM_Object		*object;
char			*owner;
{
  /* this function allocates memory for result */

  Xc_TRACE(("Getting object ownername"));
  
  if ((owner = (char *) Xc_malloc ("get owner name ptr", sizeof(char) * (strlen(object->Descriptor->Owner_Name)+1))) == NULL)
    {
      Xc_TRACE(("Memory Error, 'Get Object Owner Name' aborted"));
      return (XcLM_MALLOC_ERROR);
    }
  
  strcpy(owner, object->Descriptor->Owner_Name);

  return (0);
}


int 	XcLM_CLient_Get_Object_Data(client, object, offset, length, data)
c_LM_Client		*client;
XcLM_Object		*object;
XcLM_Object_Length	*offset;
XcLM_Object_Length	*length;
char			*data;
{
  XcLM_Object_Length	Offset;
  XcLM_Object_Length	Length;

  /* this function allocates memory for result */

  Xc_TRACE(("Getting object data"));
  
  if ((object == NULL) || (client == NULL))
    return (-1);

  if (*offset == -1)
    Offset = 0;
  else
    {
      if (*offset < object->Length)
	Offset = *offset;
      else
	return (XcLM_SIZE_ERROR);
    }
  if (*length == -1)
    Length = object->Length;
  else
    Length = *length;

  if (Offset + Length > object->Length)
    return (XcLM_SIZE_ERROR);

  if (data == NULL)
    if ((data = (char *) Xc_malloc ("get object data", sizeof(char) * (Length))) == NULL)
      {
	Xc_TRACE(("Memory Error, 'Get Object Data' aborted"));
	return (XcLM_MALLOC_ERROR);
      }
  
  memcpy((void *)data, (void *)((object->Data)+Offset), Length);  

  *offset = Offset;
  *length = Length;

  return (0);
}

/* rajouter dans la fonction ki suit la gestion des objets manages, ... */

int 	XcLM_CLient_Set_Object_Data(client, object, offset, length, data)
c_LM_Client		*client;
XcLM_Object		*object;
XcLM_Object_Length	offset;
XcLM_Object_Length	length;
char			*data;
{
  XcLM_AttributesMask	Attrib;

  Attrib = object->Descriptor->AttributesMask;

  if ((client->Descriptor->Ident == object->Descriptor->Owner_Ident) || (Attrib & (XcLM_ALL_WRITE |  XcLM_GROUP_WRITE | XcLM_CLASS_WRITE)))
    {
      Xc_TRACE(("Setting object owner data"));

      Xc_free(object->Data);

      if ((object->Data = (char *) Xc_malloc("object data", sizeof(char) * (length))) == NULL)
	{
	  Xc_TRACE(("Malloc error, can't set object data"));
	  exit(1);
	}

      object->Length = length;
      memcpy((void *)object->Data, (void *)data, length);

      return (0);
    }
  return (XcLM_PERMISSION_DENIED);
}

