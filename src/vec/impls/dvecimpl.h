/* $Id: dvecimpl.h,v 1.17 2000/06/23 15:01:29 buschelm Exp bsmith $ */
/* 
   This should not be included in users code.

  Includes definition of structure for seqential double precision vectors

  These are shared by dvec1.c dvec2.c dvec3.c bvec1.c bvec2.c 
  pvectors/pvec.c pvectors/pbvec.c 
*/

#ifndef __DVECIMPL 
#define __DVECIMPL

#include "src/vec/vecimpl.h"

typedef struct { 
  VECHEADER
} Vec_Seq;

EXTERN int VecMDot_Seq(int,Vec,const Vec[],Scalar *);
EXTERN int VecMTDot_Seq(int,Vec,const Vec[],Scalar *);
EXTERN int VecMin_Seq(Vec,int*,double *);
EXTERN int VecSet_Seq(const Scalar*,Vec);
EXTERN int VecSetRandom_Seq(PetscRandom,Vec);
EXTERN int VecMAXPY_Seq(int,const Scalar *,Vec,Vec *);
EXTERN int VecAYPX_Seq(const Scalar *,Vec,Vec);
EXTERN int VecWAXPY_Seq(const Scalar*,Vec,Vec,Vec);
EXTERN int VecPointwiseMult_Seq(Vec,Vec,Vec);
EXTERN int VecPointwiseDivide_Seq(Vec,Vec,Vec);
EXTERN int VecGetArray_Seq(Vec,Scalar *[]);
EXTERN int VecRestoreArray_Seq(Vec,Scalar *[]);
EXTERN int VecPlaceArray_Seq(Vec,const Scalar *);
EXTERN int VecResetArray_Seq(Vec);
EXTERN int VecReplaceArray_Seq(Vec,const Scalar *);
EXTERN int VecGetSize_Seq(Vec,int *);
EXTERN int VecDot_Seq(Vec,Vec,Scalar *);
EXTERN int VecTDot_Seq(Vec,Vec,Scalar *);
EXTERN int VecScale_Seq(const Scalar *,Vec);
EXTERN int VecCopy_Seq(Vec,Vec);
EXTERN int VecSwap_Seq(Vec,Vec);
EXTERN int VecAXPY_Seq(const Scalar *,Vec,Vec);
EXTERN int VecAXPBY_Seq(const Scalar *,const Scalar *,Vec,Vec);
EXTERN int VecMax_Seq(Vec,int*,double *);
EXTERN int VecDuplicate_Seq(Vec,Vec *);
EXTERN int VecGetMap_Seq(Vec,Map *);
EXTERN int VecConjugate_Seq(Vec);

#endif
