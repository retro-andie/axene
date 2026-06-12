/*
** SpinBox.c for Xclamation, XQuad and XAllWrite in SpinBox/
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
** Started on  Sun Oct 25 17:22:11 1998 Emmanuel Paris
** Last update Tue Jan 18 15:37:19 2000 Emmanuel Paris
*/

#include "SpinBox.h"
#include "SpinBoxP.h"
#include <Xm/ArrowB.h>
#include "RegisterHelp.h"
#include "Xpm.h"
#include <stdarg.h>

static void *constructor();
static void destructor();
static void *copy();
static void addCallback_SpinBox ___PROTO((c_SpinBox *This, void (*cb_func)(), 
					  void *cb_data, int cb_type));
static void setUnit_SpinBox ___PROTO((c_SpinBox *This, int unit, 
				      char *unit_prec, boolean limit_in_unit,
				      boolean text_in_unit));
static void setAutoRepeatDelay_SpinBox ___PROTO((c_SpinBox *This, long delay));
static void setIncrement_SpinBox ___PROTO((c_SpinBox *This, real increment));
static void setValueTable_SpinBox ___PROTO((c_SpinBox *This, ...));
static void setStringTable_SpinBox ___PROTO((c_SpinBox *This, ...));

static void cb_textfield ___PROTO((c_WTextField *tf, c_SpinBox *This, 
				   XmAnyCallbackStruct *cbs));
static void cb_icon_plus ___PROTO((Widget w, c_SpinBox *This, XEvent *event, 
				   Boolean *dispatch));
static void cb_icon_minus ___PROTO((Widget w, c_SpinBox *This, XEvent *event, 
				    Boolean *dispatch));
static void cb_icons ___PROTO((Widget w, c_SpinBox *This, XEvent *event,
			       boolean plus_minus));
static real get_value ___PROTO((c_SpinBox *This));
static void set_value ___PROTO((c_SpinBox *This, real value));

static void change_value ___PROTO((c_SpinBox *This, boolean plus_minus, 
				   XEvent *event));
static void check_limit ___PROTO((c_SpinBox *This, real *value));

extern void XcHandleEvent ___PROTO((XEvent *event));

sf_SpinBox fc_SpinBox =
{
 constructor,
 destructor,
 copy,
 addCallback_SpinBox,
 setUnit_SpinBox,
 setAutoRepeatDelay_SpinBox,
 setIncrement_SpinBox,
 setValueTable_SpinBox,
 setStringTable_SpinBox
};

/*----------------------------------------------------------------------**
**		Constructor for object SpinBox ...			**
**----------------------------------------------------------------------*/
static void *constructor(w_Parent, Manage_W, Title, sb_type, tf_type, 
			 tf_lower_limit, tf_upper_limit, tf_default_value,
			 tf_nb_columns)
Widget 		w_Parent;
c_ManageWidget 	*Manage_W;
char		*Title;
int		sb_type;
int		tf_type;
real		tf_lower_limit;
real		tf_upper_limit;
char		*tf_default_value;
int		*tf_nb_columns;
{
 c_SpinBox	*This;
 Arg		argsw[7];
 char		*wtitle;

 Xc_HISTORY(("Constructor ..."));
 Xc_ASSERT(sb_type < NB_XcSB_TYPE);

 if (!(This = (c_SpinBox *) Xc_malloc("SpinBox", sizeof (c_SpinBox))))
  Xc_FATAL(("Can't allocate object ... exit"));
 memset(This, 0, sizeof(c_SpinBox));

 This->f = &fc_SpinBox;
  
 This->sb_type = sb_type;

 This->limit_inf = tf_lower_limit;
 This->limit_sup = tf_upper_limit;

 wtitle = (char *)Xc_malloc("wt", strlen(Title) + 4);
 sprintf(wtitle, "~%s", Title);
 
 This->frame = (c_Frame *)NEW(c_Frame)(w_Parent, Manage_W, wtitle);
 XtVaSetValues(WIDGET(This->frame), XmNshadowThickness, 1, 
	       XmNshadowType, XmSHADOW_OUT, XmNborderWidth, 0, NULL);
 This->w_This = WIDGET(This->frame);
 Xc_free(wtitle);
 
 This->rowcolumn = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(This->frame), Manage_W, "sbRowColumnH", 0, 0, XmHORIZONTAL); 
 XtVaSetValues(WIDGET(This->rowcolumn), XmNmarginWidth, 1, XmNmarginHeight, 1,
	       XmNspacing, 0, XmNresizeWidth, True, XmNresizeHeight, True,
	       NULL);

 This->textfield = (c_WTextField *)NEW(c_WTextField)
  (WIDGET(This->rowcolumn), Manage_W, "sbTextField", tf_type, 
   tf_lower_limit, tf_upper_limit, tf_default_value, NULL, tf_nb_columns);
  
 XtVaSetValues(WIDGET(This->textfield), XmNresizeWidth, False, NULL);
 
 F(This->textfield).add_callback(This->textfield, (void *)cb_textfield, This);
 
 This->rc_plusminus = (c_RowColumn *)NEW(c_RowColumn)
  (WIDGET(This->rowcolumn), Manage_W, "sbRowColumnV", 0, 26, XmVERTICAL);
 
 XtVaSetValues(WIDGET(This->rc_plusminus), XmNspacing, 0, 
	       XmNmarginWidth, 0, XmNmarginHeight, 0, 
	       XmNborderWidth, 0, XmNtraversalOn, False, NULL);
 
 if (sb_type >= 0)
 {
  unsigned long bg;

  This->icon_plus = (c_Icon *)NEW(c_Icon)
   (WIDGET(This->rc_plusminus), Manage_W, "sbIconTop", 
    XtDisplay(WIDGET(This->rc_plusminus)), sb_record[sb_type].icon_plus,
    sb_record[sb_type].icon_plus_arm, XcPush);
  
  XtVaGetValues(WIDGET(This->icon_plus), XmNbackground, &bg, NULL);

  XtAddRawEventHandler(WIDGET(This->icon_plus), 
		       ButtonPressMask | ButtonReleaseMask,
		       True, (XtEventHandler)cb_icon_plus, (void *)This);

  XtVaSetValues(WIDGET(This->icon_plus), XmNmarginWidth, 1, 
		XmNmarginHeight, 1, XmNhighlightThickness, 0, 
		XmNshadowThickness, 0, XmNmarginTop, 1,
		XmNmarginLeft, 1, XmNarmColor, bg, NULL);

  This->icon_minus = (c_Icon *)NEW(c_Icon)
   (WIDGET(This->rc_plusminus), Manage_W, "sbIconBottom", 
    XtDisplay(WIDGET(This->rc_plusminus)), sb_record[sb_type].icon_minus,
    sb_record[sb_type].icon_minus_arm, XcPush);
  
  XtAddRawEventHandler(WIDGET(This->icon_minus), 
		       ButtonPressMask | ButtonReleaseMask,
		       True, (XtEventHandler)cb_icon_minus, (void *)This);
  
  XtVaSetValues(WIDGET(This->icon_minus), XmNmarginWidth, 1, 
		XmNmarginHeight, 0, XmNhighlightThickness, 0, 
		XmNshadowThickness, 0,  XmNmarginBottom, 1,
		XmNmarginLeft, 1, XmNarmColor, bg, NULL);
 }
 else
 {
  unsigned long bdc;
  
  XtSetArg(argsw[0], XmNbackground, &bdc);
  XtGetValues(WIDGET(This->rc_plusminus), argsw, 1);
  
  if (sb_type == XcSB_ARROWS_UP_DOWN)
   XtSetArg(argsw[0], XmNarrowDirection, XmARROW_UP);
  else
   XtSetArg(argsw[0], XmNarrowDirection, XmARROW_RIGHT);
  
  XtSetArg(argsw[1], XmNwidth, 11);
  XtSetArg(argsw[2], XmNheight, 11);
  XtSetArg(argsw[3], XmNhighlightThickness, 0);
  XtSetArg(argsw[4], XmNshadowThickness, 0);
  XtSetArg(argsw[5], XmNborderWidth, 1);
  XtSetArg(argsw[6], XmNborderColor, bdc);
  
  This->arrow_plus = XmCreateArrowButton(WIDGET(This->rc_plusminus),
					 "sbArrowUp", argsw, 7);  

  XtAddRawEventHandler(This->arrow_plus, 
		       ButtonPressMask | ButtonReleaseMask,
		       True, (XtEventHandler)cb_icon_plus, (void *)This);
  
  if (Manage_W)
   F(Manage_W).Add_child(Manage_W, This->arrow_plus, 
			 WIDGET(This->rc_plusminus));

  if (sb_type == XcSB_ARROWS_UP_DOWN)
   XtSetArg(argsw[0], XmNarrowDirection, XmARROW_DOWN);
  else
   XtSetArg(argsw[0], XmNarrowDirection, XmARROW_LEFT);

  This->arrow_minus = XmCreateArrowButton(WIDGET(This->rc_plusminus),
					  "sbArrowDown", argsw, 7);  
  XtAddRawEventHandler(This->arrow_minus, 
		       ButtonPressMask | ButtonReleaseMask,
		       True, (XtEventHandler)cb_icon_minus, (void *)This);
  
  if (Manage_W)
   F(Manage_W).Add_child(Manage_W, This->arrow_minus, 
			 WIDGET(This->rc_plusminus));
 }
 
 This->tf_cb_func = This->a1_cb_func = This->a2_cb_func = NULL;
 This->tf_cb_data = This->a1_cb_data = This->a2_cb_data = NULL;
 This->value_table = NULL;

 This->auto_repeat = SB_NO_AUTO_REPEAT;
 This->increment = 0.0;
 
 This->plus_sensitive = This->minus_sensitive = TRUE;

 if (!Manage_W)
 {
  XtManageChild(WIDGET(This->textfield));
  XtManageChild(WIDGET(This->rc_plusminus));
  
  if (sb_type >= 0)
  {
   XtManageChild(WIDGET(This->icon_plus));
   XtManageChild(WIDGET(This->icon_minus));
  }
  else
  {
   XtManageChild(This->arrow_plus);
   XtManageChild(This->arrow_minus);   
  }
 }
 
 {
  Dimension w, h;
  
  XtVaGetValues(WIDGET(This->textfield), XmNwidth, &w, XmNheight, &h, NULL);
  
  w += 17; h = MAX(h + 2, 29);
  XtVaSetValues(WIDGET(This->rowcolumn),
		XmNwidth, w, XmNheight, h,
		XmNresizeWidth, False, XmNresizeHeight, False, NULL);
  
  XtVaSetValues(WIDGET(This->frame), XmNwidth, w+2, XmNheight, h+2, NULL);
 }

 
 if (tf_type != TF_STRING && tf_type != TF_NUMSTRING)
 {
  real value = (real)atof(tf_default_value);
  check_limit(This, &value);
 }

 Xc_HISTORY(("Constructor ok"));
 return This;
}

/*----------------------------------------------------------------------**
**		Destructor for object SpinBox ...			**
**----------------------------------------------------------------------*/

static void destructor(This)
c_SpinBox *This;
{
 Xc_HISTORY(("Destructor ..."));
  
 if (This->sb_type >= 0)
 {
  DELETE(c_Icon)(This->icon_plus);
  DELETE(c_Icon)(This->icon_minus);
 }
 else
 {
  XtDestroyWidget(This->arrow_plus);
  XtDestroyWidget(This->arrow_minus);
 }
 DELETE(c_RowColumn)(This->rc_plusminus);
 DELETE(c_WTextField)(This->textfield);
 DELETE(c_RowColumn)(This->rowcolumn);
 DELETE(c_Frame)(This->frame);

 if (This->value_table) Xc_free(This->value_table);
 Xc_free(This);

 Xc_HISTORY(("Destructor ok"));
}

/*----------------------------------------------------------------------**
**		Copy method for object SpinBox ...			**
**----------------------------------------------------------------------*/

static void *copy(ObjectToCopy)
c_SpinBox *ObjectToCopy;
{
 Xc_HISTORY(("Copy ...")); 

 Xc_HISTORY(("Copy failed"));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** addCallback function                                              ** 
** ----------------------------------------------------------------- */
static void addCallback_SpinBox(This, cb_func, cb_data, cb_type)
c_SpinBox *This;
void (*cb_func)();
void *cb_data;
int cb_type;
{
 switch(cb_type)
 {
 case SB_TEXTFIELD_CALLBACK:
  This->tf_cb_func = cb_func;
  This->tf_cb_data = cb_data;
  break;
 case SB_ARROW_UP_CALLBACK:
  This->a1_cb_func = cb_func;
  This->a1_cb_data = cb_data;
  break;
 case SB_ARROW_DOWN_CALLBACK:
  This->a2_cb_func = cb_func;
  This->a2_cb_data = cb_data;
  break;
 default:
  Xc_WARNING(("Unknow callback type"));
  break;
 }
}

/* ----------------------------------------------------------------- ** 
** setUnit                                                           ** 
** ----------------------------------------------------------------- */
static void setUnit_SpinBox(This, unit, unit_prec, limit_in_unit, text_in_unit)
c_SpinBox *This;
int unit;
char *unit_prec;
boolean limit_in_unit;
boolean text_in_unit;
{
 real value;
 Xc_TRACE(("set unit"));
 
#if !defined(Xc_NO_UNIT)
 F(This->textfield).set_unit(This->textfield, unit, unit_prec, 
			     limit_in_unit, text_in_unit);
 
 if (limit_in_unit)
 {
  This->limit_inf = This->textfield->LimitInf;
  This->limit_sup = This->textfield->LimitSup;
 }

 if (limit_in_unit || text_in_unit)
 {
  value = get_value(This);
  check_limit(This, &value);
 }
#endif
}

/* ----------------------------------------------------------------- ** 
** setAutoRepeatDelay                                                ** 
** ----------------------------------------------------------------- */
static void setAutoRepeatDelay_SpinBox(This, delay)
c_SpinBox *This;
long delay;
{
 Xc_TRACE(("set AutoRepeat delay: %ld �s", delay));

 This->auto_repeat = delay;
}


/* ----------------------------------------------------------------- ** 
** setIncrement                                                      ** 
** ----------------------------------------------------------------- */
static void setIncrement_SpinBox(This, increment)
c_SpinBox *This;
real increment;
{
 This->increment = increment;
}

/* ----------------------------------------------------------------- ** 
** setValueTable                                                     ** 
** ----------------------------------------------------------------- */
static void setValueTable_SpinBox(c_SpinBox *This, ...)
{
 real value, *vt;
 int count;
 va_list ap;

 Xc_TRACE(("setValueTable"));

 count = 0;
 va_start(ap, This);
 while((value = va_arg(ap, real)) != SB_VALUE_TABLE_END) count++;
 va_end(ap);

 if (This->value_table) Xc_free(This->value_table);

 if (count)
 {
  This->value_table = vt = (real *)Xc_malloc("vt", sizeof(real) * (count + 1));

  va_start(ap, This);
  value = va_arg(ap, real);
  This->limit_inf = This->limit_sup = value;
  *vt++ = value;
  while((value = va_arg(ap, real)) != SB_VALUE_TABLE_END)
  {
   *vt++ = value;
   This->limit_sup = value;
  }
  *vt = SB_VALUE_TABLE_END;
  va_end(ap);
 }
 else
  This->value_table = NULL;
}

/* ----------------------------------------------------------------- ** 
** setStringTable                                                    ** 
** ----------------------------------------------------------------- */
static void setStringTable_SpinBox(c_SpinBox *This, ...)
{
 Xc_TRACE(("setStringTable"));
 
 /*--- not done ---*/
}


/* ----------------------------------------------------------------- ** 
** Callbacks                                                         ** 
** ----------------------------------------------------------------- */

static void cb_textfield(tf, This, cbs)
c_WTextField *tf;
c_SpinBox *This;
XmAnyCallbackStruct *cbs;
{
 real		value;
 
 Xc_TRACE(("textfield callback"));
 
 if (tf->Type != TF_STRING && tf->Type != TF_NUMSTRING)
 {
  value = (real)get_value(This);
  check_limit(This, &value);
 }
 
 if (This->tf_cb_func)
  This->tf_cb_func(tf, This->tf_cb_data, cbs);
}

static void cb_icon_plus(w, This, event, dispatch)
Widget w;
c_SpinBox *This;
XEvent *event;
Boolean *dispatch;
{
 Xc_TRACE(("icon plus callback"));
 
 *dispatch = True;
 cb_icons(w, This, event, TRUE);
}

static void cb_icon_minus(w, This, event, dispatch)
Widget w;
c_SpinBox *This;
XEvent *event;
Boolean *dispatch;
{
 Xc_TRACE(("icon minus callback"));

 *dispatch = True;
 cb_icons(w, This, event, FALSE);
}

static void cb_icons(w, This, event, plus_minus)
Widget w;
c_SpinBox *This;
XEvent *event;
boolean plus_minus;
{
 Display *display = event->xany.display;
 XEvent event2;
 boolean sortie = FALSE;
 int i, nb_event;
 double rate = 1.0;
 
 switch(event->type)
 {
 case ButtonPress:
  if (event->xbutton.button == 1)
  {
   Xc_TRACE(("icon press"));
   
   if ((plus_minus && This->plus_sensitive) ||
       (!plus_minus && This->minus_sensitive))
    XtCallActionProc(w, "Arm", event, NULL, 0);
   if (This->auto_repeat)
   {
    rate = This->auto_repeat / 30.0;
    for(i = 0; i < 15; i++)
    {
     if (XCheckMaskEvent(display, ButtonReleaseMask, &event2) == True &&
	 event2.xbutton.button == 1)
     {
      XPutBackEvent(display, &event2);
      break;
     }
     Zusleep((long)rate);
    }
    if (i != 15) break;
    rate = 1.0;
    do
    {
     if (XCheckMaskEvent(display, ButtonReleaseMask, &event2) == True &&
	 event2.xbutton.button == 1)
     {
      XPutBackEvent(display, &event2);
      sortie = TRUE; 
      nb_event = 0;
     }
     else
      nb_event = XEventsQueued(display, QueuedAfterReading);
     
     while(nb_event)
     {
      XNextEvent(display, &event2);
      XcHandleEvent(&event2);
      nb_event--;
     }
     
     if (!sortie)
     {
      Xc_TRACE(("icon repeat"));
      
      change_value(This, plus_minus, event);
      
      rate += 0.2;
      if (rate > 10.0) rate = 10.0;
      Zusleep((long)(This->auto_repeat / rate));
     }
    }
    while (!sortie);

    if (((plus_minus && !This->plus_sensitive) ||
	 (!plus_minus && !This->minus_sensitive)) && This->sb_type >= 0 && rate != 1.0)
    {
     XtSetSensitive(w, True);
    }
   }
  }
  break;
 case ButtonRelease:
  if (event->xbutton.button == 1)
  {
   Xc_TRACE(("icon release"));
   
   if (!This->auto_repeat || rate == 1.0)
   {
    change_value(This, plus_minus, event);
   }

   if (((plus_minus && !This->plus_sensitive) ||
	(!plus_minus && !This->minus_sensitive)) && This->sb_type >= 0)
   {
    XtSetSensitive(w, False);
   }
  }
  break;
 }
}

static real get_value(This)
c_SpinBox *This;
{
 c_WTextField   *tf = This->textfield;
 char		*vstr;
 real		value = 0.0;
 
 switch(tf->Type)
 {
 case TF_STRING:
 case TF_NUMSTRING:
  break;
 case TF_INTEGER:
 case TF_REAL:
  if ((vstr = F(tf).get(tf)) != NULL)
  {
   value = (real)atof(vstr);
   Xc_free(vstr);
  }
  break;
 case TF_UNIT:
#if !defined(Xc_NO_UNIT)
  value = F(tf).get_value_in_unit(tf);
#endif
  break;
 }
 return value;
}

static void set_value(This, value)
c_SpinBox *This;
real value;
{
 c_WTextField   *tf = This->textfield;
 char str[25];

 switch(tf->Type)
 {
 case TF_STRING:
 case TF_NUMSTRING:
  break;
 case TF_INTEGER:
  sprintf(str, "%d", (int)value);
  F(tf).set(tf, str);
  break;
 case TF_REAL:
  sprintf(str, "%g", value);
  F(tf).set(tf, str);
  break;
 case TF_UNIT:
#if !defined(Xc_NO_UNIT)
  F(tf).set_value_no_unit(tf, value);
#endif
  break;
 }

}

static void change_value(This, plus_minus, event)
c_SpinBox *This;
boolean plus_minus;
XEvent *event;
{
 c_WTextField   *tf = This->textfield;
 real		value, value2, *ptr;
 
 if (tf->Type != TF_STRING && tf->Type != TF_NUMSTRING)
 {
  value = value2 = get_value(This);
  
  if (This->increment != 0.0)
  {
   value2 = value + (plus_minus ? This->increment : -This->increment);
   check_limit(This, &value2);
  }

  if (This->value_table)
  {
   if (plus_minus)
   {
    ptr = This->value_table;
    while(*ptr != SB_VALUE_TABLE_END)
    {
     if (value2 < *ptr) 
     {
      value2 = *ptr;
      check_limit(This, &value2);
      break;
     }
     ptr++;
    }
   }
   else
   {
    ptr = This->value_table;
    while(*ptr != SB_VALUE_TABLE_END)
     ptr++;
    
    do
    {
     ptr--;
     if (value2 > *ptr)
     {
      value2 = *ptr;
      check_limit(This, &value2);
      break; 
     }
    }
    while(ptr != This->value_table);
   }
  }
  
  if (value != value2)
  {
   XmAnyCallbackStruct cbs;
   
   set_value(This, value2);
   cbs.reason = XmCR_MODIFYING_TEXT_VALUE;
   cbs.event = event;
   cb_textfield(tf, This, &cbs);
  }
 }

 if (plus_minus)
 {
  if (This->a1_cb_func)
  {
   This->a1_cb_func(This->sb_type >= 0 ? 
		    WIDGET(This->icon_plus) : This->arrow_plus, 
		    This->a1_cb_data, event);
   value = get_value(This);
   check_limit(This, &value);
  }
 }
 else
 {
  if (This->a2_cb_func)
  {
   This->a2_cb_func(This->sb_type >= 0 ? 
		    WIDGET(This->icon_minus) : This->arrow_minus, 
		    This->a2_cb_data, event);
   value = get_value(This);
   check_limit(This, &value);
  }
 }
}

static void check_limit(This, value)
c_SpinBox *This;
real	  *value;
{
 if (*value <= This->limit_inf)
 {
  *value = This->limit_inf;
  if (This->minus_sensitive)
  {
   This->minus_sensitive = FALSE;
   if (This->sb_type >= 0)
    XtSetSensitive(WIDGET(This->icon_minus), False);
  }
 }
 else if (!This->minus_sensitive)
 {
  This->minus_sensitive = TRUE;
  if (This->sb_type >= 0)
   XtSetSensitive(WIDGET(This->icon_minus), True);
 }
 
 if (*value >= This->limit_sup)
 {
  *value = This->limit_sup;
  if (This->plus_sensitive)
  {
   This->plus_sensitive = FALSE;
   if (This->sb_type >= 0)
    XtSetSensitive(WIDGET(This->icon_plus), False);
  }
 }
 else if (!This->plus_sensitive)
 {
  This->plus_sensitive = TRUE;
  if (This->sb_type >= 0)
   XtSetSensitive(WIDGET(This->icon_plus), True);
 }
}
