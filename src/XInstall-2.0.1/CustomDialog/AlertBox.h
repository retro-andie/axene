/*
** AlertBox.h for XInstall in CustomDialog/
** Declaration for the simple alert boxes
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
** Started on  Fri Dec  2 13:29:39 1994 St�phane Boisson
** Last update Tue Dec 30 19:32:14 1997 Emmanuel Paris
*/

#ifndef __XC_ALERT_BOX_H__
#define __XC_ALERT_BOX_H__

void Xc_ErrorAlert ___PROTO((char *text));
void Xc_ErrorAlert2 ___PROTO((XmString mtext));
boolean Xc_ExitContinueAlert ___PROTO((char *text));
boolean Xc_YesNoAlert ___PROTO((char *text));
int Xc_CancelRetryContinueAlert ___PROTO((char *text));
int Xc_CancelRetryAlert ___PROTO((char *text));
boolean Xc_OkCancelAlert ___PROTO((char *text));
void Xc_SyserrorAlert ___PROTO((char *format, ...));
int Xc_SigErrorAlert ___PROTO((XmString mtext, boolean save));     
int Xc_IOErrorAlert ___PROTO((boolean save));     
int Xc_CloseDocAlert ___PROTO((char *doc_name, char *file_name));
int Xc_SaveDocAlert ___PROTO((char *doc_name, char *file_name));

#endif /* !__XC_ALERT_BOX_H__ */







