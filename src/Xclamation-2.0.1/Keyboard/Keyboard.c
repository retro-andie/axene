/*
** Keyboard.c for Xclamation and XAllWrite in Keyboard/
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
** Started on  Sat Aug  5 18:26:36 1995 Emmanuel Paris
** Last update Fri Jan  7 14:55:51 2000 Emmanuel Paris
*/

#define NTRACE

#include "Keyboard.h"
#define KBD_CONTEXT_MENU

#ifdef KBD_CONTEXT_MENU
#include "ContextMenu.h"
#endif

#define Xc_TRACE_MODIFIER(message) \
Xc_TRACE(("%s: MODIFIER: %c%c%c%c%c%c%c%c", message, \
	  This->Shift ? 'S' : '-', This->Lock ? 'L' : '-', \
	  This->Control ? 'C' : '-', This->Mod1 ? '1' : '-', \
	  This->Mod2 ? '2' : '-', This->Mod3 ? '3' : '-', \
	  This->Mod4 ? '4' : '-', This->Mod5 ? '5' : '-'))

static void *cons_Keyboard();
static void dest_Keyboard();
static void *copy_Keyboard();
static void eventlook_Keyboard();
static void queued_mode_Keyboard();
static void test_mode_Keyboard();

static void concat_to_queued_keyb();
static void concat_to_queued_keyc();

sf_Keyboard fc_Keyboard =
{
 cons_Keyboard,
 dest_Keyboard,
 copy_Keyboard,
 eventlook_Keyboard,
 queued_mode_Keyboard,
 create_Box_Insert,
 get_one_char_Box_Insert
};

static void *cons_Keyboard(display, window)
Display	*display;
Window	window;
{
 int	i, j, length;
 char	keyb_vector[32];
 char	*atom, filler;
 KeyCode		keycode;
 int	min_keycode, max_keycode;
 c_Keyboard *This;
 XModifierKeymap	*xmodmap;
 XSetWindowAttributes	set_attrib;
 XWindowAttributes	get_attrib;
  
 Xc_HISTORY(("constructor"));

 if((This=(c_Keyboard *)Xc_malloc("Keyboard",sizeof(c_Keyboard)))==NULL)
  return NULL;

 This->f= &fc_Keyboard;  
 This->display = display;
 This->window = window;
 This->buf_length = 0;
 This->buf_keyb = This->buf_keybs;
 This->insertBox = FALSE;
 This->event_enable = TRUE;
 This->keyb_box.Dialog = NULL;

 This->queued_buf = FALSE;
 This->qbuf_keyb = This->qbuf_keybs;
 This->qbuf_keyc = This->qbuf_keycs;
 This->qbuf_blng = This->qbuf_clng = 0;

 XGetWindowAttributes(display, window, &get_attrib);
 set_attrib.event_mask = get_attrib.all_event_masks | KeymapStateMask;
 XChangeWindowAttributes(display, window, CWEventMask, &set_attrib);
  
 XDisplayKeycodes(display, &min_keycode, &max_keycode);
 This->first_keycode = min_keycode;

 This->keyb_tab = 
  (keyboard_type_t *)Xc_malloc("keyb tab", sizeof(keyboard_type_t) * 
			       (max_keycode - min_keycode + 1));
 memset(This->keyb_tab, 0, sizeof(keyboard_type_t) * 
	(max_keycode - min_keycode + 1));
  
 xmodmap = XGetModifierMapping(display);
 This->max_keypermod = xmodmap->max_keypermod;

 for(i=0; i<8; i++)
 {
  This->keyb_mode[i] = 
   (keyboard_mode_t *)Xc_malloc("kb modif", sizeof(keyboard_mode_t) *
				This->max_keypermod);
  for(j = 0; j < xmodmap->max_keypermod ; j++)
  {
   keycode = xmodmap->modifiermap[i*xmodmap->max_keypermod+j];	
   This->keyb_mode[i][j].keycode = keycode;
   if (keycode >= This->first_keycode)
   {
    This->keyb_tab[keycode - This->first_keycode] = KeyCode_Mode;
    This->keyb_mode[i][j].keyb_offset = This->keyb_mode[i][j].keycode >> 3;
    This->keyb_mode[i][j].keyb_bit = 1 << (This->keyb_mode[i][j].keycode & 7);
   }
   else
    This->keyb_mode[i][j].keyb_offset = This->keyb_mode[i][j].keyb_bit = 0;

   This->keyb_mode[i][j].keysym = 
    XKeycodeToKeysym(display, This->keyb_mode[i][j].keycode, 0);
  }
 }
 XFreeModifiermap(xmodmap);
 XQueryKeymap(display, keyb_vector);
 test_mode_Keyboard(This, keyb_vector);
  
 This->compose = This->start_comp = FALSE;
 i = 0;
 while( keyboard_specs[i].modifier != KB_MODIF_END)
 {
  keycode = XKeysymToKeycode(display, keyboard_specs[i].keysym);
  if (keycode >= This->first_keycode)
   This->keyb_tab[keycode - This->first_keycode] = KeyCode_Specs;
  i++;
 }

 filler = '\0';
 for(i=0; i<8; i++)
 {
  atom = XFetchBuffer(display, &length, i);
  if (length == 0 || atom == NULL)
   XStoreBuffer(display, &filler, 1, i);
 }
 Xc_mcheck();
 Xc_TRACE(("constructor ok"));
 return This;
}

static void dest_Keyboard(This)
c_Keyboard	*This;
{
 int i;
  
 if (This->buf_keyb != This->buf_keybs)
  Xc_free(This->buf_keyb);
 Xc_free(This->keyb_tab);
 for(i=0; i<8; i++)
  Xc_free(This->keyb_mode[i]);
 Xc_free(This);
}

static void *copy_Keyboard(This)
c_Keyboard	This;
{
 Xc_TRACE(("copy Keyboard function not done.normally not usefull."));
 return NULL;
}

static void eventlook_Keyboard(This, event)
c_Keyboard	*This;
XEvent		event;
{
 int		nb_key;
 int		i, j, length;
 char		*atom;
#ifdef KBD_CONTEXT_MENU
 char		atoms[8][45];
 c_ContextMenu *context;
 int		k, ret_button;
 static char	cnv[] = 
  "^@^A^B^C^D^E^F\\a\\b\\t\\n^K\\f\\n^N^O^P^Q^R^S^T^U^V^W^X^Y^Z\\e^\\^]^^^_";
#endif
 boolean	*mode;
 KeyCode	keycode;
 KeySym		keysym;
 XComposeStatus sequence;

 switch(event.type)
 {
 case ButtonPress:
  if (event.xbutton.button == 1)
  {
   if (This->buf_keyb != This->buf_keybs)
   {
    Xc_free(This->buf_keyb);
    This->buf_keyb = This->buf_keybs;
   }
   
#ifdef KBD_CONTEXT_MENU
   if (This->Control)
   {
    for(i = 0; i < 8; i++)
    {
     atom = XFetchBuffer(This->display, &length, i);
     sprintf(atoms[i], "%d: ", i+1);
     j = 3; k = 0;
     while(j < 43 && atom[k] !='\0')
     {
      if (atom[k] < 32)
      {
       atoms[i][j++] = cnv[atom[k] << 1];
       atoms[i][j] = cnv[(atom[k] << 1) + 1];
      }
      else
      {
       atoms[i][j] = atom[k];
      }
      j++; k++;
     }
     atoms[i][j] = '\0';
     XFree(atom);
    }
    context = (c_ContextMenu *)
     NEW(c_ContextMenu)(This->display, CM_KEYBOARD, &ret_button, 
			FALSE, &event, atoms);
	  
    if (context)
    {
     F(context).waitForUnmap(context);
     DELETE(c_ContextMenu)(context);
	    
     Xc_TRACE(("ret_button: %d", ret_button));
     if (ret_button <= 8 && ret_button >0)
     {
      XRotateBuffers(This->display, 9 - ret_button);
      event.type = ButtonRelease;	/* to fuck motif */
      XPutBackEvent(This->display, &event);
      event.type = ButtonPress;
     }
     else
     {
      This->buf_length = 0;
      return;
     }
    }
    else
    {
     Xc_TRACE(("Insert Box"));
     This->buf_length = 0;
     if (This->keyb_box.Dialog == NULL)
      This->insertBox = TRUE;
     break;
    }
   }
#else
   if (This->Control)
   {
    Xc_TRACE(("Insert Box"));
    This->buf_length = 0;
    if (This->keyb_box.Dialog == NULL)
      This->insertBox = TRUE;
    break;
   }
#endif
   else
   {
    This->buf_length = 0;
    return;
   }

   atom = XFetchBytes(This->display, &length);
   if (length == 0)
   { 
    This->buf_length = 0;
    return;
   }
   for(i=0; i < length; i++)
    if (atom[i] == '\0') 
     break;
   if (i == length)
   {
    This->buf_keyb = Xc_malloc("buf kba", length + 1);
    memcpy(This->buf_keyb, atom, length);
    This->buf_keyb[length] = '\0';
   }
   else
   {
    strcpy(This->buf_keyb, atom);
   }
   This->buf_length = i;
   if (This->queued_buf) concat_to_queued_keyb(This);
   
   XFree(atom);
   Xc_TRACE(("Copy Buffer: %s", This->buf_keyb));
  }
  break;
 case KeyPress:
  if (This->buf_keyb != This->buf_keybs)
  {
   Xc_free(This->buf_keyb);
   This->buf_keyb = This->buf_keybs;
  }
  nb_key = XLookupString((XKeyEvent *)&event, This->buf_keyb, 
			 KEYB_BUFFER_SIZE, &keysym, &sequence);
  This->buf_length = nb_key;
  if (nb_key)
  {
   if (nb_key == KEYB_BUFFER_SIZE)
    Xc_WARNING(("Incorrect KeyPress Event"));
   else
   {
    This->buf_keyb[nb_key] = '\0';
   }
  }
  This->compose = FALSE;
  if (This->start_comp && This->buf_length)
  {
   if (!keyboard_specs[This->comp_specs].convertion(This))
   {
    This->start_comp = FALSE;
   }
  }
    
  keycode = event.xkey.keycode;
  switch(This->keyb_tab[keycode - This->first_keycode])
  {
  case KeyCode_Mode:
   mode = &This->Shift;
   for(i = 0; i < 8; i++)
   {
    for(j = 0; j < This->max_keypermod; j++)
    {
     if (This->keyb_mode[i][j].keycode == keycode)
      mode[i] = TRUE;
    }
   }  
   break;
  case KeyCode_Specs:
   i = 0;
   mode = &This->Shift;
   while( keyboard_specs[i].modifier != KB_MODIF_END)
   {
    if (mode[keyboard_specs[i].modifier] == TRUE && 
	keysym == keyboard_specs[i].keysym)
    {
     This->compose = TRUE;
     This->start_comp = FALSE;
     This->comp_specs = i;
     This->buf_keyb[0] = '\0';
     This->buf_length = 0;
     event.xkey.keycode = 0;
     break;
    }
    i++;
   }
  case KeyCode_Normal:
   if (This->Mod1 == TRUE)	/* skip key when Alt is pressed */
   {
    This->buf_keyb[0] = '\0';
    This->buf_length = 0;
   }
   break;
  }
  
  if (This->queued_buf)
  {
   if (This->buf_length && (unsigned char)This->buf_keyb[0] >= ' ' &&
       This->buf_keyb[0] != 127)  /* XK_Delete provide this case ! */
    concat_to_queued_keyb(This);
   else
    concat_to_queued_keyc(This, keycode);
  }
  
#ifndef NTRACE
  if (This->buf_length)
   Xc_TRACE(("KeyPress: %s  ascii: %d length: %d", 
	     This->buf_keyb, This->buf_keyb[0], This->buf_length));

  if (This->buf_keyb[0] == 'm')
  {
   int i, j;
   
   for(i = 0; i < 8; i++)
   {
    printf("Modifier: %d:\n", i);
    for(j = 0; j < This->max_keypermod; j++)
    {
     printf("\t%d) keycode: %d key_off: %d key_bit: %d\n", j, 
	    This->keyb_mode[i][j].keycode, This->keyb_mode[i][j].keyb_offset,
	    This->keyb_mode[i][j].keyb_bit);
    }
   }
  }

#endif
  break;
 case KeyRelease:
  keycode = event.xkey.keycode;
  switch(This->keyb_tab[keycode-This->first_keycode])
  {
  case KeyCode_Mode:
   mode = &This->Shift;
   for(i=0; i<8; i++)
   {
    for(j=0; j<This->max_keypermod; j++)
    {
     if (This->keyb_mode[i][j].keycode == keycode)
      mode[i] = FALSE;
    }
   }  
   break;
  case KeyCode_Specs:
   if (This->compose) 
    This->start_comp = TRUE;
   break;
  case KeyCode_Normal:
   break;
  }
  break;
 case KeymapNotify:
  Xc_TRACE_MODIFIER("before keymap");
  test_mode_Keyboard(This, event.xkeymap.key_vector);
  Xc_TRACE_MODIFIER("after keymap");
  break;
 case MappingNotify:
  XRefreshKeyboardMapping((XMappingEvent *)&event);
  break;
 case EnterNotify:
  {
   Window    root, fille;
   int       xrac, yrac, xm, ym;
   unsigned int      mod_button;
      
   XQueryPointer(This->display, event.xcrossing.window, &root, &fille,
		     &xrac, &yrac, &xm, &ym, &mod_button);
   
   This->Shift = ((mod_button & ShiftMask) != 0);
   This->Lock = ((mod_button & LockMask) != 0);
   This->Control = ((mod_button & ControlMask) != 0);
   This->Mod1 = ((mod_button & Mod1Mask) != 0);
   This->Mod2 = ((mod_button & Mod2Mask) != 0);
   This->Mod3 = ((mod_button & Mod3Mask) != 0);
   This->Mod4 = ((mod_button & Mod4Mask) != 0);
   This->Mod5 = ((mod_button & Mod5Mask) != 0);
  }
  break;
 default:
  break;
 }
}

static void queued_mode_Keyboard(This, queued)
c_Keyboard	*This;
boolean		queued;
{
 if (This->queued_buf == queued) return;

 This->queued_buf = queued;
 if (This->qbuf_keyb != This->qbuf_keybs)
 {
  Xc_free(This->qbuf_keyb);
  This->qbuf_keyb = This->qbuf_keybs;
 }
 
 if (This->qbuf_keyc != This->qbuf_keycs)
 {
  Xc_free(This->qbuf_keyc);
  This->qbuf_keyc = This->qbuf_keycs;
 }

 This->qbuf_blng = This->qbuf_clng = 0;
}

static void test_mode_Keyboard(This, key_vector)
c_Keyboard	*This;
char		*key_vector;
{
 int	i,j;
 boolean	*mode;

#ifndef NTRACE
 printf("bits          |7|6|5|4|3|2|1|0|\n");
 printf("--------------+-+-+-+-+-+-+-+-+\n");
 for(i = 0; i < 32; i++)
 {
  printf("byte: %2d      |", i);
  for(j = 128; j > 0; j >>= 1)
   printf("%c|", ((key_vector[i] & j) == 0) ? ' ' : '*');
  printf("\n");
 }
#endif

 mode = &This->Shift;
 for(i=0; i<8; i++)
 {
  mode[i] = FALSE;
  for(j=0; j<This->max_keypermod; j++)
  {
   mode[i] |= (key_vector[This->keyb_mode[i][j].keyb_offset] & 
	       This->keyb_mode[i][j].keyb_bit) != 0;
  }
 }  
}

static void concat_to_queued_keyb(This)
c_Keyboard	*This;
{
 int space;

 if (This->buf_length == 0)
  return;
 
 space = (This->qbuf_blng / KEYB_BUFFER_SIZE + 1) * KEYB_BUFFER_SIZE;
 
 if (This->qbuf_blng + This->buf_length >= space)
 {
  if (This->qbuf_keyb == This->qbuf_keybs)
   This->qbuf_keyb = (char *)Xc_malloc("qb", space + KEYB_BUFFER_SIZE);
  else
   This->qbuf_keyb = (char *)Xc_realloc("qb", This->qbuf_keyb, 
					space + KEYB_BUFFER_SIZE);
 }

 memcpy(This->qbuf_keyb + This->qbuf_blng, This->buf_keyb, This->buf_length);
 This->qbuf_blng += This->buf_length;
 This->qbuf_keyb[This->qbuf_blng] = '\0';
}

static void concat_to_queued_keyc(This, keycode)
c_Keyboard	*This;
KeyCode		keycode;
{
 int space;

 space = (This->qbuf_clng / KEYB_BUFFER_SIZE + 1) * KEYB_BUFFER_SIZE;
 
 if (This->qbuf_clng + 1 >= space)
 {
  if (This->qbuf_keyc == This->qbuf_keycs)
   This->qbuf_keyc = (unsigned char *)Xc_malloc("qc", 
						(space + KEYB_BUFFER_SIZE) 
						* sizeof(KeyCode));
  else
   This->qbuf_keyc = (unsigned char *)Xc_realloc("qc", This->qbuf_keyc, 
						 (space + KEYB_BUFFER_SIZE)
						 * sizeof(KeyCode));
 }
 
 This->qbuf_keyc[This->qbuf_clng] = keycode;
 This->qbuf_clng++;

 space = (This->qbuf_blng / KEYB_BUFFER_SIZE + 1) * KEYB_BUFFER_SIZE;
 
 if (This->qbuf_blng + 1 >= space)
 {
  if (This->qbuf_keyb == This->qbuf_keybs)
   This->qbuf_keyb = (char *)Xc_malloc("qb", space + KEYB_BUFFER_SIZE);
  else
   This->qbuf_keyb = (char *)Xc_realloc("qb", This->qbuf_keyb, 
					space + KEYB_BUFFER_SIZE);
 }

 This->qbuf_keyb[This->qbuf_blng] = '\1';
 This->qbuf_blng++;
}









