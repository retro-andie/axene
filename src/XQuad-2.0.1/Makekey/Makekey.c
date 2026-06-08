/*
** Makekey.c for Xclamation, XQuad and XAllWrite in Makekey/
** program that parse an input file and generate a valid key
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
** Started on  Mon Jul 17 15:05:00 1995 Antoine Buat
** Last update Fri Oct 30 15:42:12 1998 Emmanuel Paris
*/

#define __XC_MAIN__

#define NTRACE
#define NHISTORY

#include "xcalibur.h"
#include "Encrypt.h"
#include <time.h>

#define MAX_SIZE 1024

char	buffer[MAX_SIZE];
char	crypt_buffer[20];

char *soft_name[]=
{
 "xclamation",
 "xquad",
 "xallwrite",
 "axeneoffice",
 NULL
};

char *type_name[]=
{
 "free",
 "commercial",
 NULL
};


/* time t is a long print it as %x*/

struct info
{
 int	uniqueid;
 char	*name;
 char	*society;
 int	soft;
 int	type;
 time_t	date;
};

boolean date_error = FALSE;
void Fill_info();
void purge();
char *parse_name();
char *parse_society();
int parse_soft();
int parse_version();
int parse_type();
int parse_license_number();
time_t parse_date();
char *crypt_it();
char *print_hexa();
char *print_uniqueid();
int yyparse();
extern char *yyInput;
extern time_t yyMonth;
extern time_t yyDay;
extern time_t yyYear;
char *crypt();
void conv_7_bits();

int main(argc, argv)
int	argc;
char	**argv;
{
 FILE	*fd;
 long	file_size;
 struct info ninfo;
 int	readed;
 char	*sZcrypt;
 FILE	*fd_id;
 char	*home_dir;
 char	file_info[512];
 int	cur_id;
 char fname[100];
 char fsociety[100];

 if ((argc < 2) || (argc > 3))
 {
  printf("Usage: %s [-e] form.txt\n", argv[0]);
  return -1;
 }

 if (argc == 3)
 {
  if (!strcmp(argv[1], "-e"))
  {
   date_error = TRUE;
   argv[1] = argv[2];
  }
  else
  {
   printf("Usage: %s [-e] form.txt\n", argv[0]);
   return -1;
  }
 }

 /*--- get the current id for key (should be unique) ---*/
 /*--- it is auto-incremented in the .makekey_id ---*/
 home_dir = getenv("HOME");
 sprintf(file_info, "%s/%s", home_dir, ".makekey_id");
 fd_id = fopen(file_info, "r+");
 if (fd_id)
 {
  fgets(file_info, 512, fd_id);
  cur_id = atoi(file_info);
 }
 else
  cur_id = 0;

 fd = fopen(argv[1], "r");
 if (!fd)
 {
  perror(argv[1]);
  return -1;
 }
 if (!fseek(fd, 0, SEEK_END))
  file_size = ftell(fd);
 else
 {
  perror(argv[1]);
  return -1;
 }
 if (file_size > MAX_SIZE)
 {
  Xc_ERROR(("file too long can't be only a key form"));
 }
 fseek(fd, 0, SEEK_SET);
 readed = fread(buffer, 1, file_size, fd);
 buffer[readed] = 0;
 Xc_TRACE( ("readed: %d", readed));

 /*--- init struct ---*/ 
 bzero(&ninfo, sizeof(ninfo));
 ninfo.name = ninfo.society = "";

 /*--- axn is the AXENE key distribution code ---*/ 
 ninfo.uniqueid = 60466176*('a'-'a'+10)+1679616*(int)('x'-'a'+10)+
  46656*(int)('n'-'a'+10)+cur_id;

 /*--- fill the structure ---*/
 Fill_info(&ninfo, buffer);
 sZcrypt = crypt_it(&ninfo);

 /*--- output the key ---*/ 
 conv_7_bits(ninfo.name, fname);
 conv_7_bits(ninfo.society, fsociety);
 printf("%s-%d-%s-%d-%s-%d-%d-%08lx-%s\n",
	print_uniqueid(ninfo.uniqueid),
	strlen(ninfo.name), fname,
	strlen(ninfo.society), fsociety,
	ninfo.soft,
	ninfo.type,
	ninfo.date,
	sZcrypt);

 /*--- increment the makekey_id ---*/
 if (fd_id)
 {
  cur_id++;
  fseek(fd_id, 0, SEEK_SET);
  sprintf(file_info, "%d", cur_id);
  fputs(file_info, fd_id);  
  fclose(fd_id);
 }
 fclose(fd);
 return 0;
}

void Fill_info(ninfo, buffer)
struct info	*ninfo;
char		*buffer;
{
 int	index;
 char	mini_buffer[MAX_SIZE];
 int	mini_index;
 int	iLicNum=1;

 iLicNum = 0;
 index=0;
 mini_index=0;
 while (buffer[index])
 {
  if (buffer[index]==':')
  {
   mini_buffer[mini_index]=0;
   purge(mini_buffer, mini_index);
   Xc_TRACE(("keyword:%s", mini_buffer));
   if (!strcmp("NAME", mini_buffer))
   {
    index++;
    ninfo->name = parse_name(buffer, &index);
    Xc_TRACE(("name: %s", ninfo->name));
   }
   if (!strcmp("SOCIETY", mini_buffer))
   {
    index++;
    ninfo->society = parse_society(buffer, &index);
    Xc_TRACE(("society: %s", ninfo->society));
   }
   if (!strcmp("SOFT", mini_buffer))
   {
    index++;
    ninfo->soft = parse_soft(buffer, &index);
    if (ninfo->soft == -1)
    {
     Xc_ERROR(("Unknown software"));
     exit(-1);
    }
    Xc_TRACE(("soft: %d", ninfo->soft));
   }
   if (!strcmp("TYPE", mini_buffer))
   {
    index++;
    ninfo->type = parse_type(buffer, &index);
    if (ninfo->type == -1)
    {
     Xc_ERROR(("Unknown license type"));
     exit(-1);
    }
    Xc_TRACE(("type: %d", ninfo->type));
   }
   if (!strcmp("LICENSENUMBER", mini_buffer))
   {
    index++;
    iLicNum = parse_license_number(buffer, &index);
    Xc_TRACE(("Nombre de license: %d", iLicNum));
   }
   if (!strcmp("DATE", mini_buffer))
   {
    index++;
    ninfo->date = parse_date(buffer, &index);
    if (ninfo->date == -1)
    {
     Xc_ERROR(("Unknown registration date"));
     exit(-1);
    }	      
    Xc_TRACE(("registration date: %ld", ninfo->date));
   }
   index++;
   mini_index=0;
   continue;
  }
  if ((buffer[index]=='\r')
      || (buffer[index]=='\n'))
  {
   mini_index=0;
   index++;
   continue;
  }
  mini_buffer[mini_index++]=buffer[index++];
 }
 if ((iLicNum > 1) && (ninfo->type == 0))
 {
  Xc_ERROR(("You may not have type free with more than 1 license\n"));
  exit (-1);
 }
}

void purge(buffer, size)
char	*buffer;
int	size;
{
 char	*new_buf;
 int	index1;
 int	index2;

 new_buf = (char *)Xc_malloc("nb", size+1);

 index1 = 0;
 index2 = 0;
 while (index1 < size)
 {
  if (buffer[index1]==' ' || buffer[index1]=='\t')
  {
   index1++;
   continue;
  }
  if ((buffer[index1]>'a') && (buffer[index1]<'z'))
  {
   new_buf[index2++]=buffer[index1++]+'A'-'a';
   continue;
  }
  new_buf[index2++]=buffer[index1++];
 }
 new_buf[index2]=0;
 strcpy(buffer, new_buf);
 Xc_free(new_buf);
}

char *parse_name(buffer, index)
char	*buffer;
int	*index;
{
 int	start_index;
 char	*ret_ptr;

 while(buffer[*index]==' ')
  (*index)++;
 start_index = *index;
 while (buffer[*index] && (buffer[*index]!='\r') && (buffer[*index]!='\n'))
  (*index)++;
 if (start_index != *index)
 {
  while(buffer[*index-1] == ' ')
   (*index)--;
  ret_ptr = (char *)Xc_malloc("pn", *index-start_index+1);
  strncpy(ret_ptr, buffer+start_index, *index-start_index);
  ret_ptr[*index-start_index]=0;
  return ret_ptr;
 }
 else
  return NULL;
}

char *parse_society(buffer, index)
char	*buffer;
int	*index;
{
 int	start_index;
 char	*ret_ptr;

 while(buffer[*index]==' ')
  (*index)++;
 start_index = *index;
 while (buffer[*index] && (buffer[*index]!='\r') && (buffer[*index]!='\n'))
  (*index)++;
 if (start_index != *index)
 {
  while(buffer[*index-1] == ' ')
   (*index)--;
  ret_ptr = (char *)Xc_malloc("ps", *index-start_index+1);
  strncpy(ret_ptr, buffer+start_index, *index-start_index);
  ret_ptr[*index-start_index]=0;
  return ret_ptr;
 }
 else
  return "";
}

int parse_soft(buffer, index)
char	*buffer;
int	*index;
{
 int	start_index;
 int	soft_index;

 while(buffer[*index]==' ')
  (*index)++;
 start_index = *index;
 while (buffer[*index] && (buffer[*index]!='\r') && (buffer[*index]!='\n'))
 {
  if ((buffer[*index]>='A') && (buffer[*index]<='Z'))
   buffer[*index] += 'a' - 'A';
  (*index)++;
 }
 if (start_index != *index)
 {
  if (buffer[*index])
   buffer[*index] = 0;
  Xc_TRACE(("softname %s", buffer+start_index));
  soft_index = 0;
  while (soft_name[soft_index])
  {
   if (strncmp(soft_name[soft_index], buffer+start_index, 
	       strlen(soft_name[soft_index])))
    soft_index++;
   else
    return soft_index;
  }
  return -1;
 }
 else
  return -1;
}

int parse_type(buffer, index)
char	*buffer;
int	*index;
{
 int	start_index;
 int	type_index;

 while(buffer[*index]==' ')
  (*index)++;
 start_index = *index;
 while (buffer[*index] && (buffer[*index]!='\r') && (buffer[*index]!='\n'))
 {
  if ((buffer[*index]>='A') && (buffer[*index]<='Z'))
   buffer[*index] += 'a' - 'A';
  (*index)++;
 }
 if (start_index != *index)
 {
  if (buffer[*index])
   buffer[*index] = 0;
  Xc_TRACE(("typename %s", buffer+start_index));
  type_index = 0;
  while (type_name[type_index])
  {
   if (strncmp(type_name[type_index], buffer+start_index,
	       strlen(type_name[type_index])))
    type_index++;
   else
    return type_index;
  }
  return -1;
 }
 else
  return -1;
}

int parse_license_number(buffer, index)
char	*buffer;
int	*index;
{
 int	nbr;

 while(buffer[*index]==' ')
  (*index)++;
 nbr = 0;
 while (buffer[*index] && (buffer[*index]!='\r') && (buffer[*index]!='\n'))
 {
  if ((buffer[*index] < '0') ||
      (buffer[*index] > '9'))
   return -1;
  nbr = nbr*10 + buffer[*index]-'0';
  (*index)++;
 }
 return nbr;
}

time_t parse_date(buffer, index)
char	*buffer;
int	*index;
{
 time_t rtime;
 int	ret;
 struct tm	s_time; 

 yyInput = buffer+*index;
 ret = yyparse();
 ret = yyInput - (buffer+*index);
 Xc_TRACE(("day: %d month: %d year: %d", (int)yyDay, (int)yyMonth,
	   (int)yyYear));
 s_time.tm_sec = 0;
 s_time.tm_min = 0;
 s_time.tm_hour = 0;
 s_time.tm_mday = yyDay;
 s_time.tm_mon = yyMonth-1;
 if ((yyYear > 95) && (yyYear < 100))
  s_time.tm_year = yyYear;
 else
  s_time.tm_year = yyYear-1900;
 s_time.tm_wday = 0;
 s_time.tm_yday = 0;
 s_time.tm_isdst = 0;
 rtime = mktime(&s_time);
 
 if (date_error)
 {
  int i;
  char	asciicnv[20];
  
  sprintf(asciicnv, "%lx", rtime);
  for(i = 1; i < (int)strlen(asciicnv); i++)
  {
   if (asciicnv[i] >= 'a' && asciicnv[i] <= 'f')
   {
    if (asciicnv[i-1] != '9') asciicnv[i-1] ++;
    for(; i < (int)strlen(asciicnv); i++)
     asciicnv[i] = '0';
   }
  }
  rtime = (time_t)strtol(asciicnv, NULL, 16);
 }

 return rtime;
}

char *crypt_it(ninfo)
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

 crypt_ret = xc_encrypt(crypt_buffer, ninfo->type ? 1 : 0, ninfo->soft);
 Xc_TRACE(("*******final crypt******"));
 Xc_TRACE(("crypted pass: %s ", crypt_ret));
 return crypt_ret;
}

char *print_hexa(num)
long int num;
{
 char	buffer[30];
 int	i;
 int	tmp_num;
 char	*ret;

 buffer[30]=0;
 i=30;
 while (num)
 {
  i--;
  tmp_num = num%16;
  if (tmp_num > 9)
   buffer[i]='a'+tmp_num-10;
  else
   buffer[i]='0'+tmp_num;
  num = num/16;
 }
 if (i != 30)
 {
  ret = (char *)Xc_malloc("ph", 30-i);
  strcpy(ret, buffer+i);
 }
 else
  ret = NULL;
 return ret;
}

char *print_uniqueid(num)
long int num;
{
 char	buffer[6];
 int	i;
 int	tmp_num;
 char	*ret;

 buffer[6]=0;
 i=6;
 while (num)
 {
  i--;
  tmp_num = num%36;
  if (tmp_num > 9)
   buffer[i]='a'+tmp_num-10;
  else
   buffer[i]='0'+tmp_num;
  num = num/36;
 }

 while(i)
 {
  i--;
  buffer[i]='0';
 }
 ret = (char *)Xc_malloc("puid", 7);
 strcpy(ret, buffer);
 return ret;
}

static char *hex_conv = "0123456789abcdef";

void conv_7_bits(str, ret_str)
char *str;
char *ret_str;
{
 int i, j, k;

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
 
 j = 0;
 for(; i <= k; i++)
 {
  if ((unsigned char)str[i] <= ' ')
  {
   ret_str[j++] = '\\';  ret_str[j++] = '_';
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
  }
 }
 ret_str[j] = '\0';
}
