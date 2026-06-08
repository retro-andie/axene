/*
** Import_Text.c for XQuad in Import/
** Import_Text Object
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
** Started on  Wed Feb 22 16:59:27 1995 Emmanuel Paris
** Last update Thu Sep 11 20:19:49 1997 Robin Castanier
*/

/*#define NTRACE*/

#include "Import_Text.h"

static void *cons ___PROTO((int style));
static void dest ___PROTO((c_Import_Text *This));
static void *copy ___PROTO((c_Import_Text *This));

static boolean test ___PROTO((c_Import_Text *This));
static boolean create_new_sheet ___PROTO((c_Import_Text *This));
static error load ___PROTO((c_Import_Text * This, c_Tableau *sheet));

static int get_field();
static void trim_before();
static void trim_after();
static boolean get_sequence();
static boolean get_separator_cmd();
static boolean get_separator_nbchar();
void repair_cmd_line();

static boolean			box_created = FALSE;
static import_text_box_t	box;

sf_Import_Std fc_Import_Text = 
{
  cons, 
  dest, 
  copy,
  txt_create_attribs_box, 
  txt_manage_attribs_box, 
  txt_destroy_attribs_box,
  test, 
  create_new_sheet, 
  load
};

static 	char *Extensions_TabPrn[] = { "prn" };
static 	char *Extensions_TabTxt[] = { "txt" };
static 	char *Extensions_TabCsv[] = { "csv" };

static void *cons(style)
int style;
{
  c_Import_Text	*This;
    
  if ((This = (c_Import_Text *)Xc_malloc("Import Text", 
					 sizeof(c_Import_Text))) == NULL)
  {
    Xc_FATAL(("Can't build this object: memory allocation error"));
  }
  This->f = &fc_Import_Text;

  switch(style)
    {
    case 0:
      This->name = "Text Unix, separator space";
      This->copyright = "Copyright Gato 1995";
      This->id =  
	F(GlobResources).getString(GlobResources, XcR_importDocTextSpaceUnix);
      This->extension_count = 1;
      This->extensions = Extensions_TabPrn;
      strcpy(This->sep_cmd, "\" \"");
      This->trim_before = TRUE;
      This->trim_after = TRUE;
      This->encoding = XcE_LATIN1;
      break;
    case 1:
      This->name = "Text Unix, separator tab";
      This->copyright = "Copyright Gato 1995";
      This->id = 
	F(GlobResources).getString(GlobResources, XcR_importDocTextTabUnix);
      This->extension_count = 1;
      This->extensions = Extensions_TabTxt;
      strcpy(This->sep_cmd, "\"\\t\"");
      This->trim_before = TRUE;
      This->trim_after = TRUE;
      This->encoding = XcE_LATIN1;
      break;
    case 2:
      This->name = "Text Unix, separator ;";
      This->copyright = "Copyright Gato 1995";
      This->id = 
	F(GlobResources).getString(GlobResources, XcR_importDocTextCSVUnix);
      This->extension_count = 1;
      This->extensions = Extensions_TabCsv;
      strcpy(This->sep_cmd, "\";\"");
      This->trim_before = TRUE;
      This->trim_after = TRUE;
      This->encoding = XcE_LATIN1;
      break;
    case 3:
      This->name = "Text OS2 Dos, separator space";
      This->copyright = "Copyright Gato 1995";
      This->id =  
	F(GlobResources).getString(GlobResources, XcR_importDocTextSpaceDos);
      This->extension_count = 1;
      This->extensions = Extensions_TabPrn;
      strcpy(This->sep_cmd, "\" \"");
      This->trim_before = TRUE;
      This->trim_after = TRUE;
      This->encoding = XcE_DOS;
      break;
    case 4:
      This->name = "Text OS2 Dos, separator tab";
      This->copyright = "Copyright Gato 1995";
      This->id = 
	F(GlobResources).getString(GlobResources, XcR_importDocTextTabDos);
      This->extension_count = 1;
      This->extensions = Extensions_TabTxt;
      strcpy(This->sep_cmd, "\"\\t\"");
      This->trim_before = TRUE;
      This->trim_after = TRUE;
      This->encoding = XcE_DOS;
      break;
    case 5:
      This->name = "Text OS2 Dos, separator ;";
      This->copyright = "Copyright Gato 1995";
      This->id = 
	F(GlobResources).getString(GlobResources, XcR_importDocTextCSVDos);
      This->extension_count = 1;
      This->extensions = Extensions_TabCsv;
      strcpy(This->sep_cmd, "\";\"");
      This->trim_before = TRUE;
      This->trim_after = TRUE;
      This->encoding = XcE_DOS;
      break;
    case 6:
      This->name = "Text Mac, separator space";
      This->copyright = "Copyright Gato 1995";
      This->id =  
	F(GlobResources).getString(GlobResources, XcR_importDocTextSpaceMac);
      This->extension_count = 1;
      This->extensions = Extensions_TabPrn;
      strcpy(This->sep_cmd, "\" \"");
      This->trim_before = TRUE;
      This->trim_after = TRUE;
      This->encoding = XcE_MAC;
      break;
    case 7:
      This->name = "Text Mac, separator tab";
      This->copyright = "Copyright Gato 1995";
      This->id = 
	F(GlobResources).getString(GlobResources, XcR_importDocTextTabMac);
      This->extension_count = 1;
      This->extensions = Extensions_TabTxt;
      strcpy(This->sep_cmd, "\"\\t\"");
      This->trim_before = TRUE;
      This->trim_after = TRUE;
      This->encoding = XcE_MAC;
      break;
    case 8:
      This->name = "Text Mac, separator ;";
      This->copyright = "Copyright Gato 1995";
      This->id = 
	F(GlobResources).getString(GlobResources, XcR_importDocTextCSVMac);
      This->extension_count = 1;
      This->extensions = Extensions_TabCsv;
      strcpy(This->sep_cmd, "\";\"");
      This->trim_before = TRUE;
      This->trim_after = TRUE;
      This->encoding = XcE_MAC;
      break;
    }
  
  This->filename = NULL;
  This->box_created = &box_created;
  
  This->box = &box;
  This->box->managed = FALSE;
  This->new_doc = TRUE;
  return This;
}

static void dest(This)
c_Import_Text *This;
{
  Xc_free(This);
}

static void *copy(This)
c_Import_Text *This;
{
  Xc_TRACE(("COPY Import_Text Object not done: normally not usefull."));
  return NULL;
}


static boolean test(This)
c_Import_Text *This;
{
  return TRUE;
}

static boolean create_new_sheet(This)
c_Import_Text *This; 
{
  return This->new_doc;
}

#define SEPARATOR_REACHED 0
#define DO_NOT_ADD_COLUMN 1
#define END_OF_LINE_REACHED 2
#define END_OF_FILE_REACHED 3

enum {
  SEP_ANY, SEP_CMD, SEP_NBCHAR, SEP_REPEAT, SEP_LAST
};

typedef union _sep_info sep_info_t;

typedef struct
{
  int	type;
  sep_info_t	*Next;
  boolean	not;
} sep_any_t; 

typedef struct
{
  int	type;
  sep_info_t	*Next;
  boolean	not;
  int	sep_cmd_lng;
  char	*sep_cmd;
  boolean sep_repeat;
} sep_cmd_t;

typedef struct
{
  int	type;
  sep_info_t	*Next;
  boolean	not;
  int	sep_nbchar;
} sep_nbchar_t;

typedef struct
{
  int	type;
  sep_info_t	*Next;
  boolean	not;
  sep_info_t	*sep_repeat;
} sep_repeat_t;

typedef struct
{
  int	type;
  sep_info_t	*Next;
  boolean	not;
  boolean	repeat;
  sep_info_t	*sep_repeat;
} sep_last_t;

union _sep_info
{
  int		type;
  sep_any_t	sep_any;
  sep_cmd_t	sep_cmd;
  sep_nbchar_t	sep_nbchar;
  sep_repeat_t	sep_repeat;
  sep_last_t	sep_last;
};

static error load(This, sheet)
c_Import_Text * This;
c_Tableau *sheet;
{
  FILE	*file;
  long	taille;
  char	cmd_field[200];
  char	*cmd_line;
  int	cmd_lng;
  boolean	last_sequence;
  boolean	not_sequence;
  char	sep_cmd[200];
  int	sep_lng;
  int	nbchar;
  boolean 	sep_rept;
  boolean	sep_error;
  
  sep_info_t	*sep_info;
  sep_info_t	*sep_info_prev;
  sep_info_t	*sep_info_top;
  
  char	buffer[Xq_FORMULA_LENGTH];
  int	buf_lng;
  int	column, orig_column, row;

  boolean	sortie = FALSE;
  c_Moteur	*moteur;
  cell	*cellule;
  
  if (!(file = fopen(This->filename,"rb")))
  {
    Xc_WARNING(("file %s doesn't exist or unreadable", This->filename));
    return XC_FILE_NOT_EXIST;
  }
  fseek(file, 0L, SEEK_END);
  taille = ftell(file);
  fseek(file, 0L, SEEK_SET);
  
  if (This->new_doc)
  {
    column = orig_column = row = 0;
  }
  else
  {
    column = orig_column = sheet->Feuille->cell_x;
    row = sheet->Feuille->cell_y;
  }
  
  cmd_line = This->sep_cmd; 
  last_sequence = FALSE;
  cmd_lng = strlen(cmd_line);
  trim_before(cmd_line, &cmd_lng);
  trim_after(cmd_line, &cmd_lng);
  sep_info_prev = sep_info_top = NULL;

  while(!last_sequence)
  {
    last_sequence = get_sequence(cmd_line, cmd_field, &cmd_lng);
    Xc_TRACE(("sequence: cmd: %s",  cmd_field));
    
    if (cmd_field[0] == '!')
    {
      not_sequence = TRUE;
      memmove (cmd_field, cmd_field + 1, strlen(cmd_field) + 1);
      Xc_TRACE(("not sequence"));
    }
    else
      not_sequence = FALSE;
    
    if (cmd_field[0] == '"')
    {
      sep_error = get_separator_cmd(cmd_field, sep_cmd, &sep_lng, &sep_rept);
      if (!sep_error)
      {
	Xc_TRACE(("separator: %s", sep_cmd));
	
	sep_info = (sep_info_t *)Xc_malloc("sep info", sizeof(sep_info_t));
	if (sep_info_prev)
	  sep_info_prev->sep_any.Next = sep_info;
	if (!sep_info_top) 
	  sep_info_top = sep_info;
	sep_info->sep_any.Next = NULL;
	
	sep_info->sep_cmd.type = SEP_CMD;
	sep_info->sep_cmd.not = not_sequence;
	sep_info->sep_cmd.sep_cmd_lng = sep_lng;
	sep_info->sep_cmd.sep_cmd = (char *)Xc_malloc("sep cmd", sep_lng);
	memcpy(sep_info->sep_cmd.sep_cmd, sep_cmd, sep_lng);
	sep_info->sep_cmd.sep_repeat = sep_rept;
	sep_info_prev = sep_info;
      }
      else
      {
	Xc_TRACE(("separator cmd error"));	
      }
    }
    else
    {
      if (cmd_field[0] == '\0')
      {
	if (sep_info_top)
	{
	  sep_info = (sep_info_t *)Xc_malloc("sep info", sizeof(sep_info_t));
	  sep_info_prev->sep_any.Next = sep_info;
	  sep_info->sep_any.Next = NULL;
	  
	  sep_info->sep_repeat.type = SEP_REPEAT;
	  sep_info->sep_repeat.not = not_sequence;
	  sep_info->sep_repeat.sep_repeat = sep_info_prev;
	  sep_info_prev = sep_info;
	}
	else
	{
	  Xc_TRACE(("; in first position not allowed"));
	}
      }
      else
      {
	sep_error = get_separator_nbchar(cmd_field, &nbchar);
	if (!sep_error)
	{
	  Xc_TRACE(("separator nbchar: %d", nbchar));
	  
	  sep_info = (sep_info_t *)Xc_malloc("sep info", sizeof(sep_info_t));
	  if (sep_info_prev)
	    sep_info_prev->sep_any.Next = sep_info;
	  if (!sep_info_top) 
	    sep_info_top = sep_info;
	  sep_info->sep_any.Next = NULL;
	  
	  sep_info->sep_nbchar.type = SEP_NBCHAR;
	  sep_info->sep_nbchar.not = not_sequence;
	  sep_info->sep_nbchar.sep_nbchar = nbchar;
	  sep_info_prev = sep_info;
	}
	else
	{
	  Xc_TRACE(("separator nbchar error"));	
	}
      }
    }
    
    if (last_sequence)
    {
      sep_info = (sep_info_t *)Xc_malloc("sep info", sizeof(sep_info_t));
      if (sep_info_prev)
	sep_info_prev->sep_any.Next = sep_info;
      if (!sep_info_top) 
	sep_info_top = sep_info;
      sep_info->sep_any.Next = NULL;
      
      sep_info->sep_last.type = SEP_LAST;
      sep_info->sep_last.not = not_sequence;
      if ((sep_info_prev && sep_info_prev->type == SEP_REPEAT) 
	  || !sep_info_prev)
      {
	sep_info->sep_last.repeat = FALSE;
      }
      else
      {
	sep_info->sep_last.repeat = TRUE;
	sep_info->sep_last.sep_repeat = sep_info_prev;
      }
    }
    cmd_line += cmd_lng;
  }
  
  sep_info = sep_info_top;
  moteur = sheet->moteur; 
  
  while(sortie == FALSE)
  {
    int ret_type;
    
    ret_type = get_field(file, buffer, &buf_lng, sep_info);
    convert_encoding(buffer, This->encoding);
    if (This->trim_before)
      trim_before(buffer, &buf_lng);
      if (This->trim_after)
	trim_after(buffer, &buf_lng);
    if (buf_lng)
      {
	cellule = F(moteur).add_cell(moteur, row, column);
	F(moteur).set_string(moteur, column, row, cellule, buffer, 
			     sheet->Feuille);
	Xc_TRACE(("fill cell %c%d with: >>%s<<", column+'A', row+1, buffer));
      }
    switch(ret_type)
      {
      case SEPARATOR_REACHED:
	column++;
	if (column > COLUMN_MAX)
	{
	  row++;
	  column = orig_column;
	  if (row > ROW_MAX)
	    sortie = TRUE;
	}
	if (sep_info->type != SEP_LAST)
	  sep_info = sep_info->sep_any.Next;
	break;
      case DO_NOT_ADD_COLUMN:
	if (sep_info->type != SEP_LAST)
	  sep_info = sep_info->sep_any.Next;
	break;
      case END_OF_LINE_REACHED:
	column = orig_column;
	row++;
	if (row > ROW_MAX)
	  sortie = TRUE;
	sep_info = sep_info_top;
	break;
      case END_OF_FILE_REACHED:
	sortie = TRUE;
	break;
      default:
	Xc_WARNING(("unknow return type. error !!!!!!! (sep_info->type = %d)",
		    sep_info->type));
	break;
      }
  }
  
  sep_info = sep_info_top;
  while(sep_info)
    {
      sep_info_prev = sep_info;
      sep_info = sep_info->sep_any.Next;
      if (sep_info_prev->type == SEP_CMD)
	Xc_free(sep_info_prev->sep_cmd.sep_cmd);
      Xc_free(sep_info_prev);
    }
  fclose(file);
  return XC_NO_ERROR;
}


static int get_field(file, buffer, buf_lng, sep_info)
FILE	*file;
char	buffer[Xq_FORMULA_LENGTH];
int	*buf_lng;
sep_info_t	*sep_info;
{
  int		ret_type = -1, i, j;
  boolean	sortie = FALSE;
  boolean	skip = FALSE;
  boolean	endofcol;
  int		sep_lng;
  boolean	sep_rept;
  char		*sep_ss, c, c2;
  long		pos;
  
  i = 0;
  switch(sep_info->type)
  {
  case SEP_CMD:
    sep_ss = sep_info->sep_cmd.sep_cmd;
    sep_lng = sep_info->sep_cmd.sep_cmd_lng;
    sep_rept = sep_info->sep_cmd.sep_repeat;

    while(!sortie)
    {
      c =  fgetc(file);
      
      if (feof(file))
	{
	  sortie = TRUE;
	  ret_type = END_OF_FILE_REACHED;
	  continue;
	}
      
      if ( sep_lng && c == sep_ss[0])
	{
	  if (sep_rept)
	    {
	      endofcol = FALSE;
	      fseek(file, -1L, SEEK_CUR);
	      while(!sortie)
		{
		  pos = ftell(file);
		  j = 0;
		  while(j < sep_lng)
		    {
		      c2 =  fgetc(file);
		      if (c2 == EOF && feof(file))
			break;
		      if (c2 != sep_ss[j])
			break;
		      j++;
		    }
		  if (j != sep_lng)
		    {	/* not a separator */
		      if (!endofcol) 
			{
			  if (!skip)
			    buffer[i++] = c;
			  fseek(file, pos+1, SEEK_SET);
			}
		      else
			{
			  fseek(file, pos, SEEK_SET);
			}
		      sortie = TRUE;
		    }
		  else
		    {	/* separator found */
		      ret_type = SEPARATOR_REACHED;
		      endofcol = TRUE;
		    }
		}
	      sortie = endofcol;
	    }
	  else
	    {
	      pos = ftell(file);
	      j = 1;
	      while(j < sep_lng)
		{
		  c2 =  fgetc(file);
		  if (c2 == EOF && feof(file))
		    break;
		  if (c2 != sep_ss[j])
		    break;
		  j++;
		}
	      if (j != sep_lng)
		{	/* not a separator */
		  if (!skip)
		    buffer[i++] = c;
		  fseek(file, pos, SEEK_SET);
		}
	      else
		{	/* separator found */
		  sortie = TRUE;
		  ret_type = SEPARATOR_REACHED;
		}
	    }
	}
      else
	{
	  if (c == 0x0d)
	    {
	      c = fgetc(file);
	      if (c != 0x0a)
		{
		  if (c == EOF && feof(file))
		    {
		      sortie = TRUE;
		      ret_type = END_OF_FILE_REACHED;
		      continue;
		    } 
		  fseek(file, -1L, SEEK_CUR);
		}
	      sortie = TRUE;
	      ret_type = END_OF_LINE_REACHED;
	      continue;
	    }
	  if (c == 0x0a)
	    {
	      c = fgetc(file);
	      if (c != 0x0d)
		{
		  if (c == EOF && feof(file))
		    {
		      sortie = TRUE;
		      ret_type = END_OF_FILE_REACHED;
		      continue;
		    } 
		  fseek(file, -1L, SEEK_CUR);
		}
	      sortie = TRUE;
	      ret_type = END_OF_LINE_REACHED;
	      continue;
	    }
	  if (!skip)
	    buffer[i++] = c;
	}
      if (i > 298)
	skip = TRUE;
    }
    if (sep_info->sep_cmd.not)
      {
	if (ret_type == SEPARATOR_REACHED) 
	  ret_type = DO_NOT_ADD_COLUMN;
	i = 0;
      }
    *buf_lng = i;
    buffer[i] = '\0';
    break;
  case SEP_NBCHAR:
    while(!sortie)
    {
      c =  fgetc(file);
      
      switch(c)
      { 
      case 0x0d:
	c = fgetc(file);
	if (c != 0x0a)
	{
	  if (c == EOF && feof(file))
	  {
	    sortie = TRUE;
	    ret_type = END_OF_FILE_REACHED;
	    break;
	  } 
	  fseek(file, -1L, SEEK_CUR);
	}
	sortie = TRUE;
	ret_type = END_OF_LINE_REACHED;
	break;
      case 0x0a:
	c = fgetc(file);
	if (c != 0x0d)
	{
	  if (c == EOF && feof(file))
	  {
	    sortie = TRUE;
	    ret_type = END_OF_FILE_REACHED;
	    break;
	  } 
	  fseek(file, -1L, SEEK_CUR);
	}
	sortie = TRUE;
	ret_type = END_OF_LINE_REACHED;
	break;
      case EOF:
	if (feof(file))
	{
	  sortie = TRUE;
	  ret_type = END_OF_FILE_REACHED;
	  break;
	}
      default:
	if (!skip)
	  buffer[i++] = c;
	if (i >= sep_info->sep_nbchar.sep_nbchar)
	  {
	    sortie = TRUE;
	    ret_type = SEPARATOR_REACHED;
	  }
      }
      if (i > 298)
	skip = TRUE;
    }
    
    if (sep_info->sep_nbchar.not)
      {
	if (ret_type == SEPARATOR_REACHED) 
	  ret_type = DO_NOT_ADD_COLUMN;
	i = 0;
      }
    *buf_lng = i;
    buffer[i] = '\0';
    break;
  case SEP_REPEAT:
    ret_type = get_field(file, buffer, buf_lng, 
			 sep_info->sep_repeat.sep_repeat);
    if (sep_info->sep_last.not)
    {
      if (ret_type == SEPARATOR_REACHED) 
	ret_type = DO_NOT_ADD_COLUMN;
      *buf_lng = i;
      buffer[i] = '\0';
    }
    break;
  case SEP_LAST:
    if (sep_info->sep_last.repeat)
    {
      ret_type = get_field(file, buffer, buf_lng, 
			   sep_info->sep_last.sep_repeat);
      if (sep_info->sep_last.not)
      {
	if (ret_type == SEPARATOR_REACHED) 
	  ret_type = DO_NOT_ADD_COLUMN;
	*buf_lng = i;
	buffer[i] = '\0';
      }
    }
    else
    {
      while(!sortie)
      {
	c =  fgetc(file);
	
	switch(c)
	{ 
	case 0x0d:
	  c = fgetc(file);
	  if (c != 0x0a)
	  {
	    if (c == EOF && feof(file))
	    {
	      sortie = TRUE;
	      ret_type = END_OF_FILE_REACHED;
	      break;
	    } 
	    fseek(file, -1L, SEEK_CUR);
	  }
	  sortie = TRUE;
	  ret_type = END_OF_LINE_REACHED;
	  break;
	case 0x0a:
	  c = fgetc(file);
	  if (c != 0x0d)
	  {
	    if (c == EOF && feof(file))
	    {
	      sortie = TRUE;
	      ret_type = END_OF_FILE_REACHED;
	      break;
	    } 
	    fseek(file, -1L, SEEK_CUR);
	  }
	  sortie = TRUE;
	  ret_type = END_OF_LINE_REACHED;
	  break;
	case EOF:
	  if (feof(file))
	  {
	    sortie = TRUE;
	    ret_type = END_OF_FILE_REACHED;
	    break;
	  }
	default:
	  if (!skip)
	    buffer[i++] = c;
	}
	if (i > 298)
	  skip = TRUE;
      }
      if (sep_info->sep_last.not)
	{
	  if (ret_type == SEPARATOR_REACHED) 
	    ret_type = DO_NOT_ADD_COLUMN;
	  i = 0;
	}
      *buf_lng = i;
      buffer[i] = '\0';
    }
    break;
  }
  return ret_type;
}

static void trim_before(buffer, buf_lng)
char	buffer[Xq_FORMULA_LENGTH];
int	*buf_lng;
{
  int	i;
  
  i = 0;
  while( i < *buf_lng && buffer[i] == ' ')
    i++;
  
  if (i!=0)
  {
    memmove(buffer, buffer + i, *buf_lng - i + 1);
    *buf_lng = *buf_lng - i;
  }
}

static void trim_after(buffer, buf_lng)
char	buffer[Xq_FORMULA_LENGTH];
int	*buf_lng;
{
  int	i;
  
  i = *buf_lng - 1;
  while( i >= 0 && buffer[i] == ' ')
    i--;
  
  buffer[i + 1] = '\0';
  *buf_lng = i + 1;
}

/* ----------------------------------------------------------------- ** 
** get_sequence                                                      ** 
** ----------------------------------------------------------------- */ 
static boolean get_sequence(cmd_line, cmd, lng)
char	*cmd_line;
char	*cmd;
int	*lng;
{
  int i, guillemot;
  boolean slash;
  
  i = guillemot = 0; slash = FALSE;
  
  while(1)
  {
    if (cmd_line[i] == '\0')
      break;
    
    if (cmd_line[i] == '\\' && !slash)
    {
      slash = TRUE;
    }
    else
    {
      if (cmd_line[i] == '"' && !slash)
	guillemot++;
      
      if (cmd_line[i] == ';' && (guillemot == 0 || guillemot == 2))
      {
	break;
      }
      slash = FALSE;
    }
    i++;
  }
  
  if (i==0)
  {
    cmd[0] = '\0';
  }
  else
  {
    memcpy(cmd, cmd_line, i);
    cmd[i] = '\0';
  }
  if (cmd_line[i] == ';')
  {
    *lng = i+1;
    return FALSE;
  }
  *lng = i;
  return TRUE;
}

/* ----------------------------------------------------------------- ** 
** get_separator_cmd                                                 ** 
** ----------------------------------------------------------------- */ 
static boolean get_separator_cmd(cmd_field, sep_cmd, sep_lng, sep_rept)
char	*cmd_field;
char	*sep_cmd;
int	*sep_lng;
boolean	*sep_rept;
{
  int i, j, dt;
  char	c;
  
  i = strlen(cmd_field);
  if (i<3) return TRUE;
  if (cmd_field[i-1] == '*')
    {
      cmd_field[--i] = '\0';
      *sep_rept = TRUE;
    }
  else
    *sep_rept = FALSE;
  if (cmd_field[0] != '"' || cmd_field[i-1] != '"') return TRUE;
  cmd_field[i-1] = '\0';
  
  i = 1; j = 0;
  while(cmd_field[i])
  {
    if (cmd_field[i] == '\\')
    {
      c = cmd_field[++i];
      switch(c)
      {
      case '\0':
	i--;  /* with the i++; at then end, can exit normally */
      case '\\':
	sep_cmd[j++] = c;
	break;
      case 't':
	sep_cmd[j++] = 0x09;
	break;
      case 'n':
	sep_cmd[j++] = 0x0a;
	break;
      case 'r':
	sep_cmd[j++] = 0x0d;
	break;
      case '0':
	c = cmd_field[++i];
	switch(c)
	{
	case 'x':
	  i++;
	  dt = 0;
	  while(  (cmd_field[i] >= '0' && cmd_field[i] <= '9')
		||(cmd_field[i] >= 'a' && cmd_field[i] <= 'f')
		||(cmd_field[i] >= 'A' && cmd_field[i] <= 'F'))
	  {
	    dt *= 16;
	    c = cmd_field[i++];
	    if (c < '9')
	      dt += c - '0';
	    else
	    {
	      if ( c < 'a')
		dt += c - 'A';
	      else
		dt += c - 'a';
	    }
	  }
	  i--;
	  if (dt >= 0 && dt <= 255)
	    sep_cmd[j++] = dt;
	  break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	  dt = 0;
	  while(cmd_field[i] >= '0' && cmd_field[i] <= '7')
	  {
	    dt *= 8;
	    dt += cmd_field[i++] - '0';
	  }
	  i--;
	  if (dt >= 0 && dt <= 255)
	    sep_cmd[j++] = dt;
	  break;
	default:
	  sep_cmd[j++] = '\0';
	  break;
	}
	break;
      default:
	if (c>'0' && c<='9')
	{
	  dt = 0;
	  while(cmd_field[i] >= '0' && cmd_field[i] <= '9')
	  {
	    dt *= 10;
	    dt += cmd_field[i++] - '0';
	  }
	  i--;
	  if (dt >= 0 && dt <= 255)
	    sep_cmd[j++] = dt;
	}
	break;
      }
    }
    else
    {
      sep_cmd[j++] = cmd_field[i];
    }
    i++;
  }
  sep_cmd[j] = '\0';
  *sep_lng = j;
  
  return FALSE;
}

/* ----------------------------------------------------------------- ** 
** get_separator_nbchar                                              ** 
** ----------------------------------------------------------------- */ 
static boolean get_separator_nbchar(cmd_field, nbchar)
char	*cmd_field;
int	*nbchar;
{
  int	i;
  
  i = 0;
  while(cmd_field[i] >= '0' && cmd_field[i]<='9')
    i++;
  
  if (cmd_field[i] != '\0')
    return TRUE;
  
  *nbchar = atoi(cmd_field);
  if ( (*nbchar <= 0) || (*nbchar > 299))
    return TRUE;
  return FALSE;
}

/* ----------------------------------------------------------------- ** 
** repair_cmd_line	                                             ** 
** ----------------------------------------------------------------- */ 
void repair_cmd_line(the_cmd_line)
char	the_cmd_line[200];
{
  char	rep_cmd[200];
  char	cmd_field[200];
  char	*cmd_line;
  int	cmd_lng;
  boolean	last_sequence;
  boolean	not_sequence;
  boolean	sep_error;
  char	sep_cmd[200];
  int	sep_lng;
  boolean sep_rept;
  int	nbchar;
  char	*ptr;
  
  cmd_line = the_cmd_line;
  cmd_lng = strlen(cmd_line);
  trim_before(cmd_line, &cmd_lng);
  trim_after(cmd_line, &cmd_lng);
  
  ptr = rep_cmd;
  last_sequence = FALSE;
  while(!last_sequence)
  {
    last_sequence = get_sequence(cmd_line, cmd_field, &cmd_lng);
    Xc_TRACE(("sequence: cmd: %s", cmd_field));
    
    if (cmd_field[0] == '!')
    {
      not_sequence = TRUE;
      memmove(cmd_field, cmd_field + 1, strlen(cmd_field) + 1);
      Xc_TRACE(("not sequence"));
    }
    else
      not_sequence = FALSE;
    
    if (cmd_field[0] == '"')
    {
      sep_error = get_separator_cmd(cmd_field, sep_cmd, &sep_lng, &sep_rept);
      if (!sep_error)
      {
	memcpy(ptr, cmd_line, cmd_lng);
	ptr += cmd_lng;
      }
      else
      {
	Xc_TRACE(("separator cmd error"));
      }
    }
    else
    {
      if (cmd_field[0] == '\0')
      {
	if (ptr == rep_cmd)
	{
	  Xc_TRACE(("; in first position not allowed"));
	}
	else
	{
	  if (!last_sequence)
	    {
	      *ptr = ';';
	      ptr ++;
	    }
	}
      }
      else
      {
	sep_error = get_separator_nbchar(cmd_field, &nbchar);
	if (!sep_error)
	{
	  memcpy(ptr, cmd_line, cmd_lng);
	  ptr += cmd_lng;
	}
	else
	{
	  Xc_TRACE(("separator nbchar error"));	
	}
      }
    }
    cmd_line += cmd_lng;
  }
  
  *ptr = '\0';
  memcpy(the_cmd_line, rep_cmd, strlen(rep_cmd)+1);
}



