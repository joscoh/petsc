/*$Id: ex4.c,v 1.11 2000/05/05 22:19:15 balay Exp bsmith $*/

static char help[] = "Tests AOData loading\n\n";

#include "petscao.h"

#undef __FUNC__
#define __FUNC__ "main"
int main(int argc,char **argv)
{
  AOData      aodata;
  PetscViewer      binary;
  int         ierr,indices[4],*intv,i,rank;

  PetscInitialize(&argc,&argv,(char*)0,help);
  ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRA(ierr);

  /*
        Load the database from the file
  */
  ierr = PetscViewerBinaryOpen(PETSC_COMM_WORLD,"dataoutput",PETSC_BINARY_RDONLY,&binary);CHKERRA(ierr);
  ierr = AODataLoadBasic(binary,&aodata);CHKERRA(ierr);
  ierr = PetscViewerDestroy(binary);CHKERRA(ierr);

  /*
        Access part of the data 
  */
  indices[0] = 0; indices[1] = 2; indices[2] = 1; indices[3] = 5;
  ierr = AODataSegmentGet(aodata,"key1","seg1",4,indices,(void **)&intv);CHKERRA(ierr);
  for (i=0; i<4; i++) {
    ierr = PetscSynchronizedPrintf(PETSC_COMM_WORLD,"[%d] %d %d\n",rank,i,intv[i]);CHKERRA(ierr);
  }
  ierr = PetscSynchronizedFlush(PETSC_COMM_WORLD);CHKERRA(ierr);
  ierr = AODataSegmentRestore(aodata,"key1","seg1",4,indices,(void **)&intv);CHKERRA(ierr);
 
  ierr = AODataDestroy(aodata);CHKERRA(ierr);

  PetscFinalize();
  return 0;
}
 


