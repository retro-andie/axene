/*
 * giftoh.c - read a gif file and mute it in an ascii form
 * writen by Xibios 2 for Axene Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __GNUC__
#include <unistd.h>
#endif

#if defined(___hpux10)
# define REPLACE_BIN 0
#else
# define REPLACE_BIN 1
#endif

#ifndef NULL
#define NULL (void *)0
#endif

#ifndef SEEK_END
#define SEEK_END L_XTND 
#endif

#ifndef SEEK_SET
#define SEEK_SET L_SET
#endif

#ifndef SEEK_CUR
#define SEEK_CUR L_INCR 
#endif

#if !REPLACE_BIN
static char *hex_conv = "0123456789abcdef";
#endif

int main(argc, argv)
int argc;
char **argv;
{
 char *filename, *outfilename, *statstr;
 FILE *input, *output;
 int  fsize, i;
#if !REPLACE_BIN
 char str[12], out[100];
 int lns, j;
#endif
 
 if (argc == 1 || argc > 2)
 {
  printf("Usage:  %s [gif_file]\n", argv[0]);
  return -1;
 }

 filename = strrchr(argv[1], '.');
 if (filename == NULL || 
     ( filename[1] == 'g' && filename[1] == 'i' && filename[1] == 'f'))
 {
  printf("%s must have .gif extensions.\n", argv[1]);
  return -1;
 }
  
 outfilename = (char *)malloc(strlen(argv[1]) + 1);
 statstr = (char *)malloc(strlen(argv[1]) + 1);
 for(i = 0; i < (filename - argv[1]); i++)
 {
  outfilename[i] = argv[1][i];
 }
 outfilename[i] = '.';
 outfilename[i+1] = 'h';
 outfilename[i+2] = '\0';

 strcpy(statstr, argv[1]);
 for(i = 0; i < strlen(statstr); i++)
 {
  if (statstr[i] == '.')
   statstr[i] = '_';
 }
 
 input = fopen(argv[1], "rb");
 output = fopen(outfilename, "wb");

 fseek(input, 0, SEEK_END);
 fsize = (int)ftell(input);
 fseek(input, 0, SEEK_SET);

 fprintf(output, "/* GIF */\n#define %s_size %d\n", statstr, fsize);

#if REPLACE_BIN
 fprintf(output, "static char %s[%s_size] = \"%s\";\n", statstr, statstr,
	 argv[1]);
#else
 fprintf(output, "static char %s[] =\n{\n", statstr);

 while((lns = fread(str, 1, 12, input)) != 0)
 {
  j = 0;
  for(i = 0; i < lns; i++)
  {
   if (i != 0)
    out[j++] = ',';
   out[j++] = ' ';
   out[j++] = '0';
   out[j++] = 'x';
   out[j++] = hex_conv[ ((str[i] & 0xf0) >> 4)];
   out[j++] = hex_conv[ (str[i] & 0x0f)];
  }
  if (!feof(input))
  {
   out[j++] = ',';
   out[j++] = '\n';
  }
  out[j++] = '\0';
  fprintf(output, out);
 }

 fprintf(output, " };\n");
#endif

 fclose(input);
 fclose(output);

 return 0;
}
