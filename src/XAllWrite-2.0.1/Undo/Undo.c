/*
** Undo.c for XAllWrite in Undo/
** Undo object. Use context duplication by fork().
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Mon Sep  7 18:03:49 1998 Emmanuel Paris
** Last update Sat Jul 31 16:14:26 1999 Emmanuel Paris
*/

/*#define NTRACE*/

#define XLIB_ILLEGAL_ACCESS
#include "Undo.h"
#include "Resource.h"
#include "HelpLine.h"
#include <Xm/PushB.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <setjmp.h>

extern Widget wMainWindow;
extern c_Resource *GlobResources;
extern c_HelpLine *GlobHelpLine;
extern void XcHandleEvent ___PROTO((XEvent *event));
extern void XcFlushEvents ___PROTO((Widget w, void (*func) 
				    ___PROTO((XEvent *))));


/*--- public functions ---*/
static void *cons_Undo();
static void dest_Undo();
static void *copy_Undo();
static void add_widget ___PROTO((c_Undo *This, Widget wUndo, Widget wRedo,
				 int wType));
static void add_callback ___PROTO((c_Undo *This, 
				   void (*callback)(), void *cb_data));
static boolean save_state ___PROTO((c_Undo *This, int fname));
static void registerPixmap ___PROTO((c_Undo *This, Pixmap pixmap));
static boolean unregisterPixmap ___PROTO((c_Undo *This, Pixmap pixmap));

/*--- private functions ---*/
static void restore_state ___PROTO((c_Undo *This));

static void sensitivize ___PROTO((c_Undo *This, boolean sens));
static void toggleManage ___PROTO((c_Undo *This, boolean onredo));
static void changeLabel ___PROTO((c_Undo *This, boolean onredo, int fname));

static void callback_Undo ___PROTO((Widget w, c_Undo *This, 
				    XmPushButtonCallbackStruct *cbs));
static void callback_Redo ___PROTO((Widget w, c_Undo *This, 
				    XmPushButtonCallbackStruct *cbs));
static void sleep_mode ___PROTO((c_Undo *This));
static void void_signal_handler ___PROTO((int sig));
static void kill_save_process ___PROTO((c_Undo *This));
static void kill_save_process2();
static void wakeup_save_process ___PROTO((c_Undo *This));

static void clear_pixmap_cache ___PROTO((c_Undo *This, boolean registered));
static void clear_gc_cache ___PROTO((c_Undo *This));
#if 0
static void undo_gc_cache ___PROTO((c_Undo *This));
static void redo_gc_cache ___PROTO((c_Undo *This));
#endif

static boolean launcher;	/* TRUE if Axene Launcher detected */

static jmp_buf lngjmp;

sf_Undo fc_Undo =
{
 cons_Undo,
 dest_Undo,
 copy_Undo,
 add_widget,
 add_callback,
 save_state,
 registerPixmap,
 unregisterPixmap
};

static void *cons_Undo()
{  
 c_Undo *This;
 Xc_HISTORY(("constructor"));

 if((This=(c_Undo *)Xc_malloc("Undo",sizeof(c_Undo)))==NULL)
  return NULL;
 memset(This, 0, sizeof(c_Undo));
 This->f = &fc_Undo;  
 
 This->save_pid = 0;
 This->wList = NULL;

 This->pHandle = Xc_createItem("pList", 10, sizeof(undo_pix_t));
 This->gHandle = Xc_createItem("pList", 10, sizeof(undo_gc_t));
 This->pList = NULL;
 This->gList = NULL;

 if (getppid() == (pid_t)1)
 {
  launcher = TRUE;
  Xc_TRACE(("Process launched by the Axene Launcher"));
 }
 else
 {
  launcher = FALSE;
  Xc_TRACE(("Process launched by the shell"));
 }
 signal(SIGCHLD, SIG_IGN);
#ifdef HAVE_ON_EXIT
 on_exit(kill_save_process2, (void *)getppid());
#else
 atexit(kill_save_process2);
#endif

 Xc_TRACE(("constructor ok"));
 return This;
}

static void dest_Undo(This)
c_Undo	*This;
{
 undo_wid_t *uw2, *uw = This->wList;

 if (This->save_pid != 0)
  kill_save_process(This);

 while(uw)
 {
  uw2 = uw->Next;

  if (uw->w_Type != XcU_MENU_ITEM)
  {
   char *omess;
   int  rmess;
    
   rmess = F(GlobHelpLine).get_helpline_message(GlobHelpLine, uw->w_Undo);
   if ((omess = F(GlobResources).getString(GlobResources, rmess)) != uw->sundo)
   {
    Xc_free(omess);
    F(GlobResources).changeString(GlobResources, rmess, uw->sundo);
   }
   rmess = F(GlobHelpLine).get_helpline_message(GlobHelpLine, uw->w_Redo);
   if ((omess = F(GlobResources).getString(GlobResources, rmess)) != uw->sredo)
   {
    Xc_free(omess);
    F(GlobResources).changeString(GlobResources, rmess, uw->sredo);
   }
  }
  Xc_free(uw);
  uw = uw2;
 }
 clear_pixmap_cache(This, FUZZY);
 clear_gc_cache(This);
 Xc_destroyItem(This->pHandle);
 Xc_destroyItem(This->gHandle);
 Xc_free(This);
}

static void *copy_Undo(This)
c_Undo	This;
{
 Xc_TRACE(("copy Undo function not done.normally not usefull."));
 return NULL;
}

static void add_callback(This, callback, cb_data)
c_Undo *This;
void (*callback)();
void *cb_data;
{
 int	message;
 undo_wid_t *uw = This->wList;

 This->callback = callback;
 This->cb_data = cb_data;
 
 This->sensitive = FALSE;
 This->onredo = FALSE;

 while(uw)
 {
  XtAddCallback(uw->w_Undo, XmNactivateCallback, 
		(void (*)())callback_Undo, This);
  
  XtAddCallback(uw->w_Redo, XmNactivateCallback, 
		(void (*)())callback_Redo, This);
 
  switch(uw->w_Type)
  {
  case XcU_MENU_ITEM:
   
   uw->sundo = F(GlobResources).getString(GlobResources, XcR_Undo);
   uw->sredo = F(GlobResources).getString(GlobResources, XcR_Redo);
   sensitivize(This, FALSE);
   break;
  case XcU_TOGGLE_ICON:
  case XcU_COUPLE_ICON:
   message = F(GlobHelpLine).get_helpline_message(GlobHelpLine, uw->w_Undo);
   if (message != -1)
    uw->sundo = F(GlobResources).getString(GlobResources, message);
   message = F(GlobHelpLine).get_helpline_message(GlobHelpLine, uw->w_Redo);
   if (message != -1)
    uw->sredo = F(GlobResources).getString(GlobResources, message);
   sensitivize(This, FALSE);
   break;
  default:
   break;
  }
  uw = uw->Next;
 }
}

static void add_widget(This, wUndo, wRedo, wType)
c_Undo *This;
Widget wUndo;
Widget wRedo;
int wType;
{
 undo_wid_t *uw;
 
 uw = (undo_wid_t *)Xc_malloc("uw", sizeof(undo_wid_t));
 uw->w_Undo = wUndo;
 uw->w_Redo = wRedo;
 uw->w_Type = wType;
 uw->sundo = uw->sredo = NULL;
 uw->Next = This->wList;
 
 This->wList = uw;
}

static boolean save_state(This, fname)
c_Undo *This;
int fname;
{
 if (This->onredo) 
  toggleManage(This, FALSE);

 if (This->save_pid != 0)
  kill_save_process(This);

 if (fname != 0)
 {
  if (!This->sensitive)
   sensitivize(This, TRUE);
  
  changeLabel(This, FALSE, fname);
  This->fname = fname;

/*
  XFlush(XtDisplay(wMainWindow));
  XcFlushEvents(wMainWindow, (void (*)())XcHandleEvent);
  XFlush(XtDisplay(wMainWindow));
*/
  if ( (This->save_pid = fork()) == 0)
  {				/* in son : save process */
   This->save_pid = getppid();	/* for redo */
   sleep_mode(This);		
   restore_state(This);
   return TRUE;
  }
  else
  {				/* in father : active process */
   if (This->save_pid == -1)
   {
    This->save_pid = 0;
    goto no_undo;
   }
   return FALSE;
  }
 }
 else
 {
 no_undo:
  if (This->sensitive) 
   sensitivize(This, FALSE);
 }
 return FALSE;
}

static void registerPixmap(This, pixmap)
c_Undo *This;
Pixmap pixmap;
{
 undo_pix_t *pitem;

 pitem = (undo_pix_t *)Xc_new(This->pHandle);
 
 pitem->pixmap = pixmap;
 pitem->state = TRUE;
 pitem->Next = This->pList;
 This->pList = pitem;
}

static boolean unregisterPixmap(This, pixmap)
c_Undo *This;
Pixmap pixmap;
{
 undo_pix_t *pitem, *bpitem;

 bpitem = pitem = This->pList;
 
 while(pitem)
 {
  if (pitem->pixmap == pixmap) break;
  bpitem = pitem;
  pitem = pitem->Next;
 }
 
 if (pitem)	/* pixmap found in register list */
 {
  if (bpitem == pitem)
   This->pList = pitem->Next;
  else
   bpitem->Next = pitem->Next;
  Xc_delete(This->pHandle, pitem);
  return TRUE;	/* allow pixmap destruction */
 }
 /* pixmap not found in register list */
 pitem = (undo_pix_t *)Xc_new(This->pHandle);
 
 pitem->pixmap = pixmap;
 pitem->state = FALSE;
 pitem->Next = This->pList;
 This->pList = pitem;
 
 return FALSE;	/* forbid pixmap destruction */
}

static void restore_state(This)
c_Undo *This;
{
 int i;
 XEvent	event, sevent;
 Display *display = XtDisplay(wMainWindow);
 Window window = XtWindow(wMainWindow);
 unsigned long serial;
 Xc_TRACE(("Restore state"));

 Xc_TRACE(("start synchronize"));
 XFlush(display);
 for(i = 0; i < 50; i++)
 {
  sevent.type = ClientMessage;
  sevent.xclient.send_event = True;
  sevent.xclient.display = display;
  sevent.xclient.window = window;
  sevent.xclient.format = 32;
  sevent.xclient.data.l[0] = (long)i;
  XSendEvent(display, window, True, 0, &sevent);
  
  while(1)
  {
   XNextEvent(display, &event);
   serial = event.xany.serial;
   ((_XPrivDisplay)display)->request = serial;
   if (event.type == ClientMessage && event.xclient.data.l[0] == i)
    break;
  }
 }
 XSync(display, True);
 XFlush(display);
 XSync(display, False);

 if (!This->onredo)
 {
  changeLabel(This, TRUE, This->fname);
  toggleManage(This, TRUE);
 }
 else
 {
  toggleManage(This, FALSE);
 }
 
 XFlush(display);
 XcFlushEvents(XtParent(wMainWindow), XcHandleEvent);
 
 if (This->cb_data)
  This->callback(This->cb_data);
 else
  This->callback();
}

/* ----------------------------------------------------------------- ** 
** Sensitivize                                                       ** 
** ----------------------------------------------------------------- */
static void sensitivize(This, sens)
c_Undo *This;
boolean sens;
{
 XmString string;
 undo_wid_t *uw = This->wList;
 
 while(uw)
 {
  switch(uw->w_Type)
  {
  case XcU_MENU_ITEM:
   if (sens)
   {
    XtSetSensitive(uw->w_Undo, True);
   }
   else
   {
    string = F(GlobResources).getXmString(GlobResources, XcR_NoUndo);
    XtVaSetValues(uw->w_Undo, XmNlabelString, string, NULL);
    XmStringFree(string);
    XtSetSensitive(uw->w_Undo, False);
   }
   break;
  case XcU_COUPLE_ICON:
   XtSetSensitive(uw->w_Redo, False);
  case XcU_TOGGLE_ICON:
   XtSetSensitive(uw->w_Undo, sens ? True : False);
   break;
  default:
   break;
  }
  uw = uw->Next;
 }
 This->sensitive = sens;
}

/* ----------------------------------------------------------------- ** 
** toggleManage                                                      ** 
** ----------------------------------------------------------------- */
static void toggleManage(This, onredo)
c_Undo *This;
boolean onredo;
{
 undo_wid_t *uw = This->wList;
 
 while(uw)
 {
  switch(uw->w_Type)
  {
  case XcU_MENU_ITEM:
  case XcU_TOGGLE_ICON:
   if (onredo)
   {
    XtManageChild(uw->w_Redo);
    XtUnmanageChild(uw->w_Undo);
   }
   else
   {
    XtManageChild(uw->w_Undo);
    XtUnmanageChild(uw->w_Redo);
   }
   break;
  case XcU_COUPLE_ICON:
   if (onredo)
   {
    XtSetSensitive(uw->w_Undo, False);
    XtSetSensitive(uw->w_Redo, True);
   }
   else
   {
    XtSetSensitive(uw->w_Undo, True);
    XtSetSensitive(uw->w_Redo, False);
   }
   break;
  default:
   break;
  }
  uw = uw->Next;
 }
 This->onredo = onredo;
}

static void changeLabel(This, onredo, fname)
c_Undo *This;
boolean onredo;
int fname;
{
 char message[100];
 XmString string;
 char *func, *mess, *nmess, *omess;
 int  rmess;
 undo_wid_t *uw = This->wList;

 while(uw)
 {
  switch(uw->w_Type)
  {
  case XcU_MENU_ITEM:
   sprintf(message, (onredo ? uw->sredo : uw->sundo), 
	   F(GlobResources).getString(GlobResources, fname));
   string = XmStringCreateLtoR(message, XmSTRING_DEFAULT_CHARSET);
   XtVaSetValues((onredo ? uw->w_Redo : uw->w_Undo),
		 XmNlabelString, string, NULL);
   XmStringFree(string);
   break;
  case XcU_TOGGLE_ICON:
  case XcU_COUPLE_ICON:
   if ((nmess = onredo ? uw->sredo : uw->sundo) != NULL)
   {
    rmess = F(GlobHelpLine).get_helpline_message
     (GlobHelpLine, onredo ? uw->w_Redo : uw->w_Undo);

    if ((omess = F(GlobResources).getString(GlobResources, rmess)) != nmess)
     Xc_free(omess);
    
    func = F(GlobResources).getString(GlobResources, fname);
    mess = (char *)Xc_malloc("mess", strlen(func) + strlen(nmess) + 2);
    sprintf(mess, nmess, func);
    F(GlobResources).changeString(GlobResources, rmess, mess);
   }
   break;
  }
  uw = uw->Next;
 }
}

/* ----------------------------------------------------------------- ** 
** Callbacks Functions                                               ** 
** ----------------------------------------------------------------- */
static void callback_Undo(w, This, cbs)
Widget w;
c_Undo *This;
XmPushButtonCallbackStruct *cbs;
{
 Xc_TRACE(("Undo called"));
 
 This->onredo = TRUE;

 wakeup_save_process(This);
 sleep_mode(This);
 restore_state(This);
}

static void callback_Redo(w, This, cbs)
Widget w;
c_Undo *This;
XmPushButtonCallbackStruct *cbs;
{
 Xc_TRACE(("Redo called"));

 This->onredo = FALSE;

 wakeup_save_process(This);
 sleep_mode(This);
 restore_state(This);
}

/* ----------------------------------------------------------------- ** 
** Process and signals handling                                      ** 
** ----------------------------------------------------------------- */
static void sleep_mode(This)
c_Undo *This;
{
 int i, sig;
 void (*def_handler[NSIG])();
 boolean wakeup = FALSE;

 /*--- store signal handlers ---*/
 for(i = 1; i < NSIG; i++)
 {
  if (i != SIGCHLD)
   def_handler[i] = signal(i, void_signal_handler);
 }
 
 do
 {
  if ((sig = setjmp(lngjmp)) != 0)
  {
   Xc_TRACE(("Save process receive signal: %d", sig));
   wakeup = FALSE;
   switch(sig)
   {
   case SIGUSR1:
    wakeup = TRUE;
    break;
   case SIGUSR2:
    clear_pixmap_cache(This, TRUE);
    _exit(0);
    break;
   default:
    pause();
    break;
   }
  }
  else
  {
   wakeup = FALSE;
   pause();
  }
 } while(!wakeup);

 /*--- restore signal handlers ---*/
 for(i = 1; i < NSIG; i++)
 {
  if (i != SIGCHLD)
   signal(i, def_handler[i]);
 } 
}

static void void_signal_handler(sig)
int sig;
{
 Xc_TRACE(("Save Process receive a signal process: %d", sig));
 
 signal(sig, void_signal_handler);
 longjmp(lngjmp, sig);
}

static void kill_save_process(This)
c_Undo *This;
{
 clear_pixmap_cache(This, FALSE);
 clear_gc_cache(This);
 kill(This->save_pid, SIGUSR2);
 This->save_pid = 0;
}

#ifdef HAVE_ON_EXIT
static void kill_save_process2(retcode, pid)
int retcode;
int pid;
#else
static void kill_save_process2()
#endif
{
 if (GlobUndo)
 {
  Xc_TRACE(("Trap exit"));
  if (GlobUndo->save_pid)
   kill_save_process(GlobUndo);
 }
 
 if (launcher)
 {
  Xc_TRACE(("Warn master process that active process exit."));
#ifdef HAVE_ON_EXIT
  if (retcode == 0)		/* normal exit */
   killpg(0, SIGUSR1);
  else				/* exit on error */
   killpg(0, SIGUSR2);
#else
  killpg(0, SIGUSR1);
#endif
 }
}

static void wakeup_save_process(This)
c_Undo *This;
{
 Display *display =  XtDisplay(wMainWindow);
 Window window = XtWindow(wMainWindow);
 XEvent	event, sevent;

 XSync(display, True);
 XFlush(display);

 sevent.type = ClientMessage;
 sevent.xclient.send_event = True;
 sevent.xclient.display = display;
 sevent.xclient.window = window;
 sevent.xclient.format = 32;
 sevent.xclient.data.l[0] = (long)155954;
 XSendEvent(display, window, True, 0, &sevent);
 
 while(1)
 {
  XNextEvent(display, &event);
  if (event.type == ClientMessage && event.xclient.data.l[0] == 155954)
   break;
  XcHandleEvent(&event);
 }
 XFlush(display);
 XSync(display, False);

 kill(This->save_pid, SIGUSR1);
}


static void clear_pixmap_cache(This, registered)
c_Undo *This;
boolean registered;
{
 Display *display = XtDisplay(wMainWindow);
 undo_pix_t *pitem, *bpitem = NULL;

 Xc_TRACE(("Remove all %sregistered Pixmap", registered ? "" : "un"));
 pitem = This->pList;
 while(pitem)
 {
  bpitem = pitem->Next;
  
  if (pitem->state == registered)
   XFreePixmap(display, pitem->pixmap);
  Xc_delete(This->pHandle, pitem);

  pitem = bpitem;
 }
 This->pList = NULL;
}

static void clear_gc_cache(This)
c_Undo *This;
{
 undo_gc_t *gitem, *bgitem = NULL;
 
 Xc_TRACE(("Clear gc cache"));
 gitem = This->gList;
 while(gitem)
 {
  bgitem = gitem->Next;
  Xc_delete(This->gHandle, gitem);
  gitem = bgitem;
 }
 This->gList = NULL;
}

#if 0
static void undo_gc_cache(This)
c_Undo *This;
{


}

static void redo_gc_cache(This)
c_Undo *This;
{


}
#endif
