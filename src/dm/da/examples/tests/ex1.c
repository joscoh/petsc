/*$Id: ex1.c,v 1.41 2000/05/05 22:19:31 balay Exp bsmith $*/

static char help[] = "Tests various DA routines.\n\n";

#include "petscda.h"
#include "petscsys.h"

#undef __FUNC__
#define __FUNC__ "main"
int main(int argc,char **argv)
{
  int      rank,M = 10,N = 8,m = PETSC_DECIDE,n = PETSC_DECIDE,ierr;
  DA       da;
  PetscViewer   viewer;
  Vec      local,global;
  Scalar   value;

  PetscInitialize(&argc,&argv,(char*)0,help);
  ierr = PetscViewerDrawOpen(PETSC_COMM_WORLD,0,"",300,0,300,300,&viewer);CHKERRA(ierr);

  /* Read options */
  ierr = PetscOptionsGetInt(PETSC_NULL,"-M",&M,PETSC_NULL);CHKERRA(ierr);
  ierr = PetscOptionsGetInt(PETSC_NULL,"-N",&N,PETSC_NULL);CHKERRA(ierr);
  ierr = PetscOptionsGetInt(PETSC_NULL,"-m",&m,PETSC_NULL);CHKERRA(ierr);
  ierr = PetscOptionsGetInt(PETSC_NULL,"-n",&n,PETSC_NULL);CHKERRA(ierr);

  /* Create distributed array and get vectors */
  ierr = DACreate2d(PETSC_COMM_WORLD,DA_NONPERIODIC,DA_STENCIL_BOX,
                    M,N,m,n,1,1,PETSC_NULL,PETSC_NULL,&da);CHKERRA(ierr);
  ierr = DACreateGlobalVector(da,&global);CHKERRA(ierr);
  ierr = DACreateLocalVector(da,&local);CHKERRA(ierr);

  value = -3.0;
  ierr = VecSet(&value,global);CHKERRA(ierr);
  ierr = DAGlobalToLocalBegin(da,global,INSERT_VALUES,local);CHKERRA(ierr);
  ierr = DAGlobalToLocalEnd(da,global,INSERT_VALUES,local);CHKERRA(ierr);

  ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRA(ierr);
  value = rank+1;
  ierr = VecScale(&value,local);CHKERRA(ierr);
  ierr = DALocalToGlobal(da,local,ADD_VALUES,global);CHKERRA(ierr);

  ierr = PetscViewerPushFormat(PETSC_VIEWER_STDOUT_WORLD,PETSC_VIEWER_FORMAT_NATIVE,0);CHKERRA(ierr);
  ierr = VecView(global,PETSC_VIEWER_STDOUT_WORLD);CHKERRA(ierr);
  ierr = DAView(da,viewer);CHKERRA(ierr);

  /* Free memory */
  ierr = PetscViewerDestroy(viewer);CHKERRA(ierr);
  ierr = VecDestroy(local);CHKERRA(ierr);
  ierr = VecDestroy(global);CHKERRA(ierr);
  ierr = DADestroy(da);CHKERRA(ierr);
  PetscFinalize();
  return 0;
}
 
