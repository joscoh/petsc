/*$Id: cgtype.c,v 1.20 2000/05/05 22:17:34 balay Exp bsmith $*/

#include "src/sles/ksp/impls/cg/cgctx.h"       /*I "petscksp.h" I*/

#undef __FUNC__  
#define __FUNC__ "KSPCGSetType" 
/*@
    KSPCGSetType - Sets the variant of the conjugate gradient method to
    use for solving a linear system with a complex coefficient matrix.
    This option is irrelevant when solving a real system.

    Collective on KSP

    Input Parameters:
+   ksp - the iterative context
-   type - the variant of CG to use, one of
.vb
      KSP_CG_HERMITIAN - complex, Hermitian matrix (default)
      KSP_CG_SYMMETRIC - complex, symmetric matrix
.ve

    Level: intermediate
    
    Options Database Keys:
+   -ksp_cg_Hermitian - Indicates Hermitian matrix
-   -ksp_cg_symmetric - Indicates symmetric matrix

    Note:
    By default, the matrix is assumed to be complex, Hermitian.

.keywords: CG, conjugate gradient, Hermitian, symmetric, set, type
@*/
int KSPCGSetType(KSP ksp,KSPCGType type)
{
  int ierr,(*f)(KSP,KSPCGType);

  PetscFunctionBegin;
  PetscValidHeaderSpecific(ksp,KSP_COOKIE);
  ierr = PetscObjectQueryFunction((PetscObject)ksp,"KSPCGSetType_C",(void **)&f);CHKERRQ(ierr);
  if (f) {
    ierr = (*f)(ksp,type);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}





