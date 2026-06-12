/*
** TestCDKey.c for XInstall in TestCDKey/
** declaration of inlines function to test the validity
** of a cd key.
**
** Copyright (C) 1997-2000 Axene.
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
** Started on  Wed Jun 25 16:18:24 1997 Emmanuel Paris
** Last update Thu Oct  8 17:46:11 1998 Antoine Buat
*/

#include "xcalibur.h"

extern int soft;
extern int version;

static int stage;
static int digit[10];

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

#define DIGIT_SUM_BASE	50

boolean start_cdkey_check(keys)
char	*keys;
{
 int i, j;
 char	*ptr;

 stage = 0;
 if (strlen(keys) != 14)
  return FALSE;
 
 ptr = keys;
 if (*ptr < '0' || *ptr > '9') return FALSE;
 version = 10 * (*ptr++ - '0');
 if (*ptr < '0' || *ptr > '9') return FALSE;
 version += *ptr++ - '0';
 if (*ptr < '0' || *ptr > '9') return FALSE;
 soft = *ptr++ - '0';
 if (*ptr != '-') return FALSE;
 ptr++;
 
 if (soft > 3) /* WARNING! allow only 1,2,3 softs and 0 for office! */
  return FALSE;

 for(i = 0; i < 10; i++)
 {
  j = (i + version + soft) % 10;
  if (*ptr < '0' || *ptr > '9') return FALSE;
  digit[i] = roll_digit[j][*ptr - '0'];
  ptr++;
 }
 
 stage = 484278;

 Xc_TRACE(("Start: key=%2d%1d-%d%d%d%d%d%d%d%d%d%d", version, soft,
	   digit[0], digit[1], digit[2], digit[3], digit[4],
	   digit[5], digit[6], digit[7], digit[8], digit[9]));

 return TRUE;
}

boolean check_cdkey_stage1()
{
 if ((digit[0] == digit[8] && digit[1] == digit[9]) || digit[0] == digit[9])
  return FALSE;

 stage += 156646;

 return TRUE;
}

boolean check_cdkey_stage2()
{
 if ((!digit[0] && !digit[1]) || (!digit[8] && !digit[9]))
  return FALSE;

 stage = 789845 - stage;

 return TRUE;
}

boolean check_cdkey_stage3()
{
 int i, prod;
 
 prod = digit[2];
 for(i = 3; i < 10; i++)
 {
  prod *= 10;
  prod += digit[i];
 }

 if (!digit[0] && !digit[1]) return FALSE;
 if (prod % (digit[0] * 10 + digit[1]) != 0) return FALSE;
 
 stage ^= 164231;

 return TRUE;
}

boolean check_cdkey_stage4()
{
 int i, prod;
 
 prod = digit[0];
 for(i = 1; i < 8; i++)
 {
  prod *= 10;
  prod += digit[i];
 }
 
 if (!digit[8] && !digit[9]) return FALSE;
 if (prod % (digit[8] * 10 + digit[9]) != 0) return FALSE;
 
 stage &= 422179;

 return TRUE;
}

boolean check_cdkey_stage5()
{
 int i, value;

 value = digit[0] * 11 + digit[1];
  
 for(i = 2; i < 9; i++)
 {
  if ((value & 1) == 0)
  {
   if (digit[i] <= digit[i+1])
    return FALSE;
  }
  else
  {
   if (digit[i] > digit[i+1])
    return FALSE;
  }
  stage += 9142 + i * 213;
  value >>= 1;
 }
 
 stage |= 721451;

 return TRUE;
}

boolean check_cdkey_stage6()
{
 int i, sum, value;

 value = DIGIT_SUM_BASE + soft + (version / 10) + (version % 10);
 
 sum = 0;
 for(i = 0; i < 10; i++)
 {
  sum += digit[roll_digit[4][i]];
  stage -= roll_digit[i][10-i] * 6;
 }
 if (value != sum) return FALSE;
 
 stage -= 154217;
 
 return TRUE;
} 

int end_cdkey_check()
{
 stage -= 564521;
 return stage;
}
