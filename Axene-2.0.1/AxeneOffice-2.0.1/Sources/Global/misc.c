/*
** misc.c for AxeneOffice in Global/
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Mon Jul 17 17:35:34 1995 Emmanuel Paris
** Last update Tue Jan 20 17:07:50 1998 Robin Castanier
*/


#ifndef NTRACE
#define NTRACE
#endif

#include "xcalibur.h"
#include <sys/time.h>
#include <sys/types.h>
#include <varargs.h>

extern int select();

boolean Xstrcmp(chaine1,chaine2)
char	*chaine1;
char	*chaine2;
{
 int	i;

 if ((chaine1 && !chaine2) || (!chaine1 && chaine2))
  return FALSE;
 if (!chaine1 && !chaine2)
  return TRUE;
 i = 0;
 while(chaine1[i]&&chaine2[i]&&(chaine1[i]==chaine2[i]))
  i++;
 if (chaine1[i]|chaine2[i])
  return FALSE;
 return TRUE;
}


boolean Xstrncmp(chaine1,chaine2,n)
char	*chaine1;
char	*chaine2;
int	n;
{
 int	i;

 if ((chaine1 && !chaine2) || (!chaine1 && chaine2))
  return FALSE;
 if (!chaine1 && !chaine2)
  return TRUE;
 i = 0;
 while(chaine1[i]&&chaine2[i]&&(chaine1[i]==chaine2[i])&&i<n)
  i++;
 if (chaine1[i]|chaine2[i]&&(i!=n))
  return FALSE;
 return TRUE;
}

int Xstrnchr(chaine,c)
char	*chaine;
char	c;
{
 int	i;

 i = strlen(chaine) - 1;
 while(i >= 0 && chaine[i] != c)
  i--;
 return i;
}

char *Xstrstr(chaine1,chaine2)
char	*chaine1;
char	*chaine2;
{
 char		*svch1;	
 char	 	*svch2;
  
 while(1)
 {
  while(*chaine1 != *chaine2 && *chaine1)
   chaine1++;
  if (*chaine1)
  {
   svch1=chaine1;
   svch2=chaine2;
   while(*chaine1 == *chaine2 && *chaine1 && *chaine2)
   {
    chaine1++;
    chaine2++;
   }
   if (!*chaine2)
    return svch1;
   else
   {
    chaine1 = svch1+1;
    chaine2 = svch2;
   }
  }
  else
   return NULL;
 }
}

#define MAX_ENVIRONMENT_VARIABLE	3
static char putenv_var[MAX_ENVIRONMENT_VARIABLE][300];
static int  putenv_ptr = 0;    

int Xsetenv(variable, argument)
char *variable;
char *argument;
{ 
 int  result;
  
 if (putenv_ptr >= MAX_ENVIRONMENT_VARIABLE ||
     strlen(variable) + strlen(argument) + 2 > 300)
 {
  Xc_WARNING(("too many environment variable defined, or variable too large."));
  return -1;
 }
  
 sprintf(putenv_var[putenv_ptr], "%s=%s", variable, argument);
 result = putenv(putenv_var[putenv_ptr]);
 putenv_ptr++;
 return result;
}

double Xrint(value)
double value;
{
 double dbl;
  
 if (value >= 0.0)
 {
  dbl = floor(value);
  if (value - dbl >= 0.5)
   return ceil(value);
  else
   return dbl;
 }
 else
 {
  dbl = ceil(value);
  if (dbl - value >= 0.5)
   return floor(value);
  else
   return dbl;
 }
}


/* ----------------------------------------------------------------- ** 
** Zusleep - usleep                                                  ** 
** ----------------------------------------------------------------- */
void Zusleep(u_during)
long int	u_during;
{
 struct timeval	gtime;

 if (u_during >= 1000000)
 {
  gtime.tv_sec = u_during / 1000000;
  gtime.tv_usec = u_during % 1000000;
 }
 else
 {
  gtime.tv_sec = 0;
  gtime.tv_usec = u_during;
 }
 select(0, NULL, NULL, NULL, &gtime);
}


/* ----------------------------------------------------------------- ** 
** Xc_strconcat - Concat multiples strings into one                  ** 
** ----------------------------------------------------------------- */
char *Xc_strconcat(va_alist)
va_dcl
{
 register char *string, *ptr, *source;
 va_list ap;
 long total;

 /*--- Get final string length ---*/
 va_start(ap);
 for(total = 0; (ptr = va_arg(ap, char *)) != NULL; total += strlen(ptr));
 va_end(ap);

 if((string = Xc_malloc("string", total + 1)) == NULL) return NULL;

 /*--- Concat all strings ---*/
 va_start(ap);
 for(ptr = string; (source = va_arg(ap, char *)) != NULL; )
 {
  register char c;

  while((c = *source++) != 0) *ptr++ = c; 
 }
 va_end(ap);
 *ptr = 0;

 return string;
}


/* ----------------------------------------------------------------- ** 
** Xc_strndup - Duplicate a string chunk into a new string           ** 
** ----------------------------------------------------------------- */
char *Xc_strndup(desc, string, size)
char *desc;
char *string;
long size;
{
 char *ptr;
 long len;

 len = strlen(string);
 if(len > size) len = size;
 if((ptr = Xc_malloc(desc, len + 1)) == NULL) return NULL;
 memcpy(ptr, string, len);
 ptr[len] = 0;
 return ptr;
}

