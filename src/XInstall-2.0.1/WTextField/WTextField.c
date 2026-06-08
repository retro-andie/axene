/*
** WTextField.c for Xclamation, XQuad, XAllWrite, XMayday and XInstall in 
** 	WTextField/
** Object and method for WTextField
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
** Started on  Thu Nov 24 15:51:53 1994 Antoine Buat
** Last update Tue May 30 14:39:35 2000 Emmanuel Paris
*/

/*#define NHISTORY */
/*#define NTRACE */

#include "WTextField.h"
#include "RegisterHelp.h"

static void *cons_WTextField();
static void dest_WTextField();
static void *copy_WTextField();
static void integer_f();
static void real_f();
static void string_f();
static void numstring_f();
static void WTextField_set_value ___PROTO((c_WTextField *this,
					   char *string_val));
static char *WTextField_get_value ___PROTO((c_WTextField *this));
static void WTextField_add_callback ___PROTO((c_WTextField *this, 
					      WTextFieldProc func,
					      void *data));
static void WTextField_add_control_callback ___PROTO((c_WTextField *this,
						      WTextFieldProc func,
						      void *data));

static void WTextField_remove_lostfocus_callback ___PROTO((c_WTextField *t));
static void WTextField_add_escape_func ___PROTO((c_WTextField *this, 
						 void (*func)(), void *param));

#ifdef WTF_UNIT
static void WTextField_set_unit ___PROTO((c_WTextField *this, int unit,
					  char *unit_prec,
					  boolean limit_in_unit,
					  boolean text_in_unit));
static void WTextField_set_unit_value ___PROTO((c_WTextField *this,
						real value));
static real WTextField_get_unit_value ___PROTO((c_WTextField *this));

static void WTextField_set_no_unit_value ___PROTO((c_WTextField *this,
						   real value));
#endif

static void compute_control_text ___PROTO((c_WTextField *this, 
					   XmTextVerifyCallbackStruct *cbs));
static void cb_activate ___PROTO((Widget w, c_WTextField *this,
				  XmAnyCallbackStruct *cbs));
static void cb_cursor ___PROTO((Widget w, c_WTextField *this, XEvent *event));
static void cb_escape ___PROTO((Widget w, c_WTextField *this,
				XKeyEvent *event));
static void cb_focus ___PROTO((Widget w, c_WTextField *this, 
			       XmAnyCallbackStruct *cbs));
static void cb_unfocus ___PROTO((Widget w, c_WTextField *this, 
				 XmAnyCallbackStruct *cbs));


sf_WTextField fc_WTextField =
{
 cons_WTextField,
 dest_WTextField,
 copy_WTextField,
 WTextField_set_value,
 WTextField_get_value,
 WTextField_add_callback,
 WTextField_remove_lostfocus_callback,
 WTextField_add_escape_func,
#ifdef WTF_UNIT
 WTextField_set_unit,
 WTextField_set_unit_value,
 WTextField_get_unit_value,
 WTextField_set_no_unit_value,
#endif
 WTextField_add_control_callback
};

/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *cons_WTextField(w_Parent, ManageW, Title, Type, 
			     limitinf, limitsup, DefaultValue, string,
			     DefaultColumns)
Widget		w_Parent;
c_ManageWidget	*ManageW;
char		*Title;
int		Type;
real		limitinf;
real		limitsup;
char		*DefaultValue;
char		*string;
int		DefaultColumns;
{
 c_WTextField	*this;
 Arg		argsw[10];
 int		nargs;
 Widget	wdtmp;

 Xc_HISTORY(("constructor(`%s')", Title));
 if ((this = Xc_malloc("WTextField", sizeof(c_WTextField))) == NULL)
  return NULL;
 memset(this, 0, sizeof(c_WTextField));
 this->f = &fc_WTextField;

 this->cb_func = NULL;
 if (limitinf < limitsup)
 {
  this->LimitInf = limitinf;
  this->LimitSup = limitsup;
 }
 else
 {
  this->LimitInf = limitsup;
  this->LimitSup = limitinf;
 }
 this->SetT = 0;
 this->flag_edited = 0;
 this->old_string = NULL;
 this->esc_func = NULL;
 this->esc_param = NULL;
 this->unit_prec = NULL;
 this->old_value = 0.0;

 this->cb_ctrl_func = (WTextFieldProc)NULL;
 this->cb_ctrl_data = NULL;
 this->ctrl_text = NULL;

#ifdef LESSTIF_VERSION
# if (LESSTIF_VERSION <= 0) && (LESSTIF_REVISION < 90)
 DefaultColumns = (DefaultColumns * 2) / 3;
# endif
#endif

 if (string)
 {
  Xc_TRACE(("string = `%s'", string));
  nargs = 0;
  XtSetArg(argsw[nargs], XmNorientation, XmHORIZONTAL);
  nargs++;
  XtSetArg(argsw[nargs], XmNnumColumns, 1);
  nargs++;
  XtSetArg(argsw[nargs], XmNpacking, XmPACK_COLUMN);
  nargs++;
  this->w_This = XmCreateRowColumn(w_Parent, Title, argsw, nargs);
  nargs = 0;
  this->Label = XmCreateLabel(this->w_This, string, argsw, nargs);
  XtManageChild(this->Label);
  nargs = 0;
  if (DefaultValue)
  {
   this->old_string = Xc_strdup("undo1 from TF", DefaultValue);
   XtSetArg(argsw[nargs], XmNvalue, DefaultValue);
   nargs++;
  }

  XtSetArg(argsw[nargs], XmNcolumns, DefaultColumns);
  nargs++;
  this->TextF = XmCreateTextField(this->w_This, Title, argsw, nargs);
  wdtmp = this->TextF;
  XtManageChild(this->TextF);
 }
 else
 {
  Xc_TRACE(("string null!"));
  this->Label = 0;
  nargs = 0;
  if (DefaultValue)
  {
   this->old_string = Xc_strdup("undo1 from TF", DefaultValue);
   XtSetArg(argsw[nargs], XmNvalue, DefaultValue);
   nargs++;
  }

  XtSetArg(argsw[nargs], XmNcolumns, DefaultColumns);
  nargs++;
  this->w_This = wdtmp = XmCreateTextField(w_Parent, Title, argsw, nargs);
  this->TextF = this->w_This;
 }
     
 Xc_REGISTER_HELP(this->TextF);

 this->cursor = NULL;
 this->Type = Type;
 switch (Type)
 {
  /*--- Integer ---*/
 case TF_INTEGER:
  if (DefaultValue)
  {
   if (DefaultValue[0] == '-')
    this->neg = 1;
   else
    this->neg = 0;
  }
  XtAddCallback(wdtmp, XmNmodifyVerifyCallback, integer_f, this);
  break;

  /*--- Real ---*/
 case TF_REAL:
  if (DefaultValue)
  {
   if (DefaultValue[0] == '-')
    this->neg = 1;
   else
    this->neg = 0;
  }
  XtAddCallback(wdtmp, XmNmodifyVerifyCallback, real_f, this);
  break;

  /*--- String ---*/
 case TF_STRING:
  XtAddCallback(wdtmp, XmNmodifyVerifyCallback, string_f, this);
  break;

 case TF_NUMSTRING:
  XtAddCallback(wdtmp, XmNmodifyVerifyCallback, numstring_f, this);
  break;
 }    

 if(ManageW) 
  F(ManageW).Add_child(ManageW, this->w_This, w_Parent);

 Xc_TRACE(("addcallback"));
 XtAddCallback(wdtmp, XmNactivateCallback, (void *)cb_activate, this);
 XtAddEventHandler(wdtmp, EnterWindowMask, False,
		   (void *)cb_cursor, (void *)this);
 XtAddEventHandler(wdtmp, KeyPressMask, False,
		   (void *)cb_escape, (void *)this);

 XtAddCallback(wdtmp, XmNlosingFocusCallback, (void *)cb_activate, this);
 XtAddCallback(wdtmp, XmNlosingFocusCallback, (void *)cb_unfocus, this);
 XtAddCallback(wdtmp, XmNfocusCallback, (void *)cb_focus, this);

 Xc_TRACE(("Object WTextField build"));
 return this;
}

/* ----------------------------------------------------------------- ** 
** destructor                                                        ** 
** ----------------------------------------------------------------- */
static void dest_WTextField(this)
c_WTextField *this;
{
 Xc_HISTORY(("destructor"));

 if (this->ctrl_text)
  Xc_free(this->ctrl_text);
 
 if (this->old_string)
  Xc_free(this->old_string);
  
 if (this->unit_prec)
  Xc_free(this->unit_prec);
  
 if (this->cursor!=NULL)
  DELETE(c_Cursor)(this->cursor);
 if (this->Label)
 {
  XtDestroyWidget(this->Label);
  XtDestroyWidget(this->TextF);
 }
 XtDestroyWidget(this->w_This);
 Xc_free(this);
 Xc_TRACE(("Object WTextField destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy_WTextField(this)
c_WTextField *this;
{
 c_WTextField *ObjTmp;
  
 if ((ObjTmp = (c_WTextField *)Xc_malloc("CWTextField", 
					 sizeof(c_WTextField))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }
  
 memcpy(ObjTmp, this, sizeof(c_WTextField));
 Xc_TRACE(("Object WTextField copied"));
 return ObjTmp;
}


/* ----------------------------------------------------------------- ** 
** integer_f - verifiy integer num callback                          ** 
** ----------------------------------------------------------------- */
static void integer_f(widget, this, cbs)
Widget				widget;
c_WTextField			*this;
XmTextVerifyCallbackStruct	*cbs;
{
 int	len;
 char	*ptr;
 char	*ptr2;

 Xc_TRACE(("enter integer check"));

 if (this->SetT)
 {
  this->SetT = 0;
  goto end_integer_f;
 }

 this->flag_edited = 1;

 if (cbs->text->ptr == NULL)
 {
  if (cbs->startPos == 0 && this->neg)
   this->neg = 0;
  goto end_integer_f;
 }

 len = cbs->text->length;
 if (len>1)
 {
  len--;
  while (len)
  {
   if ((this->neg && (cbs->startPos == 0))
       ||
       (cbs->text->ptr[len]>'9' || cbs->text->ptr[len] < '0'))
   {
    strcpy(cbs->text->ptr+len, cbs->text->ptr+len+1);
    cbs->text->length--;
   }
   len--;
  }
  if ((this->neg == 0) && 
      (cbs->text->ptr[len] == '-') &&
      (cbs->startPos == 0))
   this->neg = 1;
  else
  {
   if ((this->neg && (cbs->startPos == 0))
       ||
       (cbs->text->ptr[len]>'9' || cbs->text->ptr[len]<'0'))
   {
    strcpy(cbs->text->ptr+len, cbs->text->ptr + len + 1);
    cbs->text->length--;
   }
  }
  goto end_integer_f;
 }

 switch(cbs->text->ptr[0])
 {
 case '+':
  if (this->neg == 1)
  {
   ptr = XmTextFieldGetString(widget);
   this->SetT = 1;
   XmTextFieldSetString(widget, ptr + 1);
   XtFree(ptr);
   this->neg = 0;
  }
  len--;
  cbs->text->ptr[0] = 0;
  break;
 case '-':
  if (this->neg == 0)
  {
   ptr = XmTextFieldGetString(widget);
   if (strlen(ptr))
   {
    ptr2 = (char *)Xc_malloc("tmp_TF", strlen(ptr) + 2);
    *ptr2 = '-';
    strcpy(ptr2+1, ptr);
    this->SetT = 1;
    XmTextFieldSetString(widget, ptr2);
    XtFree(ptr);
    Xc_free(ptr2);
    len--;
    cbs->text->ptr[0] = 0;
   }
   this->neg = 1;
   break;
  }
  len--;
  cbs->text->ptr[0] = 0;
  break;
 default:
  if ((this->neg && (cbs->startPos == 0))
      ||
      (cbs->text->ptr[0]>'9' || cbs->text->ptr[0] < '0'))
  {
   len--;
   cbs->text->ptr[0] = 0;
  }
 }
 cbs->text->length = len;

 end_integer_f:
 if (this->cb_ctrl_func)
 {
  compute_control_text(this, cbs);
  this->cb_ctrl_func(this, this->cb_ctrl_data, (XmAnyCallbackStruct *)cbs);
 }
 Xc_TRACE(("leave integer check"));
}

/* ----------------------------------------------------------------- ** 
** real_f - verify real num callback                                 ** 
** ----------------------------------------------------------------- */
static void real_f(widget, this, cbs)
Widget				widget;
c_WTextField			*this;
XmTextVerifyCallbackStruct	*cbs;
{
 int	len;
 char	*ptr;
 char	*ptr2;
 int	num;
 int	coma;
 int	coma_pos;

 Xc_TRACE(("enter real check"));
 if (this->SetT)
 {
  this->SetT = 0;
  goto end_real_f;
 }

 this->flag_edited = 1;
 if (cbs->text->ptr == NULL)
 {
  if (cbs->startPos == 0 && this->neg)
   this->neg = 0;
  goto end_real_f;
 }

 ptr = XmTextFieldGetString(widget);
 num = 0;
 coma = 0;
 coma_pos = -1;
 if (ptr)
 {
  while (ptr[num])
  {
   if (ptr[num] == '.')
   {
    coma_pos = num;
    coma = 1;
    break;
   }
   num++;
  }
  XtFree(ptr);
 }
 Xc_TRACE(("str %s", cbs->text->ptr));
 Xc_TRACE(("coma %d && Pos %d\n", coma, coma_pos));

 len = cbs->text->length;
 if (len>1)
 {
  len--;
  while (len)
  {
   if ((coma == 0) && 
       ((cbs->text->ptr[len] == '.') || (cbs->text->ptr[len] == ',')))
   {
    cbs->text->ptr[len] = '.';
    coma = 1;
    len--;
    continue;
   }
   if ((this->neg && (cbs->startPos == 0))
       ||
       (cbs->text->ptr[len] > '9' || cbs->text->ptr[len] < '0'))
   {
    strcpy(cbs->text->ptr + len, cbs->text->ptr + len + 1);
    cbs->text->length--;
   }
   len--;
  }
  if ((this->neg == 0) && 
      (cbs->text->ptr[len] == '-') &&
      (cbs->startPos == 0))
   this->neg = 1;
  else
  {
   if ((coma == 0) && 
       ((cbs->text->ptr[len] == '.') || (cbs->text->ptr[len] == ',')))
   {
    cbs->text->ptr[len] = '.';
    coma = 1;
    len--;
    goto end_real_f;
   }
   if ((this->neg && (cbs->startPos == 0))
       ||
       (cbs->text->ptr[len] > '9' || cbs->text->ptr[len] < '0'))
   {
    strcpy(cbs->text->ptr + len, cbs->text->ptr + len + 1);
    cbs->text->length--;
   }
  }
  goto end_real_f;
 }

 switch(cbs->text->ptr[0])
 {
 case '+':
  if (this->neg == 1)
  {
   ptr = XmTextFieldGetString(widget);
   this->SetT = 1;
   XmTextFieldSetString(widget, ptr + 1);
   XtFree(ptr);
   this->neg = 0;
  }
  len--;
  cbs->text->ptr[0] = 0;
  break;
 case '-':
  if (this->neg == 0)
  {
   ptr = XmTextFieldGetString(widget);
   if (strlen(ptr))
   {
    ptr2 = (char *)Xc_malloc("tmp_TF", strlen(ptr) + 2);
    *ptr2 = '-';
    strcpy(ptr2 + 1, ptr);
    this->SetT = 1;
    XmTextFieldSetString(widget, ptr2);
    XtFree(ptr);
    Xc_free(ptr2);
    len--;
    cbs->text->ptr[0] = 0;
   }
   this->neg = 1;
   break;
  }
  len--;
  cbs->text->ptr[0] = 0;
  break;
 case ',':
  cbs->text->ptr[0] = '.';
 case '.':
  if (coma == 0)
   break;
 default:
  if ((this->neg && (cbs->startPos == 0))
      ||
      (cbs->text->ptr[0] > '9' || cbs->text->ptr[0] < '0'))
  {
   len--;
   cbs->text->ptr[0] = 0;
  }
 }
 Xc_TRACE(("len %d", len));
 cbs->text->length = len;

 end_real_f:
 if (this->cb_ctrl_func)
 {
  compute_control_text(this, cbs);
  this->cb_ctrl_func(this, this->cb_ctrl_data, (XmAnyCallbackStruct *)cbs);
 }
 Xc_TRACE(("leave real check"));
}

/* ----------------------------------------------------------------- ** 
** string_f - verify string callback                                 ** 
** ----------------------------------------------------------------- */
static void string_f(widget, this, cbs)
Widget				widget;
c_WTextField			*this;
XmTextVerifyCallbackStruct	*cbs;
{
 char	*ptr;
 int	len;

 Xc_TRACE(("enter string check %g", this->LimitSup));
 this->cleanSetT = 0;
 if (this->SetT)
 {
  this->SetT = 0;
  this->cleanSetT = 1;
  goto end_string_f;
 }

 this->flag_edited = 1;
 if (cbs->text->ptr == NULL)
 {
  goto end_string_f;
 }

 ptr = XmTextFieldGetString(widget);
 len = strlen(ptr);
 XtFree(ptr);

 if (len >= this->LimitSup)
 {
  cbs->text->length = 0;
  cbs->text->ptr[0] = 0;
  cbs->doit = False;
  goto end_string_f;
 }  

 if (cbs->text->length > 1)
 {
  if ( (len + cbs->text->length) >= this->LimitSup)
  {
   cbs->text->length = this->LimitSup - len;
   cbs->text->ptr[cbs->text->length] = 0;
  }
 }

 end_string_f:
 if (this->cb_ctrl_func)
 {
  compute_control_text(this, cbs);
  this->cb_ctrl_func(this, this->cb_ctrl_data, (XmAnyCallbackStruct *)cbs);
 }
 Xc_TRACE(("leave string check"));
}

/* ----------------------------------------------------------------- ** 
** numstring_f - verify num string callback                          ** 
** ----------------------------------------------------------------- */
static void numstring_f(widget, this, cbs)
Widget				widget;
c_WTextField			*this;
XmTextVerifyCallbackStruct	*cbs;
{
 char	*ptr;
 int	len;
 int	i;

 Xc_TRACE(("enter numstring check"));
 this->cleanSetT = 0;
 if (this->SetT)
 {
  this->SetT = 0;
  this->cleanSetT = 1;
  goto end_numstring_f;
 }

 this->flag_edited = 1;
 if (cbs->text->ptr == NULL)
 {
  goto end_numstring_f;
 }

 ptr = XmTextFieldGetString(widget);
 len = strlen(ptr);
 XtFree(ptr);

 i = 0;
 while (i < cbs->text->length)
 {
  if ((cbs->text->ptr[i] < '0') || (cbs->text->ptr[i] > '9'))
  {
   int j;
   j = i;
   while (j < cbs->text->length)
   {
    cbs->text->ptr[j] = cbs->text->ptr[j+1];
    j++;
   }
   cbs->text->length--;
  }
  else
   i++;
 }

 if (len >= this->LimitSup)
 {
  cbs->text->length = 0;
  cbs->text->ptr[0] = 0;
  cbs->doit = False;
  goto end_numstring_f;
 }  

 if (cbs->text->length > 1)
 {
  if ( (len + cbs->text->length) >= this->LimitSup)
  {
   cbs->text->length = this->LimitSup - len;
   cbs->text->ptr[cbs->text->length] = 0;
  }
 }

 end_numstring_f:
 if (this->cb_ctrl_func)
 {
  compute_control_text(this, cbs);
  this->cb_ctrl_func(this, this->cb_ctrl_data, (XmAnyCallbackStruct *)cbs);
 }
 Xc_TRACE(("leave numstring check"));
}

/* ----------------------------------------------------------------- ** 
** WTextF_set_value                                                  ** 
** ----------------------------------------------------------------- */
static char snull[10];

static void WTextField_set_value(this, string_val)
c_WTextField	*this;
char		*string_val;
{
 char	*temp;
  
 this->SetT = 1;
 this->flag_edited = 0;
 temp = this->old_string;	/* in case of string_val == this->old_string */
 if (string_val)
  this->old_string = Xc_strdup("undo from TF", string_val);
 else
  this->old_string = NULL;
 if((string_val == NULL) || (*string_val == 0))
 {
  XmTextFieldSetString(TEXTFIELD(this), snull);
 }
 else
 {
  XmTextFieldSetString(TEXTFIELD(this), string_val);
 }
 if (temp)
  Xc_free(temp);
}

/* ----------------------------------------------------------------- ** 
** WTextF_get_value                                                  ** 
** ----------------------------------------------------------------- */
static char *WTextField_get_value(this)
c_WTextField	*this;
{
 char *text, *new;

 if (this->cb_ctrl_func)
 {
  new = Xc_strdup("cvalue", this->ctrl_text);
 }
 else
 {
  if((text = XmTextFieldGetString(TEXTFIELD(this))) == NULL) return NULL;
  new = Xc_strdup("value", text);
  XtFree(text);
 }
 return new;
}


/* ----------------------------------------------------------------- ** 
** WTextField_add_callback - add callback function                   ** 
** ----------------------------------------------------------------- */
static void WTextField_add_callback(this, func, data)
c_WTextField	*this;
WTextFieldProc func;
void 		*data;
{
 Xc_HISTORY(("addCallback"));
 this->cb_func = func;
 this->cb_data = data;
}

/* ----------------------------------------------------------------- ** 
** WTextField_add_control_callback - add control (modify) cb function * 
** ----------------------------------------------------------------- */
static void WTextField_add_control_callback(this, func, data)
c_WTextField *this;
WTextFieldProc func;
void *data;
{
 Xc_HISTORY(("addControlCallback"));
 this->ctrl_text = F(this).get(this);
 this->cb_ctrl_func = func;
 this->cb_ctrl_data = data;
}

static void WTextField_remove_lostfocus_callback(this)
c_WTextField	*this;
{
 XtRemoveCallback(this->TextF, XmNlosingFocusCallback, 
		  (void *)cb_activate, this);
}

static void WTextField_add_escape_func(this, func, param)
c_WTextField *this;
void (*func)();
void *param;
{
 this->esc_func = func;
 this->esc_param = param;
}

#ifdef WTF_UNIT
/* ----------------------------------------------------------------- ** 
** set_unit - change WTextField type to unit                         ** 
** ----------------------------------------------------------------- */
static void WTextField_set_unit(this, unit, unit_prec,
				limit_in_unit, text_in_unit)
c_WTextField *this;
int unit;
char *unit_prec;
boolean limit_in_unit;
boolean text_in_unit;
{
 switch(this->Type)
 {
 case TF_INTEGER:
  XtRemoveCallback(this->TextF, XmNmodifyVerifyCallback, integer_f, this);
  break;
 case TF_REAL:
  XtRemoveCallback(this->TextF, XmNmodifyVerifyCallback, real_f, this);
  break;
 case TF_STRING:
  XtRemoveCallback(this->TextF, XmNmodifyVerifyCallback, string_f, this);
  break;
 }
  
 this->Type = TF_UNIT;
 this->unitb = unit;
  
 if (unit_prec)
  this->unit_prec = Xc_strdup("up", unit_prec);
 else
  this->unit_prec = Xc_strdup("up", "%g");
  
 if (limit_in_unit)
 {
  char	str[50];
    
  sprintf(str, "%g", this->LimitInf);
  this->LimitInf = tf_convert_string_to_real(unit, str, 0.0, 1.0, 0.0);
    
  sprintf(str, "%g", this->LimitSup);
  this->LimitSup = tf_convert_string_to_real(unit, str, 0.0, 1.0, 0.0);
 }
  
 if (!text_in_unit)
 {    
  char *str, *str2;
    
    
  str = F(this).get(this);
  str2 = tf_convert_string_to_string(unit, str, &this->old_value,
				     this->LimitInf, this->LimitSup);
  F(this).set(this, str2);
    
  Xc_free(str);
  Xc_free(str2);
 }
 else
 {
  char *str;
    
  str = F(this).get(this);
  this->old_value = tf_convert_string_to_real(unit, str, 0.0,
					      this->LimitInf, this->LimitSup);
 }
}

/* ----------------------------------------------------------------- ** 
** set_unit_value - set the value of the WTextField in the unit      ** 
** ----------------------------------------------------------------- */
static void WTextField_set_unit_value(this, value)
c_WTextField *this;
real	value;
{
 char	str[50], *str2;
  
 sprintf(str, this->unit_prec, value);
  
 str2 = tf_convert_string_to_string(this->unitb, str, &this->old_value,
				    this->LimitInf, this->LimitSup);
  
 F(this).set(this, str2);
 Xc_free(str2);
}

/* ----------------------------------------------------------------- ** 
** set_no_unit_value - set the value of the WTextField in no unit    ** 
** ----------------------------------------------------------------- */
static void WTextField_set_no_unit_value(this, value)
c_WTextField *this;
real	value;
{
 char	*str2;
 
 str2 = tf_convert_real_to_string(this->unitb, value, &this->old_value,
				  this->LimitInf, this->LimitSup);
 
 F(this).set(this, str2);
 Xc_free(str2);
}

/* ----------------------------------------------------------------- ** 
** get_unit_val - return the value in the unit                       ** 
** ----------------------------------------------------------------- */
static real WTextField_get_unit_value(this)
c_WTextField *this;
{
 real	val;
 char	*str;
  
 str = F(this).get(this);
  
 this->old_value = val =
  tf_convert_string_to_real(this->unitb, str, this->old_value,
			    this->LimitInf, this->LimitSup);
 Xc_free(str);
 return val;
}
#endif

static void compute_control_text(this, cbs)
c_WTextField *this; 
XmTextVerifyCallbackStruct *cbs;
{
 char *tmp;
 int insert, blen, tlen;

 if (cbs->text->ptr != NULL && cbs->text->length)
  blen = cbs->text->length;
 else
  blen = 0;
 
 tmp = Xc_malloc("~", strlen(this->ctrl_text) + blen + 1);
 
 strcpy(tmp, this->ctrl_text);
 tlen = strlen(tmp);
 
 insert = cbs->currInsert;
 if (cbs->startPos < cbs->endPos && cbs->doit)
 {
  memmove(tmp + cbs->startPos, tmp + cbs->endPos, tlen - cbs->endPos);
  tlen -= (cbs->endPos - cbs->startPos);
  insert = cbs->startPos;
 }
 else if (cbs->text->ptr == NULL)/* backspace or delete not in selection */
 {
  if (insert != cbs->newInsert) /* backspaced */
   insert = cbs->newInsert;
  memmove(tmp + insert, tmp + insert + 1, tlen - insert - 1);
  tlen--;
 }
 
 if (blen > 0)			/* insert cbs->text->ptr */
 {
  if (insert != tlen)
   memmove(tmp + insert + blen, tmp + insert, tlen - insert);
  memcpy(tmp + insert, cbs->text->ptr, blen);
  tlen += blen; 
 }
 tmp[tlen] = '\0';
 Xc_free(this->ctrl_text);
 this->ctrl_text = tmp;
}

static char tab[20];
/* ----------------------------------------------------------------- ** 
** cb_activate                                                       ** 
** ----------------------------------------------------------------- */
static void cb_activate(w, this, cbs)
Widget w;
c_WTextField *this;
XmAnyCallbackStruct *cbs;
{
#ifdef WTF_UNIT
 char *str;
#endif
 char *ptr;
 real	num;

 /* verify limit */
 switch (this->Type)
 {
 case TF_INTEGER:
  ptr = XmTextFieldGetString(w);
  num = atof(ptr);
  Xc_TRACE(("%f", num));
  if (num < this->LimitInf)
   num = this->LimitInf;
  if (num > this->LimitSup)
   num = this->LimitSup;
  sprintf(tab, "%d", (int)num);
  this->SetT = 1;
  XmTextFieldSetString(w, tab);
  XtFree(ptr);
  break;
 case TF_REAL:
  ptr = XmTextFieldGetString(w);
  num = atof(ptr);
  Xc_TRACE(("%f", num));
  if (num < this->LimitInf)
   num = this->LimitInf;
  if (num > this->LimitSup)
   num = this->LimitSup;
  sprintf(tab, "%g", num);
  this->SetT = 1;
  XmTextFieldSetString(w, tab);
  XtFree(ptr);
  break;
#ifdef WTF_UNIT
 case TF_UNIT:
  ptr = XmTextFieldGetString(w);
  str = tf_convert_string_to_string(this->unitb, ptr, &this->old_value,
				    this->LimitInf, this->LimitSup);
  if (!Xstrcmp(ptr, str))
  {
   this->SetT = 1;
   XmTextFieldSetString(w, str);
  }
  Xc_free(str);
  XtFree(ptr);
  break;
#endif
 }
 if(this->cb_func != NULL) 
  this->cb_func(this, this->cb_data, cbs);
 Xc_TRACE(("cb_activate Done"));
}

/* ----------------------------------------------------------------- ** 
** cb_cursor                                                         ** 
** ----------------------------------------------------------------- */
static void cb_cursor(w, this, event)
Widget w;
c_WTextField *this;
XEvent	*event;
{
 if (this->cursor==NULL)
 {
  this->cursor = (c_Cursor *)NEW(c_Cursor)(XtDisplay(w), XtWindow(w));
  F(this->cursor).set_cursor(this->cursor, C_EDIT_TEXT);
 }
}

/* ----------------------------------------------------------------- ** 
** cb_escape                                                         ** 
** ----------------------------------------------------------------- */
static void cb_escape(w, this, event)
Widget w;
c_WTextField *this;
XKeyEvent *event;
{
 KeySym real_key;

 real_key = XKeycodeToKeysym(XtDisplay(w), event->keycode, 0);
 if ((real_key == XK_Escape) ||
     (real_key == XK_Undo))
 {
  if (this->flag_edited)
  {
   if (this->old_string)
    F(this).set(this, this->old_string);
   XmTextFieldSetSelection(w,
			   0, XmTextFieldGetLastPosition(w),
			   event->time);
   this->flag_edited = 0;
   if (this->esc_func)
    this->esc_func(this, this->esc_param, FALSE);
  }
  else
  {
   XmTextFieldClearSelection(w, event->time);
   this->flag_edited = 1;
   if (this->esc_func)
    this->esc_func(this, this->esc_param, TRUE);
  }
 }
}

/* ----------------------------------------------------------------- ** 
** cb_focus                                                          ** 
** ----------------------------------------------------------------- */
static void cb_focus(w, this, cbs)
Widget w;
c_WTextField *this;
XmAnyCallbackStruct *cbs;
{
 Xc_TRACE(("set XmNcursorPositionVisible True"));
 XtVaSetValues(w, XmNcursorPositionVisible, True, NULL);
}

/* ----------------------------------------------------------------- ** 
** cb_unfocus                                                        ** 
** ----------------------------------------------------------------- */
static void cb_unfocus(w, this, cbs)
Widget w;
c_WTextField *this;
XmAnyCallbackStruct *cbs;
{
 Xc_TRACE(("set XmNcursorPositionVisible False"));
 XtVaSetValues(w, XmNcursorPositionVisible, False, NULL);
 XClearArea(XtDisplay(w), XtWindow(w), 0, 0, 0, 0, True );
}
