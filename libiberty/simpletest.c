#include <stdio.h>

int main (void) {
  const double PI = 3.1415926535897932385;
  const double *pi = &PI;
  printf("&PI: %p, PI: %.16f (double *)&PI: %.16f\n",
	    &PI, PI, (double *)&PI);
  printf("pi: %p, *pi: %.16f\n", pi, *pi);
  return 0;
}
