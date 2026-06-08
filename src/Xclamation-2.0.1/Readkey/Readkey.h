/*
** Readkey.h for Xclamation, XQuad and XAllWrite in Readkey/
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
** Started on  Thu Jul 11 17:05:21 1996 Antoine Buat
** Last update Tue Oct 20 18:38:55 1998 Emmanuel Paris
*/

#ifndef __XC_READKEY_H__
#define __XC_READKEY_H__

#include "xcalibur.h"
#include "file.h"
#include "DocFile.h"
#include "Decrypt.h"

#define	Xc_OFFICE_KEY_FILE "axeneofficekey"

#ifdef Xc_XCLAMATION
#define	Xc_KEY_FILE "xclamationkey"
#define Xc_KEY_SOFT 0
#endif

#ifdef Xc_XQUAD
#define	Xc_KEY_FILE "xquadkey"
#define Xc_KEY_SOFT 1
#endif

#ifdef Xc_XALLWRITE
#define	Xc_KEY_FILE "xallwritekey"
#define Xc_KEY_SOFT 2
#endif

enum 
{
 Xc_KEY_GOOD,
 Xc_KEY_NO_KEY,
 Xc_KEY_NOT_VALID,
 Xc_KEY_EXPIRED,
};

struct key_info
{
 int	valid;
 int	uniqueid;
 char	*name;
 char	*society;
 int	soft;
 int	type;
 int	nbr_license;
 time_t	date;
 char	*key;
};

char *crypt();

#ifdef __XC_MAIN__
int		XcKEY_OK;
struct key_info	gkinfo;
#else
extern int		XcKEY_OK;
extern struct key_info	gkinfo;
#endif

extern char *Globprog_name;

#define SET_ZERO(k) \
{ \
    int	macrob = 0; \
    unsigned int macrob2; \
    \
    macrob |= (20 * __LINE__); \
    macrob ^= ~__LINE__; \
    macrob2 = 1 >> (unsigned int)macrob; \
    macrob += (__LINE__ >> 1); \
    macrob2 = 1 >> (unsigned int)(++macrob); \
    k = macrob2; \
}

/*
#define XcDO_IF_KEY_OK(_key_func) \
if(XcKEY_OK) \
 _key_func; 
*/

#define XcDO_IF_KEY_OK(_key_func) \
if (XcKEY_OK) \
{ \
  int	salt_number; \
  char	salt[3]; \
  char	crypt_buffer[20]; \
  char	crypt_buffer2[20]; \
  char	*crypt_ret; \
  char	*decrypted_key; \
  int	i, k, l; \
  int	take; \
  take = rand()%4+1; \
  switch(take) \
    { \
    case 1: \
    case 2: \
    case 3: \
    case 4: \
       Xc_TRACE(("test soft")); \
       if ((gkinfo.soft == 3) || (Xc_KEY_SOFT == gkinfo.soft)) \
	SET_ZERO(take); \
      break; \
   } \
  bzero(crypt_buffer, 20); \
  bzero(crypt_buffer2, 20); \
  salt_number = strlen(gkinfo.name) * gkinfo.name[0] - strlen(gkinfo.society);\
  sprintf(salt, "%2x", salt_number); \
  sprintf(crypt_buffer, "%x", gkinfo.uniqueid); \
  crypt_ret = crypt(crypt_buffer, salt); \
  strcpy(crypt_buffer, crypt_ret); \
  sprintf(crypt_buffer2, "303"); \
  crypt_ret = crypt(crypt_buffer2, salt); \
  strcpy(crypt_buffer2, crypt_ret); \
  i=0; \
  while (crypt_buffer[i]) \
    { \
      if (crypt_buffer[i] < crypt_buffer2[i]) \
	crypt_buffer[i] = crypt_buffer2[i]; \
      i++; \
    } \
  sprintf(crypt_buffer2, "%d%d", gkinfo.soft, gkinfo.type); \
  crypt_ret = crypt(crypt_buffer2, salt); \
  strcpy(crypt_buffer2, crypt_ret); \
  i=0; \
  while (crypt_buffer[i]) \
    { \
      crypt_buffer[i] = (crypt_buffer2[i] + crypt_buffer[i]) / 2; \
      i++; \
    } \
  decrypted_key = xc_decrypt(gkinfo.key, (gkinfo.type ? 1 : 0), (gkinfo.soft == 3) ? 1 : 0); \
  bzero(crypt_buffer2, 20); \
  i = 0; \
  SET_ZERO(l); \
  SET_ZERO(k); \
  while (gkinfo.name[i] || gkinfo.society[l]) \
    { \
      int	res; \
      int	div; \
      if (k > 11) \
	k = 0; \
      res = 0; \
      SET_ZERO(div); \
      if (gkinfo.name[i]) \
	{ \
	  res += gkinfo.name[i]; \
	  i++; \
	  div++; \
	} \
      if (crypt_buffer2[k]) \
	{ \
	  res += crypt_buffer2[k]; \
	  div++; \
	} \
      if (gkinfo.society[l]) \
	{ \
	  res += gkinfo.society[l]; \
	  l++; \
	  div++; \
	} \
      crypt_buffer2[k++] ^= res / div; \
    } \
  crypt_ret = crypt(crypt_buffer2, salt); \
  strcpy(crypt_buffer2, crypt_ret); \
  i=0; \
  while (crypt_buffer[i]) \
    { \
      crypt_buffer[i] = (crypt_buffer2[i] + crypt_buffer[i])/2; \
      i++; \
    } \
  sprintf(crypt_buffer2, "%lx", gkinfo.date); \
  crypt_ret = crypt(crypt_buffer2, salt); \
  strcpy(crypt_buffer2, crypt_ret); \
  SET_ZERO(i); \
  while (crypt_buffer[i]) \
    { \
      crypt_buffer[i] = (crypt_buffer2[i] + crypt_buffer[i])/2; \
      i++; \
    } \
   SET_ZERO(i); \
   if (!(strcmp(crypt_buffer, decrypted_key) + take + i) && XcKEY_OK) \
    _key_func; \
 }
 

#define XcDO_IF_NO_KEY(_key_func) \
if(!XcKEY_OK) \
 _key_func; 

/*--- Protoypes ---*/
extern void	read_key_file ___PROTO((char **pkey));
extern int	get_info_from_key ___PROTO((struct key_info *ninfo, 
					    char *buffer));
extern boolean	check_key_validity ___PROTO((struct key_info *kinfo, 
					     int client, char *display));
extern int	display_to_host ___PROTO((char **display));

#endif



