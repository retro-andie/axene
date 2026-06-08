/*
** Tar.c for XInstall in Tar/
** extract file from tar
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
** Started on  Thu Sep 18 15:57:54 1997 Antoine Buat
** Last update Thu Feb  3 15:36:06 2000 Emmanuel Paris
*/

/*--- include ---*/
#include "xcalibur.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Tar.h"

/*--- define ---*/
#define BLOCK_LOAD	30

#define	ISODIGIT(Char) \
((unsigned char) (Char) >= '0' && (unsigned char) (Char) <= '7')
#define ISSPACE(Char) (isspace (Char))


int extract_tar();
static long from_oct();
static int make_dirs();
int extract_tar_memory();
#ifndef NTRACE
static void print_buf();
#endif

/* ----------------------------------------------------------------- ** 
** extract tar                                                       ** 
** ----------------------------------------------------------------- */
int extract_tar(This, init_f, change, fd, p_tsize, all_total)
void		*This;
void		*(init_f)();
void		*(change)();
int		fd;
unsigned long	p_tsize;
unsigned long	*all_total;
{
 char	*buffer;
 int	size;
 int	tsize;
 long	p_total;
 long	fmode;
 long	fsize;
 long	ftsize;
 long	fmtime;
 long	fchecksum;
 int	ftype;
 int	fblocks;
 int	fd_out;
 int	cur_size;
 int		zap_space;

 buffer = Xc_malloc("file", 512*BLOCK_LOAD+1);
 p_total = 0;
 
 while ((size = read(fd, buffer, 512)))
 {
  if (strncmp("ustar", buffer+0x101, 5))
  {
   /*--- test for void block ? ---*/
   memset(buffer+512, 0, 512);
#ifndef NTRACE
    print_buf(buffer, 512);
    Xc_TRACE(("next for readed : %d", size));
    print_buf(buffer+512, 512);
#endif
    Xc_free(buffer);
    return -1;
  }
  fmode = from_oct(8, buffer+100);
#if 0
  fuid = from_oct(8, buffer+108);
  fgid = from_oct(8, buffer+116);
#endif
  fsize = from_oct(12, buffer+124);
  ftsize = fsize;
  fmtime = from_oct(12, buffer+136);
  fchecksum = from_oct(8, buffer+148);
  ftype = from_oct(1, buffer+156);
  fd_out = 0;
  switch (ftype)
  {
  case 0:
   fd_out = open(buffer, O_WRONLY | O_CREAT | O_TRUNC , fmode);
   if (fd_out == -1)
   {
    Xc_SYSERROR(("open error: opening for writing\n%s", buffer));
    continue;
   }
   break;
  case 2:
   /*--- create sym link ---*/
   symlink(buffer+157, buffer);
   continue;
  case 5:
   make_dirs(buffer, fmode);
   continue;
  }

  /*printf("%ld Current file: %s\n", fsize, buffer);*/
  zap_space = 0;
  if (!strcmp(buffer, ".Icon") || !strcmp(buffer, ".Axene_desc"))
   zap_space = 1;

  if (init_f)
   init_f(This, buffer, ftsize);

  cur_size = 0;
  if (fsize%512)
   fblocks=(fsize/512)+1;
  else
   fblocks=(fsize/512);
  /*printf("file_size: %ld, blocks: %d\n", fsize, fblocks);*/
  while(fblocks)
  {
   if (fblocks > BLOCK_LOAD)
   {
    size = 0;
    while (size != BLOCK_LOAD*512)
    {
     tsize = read(fd, buffer+size, BLOCK_LOAD*512-size);
     if (!tsize)
     {
      Xc_WARNING(("archive truncated"));
      Xc_free(buffer);
      return -1;
     }
     size += tsize;
    }
    fblocks -= BLOCK_LOAD;
    size = write(fd_out, buffer, BLOCK_LOAD*512);
    if (size < BLOCK_LOAD*512)
    {     
     Xc_SYSERROR(("write error: at offset %d of file\n%s", size, buffer));
     Xc_free(buffer);
     return -1;
    }
    fsize -= BLOCK_LOAD*512;
    cur_size += BLOCK_LOAD*512;
    if (!zap_space)
    {
     p_total += BLOCK_LOAD*512;
     *all_total += BLOCK_LOAD*512;
    }
    if (!fblocks)
     close(fd_out);

    if (change)
     change(This, cur_size, ftsize, p_total, p_tsize, *all_total);
   }
   else
   {
    size = 0;
    while (size != fblocks*512)
    {
     tsize = read(fd, buffer+size, fblocks*512-size);
     if (!tsize)
     {
      Xc_WARNING(("archive truncated"));
      Xc_free(buffer);
      return -1;
     }
     size += tsize;
    }
    size = write(fd_out, buffer, fsize);
    if (size < fsize)
    {
     Xc_SYSERROR(("write error: at offset %d of file\n%s", size, buffer));
     Xc_free(buffer);
     return -1;
    }
    if (!zap_space)
    {
     p_total += fsize;
     *all_total += fsize;
    }
    close(fd_out);
    fsize = 0;
    fblocks = 0;

    if (change)
     change(This, ftsize, ftsize, p_total, p_tsize, *all_total);
   }
  }
 }
 Xc_free(buffer);
 return 0;
}

/* ----------------------------------------------------------------- ** 
** convert from octal                                                ** 
** ----------------------------------------------------------------- */
static long from_oct(len, str)
int	len;
char	*str;
{
 long value;

 /*--- skip spaces ---*/
 while (ISSPACE(*str))
 {
  str++;
  if (--len <= 0)
   return -1;
 }
 value = 0;
 while ((len > 0) && ISODIGIT(*str))
 {
  value = (value << 3) | (*str++ - '0');
  --len;
 }
 if ((len > 0) && (*str) && !ISSPACE(*str))
  return -1;
 return value;
}

/* ----------------------------------------------------------------- ** 
** make directory                                                    ** 
** ----------------------------------------------------------------- */
static int make_dirs(pathname, mode)
char	*pathname;
mode_t	mode;
{
 char *p, c;
 int madeone = 0;
 int i, j, len, check;
 
 i = -1;
 if (pathname[0] == '.' && pathname[1] == '/') pathname++;
 if (pathname[0] == '/') pathname++; 
 p = pathname;
 len = strlen(p);
 if (len == 0) return 0;
 do
 {
  j = ++i;
  while(i < len)
  {
   if (p[i] == '/') break;
   i++;
  }
  c = p[i]; p[i] = '\0';
  
  /*--- try to create it as a dir ---*/
  check = mkdir (pathname, mode);
  p[i] = c;
  if (check == 0)
  {
   /*--- remember if we made one ---*/
   madeone++;			
  }
  else
  {
   /*--- Directory already exists. ---*/
   if (errno != EEXIST)
   {
    Xc_SYSERROR(("mkdir error: can not create\n%s", pathname));
    break;
   }
  }
 } while(i < len);
 return madeone;

#if 0
 char	*p;
 int	madeone = 0;
 int	check;

 for (p = strchr(pathname, '/'); p != NULL; p = strchr(p + 1, '/'))
 {
  /*--- Avoid mkdir of empty string, if leading or double '/'.  ---*/
  if (p == pathname || p[-1] == '/')
   continue;

  /*--- Avoid mkdir where last part of path is '.'.  ---*/
  if (p[-1] == '.' && (p == pathname + 1 || p[-2] == '/'))
   continue;

  /*--- truncate the path there ---*/
  *p = 0;			
  /*--- try to create it as a dir ---*/
  check = mkdir (pathname, mode);
  printf("mkdir %s result: %d\n", pathname, check);
  if (check == 0)
  {
   /*--- remember if we made one ---*/
   madeone++;			
   *p = '/';
   continue;
  }
  *p = '/';
  /*--- Directory already exists. ---*/
  if (errno == EEXIST)
   continue;
  else
   Xc_SYSERROR(("mkdir error: can not create\n%s", pathname));
  break;
 }
 /*--- tell them to retry if we made one ---*/
 return madeone;
#endif
}

#ifndef NTRACE
/* ----------------------------------------------------------------- ** 
** print buf                                                         ** 
** ----------------------------------------------------------------- */
static void print_buf(nbuf, readed)
unsigned char	*nbuf;
int	readed;
{
 int	i;
 int	j;

 i=0;
 while(i < readed)
 {
  j=0;
  while ((j < 16) && (i < readed))
  {
   printf("%02x ", nbuf[i]);
   j++;
   i++;
  }
  i-=j;
  j=0;
  printf(" ");
  while ((j < 16) && (i < readed))
  {
   if (nbuf[i]<32)
    printf(".");
   else
    printf("%c", nbuf[i]);
   j++;
   i++;
  }
  printf("\n");
 }
}
#endif

/* ----------------------------------------------------------------- ** 
** extract tar to memory                                             ** 
** ----------------------------------------------------------------- */
int extract_tar_memory(fd, mf)
int		fd;
mem_file	**mf;
{
 char		*buffer;
 int		size;
 int		tsize;
 long		fsize;
 long		ftsize;
 int		ftype;
 int		fblocks;
 mem_file	*pmf;

 buffer=Xc_malloc("file", 512*2+1);
 /*Xc_mdump();*/
 /*Xc_mcheck();*/
 while ((size = read(fd, buffer, 512)))
 {
  if (strncmp("ustar", buffer+0x101, 5))
  {
   /*--- test for void block ? ---*/
   memset(buffer+512, 0, 512);
   if (memcmp(buffer, buffer+512, 512))
   {
#ifndef NTRACE
    print_buf(buffer, 512);
    Xc_TRACE(("next for readed : %d", size));
    print_buf(buffer+512, 512);
#endif
    Xc_WARNING(("Can't extract this package\n"));
    Xc_free(buffer);
    return -1;
   }
   continue;
  }
  fsize = from_oct(12, buffer+124);
  if (fsize > 10000)
  {
   Xc_free(buffer);
   return -1;
  }
  ftsize = fsize;
  ftype = from_oct(1, buffer+156);
  if (fsize%512)
   fblocks=(fsize/512)+1;
  else
   fblocks=(fsize/512);
  switch (ftype)
  {
  case 0:
   if (*mf)
    mf = &((*mf)->Next);
   (*mf) = (mem_file *)Xc_malloc("mem_file", sizeof(mem_file));
   (*mf)->name = Xc_malloc("str", strlen(buffer)+1);
   strcpy((*mf)->name, buffer);
   /*printf("Found: %s (%d)\n", (*mf)->name, fblocks*512);*/
   (*mf)->size = fsize;
   (*mf)->buffer = Xc_malloc("buf_mf", fblocks*512);
   (*mf)->Next = NULL;
   break;
  case 2:
   /*--- skip sym link ---*/
   continue;
  case 5:
   /*--- skip create dir ---*/
   continue;
  }


  size = 0;
  while (size != fblocks*512)
  {
   tsize = read(fd, (*mf)->buffer+size, fblocks*512-size);
   if (tsize <= 0)
   {
    Xc_TRACE(("archive truncated\n"));
    Xc_free((*mf)->buffer);
    Xc_free((*mf)->name);
    pmf = (*mf);
    (*mf) = NULL;
    Xc_free(pmf);
    Xc_free(buffer);
    return -1;
   }
   size += tsize;
  }
 }
 Xc_free(buffer);
 return 0;
}
