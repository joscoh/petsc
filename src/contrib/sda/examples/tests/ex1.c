/*$Id: ex1.c,v 1.11 2000/05/05 22:19:43 balay Exp bsmith $*/

static char help[] = "Tests SDALocalToLocal().\n\n";

#include "petscda.h"
#include "src/contrib/sda/src/sda.h"
#include "petscsys.h"

/*
         For testing purposes this example also creates a 
   DA context. Actually codes using SDA routines will probably 
   not also work with DA contexts.
*/

#undef __FUNC__
#define __FUNC__ "main"
int main(int argc,char **argv)
{
  int            rank,M=8,ierr,dof=1,stencil_width=1,i,start,end,P=5;
  int            N = 6,m=PETSC_DECIDE,n=PETSC_DECIDE,p=PETSC_DECIDE;
  PetscTruth     flg2,flg3,flg;
  DAPeriodicType periodic = DA_NONPERIODIC;
  DAStencilType  stencil_type = DA_STENCIL_STAR;
  DA             da;
  SDA            sda;
  Vec            local,global,local_copy;
  Scalar         value,mone = -1.0,*in,*out;
  double         norm,work;
  PetscViewer         viewer;
  char           filename[64];
  FILE           *file;


  PetscInitialize(&argc,&argv,(char*)0,help);

  ierr = PetscOptionsGetInt(PETSC_NULL,"-M",&M,PETSC_NULL);CHKERRA(ierr);
  ierr = PetscOptionsGetInt(PETSC_NULL,"-N",&N,PETSC_NULL);CHKERRA(ierr);
  ierr = PetscOptionsGetInt(PETSC_NULL,"-P",&P,PETSC_NULL);CHKERRA(ierr);
  ierr = PetscOptionsGetInt(PETSC_NULL,"-dof",&dof,PETSC_NULL);CHKERRA(ierr); 
  ierr = PetscOptionsGetInt(PETSC_NULL,"-stencil_width",&stencil_width,PETSC_NULL);CHKERRA(ierr); 
  ierr = PetscOptionsGetInt(PETSC_NULL,"-periodic",(int*)&periodic,PETSC_NULL);CHKERRA(ierr); 
  ierr = PetscOptionsGetInt(PETSC_NULL,"-stencil_type",(int*)&stencil_type,PETSC_NULL);CHKERRA(ierr); 

  ierr = PetscOptionsHasName(PETSC_NULL,"-2d",&flg2);CHKERRA(ierr);
  ierr = PetscOptionsHasName(PETSC_NULL,"-3d",&flg3);CHKERRA(ierr);
  if (flg2) {
    ierr = DACreate2d(PETSC_COMM_WORLD,periodic,stencil_type,M,N,m,n,dof,stencil_width,0,0,&da);
          CHKERRA(ierr);
    ierr = SDACreate2d(PETSC_COMM_WORLD,periodic,stencil_type,M,N,m,n,dof,stencil_width,0,0,&sda);
          CHKERRA(ierr);
  } else if (flg3) {
    ierr = DACreate3d(PETSC_COMM_WORLD,periodic,stencil_type,M,N,P,m,n,p,dof,stencil_width,
                      0,0,0,&da);CHKERRA(ierr);
    ierr = SDACreate3d(PETSC_COMM_WORLD,periodic,stencil_type,M,N,P,m,n,p,dof,stencil_width,
                      0,0,0,&sda);CHKERRA(ierr);
  }
  else {
    ierr = DACreate1d(PETSC_COMM_WORLD,periodic,M,dof,stencil_width,PETSC_NULL,&da);CHKERRA(ierr);
    ierr = SDACreate1d(PETSC_COMM_WORLD,periodic,M,dof,stencil_width,PETSC_NULL,&sda);CHKERRA(ierr);
  }

  ierr = DACreateGlobalVector(da,&global);CHKERRA(ierr);
  ierr = DACreateLocalVector(da,&local);CHKERRA(ierr);
  ierr = VecDuplicate(local,&local_copy);CHKERRA(ierr);

  
  /* zero out vectors so that ghostpoints are zero */
  value = 0;
  ierr = VecSet(&value,local);CHKERRA(ierr);
  ierr = VecSet(&value,local_copy);CHKERRA(ierr);

  ierr = VecGetOwnershipRange(global,&start,&end);CHKERRA(ierr);
  for (i=start; i<end; i++) {
    value = i + 1;
    ierr = VecSetValues(global,1,&i,&value,INSERT_VALUES);CHKERRA(ierr);
  }
  ierr = VecAssemblyBegin(global);CHKERRA(ierr);
  ierr = VecAssemblyEnd(global);CHKERRA(ierr);

  ierr = DAGlobalToLocalBegin(da,global,INSERT_VALUES,local);CHKERRA(ierr);
  ierr = DAGlobalToLocalEnd(da,global,INSERT_VALUES,local);CHKERRA(ierr);


  ierr = PetscOptionsHasName(PETSC_NULL,"-same_array",&flg);CHKERRA(ierr);
  if (flg) {
    /* test the case where the input and output array is the same */
    ierr = VecCopy(local,local_copy);CHKERRA(ierr);
    ierr = VecGetArray(local_copy,&in);CHKERRQ(ierr);
    ierr = VecRestoreArray(local_copy,&in);CHKERRQ(ierr);
    ierr = SDALocalToLocalBegin(sda,in,INSERT_VALUES,in);CHKERRA(ierr);
    ierr = SDALocalToLocalEnd(sda,in,INSERT_VALUES,in);CHKERRA(ierr);
  } else {
    ierr = VecGetArray(local,&out);CHKERRQ(ierr);
    ierr = VecRestoreArray(local,&out);CHKERRQ(ierr);
    ierr = VecGetArray(local_copy,&in);CHKERRQ(ierr);
    ierr = VecRestoreArray(local_copy,&in);CHKERRQ(ierr);
    ierr = SDALocalToLocalBegin(sda,out,INSERT_VALUES,in);CHKERRA(ierr);
    ierr = SDALocalToLocalEnd(sda,out,INSERT_VALUES,in);CHKERRA(ierr);
  }

  ierr = PetscOptionsHasName(PETSC_NULL,"-save",&flg);CHKERRA(ierr);
  if (flg) {
    ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRA(ierr);
    sprintf(filename,"local.%d",rank);
    ierr = PetscViewerASCIIOpen(PETSC_COMM_SELF,filename,&viewer);CHKERRA(ierr);
    ierr = PetscViewerASCIIGetPointer(viewer,&file);CHKERRA(ierr);
    ierr = VecView(local,viewer);CHKERRA(ierr);
    fprintf(file,"Vector with correct ghost points\n");
    ierr = VecView(local_copy,viewer);CHKERRA(ierr);
    ierr = PetscViewerDestroy(viewer);CHKERRA(ierr);
  }

  ierr = VecAXPY(&mone,local,local_copy);CHKERRA(ierr);
  ierr = VecNorm(local_copy,NORM_MAX,&work);CHKERRA(ierr);
  ierr = MPI_Allreduce(&work,&norm,1,MPI_DOUBLE,MPI_MAX,PETSC_COMM_WORLD);CHKERRA(ierr);
  ierr = PetscPrintf(PETSC_COMM_WORLD,"Norm of difference %g should be zero\n",norm);CHKERRA(ierr);
   
  ierr = DADestroy(da);CHKERRA(ierr);
  ierr = SDADestroy(sda);CHKERRA(ierr);
  ierr = VecDestroy(local_copy);CHKERRA(ierr);
  ierr = VecDestroy(local);CHKERRA(ierr);
  ierr = VecDestroy(global);CHKERRA(ierr);
  PetscFinalize();
  return 0;
}
