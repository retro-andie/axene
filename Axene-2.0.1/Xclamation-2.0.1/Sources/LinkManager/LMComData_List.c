/*
** LMComData_List.c for Xclamation in LinkManager/
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
** Started on  Mon Jul 17 19:10:44 1995 Marc Blet
** Last update Mon Jul 17 19:10:44 1995 Emmanuel Paris
*/
/*
#define NTRACE
*/
#include "ObjStd.h"
#include "LinkManager.h"


/*-----------------------------------------------------------------------------------*/
/*----------     COMMUNICATION DATA LIST : INITALIZE, DESTROY, ...    ---------------*/
/*-----------------------------------------------------------------------------------*/

int	XcLM_Client_Initialize_ComData_List(List)
XcLM_ComData_List	**List;
{
  Xc_TRACE(("   Initialize ComData list ..."));

  if (*List != NULL)
    while (NEXT(*List)  != NULL)
      *List = NEXT(*List);

  if (( *List = (XcLM_ComData_List *) Xc_malloc("com list", sizeof(XcLM_ComData_List))) == NULL)
    {
      Xc_WARNING(("Malloc error in initialize ComData List ..."));
      return (XcLM_MALLOC_ERROR);
    }

  PREV(*List) 		= NULL;
  NEXT(*List) 		= NULL;
  (*List)->ComData 	= NULL;

  return (0);
}

int 	XcLM_Client_Destroy_ComData_List(List)
XcLM_ComData_List	**List;
{
  XcLM_ComData_List	*Temp;

  while (PREV(*List) != NULL)
    *List = PREV(*List);
  
  while (NEXT(*List) != NULL)
    {
      Xc_free((*List)->ComData->Data);
      Temp = NEXT(*List);
      Xc_free((*List)->ComData);
      Xc_free(*List);
      *List = Temp;
    }

  Xc_free(*List);
  *List = NULL;
  
  return (0);
}



