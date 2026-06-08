/*
** Encrypt.c for Xclamation, XQuad, XAllWrite and XInstall in Encrypt/
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
** Started on  Thu Aug 26 17:05:21 1996 Antoine Buat
** Last update Tue Oct  6 22:54:10 1998 Antoine Buat
*/

#include "xcalibur.h"
#include "EncryptP.h"

/* partie similaire que dans Decrypt.c */
static void compute_crypt_table();
static int  xc_random();
static int  seed, seed2;
static int  crypt_table[TABLE_SIZE];
static int  crypt_seed = -1; 
/* fin partie */

static void create_table();
static int table[TABLE_SIZE];
#if 0
static void print_table();
#endif

#ifndef Xc_XINSTALL
char *xc_encrypt(str, type, soft)
char	*str;
int	type;
int	soft;
#else  
char *xc_encrypt(str, soft, cseed)
char	*str;
int	soft;
int	cseed;
#endif
{
 char	*ptr;
 int	i;
 int	mseed;
  
#ifndef Xc_XINSTALL
 seed = type_crypt_seed[type];
#else
 seed = cseed;
#endif

 seed2 = soft_crypt_seed[soft];

 compute_crypt_table();
  
 ptr = (char *)malloc(strlen(str) + 1);
  
 mseed = crypt_seed;
 for(i = 0; i < (int)strlen(str); i++)
 {
  create_table(mseed);
  ptr[i]= table[str[i] - 33] + 33;
  mseed = str[i] - 33;
 }
 ptr[i] = 0;
 return ptr;
}

static void create_table(num)
int num;
{
 int i, k;
 int seed;
 int tmp_table[TABLE_SIZE];
  
 for(i = 0; i < TABLE_SIZE; i++)
 {
  seed = num;
  for(k = 0; k < i + 1; k++)
   seed = crypt_table[seed];
  tmp_table[i] = seed;
 }
  
 for(i = 0; i < TABLE_SIZE; i++)
 {
  table[tmp_table[i]] = i;
 }
}

static void compute_crypt_table()
{
 int	i, j, k;
 int	first;
  
 crypt_seed = xc_random();
  
 for(i = 0; i < TABLE_SIZE; i++)
  crypt_table[i] = -1;
  
 i = first = xc_random();
 for(j = 0; j < TABLE_SIZE - 1; j++)
 {
  k = xc_random();
  while(crypt_table[k] != -1 || k == i)
  {
   k ++;
   if (k >= TABLE_SIZE)
    k = 0;
  }
  crypt_table[i] = k;
  i = k;
 }
 crypt_table[i] = first;
}

static int xc_random()
{
 int	i;
  
 i = (seed + 2 * seed2) / 3;
 seed ^= (5 * i - seed2);
 seed2 = i * 2 - seed;
 return ABS((seed2 * seed) % TABLE_SIZE);
}

#if 0
static void print_table(table)
int *table;
{
 int i, j;
  
 for(i = 0; i < 9; i++)
 {
  printf("%2d: ", i*10);
  for(j = 0; j < 10; j++)
   printf("%2d | ", table[i * 10 + j]);
  printf("\n");
 }
 printf("%2d: ", i*10);
 for(j = 0; j < 4; j++)
  printf("%2d | ", table[i * 10 + j]);
 printf("\n");
}
#endif

#ifndef Xc_XINSTALL
void special_os_seed(os, newseed)
int os;
int newseed;
{
 type_crypt_seed[os] = newseed;
}
#endif





