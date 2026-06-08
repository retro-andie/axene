/*
** gen_cdkey.c for XInstall in GenCDKey/
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
** Started on  Thu Apr 10 18:12:07 1997 Emmanuel Paris
** Last update Fri Jun  4 19:39:53 1999 Emmanuel Paris
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DIGIT_SUM_BASE	50

static char *helpString[] = { "--help", "-help", "-h", NULL};
static char *licenseString[] = { "--license", "-license", "-l", NULL};
static char *softString[] = { "--soft", "-soft", "-s", NULL};
static char *usageString = "%s: [-help] [-license #license] [-soft #soft]\n\n\
 detail:\n\
 -help, --help, -h			print this help.\n\
 -license, ---license, -l #license	number of license number: [0-99].\n\
 -soft, --soft, -s #soft		software: Xcl -> 1, Xquad -> 2...\n\n";

static int roll_digit[10][10] = 
{
 { 5, 9, 8, 1, 0, 4, 7, 6, 3, 2 },
 { 9, 0, 6, 7, 3, 8, 4, 1, 2, 5 },
 { 4, 2, 7, 3, 6, 8, 0, 1, 9, 5 },
 { 8, 5, 7, 0, 3, 1, 9, 2, 4, 6 },
 { 3, 7, 9, 6, 4, 2, 1, 5, 0, 8 },
 { 7, 3, 4, 9, 1, 6, 5, 8, 0, 2 },
 { 6, 7, 0, 2, 3, 4, 8, 5, 1, 9 },
 { 1, 8, 5, 3, 4, 9, 7, 2, 6, 0 },
 { 2, 8, 6, 9, 3, 0, 4, 5, 7, 1 },
 { 3, 0, 7, 5, 9, 8, 2, 4, 6, 1 }
};

int main(argc, argv)
int argc;
char **argv;
{
 int	i, j, value, ovalue;
 int	sum, prod, swap;
 int	digit[10];
 int	ok_sum, ok_prod;
 int	license = -1, soft = -1;
 int	DIGIT_SUM;

 {
  i = 1;
  while(i < argc)
  {

   for(j = 0; helpString[j]; j++)
   {
    if (!strcmp(argv[i], helpString[j]))
    {
     printf(usageString, argv[0]);
     return 0;
    }	
   }

   for(j = 0; licenseString[j]; j++)
   {
    if (!strcmp(argv[i], licenseString[j]))
    {
     license = atoi(argv[++i]);
     goto next_arg;
    }	
   }

   for(j = 0; softString[j]; j++)
   {
    if (!strcmp(argv[i], softString[j]))
    {
     soft = atoi(argv[++i]);
     break;
    }	
   }
  next_arg:   
   i++;
  }
 }
 if (soft == -1 || license == -1)
 {
  printf(usageString, argv[0]);
  return 1;
 }

 srand(time(NULL));
 srand(rand());

 DIGIT_SUM = DIGIT_SUM_BASE;
 DIGIT_SUM += soft;
 i = license;
 while(i)
 {
  DIGIT_SUM += i % 10;
  i = i / 10;
 }

 ok_prod = 0;
 do
 {
  ok_sum = 0;
  do
  {
   sum = 0;
   for(i = 0; i < 9; i++)
   {
    value = rand() % 9 + 1;
    sum += value;
    digit[i] = value;
   }
   
   if (sum < DIGIT_SUM && sum > DIGIT_SUM - 10)
   {
    digit[9] = DIGIT_SUM - sum;
    ok_sum = 1;
   }
  }
  while(!ok_sum);

  value = digit[0] * 11 + digit[1];
  
  ovalue = 0;
  for(i = 2; i < 9; i++)
  {
   if ((value & 1) == 0)
   {
    if (digit[i] <= digit[i+1])
    {
     if (digit[i] == digit[i+1]) { ok_sum = 0; break; }
     
     swap = digit[i];
     digit[i] = digit[i+1];
     digit[i+1] = swap;
     if (i > 2)
      if (ovalue == 0)
      {
       if (digit[i-1] <= digit[i]) { ok_sum = 0; break; }
      }
      else
      {
       if (digit[i-1] > digit[i]) { ok_sum = 0; break; }
      }
    }
   }
   else
   {
    if (digit[i] > digit[i+1])
    {
     swap = digit[i];
     digit[i] = digit[i+1];
     digit[i+1] = swap;
     if (i > 2)
      if (ovalue == 0)
      {
       if (digit[i-1] <= digit[i]) { ok_sum = 0; break; }
      }
      else
      {
       if (digit[i-1] > digit[i]) { ok_sum = 0; break; }
      }
    }   
   }
   ovalue = value & 1;
   value >>= 1;
  }
  
  if (!ok_sum || (!digit[0] && !digit[1]) || (!digit[8] && !digit[9]))
   continue;
  
  if ((digit[0] == digit[8] && digit[1] == digit[9]) || digit[0] == digit[9])
   continue;
  
  prod = digit[2];
  for(i = 3; i < 10; i++)
  {
   prod *= 10;
   prod += digit[i];
  }

  if (prod % (digit[0] * 10 + digit[1]) == 0)
  {
   prod = digit[0];
   for(i = 1; i < 8; i++)
   {
    prod *= 10;
    prod += digit[i];
   }
   
   if (prod % (digit[8] * 10 + digit[9]) == 0)
    ok_prod = 1;
  }
 }
 while(!ok_prod);

/*
 printf("%2d%1d-", license, soft);
 for(i = 0; i < 10; i++)
  printf("%c", digit[i] + '0');
 printf("\n");
*/

 printf("%02d%1d-", license, soft);
 for(i = 0; i < 10; i++)
 {
  swap = (i + license + soft) % 10;
   for(j = 0; j < 10; j++)
  {
   if (roll_digit[swap][j] == digit[i])
    break;
  }
  printf("%c", j + '0'); /* encode[i]); */

 }
 printf("\n");
 
 return 0;
}



