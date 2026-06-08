/*
** AlertBox.c for XInstall in CustomDialog/
** Simple warnings dialogs
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
** Started on  Fri Dec  2 13:17:46 1994 Stéphane Boisson
** Last update Tue Dec 30 19:31:32 1997 Emmanuel Paris
*/

#define ___ALERT_BOX

#include <stdio.h>
#include <varargs.h>
#include "CustomDialog.h"
#include "MainInterface.h"
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/DialogS.h>

extern Widget wMainWindow;
static boolean recurse_flag = FALSE;

/* ----------------------------------------------------------------- ** 
** Xc_ErrorAlert - Error message                                     ** 
** ----------------------------------------------------------------- */
void Xc_ErrorAlert2();

void Xc_ErrorAlert(text)
char *text;
{
      
 Xc_TRACE(("Xc_ErrorAlert"));

#ifdef ___ALERT_BOX
 if(recurse_flag == FALSE)
 {
  XmString string;

  recurse_flag = TRUE;
      
  string = XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET);
  Xc_ErrorAlert2(string);
  XmStringFree(string);
      
  recurse_flag = FALSE;
 }
 else
#else
 {
  fputs(text, stderr);
  putc('\n', stderr);
 }
#endif  
 return;
}

void Xc_ErrorAlert2(mtext)
XmString mtext;
{
 c_CustomDialog *dialog;
 Widget	label;
 Dimension	width, height;
 Widget	w_box;
 Widget	w_bul;

 w_box = XtVaCreatePopupShell("alertbox", xmDialogShellWidgetClass, 
			      wMainWindow,
			      XmNdeleteResponse, XmDO_NOTHING,
			      XmNmappedWhenManaged, False,
			      XmNinitialResourcesPersistent, False,
			      NULL);
 w_bul = XtVaCreateWidget("msgalert_bboard", xmBulletinBoardWidgetClass,
			  w_box,
			  XmNallowShellResize, True,
			  XmNautoUnmanage, False,
			  XmNresizePolicy, XmRESIZE_NONE,
			  XmNallowOverlap, False,
			  XmNwidth, 450, XmNheight, 125,
			  NULL);
 /*XtVaSetValues(w_box, XmNminWidth, 400, XmNmaxWidth, 400, NULL);*/
 XtVaSetValues(w_box, XmNminHeight, 125, XmNmaxHeight, 125, NULL);

 dialog = NEW(c_CustomDialog)(w_bul, "error_alert", 450, 125,
			      XcCD_HRESIZE);
 label = XtVaCreateManagedWidget("error_text", xmLabelGadgetClass,
				 WIDGET(dialog), XmNlabelString, mtext, NULL);
  
 XtVaGetValues(label, XmNwidth, &width, XmNheight, &height, NULL);
  
 if (width > 300 || height > 100)
 {
  XtDestroyWidget(label);
  DELETE(c_CustomDialog)(dialog);

  XtVaSetValues(w_box, XmNminWidth, MAX(450, width+20), 
		XmNmaxWidth, MAX(450, width+20), NULL);
  XtVaSetValues(w_box, XmNminHeight, MAX(125, height),
		XmNmaxHeight, MAX(125, height), NULL);

  dialog = NEW(c_CustomDialog)(w_bul, "error_alert", 
			       MAX(450, width+20), MAX(125, height),
			       XcCD_HRESIZE);
  label = 
   XtVaCreateManagedWidget("error_text", xmLabelGadgetClass,
			   WIDGET(dialog), XmNlabelString, mtext, NULL);
    
 }
  
 F(dialog).createActionArea(dialog, XcCD_OK, XcCD_OK, NULL, NULL);
 XtManageChild(w_box);
 XtManageChild(w_bul);
 XtPopup(w_box, XtGrabExclusive);
 F(dialog).map(dialog, TRUE);
 F(dialog).waitForUnmap(dialog);
 XtDestroyWidget(label); 
 DELETE(c_CustomDialog)(dialog);
 XtDestroyWidget(w_bul);
 XtDestroyWidget(w_box);
 return;
}

/* ----------------------------------------------------------------- ** 
** Xc_YesNoAlert - Yes or No alert                                   ** 
** ----------------------------------------------------------------- */
boolean Xc_YesNoAlert(text)
char *text;
{
 c_CustomDialog *dialog;
 Widget	label;
 XmString string;
 boolean rc;
 Widget	w_box;
 Widget	w_bul;

 Xc_TRACE(("Xc_YesNoAlert"));

 w_box = XtVaCreatePopupShell("alertbox", xmDialogShellWidgetClass, 
			      wMainWindow,
			      XmNdeleteResponse, XmDO_NOTHING,
			      XmNmappedWhenManaged, False,
			      XmNinitialResourcesPersistent, False,
			      NULL);
 w_bul = XtVaCreateWidget("alert_bboard", xmBulletinBoardWidgetClass,
			  w_box,
			  XmNallowShellResize, True,
			  XmNautoUnmanage, False,
			  XmNresizePolicy, XmRESIZE_NONE,
			  XmNallowOverlap, False,
			  XmNwidth, 400, XmNheight, 125,
			  NULL);
 /*XtVaSetValues(w_box, XmNminWidth, 400, XmNmaxWidth, 400, NULL);*/
 XtVaSetValues(w_box, XmNminHeight, 125, XmNmaxHeight, 125, NULL);

 dialog = NEW(c_CustomDialog)(w_bul, "yesno_alert", 400, 125,
			      XcCD_HRESIZE);
 string = XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET);
 label = XtVaCreateManagedWidget("yesno_text", xmLabelGadgetClass,
				 WIDGET(dialog), XmNlabelString, string, NULL);
 XmStringFree(string);

 F(dialog).createActionArea(dialog, XcCD_YES|XcCD_NO, XcCD_YES, NULL, NULL);
 XtManageChild(w_box);
 XtManageChild(w_bul);
 XtPopup(w_box, XtGrabExclusive);
 F(dialog).map(dialog, TRUE);
 rc = (F(dialog).waitForUnmap(dialog) == XcCD_YES)? TRUE : FALSE;
 XtDestroyWidget(label); 
 DELETE(c_CustomDialog)(dialog);
 XtDestroyWidget(w_bul);
 XtDestroyWidget(w_box);
 return rc;
}

/* ----------------------------------------------------------------- ** 
** Xc_YesNoAlert - Yes or No alert                                   ** 
** ----------------------------------------------------------------- */
boolean Xc_ExitContinueAlert(text)
char *text;
{
 c_CustomDialog *dialog;
 Widget	label;
 XmString string;
 boolean rc;
 Widget	w_box;
 Widget	w_bul;

 Xc_TRACE(("Xc_ExitContinueAlert"));

 w_box = XtVaCreatePopupShell("alertbox", xmDialogShellWidgetClass, 
			      wMainWindow,
			      XmNdeleteResponse, XmDO_NOTHING,
			      XmNmappedWhenManaged, False,
			      XmNinitialResourcesPersistent, False,
			      NULL);
 w_bul = XtVaCreateWidget("alert_bboard", xmBulletinBoardWidgetClass,
			  w_box,
			  XmNallowShellResize, True,
			  XmNautoUnmanage, False,
			  XmNresizePolicy, XmRESIZE_NONE,
			  XmNallowOverlap, False,
			  XmNwidth, 450, XmNheight, 125,
			  NULL);
 /*XtVaSetValues(w_box, XmNminWidth, 400, XmNmaxWidth, 400, NULL);*/
 XtVaSetValues(w_box, XmNminHeight, 125, XmNmaxHeight, 125, NULL);

 dialog = NEW(c_CustomDialog)(w_bul, "exitcontinue_alert", 450, 125,
			      XcCD_HRESIZE);
 string = XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET);
 label = XtVaCreateManagedWidget("exitcontinue_text", xmLabelGadgetClass,
				 WIDGET(dialog), XmNlabelString, string, NULL);
 XmStringFree(string);

 F(dialog).createActionArea(dialog, XcCD_QUIT|XcCD_CONTINUE, XcCD_QUIT,
			    NULL, NULL);
 XtManageChild(w_box);
 XtManageChild(w_bul);
 XtPopup(w_box, XtGrabExclusive);
 F(dialog).map(dialog, TRUE);
 rc = (F(dialog).waitForUnmap(dialog) == XcCD_QUIT)? TRUE : FALSE;
 XtDestroyWidget(label); 
 DELETE(c_CustomDialog)(dialog);
 XtDestroyWidget(w_bul);
 XtDestroyWidget(w_box);
 return rc;
}

/* ----------------------------------------------------------------- ** 
** Xc_CancelRetryContinueAlert - Cancel Retry or Continue alert      ** 
** ----------------------------------------------------------------- */
int Xc_CancelRetryContinueAlert(text)
char *text;
{
 c_CustomDialog	*dialog;
 Widget		label;
 XmString	string;
 int		rc;
 Widget		w_box;
 Widget		w_bul;

 Xc_TRACE(("Xc_CancelRetryContinueAlert"));

 w_box = XtVaCreatePopupShell("alertbox", xmDialogShellWidgetClass, 
			      wMainWindow,
			      XmNdeleteResponse, XmDO_NOTHING,
			      XmNmappedWhenManaged, False,
			      XmNinitialResourcesPersistent, False,
			      NULL);
 w_bul = XtVaCreateWidget("alert_bboard", xmBulletinBoardWidgetClass,
			  w_box,
			  XmNallowShellResize, True,
			  XmNautoUnmanage, False,
			  XmNresizePolicy, XmRESIZE_NONE,
			  XmNallowOverlap, False,
			  XmNwidth, 400, XmNheight, 125,
			  NULL);
 /*XtVaSetValues(w_box, XmNminWidth, 400, XmNmaxWidth, 400, NULL);*/
 XtVaSetValues(w_box, XmNminHeight, 125, XmNmaxHeight, 125, NULL);

 dialog = NEW(c_CustomDialog)(w_bul, "cancelretry_alert", 400, 125,
			      XcCD_HRESIZE);
 string = XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET);
 label = XtVaCreateManagedWidget("cancelretry_text", xmLabelGadgetClass,
				 WIDGET(dialog), XmNlabelString, string, NULL);
 XmStringFree(string);

 F(dialog).createActionArea(dialog, XcCD_CANCEL|XcCD_RETRY|XcCD_CONTINUE,
			    XcCD_RETRY, NULL, NULL);
 XtManageChild(w_box);
 XtManageChild(w_bul);
 XtPopup(w_box, XtGrabExclusive);
 F(dialog).map(dialog, TRUE);
 rc = F(dialog).waitForUnmap(dialog);
 XtDestroyWidget(label); 
 DELETE(c_CustomDialog)(dialog);
 XtDestroyWidget(w_bul);
 XtDestroyWidget(w_box);
 return rc;
}

/* ----------------------------------------------------------------- ** 
** Xc_CancelRetryAlert - Cancel or Retry alert                       ** 
** ----------------------------------------------------------------- */
int Xc_CancelRetryAlert(text)
char *text;
{
 c_CustomDialog	*dialog;
 Widget		label;
 XmString	string;
 int		rc;
 Widget		w_box;
 Widget		w_bul;

 Xc_TRACE(("Xc_CancelRetryAlert"));

 w_box = XtVaCreatePopupShell("alertbox", xmDialogShellWidgetClass, 
			      wMainWindow,
			      XmNdeleteResponse, XmDO_NOTHING,
			      XmNmappedWhenManaged, False,
			      XmNinitialResourcesPersistent, False,
			      NULL);
 w_bul = XtVaCreateWidget("alert_bboard", xmBulletinBoardWidgetClass,
			  w_box,
			  XmNallowShellResize, True,
			  XmNautoUnmanage, False,
			  XmNresizePolicy, XmRESIZE_NONE,
			  XmNallowOverlap, False,
			  XmNwidth, 400, XmNheight, 125,
			  NULL);
 /*XtVaSetValues(w_box, XmNminWidth, 400, XmNmaxWidth, 400, NULL);*/
 XtVaSetValues(w_box, XmNminHeight, 125, XmNmaxHeight, 125, NULL);

 dialog = NEW(c_CustomDialog)(w_bul, "cancelretry_alert", 400, 125,
			      XcCD_HRESIZE);
 string = XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET);
 label = XtVaCreateManagedWidget("cancelretry_text", xmLabelGadgetClass,
				 WIDGET(dialog), XmNlabelString, string, NULL);
 XmStringFree(string);

 F(dialog).createActionArea(dialog, XcCD_CANCEL | XcCD_RETRY,
			    XcCD_RETRY, NULL, NULL);
 XtManageChild(w_box);
 XtManageChild(w_bul);
 XtPopup(w_box, XtGrabExclusive);
 F(dialog).map(dialog, TRUE);
 rc = F(dialog).waitForUnmap(dialog);
 XtDestroyWidget(label); 
 DELETE(c_CustomDialog)(dialog);
 XtDestroyWidget(w_bul);
 XtDestroyWidget(w_box);
 return rc;
}



/* ----------------------------------------------------------------- ** 
** Xc_OkCancelAlert - Ok and cancel alert box                        ** 
** ----------------------------------------------------------------- */
boolean Xc_OkCancelAlert(text)
char *text;
{
 c_CustomDialog *dialog;
 Widget	label;
 XmString string;
 boolean rc;
 Widget	w_box;
 Widget	w_bul;

 Xc_TRACE(("Xc_OkCancelAlert"));

 w_box = XtVaCreatePopupShell("alertbox", xmDialogShellWidgetClass, 
			      wMainWindow,
			      XmNdeleteResponse, XmDO_NOTHING,
			      XmNmappedWhenManaged, False,
			      XmNinitialResourcesPersistent, False,
			      NULL);
 w_bul = XtVaCreateWidget("alert_bboard", xmBulletinBoardWidgetClass,
			  w_box,
			  XmNallowShellResize, True,
			  XmNautoUnmanage, False,
			  XmNresizePolicy, XmRESIZE_NONE,
			  XmNallowOverlap, False,
			  XmNwidth, 400, XmNheight, 125,
			  NULL);
 /*XtVaSetValues(w_box, XmNminWidth, 400, XmNmaxWidth, 400, NULL);*/
 XtVaSetValues(w_box, XmNminHeight, 125, XmNmaxHeight, 125, NULL);

 dialog = NEW(c_CustomDialog)(w_bul, "okcancel_alert", 400, 125,
			      XcCD_HRESIZE);
 string = XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET);
 label = XtVaCreateManagedWidget("okcancel_text", xmLabelGadgetClass,
				 WIDGET(dialog), XmNlabelString, string, NULL);
 XmStringFree(string);

 F(dialog).createActionArea(dialog, XcCD_OK|XcCD_CANCEL, XcCD_OK, NULL, NULL);
 XtManageChild(w_box);
 XtManageChild(w_bul);
 XtPopup(w_box, XtGrabExclusive);
 F(dialog).map(dialog, TRUE);
 rc = (F(dialog).waitForUnmap(dialog) == XcCD_OK)? TRUE : FALSE;
 XtDestroyWidget(label); 
 DELETE(c_CustomDialog)(dialog);
 XtDestroyWidget(w_bul);
 XtDestroyWidget(w_box);
 return rc;
}


/* ----------------------------------------------------------------- ** 
** Xc_SyserrorAlert - System error alert                             ** 
** ----------------------------------------------------------------- */
void Xc_SyserrorAlert(format, va_alist)
char *format;
va_dcl
{
 static char buffer[512];
 c_CustomDialog *dialog;
 Widget label;
 XmString string;
 va_list ap;
 long size;
 Widget	w_box;
 Widget	w_bul;

 Xc_TRACE(("Xc_SyserrorAlert"));

 va_start(ap);
 vsprintf(buffer, format, ap);
 va_end(ap);
 size = strlen(buffer);
 buffer[size] = '\n';
 strcpy(buffer + size + 1, Xc_strerror(errno));

 w_box = XtVaCreatePopupShell("alertbox", xmDialogShellWidgetClass, 
			      wMainWindow,
			      XmNdeleteResponse, XmDO_NOTHING,
			      XmNmappedWhenManaged, False,
			      XmNinitialResourcesPersistent, False,
			      NULL);
 w_bul = XtVaCreateWidget("alert_bboard", xmBulletinBoardWidgetClass,
			  w_box,
			  XmNallowShellResize, True,
			  XmNautoUnmanage, False,
			  XmNresizePolicy, XmRESIZE_NONE,
			  XmNallowOverlap, False,
			  XmNwidth, 400, XmNheight, 125,
			  NULL);
 /*XtVaSetValues(w_box, XmNminWidth, 400, XmNmaxWidth, 400, NULL);*/
 XtVaSetValues(w_box, XmNminHeight, 125, XmNmaxHeight, 125, NULL);

 dialog = NEW(c_CustomDialog)(w_bul, "syserror_alert", 400, 125,
			      XcCD_HRESIZE|XcCD_VRESIZE);
 string = XmStringCreateLtoR(buffer, XmSTRING_DEFAULT_CHARSET);
 label = XtVaCreateManagedWidget("syserror_text", xmLabelGadgetClass,
				 WIDGET(dialog), XmNlabelString, string, NULL);
 XmStringFree(string);

 F(dialog).createActionArea(dialog, XcCD_OK, XcCD_OK, NULL, NULL);
 XtManageChild(w_box);
 XtManageChild(w_bul);
 XtPopup(w_box, XtGrabExclusive);
 F(dialog).map(dialog, TRUE);
 F(dialog).waitForUnmap(dialog);

 XtDestroyWidget(label); 

 XtPopdown(w_box);
 DELETE(c_CustomDialog)(dialog);
 XtDestroyWidget(w_bul);
 XtDestroyWidget(w_box);
}

/* ----------------------------------------------------------------- ** 
** Xc_SigErrorAlert - Signal error alert                             ** 
** ----------------------------------------------------------------- */
int Xc_SigErrorAlert(mtext, save)
XmString mtext;
boolean	save;
{
 Arg	argsw[10];
 int	nargs;
 int	rc;	 
 c_CustomDialog *dialog;
 Widget	Label1;
 Widget	Label2;
 int		button, def_button;
 Widget	w_box;
 Widget	w_bul;
  
 Xc_TRACE(("Xc_SigErrorAlert"));

 w_box = XtVaCreatePopupShell("alertbox", xmDialogShellWidgetClass, 
			      wMainWindow,
			      XmNdeleteResponse, XmDO_NOTHING,
			      XmNmappedWhenManaged, False,
			      XmNinitialResourcesPersistent, False,
			      NULL);
 w_bul = XtVaCreateWidget("alert_bboard", xmBulletinBoardWidgetClass,
			  w_box,
			  XmNallowShellResize, True,
			  XmNautoUnmanage, False,
			  XmNresizePolicy, XmRESIZE_NONE,
			  XmNnoResize, False,
			  XmNwidth, 400, XmNheight, 125,
			  XmNallowOverlap, False,
			  NULL);
 /*XtVaSetValues(w_box, XmNminWidth, 400, XmNmaxWidth, 400, NULL);*/
 XtVaSetValues(w_box, XmNminHeight, 125, XmNmaxHeight, 125, NULL);
 dialog = NEW(c_CustomDialog)(w_bul, "sigerror_alert", 400, 125,
			      XcCD_VERTICAL | XcCD_HRESIZE);
  
 Label1 =  XmCreateLabel(WIDGET(dialog), "lCaughtSignal", NULL, 0);
 nargs = 0;
 XtSetArg(argsw[nargs], XmNlabelString, mtext);
 nargs++;
 Label2 =  XmCreateLabel(WIDGET(dialog), "lSignalType", argsw, nargs);
  
 XtManageChild(Label1);
 XtManageChild(Label2);
  
 if (save) 
  def_button = XcCD_SAVE; 
 else 
  def_button = XcCD_QUIT;
  
 button = XcCD_QUIT|XcCD_CONTINUE;
 if (save) button |= XcCD_SAVE;
  
 F(dialog).createActionArea(dialog, button, def_button, NULL, NULL);
  
 XtManageChild(w_box);
 XtManageChild(w_bul);
 F(dialog).map(dialog, TRUE);
 XtPopup(w_box, XtGrabNonexclusive);

 rc = F(dialog).waitForUnmap(dialog);

 XtPopdown(w_box);

 XtDestroyWidget(Label1); 
 XtDestroyWidget(Label2);
 DELETE(c_CustomDialog)(dialog);

 XtDestroyWidget(w_bul);
 XtDestroyWidget(w_box);
  
 Xc_TRACE(("returning button : %d", rc));
 return rc;
}

/* ----------------------------------------------------------------- ** 
** Xc_IOErrorAlert - IO error alert                                  ** 
** ----------------------------------------------------------------- */
int Xc_IOErrorAlert(save)
boolean save;
{
 int	rc;	 
 c_CustomDialog *dialog;
 Widget	Label1;
 Widget	Label2;
 int		button, def_button;
 Widget	w_box;
 Widget	w_bul;
  
 Xc_TRACE(("Xc_IOErrorAlert"));

 w_box = XtVaCreatePopupShell("alertbox", xmDialogShellWidgetClass, 
			      wMainWindow,
			      XmNdeleteResponse, XmDO_NOTHING,
			      XmNmappedWhenManaged, False,
			      XmNinitialResourcesPersistent, False,
			      NULL);
 w_bul = XtVaCreateWidget("alert_bboard", xmBulletinBoardWidgetClass,
			  w_box,
			  XmNallowShellResize, True,
			  XmNautoUnmanage, False,
			  XmNresizePolicy, XmRESIZE_NONE,
			  XmNallowOverlap, False,
			  XmNwidth, 400, XmNheight, 125,
			  NULL);
 /*XtVaSetValues(w_box, XmNminWidth, 400, XmNmaxWidth, 400, NULL);*/
 XtVaSetValues(w_box, XmNminHeight, 125, XmNmaxHeight, 125, NULL);
 dialog = NEW(c_CustomDialog)(w_bul, "syserror_alert", 400, 125,
			      XcCD_VERTICAL | XcCD_HRESIZE);
  
 Label1 =  XmCreateLabel(WIDGET(dialog), "lCaughtIOerror", NULL, 0);
 Label2 =  XmCreateLabel(WIDGET(dialog), "lCannotContinue", NULL, 0);
  
 XtManageChild(Label1);
 XtManageChild(Label2);
  
 if (save) 
  def_button = XcCD_SAVE; 
 else 
  def_button = XcCD_QUIT;
  
 button = XcCD_QUIT;
 if (save) button |= XcCD_SAVE;
  
 F(dialog).createActionArea(dialog, button, def_button, NULL, NULL);
  
 XtManageChild(w_box);
 XtManageChild(w_bul);
 XtPopup(w_box, XtGrabExclusive);
 F(dialog).map(dialog, TRUE);
 rc = F(dialog).waitForUnmap(dialog);
 XtDestroyWidget(Label1); 
 XtDestroyWidget(Label2); 
  
 XtPopdown(w_box);
 DELETE(c_CustomDialog)(dialog);
 XtDestroyWidget(w_bul);
 XtDestroyWidget(w_box);
  
 Xc_TRACE(("returning button : %d", rc));
 return rc;
}

