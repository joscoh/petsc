/*$Id: ex81.c,v 1.3 2000/09/28 21:11:49 bsmith Exp bsmith $*/

static char help[] = "Reads in a PETSc binary matrix and saves in Harwell-Boeing format\n\
  -fout <output_file> : file to load.\n\
  -fin <input_file> : For a 5X5 example of the 5-pt. stencil,\n\
                       use the file petsc/src/mat/examples/matbinary.ex\n\n";

/*
  Include the private file (not included by most applications) so we have direct
  access to the matrix data structure.
*/
#include "src/mat/impls/aij/seq/aij.h"

#undef __FUNC__
#define __FUNC__ "main"
int main(int argc,char **args)
{
  int         ierr,n,m,i,*ai,*aj,size,nz;
  PetscTruth  flg;
  Mat         A;
  Vec         x;
  char        bfile[512],hbfile[512]; 
  PetscViewer      fd;
  Mat_SeqAIJ  *a;
  Scalar      *aa,*xx;
  FILE        *file;
  char        head[81];

  PetscInitialize(&argc,&args,(char *)0,help);

#if defined(PETSC_USE_COMPLEX)
  SETERRA(1,"This example does not work with complex numbers");
#endif
  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRA(ierr);
  if (size > 1) SETERRQ(1,"Only runs on one processor");

  ierr = PetscOptionsGetString(PETSC_NULL,"-fin",bfile,127,PETSC_NULL);CHKERRA(ierr);
  ierr = PetscOptionsGetString(PETSC_NULL,"-fout",hbfile,127,PETSC_NULL);CHKERRA(ierr);

  /* Read matrix and RHS */
  ierr = PetscViewerBinaryOpen(PETSC_COMM_WORLD,bfile,PETSC_BINARY_RDONLY,&fd);CHKERRA(ierr);
  ierr = MatLoad(fd,MATSEQAIJ,&A);CHKERRA(ierr);
  ierr = VecLoad(fd,&x);CHKERRA(ierr);
  ierr = PetscViewerDestroy(fd);CHKERRA(ierr);

  /* Format is in column storage so we print transpose matrix */
  ierr = MatTranspose(A,0);CHKERRQ(ierr);

  m = A->m;
  n = A->n;
  if (n != m) SETERRQ(1,"Only for square matrices");

  /* charrage returns \n may not belong below
    depends on what 80 charactor fixed format means to Fortran */

  file = fopen(hbfile,"w"); if (!file) SETERRQ(1,"Cannot open HB file");
  sprintf(head,"%-72s%-8s\n","Title","Key");
  fprintf(file,head);
  a  = (Mat_SeqAIJ*)A->data;
  aa = a->a;
  ai = a->i;
  aj = a->j;
  nz = a->nz;


  sprintf(head,"%14d%14d%14d%14d%14d%10s\n",3*m+1,m+1,nz,nz," ");
  fprintf(file,head);
  sprintf(head,"RUA%14d%14d%14d%14d%13s\n",m,m,nz," ");
  fprintf(file,head);

  fprintf(file,"Formats I don't know\n");

  for (i=0; i<m+1; i++) {
    fprintf(file,"%10d%70s\n",ai[i]," ");
  }
  for (i=0; i<nz; i++) {
    fprintf(file,"%10d%70s\n",aj[i]," ");
  }

  for (i=0; i<nz; i++) {
    fprintf(file,"%16.14e,%64s\n",aa[i]," ");
  }

  /* print the vector to the file */
  ierr = VecGetArray(x,&xx);CHKERRQ(ierr);
  for (i=0; i<m; i++) {
    fprintf(file,"%16.14e%64s\n",xx[i]," ");
  }
  ierr = VecRestoreArray(x,&xx);CHKERRQ(ierr);

  fclose(file);
  ierr = MatDestroy(A);CHKERRA(ierr);
  ierr = VecDestroy(x);CHKERRA(ierr);

  PetscFinalize();
  return 0;
}
