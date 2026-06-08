#ifndef _XcLMComData_h
#define _XcLMComData_h

int	XcLM_Client_Allocate_ComData();
int	XcLM_Client_Destroy_ComData();
int	XcLM_Client_Copy_ComData();
int 	XcLM_Client_Convert_Int_To_Com();
int 	XcLM_Client_Convert_String_To_Com();
int 	XcLM_Client_Convert_Com_To_String();
int 	XcLM_Client_Convert_Com_To_Int();
int 	XcLM_Client_Convert_Byte_To_Com();
int 	XcLM_Client_Convert_Com_To_Byte();
int	XcLM_Client_Convert_Client_Descriptor_To_Com();
int	XcLM_Client_Convert_Com_To_Client_Descriptor();
int	XcLM_Client_Convert_Client_Descriptor_List_To_Com();
int	XcLM_Client_Convert_Com_To_Client_Descriptor_List();


#endif
