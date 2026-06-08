#ifndef _XcObject_Descriptor_List_h
#define _XcObject_Descriptor_List_h

int			XcLM_Client_Initialize_Object_Descriptor_List();		/* OK */
int			XcLM_CLient_Register_Object_Descriptor();			/* OK */
int 			XcLM_Client_List_Object_Descriptor();				/* 60 */
int 			XcLM_Client_Test_Object_Descriptor_List();			/* OK */
int 			XcLM_Client_Destroy_Object_Descriptor_List();	       		/* OK */
int			XcLM_Client_Remove_Object_Descriptor_List();			/* OK */
int			XcLM_Client_Remove_Element_Object_Descriptor_List();		/* OK */
int			XcLM_Client_Destroy_Element_Object_Descriptor_List();		/* OK */
int			XcLM_Client_Convert_Object_Descriptor_List_To_Com();
int			XcLM_Client_Convert_Com_To_Object_Descriptor_List();
#endif
