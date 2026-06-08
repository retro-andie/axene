/*
** checkpoint.h for XInstall in Global/
** Protect macro with checksum from code to be change.
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
** Started on  Mon Jan 12 16:51:19 1998 Emmanuel Paris
** Last update Mon Jan 12 17:12:22 1998 Emmanuel Paris
*/

#include "checksum.h"
#include "crypted.h"
#include "file.h"

extern char *Globprog_name;

#define SET_ZERO(k) \
{ \
    int	macrob = 0; \
    unsigned int macrob2; \
    \
    macrob |= (20 * __LINE__); \
    macrob ^= ~__LINE__; \
    macrob2 = 1 >> (unsigned int)macrob; \
    macrob += (__LINE__ >> 1); \
    macrob2 = 1 >> (++macrob); \
    k = macrob2; \
}

#if defined(___hpux10) || defined(___hpux9)
#define NO_CHECKSUM
#endif

#ifndef NO_CHECKSUM
# if defined(___sun5) || defined(___sun4) || defined(___sparc_linux)
#  define CHECK_POINT	\
{ \
    FILE *fd; \
    unsigned long	addr; \
    register unsigned long	check; \
    register unsigned long	check2; \
    unsigned long	pos; \
    int			readed; \
    unsigned char	buffer[512]; \
    int			i; \
    i = rand(); \
    if (i) \
     i = 0/i;\
    addr = 0x5a4f5247; \
    check = 0x4c4f4e44; \
    addr +=i; \
    check +=i; \
    Xc_TRACE(("addr: %lx", addr)); \
    Xc_TRACE(("check: %lx", check)); \
    fd = fopen(Globprog_name, "r"); \
    fseek(fd, addr, SEEK_SET); \
    SET_ZERO(pos); \
    readed = 512; \
    check2 = 0; \
    while ((readed == 512) && (pos < CHECKSUM_LEN)) \
    { \
	readed = fread(buffer, 1, 512, fd); \
	i = 0; \
	while ((i < readed) && (pos < CHECKSUM_LEN)) \
	{ \
	    check2 += buffer[i]; \
	    i++; \
	    pos++; \
	} \
    } \
    check2 -= check & 0xff; \
    check2 -= (check >> 8) & 0x3; \
    check2 -= (check >> 10) & 0xff; \
    check2 -= (check >> 18) & 0xff; \
    check2 -= (check >> 26) & 0x3f; \
    Xc_TRACE(("check2: %lx", check2)); \
    if ((check2&0xffff) != (check&0xffff)) \
    { \
	SET_ZERO(i); \
	while (crypt_str0[i] != 0) \
	{ \
	    putchar(crypt_str0[i] + 'K'); \
	    i++; \
	} \
	putchar(0x0a); \
	Xc_TRACE(("programm changed")); \
	exit (-1); \
    } \
    fclose(fd); \
} 
# else
#  define CHECK_POINT	\
{ \
    FILE *fd; \
    unsigned long	addr; \
    register unsigned long	check; \
    register unsigned long	check2; \
    unsigned long	pos; \
    int			readed; \
    unsigned char	buffer[512]; \
    int			i; \
    i = rand(); \
    if (i) \
     i = 0/i;\
    addr = 0x5a4f5247; \
    check = 0x4c4f4e44; \
    addr +=i; \
    check +=i; \
    Xc_TRACE(("addr: %lx", addr)); \
    Xc_TRACE(("check: %lx", check)); \
    fd = fopen(Globprog_name, "r"); \
    fseek(fd, addr, SEEK_SET); \
    SET_ZERO(pos); \
    readed = 512; \
    check2 = 0; \
    while ((readed == 512) && (pos < CHECKSUM_LEN)) \
    { \
	readed = fread(buffer, 1, 512, fd); \
	i = 0; \
	while ((i < readed) && (pos < CHECKSUM_LEN)) \
	{ \
	    check2 += buffer[i]; \
	    i++; \
	    pos++; \
	} \
    } \
    i=0; \
    while (i < 4) \
    { \
	check2 -= (check >> (i*8)) & 0xff; \
	i++; \
    } \
  Xc_TRACE(("check2: %lx", check2)); \
  if ((check2&0xffff) != (check&0xffff)) \
    { \
	SET_ZERO(i); \
        printf("i: %d\n", i); \
	while (crypt_str0[i] != 0) \
	{ \
	    putchar(crypt_str0[i] + 'K'); \
	    i++; \
	} \
	putchar(0x0a); \
	Xc_TRACE(("programm changed")); \
	exit (-1); \
    } \
    fclose(fd); \
} 
# endif
#else
# define CHECK_POINT 
#endif
