/*$Id: ex11.c,v 1.12 2000/05/05 22:19:31 balay Exp bsmith $*/

static char help[] = "Tests various 1-dimensional DA routines.\n\n";

#include "petscda.h"
#include "petscsys.h"

#undef __FUNC__
#define __FUNC__ "main"
int main(int argc,char **argv)
{
  int    M = 5,N = 4,ierr,dof=1,s=1,wrap=0,i,n,j,k,m,cnt;
  DA     da;
  PetscViewer viewer;
  Vec    local,locala,global,coors;
  Scalar *xy,*alocal;
  PetscDraw   draw;
  char   fname[16];

  PetscInitialize(&argc,&argv,(char*)0,help);

  /* Create viewers */
  ierr = PetscViewerDrawOpen(PETSC_COMM_WORLD,0,"",PETSC_DECIDE,PETSC_DECIDE,600,200,&viewer);CHKERRA(ierr);
  ierr = PetscViewerDrawGetDraw(viewer,0,&draw);CHKERRA(ierr);
  ierr = PetscDrawSetDoubleBuffer(draw);CHKERRA(ierr);

  /* Read options */
  ierr = PetscOptionsGetInt(PETSC_NULL,"-M",&M,PETSC_NULL);CHKERRA(ierr);
  ierr = PetscOptionsGetInt(PETSC_NULL,"-N",&N,PETSC_NULL);CHKERRA(ierr);
  ierr = PetscOptionsGetInt(PETSC_NULL,"-dof",&dof,PETSC_NULL);CHKERRA(ierr); 
  ierr = PetscOptionsGetInt(PETSC_NULL,"-s",&s,PETSC_NULL);CHKERRA(ierr); 
  ierr = PetscOptionsGetInt(PETSC_NULL,"-periodic",&wrap,PETSC_NULL);CHKERRA(ierr); 

  /* Create distributed array and get vectors */
  ierr = DACreate2d(PETSC_COMM_WORLD,(DAPeriodicType)wrap,DA_STENCIL_BOX,M,N,PETSC_DECIDE,
                    PETSC_DECIDE,dof,s,PETSC_NULL,PETSC_NULL,&da);CHKERRA(ierr);
  ierr = DASetUniformCoordinates(da,0.0,1.0,0.0,1.0,0.0,0.0);CHKERRA(ierr);
  for (i=0; i<dof; i++) {
    sprintf(fname,"Field %d",i);
    ierr = DASetFieldName(da,i,fname);CHKERRA(ierr);
  }

  ierr = DAView(da,viewer);CHKERRA(ierr);
  ierr = DACreateGlobalVector(da,&global);CHKERRA(ierr);
  ierr = DACreateLocalVector(da,&local);CHKERRA(ierr);
  ierr = DACreateLocalVector(da,&locala);CHKERRA(ierr);
  ierr = DAGetCoordinates(da,&coors);CHKERRA(ierr);
  ierr = VecGetArray(coors,&xy);CHKERRA(ierr);

ierr = VecView(coors,PETSC_VIEWER_STDOUT_SELF);

  /* Set values into local vectors */
  ierr = VecGetArray(local,&alocal);CHKERRA(ierr);
  ierr = DAGetGhostCorners(da,0,0,0,&m,&n,0);CHKERRA(ierr);
  n    = n/dof;
  for (k=0; k<dof; k++) {
    cnt = 0;
    for (j=0; j<n; j++) {
      for (i=0; i<m; i++) {
        alocal[k+dof*cnt] = PetscSinScalar(2.0*PETSC_PI*(k+1)*xy[2*cnt]);
        cnt++;
      }
    }
  }
  ierr = VecRestoreArray(local,&alocal);CHKERRA(ierr);
  ierr = VecRestoreArray(coors,&xy);CHKERRA(ierr);

  ierr = DALocalToGlobal(da,local,INSERT_VALUES,global);CHKERRA(ierr);

  ierr = VecView(global,viewer);CHKERRA(ierr); 

  /* Send ghost points to local vectors */
  ierr = DAGlobalToLocalBegin(da,global,INSERT_VALUES,locala);CHKERRA(ierr);
  ierr = DAGlobalToLocalEnd(da,global,INSERT_VALUES,locala);CHKERRA(ierr);

  /* Free memory */
  ierr = PetscViewerDestroy(viewer);CHKERRA(ierr);
  ierr = VecDestroy(global);CHKERRA(ierr);
  ierr = VecDestroy(local);CHKERRA(ierr);
  ierr = VecDestroy(locala);CHKERRA(ierr);
  ierr = DADestroy(da);CHKERRA(ierr);
  PetscFinalize();
  return 0;
}
 









