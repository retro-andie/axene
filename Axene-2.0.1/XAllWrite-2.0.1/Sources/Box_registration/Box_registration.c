/*
** Box_registration.c for Xclamation, XQuad and XAllWrite in Box_registration/
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
** Started on  Wed Feb  7 16:41:07 1996 Robin Castanier
** Last update Mon Aug  9 23:16:46 1999 Emmanuel Paris
*/

#include "Box_registration.h"
#include <sys/file.h>

void *cons_Box_registration();
void dest_Box_registration();
void callback_dest_BoxRegistration();

static void cb_action ___PROTO((c_CustomDialog *cb_dialog,
				c_Box_registration *box, int cb_reason));
static void cb_register_prod1 ___PROTO((Widget This,
					c_Box_registration *Obj));
static void cb_register_prod2 ___PROTO((Widget This,
					c_Box_registration *Obj));
static void cb_register_prod3 ___PROTO((Widget This,
					c_Box_registration *Obj));
static void certify_mail_command ___PROTO((c_WTextField *this,
					   c_Box_registration *Obj));
static void certify_reply_adress ___PROTO((c_WTextField *this,
					   c_Box_registration *Obj));

sf_Box_registration fc_Box_registration =
{
  cons_Box_registration,
  dest_Box_registration,
};

void *cons_Box_registration(w_Parent, pszTitle)
Widget w_Parent;
char   *pszTitle;
{
  Display        *display;
  c_Box_registration	 *ObjTmp;
  listbutton	*buttonptr;
  
  if ((ObjTmp = (c_Box_registration *)Xc_malloc("Box_registration",
					 sizeof(c_Box_registration))) == NULL)
    {
      Xc_FATAL(("Can't build this object: memory allocation error"));
    }
  ObjTmp->f = &fc_Box_registration;
  ObjTmp->register_item = BR_REGISTER_NONE;
  strcpy(ObjTmp->mail_command, "mail majordomo@axene.org");
  ObjTmp->reply_adress[0] = '\0';
  
  ObjTmp->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
							 pszTitle, 420, 550,
							 XcCD_HORIZONTAL);
  ObjTmp->w_This = WIDGET(ObjTmp->Dialog);

  ObjTmp->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp),
				       MANAGER(ObjTmp->Dialog),
				       "gBoxRegistration", FALSE);
  
  display = XtDisplay(w_Parent);

  ObjTmp->lBRegisterTitle = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->Grid),
						    MANAGER(ObjTmp->Dialog),
						    "lBoxRegisterTitle" );
  
  ObjTmp->fText = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					  MANAGER(ObjTmp->Dialog),
					  NULL, 0);
  ObjTmp->sText = 
    (c_ScrolledText *)NEW(c_ScrolledText)(WIDGET(ObjTmp->fText),
					      MANAGER(ObjTmp->Dialog),
					      "sBRegisterText");  
 
  ObjTmp->fAdress = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					    MANAGER(ObjTmp->Dialog), 
					    NULL, 0);
  ObjTmp->lAdress = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->fAdress),
					    MANAGER(ObjTmp->Dialog),
					    "lBRegisterAdress" );

  ObjTmp->fRegistered = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					       MANAGER(ObjTmp->Dialog), 
					       "fBRRegistered", 0);
  ObjTmp->rbRegistered = 
    (c_RadioBox *)NEW(c_RadioBox)(WIDGET(ObjTmp->fRegistered),
				  NULL, RADIO_NORMAL,
				  "rbRegistered",
				  "rbRegisterAnnounces",
				  "rbRegisterBugs",
				  "rbRegisterUsers",
				  NULL);

  buttonptr = ObjTmp->rbRegistered->Button;
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
  buttonptr = buttonptr->Next;
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
  buttonptr = buttonptr->Next;
  XmToggleButtonGadgetSetState(buttonptr->w_Buttons, False, False);
  
  F(ObjTmp->rbRegistered).AddCallback(ObjTmp->rbRegistered, 0,
				      XmNvalueChangedCallback,
				      cb_register_prod1, ObjTmp);
  F(ObjTmp->rbRegistered).AddCallback(ObjTmp->rbRegistered, 1,
				      XmNvalueChangedCallback,
				      cb_register_prod2, ObjTmp);
  F(ObjTmp->rbRegistered).AddCallback(ObjTmp->rbRegistered, 2,
				      XmNvalueChangedCallback,
				      cb_register_prod3, ObjTmp);
  
  ObjTmp->fCommand = (c_Frame *)NEW(c_Frame)(WIDGET(ObjTmp->Grid),
					     MANAGER(ObjTmp->Dialog), 
					     "fBRegisterCommand", 0);


  ObjTmp->gCommand = (c_Grid *)NEW(c_Grid)(WIDGET(ObjTmp->fCommand),
					   MANAGER(ObjTmp->Dialog),
					   "gBRegisterCommand", TRUE);
  
  ObjTmp->lMailCommand = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->gCommand),
						 MANAGER(ObjTmp->Dialog),
						 "lMailCommand");
  
  ObjTmp->tfMailCommand = NEW(c_WTextField)(WIDGET(ObjTmp->gCommand),
					    MANAGER(ObjTmp->Dialog),
					    "tfMailCommand",
					    TF_STRING, 0.0, 100.0,
					    ObjTmp->mail_command, NULL, 22);
  F(ObjTmp->tfMailCommand).add_callback
    (ObjTmp->tfMailCommand, (void *)certify_mail_command, ObjTmp);
  
  ObjTmp->lReplyAdress = (c_Label *)NEW(c_Label)(WIDGET(ObjTmp->gCommand),
						 MANAGER(ObjTmp->Dialog),
						 "lRegisterReply");

  ObjTmp->tfReplyAdress = NEW(c_WTextField)(WIDGET(ObjTmp->gCommand),
					    MANAGER(ObjTmp->Dialog),
					    "tfRegisterReply",
					    TF_STRING, 0.0, 100.0,
					    ObjTmp->reply_adress, NULL, 22);
  
  F(ObjTmp->tfReplyAdress).add_callback
    (ObjTmp->tfReplyAdress, (void *)certify_reply_adress, ObjTmp);
  
  F(ObjTmp->gCommand).addColumn(ObjTmp->gCommand, XcG_BEST_SIZE,
				WIDGET(ObjTmp->lMailCommand), XcG_MAX_SIZE,
				WIDGET(ObjTmp->tfMailCommand), XcG_BEST_SIZE,
				NULL);
  
  F(ObjTmp->gCommand).addColumn(ObjTmp->gCommand,  XcG_BEST_SIZE,
				WIDGET(ObjTmp->lReplyAdress), XcG_MAX_SIZE,
				WIDGET(ObjTmp->tfReplyAdress), XcG_BEST_SIZE,
				NULL);
    
  F(ObjTmp->Grid).addColumn(ObjTmp->Grid, XcG_MAX_SIZE,
			    WIDGET(ObjTmp->lBRegisterTitle), 30,
			    WIDGET(ObjTmp->fText), XcG_MAX_SIZE,
			    WIDGET(ObjTmp->fAdress), XcG_BEST_SIZE,
			    WIDGET(ObjTmp->fRegistered), 100,
			    WIDGET(ObjTmp->fCommand), 100,
			    NULL);

  F(ObjTmp->Dialog).createActionArea(ObjTmp->Dialog, 
				     XcCD_OK | XcCD_CANCEL, XcCD_OK,
				     FALSE, (void (*)())cb_action, ObjTmp);
  
  F(ObjTmp->Dialog).map(ObjTmp->Dialog, TRUE);

  XmTextInsert(WIDGET(ObjTmp->sText), 0, 
	       F(GlobResources).getString(GlobResources,
					  XcR_mailingList));
  XmTextSetInsertionPosition(WIDGET(ObjTmp->sText), 0);

  Xc_TRACE(("Object Box_registration build"));
  return ObjTmp;
}

void dest_Box_registration(This)
c_Box_registration *This;
{
  
  DELETE(c_Label)(This->lBRegisterTitle);
  
  DELETE(c_ScrolledText)(This->sText);
  DELETE(c_Frame)(This->fText);
  
  DELETE(c_Label)(This->lAdress);
  DELETE(c_Frame)(This->fAdress);
  
  DELETE(c_RadioBox)(This->rbRegistered);
  DELETE(c_Frame)(This->fRegistered);
  
  DELETE(c_Label)(This->lReplyAdress);
  DELETE(c_WTextField)(This->tfReplyAdress);
  DELETE(c_Label)(This->lMailCommand);
  DELETE(c_WTextField)(This->tfMailCommand);
  DELETE(c_Grid)(This->gCommand);
  DELETE(c_Frame)(This->fCommand);
  
  DELETE(c_Grid)(This->Grid);
  DELETE(c_CustomDialog)(This->Dialog);
  
  Xc_free(This);
  Xc_TRACE(("Object Box_registration destroyed"));
}

static void cb_action(cb_dialog, box, cb_reason)
c_CustomDialog		*cb_dialog;
c_Box_registration	*box;
int			cb_reason;
{
  FILE	*file;
  char	buffer[200];
  
  F(cb_dialog).unmap(cb_dialog);
  if (cb_reason == XcCD_OK)
    {
      if (box->register_item != BR_REGISTER_NONE)
	{
	  file = popen(box->mail_command, "w");
	  if (file == NULL)
	  {
	    Xc_SYSERROR(("Can't open file"));
	    return;
	  }
	  
	  buffer[0] = '\0';
	  if ((box->register_item & BR_REGISTER_PROD1)
	      == BR_REGISTER_PROD1)
	  {
	    strcat(buffer, BR_SUBSCRIBE_KEYWORD);
	    strcat(buffer, BR_REG_PROD1);
	    if (*box->reply_adress)
	      strcat(buffer, box->reply_adress);
	    strcat(buffer, "\n");
	  }
	  
	  if ((box->register_item & BR_REGISTER_PROD2)
	      == BR_REGISTER_PROD2)
	  {
	    strcat(buffer, BR_SUBSCRIBE_KEYWORD);
	    strcat(buffer, BR_REG_PROD2);
	    if (*box->reply_adress)
	      strcat(buffer, box->reply_adress);
	    strcat(buffer, "\n");
	  }
	  
	  if ((box->register_item & BR_REGISTER_PROD3)
	      == BR_REGISTER_PROD3)
	  {
	    strcat(buffer, BR_SUBSCRIBE_KEYWORD);
	    strcat(buffer, BR_REG_PROD3);
	    if (*box->reply_adress)
	      strcat(buffer, box->reply_adress);
	    strcat(buffer, "\n");
	  }
	  
	  if (fwrite(buffer, strlen(buffer), 1, file) != 1)
	    {
	      Xc_SYSERROR(("Can't write in file"));
	      return;
	    }
	  pclose(file);
	}
      else
	{
	  Xc_TRACE(("No subscibe"));
	  return;
	}
    }
  else
    {
      Xc_TRACE(("canceled"));
    }
}


static void cb_register_prod1(This, Obj)
Widget This;
c_Box_registration *Obj;
{
  Obj->register_item ^= BR_REGISTER_PROD1;
  Xc_TRACE(("Will %sregister for Mailing list 1",  
	    Obj->register_item & BR_REGISTER_PROD1?"":"not ")); 
}

static void cb_register_prod2(This, Obj)
Widget This;
c_Box_registration *Obj;
{
  Obj->register_item ^= BR_REGISTER_PROD2;
  Xc_TRACE(("Will %sregister for Mailing list 2",  
	    Obj->register_item & BR_REGISTER_PROD2?"":"not ")); 
}

static void cb_register_prod3(This, Obj)
Widget This;
c_Box_registration *Obj;
{
  Obj->register_item ^= BR_REGISTER_PROD3;
  Xc_TRACE(("Will %sregister for Mailing list 3",  
	    Obj->register_item & BR_REGISTER_PROD3?"":"not ")); 
}

static void certify_mail_command(this, Obj)
c_WTextField *this;
c_Box_registration *Obj;
{
  char	*vstr;
  
  if((vstr = F(this).get(this)) != NULL)
  {
    strcpy(Obj->mail_command, vstr);
    Xc_free(vstr);
    F(Obj->Dialog).resetTraversal(Obj->Dialog);
  }
}

static void certify_reply_adress(this, Obj)
c_WTextField *this;
c_Box_registration *Obj;
{
  char	*vstr;
  
  if((vstr = F(this).get(this)) != NULL)
  {
    strcpy(Obj->reply_adress, vstr);
    Xc_free(vstr);
    F(Obj->Dialog).resetTraversal(Obj->Dialog);
  }
}
