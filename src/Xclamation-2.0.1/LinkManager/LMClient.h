#ifndef _LMClient_h
#define _LMClient_h



void 			*cons_LM_Client();				/* OK */
void 			desXcLM_Client();				/* OK */
void 			*copy_LM_Client();				/* OK */

int 			XcLM_Client_Send();				/* 30 % */
int 			XcLM_Client_Receive();				/* 50 % */
int			XcLM_Client_Register_To_Others();		/* OK */
/* int			XcLM_Client_Register_Receive_Client_Descriptor_List(); */
int			XcLM_Client_Register_Receive_Client_Descriptor();


int 			XcLM_Show_Data();				/* 0 % */

#endif
