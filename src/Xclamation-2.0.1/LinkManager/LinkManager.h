/*
** LinkManager.h for Xclamation in LinkManager/
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
** Started on  Mon Jul 17 19:06:41 1995 Marc Blet
** Last update Thu Jun 20 15:19:25 1996 Emmanuel Paris
*/

#ifndef _XcLinkManager_h
#define _XcLinkManager_h

#include <X11/Intrinsic.h>
#include "xcalibur.h"
#include "LMComData.h"
#include "LMComData_List.h"
#include "LMObject_List.h"
#include "LMAttrib_Class.h"
#include "LMObject_Descriptor_List.h"
#include "LMObject.h"
#include "LMObject_Descriptor.h"
#include "LMClient_Descriptor_List.h"
#include "LMClient_Descriptor.h"
#include "LMClient.h"

#ifndef ___mips
#include "Client.h"
#else
/*--- Fake structure definition ---*/
#include "Xcaliburd.h"
typedef struct {
  struct {
    void (*SendMsg)();
  } *f;
  char Ident;
  listclient    *ListClient;

} c_Client;
#endif 

/* XcLM_... defines the Xcalibur Link Manager defines ... */ 



#define OBJECT_DESCRIPTOR(x)  	((x->Descriptor))
#define CLIENT_DESCRIPTOR(x)  	((x->Descriptor))
#define DESCRIPTOR(x)		((x->Descriptor))

#define CLIENT_DESCRIPTOR_LIST(x)		((x->Client_Descriptor_List))

#define PRIVATE_LIST(x) ((x->Private_Object_List))
#define PUBLIC_LIST(x) ((x->Public_Object_List))

#define FIRST(x)  	(while ((x)->Prev != NULL) (x) = (x)->Prev)
#define NEXT(x) 	((x)->Next)
#define PREV(x) 	((x)->Prev)

#define IF_ERROR(x)	if ( (Ii = x) <0) return (Ii)


#define XcLM_MAX_NAME_LENGTH		256

/* je pense que les attributs correspondants a tous les utilisateurs
   ne sera pas utilise dans un premier temps ...
   'user' correspond a celui qui a mis l'objet le premier dans le clipboard : l'initiateur
   'group' correspond a la liste de destinataires ...
*/


/* XcLM_USER_READ : Unix user can read data */
/* XcLM_USER_WRITE : Unix user can overwrite data */
/* XcLM_USER_DYNAMIC : data are linked or not for the Unix user*/
/* XcLM_USER_ATTRIBUTES : enable Unix user to modify data attributes */

/* XcLM_GROUP_READ : Unix group can read data */
/* XcLM_GROUP_WRITE : Unix group can overwrite data */
/* XcLM_GROUP_DYNAMIC : data are linked or not for the Unix group */
/* XcLM_GROUP_ATTRIBUTES : Enbale Unix group to modify data attributes */

/* XcLM_ALL_READ : All Unix users are able to read data */
/* XcLM_ALL_WRITE : All Unix users are able to overwrite data */
/* XcLM_ALL_DYNAMIC : data are linked or not for all Unix users */
/* XcLM_ALL_ATTRIBUTES : Enable all Unix users to modify data attributes */

/* XcLM_CLASS_READ : defines which classes are able to read datas */
/* XcLM_CLASS_WRITE : defines which classes are able to write datas */
/* XcLM_CLASS_DYNAMIC : defines linkage with a set of classes */

typedef enum _XcLM_Object_Attributes {
  XcLM_USER_READ = 1,
  XcLM_USER_WRITE = 2,
  XcLM_USER_DYNAMIC = 4,
  XcLM_USER_ATTRIBUTES = 8,
  XcLM_GROUP_READ = 16,
  XcLM_GROUP_WRITE = 32,
  XcLM_GROUP_DYNAMIC = 64,
  XcLM_GROUP_ATTRIBUTES = 128,
  XcLM_ALL_READ = 256,
  XcLM_ALL_WRITE = 512,
  XcLM_ALL_DYNAMIC = 1024,
  XcLM_ALL_ATTRIBUTES = 2048,
  XcLM_CLASS_READ = 4096,
  XcLM_CLASS_WRITE = 8192,
  XcLM_CLASS_DYNAMIC = 16384
  } XcLM_Object_Attributes;


/* La notion de classe permet d'etendre la notion de groupe Unix
   un groupe Unix definit un ensemble d'utilisateurs ...
   une classe definit un ensemble d'applications ...
   (texte, bitmap, vectoriel, son, animation, tableur, communication, ...)

   Ainsi, on pourra envoyer un document dans le clipboard pour tous les utilisateurs faisant 
   du graphisme par exemple.
*/

#define 	XcLM_FOR_PRIVATE	"Private"
#define 	XcLM_FOR_SERVER		"Server"
#define 	XcLM_FOR_ALL_CLIENTS	"All Clients"


typedef enum _XcLM_Class {
  XcLM_ALL_CLASS = -1,
  XcLM_FIRST_CLASS = 1,
  XcLM_LOCAL_MANAGED = 1,
  XcLM_PRIVATE = 2,
  XcLM_PUBLIC = 4,
  XcLM_VECTOR_CLASS = 8,
  XcLM_SOUND_CLASS = 16,
  XcLM_ANIMATION_CLASS = 32,
  XcLM_SPREAD_CLASS = 64, 
  XcLM_TEXT_CLASS = 128,
  XcLM_BITMAP_CLASS = 256,
  XcLM_SELECTED = 512,
  XcLM_REMOTE_MANAGED = 1024,
  XcLM_TRASH = 2048,
  XcLM_LAST_CLASS = 11
  } XcLM_Class;

/* XcLM_TRASH : pour dire que l'objet est dans la poubelle */

/*
  XcLM_SELECTED = 512 -> flag pour dire que c'est selectionne dans la liste . 
  Cela est de ce fait valable pour les objets et les clients ...
  (XcLM_Class est dans les objets et dans les clients ...)
  */
/* LOCAL_MANAGED : cas ou on ne recopie pas les donnees a la creation
   de l'objet : on ne reccopie que le pointeur de l'objet interne que l'on
   met dans le clipboard. Dans ce cas, la longueur des donnees vaut -1.

   Exple : si on met un Cadre en LOCAL_MANAGED, on copie uniquement le pointeur
   de type c_Cadre.
*/
/* REMOTE_MANAGED : quand on envoie u objet a une autre appli et que les 
   donnees ne sont pas envoyees, elles sont referencees a l'objet de l'envoyeur
   Ce type de gestion peut etre combine avec LOCAL_MANAGED
*/

/* FIRST_CLASS correspond au bit de la premiere classe, LAST_CLASS a celui de la derniere */

/*
   the XcLM_PUBLIC classflag is used to set an object to public clipboard
   idem with the XcLM_PRIVATE classflag ...
   */

typedef enum _XcLM_Errors {
  XcLM_NO_ERROR = 0,
  XcLM_NO_CONNECTION_ERROR = -1,
  XcLM_NO_SERVER_ERROR = -2,
  XcLM_WRITE_ERROR = -3, 
  XcLM_READ_ERROR = -4,
  XcLM_DYNAMIC_ERROR = -5,
  XcLM_ATTRIBUTES_READ_ERROR = -6, 
  XcLM_ATTRIBUTES_WRITE_ERROR = -7,
  XcLM_DESTINATION_NAME_ERROR = -8,
  XcLM_OBJECT_DOES_NOT_EXIST_ERROR = -9,
  XcLM_NO_SUCH_OBJECT_ERROR = -9,
  XcLM_MALLOC_ERROR = -10,
  XcLM_PERMISSION_DENIED = -11, 
  XcLM_STRUCTURE_ERROR = -12,
  XcLM_SIZE_ERROR = -13,
  XcLM_UNREGISTER_FAILED_ERROR = -14,
  XcLM_NO_SUCH_CLIENT_ERROR = -15,
  XcLM_NEED_CLIENT_ERROR= -16,
  XcLM_CALLBACK_EVENT_UNKNOWN = -17
  } XcLM_Errors;

typedef enum _XcLM_Client_Function_Codes
{
  XcLM_WRITE_ATTRIBUTES = 0, 
  XcLM_READ_ATTRIBUTES, 
  XcLM_WRITE, 
  XcLM_READ, 
  XcLM_SEND_OBJECT, 
  XcLM_RECEIVE_OBJECT, 
  XcLM_SEND_OBJECT_LIST, 
  XcLM_RECEIVE_OBJECT_LIST, 
  XcLM_SEND_OBJECT_DESCRIPTOR, 
  XcLM_RECEIVE_OBJECT_DESCRIPTOR, 
  XcLM_SEND_CLIENT_DESCRIPTOR, 
  XcLM_RECEIVE_CLIENT_DESCRIPTOR, 
  XcLM_SEND_OBJECT_DESCRIPTOR_LIST, 
  XcLM_RECEIVE_OBJECT_DESCRIPTOR_LIST, 
  XcLM_SEND_CLIENT_DESCRIPTOR_LIST, 
  XcLM_RECEIVE_CLIENT_DESCRIPTOR_LIST,
  XcLM_REFUSE_OBJECT_DESCRIPTOR, 
  XcLM_REFUSE_OBJECT_DESCRIPTOR_LIST, 
  XcLM_RESCAN_ALL
  } XcLM_Client_Function_Codes;

typedef enum _XcLM_Data_Types { 
  XcLM_TYPE_STRING = 0, 
  XcLM_TYPE_OBJECT,
  XcLM_TYPE_OBJECT_DESCRIPTOR, 
  XcLM_TYPE_OBJECT_LIST,
  XcLM_TYPE_OBJECT_DESCRIPTOR_LIST,
  XcLM_TYPE_CLIENT, 
  XcLM_TYPE_CLIENT_DESCRIPTOR, 
  XcLM_TYPE_CLIENT_DESCRIPTOR_LIST, 
  XcLM_TYPE_CLIENT_LIST,
  XcLM_TYPE_COMDATA
  } XcLM_Data_Types;

typedef enum _XcLM_Callback_Type {
  XcLM_DATA_CHANGE = 0,
  XcLM_DESC_CHANGE,
  XcLM_OBJECT_DELETED
} XcLM_Callback_Type;

typedef enum _XcLM_Com_Data_Types { 
  XcLM_COM_OBJECT_LIST = 1, 
  XcLM_COM_OBJECT,
  XcLM_COM_OBJECT_DESCRIPTOR_LIST, 
  XcLM_COM_OBJECT_DESCRIPTOR, 
  XcLM_COM_CLIENT,
  XcLM_COM_CLIENT_LIST, 
  XcLM_COM_CLIENT_DESCRIPTOR_LIST,
  XcLM_COM_CLIENT_DESCRIPTOR, 
  XcLM_COM_ATTRIBUTES, 
  XcLM_COM_CLASS, 
  XcLM_COM_STRING,
  XcLM_COM_INT 
  } XcLM_Com_Data_Types;

typedef enum _XcLM_Com_Header_Length {
  XcLM_COM_HEADER_LENGTH_INT = 2, 
  XcLM_COM_HEADER_LENGTH_BYTE = 8, 
  XcLM_COM_HEADER_LENGTH_OBJECT_DESCRIPTOR_LIST = 3, 
  XcLM_COM_HEADER_LENGTH_CLIENT_DESCRIPTOR_LIST = 3, 
  XcLM_COM_HEADER_LENGTH_OBJECT_LIST = 3, 
  XcLM_COM_HEADER_LENGTH_CLIENT_LIST = 3, 
  XcLM_COM_HEADER_LENGTH_CLIENT_DESCRIPTOR = 2, 
  XcLM_COM_HEADER_LENGTH_OBJECT_DESCRIPTOR = 2, 
  XcLM_COM_HEADER_LENGTH_FUNCTION_CODE = 1, 
  XcLM_COM_HEADER_LENGTH_OBJECT = 1 
  } XcLM_Com_Header_Length;

typedef enum _XcLM_Object_Type {
  XcLM_NO_TYPE = 0,
  XcLM_FRAME,
  XcLM_PAGE,
  XcLM_DOCUMENT,
  XcLM_VECTOR,
  XcLM_BITMAP,
  XcLM_TEXT,
  XcLM_LAST_TYPE,
  XcLM_ANIMATION,
  XcLM_SOUND
} XcLM_Object_Type;

/* data type COM_FLOAT does not exist : use a string to encode it and then use COM_STRING to encode */


typedef 	int 		XcLM_Descriptor;
typedef 	int 		XcLM_Object_AttributesMask;
typedef 	int 		XcLM_AttributesMask;
typedef 	int 		XcLM_ClassMask;
typedef 	int 		XcLM_Refresh_Delay;
typedef 	int 		XcLM_Object_Length;
typedef 	int		XcLM_Private_Attributes;


typedef struct 
{
  int			Type;
  char 			*Data;
  XcLM_Object_Length	Length;
  XcLM_Object_Length	Offset;
} XcLM_ComData;

typedef	struct s_XcLM_ComData_List
{
  XcLM_ComData			*ComData;
  struct s_XcLM_ComData_List	*Next;
  struct s_XcLM_ComData_List	*Prev;
} XcLM_ComData_List;


typedef struct s_LM_Object_Descriptor
{
  int				Type;
  char				*Name; /* ascii name (for user ...) */
  void				*Private; 
  /* a pointer for private datas ... only in version 2.0 */
  XcLM_Object_AttributesMask	AttributesMask;
  /* attribute(s) (r/w for user, group, all, class, dynamic ...) */
  char				*Owner_Name;
  /* ascii name of the owner (it is a client name) */
  XcLM_ClassMask		ClassMask;
  /* object class(es) */
  int				Owner_Ident;
  /* ident (for developper) */
} XcLM_Object_Descriptor;



typedef struct s_LM_Server_Descriptor
{
  char 				*Name;
  char 				*User_Name;
  char				*Group_Name;
  XcLM_Object_AttributesMask 	AttributesMask;
  XcLM_ClassMask		ClassMask;
  XcLM_Refresh_Delay		Refresh_Delay;
} XcLM_Server_Descriptor;

typedef struct s_LM_Client_Descriptor
{
  int				Type;
  int				Ident; /* low level ident ... */
  char 				*Name; /* name given to the application */
  char 				*Owner_Name; /* name of the user */
  char				*Group_Name;
  XcLM_Object_AttributesMask 	AttributesMask;
  XcLM_ClassMask		ClassMask;
  XcLM_Refresh_Delay		Refresh_Delay;
} XcLM_Client_Descriptor;



typedef struct s_LM_Object_Zone_List
{
  XcLM_Object_Length           	Start;
  XcLM_Object_Length		Length;
  struct s_LM_Object_List	*Next;
  struct s_LM_Object_List	*Prev;
} XcLM_Object_Zone_List;



typedef struct s_XcLM_Callback
{
  void 		(*LMCallback)();		/* ptr de fonction */
  char		*Name;			        /* chaine pour le nom du Callback */
  void		*User_Data;			/* un pointeur kon file en param kan on apelle le callback */
  int		Number;				/* nbr de callbacks */
} XcLM_Callback;


/* Dans la structure suivante, 'nbr de references de l'objet' = nombre des instances d'un objet du
   clipboard dans le client */

typedef struct s_LM_Object
{
  XcLM_Object_Descriptor	*Descriptor;
  char 				*Data;
  XcLM_Object_Length		Length;
  void				*Pointer_In_List;
  char				**Data_Ptr;		/* pointer on 'Data' zone */
  XcLM_Object_Zone_List		*Send_Zone_List;	/* si ce ptr = NULL, on n'envoi pas l'objet */

  XcLM_Object_Type		Type;

  XcLM_Callback			*Data_Change_Callback;   
  XcLM_Callback			*Desc_Change_Callback;   
  XcLM_Callback			*Object_Deleted_Callback;   

  int				Reference_Number;
  /* nbr de references de l'objet */
} XcLM_Object;



typedef struct s_LM_Object_List
{
  int				Type;
  XcLM_Object            	*Object;
  struct s_LM_Object_List	*Next;
  struct s_LM_Object_List	*Prev;
} XcLM_Object_List;



typedef struct s_LM_Object_Descriptor_List
{
  int					Type;
  XcLM_Object_Descriptor            	*Descriptor;
  struct s_LM_Object_Descriptor_List	*Next;
  struct s_LM_Object_Descriptor_List	*Prev;
} XcLM_Object_Descriptor_List;

typedef struct s_LM_Client_Descriptor_List
{
  int					Type;
  XcLM_Client_Descriptor          	*Descriptor;
  struct s_LM_Client_Descriptor_List	*Next;
  struct s_LM_Client_Descriptor_List	*Prev;
} XcLM_Client_Descriptor_List;



typedef struct
{
  F_STD;
  int (*S_Test_Link)();
  int (*S_Send)();
  int (*S_Receive)();
  int (*S_List_Clients)();
  int (*S_Set_Data_Atributes)();
  int (*S_Set_Data_Class)();
  int (*S_Set_Client_Class)();
} sf_LM_Server;


typedef struct {
  F_STD;
  int 				(*C_Send)();
  int 				(*C_Receive)();

  int		           	(*C_Build_Object)();
  int 				(*C_Destroy_Object)();
  int				(*C_Copy_Object)();

  int				(*C_Allocate_Object_Descriptor)();
  int 				(*C_Destroy_Object_Descriptor)();
  int				(*C_Copy_Object_Descriptor)();

  int				(*C_Allocate_Client_Descriptor)();
  int 				(*C_Destroy_Client_Descriptor)();
  int				(*C_Copy_Client_Descriptor)();

  int				(*C_Get_Client_Class)();
  int				(*C_Set_Client_Class)();
  int 				(*C_Unset_Client_Class)();
  int 				(*C_Test_Client_Class)();

  int 				(*C_Get_Client_Attributes)();
  int 				(*C_Set_Client_Attributes)();
  int 				(*C_Unset_Client_Attributes)();
  int 				(*C_Test_Client_Attributes)();

  int				(*C_Get_Object_Class)();
  int				(*C_Set_Object_Class)();
  int 				(*C_Unset_Object_Class)();
  int 				(*C_Test_Object_Class)();

  int 				(*C_Get_Object_Attributes)();
  int 				(*C_Set_Object_Attributes)();
  int 				(*C_Unset_Object_Attributes)();
  int 				(*C_Test_Object_Attributes)();

  int				(*C_Initialize_Object_List)();
  int				(*C_Destroy_Object_List)();
  int				(*C_Remove_Object_List)();
  int				(*C_Register_Object)();
  int				(*C_Auto_Register_Object)();
  int 				(*C_List_Object)();
  int				(*C_Test_Object_List)();

  int 				(*C_Convert_Object_Descriptor_To_Com)();
  int				(*C_Convert_Com_To_Object_Descriptor)();

  int 				(*C_Convert_String_To_Com)();
  int				(*C_Convert_Com_To_String)();

  int 				(*C_Convert_Byte_To_Com)();
  int				(*C_Convert_Com_To_Byte)();

  int 				(*C_Convert_Int_To_Com)();
  int				(*C_Convert_Com_To_Int)();

  int				(*C_Convert_Object_Descriptor_List_To_Com)();
  int				(*C_Convert_Com_To_Object_Descriptor_List)();

  int				(*C_Convert_Client_Descriptor_To_Com)();
  int 				(*C_Convert_Com_To_Client_Descriptor)();

  int				(*C_Convert_Client_Descriptor_List_To_Com)();
  int 				(*C_Convert_Com_To_Client_Descriptor_List)();

  int				(*C_Convert_Object_To_Com)();
  int				(*C_Convert_Com_To_Object)();

  int				(*C_Convert_Object_List_To_Com)();
  int				(*C_Convert_Com_To_Object_List)();

  int 				(*C_Convert_Object_List_To_Object_Descriptor_List)();


  int				(*C_List_Object_Descriptor)();
  int				(*C_Test_Object_Descriptor_List)();
  int				(*C_Destroy_Object_Descriptor_List)();
  int				(*C_Remove_Object_Descriptor_List)();
  int				(*C_Destroy_Element_Object_Descriptor_List)();
  int				(*C_Remove_Element_Object_Descriptor_List)();
  int				(*C_List_Client_Descriptor)();
  int				(*C_Test_Client_Descriptor_List)();
  int				(*C_Destroy_Client_Descriptor_List)();
  int				(*C_Remove_Client_Descriptor_List)();
  int				(*C_Destroy_Element_Client_Descriptor_List)();
  int				(*C_Remove_Element_Client_Descriptor_List)();

  int				(*C_Allocate_ComData)();
  int				(*C_Destroy_ComData)();
  int				(*C_Copy_ComData)();

  int 				(*C_Test_Link)();

  int 				(*C_List_Clients)();
  

} sf_LM_Client;

/* define the LinkManagerClient Class */

typedef struct {
  int		Type;

  sf_LM_Client	*f;

  XcLM_Client_Descriptor 	*Descriptor;
  XcLM_Object_List		*Public_Object_List; /* current ptr in list */
  XcLM_Object_List		*Private_Object_List; /* current ptr in list */
  XcLM_Object_List		*Trash_Object_List;

  XcLM_Object_List		*List1, List2, List3, List4; /* four lists for global usage */

  XcLM_Client_Descriptor_List	*Client_Descriptor_List;

  XcLM_Object_List		*Receive_Object_List;
  XcLM_Object_Descriptor_List	*Receive_Object_Descriptor_List;
  XcLM_Client_Descriptor_List	*Receive_Client_Descriptor_List;

  XcLM_Object_List		*Send_Object_List;
  XcLM_Object_Descriptor_List	*Send_Object_Descriptor_List;
  XcLM_Client_Descriptor_List	*Send_Client_Descriptor_List;


  XcLM_Object			*Receive_Object;
  XcLM_Object			*Send_Object;

  XcLM_Object_Descriptor	*Receive_Object_Descriptor;
  XcLM_Object_Descriptor	*Send_Object_Descriptor;

  XcLM_Client_Descriptor	*Receive_Client_Descriptor;
  XcLM_Client_Descriptor	*Send_Client_Descriptor;

  XcLM_ComData			*Receive_ComData;
  XcLM_ComData			*Send_ComData;

  int				Public_Object_Number;
  int				Private_Object_Number;
  int				Trash_Object_Number;
  int 				Trash_Object_Quota;
  XcLM_Client_Descriptor	*Sender;
  
  c_Client			*Low_Client;

} c_LM_Client;


/* define the LinkManagerServer Class */
typedef struct
{
  sf_LM_Server		*f;
  XcLM_Client_Descriptor *Client_Descriptor;
  XcLM_Server_Descriptor *Server_Descriptor;
} c_LM_Server;

/* extern sf_LM_Server fc_LM_Server; */
extern sf_LM_Client fc_LM_Client;


#endif







