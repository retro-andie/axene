/*
** filename.c for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Global/
** Filename functions
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
** Started on  Mon Jan 16 11:12:15 1995 Stéphane Boisson
** Last update Tue Jan 20 17:28:16 1998 Robin Castanier
*/

#ifndef NHISTORY
#define NHISTORY
#endif
#ifndef NTRACE
#define NTRACE
#endif

#include "xcalibur.h"
#include "file.h"

/* ----------------------------------------------------------------- ** 
** UntranslateFilename - Get the original filename before the        ** 
**                       `TranslateFilename' conversion              ** 
** ----------------------------------------------------------------- */
char *UntranslateFilename(subdir, filename)
char *subdir;
char *filename;
{
  char *path;

  Xc_HISTORY(("UntranslateFilename(`%s', `%s')", subdir? subdir:"", filename));
  
  if ((path = getenv(Xc_ENVIRONNEMENT)) == NULL)
   path = getenv(Xc_ENVIRONNEMENT_GLOBAL);
  
  if(path != NULL)
  {
   long path_len;
   
   path_len = strlen(path);
   if(!strncmp(path, filename, path_len))
   {
    boolean flag;

    flag = (path[path_len] == '/')? TRUE : FALSE;
    if(flag || (!flag && (filename[path_len] == '/')))
    {
     if(!flag) path_len++;
     if(subdir != NULL)
     {
      long subdir_len;

      subdir_len = strlen(subdir);
      if(!strncmp(subdir, filename + path_len, subdir_len))
       path_len += subdir_len;
      if(filename[path_len++] != '/') return filename;
     }
     return(filename + path_len);
    }
   }
  }
  return filename;
 }


/* ------------------------------------------------------------------ ** 
** TranslateFilename - Translate a simple filename to a complete      ** 
**                     filename in `subdir' in the Xclamation lib dir ** 
** ------------------------------------------------------------------ */
char *TranslateFilename(subdir, filename)
char *subdir;
char *filename;
{
  char *path, *final;

  Xc_HISTORY(("TranslateFilename(`%s', `%s')", subdir? subdir : "", filename));
  
  while(*filename == 32) filename++;
  
  if ((path = getenv(Xc_ENVIRONNEMENT)) == NULL)
   path = getenv(Xc_ENVIRONNEMENT_GLOBAL);
  
  if((path != NULL) && (*filename != '/'))
  {
   long len, path_len;

   path_len = strlen(path);
   if(path[path_len] == '/') path_len--;
   len = (path_len + ((subdir == NULL)? -1 : strlen(subdir)) +
	  strlen(filename) + 3);
   if((final = Xc_malloc("filename", len)) == NULL) return NULL;

   memcpy(final, path, path_len);
   if(subdir != NULL)
    sprintf(final + path_len, "/%s/%s", subdir, filename);
   else
    sprintf(final + path_len, "/%s", filename);
  }
  else
   final = Xc_strdup("filename", filename);
  
  filename = final + strlen(final) - 1;
  while((filename >= final) && (*filename == 32)) filename--;
  filename[1] = 0;
  return final;
 }


/* ----------------------------------------------------------------- ** 
** MakeRelativeFile - Make filename relative from a other file       ** 
** ----------------------------------------------------------------- */
char *MakeRelativeFile(basefile, file)
char *basefile;
char *file;
{
 register char *source, *target, *sfile, c;
 unsigned long offset, count;

 Xc_HISTORY(("MakeRelativeFile(`%s', `%s')", basefile, file));

 if((file = sfile = StripFilename(file, TRUE)) == NULL) return NULL;
 if((basefile = GetDirname(basefile, TRUE)) == NULL)
 {
  Xc_free(file);
  return NULL;
 }
 source = StripFilename(basefile, FALSE);
 target = file;
 offset = 0;

 Xc_ASSERT(source == basefile);

 /*--- Get last common dir ---*/
 while((c = *source++) == *target++)
 {
  if(c == 0)
  {
   target--;
   break;
  }
  else if(c == '/')
   offset = source - basefile;
 }

 if(((c == 0) && (target[-1] == '/')) || (offset == 0))
 {
  Xc_free(basefile);
  file = Xc_strdup("filename", offset? target : sfile);
  Xc_free(sfile);
  return file;
 }

 /*--- Count subdirs to traverse up ---*/
 source = basefile + offset;
 file += offset;

 for(count = 1, c = *source++; c != 0; )
  if(c == '/')
  {
   while((c = *source++) == '/');
   count++;
  }
  else
   c = *source++;

 Xc_free(basefile);

 offset = strlen(file) + 1;
 if((target = Xc_malloc("filename", offset + (count * 3))) == NULL)
  return NULL;

 source = target;
 while(count--)
 {
  *target++ = '.';
  *target++ = '.';
  *target++ = '/';
 }
 memcpy(target, file, offset);
 Xc_free(sfile);
  
 return source;
}


/* ----------------------------------------------------------------- ** 
** MakeAbsoluteFile - Make filename absolute from a other file       ** 
** ----------------------------------------------------------------- */
char *MakeAbsoluteFile(basefile, file)
char *basefile;
char *file;
{
 char *tmp;

 Xc_HISTORY(("MakeAbsoluteFile(`%s', `%s')", basefile, file));

 if(*file == '/') return StripFilename(file, TRUE);

 if((basefile = GetDirname(basefile, TRUE)) == NULL) return NULL;
 if((tmp = GetFullname(basefile, file, TRUE)) != NULL)
  return StripFilename(tmp, FALSE);

 Xc_free(basefile);
 return NULL;
}


/* ----------------------------------------------------------------- ** 
** GetFullname - Concat pathname and filename for full filename      ** 
** ----------------------------------------------------------------- */
char *GetFullname(path, file, realloc_flag)
char *path;
char *file;
boolean realloc_flag;
{
 unsigned long pathlen, filelen;
 char *fullname;

 Xc_HISTORY(("GetFullname(`%s', `%s', %s)", path, file,
	     realloc_flag? "with realloc(path)" : "with malloc"));

 pathlen = strlen(path);
 if(pathlen == 0)
 {
  fullname = Xc_strdup("filename", file);
  if(realloc_flag && (fullname != NULL)) Xc_free(path);
  return fullname;
 }

 filelen = strlen(file) + 1;
 if(realloc_flag == FALSE)
 {
  if((fullname = Xc_malloc("filename", pathlen + filelen + 2)) == NULL)
   return NULL;
  memcpy(fullname, path, pathlen);
 }
 else
 {
  fullname = Xc_realloc("filename", path, pathlen + filelen + 2);
  if(fullname == NULL) return NULL;
 }

 fullname[pathlen++] = '/';
 memcpy(fullname + pathlen, file, filelen);

 return fullname;
}


/* ----------------------------------------------------------------- ** 
** GetBasename - Extract basename from filename                      ** 
** ----------------------------------------------------------------- */
char *GetBasename(name, dup_flag)
char *name;
boolean dup_flag;
{
 register char *base;

 Xc_HISTORY(("GetBasename(`%s', %s)", name, dup_flag? "True" : "False"));

 if((base = strrchr(name, '/')) == NULL)
  base = name;
 else
  base++;
 return dup_flag? Xc_strdup("basename", base) : base; 
}


/* ----------------------------------------------------------------- ** 
** GetDirname - Extract basename from filename                       ** 
** ----------------------------------------------------------------- */
char *GetDirname(path, dup_flag)
char *path;
boolean dup_flag;
{
 char *newpath, *slash;
 int length;

 Xc_HISTORY(("GetDirname(`%s', %s)", path, dup_flag? "True" : "False"));

 if (path)
  slash = strrchr(path, '/');
 else
  slash = 0;
 if(slash == 0)
 {
  /*--- File is in the current directory ---*/
  if(dup_flag == FALSE)
   *path = '.';
  else
   path = ".";
  length = 1;
 }
 else
 {
  /*--- Remove any trailing slashes from the result ---*/
  while(slash > path && *slash == '/') slash--;

  length = slash - path + 1;
 }

 if(dup_flag == FALSE)
 {
  path[length] = 0;
  return path;
 }

 if((newpath = Xc_malloc("dirname", length + 1)) == NULL) return NULL;
 memcpy(newpath, path, length);
 newpath[length] = 0;

 return newpath;
}


/* ----------------------------------------------------------------- ** 
** StripFilename - Strip filename from dummy stuff                   ** 
** ----------------------------------------------------------------- */
char *StripFilename(file, dup_flag)
char *file;
boolean dup_flag;
{
 register char *source, *target, *limit, c;

 Xc_HISTORY(("StripFilename(`%s', %s)", file, dup_flag? "True" : "False"));

 if((dup_flag != FALSE) && ((file = Xc_strdup("filename", file)) == NULL))
  return NULL;

 source = limit = target = file;
 if((c = *target++ = *source++) == 0) return file;
 if(c == '/')
  limit = target;
 else
 {
  target = source = file;
  c = '/';
 }

 do 
 {
  if(c == '/')
   while(42)
   {
    while(*source == '/') source++;

    if(*source != '.') break;

    if((c = source[1]) == '/')
     source += 2;
    else if((c == '.') && (source[2] == '/'))
    {
     source += 3;

     *target = 0;
     Xc_TRACE(("back in `%s'", file));

     if((target == limit) ||
	((target[-2] == '.') && (target[-3] == '.') &&
	 ((((target - 4) >= limit) && (target[-4] == '/')) ||
	  ((target - 3) == limit))))
     {
      Xc_TRACE(("add `../'"));
      *target++ = '.';
      *target++ = '.';
      *target++ = '/';
     }
     else
     {
      register char *ptr;

      for(ptr = target - 2; ptr != limit; ptr--)
       if(*ptr == '/')
       {
	ptr++;
	break;
       }
      target = ptr;
      Xc_TRACE(("remove `%s'", ptr));
     }
    }
    else
     break;
   }
 } while((c = *target++ = *source++) != 0);  
 Xc_TRACE(("--> stripped: `%s'", file));
 return file;
}

/* uncomment this line if char *Globprog_name doesn't exist */
/*#define NO_GLOBPROGNAME*/

#ifndef NO_GLOBPROGNAME
extern char *Globprog_name;
#endif

/* ----------------------------------------------------------------- ** 
** SearchFile - search file in all $PATH directories                 ** 
** ----------------------------------------------------------------- */
char *SearchFile(file)
char *file;
{
 char *found;
 char *spath, *path, *home;
 int i, j;
  
 if (IsFileExecutable(file))
 {
  if (*file == '/')
   return Xc_strdup("ff", file);
  path = GetWorkingDirectory();
  found = Xc_malloc("ff", strlen(file) + strlen(path) + 2);
  sprintf(found, "%s/%s", path, file);
  Xc_free(path);
  return found;
 }

#ifndef NO_GLOBPROGNAME
 if (Globprog_name)
 {
  char *tmp;
  
  j = strlen(Globprog_name) - 1;
  while(j > 0 && Globprog_name[j] != '/') j--;
  if (j > 0)
  {
   tmp = (char *)Xc_malloc("gpath", j + strlen(file) + 2); 
   strncpy(tmp, Globprog_name, j+1);
   tmp[j+1] = '\0';
   strcat(tmp, file);
   if (IsFileExecutable(tmp))
    return tmp;
   Xc_free(tmp);
  }
 }
#endif

 if ((spath = getenv("PATH")) == NULL)
  return NULL;
  
 path = Xc_strdup("path", spath);
 i = j = 0;
 while(path[i])
 {
  if (path[i] == ':')
  {
   path[i] = '\0';
   if (path[j] == '~')
   {
    if ((home = getenv("HOME")) == NULL)
    {
     j = i + 1;
     continue;
    }
    found = Xc_malloc("ff", strlen(file) + strlen(home) +
		      strlen(path + j) + 2);
    sprintf(found, "%s%s/%s", home, path + j + 1, file);
   }
   else
   {
    found = Xc_malloc("ff", strlen(file) + strlen(path + j) + 2);
    sprintf(found, "%s/%s", path + j, file);
   }
   if (IsFileExecutable(found))
   {
    Xc_free(path);
    return found;
   }
   Xc_free(found);
   j = i+1;
  }
  i++;
 }
  
 if (path[j] == '~')
 {
  if ((home = getenv("HOME")) == NULL)
   return NULL;
    
  found = Xc_malloc("ff", strlen(file) + strlen(home) +
		    strlen(path + j) + 2);
  sprintf(found, "%s%s/%s", home, path + j + 1, file);
 }
 else
 {
  found = Xc_malloc("ff", strlen(file) + strlen(path + j) + 2);
  sprintf(found, "%s/%s", path + j, file);
 }
 if (IsFileExecutable(found))
 {
  Xc_free(path);
  return found;
 }
 Xc_free(found);
  
 Xc_free(path);
 return NULL;
}

#if 0
void main(argc, argv)
int argc;
char **argv;
{
 char *basefile;
 char *file;

 if(argc < 3)
 {
  basefile = "/home/xcalibur/docs/document.xc";
  file = "/home/xcalibur/domages/image.gif";
 }
 else
 {
  basefile = argv[1];
  file = argv[2];
 }

 file = MakeRelativeFile(basefile, file);
 Xc_TRACE(("Relative: `%s'", file));

 basefile = MakeAbsoluteFile(basefile, file);
 Xc_free(file);
 Xc_TRACE(("Absolute: `%s'", basefile));
 free(basefile);
}
#endif
