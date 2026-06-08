#include <stdio.h>
#include "mainzip.h"

int main(argc, argv)
int	argc;
char	**argv;
{
 if ((argc < 2) || (argc > 2))
  {
   printf("Usage: %s <file>\n", argv[0]);
   return -1;
  }

 if (gunzip(argv[1])<0)
  return -1;
 return 0;
}
