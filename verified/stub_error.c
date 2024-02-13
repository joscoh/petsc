#include <stdio.h>
#include <stdlib.h>

void stub_error(char * str) {
  fprintf(stderr, "ERROR: Called %s, which is not in the verified subset of PETSc\n", str);
  exit(1);
}