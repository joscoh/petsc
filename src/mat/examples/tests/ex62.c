/*$Id: ex62.c,v 1.16 2000/10/24 20:26:04 bsmith Exp bsmith $*/

static char help[] = "Tests the use of MatSolveTranspose().\n\n";

#include "petscmat.h"

#undef __FUNC__
#define __FUNC__ "main"
int main(int argc,char **args)
{
  Mat        C,A;
  int        i,j,m,ierr,size;
  IS         row,col;
  Vec        x,u,b;
  double     norm;
  PetscViewer     fd;
  char       type[256];
  char       file[128];
  Scalar     one = 1.0,mone = -1.0;
  PetscTruth flg;

  PetscInitialize(&argc,&args,(char *)0,help);
  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRA(ierr);
  if (size > 1) SETERRA(1,"Can only run on one processor");

  ierr = PetscOptionsGetString(PETSC_NULL,"-f",file,127,&flg);CHKERRA(ierr);
  if (!flg) SETERRA(1,"Must indicate binary file with the -f option");
  /* 
     Open binary file.  Note that we use PETSC_BINARY_RDONLY to indicate
     reading from this file.
  */
  ierr = PetscViewerBinaryOpen(PETSC_COMM_WORLD,file,PETSC_BINARY_RDONLY,&fd);CHKERRA(ierr);

  /* 
     Determine matrix format to be used (specified at runtime).
     See the manpage for MatLoad() for available formats.
  */
  ierr = PetscStrcpy(type,MATSEQAIJ);CHKERRQ(ierr);
  ierr = PetscOptionsGetString(PETSC_NULL,"-mat_type",type,256,PETSC_NULL);CHKERRQ(ierr);

  /*
     Load the matrix and vector; then destroy the viewer.
  */
  ierr = MatLoad(fd,type,&C);CHKERRA(ierr);
  ierr = VecLoad(fd,&u);CHKERRA(ierr);
  ierr = PetscViewerDestroy(fd);CHKERRA(ierr);

  ierr = VecDuplicate(u,&x);CHKERRA(ierr);
  ierr = VecDuplicate(u,&b);CHKERRA(ierr);

  ierr = MatMultTranspose(C,u,b);CHKERRA(ierr);

  /* Set default ordering to be Quotient Minimum Degree; also read
     orderings from the options database */
  ierr = MatGetOrdering(C,MATORDERING_QMD,&row,&col);CHKERRA(ierr);

  ierr = MatLUFactorSymbolic(C,row,col,PETSC_NULL,&A);CHKERRA(ierr);
  ierr = MatLUFactorNumeric(C,&A);CHKERRA(ierr);
  ierr = MatSolveTranspose(A,b,x);CHKERRA(ierr);

  ierr = VecAXPY(&mone,u,x);CHKERRA(ierr);
  ierr = VecNorm(x,NORM_2,&norm);CHKERRA(ierr);
  ierr = PetscPrintf(PETSC_COMM_SELF,"Norm of error %g\n",norm);CHKERRA(ierr);

  ierr = ISDestroy(row);CHKERRA(ierr);
  ierr = ISDestroy(col);CHKERRA(ierr);
  ierr = VecDestroy(u);CHKERRA(ierr);
  ierr = VecDestroy(x);CHKERRA(ierr);
  ierr = VecDestroy(b);CHKERRA(ierr);
  ierr = MatDestroy(C);CHKERRA(ierr);
  ierr = MatDestroy(A);CHKERRA(ierr);
  PetscFinalize();
  return 0;
}
