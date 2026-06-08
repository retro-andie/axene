/*
 * main.c 		- main part of the program
 *
 * Copyright (C) 1994	Xcalibur <xcalibur@axene.org>
 *
 * This file can be redistributed under the terms of the GNU General
 * Public License
 */

#include "ObjStd.h"
#include "LinkManager.h"


c_LM_Client 	*client1;
c_LM_Client	*client2;
c_LM_Client	*client3;

int main(argc, argv)
int	argc;
char	**argv;
{

  XcLM_ClassMask		Class;
  char				*chaine;
  char				*chaine1;
  char				*chaine2;
  char				*nom_client;
  XcLM_Object_AttributesMask	Attrib;
  XcLM_Object_Length		Length;
  XcLM_Object			*object1;
  XcLM_Object			*object2;
  XcLM_Object			*object3;
  XcLM_Object			*object4;
  XcLM_Object			*object5;
  XcLM_Object			*object6;
  XcLM_Object			*object20;
  XcLM_ComData			*ComData;
  double			Dessai;
  XcLM_Object_Descriptor_List	*ObjDescList;
  XcLM_Object_List		*ObjList;
  XcLM_Client_Descriptor	*ClientDesc;
  int				Param;

  chaine1 = (char *) malloc(20);
  strcpy(chaine1, "Ceci est un essai");
  chaine2 = (char *) malloc(20);
  strcpy(chaine1, "Un autre essai");
  nom_client = (char *) malloc(20);
  strcpy(nom_client, "Marc, client 1");

  if (argc >1)
    Param = atoi(argv[1]);
  else
    {
      Xc_TRACE(("param please"));
      Param = 0;
    }

  Class = 0;
  Attrib = 0;
  Attrib = XcLM_USER_READ | XcLM_USER_WRITE | XcLM_CLASS_READ | XcLM_CLASS_WRITE; 
  Class =  XcLM_TEXT_CLASS | XcLM_SPREAD_CLASS | XcLM_PRIVATE;
  Length = strlen(chaine1)+1;

  client1 = (c_LM_Client *)NEW(c_LM_Client)(nom_client, "iwy1", "xcalibur", NULL, Attrib, Class);
  client2 = (c_LM_Client *)NEW(c_LM_Client)("client numero 2", "iwy2", "xcalibur", NULL, Attrib, Class);

  client3 = (c_LM_Client *)NEW(c_LM_Client)("3eme client", "iwy3", "xcalibur", NULL, Attrib, Class);


  F(client1).C_Build_Object(&object1, client1, "Essai 1", nom_client, 1, Attrib, Class, Length, chaine1);
  F(client1).C_Build_Object(&object2, client1, "Essai 2", nom_client, 1, Attrib, Class , Length, chaine2);

  F(client2).C_Build_Object(&object3, client2, "Essai 3", nom_client, 2, Attrib, Class, Length, chaine2);
  F(client2).C_Build_Object(&object4, client2, "Essai 4", nom_client, 2, Attrib, Class, Length, chaine2);

  F(client3).C_Build_Object(&object5, client3, "Essai 5", nom_client, 3, Attrib, Class, Length, chaine2);
  F(client3).C_Build_Object(&object6, client3, "Essai 6", nom_client, 3, Attrib, Class, Length, chaine2);


  if (object1 != NULL)
    {
      if (Param == 1)
	{
	  F(client2).C_Set_Client_Class(client2->Descriptor, XcLM_ANIMATION_CLASS);
	  F(client3).C_Unset_Client_Class(CLIENT_DESCRIPTOR(client3), XcLM_PRIVATE);
	}

      if (Param == 3)
	{
	  ObjDescList = NULL;
	  while (PREV(PRIVATE_LIST(client1)) != NULL)
	    PRIVATE_LIST(client1) = PREV(PRIVATE_LIST(client1));

	  F(client1).C_Convert_Object_List_To_Object_Descriptor_List(&ObjDescList, PRIVATE_LIST(client1));
	  F(client1).C_List_Object_Descriptor(ObjDescList, Class);
    
	  ComData = NULL;
	  F(client1).C_Convert_Object_Descriptor_List_To_Com(&ComData, ObjDescList);

	  F(client1).C_Destroy_Object_Descriptor_List(&ObjDescList);
     
	  ComData->Offset = 0;
	  ObjDescList = NULL;
	  F(client1).C_Convert_Com_To_Object_Descriptor_List(ComData, &ObjDescList);

	  F(client1).C_List_Object_Descriptor(ObjDescList, Class);

	  F(client1).C_Destroy_ComData(&ComData);
	  F(client1).C_Destroy_Object_Descriptor_List(&ObjDescList);
	}

      if (Param == 4)
	{
	  ObjDescList = NULL;
	  while (PREV(PRIVATE_LIST(client1)) != NULL)
	    PRIVATE_LIST(client1) = PREV(PRIVATE_LIST(client1));

	  F(client1).C_Convert_Object_List_To_Object_Descriptor_List(&ObjDescList, PRIVATE_LIST(client1));

	  ComData = NULL;
	  F(client1).C_Convert_Object_Descriptor_List_To_Com(&ComData, ObjDescList);
           
	  F(client1).C_Send(client1, client2->Descriptor, XcLM_RECEIVE_OBJECT_DESCRIPTOR_LIST, ComData);

	  F(client1).C_Convert_Object_List_To_Object_Descriptor_List(&ObjDescList, PRIVATE_LIST(client1));

	  F(client1).C_Destroy_ComData(&ComData);
	  F(client1).C_Destroy_Object_Descriptor_List(&ObjDescList);
	}

      if (Param == 5)
	{
	  ObjList = PRIVATE_LIST(client1);
	  ComData = NULL;

	  F(client1).C_Convert_Object_List_To_Com(&ComData, ObjList);
           
	  F(client1).C_Send(client1, client2->Descriptor, XcLM_RECEIVE_OBJECT_LIST, ComData);

	  F(client1).C_Copy_ComData(&(client2->Receive_ComData), client1->Send_ComData);

	  F(client2).C_Receive(client2, client2->Receive_ComData);

	  F(client2).C_List_Object(PUBLIC_LIST(client2), Class);

	  F(client1).C_Destroy_ComData(&ComData);
	}

      if (Param == 6)
	{
	  ComData = NULL;
	  F(client1).C_Convert_Client_Descriptor_To_Com(&ComData, client1->Descriptor);

	  F(client1).C_Send(client1, client2->Descriptor, XcLM_RECEIVE_CLIENT_DESCRIPTOR, ComData);

	  F(client1).C_Copy_ComData(&(client2->Receive_ComData), client1->Send_ComData);

	  F(client1).C_Destroy_ComData(&ComData);

	  F(client2).C_Receive(client2, client2->Receive_ComData);

	  Xc_TRACE(("\n\n"));

	  ComData = NULL;
	  F(client2).C_Convert_Client_Descriptor_List_To_Com(&ComData, client2->Client_Descriptor_List);

	  Xc_TRACE(("send datas ...........\n"));
	  F(client2).C_Send(client2, client3->Descriptor, XcLM_RECEIVE_CLIENT_DESCRIPTOR_LIST, ComData);
	  Xc_TRACE(("copy comdat ...\n"));
	  F(client2).C_Copy_ComData(&(client3->Receive_ComData), client2->Send_ComData);
	  F(client2).C_Destroy_ComData(&ComData);

      
	  F(client3).C_Receive(client3, client3->Receive_ComData);

	}

      if (Param == 7)
	{
	  ObjDescList = NULL;
	  F(client1).C_Convert_Object_List_To_Object_Descriptor_List(&ObjDescList, PRIVATE_LIST(client1));

	  F(client1).C_List_Object_Descriptor(ObjDescList, Class);

	  ComData = NULL;
	  F(client1).C_Convert_Object_Descriptor_List_To_Com(&ComData, ObjDescList);
           
	  F(client1).C_Send(client1, client2->Descriptor, XcLM_RECEIVE_OBJECT_DESCRIPTOR_LIST, ComData);

	  F(client1).C_Copy_ComData(&(client2->Receive_ComData), client1->Send_ComData);

	  F(client2).C_Receive(client2, client2->Receive_ComData);

	  F(client2).C_Destroy_Object_Descriptor_List(&ObjDescList);
	  F(client2).C_Destroy_ComData(&ComData);
	}


    }

  DELETE(c_LM_Client)(client1);
  DELETE(c_LM_Client)(client2);
  DELETE(c_LM_Client)(client3);

  Xc_mdump();

  free(chaine1);
  free(chaine2);
  free(nom_client);
  
  return 0;
}









