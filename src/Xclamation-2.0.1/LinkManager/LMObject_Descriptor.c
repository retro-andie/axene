/*
** LMObject_Descriptor.c for Xclamation in LinkManager/
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
** Started on  Mon Jul 17 19:09:47 1995 Marc Blet
** Last update Mon Jul 17 19:09:47 1995 Emmanuel Paris
*/
 
#include "ObjStd.h"
#include "LinkManager.h"


/*--------------------------------------------------------------------------*/
/*-----  OBJECT DESCRIPTOR : ALLOCATE, DESTROY, COPY, UPLOAD, DOWNLOAD -----*/
/*--------------------------------------------------------------------------*/


int 	XcLM_Client_Allocate_Object_Descriptor(object_desc)
XcLM_Object_Descriptor	**object_desc;
{
  if ((*object_desc = (XcLM_Object_Descriptor *) Xc_malloc("objdesc", sizeof(XcLM_Object_Descriptor))) == NULL)
    {
      Xc_WARNING(("Object allocation error"));
      return(XcLM_MALLOC_ERROR);
    }
  (*object_desc)->Name			=	NULL;
  (*object_desc)->Owner_Name		=	NULL;
  (*object_desc)->AttributesMask	=	0;
  (*object_desc)->ClassMask		=	0;
  (*object_desc)->Owner_Ident		=	0;
  (*object_desc)->Type			=	0;

  return (0);
}



int 	XcLM_Client_Destroy_Object_Descriptor(desc)
XcLM_Object_Descriptor	**desc;
{
  /*
     Xc_TRACE(("    destroying object descriptor ..."));
     */
  Xc_free((*desc)->Name);
  Xc_free((*desc)->Owner_Name);
  Xc_free((*desc));
  *desc = NULL;
  return (0);
}


int	XcLM_Client_Copy_Object_Descriptor(ObjTmp, Object_desc)
XcLM_Object_Descriptor	**ObjTmp;
XcLM_Object_Descriptor	*Object_desc;
{
  int				Length;

  Xc_TRACE(("COPY OBJECT DESCRIPTOR ..."));

  if ((*ObjTmp = (XcLM_Object_Descriptor *)
       Xc_malloc("objdesc in copy", sizeof(XcLM_Object_Descriptor)))==NULL)
    {
      Xc_WARNING(("No enough memory to create object for copy"));
      return ((XcLM_MALLOC_ERROR));
    }
  
  Length =  strlen(Object_desc->Name)+1;
  if (((*ObjTmp)->Name = (char *) Xc_malloc("objdesc name in copy", Length * sizeof(char))) == NULL)
    {
      Xc_TRACE(("No enough memory to create Name in Object Descriptor"));
      return ((XcLM_MALLOC_ERROR));
    }
  strcpy((*ObjTmp)->Name, Object_desc->Name);
  
  Length =  strlen(Object_desc->Owner_Name)+1;
  if (((*ObjTmp)->Owner_Name = (char *) Xc_malloc("objdesc owner name in copy", Length * sizeof(char))) == NULL)
    {
      Xc_TRACE(("No enough memory to create Owner_Name in Object Descriptor"));
      return ((XcLM_MALLOC_ERROR));
    }
  strcpy((*ObjTmp)->Owner_Name, Object_desc->Owner_Name);
  
  Xc_TRACE(("owner name = %s", (*ObjTmp)->Owner_Name));
  
  (*ObjTmp)->AttributesMask 	= 	Object_desc->AttributesMask;
  (*ObjTmp)->ClassMask 		= 	Object_desc->ClassMask;
  (*ObjTmp)->Owner_Ident	= 	Object_desc->Owner_Ident;
  (*ObjTmp)->Type		= 	Object_desc->Type;
  Xc_TRACE(("copy descriptor ... type = %d",(*ObjTmp)->Type));

  return (0);
}


/******************************************************************************/

int 	XcLM_Client_Convert_Com_To_Object_Descriptor(com, object_desc)
XcLM_ComData			*com;
XcLM_Object_Descriptor		**object_desc;
{
  int 		Length;
  int		Offset;
  int 		Ii;

  Xc_TRACE(("  Converting Com->Object_Desc"));

  if (com == NULL)
    {
      *object_desc = NULL;
      return (-1);
    }

  if (*object_desc == NULL)
    {
      if ((*object_desc = (XcLM_Object_Descriptor *) Xc_malloc("objdesc, convert com->...", sizeof(XcLM_Object_Descriptor))) == NULL)
	{
	  Xc_WARNING(("Malloc error, can't create object descriptor"));
	  return (XcLM_MALLOC_ERROR);
	}
    }
  else
    {
      if ((*object_desc)->Name != NULL)
	Xc_free((*object_desc)->Name);
      (*object_desc)->Name = NULL;
      if ((*object_desc)->Owner_Name != NULL)
	Xc_free((*object_desc)->Owner_Name);
      (*object_desc)->Owner_Name = NULL;
    }

  Offset = com->Offset;

  if ((com->Data)[Offset++] != XcLM_COM_OBJECT_DESCRIPTOR)
    {
      Xc_WARNING(("         ralala, mvse structure"));
      *object_desc = NULL;
      return (-1);
    }

  Length = (com->Data)[Offset++];

  if (Length != 5)
    {
      Xc_TRACE(("object descriptor length error"));
      return (XcLM_STRUCTURE_ERROR);
    }
  com->Offset = Offset;

  /*
     Xc_TRACE(("now decoding descriptor structure"));
     */
  IF_ERROR(XcLM_Client_Convert_Com_To_String(com, &((*object_desc)->Name)));
  IF_ERROR(XcLM_Client_Convert_Com_To_String(com, &((*object_desc)->Owner_Name)));
  IF_ERROR(XcLM_Client_Convert_Com_To_Int(com, &((*object_desc)->Owner_Ident)));
  IF_ERROR(XcLM_Client_Convert_Com_To_Int(com, &((*object_desc)->AttributesMask)));
  IF_ERROR(XcLM_Client_Convert_Com_To_Int(com, &((*object_desc)->ClassMask)));

  return(0);
}


int XcLM_Client_Convert_Object_Descriptor_To_Com(com, object_desc)
XcLM_ComData		**com;
XcLM_Object_Descriptor	*object_desc;
{
  int 		Length;
  int 		Length2;
  int 		Length3;
  int		Ii;
  register int  Offset;

  Xc_TRACE(("  Converting Object_Descriptor->Com"));

  Length2 = strlen(object_desc->Name) + 1; /* the '\0' is 1 char long */
  Length3 = strlen(object_desc->Owner_Name) + 1 ; /* idem */

  Length = XcLM_COM_HEADER_LENGTH_OBJECT_DESCRIPTOR; /* type field */
  Length += XcLM_COM_HEADER_LENGTH_INT + sizeof(int); /* type + legnth + data for 'owner_ident' */
  Length += XcLM_COM_HEADER_LENGTH_INT + sizeof(XcLM_Object_AttributesMask); /* type + legnth + data for 'AttributesMask' */
  Length += XcLM_COM_HEADER_LENGTH_INT + sizeof(XcLM_ClassMask); /* type + legnth + data for 'ClassMask' */
  Length += XcLM_COM_HEADER_LENGTH_BYTE + Length2; /* type + legnth + data for 'Name' */
  Length += XcLM_COM_HEADER_LENGTH_BYTE + Length3; /* type + legnth + data for 'Owner_Name' */

  if (*com == NULL)
    {
      XcLM_Client_Allocate_ComData(&(*com));

      if (((*com)->Data = (char *) Xc_malloc("comdata, convert ->com", Length * sizeof(char))) == NULL)
	{
	  Xc_WARNING(("Malloc error for conversion of object descriptor"));
	  return (XcLM_MALLOC_ERROR);
	}
    }
  Offset = (*com)->Offset;

  ((*com)->Data)[Offset++] = (char) XcLM_COM_OBJECT_DESCRIPTOR;	/* type of data */
  ((*com)->Data)[Offset++] = 5;	/* length = 5 fields (name, ownername, desc, attrib, class */
  (*com)->Offset = Offset;

  IF_ERROR(XcLM_Client_Convert_String_To_Com(com, object_desc->Name));
  IF_ERROR(XcLM_Client_Convert_String_To_Com(com, object_desc->Owner_Name));
  IF_ERROR(XcLM_Client_Convert_Int_To_Com(com, object_desc->Owner_Ident));
  IF_ERROR(XcLM_Client_Convert_Int_To_Com(com, object_desc->AttributesMask));
  IF_ERROR(XcLM_Client_Convert_Int_To_Com(com, object_desc->ClassMask));

  /* everything should be ok here */
  return (0);
}










