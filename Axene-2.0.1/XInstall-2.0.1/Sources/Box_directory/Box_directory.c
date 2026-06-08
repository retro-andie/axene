/*
** Box_directory.c for XInstall in Box_directory/
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
** Last update Sat Jan 15 18:53:42 2000 Emmanuel Paris
*/

#include "Box_directory.h"
#include "SpecialDirSelector.h"
#include "../Pixmaps/browse_in.zpm"
#include "../Pixmaps/browse_out.zpm"
#include "AlertBox.h"
#include "DocIcon.h"

#define FRAME_SIZE 60

extern Widget wMainWindow;
extern c_DocIcon	*GlobDocIcon;
extern boolean GlobKeyOpt;

void *cons_Box_directory();
void dest_Box_directory();
void *copy_Box_directory();
static int BDwaitForUnMap();

static void launch_browser();
static void free_space_dir();
static boolean test_dir();
static boolean create_dir();
static char *retract_dir();

sf_Box_directory fc_Box_directory =
{
 cons_Box_directory,
 dest_Box_directory,
 copy_Box_directory,
 BDwaitForUnMap
};

void *cons_Box_directory(w_Parent, pszTitle, sdir, ddir)
Widget		w_Parent;
char		*pszTitle;
char		*sdir;
char		*ddir;
{
 c_Box_directory	*This;
 Display		*display;
 Arg			argsw[1];
 XmString		xms;
 char			string[200];
 Dimension		h1, h2;
 Pixmap			pixmap;

 if((This = (c_Box_directory *)Xc_malloc("Box_directory",
					 sizeof(c_Box_directory))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
  return NULL;
 }

 memset(This, 0, sizeof(c_Box_directory));
 This->f = &fc_Box_directory;
  
 This->SrcDir = sdir;
 This->DstDir = ddir;

 XtVaGetValues(w_Parent, XmNwidth, &h1, XmNheight, &h2, NULL);
    
 This->Dialog = (c_CustomDialog *)NEW(c_CustomDialog)(w_Parent,
						      pszTitle, h1, h2,
						      XcCD_HORIZONTAL);
 This->w_This = WIDGET(This->Dialog);
 display = XtDisplay(WIDGET(This->Dialog));

 This->Grid = (c_Grid *)NEW(c_Grid)(WIDGET(This),
				    MANAGER(This->Dialog),
				    "gBoxDirectory", TRUE);

 /* grid son */
 This->Ltitre = (c_Label *)NEW(c_Label)(WIDGET(This->Grid),
					MANAGER(This->Dialog),
					"lBoxDirectoryTitleSrc");
 This->FrameSrc = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					  MANAGER(This->Dialog), 
					  NULL, 0);
 
 /* grid framesrc son */
 This->GridSrc = (c_Grid *)NEW(c_Grid)(WIDGET(This->FrameSrc),
				       MANAGER(This->Dialog),
				       "fBDSrc", TRUE);

 /* Grid GridSrc son */
 This->LSrc = (c_Label *)NEW(c_Label)(WIDGET(This->GridSrc),
				      MANAGER(This->Dialog),
				      "lBDSrc");
 This->LSrcD =
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->GridSrc),
				    MANAGER(This->Dialog),
				    "lBDSrcD",
				    TF_STRING,
				    1.0, (float)(200),
				    This->SrcDir, NULL, 10);
 This->IconSrc = (c_Icon *)NEW(c_Icon)(WIDGET(This->GridSrc),
				       MANAGER(This->Dialog),
				       "BDIconSrc", display,
				       browse_out_xpm,
				       NULL, XcPush);
 CALLBACK(WIDGET(This->IconSrc), FUNC_ACTIVATE, launch_browser, This);

 This->FrameSrcInfo = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					      MANAGER(This->Dialog), 
					      NULL, 0);
 This->Ltitre2 = (c_Label *)NEW(c_Label)(WIDGET(This->Grid),
					 MANAGER(This->Dialog),
					 "lBoxDirectoryTitleDst");
 This->FrameDst = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					  MANAGER(This->Dialog), 
					  NULL, 0);
 /* framedst son */
 This->GridDst = (c_Grid *)NEW(c_Grid)(WIDGET(This->FrameDst),
				       MANAGER(This->Dialog),
				       "fBDDst", TRUE);


 /* Grid Griddst son */
 This->LDst = (c_Label *)NEW(c_Label)(WIDGET(This->GridDst),
				      MANAGER(This->Dialog),
				      "lBDDst");
 This->LDstD =
  (c_WTextField *)NEW(c_WTextField)(WIDGET(This->GridDst),
				    MANAGER(This->Dialog),
				    "lBDDstD",
				    TF_STRING,
				    1.0, (float)(200),
				    This->DstDir, NULL, 10);
 This->IconDst = (c_Icon *)NEW(c_Icon)(WIDGET(This->GridDst),
				       MANAGER(This->Dialog),
				       "BDIconDst", display,
				       browse_in_xpm,
				       NULL, XcPush);
 CALLBACK(WIDGET(This->IconDst), FUNC_ACTIVATE, launch_browser, This);

 This->FrameSize = (c_Frame *)NEW(c_Frame)(WIDGET(This->Grid),
					   MANAGER(This->Dialog), 
					   NULL, 0);

 This->LDstSpace = (c_Label *)NEW(c_Label)(WIDGET(This->FrameSize),
					   MANAGER(This->Dialog),
					   "lBDDstSpace");

 free_space_dir(This->DstDir, string);
 xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
 XtSetArg(argsw[0], XmNlabelString, xms);
 XtSetValues(WIDGET(This->LDstSpace), argsw, 1);
 XmStringFree(xms);

 /* grid add col */

 F(This->GridSrc).addColumn(This->GridSrc, XcG_MAX_SIZE,
			    WIDGET(This->LSrc), 100,
			    WIDGET(This->LSrcD), XcG_MAX_SIZE,
			    XcG_WIDGET_NULL, 10,
			    WIDGET(This->IconSrc), XcG_BEST_SIZE,
			    NULL);
 F(This->GridDst).addColumn(This->GridDst, XcG_MAX_SIZE,
			    WIDGET(This->LDst), 100,
			    WIDGET(This->LDstD), XcG_MAX_SIZE,
			    XcG_WIDGET_NULL, 10,
			    WIDGET(This->IconDst), XcG_BEST_SIZE,
			    NULL);
  
 F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE, NULL);
 F(This->Grid).addColumn(This->Grid, XcG_BEST_SIZE,
			 WIDGET(This->Ltitre), XcG_MAX_SIZE,
			 NULL);
 /*F(This->Grid).addColumn(This->Grid, 10, NULL);*/
 F(This->Grid).addColumn(This->Grid, FRAME_SIZE,
			 WIDGET(This->FrameSrc), XcG_MAX_SIZE ,
			 NULL);
 F(This->Grid).addColumn(This->Grid, 5, NULL);
 F(This->Grid).addColumn(This->Grid, 30,
			 WIDGET(This->FrameSrcInfo), XcG_MAX_SIZE,
			 NULL);
 F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE, NULL);
 F(This->Grid).addColumn(This->Grid, XcG_BEST_SIZE,
			 WIDGET(This->Ltitre2), XcG_MAX_SIZE,
			 NULL);
 /*F(This->Grid).addColumn(This->Grid, 10, NULL);*/
 F(This->Grid).addColumn(This->Grid, FRAME_SIZE,
			 WIDGET(This->FrameDst), XcG_MAX_SIZE,
			 NULL);
 F(This->Grid).addColumn(This->Grid, 5, NULL);
 F(This->Grid).addColumn(This->Grid, 30,
			 WIDGET(This->FrameSize), XcG_MAX_SIZE,
			 NULL);
 F(This->Grid).addColumn(This->Grid, XcG_MAX_SIZE, NULL);
  
 F(This->Dialog).createActionArea(This->Dialog, 
				  XcCD_CANCEL | XcCD_BACK | XcCD_NEXT,
				  XcCD_NEXT,
				  NULL, NULL);

 if (GlobKeyOpt)
 {
  XtSetSensitive(WIDGET(This->Ltitre), FALSE);
  XtSetSensitive(WIDGET(This->FrameSrc), FALSE);
 }
  
 pixmap = F(GlobDocIcon).get_pixmap(GlobDocIcon, OrchidPattern);
 if (pixmap != (Pixmap)0)
 {
  XtSetArg(argsw[0], XmNbackgroundPixmap, pixmap);
  XtSetValues(WIDGET(This->LDstSpace), argsw, 1);
  XtSetValues(WIDGET(This->FrameSrcInfo), argsw, 1);
 }
 
 Xc_TRACE(("Object Box_directory build"));
 return This;
}

void dest_Box_directory(This)
c_Box_directory *This;
{
 if (BrowseDirectorySrc) 
 {
  DELETE(c_DirSelector)(BrowseDirectorySrc);
  BrowseDirectorySrc = NULL;
 }
 if (BrowseDirectoryDst) 
 {
  DELETE(c_DirSelector)(BrowseDirectoryDst);
  BrowseDirectoryDst = NULL;
 }

 DELETE(c_Label)(This->LDstSpace);
 DELETE(c_Frame)(This->FrameSize);
 DELETE(c_Icon)(This->IconDst);
 DELETE(c_WTextField)(This->LDstD);
 DELETE(c_Label)(This->LDst);
 DELETE(c_Grid)(This->GridDst);
 DELETE(c_Frame)(This->FrameDst);
 DELETE(c_Label)(This->Ltitre2);
 DELETE(c_Frame)(This->FrameSrcInfo);
 DELETE(c_Icon)(This->IconSrc);
 DELETE(c_WTextField)(This->LSrcD);
 DELETE(c_Label)(This->LSrc);
 DELETE(c_Grid)(This->GridSrc);
 DELETE(c_Frame)(This->FrameSrc);
 DELETE(c_Label)(This->Ltitre);
 DELETE(c_Grid)(This->Grid);
 DELETE(c_CustomDialog)(This->Dialog);

 Xc_free(This);
 Xc_TRACE(("Object Box_directory destroyed"));
}

void *copy_Box_directory(This)
c_Box_directory *This;
{
 Xc_TRACE(("Copy not impemented"));
 return NULL;
}

/* ----------------------------------------------------------------- ** 
** specific wait fo unmap                                            ** 
** ----------------------------------------------------------------- */
static int BDwaitForUnMap(This)
c_Box_directory *This;
{
 int	rc;
 char   string[200];
 
 rc = 0;
 while(!rc)
 {
  F(This->Dialog).map(This->Dialog, TRUE);
    
  rc = F(This->Dialog).waitForUnmap(This->Dialog);
  
  if (rc == XcCD_NEXT)
  {
   if (test_dir(This->DstDir))
    break;
   
   if (!IsDirExist(This->DstDir))
   {
    sprintf(string, F(GlobResources).getString(GlobResources, XcR_createdir), 
	    This->DstDir);
    if (Xc_YesNoAlert(string))
    {
     Xc_TRACE(("create"));
     if (!create_dir(This->DstDir))
     {
      Xc_SYSERROR((F(GlobResources).getString
		   (GlobResources, XcR_cannotCreateDir), This->DstDir));
      rc = 0;
     }
    }
    else rc = 0;
   }
   else
   {
    sprintf(string, F(GlobResources).getString
	    (GlobResources, XcR_dirNotWritable), This->DstDir);
    Xc_ErrorAlert(string);
    rc = 0;
   }
  }
  else break;
 }
 return rc;
}


static void launch_browser(w, Obj)
Widget		w;
c_Box_directory	*Obj;
{
 char	*filename;
 Xc_HISTORY(("change"));
  
 if (w == WIDGET(Obj->IconSrc))
 {
  if(BrowseDirectorySrc == NULL)
  {
   if (CreateBrowseDirectorySrc(wMainWindow) == FALSE)
    return;
   else
    F(BrowseDirectorySrc).setPath(BrowseDirectorySrc,
				  Obj->SrcDir);
  }

  if((filename = F(BrowseDirectorySrc).
      getFile(BrowseDirectorySrc)) == NULL)
   return;

  strcpy(Obj->SrcDir, filename);

  F(Obj->LSrcD).set(Obj->LSrcD, filename);

  Xc_free(filename);
  return;
 }
 if (w == WIDGET(Obj->IconDst))
 {
  Arg			argsw[1];
  XmString		xms;
  char			*dstdir;
  char			string[200];
  
  dstdir = Xc_strdup("ddir", Obj->DstDir);
  if (!test_dir(dstdir))
  {
   dstdir = retract_dir(dstdir);
  }

  if(BrowseDirectoryDst == NULL)
  {
   if (CreateBrowseDirectoryDst(WIDGET(Obj)) == FALSE)
    return;
   else
    F(BrowseDirectoryDst).setPath(BrowseDirectoryDst,
				  dstdir);
  }
  
  if((filename = F(BrowseDirectoryDst).
      getFile(BrowseDirectoryDst)) == NULL)
   return;

  strcpy(Obj->DstDir, filename);

  F(Obj->LSrcD).set(Obj->LDstD, filename);

  Xc_free(filename);
  Xc_free(dstdir);
  
  free_space_dir(Obj->DstDir, string);
  xms = XmStringCreate(string, XmSTRING_DEFAULT_CHARSET);
  XtSetArg(argsw[0], XmNlabelString, xms);
  XtSetValues(WIDGET(Obj->LDstSpace), argsw, 1);
  XmStringFree(xms);

  return;
 }
}

/* ----------------------------------------------------------------- ** 
** free_space_dir: get free space message for specified directory    ** 
** ----------------------------------------------------------------- */
static void free_space_dir(ddir, string)
char *ddir;
char *string; /* result string, must be preallocated */
{
 char *dstdir, *ddir2;
 struct statfs	infofs;
 
 if (!statfs(ddir, &infofs))
  sprintf(string, F(GlobResources).getString(GlobResources, XcR_freeSpace), 
	  infofs.f_bavail * infofs.f_bsize / 1024);
 else
 {
  ddir2 = dstdir = Xc_strdup("ddir", ddir);
  do
  {
   dstdir = retract_dir(dstdir);
   if (dstdir == ddir2)
   {
    char tmp[200], *cut;
    strcpy(tmp, F(GlobResources).getString(GlobResources, XcR_freeSpace));
    cut = strchr(tmp, ':');
    if (cut)
    {
     *(cut+1) = '\0';
     sprintf(string, "%s %s.", tmp, 
	     F(GlobResources).getString(GlobResources, XcR_unknownSize));
    }
    else
     sprintf(string, tmp, 0L);
    break;
   }
   if (!statfs(dstdir, &infofs))
   {
    sprintf(string, F(GlobResources).getString(GlobResources, XcR_freeSpace), 
	    infofs.f_bavail * infofs.f_bsize / 1024);
    break;
   }
   ddir2 = dstdir;
  } while(1);
  Xc_free(dstdir);
 }
}

/* ----------------------------------------------------------------- ** 
** test_dir                                                          ** 
** ----------------------------------------------------------------- */
static boolean test_dir(ddir)
char *ddir;
{
 struct stat		bstat;
 uid_t			curuid;
 uid_t			curgid;

 if (stat(ddir, &bstat) < 0)
 {
  Xc_TRACE(("dir %s doesn't exist", ddir));
  return FALSE;
 }

 curuid = geteuid();
 curgid = getegid();
 
 if (!S_ISDIR(bstat.st_mode))
 {
  Xc_TRACE(("dest not dir: %s", ddir));
  return FALSE;
 }
 
 if (curuid != bstat.st_uid || (S_IWUSR & bstat.st_mode) == 0)
 {				/* not owner or not owner write permission */
  if (curgid != bstat.st_uid || (S_IWGRP & bstat.st_mode) == 0)
  {				/* not group or not group write permission */
   if ((S_IWOTH & bstat.st_mode) == 0)
   {				/* nor other write permission */
    Xc_TRACE(("can't write dest: %s", ddir));
    return FALSE;
   }
  }
 }
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** create_dir                                                        ** 
** ----------------------------------------------------------------- */
static boolean create_dir(ptr)
char	*ptr;
{
 int		j;
 int		dir_sub;
 int		slash_pos[30];
 char		ptrdup[200];
 struct stat	bstat;
 
 strncpy(ptrdup, ptr, 200);
 
 j = strlen(ptrdup) - 1;
 if (ptrdup[j] == '/') 
 {
  ptrdup[j--] = '\0';
 }
 if (!j) return FALSE;

 dir_sub = 0;
 while ((stat(ptrdup, &bstat)<0) && (dir_sub < 30))
 {
  while(j && ptrdup[j] != '/') j--;
  if (!j) return FALSE;
  slash_pos[dir_sub] = j;
  ptrdup[j] = '\0';
  Xc_TRACE(("%s", ptrdup));
  dir_sub ++;
 }
 while (dir_sub)
 {
  dir_sub--;
  ptrdup[slash_pos[dir_sub]] = '/';
  if (mkdir(ptrdup, 0755) == -1) return FALSE;
 }
 return TRUE;
}

/* ----------------------------------------------------------------- ** 
** retract_dir                                                       ** 
** ----------------------------------------------------------------- */
static char *retract_dir(ptr)
char *ptr;
{
 char *lastc;
 char *retptr;

 if (!ptr || strlen(ptr) < 2)
  return ptr;
 
 lastc = ptr + strlen(ptr) - 1;
 if (*lastc == '/') lastc--;
 while(lastc != ptr && *lastc != '/')
  lastc--;
 
 if (*lastc != '/') return ptr;

 retptr = (char *)Xc_malloc("rptr", (int)(lastc - ptr) + 2);
 strncpy(retptr, ptr, (int)(lastc - ptr) + 1);
 retptr[(int)(lastc - ptr) + 1] = '\0';
 
 Xc_free(ptr);
 return retptr;
}
