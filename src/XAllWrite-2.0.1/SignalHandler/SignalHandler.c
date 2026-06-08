/*
** SignalHandler.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in SignalHandler/
** Method for SignalHandler Class
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
** Last update Mon Feb  1 23:39:42 1999 Emmanuel Paris
*/

#include "SignalHandler.h"

static void *cons_SignalHandler();
static void dest_SignalHandler();
static void *copy_SignalHandler();
static void Add_Callback();
static void Remove_Callback();
static void Modify_Callback();
static void Handle_Signal ___PROTO((int));
static Signal_Callback_t *Allocate_Signal_Callback();
#ifdef NDEBUG
static void void_handler ___PROTO((int));
#endif

sf_SignalHandler fc_SignalHandler =
{
 cons_SignalHandler,
 dest_SignalHandler,
 copy_SignalHandler,
 Add_Callback,
 Remove_Callback,
 Modify_Callback,
};

static  c_SignalHandler *The_Handler;

/* ----------------------------------------------------------------- ** 
** construction method                                               ** 
** ----------------------------------------------------------------- */
static void *cons_SignalHandler()
{
 c_SignalHandler *This;
  
 Xc_HISTORY(("Constructor ..."));
 if ((This = (c_SignalHandler *)Xc_malloc("SignalHandler",
					  sizeof(c_SignalHandler))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation Signal"));
 }
  
 This->f = &fc_SignalHandler;
 This->Callback_List = NULL;
 The_Handler = This;
  
 Xc_HISTORY(("Constructor ok"));
 return (This);
}
/* ----------------------------------------------------------------- ** 
** destruction method                                                ** 
** ----------------------------------------------------------------- */
static void dest_SignalHandler(This)
c_SignalHandler *This;
{
 Signal_Callback_t	*List, *List2;

 Xc_HISTORY(("Destructor ..."));
  
 List = This->Callback_List;
 while(List)
 {
  List2 = List->Next;
#ifndef NDEBUG
  signal(List->Reason, SIG_DFL);
#else
  signal(List->Reason, void_handler);
#endif
  Xc_free(List);
  List = List2;
 }
 Xc_free(The_Handler);

 Xc_HISTORY(("Destructor ok"));
}
/* -------------------------------------------------------------------- ** 
** 				copy method 				**
** -------------------------------------------------------------------- */
static void *copy_SignalHandler(This)
c_SignalHandler *This;
{
 c_SignalHandler *ObjTmp;
  
 if ((ObjTmp = (c_SignalHandler *)Xc_malloc("CSignalHandler", 
					    sizeof(c_SignalHandler))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation Signal"));
 }
 memcpy(ObjTmp, This, sizeof(c_SignalHandler));
 Xc_TRACE(("Object SignalHandler copied but not working"));
 return ObjTmp;
}
/* ----------------------------------------------------------------- ** 
** set another Signal handler                                         ** 
** ----------------------------------------------------------------- */
static void Add_Callback(This, Signal, new_func, callback_data)
c_SignalHandler *This;
int		Signal;
void 		(*new_func)();
void		*callback_data;
{
 Signal_Callback_t	*List;

 List = This->Callback_List;
 if (List)
 {
  while (List->Next != NULL)
   List = List->Next;
      
  List->Next = Allocate_Signal_Callback();
  List = List->Next;
 }
 else
 {
  List = This->Callback_List =  Allocate_Signal_Callback();
 }
 if (!List) return;
  
 List->Callback 	= new_func;
 List->Callback_Data 	= callback_data;
 List->Reason 		= Signal; 
 if (signal(Signal, Handle_Signal) == (void (*)()) -1)
 {
  Xc_WARNING(("'signal' function returns Signal"));
  return;
 }
  
 Xc_HISTORY(("Signal %d handling ok", Signal));
 return;
}
/* ----------------------------------------------------------------- ** 
** unset an Signal handler                                       ** 
** ----------------------------------------------------------------- */
static void Remove_Callback(This, Signal, func)
c_SignalHandler *This;
int	Signal;
void	(*func)();
{
 Signal_Callback_t	*List, *List2, *List3, *Listp;
 boolean		found;
  
 List = Listp = This->Callback_List;
 while(List)
 {
  if ((List->Reason == Signal) && ((func == NULL) || 
				   (List->Callback == func)))
  {
   List2 = List->Next;
	  
   List3 = This->Callback_List;
   found = FALSE;
   while(List3 && !found)
   {
    if (List3->Reason == Signal && List3 != List)
     found = TRUE;
    List3 = List3->Next;
   }
   if (!found)
    signal(List->Reason, SIG_DFL);
	  
   Xc_free(List);
   if (Listp != List)
    Listp->Next = List2;
   else
    This->Callback_List = List2;
   List = List2;
   Xc_TRACE(("Signal Handler %d removed", Signal));
  }
  else
  {
   Listp = List;
   List = List->Next;
  }
 }
 Xc_HISTORY(("Removing Signal handling ok"));
 return;
}
/* ----------------------------------------------------------------- ** 
** -------		Modify a Signal Handler ...          ------- ** 
** ----------------------------------------------------------------- */
static void Modify_Callback(This, Signal, func, new_func, data)
c_SignalHandler *This;
int	Signal;
void 	(*func)();
void 	(*new_func)();
void	*data;
{
 Signal_Callback_t	*List;

 List = This->Callback_List;
 while(List)
 {
  if ((List->Reason == Signal) && (List->Callback == func))
  {
   List->Callback =  new_func;
   List->Callback_Data = data;
  }
  List = List->Next;
 }
 Xc_HISTORY(("Modify Signal handling ok"));
 return;
}

/* -------------------------------------------------------------------- **
** -------  Signal Handler function : this function calls the  --------	**
** -------  callback function and set the signal handler again -------- **
** -------------------------------------------------------------------- */

static void Handle_Signal(Reason)
int	Reason;
{
 Signal_Callback_t	*List;
  
 signal(Reason, Handle_Signal);
  
#ifndef NTRACE
 if (Reason != SIGALRM)
  Xc_TRACE(("signal #%d has ocurred...",Reason));
#endif
 List = The_Handler->Callback_List;
 while (List)
 {
  if (List->Reason == Reason)
  {
#ifndef NTRACE
   if (Reason != SIGALRM)
    Xc_TRACE(("Calling signal handler..."));
#endif
   (List->Callback)(List->Callback_Data);
  }
  List = List->Next;
 }
 return;
}


/* -------------------------------------------------------------------- ** 
**          Allocation of an element of the Callback List		**
** -------------------------------------------------------------------- */
static Signal_Callback_t *Allocate_Signal_Callback()
{
 Signal_Callback_t *ptr;
  
 if (!(ptr = (Signal_Callback_t *)
       Xc_malloc("XErr callback", sizeof(Signal_Callback_t))))
  return NULL;
  
 ptr->Callback = NULL;
 ptr->Callback_Data = NULL;
 ptr->Reason = 0;
 ptr->Next = NULL;
 return ptr;
}

#ifdef NDEBUG
static void void_handler(sig)
int sig;
{
 exit(1);
}
#endif





