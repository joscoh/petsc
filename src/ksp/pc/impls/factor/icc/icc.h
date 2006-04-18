
#include "private/pcimpl.h"          

#if !defined(__ICC_H)
#define __ICC_H

/* Incomplete Cholesky factorization context */

typedef struct {
  Mat             fact;
  MatOrderingType ordering;
  MatFactorInfo   info;
  PetscReal       actualfill;
  void            *implctx;
} PC_ICC;

#endif
