/*$Id: aijmatlab.c,v 1.8 2000/10/24 20:25:32 bsmith Exp bsmith $*/

/* 
        Provides an interface for the Matlab engine sparse solver

*/
#include "src/mat/impls/aij/seq/aij.h"

#if defined(PETSC_HAVE_MATLAB) && !defined(PETSC_USE_COMPLEX)
#include "engine.h"   /* Matlab include file */
#include "mex.h"      /* Matlab include file */

#undef __FUNC__  
#define __FUNC__ "MatSolve_SeqAIJ_Matlab"
int MatSolve_SeqAIJ_Matlab(Mat A,Vec b,Vec x)
{
  Mat_SeqAIJ      *a = (Mat_SeqAIJ*)A->data;
  int             ierr;
  char            *_A,*_b,*_x;

  PetscFunctionBegin;
  /* make sure objects have names; use default if not */
  ierr = PetscObjectName((PetscObject)b);CHKERRQ(ierr);
  ierr = PetscObjectName((PetscObject)x);CHKERRQ(ierr);

  ierr = PetscObjectGetName((PetscObject)A,&_A);CHKERRQ(ierr);
  ierr = PetscObjectGetName((PetscObject)b,&_b);CHKERRQ(ierr);
  ierr = PetscObjectGetName((PetscObject)x,&_x);CHKERRQ(ierr);
  ierr = PetscMatlabEnginePut(MATLAB_ENGINE_(A->comm),(PetscObject)b);CHKERRQ(ierr);
  ierr = PetscMatlabEngineEvaluate(MATLAB_ENGINE_(A->comm),"%s = u%s\\(l%s\\(p%s*%s));",_x,_A,_A,_A,_b);CHKERRQ(ierr);
  /* ierr = PetscMatlabEnginePrintOutput(MATLAB_ENGINE_(A->comm),stdout);CHKERRQ(ierr);  */
  ierr = PetscMatlabEngineGet(MATLAB_ENGINE_(A->comm),(PetscObject)x);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatLUFactorNumeric_SeqAIJ_Matlab"
int MatLUFactorNumeric_SeqAIJ_Matlab(Mat A,Mat *F)
{
  Mat_SeqAIJ      *f = (Mat_SeqAIJ*)(*F)->data;
  Mat_SeqAIJ      *a = (Mat_SeqAIJ*)(A)->data;
  int             ierr,len;
  char            *_A,*name;

  PetscFunctionBegin;
  ierr = PetscMatlabEnginePut(MATLAB_ENGINE_(A->comm),(PetscObject)A);CHKERRQ(ierr);
  _A   = A->name;
  ierr = PetscMatlabEngineEvaluate(MATLAB_ENGINE_(A->comm),"[l_%s,u_%s,p_%s] = lu(%s',%g);",_A,_A,_A,_A,f->lu_dtcol);CHKERRQ(ierr);

  ierr = PetscStrlen(_A,&len);CHKERRQ(ierr);
  ierr = PetscMalloc((len+2)*sizeof(char),&name);CHKERRQ(ierr);
  sprintf(name,"_%s",_A);
  ierr = PetscObjectSetName((PetscObject)*F,name);CHKERRQ(ierr);
  ierr = PetscFree(name);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatLUFactorSymbolic_SeqAIJ_Matlab"
int MatLUFactorSymbolic_SeqAIJ_Matlab(Mat A,IS r,IS c,MatLUInfo *info,Mat *F)
{
  Mat_SeqAIJ      *a = (Mat_SeqAIJ*)A->data,*f;
  int             ierr;

  PetscFunctionBegin;
  if (A->N != A->M) SETERRQ(PETSC_ERR_ARG_SIZ,"matrix must be square"); 
  ierr                       = MatCreateSeqAIJ(A->comm,A->m,A->n,0,PETSC_NULL,F);CHKERRQ(ierr);
  (*F)->ops->solve           = MatSolve_SeqAIJ_Matlab;
  (*F)->ops->lufactornumeric = MatLUFactorNumeric_SeqAIJ_Matlab;
  (*F)->factor               = FACTOR_LU;
  f                          = (Mat_SeqAIJ*)(*F)->data;
  if (info) f->lu_dtcol = info->dtcol;
  else      f->lu_dtcol = 0.0;
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatILUDTFactor_SeqAIJ_Matlab"
int MatILUDTFactor_SeqAIJ_Matlab(Mat A,MatILUInfo *info,IS isrow,IS iscol,Mat *F)
{
  Mat_SeqAIJ *a = (Mat_SeqAIJ*)A->data,*b;
  int        ierr,len;
  char       *_A,*name;

  PetscFunctionBegin;
  if (info->dt == PETSC_DEFAULT)      info->dt      = .005;
  if (info->dtcol == PETSC_DEFAULT)   info->dtcol   = .01;
  if (A->N != A->M) SETERRQ(PETSC_ERR_ARG_SIZ,"matrix must be square"); 
  ierr                       = MatCreateSeqAIJ(A->comm,A->m,A->n,0,PETSC_NULL,F);CHKERRQ(ierr);
  (*F)->ops->solve           = MatSolve_SeqAIJ_Matlab;
  (*F)->factor               = FACTOR_LU;
  ierr = PetscMatlabEnginePut(MATLAB_ENGINE_(A->comm),(PetscObject)A);CHKERRQ(ierr);
  _A   = A->name;
  ierr = PetscMatlabEngineEvaluate(MATLAB_ENGINE_(A->comm),"info_%s = struct('droptol',%g,'thresh',%g);",_A,info->dt,info->dtcol);CHKERRQ(ierr);
  ierr = PetscMatlabEngineEvaluate(MATLAB_ENGINE_(A->comm),"[l_%s,u_%s,p_%s] = luinc(%s',info_%s);",_A,_A,_A,_A,_A);CHKERRQ(ierr);

  ierr = PetscStrlen(_A,&len);CHKERRQ(ierr);
  ierr = PetscMalloc((len+2)*sizeof(char),&name);CHKERRQ(ierr);
  sprintf(name,"_%s",_A);
  ierr = PetscObjectSetName((PetscObject)*F,name);CHKERRQ(ierr);
  ierr = PetscFree(name);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatUseMatlab_SeqAIJ"
int MatUseMatlab_SeqAIJ(Mat A)
{
  PetscFunctionBegin;
  A->ops->lufactorsymbolic = MatLUFactorSymbolic_SeqAIJ_Matlab;
  A->ops->iludtfactor      = MatILUDTFactor_SeqAIJ_Matlab;
  PetscLogInfo(0,"Using Matlab for SeqAIJ LU and ILUDT factorization and solves");
  PetscFunctionReturn(0);
}

#else

#undef __FUNC__  
#define __FUNC__ "MatUseMatlab_SeqAIJ"
int MatUseMatlab_SeqAIJ(Mat A)
{
  PetscFunctionBegin;
  PetscFunctionReturn(0);
}

#endif


