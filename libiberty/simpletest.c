#include <stdio.h>
#include <stdlib.h>

char * doublestring(double number);

int main (void) {
  const double PI = 3.1415926535897932385;
  const double *pi = &PI;
  char *string_pi = doublestring(PI);
  printf("&PI: %p, PI: %.16f (double)*&PI: %.16f\n",
	    &PI, PI, (double)*&PI);
  printf("pi: %p, *pi: %.16f\n", pi, *pi);
  printf("string_pi: \"%s\" (%.16f)\n", string_pi, (double) *string_pi);
  return 0;
}

char * doublestring(double number) {
  /* I'm not even sure such a thing is necessary, but so far have been stymied
   * in inserting and retrieving doubles to/from compound literals. */
  char *string = malloc(65);  /* at most 4 per digit (\000) plus final null */
  /* we won't worry about `free`ing the memory, that will happen automatically
   * at program termination */
  int offset = 0;
  int i;
  unsigned char u, *ptr = (unsigned char *)&number;
  /* let's do this for little-endian, work on endian-agnostic version later */
  for (i = 0; i < 8; i++) {
    u = ptr[i];
    if (u < ' ' || u > '_') offset += sprintf(string + offset, "\\%03o", u);
    else string[offset++] = u;
    string[offset] = '\0';
  }
  return string;
}
