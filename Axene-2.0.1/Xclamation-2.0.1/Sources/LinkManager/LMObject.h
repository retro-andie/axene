#ifndef _XcLM_Object_h
#define _XcLM_Object_h

#include "LinkManager.h"

int 			XcLM_Client_Allocate_Object();				/* OK */
int	  		XcLM_Client_Build_Object();			/* 100 % */
int 			XcLM_Client_Destroy_Object();			/* OK */
int			XcLM_Client_Copy_Object();			/* OK */
int			XcLM_Client_Convert_Object_To_Com();
int			XcLM_Client_Convert_Com_To_Object();
int			XcLM_Client_Destroy_Object_Callback();
int 			XcLM_Client_Add_Object_Callback();
int			XcLM_Client_List_Object_Callbacks();
int			XcLM_Client_Destroy_Object_Callbacks();
int 			XcLM_Client_Remove_Object();
int 			XcLM_Client_Move_Object();
#endif
