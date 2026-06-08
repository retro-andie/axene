/*
** Box_select.c for XInstall in Box_select/
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
** Started on  Thu Jan 16 00:49:53 1997 Antoine Buat
** Last update Sat Jan 15 20:08:47 2000 Emmanuel Paris
*/

/*--- include ---*/
#include "Box_select.h"
#include "../Pixmaps/check_no.xpm"
#include "../Pixmaps/check_yes.xpm"
#include "AlertBox.h"
#include "Cursor.h"
#include "Colormap.h"
#include "fnmatch.h"
#include "file.h"
#include <sys/wait.h>
#include "mainzip.h"
#include "Tar.h"
#include <signal.h>

/*-- interface fixed size --*/
#define FRAME_SIZE 30

/*-- buffer fixed size --*/
#define PIPE_SIZE 100
#define LOAD_SIZE 30000
#define ALLOC_SIZE 10000

/*--- prototypes ---*/
void *cons_Box_select();
void dest_Box_select();
void *copy_Box_select();
static int BSwaitForUnMap();
static void launch_package_select();
static void select_all_packages();
void read_files();
void close_mf();
char *get_file();
mem_file *get_first();
char **convert_load_xpm();
char *convert_label();
void print_buf();
void create_interface();
int get_num();
void add_dep();
void sub_dep();
void print_dep();
void make_dep();
void unmake_dep();
static void deletePackList ___PROTO((icon_list **Ilist, boolean onlyone));

/*--- external ---*/
extern c_GCursor *GlobCursor;
extern c_Colormap *GlobColormap;
extern void XcHandleEvent();
extern int soft;

#define NB_MAN_LANG 6
static char *manlanguage[NB_MAN_LANG] = 
{ "English", "English", "English", "French", "German", "Spanish" };

/*--- fill class method ---*/
sf_Box_select fc_Box_select =
{
 cons_Box_select,
 dest_Box_select,
 copy_Box_select,
 BSwaitForUnMap,
 deletePackList
};

/* ----------------------------------------------------------------- ** 
** constructeur                                                      ** 
** ----------------------------------------------------------------- */
void *cons_Box_select(w_Parent, pszTitle, SrcDir, DstDir, tongue, pack_list)
Widget		w_Parent;
char		*pszTitle;
char		*SrcDir;
char		*DstDir;
int		tongue;
icon_list	**pack_list;
{
 c_Box_select		*This;
 struct statfs		infofs;
 char			string[50];
 Arg			argsw[5];
 XmString		xms;
 c_Form			*formWaiting;
 c_Frame		*fWaiting;
 c_Label		*lWaiting;
 XtAppContext		app;
 Dimension		h1, h2;
 int			count, i, j;
 long			lbackg;

 XtVaGetValues(w_Parent, XmNwidth, &h1, XmNheight, &h2, NULL);
  
 formWaiting = (c_Form *)NEW(c_Form)(w_Parent, NULL, "formWaiting", TRUE);
 XtVaSetValues(WIDGET(formWaiting),
	       XmNwidth, h1,
	       XmNheight, h2,
	       XmNbackground,7,
	       NULL);
 fWaiting = (c_Frame *)NEW(c_Frame)(WIDGET(formWaiting), NULL, NULL, 0);
 XtVaSetValues(WIDGET(fWaiting),
	       XmNtopOffset, 180,
	       XmNbottomOffset, 180,
	       XmNleftOffset, 100,
	       XmNrightOffset, 100,
	       XmNtopAttachment, XmATTACH_FORM,
	       XmNbottomAttachment, XmATTACH_FORM,
	       XmNleftAttachment, XmATTACH_FORM,
	       XmNrightAttachment,  XmATTACH_FORM,
	       XmNresizable, True,
	       NULL);
 lWaiting = (c_Label *)NEW(c_Label)(WIDGET(fWaiting), NULL, "lWaiting");
 XtManageChild(WIDGET(lWaiting));
 XtManageChild(WIDGET(fWaiting));
 XtManageChild(WIDGET(formWaiting));
 app = XtWidgetToApplicationContext(WIDGET(lWaiting));
 XSync(XtDisplay(WIDGET(lWaiting)), 0);
 while (XtAppPending(app))
 {
  XEvent event;
    
  XtAppNextEvent(app, &event);
  XcHandleEvent(&event);
 }
  
	      
 if((This = (c_Box_select *)Xc_malloc("Box_select",
				      sizeof(c_Box_select))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }

 This->artificial_select = NORMAL_SELECT;
 This->f = &fc_Box_select;
 This->used_size = 0;
 This->select_count = 0;
 This->tongue = tongue;
 This->Ilist = pack_list;

 SET_WAIT_MODE_STATIC;

 
 This->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
						      pszTitle, h1, h2,
						      XcCD_HORIZONTAL);
    
 This->w_This = WIDGET(This->Dialog);

 This->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(This),
				    MANAGER(This->Dialog),
				    "gBoxSelect", TRUE);
 /*--- grid son ---*/
 This->Ltitre = 
  (c_Label *)NEW(c_Label)(WIDGET(This->Grid),
			  MANAGER(This->Dialog),
			  "lBoxSelectTitle");

 This->FrameInfo1 = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					    MANAGER(This->Dialog), 
					    NULL, 0);
 This->LInfo1 = (c_Label *)NEW(c_Label)(WIDGET(This->FrameInfo1),
					MANAGER(This->Dialog),
					"lBIInfo1");
 XtSetArg(argsw[0], XmNbackground, &lbackg);
 XtGetValues(WIDGET(This->LInfo1), argsw, 1);
  
 This->FrameInfo2 = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					    MANAGER(This->Dialog), 
					    NULL, 0);
 This->LInfo2 = XtVaCreateWidget("lBIInfo2",
				 xmPushButtonWidgetClass, 
				 WIDGET(This->FrameInfo2),
				 NULL);
 F(MANAGER(This->Dialog)).Add_child(MANAGER(This->Dialog), This->LInfo2,
				    WIDGET(This->FrameInfo2));
 XtVaSetValues(This->LInfo2, XmNshadowThickness, 0, XmNtraversalOn, False,
	       XmNrecomputeSize, False, XmNbackground, lbackg, NULL);

 CALLBACK(This->LInfo2, FUNC_ACTIVATE, select_all_packages, This);


 sprintf(string, 
	 F(GlobResources).getString(GlobResources,
				    XcR_noPackageSelected));
 xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[0], XmNlabelString, xms);
 XtSetValues(This->LInfo2, argsw, 1);
 XmStringFree(xms);
  
 This->FrameSelect = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					     MANAGER(This->Dialog), 
					     NULL, 0);

 This->SWindow = 
  (c_ScrolledWindow *)NEW(c_ScrolledWindow)(WIDGET(This->FrameSelect),
					    MANAGER(This->Dialog),
					    "swBoxSelect");

 This->Grid2 = (c_Grid *)NEW(c_Grid)(WIDGET(This->SWindow),
				     MANAGER(This->Dialog),
				     "gBoxSelect2", TRUE);

 This->FrameFreeSpace = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
						MANAGER(This->Dialog), 
						NULL, 0);
 This->FrameUsedSpace = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
						MANAGER(This->Dialog), 
						NULL, 0);
  
 /*--- grid2 son ---*/
 This->GridIcon1 = (c_Grid *)NEW(c_Grid)(WIDGET(This->Grid2),
					 MANAGER(This->Dialog),
					 "gBSIcon1", TRUE);
 This->GridIcon2 = (c_Grid *)NEW(c_Grid)(WIDGET(This->Grid2),
					 MANAGER(This->Dialog),
					 "gBSIcon2", TRUE);

 /*--- read all '*.tar.gz' file ---*/
 read_files(This, SrcDir);

 /*--- gridIcon1 & 2 add col ---*/
 create_interface(This, &count);
  
 switch (count)
 {
 case 0:
  sprintf(string, 
	  F(GlobResources).getString(GlobResources,
				     XcR_noPackageFound));
  
  break;
 case 1:
  sprintf(string, 
	  F(GlobResources).getString(GlobResources,
				     XcR_onePackageFound));
  break;
 default:
  sprintf(string, 
	  F(GlobResources).getString(GlobResources,
				     XcR_manyPackageFound), count);
 }
 
 xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[0], XmNlabelString, xms);
 XtSetValues(WIDGET(This->LInfo1), argsw, 1);
 XmStringFree(xms);


 /*--- grid2 addcol ---*/
 F(This->Grid2).addColumn(This->Grid2, XcG_MAX_SIZE,
			  WIDGET(This->GridIcon1), XcG_MAX_SIZE,
			  WIDGET(This->GridIcon2), XcG_MAX_SIZE,
			  NULL);

 This->LFreeSpace = (c_Label *)NEW(c_Label)(WIDGET(This->FrameFreeSpace),
					    MANAGER(This->Dialog),
					    "lBIFreeSpace");

 if (!statfs(DstDir, &infofs))
 {
  This->free_space = infofs.f_bavail * infofs.f_bsize;
  sprintf(string, F(GlobResources).getString(GlobResources, XcR_freeSpace), 
	  This->free_space / 1024);
  xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
  XtSetArg(argsw[0], XmNlabelString, xms);
  XtSetValues(WIDGET(This->LFreeSpace), argsw, 1);
  XmStringFree(xms);
 }
 else
  This->free_space = (unsigned long)-1;
 
  
 This->LUsedSpace = (c_Label *)NEW(c_Label)(WIDGET(This->FrameUsedSpace),
					    MANAGER(This->Dialog),
					    "lBIUsedSpace");
			
 This->used_size = 0;
 sprintf(string, F(GlobResources).getString(GlobResources,
					    XcR_usedSpace), 0);
 xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[0], XmNlabelString, xms);
 XtSetValues(WIDGET(This->LUsedSpace), argsw, 1);
 XmStringFree(xms); 
 
 XtSetArg(argsw[0], XmNforeground, &This->lforeground);
 XtGetValues(WIDGET(This->LUsedSpace), argsw, 1);
   
 /*--- grid addcol ---*/
 F(This->Grid).addColumn(This->Grid, XcG_BEST_SIZE,
			 WIDGET(This->Ltitre), XcG_MAX_SIZE,
			 NULL);
 F(This->Grid).addColumn(This->Grid, FRAME_SIZE,
			 WIDGET(This->FrameInfo1), XcG_MAX_SIZE,
			 WIDGET(This->FrameInfo2), XcG_MAX_SIZE,
			 NULL);
 F(This->Grid).addColumn(This->Grid, 7, NULL);
 F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE,
			 WIDGET(This->FrameSelect), XcG_MAX_SIZE,
			 NULL);
 F(This->Grid).addColumn(This->Grid, 7, NULL);
 F(This->Grid).addColumn(This->Grid, FRAME_SIZE,
			 WIDGET(This->FrameFreeSpace), XcG_MAX_SIZE,
			 WIDGET(This->FrameUsedSpace), XcG_MAX_SIZE,
			 NULL);

 F(This->Dialog).createActionArea(This->Dialog, 
				  XcCD_CANCEL | XcCD_BACK | XcCD_INSTALL,
				  XcCD_INSTALL,
				  NULL, NULL);
  
 This->bInstallAlreadyGrised = TRUE;
 i = XcCD_INSTALL; for(j=0; i != 1; i >>= 1, j++);
 XtSetSensitive(This->Dialog->w_button[j], False);
  
 DELETE(c_Label)(lWaiting);
 DELETE(c_Frame)(fWaiting);
 DELETE(c_Form)(formWaiting);
 UNSET_WAIT_MODE;

 F(This->Dialog).map(This->Dialog, TRUE);
 
 XtVaSetValues(WIDGET(This->Grid2), XmNwidth, 580, XmNheight, 
	       20 + (count + 1) / 2 * 61, NULL);
 
 Xc_TRACE(("Object Box_select build"));
 return This;
}

/* ----------------------------------------------------------------- ** 
** destructeur                                                       ** 
** ----------------------------------------------------------------- */
void dest_Box_select(This)
c_Box_select *This;
{
 icon_list *pack_list = *This->Ilist;

 DELETE(c_Label)(This->LUsedSpace);
 DELETE(c_Label)(This->LFreeSpace);

 while(pack_list)
 {
  DELETE(c_Icon)(pack_list->IconValid);
  if (pack_list->pix_buffer)
   DELETE(c_Icon)(pack_list->Icon);
  XtDestroyWidget(pack_list->Label);

  pack_list = pack_list->Next;
 }

/* sould not be there (this is used to transmit the package to install list 
  F(This).deletePackList(This->Ilist, FALSE);
 */

 DELETE(c_Grid)(This->GridIcon1);
 DELETE(c_Grid)(This->GridIcon2);
 DELETE(c_Frame)(This->FrameUsedSpace);
 DELETE(c_Frame)(This->FrameFreeSpace);
 DELETE(c_Grid)(This->Grid2);
 DELETE(c_ScrolledWindow)(This->SWindow);
 DELETE(c_Frame)(This->FrameSelect);
 XtDestroyWidget(This->LInfo2);
 DELETE(c_Frame)(This->FrameInfo2);
 DELETE(c_Label)(This->LInfo1);
 DELETE(c_Frame)(This->FrameInfo1);
 DELETE(c_Label)(This->Ltitre);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);
  
 Xc_free(This);
 Xc_TRACE(("Object Box_select destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copieur                                                           ** 
** ----------------------------------------------------------------- */
void *copy_Box_select(This)
c_Box_select *This;
{
 Xc_TRACE(("Copy not impemented"));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** specific wait fo unmap                                            ** 
** ----------------------------------------------------------------- */
static int BSwaitForUnMap(This)
c_Box_select *This;
{
 int	rc;
 char   string[200];
 
 rc = 0;
 while(!rc)
 {
  rc = F(This->Dialog).waitForUnmap(This->Dialog);
  
  if (rc == XcCD_INSTALL)
  {
   if (This->used_size > This->free_space)
   {
    sprintf(string, F(GlobResources).getString
	    (GlobResources, XcR_notEnoughSpace), 
	    (This->used_size - This->free_space + 1023) / 1024);
    Xc_ErrorAlert(string);
    rc = 0;
    
    F(This->Dialog).map(This->Dialog, TRUE);
   }
  }
  else break;
 }
 return rc;
}

/* ----------------------------------------------------------------- ** 
** launch package select                                             ** 
** ----------------------------------------------------------------- */
static void launch_package_select(w, Obj)
Widget		w;
c_Box_select	*Obj;
{
 Boolean	state;
 icon_list	*ilist;

 Xc_HISTORY(("package push button"));
  
 ilist = *(Obj->Ilist);
 while(ilist)
 {
  if (((ilist->Icon) && (w == WIDGET(ilist->Icon))) || (w == ilist->Label))
  {
   state = XmToggleButtonGetState(WIDGET(ilist->IconValid));
   XmToggleButtonSetState(WIDGET(ilist->IconValid), (state+1)%2, True);
   return;
  }
  ilist = ilist->Next;
 }
}

/* ----------------------------------------------------------------- ** 
** select all packages                                               ** 
** ----------------------------------------------------------------- */
static void select_all_packages(w, Obj)
Widget		w;
c_Box_select	*Obj;
{
 Boolean	state;
 icon_list	*ilist;

 Xc_HISTORY(("package select all"));
  
 ilist = *(Obj->Ilist);
 while(ilist)
 {
  state = XmToggleButtonGetState(WIDGET(ilist->IconValid));
  if(!state)
   XmToggleButtonSetState(WIDGET(ilist->IconValid), True, True);
  ilist = ilist->Next;
 }
 return;
}

/* ----------------------------------------------------------------- ** 
** change state                                                      ** 
** ----------------------------------------------------------------- */
static void change_state(w, Obj)
Widget		w;
c_Box_select	*Obj;
{
 icon_list	*ilist;
 Arg		argsw[2];
 int		nargs;
 XmString	xms;
 char		string[100];
 int		i, j;
 unsigned long	used_size;

 Xc_HISTORY(("change"));
 
 used_size = Obj->used_size;
 ilist = *(Obj->Ilist);
 while(ilist)
 {
  if (w == WIDGET(ilist->IconValid))
  {
   if (ilist->selected)
   {
    if (ilist->mandated)
    {
     Obj->artificial_select = NORMAL_SELECT;
     return;
    }
    ilist->selected = FALSE;
    ilist->sel_type = NONE_SELECT;
    unmake_dep(Obj, ilist);
    Obj->used_size -= ilist->size;
    Obj->select_count--;
   }
   else
   {
    ilist->selected = TRUE;
    ilist->sel_type = Obj->artificial_select;
    make_dep(Obj, ilist);
    Obj->used_size += ilist->size;
    Obj->select_count++;
   }
   sprintf(string, F(GlobResources).getString(GlobResources, XcR_usedSpace), 
	   Obj->used_size / 1024);
   xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
 
   XtSetArg(argsw[0], XmNlabelString, xms); nargs = 1;
   if (Obj->used_size > Obj->free_space && used_size <= Obj->free_space)
   {
    XtSetArg(argsw[1], XmNforeground, GlobColormap->Cred); nargs = 2;
   }
   else 
    if (Obj->used_size <= Obj->free_space && used_size > Obj->free_space)
    {
     XtSetArg(argsw[1], XmNforeground, Obj->lforeground); nargs = 2;
    }
   XtSetValues(WIDGET(Obj->LUsedSpace), argsw, nargs);
   XmStringFree(xms);
	  
   switch (Obj->select_count)
   {
   case 0:
    sprintf(string, 
	    F(GlobResources).getString(GlobResources,
				       XcR_noPackageSelected));	
    if (!Obj->bInstallAlreadyGrised)
    {
     Obj->bInstallAlreadyGrised = TRUE;
     i=XcCD_INSTALL; for(j=0; i != 1; i >>= 1, j++);
     XtSetSensitive(Obj->Dialog->w_button[j], False);
    }
    break;
   case 1:
    sprintf(string, 
	    F(GlobResources).getString(GlobResources,
				       XcR_onePackageSelected));
    if (Obj->bInstallAlreadyGrised)
    {
     Obj->bInstallAlreadyGrised = FALSE;
     i=XcCD_INSTALL; for(j=0; i != 1; i >>= 1, j++);
     XtSetSensitive(Obj->Dialog->w_button[j], True);
    }
    break;
   default:
    sprintf(string, 
	    F(GlobResources).getString(GlobResources, XcR_manyPackageSelected),
	    Obj->select_count);
    if (Obj->bInstallAlreadyGrised)
    {
     Obj->bInstallAlreadyGrised = FALSE;
     i=XcCD_INSTALL; for(j=0; i != 1; i >>= 1, j++);
     XtSetSensitive(Obj->Dialog->w_button[j], True);
    }
   }
   xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(argsw[0], XmNlabelString, xms);
   XtSetValues(Obj->LInfo2, argsw, 1);
   XmStringFree(xms);

   Obj->artificial_select = NORMAL_SELECT;
   return;
  }
  ilist = ilist->Next;
 }
 Obj->artificial_select = NORMAL_SELECT;
}

/* ----------------------------------------------------------------- ** 
** read_files                                                        ** 
** ----------------------------------------------------------------- */
void read_files(This, path)
c_Box_select	*This;
char		*path;
{
 DIR		*dir_handle;
 struct dirent	*dir_entry;
 char		*buffer;
 icon_list	*Ilist, *Itmp, *Itmp2;
 int		path_len;
 mem_file	*mf;
 
 path_len = strlen(path);
 F(This).deletePackList(This->Ilist, FALSE);
 Ilist = NULL;
 
 if((dir_handle = opendir(path)) == NULL)
 {
  Xc_SYSERROR((path));
 }
 while((dir_entry = readdir(dir_handle)) != NULL)
 {
  char	*filename;
  char	*fullname;
  char	c;

  filename = dir_entry->d_name;
  Xc_TRACE(("test: %s", filename));
  if((c = *filename) == 0)
   continue;
  if(c == '.')
   continue;
  if(fnmatch("*.tar.gz", filename, FNM_PERIOD))
   continue;

  Xc_TRACE(("filename: %s", filename));
  fullname = Xc_malloc("ifilename", path_len+1+strlen(filename)+1);
  if (path[path_len-1]=='/')
   sprintf(fullname, "%s%s", path, filename);
  else
   sprintf(fullname, "%s/%s", path, filename);

  mf = get_first(fullname);

  buffer = get_file(mf, ".Axene_desc");
  if (!buffer)
  {
   close_mf(mf);
   Xc_free(fullname);
   continue;
  }
  /*--- yes found ---*/
  if (!Ilist)
  {
   *This->Ilist = Ilist = Xc_malloc("ilist", sizeof(icon_list));
   Ilist->Prev = NULL;
  }
  else
  {
   Ilist->Next = Xc_malloc("ilist", sizeof(icon_list));
   Ilist->Next->Prev = Ilist;
   Ilist = Ilist->Next;
  }
  
  Ilist->filename = fullname;
  Ilist->selected = FALSE;
  Ilist->mandatory = NULL;
  Ilist->optional = NULL;
  Ilist->mandated = NULL;
  Ilist->optioned = NULL;
  Ilist->number = -1;
  Ilist->language = 0;
  Ilist->treated = FALSE;
  Ilist->pix_buffer = NULL;
  Ilist->Next = NULL;
  /*--- this set size, mandatory and optional ---*/
  Ilist->tmp_label = convert_label(buffer, Ilist);

  if (Ilist->language)
  {
   if (Ilist->language != This->tongue)
   {
    if (!Ilist->Prev)
    {
     F(This).deletePackList(&Ilist, TRUE);
     *This->Ilist = Ilist = NULL;
    }
    else
    {
     Itmp = Ilist->Prev;
     F(This).deletePackList(&Ilist, TRUE);
     Ilist = Itmp;
    }
    close_mf(mf);
    continue;
   }
   else
   {
    Itmp = *This->Ilist;
    while(Itmp && Itmp != Ilist)
    {
     if (Itmp->number == Ilist->number)
     {
      Itmp2 = Itmp->Next;
      if (Itmp->Prev)
       F(This).deletePackList(&Itmp, TRUE);
      else
       F(This).deletePackList(This->Ilist, TRUE);       
      Itmp = Itmp2;
     }
     else
      Itmp = Itmp->Next;
    }
   }
  }
 
  buffer = get_file(mf, ".Icon");
  if (buffer)
   Ilist->pix_buffer = convert_load_xpm(buffer, strlen(buffer));
  
  close_mf(mf);
 }
 
 /* delete same package unspecified tongue */
 Ilist = *This->Ilist;
 while(Ilist)
 {
  if (Ilist->language)
  {
   Itmp = Ilist->Next;
   while(Itmp)
   {
    if (Itmp->number == Ilist->number)
    {
     Itmp2 = Itmp->Next;
     F(This).deletePackList(&Itmp, TRUE);     
     Itmp = Itmp2;
    }
    else
     Itmp = Itmp->Next;
   }
  }
  Ilist = Ilist->Next;
 }
}

/* ----------------------------------------------------------------- ** 
** close a mf                                                        ** 
** ----------------------------------------------------------------- */
void close_mf(mf)
mem_file	*mf;
{
 mem_file	*pmf;

 while (mf)
 {
  pmf = mf->Next;
  Xc_free(mf->buffer);
  Xc_free(mf->name);
  Xc_free(mf);
  mf = pmf;
 }
}

/* ----------------------------------------------------------------- ** 
** get file in a mf                                                  ** 
** ----------------------------------------------------------------- */
char *get_file(mf, file)
mem_file	*mf;
char		*file;
{
 while (mf)
 {
  if (!strcmp(file, mf->name))
   return mf->buffer;
  mf = mf->Next;
 }
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** get first files in a mf                                           ** 
** ----------------------------------------------------------------- */
mem_file *get_first(tar)
char	*tar;
{
 int		std_tar_zip[2];
 int		std_zip_cat[2];
 int		ret;
 int		pid;
 mem_file	*mf;
 int		no_way;

 ret = pipe(std_tar_zip);
 if (ret == -1) 
 {
  perror("Could not create pipe");
  exit(1);
 }

 pid = vfork();
 if (pid == 0)
 {
  unsigned char	*nbuf;
  int	readed;

  ret = pipe(std_zip_cat);
  if (ret == -1) 
  {
   perror("Could not create pipe");
   exit(1);
  }

  pid = vfork();
  if (pid == 0)
  {
   int	filed;
   int	size_r;
   int	tsize;

   filed = open(tar, O_RDONLY);
   nbuf = Xc_malloc("pixmap_buf", LOAD_SIZE);
   readed = read(filed, nbuf, LOAD_SIZE);
   close(filed);
   tsize = 0;
   while (readed)
   {
    if (readed > PIPE_SIZE)
    {
     size_r = write(std_zip_cat[1], nbuf+tsize, PIPE_SIZE);
     readed -= PIPE_SIZE;
     tsize += PIPE_SIZE;
    }
    else
    {
     size_r = write(std_zip_cat[1], nbuf+tsize, readed);
     readed = 0;
    }
   }
   /*nbuf[0]=EOF;*/
   /*write(std_zip_cat[1], nbuf, 1);*/
   /*flush*/
   Xc_free(nbuf);
   close(std_zip_cat[1]);
   exit(0);
  }

  close(std_zip_cat[1]);
  dup2(std_tar_zip[1], 1);
  /*--- can't get out ---*/
  gunzip_fd(std_zip_cat[0]);
  close(std_zip_cat[0]);
  wait(&no_way);
  exit(0);
 }
 close(std_tar_zip[1]);

 mf = NULL;
 extract_tar_memory(std_tar_zip[0], &mf);
 close(std_tar_zip[0]);
 /*--- kill as gunzip can't get out ---*/
 kill(pid, SIGKILL);
 wait(&no_way);
 return mf;
}

/* ----------------------------------------------------------------- ** 
** load xpm                                                          ** 
** ----------------------------------------------------------------- */
char **convert_load_xpm(_str, size)
char	*_str;
int	size;
{
 int	i;
 char	**ret_table;
 int	table_i;
 int	s_pos;
 int	len;

 ret_table=(char **)Xc_malloc("load table", 500*sizeof(char*));
 i = 0;
 /*--- treatment ---*/
 while ((_str[i]!='=') && (i<size))
  i++;
 while ((_str[i]!='{') && (i<size))
  i++;
      
 table_i = 0;
 while ((i < size) && (table_i < 500))
 {
  while ((_str[i]!='"') && (_str[i]!='}') && (i<size))
   i++;
  if ((_str[i]=='}') || (i>=size))
   break;
  else
   i++;
  s_pos = i;
  len = 0;
  while ((_str[i]!='"') && (i<size))
  {
   i++;
   len++;
  }
  if (len > 0)
  {
   ret_table[table_i] = Xc_malloc("line", len+1);
   strncpy(ret_table[table_i], _str+s_pos, len);
   ret_table[table_i][len] = 0;
  }
  table_i++;
  if (i < size)
   i++;
 }
 ret_table[table_i] = NULL;
 ret_table = Xc_realloc("table_realloc", ret_table, 
			(table_i+1)*sizeof(char*));
 return ret_table;
}

/* ----------------------------------------------------------------- ** 
** convert label                                                     ** 
** ----------------------------------------------------------------- */
char *convert_label(str, CIcon)
char		*str;
icon_list	*CIcon;
{
 char		*ret_str;
 int		len;
 int		i;
 int		o;
 int		last;
 char		tmp_s[30];
 char		*tmp_ps;
 int		num;
 int		tnum;

 unsigned long	pack_size;

 /*--- find size ---*/
 len = last = i = 0;
 while (str[i] && (str[i]!='\n'))
  i++;
 if (i)
 {
  last = i;
  len = i;
  pack_size = 0;
  i++;
  while (str[i] != '\n')
  {
   if (str[i] == ' ')
   {
    i++;
    continue;
   }
   if ((str[i] < '0') || (str[i] > '9'))
   {
    pack_size = 0;
    break;
   }
   pack_size *= 10;
   pack_size += str[i]-'0';
   i++;
  }
 }
 else
  pack_size = 0;
 CIcon->size = pack_size;

 /*--- find numer & dependancies ---*/
 num = 0;
 while (str[i] && (str[i]!='\n'))
  i++;
 if (str[i])
 {
  i++;
  while (str[i] && (str[i]==' '))
   i++;
  if (str[i])
  {
   while (str[i] && (str[i] != ' ') && (str[i] != '\n'))
   {
    if ((str[i] < '0') || (str[i] > '9'))
    {
     num = 0;
     break;
    }
    num *= 10;
    num += str[i]-'0';
    i++;
   }
   CIcon->number = num;
   while (str[i] && (str[i]==' '))
    i++;
   if (str[i])
   {
    while (str[i] && (str[i] != ' ') && (str[i] != '\n'))
    {
     if ((str[i] < '0') || (str[i] > '9'))
     {
      num = 0;
      break;
     }
     num *= 10;
     num += str[i]-'0';
     i++;
    }
    CIcon->key_number = num;
    while (str[i] && (str[i]==' '))
     i++;
    if (str[i])
    {
     /* cherche dependances */
     Xc_TRACE(("gen for %d : %s", CIcon->number, str+i));
     while (str[i] && (str[i]!='\n'))
     {
      switch (str[i])
      {
      case 'O':
      case 'o':
       i++;
       Xc_TRACE(("opt for %d : %s", CIcon->number, str+i));
       tnum = get_num(str+i, &num);
       if (tnum != -1)	
       {
	add_dep(&(CIcon->optional), num);
	i += tnum;
       }
       break;
      case 'M':
      case 'm':
       i++;
       Xc_TRACE(("mand for %d : %s", CIcon->number, str+i));
       tnum = get_num(str+i, &num);
       if (tnum != -1)	
       {
	add_dep(&(CIcon->mandatory), num);
	i += tnum;
       }
       break;
      case 'L':
      case 'l':
       i++;
       Xc_TRACE(("lang for %d : %s", CIcon->number, str+i));
       tnum = get_num(str+i, &num);
       if (tnum != -1)
       {
	CIcon->language = num;
	i += tnum;
       }
       break;
      default:
       i++;
      }
     }
    }
   }
  }
 }
 
 ret_str=(char *)Xc_malloc("string", len*3+50);
 i=0;
 o=0;
 while ((i<last) && (o<len*3-1))
 {
  if (str[i]=='\\')
  {
   i++;
   switch (str[i])
   {
   case 'n':
    ret_str[o]='\n';
    i++;
    break;
   case '\\':
    i++;
   default:
    ret_str[o]='\\';
   }
   o++;
   continue;
  }
  if (str[i]=='[')
  {
   i++;
   while (str[i] && (str[i] != ']') && (o<len*3+30))
   {
    switch (str[i])
    {
    case 'b':
     if (pack_size)
      sprintf(tmp_s, "%ld %s", pack_size, 
	      F(GlobResources).getString(GlobResources, XcR_bytes));
     else
      sprintf(tmp_s, "%s", 
	      F(GlobResources).getString(GlobResources, XcR_unknownSize));
     strcpy(ret_str+o, tmp_s);
     o+=strlen(tmp_s);
     break;
    case 'k':
     if (pack_size)
      sprintf(tmp_s, "%.2f %s", (float)pack_size / 1024, 
	      F(GlobResources).getString(GlobResources, XcR_kiloBytes));
     else
      sprintf(tmp_s, "%s", 
	      F(GlobResources).getString(GlobResources, XcR_unknownSize));
     strcpy(ret_str+o, tmp_s);
     o+=strlen(tmp_s);
     break;
    case 'm':
     if (pack_size)
      sprintf(tmp_s, "%.2f %s", (float)pack_size / (1024*1024),
	      F(GlobResources).getString(GlobResources, XcR_megaBytes));
     else
      sprintf(tmp_s, "%s", 
	      F(GlobResources).getString(GlobResources, XcR_unknownSize));
     strcpy(ret_str+o, tmp_s);
     o+=strlen(tmp_s);
     break;
    case 's':
     tmp_ps = F(GlobResources).getString(GlobResources, XcR_size);
     strcpy(ret_str+o, tmp_ps);
     o+=strlen(tmp_ps);
     break;
    case 'n':
     tmp_ps = F(GlobResources).getString(GlobResources, XcR_notavailable);
     strcpy(ret_str+o, tmp_ps);
     o+=strlen(tmp_ps);
     break;
    case 'd':
     if (CIcon->key_number == soft || soft == 0)
     {
      while (o & (ret_str[o] != '\n'))
       o--;
      if (o)
       o--;
      while (str[i] && (str[i] != '\n'))
       i++;
      if (!str[i])
       i--;
      str[i]=']';
      i--;
     }
     else
     {
      tmp_ps = F(GlobResources).getString(GlobResources, XcR_demo);
      strcpy(ret_str+o, tmp_ps);
      o+=strlen(tmp_ps);
     }
     break;
    case 'v':
     tmp_ps = F(GlobResources).getString(GlobResources, XcR_manual);
     strcpy(ret_str+o, tmp_ps);
     o += strlen(tmp_ps);
     if (CIcon->language >= 0 && CIcon->language < NB_MAN_LANG)
     {
      sprintf(tmp_s, " (%s)", manlanguage[CIcon->language]);
      strcpy(ret_str + o, tmp_s);
      o += strlen(tmp_s);
     }
     break;
    case 'l':
     tmp_ps = F(GlobResources).getString(GlobResources, XcR_libraries);
     strcpy(ret_str+o, tmp_ps);
     o += strlen(tmp_ps);
     break;
    }
    i++;
   }
   i++;
   continue;
  }
  ret_str[o]=str[i];
  o++;
  i++;
 }
 ret_str[o]=0;
 return ret_str;
}

/* ----------------------------------------------------------------- ** 
** print buf                                                         ** 
** ----------------------------------------------------------------- */
void print_buf(nbuf, readed)
unsigned char	*nbuf;
int	readed;
{
 int	i;
 int	j;

 i=0;
 while(i < readed)
 {
  j=0;
  while ((j < 16) && (i < readed))
  {
   printf("%02x ", nbuf[i]);
   j++;
   i++;
  }
  i-=j;
  j=0;
  printf(" ");
  while ((j < 16) && (i < readed))
  {
   if (nbuf[i]<32)
    printf(".");
   else
    printf("%c", nbuf[i]);
   j++;
   i++;
  }
  printf("\n");
 }
}

/* ----------------------------------------------------------------- ** 
** create interface                                                  ** 
** ----------------------------------------------------------------- */
void create_interface(This, count)
c_Box_select	*This;
int		*count;
{
 icon_list	*ilist;
 icon_list	*found_icon;
 boolean	is_on_col1;
 Display	*display;

 *count = 0;
 is_on_col1 = TRUE;
 display = XtDisplay(WIDGET(This->Dialog));
 
 while (1)
 {
  /*--- find the smallest ---*/
  found_icon = NULL;
  ilist = *(This->Ilist);
  while(ilist)
  {
   if (!ilist->treated)
   {
    if (found_icon)
    {
     if (found_icon->number > ilist->number)
      found_icon = ilist;
    }
    else
     found_icon = ilist;
   }
   ilist = ilist->Next;
  }
  if (!found_icon)
   return;
 
  if (is_on_col1)
  {
   XmString		xms;
   Arg			argsw[2];

   found_icon->Label = XtVaCreateWidget("sel_label",
					xmPushButtonWidgetClass, 
					WIDGET(This->GridIcon1),
					NULL);
   xms = XmStringLtoRCreate(found_icon->tmp_label, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(argsw[0], XmNlabelString, xms);
   XtSetValues(found_icon->Label, argsw, 1);
   XmStringFree(xms);
   F(MANAGER(This->Dialog)).Add_child(MANAGER(This->Dialog), found_icon->Label,
				      WIDGET(This->GridIcon1));
   if (found_icon->size)
    CALLBACK(found_icon->Label, FUNC_ACTIVATE, 
	     launch_package_select, This);

   if (found_icon->pix_buffer)
   {
    found_icon->Icon = (c_Icon *)NEW(c_Icon)(WIDGET(This->GridIcon1),
					     MANAGER(This->Dialog),
					     "tbBSIconPack", display,
					     found_icon->pix_buffer,
					     NULL, XcPush);
    if (found_icon->size)
     CALLBACK(WIDGET(found_icon->Icon), FUNC_ACTIVATE, 
	      launch_package_select, This);
   }
   else
    found_icon->Icon = NULL;
   if (found_icon->size)
   {
    found_icon->IconValid = (c_Icon *)NEW(c_Icon)
     (WIDGET(This->GridIcon1), MANAGER(This->Dialog),
      "tbBSSelect", display,
      check_no_xpm,
      check_yes_xpm, XcToggle);
    CALLBACK(WIDGET(found_icon->IconValid), FUNC_VALUE,
	     change_state, This);
   }
   else
    found_icon->IconValid = (c_Icon *)NEW(c_Icon)
     (WIDGET(This->GridIcon1), MANAGER(This->Dialog),
      "tbBSSelect", display,
      check_no_xpm,
      check_yes_xpm, XcLabel);
   if (found_icon->pix_buffer)
    F(This->GridIcon1).
     addColumn(This->GridIcon1, XcG_BEST_SIZE,
	       WIDGET(found_icon->IconValid), XcG_BEST_SIZE,
	       WIDGET(found_icon->Icon), XcG_BEST_SIZE,
	       found_icon->Label, XcG_MAX_SIZE,
	       NULL);
   else
    F(This->GridIcon1).
     addColumn(This->GridIcon1, XcG_BEST_SIZE,
	       WIDGET(found_icon->IconValid), XcG_BEST_SIZE,
	       found_icon->Label, XcG_MAX_SIZE,
	       NULL);
   is_on_col1 = FALSE;
  }
  else
  {
   XmString		xms;
   Arg			argsw[2];

   found_icon->Label = XtVaCreateWidget("sel_label",
					xmPushButtonWidgetClass, 
					WIDGET(This->GridIcon2),
					NULL);
   xms = XmStringLtoRCreate(found_icon->tmp_label, XmSTRING_DEFAULT_CHARSET);
   XtSetArg(argsw[0], XmNlabelString, xms);
   XtSetValues(found_icon->Label, argsw, 1);
   XmStringFree(xms);
   F(MANAGER(This->Dialog)).Add_child(MANAGER(This->Dialog), found_icon->Label,
				      WIDGET(This->GridIcon2));
   if (found_icon->size)
    CALLBACK(found_icon->Label, FUNC_ACTIVATE, 
	     launch_package_select, This);

   if (found_icon->pix_buffer)
   {
    found_icon->Icon = (c_Icon *)NEW(c_Icon)(WIDGET(This->GridIcon2),
					     MANAGER(This->Dialog),
					     "tbBSIconPack", display,
					     found_icon->pix_buffer,
					     NULL, XcPush);
    if (found_icon->size)
     CALLBACK(WIDGET(found_icon->Icon), FUNC_ACTIVATE, 
	      launch_package_select, This);
   }
   else
    found_icon->Icon = NULL;
   if (found_icon->size)
   {
    found_icon->IconValid = (c_Icon *)NEW(c_Icon)
     (WIDGET(This->GridIcon2), MANAGER(This->Dialog),
      "tbBSSelect", display,
      check_no_xpm,
      check_yes_xpm, XcToggle);
    CALLBACK(WIDGET(found_icon->IconValid), FUNC_VALUE,
	     change_state, This);
   }
   else
    found_icon->IconValid = (c_Icon *)NEW(c_Icon)
     (WIDGET(This->GridIcon2), MANAGER(This->Dialog),
      "tbBSSelect", display,
      check_no_xpm,
      check_yes_xpm, XcLabel);
   if (found_icon->pix_buffer)
    F(This->GridIcon2).
     addColumn(This->GridIcon2, XcG_BEST_SIZE,
	       WIDGET(found_icon->IconValid), XcG_BEST_SIZE,
	       WIDGET(found_icon->Icon), XcG_BEST_SIZE,
	       found_icon->Label, XcG_MAX_SIZE,
	       NULL);
   else
    F(This->GridIcon2).
     addColumn(This->GridIcon2, XcG_BEST_SIZE,
	       WIDGET(found_icon->IconValid), XcG_BEST_SIZE,
	       found_icon->Label, XcG_MAX_SIZE,
	       NULL);
   is_on_col1 = TRUE;
  }
  found_icon->treated = TRUE;
  (*count)++;
 }
}

/* ----------------------------------------------------------------- ** 
** get num                                                           ** 
** ----------------------------------------------------------------- */
int get_num(str, num)
char	*str;
int	*num;
{
 int	i;

 i = 0;
 *num = 0;
 while (str[i] && (str[i] == ' '))
  i++;
 if (!str[i])
  return -1;
 while (str[i] && (str[i] != ' ') && (str[i] != '\n'))
 {
  if ((str[i] < '0') || (str[i] > '9'))
  {
   num = 0;
   return -1;
  }
  (*num) *= 10;
  (*num) += str[i]-'0';
  i++;
 }
 return i;
}

/* ----------------------------------------------------------------- ** 
** add dep                                                           ** 
** ----------------------------------------------------------------- */
void add_dep(dep, num)
icon_dep	**dep;
int		num;
{
 while (*dep)
  dep = &((*dep)->next);
 *dep = Xc_malloc("icon dep", sizeof(icon_dep));
 (*dep)->number = num;
 (*dep)->next = NULL;
}

/* ----------------------------------------------------------------- ** 
** sub dep                                                           ** 
** ----------------------------------------------------------------- */
void sub_dep(dep, num)
icon_dep	**dep;
int		num;
{
 icon_dep	*tdep;

 while (*dep && ((*dep)->number != num))
  dep = &((*dep)->next);
 if (*dep)
 {
  tdep = *dep;
  *dep = tdep->next;
  Xc_free(tdep);
 }
}

/* ----------------------------------------------------------------- ** 
** print dep                                                         ** 
** ----------------------------------------------------------------- */
void print_dep(CIcon)
icon_list	*CIcon;
{
 icon_dep	*dep;

 dep = CIcon->optional;
 printf("optional:");
 while (dep)
 {
  printf("%d ", dep->number);
  dep = dep->next;
 }
 dep = CIcon->mandatory;
 printf("\nmandatory:");
 while (dep)
 {
  printf("%d ", dep->number);
  dep = dep->next;
 }
 printf("\n");
}

/* ----------------------------------------------------------------- ** 
** make dep                                                          ** 
** ----------------------------------------------------------------- */
void make_dep(This, CIcon)
c_Box_select	*This;
icon_list	*CIcon;
{
 icon_list	*tlist;
 icon_dep	*dep;

 dep = CIcon->optional;
 while (dep)
 {
  tlist = *(This->Ilist);
  while (tlist && (tlist->number != dep->number))
   tlist = tlist->Next;
  if (tlist)
  {
   add_dep(&(tlist->optioned), CIcon->number);
   This->artificial_select = OPTION_SELECT;
   XmToggleButtonSetState(WIDGET(tlist->IconValid), 1, True);
  }
  dep = dep->next;
 }

 dep = CIcon->mandatory;
 while (dep)
 {
  tlist = *(This->Ilist);
  while (tlist && (tlist->number != dep->number))
   tlist = tlist->Next;
  if (tlist)
  {
   add_dep(&(tlist->mandated), CIcon->number);
   This->artificial_select = MANDAT_SELECT;
   XmToggleButtonSetState(WIDGET(tlist->IconValid), 1, True);
   REMOVE_CALLBACK(tlist->Label, FUNC_ACTIVATE, 
		   launch_package_select, This);
   REMOVE_CALLBACK(WIDGET(tlist->Icon), FUNC_ACTIVATE, 
		   launch_package_select, This);
   XtSetSensitive(WIDGET(tlist->IconValid), False);
   REMOVE_CALLBACK(WIDGET(tlist->IconValid), FUNC_VALUE, 
		   change_state, This);
  }
  dep = dep->next;
 } 
}

/* ----------------------------------------------------------------- ** 
** unmake dep                                                        ** 
** ----------------------------------------------------------------- */
void unmake_dep(This, CIcon)
c_Box_select	*This;
icon_list	*CIcon;
{
 icon_list	*tlist;
 icon_dep	*dep;

 dep = CIcon->optional;
 while (dep)
 {
  tlist = *(This->Ilist);
  while (tlist && (tlist->number != dep->number))
   tlist = tlist->Next;
  if (tlist)
  {
   sub_dep(&(tlist->optioned), CIcon->number);
   if (tlist->selected && (tlist->sel_type != NORMAL_SELECT))
   {
    This->artificial_select = NONE_SELECT;
    XmToggleButtonSetState(WIDGET(tlist->IconValid), 0, True);    
   }
  }
  dep = dep->next;
 } 

 dep = CIcon->mandatory;
 while (dep)
 {
  tlist = *(This->Ilist);
  while (tlist && (tlist->number != dep->number))
   tlist = tlist->Next;
  if (tlist)
  {
   int		i;
   icon_dep	*mand;
   sub_dep(&(tlist->mandated), CIcon->number);
   /*--- compte le nombre de mandated restant ---*/
   mand = tlist->mandated;
   i=0;
   while (mand)
   {
    i++;
    mand = mand->next;
   }
   if (!i)
   {
    CALLBACK(tlist->Label, FUNC_ACTIVATE, 
	     launch_package_select, This);
    CALLBACK(WIDGET(tlist->Icon), FUNC_ACTIVATE, 
	     launch_package_select, This);
    XtSetSensitive(WIDGET(tlist->IconValid), True);
    CALLBACK(WIDGET(tlist->IconValid), FUNC_VALUE, 
	     change_state, This);
    if (tlist->selected && (tlist->sel_type != NORMAL_SELECT))
    {
     This->artificial_select = NONE_SELECT;
     XmToggleButtonSetState(WIDGET(tlist->IconValid), 0, True);
    }
   }
   /*printf("Mandated number: %d\n", i);*/
  }
  dep = dep->next;
 } 
}

/* ----------------------------------------------------------------- ** 
** deletePackList: destroy global pack_list                          ** 
** ----------------------------------------------------------------- */
static void deletePackList(Ilist, onlyone)
icon_list **Ilist;
boolean onlyone;
{
 icon_list *pack_list = *Ilist;
 icon_list *pl2;
 icon_dep *id, *id2;

 if (!pack_list) return;
 
 while(pack_list)
 {
  Xc_free(pack_list->filename);
  Xc_free(pack_list->tmp_label);
  
  id = pack_list->mandatory;
  while(id){ id2 = id->next; Xc_free(id); id = id2; }
  id = pack_list->optional;
  while(id){ id2 = id->next; Xc_free(id); id = id2; }
  id = pack_list->mandated;
  while(id){ id2 = id->next; Xc_free(id); id = id2; }
  id = pack_list->optioned;
  while(id){ id2 = id->next; Xc_free(id); id = id2; }
  
  if (pack_list->pix_buffer)
  {
   int i = 0;
   
   while(pack_list->pix_buffer[i])
    Xc_free(pack_list->pix_buffer[i++]);
   
   Xc_free(pack_list->pix_buffer);
  }

  if (onlyone)
  {
   if (pack_list->Prev)
    pack_list->Prev->Next = pack_list->Next;
   else
   {
    *Ilist = pack_list->Next;
   }
   if (pack_list->Next)
    pack_list->Next->Prev = pack_list->Prev;
   Xc_free(pack_list);
   
   return;
  }
  pl2 = pack_list->Next;
  Xc_free(pack_list);
  pack_list = pl2;
 }

 *Ilist = NULL;
}
