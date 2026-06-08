/*
** typeseed.h for XInstall in Global/
** define Xc_CRYPT_SEED
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
** Started on  Thu Oct  8 16:44:41 1998 Antoine Buat
** Last update Thu Oct  8 16:46:01 1998 Antoine Buat
*/

#ifndef _osid_h_
#define _osid_h_

#define Xc_CRYPT_SEED 89456

#ifdef SPECIAL_EDITION_ID
# if SPECIAL_EDITION_ID == 1
#  define Xc_CRYPT_SEED2 957543
#  ifndef SPECIAL_EDITION
#  define SPECIAL_EDITION "Dream0398"
#  endif
# endif
#else /* SPECIAL_EDITION_ID */
# define Xc_CRYPT_SEED2 Xc_CRYPT_SEED
#endif	

#define Xc_MERGE_CRYPT_SEED  (Xc_CRYPT_SEED ^ Xc_CRYPT_SEED2)

#endif
