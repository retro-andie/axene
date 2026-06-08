/*
** Decrypt.c for Xclamation, XQuad and XAllWrite in Decrypt/
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
** Last update Thu Oct  8 13:31:19 1998 Antoine Buat
*/

#include "xcalibur.h"
#include "DecryptP.h"

static void compute_crypt_table();
static int  xc_random();
static int  seed, seed2;

static int crypt_table[TABLE_SIZE];
static int crypt_seed = -1; 

#if 0
static void print_table();
#endif

char *xc_decrypt(str, type, office)
char	*str;
int	type;
int	office;
{
  char	*ptr;
  int	i;
  int	nbr_boucle;
  int	mseed;
  
  if (type)
   seed = Xc_CRYPT_SEED_COMM;
  else
   seed = Xc_CRYPT_SEED_FREE;

  if (office)
   seed2 = Xc_CRYPT_SEED3;
  else
   seed2 = Xc_CRYPT_SEED2;
  
  compute_crypt_table();
  
  ptr = (char *)malloc(strlen(str)+1);
  i = 0;
  mseed = crypt_seed;
  while (str[i])
    {
      nbr_boucle = str[i] - 33 + 1;
      while (nbr_boucle)
	{
	  mseed = crypt_table[mseed];
	  nbr_boucle--;
	}
      ptr[i] = mseed + 33;
      i++;
    }
  ptr[i] = 0;
  return ptr;
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








