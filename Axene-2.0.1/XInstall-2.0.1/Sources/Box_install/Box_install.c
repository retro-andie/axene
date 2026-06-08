/*
** Box_install.c for XInstall in Box_install/
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
** Last update Tue Feb  1 15:59:46 2000 Emmanuel Paris
*/

/*--- include ---*/
#include "Box_install.h"
#include "file.h"
#include "AlertBox.h"
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
#include "mainzip.h"
#include "Tar.h"
#include "Cursor.h"
#include "Resource.h"
#include "DocIcon.h"

/*--- include pixmap ---*/
#include "../Pixmaps/check_no.zpm"
#include "../Pixmaps/check_yes.zpm"
#include "../Pixmaps/led_on.zpm"
#include "../Pixmaps/led_off.zpm"
#include "../Pixmaps/led_on2.zpm"
#include "../Pixmaps/led_off2.zpm"

extern c_DocIcon *GlobDocIcon;

/*--- define ---*/

#define LABEL_SIZE	130
#define FRAME_SIZE	30
#define PB_WIDTH	310
#define PB_HEIGHT	30
#define PB2_WIDTH	200
#define PB2_HEIGHT	20

/*--- external ---*/
extern c_GCursor	*GlobCursor;
extern void XcHandleEvent ___PROTO((XEvent *event));
extern int selected_tongue;

/*--- prototypes ---*/
void *cons_Box_install();
void dest_Box_install();
void *copy_Box_install();

void install_package();
static void init_total();
static void end_total();
static void init_file();
static void change();
static void treat_event();
static void update_free_space();

/*--- fill class method ---*/
sf_Box_install fc_Box_install =
{
 cons_Box_install,
 dest_Box_install,
 copy_Box_install
};

char *strlanguage[5] = { "English.us", "English.uk", 
			  "French", "German", "Spanish" };

/* ----------------------------------------------------------------- ** 
** constructeur                                                      ** 
** ----------------------------------------------------------------- */
void *cons_Box_install(w_Parent, pszTitle, sdir, ddir, pack_list)
Widget		w_Parent;
char		*pszTitle;
char		*sdir;
char		*ddir;
icon_list	*pack_list;
{
 c_Box_install		*This;
 Display		*display;
 icon_list		*tmp_list;
 unsigned long		all_total;
 struct statfs		infofs;
 Arg			argsw[1];
 XmString		xms;
 int			i;
 Dimension		h1, h2;
 Pixmap			pixmap;

 if((This = (c_Box_install *)Xc_malloc("Box_install",
				       sizeof(c_Box_install))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 memset(This, 0, sizeof(c_Box_install));
 
 This->f = &fc_Box_install;
 This->SrcDir = sdir;
 This->DstDir = ddir;
 
 XtVaGetValues(w_Parent, XmNwidth, &h1, XmNheight, &h2, NULL);
 This->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
						      pszTitle, h1, h2,
						      XcCD_HORIZONTAL);
 This->w_This = WIDGET(This->Dialog);
 display = XtDisplay(WIDGET(This->Dialog));

 This->Icon = NULL;
 
 This->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(This),
				    MANAGER(This->Dialog),
				    "gBoxInstall", TRUE);

 /*--- grid son ---*/
 This->Ltitre = (c_Label *)NEW(c_Label)(WIDGET(This->Grid),
					MANAGER(This->Dialog),
					"lBoxInstallTitle");
 This->FrameInfo = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					   MANAGER(This->Dialog), 
					   NULL, 0);
 This->Lcurfile = (c_Label *)NEW(c_Label)(WIDGET(This->FrameInfo),
					  MANAGER(This->Dialog),
					  "lBDIinfo");
 This->FrameProgress = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					       MANAGER(This->Dialog), 
					       NULL, 0);
 This->FrameSpace = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					    MANAGER(This->Dialog), 
					    NULL, 0);

 /*--- grid frameProgress son ---*/
 This->GridProgress = (c_Grid *)NEW(c_Grid)(WIDGET(This->FrameProgress),
					    MANAGER(This->Dialog),
					    "gProgress", TRUE);

 /*--- form GridProgress son ---*/
 This->FrameFileGlob = (c_Frame *)NEW(c_Frame)(WIDGET(This->GridProgress),
					       MANAGER(This->Dialog), 
					       NULL, 0);
 This->GridFile = (c_Grid *)NEW(c_Grid)(WIDGET(This->FrameFileGlob),
					MANAGER(This->Dialog),
					"gFile", FALSE);
  
 This->Lfile = (c_Label *)NEW(c_Label)(WIDGET(This->GridFile),
				       MANAGER(This->Dialog),
				       "lBDIfile");
 This->LfileName = (c_XLabel *)NEW(c_XLabel)(WIDGET(This->GridFile),
					     MANAGER(This->Dialog),
					     "lBDIfileName",
					     XlALIGNMENT_BEGINNING,
					     XcR_installNormal);
 This->FrameFile = (c_Frame *)NEW(c_Frame)(WIDGET(This->GridFile),
					   MANAGER(This->Dialog), 
					   NULL, 0);
 This->PBfile = (c_ProgressBar *)NEW(c_ProgressBar)
  (WIDGET(This->FrameFile), MANAGER(This->Dialog),
   "pbBDIfile", PB_WIDTH, PB_HEIGHT, led_on_xpm, led_off_xpm);
 This->Lfilep = (c_XLabel *)NEW(c_XLabel)(WIDGET(This->GridFile),
					  MANAGER(This->Dialog),
					  "lBDIfilep",
					  XlALIGNMENT_END,
					  XcR_installItalic);

 This->FramePackageGlob = 
  (c_Frame *)NEW(c_Frame)(WIDGET(This->GridProgress),
			  MANAGER(This->Dialog), 
			  NULL, 0);
 This->GridPackage = (c_Grid *)NEW(c_Grid)(WIDGET(This->FramePackageGlob),
					   MANAGER(This->Dialog),
					   "gPackage", FALSE);
 This->Lpackage = (c_Label *)NEW(c_Label)(WIDGET(This->GridPackage),
					  MANAGER(This->Dialog),
					  "lBDIpackage");
 This->LpackageName = (c_XLabel *)NEW(c_XLabel)(WIDGET(This->GridPackage),
						MANAGER(This->Dialog),
						"lBDIpackageName",
						XlALIGNMENT_BEGINNING,
						XcR_installNormal);
 This->FramePackage = (c_Frame *)NEW(c_Frame)(WIDGET(This->GridPackage),
					      MANAGER(This->Dialog), 
					      NULL, 0);
 This->PBpackage = 
  (c_ProgressBar *)NEW(c_ProgressBar)(WIDGET(This->FramePackage),
				      MANAGER(This->Dialog),
				      "pbBDIpackage", PB_WIDTH, PB_HEIGHT,
				      led_on_xpm, led_off_xpm);
 This->Lpackagep = (c_XLabel *)NEW(c_XLabel)(WIDGET(This->GridPackage),
					     MANAGER(This->Dialog),
					     "lBDIpackagep",
					     XlALIGNMENT_END,
					     XcR_installItalic);
 
 This->FrameTotalGlob = (c_Frame *)NEW(c_Frame)(WIDGET(This->GridProgress),
						MANAGER(This->Dialog), 
						NULL, 0);
 This->GridTotal = (c_Grid *)NEW(c_Grid)(WIDGET(This->FrameTotalGlob),
					 MANAGER(This->Dialog),
					 "gTotal", TRUE);

 This->FrameIcon = (c_Frame *)NEW(c_Frame)(WIDGET(This->GridProgress),
					   MANAGER(This->Dialog), 
					   NULL, 0);

 
 This->FormIcon = (c_Form *)NEW(c_Form)(WIDGET(This->FrameIcon),
					 MANAGER(This->Dialog),
					"foIcon");
 XtVaSetValues(WIDGET(This->FormIcon), XmNwidth, 57, XmNheight, 57, 
	       XmNnoResize, True, XmNresizePolicy, XmRESIZE_NONE, 
	       XmNautoUnmanage, False, NULL);
 
 This->Ltotal = (c_Label *)NEW(c_Label)(WIDGET(This->GridTotal),
					MANAGER(This->Dialog),
					"lBDItotal");
 This->FrameTotal = (c_Frame *)NEW(c_Frame)(WIDGET(This->GridTotal),
					    MANAGER(This->Dialog), 
					    NULL, 0);
 This->PBtotal = (c_ProgressBar *)NEW(c_ProgressBar)
  (WIDGET(This->FrameTotal), MANAGER(This->Dialog),
   "pbBDItotal", PB_WIDTH, PB_HEIGHT, led_on2_xpm, led_off2_xpm);
 This->Ltotalp = (c_XLabel *)NEW(c_XLabel)(WIDGET(This->GridTotal),
					   MANAGER(This->Dialog),
					   "lBDItotalp",
					   XlALIGNMENT_END,
					   XcR_installItalic);
 
 /*--- GridProgress addcol ---*/
 F(This->GridFile).addColumn(This->GridFile, LABEL_SIZE,
			     WIDGET(This->Lfile), XcG_MAX_SIZE,
			     NULL);
 F(This->GridFile).addColumn(This->GridFile, XcG_MAX_SIZE,
			     WIDGET(This->FrameFile), 34,
			     WIDGET(This->LfileName), XcG_MAX_SIZE,
			     NULL);
 F(This->GridFile).addColumn(This->GridFile, 50,
			     WIDGET(This->Lfilep), XcG_MAX_SIZE,
			     NULL);
  
 F(This->GridPackage).addColumn(This->GridPackage, LABEL_SIZE,
				WIDGET(This->Lpackage), XcG_MAX_SIZE,
				NULL);
 F(This->GridPackage).addColumn(This->GridPackage, XcG_MAX_SIZE,
				WIDGET(This->FramePackage), 34,
				WIDGET(This->LpackageName), XcG_MAX_SIZE,
				NULL);
 F(This->GridPackage).addColumn(This->GridPackage, 50,
				WIDGET(This->Lpackagep), XcG_MAX_SIZE,
				NULL);
  
 F(This->GridTotal).addColumn(This->GridTotal, XcG_MAX_SIZE, NULL);
 F(This->GridTotal).addColumn(This->GridTotal, 44,
			      WIDGET(This->Ltotal), 80,
			      WIDGET(This->FrameTotal), XcG_MAX_SIZE,
			      WIDGET(This->Ltotalp), 50,
			      NULL);
 F(This->GridTotal).addColumn(This->GridTotal, XcG_MAX_SIZE, NULL);

 F(This->GridProgress).addColumn(This->GridProgress, XcG_MAX_SIZE, NULL);
 F(This->GridProgress).addColumn(This->GridProgress, 78,
				 WIDGET(This->FrameFileGlob), 
				 XcG_MAX_SIZE,
				 NULL);
 F(This->GridProgress).addColumn(This->GridProgress, XcG_MAX_SIZE, NULL);
 F(This->GridProgress).addColumn(This->GridProgress, 78,
				 WIDGET(This->FramePackageGlob),
				 XcG_MAX_SIZE,
				 NULL);
 F(This->GridProgress).addColumn(This->GridProgress, XcG_MAX_SIZE, NULL);
 F(This->GridProgress).addColumn(This->GridProgress, 78,
				 WIDGET(This->FrameIcon), 78,
				 XcG_WIDGET_NULL, 7,
				 WIDGET(This->FrameTotalGlob), 
				 XcG_MAX_SIZE,
				 NULL);
 F(This->GridProgress).addColumn(This->GridProgress, XcG_MAX_SIZE, NULL);

 /*--- framespace son ---*/
 This->FormSpace = (c_Form *)NEW(c_Form)(WIDGET(This->FrameSpace),
					 MANAGER(This->Dialog),
					 "foSpace", TRUE);


 /*--- form formdst son ---*/
 This->Lspace = (c_Label *)NEW(c_Label)(WIDGET(This->FormSpace),
					MANAGER(This->Dialog),
					"lBDIspace");

 This->PBspace = (c_ProgressBar *)NEW(c_ProgressBar)
  (WIDGET(This->FormSpace), MANAGER(This->Dialog),
   "pbBDIspace", PB2_WIDTH, PB2_HEIGHT, check_yes_xpm, check_no_xpm);

 if (!statfs(This->DstDir, &infofs))
 {
  This->disk_block = infofs.f_blocks;
 }
 F(This->PBspace).change_total(This->PBspace, This->disk_block);

 This->Lspacep = (c_XLabel *)NEW(c_XLabel)(WIDGET(This->FormSpace),
					   MANAGER(This->Dialog),
					   "lBDIspacep",
					   XlALIGNMENT_END,
					   XcR_installItalicSmall);
 /*
   This->wSspace = XtVaCreateWidget("separator", xmSeparatorGadgetClass,
   WIDGET(This->FormSpace),
   XmNorientation, XmVERTICAL,
   XmNshadowThickness, 2,
   NULL);
   
   F(MANAGER(This->Dialog)).Add_child(MANAGER(This->Dialog),
   This->wSspace, 
   WIDGET(This->FormSpace));
   This->Lspaceal = (c_Label *)NEW(c_Label)(WIDGET(This->FormSpace),
   MANAGER(This->Dialog),
   "lBDIspaceal");
   */
 This->Lspacea = (c_XLabel *)NEW(c_XLabel)(WIDGET(This->FormSpace),
					   MANAGER(This->Dialog),
					   "lBDIspacea",
					   XlALIGNMENT_END,
					   XcR_installNormal);
 update_free_space(This);


 /*--- formSpace attachement ---*/
 XtVaSetValues(WIDGET(This->Lspace),
	       XmNtopAttachment, XmATTACH_FORM,
	       XmNbottomAttachment, XmATTACH_FORM,
	       XmNleftAttachment, XmATTACH_FORM,
	       XmNleftOffset, 30,
	       XmNrightAttachment, XmATTACH_NONE,
	       XmNresizable, False,
	       NULL);
 XtVaSetValues(WIDGET(This->PBspace),
	       XmNtopAttachment, XmATTACH_FORM,
	       XmNbottomAttachment, XmATTACH_FORM,
	       XmNleftAttachment, XmATTACH_WIDGET,
	       XmNleftWidget, WIDGET(This->Lspace),
	       XmNleftOffset, 5,
	       XmNrightAttachment, XmATTACH_NONE,
	       XmNresizable, False,
	       NULL);
 XtVaSetValues(WIDGET(This->Lspacep),
	       XmNtopAttachment, XmATTACH_FORM,
	       XmNbottomAttachment, XmATTACH_FORM,
	       XmNleftAttachment, XmATTACH_WIDGET,
	       XmNleftWidget, WIDGET(This->PBspace),
	       XmNrightAttachment, XmATTACH_NONE,
	       XmNwidth, 50,
	       XmNresizable, False,
	       NULL);
 /* XtVaSetValues(This->wSspace,
    XmNtopAttachment, XmATTACH_FORM,
    XmNbottomAttachment, XmATTACH_FORM,
    XmNleftAttachment, XmATTACH_WIDGET,
    XmNleftWidget, WIDGET(This->Lspacep),
    XmNrightAttachment, XmATTACH_NONE,
    XmNresizable, False,
    NULL);
    XtVaSetValues(WIDGET(This->Lspaceal),
    XmNtopAttachment, XmATTACH_FORM,
    XmNbottomAttachment, XmATTACH_FORM,
    XmNleftAttachment, XmATTACH_WIDGET,
    XmNleftWidget, This->wSspace,
    XmNrightAttachment, XmATTACH_NONE,
    XmNresizable, True,
    NULL);
    */
 XtVaSetValues(WIDGET(This->Lspacea),
	       XmNtopAttachment, XmATTACH_FORM,
	       XmNbottomAttachment, XmATTACH_FORM,
	       XmNleftAttachment, XmATTACH_WIDGET,
	       XmNleftWidget, WIDGET(This->Lspacep),
	       XmNrightAttachment, XmATTACH_FORM,
	       XmNwidth, 140,
	       XmNrightOffset, 30,
	       XmNresizable, False,
	       NULL);

 /*--- grid add col ---*/
 F(This->Grid).addColumn(This->Grid, 33,
			 WIDGET(This->Ltitre), XcG_MAX_SIZE,
			 NULL);
 F(This->Grid).addColumn(This->Grid, 10,
			 WIDGET(This->FrameInfo), XcG_MAX_SIZE,
			 NULL);
 F(This->Grid).addColumn(This->Grid, 7, NULL);
 F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE,
			 WIDGET(This->FrameProgress), XcG_MAX_SIZE,
			 NULL);
 F(This->Grid).addColumn(This->Grid, 7, NULL);
 F(This->Grid).addColumn(This->Grid, FRAME_SIZE,
			 WIDGET(This->FrameSpace), XcG_MAX_SIZE,
			 NULL);




 F(This->Dialog).createActionArea(This->Dialog, 
				  XcCD_QUIT/* | XcCD_BACK */ ,
				  XcCD_QUIT,
				  NULL, NULL);
  
 F(This->Dialog).map(This->Dialog, TRUE);
 
 i = 0;
 while (i < XcCD_NB_BUTTON_DEF)
 {
  i++;
  if (This->Dialog->w_button[i] && i < XcCD_NB_BUTTON_DEF)
   XtSetSensitive(This->Dialog->w_button[i], False);
 }

  
 pixmap = F(GlobDocIcon).get_pixmap(GlobDocIcon, OrchidPattern);
 if (pixmap != (Pixmap)0)
 {
  XtSetArg(argsw[0], XmNbackgroundPixmap, pixmap);
  XtSetValues(WIDGET(This->GridProgress), argsw, 1);
  /*XtSetValues(WIDGET(This->Lcurfile), argsw, 1);*/
 }
 
 This->app = XtWidgetToApplicationContext(WIDGET(This->Dialog));
 
 SET_WAIT_MODE_STATIC;
 
 tmp_list = pack_list;
 This->total_size = 0;
 while(tmp_list)
 {
  if (tmp_list->selected)
  {
   This->total_size += tmp_list->size;
  }
  tmp_list = tmp_list->Next;
 }
 
 all_total = 0;
 init_total(This->PBtotal, This->total_size);
 while(pack_list)
 {
  if (pack_list->selected)
  {
   if (This->Icon)
   {
    XtUnmanageChild(WIDGET(This->Icon));
    DELETE(c_Icon)(This->Icon);
    This->Icon = NULL;
   }

   if (pack_list->pix_buffer)
   {
    This->Icon = (c_Icon *)NEW(c_Icon)(WIDGET(This->FormIcon),
				       NULL,
				       "IconPack", display,
				       pack_list->pix_buffer,
				       NULL, XcLabel);

    F(This->FormIcon).attach_top(This->FormIcon, WIDGET( This->Icon), NULL);
    F(This->FormIcon).attach_left(This->FormIcon, WIDGET( This->Icon), NULL);
    F(This->FormIcon).attach_right(This->FormIcon, WIDGET( This->Icon), NULL);
    F(This->FormIcon).attach_bottom(This->FormIcon, WIDGET( This->Icon), NULL);
    XtManageChild(WIDGET(This->Icon));
   }

   install_package(This, pack_list->filename, pack_list->size,
		   pack_list->pix_buffer, &all_total);
  }
  pack_list = pack_list->Next;
 }
 end_total(This->Ltotalp, This->PBtotal, This->total_size);
 
 UNSET_WAIT_MODE;
 
 xms = XmStringCreate(F(GlobResources).getString(GlobResources,
						 XcR_installComplete),
		      XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[0], XmNlabelString, xms);
 XtSetValues(WIDGET(This->Ltitre), argsw, 1);
 XmStringFree(xms);
 

 /*
   XtUnmanageChild(WIDGET(This->FrameFileGlob));
   XtUnmanageChild(WIDGET(This->FramePackageGlob));
   */ 

 /*
   xms = XmStringCreate(F(GlobResources).getString(GlobResources,
   XcR_installQuit),
   XmSTRING_DEFAULT_CHARSET);
   XtSetArg(argsw[0], XmNlabelString, xms);
   
   i = 0;
   while ((This->Dialog->w_button[i]==0) && (i <= XcCD_NB_BUTTON_DEF))
   i++;
   if (This->Dialog->w_button[i] && (i <= XcCD_NB_BUTTON_DEF))
   XtSetValues(This->Dialog->w_button[i], argsw, 1);
   XmStringFree(xms);
   */

 i = 0;
 while (i < XcCD_NB_BUTTON_DEF)
 {
  i++;
  if (This->Dialog->w_button[i] && i < XcCD_NB_BUTTON_DEF)
   XtSetSensitive(This->Dialog->w_button[i], True);
 }

 {
  char *string;
  
  string = F(GlobResources).getString(GlobResources, XcR_installEndMessage);
  Xc_ErrorAlert(string);
 }
 
 Xc_TRACE(("Object Box_install build"));
 return This;
}

/* ----------------------------------------------------------------- ** 
** destructor                                                        ** 
** ----------------------------------------------------------------- */
void dest_Box_install(This)
c_Box_install *This;
{
 DELETE(c_Label)(This->Ltitre);
 DELETE(c_Label)(This->Lcurfile);
 DELETE(c_Label)(This->Lfile);
 DELETE(c_XLabel)(This->LfileName);
 DELETE(c_XLabel)(This->Lfilep);
 DELETE(c_XLabel)(This->LpackageName);
 DELETE(c_XLabel)(This->Lpackagep);
 DELETE(c_Label)(This->Ltotal);
 DELETE(c_XLabel)(This->Ltotalp);
 DELETE(c_Label)(This->Lpackage);
 DELETE(c_Label)(This->Lspace);
 DELETE(c_XLabel)(This->Lspacep);
 DELETE(c_XLabel)(This->Lspacea);

 DELETE(c_Frame)(This->FrameInfo);
 DELETE(c_ProgressBar)(This->PBfile);
 DELETE(c_ProgressBar)(This->PBpackage);
 DELETE(c_ProgressBar)(This->PBtotal);
 DELETE(c_ProgressBar)(This->PBspace);

 DELETE(c_Frame)(This->FrameFile);
 DELETE(c_Grid)(This->GridFile);
 DELETE(c_Frame)(This->FrameFileGlob);

 DELETE(c_Frame)(This->FramePackage);
 DELETE(c_Grid)(This->GridPackage);
 DELETE(c_Frame)(This->FramePackageGlob);

 if (This->Icon) DELETE(c_Icon)(This->Icon);
 DELETE(c_Form)(This->FormIcon);
 DELETE(c_Frame)(This->FrameIcon);

 DELETE(c_Frame)(This->FrameTotal);
 DELETE(c_Grid)(This->GridTotal);
 DELETE(c_Frame)(This->FrameTotalGlob);

 DELETE(c_Grid)(This->GridProgress);
 DELETE(c_Frame)(This->FrameProgress);

 DELETE(c_Form)(This->FormSpace);
 DELETE(c_Frame)(This->FrameSpace);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);
  
 Xc_free(This);
 Xc_TRACE(("Object Box_install destroyed"));
}

/* ----------------------------------------------------------------- ** 
** copier                                                            ** 
** ----------------------------------------------------------------- */
void *copy_Box_install(This)
c_Box_install *This;
{
 Xc_TRACE(("Copy not impemented"));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** install package                                                   ** 
** ----------------------------------------------------------------- */
void install_package(This, tar, p_tsize, pix_buffer, all_total)
c_Box_install	*This;
char		*tar;
unsigned long	p_tsize;
char		**pix_buffer;
unsigned long	*all_total;
{
 int		std_prg_tar[2];
 int		ret;
 int		pid;
 int		status;
 char		cur_dir[1000];
 char		exec_file[1000];

 
 F(This->LpackageName).set_string(This->LpackageName, GetBasename(tar, FALSE));
 
 /* set icon */

 


 /* end set icon */

 /*printf("Pack: %s\n", tar);*/
 ret = pipe(std_prg_tar);
 if (ret == -1) 
 {
  perror("Could not create pipe");
  exit(1);
 }

 pid = vfork();
 if (pid == 0)
 {
  dup2(std_prg_tar[1], 1);
  /*close(2);*/
  gunzip(tar);
  exit(0);
 }
 getcwd(cur_dir, 1000);
 chdir(This->DstDir);
 close(std_prg_tar[1]);
 /*printf("%d\n", std_prg_tar[0]);*/
 init_total(This->PBpackage, p_tsize);
 ret = extract_tar(This, init_file, change, 
		   std_prg_tar[0], p_tsize, all_total);
 end_total(This->Lpackagep, This->PBpackage, p_tsize);
 chdir(cur_dir);
 if (ret == -1)
  kill(pid, SIGKILL);
 else
  wait(&status);

 /* teste l'existence de .post_exec et execute*/
 sprintf(exec_file, "%s/%s", This->DstDir, ".post_exec");
 if (IsFileExecutable(exec_file))
 {
  sprintf(cur_dir, "%s %s %s", exec_file, This->DstDir, 
	  strlanguage[selected_tongue -1]);
  /*printf("exec: %s\n", cur_dir);*/
  system(cur_dir); 
 } 
}

/* ----------------------------------------------------------------- ** 
** init total                                                        ** 
** ----------------------------------------------------------------- */
static void init_total(progressbar, size)
c_ProgressBar	*progressbar;
int		size;
{
 F(progressbar).change_total(progressbar, size);
 F(progressbar).change_part(progressbar, 0);
 F(progressbar).redraw(0, progressbar);
}

/* ----------------------------------------------------------------- ** 
** end total                                                         ** 
** ----------------------------------------------------------------- */
static void end_total(label, progressbar, size)
c_XLabel	*label;
c_ProgressBar	*progressbar;
int		size;
{
 char		string[150];

 sprintf(string, "100%%");
 F(label).set_string(label, string);
 F(progressbar).change_part(progressbar, size);
 F(progressbar).redraw(0, progressbar);
}

/* ----------------------------------------------------------------- ** 
** init file                                                         ** 
** ----------------------------------------------------------------- */
static void init_file(This, buffer, size)
c_Box_install	*This;
char		*buffer;
int		size;
{
 char		string[15];
 
 F(This->LfileName).set_string(This->LfileName, buffer);

 sprintf(string, "0%%");
 F(This->Lfilep).set_string(This->Lfilep, string);

 F(This->PBfile).change_total(This->PBfile, size);
 F(This->PBfile).change_part(This->PBfile, 0);
 F(This->PBfile).redraw(0, This->PBfile);
 treat_event(This);
}

/* ----------------------------------------------------------------- ** 
** change                                                            ** 
** ----------------------------------------------------------------- */
static void change(This, fsize, ftsize, psize, ptsize, tsize)
c_Box_install	*This;
int		fsize;
int		ftsize;
int		psize;
int		ptsize;
int		tsize;
{
 char		string[150];

 sprintf(string, "%d%%", (int)((fsize*100)/ftsize));
 F(This->Lfilep).set_string(This->Lfilep, string);

 sprintf(string, "%d%%", (int)((psize*100)/ptsize));
 F(This->Lpackagep).set_string(This->Lpackagep, string);
 
 sprintf(string, "%d%%", 
	 (int)((tsize*100)/(This->total_size)));
 F(This->Ltotalp).set_string(This->Ltotalp, string);
 
 F(This->PBfile).change_part(This->PBfile, fsize);
 F(This->PBfile).redraw(0, This->PBfile);
 F(This->PBpackage).change_part(This->PBpackage, psize);
 F(This->PBpackage).redraw(0, This->PBpackage);
 F(This->PBtotal).change_part(This->PBtotal, tsize);
 F(This->PBtotal).redraw(0, This->PBtotal);
 update_free_space(This);
 F(This->PBspace).redraw(0, This->PBspace);
 treat_event(This);
}

/* ----------------------------------------------------------------- ** 
** event                                                             ** 
** ----------------------------------------------------------------- */
static void treat_event(This)
c_Box_install *This;
{
 XSync(XtDisplay(WIDGET(This)), 0);
 while((This->Dialog->mapped_flag) && 
       XEventsQueued(XtDisplay(WIDGET(This)),QueuedAlready))
 {
  XEvent event;
      
  XtAppNextEvent(This->app, &event);
	  
  XtDispatchEvent(&event);
  XFlush(XtDisplay(WIDGET(This)));
 }
}


/* ----------------------------------------------------------------- ** 
** update free space                                                 ** 
** ----------------------------------------------------------------- */
static void update_free_space(This)
c_Box_install *This;
{
 struct statfs		infofs;
 char			string[25];
 int			space;

 if (!statfs(This->DstDir, &infofs))
 {
  /*--- percent ---*/
  space = (infofs.f_bavail * 100) / infofs.f_blocks;
  if (space != This->last_spacep)
  {
   sprintf(string, "(%d%%)", space);
   F(This->Lspacep).set_string(This->Lspacep, string);
  }
  
  /*--- progress bar ---*/
  F(This->PBspace).change_part(This->PBspace, 
			       infofs.f_blocks - infofs.f_bavail);
  
  /*--- absolute ---*/
  sprintf(string, F(GlobResources).getString(GlobResources,
					     XcR_freeSpace2), 
	  infofs.f_bavail * infofs.f_bsize / 1024);
  F(This->Lspacea).set_string(This->Lspacea, string);
 }
}








