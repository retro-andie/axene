/*
** Scale.c for Xclamation, XQuad and XAllWrite in Scale/
** Object and method for class Scale
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
** Started on  Wed Dec  7 16:16:59 1994 Antoine Buat
** Last update Wed Jan 21 01:03:59 1998 Emmanuel Paris
*/

#include "Scale.h"
#include "RegisterHelp.h"

void *cons_Scale();
void dest_Scale();
void *copy_Scale();
void scale_update_value ___PROTO((Widget w, c_Scale *this,
				  XmScaleCallbackStruct	*cbs));
void scale_set_value ___PROTO((c_Scale *this, int *value));
void scale_change_value ___PROTO((c_Scale *this, int val));
void scale_set_drag_callback ___PROTO((c_Scale *this,
				       void (*func)(), void *data));
void scale_set_valuechange_callback ___PROTO((c_Scale *this,
					      void (*func)(), void *data));

sf_Scale fc_Scale =
{
 cons_Scale,
 dest_Scale,
 copy_Scale,
 scale_set_value,
 scale_change_value,
 scale_set_drag_callback,
 scale_set_valuechange_callback
};

/* ----------------------------------------------------------------- ** 
** constructor                                                       ** 
** ----------------------------------------------------------------- */
void *cons_Scale(w_Parent, ManageW, Title, iMinimum, iValue, iMaximum, cf, icf)
Widget		w_Parent;
c_ManageWidget	*ManageW;
char		*Title;
int		iMinimum;
int		*iValue;
int		iMaximum;
int		(*cf)();
int		(*icf)();
{
 c_Scale *This;
 Arg     argsw[4];
 
 if ((This = (c_Scale *)Xc_malloc("Scale", sizeof(c_Scale))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 memset(This, 0, sizeof(c_Scale));

 This->f = &fc_Scale;
 This->value = iValue;
 This->cf = cf;
 This->icf = icf;

 XtSetArg(argsw[0], XmNminimum, iMinimum);
 XtSetArg(argsw[1], XmNmaximum, iMaximum);
 XtSetArg(argsw[2], XmNvalue, iMinimum);
 This->w_This = XmCreateScale(w_Parent, Title, argsw, 3);
 Xc_REGISTER_HELP(This->w_This);

 if (iValue)
  F(This).set(This, iValue);
 
 if (ManageW)
  F(ManageW).Add_child(ManageW, This->w_This, w_Parent);
  
 XtAddCallback(This->w_This, XmNvalueChangedCallback, 
	       (XtCallbackProc)scale_update_value, This);
 XtAddCallback(This->w_This, XmNdragCallback, 
	       (XtCallbackProc)scale_update_value, This);
  
 Xc_TRACE(("Object Scale build"));
 return This;
}

/* ----------------------------------------------------------------- ** 
** destructor                                                        ** 
** ----------------------------------------------------------------- */
void dest_Scale(This)
c_Scale *This;
{
 XtDestroyWidget(This->w_This);
 Xc_free(This);
 Xc_TRACE(("Object Scale destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copy                                                              ** 
** ----------------------------------------------------------------- */
void *copy_Scale(This)
c_Scale *This;
{
 c_Scale *ObjTmp;

 if ((ObjTmp = (c_Scale *)Xc_malloc("CScale", sizeof(c_Scale))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }

 memcpy(ObjTmp, This, sizeof(c_Scale));
 Xc_TRACE(("Object Scale copied"));
 return ObjTmp;
}

/* ----------------------------------------------------------------- ** 
** scale_update_value - update value in Widget                       ** 
** ----------------------------------------------------------------- */
void scale_update_value(w, this, cbs)
Widget			w;
c_Scale			*this;
XmScaleCallbackStruct	*cbs;
{
 if (this->value)
 {
  if (this->icf)
   *this->value = (int)this->icf(cbs->value);
  else
   *this->value = (int)cbs->value;
 }
}

/* ----------------------------------------------------------------- ** 
** scale_change_value - change value in Widget                       ** 
** ----------------------------------------------------------------- */
void scale_change_value(this, val)
c_Scale	*this;
int	val;
{
 Arg		argsw[1];

 if (this->value)
 {
  if (this->icf)
   *this->value = (int)this->icf(val);
  else
   *this->value = val;
 }
 XtSetArg(argsw[0], XmNvalue, val);
 XtSetValues(WIDGET(this), argsw, 1);
}


/* ----------------------------------------------------------------- ** 
** scale_set_value - set value in Widget                             ** 
** ----------------------------------------------------------------- */
void scale_set_value(this, value)
c_Scale	*this;
int	*value;
{
 Arg	argsw[1];
 int	val;
  
 this->value = value;
 if (this->cf)
  val = this->cf(*value);
 else
  val = *value; 

 XtSetArg(argsw[0], XmNvalue, val);
 XtSetValues(WIDGET(this), argsw, 1);
}

/* ----------------------------------------------------------------- ** 
** scale_set_drag_callback - add callback function                   ** 
** ----------------------------------------------------------------- */
void scale_set_drag_callback(this, func, data)
c_Scale	*this;
void 	(*func) ___PROTO((void *data));
void	*data;
{
 XtAddCallback(WIDGET(this), XmNdragCallback, (XtCallbackProc)func, data);
}

/* ----------------------------------------------------------------- ** 
** scale_set_valuechange_callback - add callback function            ** 
** ----------------------------------------------------------------- */
void scale_set_valuechange_callback(this, func, data)
c_Scale	*this;
void 	(*func) ___PROTO((void *data));
void 	*data;
{
 XtAddCallback(WIDGET(this), XmNvalueChangedCallback,
	       (XtCallbackProc)func, data);
}


