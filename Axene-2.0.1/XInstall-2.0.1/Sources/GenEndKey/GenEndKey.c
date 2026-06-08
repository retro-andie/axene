/*
** GenEndKey.c for XInstall in GenEndKey/
** Generate the Keys for installed softwares.
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Fri Jun 27 16:08:13 1997 Emmanuel Paris
** Last update Fri Jun  4 20:01:27 1999 Emmanuel Paris
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include "xcalibur.h"
#include "Encrypt.h"

#define FREE_PORT 8410

static char crypt_buffer[20];
static char *hex_conv = "0123456789abcdef";
static int soft_conv_id[] = { 3, 0, 1, 2 };

static char key_file[300];

static char key_filen1[] =		/* xclamationkey */
{ 'x' + 128, 'c' + 128, 'l' + 128, 'a' + 128, 'm' + 128, 'a' + 128,
   't' + 128, 'i' + 128, 'o' + 128, 'n' + 128, 'k' + 128, 'e' + 128, 
   'y' + 128, 128 };

static char key_filen2[] =		/* xquadkey */ 
{ 'x' + 128, 'q' + 128, 'u' + 128, 'a' + 128, 'd' + 128, 'k' + 128, 
   'e' + 128, 'y' + 128, 128 };

static char key_filen3[] =		/* xallwritekey */
{ 'x' + 128, 'a' + 128, 'l' + 128, 'l' + 128, 'w' + 128, 'r' + 128, 
   'i' + 128, 't' + 128, 'e' + 128, 'k' + 128, 'e' + 128, 'y' + 128, 128 };

static char key_filen4[] =		/* axeneofficekey */
{ 'a' + 128, 'x' + 128, 'e' + 128, 'n' + 128, 'e' + 128, 'o' + 128, 
   'f' + 128, 'f' + 128, 'i' + 128, 'c' + 128, 'e' + 128, 'k' + 128,
   'e' + 128, 'y' + 128, 128 };

static char *key_filename[4] = { key_filen1, key_filen2, key_filen3, key_filen4 };

static char key_struct[] = ")w1)h1)w1)h1)w1)h1)h1)4<p|1)w\16";

struct info
{
 int	uniqueid;
 char	*name;
 char	*society;
 int	soft;
 int	type;
 time_t	date;
};

void gen_end_key ___PROTO((char *cdkey, char *name, char *society,
			   char *display, char *installdir));
static void conv_7_bits ___PROTO((char *str, char *ret_str));
static int calcul_uniqueid ___PROTO((char *cdkey));
static int get_soft ___PROTO((char *cdkey));
static int get_type ___PROTO((char *cdkey));
static char *crypt_it ___PROTO((struct info *ninfo));
static char *decode_soft_name ___PROTO((int soft_id));
static char *print_uniqueid ___PROTO((long int num));

extern boolean GlobLiveOpt;
extern boolean GlobKeyOpt;
extern int seed;
extern char *crypt();

void gen_end_key(cdkey, name, society, display, installdir)
char *cdkey;
char *name;
char *society;
char *display;
char *installdir;
{
 FILE *file;
 char fname[100];
 char fsociety[100];
 char hostname[100];
 struct info ninfo;
 int	i, soft;
 char	*sZcrypt;

 for(i = 0; i < (int)strlen(key_struct); i++)
  key_struct[i] -= 4;
 
 if (!name) name = "";
 if (!society) society = "";

 conv_7_bits(name, fname);
 conv_7_bits(society, fsociety);
 gethostname(hostname, 100);
 soft = get_soft(cdkey);

 ninfo.uniqueid = calcul_uniqueid(cdkey);
 ninfo.name = name;
 ninfo.society = society;
 ninfo.type = get_type(cdkey);
 ninfo.date = (ninfo.type ? 0 : time(NULL));
 
 ninfo.soft = soft_conv_id[soft];
 sZcrypt = crypt_it(&ninfo);
  
 if (GlobLiveOpt && !GlobKeyOpt)
 {
  char *home = getenv("HOME");
   
  if (home == NULL)
  {
   Xc_WARNING(("$HOME not defined."));
   return;
  }
   
  sprintf(key_file, "%s/.%s", home, decode_soft_name(soft_conv_id[soft]));
 }
 else
  sprintf(key_file, "%s/lib/%s", installdir, 
	  decode_soft_name(soft_conv_id[soft]));
  
 Xc_TRACE(("Writing key file: %s.", key_file));
  
 file = fopen(key_file, "wb");
 if (!file)
 {
  Xc_TRACE(("Write key failed"));
  return;
 }
  
 fprintf(file, key_struct,
	 print_uniqueid(ninfo.uniqueid),
	 strlen(name), fname,
	 strlen(society), fsociety,
	 ninfo.soft,
	 ninfo.type,
	 ninfo.date,
	 sZcrypt);

 fclose(file);
}

static void conv_7_bits(str, ret_str)
char *str;
char *ret_str;
{
 int i, j, k, m;
 char tmp[100];
    
 if (!str || !*str)
 {
  *ret_str = '\0';
  return;
 }

 i = 0;
 while(str[i] && str[i] == ' ') i++;

 k = strlen(str) - 1;
 while(k && str[k] == ' ') k--;
 
 if (i >= k)
 {
  *ret_str = '\0';
  return;
 }
 
 j = m = 0;
 for(; i <= k; i++)
 {
  if ((unsigned char)str[i] <= ' ')
  {
   ret_str[j++] = '\\';  ret_str[j++] = '_';
   tmp[m++] = ' ';
  }
  else
  {
   if (str[i] == '\\')
   {
    ret_str[j++] = '\\';  ret_str[j++] = '\\';
   }
   else
   {
    if ((unsigned char)str[i] >= 128)
    {
     ret_str[j++] = '\\';
     ret_str[j++] = hex_conv[((str[i] & 0xf0) >> 4)];
     ret_str[j++] = hex_conv[(str[i] & 0xf)];
    }
    else
    {
     ret_str[j++] = str[i];
    }
   }
   tmp[m++] = str[i];
  }
 }
 ret_str[j] = '\0';
 tmp[m] = '\0';
 memcpy(str, tmp, m+1);
}

static int calcul_uniqueid(cdkey)
char *cdkey;
{
 int value = 0, i;
 
 for(i = 5; i < 13; i++)
 {
  value = value * 10 + (cdkey[i] - '0');
 }
 return value;
}

static int get_soft(cdkey)
char *cdkey;
{
 return cdkey[2] - '0';
}

static int get_type(cdkey)
char *cdkey;
{
 return ((cdkey[0] - '0') * 10) + cdkey[1] - '0';
}

static char *crypt_it(ninfo)
struct info	*ninfo;
{
 int	salt_number;
 char	salt[3];
 char	crypt_buffer2[20];
 char	*crypt_ret;
 int	i, k, l;
  
 for (i = 0; i<20; i++)
 {
  crypt_buffer[20]=0;
  crypt_buffer2[20]=0;
 }
 Xc_TRACE(("**********Mixing crypt begin**********"));
 Xc_TRACE(("***salt = (len(name)+len(disp_name)*len(serv_name)+name[0]+serv_name[0]-strlen(ninfo->society"));
 salt_number = strlen(ninfo->name) * ninfo->name[0] - strlen(ninfo->society);
 sprintf(salt, "%x", salt_number);
 Xc_TRACE(("salt: %s", salt));
  
 Xc_TRACE(("*******part one: use [uniqueid]******"));
 Xc_TRACE(("uniqueid: %x", ninfo->uniqueid));
 sprintf(crypt_buffer, "%x", ninfo->uniqueid);
 crypt_ret = crypt(crypt_buffer, salt);
 strcpy(crypt_buffer, crypt_ret);

 /*--- use 303 for old hostid ---*/ 
 sprintf(crypt_buffer2, "303");
 crypt_ret = crypt(crypt_buffer2, salt);
 strcpy(crypt_buffer2, crypt_ret);
 Xc_TRACE(("crypt_buffer: %s", crypt_buffer));
 Xc_TRACE(("crypt_buffer2: %s", crypt_buffer2));
 i=0;
 while (crypt_buffer[i])
 {
  if (crypt_buffer[i] < crypt_buffer2[i])
   crypt_buffer[i] = crypt_buffer2[i];
  i++;
 }
 Xc_TRACE(("result: %s", crypt_buffer));

 Xc_TRACE(("*******part two: use [(soft, type), old]******"));
 sprintf(crypt_buffer2, "%d%d", ninfo->soft, ninfo->type);
 crypt_ret = crypt(crypt_buffer2, salt);
 strcpy(crypt_buffer2, crypt_ret);
 Xc_TRACE(("crypt_buffer: %s", crypt_buffer));
 Xc_TRACE(("crypt_buffer2: %s", crypt_buffer2));
 i=0;
 while (crypt_buffer[i])
 {
  crypt_buffer[i] = (crypt_buffer2[i] + crypt_buffer[i])/2;
  i++;
 }
 Xc_TRACE(("result: %s", crypt_buffer));

 Xc_TRACE(("*******part three: use [(name, society), old]******"));
 bzero(crypt_buffer2, 20);
 i = 0;
 k = 0;
 l = 0;
 while (ninfo->name[i] || ninfo->society[l])
 {
  int	res;
  int	div;

  if (k > 11)
   k = 0;
  res = 0;
  div = 0;
  if (ninfo->name[i])
  {
   res += ninfo->name[i];
   i++;
   div++;
  }
  if (crypt_buffer2[k])
  {
   res += crypt_buffer2[k];
   div++;
  }
  if (ninfo->society[l])
  {
   res += ninfo->society[l];
   l++;
   div++;
  }
  crypt_buffer2[k++] ^= res / div;
 }
 crypt_ret = crypt(crypt_buffer2, salt);
 strcpy(crypt_buffer2, crypt_ret);
 Xc_TRACE(("crypt_buffer: %s", crypt_buffer));
 Xc_TRACE(("crypt_buffer2: %s", crypt_buffer2));
 i = 0;
 while (crypt_buffer[i])
 {
  crypt_buffer[i] = (crypt_buffer2[i] + crypt_buffer[i])/2;
  i++;
 }
 Xc_TRACE(("result: %s", crypt_buffer));

 Xc_TRACE(("*******part four: use [(date), old]******"));
 sprintf(crypt_buffer2, "%lx", ninfo->date);
 crypt_ret = crypt(crypt_buffer2, salt);
 strcpy(crypt_buffer2, crypt_ret);
 Xc_TRACE(("crypt_buffer: %s", crypt_buffer));
 Xc_TRACE(("crypt_buffer2: %s", crypt_buffer2));
 i=0;
 while (crypt_buffer[i])
 {
  crypt_buffer[i] = (crypt_buffer2[i] + crypt_buffer[i])/2;
  i++;
 }
 Xc_TRACE(("result: %s", crypt_buffer));
  
 crypt_ret = xc_encrypt(crypt_buffer, ninfo->soft, seed);
 Xc_TRACE(("*******final crypt******"));
 Xc_TRACE(("crypted pass (seed %d): %s ", seed, crypt_ret));
 return crypt_ret;
}

static char *decode_soft_name(soft_id)
int soft_id;
{
 char *ptr;
 
 ptr = key_filename[soft_id];
 if (*ptr == 'x') return ptr;
 do
 {
  *ptr -= 128;
 }
 while(*ptr++);
 
 return key_filename[soft_id];
}

static char puid[7];
static char *print_uniqueid(num)
long int num;
{
 char	buffer[7];
 int	i;
 int	tmp_num;
 char	*ret;

 buffer[6] = '\0';
 i = 6;
 while (num)
 {
  i--;
  tmp_num = num % 36;
  if (tmp_num > 9)
   buffer[i] = 'a'+tmp_num-10;
  else
   buffer[i] = '0'+tmp_num;
  num = num / 36;
 }

 while(i)
 {
  i--;
  buffer[i]='0';
 }
 ret = puid;
 strcpy(ret, buffer);
 return ret;
}






