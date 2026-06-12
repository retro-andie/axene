/*
** AlertBox.c for Xclamation, XQuad, XAllWrite, XMayday and AxeneOffice in 
** 	CustomDialog/
** Simple warnings dialogs
**
** Copyright (C) 1994-2000 Axene.
** Authors: St�phane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
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
** Started on  Fri Dec  2 13:17:46 1994 St�phane Boisson
** Last update Tue Jan 20 17:00:55 1998 Robin Castanier
*/

#define ___ALERT_BOX

#include <stdio.h>
#include <stdarg.h>
#include "CustomDialog.h"
#include "MainInterface.h"
#include <Xm/Label.h>
#include <Xm/LabelG.h>

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
  
  
 dialog = NEW(c_CustomDialog)(wMainWindow, "error_alert", 300, 125,
			      XcCD_HRESIZE);
 label = XtVaCreateManagedWidget("error_text", xmLabelGadgetClass,
				 WIDGET(dialog), XmNlabelString, mtext, NULL);
  
 XtVaGetValues(label, XmNwidth, &width, XmNheight, &height, NULL);
  
 if (width > 300 || height > 100)
 {
  XtDestroyWidget(label);
  DELETE(c_CustomDialog)(dialog);
  dialog = NEW(c_CustomDialog)(wMainWindow, "error_alert", 
			       MAX(300, width+20), MAX(125, height),
			       XcCD_HRESIZE);
  label = 
   XtVaCreateManagedWidget("error_text", xmLabelGadgetClass,
			   WIDGET(dialog), XmNlabelString, mtext, NULL);
    
 }
  
 F(dialog).createActionArea(dialog, XcCD_OK, XcCD_OK, FALSE, NULL, NULL);
 F(dialog).map(dialog, TRUE);
 F(dialog).waitForUnmap(dialog);
 XtDestroyWidget(label); 
 DELETE(c_CustomDialog)(dialog);
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

 Xc_TRACE(("Xc_YesNoAlert"));

 dialog = NEW(c_CustomDialog)(wMainWindow, "yesno_alert", 300, 125,
			      XcCD_HRESIZE);
 string = XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET);
 label = XtVaCreateManagedWidget("yesno_text", xmLabelGadgetClass,
				 WIDGET(dialog), XmNlabelString, string, NULL);
 XmStringFree(string);

 F(dialog).createActionArea(dialog, XcCD_YES|XcCD_NO, XcCD_YES, FALSE, NULL, NULL);
 F(dialog).map(dialog, TRUE);
 rc = (F(dialog).waitForUnmap(dialog) == XcCD_YES)? TRUE : FALSE;
 XtDestroyWidget(label); 
 DELETE(c_CustomDialog)(dialog);

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

 Xc_TRACE(("Xc_OkCancelAlert"));

 dialog = NEW(c_CustomDialog)(wMainWindow, "okcancel_alert", 300, 200,
			      XcCD_HRESIZE);
 string = XmStringCreateLtoR(text, XmSTRING_DEFAULT_CHARSET);
 label = XtVaCreateManagedWidget("okcancel_text", xmLabelGadgetClass,
				 WIDGET(dialog), XmNlabelString, string, NULL);
 XmStringFree(string);

 F(dialog).createActionArea(dialog, XcCD_OK|XcCD_CANCEL, XcCD_OK, FALSE, NULL, NULL);
 F(dialog).map(dialog, TRUE);
 rc = (F(dialog).waitForUnmap(dialog) == XcCD_OK)? TRUE : FALSE;
 XtDestroyWidget(label); 
 DELETE(c_CustomDialog)(dialog);

 return rc;
}


/* ----------------------------------------------------------------- ** 
** Xc_SyserrorAlert - System error alert                             ** 
** ----------------------------------------------------------------- */
void Xc_SyserrorAlert(char *format, ...)
{
 static char buffer[512];
 c_CustomDialog *dialog;
 Widget label;
 XmString string;
 va_list ap;
 long size;

 Xc_TRACE(("Xc_SyserrorAlert"));

 va_start(ap, format);
 vsprintf(buffer, format, ap);
 va_end(ap);
 size = strlen(buffer);
 buffer[size] = '\n';
 strcpy(buffer + size + 1, Xc_strerror(errno));

 dialog = NEW(c_CustomDialog)(wMainWindow, "syserror_alert", 400, 125,
			      XcCD_HRESIZE|XcCD_VRESIZE);
 string = XmStringCreateLtoR(buffer, XmSTRING_DEFAULT_CHARSET);
 label = XtVaCreateManagedWidget("syserror_text", xmLabelGadgetClass,
				 WIDGET(dialog), XmNlabelString, string, NULL);
 XmStringFree(string);

 F(dialog).createActionArea(dialog, XcCD_OK, XcCD_OK, FALSE, NULL, NULL);
 F(dialog).map(dialog, TRUE);
 F(dialog).waitForUnmap(dialog);
 XtDestroyWidget(label); 
 DELETE(c_CustomDialog)(dialog);
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
  
 Xc_TRACE(("Xc_SigErrorAlert"));

 dialog = NEW(c_CustomDialog)(wMainWindow, "syserror_alert", 400, 125,
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
  
 F(dialog).createActionArea(dialog, button, def_button, FALSE, NULL, NULL);
  
 F(dialog).map(dialog, TRUE);
 rc = F(dialog).waitForUnmap(dialog);
 XtDestroyWidget(Label1); 
 XtDestroyWidget(Label2); 
 DELETE(c_CustomDialog)(dialog);
  
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
  
 Xc_TRACE(("Xc_IOErrorAlert"));

 dialog = NEW(c_CustomDialog)(wMainWindow, "syserror_alert", 400, 125,
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
  
 F(dialog).createActionArea(dialog, button, def_button, FALSE, NULL, NULL);
  
 F(dialog).map(dialog, TRUE);
 rc = F(dialog).waitForUnmap(dialog);
 XtDestroyWidget(Label1); 
 XtDestroyWidget(Label2); 
  
 DELETE(c_CustomDialog)(dialog);
  
 Xc_TRACE(("returning button : %d", rc));
 return rc;
}

/* ----------------------------------------------------------------- ** 
** Xc_CloseDocAlert - Closing document alert                         ** 
** ----------------------------------------------------------------- */
int Xc_CloseDocAlert(doc_name, file_name)
char	*doc_name;
char	*file_name;
{
 Arg	argsw[10];
 int	nargs;
 int	rc;	 
 c_CustomDialog *dialog;
 XmString string;
 Widget	Label1;
 Widget	Label2;
 Widget	Label3;
 Widget	Label4;
 int		button, def_button;
  
 Xc_TRACE(("Xc_CloseDocAlert"));

 if (file_name)
  dialog = NEW(c_CustomDialog)(wMainWindow, "closedoc_alert", 400, 180,
			       XcCD_VERTICAL | XcCD_HRESIZE);
 else
  dialog = NEW(c_CustomDialog)(wMainWindow, "closedoc_alert", 400, 125,
			       XcCD_VERTICAL | XcCD_HRESIZE);

 Label1 =  XmCreateLabel(WIDGET(dialog), "lCloseDocument", NULL, 0);
 string = XmStringCreateLtoR(doc_name, XmSTRING_DEFAULT_CHARSET);
 nargs = 0;
 XtSetArg(argsw[nargs], XmNlabelString, string);
 nargs++;
 Label2 =  XmCreateLabel(WIDGET(dialog), "lDocumentName", argsw, nargs);
 XmStringFree(string);

 if (file_name)
 {
  Label3 =  XmCreateLabel(WIDGET(dialog), "lSavingName",
			  NULL, 0);
  string = XmStringCreateLtoR(file_name, XmSTRING_DEFAULT_CHARSET);
  nargs = 0;
  XtSetArg(argsw[nargs], XmNlabelString, string);
  nargs++;
  Label4 =  XmCreateLabel(WIDGET(dialog), "lFileName",  argsw, nargs);
      
  XtManageChild(Label1);
  XtManageChild(Label2);
  XtManageChild(Label3);
  XtManageChild(Label4);
      
  def_button = XcCD_SAVE; 
  button = XcCD_OK | XcCD_CANCEL | XcCD_SAVE;
 }
 else
 {
  XtManageChild(Label1);
  XtManageChild(Label2);
      
  def_button = XcCD_SAVE_AS; 
  button = XcCD_OK | XcCD_CANCEL | XcCD_SAVE_AS;
 }
 F(dialog).createActionArea(dialog, button, def_button, FALSE, NULL, NULL);
  
 F(dialog).map(dialog, TRUE);
 rc = F(dialog).waitForUnmap(dialog);
 DELETE(c_CustomDialog)(dialog);
  
 Xc_TRACE(("returning button : %d", rc));
 return rc;
}
/* ----------------------------------------------------------------- ** 
** Xc_SaveDocAlert - Saving document alert                           ** 
** ----------------------------------------------------------------- */
int Xc_SaveDocAlert(doc_name, file_name)
char	*doc_name;
char	*file_name;
{
 Arg	argsw[10];
 int	nargs;
 int	rc;	 
 c_CustomDialog *dialog;
 XmString string;
 Widget	Label1;
 Widget	Label2;
 Widget	Label3;
 Widget	Label4;
 int		button, def_button;
  
 Xc_TRACE(("Xc_CloseDocAlert"));

 if (file_name)
  dialog = NEW(c_CustomDialog)(wMainWindow, "savedoc_alert", 500, 180,
			       XcCD_VERTICAL | XcCD_HRESIZE);
 else
  dialog = NEW(c_CustomDialog)(wMainWindow, "savedoc_alert", 500, 125,
			       XcCD_VERTICAL | XcCD_HRESIZE);
  
 Label1 =  XmCreateLabel(WIDGET(dialog), "lSaveDocument",
			 NULL, 0);
 string = XmStringCreateLtoR(doc_name, XmSTRING_DEFAULT_CHARSET);
 nargs = 0;
 XtSetArg(argsw[nargs], XmNlabelString, string);
 nargs++;
 Label2 =  XmCreateLabel(WIDGET(dialog), "lDocumentName", argsw, nargs);
 XmStringFree(string);

 if (file_name)
 {
  Label3 =  XmCreateLabel(WIDGET(dialog), "lSavingName",
			  NULL, 0);
  string = XmStringCreateLtoR(file_name, XmSTRING_DEFAULT_CHARSET);
  nargs = 0;
  XtSetArg(argsw[nargs], XmNlabelString, string);
  nargs++;
  Label4 =  XmCreateLabel(WIDGET(dialog), "lFileName",  argsw, nargs);
      
  XtManageChild(Label1);
  XtManageChild(Label2);
  XtManageChild(Label3);
  XtManageChild(Label4);
      
  def_button = XcCD_YES; 
  button = XcCD_YES | XcCD_NO | XcCD_CANCEL | XcCD_SAVE_AS;
 }
 else
 {
  XtManageChild(Label1);
  XtManageChild(Label2);
      
  def_button = XcCD_YES; 
  button = XcCD_YES | XcCD_NO |  XcCD_CANCEL;
 }
 F(dialog).createActionArea(dialog, button, def_button, FALSE, NULL, NULL);
  
 F(dialog).map(dialog, TRUE);
 rc = F(dialog).waitForUnmap(dialog);
 DELETE(c_CustomDialog)(dialog);
  
 Xc_TRACE(("returning button : %d", rc));
 return rc;
}







