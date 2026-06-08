/*
** Import_Text.c for XAllWrite in Import/
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
** Last update Fri Aug 14 19:18:43 1998 Emmanuel Paris
*/

/*#define NTRACE*/

#include "Import_Text.h"
#include "char_translate.h"

extern c_VectorFont *GlobFontBase;

static void *cons ___PROTO((int style));
static void dest ___PROTO((c_Import_Text *This));
static void *copy ___PROTO((c_Import_Text *This));

static boolean test ___PROTO((c_Import_Text *This));
static boolean create_new_document ___PROTO((c_Import_Text *This));
static error load ___PROTO((c_Import_Text * This, c_Document *document));

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
 create_new_document, 
 load
};

static 	char *Extensions_Text[] = { "txt" };

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
  This->name = "Text";
  This->copyright = "Copyright Axene Software 1998";
  This->id =  
   F(GlobResources).getString(GlobResources, XcR_importDocText);
  This->extension_count = 1;
  This->extensions = Extensions_Text;
  This->suppress_newline = FALSE;
  This->encoding = XcE_LATIN1;
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

static boolean create_new_document(This)
c_Import_Text *This; 
{
 return This->new_doc;
}


static error load(This, document)
c_Import_Text * This;
c_Document *document;
{
 FILE	*file;
 long	taille;
 char	bline[201], *ptr;
 c_Text	*text;
 int	i, len;

 if (!(file = fopen(This->filename,"rb")))
 {
  Xc_WARNING(("file %s doesn't exist or unreadable", This->filename));
  return XC_FILE_NOT_EXIST;
 }
 fseek(file, 0L, SEEK_END);
 taille = ftell(file);
 fseek(file, 0L, SEEK_SET);
 
 text = document->BaseStd.text_base;
 if (This->new_doc)
 {
  c_TextStyle *style;
  c_TextRuler *ruler;
  c_VectorFont *font;
  
  ruler = F(text->ruler).get(text->ruler, text->BaseStd, XcTR_NAME, NULL,
			     XcTR_HIDE_ON, XcTR_INDENT, COORD_ZERO,
			     XcTR_INHERIT, text->ruler, 
			     (unsigned int)XcTR_H_INDENT, XcTR_END);
  F(text->ruler).markUnused(text->ruler);
  text->item_base->linefeed.ruler = text->ruler = ruler;
  F(ruler).markUsed(ruler);
  
  if ((font = F(GlobFontBase).getFontByName(GlobFontBase, "Courier")) != NULL)
  {
   style = F(text->style).get(text->style, text->BaseStd, XcTS_NAME, NULL,
			      XcTS_HIDE_ON, XcTS_FONT, font, 
			      XcTS_INHERIT, text->style, 
			      (unsigned int)XcTS_H_FONT,
			      XcTS_END);
   F(text->style).markUnused(text->style);
   F(text->style).markUnused(text->style);
   text->item_base->any.style = text->item_base->any.next->any.style = 
    text->style = style;
   F(style).markUsed(style);
   F(style).markUsed(style);
   
   if (!This->suppress_newline)
    document->PageArea->PageList->left_margin = 
     document->PageArea->PageList->right_margin = 
      SCALE_FROM_CENTIMETERS(1.5);
  }
 }
 memset(bline, 0, 201);
 
 while(fgets(bline, 200, file))
 {
 next_line:
  
  ptr = bline;
  len = MIN(strlen(bline), 200);
  for(i = 0; i < len; i++)
  {
   if ((unsigned char)bline[i] < (unsigned char)' ')
   {
    switch(bline[i])
    {
    case '\n':
    case '\r':
     bline[i] = '\0';
     if (ptr != (bline + i))
      F(text).insert(text, XA_TEXT, ptr);
     ptr = bline + i + 1;
     if (This->suppress_newline)
     {
      do
      {
       boolean need_space = FALSE;
       
       if (i == 0) break;
       if (bline[i-1] == ':') break;
       if (bline[i-1] != ' ') need_space = TRUE;
       
       if (!fgets(bline, 200, file)) goto end_import;
       
       if (*bline == ' ') need_space = FALSE;
       
       if (Xc_IS_PUNCTUATION(*bline) || Xc_IS_OPERATOR(*bline) ||
	   Xc_IS_DIGIT(*bline) || (unsigned char)*bline < 32)
       {
	F(text).insert(text, XA_LINEFEED, ptr);
       }
       else if (need_space)
	F(text).insert(text, XA_TEXT, " ");
       
       goto next_line;
      } while(0);
     }
     F(text).insert(text, XA_LINEFEED, ptr);
     if (bline[i+1] == '\r' || bline[i+1] == '\n') i = 300;
     break;
    case '\t':
     bline[i] = '\0';
     if (ptr != (bline + i))
      F(text).insert(text, XA_TEXT, ptr);
     ptr = bline + i + 1;
     F(text).insert(text, XA_TAB, ptr);
     break;
    default:
     bline[i] = '\0';
     if (ptr != (bline + i))
      F(text).insert(text, XA_TEXT, ptr);
     ptr = bline + i + 1;
    }
   }
  }

  if (i == len && ptr != (bline + i))
  {
   bline[i] = '\0';
   F(text).insert(text, XA_TEXT, ptr);
  }

 }
 end_import: 

 fclose(file);
 return XC_NO_ERROR;
}


