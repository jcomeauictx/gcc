#include <stdio.h>
#include <stdlib.h>

int main (void) {
  const double PI = 3.1415926535897932385;
  const double *pi = &PI;
  char *string_pi = (unsigned char *)&PI;
  printf("&PI: %p, PI: %.16f (double)*&PI: %.16f\n",
	    &PI, PI, (double)*&PI);
  printf("pi: %p, *pi: %.16f\n", pi, *pi);
  printf("string_pi: \"%s\" (%.16f)\n", string_pi, *(double *)string_pi);
  return 0;
}
