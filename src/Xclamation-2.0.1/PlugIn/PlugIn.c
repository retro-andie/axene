/*
** PlugIn.c for Xclamation in PlugIn/
** Methods for PlugIn class
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
** Started on  Thu Oct 12 11:43:37 1995 St�phane Boisson
** Last update Thu Oct 24 19:19:27 1996 Emmanuel Paris
*/

#include "PlugInP.h"
#include "file.h"
#include "fnmatch.h"
#include <ctype.h>
#include <stdarg.h>

/*--- Prototypes ---*/
static void *constructor();
static void destructor();
static void *copy();

static plugin_item_t *createItem ___PROTO((c_PlugIn *this, char *id));
static boolean addItem ___PROTO((c_PlugIn *this, char *path, char *file));
static void removeItem ___PROTO((c_PlugIn *this, plugin_item_t *item));
static plugin_item_t *pluginGetByName ___PROTO((c_PlugIn *this, char *name));
static int pluginRequest(c_PlugIn *, plugin_item_t *, char *, ...);

plugin_header_t *pluginAddHeader ___PROTO((char *key, char *value));
plugin_header_list_t *pluginReadHeader ___PROTO((int fd));
boolean pluginWriteHeader ___PROTO((int fd,
				    plugin_header_list_t *header_list));
void pluginFreeHeader ___PROTO((plugin_header_list_t *header_list));
static char *getFromHeader ___PROTO((plugin_header_t *header, char *key));
static int pluginRegister ___PROTO((c_PlugIn *this, char *filename,
				    plugin_header_list_t *header));

/*--- Methods template init ---*/
sf_PlugIn fc_PlugIn =
{
  constructor, destructor, copy,
  addItem, removeItem,
  pluginGetByName,
  pluginRequest
};


/* ----------------------------------------------------------------- ** 
** constructor                                                       ** 
** ----------------------------------------------------------------- */
static void *constructor(type)
char *type;
{
  c_PlugIn *this;

  Xc_HISTORY(("constructor(%s)", type));

  if((this = Xc_malloc("PlugIn", sizeof(c_PlugIn))) == NULL)
    return this;
  memset(this, 0, sizeof(c_PlugIn));
  this->f = &fc_PlugIn;
  
  strncpy(this->type, type, XcMAX_NAME_LENGTH - 1);

  Xc_TRACE(("contructor done"));
  return this;
}


/* ----------------------------------------------------------------- ** 
** destructor                                                        ** 
** ----------------------------------------------------------------- */
static void destructor(this)
c_PlugIn *this;
{
  Xc_HISTORY(("destructor(%s)", this->type));

  /*--- Free all plug-in items ---*/
  while(this->base != NULL) F(this).removeItem(this, this->base);
  Xc_free(this);

  Xc_TRACE(("destructor done"));
}


/* ----------------------------------------------------------------- ** 
** Copy                                                              ** 
** ----------------------------------------------------------------- */
static void *copy(this)
c_PlugIn *this;
{
  Xc_HISTORY(("copy(%s)", this->type));

  Xc_FATAL(("not implemented"));
  return NULL;
}


/* ----------------------------------------------------------------- ** 
** createItem - Allocate a new plug-in item                          ** 
** ----------------------------------------------------------------- */
static plugin_item_t *createItem(this, id)
c_PlugIn *this;
char *id;
{
  plugin_item_t *item;

  Xc_HISTORY(("create(%s/%s)", this->type, id? id : "*UNKNOWN*"));
  
  if((item = Xc_malloc("plug-in item", sizeof(plugin_item_t))) == NULL)
    return item;

  /*--- Init fields ---*/
  memset(item, 0, sizeof(plugin_item_t));
  if(id != NULL)
    {
      char *ptr;

      strncpy(item->id, id, XcMAX_NAME_LENGTH - 1);
      for(ptr = item->id; *ptr; ptr++)
	if(isupper(*ptr)) *ptr = tolower(*ptr);
    }

  /*--- Insert in base ---*/
  item->next = this->base;
  this->base = item;
  return item;
}


/* ----------------------------------------------------------------- ** 
** removeItem - Remove a plug-in item                                ** 
** ----------------------------------------------------------------- */
static void removeItem(this, item)
c_PlugIn *this;
plugin_item_t *item;
{
  plugin_item_t **ptr;

  Xc_HISTORY(("remove(%s/%s)", this->type, item->id));

  for(ptr = &this->base; *ptr != NULL; ptr = &(*ptr)->next)
    if(*ptr == item)
      {
	int i;
	
	*ptr = item->next;
	
	if(item->filename != NULL) Xc_free(item->filename);
	if(item->name != NULL) Xc_free(item->name);
	if(item->author != NULL) Xc_free(item->author);
	if(item->copyright != NULL) Xc_free(item->copyright);
	if(item->comment != NULL) Xc_free(item->comment);
	for(i = 0; i <= XcPI_MAX_PREFIX; i++)
	  if(item->prefix[i] != NULL)
	    Xc_free(item->prefix[i]);
	  else
	    break;
	
	Xc_free(item);
	Xc_TRACE(("remove done"));
	return;
      }

  Xc_BREAK(("item %s/%s no found!", this->type, item->id));
}


/* ----------------------------------------------------------------- ** 
** addItem - Add a or several plugs-in                               ** 
** ----------------------------------------------------------------- */
static boolean addItem(this, path, filemask)
c_PlugIn *this;
char *path;
char *filemask;
{
  struct dirent *dir_entry;
  char *filename, *argv[2];
  DIR *dir_handle;
  long path_len;
  
  Xc_HISTORY(("add(%s, %s)", this->type, (path? path : "<default>")));

  if(filemask == NULL) filemask = "*";
  path = (path? Xc_strdup("plugs-in path", path) :
	  TranslateFilename(XcPLUGIN_DIR, ""));
  if(path == NULL) return FALSE;

  path_len = strlen(path);
  if(path[path_len? path_len - 1 : 0] == '/')
    path[path_len? path_len - 1 : 0] = 0;
  
  /*--- Open dir ---*/
  Xc_TRACE(("realpath=%s", path));
  if((dir_handle = opendir(path)) == NULL)
    {
      Xc_SYSERROR((path));
      Xc_free(path);
      return FALSE;
    }
  filename = NULL;
  
  /*--- Get files ---*/
  while((dir_entry = readdir(dir_handle)) != NULL)
    {
      plugin_header_list_t *header_list;
      struct stat file_info;
      int fd, pid;

      Xc_TRACE(("file: %s", dir_entry->d_name));

      /*--- Free previous filenames ---*/
      if(filename != NULL)
	{
	  Xc_free(filename);
	  filename = NULL;
	}

      /*--- Exclude "." and ".." ---*/
      if((dir_entry->d_name[0] == '.') &&
	 (((dir_entry->d_name[1] == '.') && !dir_entry->d_name[2]) ||
	  !dir_entry->d_name[1])) continue;
      
      if((filename = Xc_strconcat(path, "/", dir_entry->d_name, NULL)) == NULL)
	break;
      Xc_TRACE(("filename=%s", filename));

      /*--- Get more infos about file ---*/
      if((stat(filename, &file_info) < 0) && (lstat(filename, &file_info) < 0))
	continue;
      
      /*--- Recurse if directory ---*/
      if((file_info.st_mode & S_IFMT) == S_IFDIR)
	{
	  boolean rc;

	  Xc_TRACE(("recurse"));
	  rc = F(this).add(this, filename, filemask);
	  Xc_free(filename);
	  if(rc == FALSE) return FALSE;
	  continue;
	}
      else if((fnmatch(filemask, dir_entry->d_name, FNM_PERIOD) != 0) ||
	      (dir_entry->d_name[NLENGTH(dir_entry) - 1] == '~')) continue;
      
      /*--- Open module ---*/
      Xc_TRACE(("Get info"));
      argv[0] = dir_entry->d_name;
      argv[1] = NULL;
      if((pid = PipeOpen(filename, argv, &fd, NULL)) < 0)
	continue;
      Xc_TRACE(("pid=%d", pid));

      if((header_list = pluginReadHeader(fd)) == NULL) continue;
      pluginRegister(this, filename, header_list);
      pluginFreeHeader(header_list);      
    }

  closedir(dir_handle);

  /*--- Free previous filenames ---*/
  if(filename != NULL)
    {
      Xc_free(filename);
      filename = NULL;
    }
  return TRUE;
}



/* ----------------------------------------------------------------- ** 
** pluginReadHeader - Read header plug-in structure from stream      ** 
** ----------------------------------------------------------------- */
plugin_header_list_t *pluginReadHeader(fd)
int fd;
{
  plugin_header_list_t *base, **pbase;
  char buffer[XcPLUGIN_LINE_LENGTH + 1];
  plugin_header_t **pheader;
  int minor, major;
  boolean ok_flag;
  FILE *handle;

  Xc_TRACE(("pluginReadHeader"));

  if((handle = fdopen(fd, "r")) == NULL)
    {
      close(fd);
      return NULL;
    }

  /*--- Get header ---*/
  if((fscanf(handle, XcPLUGIN_HEADER, &major, &minor) != 2) ||
     (XcPLUGIN_MAKE_VERSION(major, minor) > XcPLUGIN_VERSION))
    {
      Xc_TRACE(("Not a plug-in module!"));
      fclose(handle);
      return NULL;
    }
  
  base = NULL;
  pbase = &base;
  pheader = NULL;

  ok_flag = TRUE;
  while(fgets(buffer, XcPLUGIN_LINE_LENGTH, handle) != NULL)
    {
      char *ptr, *mark;

      if((ptr = strrchr(buffer, '\n')) != NULL) *ptr = 0;
      if((ptr = strrchr(buffer, '\r')) != NULL) *ptr = 0;

      if(*buffer == 0)
	{
	  Xc_TRACE(("plug-in header end"));
	  ok_flag = TRUE;
	  continue;
	}
      else if(ok_flag == TRUE)
	{
	  ok_flag = FALSE;
	  *pbase = Xc_malloc("header list", sizeof(plugin_header_list_t));
	  if(*pbase == NULL) break;

	  (*pbase)->major = major;
	  (*pbase)->minor = minor;
	  pheader = &(*pbase)->header;
	  pbase = &(*pbase)->next;
	  *pheader = NULL;
	  *pbase = NULL;
	}

      if(*buffer == '#')
	continue;
      else if(!isalpha(*buffer))
	break;

      /*--- Parse header line ---*/
      for(ptr = buffer; (*ptr > 32) && (*ptr != ':'); ptr++);
      if(*ptr == 0) break;
      for(mark = ptr; *mark && (*mark != ':'); mark++);
      if(*mark == 0) break;
      *ptr = 0;
      for(mark++; *mark && (*mark <=32); mark++); 

      Xc_TRACE(("Key=<%s> Value=<%s>", buffer, mark));

      /*--- Alloc new header line ---*/
      if((*pheader = pluginAddHeader(buffer, mark)) == NULL) break;
      pheader = &(*pheader)->next;
    }
  fclose(handle);

  /*--- Test if error ---*/
  if(ok_flag == FALSE)
    {
      Xc_TRACE(("Error during parsing"));
      pluginFreeHeader(base);
      base = NULL;
    }

  return base;
}


/* ----------------------------------------------------------------- ** 
** pluginWriteHeader - Write header plug-in structure to stream      ** 
** ----------------------------------------------------------------- */
boolean pluginWriteHeader(fd, header_list)
int fd;
plugin_header_list_t *header_list;
{
  plugin_header_t *ptr;
  FILE *handle;
  boolean flag;

  Xc_TRACE(("pluginWriteHeader"));

  if((handle = fdopen(fd, "w")) == NULL)
  {
    close(fd);
    return FALSE;
  }

  flag = FALSE;
  do {
    /*--- Put header ---*/
    fprintf(handle, XcPLUGIN_HEADER, XcPLUGIN_VERSION_MAJOR,
	    XcPLUGIN_VERSION_MINOR);
    fputc('\n', handle);
    if(ferror(handle) || feof(handle)) break;

    for( ; header_list != NULL; header_list = header_list->next)
      {
	for(ptr = header_list->header; ptr != NULL; ptr = ptr->next)
	  {
	    fprintf(handle, "%s: %s\n", ptr->key, ptr->value);
	    if(ferror(handle) || feof(handle)) break;
	  }

	if(ptr != NULL) break;
	fputc('\n', handle);
	if(ferror(handle) || feof(handle)) break;
      }
    flag = (header_list == NULL)? TRUE : FALSE;
  } while(0);

  fclose(handle);
  return flag;
}


/* ----------------------------------------------------------------- ** 
** pluginAddHeader - Add a header to a header list                   ** 
** ----------------------------------------------------------------- */
plugin_header_t *pluginAddHeader(key, value)
char *key;
char *value;
{
  plugin_header_t *ptr;

  Xc_TRACE(("addHeader(%s, %s)", key, value));

  /*--- Alloc structure ---*/
  if((ptr = Xc_malloc("header", sizeof(plugin_header_t))) == NULL)
    return ptr;

  /*--- Alloc key string ---*/
  if((ptr->key = Xc_strdup("key", key)) == NULL)
    {
      Xc_free(ptr);
      return NULL;
    }

  /*--- Alloc value string ---*/
  if((ptr->value = Xc_strdup("value", value)) == NULL)
    {
      Xc_free(ptr->key);
      Xc_free(ptr);
      return NULL;
    }

  ptr->next = NULL;
  return ptr;
}


/* ----------------------------------------------------------------- ** 
** pluginFreeHeader - Free header plug-in structure                  ** 
** ----------------------------------------------------------------- */
void pluginFreeHeader(header_list)
plugin_header_list_t *header_list;
{
  plugin_header_t *header;
  void *ptr;

  Xc_TRACE(("pluginFreeHeader"));

  while(header_list != NULL)
    {
      header = header_list->header;
      ptr = header_list;
      header_list = header_list->next;
      Xc_free(ptr);

      while(header != NULL)
	{
	  ptr = header;
	  if(header->key != NULL) Xc_free(header->key);
	  if(header->value != NULL) Xc_free(header->value);
	  header = header->next;
	  Xc_free(ptr);
	}
    }
}


/* ----------------------------------------------------------------- ** 
** pluginGetByName - Get a plug-in item by name                      ** 
** ----------------------------------------------------------------- */
static plugin_item_t *pluginGetByName(this, name)
c_PlugIn *this;
char *name;
{
  plugin_item_t *ptr;
  int i;

  Xc_HISTORY(("getByName(%s/%s)", this->type, name));

  for(ptr = this->base; ptr != NULL; ptr = ptr->next)
    for(i = 0; (isupper(name[i])? tolower(name[i]):name[i]) == ptr->id[i]; i++)
      if(name[i] == 0) return ptr;
  
  Xc_TRACE(("Not found"));
  return NULL;
}


/* ----------------------------------------------------------------- ** 
** pluginRequest - Send a request to a plug-in module                ** 
** ----------------------------------------------------------------- */
static int pluginRequest(c_PlugIn *this, plugin_item_t *item, char *request, ...)
{
  char buffer[XcPLUGIN_LINE_LENGTH + 1], *argv[2], *key;
  plugin_header_list_t *list;
  plugin_header_t **pheader;
  int in, out;

  va_list ap;

  Xc_HISTORY(("request(%s %s/%s)", request, this->type, item->id));

  if((list = Xc_malloc("header list", sizeof(plugin_header_list_t))) == NULL)
    return -1;
  list->next = NULL;
  list->header = NULL;
  pheader = &list->header;

  va_start(ap, request);
  
  in = -1;
  do {
    /*--- Type header ---*/
    sprintf(buffer, "%s/%s", this->type, item->id);
    if((*pheader = pluginAddHeader(XcPI_TYPE_KEYWORD, buffer)) == NULL) break;

    /*--- User headers ---*/
    while((key = va_arg(ap, char *)) != XcPI_END_KEYWORD)
      {
	Xc_ASSERT(strcmp(key, XcPI_TYPE_KEYWORD));
	Xc_ASSERT(strcmp(key, XcPI_REQUEST_KEYWORD));
	if((*pheader = pluginAddHeader(key, va_arg(ap, char *))) == NULL)
	  break;
      }
    if(key != NULL) break;

    /*--- Request header ---*/
    if((*pheader = pluginAddHeader(XcPI_REQUEST_KEYWORD, request)) == NULL)
      break;

    /*--- Execute command ---*/
    argv[0] = XcPLUGIN_COMMAND;
    argv[1] = NULL;
    if(PipeOpen(item->filename, argv, &in, &out) < 0) break;

    /*--- Send request to module ---*/
    if(pluginWriteHeader(out, list) == FALSE)
      {
	close(in);
	in = -1;
      }
    
  } while(0);

  va_end(ap);
  pluginFreeHeader(list);
  return in;
}


/* ----------------------------------------------------------------- ** 
** getFromHeader - Get a value from a key                            ** 
** ----------------------------------------------------------------- */
static char *getFromHeader(header, key)
plugin_header_t *header;
char *key;
{
  Xc_TRACE(("getFromHeader(`%s')", key));

  for( ; header != NULL; header = header->next)
    if(!strcmp(header->key, key))
      {
	Xc_TRACE(("value: `%s'", header->value));
	return header->value;
      }
  return NULL;
}


/* ----------------------------------------------------------------- ** 
** pluginRegister - Register plug-in modules                         ** 
** ----------------------------------------------------------------- */
static int pluginRegister(this, filename, header_list)
c_PlugIn *this;
char *filename;
plugin_header_list_t *header_list;
{
  plugin_header_t *header;
  plugin_item_t *item;
  char *value;
  int count;
  long len;

  Xc_TRACE(("pluginRegister(%s)", this->type));

  len = strlen(this->type);
  item = NULL;

  for(count = 0; header_list != NULL; header_list = header_list->next)
    {
      header = header_list->header;

      if(item != NULL) F(this).removeItem(this, item);

      if((value = getFromHeader(header, XcPI_TYPE_KEYWORD)) == NULL)
	continue;
      if(strncmp(this->type, value, len) || (value[len] != '/'))
	continue;

      /*--- Got a good one ---*/
      if((item = createItem(this, value + len + 1)) == NULL)
	continue;
      if((item->filename = Xc_strdup("filename", filename)) == NULL)
	continue;
      item->major = header_list->major;
      item->minor = header_list->minor;
      
      /*--- Module name ---*/
      if(((value = getFromHeader(header, XcPI_NAME_KEYWORD)) == NULL) ||
	 ((item->name = Xc_strdup("name", value)) == NULL)) continue;

      /*--- Module author ---*/
      if(((value = getFromHeader(header, XcPI_AUTHOR_KEYWORD)) != NULL) &&
	 ((item->author = Xc_strdup("author", value)) == NULL)) continue;

      /*--- Module copyright ---*/
      if(((value = getFromHeader(header, XcPI_COPYRIGHT_KEYWORD)) != NULL) &&
	 ((item->copyright = Xc_strdup("copyright", value)) == NULL)) continue;

      /*--- Module comments ---*/
      if(((value = getFromHeader(header, XcPI_COMMENT_KEYWORD)) != NULL) &&
	 ((item->comment = Xc_strdup("comment", value)) == NULL)) continue;
      
      /*--- File prefixes ---*/
      if((value = getFromHeader(header, XcPI_FILE_PREFIX_KEYWORD)) != NULL)
	{
	  char *ptr;
	  int i;

	  for(i = 0; (i <= XcPI_MAX_PREFIX) && (*value != 0); i++)
	    {
	      for(ptr = value; *ptr > 32; ptr++);
	      Xc_ASSERT(ptr > value);
	      item->prefix[i] = Xc_strndup("prefix", value, ptr - value);
	      if(item->prefix[i] == NULL) break;
	      Xc_TRACE(("prefix: %s", item->prefix[i]));
	      for(value = ptr; *value && (*value <= 32); value++);
	    }
	}
      
      count++;
      item = NULL;
    }

  if(item != NULL) F(this).removeItem(this, item);
  return count;
}
