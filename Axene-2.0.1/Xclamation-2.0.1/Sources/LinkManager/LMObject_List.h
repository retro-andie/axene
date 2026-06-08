/*
** LMObject_List.h for Xclamation in LinkManager/
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
** Started on  Mon Jul 17 19:07:15 1995 Marc Blet
** Last update Mon Jul 17 19:07:15 1995 Emmanuel Paris
*/

#ifndef _XcLM_Object_List_h
#define _XcLM_Object_List_h

int		XcLM_Client_Initialize_Object_List();			/* OK */
int 		XcLM_Client_Register_Object();			/* OK */
int		XcLM_Client_Auto_Register_Object();		/* OK */
int 		XcLM_Client_List_Object();			/* 100 */
int 		XcLM_Client_List_Object_In_Descriptor_List();	/* 60 */

int 		XcLM_Client_Test_Object_List();		/* OK */
int 		XcLM_Client_Test_Object_List_By_Name(); 	/* OK */
int		XcLM_Client_Destroy_Object_List();		/* ok */
int		XcLM_Client_Convert_Object_List_To_Object_Descriptor_List();		/* 100 % */
int		XcLM_Client_Destroy_Object_List();		/* OK */
int		XcLM_Client_Remove_Object_List();		/* OK */

int  		XcLM_Client_Convert_Object_List_To_Com();
int   		XcLM_Client_Convert_Com_To_Object_List();

#endif

