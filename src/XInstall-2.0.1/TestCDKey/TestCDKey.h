/*
** TestCDKey.h for XInstall in TestCDKey/
** declaration of inlines function to test the validity
** of a cd key.
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
** Started on  Wed Jun 25 16:18:24 1997 Emmanuel Paris
** Last update Wed Jun 25 19:46:34 1997 Emmanuel Paris
*/

#ifndef _TestCDKey_h_
#define _TestCDKey_h_

extern boolean start_cdkey_check ___PROTO((char *keys));
extern boolean check_cdkey_stage1();
extern boolean check_cdkey_stage2();
extern boolean check_cdkey_stage3();
extern boolean check_cdkey_stage4();
extern boolean check_cdkey_stage5();
extern boolean check_cdkey_stage6();
extern int end_cdkey_check();

#endif


























