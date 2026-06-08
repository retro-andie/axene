/*
** Export_Text.c for XQuad in Export/
** Export_Text Object
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
** Started on  Sun Dec 15 17:30:00 1996 Robin Castanier
** Last update Mon Mar 23 14:51:34 1998 Emmanuel Paris
*/

/*#define NTRACE*/

#include "Export_Text.h"

static void *cons ___PROTO((int style));
static void dest ___PROTO((c_Export_Text *This));
static void *copy ___PROTO((c_Export_Text *This));

static error save ___PROTO((c_Export_Text *This, c_Tableau *sheet));
static void treat_sep ___PROTO((char *cmd_field, char *sep_cmd));

static boolean			box_created = FALSE;
static export_text_box_t	box;

sf_Export_Std fc_Export_Text = 
{
 cons, 
 dest, 
 copy,
 export_txt_create_attribs_box, 
 export_txt_manage_attribs_box, 
 export_txt_destroy_attribs_box,
 save
};

static 	char *Extensions_TabTxt[] = { "txt" };
static 	char *Extensions_TabCsv[] = { "csv" };

static void *cons(style)
int style;
{
 c_Export_Text	*This;
    
 if ((This = (c_Export_Text *)Xc_malloc("Export Text", 
					sizeof(c_Export_Text))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f = &fc_Export_Text;

 switch(style)
 {
 case 0:
  This->name = "Text Unix, separator tab";
  This->copyright = "Copyright Axene 1995";
  This->id = 
   F(GlobResources).getString(GlobResources, XcR_exportDocTextTabUnix);
  This->extension_count = 1;
  This->extensions = Extensions_TabTxt;
  strcpy(This->sep_cmd, "\\t");
  This->encoding = XcE_LATIN1;
  break;
 case 1:
  This->name = "Text Unix, separator ;";
  This->copyright = "Copyright Axene 1995";
  This->id = 
   F(GlobResources).getString(GlobResources, XcR_exportDocTextCSVUnix);
  This->extension_count = 1;
  This->extensions = Extensions_TabCsv;
  strcpy(This->sep_cmd, ";");
  This->encoding = XcE_LATIN1;
  break;
 case 2:
  This->name = "Text OS2 Dos, separator tab";
  This->copyright = "Copyright Axene 1995";
  This->id = 
   F(GlobResources).getString(GlobResources, XcR_exportDocTextTabDos);
  This->extension_count = 1;
  This->extensions = Extensions_TabTxt;
  strcpy(This->sep_cmd, "\\t");
  This->encoding = XcE_DOS;
  break;
 case 3:
  This->name = "Text OS2 Dos, separator ;";
  This->copyright = "Copyright Axene 1995";
  This->id = 
   F(GlobResources).getString(GlobResources, XcR_exportDocTextCSVDos);
  This->extension_count = 1;
  This->extensions = Extensions_TabCsv;
  strcpy(This->sep_cmd, ";");
  This->encoding = XcE_DOS;
  break;
 case 4:
  This->name = "Text Mac, separator tab";
  This->copyright = "Copyright Axene 1995";
  This->id = 
   F(GlobResources).getString(GlobResources, XcR_exportDocTextTabMac);
  This->extension_count = 1;
  This->extensions = Extensions_TabTxt;
  strcpy(This->sep_cmd, "\\t");
  This->encoding = XcE_MAC;
  break;
 case 5:
  This->name = "Text Mac, separator ;";
  This->copyright = "Copyright Axene 1995";
  This->id = 
   F(GlobResources).getString(GlobResources, XcR_exportDocTextCSVMac);
  This->extension_count = 1;
  This->extensions = Extensions_TabCsv;
  strcpy(This->sep_cmd, ";");
  This->encoding = XcE_MAC;
  break;
 }
  
 This->filename = NULL;
 This->box_created = &box_created;
  
 This->box = &box;
 This->box->managed = FALSE;
 This->formula = TRUE;
 This->selection = FALSE;
 return This;
}

static void dest(This)
c_Export_Text *This;
{
 Xc_free(This);
}

static void *copy(This)
c_Export_Text *This;
{
 Xc_TRACE(("COPY Export_Text Object not done: normally not usefull."));
 return NULL;
}

extern cell voidcell;

static error save(This, sheet)
c_Export_Text *This;
c_Tableau *sheet;

{
 FILE		*file;
 c_Moteur	*moteur;
 region_t	region;
 long int	rx;
 long int	ry;
 char		str[Xq_FORMULA_LENGTH];
 char		sep[30];
 int		type;
 cell		*tmp_cell;
  
 if (This->formula)
  type = Xq_FORMULA;
 else
  type = Xq_VALUE;

 if (!(file = fopen(This->filename,"w+")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable", This->filename));
  return XC_FILE_NOT_EXIST;
 }

 treat_sep(This->sep_cmd, sep);
 moteur = sheet->moteur;
 if (This->selection)
 {
  F(moteur).getBoundingRegion(moteur, &region);
  if (sheet->Feuille->cell_region->x_max != COLUMN_MAX)
  {
   region.x_min=sheet->Feuille->cell_region->x_min;
   region.x_max=sheet->Feuille->cell_region->x_max;
  }
  if (sheet->Feuille->cell_region->y_max != ROW_MAX)
  {
   region.y_min=sheet->Feuille->cell_region->y_min;
   region.y_max=sheet->Feuille->cell_region->y_max;
  }
 }
 else
 {
  F(moteur).getBoundingRegion(moteur, &region);
  /* clip region */
 }

 tmp_cell = NULL;
 ry = region.y_min;
 while (ry <= region.y_max)
 {
  rx = region.x_min;
  while (rx <= region.x_max)
  {
   tmp_cell = F(moteur).get_cell(moteur, rx, ry);
   rx++;
   if (tmp_cell != &voidcell)
   {
    F(moteur).get_string(moteur, tmp_cell, str, type);
    /* translate */
    convert_to_encoding(str, This->encoding);
    fputs(str, file);
   }
   fputs(sep, file);
  }
  ry++;
  if (This->encoding == XcE_DOS)
   fputc('\r', file);
  fputc('\n', file);
 }
 fclose(file);
 return XC_NO_ERROR;
}


static void treat_sep(cmd_field, sep_cmd)
char *cmd_field;
char *sep_cmd;
{
 int	i;
 int	j;
 int	dt;
 char	c;

 i=0;
 j=0;
 while(cmd_field[i])
 {
  if (cmd_field[i] == '\\')
  {
   c = cmd_field[++i];
   switch(c)
   {
   case '\0':
    i--;			/* with the i++; at then end, can exit normally */
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
}
