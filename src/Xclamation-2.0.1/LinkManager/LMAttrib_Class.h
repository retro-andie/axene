#ifndef _XcLMAttrib_Class_h
#define _XcLMAttrib_Class_h



int			XcLM_Client_Set_Client_Class();			/* OK */
int			XcLM_Client_Unset_Client_Class();		/* OK */
int			XcLM_Client_Get_Client_Class();			/* OK */
int 			XcLM_Client_Test_Client_Class();		/* OK */

int			XcLM_Client_Set_Object_Class();			/* OK */
int			XcLM_Client_Unset_Object_Class();		/* OK */
int			XcLM_Client_Get_Object_Class();			/* OK */
int 			XcLM_Client_Test_Object_Class();		/* OK */

int 			XcLM_Client_Set_Object_Name();			/* OK */
int 			XcLM_Client_Set_Object_Owner();
int 			XcLM_Client_Set_Object_Data();			/* 10 %*/

int 			XcLM_Client_Get_Object_Name();			/* OK */
int 			XcLM_Client_Get_Object_Owner();
int 			XcLM_Client_Get_Object_Data();			/* OK */

int 			XcLM_Client_Set_Object_Attributes();		/* OK */
int 			XcLM_Client_Get_Object_Attributes();		/* OK */
int 			XcLM_Client_Unset_Object_Attributes();		/* OK */
int			XcLM_Client_Test_Object_Attributes();		/* OK */

int 			XcLM_Client_Set_Client_Attributes();		/* OK */
int 			XcLM_Client_Get_Client_Attributes();		/* OK */
int 			XcLM_Client_Unset_Client_Attributes();		/* OK */
int			XcLM_Client_Test_Client_Attributes();		/* OK */


#endif




