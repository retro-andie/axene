/*--- include ---*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
/*       #include <linux/dirent.h>
       #include <linux/unistd.h>
*/

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
  printf("Usage: %s <directory>\n", argv[0]);
  return -1;
 }
 size = 0;
 list_dir(argv[1], &size);
 printf("%d\n", size);
 return 0;
}

/* ----------------------------------------------------------------- ** 
** list tar function                                                 ** 
** ----------------------------------------------------------------- */
int list_dir(dir, total)
char	*dir;
int	*total;
{
 DIR		*fd_dir;
 struct dirent	*dirp;
 char		filename[1000];
 struct stat	stat_buf;
 int		copy_pos;
 
 strcpy(filename, dir);
 copy_pos = strlen(filename);
 if (filename[copy_pos-1] != '/')
  filename[copy_pos++]='/';
 fd_dir = opendir(dir);
 if (!fd_dir)
  return -1;
 while ((dirp = readdir(fd_dir)) != NULL)
 {
  if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
   continue;
  strcpy(filename+copy_pos, dirp->d_name);
  if (!lstat(filename, &stat_buf))
  {
   if (stat_buf.st_mode & S_IFDIR)
   {
    list_dir(filename, total);
   }
   else
   {
    if (!S_ISLNK(stat_buf.st_mode))
    {
/*printf("%d Current file: %s\n", stat_buf.st_size, filename);*/
     *total += stat_buf.st_size;
    }
   }
  }
 }
 closedir(fd_dir);
 return *total;
}
