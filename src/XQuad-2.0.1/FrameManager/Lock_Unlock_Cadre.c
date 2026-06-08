/*
 * Lock_Unlock_Cadre.c  - function definition for locking cadre
 *
 * Copyright (C) 1994	Xcalibur <xcalibur@axene.org>
 *
 * This file can be redistributed under the terms of the GNU General
 * Public License
 */

#include "FrameManager.h"

void lock_cadre(cfrmg)
c_FrMg	*cfrmg;
{
  l_Cadre	*lcadre;
  
  if (cfrmg->verrou)
    {
      F(cfrmg).unlock_cadre(cfrmg);
      return;
    }
  cfrmg->verrou=TRUE;
  lcadre=cfrmg->SelectedCadre;
  while(lcadre)
  {
    lcadre->cadre->verrou=TRUE;
    lcadre=lcadre->NextCadre;
  }
}

void unlock_cadre(cfrmg)
c_FrMg	*cfrmg;
{
  l_Cadre	*lcadre;
  
  if (cfrmg->verrou)
  {
    cfrmg->verrou=FALSE;
    lcadre=cfrmg->SelectedCadre;
    while(lcadre)
    {
      lcadre->cadre->verrou=FALSE;
      lcadre=lcadre->NextCadre;
    }
  }
}
  
