/*
** check.c for XQuad in Main/
** functions that check the existence of various necessary environment
** variables and files
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Mon Jan 29 18:55:24 1996 Emmanuel Paris
** Last update Wed Dec 17 17:24:49 1997 Emmanuel Paris
*/

#include "check.h"
#include "file.h"
#include "readkey.h"

static boolean check_config_dir();
static boolean ask_config_dir();
static boolean check_XKeysymDB();
static boolean ask_XKeysymDB();

#ifdef Xc_NEED_LOCALE
static boolean check_locale_dir();
static boolean test_locale_dir_contents ___PROTO((char *directory));
static boolean ask_locale_dir();
#endif

#ifdef Xc_NEED_NLS
static boolean check_nls_dir();
static boolean test_nls_dir_contents ___PROTO((char *directory));
static boolean ask_nls_dir();
#endif

static void preprocess_for_home_dir ___PROTO((char *name, int  size));
static char ask_simple_question ___PROTO((char *question, char *respond));

void check_configuration()
{
 Xc_HISTORY(("Checking system configuration."));
  
 if (check_config_dir())
 {
  do
  {
   if (ask_config_dir())
   {
    Xc_FATAL(("Set a valid $%s or $%s environment variable before restarting this programme.", Xc_ENVIRONNEMENT_GLOBAL, Xc_ENVIRONNEMENT));
   }
  } while (check_config_dir());
 }
 Xc_TRACE(("Check $%s or $%s environment variable: OK",
	   Xc_ENVIRONNEMENT_GLOBAL,
	   Xc_ENVIRONNEMENT));
  
 if (check_XKeysymDB())
 {
  if (ask_XKeysymDB())
  {
   Xc_FATAL(("Put a valid file `%s' in directory `%s' or set a valid $%s environment variable before restarting this programme.", XcF_XKEYSYMDB, Xc_XTOP, XcE_XKEYSYMDB));
  }
 }
 Xc_TRACE(("Check for file `%s': OK", XcF_XKEYSYMDB));

#ifdef Xc_NEED_LOCALE
 if (check_locale_dir())
 {
  do
  {
   if (ask_locale_dir())
   {
    Xc_FATAL(("Set a valid $%s environment variable before restarting this programme.", XcE_LOCALEDIR));
   }
  } while (check_locale_dir());
 }
  
 Xc_TRACE(("Check for `%s' directory: OK", XcF_LOCALEDIR));
#endif
  
#ifdef Xc_NEED_NLS
 if (check_nls_dir())
 {
  do
  {
   if (ask_nls_dir())
   {
    Xc_FATAL(("Set a valid $%s environment variable before restarting this programme.", XcE_NLSPATH));
   }
  } while (check_nls_dir());
 }
  
 Xc_TRACE(("Check for `%s' directory: OK", XcF_NLSPATH));
#endif
  
 Xc_TRACE(("Check successfully passed, go on..."));
}


/* ----------------------------------------------------------------- ** 
** check and ask for $Xc_ENVIRONNEMENT                               ** 
** ----------------------------------------------------------------- */
static boolean check_config_dir()
{
 char	*path;
  
 if ((path = getenv(Xc_ENVIRONNEMENT)) == NULL)
  path = getenv(Xc_ENVIRONNEMENT_GLOBAL);
 if ( path == NULL)
 {
  Xc_WARNING(("No $%s nor $%s environment variable defined !",
	      Xc_ENVIRONNEMENT_GLOBAL, Xc_ENVIRONNEMENT));
  return TRUE;
 }
 if (IsDirExist(path) == FALSE)
 {
  if (getenv(Xc_ENVIRONNEMENT) == NULL)
   Xc_WARNING(("Environment variable $%s defined, but can not reach the directory `%s' pointed by it !", Xc_ENVIRONNEMENT_GLOBAL, path));
  else
   Xc_WARNING(("Environment variable $%s defined, but can not reach the directory `%s' pointed by it !", Xc_ENVIRONNEMENT, path));
  return TRUE;
 }
 return FALSE;
}

static boolean ask_config_dir()
{
 char	name[200];
 printf("Enter the $%s environment variable or nothing to exit: ",
	Xc_ENVIRONNEMENT);
 fflush(stdout);
 fgets(name, 200, stdin);
 name[strlen(name) - 1] = '\0';
 if (name[0] != '\0')
 {
  preprocess_for_home_dir(name, 200);
  if (setenv(Xc_ENVIRONNEMENT, name, 1) != 0)
   return TRUE;
  return FALSE;
 }
 else
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** check and ask for XKeysymDB                                       ** 
** ----------------------------------------------------------------- */
static boolean check_XKeysymDB()
{
 char name[300];
 char *file;
  
 sprintf(name, "%s/%s", Xc_XTOP, XcF_XKEYSYMDB);
 if (IsFileExist(name)) return FALSE;
  
 file = getenv(XcE_XKEYSYMDB);
 if (file != NULL && IsFileExist(file)) return FALSE;
  
 if (file)
  Xc_WARNING(("File `%s' doesn't exist and $%s environment variable pointed to an unreachable file `%s' !", name, XcE_XKEYSYMDB, file)); 
 else
  Xc_WARNING(("File `%s' doesn't exist and $%s environment variable is not defined !", name, XcE_XKEYSYMDB)); 
    
 if ((file = getenv(Xc_ENVIRONNEMENT)) == NULL)
  file = getenv(Xc_ENVIRONNEMENT_GLOBAL);
 if (file == NULL) return TRUE;
  
 sprintf(name, "%s/%s/%s", file, Xc_MXTOP, XcF_XKEYSYMDB);
 if (IsFileExist(name))
 {
  if (setenv(XcE_XKEYSYMDB, name, 1) != 0) return TRUE;
  Xc_WARNING(("Taking `%s' file instead", name));
  return FALSE;
 }
 return TRUE;
}

static boolean ask_XKeysymDB()
{
 boolean to_exit = FALSE;
 char name[200];
  
 do
 {
  printf("Enter a `%s' file location or nothing to skip: ", XcF_XKEYSYMDB);
  fflush(stdout);
  fgets(name, 200, stdin);
  name[strlen(name) - 1] = '\0';
  if (name[0] != '\0')
  {
   preprocess_for_home_dir(name, 200);
   if (IsFileExist(name))
   {
    if (setenv(XcE_XKEYSYMDB, name, 1) != 0) return TRUE;
    return FALSE;
   }
  }
  else
  {
   Xc_WARNING(("Without any `%s' file declared, many warnings about \"unknown keysyms\" will be generated, but this programme will work although.", XcF_XKEYSYMDB));
   if (ask_simple_question("Do you want to go on (y/n)? ", "yn") == 'y')
    return FALSE;
   to_exit = TRUE;
  }
 } while(!to_exit);
 return TRUE;
}

#ifdef Xc_NEED_LOCALE
/* ----------------------------------------------------------------- ** 
** check and ask for locale directory                                ** 
** ----------------------------------------------------------------- */
static boolean check_locale_dir()
{
 char name[300], directory[300];
 char *file;
 boolean localedir_exist = FALSE;
  
 sprintf(name, "%s/%s", Xc_XTOP, XcF_LOCALEDIR);
 if (IsDirExist(name) && test_locale_dir_contents(name)) 
 {
  strcpy(directory, name);
  localedir_exist = TRUE;
 }
 else
 {
  file = getenv(XcE_LOCALEDIR);
  if (file != NULL && IsDirExist(file) && test_locale_dir_contents(file))
  {
   strcpy(directory, file);
   localedir_exist = TRUE;
  }
  else
  {
   if (file)
    Xc_WARNING(("File `%s' doesn't exist and $%s environment variable pointed to an unreachable directory `%s' !", name, XcE_LOCALEDIR, file)); 
   else
    Xc_WARNING(("File `%s' doesn't exist and $%s environment variable is not defined !", name, XcE_LOCALEDIR)); 
      
   if ((file = getenv(Xc_ENVIRONNEMENT)) == NULL)
    file = getenv(Xc_ENVIRONNEMENT_GLOBAL);
   if (file == NULL) return TRUE;
      
   sprintf(name, "%s/%s/%s", file, Xc_MXTOP, XcF_LOCALEDIR);
   if (IsDirExist(name) && test_locale_dir_contents(name))
   {	
    if (setenv(XcE_LOCALEDIR, name, 1) != 0) return TRUE;
    Xc_WARNING(("Taking `%s' directory instead", name));
    strcpy(directory, name);
    localedir_exist = TRUE;
   }
  }
 }
  
 return !localedir_exist;
}

static boolean test_locale_dir_contents(directory)
char *directory;
{
 boolean loc_dir_exist, loc_alias_exist;
 char name[300];
  
 sprintf(name, "%s/%s", directory, XcF_LOCALE_DIR);
 loc_dir_exist = IsFileExist(name);
 if (!loc_dir_exist)
  Xc_WARNING(("There is no `%s' file in directory `%s' !", XcF_LOCALE_DIR, directory));
  
 sprintf(name, "%s/%s", directory, XcF_LOCALE_ALIAS);
 loc_alias_exist = IsFileExist(name);
 if (!loc_alias_exist)
  Xc_WARNING(("There is no `%s' file in directory `%s' !", XcF_LOCALE_ALIAS, directory));
  
 return loc_dir_exist && loc_alias_exist;
}

static boolean ask_locale_dir()
{
 char	name[200];
 printf("Enter the $%s environment variable or nothing to exit: ",
	XcE_LOCALEDIR);
 fflush(stdout);
 fgets(name, 200, stdin);
 name[strlen(name) - 1] = '\0';
 if (name[0] != '\0')
 {
  preprocess_for_home_dir(name, 200);
  if (setenv(XcE_LOCALEDIR, name, 1) != 0)
   return TRUE;
  return FALSE;
 }
 else
  return TRUE;
}
#endif

#ifdef Xc_NEED_NLS
/* ----------------------------------------------------------------- ** 
** check and ask for nls directory                                   ** 
** ----------------------------------------------------------------- */
static boolean check_nls_dir()
{
 char name[300], directory[300];
 char *file;
 boolean nlsdir_exist = FALSE;
  
 sprintf(name, "%s/%s", Xc_XTOP, XcF_NLSPATH);
 if (IsDirExist(name) && test_nls_dir_contents(name)) 
 {
  strcpy(directory, name);
  nlsdir_exist = TRUE;
 }
 else
 {
  file = getenv(XcE_NLSPATH);
  if (file != NULL && IsDirExist(file) && test_nls_dir_contents(name))
  {
   strcpy(directory, file);
   nlsdir_exist = TRUE;
  }
  else
  {
   if (file)
    Xc_WARNING(("File `%s' doesn't exist and $%s environment variable pointed to an unreachable directory `%s' !", name, XcE_NLSPATH, file)); 
   else
    Xc_WARNING(("File `%s' doesn't exist and $%s environment variable is not defined !", name, XcE_NLSPATH)); 
      
   if ((file = getenv(Xc_ENVIRONNEMENT)) == NULL)
    file = getenv(Xc_ENVIRONNEMENT_GLOBAL);
   if (file == NULL) return TRUE;
      
   sprintf(name, "%s/%s/%s", file, Xc_MXTOP, XcF_NLSPATH);
   if (IsDirExist(name) && test_nls_dir_contents(name))
   {	
    if (setenv(XcE_NLSPATH, name, 1) != 0) return TRUE;
    Xc_WARNING(("Taking `%s' directory instead", name));
    strcpy(directory, name);
    nlsdir_exist = TRUE;
   }
  }
 }
 return !nlsdir_exist;
}

static boolean test_nls_dir_contents(directory)
char *directory;
{
 boolean nls_dir_exist;
 char name[300];
  
 sprintf(name, "%s/%s", directory, XcF_NLS_DIR);
 nls_dir_exist = IsFileExist(name);
 if (!nls_dir_exist)
  Xc_WARNING(("There is no `%s' file in directory `%s' !", XcF_NLS_DIR, directory));
  
 return nls_dir_exist;
}

static boolean ask_nls_dir()
{
 char	name[200];
 printf("Enter the $%s environment variable or nothing to exit: ",
	XcE_NLSPATH);
 fflush(stdout);
 fgets(name, 200, stdin);
 name[strlen(name) - 1] = '\0';
 if (name[0] != '\0')
 {
  preprocess_for_home_dir(name, 200);
  if (setenv(XcE_NLSPATH, name, 1) != 0)
   return TRUE;
  return FALSE;
 }
 else
  return TRUE;
}
#endif

/* ----------------------------------------------------------------- ** 
** preprocess - check if name begin w/ '~' and translate w/ $HOME    ** 
** ----------------------------------------------------------------- */
static void preprocess_for_home_dir(name, size)
char *name;     
int  size;
{
 char	*home_dir;
 char	*dup_name;
  
 if (name[0] != '~') return;
  
 home_dir = getenv("HOME");
 if (home_dir == NULL)
  home_dir = "/";
  
 if ((int)strlen(name) + (int)strlen(home_dir) < size)
 {
  dup_name = (char *)Xc_strdup("dup_name", name + 1);
  sprintf(name, "%s%s", home_dir, dup_name);
  Xc_free(dup_name);
 }
}

/* ------------------------------------------------------------------- ** 
** ask_simple_question - only accept one character included in respond **
** ------------------------------------------------------------------- */
static char ask_simple_question(question, respond)
char *question;
char *respond;
{
 char c;
 int i, j;
  
 printf(question);
 fflush(stdout);
 j = strlen(respond);
 do 
 {
  c = xc_read_key_without_echo();
  if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
  for(i = 0; i < j; i++)
   if (c == respond[i]) break;
 } while( i == j);
 putchar(c);
 putchar('\n');
 return c;
}

