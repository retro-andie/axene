/*
** Export_FText.c for XQuad in Export/
** Export_FText Object
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
** Last update Fri Oct  2 17:02:51 1998 Robin Castanier
*/

#include "Export_FText.h"

static void *cons ___PROTO((int style));
static void dest ___PROTO((c_Export_FText *This));
static void *copy ___PROTO((c_Export_FText *This));

static error save ___PROTO((c_Export_FText *This, c_Tableau *sheet));
static void crunch_string ___PROTO((char *value,
				    int value_type,
				    conv_info_t	*conv_info,
				    int sWidth,
				    char *svg_value,
				    CellStyle_t	*style));

static boolean			box_created = FALSE;
static Export_FText_box_t	box;

sf_Export_Std fc_Export_FText = 
{
 cons, 
 dest, 
 copy,
 export_ftxt_create_attribs_box, 
 export_ftxt_manage_attribs_box, 
 export_ftxt_destroy_attribs_box,
 save
};

#define Gr_HORIZ	0
#define Gr_VERT		1
#define Gr_CROSS	2
#define Gr_TOPL		3
#define Gr_TOPR		4
#define Gr_TOPLR	5
#define Gr_BOTL		6
#define Gr_BOTR		7
#define Gr_BOTLR	8
/* right top bottom */
#define Gr_RIGTB	9  
/* left top bottom */
#define Gr_LEFTB	10

char graph[3][11] = 
{
 {
  '-',
  '|',
  '+',
  '|',
  '|',
  '-',
  '|',
  '|',
  '-',
  '|',
  '|'
 },
  {
   0xC4,
   0xB3,
   0xC5,
   0xDA,
   0xBF,
   0xC2,
   0xC0,
   0xD9,
   0xC1,
   0xB4,
   0xC3
  },
   {
    '|',
    '-',
    '+',
    '|',
    '|',
    '|',
    '|',
    '|',
    '|',
    '|',
    '|'
   }
};

static 	char *Extensions_TabPrn[] = { "prn" };

static void *cons(style)
int style;
{
 c_Export_FText	*This;
    
 if ((This = (c_Export_FText *)Xc_malloc("Export Text", 
					 sizeof(c_Export_FText))) == NULL)
 {
  Xc_FATAL(("Can't build this object: memory allocation error"));
 }
 This->f = &fc_Export_FText;

 switch(style)
 {
 case 0:
  This->name = "Text Unix, separator space";
  This->copyright = "Copyright Axene 1995";
  This->id =  
   F(GlobResources).getString(GlobResources, XcR_exportDocTextSpaceUnix);
  This->extension_count = 1;
  This->extensions = Extensions_TabPrn;
  This->encoding = XcE_LATIN1;
  break;
 case 1:
  This->name = "Text OS2 Dos, separator space";
  This->copyright = "Copyright Axene 1995";
  This->id =  
   F(GlobResources).getString(GlobResources, XcR_exportDocTextSpaceDos);
  This->extension_count = 1;
  This->extensions = Extensions_TabPrn;
  This->encoding = XcE_DOS;
  break;
 case 2:
  This->name = "Text Mac, separator space";
  This->copyright = "Copyright Axene 1995";
  This->id =  
   F(GlobResources).getString(GlobResources, XcR_exportDocTextSpaceMac);
  This->extension_count = 1;
  This->extensions = Extensions_TabPrn;
  This->encoding = XcE_MAC;
  break;
 }
  
 This->filename = NULL;
 This->box_created = &box_created;
  
 This->box = &box;
 This->box->managed = FALSE;
 This->formula = FALSE;
 This->selection = FALSE;
 This->grid = TRUE;
 This->headers = TRUE;
 return This;
}

static void dest(This)
c_Export_FText *This;
{
 Xc_free(This);
}

static void *copy(This)
c_Export_FText *This;
{
 Xc_TRACE(("COPY Export_FText Object not done: normally not usefull."));
 return NULL;
}

extern cell voidcell;

static error save(This, sheet)
c_Export_FText *This;
c_Tableau *sheet;
{
 FILE		*file;
 c_Moteur	*moteur;
 region_t	region;
 long int	rx;
 long int	ry;
 char		str[Xq_FORMULA_LENGTH];
 char		svg_value[Xq_FORMULA_LENGTH];
 int		type;
 cell		*tmp_cell;
 CellStyle_t	*style;
 int		nbrchar;
 conv_info_t	conv_info;
 int		value_type;
 int		lcar;
 int		i;
 int		j;
 int		k;
 char		*x_cell;
 int		*x_size;
 boolean	border;
 boolean	header;
 int		i_droite;
 int		i_gauche;
 char		head_str[10];
 
 border = This->grid;
 header = This->headers;
 if (This->formula)
  type = Xq_FORMULA;
 else
  type = Xq_VALUE;

 if (!(file = fopen(This->filename,"w+")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable", This->filename));
  return XC_FILE_NOT_EXIST;
 }

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

 x_size=(int *)Xc_malloc("size_list", 
			 (region.y_max-region.y_min)+1*sizeof(int));
 x_cell=(char *)Xc_malloc("size_list", 
			  (region.y_max-region.y_min)+1);


 rx = region.x_min;
 while(rx <= region.x_max)
 {
  x_size[rx-region.x_min] = F(moteur).get_size(moteur,
					       Xc_COLUMN,
					       rx)/6432;
  /* 9 */
  /* 7236 - 8 */
  rx++;
 }
 /* skip ligne header */
 if (header)
 {
  if (border)
  {
   i = 6;
   while (i)
   {
    fputc(' ', file);
    i--;
   }
   fputc(graph[This->encoding][Gr_TOPL], file);
   rx = region.x_min;
   while(rx < region.x_max)
   {
    i = x_size[rx-region.x_min];
    while(i)
    {
     fputc(graph[This->encoding][Gr_HORIZ], file);
     i--;
    }
    fputc(graph[This->encoding][Gr_TOPLR], file);
    rx++;
   }
   i = x_size[rx-region.x_min];
   while(i)
   {
    fputc(graph[This->encoding][Gr_HORIZ], file);
    i--;
   }
   fputc(graph[This->encoding][Gr_TOPR], file);
   if (This->encoding == XcE_DOS)
    fputc('\r', file);
   fputc('\n', file);
  }
  i = 5+border;
  while (i)
  {
   fputc(' ', file);
   i--;
  }
  if (border)
   fputc(graph[This->encoding][Gr_VERT], file);
 }
 rx = region.x_min;
 while(rx <= region.x_max)
 {
  /* if header */
  if (header)
  {
   nbrchar = x_size[rx-region.x_min];
   if (nbrchar != 0)
   {
    coltostr(rx, NULL, head_str);
    i = strlen(head_str);
    if ((int)strlen(head_str) > nbrchar)
    {
     head_str[nbrchar]=0;
     fputs(head_str, file);
    }
    else
    {
     nbrchar -= i;
     j = nbrchar / 2;
     i = nbrchar-j;
     while (j)
     {
      fputc(' ', file);
      j--;
     }
     fputs(head_str, file);		      
     while (i)
     {
      fputc(' ', file);
      i--;
     }
    }
   }
   if (border)
    fputc(graph[This->encoding][Gr_VERT], file);
  }
  rx++;
 }
 if (header)
 {
  if (This->encoding == XcE_DOS)
   fputc('\r', file);
  fputc('\n', file);
 }
 if (border)
 {
  if (header)
  {
   i = 5;
   fputc(graph[This->encoding][Gr_TOPL], file);
   while (i)
   {
    fputc(graph[This->encoding][Gr_HORIZ], file);
    i--;
   }
  }
  if (header)
   fputc(graph[This->encoding][Gr_CROSS], file);
  else
   fputc(graph[This->encoding][Gr_TOPL], file);
  rx = region.x_min;
  while(rx < region.x_max)
  {
   i = x_size[rx-region.x_min];
   while(i)
   {
    fputc(graph[This->encoding][Gr_HORIZ], file);
    i--;
   }
   if (header)
    fputc(graph[This->encoding][Gr_CROSS], file);
   else
    fputc(graph[This->encoding][Gr_TOPLR], file);

   rx++;
  }
  i = x_size[rx-region.x_min];
  while(i)
  {
   fputc(graph[This->encoding][Gr_HORIZ], file);
   i--;
  }
  if (header)
   fputc(graph[This->encoding][Gr_RIGTB], file);
  else
   fputc(graph[This->encoding][Gr_TOPR], file);
  if (This->encoding == XcE_DOS)
   fputc('\r', file);
  fputc('\n', file);
 }

 tmp_cell = NULL;
 ry = region.y_min;
 while (ry <= region.y_max)
 {
  rx = region.x_min;
  if (header)
  {
   if (border)
    fputc(graph[This->encoding][Gr_VERT], file);
   sprintf(head_str, "%ld", ry);
   i = 5-strlen(head_str);
   j = i/2;
   i -= j;
   while (j)
   {
    fputc(' ', file);
    j--;
   }
   fputs(head_str, file);
   while (i)
   {
    fputc(' ', file);
    i--;
   }
  }
  if (border)
   fputc(graph[This->encoding][Gr_VERT], file);
  while (rx <= region.x_max)
  {
   tmp_cell = F(moteur).get_cell(moteur, rx, ry);
   nbrchar = x_size[rx-region.x_min];
   if (tmp_cell != &voidcell)
   {
    x_cell[rx-region.x_min]=1;
    style = F(moteur).get_cell_style(moteur, rx, ry);

    value_type = F(moteur).get_string(moteur, tmp_cell, str, type);
    strcpy(svg_value, str);
    F(style->format).convert_string(style->format, str,
				    value_type,
				    &conv_info);
    lcar = strlen(str);
    if(lcar>nbrchar)
    {
     crunch_string(str, value_type, &conv_info, 
		   nbrchar+1, svg_value, style);
     lcar = strlen(str);
     if (lcar>nbrchar)
     {
      i = lcar - nbrchar;
      switch (style->ruler_type & XqR_H_ALIGN_MASK)
      {
      case 0:
      case 4:
      case 1:
       while ((rx < region.x_max) && (i>0))
       {
	rx++;
	tmp_cell = F(moteur).get_cell(moteur, rx, ry);
	if (tmp_cell == &voidcell)
	 i -= border+nbrchar;
	else
	{
	 rx--;
	 break;
	}
       }
       if (i>0)
       {
	str[lcar-i]=0;
       }
       else
       {
	i=abs(i);
	str[lcar+i]=0;
	i--;
	while (i>=0)
	{
	 str[lcar+i]=' ';
	 i--;
	}
       }
       break;
      case 2:
       j=(rx-region.x_min)-1;
       while ((j>=0) && (x_cell[j]==0) && (i>0))
       {
	j--;
	fseek(file,  -(border+nbrchar), SEEK_CUR);
	i -= border+nbrchar;
       }
       if (i>0)
       {
	j = i;
	i = 0;
	while (j<=lcar)
	{
	 str[i]=str[j];
	 i++;
	 j++;
	}
       }
       else
       {
	i=abs(i);
	str[lcar-i+1]=0;
	j = lcar;
	while (j>=i)
	{
	 str[j]=str[j-i];
	 j--;
	}
	i--;
	while (i>=0)
	{
	 str[i]=' ';
	 i--;
	}
       }
       break;
      case 3:
       i_droite = i/2;
       i_gauche = i-i_droite;

       /* partie de gauche */
       i=i_gauche;
       j=(rx-region.x_min)-1;
       while ((j>=0) && (x_cell[j]==0) && (i>0))
       {
	j--;
	fseek(file,  -(border+nbrchar), SEEK_CUR);
	i -= border+nbrchar;
       }
       if (i>0)
       {
	j = i;
	i = 0;
	while (i<=lcar-j)
	{
	 str[i]=str[i+j];
	 i++;
	}
	lcar-=j;
       }
       else
       {
	i=abs(i);
	lcar += i;
	j = lcar;
	while (j>=i)
	{
	 str[j]=str[j-i];
	 j--;
	}
	i--;
	while (i>=0)
	{
	 str[i]=' ';
	 i--;
	}
       }

       /* partie de droite */
       i = i_droite;
       while ((rx < region.x_max) && (i>0))
       {
	rx++;
	tmp_cell = F(moteur).get_cell(moteur, rx, ry);
	if (tmp_cell == &voidcell)
	 i -= border+nbrchar;
	else
	{
	 rx--;
	 break;
	}
       }
       if (i>0)
       {
	str[lcar-i]=0;
       }
       else
       {
	i=abs(i);
	str[lcar+i]=0;
	i--;
	while (i>=0)
	{
	 str[lcar+i]=' ';
	 i--;
	}
       }
       break;
      }
     }
    }
    else
    {
     /*pad*/
     switch (style->ruler_type & XqR_H_ALIGN_MASK)
     {
     case 0:
     case 4:
      if (Xq_IS_VALUE(value_type))
      {
       i = lcar+1;
       j = nbrchar+1;
       while (i>=0)
       {
	str[j]=str[i];
	i--;
	j--;
       }
       i=0;
       while (i < (nbrchar-lcar))
       {
	str[i++]=' ';
       }
       break;
      }
     case 1:
      while (lcar < nbrchar)
      {
       str[lcar++]=' ';
      }
      str[lcar]=0;
      break;
     case 2:
      i = lcar+1;
      j = nbrchar+1;
      while (i>=0)
      {
       str[j]=str[i];
       i--;
       j--;
      }
      i=0;
      while (i < (nbrchar-lcar))
      {
       str[i++]=' ';
      }
      break;
     case 3:
      i=(nbrchar-lcar)/2;
      j = lcar;
      k = i+lcar;
      while (j>=0)
      {
       str[k]=str[j];
       j--;
       k--;
      }
      j=0;
      while (j < i)
      {
       str[j++]=' ';
      }
      j += lcar;
      while (j < nbrchar)
      {
       str[j++]=' ';
      }
      str[j]=0;
     }
    }
    /* translate */
    convert_to_encoding(str, This->encoding);
    fputs(str, file);
   }
   else
   {
    x_cell[rx-region.x_min]=0;
    i = 0;
    while (i < nbrchar)
    {
     str[i++]=' ';
    }
    str[i]=0;
    fputs(str, file);
   }
   rx++;
   if (border)
    fputc(graph[This->encoding][Gr_VERT], file);
  }
  if (This->encoding == XcE_DOS)
   fputc('\r', file);
  fputc('\n', file);
  if (border)
  {
   if (header)
   {
    i = 5;
    if (ry == region.y_max)
     fputc(graph[This->encoding][Gr_BOTL], file);
    else
     fputc(graph[This->encoding][Gr_LEFTB], file);
    while (i)
    {
     fputc(graph[This->encoding][Gr_HORIZ], file);
     i--;
    }
   }
   if (header)
   {
    if (ry == region.y_max)
     fputc(graph[This->encoding][Gr_BOTLR], file);
    else
     fputc(graph[This->encoding][Gr_CROSS], file);
   }
   else
   {
    if (ry == region.y_max)
     fputc(graph[This->encoding][Gr_BOTL], file);
    else
     fputc(graph[This->encoding][Gr_LEFTB], file);
   }

   rx = region.x_min;
   while(rx < region.x_max)
   {
    i = x_size[rx-region.x_min];
    while(i)
    {
     fputc(graph[This->encoding][Gr_HORIZ], file);
     i--;
    }
    if (ry == region.y_max)
     fputc(graph[This->encoding][Gr_BOTLR], file);
    else
     fputc(graph[This->encoding][Gr_CROSS], file);
    rx++;
   }
   i = x_size[rx-region.x_min];
   while(i)
   {
    fputc(graph[This->encoding][Gr_HORIZ], file);
    i--;
   }
   if (ry == region.y_max)
    fputc(graph[This->encoding][Gr_BOTR], file);
   else
    fputc(graph[This->encoding][Gr_RIGTB], file);
   if (This->encoding == XcE_DOS)
    fputc('\r', file);
   fputc('\n', file);
  }
  ry++;
 }

 fclose(file);
 return XC_NO_ERROR;
}

void crunch_string(value, value_type, conv_info, sWidth, svg_value, style)
char		*value;
int		value_type;
conv_info_t	*conv_info;
int		sWidth;
char		*svg_value;
CellStyle_t	*style;
{
 int		trunc_beg, trunc_end, comma_pos;
 int		string_width;
 int		length;
 int		i;
 int		j;
 int		k;
 boolean	set_diese;
 boolean	comma;
 boolean	carry;
 int		inc_width;

 length = strlen(value);
 set_diese = TRUE;
 if (Xq_IS_VALUE(value_type))
 {
  if (conv_info->truncable)
  {
   comma_pos = conv_info->comma_pos;
   trunc_beg = conv_info->trunc_begin;
   trunc_end = conv_info->trunc_end;
   string_width = trunc_beg+(length-trunc_end);
   if (string_width < sWidth)
   {
    i = trunc_beg;
    inc_width = 0; 
    while( (inc_width + string_width) < sWidth && i < trunc_end)
    {
     inc_width++;
     i++;
    }

    if ( ((--i) == (trunc_beg + 1)) && (trunc_beg == comma_pos)) 
     i--;
    if ( (i == trunc_beg) && (trunc_beg == comma_pos))
     j = i+1;
    else
     j = i;
    if (value[j] >= '5')
    {
     k = 0;
     while(svg_value[k] != '.')
      k++;
     k += i - 1 - comma_pos;
     svg_value[k+1] = '\0';
     comma = FALSE;
     carry = TRUE;
     do
     {
      if (svg_value[k] == '.')
      {
       k--;
       comma = TRUE;
       continue;
      }
      if (svg_value[k] != '9')
      {
       if (svg_value[k]>='0' && svg_value[k]<='8')
       {
	svg_value[k] += 1;
	carry = FALSE;
       }
       else
       {
	k++;
	carry = TRUE;
	break;
       }
      }
      else
      {
       svg_value[k] = '0';
       carry = TRUE;
       if (k==0) 
	break;
       k--;
      }
     }
     while(carry);
     if (carry)
     {
      memmove(svg_value + k + 1, svg_value + k, 
	      strlen(svg_value)+1);
      svg_value[k] = '1';
     }
     memcpy(value, svg_value, strlen(svg_value) + 1);
     if (comma)
     {
      k = strlen(value)-1;
      while(value[k] == '0')
       k--;
      if (value[k]=='.')
       k--;
      value[k+1]='\0';
     }

     Xc_TRACE(("before the second convertion: %s", value));
     F(style->format).
      convert_string(style->format, value, Xq_VALUE, NULL);
     length = strlen(value);
     if (length < sWidth)
     {
      set_diese = FALSE;
      return;
     }
    }
    else
    {
     k = --trunc_beg;
     while(k < i && (value[k] < '1' || value[k] > '9'))
      k++;

     if (k < i)
     {
      if (length - trunc_end + 1)
       memmove(value + i, value + trunc_end, 
	       length - trunc_end + 1);
     }
     else
     {
      if (trunc_beg && value[trunc_beg-1] =='-')
      {
       value[trunc_beg-1] = '0';
       if (length - trunc_end + 1)
	memcpy(value + trunc_beg, value + trunc_end, 
	       length - trunc_end + 1);
      }
      else
      {
       if (length - trunc_end + 1)
	memcpy(value + 1 + trunc_beg, value + trunc_end, 
	       length - trunc_end + 1);		
      }
     }
     Xc_TRACE(("value truncated: result: %s", value));
	    
     length = strlen(value);
     set_diese = FALSE;
     return;
    }
   }
  }
 }

 if (Xq_IS_VALUE(value_type) || value_type == Xq_FORMULA)
 {
  if (set_diese)
  {
   for(i=0; i<sWidth-1; i++)
    value[i]='#';
   value[i]='\0';
  }
 }
}
