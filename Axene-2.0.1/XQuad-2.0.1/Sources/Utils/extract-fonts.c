#include <stdio.h>

int main(argc, argv)
int argc;
char **argv;
{
  FILE *in, *out;
  char buffer[258], outfile[256];
  
  out = NULL;
  while(argv++, --argc)
    if((in = fopen(*argv, "r")) != NULL)
      while(!feof(in))
        {
          fgets(buffer, 256, in);
          if(out == NULL)
            if(sscanf(buffer, "%%!FontType1-1.0: %s", outfile) == 1)
              {
                puts(outfile);
                if((out = fopen(outfile, "w")) == NULL) exit(1);
              }
            else
              continue;
           
           fputs(buffer, out);
           if(!strncmp(buffer, "cleartomark", 11))
             {
               fclose(out);
               out = NULL;
             }
         }

}
