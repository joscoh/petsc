/*$Id: shell.c,v 1.83 2000/10/24 20:25:38 bsmith Exp bsmith $*/

/*
   This provides a simple shell for Fortran (and C programmers) to 
  create a very simple matrix class for use with KSP without coding 
  much of anything.
*/

#include "src/mat/matimpl.h"        /*I "petscmat.h" I*/
#include "src/vec/vecimpl.h"  

typedef struct {
  int  (*destroy)(Mat);
  void *ctx;
} Mat_Shell;      

#undef __FUNC__  
#define __FUNC__ "MatShellGetContext"
/*@
    MatShellGetContext - Returns the user-provided context associated with a shell matrix.

    Not Collective

    Input Parameter:
.   mat - the matrix, should have been created with MatCreateShell()

    Output Parameter:
.   ctx - the user provided context

    Level: advanced

    Notes:
    This routine is intended for use within various shell matrix routines,
    as set with MatShellSetOperation().
    
.keywords: matrix, shell, get, context

.seealso: MatCreateShell(), MatShellSetOperation()
@*/
int MatShellGetContext(Mat mat,void **ctx)
{
  int        ierr;
  PetscTruth flg;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(mat,MAT_COOKIE); 
  ierr = PetscTypeCompare((PetscObject)mat,MATSHELL,&flg);CHKERRQ(ierr);
  if (!flg) *ctx = 0; 
  else      *ctx = ((Mat_Shell*)(mat->data))->ctx; 
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatDestroy_Shell"
int MatDestroy_Shell(Mat mat)
{
  int       ierr;
  Mat_Shell *shell;

  PetscFunctionBegin;
  shell = (Mat_Shell*)mat->data;
  if (shell->destroy) {ierr = (*shell->destroy)(mat);CHKERRQ(ierr);}
  ierr = PetscFree(shell);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatGetOwnershipRange_Shell"
int MatGetOwnershipRange_Shell(Mat mat,int *rstart,int *rend)
{
  int ierr,tmp;

  PetscFunctionBegin;
  ierr = MPI_Scan(&mat->m,&tmp,1,MPI_INT,MPI_SUM,mat->comm);CHKERRQ(ierr);
  if (rstart) *rstart = tmp - mat->m;
  if (rend)   *rend   = tmp;
  PetscFunctionReturn(0);
}
extern int MatConvert_Shell(Mat,MatType,Mat*);

static struct _MatOps MatOps_Values = {0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       MatGetOwnershipRange_Shell,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       MatDestroy_Shell,
       0,
       MatGetMaps_Petsc,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       MatConvert_Shell};

EXTERN_C_BEGIN
#undef __FUNC__  
#define __FUNC__ "MatCreate_Shell"
int MatCreate_Shell(Mat A)
{
  Mat_Shell *b;
  int       ierr;

  PetscFunctionBegin;
  ierr            = PetscMemcpy(A->ops,&MatOps_Values,sizeof(struct _MatOps));CHKERRQ(ierr);

  ierr = PetscNew(Mat_Shell,&b);CHKERRQ(ierr);
  PetscLogObjectMemory(A,sizeof(struct _p_Mat)+sizeof(Mat_Shell));
  ierr    = PetscMemzero(b,sizeof(Mat_Shell));CHKERRQ(ierr);
  A->data = (void*)b;

  if (A->m == PETSC_DECIDE || A->n == PETSC_DECIDE) {
    SETERRQ(1,"Must give local row and column count for matrix");
  }

  ierr = PetscSplitOwnership(A->comm,&A->m,&A->M);CHKERRQ(ierr);
  ierr = PetscSplitOwnership(A->comm,&A->n,&A->N);CHKERRQ(ierr);

  ierr = MapCreateMPI(A->comm,A->m,A->M,&A->rmap);CHKERRQ(ierr);
  ierr = MapCreateMPI(A->comm,A->n,A->N,&A->cmap);CHKERRQ(ierr);

  b->ctx = 0;
  A->assembled    = PETSC_TRUE;
  A->preallocated = PETSC_TRUE;
  PetscFunctionReturn(0);
}
EXTERN_C_END

#undef __FUNC__  
#define __FUNC__ "MatCreateShell"
/*@C
   MatCreateShell - Creates a new matrix class for use with a user-defined
   private data storage format. 

  Collective on MPI_Comm

   Input Parameters:
+  comm - MPI communicator
.  m - number of local rows (must be given)
.  n - number of local columns (must be given)
.  M - number of global rows (may be PETSC_DETERMINE)
.  N - number of global columns (may be PETSC_DETERMINE)
-  ctx - pointer to data needed by the shell matrix routines

   Output Parameter:
.  A - the matrix

   Level: advanced

  Usage:
$    extern int mult(Mat,Vec,Vec);
$    MatCreateShell(comm,m,n,M,N,ctx,&mat);
$    MatShellSetOperation(mat,MATOP_MULT,(void *)mult);
$    [ Use matrix for operations that have been set ]
$    MatDestroy(mat);

   Notes:
   The shell matrix type is intended to provide a simple class to use
   with KSP (such as, for use with matrix-free methods). You should not
   use the shell type if you plan to define a complete matrix class.

   PETSc requires that matrices and vectors being used for certain
   operations are partitioned accordingly.  For example, when
   creating a shell matrix, A, that supports parallel matrix-vector
   products using MatMult(A,x,y) the user should set the number
   of local matrix rows to be the number of local elements of the
   corresponding result vector, y. Note that this is information is
   required for use of the matrix interface routines, even though
   the shell matrix may not actually be physically partitioned.
   For example,

$
$     Vec x, y
$     extern int mult(Mat,Vec,Vec);
$     Mat A
$
$     VecCreateMPI(comm,PETSC_DECIDE,M,&y);
$     VecCreateMPI(comm,PETSC_DECIDE,N,&x);
$     VecGetLocalSize(y,&m);
$     VecGetLocalSize(x,&n);
$     MatCreateShell(comm,m,n,M,N,ctx,&A);
$     MatShellSetOperation(mat,MATOP_MULT,(void *)mult);
$     MatMult(A,x,y);
$     MatDestroy(A);
$     VecDestroy(y); VecDestroy(x);
$

.keywords: matrix, shell, create

.seealso: MatShellSetOperation(), MatHasOperation(), MatShellGetContext()
@*/
int MatCreateShell(MPI_Comm comm,int m,int n,int M,int N,void *ctx,Mat *A)
{
  int       ierr;

  PetscFunctionBegin;
  ierr = MatCreate(comm,m,n,M,N,A);CHKERRQ(ierr);
  ierr = MatSetType(*A,MATSHELL);CHKERRQ(ierr);
  ierr = MatShellSetContext(*A,ctx);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatShellSetContext"
/*@C
    MatShellSetContext - sets the context for a shell matrix

   Collective on Mat

    Input Parameters:
+   mat - the shell matrix
-   ctx - the context

   Level: advanced


.seealso: MatCreateShell(), MatShellGetContext(), MatShellGetOperation()
@*/
int MatShellSetContext(Mat mat,void *ctx)
{
  Mat_Shell  *shell = (Mat_Shell*)mat->data;
  int        ierr;
  PetscTruth flg;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(mat,MAT_COOKIE);
  ierr = PetscTypeCompare((PetscObject)mat,MATSHELL,&flg);CHKERRQ(ierr);
  if (flg) {
    shell->ctx = ctx;
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatShellSetOperation"
/*@C
    MatShellSetOperation - Allows user to set a matrix operation for
                           a shell matrix.

   Collective on Mat

    Input Parameters:
+   mat - the shell matrix
.   op - the name of the operation
-   f - the function that provides the operation.

   Level: advanced

    Usage:
$      extern int usermult(Mat,Vec,Vec);
$      ierr = MatCreateShell(comm,m,n,M,N,ctx,&A);
$      ierr = MatShellSetOperation(A,MATOP_MULT,(void*) usermult);

    Notes:
    See the file include/petscmat.h for a complete list of matrix
    operations, which all have the form MATOP_<OPERATION>, where
    <OPERATION> is the name (in all capital letters) of the
    user interface routine (e.g., MatMult() -> MATOP_MULT).

    All user-provided functions should have the same calling
    sequence as the usual matrix interface routines, since they
    are intended to be accessed via the usual matrix interface
    routines, e.g., 
$       MatMult(Mat,Vec,Vec) -> usermult(Mat,Vec,Vec)

    Within each user-defined routine, the user should call
    MatShellGetContext() to obtain the user-defined context that was
    set by MatCreateShell().

.keywords: matrix, shell, set, operation

.seealso: MatCreateShell(), MatShellGetContext(), MatShellGetOperation()
@*/
int MatShellSetOperation(Mat mat,MatOperation op,void *f)
{
  int        ierr;
  PetscTruth flg;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(mat,MAT_COOKIE);
  if (op == MATOP_DESTROY) {
    ierr = PetscTypeCompare((PetscObject)mat,MATSHELL,&flg);CHKERRQ(ierr);
    if (flg) {
       Mat_Shell *shell = (Mat_Shell*)mat->data;
       shell->destroy                 = (int (*)(Mat)) f;
    } else mat->ops->destroy            = (int (*)(Mat)) f;
  } 
  else if (op == MATOP_VIEW) mat->ops->view  = (int (*)(Mat,PetscViewer)) f;
  else                       (((void**)mat->ops)[op]) = f;

  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "MatShellGetOperation"
/*@C
    MatShellGetOperation - Gets a matrix function for a shell matrix.

    Not Collective

    Input Parameters:
+   mat - the shell matrix
-   op - the name of the operation

    Output Parameter:
.   f - the function that provides the operation.

    Level: advanced

    Notes:
    See the file include/petscmat.h for a complete list of matrix
    operations, which all have the form MATOP_<OPERATION>, where
    <OPERATION> is the name (in all capital letters) of the
    user interface routine (e.g., MatMult() -> MATOP_MULT).

    All user-provided functions have the same calling
    sequence as the usual matrix interface routines, since they
    are intended to be accessed via the usual matrix interface
    routines, e.g., 
$       MatMult(Mat,Vec,Vec) -> usermult(Mat,Vec,Vec)

    Within each user-defined routine, the user should call
    MatShellGetContext() to obtain the user-defined context that was
    set by MatCreateShell().

.keywords: matrix, shell, set, operation

.seealso: MatCreateShell(), MatShellGetContext(), MatShellSetOperation()
@*/
int MatShellGetOperation(Mat mat,MatOperation op,void **f)
{
  int        ierr;
  PetscTruth flg;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(mat,MAT_COOKIE);
  if (op == MATOP_DESTROY) {
    ierr = PetscTypeCompare((PetscObject)mat,MATSHELL,&flg);CHKERRQ(ierr);
    if (flg) {
      Mat_Shell *shell = (Mat_Shell*)mat->data;
      *f = (void*)shell->destroy;
    } else {
      *f = (void*)mat->ops->destroy;
    }
  } else if (op == MATOP_VIEW) {
    *f = (void*)mat->ops->view;
  } else {
    *f = (((void**)mat->ops)[op]);
  }

  PetscFunctionReturn(0);
}

