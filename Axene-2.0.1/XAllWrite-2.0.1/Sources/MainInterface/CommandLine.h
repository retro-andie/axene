/*
** CommandLine.h for XAllWrite in MainInterface/
** Help String for command-line options
**
** Copyright (C) 1996-2000 Axene.
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
** Started on  Tue Apr 23 15:58:52 1996 Emmanuel Paris
** Last update Fri Feb  4 11:24:01 2000 Emmanuel Paris
*/

static char helpDisplay[] = "\
usage:   XAllWrite [-help] [-version] [-fg <color>] [-bg <color>]\n\
         [-display <displayname>] [-geometry <geom>] [-/+opaqueResize]\n\
         [-opaqueMove <percentage>] [-/+ownColormap]\n\
         [-keepFreeColor <number>] [files ...]\n\
\n\
detail:\n\
    -help, --help, -h            give this help\n\
    -version, -v                 display version number\n\
    -fg color                    foreground color\n\
    -bg color                    background color\n\
    -display host:dpy            X server to use\n\
    -geometry geom               size of main window\n\
    -opaqueResize                toggle off internal opaque resize\n\
    +opaqueResize                toggle on internal opaque resize\n\
    -opaqueMove percentage       set opaque move percentage\n\
    -ownColormap                 use public colormap\n\
    +ownColormap                 create a private colormap\n\
    -keepFreeColor number        number of color to keep free\n\
\n\
";
static char copyrightDisplay[] = "\
Copyright 1997-2000 Axene.";

