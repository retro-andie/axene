/*
** Export_Text_HTML.c for Xclamation in Advanced_Editor/
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
** Started on  Mon Jul 17 13:52:07 1995 Marc Blet
** Last update Thu Oct 24 18:08:56 1996 Emmanuel Paris
*/

#include "ExportTextFileSelector.h"
#include "Advanced_Editor.h"
#include "TextRuler.h"
#include "TextStyle.h"
#include "file.h"

export_text_t	export_text = {
  { XcE_LATIN1}, {TRUE, FALSE, FALSE, 0, NULL, NULL, NULL,
		    (char *)-1, (char *)-1, 0}
};

static void calcul_style_html ___PROTO((c_TextStyle *tstyle, char **begin,
					char **end));

void initialize_export_text(tstyle)
c_TextStyle	*tstyle;
{
  c_TextStyle	*style;
  expt_HTML_t	*export;
  char		**ostyle_name;
  char		**ostyle_begin;
  char		**ostyle_end;
  int		onb_style;
  int		nb_style;
  int		i, j;
  
  export = &export_text.expt_HTML;
  
  if (export->frame_break == (char *)-1)
  {
    export->frame_break = Xc_strdup("fb", "<HR>");
  }
  if (export->carriage_return == (char *)-1)
  {
    export->carriage_return = Xc_strdup("cr", "<BR>");
  }
  
  onb_style = export->nb_style;
  ostyle_name = export->style_name;
  ostyle_begin = export->style_begin;
  ostyle_end = export->style_end;
  
  nb_style = 0;
  style = tstyle;
  while(style)
  {
    nb_style++;
    style = style->next;
  }
  
  export->nb_style = nb_style;
  export->style_name = (char **)Xc_malloc("sn", sizeof(char *) * nb_style);
  export->style_begin = (char **)Xc_malloc("sb", sizeof(char *) * nb_style);
  export->style_end = (char **)Xc_malloc("se", sizeof(char *) * nb_style);
  
  i = 0;
  style = tstyle;
  while(style)
  {
    j = 0;
    while((j < onb_style) && !Xstrcmp(style->name, ostyle_name[j]))
      j++;
    
    if (j != onb_style)
    {
      export->style_name[i] = ostyle_name[j];
      export->style_begin[i] = ostyle_begin[j];
      export->style_end[i] = ostyle_end[j];
      
      ostyle_name[j] = ostyle_begin[j] = ostyle_end[j] = NULL;
    }
    else
    {
      export->style_name[i] = Xc_strdup("*sn", style->name);
      
      calcul_style_html(style, &(export->style_begin[i]), 
			&(export->style_end[i]));
    }
    style = style->next;
    i++;
  }
  
  for(i = 0; i < onb_style; i++)
  {
    if (ostyle_name[i])   Xc_free(ostyle_name);
    if (ostyle_begin[i])  Xc_free(ostyle_begin);
    if (ostyle_end[i])    Xc_free(ostyle_end);    
  }
  if (onb_style)
  {
    Xc_free(ostyle_name);
    Xc_free(ostyle_begin);
    Xc_free(ostyle_end);
  }  
}

static void calcul_style_html(tstyle, sbegin, send)
c_TextStyle *tstyle;
char **sbegin;
char **send;
{
  c_VectorFont	*vfont;
  char	*bold_b, *bold_e;
  char	*italic_b, *italic_e;
  char	*teletype_b, *teletype_e;
  char	*underline_b, *underline_e;
  char	*font_size_b, *font_size_e;
  char	st_begin[60], st_end[60];
  
  vfont = tstyle->font;
  
  bold_b = bold_e = italic_b = italic_e = teletype_b = teletype_e =
    underline_b = underline_e = font_size_b = font_size_e = "\0";
  
  if (Xstrstr(vfont->font_name, "ourier"))
    {
      teletype_b = "<TT>";
      teletype_e = "</TT>";
    }
  
  if (vfont->specific->is_bold || vfont->specific->is_bold_italic)
    {
      bold_b = "<B>";
      bold_e = "</B>";
    }
  
  if (vfont->specific->is_italic || vfont->specific->is_bold_italic)
    {
      italic_b = "<I>";
      italic_e = "</I>";
    }
  
  if (tstyle->attributes & XcTS_B_UNDERLINE)
  {
    if (tstyle->underline.dy > SCALE_FROM_COEF(0.3))
      {
	underline_b = "<S>";
	underline_e = "</S>";
      }
    else
      {
	underline_b = "<U>";
	underline_e = "</U>";
      }
  }
  
  if (tstyle->point_size >= SCALE_FROM_POINTS(16.0))
    {
      font_size_b = "<BIG>";
      font_size_e = "</BIG>"; 
    }
  else
    {
      if (tstyle->point_size <= SCALE_FROM_POINTS(8.0))
	{
	  font_size_b = "<SMALL>";
	  font_size_e = "</SMALL>"; 
	}
    }
  
  sprintf(st_begin, "%s%s%s%s%s", bold_b, italic_b, teletype_b,
	  underline_b, font_size_b);
  sprintf(st_end, "%s%s%s%s%s", bold_e, italic_e, teletype_e,
	  underline_e, font_size_e);
  
  *sbegin =  Xc_strdup("*sb", st_begin);
  *send =  Xc_strdup("*se", st_end);
}


/* ----------------------------------------------------------------- ** 
** generate ASCII                                                    ** 
** ----------------------------------------------------------------- */

void 	generate_ascii(This, file)
c_AEditor	*This;
FILE		*file;
{
 XcAE_Escape_List	*List;
 XcAE_Escape_List	*FoundL;
 int			Length;
 XmTextPosition		The_Pos, BestPos;
 boolean		Paragraph;
 
 Length = The_Pos = 0; 
 /* ---- Algo pour regenerer le texte : faire un insert string de ce --- */
 /* ---- qu'il y a entre deux references, inserer la reference, etc -- */ 
 
 List = This->List;
 while(List->Next != NULL)
 {
   List->Flag = FALSE;
   List = List->Next;
 }
 
 do
   {
     Paragraph = TRUE;
     FoundL = NULL;
     BestPos = XcAE_MAX_TEXT_LENGTH;
     List = This->List;
     while(List->Next != NULL)
     {
       if (List->Flag == FALSE)
       {
	 if ((List->StartPos < BestPos))
	 {
	   BestPos = List->StartPos;
	   FoundL = List;
	 }
       }
       List = List->Next;
     }
     if ((FoundL != NULL) && (The_Pos == FoundL->StartPos))
       FoundL->Flag = TRUE;
     
     if ((FoundL == NULL) || (The_Pos != FoundL->StartPos))
     {
	 /* cas du texte sans style gere ... */
	 if (FoundL == NULL)
	   Length = This->Text_Length - The_Pos;
	 else
	   Length = FoundL->StartPos - The_Pos;
	 
	 fwrite(This->W_Text + The_Pos, sizeof(char), Length, file);
	 The_Pos += Length;
       }
     else
       {
	 Length = FoundL->EndPos - FoundL->StartPos;
	 switch (FoundL->Type)
	   {
	   case XcAE_RULER :
	     if (The_Pos != 0)
	       fwrite("\n", sizeof(char), 1, file);
	     break;
	   case XcAE_STYLE :
	     break;
	   case XcAE_SPECIAL:
	     switch(FoundL->Data.Special)
	       {
	       case XcAE_SPEC_CR:
		 fwrite("\n", sizeof(char), 1, file);
		 break;
	       case XcAE_SPEC_NC:
		 break;
	       case XcAE_SPEC_NP:
		 fwrite("\n", sizeof(char), 1, file);
		 Paragraph = FALSE;
		 break;
	       }
	     break;
	   default : 
	     Xc_WARNING(("Unknown style..."));
	     The_Pos -= Length;	/* Pour compenser l'ajout suivant */
	   }
	 The_Pos += Length;
       }
   }
 while (The_Pos < This->Text_Length);
 
 if (Paragraph)
   fwrite("\n", sizeof(char), 1, file);
}


/* ----------------------------------------------------------------- ** 
** generate HTML                                                     ** 
** ----------------------------------------------------------------- */

void 	generate_html(This, file, filename)
c_AEditor	*This;
FILE		*file;
char		*filename;
{
  char			*text = NULL, *textc;
  XcAE_Escape_List	*List;
  XcAE_Escape_List	*FoundL;
  int			Length, tlng = 0, i;
  XmTextPosition	The_Pos, BestPos;
  expt_HTML_t		*export;
  boolean		Paragraph;
  char			*prev_ruler = NULL;
  char			*prev_style = NULL;
  c_TextStyle		*tstyle, *tstyle_base;
  
  fwrite("<HTML>\n<head>\n<title>", 21, 1, file);
  fwrite(GetBasename(filename, FALSE), strlen(GetBasename(filename, FALSE)),
	 1, file);
  fwrite("</title>\n</head>\n<body>\n", 24, 1, file);
  
  export = &export_text.expt_HTML;
  Length = The_Pos = 0; 
  tstyle_base = This->Page->BaseStd->text_style_base;
  /* ---- Algo pour regenerer le texte : faire un insert string de ce --- */
  /* ---- qu'il y a entre deux references, inserer la reference, etc -- */ 
  
  List = This->List;
  while(List->Next != NULL)
  {
    List->Flag = FALSE;
    List = List->Next;
  }

  switch(This->FirstDefaultRuler->justification)
    {
    case XcTR_LEFT:
      prev_ruler = "\n<P>";
      break;
    case XcTR_RIGHT:
      fwrite("<right>", 7, 1, file);
      prev_ruler = "\n<P align=right>";
      break;
    case XcTR_CENTER:
      fwrite("<center>", 8, 1, file);
      prev_ruler = "\n<P align=center>";
      break;
    case XcTR_JUSTIFIED:
      fwrite("<justify>", 9, 1, file);
      prev_ruler = "\n<P align=justify>";
      break;
    }
  i = 0;
  tstyle = tstyle_base;
  while(tstyle && tstyle != This->FirstDefaultStyle)
    {
      i++;
      tstyle = tstyle->next;
    }
  
  if (tstyle && i < export->nb_style)
    {
      fwrite(export->style_begin[i], strlen(export->style_begin[i]),
	     1, file);
      prev_style = export->style_end[i];
    }
  else
    {
      Xc_ERROR(("Bad Style in editor list."));
    }
  do
    {
      Paragraph = TRUE;
      FoundL = NULL;
      BestPos = XcAE_MAX_TEXT_LENGTH;
      List = This->List;
      while(List->Next != NULL)
	{
	  if (List->Flag == FALSE)
	    {
	      if ((List->StartPos < BestPos))
		{
		  BestPos = List->StartPos;
		  FoundL = List;
		}
	    }
	  List = List->Next;
	}
      if ((FoundL != NULL) && (The_Pos == FoundL->StartPos))
	FoundL->Flag = TRUE;
      
      if ((FoundL == NULL) || (The_Pos != FoundL->StartPos))
	{
	  /* cas du texte sans style gere ... */
	  if (FoundL == NULL)
	    Length = This->Text_Length - The_Pos;
	  else
	    Length = FoundL->StartPos - The_Pos;
	  
	  if (tlng < Length + 1)
	    {
	      if (tlng)
		text = Xc_realloc("text", text, Length + 1);
	      else
		text = Xc_malloc("text", Length + 1);
	      tlng = Length + 1;
	    }
	  memcpy(text, This->W_Text + The_Pos, Length);
	  text[Length] = '\0';
	  
	  textc = convert_to_encoding(text, XcE_HTML);
	  fwrite(textc, strlen(textc), 1, file);
	  Xc_free(textc);
	  
	  The_Pos += Length;
	}
      else
	{
	  Length = FoundL->EndPos - FoundL->StartPos;
	  switch (FoundL->Type)
	    {
	    case XcAE_RULER :
	      if (export->rulers)
		{
		  if (FoundL->Data.Ruler->justification == XcTR_LEFT)
		    {
		      if (The_Pos != 0)
			fwrite("\n<P>", 4, 1, file);
		      prev_ruler = "<P>";
		    }
		  else
		    {
		      if (The_Pos != 0)
			fwrite("\n<P align=", 10, 1, file);
		      else
			fwrite("<", 1, 1, file);
		      
		      switch(FoundL->Data.Ruler->justification)
			{
			case XcTR_RIGHT:
			  fwrite("right", 5, 1, file);
			  prev_ruler = "\n<P align=right>";
			  break;
			case XcTR_CENTER:
			  fwrite("center", 6, 1, file);
			  prev_ruler = "\n<P align=center>";
			  break;
			case XcTR_JUSTIFIED:
			  fwrite("justify", 7, 1, file);
			  prev_ruler = "\n<P align=justify>";
			case XcTR_LEFT:
			  break;
			}
		      fwrite(">", 1, 1, file);
		    }
		}
	      else
		{
		  if (The_Pos != 0)
		    {
		      fwrite("\n<P>", 4, 1, file);
		    }
		}
	      break;
	    case XcAE_STYLE :
	      
	      fwrite(prev_style, strlen(prev_style), 1, file);
	      i = 0;
	      tstyle = tstyle_base;
	      while(tstyle && tstyle != FoundL->Data.Style)
		{
		  i++;
		  tstyle = tstyle->next;
		}
	      
	      if (tstyle && i < export->nb_style)
		{
		  fwrite(export->style_begin[i],
			 strlen(export->style_begin[i]), 1, file);
		  prev_style = export->style_end[i];
		}
	      else
		{
		  Xc_ERROR(("Bad Style in editor list."));
		}
	      break;
	    case XcAE_SPECIAL:
	      switch(FoundL->Data.Special)
		{
		case XcAE_SPEC_CR:
		  fwrite(export->carriage_return,
			 strlen(export->carriage_return), 1, file);
		  break;
		case XcAE_SPEC_NC:
		  fwrite(export->frame_break, strlen(export->frame_break),
			 1, file);
		  break;
		case XcAE_SPEC_NP:
		  if (export->rulers)
		    fwrite(prev_ruler, strlen(prev_ruler), 1, file);
		  else
		    fwrite("\n<P>", 4, 1, file);
		  Paragraph = FALSE;
		  break;
		}
	      break;
	    default : 
	      Xc_WARNING(("Unknown style..."));
	      The_Pos -= Length;	/* Pour compenser l'ajout suivant */
	    }
	  The_Pos += Length;
	}
    }
  while (The_Pos < This->Text_Length);
  
  if (Paragraph)
    fwrite("\n", sizeof(char), 1, file);
  
  fwrite("\n</body>\n</HTML>\n", 17, 1, file);

  if (tlng)
    Xc_free(text);
}

