/*
** LMComData.c for Xclamation in LinkManager/
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
** Started on  Mon Jul 17 19:11:28 1995 Marc Blet
** Last update Mon Jul 17 19:11:28 1995 Emmanuel Paris
*/

#define NTRACE


#include "ObjStd.h"
#include "LinkManager.h"


int 		XcLM_Client_Allocate_ComData(com)
XcLM_ComData	**com;
{
  
  if (( *com = (XcLM_ComData *) Xc_malloc("com ",sizeof(XcLM_ComData))) == NULL)
    {
      Xc_WARNING(("Malloc error for conversion of object descriptor"));
      return (XcLM_MALLOC_ERROR);
    }
  (*com)->Data = NULL;
  (*com)->Length = 0;
  (*com)->Offset = 0;

  return (0);
}

int  	XcLM_Client_Destroy_ComData(com)
XcLM_ComData	**com;
{
  if (*com == NULL)
    return (-1);

  Xc_free((*com)->Data);
  Xc_free(*com);
  *com = NULL;
  
  return (0);
}


int	XcLM_Client_Copy_ComData(comdest, comsrc)
XcLM_ComData	**comdest;
XcLM_ComData	*comsrc;
{
  int 	Ii;

  Ii = XcLM_Client_Allocate_ComData(&(*comdest));
  if (Ii <0)
    return (Ii);

  if (((*comdest)->Data = (char *) Xc_malloc("com", comsrc->Length * sizeof(char))) == NULL)
    {
      Xc_TRACE(("length of comdata = %d",comsrc->Length));
      Xc_WARNING(("Malloc error for conversion of object descriptor"));
      return (XcLM_MALLOC_ERROR);
    }

  memcpy((*comdest)->Data, comsrc->Data, comsrc->Length);
  (*comdest)->Length = comsrc->Length;
  (*comdest)->Offset = comsrc->Offset;
  
  return (0);
}

/*------------------------------------------------------------------------------------*/
/*-----       COMMUNICATION DATA : CONVERSION OF ALL KIND OF DATAS              ------*/
/*------------------------------------------------------------------------------------*/

int XcLM_Client_Convert_Int_To_Com(com, number)
XcLM_ComData	**com;
int		number;
{
  int 		Length;
  int 		Offset;
  int 		Ii;
  int		Ij;

  /*
     Xc_TRACE(("    Converting Int->Com"));
     */
  if (*com == NULL)
    {
      if ((*com = (XcLM_ComData *) Xc_malloc("com, in int->com", sizeof(XcLM_ComData))) == NULL)
	{
	  Xc_WARNING(("Malloc Error in int to com conversion"));
	  return (XcLM_MALLOC_ERROR);
	}
      else
	{
	  if (( (*com)->Data = (char *) Xc_malloc("com->Data", XcLM_COM_HEADER_LENGTH_INT+sizeof(int))) == NULL)
	    {
	      Xc_WARNING(("Malloc Error in int to com conversion"));
	      return (XcLM_MALLOC_ERROR);
	    }
	  (*com)->Length = 0;
	  (*com)->Offset = 0;
	}
    }
  Offset = (*com)->Offset;
  ((*com)->Data)[Offset++] = (char) XcLM_COM_INT;

  Length = sizeof(int);
  ((*com)->Data)[Offset++] = (char) Length;

  Ii = 0;
  Ij = 1<<((Length-1)*8);
  while (Ii++ < Length )
    {
      ((*com)->Data)[Offset++] = (unsigned char) (number/Ij);
      number -= (number/Ij)*Ij;
      Ij = Ij/256;
    } 

  (*com)->Offset = Offset;
  (*com)->Length = Offset;
  return (0);
}


int 	XcLM_Client_Convert_String_To_Com(com, str)
XcLM_ComData	**com;
char		*str;
{
  int 		Length;

  if (str == NULL)
    Length = 1;
  else
    Length = strlen(str)+1;

  /* pour eviter un seg fault dans le strlen */
  
  return (XcLM_Client_Convert_Byte_To_Com(&(*com), str, Length));
}


int 	XcLM_Client_Convert_Byte_To_Com(com, str, Length)
XcLM_ComData	**com;
char		*str;
int		Length;
{
  register int	Offset;
  int		Ii;

  /*
     Xc_TRACE(("    Converting Byte->Com"));
     */
  if (*com == NULL)
    {
      if ((*com = (XcLM_ComData *) Xc_malloc("com ",sizeof(XcLM_ComData))) == NULL)
	{
	  Xc_WARNING(("Malloc Error in int to com conversion"));
	  return (XcLM_MALLOC_ERROR);
	}
      if (( (*com)->Data = (char *) Xc_malloc("com->Data ",XcLM_COM_HEADER_LENGTH_BYTE + Length)) == NULL)
	{
	  Xc_WARNING(("Malloc Error in int to com conversion"));
	  return (XcLM_MALLOC_ERROR);
	}
      (*com)->Length = 0;
      (*com)->Offset = 0;
    }
  Offset = (*com)->Offset;

  ((*com)->Data)[Offset++] = (char ) XcLM_COM_STRING; /* string data type */

  (*com)->Offset = Offset;
  (*com)->Length = Offset;

  if ( (Ii = XcLM_Client_Convert_Int_To_Com(&(*com), Length)) < 0)
    {
      Xc_WARNING(("error in byte zone length conversion"));
      return (Ii);
    }

  if (str != NULL)
    memcpy((*com)->Data+(*com)->Offset, str, Length);
  else
    ((*com)->Data)[(*com)->Offset] = 0;
  
  (*com)->Offset += Length;
  (*com)->Length = (*com)->Offset;
  
  return(0);
}


int 	XcLM_Client_Convert_Com_To_Byte(com, str, Length)
XcLM_ComData	*com;
char		**str;
int		*Length;
{
  int		Offset;
  int		Ii;

  if (com == NULL)
    *str = NULL;
  else
    {
      Offset = com->Offset;

      if ((com->Data)[Offset++] != XcLM_COM_STRING)
	{
	  Xc_TRACE(("Bad structure error, offset = %d, code = %d",com->Offset, (com->Data)[com->Offset]));
	  return (XcLM_STRUCTURE_ERROR);
	}
      com->Offset = Offset;

      Ii = XcLM_Client_Convert_Com_To_Int(com, Length);

      if (Ii <0)
	{
	  Xc_TRACE(("error while converting com->int for the byte zone length"));
	  return (Ii);
	}

      if ((*str = (char *) Xc_malloc("string ",*Length)) == NULL)
	{
	  Xc_WARNING(("Malloc error, com->Byte conversion"));
	  return (XcLM_MALLOC_ERROR);
	}

      memcpy(*str, com->Data + com->Offset, *Length);

      com->Offset +=  (*Length);
    }

  return(0);
}

int	XcLM_Client_Convert_Com_To_String(com, str)
XcLM_ComData	*com;
char		**str;
{
  int 	Length;

  return (XcLM_Client_Convert_Com_To_Byte(com, &(*str), &Length));
}

int 	XcLM_Client_Convert_Com_To_Int(com, number)
XcLM_ComData	*com;
int		*number;
{
  int 		Length;
  int		Offset;
  int 		Ii;
  int		Ij;
  /*
     Xc_TRACE(("    Converting Com->Int"));
     */
  if (com == NULL)
    number = NULL;
  else
    {
      Offset = com->Offset;

      if ((com->Data)[Offset++] != XcLM_COM_INT)
	{
	  Xc_TRACE(("Bad structure error, offset = %d , code = %d",com->Offset, (com->Data)[com->Offset]));
	  return (XcLM_STRUCTURE_ERROR);
	}
      Length = com->Data[Offset++];

      *number = 0;
      Ii = Offset;
      Ij = 1 << ((Length -1)*8);
      do 
	{
	  *number += ((unsigned char) ((com->Data)[Offset++]))*Ij;
	  Ij /= 256;
	}
      while (Offset < Ii + Length);

      com->Offset = Offset;
    }
  return(0);
}







int	XcLM_Client_Convert_Client_Descriptor_List_To_Com(Com, List)
XcLM_ComData			**Com;
XcLM_Client_Descriptor_List	*List;
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
 
  /* header of the client descriptor list is now ok */
  ComList = NULL;
  if ((Ii = XcLM_Client_Initialize_ComData_List(&ComList)) < 0)
    return (Ii);

  Inbr = Itotal_length =0;

  while (List->Next != NULL)
    {
      ComVoid1 = NULL;
      if ((Ii = XcLM_Client_Convert_Client_Descriptor_To_Com(&(ComList->ComData), List->Descriptor)) != 0)
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
      if ((*Com = (XcLM_ComData *) Xc_malloc("com ",sizeof(XcLM_ComData))) == NULL)
	{
	  Xc_WARNING(("Malloc Error in int to com conversion"));
	  return (XcLM_MALLOC_ERROR);
	}

      if (( (*Com)->Data = (char *) Xc_malloc("com->Data ",XcLM_COM_HEADER_LENGTH_CLIENT_DESCRIPTOR_LIST + Itotal_length)) == NULL)
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

      if (( (*Com)->Data = (char *) Xc_malloc("com->data", XcLM_COM_HEADER_LENGTH_CLIENT_DESCRIPTOR_LIST + Itotal_length + (*Com)->Length )) == NULL)
	{
	  Xc_WARNING((" malloc error for final ComData in obj_desc_list conversion"));
	  return (XcLM_MALLOC_ERROR);
	}
      
      memcpy((*Com)->Data, temp_data, (*Com)->Length);
      Xc_free(temp_data);
    }
  Offset = (*Com)->Offset;

  ((*Com)->Data)[Offset++] = (char) XcLM_COM_CLIENT_DESCRIPTOR_LIST;
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

int	XcLM_Client_Convert_Com_To_Client_Descriptor_List(Com, List)
XcLM_ComData			*Com;
XcLM_Client_Descriptor_List	**List;
{
  int				Offset;
  int				Inbr;		
  int				Ii;
  XcLM_Client_Descriptor	*Desc;
  
  Xc_TRACE(("converting com -> client_descriptor_list ..."));
    
  if (Com == NULL)
    {
      *List = NULL;
      return (-1);
    }

  if (*List == NULL)
    {
      Inbr = XcLM_Client_Initialize_Client_Descriptor_List(&(*List));
      if (Inbr < 0)
	{
	  *List = NULL;
	  return (Inbr);
	}
    }

  Offset = Com->Offset;
  
  Xc_TRACE(("offset in com = %d",Offset));

  /* verify the header */

  if ((Com->Data)[Offset++] != (char) XcLM_COM_CLIENT_DESCRIPTOR_LIST)
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
      IF_ERROR( XcLM_Client_Convert_Com_To_Client_Descriptor(Com, &Desc));
      /*     
	 Xc_TRACE(("descripteur converti ... '%s' attrib = %d",Desc->Name, Desc->AttributesMask));
	 */      
      IF_ERROR( XcLM_Client_Register_Client_Descriptor(&(*List), Desc));
    }
  return (0);
}




