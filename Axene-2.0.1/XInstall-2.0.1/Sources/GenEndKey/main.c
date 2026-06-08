/*
** main.c for XInstall in GenEndKey/
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
** Started on  Wed Jun 25 18:04:24 1997 Emmanuel Paris
** Last update Mon Jun 30 21:23:57 1997 Emmanuel Paris
*/

#include "xcalibur.h"
#include "file.h"
#include "TestCDKey.h"
#include "TestIDFile.h"
#include "GenEndKey.h"

/* global declarations */
char	*Globprog_name;
int seed;
int soft;
int version;
/* end of global declaration */

static char *soft_name[] = { "Office", "Xclamation", "XQuad", "XAllWrite" };

int main(argc, argv)
int argc;
char **argv;
{
 char		cdkey[] = "141-1543823164";
 char		name[] = "Emmanuel Paris";
 char		society[] = "Axène";
 char		installdir[] = "/tmp";
 boolean	key_validity, fileid_validity;

 Globprog_name = SearchFile(argv[0]);

 key_validity = start_cdkey_check(cdkey);
 
 printf("%s - %s v%d.%d\n", key_validity ? "VALID" : "INVALID",
	soft_name[soft], version / 10, version % 10);

 key_validity = check_cdkey_stage1(); printf("Stage1: ");
 if (key_validity) printf("passed\n"); else { printf("failed\n"); return 1; }

 key_validity = check_cdkey_stage2(); printf("Stage2: ");
 if (key_validity) printf("passed\n"); else { printf("failed\n"); return 1; }

 key_validity = check_cdkey_stage3(); printf("Stage3: ");
 if (key_validity) printf("passed\n"); else { printf("failed\n"); return 1; }
 
 key_validity = check_cdkey_stage4(); printf("Stage4: ");
 if (key_validity) printf("passed\n"); else { printf("failed\n"); return 1; }
 
 key_validity = check_cdkey_stage5(); printf("Stage5: ");
 if (key_validity) printf("passed\n"); else { printf("failed\n"); return 1; }
 
 key_validity = check_cdkey_stage6(); printf("Stage6: ");
 if (key_validity) printf("passed\n"); else { printf("failed\n"); return 1; }
 
 seed = end_cdkey_check();
 fileid_validity = test_idfile(); printf("Test Id file: ");
 if (fileid_validity) printf("passed\n"); else { printf("failed\n"); return 1;}
 printf("final seed value: %d (should be = 670892)\n", seed);
 
 printf("Generate a key for %s of %s\n", name, society);
 gen_end_key(cdkey, name, society, ":0.0", installdir);

  Xc_free(Globprog_name);
 return 0;
}

void Xc_SyserrorAlert(message)
char *message;
{
 perror(message);
}



