#include <stdio.h>

main(argn, argv)
int argn;
char **argv;
{
  FILE *in, *out;
  char buffer[256];
  int count = -1;

  if((in = fopen(argv[1], "r")) == NULL)
    perror(argv[1]), exit(1);

  printf("%%{\n#include \"xcalibur.h\"\n%%}\nstruct code_t { char *name; char_t code;};\n%%%%\n");
  while(!feof(in))
    {
      char *ptr;

      if(fgets(buffer, 256, in)==0) continue;
      if((buffer[0] == '%') || (buffer[0] == 0) ) continue;
      ptr = buffer + strlen(buffer) - 1;
      if(buffer[0] == 0) continue;
      if(*ptr == '\n') *ptr-- = 0; 
      if(count < 0)
	printf("#%s\n", buffer);
      else
	if(strcmp(".notdef", buffer))
	  printf("%s, %d\n", buffer, count);
      count++;
    }
  fclose(in);
}
