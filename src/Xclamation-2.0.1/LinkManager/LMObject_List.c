/*
** LMObject_List.c for Xclamation in LinkManager/
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
** Started on  Mon Jul 17 19:08:27 1995 Marc Blet
** Last update Sat Oct 26 17:01:32 1996 Emmanuel Paris
*/
/*
   #define 	NTRACE
*/
#include "ObjStd.h"
#include "LinkManager.h"


/*---------------------------------------------------------------------------*/
/*-------    OBJECT LIST : DESTROY, INITIALIZE, REGISTER, LIST,TEST      ----*/
/*---------------------------------------------------------------------------*/

int 	XcLM_Client_Initialize_Object_List(List)
XcLM_Object_List	**List;
{
  /*
     Xc_TRACE(("Initializing object lists ..."));
     */
  if ((*List = 
       (XcLM_Object_List *)Xc_malloc("obj list", 
				     sizeof(XcLM_Object_List))) == NULL)
    {
      Xc_TRACE(("can not initialize list"));
      return (XcLM_MALLOC_ERROR);
    }

  (*List)->Next = NULL;
  (*List)->Prev = NULL;
  (*List)->Object = NULL;

  return (0);
}


int    	XcLM_Client_Destroy_Object_List(Client, List)
c_LM_Client			*Client;
XcLM_Object_List		**List;
{
  XcLM_Object_List		*List2;
  
  /*
Xc_TRACE(("destroy object list"));
*/
  if (*List == NULL)
    return (0);

  while (PREV(*List) != NULL)
    *List = PREV(*List);

  while (NEXT(*List) != NULL)
    {
      List2 = NEXT(*List);
      XcLM_Client_Destroy_Object(Client, &((*List)->Object));
      *List = List2;

      /* we don't Xc_free '*List' bicoz 'Destroy_Object' do it itself ...
yes, the object knows its pointer in the list, so it Xc_frees it... */
    }

  if(*List)
    {
      Xc_free(*List);
      *List = NULL;
    }
  /* I don't free the '*List' pointer : I need it to add objects to the list,
it is the list starting pointer ... */

  return (0);
}



/* la fonction suivante efface une liste sans effacer les objets qu'elle
 * contient, d'ou le 'remove' a la place de 'Destroy'
 */

int    	XcLM_Client_Remove_Object_List(Client, List)
c_LM_Client			*Client;
XcLM_Object_List		**List;
{
  XcLM_Object_List		*List2;
  /*
     Xc_TRACE(("remove object list"));
     */
  while (PREV(*List) != NULL)
    *List = PREV(*List);

  while (NEXT(*List) != NULL)
    {
      List2 = NEXT(*List);
      Xc_free(*List);
      *List = List2;
    }
  Xc_free(*List);
  *List = NULL;

  return (0);
}

int	XcLM_Client_Register_Object(list , object)
XcLM_Object_List	**list;
XcLM_Object		*object;

{
  Xc_TRACE(("Register Object ...."));
  /*
     Xc_TRACE(("   Adding object '%s' to the list ...",object->Descriptor->Name));
     */
  while ((*list)->Next != NULL)
    *list = (*list)->Next;
  
  (*list)->Object = object;
  (*list)->Object->Pointer_In_List = *list;

  if (((*list)->Next = (XcLM_Object_List *) Xc_malloc("obj list", sizeof(XcLM_Object_List))) == NULL)
    {
      Xc_TRACE(("No enough memory to allocate memory for next object"));
      return (XcLM_MALLOC_ERROR);
    }
  PREV(NEXT(*list)) = *list;
  *list = (*list)->Next;

  (*list)->Next = NULL;

  return (0);
}


int 	XcLM_Client_Auto_Register_Object(Client, Object2)
c_LM_Client	*Client;
XcLM_Object	*Object2;
{

     Xc_TRACE(("Auto register object, class = %d", Object2->Descriptor->ClassMask));

  if ((Object2->Descriptor->ClassMask & XcLM_PUBLIC) != 0)
    {
      while (NEXT(Client->Public_Object_List) != NULL)
	Client->Public_Object_List = NEXT(Client->Public_Object_List);

      XcLM_Client_Register_Object(&(Client->Public_Object_List), Object2);      
      Client->Public_Object_Number++;
      Xc_TRACE(("register public object"));
      return (0);
    }
  if ((Object2->Descriptor->ClassMask & XcLM_PRIVATE) != 0)
    {
      XcLM_Client_Register_Object(&(Client->Private_Object_List), Object2);
      Client->Private_Object_Number++;
      Xc_TRACE(("register private object"));
      return (0);
    }

  if ((Object2->Descriptor->ClassMask & XcLM_TRASH) != 0)
    {
      XcLM_Client_Register_Object(&(Client->Trash_Object_List), Object2);
      Client->Trash_Object_Number++;

      Xc_TRACE(("there is %d objects in Trash", Client->Trash_Object_Number));
      if (Client->Trash_Object_Number > Client->Trash_Object_Quota)
	{
	  while( PREV(Client->Trash_Object_List) != NULL)
	    Client->Trash_Object_List = PREV(Client->Trash_Object_List);

	  Xc_WARNING(("Quota exceeded, ojbect '%s' will be deleted",
		      Client->Trash_Object_List->Object->Descriptor->Name));

	  XcLM_Client_Destroy_Object(Client,
				     &(Client->Trash_Object_List->Object));
	}
      return (0);
    }
  return (-1);
}

int XcLM_Client_Test_Object_List(List, object_desc)
XcLM_Object_List	**List;
XcLM_Object_Descriptor	*object_desc;
{
  
  Xc_TRACE(("searching existence of object '%s'",object_desc->Name));
  
  if (*List == NULL)
    {
      Xc_WARNING(("pointer is null"));
      return (-1);
    }
  while (PREV(*List) != NULL)
    *List = PREV(*List);

  while (NEXT(*List) != NULL)
    {
      if ((*List)->Object != NULL)
	if (!strcmp(object_desc->Name, (*List)->Object->Descriptor->Name))
	  {
 	    /* objet name is found, but is it the same owner ??? */
	    if (!strcmp(object_desc->Owner_Name, (*List)->Object->Descriptor->Owner_Name))
	      {
		Xc_TRACE(("object found !!!!"));
		return (0);
	      }
	  }
      *List = NEXT(*List);
    }
  return (-1);
}

int XcLM_Client_Test_Object_List_By_Name(List, Name)
XcLM_Object_List	**List;
char			*Name;
{
  
  Xc_TRACE(("searching existence of object '%s'",Name));
  
  if (*List == NULL)
    {
      Xc_WARNING(("pointer is null"));
      return (-1);
    }
  while (PREV(*List) != NULL)
    *List = PREV(*List);

  while (NEXT(*List) != NULL)
    {
      if ((*List)->Object != NULL)
	if (!strcmp(Name, (*List)->Object->Descriptor->Name))
	  {
 	    /* objet name is found, but is it the same owner ??? */
	    Xc_TRACE(("object found !!!!"));
	    return (TRUE);
	  }
      *List = NEXT(*List);
    }
  return (FALSE);
}

int XcLM_Client_List_Object(List, class)
XcLM_Object_List	*List;
XcLM_ClassMask		class;
{
  XcLM_Object		*Object_I;
  XcLM_Object		*Found;

  if (List == NULL)
    return (-1);
  
  while (List->Prev != NULL)
    List = List->Prev;

  while (List->Next != NULL)
    {
      Found = NULL;
      Object_I = List->Object;

      /* now testing flag for each class */
      if (Object_I != NULL)
	{
	  if ((Object_I->Descriptor->ClassMask & class) != 0)
	    Found = Object_I;

#ifndef NTRACE
	  if (Found != NULL)
	    Xc_TRACE(("   found object name = %s",Found->Descriptor->Name));
#endif
	}
      List = List->Next;	/* continue */
      Xc_TRACE(("next element"));
    }

  return (0);
}



int XcLM_Client_List_Object_In_Descriptor_List(List, class, DestList)
XcLM_Object_List		*List;
XcLM_ClassMask			class;
XcLM_Object_Descriptor_List	**DestList;
{

  int				Ii;
  XcLM_Object			*Object_I;
  XcLM_Object			*Found;
  XcLM_Object_Descriptor_List	*Previous;

  if (List == NULL)
    return (-1);
  
  while (List->Prev != NULL)
    List = List->Prev;

  if (*DestList != NULL)
    {
      while (NEXT(*DestList) != NULL)
	*DestList = NEXT(*DestList);
      Previous = *DestList;
      *DestList = NEXT(*DestList);
      
    }
  else
    {
      Previous = NULL;
    }

  /* quelque soit la valeur de *DestList au debut de la fonction, maintenant elle pointe sur
   * un maillon de la liste qu'il faut allouer
   */

  while (List->Next != NULL)
    {
      Found = NULL;
      Object_I = List->Object;

      /* now testing flag for each class */
      if (Object_I != NULL)
	{
	  if (Object_I->Descriptor->ClassMask & class)
	    Found = Object_I;

	  if (Found != NULL)
	    {
	      Xc_TRACE(("   found object name = %s",Found->Descriptor->Name));
	      
	      IF_ERROR(XcLM_Client_Initialize_Object_Descriptor_List(&(*DestList)));
	      PREV(*DestList) = Previous;
	      
	      /*  on vient de retablir le pointer sur le maillon precedent
                 car il a saute a coz  du initialize...
	       */ 
	      
	      IF_ERROR(XcLM_Client_Copy_Object_Descriptor
		       (&((*DestList)->Descriptor), Found->Descriptor));
	      PREV(NEXT(*DestList)) = *DestList;

	      *DestList = NEXT(*DestList);
	    }
	}
      List = List->Next;	/* continue */
    }

  return (0);
}


/* cette fonction devra renvoyer un pointeur de type XcLM_Object_List */
/* elle ira interroger tous les clients pour avoir une liste d'objets
*/

int XcLM_Global_List_Object(List, class)
XcLM_Object_List	*List;
XcLM_ClassMask		class;
{
  XcLM_Object		*Object_I;
  XcLM_Object		*Found;

  if (List == NULL)
    return (-1);
  
  while (List->Prev != NULL)
    List = List->Prev;

  while (List->Next != NULL)
    {
      Found = NULL;
      Object_I = List->Object;

      /* now testing flag for each class */
/*
      if (Object_I->Descriptor->ClassMask & class)
	Found = Object_I;
*/
#ifndef NTRACE
      if (Found != NULL)
	Xc_TRACE(("   found object name = %s",Found->Descriptor->Name));
#endif
      List = List->Next;	/* continue */
      Xc_TRACE(("next element of the list"));
    }

  return (0);
}


int 	XcLM_Client_Convert_Object_List_To_Object_Descriptor_List(DescList, ObjList)
XcLM_Object_Descriptor_List	**DescList;
XcLM_Object_List		*ObjList;
{
  XcLM_Object_Descriptor_List	*templist;

  Xc_TRACE(("Object_List->Object_Descriptor_List ...\n"));

  if (ObjList == NULL)
    {
      Xc_TRACE(("object_list == null ..."));
      *DescList = NULL;
      return (-1);
    }

  if (*DescList == NULL)
    {
      if (XcLM_Client_Initialize_Object_Descriptor_List(&(*DescList))
	  == XcLM_MALLOC_ERROR)
	{
	  Xc_WARNING(("malloc error, obj_list-> obj_desc_list aborted"));
	  return (XcLM_MALLOC_ERROR);
	}
    }

  while (PREV(ObjList) != NULL)
    ObjList = PREV(ObjList);

  while (NEXT(ObjList) != NULL)
    {
      if (XcLM_Client_Copy_Object_Descriptor(&((*DescList)->Descriptor),
					     ObjList->Object->Descriptor)
	  == XcLM_MALLOC_ERROR)
	{
	  Xc_WARNING(("malloc error in obj_list -> obj_desc_list for descriptor"));
	  return (XcLM_MALLOC_ERROR);
	}
      Xc_TRACE(("descriptor '%s' copied", (*DescList)->Descriptor->Name));
      Xc_TRACE(("its owner is : %s", (*DescList)->Descriptor->Owner_Name));

      templist = *DescList;
      if (XcLM_Client_Initialize_Object_Descriptor_List(&((*DescList)->Next)) == XcLM_MALLOC_ERROR)
	{
	  Xc_WARNING(("malloc error in obj_list -> obj_desc_list for next element"));
	  return (XcLM_MALLOC_ERROR);
	}

      /* next struct initialized, prev = next = null */

      PREV(NEXT(*DescList)) = *DescList;

      /* now it's ok */
      
      *DescList = NEXT(*DescList);
      ObjList = NEXT(ObjList);
    }
  return (0);
}


/*************************************************************************/

int	XcLM_Client_Convert_Object_List_To_Com(Com, List)
XcLM_ComData			**Com;
XcLM_Object_List		*List;
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
  IF_ERROR(XcLM_Client_Initialize_ComData_List(&ComList));

  Inbr = Itotal_length = 0;

  while (List->Next != NULL)
    {
      ComVoid1 = NULL;
      IF_ERROR(XcLM_Client_Convert_Object_To_Com(&(ComList->ComData), List->Object));
      Itotal_length += ComList->ComData->Length;
      Inbr++;

      IF_ERROR(XcLM_Client_Initialize_ComData_List(&(ComList->Next)));

      PREV(NEXT(ComList)) = ComList;
      ComList = NEXT(ComList);
      List = NEXT(List);
    }
  /* now we can start the job de 'pour dans deux ans' :-)) */

  Xc_TRACE(("          there are %d objects in list ...", Inbr));

  if (*Com == NULL)
    {
      if ((*Com = (XcLM_ComData *) Xc_malloc("com ", sizeof(XcLM_ComData))) == NULL)
	{
	  Xc_WARNING(("Malloc Error in object list to com conversion"));
	  return (XcLM_MALLOC_ERROR);
	}

      if (( (*Com)->Data = (char *) Xc_malloc("com->data", XcLM_COM_HEADER_LENGTH_OBJECT_LIST + Itotal_length)) == NULL)
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

      if (( (*Com)->Data = (char *) Xc_malloc("com->data", XcLM_COM_HEADER_LENGTH_OBJECT_LIST + Itotal_length + (*Com)->Length )) == NULL)
	{
	  Xc_WARNING((" malloc error for final ComData in obj_desc_list conversion"));
	  return (XcLM_MALLOC_ERROR);
	}
      
      memcpy((*Com)->Data, temp_data, (*Com)->Length);
      Xc_free(temp_data);
    }
  Offset = (*Com)->Offset;

  ((*Com)->Data)[Offset++] = (char) XcLM_COM_OBJECT_LIST;
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

int	XcLM_Client_Convert_Com_To_Object_List(Com, List)
XcLM_ComData			*Com;
XcLM_Object_List		**List;
{
  int				Offset;
  int				Inbr;		
  int				Ii;
  XcLM_Object			*Obj;
  
  Xc_TRACE(("converting com -> object_list ..."));
    
  if (Com == NULL)
    {
      *List = NULL;
      return (-1);
    }

  if (*List == NULL)
    {
      Xc_TRACE(("allocating list"));
      Inbr = XcLM_Client_Initialize_Object_List(&(*List));
      if (Inbr < 0)
	{
	  *List = NULL;
	  return (Inbr);
	}
    }

  Offset = Com->Offset;
  
  /* verify the header */

  if ((Com->Data)[Offset++] != (char) XcLM_COM_OBJECT_LIST)
    return (XcLM_STRUCTURE_ERROR);

  /* how many descriptors are there in the list ??? */

  Inbr = (Com->Data)[Offset++]*256;
  Inbr += (Com->Data)[Offset++];

  Xc_TRACE(("there are %d objects in list",Inbr));
  
  Com->Offset = Offset;

  while (Inbr--)
    {
      /* comme on a fait un initialize avant, Object = NULL, donc la fonction de decodage allouera la memoire */

      Obj = NULL;

      if ( (Ii = XcLM_Client_Convert_Com_To_Object(Com , &Obj)) <0)
	return (Ii);

      if ( (Ii = XcLM_Client_Register_Object(&(*List), Obj)) < 0)
	return (Ii);

    }
  /*
     if ( (Ii = XcLM_Client_Initialize_Object_List(&(*List)->Next)) < 0)
     return (Ii);
     
     PREV(NEXT(*List)) = *List;
     *List = NEXT(*List);
     */
  return (0);
}






