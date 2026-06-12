/*
** misc.c for XQuad in Global/
**
** Copyright (C) 1995-2000 Axene.
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
** Started on  Mon Jul 17 17:35:34 1995 Emmanuel Paris
** Last update Tue Jan 18 16:18:06 2000 Emmanuel Paris
*/


#define NTRACE

#include "xcalibur.h"
#include <sys/time.h>
#include <sys/types.h>
#include <stdarg.h>

/*--- local include for Xcf_gethostbyname ---*/
#include <netdb.h>

static char static_buffer[MAX(sizeof(XSegment) * MAX_SEGMENT, 
			      sizeof(XPoint) * MAX_POINT)];
XSegment *segments = (XSegment *)static_buffer;
XPoint *points = (XPoint *)static_buffer;

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
 i=0;
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
 i=0;
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

 i=strlen(chaine)-1;
 while(i>=0 && chaine[i]!=c)
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
  while(*chaine1!=*chaine2 && *chaine1)
   chaine1++;
  if (*chaine1)
  {
   svch1=chaine1;
   svch2=chaine2;
   while(*chaine1==*chaine2 && *chaine1 && *chaine2)
   {
    chaine1++;
    chaine2++;
   }
   if (!*chaine2)
    return svch1;
   else
   {
    chaine1=svch1+1;
    chaine2=svch2;
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

void TriSimple(nel,liste)
int nel;
int *liste;
{
 int mini,pos_mini;
 int i,j;
  
 for(i=0;i<nel;i++)
 {
  mini=liste[i];
  pos_mini=i;
  for(j=i+1;j<nel;j++)
  {
   if (liste[j]<mini)
   {
    mini=liste[j];
    pos_mini=j;
   }
  }
  liste[pos_mini]=liste[i];
  liste[i]=mini;
 } 
}

void TriSimpleForCoord_t(nel,liste)
int nel;
coord_t *liste;
{ 
 coord_t mini;
 int pos_mini;
 int i,j;
  
 for(i=0;i<nel;i++)
 {
  mini=liste[i];
  pos_mini=i;
  for(j=i+1;j<nel;j++)
  {
   if (liste[j]<mini)
   {
    mini=liste[j];
    pos_mini=j;
   }
  }
  liste[pos_mini]=liste[i];
  liste[i]=mini;
 } 
}


/* ----------------------------------------------------------------- ** 
** strfind - find with the KMP algorithm                             ** 
** ----------------------------------------------------------------- */
int strfind(Text, Pattern, Textlen)
char *Text;
char *Pattern;
int Textlen;
{
 char	*echec;
 int	Patternlen;
 int	varl;
 int	k;
 int	Patterncursor;
 int	Textcursor;

 /* calcul du tableau d'echec */
 Patternlen = strlen(Pattern);
 echec = (char *)Xc_malloc("pattern", Patternlen);
 echec[0] = -1;
 for (k = 1; k<Patternlen; k++)
 {
  varl = echec[k-1];
  while ((varl >= 0) && (Pattern[varl] != Pattern[k-1]))
   varl = echec[varl];
  echec[k] = varl+1;
 }
 /* recherche de la pattern */
 Textcursor=0;
 Patterncursor=0;
 while (Textcursor<Textlen)
 {
  while ((Patterncursor >= 0) 
	 && (Pattern[Patterncursor] != Text[Textcursor]))
   Patterncursor = echec[Patterncursor];
  if (Patterncursor == Patternlen-1)
  { 
   Xc_free(echec);
   return Textcursor+1;
  }
  else
  {
   Textcursor++;
   Patterncursor++;
  }
 }
 Xc_free(echec);
 return -1;
}


/* ----------------------------------------------------------------- ** 
** Xc_strconcat - Concat multiples strings into one                  ** 
** ----------------------------------------------------------------- */
char *Xc_strconcat(const char *first, ...)
{
 register char *string, *ptr, *source;
 va_list ap;
 long total;

 if (first == NULL) return NULL;

 /*--- Get final string length ---*/
 total = (long)strlen(first);
 va_start(ap, first);
 while ((ptr = va_arg(ap, char *)) != NULL)
  total += (long)strlen(ptr);
 va_end(ap);

 if((string = Xc_malloc("string", total + 1)) == NULL) return NULL;

 /*--- Concat all strings ---*/
 ptr = string;
 source = (char *)first;
 while (*source) *ptr++ = *source++;
 va_start(ap, first);
 while ((source = va_arg(ap, char *)) != NULL)
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

#if defined(___alpha) || defined(___ncr_svr4) || defined(___sun4)
/* ----------------------------------------------------------------- ** 
** Xcf_gethostbyname - emulate gethostbyname                         ** 
** ----------------------------------------------------------------- */
struct hostent *Xcf_gethostbyname(name)
char	*name;
{
 struct hostent	*tmp_host;
 char			*tmp_name;
 int			i;
 unsigned char		tmp_addr[4];
 unsigned char		*tmp_addr2;
 int			j;
 boolean		is_net_addr;

 sethostent(0);
 tmp_host = gethostent();

 is_net_addr = TRUE;
 j=0;
 i=0;
 memset(tmp_addr, 0, 4);
 while (j!=4)
 {
  if ((name[i]>='0') && (name[i]<='9'))
  {
   while ((name[i]>='0') && (name[i]<='9'))
   {
    tmp_addr[j]*=10;
    tmp_addr[j]+=name[i]-'0';
    i++;
   }
  }
  else
  {
   is_net_addr=FALSE;
   break;
  }
  if ((name[i]!='.') && (j<3))
  {
   is_net_addr=FALSE;
   break;
  }
  i++;
  j++;
 }
 if (is_net_addr)
  i--;
 while (name[i]==' ')
  i++;
 if (name[i]!=0)
  is_net_addr=FALSE;

 while (tmp_host)
 {
  if (!is_net_addr)
  {
   /* search for name */
   tmp_name = tmp_host->h_name;
   if (Xstrcmp(tmp_name, name))
   {
    endhostent();
    return tmp_host;
   }
   tmp_name = tmp_host->h_aliases[0];
   i=1;
   while (tmp_name)
   {
    if (Xstrcmp(tmp_name, name))
    {
     endhostent();
     return tmp_host;
    }
    tmp_name = tmp_host->h_aliases[i];
    i++;
   }
  }
  else
  {
   /* search for addr */
   i=1;
   tmp_addr2 = (unsigned char *)tmp_host->h_addr_list[0];
   if (!memcmp(tmp_addr2, tmp_addr, 4))
   {
    endhostent();
    return tmp_host;
   }
  }
  tmp_host = gethostent();
 }
 endhostent();
 return NULL;
}
#endif

#if defined(___sco386)

#include <sys/socket.h>
/* ----------------------------------------------------------------- ** 
** Xcf_gethostbyname - emulate gethostbyname                         ** 
** ----------------------------------------------------------------- */
struct hostent *Xcf_gethostbyname(name)
char	*name;
{
 char			tmp_addr[4];
 char			*tmp_addr2;
 int			i, j;
 boolean		is_net_addr;

 is_net_addr = TRUE;
 j=0;
 i=0;
 memset(tmp_addr, 0, 4);
 while (j!=4)
 {
  if ((name[i]>='0') && (name[i]<='9'))
  {
   while ((name[i]>='0') && (name[i]<='9'))
   {
    tmp_addr[j]*=10;
    tmp_addr[j]+=name[i]-'0';
    i++;
   }
  }
  else
  {
   is_net_addr=FALSE;
   break;
  }
  if ((name[i]!='.') && (j<3))
  {
   is_net_addr=FALSE;
   break;
  }
  i++;
  j++;
 }

 if (is_net_addr) i--;
 while (name[i]==' ')
  i++;
 if (name[i]!=0)
  is_net_addr=FALSE;
  
 if (is_net_addr)
 {
  return gethostbyaddr(tmp_addr, 4, AF_INET);
 }
 else
 {
  return gethostbyname(name);
 }
}
#endif

/* ----------------------------------------------------------------- ** 
** special tablor cells conversion                                   ** 
** ----------------------------------------------------------------- */
char *regtostr(region)
region_t	*region;
{
 char		fstr[40];
 int		index;
 int		indext;
 char		aff[8];
 int		x;
 char		*result;

 x = region->x_min;
 indext = 6;
 do
 {
  aff[indext]=(x % 26)+'A';
  indext--;
  x /= 26;
 }while(x--);
 aff[7] = '\0';
 sprintf(fstr, "%s%d", aff+indext+1, region->y_min+1);
 index = strlen (fstr);
 x = region->x_max;
 indext = 6;
 do
 {
  aff[indext]=(x % 26)+'A';
  indext--;
  x /= 26;
 }while(x--);
 aff[7] = '\0';
 sprintf(fstr+index, ":%s%d", aff+indext+1, region->y_max+1);
 result = (char *)Xc_malloc("regtostr", strlen(fstr)+1);
 strcpy(result, fstr);
 return result;
}

region_t *strtoreg(string)
char *string;
{
 region_t	*region;
 int		x_min, x_max;
 int		y_min, y_max;
 int		nb_letter;
 int		pos,i,j,k,n;
  
 nb_letter = 0;
 while(string[nb_letter]>='A' && string[nb_letter]<='Z')
  nb_letter++;
  
 if (!nb_letter || nb_letter>2)
  return NULL;
  
 i=nb_letter;
 while(string[i] && string[i]!=':' && string[i]>='0' && string[i]<='9')
  i++;
 if (string[i]!=':')
  return NULL;
  
 x_min = 0;
 j = nb_letter-1;
 k = 0; n=1;
 while(j>=k)
 {
  x_min += (string[j]-'A'+(n==1?0:1))*n;
  n = n*26;
  j--;
 }
 y_min = atoi(string+nb_letter)-1;
  
 nb_letter = 0; i++;
 while(string[nb_letter+i]>='A' && string[nb_letter+i]<='Z')
  nb_letter++;
  
 if (!nb_letter || nb_letter>2)
  return NULL;

 pos = i; i+=nb_letter; 
 while(string[i] && string[i]>='0' && string[i]<='9')
  i++;
 if (string[i])
  return NULL;
  
 x_max = 0;
 j = pos + nb_letter-1;
 k = pos; n=1;
 while(j>=k)
 {
  x_max += (string[j]-'A'+(n==1?0:1))*n;
  n = n*26;
  j--;
 }
 y_max = atoi(string+pos+nb_letter)-1;
  
 if (x_min>x_max || y_min>y_max)
  return NULL;
  
 if ( (region = (region_t *)Xc_malloc("str2reg", sizeof(region_t))) == NULL)
  return NULL;
 region->x_min = x_min;
 region->x_max = x_max;
 region->y_min = y_min;
 region->y_max = y_max;
 return region;
}

void colrowtostr(column, row, str)
int	column;
int	row;
char	*str;
{
 int		indext;
 char		aff[8];
 int		x;
  
 x = column;
 indext = 6;
 do
 {
  aff[indext]=(x % 26)+'A';
  indext--;
  x /= 26;
 }
 while(x--);
 aff[7] = '\0';
 sprintf(str, "%s%d", aff+indext+1, row+1);
}

boolean strtocolrow(string, column, row)
char	*string;
int	*column;
int	*row;
{
 int		cold, rowd;
 int		nb_letter;
 int		i,j,k,n;
  
 nb_letter = 0;
 while((string[nb_letter]>='A' && string[nb_letter]<='Z') || 
       (string[nb_letter]>='a' && string[nb_letter]<='z'))
  nb_letter++;
  
 if (!nb_letter || nb_letter>2)
  return FALSE;
  
 i=nb_letter;
 while(string[i] && string[i]>='0' && string[i]<='9')
  i++;
 if (string[i])
  return FALSE;
  
 cold = 0;
 j = nb_letter-1;
 k = 0; n=1;
 while(j>=k)
 {
  cold += (string[j]-(string[j]>'Z'?'a':'A')+(n==1?0:1))*n;      
  n = n*26;
  j--;
 }
 rowd = atoi(string+nb_letter)-1;
 if (cold >= 0 && cold <= COLUMN_MAX && rowd >= 0 && rowd <= ROW_MAX)
 {
  *column = cold;
  *row = rowd;
  return TRUE;
 }
 return FALSE;
}

void coltostr(column, lng, str)
int	column;
int	*lng;
char	*str;
{
 int		indext;
 char		aff[8];
 int		x;
  
 x = column;
 indext = 6;
 do
 {
  aff[indext]=(x % 26)+'A';
  indext--;
  x /= 26;
 }
 while(x--);
 aff[7] = '\0';
 sprintf(str, "%s", aff+indext+1);
 if (lng)
  *lng = 6 - indext;
}

boolean strtocol(string, column)
char	*string;
int	*column;
{
 int		cold;
 int		nb_letter;
 int		j,k,n;
  
 nb_letter = 0;
 while((string[nb_letter]>='A' && string[nb_letter]<='Z') || 
       (string[nb_letter]>='a' && string[nb_letter]<='z'))
  nb_letter++;
  
 if (!nb_letter || nb_letter>2 || string[nb_letter])
  return FALSE;
    
 cold = 0;
 j = nb_letter-1;
 k = 0; n=1;
 while(j>=k)
 {
  cold += (string[j]-(string[j]>'Z'?'a':'A')+(n==1?0:1))*n;      
  n = n*26;
  j--;
 }

 if (cold >= 0 && cold <= COLUMN_MAX)
 {
  *column = cold;
  return TRUE;
 }
 return FALSE;
}
