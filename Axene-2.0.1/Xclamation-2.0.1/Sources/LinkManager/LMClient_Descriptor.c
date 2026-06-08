/*
** LMClient_Descriptor.c for Xclamation in LinkManager/
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
** Started on  Mon Jul 17 19:12:21 1995 Marc Blet
** Last update Mon Jul 17 19:12:21 1995 Emmanuel Paris
*/

#include "ObjStd.h"
#include "LinkManager.h"


/*------------------------------------------------------------------------------------*/
/*----------     CLIENT DESCRIPTOR : ALLOCATE, DESTROY, COPY, UPLOAD, DOWNLOAD -------*/
/*------------------------------------------------------------------------------------*/


int 	XcLM_Client_Allocate_Client_Descriptor(client_desc)
XcLM_Client_Descriptor	**client_desc;
{
  if ((*client_desc = (XcLM_Client_Descriptor *) Xc_malloc("clidesc", sizeof(XcLM_Client_Descriptor))) == NULL)
    {
      Xc_WARNING(("Client allocation error"));
      return(XcLM_MALLOC_ERROR);
    }

  (*client_desc)->Name = NULL;
  (*client_desc)->Owner_Name = NULL;
  (*client_desc)->Group_Name = NULL;
  (*client_desc)->Ident = 0;
  (*client_desc)->AttributesMask = 0;
  (*client_desc)->ClassMask = 0;
  (*client_desc)->Refresh_Delay = 0;
  return (0);
}



int 	XcLM_Client_Destroy_Client_Descriptor(desc)
XcLM_Client_Descriptor	**desc;
{
  /*
     Xc_TRACE(("    destroying client descriptor ..."));
     */
  if (*desc == NULL)
    return (0);
  
  Xc_free((*desc)->Name);
  Xc_free((*desc)->Owner_Name);
  Xc_free((*desc)->Group_Name);
  Xc_free((*desc));
  *desc = NULL;
  return (0);
}


int	XcLM_Client_Copy_Client_Descriptor(CltTmp, Client_desc)
XcLM_Client_Descriptor	**CltTmp;
XcLM_Client_Descriptor	*Client_desc;
{
  int				Length;

  if ((*CltTmp = (XcLM_Client_Descriptor *) Xc_malloc("clidesc",sizeof(XcLM_Client_Descriptor)))==NULL)
    {
      Xc_TRACE(("No enough memory to create client for copy"));
      return ((XcLM_MALLOC_ERROR));
    }
  
  Length =  strlen(Client_desc->Name)+1;
  if (((*CltTmp)->Name = (char *) Xc_malloc("clidesc name",Length * sizeof(char))) == NULL)
    {
      Xc_TRACE(("No enough memory to create Name in Client Descriptor"));
      return ((XcLM_MALLOC_ERROR));
    }
  strcpy((*CltTmp)->Name, Client_desc->Name);
  
  Length =  strlen(Client_desc->Owner_Name)+1;
  if (((*CltTmp)->Owner_Name = (char *) Xc_malloc("clidesc owner name", Length * sizeof(char))) == NULL)
    {
      Xc_TRACE(("No enough memory to create Owner_Name in Client Descriptor"));
      return ((XcLM_MALLOC_ERROR));
    }
  strcpy((*CltTmp)->Owner_Name, Client_desc->Owner_Name);
  
  Length =  strlen(Client_desc->Group_Name)+1;
  if (((*CltTmp)->Group_Name = (char *) Xc_malloc("clidesc group name", Length * sizeof(char))) == NULL)
    {
      Xc_TRACE(("No enough memory to create Group_Name in Client Descriptor"));
      return ((XcLM_MALLOC_ERROR));
    }
  strcpy((*CltTmp)->Group_Name, Client_desc->Group_Name);
  
  (*CltTmp)->AttributesMask 	= 	Client_desc->AttributesMask;
  (*CltTmp)->ClassMask 		= 	Client_desc->ClassMask;
  (*CltTmp)->Ident 		= 	Client_desc->Ident;

  return (0);
}




/************************************************************************/

int XcLM_Client_Convert_Client_Descriptor_To_Com(com, client_desc)
XcLM_ComData		**com;
XcLM_Client_Descriptor	*client_desc;
{
  int 		Length;
  int		Ii;
  register int  Offset;

  Xc_TRACE(("  Converting Client_Descriptor->Com : '%s'", client_desc->Name));

  Length = XcLM_COM_HEADER_LENGTH_CLIENT_DESCRIPTOR; /* type field */
  Length += XcLM_COM_HEADER_LENGTH_INT + sizeof(int); /* type + legnth + data for 'Ident' */
  Length += XcLM_COM_HEADER_LENGTH_INT + sizeof(int); /* type + legnth + data for 'Refresh Delay' */
  Length += XcLM_COM_HEADER_LENGTH_INT + sizeof(XcLM_Object_AttributesMask); /* type + legnth + data for 'AttributesMask' */
  Length += XcLM_COM_HEADER_LENGTH_INT + sizeof(XcLM_ClassMask); /* type + legnth + data for 'ClassMask' */
  Length += XcLM_COM_HEADER_LENGTH_BYTE + strlen(client_desc->Name)+1; /* type + legnth + data for 'Name' */
  Length += XcLM_COM_HEADER_LENGTH_BYTE + strlen(client_desc->Owner_Name)+1; /* type + legnth + data for 'Owner_Name' */
  Length += XcLM_COM_HEADER_LENGTH_BYTE + strlen(client_desc->Group_Name)+1; /* type + legnth + data for 'Group_Name' */

  if (*com == NULL)
    {
      if (( *com = (XcLM_ComData *) Xc_malloc("com, convert ->com", sizeof(XcLM_ComData))) == NULL)
	{
	  Xc_WARNING(("Malloc error for conversion of client descriptor"));
	  return (XcLM_MALLOC_ERROR);
	}
      if (((*com)->Data = (char *) Xc_malloc("com->data, convert ->com", Length * sizeof(XcLM_ComData))) == NULL)
	{
	  Xc_WARNING(("Malloc error for conversion of client descriptor"));
	  return (XcLM_MALLOC_ERROR);
	}
      (*com)->Length = 0;
      (*com)->Offset = 0;
    }
  Offset = (*com)->Offset;

  ((*com)->Data)[Offset++] = (char) XcLM_COM_CLIENT_DESCRIPTOR;	/* type of data */
  ((*com)->Data)[Offset++] = 7;	/* length = 7fields (name,owner,grp,ident,refresh,attrib,class */
  (*com)->Offset = Offset;

  IF_ERROR(XcLM_Client_Convert_String_To_Com(com, client_desc->Name));
  IF_ERROR(XcLM_Client_Convert_String_To_Com(com, client_desc->Owner_Name));
  IF_ERROR(XcLM_Client_Convert_String_To_Com(com, client_desc->Group_Name));
  IF_ERROR(XcLM_Client_Convert_Int_To_Com(com, client_desc->Ident));
  IF_ERROR(XcLM_Client_Convert_Int_To_Com(com, client_desc->Refresh_Delay));
  IF_ERROR(XcLM_Client_Convert_Int_To_Com(com, client_desc->AttributesMask));
  IF_ERROR(XcLM_Client_Convert_Int_To_Com(com, client_desc->ClassMask));

  /* everything should be ok here */
  return (0);
}



int 	XcLM_Client_Convert_Com_To_Client_Descriptor(com, client_desc)
XcLM_ComData			*com;
XcLM_Client_Descriptor		**client_desc;
{
  int 		Length;
  int		Offset;
  int 		Ii;

  Xc_TRACE(("  Converting Com->Client_Desc"));

  if (com == NULL)
    {
      *client_desc = NULL;
      return (-1);
    }

  if (*client_desc == NULL)
    {
      if ((*client_desc = (XcLM_Client_Descriptor *) Xc_malloc("clidesc, convert com->",  sizeof(XcLM_Client_Descriptor))) == NULL)
	{
	  Xc_WARNING(("Malloc error, can't create client descriptor"));
	  return (XcLM_MALLOC_ERROR);
	}
    }
  else
    {
      if ((*client_desc)->Name != NULL)
	Xc_free((*client_desc)->Name);
      (*client_desc)->Name = NULL;
      if ((*client_desc)->Owner_Name != NULL)
	Xc_free((*client_desc)->Owner_Name);
      (*client_desc)->Owner_Name = NULL;
      if ((*client_desc)->Group_Name != NULL)
	Xc_free((*client_desc)->Group_Name);
      (*client_desc)->Group_Name = NULL;
    }

  Offset = com->Offset;

  if ((com->Data)[Offset++] != XcLM_COM_CLIENT_DESCRIPTOR)
    {
      Xc_WARNING(("         ralala, mvse structure"));
      *client_desc = NULL;
      return (-1);
    }

  Length = (com->Data)[Offset++];

  if (Length != 7)
    {
      Xc_TRACE(("client descriptor length error"));
      return (XcLM_STRUCTURE_ERROR);
    }
  com->Offset = Offset;

  IF_ERROR(XcLM_Client_Convert_Com_To_String(com, &((*client_desc)->Name)));
  IF_ERROR(XcLM_Client_Convert_Com_To_String(com, &((*client_desc)->Owner_Name)));
  IF_ERROR(XcLM_Client_Convert_Com_To_String(com, &((*client_desc)->Group_Name)));
  IF_ERROR(XcLM_Client_Convert_Com_To_Int(com, &((*client_desc)->Ident)));
  IF_ERROR(XcLM_Client_Convert_Com_To_Int(com, &((*client_desc)->Refresh_Delay)));
  IF_ERROR(XcLM_Client_Convert_Com_To_Int(com, &((*client_desc)->AttributesMask)));
  IF_ERROR(XcLM_Client_Convert_Com_To_Int(com, &((*client_desc)->ClassMask)));

  return(0);
}





