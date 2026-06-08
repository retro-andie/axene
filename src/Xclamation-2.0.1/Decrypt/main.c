#include <stdio.h>

#define __XC_MAIN__

#include "Encrypt.h"
#include "Decrypt.h"
#include <stdlib.h>

char	*gato="Ceci_est_un_test";

int main()
{
  char	*test;
  char	*test2;
  
  printf("original     : %s\n", gato);
  test = xc_encrypt(gato, 0, 6, 0);
  printf("encrypt      : %s\n", test);
  test2 = xc_decrypt(test, -1);
  printf("decrypt      : %s\n", test2);  
  free(test);
  free(test2);
  test2 = xc_decrypt(gato, -1);
  printf("decrypt(bis) : %s\n", test2);  
  free(test2);
  test = xc_encrypt(gato, 1, 2, 0);
  printf("encrypt      : %s\n", test);
  test2 = xc_decrypt(test, 2);
  printf("decrypt      : %s\n", test2);  
  free(test);
  free(test2);

  return 0;
}

