#include <sys/statfs.h>

int main(argc, argv)
     int argc;
     char **argv;
{
  struct statfs infofs;
  
  if (argc == 1) return 1;
  
  if (!statfs(argv[1], &infofs))
    {
      printf("Statfs:\n\tf_bsize: %d\n\tf_bavail: %d\n",
	     infofs.f_bsize, infofs.f_bavail);
    }
  else
    {
      printf("Statfs error for dir: %s\n", argv[1]);
      return 1;
    }  
  return 0;
}
