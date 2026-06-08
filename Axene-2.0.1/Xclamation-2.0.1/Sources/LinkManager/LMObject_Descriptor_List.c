/*
** LMObject_Descriptor_List.c for Xclamation in LinkManager/
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
** Started on  Mon Jul 17 19:09:04 1995 Marc Blet
** Last update Sat Oct 26 17:02:53 1996 Emmanuel Paris
*/

#include "ObjStd.h"
#include "LinkManager.h"


/*--------------------------------------------------------------------------*/
/* OBJECT_DESCRIPTOR LIST :  INITIALIZE, DESTROY,  REGISTER, LIST,TEST -----*/
/*--------------------------------------------------------------------------*/

int 	XcLM_Client_Initialize_Object_Descriptor_List(List)
XcLM_Object_Descriptor_List	**List;
{
  /*
     Xc_TRACE(("Initializing object lists ..."));
     */
  if (( *List = (XcLM_Object_Descriptor_List *) Xc_malloc("objdesc list",sizeof(XcLM_Object_Descriptor_List))) == NULL)
    {
      Xc_TRACE(("can not initialize object descriptor list"));
      return (XcLM_MALLOC_ERROR);
    }

  (*List)->Next 	= NULL;
  (*List)->Prev 	= NULL;
  (*List)->Descriptor 	= NULL;


  return (0);
}


int 	XcLM_Client_Destroy_Object_Descriptor_List(List)
XcLM_Object_Descriptor_List	**List;
{
  XcLM_Object_Descriptor_List	*TempList;

  Xc_TRACE(("  destroy object descriptor list ..."));

  if (*List == NULL)
    return (-1);

  while (PREV(*List) != NULL)
    *List = PREV(*List);
  
  while ( NEXT(*List) != NULL)
    {
      if ((*List)->Descriptor != NULL)
	XcLM_Client_Destroy_Object_Descriptor(&((*List)->Descriptor));
      TempList = (*List)->Next;
      Xc_free(*List);
      *List = TempList;
    }
  Xc_free(*List);
  *List = NULL;

  return (0);
}


int 	XcLM_Client_Remove_Object_Descriptor_List(List)
XcLM_Object_Descriptor_List	**List;
{
  XcLM_Object_Descriptor_List	*TempList;

  Xc_TRACE(("  remove object descriptor list ..."));

  if (List == NULL)
    return (-1);

  while (PREV(*List) != NULL)
    *List = PREV(*List);
  
  while ((*List)->Next != NULL)
    {
      TempList = (*List)->Next;
      Xc_free(*List);
      *List = TempList;
    }
  Xc_free(*List);
  *List = NULL;

  return (0);
}

int	XcLM_Client_Destroy_Element_Object_Descriptor_List(List)
XcLM_Object_Descriptor_List	**List;
{
  XcLM_Object_Descriptor_List	*ListP;
  XcLM_Object_Descriptor_List	*ListN;

  Xc_TRACE(("destroy an element in object descriptor list"));

  if (*List == NULL)
    return (-1);

  ListP = PREV(*List);
  ListN = NEXT(*List);

  ListN->Prev = ListP;

  if (ListP != NULL)
    ListP->Next = ListN;

  Xc_free((*List)->Descriptor->Name);
  Xc_free((*List)->Descriptor->Owner_Name);
  Xc_free((*List)->Descriptor);
  Xc_free(*List);
  
  *List = ListN;

  return (0);
}

int	XcLM_Client_Remove_Element_Object_Descriptor_List(List)
XcLM_Object_Descriptor_List	**List;
{
  XcLM_Object_Descriptor_List	*ListP;
  XcLM_Object_Descriptor_List	*ListN;

  Xc_TRACE(("remove an element in object descriptor list"));
  
  if (*List == NULL)
    return (-1);

  ListP = PREV(*List);
  ListN = NEXT(*List);

  ListN->Prev = ListP;

  if (ListP != NULL)
    ListP->Next = ListN;

  Xc_free(*List);
  
  *List = ListN;

  return (0);
}

int	XcLM_Client_Register_Object_Descriptor(list , object_desc)
XcLM_Object_Descriptor_List	**list;
XcLM_Object_Descriptor		*object_desc;

{
  XcLM_Object_Descriptor_List	*templist;

  Xc_TRACE(("   Adding object '%s' to the descriptor list ...",object_desc->Name));

  while (NEXT(*list) != NULL)
    *list = NEXT(*list);
  
  (*list)->Descriptor = object_desc;

  if (((*list)->Next = (XcLM_Object_Descriptor_List *) Xc_malloc("objdesc list", sizeof(XcLM_Object_Descriptor_List))) == NULL)
    {
      Xc_TRACE(("No enough memory to allocate memory for next object"));
      return (XcLM_MALLOC_ERROR);
    }

  templist = *list;
  *list = NEXT(*list);

  PREV(*list) = templist;
  NEXT(*list) = NULL;
  
  return (0);
}


int XcLM_Client_Test_Object_Descriptor_List(List, object_desc)
XcLM_Object_Descriptor_List	**List;
XcLM_Object_Descriptor	*object_desc;
{
  Xc_TRACE(("searching existence of object '%s'",object_desc->Name));
  
  while (PREV(*List) != NULL)
    *List = PREV(*List);

  while (NEXT(*List) != NULL)
    {
      if ((*List)->Descriptor != NULL)
	if (!strcmp(object_desc->Name, (*List)->Descriptor->Name))
	  if ( object_desc->Owner_Ident == (*List)->Descriptor->Owner_Ident)
	    return (0);
      *List = NEXT(*List);
    }
  return (-1);
}

int XcLM_Client_List_Object_Descriptor(List, class)
XcLM_Object_Descriptor_List	*List;
XcLM_ClassMask			class;
{
  XcLM_Object_Descriptor	*ObjectDesc;
  XcLM_Object_Descriptor	*Found;

  if (List == NULL)
    {
      Xc_WARNING(("empty list !!!"));
      return (-1);
    }
  while (List->Prev != NULL)
    List = List->Prev;

  while (List->Next != NULL)
    {
      Found = NULL;
      ObjectDesc = List->Descriptor;

      /* now testing flag for each class */
      if (ObjectDesc != NULL)
	{
	  if (ObjectDesc->ClassMask & class)
	    Found = ObjectDesc;

#ifndef NTRACE
	  if (Found != NULL)
	    Xc_TRACE(("   found object name = %s, proprio = %s",
		      Found->Name, Found->Owner_Name));
#endif
	}
      List = List->Next;	/* continue */
    }

  Xc_TRACE(("end of listing\n"));
  return (0);
}


/* cette fonction devra renvoyer un pointeur de type XcLM_Object_List */
/* elle ira interroger tous les clients pour avoir une liste d'objets
*/

int XcLM_Global_List_Object_Descriptor(List, class)
XcLM_Object_Descriptor_List	*List;
XcLM_ClassMask			class;
{
  XcLM_Object_Descriptor	*ObjectDesc;
  XcLM_Object_Descriptor	*Found;

  if (List == NULL)
    return (-1);
  
  while (List->Prev != NULL)
    List = List->Prev;

  while (List->Next != NULL)
    {
      Found = NULL;
      ObjectDesc = List->Descriptor;

      /* now testing flag for each class */

      if (ObjectDesc->ClassMask & class)
	Found = ObjectDesc;

#ifndef NTRACE
      if (Found != NULL)
	Xc_TRACE(("   found object name = %s",Found->Name));
#endif
      
      List = List->Next;	/* continue */
    }

  return (0);
}

/**********************************************************************/

int	XcLM_Client_Convert_Object_Descriptor_List_To_Com(Com, List)
XcLM_ComData			**Com;
XcLM_Object_Descriptor_List	*List;
{
  XcLM_ComData_List		*ComList;
  XcLM_ComData_List		*ComVoid1;
  int				Offset;
  int				Ii;
  int				Inbr;
  int				Itotal_length;
  char				*temp_data;


  /* go to the begining of the list */

  while (List->Prev != NULL)
    List = List->Prev;
 


  /* header of the object descriptor list is now ok */

  ComList = NULL;
  if ((Ii = XcLM_Client_Initialize_ComData_List(&ComList)) < 0)
    return (Ii);

  Inbr = Itotal_length =0;

  while (List->Next != NULL)
    {
      ComVoid1 = NULL;
      if ((Ii = XcLM_Client_Convert_Object_Descriptor_To_Com(&(ComList->ComData), List->Descriptor)) != 0)
	return (Ii);

      Itotal_length += ComList->ComData->Length;
      Inbr++;

      if ((Ii = XcLM_Client_Initialize_ComData_List(&(ComList->Next))) < 0)
	return (Ii);

      PREV(NEXT(ComList)) = ComList;
      ComList = NEXT(ComList);

      List = NEXT(List);
    }
  
  /* now we can start the job de 'pour dans deux ans' :-)) */

  Xc_TRACE(("          there are %d descriptors in list ...", Inbr));

  if (*Com == NULL)
    {
      if ((*Com = (XcLM_ComData *) Xc_malloc("com",sizeof(XcLM_ComData))) == NULL)
	{
	  Xc_WARNING(("Malloc Error in int to com conversion"));
	  return (XcLM_MALLOC_ERROR);
	}

      if (( (*Com)->Data = (char *) Xc_malloc("com->data", XcLM_COM_HEADER_LENGTH_OBJECT_DESCRIPTOR_LIST + Itotal_length)) == NULL)
	{
	  Xc_WARNING((" malloc error for final ComData in obj_desc_list conversion"));
	  return (XcLM_MALLOC_ERROR);
	}
      (*Com)->Length = 0;
      (*Com)->Offset = 0;
    }
  else
    {
      /* reallocate the 'Data' field to put in the new datas */
      temp_data = (*Com)->Data;

      if (( (*Com)->Data = (char *) Xc_malloc("com->data", XcLM_COM_HEADER_LENGTH_OBJECT_DESCRIPTOR_LIST + Itotal_length + (*Com)->Length )) == NULL)
	{
	  Xc_WARNING((" malloc error for final ComData in obj_desc_list conversion"));
	  return (XcLM_MALLOC_ERROR);
	}
      
      memcpy((*Com)->Data, temp_data, (*Com)->Length);
      Xc_free(temp_data);
    }
  Offset = (*Com)->Offset;

  ((*Com)->Data)[Offset++] = (char) XcLM_COM_OBJECT_DESCRIPTOR_LIST;
  ((*Com)->Data)[Offset++] = (char) (Inbr/256);
  ((*Com)->Data)[Offset++] = (char) (Inbr - 256*(Ii/256));

  (*Com)->Offset = Offset;
  (*Com)->Length = Offset;
  
  while (PREV(ComList) != NULL)
    ComList = PREV(ComList);

  while (NEXT(ComList) != NULL)
    {
      memcpy((*Com)->Data+Offset, ComList->ComData->Data, ComList->ComData->Length);
      Offset += ComList->ComData->Length;
      (*Com)->Length += ComList->ComData->Length;
      ComList = NEXT(ComList);
    }

  (*Com)->Offset = Offset;
  (*Com)->Length = Offset;

  XcLM_Client_Destroy_ComData_List(&ComList);

  return (0);
}

int	XcLM_Client_Convert_Com_To_Object_Descriptor_List(Com, List)
XcLM_ComData			*Com;
XcLM_Object_Descriptor_List	**List;
{
  int				Offset;
  int				Inbr;		
  int				Ii;
  XcLM_Object_Descriptor	*Desc;
  
  Xc_TRACE(("converting com -> object_descriptor_list ..."));
    
  if (Com == NULL)
    {
      *List = NULL;
      return (-1);
    }

  if (*List == NULL)
    {
      Inbr = XcLM_Client_Initialize_Object_Descriptor_List(&(*List));
      if (Inbr < 0)
	{
	  *List = NULL;
	  return (Inbr);
	}
    }

  Offset = Com->Offset;
  
  /* verify the header */

  if ((Com->Data)[Offset++] != (char) XcLM_COM_OBJECT_DESCRIPTOR_LIST)
    return (XcLM_STRUCTURE_ERROR);

  /* how many descriptors are there in the list ??? */

  Inbr = (Com->Data)[Offset++]*256;
  Inbr += (Com->Data)[Offset++];

  Xc_TRACE(("there are %d descriptors in list",Inbr));
  
  Com->Offset = Offset;

  while (Inbr--)
    {
      /* comme on a fait un initialize avant, Descriptor = NULL, donc la fonction de decodage allouera la memoire */

      Desc = NULL;

      IF_ERROR(XcLM_Client_Convert_Com_To_Object_Descriptor(Com, &Desc));

      IF_ERROR(XcLM_Client_Register_Object_Descriptor(&(*List), Desc));
    }
  return (0);
}

