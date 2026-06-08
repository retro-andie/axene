/*
 * xpmrsize.c - get an xpm real size (when loaded in memory).
 * writen by Xibios 2 for Axene Inc.
 */


#include <stdio.h>
#include <stdlib.h>

int main(argc, argv)
int argc;
char **argv;
{
  char str[400];
  FILE *input = stdin;
  int  rsize = 0;
  
  if (argc > 2)
  {
    printf("Usage:  %s [input_file]\n", argv[0]);
    return -1;
  }
  
  if (argc > 1) 
  {
    input = fopen(argv[1], "rb");
    if (!input) input = stdin;
  }
  
  while(fgets(str, 400, input))
  {
    int i;
    
    i = 0;
    while(str[i] && str[i] != 34 && str[i] != '/')
      i++;
    
    if (str[i] == '/') continue;
    
    if (str[i])
    {
      i++;
      rsize++;
      while(str[i] && str[i] != 34)
      {
	rsize++;
	i++;
      }
    }
  }
  printf("%d\n", rsize);
  return 0;
}







