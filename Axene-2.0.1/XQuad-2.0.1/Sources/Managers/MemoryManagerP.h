/*
** MemoryManagerP.h for Xclamation, XQuad, XAllWrite, XMayday, XInstall and 
** 	AxeneOffice in Managers/
** Private header for MemoryManager
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
** Started on  Tue Mar 21 13:10:58 1995 Stéphane Boisson
** Last update Tue Jan 20 17:53:04 1998 Robin Castanier
*/

#ifndef __XC_MEMORY_MANAGER_P__
#define __XC_MEMORY_MANAGER_P__

#ifndef NTRACE
#define NTRACE
#endif
#ifndef NDEBUG
#define NDEBUG
#endif
#ifndef NMEMDEBUG
#define NMEMDEBUG
#endif

#include "MemoryManager.h"
#include "ErrorManager.h"

extern void *malloc();
extern void *realloc();

/*--- Default value for slot_per_chunk ---*/
#define SLOTS_PER_CHUNK 100

void itemInitList ___PROTO((size_t nslots, size_t size, void *slots));

#endif /* !__XC_MEMORY_MANAGER_P__ */
