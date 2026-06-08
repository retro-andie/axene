/*
** ModifList.c for Xclamation, XQuad and XAllWrite in ModifList/
** Object and method for ModifList
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
** Last update Fri Oct 16 15:46:57 1998 Emmanuel Paris
*/

/*#define NHISTORY */
/*#define NTRACE */

#include <varargs.h>
#include "ModifList.h"

static void *cons_ModifList();
static void dest_ModifList();
static void *copy_ModifList();
static void ModifList_set();
static int ModifList_get();
static void ModifList_add();
static void ModifList_sub();
static void ModifList_change_all();
static void ModifList_change_all2();
static boolean ModifList_something_to_change ___PROTO((c_ModifList *This));

sf_ModifList fc_ModifList =
{
 cons_ModifList,
 dest_ModifList,
 copy_ModifList,
 ModifList_set,
 ModifList_get,
 ModifList_add,
 ModifList_sub,
 ModifList_change_all,
 ModifList_change_all2,
 ModifList_something_to_change
};

/* ----------------------------------------------------------------- ** 
** Constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *cons_ModifList(Box, func, num, va_alist)
void 	*Box;
void	(*func)();
int	num;
va_dcl
{
 va_list 	ap;
 c_ModifList	*This;
 int		numtmp;
 int		tmpsize;
 int		mod_long;

 Xc_HISTORY(("constructor ModifList"));
 if ((This = Xc_malloc("ModifList", sizeof(c_ModifList))) == NULL)
  return NULL;
 This->f = &fc_ModifList;
 This->Box = Box;
 This->o_save = NULL;
 This->off_inout = (int *)Xc_malloc("inout", num*3*sizeof(int));
 memset(This->off_inout, 0, num*3*sizeof(int));
 This->globsize = 0;
 This->num = num;
 This->func = func;
 This->last_use = NULL;
 numtmp = 0;
 va_start(ap);
 while (numtmp < num)
 {
  (This->off_inout)[numtmp*3]=va_arg(ap, int);
  tmpsize=va_arg(ap, int);
  (This->off_inout)[numtmp*3+1] = tmpsize;
  mod_long = tmpsize%sizeof(long);
  if (mod_long)
   tmpsize += sizeof(long)-mod_long;
  (This->off_inout)[numtmp*3+2] = This->globsize;
  This->globsize+=tmpsize;
  numtmp++;
 }
 /* receive offset, size ... */
 va_end(ap);
 Xc_TRACE(("Object ModifList build"));
 return This;
}

/* ----------------------------------------------------------------- ** 
** destructor                                                        ** 
** ----------------------------------------------------------------- */
static void dest_ModifList(This)
c_ModifList *This;
{
 obj_save	*obj_tmplist;
 obj_save	*obj_tmplist2;
 Xc_HISTORY(("destructor"));
 obj_tmplist = This->o_save;
 while (obj_tmplist)
 {
  obj_tmplist2 = obj_tmplist->Next;
  Xc_free(obj_tmplist->contain);
  Xc_free(obj_tmplist);
  obj_tmplist = obj_tmplist2;
 }
 Xc_free(This->off_inout);
 Xc_free(This);
}

/* ----------------------------------------------------------------- ** 
** copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy_ModifList(This)
c_ModifList *This;
{
 c_ModifList *ObjTmp;
  
 if ((ObjTmp = (c_ModifList *)Xc_malloc("CModifList", sizeof(c_ModifList))) == NULL)
 {
  Xc_FATAL(("Can't copy this object: memory allocation error"));
 }

 memcpy(ObjTmp, This, sizeof(c_ModifList));
 Xc_TRACE(("Object ModifList cpi not implemented"));
 return ObjTmp;
}

static void ModifList_set(this, object, field, val)
c_ModifList	*this;
void		*object;
int		field;
void		*val;
{
 obj_save	**obj_tmplist;
 int		numt;

 if ((field < 0) || (field >= this->num))
 {
  Xc_TRACE(("Set field out of range"));
  return;
 }
 if (this->last_use)
  if (this->last_use->object == object)
  {
   memcpy(this->last_use->contain+this->off_inout[field*3+2],
	  val, this->off_inout[field*3+1]);
   return;
  }
 obj_tmplist= &(this->o_save);
 while (*obj_tmplist)
 {
  if ((*obj_tmplist)->object == object)
  {
   this->last_use=(*obj_tmplist);
   memcpy((*obj_tmplist)->contain+this->off_inout[field*3+2],
	  val, this->off_inout[field*3+1]);
   return;
  }
  obj_tmplist = &((*obj_tmplist)->Next);
 }
 (*obj_tmplist)=(obj_save *)Xc_malloc("obj_save_f", sizeof(obj_save));
 (*obj_tmplist)->object = object;
 (*obj_tmplist)->contain = Xc_malloc("obj_save_c", this->globsize);
 memset((*obj_tmplist)->contain, 0, this->globsize);
 (*obj_tmplist)->type = TOSET;
 (*obj_tmplist)->Next = NULL;
 numt = 0;
 while (numt < this->num)
 {
  memcpy((*obj_tmplist)->contain+this->off_inout[numt*3+2],
	 (char *)object+this->off_inout[numt*3],
	 this->off_inout[numt*3+1]);
  numt++;
 }
 this->last_use=(*obj_tmplist);
 memcpy((*obj_tmplist)->contain+this->off_inout[field*3+2],
	val, this->off_inout[field*3+1]);
}

static int ModifList_get(this, object, field, val)
c_ModifList	*this;
void		*object;
int		field;
void		*val;
{
 obj_save	**obj_tmplist;

 if ((field < 0) || (field >= this->num))
 {
  Xc_TRACE(("Get field out of range"));
  return -1;
 }
 if (this->last_use)
  if (this->last_use->object == object)
  {
   memcpy(val, this->last_use->contain+this->off_inout[field*3+2],
	  this->off_inout[field*3+1]);
   return 1;
  }
 obj_tmplist= &(this->o_save);
 while (*obj_tmplist)
 {
  if ((*obj_tmplist)->object == object)
  {
   this->last_use=(*obj_tmplist);
   memcpy(val, (*obj_tmplist)->contain+this->off_inout[field*3+2],
	  this->off_inout[field*3+1]);
   return 1;
  }
  obj_tmplist = &((*obj_tmplist)->Next);
 }
 return -1;
}

static void ModifList_add(this, object)
c_ModifList	*this;
void		*object;
{
 obj_save	**obj_tmplist;
 int		numt;

 obj_tmplist= &(this->o_save);
 while (*obj_tmplist)
 {
  obj_tmplist = &((*obj_tmplist)->Next);
 }
 (*obj_tmplist)=(obj_save *)Xc_malloc("obj_save_f", sizeof(obj_save));
 (*obj_tmplist)->object = object;
 (*obj_tmplist)->contain = Xc_malloc("obj_save_c", this->globsize);
 (*obj_tmplist)->type = TOADD;
 (*obj_tmplist)->Next = NULL;
 numt = 0;
 while (numt < this->num)
 {
  memcpy((*obj_tmplist)->contain+this->off_inout[numt*3+2],
	 (char *)object+this->off_inout[numt*3],
	 this->off_inout[numt*3+1]);
  numt++;
 }
 this->last_use=(*obj_tmplist);
}

static void ModifList_sub(this, object)
c_ModifList	*this;
void		*object;
{
 obj_save	**obj_tmplist;

 if (this->last_use)
  if (this->last_use->object == object)
  {
   this->last_use->type = TOSUB;
   return;
  }
 obj_tmplist= &(this->o_save);
 while (*obj_tmplist)
 {
  if ((*obj_tmplist)->object == object)
  {
   (*obj_tmplist)->type = TOSUB;
   return;
  }
  obj_tmplist = &((*obj_tmplist)->Next);
 }
 (*obj_tmplist)=(obj_save *)Xc_malloc("obj_save_f", sizeof(obj_save));
 (*obj_tmplist)->object = object;
 (*obj_tmplist)->contain = Xc_malloc("obj_save_c", this->globsize);
 (*obj_tmplist)->type = TOSUB;
 (*obj_tmplist)->Next = NULL;
}

static void ModifList_change_all(this)
c_ModifList	*this;
{
 obj_save	*obj_tmplist;
 int		f_tmp;
 int		changed;

 obj_tmplist = this->o_save;
 while (obj_tmplist)
 {
  f_tmp = 0;
  changed = 0;
  if (obj_tmplist->type!=TOSET)
   changed = 1;
  else
  {
   while(f_tmp < this->num)
   {
    if (memcmp((char *)obj_tmplist->object+this->off_inout[f_tmp*3], 
	       obj_tmplist->contain+this->off_inout[f_tmp*3+2], 
	       this->off_inout[f_tmp*3+1]))
    {
     changed = 1;
     break;
    }
    f_tmp++;
   }
  }
  if (changed)
  {
   f_tmp = 0;
   while (f_tmp < this->num)
   {
    memcpy((char *)obj_tmplist->object+this->off_inout[f_tmp*3], 
	   obj_tmplist->contain+this->off_inout[f_tmp*3+2],
	   this->off_inout[f_tmp*3+1]);
    f_tmp++;
   }
   this->func(this->Box, obj_tmplist->object, obj_tmplist->type);
  }
  obj_tmplist = obj_tmplist->Next;
 }
}

static void ModifList_change_all2(this)
c_ModifList	*this;
{
 obj_save	*obj_tmplist;
 int		f_tmp;
 int		changed;

 obj_tmplist = this->o_save;
 while (obj_tmplist)
 {
  f_tmp = 0;
  changed = 0;
  switch (obj_tmplist->type)
  {
  case TOSUB:
   this->func(this->Box, obj_tmplist->object, TOSUB, 0, 0);
   break;
  case TOADD:
   this->func(this->Box, obj_tmplist->object, TOADD, 0, 0);
   obj_tmplist->type = NEWSET;
  case TOSET:
   while(f_tmp < this->num)
   {
    if (memcmp((char *)obj_tmplist->object+this->off_inout[f_tmp*3], 
	       obj_tmplist->contain+this->off_inout[f_tmp*3+2], 
	       this->off_inout[f_tmp*3+1]))
    {
     changed = 1;
     break;
    }
    f_tmp++;
   }
   if (changed)
   {
    f_tmp = 0;
    while (f_tmp < this->num)
    {
     this->func(this->Box, obj_tmplist->object,
		obj_tmplist->type,
		f_tmp, 
		obj_tmplist->contain+this->off_inout[f_tmp*3+2]);
     f_tmp++;
    }
   }
  }
  obj_tmplist = obj_tmplist->Next;
 }
}

static boolean ModifList_something_to_change(This)
c_ModifList *This;
{
 obj_save	*obj_tmplist;
 int		f_tmp;

 obj_tmplist = This->o_save;
 while (obj_tmplist)
 {
  f_tmp = 0;
  if (obj_tmplist->type != TOSET)
   return TRUE;
  else
  {
   while(f_tmp < This->num)
   {
    if (memcmp((char *)obj_tmplist->object + This->off_inout[f_tmp * 3], 
	       obj_tmplist->contain + This->off_inout[f_tmp * 3 + 2], 
	       This->off_inout[f_tmp * 3 + 1]))
     return TRUE;
    f_tmp++;
   }
  }
  obj_tmplist = obj_tmplist->Next;
 }

 return FALSE;
}
