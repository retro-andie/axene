/*
** LMClient_Descriptor_List.c for Xclamation in LinkManager/
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
** Started on  Mon Jul 17 19:11:56 1995 Marc Blet
** Last update Sat Oct 26 17:10:25 1996 Emmanuel Paris
*/
/*
#define    	NTRACE
*/

#include "ObjStd.h"
#include "LinkManager.h"


/*------------------------------------------------------------------------------------*/
/*----  CLIENT_DESCRIPTOR LIST :  INITIALIZE, DESTROY,  REGISTER, LIST,TEST ----------*/
/*------------------------------------------------------------------------------------*/

int 	XcLM_Client_Initialize_Client_Descriptor_List(List)
XcLM_Client_Descriptor_List	**List;
{ 
  /*
     Xc_TRACE(("Initializing client lists ..."));
     */
  if (( *List = (XcLM_Client_Descriptor_List *) Xc_malloc("client desc list", sizeof(XcLM_Client_Descriptor_List))) == NULL)
    {
      Xc_TRACE(("can not initialize client descriptor list"));
      return (XcLM_MALLOC_ERROR);
    }
  (*List)->Next 	= NULL;
  (*List)->Prev 	= NULL;
  (*List)->Descriptor 	= NULL;

  return (0);
}


int 	XcLM_Client_Destroy_Client_Descriptor_List(List)
XcLM_Client_Descriptor_List	**List;
{
  XcLM_Client_Descriptor_List	*TempList;
  /*
     Xc_TRACE(("  destroy client descriptor list ..."));
     */
  if (*List == NULL)
    return (-1);


  while (PREV(*List) != NULL)
    *List = PREV(*List);
  
  while (NEXT(*List) != NULL) 
    {
     
      if ((*List)->Descriptor != NULL)
	XcLM_Client_Destroy_Client_Descriptor(&((*List)->Descriptor));
      TempList = (*List)->Next;
      Xc_free(*List);
      *List = TempList;
    }
  Xc_free(*List);
  *List = NULL;

  return (0);
}


int 	XcLM_Client_Remove_Client_Descriptor_List(List)
XcLM_Client_Descriptor_List	**List;
{
  XcLM_Client_Descriptor_List	*TempList;

  Xc_TRACE(("  remove client descriptor list ..."));

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
  Xc_TRACE(("       client descriptor list removed"));

  return (0);
}

int	XcLM_Client_Destroy_Element_Client_Descriptor_List(List)
XcLM_Client_Descriptor_List	**List;
{
  XcLM_Client_Descriptor_List	*ListP;
  XcLM_Client_Descriptor_List	*ListN;

  Xc_TRACE(("destroy an element in client descriptor list"));

  if (*List == NULL)
    return (-1);

  ListP = PREV(*List);
  ListN = NEXT(*List);

  ListN->Prev = ListP;

  if (ListP != NULL)
    ListP->Next = ListN;

  Xc_free((*List)->Descriptor->Name);
  Xc_free((*List)->Descriptor->Owner_Name);
  Xc_free((*List)->Descriptor->Group_Name);
  Xc_free((*List)->Descriptor);
  Xc_free(*List);
  
  *List = ListN;

  return (0);
}

int	XcLM_Client_Remove_Element_Client_Descriptor_List(List)
XcLM_Client_Descriptor_List	**List;
{
  XcLM_Client_Descriptor_List	*ListP;
  XcLM_Client_Descriptor_List	*ListN;

  Xc_TRACE(("remove an element in client descriptor list"));
  
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

int	XcLM_Client_Auto_Register_Client_Descriptor(list , client_desc)
XcLM_Client_Descriptor_List	**list;
XcLM_Client_Descriptor		*client_desc;
{

  return (0);
}


int	XcLM_Client_Register_Client_Descriptor(list , client_desc)
XcLM_Client_Descriptor_List	**list;
XcLM_Client_Descriptor		*client_desc;

{
  XcLM_Client_Descriptor_List	*templist;

  Xc_TRACE(("   Adding client '%s' to the descriptor list ...",client_desc->Name));

  while (NEXT(*list) != NULL)
    *list = NEXT(*list);
  
  (*list)->Descriptor = client_desc;

  templist = *list;
  if (((*list)->Next = (XcLM_Client_Descriptor_List *) Xc_malloc("cli desc list", sizeof(XcLM_Client_Descriptor_List))) == NULL)
    {
      Xc_TRACE(("No enough memory to allocate memory for next client"));
      return (XcLM_MALLOC_ERROR);
    }

  *list = NEXT(*list);

  PREV(*list) = templist;
  NEXT(*list) = NULL;
  
  return (0);
}


int XcLM_Client_Test_Client_Descriptor_List(List, client_desc)
XcLM_Client_Descriptor_List	**List;
XcLM_Client_Descriptor	*client_desc;
{

  
  Xc_TRACE(("searching existence of client '%s'",client_desc->Name));
  
  while (PREV(*List) != NULL)
    *List = PREV(*List);

  /*  FIRST(List);
   */
  while (NEXT(*List) != NULL)
    {
      if ((*List)->Descriptor != NULL)
	if (client_desc->Ident == (*List)->Descriptor->Ident)
	  {
	    return (0);
	  }
      *List = NEXT(*List);
    }
  return (-1);
}

int XcLM_Client_List_Client_Descriptor(List, class)
XcLM_Client_Descriptor_List	*List;
XcLM_ClassMask			class;
{
  XcLM_Client_Descriptor	*ClientDesc;
  XcLM_Client_Descriptor	*Found;

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
      ClientDesc = List->Descriptor;

      /* now testing flag for each class */
      if (ClientDesc != NULL)
	{
	  if (ClientDesc->ClassMask & class)
	    Found = ClientDesc;

	  if (Found != NULL)
	    {
	      Xc_TRACE(("   found client name = %s, ident = %d",Found->Name, Found->Ident));
	    }
	  else
	    {
	      Xc_TRACE(("le client ne repond pas au critere de recherche, il a  ->%d",ClientDesc->ClassMask));
	    }
	}
      List = List->Next;	/* continue */
    }

  Xc_TRACE(("end of listing\n"));
  return (0);
}


/* cette fonction devra renvoyer un pointeur de type XcLM_Client_List */
/* elle ira interroger tous les clients pour avoir une liste d'objets
*/

int XcLM_Global_List_Client_Descriptor(List, class)
XcLM_Client_Descriptor_List	*List;
XcLM_ClassMask			class;
{
  XcLM_Client_Descriptor	*ClientDesc;
  XcLM_Client_Descriptor	*Found;

  if (List == NULL)
    return (-1);
  
  while (List->Prev != NULL)
    List = List->Prev;

  while (List->Next != NULL)
    {
      Found = NULL;
      ClientDesc = List->Descriptor;

      /* now testing flag for each class */

      if (ClientDesc->ClassMask & class)
	Found = ClientDesc;

#ifndef NTRACE
      if (Found != NULL)
	Xc_TRACE(("   found client name = %s",Found->Name));
#endif
      List = List->Next;	/* continue */
    }

  return (0);
}





