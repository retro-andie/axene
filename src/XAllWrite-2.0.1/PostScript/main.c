#include "PostScript.h"

c_PostScript	*print;

int main()
{
  print = (c_PostScript *)NEW(c_PostScript)("gato.ps", 0);
  F(print).startPage(print, 1);
  F(print).putLine(print, "test $s $d $c $r",
		   "gato",
		   (long) 75,
		   SCALE_FROM_CENTIMETERS(1),
		   (double)3.1415
		   );
  F(print).endPage(print);
  DELETE(c_PostScript)(print);
  return 0;
}
