/*
** HTMLtable.c for XMayday in HTMLW/
**
** Copyright (C) 1996-2000 Axene.
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
** Started on  Thu May  9 16:21:51 1996 Emmanuel Paris
** Last update Mon Oct 19 15:25:41 1998 Emmanuel Paris
*/


#include "xcalibur.h"
#include <X11/Xlib.h>
#include "HTMLP.h"
#include "HTML.h"
#include "list.h"
#include "Colormap.h"

#define SHADOW_DARK	 "gray30"
#define SHADOW_LIGHT	 "gray80"

#define	DEFAULT_FIELD_WIDTH	30
#define	DEFAULT_FIELD_HEIGHT	20

#define FIELD_BORDER_SPACE	6	/*aesthetic space around element */

#define FONTHEIGHT(font) (font->max_bounds.ascent + font->max_bounds.descent)

extern c_Colormap	*GlobColormap;
extern char *ParseMarkTag();
extern void TriggerMarkChanges();
static void strip_text();
extern void clean_text();
extern int caseless_equal();
extern void TextRefresh();
extern void BulletRefresh();
extern void HRuleRefresh();
extern void ImageRefresh();
extern void WidgetRefresh();
extern void TableRefresh();
extern void HTMLDrawBackgroundImage();
extern XFontStruct *currentFont;
extern int LineNumber, LineHeight, BaseLine;
extern struct ele_rec *Current;
extern void Push_hw_TableInfo();
extern void Set_hw_TableInfo_bg();
extern void Pop_hw_TableInfo();

static void AdjustTableAlignment();

/* Allocate a TableField and initialize to default values 
* return 0 on failure 
*/
static TableField *NewTableField()
{
 TableField *tf;
  
 if (!(tf = (TableField *) malloc(sizeof(TableField))))
  return NULL;


 tf->colSpan = 1;
 tf->rowSpan = 1;
 tf->contVert = False;
 tf->contHoriz = False;
 tf->alignment = ALIGN_LEFT | ALIGN_MIDDLE;
 tf->request_width = tf->request_height = 0;
 tf->maxWidth = DEFAULT_FIELD_WIDTH;
 tf->minWidth = DEFAULT_FIELD_WIDTH;
 tf->maxHeight = DEFAULT_FIELD_HEIGHT; 
 tf->minHeight = DEFAULT_FIELD_HEIGHT;
 tf->colWidth = tf->rowHeight = 0;
 tf->header = False;
 tf->nowrap = False;
 tf->background = False;

 tf->first_rec = tf->last_rec = NULL;  
 return tf;
}


/* return word out of the text */
void GetWord(text, retStart, retEnd)
char *text;	 /* text to get a word out of */
char **retStart; /* RETURNED: start of word in text */
char **retEnd;	 /* RETURNED: end of word in text */
{
 char *start;
 char *end;
 
 if (!text)
 {
  *retStart = *retEnd = text;
  return;
 }
 
 start = text;
 while ((*start) && isspace(*start))/*skip over leading space*/
  start++;
 
 end = start;
 while((*end) && (!isspace(*end)))/* find next space */
  end++;
 
 *retStart = start;
 *retEnd = end;
 return;
}


static void TableCutString(rec, last_rec, remain_width)
struct ele_rec *rec;
struct ele_rec **last_rec;
int remain_width;
{
 int i, oi, width, owidth, ln;
 char *text, *rmtext;
 struct ele_rec *new_rec;
  
 text = rec->edata;
  
 i = -1; width = 0;
 do
 {
  oi = i++; owidth = width;
    
  if (!text[i]) return;		/* no word or only one word too short */
  while(text[i] && text[i] != ' ')
   i++;
    
  width = XTextWidth(rec->font, text, i);
 } while(width < remain_width);
  
 if (oi == -1) 
 {				/* must "cut" first word !! */
  do
  {
   rec->line_number++;
   if (rec == *last_rec) break;
   rec = rec->next;
  } while(1);
    
  return; 
 }
  
 i = oi;
 while( text[i] && text[i] == ' ') i++;
 if (!text[i])			/* text ends with spaces, only cut them */
 {
  text[oi] = '\0';
  rec->width = owidth;
  return;
 }
 /* here i is on the first non space char, oi is on the space to cut */
  
 text[oi] = '\0';
 rec->edata_len = oi + 1;
 rec->width = owidth;
  
 new_rec = (struct ele_rec *)malloc(sizeof(struct ele_rec));
 memcpy(new_rec, rec, sizeof(struct ele_rec));
 new_rec->prev = rec;		/* insert new ele_rec in list */
 new_rec->next = rec->next;
 if (rec->next) new_rec->next->prev = new_rec;
 rec->next = new_rec;
 if (rec == *last_rec)
  *last_rec = new_rec;
 new_rec->type = E_LINEFEED;
 new_rec->width = 0;
 new_rec->anchorName = new_rec->anchorHRef = new_rec->anchorSubject = NULL;
 new_rec->edata = NULL; new_rec->edata_len = 0;
  
 new_rec = (struct ele_rec *)malloc(sizeof(struct ele_rec));
 memcpy(new_rec, rec, sizeof(struct ele_rec));
  
 rec = rec->next;
 new_rec->prev = rec;		/* insert new ele_rec in list */
 new_rec->next = rec->next;
 if (rec->next) new_rec->next->prev = new_rec;
 rec->next = new_rec;
 if (rec == *last_rec)
  *last_rec = new_rec;
 new_rec->y_offset = 0;

 rmtext = Xc_NormalStrDup("rt", text + i);
 new_rec->edata = rmtext;
 new_rec->edata_len = strlen(rmtext) + 1;
 if (rec->anchorName)
  new_rec->anchorName = Xc_NormalStrDup("an", rec->anchorName);
 if (rec->anchorHRef)
  new_rec->anchorHRef = Xc_NormalStrDup("ah", rec->anchorHRef);
 if (rec->anchorSubject)
  new_rec->anchorSubject = Xc_NormalStrDup("an", rec->anchorSubject);
 new_rec->width = XTextWidth(new_rec->font, rmtext, strlen(rmtext));
  
 ln = ++new_rec->line_number;
 if (new_rec == *last_rec || new_rec->next->line_number >= ln) return;
  
 rec = new_rec->next;
 do
 {
  rec->line_number++;
  if (rec == *last_rec) break;
  rec = rec->next;
 } while(1);
}

static void strip_text(text)
char *text;
{
 unsigned char *txt;
  
 txt = (unsigned char *)text;
 while(*txt && *txt <= ' ') txt++;
  
 if (txt != (unsigned char *)text)
  memmove(text, txt, strlen(txt));
 if (!*text) return;
  
 txt = (unsigned char *)(text + strlen(text) - 1);
 while(*txt && *txt <= ' ') txt--;
 *(txt + 1) = '\0';
}

/* print out the table to stdout */
void TableDump(t) 
TableInfo *t;
{
#ifndef NTRACE
 register int x,y;
  
 Xc_TRACE(("Table dump:"));
 Xc_TRACE(("Border width is %d",t->borders));
 Xc_TRACE(("numColumns=%d, numRows=%d",t->numColumns,t->numRows));
 printf("---------------------------------------------------\n");
 for (y = 0; y < t->numRows; y++ )
 {
  printf("|");
  for (x = 0; x < t->numColumns; x++ )
  {
   printf("cw=%d,rh=%d | ",
	  t->table[y * t->numColumns + x].colWidth,
	  t->table[y * t->numColumns + x].rowHeight);
  }
  printf("\n---------------------------------------------------\n");
 }
#endif
}


/* fill out uneven rows in table */
/* return 0 on out of memory, 1 on suceess */
static int TableCleanUp(t, tableList)
TableInfo *t;
List tableList;
{
 int maxNumCols;
 int numColsInRow;
 List rowList;
 TableField *field;
 int x,y;

 /* determine number of columns all rows should have */
 rowList = (List) ListHead(tableList);
 t->numRows = 0;
 maxNumCols = 0;
 while(rowList)
 {
  field = (TableField *) ListHead(rowList);
  numColsInRow = 0;
  while (field)
  {
   numColsInRow++;
   field = (TableField *) ListNext(rowList);
  }
  maxNumCols=(maxNumCols > numColsInRow)?maxNumCols:numColsInRow;
  t->numRows++;
  rowList = (List) ListNext(tableList);
 }
  
 t->numColumns = maxNumCols;
  
  
 /* make all rows have correct number of columns */
 rowList = (List) ListHead(tableList);
 while(rowList)
 {
  numColsInRow = 0;
  field = (TableField *) ListHead(rowList);
  numColsInRow = 0;
  while (field)
  {
   numColsInRow++;
   field = (TableField *) ListNext(rowList);
  }
    
  while(numColsInRow < t->numColumns)
  {
   /* fill up the table with empty fields */
   if (!(field = NewTableField()))
   {
    return 0;			/* out of memory */
   }
   ListAddEntry(rowList,field);
   numColsInRow++;
  }
  rowList = (List) ListNext(tableList);
 }
  

 /* move 2D link list table to an array for speed */
 if (!(t->table = (TableField *) 
       malloc(sizeof(TableField) * t->numColumns * t->numRows)))
  return 0;			/* out of memory */
  
 y=0;
 rowList = (List) ListHead(tableList);
 while(rowList)
 {
  x = 0;
  field = (TableField *) ListHead(rowList);
  while (field)
  {
   memcpy(&(t->table[y * t->numColumns + x]), field, 
	  sizeof(TableField));
   x++;
   field = (TableField *) ListNext(rowList);
  }
  y++;
  rowList = (List) ListNext(tableList);
 }
  
 return 1;
}

/* return the number of connected fields */
static int TableHowManyConnectedHorizFields(t, xpos, ypos)
TableInfo *t;
int xpos,ypos;
{
 int count;
 register int x;
  
 count = 0;
 for (x = xpos+1; x < t->numColumns; x++)
 {
  if (t->table[ypos * t->numColumns + x].contHoriz)
   count++;
  else
   return count;
 }
 return count;
}

/* return the number of connected fields */
static int TableHowManyConnectedVertFields(t, xpos, ypos)
TableInfo *t;
int xpos,ypos;
{
 int count;
 register int y;
  
 count = 0;
 for (y = ypos+1; y < t->numRows; y++)
 {
  if (t->table[y * t->numColumns + xpos].contVert)
   count++;
  else
   return count;
 }
 return count;
}

static int CalculateMinWidthOfTable(t)
TableInfo *t;
{
 register int x,y;
 int sumMinWidth, minWidthOfCol = 0;
  
 sumMinWidth = 0;
 for (x = 0; x < t->numColumns; x++ )
 {
  minWidthOfCol = 0;
  for (y = 0; y < t->numRows; y++)
  {
   if (t->table[y * t->numColumns + x].minWidth > minWidthOfCol)
    minWidthOfCol = t->table[y * t->numColumns + x].minWidth;
  }
  sumMinWidth += minWidthOfCol;
 }
 return sumMinWidth + t->borders + 2;
}

static int CalculateMaxWidthOfTable(t)
TableInfo *t;
{
 register int x,y;
 int sumMaxWidth, maxWidthOfCol = 0;
  
 sumMaxWidth = 0;
 for (x = 0; x < t->numColumns; x++ )
 {
  maxWidthOfCol = 0;
  for (y = 0; y < t->numRows; y++)
  {
   if (t->table[y * t->numColumns + x].maxWidth > maxWidthOfCol)
    maxWidthOfCol = t->table[y * t->numColumns + x].maxWidth;
  }
  sumMaxWidth += maxWidthOfCol;
 }
 return sumMaxWidth +  t->borders + 2;
}

void TableCalculateDimensions(hw, t, px, py, pageWidth)
HTMLWidget hw;
TableInfo *t;
int px, py;		/* current x and y position */
int pageWidth;		/* width in pixels of output page */
{
 TableField *field;
 register int x, y;
 int xx, yy;
 int mx, my, rx, ry;
 int indent_space = 0;		/* indentation space */
 int sumMaxWidth;		/* summation of max widths */
 int maxWidthOfColumn;
 int maxHeightOfRow;
 int sumMinWidth;		/* summation of max widths */
 int maxWidthOfRow;
 int minWidthOfRow;	
 int numAdjacent;
 float percentToShrink;
  
 if (t->caption && t->captionAlignment == ALIGN_TOP)
 {
  py += FIELD_BORDER_SPACE + FONTHEIGHT(t->captionFont);
 }
  
 if (t->request_width)
 {
  if (t->request_width > 0)
   pageWidth = t->request_width;
  else
   pageWidth = (pageWidth * (-t->request_width) / 100);
 }

 /* calculate max and min width for each field*/
 sumMaxWidth = 0;
 sumMinWidth = 0;
 for (x = 0; x < t->numColumns; x++ )
 {
  maxWidthOfRow = minWidthOfRow = 0;
    
  for (y = 0; y < t->numRows; y++)
  {
   field = &(t->table[y * t->numColumns + x]);
   if (field->first_rec && field->last_rec)
   {
    int minWidth = 0, maxWidth = 0, maxWidth2 = 0, ln;
    struct ele_rec *rec;
	
    indent_space = 0;
    rec = field->first_rec;
	
    ln = rec->line_number-1;
    do
    {
     if (rec->line_number != ln)
     {
      maxWidth2 += indent_space;
      if (maxWidth2 > maxWidth) maxWidth = maxWidth2;
	    
      if (rec->indent_level)
      {
       indent_space = 
	rec->indent_level * hw->html.font->max_bounds.width * 2 +
	 hw->html.font->max_bounds.width;
      }
      else
       indent_space = 0;
	    
      ln = rec->line_number;
      maxWidth2 = 0;
     }
	  
     switch(rec->type)
     {
     case E_TEXT:
      if (field->nowrap)
      {
       maxWidth2 += rec->width + 3;
       if (rec->width + indent_space > minWidth)
	minWidth = rec->width + indent_space;
      }
      else
      {
       char *txt = rec->edata, *start, *end;
       int width;
	      
       maxWidth2 += rec->width + 3;
       do
       {
	GetWord(txt, &start, &end);
	if (start == end) break;
		
	width = XTextWidth(rec->font, start, (end - start)) 
	 + FIELD_BORDER_SPACE / 2;
	if (width + indent_space > minWidth)
	 minWidth = width + indent_space;
	txt = end;
       }
       while(1);
      }
      break;
     case E_HRULE:
      maxWidth2 += DEFAULT_FIELD_WIDTH;
      if (minWidth < DEFAULT_FIELD_WIDTH)
       minWidth = DEFAULT_FIELD_WIDTH;
      break;
     case E_TABLE:
      {
       int width;
	      
       maxWidth += rec->width;
       width = CalculateMinWidthOfTable(rec->table_data);
       if (width + indent_space > minWidth)
	minWidth = width + indent_space;
      }
      break;
     default:
      maxWidth2 += rec->width;
      if (rec->width + indent_space > minWidth)
       minWidth = rec->width + indent_space;
      break;
     }
	  
     if (rec == field->last_rec) break;
     rec = rec->next;
    } while(1);
	
    if (maxWidth2 + indent_space > maxWidth) 
     maxWidth = maxWidth2 + indent_space;
	
    if (field->request_width)
    {
     int width = 0;
	  
     if (field->request_width > 0)
      width = field->request_width;
     else if (pageWidth < 5000)
      width = (pageWidth * (-field->request_width) / 100);
     if (width > minWidth) minWidth = width;
    }
	
    if (minWidth > maxWidth) maxWidth = minWidth;
	
    field->maxWidth = maxWidth + FIELD_BORDER_SPACE;
    field->minWidth = minWidth + FIELD_BORDER_SPACE;
    field->minHeight = DEFAULT_FIELD_HEIGHT;
    field->maxHeight = DEFAULT_FIELD_HEIGHT;
   }
   else
   {
    /* empty field */
    field->maxWidth = field->minWidth = 
     field->maxHeight = field->minHeight = 0;
   }
      
   if (field->maxWidth > maxWidthOfRow) maxWidthOfRow = field->maxWidth;
   if (field->minWidth > minWidthOfRow) minWidthOfRow = field->minWidth;
  }
  /* save the length of the longest and shortest row */
  sumMaxWidth += maxWidthOfRow;
  sumMinWidth += minWidthOfRow;
 }
  
 /* divy up max width with adjacent continue Horizontal fields*/
 {
  int *colWidth, *colWidth2, maxWidth, minWidth;
  boolean colspan = FALSE;
     
  colWidth = (int *)malloc(sizeof(int) * t->numColumns);
  colWidth2 = (int *)malloc(sizeof(int) * t->numColumns);
  memset(colWidth, 0, sizeof(int) * t->numColumns);
  memset(colWidth2, 0, sizeof(int) * t->numColumns);
    
  for (x = 0; x < t->numColumns; x++)
  {
   for (y = 0; y < t->numRows; y++)
   {
    if (TableHowManyConnectedHorizFields(t, x, y))
     colspan = TRUE;
    else
    {
     if (t->table[y * t->numColumns + x].maxWidth > colWidth[x])
      colWidth[x] = t->table[y * t->numColumns + x].maxWidth;
     if (t->table[y * t->numColumns + x].minWidth > colWidth2[x])
      colWidth2[x] = t->table[y * t->numColumns + x].minWidth;
    }
   }
  } 
    
  if (colspan)
  {
   for (x = 0; x < t->numColumns; x++)
   {
    for (y = 0; y < t->numRows; y++)
    {
     numAdjacent = TableHowManyConnectedHorizFields(t, x, y);
     if (numAdjacent++)
     {
      maxWidth = 0; minWidth = 0;
      for (xx = x; xx < numAdjacent + x; xx++) 
      {
       maxWidth += colWidth[xx];
       minWidth += colWidth2[xx];
      }
	    
      if (maxWidth >= t->table[y * t->numColumns+x].maxWidth)
      {
       maxWidth = t->table[y * t->numColumns + x].maxWidth;
       for (xx = x; xx < numAdjacent + x; xx++)
       {
	if (colWidth[xx] > maxWidth) break;
	t->table[y * t->numColumns + xx].maxWidth = colWidth[xx];
	maxWidth -= colWidth[xx];
       }
       t->table[y * t->numColumns + xx].maxWidth = maxWidth;
       for (++xx; xx < numAdjacent + x; xx++)
	t->table[y * t->numColumns + xx].maxWidth = 0;
      }
      else
      {
       for (xx = x + numAdjacent - 1; xx >= x; xx--)
	t->table[y * t->numColumns + xx].maxWidth = 
	 t->table[y * t->numColumns + x].maxWidth / numAdjacent;
      }
	    
      if (minWidth >= t->table[y * t->numColumns + x].minWidth)
      {
       minWidth = t->table[y * t->numColumns + x].minWidth;
       for (xx = x; xx < numAdjacent + x; xx++)
       {
	if (colWidth2[xx] > minWidth) break;
	t->table[y * t->numColumns + xx].minWidth = colWidth2[xx];
	minWidth -= colWidth2[xx];
       }
       t->table[y * t->numColumns + xx].minWidth = minWidth;
       for (++xx; xx < numAdjacent + x; xx++)
	t->table[y * t->numColumns + xx].minWidth = 0;
      }
      else
      {
       for (xx = x + numAdjacent - 1; xx >= x; xx--)
	t->table[y * t->numColumns + xx].minWidth = 
	 t->table[y * t->numColumns + x].minWidth / numAdjacent;
      }
     }/* if (numAdjacent++) */
    }
   }
   sumMaxWidth = CalculateMaxWidthOfTable(t);
   sumMinWidth = CalculateMinWidthOfTable(t);
  }
  free(colWidth2);    
  free(colWidth);    
 }
  
 Xc_TRACE(("PageWidth: %d MinWidth: %d MaxWidth: %d",
	   pageWidth, sumMinWidth, sumMaxWidth));

 /* fit table to page */
 if (sumMaxWidth < pageWidth || sumMinWidth == sumMaxWidth)
 {
  /* fits on the page, set all fields to use max width */
    
  if (t->request_width)
  {
   percentToShrink = (float)(pageWidth) / (float)(sumMaxWidth);
      
   for (x = 0; x < t->numColumns; x++)
   {
    /* find widest field in column */
    maxWidthOfColumn = 0;
    for (y = 0; y < t->numRows; y++ )
    {
     maxWidthOfColumn =
      (maxWidthOfColumn > t->table[y * t->numColumns + x].maxWidth)?
       maxWidthOfColumn : t->table[y * t->numColumns+x].maxWidth;
    }
    maxWidthOfColumn = (int)((float)maxWidthOfColumn * percentToShrink);
    /* assign uniform width to column */
    for (y = 0; y < t->numRows; y++)
    {
     t->table[y*t->numColumns + x].colWidth = maxWidthOfColumn;
    }
   }
  }
  else
  { 
   for (x = 0; x < t->numColumns; x++)
   {
    /* find widest field in column */
    maxWidthOfColumn = 0;
    for (y = 0; y < t->numRows; y++ )
    {
     maxWidthOfColumn =
      (maxWidthOfColumn > t->table[y * t->numColumns + x].maxWidth)?
       maxWidthOfColumn : t->table[y * t->numColumns+x].maxWidth;
    }
    /* assign uniform width to column */
    for (y = 0; y < t->numRows; y++)
    {
     t->table[y*t->numColumns + x].colWidth = maxWidthOfColumn;
    }
   }
  }      
 }
 else
 {
  /* will have to squeeze fields downward to fit on page */
    
  {
   int extra_space;
      
   if (sumMinWidth > pageWidth)
    extra_space = 0;
   else
    extra_space = pageWidth - sumMinWidth;
      
   if (sumMaxWidth > sumMinWidth)
    percentToShrink = 
     ((float)extra_space) / ((float)sumMaxWidth - sumMinWidth);
   else
    percentToShrink = 0.0;
  }
    
  Xc_TRACE(("PageWidth: %d MinWidth: %d MaxWidth: %d PercentToShrink: %g",
	    pageWidth, sumMinWidth, sumMaxWidth, percentToShrink));

  for (x = 0; x < t->numColumns; x++)
  {      
#if 0               /* May be good */
   int width;
   
   maxWidthOfColumn = 0;
   for (y = 0; y < t->numRows; y++ )
   {
    field = &(t->table[y * t->numColumns + x]);
    
    width = (field->maxWidth - field->minWidth) * percentToShrink
     + field->minWidth; 

    Xc_TRACE(("Field: %dx%d minWidth: %d maxWidth: %d reduce: %d",
	      x, y,  field->minWidth, field->maxWidth, width));

    maxWidthOfColumn =
     (maxWidthOfColumn > width) ? maxWidthOfColumn : width;
   }
#else
   int mw, Mw;
   int request_width = 0;
   Mw = 0; mw = 0;

   for (y = 0; y < t->numRows; y++ )
   {
    field = &(t->table[y * t->numColumns + x]);

    if (field->request_width > 0)
     request_width = field->request_width;
    
    if (field->maxWidth > Mw) Mw = field->maxWidth;
    if (field->minWidth > mw) mw = field->minWidth;
   }
   
   if (request_width > 0)
   {
     mw = request_width;
     if (Mw < mw) Mw = mw; 
   }
   maxWidthOfColumn = (Mw - mw) * percentToShrink + mw;
   Xc_TRACE(("Field: %dx%d minWidth: %d maxWidth: %d reduce: %d",
	     x, y, mw, Mw, maxWidthOfColumn));
#endif
            
   /* assign uniform width to column */
   for (y = 0; y < t->numRows; y++)
   {
    t->table[y*t->numColumns + x].colWidth = maxWidthOfColumn;
   }
  }
 } 
  
 my = py; rx = 0;
 for(y = 0; y < t->numRows; y++)
 {
  int rowHeight = 0;
    
  mx = px;
  for (x = 0; x < t->numColumns; x++)
  {
   struct ele_rec *rec;
   int	colWidth, mh = 0, ln, i;
      
   ry = my + FIELD_BORDER_SPACE / 2 + t->borders;
   field = &(t->table[y * t->numColumns + x]);
   colWidth = field->colWidth;
      
   if (!(field->first_rec && field->last_rec))
   {
    mx += colWidth;
    if (rowHeight < field->minHeight) rowHeight = field->minHeight;
    continue;
   }
      
   numAdjacent = TableHowManyConnectedHorizFields(t, x, y) + 1;
   for(i = 1; i < numAdjacent; i++)
   {
    colWidth += t->table[y * t->numColumns + x + i].colWidth;
   }
      
   rec = field->first_rec;
   ln = rec->line_number - 1;
   do
   {
    if (ln != rec->line_number)
    {
     if (rec->indent_level)
     {
      if (rec->type == E_BULLET)
       indent_space =  
	rec->indent_level * hw->html.font->max_bounds.width * 2 +
	 hw->html.font->max_bounds.width - rec->width;
      else
       indent_space =  
	rec->indent_level * hw->html.font->max_bounds.width * 2 +
	 hw->html.font->max_bounds.width;
     }
     else
      indent_space = 0;
	  
     rx =  mx + FIELD_BORDER_SPACE / 2 + t->borders + indent_space;
     ry += mh;
     mh = 0;
     ln = rec->line_number;
    }
	
    rec->y = ry;
    switch(rec->type)
    {
    case E_LINEFEED:
     rec->line_height = mh;
     mh = 0;
     break;
    case E_TEXT:
     if (rec->width > (colWidth + mx - rx))
     {
      TableCutString(rec, &(field->last_rec), colWidth + mx - rx);
      if (ln != rec->line_number)
      {
       rx = mx + FIELD_BORDER_SPACE / 2 + t->borders + indent_space;
       ry += mh;
       mh = 0;
       ln = rec->line_number;
       rec->y = ry;
      }
     }
	  
     rec->x = rx; 
     rx += rec->width;
     break;
    case E_BULLET:
     if (rec->edata)
      rec->x = rx; 
     else
      rec->x = rx + hw->html.font->max_bounds.width;
	  
     rx += rec->width;
     break; 
    case E_TABLE:
     rec->x = rx;

     /*--- if table request width unspecified, set to 100% ---*/
     if (rec->table_data->request_width == 0 ||
	 (rec->table_data->request_width > 0 && rec->table_data->request_width
	  > colWidth - FIELD_BORDER_SPACE / 2 - 2 * t->borders))
      rec->table_data->request_width = -100;

     TableCalculateDimensions(hw, rec->table_data, rx, ry, colWidth - 
			      FIELD_BORDER_SPACE / 2 - 2 * t->borders);

     rec->width = rec->table_data->width;
     rec->y_offset = rec->table_data->height - 4;
     rec->line_height = rec->table_data->height;

     rx += rec->width;
     break;
    case E_HRULE:
     rec->width = colWidth - FIELD_BORDER_SPACE / 2 - t->borders;
    default:
     rec->x = rx; 
     rx += rec->width;
     break;
    }
    
    if (rec->line_height > mh)
     mh = rec->line_height;
    
    if (rec == field->last_rec) break;
    rec = rec->next;
   } while(1);
   ry += mh;
      
   field->minHeight = ry - my;
   if (rowHeight < (ry - my)) rowHeight = (ry - my);

   mx += field->colWidth;
  }
    
  for (x = 0; x < t->numColumns; x++)
  {
   field = &(t->table[y * t->numColumns + x]);
   if ( field->request_height && field->request_height > rowHeight)
    rowHeight = field->request_height;
  }
  for (x = 0; x < t->numColumns; x++)
   t->table[y * t->numColumns + x].rowHeight = rowHeight;
    
  my += rowHeight;
 }
  
  
 /* divy up min height with adjacent continue Horizontal fields*/
 {
  int *rowHeight, *oldRowHeight, minHeight;
  boolean colspan = FALSE;
     
  rowHeight = (int *)malloc(sizeof(int) * t->numRows);
  oldRowHeight = (int *)malloc(sizeof(int) * t->numRows);
  memset(rowHeight, 0, sizeof(int) * t->numRows);
  memset(oldRowHeight, 0, sizeof(int) * t->numRows);
    
  for (y = 0; y < t->numRows; y++)
  {
   for (x = 0; x < t->numColumns; x++)
   {
    if (TableHowManyConnectedVertFields(t, x, y))
     colspan = TRUE;
    else
    {
     if (t->table[y * t->numColumns + x].minHeight > rowHeight[y])
      rowHeight[y] = t->table[y * t->numColumns + x].minHeight;
    }
   }
   oldRowHeight[y] = t->table[y * t->numColumns].rowHeight;
  } 
    
  if (colspan)
  {
   for (y = 0; y < t->numRows; y++)
   {
    for (x = 0; x < t->numColumns; x++)
    {
     numAdjacent = TableHowManyConnectedVertFields(t, x, y);
     if (numAdjacent++)
     {
      minHeight = 0;
      for (yy = y; yy < numAdjacent + y; yy++) 
       minHeight += rowHeight[yy];
	    
      if (minHeight >= t->table[y * t->numColumns + x].minHeight)
      {
       minHeight = t->table[y * t->numColumns + x].minHeight;
       for (yy = y; yy < numAdjacent + y; yy++)
       {
	if (rowHeight[yy] > minHeight) break;
	t->table[yy * t->numColumns + x].minHeight = rowHeight[yy];
	minHeight -= rowHeight[yy];
       }
       t->table[yy * t->numColumns + x].minHeight = minHeight;
       for (++yy; yy < numAdjacent + y; yy++)
	t->table[yy * t->numColumns + x].minHeight = 0;
      }
      else
      {
       for (yy = y + numAdjacent - 1; yy >= y; yy--)
	t->table[yy * t->numColumns + x].minHeight = 
	 t->table[y * t->numColumns + x].minHeight / numAdjacent;
      }
     }
    }
   }
      
      
   maxHeightOfRow = 0;
   for (x = 0; x < t->numColumns; x++)
    maxHeightOfRow = ((t->table[x].minHeight > maxHeightOfRow) ?
		      t->table[x].minHeight : maxHeightOfRow);
      
   for (x = 0; x < t->numColumns; x++)
    t->table[x].rowHeight = maxHeightOfRow;
      
   ry = oldRowHeight[0] - maxHeightOfRow;
      
   for (y = 1; y < t->numRows; y++)
   {
    maxHeightOfRow = 0;
    for (x = 0; x < t->numColumns; x++)
    {
     maxHeightOfRow = 
      ((t->table[y * t->numColumns + x].minHeight > maxHeightOfRow) ?
       t->table[y * t->numColumns + x].minHeight : maxHeightOfRow);
    }
	
    for (x = 0; x < t->numColumns; x++)
    {
     struct ele_rec *rec;
	  
     field = &(t->table[y * t->numColumns + x]);
     field->rowHeight = maxHeightOfRow;
	  
     if (field->first_rec && field->last_rec)
     {
      rec = field->first_rec;
      do
      {
       rec->y -= ry;
       if (rec->type == E_TABLE)
       {
	AdjustTableAlignment(hw, rec, 0, -ry);
       }  
       if (rec == field->last_rec) break;
       rec = rec->next;
      } while(1);
     }
    }
    ry += oldRowHeight[y] - maxHeightOfRow;
   }
      
  }
    
  free(oldRowHeight);
  free(rowHeight);
 }
  
 /* calcul horizontal and vertical aligment */

 for(y = 0; y < t->numRows; y++)
 {
  for (x = 0; x < t->numColumns; x++)
  {
   int ln, i;
   int ewidth, colwidth, rowheight, dx, dy;
   int minheight, alignment;
   struct ele_rec *rec, *rec2;
      
   field = &(t->table[y * t->numColumns + x]);
   if (!(field->first_rec && field->last_rec))  continue;
      
   alignment = field->alignment;
      
   rowheight = minheight = 0;
      
   numAdjacent = TableHowManyConnectedVertFields(t, x, y) + 1;
   for(i = 0; i < numAdjacent; i++)
   {
    rowheight += t->table[(y + i) * t->numColumns + x].rowHeight;
    minheight +=  t->table[(y + i) * t->numColumns + x].minHeight;
   }
      
   colwidth = 0; 
   numAdjacent = TableHowManyConnectedHorizFields(t, x, y) + 1;
   for(i = 0; i < numAdjacent; i++)
   {
    colwidth += t->table[y * t->numColumns + x + i].colWidth;
   }
      
   switch(alignment & VALIGN_MASK)
   {
   case ALIGN_MIDDLE:
    dy = (rowheight - minheight) / 2;
    break;
   case ALIGN_BOTTOM:
    dy = rowheight - minheight;
    break;
   case ALIGN_TOP:
   default:
    dy = 0;
    break;
   }
      
   rec = field->first_rec;
   do
   {
    ln = rec->line_number;
	
    if (rec->indent_level)
    {
     if (rec->type == E_BULLET)
      ewidth =  
       rec->indent_level * hw->html.font->max_bounds.width * 2 +
	hw->html.font->max_bounds.width - rec->width;
     else
      ewidth =  
       rec->indent_level * hw->html.font->max_bounds.width * 2 +
	hw->html.font->max_bounds.width;
    }
    else
     ewidth = 0;
	
    alignment = field->alignment;
    rec2 = rec;
    do
    {
     ewidth += rec->width;
     if ((rec->alignment & HALIGN_MASK) != ALIGN_LEFT)
      alignment = (alignment & VALIGN_MASK) | 
       (rec->alignment & HALIGN_MASK);

     if (rec == field->last_rec) break;
     rec = rec->next;
    } while(rec->line_number == ln);
	
    rec = rec2;
    switch(alignment & HALIGN_MASK)
    {
    case ALIGN_CENTER:
     dx = (colwidth - ewidth - FIELD_BORDER_SPACE / 2 - t->borders) / 2;
     break;
    case ALIGN_RIGHT:
     dx = colwidth - ewidth  - FIELD_BORDER_SPACE / 2 - t->borders;
     break;
    case ALIGN_LEFT:
    default:
     dx = 0;
     break;
    }
	
    do
    {
     rec->x += dx;
     rec->y += dy;
     
     if (rec->type == E_TABLE)
      AdjustTableAlignment(hw, rec, dx, dy);

     if (rec == field->last_rec || rec->next->line_number != ln) break;
     rec = rec->next;
    } while(1);
		
    if (rec == field->last_rec) break;
    rec = rec->next;
   } while(1);

   x += numAdjacent - 1; 
  }
 }
  
 /* calculate table width */
 t->width = 0;
 for (x = 0; x < t->numColumns; x++)
 {
  Xc_TRACE(("colWidth for %d,0 = %d",x,t->table[x].colWidth));
    
  t->width += t->table[x].colWidth;
 }
 /* calculate table height */
 t->height = 0;
 for (y = 0; y < t->numRows; y++)
 {
  Xc_TRACE(("rowHeight for 0,%d = %d", y, 
	    t->table[y * t->numColumns].rowHeight));
  t->height += t->table[y * t->numColumns].rowHeight;
 }
  
 t->width += t->borders + 1;
 t->height += t->borders + 1;
  
 t->bwidth = t->width; t->bheight = t->height;
 if (t->caption)
  t->height += FONTHEIGHT(t->captionFont) + FIELD_BORDER_SPACE;

 t->height++;
 t->width++;

#ifndef NTRACE
 TableDump(t);
 Xc_TRACE(("TableCalculateDimensions(): table is %d x %d",
	   t->width,t->height));
#endif

}/* TableCalculateDimensions() */


/* expand colspans and rowspans in table */
/* return True if this routine did something */
static Boolean TableExpandFields(tableList, rowList, rowCount, columnCount)
List tableList;
List rowList;
int rowCount;
int *columnCount;
{
 TableField *field;
 List previousRow;		/* previous to current row */
 TableField *aboveField;	/* field above current field */
 TableField *fieldToTheLeft;	/* field to the left of current field */
 Boolean expandedSomething;
  
 expandedSomething = False;
 /* check for and take care of previous rowspans */
 if (rowCount > 1)
 {
  /* get field above this one */
  previousRow = (List) ListGetIndexedEntry
   (tableList, rowCount - 2);	/*zero indexed*/
  aboveField =(TableField *)ListGetIndexedEntry(previousRow, *columnCount);
  if (aboveField)
  {
   /*check if the above expands into this row*/
   if (aboveField->rowSpan > 1)
   {
    if (!(field = NewTableField()))
     return 0;			/* out of memory */
	
    field->rowSpan = aboveField->rowSpan - 1;
    field->contVert = True;
    field->contHoriz = aboveField->contHoriz;
    field->header = aboveField->header;
	
    ListAddEntry(rowList, field);
    expandedSomething = True;
    (*columnCount)++;
   }
  }
 }
  
 /* check for and take care of previous colspans */
 if (*columnCount)
 {
  /* get field above this one */
  fieldToTheLeft = (TableField *) ListTail(rowList);
  while(fieldToTheLeft->colSpan > 1)
  {
   if (!(field = NewTableField()))
   {
    return 0;			/* out of memory */
   }
      
   field->colSpan = fieldToTheLeft->colSpan - 1;
   field->contHoriz = True;
   field->header = fieldToTheLeft->header;
      
   if (fieldToTheLeft->rowSpan > 1)
   {
    field->rowSpan = fieldToTheLeft->rowSpan;
   }
      
   ListAddEntry(rowList, field);
   fieldToTheLeft = field;
   (*columnCount)++;
   expandedSomething = True;
  }
 }
  
 return expandedSomething;
}



static void TableCheckForTagsFont(hw, font, mptr)
HTMLWidget hw;
XFontStruct **font;
struct mark_up **mptr;
{
 struct mark_up *m;
  
 m = *(mptr);
 while(m && (m->type != M_TABLE) && (m->type != M_TABLE_ROW) &&
       (m->type != M_TABLE_DATA) && (m->type != M_TABLE_HEADER)
       && (m->type != M_CAPTION) && (m->type != M_NONE))
 {
  if (!m->is_end)
  {				/* effect is to only use the top on stack */
   switch(m->type)
   {
   case M_ITALIC:
   case M_VARIABLE:
   case M_EMPHASIZED:
    *font = hw->html.italic_font;
    break;
   case M_BOLD:
   case M_STRONG:
   case M_ANCHOR:
    *font = hw->html.bold_font;
    break;
   case M_FIXED:
   case M_CODE:
   case M_SAMPLE:
   case M_KEYBOARD:
    *font = hw->html.fixed_font;
    break;
   case M_BIG:
    *font = hw->html.big_font;
    break;
   case M_SMALL:
    *font = hw->html.small_font;
    break;
   }
  }
  m = m->next;
 }
 *mptr = m;
}

#if 0
/* set/get attributes for display from mark list for the field*/
static void TableFieldSetAttributes(hw, field, mptr)
HTMLWidget hw;
TableField *field;
struct mark_up **mptr;
{
 struct mark_up *m;

 if (field->header)
  field->font = hw->html.plainbold_font;
 else
  field->font = hw->html.plain_font;/* default font */
  
 m = (*mptr)->next;
 TableCheckForTagsFont(hw, &(field->font), &m);
    
 if (m->type == M_NONE)
 {				/* text */
  field->type = F_TEXT;
  if (!field->text)
   field->text = Xc_NormalStrDup("t6", m->text);
  else
  {
   char *txt = field->text;
		
   field->text = (char *)malloc(strlen(txt) + strlen(m->text) + 1);
   strcpy(field->text, txt);
   strcat(field->text, m->text);
   free(txt);
  }
  clean_text(field->text); strip_text(field->text);
 } 
}
#endif

TableInfo *MakeTable(hw, mptr, x, y)
HTMLWidget hw;
struct mark_up **mptr;
unsigned int x,y;
{
 struct mark_up *m;
 TableInfo *t;
 TableField *field;
 int columnCount;
 int rowCount;
 boolean captionStart = FALSE;
 boolean getFirst = FALSE;
 boolean getLast = FALSE;
 int alignment = ALIGN_LEFT | ALIGN_MIDDLE;
 boolean background = FALSE;
 unsigned long bgcolor = 0;
 char *val;
 List rowList; 			/* current row (List of TableFields)*/
 List tableList;		/* list of Row Lists */
 char *tptr;
 int	ox, oy;
 struct ele_rec *lastCurrent;
  
 if (((*mptr)->type != M_TABLE) || ((*mptr)->is_end))
  return NULL;

 if (!(t = (TableInfo *) malloc(sizeof(TableInfo))))
  return NULL;
  
 Push_hw_TableInfo();
  
 t->numColumns = 0;
 t->numRows = 0;
 t->caption = (char *) 0;
 t->captionAlignment = ALIGN_TOP;
 t->captionFont = hw->html.small_font;
 t->background = False;
 t->request_width = 0;
 t->borders = 0;
  
 if ((tptr = ParseMarkTag(((*mptr)->start), MT_TABLE, "BORDER")) != 0)
 {
  t->borders = atoi(tptr);
  free(tptr);
 }
  
 tptr = ParseMarkTag((*mptr)->start, MT_TABLE, "BGCOLOR");
 if (tptr)
 {
  int i;
  color_cells_t cell;
    
  if (*tptr != '#')
   for(i = 0; i < (int)strlen(tptr); i++)
   {
    if (tptr[i] >= 'A' && tptr[i] <= 'Z')
     tptr[i] += 'a' - 'A';
   }      
    
  if (F(GlobColormap).allocate_named_color(GlobColormap, tptr, &cell))
  {
   t->background = True;
   t->backgroundColor = cell.pixel;
   Set_hw_TableInfo_bg(t->backgroundColor);
  }
  free(tptr);
 }
  
 tptr = ParseMarkTag((*mptr)->start, MT_TABLE, "WIDTH");
 if (tptr)
 {
  char *txt, *txt2, *txt3;
  int width;
  Boolean percent;
    
  do
  {
   txt = tptr;
   while(*txt && *txt == ' ') txt++;
   if (*txt < '0' || *txt > '9') break;
   txt2 = txt;
   while(*txt && *txt >= '0' && *txt <= '9') txt++;
   txt3 = txt;
   while(*txt && *txt == ' ') txt++;
   if (*txt == '%')
    percent = True;
   else
    percent = False;
      
   *txt3 = '\0';
   width = atoi(txt2);
   if (percent) width = -width;
      
   t->request_width = width;
  } while(0);
    
  free(tptr);
 }
  
 tableList = ListCreate();
 rowList = ListCreate();
 ListAddEntry(tableList, rowList);
 columnCount = 0;
 rowCount = 1;
 m = (*mptr)->next;
 field = (TableField *) 0;
 while (m && (!((m->type == M_TABLE) && (m->is_end))))
 {
  switch(m->type)
  {
  case M_CAPTION:
   if (!m->is_end)
   {
    val = ParseMarkTag(m->start, MT_CAPTION, "align");
    if (caseless_equal(val, "top"))
     t->captionAlignment = ALIGN_TOP;
    else
     if (caseless_equal(val, "bottom"))
      t->captionAlignment = ALIGN_BOTTOM;
    captionStart = TRUE;
   }
   else
   {
    LineNumber++;
    BaseLine = -100;
    captionStart = FALSE;
   }
   break;
  case M_TABLE_ROW:
   alignment =  ALIGN_LEFT | ALIGN_MIDDLE;
   if (!getFirst && getLast) 
    field->last_rec = Current;
      
   if (!m->is_end)
   {
    /* expand at end of row */
    while(TableExpandFields(tableList, rowList,
			    rowCount, &columnCount));
	
    /* if: is this the first container <tr> or the 
       separator */
    if (ListHead(ListHead(tableList)))
    {
     rowList = ListCreate();
     ListAddEntry(tableList,rowList);
     rowCount++;
    }
    columnCount = 0;
	
    val = ParseMarkTag(m->start, MT_TABLE_DATA, "align");
    if (caseless_equal(val,"center") || caseless_equal(val,"middle"))
     alignment = (alignment & VALIGN_MASK) | ALIGN_CENTER;
    else
    {
     if (caseless_equal(val,"right"))
      alignment = (alignment & VALIGN_MASK) | ALIGN_RIGHT;
     else
      alignment = (alignment & VALIGN_MASK) | ALIGN_LEFT;
    }
    if (val) free(val);
	
    val = ParseMarkTag(m->start, MT_TABLE_DATA, "valign");
    if (caseless_equal(val, "top"))
     alignment = (alignment & HALIGN_MASK) | ALIGN_TOP;
    else
    {
     if (caseless_equal(val, "bottom"))
      alignment = (alignment & HALIGN_MASK) | ALIGN_BOTTOM;
     else
      alignment = (alignment & HALIGN_MASK) | ALIGN_MIDDLE;
    }
    if (val) free(val);
    
    val = ParseMarkTag(m->start, MT_TABLE_DATA, "bgcolor");
    if (val)
    {
     color_cells_t cell;
     
     if (F(GlobColormap).allocate_named_color(GlobColormap, val, &cell))
     {
      background = True;
      bgcolor = cell.pixel;
     }
     free(val);
    }
    else
    {
     background = FALSE;
    }
    
    /* expand cols at beginning of row */
    TableExpandFields(tableList, rowList, rowCount, &columnCount);
    LineNumber++;
    BaseLine = -100;
   } 
   break;
  case M_TABLE_DATA:
  case M_TABLE_HEADER:
   if (!m->is_end)
   {
    if (!getFirst && getLast)
     field->last_rec = Current;

    while(TableExpandFields(tableList, rowList,
			    rowCount, &columnCount));
	
    if (!(field = NewTableField()))
     return NULL;		/*out of memory */
	
	
    val = ParseMarkTag(m->start,MT_TABLE_HEADER,"colspan");
    if (val)
    {
     field->colSpan = atoi(val);
     if ((field->colSpan > 100)||(field->colSpan<1))
      field->colSpan = 1;
     free(val);
    }
	    
    val = ParseMarkTag(m->start,MT_TABLE_HEADER,"rowspan");
    if (val)
    {
     field->rowSpan = atoi(val);
     if ((field->rowSpan > 100) || (field->rowSpan < 1))
      field->rowSpan = 1;
     free(val);
    }
    
    field->background = background;
    field->bgcolor = bgcolor;

    val = ParseMarkTag(m->start, MT_TABLE_HEADER, "bgcolor");
    if (val)
    {
     color_cells_t cell;
     
     if (F(GlobColormap).allocate_named_color(GlobColormap, val, &cell))
     {
      field->background = True;
      field->bgcolor = cell.pixel;
     }
     free(val);
    }

    /* check for alignment */
    if (m->type == M_TABLE_HEADER)
    {	
     field->header = True;
     field->alignment = (alignment & VALIGN_MASK) | ALIGN_CENTER;
     currentFont = hw->html.bold_font;
    }
    else
    {
     field->header = False;
     field->alignment = alignment;
     currentFont = hw->html.font;
    }
	
    val = ParseMarkTag(m->start, MT_TABLE_DATA, "align");
    if (val)
    {
     if (caseless_equal(val, "center") || caseless_equal(val, "middle"))
      field->alignment = (field->alignment & VALIGN_MASK) | ALIGN_CENTER;
     else
     {
      if (caseless_equal(val, "right"))
       field->alignment = (field->alignment & VALIGN_MASK)| ALIGN_RIGHT;
      else
       field->alignment = (field->alignment & VALIGN_MASK)| ALIGN_LEFT;
     }
     free(val);
    }
	
    val = ParseMarkTag(m->start, MT_TABLE_DATA, "valign");
    if (val)
    {
     if (caseless_equal(val, "top"))
      field->alignment = (field->alignment & HALIGN_MASK) | ALIGN_TOP;
     else
     {
      if (caseless_equal(val, "bottom"))
       field->alignment = (field->alignment & HALIGN_MASK)|ALIGN_BOTTOM;
      else
       field->alignment = (field->alignment & HALIGN_MASK)|ALIGN_MIDDLE;
     }
     free(val);
    }	
	
    val = ParseMarkTag(m->start, MT_TABLE_DATA, "width");
    if (val)
    {
     char *txt, *txt2, *txt3;
     int width;
     Boolean percent;
	  
     do
     {
      txt = val;
      while(*txt && *txt == ' ') txt++;
      if (*txt < '0' || *txt > '9') break;
      txt2 = txt;
      while(*txt && *txt >= '0' && *txt <= '9') txt++;
      txt3 = txt;
      while(*txt && *txt == ' ') txt++;
      if (*txt == '%')
       percent = True;
      else
       percent = False;
	    
      *txt3 = '\0';
      width = atoi(txt2);
      if (percent) width = -width;
	    
      field->request_width = width;
     } while(0);
	  
     free(val);
    }
	
    val = ParseMarkTag(m->start, MT_TABLE_DATA, "height");
    if (val)
    {
     field->request_height = atoi(val);
     free(val);
    }
	
    val = ParseMarkTag(m->start, MT_TABLE_DATA, "nowrap");
    if (val)
    {
     field->nowrap = True;
     free(val);
    }
	
    getFirst = getLast = TRUE;
    ListAddEntry(rowList, field);
    columnCount++;
    LineNumber++;
    BaseLine = -100;
   }
   else
   {
    if (!getFirst && getLast)
     field->last_rec = Current;
    getFirst = getLast = FALSE;
   }
   break;
  default:
   if (captionStart)
   {
    TableCheckForTagsFont(hw, &(t->captionFont), &m);
    if (m->type == M_NONE)
    { 
     if (!t->caption)
      t->caption = Xc_NormalStrDup("caption", m->text);
     else
     {
      char *txt = t->caption;
	    
      t->caption = (char *)malloc(strlen(txt) + strlen(m->text) + 1);
      strcpy(t->caption, txt);
      strcat(t->caption, m->text);
      free(txt);
     }
     clean_text(t->caption); strip_text(t->caption);
    }
    else 
     continue;
   }
   else 
   {
    lastCurrent = Current;
    ox = oy = 0;
    TriggerMarkChanges(hw, &m, &ox, &oy);
    if (getFirst && Current != lastCurrent)
    {
     field->first_rec = lastCurrent->next;
     field->first_rec->y_offset = 0;
     getFirst = FALSE;
     lastCurrent = Current;
    }
   }
  }
  if (!m) break;
  m = m->next;
 }
 *mptr = m;			/* advance mark pointer to end of table */
 if (!getFirst && getLast)
  field->last_rec = Current;
  
 /* end of table has been hit, so wrap it up */
 /* clean up any at end of row */
 while(TableExpandFields(tableList, rowList, rowCount, &columnCount));
    
 Pop_hw_TableInfo();
  
 if (!(TableCleanUp(t,tableList)))
  return NULL;			/* out of memory */

 /* free up memory from tableList since TableCleanUp
    has already copied it into an array for speed. */
 rowList = (List) ListHead(tableList);
 while (rowList)
 {
  field = (TableField *) ListHead(rowList);
  while(field)
  {
   ListDeleteEntry(rowList,field);
   free(field);
   field = (TableField *) ListHead(rowList);
  }
  ListDeleteEntry(tableList,rowList);
  ListDestroy(rowList);
  rowList = (List) ListHead(tableList);
 }
  
 return t;
}/* MakeTable() */


/* Find actual table field dimensions considering colspans & rowspans */
static void TableGetExpandedDimensions(t,xpos,ypos, expandWidth,expandHeight)
TableInfo *t;
int xpos,ypos; /* current field index */
int *expandWidth,*expandHeight; /* returned */
{
 int x,y;
 x = xpos;
 y = ypos;
  
 *expandWidth = t->table[y * t->numColumns + x].colWidth;
 *expandHeight = t->table[y * t->numColumns + x].rowHeight;
  
 /* do width */
 x++;
 if (x < t->numColumns)
 {
  /* do width */
  while ((x < t->numColumns) && 
	 t->table[y * t->numColumns + x].contHoriz)
  {
   (*expandWidth) += t->table[y * t->numColumns + x].colWidth;
   x++;
  }
 }
  
 x = xpos;
 y++;
 if (y < t->numRows)
 {
  /* do height */
  while ((y < t->numRows) && 
	 t->table[y * t->numColumns + x].contVert)
  {
   (*expandHeight) += t->table[y * t->numColumns+x].rowHeight;
   y++;
  }
 }  
}

/* Table Locate Element: try to find record pointed by x/y */
struct ele_rec *TableLocateElement(hw, eptr, x, y)
HTMLWidget hw;
struct ele_rec *eptr;
int x, y;
{
 int xx, yy, cx, cy;
 TableInfo  *t;
 TableField *field;
 struct ele_rec *rptr = NULL, *rec;
  
 t = eptr->table_data;
 if (t == NULL) return NULL;
  
 /* try to find good cell */
  
 cx = eptr->x;
 if (x < cx) return NULL;	/* x is on the table's left */
  
 for (xx = 0; xx < t->numColumns; xx++)
 {
  cx += t->table[xx].colWidth;
  if (x <= cx) break;
 }
 if (x > cx) return NULL;	/* x is on the table's right */
  
 cy = eptr->y;
 if (y < cy) return NULL;	/* y is on the table's top */
  
 for (yy = 0; yy < t->numRows; yy++)
 {
  cy += t->table[yy * t->numColumns].rowHeight;
  if (y <= cy) break;
 }
 if (y > cy) return NULL;	/* x is on the table's bottom */
  

 field = &(t->table[yy * t->numColumns + xx]);
 if ((!field->first_rec || !field->last_rec) &&
     (field->contHoriz ||field->contVert))
 {
  if (field->contHoriz)
  {
   do
   {
    field = &(t->table[yy * t->numColumns + (--xx)]);
   }
   while(xx >=0 && field->contHoriz);
   if (xx < 0) return NULL;
  }
  else
  {
   do
   {
    field = &(t->table[(--yy) * t->numColumns + xx]);
   }
   while(yy >=0 && field->contVert);
   if (yy < 0) return NULL;
  }
 }
  
 /* here good cell is at xx/yy */  
 if (field->first_rec && field->last_rec)
 {
  rec = field->first_rec;
  do
  {
   if (x >= rec->x && x <= (rec->x + rec->width) &&
       y >= rec->y && y <= (rec->y + rec->line_height))
   {
    if (rec->type == E_TABLE)
     rptr = TableLocateElement(hw, rec, x, y);
    else
     rptr = rec;
    break;
   }
   if (rec == field->last_rec) break;
   rec = rec->next;
  } while(1);
 }
 return rptr;
}

/* Compute Table Aligmnent: add dx to all cell's record x coordinate */
void ComputeTableAlignment(hw, eptr, dx)
HTMLWidget hw;
struct ele_rec *eptr;
int dx;
{
 AdjustTableAlignment(hw, eptr, dx, 0);
}

static void AdjustTableAlignment(hw, eptr, dx, dy)
HTMLWidget hw;
struct ele_rec *eptr;
int dx;
{
 TableInfo  *t;
 TableField *field;
 struct ele_rec *rec;
 int x,y;
  
 t = eptr->table_data;
 if (t == NULL) return;
  
 field = t->table;
 for (y = 0; y < t->numRows; y++)
 {
  for (x = 0; x < t->numColumns; x++)
  {
   if (field->first_rec && field->last_rec)
   {
    rec = field->first_rec;
    do
    {
     rec->x += dx;
     rec->y += dy;
     if (rec->type == E_TABLE)
      AdjustTableAlignment(hw, rec, dx, dy);
	  
     if (rec == field->last_rec) break;
     rec = rec->next;
    } while(1);
   }
   field++;
  }
 }
}

/* display table */
void TableRefresh(hw, eptr)
HTMLWidget hw;
struct ele_rec *eptr;
{
 int x, y;					/* table origin */
 register int xx,yy;
 TableInfo  *table;
 TableField *field;
 struct ele_rec *rec;
 int xMarker, yMarker;
 int colWidth,rowHeight;
 int expandedWidth,expandedHeight;
 unsigned long shadow_light, shadow_dark;
 GC	gc_sl, gc_sd, gc_bg, gc_field;
 Display	*display;
 Window	window;
 XRectangle rect[1];
 XPoint	points[3];
  
 if (eptr->table_data == NULL)
  return;
 table = eptr->table_data;

 x = eptr->x;
 y = eptr->y;
  
 x = x - hw->html.scroll_x;
 y = y - hw->html.scroll_y;

 rect[0].x = hw->html.redraw_x;
 rect[0].y =  hw->html.redraw_y;
 rect[0].width = hw->html.redraw_width;
 rect[0].height = hw->html.redraw_height;

 display = XtDisplay(hw); window = XtWindow(hw->html.view); 
 /*   DefaultRootWindow(display); */
 gc_sl = gc_sd = gc_bg = (GC)0;
  
 /*  XSetLineAttributes(XtDisplay(hw), hw->html.drawGC,
     eptr->table_data->borders,
     LineSolid, CapNotLast, JoinMiter); */
 XSetForeground(display, hw->html.drawGC, eptr->fg);
 XSetBackground(display, hw->html.drawGC, eptr->bg);
 XSetClipRectangles(display, hw->html.drawGC, 0, 0, rect, 1, Unsorted);
 
 if (table->caption && table->captionAlignment == ALIGN_TOP)
 {
  int divh = FIELD_BORDER_SPACE / 2;
  XFontStruct	*font = table->captionFont;
    
  y += divh + font->max_bounds.ascent;
  
  XSetFont(display, hw->html.drawGC, font->fid);
  XDrawString(display, window, hw->html.drawGC, 
	      x + (table->width - XTextWidth(font, table->caption,
					     strlen(table->caption))) / 2,
	      y, table->caption, strlen(table->caption));
  y += font->max_bounds.descent + FIELD_BORDER_SPACE - divh;
 }
  
 XSetForeground(display, hw->html.drawGC, eptr->bg);

 if (!table->background || table->borders)
 {
  if(hw->html.bg_image)
   HTMLDrawBackgroundImage((Widget)hw, x, y, table->bwidth, table->bheight);
  else
   XFillRectangle(display, window, hw->html.drawGC, x, y,
		  table->bwidth, table->bheight);
 }


 XSetForeground(display, hw->html.drawGC, eptr->fg);
 XSetBackground(display, hw->html.drawGC, eptr->bg);
  
 if (table->background)
 {
  XGCValues	val_gc;
  
  val_gc.function = GXcopy;
  val_gc.foreground = table->backgroundColor;
  val_gc.fill_style = FillSolid;
  gc_bg = XCreateGC(display, window, GCFunction | GCForeground
		    | GCFillStyle, &val_gc);
  
  XSetClipRectangles(display, gc_bg, 0, 0, rect, 1, Unsorted);
  
  XFillRectangle(display, window, gc_bg, x, y, table->bwidth, table->bheight);
 }

 if (table->borders)
 {
  color_cells_t	realcolor;
  XGCValues	val_gc;
  
  if (DefaultDepth(display, DefaultScreen(display)) == 1)
  {
   shadow_light = shadow_dark = BlackPixel(display, DefaultScreen(display));
  }
  else
  {
   F(GlobColormap).allocate_named_color(GlobColormap, SHADOW_LIGHT,
					&realcolor);
   shadow_light = realcolor.pixel;
   F(GlobColormap).allocate_named_color(GlobColormap, SHADOW_DARK,
					&realcolor);
   shadow_dark = realcolor.pixel;
  }
    
  val_gc.function = GXcopy;
  val_gc.foreground = shadow_light;
  val_gc.line_width = 1;
  val_gc.line_style = LineSolid; 
  val_gc.cap_style = CapButt;
  val_gc.join_style = JoinMiter;
  gc_sl = XCreateGC(display, window, GCFunction | GCForeground
		    | GCLineWidth | GCLineStyle | GCCapStyle
		    | GCJoinStyle, &val_gc);
  val_gc.foreground = shadow_dark;    
  gc_sd = XCreateGC(display, window, GCFunction | GCForeground
		    | GCLineWidth | GCLineStyle | GCCapStyle
		    | GCJoinStyle, &val_gc);
 
  
  XSetClipRectangles(display, gc_sl, 0, 0, rect, 1, Unsorted);
  XSetClipRectangles(display, gc_sd, 0, 0, rect, 1, Unsorted);
 
  points[0].x = points[1].x = points[2].x = x; points[2].x += table->bwidth;
  points[0].y = points[1].y = points[2].y = y; points[0].y += table->bheight;
  XDrawLines(display, window, gc_sl, points, 3, CoordModeOrigin);
  points[1].x = points[2].x; points[1].y = points[0].y;
  XDrawLines(display, window, gc_sd, points, 3, CoordModeOrigin);
 }
  
 gc_field = XCreateGC(display, window, 0, NULL);
 XCopyGC(display, hw->html.drawGC, (unsigned long)-1, gc_field);
 XSetClipRectangles(display, gc_field, 0, 0, rect, 1, Unsorted);

 field = table->table;
 yMarker = y;
 for (yy = 0; yy < table->numRows; yy++)
 {
  xMarker = x;
  rowHeight = field->rowHeight;
  
  for (xx = 0; xx < table->numColumns; xx++)
  {
   colWidth = field->colWidth;
      
   TableGetExpandedDimensions(table, xx, yy, 
			      &expandedWidth, &expandedHeight);
      
   if (yMarker + expandedHeight < hw->html.redraw_y || 
       yMarker > hw->html.redraw_y + hw->html.redraw_height ||
       xMarker + expandedWidth < hw->html.redraw_x || 
       xMarker > hw->html.redraw_x + hw->html.redraw_width)
   {
    xMarker += colWidth;
    field++;
    continue;
   }
   
   if (field->first_rec && field->last_rec)
   {    
    /* draw field borders */
    if (table->borders && !field->contVert && !field->contHoriz)
    {
     points[0].x = points[1].x = xMarker + table->borders + 1;
     points[2].x = xMarker + expandedWidth;
     points[1].y = points[2].y = yMarker + table->borders + 1; 
     points[0].y = yMarker + expandedHeight;
     XDrawLines(display, window, gc_sd, points, 3, CoordModeOrigin);
     points[1].x = points[2].x; points[1].y = points[0].y;
     XDrawLines(display, window, gc_sl, points, 3, CoordModeOrigin);

     /* fill background */
     if (field->background)
     {
      XSetForeground(display, gc_field, field->bgcolor);
      XFillRectangle(display, window, gc_field,
		     xMarker + table->borders + 2,
		     yMarker + table->borders + 2, 
		     expandedWidth - table->borders - 2, 
		     expandedHeight - table->borders - 2);
     }
     else if (table->background)
     {
      XFillRectangle(display, window, gc_bg,
		     xMarker + table->borders + 2,
		     yMarker + table->borders + 2, 
		     expandedWidth - table->borders - 2, 
		     expandedHeight - table->borders - 2);
     }
    }
    else
    {
     /* fill background */
     if (field->background)
     {
      XSetForeground(display, gc_field, field->bgcolor);
      XFillRectangle(display, window, gc_field,
		     xMarker + table->borders,
		     yMarker + table->borders, 
		     expandedWidth - table->borders, 
		     expandedHeight - table->borders);
     }
     else if (table->background)
     {
      XFillRectangle(display, window, gc_bg,
		     xMarker + table->borders,
		     yMarker + table->borders, 
		     expandedWidth - table->borders, 
		     expandedHeight - table->borders);
     }
    }

    {
     XRectangle recto[1];
	  
     if (table->borders)
     {
      recto[0].x = xMarker + table->borders + 2;
      recto[0].y = yMarker + table->borders + 2;
      recto[0].width = expandedWidth - table->borders - 2;
      recto[0].height = expandedHeight - table->borders - 2;
     }
     else
     {
      recto[0].x = xMarker; recto[0].y = yMarker;
      recto[0].width = expandedWidth; recto[0].height = expandedHeight;
     }
     XSetClipRectangles(display, hw->html.drawGC, 
			0, 0, recto, 1, Unsorted);
    }

    /* fill in field */
    rec = field->first_rec;
    do
    {
     switch(rec->type)
     {
     case E_TEXT:
      if (field->background)
       rec->bg = field->bgcolor;
      else if (table->background)
       rec->bg = table->backgroundColor;
      TextRefresh(hw, rec, 0, (rec->edata_len - 2));
      break;
     case E_BULLET:
      BulletRefresh(hw, rec);
      break;
     case E_HRULE:
      HRuleRefresh(hw, rec);
      break;
     case E_IMAGE:
      ImageRefresh(hw, rec);
      break;
     case E_WIDGET:
      WidgetRefresh(hw, rec);
      break;
     case E_TABLE:
      TableRefresh(hw, rec);
      break;
     default:
      break;
     }
	  
     XSetClipRectangles(display, hw->html.drawGC, 0, 0, rect, 1, Unsorted);
     /*	  printf("type: %d\n", rec->type);
	  XFlush(XtDisplay(hw)); getchar(); */
	  
     if (rec == field->last_rec) break;
     rec = rec->next;
    } while(1);
   }
      
   xMarker += colWidth;
   field++;
  }		
    
  yMarker += rowHeight;
 }	
  
 if (table->caption && table->captionAlignment == ALIGN_BOTTOM)
 {
  int divh = FIELD_BORDER_SPACE / 2;
  XFontStruct	*font = table->captionFont;
    
  y += table->bheight + divh + font->max_bounds.ascent;
    
  XSetFont(display, hw->html.drawGC, font->fid);
  XSetForeground(display, hw->html.drawGC, eptr->fg);
  XSetBackground(display, hw->html.drawGC, eptr->bg);

  XDrawString(display, window, hw->html.drawGC, 
	      x + (table->width - XTextWidth(font, table->caption,
					     strlen(table->caption))) / 2,
	      y, table->caption, strlen(table->caption));
 }
  
 if (table->borders)
 {
  XFreeGC(display, gc_sl);
  XFreeGC(display, gc_sd);
 }	  
  
 if (table->background)
  XFreeGC(display, gc_bg);
 XFreeGC(display, gc_field);

 XSetLineAttributes(XtDisplay(hw), hw->html.drawGC, 1, LineSolid,
		    CapNotLast, JoinMiter);
}






