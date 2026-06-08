/*
** Readkey.c for Xclamation, XQuad and XAllWrite in Readkey/
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
** Last update Fri Dec 31 16:06:35 1999 Emmanuel Paris
*/


#include "Readkey.h"
#include "Decrypt.h"
#include "version.h"
#include "builtdate.h"
#include <time.h>

void read_key_file();
int get_info_from_key();
boolean check_key_validity();
int display_to_host();
boolean Xc_KEY_TRACE = FALSE;

static boolean read_file();
static boolean check_key_consistency();
static int decrypt_it();
static int read_uniqueid();
static int read_num();
static int read_hexa();

/* ----------------------------------------------------------------- **
** read_key_file                                                     **
** ----------------------------------------------------------------- */
void read_key_file(pkey)
char	**pkey;
{
 char	*filename;
 char	*filenameoffice;

 Xc_TRACE(("Read Local Key"));

 if((filenameoffice = getenv("HOME")) == NULL) 
  filenameoffice = "";
 filenameoffice = Xc_strconcat(filenameoffice, "/.", Xc_OFFICE_KEY_FILE, NULL);
 
 /*--- search global office key in home dir ---*/
 if (filenameoffice && IsFileExist(filenameoffice) && read_file(filenameoffice, pkey))
 {
  Xc_free(filenameoffice);
  return;   /* ok key found in home directory */
 }
 Xc_free(filenameoffice);

 if((filename = getenv("HOME")) == NULL) 
  filename = "";
 filename = Xc_strconcat(filename, "/.", Xc_KEY_FILE, NULL);

 /*--- search local soft key in home dir ---*/
 if (filename && IsFileExist(filename) && read_file(filename, pkey))
 {
  Xc_free(filename);
  return;   /* ok key found in home directory */
 }
 Xc_free(filename);

 /*--- search global office key in lib dir ---*/
 filenameoffice = TranslateFilename(NULL, Xc_OFFICE_KEY_FILE);
 if (filenameoffice && IsFileExist(filenameoffice) && read_file(filenameoffice, pkey))
 {
  Xc_free(filenameoffice);
  return;   /* ok key found in lib directory */
 }
 Xc_free(filenameoffice);

 /*--- search local soft key in home lib dir ---*/
 filename = TranslateFilename(NULL, Xc_KEY_FILE); 
 if (filename && IsFileExist(filename) && read_file(filename, pkey))
 {
  Xc_free(filename);  
  return;   /* ok key found in lib directory */
 }
 Xc_free(filename);  
 
 if (Xc_KEY_TRACE) printf("No key found\n");
 Xc_TRACE(("No Local Key"));
 *pkey = NULL;
}

/* ----------------------------------------------------------------- **
** get_info_from_key                                                 **
** ----------------------------------------------------------------- */
int get_info_from_key(ninfo, buffer)
struct key_info	*ninfo;
char		*buffer;
{
 int		i;
 int		tmp_num;
 struct tm	*tm_ptr;
 int		ret;

 if (!check_key_consistency(buffer))
 {
  return -1;
 }
 if (Xc_KEY_TRACE) printf("Key consistency seems to be correct\n");

 if (read_uniqueid(buffer, &(ninfo->uniqueid)))
  return -1;
 i=7;				/* skip uniqueid & '-' */

 if ((ret = read_num(buffer+i, &tmp_num))<=0)
  return -1;
 i += ret+1;			/* skip num & '-' */
 ninfo->name = (char *)malloc(tmp_num+1);
 ret = 0;
 while (buffer[i] && (ret < tmp_num))
 {
  ninfo->name[ret] = buffer[i+ret];
  ret ++;
 }
 if (ret < tmp_num)
 {
  if (Xc_KEY_TRACE) printf("ERROR: ReadK code 1\n");
  return -1;
 }
 ninfo->name[ret] = 0;
 if (Xc_KEY_TRACE) printf("Key User name ..............: \"%s\"\n", ninfo->name);
 i += tmp_num+1;		/* skip name & '-' */

 if ((ret = read_num(buffer+i, &tmp_num))<=0)
 {
  if (Xc_KEY_TRACE) printf("ERROR: ReadK code 2\n");
  return -1;
 }
 i += ret+1;			/* skip num & '-' */
 ninfo->society = (char *)malloc(tmp_num+1);
 ret = 0;
 while (buffer[i] && (ret < tmp_num))
 {
  ninfo->society[ret] = buffer[i+ret];
  ret ++;
 }
 if (ret < tmp_num)
 {
  if (Xc_KEY_TRACE) printf("ERROR: ReadK code 3\n");
  return -1;
 }
 ninfo->society[ret] = 0;
 if (Xc_KEY_TRACE)
 {
  if (*ninfo->society)
   printf("Key Company name ...........: \"%s\"\n", ninfo->society);
  else
   if (Xc_KEY_TRACE) printf("Key Company name ...........: (empty field)\n");
 }
 i += tmp_num+1;		/* skip society & '-' */

 if ((ret = read_num(buffer+i, &(ninfo->soft)))<=0)
 {
  if (Xc_KEY_TRACE) printf("ERROR: ReadK code 4\n");
  return -1;
 }
 i += ret+1;			/* skip soft & '-' */
 if (Xc_KEY_TRACE) printf("Key Software number ........: #%d\n", ninfo->soft);

 if ((ret = read_num(buffer+i, &(ninfo->type)))<=0)
 {
  if (Xc_KEY_TRACE) printf("ERROR: ReadK code 6\n");
  return -1;
 }
 i += ret+1;			/* skip type & '-' */
 if (Xc_KEY_TRACE) printf("Key type ...................: %s\n", 
			  ninfo->type?"Partner":"Standard");
 if (Xc_KEY_TRACE) 
 {
  if (ninfo->type)
   printf("Key max Licenses ...........: %d\n", ninfo->type);
  else
   printf("Key max License ............: 1\n");
 }
 if ((ret = read_hexa(buffer+i, &(ninfo->date)))<=0)
 {
  if (Xc_KEY_TRACE) printf("ERROR: ReadK code 8\n");
  return -1;
 }
 i += ret+1;			/* skip date & '-' */

 tm_ptr = localtime(&ninfo->date);
 if (Xc_KEY_TRACE)
  printf("Key registry date ..........: %02d/%02d/%d  (%lx)\n", 
	 tm_ptr->tm_mon +1, tm_ptr->tm_mday, tm_ptr->tm_year, ninfo->date);

 ninfo->key = (char *)malloc(13+1);
 ret = 0;
 while (buffer[i] && (ret < 13))
 {
  ninfo->key[ret] = buffer[i+ret];
  ret ++;
 }
 if (ret < 13)
 {
  if (Xc_KEY_TRACE) printf("ERROR: ReadK code 15\n");
  return -1;
 }
 ninfo->key[13] = 0;
 if (Xc_KEY_TRACE) 
  printf("Key Checksum ...............: \"%s\"\n", ninfo->key);
 
 return 1;
}


/* ----------------------------------------------------------------- **
** check_key_validity                                                **
** ----------------------------------------------------------------- */
boolean check_key_validity(kinfo, client, display)
struct key_info *kinfo;
int		client;
char		*display;
{
 char	*time_ptr;
 time_t	real_time;
  
  kinfo->valid = Xc_KEY_NOT_VALID;    
      
 /******************** verify crypt key *******************/
 if (decrypt_it(kinfo))
 {
  Xc_TRACE(("key invalid"));
  return FALSE;
 }
  
  /******************** verify soft *******************/
 if (kinfo->soft != 3)
  if (client%10 != kinfo->soft)
  {
   /* the problem is that this should not be easy to change in code */
   /* that's why i have checksum some very useful info (all) */
   Xc_TRACE(("bad key"));
   return FALSE;
  }

 /******************** verify date fin *******************/
 if (kinfo->date)
 {
  struct tm	*tmKey;
  struct tm	tmBuild;
  time_t	ttKey;

  tmBuild.tm_sec = 0;
  tmBuild.tm_min = 0;
  tmBuild.tm_hour = 0;
  tmBuild.tm_mday = MAKE_IDATE_DAY;
  tmBuild.tm_mon = MAKE_IDATE_MONTH;
  tmBuild.tm_year = (MAKE_IDATE_YEAR < 80 ? 
		     MAKE_IDATE_YEAR + 100 : MAKE_IDATE_YEAR);
  tmBuild.tm_wday = 0;
  tmBuild.tm_yday = 0;
  tmBuild.tm_isdst = 0;

  tmKey = localtime(&kinfo->date); 
  /*-- add two year of free upgrade --*/
  tmKey->tm_year += 2;
  ttKey = mktime(tmKey);
  time_ptr = ctime(&ttKey);

  Xc_TRACE(("available up to %s", time_ptr));
  real_time = mktime(&tmBuild);
  time_ptr = ctime(&real_time);
  Xc_TRACE(("current %s", time_ptr));
  if (ttKey < real_time)
  {
   Xc_TRACE(("Key out of date for this version"));
   kinfo->valid = Xc_KEY_EXPIRED;
   return FALSE;
  }
 }
 
 kinfo->valid = Xc_KEY_GOOD;
 return TRUE;
}

/* ----------------------------------------------------------------- **
** display_to_host                                                   **
** ----------------------------------------------------------------- */
int display_to_host(display)
char **display;
{
 char	*tmp;
 int	i;

 if ((*display)[0]==':')
 {
  tmp = Xc_malloc("dspth", 50);
  if (gethostname(tmp, 50) == -1)
  {
   Xc_free(tmp);
   return -1;
  }
 }
 else
 {
  tmp = Xc_malloc("dspth", strlen(*display)+1);
  i=0;
  while ((*display)[i] && (*display)[i] != ':')
  {
   tmp[i] = (*display)[i];
   i++;
  }
  if ((*display)[i] != ':')
   return -1;

  tmp[i] = 0;
 }
 *display = tmp;
 return 1;
}

/* ----------------------------------------------------------------- ** 
** PRIVATE FUNCTIONS                                                 ** 
** ----------------------------------------------------------------- */

/* ----------------------------------------------------------------- ** 
** read_file                                                         ** 
** ----------------------------------------------------------------- */
static boolean read_file(filename, pkey)
char *filename;
char **pkey;
{
 int	fd, size;

 if (Xc_KEY_TRACE) printf("Key found in %s\n", filename);
 fd = open(filename, O_RDONLY);
 if (fd)
 {
  Xc_TRACE(("Local Key Found"));
  size = lseek(fd, 0, SEEK_END);
  if (size <= 0)
  {
   if (Xc_KEY_TRACE) printf("ERROR: lseek returns <= 0 value\n");
   return FALSE;
  }
  *pkey = (char *)Xc_malloc("key", size+1);
  lseek(fd, 0, SEEK_SET);
  read(fd, *pkey, size);
  (*pkey)[size]=0;
  if (Xc_KEY_TRACE) printf("----- Key file: ----------------------------------------------------------\n");
  if (Xc_KEY_TRACE) printf("%s\n", *pkey);
  if (Xc_KEY_TRACE) printf("--------------------------------------------------------------------------\n");
  Xc_TRACE(("key %s", *pkey));
  return TRUE;
 }
 if (Xc_KEY_TRACE) printf("Can't open key file\n");
 return FALSE;
}

/* ----------------------------------------------------------------- **
* check key consistency - also convert 7 bits to 8 bits		     **
** ----------------------------------------------------------------- */
static boolean check_key_consistency(buffer)
char *buffer;
{
 int i, j, k, v, len;
 unsigned char tmp[300];
 
 if (!buffer || !*buffer) return FALSE;

 i = 0;
 while(buffer[i])
 {
  if (buffer[i] >= '0' && buffer[i] <= '9') break;
  if (buffer[i] >= 'a' && buffer[i] <= 'z') break;
  i++;
 }
 
 j = strlen(buffer) - 1;
 while(j >= 0)
 {
  if (buffer[j] > ' ' && (unsigned int)buffer[j] <= 127) break;
  j--;
 }
 
 for(k = 0; i <= j; i++)
 {
  if (buffer[i] < ' ') continue;
  tmp[k++] = buffer[i];
 }
 
 /*--- check for id mask ---*/
 len = k; tmp[k] = '\0';
 k = 0;
 while(k < 6)
 {
  if (tmp[k] < '0' || (tmp[k] > '9' && tmp[k] < 'a') || tmp[k] > 'z')
  {
   if (Xc_KEY_TRACE) printf("ERROR: Bad Key Consistency code 1\n");
   return FALSE;
  }
  k++;
 }
 if (tmp[k++] != '-')
 {
  if (Xc_KEY_TRACE) printf("ERROR: Bad Key Consistency code 2\n");
  return FALSE;
 } 

 /*--- check for name len mask ---*/
 i = k; j = 0;
 while(tmp[k] >= '0' && tmp[k] <= '9') { j = j * 10 + tmp[k] - '0'; k++; }
 if (i == k || k > i+3) 
  {
   if (Xc_KEY_TRACE) printf("ERROR: Bad Key Consistency code 3\n");
   return FALSE;
  }
 if (tmp[k++] != '-')
 {
  if (Xc_KEY_TRACE) printf("ERROR: Bad Key Consistency code 4\n");
  return FALSE;
 } 

 /*--- check for name mask ---*/
 for(i = k; i < k + j; i++)
 {
  if (tmp[i] == '\\')
  {
   if (tmp[i+1] == '\\')
   {
    memmove(tmp + i + 1, tmp + i + 2, len - i - 1);
   }
   else
   {
    if (tmp[i+1] == '_')
    {
     tmp[i] = ' ';
     memmove(tmp + i + 1, tmp + i + 2, len - i - 1);
    }
    else
    {
     v = (tmp[i+1] >= 'a') ? 
      ((tmp[i+1] - 'a' + 10) << 4) : ((tmp[i+1] - '0') << 4);
     v += (tmp[i+2] >= 'a') ? (tmp[i+2] - 'a' + 10) : (tmp[i+2] - '0');
     tmp[i] = (char)v;
     memmove(tmp + i + 1, tmp + i + 3, len - i - 2);
    }
   }
  }
 }
 k = i;
 if (tmp[k++] != '-') 
 {
  if (Xc_KEY_TRACE) printf("ERROR: Bad Key Consistency code 5\n");
  return FALSE;
 }

 /*--- check for society len mask ---*/
 i = k; j = 0;
 while(tmp[k] >= '0' && tmp[k] <= '9') { j = j * 10 + tmp[k] - '0'; k++; }
 if (i == k || k > i+3)
 {
  if (Xc_KEY_TRACE) printf("ERROR: Bad Key Consistency code 6\n");
  return FALSE;
 }
 if (tmp[k++] != '-') 
 {
  if (Xc_KEY_TRACE) printf("ERROR: Bad Key Consistency code 7\n");
  return FALSE;
 }

 /*--- check for society mask ---*/
 for(i = k; i < k + j; i++)
 {
  if (tmp[i] == '\\')
  {
   if (tmp[i+1] == '\\')
   {
    memmove(tmp + i + 1, tmp + i + 2, len - i - 1);
   }
   else
   {
    if (tmp[i+1] == '_')
    {
     tmp[i] = ' ';
     memmove(tmp + i + 1, tmp + i + 2, len - i - 1);
    }
    else
    {
     v = (tmp[i+1] >= 'a') ? 
      ((tmp[i+1] - 'a' + 10) << 4) : ((tmp[i+1] - '0') << 4);
     v += (tmp[i+2] >= 'a') ? (tmp[i+2] - 'a' + 10) : (tmp[i+2] - '0');
     tmp[i] = (char)v;
     memmove(tmp + i + 1, tmp + i + 3, len - i - 2);
    }
   }
  }
 }
 k = i;
 if (tmp[k++] != '-')
  {
   if (Xc_KEY_TRACE) printf("ERROR: Bad Key Consistency code 8\n");
   return FALSE;
  }

 /*--- check for soft mask ---*/
 if (tmp[k] < '0' || tmp[k] > '9') 
  {
   if (Xc_KEY_TRACE) printf("ERROR: Bad Key Consistency code 9\n");
   return FALSE;
  }
 k++;
 if (tmp[k++] != '-') return FALSE;
 
 /*--- check for type mask ---*/
 i = k;
 while((tmp[k] >= '0' && tmp[k] <= '9') || (tmp[k] >= 'a' && tmp[k] <= 'f'))
  k++;
 if (i == k || k > i + 2)
 {
  if (Xc_KEY_TRACE) printf("ERROR: Bad Key Consistency code 10\n");
  return FALSE;
 }
 if (tmp[k++] != '-')
 {
  if (Xc_KEY_TRACE) printf("ERROR: Bad Key Consistency code 11\n");
  return FALSE;
 } 

 /*--- check for date mask ---*/
 i = k;
 while((tmp[k] >= '0' && tmp[k] <= '9') || (tmp[k] >= 'a' && tmp[k] <= 'f'))
  k++;
 if (i == k || k > i + 8)
 {
  if (Xc_KEY_TRACE) printf("ERROR: Bad Key Consistency code 12\n");
  return FALSE;
 }
 if (tmp[k++] != '-')
 {
  if (Xc_KEY_TRACE) printf("ERROR: Bad Key Consistency code 13\n");
  return FALSE;
 }

 /*--- check for key mask ---*/
 i = k;
 while(tmp[k] && tmp[k] > ' ' && tmp[k] < 127)
  k++;
 
 if (k-i != 13)
 {
  if (Xc_KEY_TRACE) printf("ERROR: Bad Key Consistency code 14\n");
  return FALSE;
 }
 strcpy(buffer, tmp);

 return TRUE;
}

/* ----------------------------------------------------------------- **
** decrypt_it                                                        **
** ----------------------------------------------------------------- */
static int decrypt_it(ninfo)
struct key_info	*ninfo;
{
 int	salt_number;
 char	salt[3];
 char	crypt_buffer[20];
 char	crypt_buffer2[20];
 char	*crypt_ret;
 char	*decrypted_key;
 int	i, k, l;

 for (i = 0; i<20; i++)
 {
  crypt_buffer[i] = 0;
  crypt_buffer2[i] = 0;
 }
 Xc_TRACE(("**********Mixing crypt begin**********"));
 Xc_TRACE(("***salt = (len(name)+len(disp_name)*len(serv_name)+name[0]+serv_name[0]***"));
 salt_number = strlen(ninfo->name) * ninfo->name[0] - strlen(ninfo->society);
 sprintf(salt, "%x", salt_number);
 Xc_TRACE(("salt: %s", salt));
  
 Xc_TRACE(("*******part one: use [uniqueid, hostid]******"));
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
  if (crypt_buffer[i]<crypt_buffer2[i])
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

 Xc_TRACE(("*******zip*******"));
 decrypted_key = xc_decrypt(ninfo->key, (ninfo->type ? 1 : 0), (gkinfo.soft == 3) ? 1 : 0);
 Xc_TRACE(("decrypted key: %s", decrypted_key));
 if (!decrypted_key)
  return -1;

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
 i=0;
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

 return strcmp(crypt_buffer, decrypted_key);
}

/* ----------------------------------------------------------------- ** 
** read_uniqueid                                                     ** 
** ----------------------------------------------------------------- */
static int read_uniqueid(buffer, tmp_num)
char			*buffer;
unsigned long int	*tmp_num;
{
 int	i;

 *tmp_num = 0;
 i=0;
 while (buffer[i] && i<6)
 {
  *tmp_num *= 36;
  if (buffer[i] >= 'a')
   *tmp_num += buffer[i]-'a'+10;
  else
   *tmp_num += buffer[i]-'0';
  i++;
 }
 if (i < 6)
 {
  if (Xc_KEY_TRACE) printf("ERROR: UnikID code 1\n");
  return -1;
 }
 return 0;
}

/* ----------------------------------------------------------------- ** 
** read_num                                                          ** 
** ----------------------------------------------------------------- */
static int read_num(buffer, tmp_num)
char	*buffer;
int	*tmp_num;
{
 int	i;

 *tmp_num = 0;
 i=0;
 while ((buffer[i]>='0') && (buffer[i]<='9'))
 {
  (*tmp_num) *= 10;
  (*tmp_num) += buffer[i]-'0';
  i++;
 }
 return i;
}

/* ----------------------------------------------------------------- ** 
** read_hexa                                                         ** 
** ----------------------------------------------------------------- */
static int read_hexa(buffer, tmp_num)
char	*buffer;
int	*tmp_num;
{
 int	i;

 *tmp_num = 0;
 i=0;
 while (buffer[i] != '-')
 {
  (*tmp_num) *= 16;
  if ((buffer[i]>='0') && (buffer[i]<='9'))
   (*tmp_num) += buffer[i]-'0';
  if ((buffer[i]>='a') && (buffer[i]<='f'))
   (*tmp_num) += buffer[i]-'a'+10;
  if ((buffer[i]>='A') && (buffer[i]<='F'))
   (*tmp_num) += buffer[i]-'A'+10;
  i++;
 }
 return i;
}

