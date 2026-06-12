/*
** Keyboard_Specs.c for Xclamation and XAllWrite in Keyboard/
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
** Started on  Sat Aug  5 18:26:36 1995 Emmanuel Paris
** Last update Fri Jan  7 15:56:19 2000 Emmanuel Paris
*/

#include "Keyboard.h"

#define KEYB_NB_ACUTE	13
static char	tab_acute[KEYB_NB_ACUTE+1] = "aeiouyAEIOUY'";
static KeySym	ks_acute[KEYB_NB_ACUTE] = 
{ XK_aacute, XK_eacute, XK_iacute, XK_oacute, XK_uacute, XK_yacute,
 XK_Aacute, XK_Eacute, XK_Iacute, XK_Oacute, XK_Uacute, XK_Yacute , XK_acute};
static char	kc_acute[KEYB_NB_ACUTE] = 
{ 0xe1, 0xe9, 0xed, 0xf3, 0xfa, 0xfd, 0xc1, 
 0xc9, 0xcd, 0xd3, 0xda, 0xdd, 0xb4 };
#define KEYB_NB_GRAVE 10
static char	tab_grave[KEYB_NB_GRAVE+1] = "aeiouAEIOU";
static KeySym	ks_grave[KEYB_NB_GRAVE] = 
{ XK_agrave, XK_egrave, XK_igrave, XK_ograve, XK_ugrave, 
 XK_Agrave, XK_Egrave, XK_Igrave, XK_Ograve, XK_Ugrave };
static char	kc_grave[KEYB_NB_GRAVE] = 
{ 0xe0, 0xe8, 0xec, 0xf2, 0xf9, 0xc0, 0xc8, 0xcc, 0xd2, 0xd9};
#define KEYB_NB_CIRCUMFLEX 13
static char	tab_circumflex[KEYB_NB_CIRCUMFLEX+1] = "aeiouAEIOU123";
static KeySym	ks_circumflex[KEYB_NB_CIRCUMFLEX] = 
{ XK_acircumflex, XK_ecircumflex, XK_icircumflex, XK_ocircumflex, 
 XK_ucircumflex, XK_Acircumflex, XK_Ecircumflex, XK_Icircumflex, 
 XK_Ocircumflex, XK_Ucircumflex , XK_onesuperior, XK_twosuperior, 
 XK_threesuperior};
static char	kc_circumflex[KEYB_NB_CIRCUMFLEX] = 
{ 0xe2, 0xea, 0xee, 0xf4, 0xfb, 0xc2, 0xca, 
 0xce, 0xd4, 0xdb, 0xb9, 0xb2, 0xb3};
#define KEYB_NB_DIAERESIS 11
static char	tab_diaeresis[KEYB_NB_DIAERESIS+1] = "aeiouyAEIOU";
static KeySym	ks_diaeresis[KEYB_NB_DIAERESIS] = 
{ XK_adiaeresis, XK_ediaeresis, XK_idiaeresis, XK_odiaeresis, 
 XK_udiaeresis, XK_ydiaeresis, XK_Adiaeresis, XK_Ediaeresis, 
 XK_Idiaeresis, XK_Odiaeresis, XK_Udiaeresis };
static char	kc_diaeresis[KEYB_NB_DIAERESIS] = 
{ 0xe4, 0xeb, 0xef, 0xf6, 0xfc, 0xff, 0xc4, 0xcb, 0xcf, 0xd6, 0xdc};
#define KEYB_NB_DIAERESIS2 13
static char	tab_diaeresis2[KEYB_NB_DIAERESIS2+1] = "aeiouyAEIOU:-";
static KeySym	ks_diaeresis2[KEYB_NB_DIAERESIS2] = 
{ XK_adiaeresis, XK_ediaeresis, XK_idiaeresis, XK_odiaeresis, 
 XK_udiaeresis, XK_ydiaeresis, XK_Adiaeresis, XK_Ediaeresis, 
 XK_Idiaeresis, XK_Odiaeresis, XK_Udiaeresis, XK_diaeresis, XK_division};
static char	kc_diaeresis2[KEYB_NB_DIAERESIS2] = 
{ 0xe4, 0xeb, 0xef, 0xf6, 0xfc, 0xff, 0xc4, 
 0xcb, 0xcf, 0xd6, 0xdc, 0xa8, 0xf7};
#define KEYB_NB_TILDE 6
static char	tab_tilde[KEYB_NB_TILDE+1] = "anoANO";
static KeySym	ks_tilde[KEYB_NB_TILDE] = 
{ XK_atilde, XK_ntilde, XK_otilde, 
 XK_Atilde, XK_Ntilde, XK_Otilde };
static char	kc_tilde[KEYB_NB_TILDE] = 
{ 0xe3, 0xf1, 0xf5, 0xc3, 0xd1, 0xd5};
#define KEYB_NB_CEDILLA 5
static char	tab_cedilla[KEYB_NB_CEDILLA+1] = "cCub,";
static KeySym	ks_cedilla[KEYB_NB_CEDILLA] = 
{ XK_ccedilla, XK_Ccedilla , XK_mu, XK_ssharp, XK_cedilla};
static char	kc_cedilla[KEYB_NB_CEDILLA] = 
{ 0xe7, 0xc7, 0xb5, 0xdf, 0xb8 };
#define KEYB_NB_RING 3
static char	tab_ring[KEYB_NB_RING+1] = "aAS";
static KeySym	ks_ring[KEYB_NB_RING] = { XK_aring, XK_Aring, XK_section };
static char	kc_ring[KEYB_NB_RING] = { 0xe5, 0xc5, 0xa7 };
#define KEYB_NB_RING2 5
static char	tab_ring2[KEYB_NB_RING2+1] = "aAS.-";
static KeySym	ks_ring2[KEYB_NB_RING2] = 
{ XK_aring, XK_Aring, XK_section, XK_degree, XK_periodcentered };
static char	kc_ring2[KEYB_NB_RING2] = 
{ 0xe5, 0xc5, 0xa7, 0xb0, 0xb7 };
#define KEYB_NB_SLASH 9
static char	tab_slash[KEYB_NB_SLASH+1] = "coO/ aeAE";
static KeySym	ks_slash[KEYB_NB_SLASH] = 
{ XK_cent, XK_Ooblique, XK_oslash, XK_division, XK_nobreakspace,
 XK_ae, XK_ae, XK_AE, XK_AE};
static char	kc_slash[KEYB_NB_SLASH+1] = 
{ 0xa2, 0xd8, 0xf8, 0xf7, 0xa0, 0xe6, 0xe6, 0xc6, 0xc6 };
#define KEYB_NB_MINUS 6
static char	tab_minus[KEYB_NB_MINUS+1] = "L|-+D:";
static KeySym	ks_minus[KEYB_NB_MINUS] = 
{ XK_sterling, XK_notsign, XK_hyphen, XK_plusminus, XK_Eth, XK_division};
static char	kc_minus[KEYB_NB_MINUS] = 
{ 0xa3, 0xac, 0xad, 0xb1, 0xd0, 0xf7};
#define KEYB_NB_FRACTION 2
static char	tab_fraction[KEYB_NB_FRACTION+1] = "42";
static KeySym	ks_fraction[KEYB_NB_FRACTION] = { XK_onequarter, XK_onehalf};
static char	kc_fraction[KEYB_NB_FRACTION] = { 0xbc, 0xbd};
#define KEYB_NB_UNDERSCORE 3
static char	tab_underscore[KEYB_NB_UNDERSCORE+1] = "_ao";
static KeySym	ks_underscore[KEYB_NB_UNDERSCORE] = 
{ XK_macron, XK_ordfeminine, XK_masculine};
static char	kc_underscore[KEYB_NB_UNDERSCORE] = { 0xaf, 0xaa, 0xba};
#define KEYB_NB_CIRCLE 2
static char	tab_circle[KEYB_NB_CIRCLE+1] = "CR";
static KeySym	ks_circle[KEYB_NB_CIRCLE] = { XK_copyright, XK_registered};
static char	kc_circle[KEYB_NB_CIRCLE] = { 0xa9, 0xae};
#define KEYB_NB_BAR 5
static char	tab_bar[KEYB_NB_BAR+1] = "|cqPp";
static KeySym	ks_bar[KEYB_NB_BAR] = 
{ XK_brokenbar, XK_paragraph, XK_paragraph, XK_Thorn, XK_thorn};
static char	kc_bar[KEYB_NB_BAR] = { 0xa6, 0xb6, 0xb6, 0xde, 0xfe};
#define KEYB_NB_MULTIPLY 4
static char	tab_multiply[KEYB_NB_MULTIPLY+1] = "o*xd";
static KeySym	ks_multiply[KEYB_NB_MULTIPLY] = 
{ XK_currency, XK_multiply, XK_multiply, XK_eth};
static char	kc_multiply[KEYB_NB_MULTIPLY] = { 0xa4, 0xd7, 0xd7, 0xf0};

static char	tab_exclam[1] = { '!' };
static KeySym	ks_exclam[1] = { XK_exclamdown };
static char	kc_exclam[1] = { 0xa1 };
static char	tab_question[1] = { '?' };
static KeySym	ks_question[1] = { XK_questiondown };
static char	kc_question[1] = { 0xbf };
static char	tab_guillemotleft[1] = { '<' };
static KeySym	ks_guillemotleft[1] = { XK_guillemotleft };
static char	kc_guillemotleft[1] = { 0xab };
static char	tab_yen[1] = { 'Y' };
static KeySym	ks_yen[1] = { XK_yen };
static char	kc_yen[1] = { 0xa5 };
static char	tab_guillemotright[1] = { '>' };
static KeySym	ks_guillemotright[1] = { XK_guillemotright };
static char	kc_guillemotright[1] = { 0xbb };
static char	tab_threequarters[1] = { '4' };
static KeySym	ks_threequarters[1] = { XK_threequarters };
static char	kc_threequarters[1] = { 0xbe };

#define KB_Convert(accent, nbchar) \
static boolean	convertion_##accent(c_Keyboard *This) \
{ \
 int	i; \
   \
 if (This->buf_length == 1) \
 { \
   for(i=0; i<nbchar; i++) \
   { \
     if (This->buf_keyb[0] == tab_##accent[i]) \
     { \
	This->keysym = ks_##accent[i]; \
	This->buf_keyb[0] = kc_##accent[i]; \
	This->buf_keyb[1] = '\0'; \
	This->buf_length = 1; \
	return FALSE; \
      } \
    } \
  } \
 return FALSE; \
}

KB_Convert(acute, KEYB_NB_ACUTE);
KB_Convert(grave, KEYB_NB_GRAVE);
KB_Convert(circumflex, KEYB_NB_CIRCUMFLEX);
KB_Convert(diaeresis, KEYB_NB_DIAERESIS);
KB_Convert(diaeresis2, KEYB_NB_DIAERESIS2);
KB_Convert(tilde, KEYB_NB_TILDE);
KB_Convert(cedilla, KEYB_NB_CEDILLA);
KB_Convert(ring, KEYB_NB_RING);
KB_Convert(ring2, KEYB_NB_RING2);
KB_Convert(slash, KEYB_NB_SLASH);
KB_Convert(minus, KEYB_NB_MINUS);
KB_Convert(fraction, KEYB_NB_FRACTION);
KB_Convert(underscore, KEYB_NB_UNDERSCORE);
KB_Convert(circle, KEYB_NB_CIRCLE);
KB_Convert(bar, KEYB_NB_BAR);
KB_Convert(multiply, KEYB_NB_MULTIPLY);

KB_Convert(exclam, 1);
KB_Convert(question, 1);
KB_Convert(guillemotleft, 1);
KB_Convert(yen, 1);
KB_Convert(guillemotright, 1);
KB_Convert(threequarters, 1);

keyboard_specs_t keyboard_specs[] = 
{
 { KB_MODIF_CONTROL, XK_quoteright, convertion_acute},
 { KB_MODIF_CONTROL, XK_quoteleft, convertion_grave},
 { KB_MODIF_CONTROL, XK_asciicircum, convertion_circumflex},
 { KB_MODIF_CONTROL, XK_diaeresis, convertion_diaeresis},
 { KB_MODIF_CONTROL, XK_colon, convertion_diaeresis2},
 { KB_MODIF_CONTROL, XK_asciitilde, convertion_tilde},
 { KB_MODIF_CONTROL, XK_comma, convertion_cedilla},
 { KB_MODIF_CONTROL, XK_KP_Separator, convertion_cedilla},
 { KB_MODIF_CONTROL, XK_degree, convertion_ring},
 { KB_MODIF_CONTROL, XK_period, convertion_ring2},
 { KB_MODIF_CONTROL, XK_KP_Decimal, convertion_ring2},
 { KB_MODIF_CONTROL, XK_slash, convertion_slash},
 { KB_MODIF_CONTROL, XK_KP_Divide, convertion_slash},
 { KB_MODIF_CONTROL, XK_minus, convertion_minus},
 { KB_MODIF_CONTROL, XK_KP_Subtract, convertion_minus},
 { KB_MODIF_CONTROL, XK_1, convertion_fraction},
 { KB_MODIF_CONTROL, XK_KP_1, convertion_fraction},
 { KB_MODIF_CONTROL, XK_underscore, convertion_underscore},
 { KB_MODIF_CONTROL, XK_0, convertion_circle},
 { KB_MODIF_CONTROL, XK_KP_0, convertion_circle},
 { KB_MODIF_CONTROL, XK_at, convertion_circle},
 { KB_MODIF_CONTROL, XK_bar, convertion_bar},
 { KB_MODIF_CONTROL, XK_asterisk, convertion_multiply},
 { KB_MODIF_CONTROL, XK_KP_Multiply, convertion_multiply},
  
 { KB_MODIF_CONTROL, XK_exclam, convertion_exclam},
 { KB_MODIF_CONTROL, XK_question, convertion_question},
 { KB_MODIF_CONTROL, XK_less, convertion_guillemotleft},
 { KB_MODIF_CONTROL, XK_equal, convertion_yen},
 { KB_MODIF_CONTROL, XK_greater, convertion_guillemotright},
 { KB_MODIF_CONTROL, XK_3, convertion_threequarters},
 { KB_MODIF_CONTROL, XK_KP_3, convertion_threequarters},
 { KB_MODIF_END, 0, NULL}
};













