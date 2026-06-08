#include "VectorFont.h"

int main(argc, argv)
int argc;
char **argv;
{
  c_VectorFont *font;

  if((font = NEW(c_VectorFont)("test.pfa")) == NULL)
    Xc_exit(-1);
  Xc_mcheck();
  F(font).dump(font);
  DELETE(c_VectorFont)(font);
  Xc_mdump();
}
