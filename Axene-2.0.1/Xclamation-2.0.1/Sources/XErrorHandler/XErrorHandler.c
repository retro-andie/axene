/*
** XErrorHandler.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in XErrorHandler/
** Method for XErrorHandler Class
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
** Started on  Thu Oct 13 17:14:05 1994 Emmanuel Paris
** Last update Mon Mar 16 20:02:37 1998 Emmanuel Paris
*/

#include "XErrorHandler.h"

static void *cons_XErrorHandler();
static void dest_XErrorHandler();
static void *copy_XErrorHandler();
static void XEHset_function();
static void XEHunset_function();
static int  Allocate_Element();

sf_XErrorHandler fc_XErrorHandler =
{
 cons_XErrorHandler,
 dest_XErrorHandler,
 copy_XErrorHandler,
 XEHset_function,
 XEHunset_function
};

static 	c_XErrorHandler	*The_Handler;

/* ---------------------------------------------------------------------**
** THe first element of the callback list is allocated while		**
** constructing the object ...						**
** So the last element of the list is always empty and its fields are	**
** ready to be completed.						**
**----------------------------------------------------------------------*/

/* ----------------------------------------------------------------- ** 
** some globale definition to handle Xerror                          ** 
** ----------------------------------------------------------------- */
static int	FIOErrorHandler();
static int	FXErrorHandler();
static int	FXErrorHandlerDef();

/* -------------------------------------------------------------------- ** 
**          Allocation of an element of the Callback List		**
** -------------------------------------------------------------------- */
static int	Allocate_Element(The_Ptr)
XError_Callback_t	**The_Ptr;
{
 if (!(*The_Ptr = (XError_Callback_t *)
       Xc_malloc("XErr callback", sizeof(XError_Callback_t))))
  return(-1);
  
 (*The_Ptr)->Next = NULL;
 (*The_Ptr)->Prev = NULL;
 return (0);
}
/* ----------------------------------------------------------------- ** 
** construction method                                               ** 
** ----------------------------------------------------------------- */
static void *cons_XErrorHandler()
{
 c_XErrorHandler *This;
  
 Xc_HISTORY(("Constructor ..."));
 if ((This = (c_XErrorHandler *)Xc_malloc("XErrorHandler",
					  sizeof(c_XErrorHandler))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
  
 This->f = &fc_XErrorHandler;
 Allocate_Element(&(This->Callback_List));
  
 XSetIOErrorHandler(FIOErrorHandler);
 XSetErrorHandler(FXErrorHandler);
 The_Handler = This;

 Xc_HISTORY(("Constructor ok"));
 return (This);
}
/* ----------------------------------------------------------------- ** 
** destruction method                                                ** 
** ----------------------------------------------------------------- */
static void dest_XErrorHandler(This)
c_XErrorHandler *This;
{
 XError_Callback_t	*List;

 Xc_HISTORY(("Destructor ..."));
  
 XSetErrorHandler(FXErrorHandlerDef);
  
 while (This->Callback_List->Prev != NULL)
  This->Callback_List = This->Callback_List->Prev;

 while ((List = This->Callback_List->Next) != NULL)
 {
  Xc_free(This->Callback_List);
  This->Callback_List = List;
 }
 Xc_free(This->Callback_List);
 Xc_free(This);

 Xc_HISTORY(("Destructor ok"));
}
/* -------------------------------------------------------------------- ** 
** 				copy method 				**
** -------------------------------------------------------------------- */
static void *copy_XErrorHandler(This)
c_XErrorHandler *This;
{
 c_XErrorHandler *ObjTmp;
  
 if ((ObjTmp = (c_XErrorHandler *)Xc_malloc("CXErrorHandler", 
					    sizeof(c_XErrorHandler))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
 memcpy(ObjTmp, This, sizeof(c_XErrorHandler));
 Xc_TRACE(("Object XErrorHandler copied"));
 return ObjTmp;
}
/* ----------------------------------------------------------------- ** 
** set another error handler                                         ** 
** ----------------------------------------------------------------- */
static void XEHset_function(Error, new_func, callback_data)
int		Error;
int 		(*new_func)();
void		*callback_data;
{
 XError_Callback_t	*List;

 Xc_HISTORY(("Adding a callback for X-Error handling ..."));

 List = The_Handler->Callback_List;
 while (List->Next != NULL)
  List = List->Next;

 List->Callback 	= new_func;
 List->Callback_Data 	= callback_data;
 List->Reason 		= Error; 
 Allocate_Element(&List->Next);
 (List->Next)->Prev = List;
  
 Xc_HISTORY(("Adding a callback for X-Error handling ok"));
}
/* ----------------------------------------------------------------- ** 
** unset an error handler                                       ** 
** ----------------------------------------------------------------- */
static void XEHunset_function(This, Error)
c_XErrorHandler *This;
int		Error;
{
 XError_Callback_t	*List;

 Xc_HISTORY(("Removing a callback for X-Error handling ..."));

 List = This->Callback_List;
 while(This->Callback_List->Next != NULL)
 {
  if (List->Reason == Error)
  {
   Xc_free(List);
   if (This->Callback_List->Prev != NULL)
    (This->Callback_List->Prev)->Next = This->Callback_List->Next;
   This->Callback_List->Next->Prev = This->Callback_List->Prev;
   Xc_TRACE(("XError Handler %d removed",Error));
  }
  This->Callback_List = This->Callback_List->Next;
 }
 This->Callback_List = List;
 Xc_HISTORY(("Removing a callback for X-Error handling ok"));
}
/* ----------------------------------------------------------------- ** 
** I/O Error handler                                                 ** 
** ----------------------------------------------------------------- */
static int FIOErrorHandler(display)
Display		*display;
{
 XError_Callback_t	*List;
  
 Xc_WARNING(("X Error : IO error"));
  
 List = The_Handler->Callback_List;
 while (List->Next != NULL)
 {
  if (List->Reason == XcXH_IO_ERROR_CODE)
  {
   Xc_TRACE(("Calling callback ..."));
   (List->Callback)(List->Callback_Data);
  }
  List = List->Next;
 }

 Xc_WARNING(("Should save everything savable ..."));
 Xc_WARNING(("No way to continue the execution of this program, aborting ."));
 exit(-1);
 return 0;
}

/* ----------------------------------------------------------------- ** 
** X Error handler                                                   ** 
** ----------------------------------------------------------------- */
static int FXErrorHandler(display,Error)
Display		*display;
XErrorEvent 	*Error;
{
 char 			msg[80];
 int			Return_Value;
 XError_Callback_t	*List;
  
 XGetErrorText(display, Error->error_code, msg, 80);
#ifdef NXERROR
 Xc_TRACE(("XError #%d: %s", Error->error_code, msg));
#else
 Xc_WARNING(("XError #%d: %s", Error->error_code, msg));
#endif

 List = The_Handler->Callback_List;
 Return_Value = 0;
 while (List->Next != NULL)
 {
  if (List->Reason == Error->error_code)
  {
   Xc_TRACE(("Calling callback ..."));
   Return_Value = (List->Callback)(Error, List->Callback_Data);
  }
  List = List->Next;
 }
 return (0);
}

/* ----------------------------------------------------------------- ** 
** X Error handler when XErrH not activ                              ** 
** ----------------------------------------------------------------- */
static int FXErrorHandlerDef(display, Error)
Display		*display;
XErrorEvent 	*Error;
{
 char 			msg[80];
  
 XGetErrorText(display, Error->error_code, msg, 80);
 Xc_TRACE(("XError #%d: %s", Error->error_code, msg));
  
 return (0);
}












