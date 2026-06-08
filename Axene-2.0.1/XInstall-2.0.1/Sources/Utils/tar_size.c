/*--- include ---*/
#include <stdio.h>
#include <stdlib.h>

/*--- define ---*/
#ifdef __cplusplus
# define ___PROTO(x) x
#else
# ifdef __STDC__
#  define ___PROTO(x) x
# else
#  define ___PROTO(x) ()
# endif
#endif
#define ALLOC_SIZE 2048

/*--- prototype ---*/
int list_tar ___PROTO((char *tar));
void buffer_parse ___PROTO((char *buffer, int *size));

/*--- Globals ---*/
char	compressor_path[1000]="/bin/gunzip";
int	compressor_name_pos = 5;
char	compressor_flags[100]="-c";
char	tar_path[1000]="/bin/tar";
int	tar_name_pos = 5;
char	tar_flags[100]="tvf";
int	position=3;

/* ----------------------------------------------------------------- ** 
** main                                                              ** 
** ----------------------------------------------------------------- */
int main(argc, argv)
int	argc;
char	**argv;
{
 int	size;

 if (argc != 2)
 {
  printf("Usage: %s <filename>\n", argv[0]);
  return -1;
 }
 size = list_tar(argv[1]);
 printf("%d\n", size);
 return 0;
}

/* ----------------------------------------------------------------- ** 
** list tar function                                                 ** 
** ----------------------------------------------------------------- */
int list_tar(tar)
char	*tar;
{
 int		std_prg_tar[2];
 int		std_tar_zip[2];
 int		std_zip_cat[2];
 int		ret;
 int		pid;
 char		*buffer;
 int		size;
 long int	tsize;
 long int	talloc;
 int		retry;
 int		total_size;

 ret = pipe(std_prg_tar);
 if (ret == -1) 
 {
  perror("Could not create pipe");
  exit(1);
 }

#if defined(SYSV) || defined(USG)
#define vfork fork
#endif
 pid = vfork();
 if (pid==0)
 {
  ret = pipe(std_tar_zip);
  if (ret == -1)
  {
   perror("Could not create pipe");
   exit(1);
  }
  pid = vfork();
  if (pid==0)
  {
   unsigned char	*nbuf;
   int			readed;

   ret = pipe(std_zip_cat);
   if (ret == -1) 
   {
    perror("Could not create pipe");
    exit(1);
   }
   dup2(std_tar_zip[1],1);
   close(std_tar_zip[0]);
   close(std_tar_zip[1]);
   close(2);
   execl(compressor_path, compressor_path+compressor_name_pos,
	 compressor_flags, tar, NULL);
   exit(1);
  }

  dup2(std_prg_tar[1],1);
  dup2(std_tar_zip[0],0);
  close(std_tar_zip[0]);
  close(std_tar_zip[1]);
  close(2);

  execl(tar_path, tar_path+tar_name_pos, tar_flags, "-", NULL);
  exit(1);
 }

 close(std_prg_tar[1]);
 talloc=ALLOC_SIZE;
 buffer=(char *)malloc(talloc+1);
 tsize=0;
 retry=0;
 while ((size = read(std_prg_tar[0], buffer+tsize, talloc-tsize)))
 {
  if (size > 0)
  {
   retry = 0;
   buffer[tsize+size]=0;
   tsize+=size;
   if (tsize >= talloc)
   {
    talloc+=ALLOC_SIZE;
    buffer=(char *)realloc(buffer, talloc+1);
   }
  }
  else
  {
   retry++;
   if (retry > 10)
    break;
  }
 }
 wait(&retry);
 if (retry)
 {
  printf("error in [decompressor | tar] program or flags\n");
  tsize = 0;
 }
 if (tsize)
 {
  buffer[tsize]=0;
  total_size = 0;
  buffer_parse(buffer, &total_size);
 }  
 free(buffer);
 return total_size;
}

/* ----------------------------------------------------------------- ** 
** buffer parse                                                      ** 
** ----------------------------------------------------------------- */
void buffer_parse(buffer, size)
char	*buffer;
int	*size;
{
 int	num;
 int	i;
 int	pos;

 i=0;
 while (buffer[i])
 {
  pos = 1;
  while (pos != position)
  {
   while ((buffer[i]) && (buffer[i]!=' '))
    i++;
   pos++;
   while ((buffer[i]) && (buffer[i]==' '))
    i++;
  }
  num = 0;
  while ((buffer[i]) && (buffer[i]>='0') && (buffer[i]<='9'))
  {
   num *= 10;
   num += buffer[i]-'0';
   i++;
  }
  *size += num;
  while ((buffer[i]) && (buffer[i]!='\n'))
   i++;
  if (buffer[i])
   i++;
 }
}
