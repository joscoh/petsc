/*$Id: ex4.c,v 1.3 2000/10/24 20:27:27 bsmith Exp bsmith $*/
/*
       The Problem:
           Solve the convection-diffusion equation:
           
             u_t+a*(u_x+u_y)=epsilon*(u_xx+u_yy)
             u=0 at x=0, y=0
             u_x=0 at x=1
             u_y=0 at y=1
        
       This program tests the routine of computing the Jacobian by the 
       finite difference method as well as PETSc with PVODE.

*/

static char help[] = "Solve the convection-diffusion equation \n\n";

#include "petscsys.h"
#include "petscts.h"

extern int Monitor(TS,int,double,Vec,void *);
extern int Initial(Vec,void *);

typedef struct 
{
  int 		m;	/* the number of mesh points in x-direction */
  int 		n;      /* the number of mesh points in y-direction */
  double 	dx;     /* the grid space in x-direction */
  double        dy;     /* the grid space in y-direction */
  double        a;      /* the convection coefficient    */
  double        epsilon; /* the diffusion coefficient    */
} Data;

/* two temporal functions */
extern int FormJacobian(SNES,Vec,Mat*,Mat*,MatStructure*,void*);
extern int FormFunction(SNES,Vec,Vec,void*);
extern int RHSFunction(TS,double,Vec,Vec,void*);
extern int RHSJacobian(TS,double,Vec,Mat*,Mat*,MatStructure *,void*);

/* the initial function */
double f_ini(double x,double y)
{
  double f;
  f=exp(-20.0*(pow(x-0.5,2.0)+pow(y-0.5,2.0)));
  return f;
}


#define linear_no_matrix       0
#define linear_no_time         1
#define linear                 2
#define nonlinear_no_jacobian  3
#define nonlinear              4

#undef __FUNC__
#define __FUNC__ "main"
int main(int argc,char **argv)
{
  int           ierr,time_steps = 100,steps,size;
  Vec           global;
  double        dt,ftime;
  TS            ts;
  PetscViewer	viewfile;
  MatStructure  A_structure;
  Mat           A = 0;
  TSProblemType tsproblem = TS_NONLINEAR; /* Need to be TS_NONLINEAR */
  SNES  	snes;
  Vec 		x;
  Data		data;
  int 		mn;
#if defined(PETSC_HAVE_PVODE) && !defined(__cplusplus)
  PC		pc;
  PetscViewer        viewer;
  char          pcinfo[120],tsinfo[120];
#endif

  PetscInitialize(&argc,&argv,(char*)0,help);
  ierr = MPI_Comm_size(PETSC_COMM_WORLD,&size);CHKERRA(ierr);
 
  /* set Data */
  data.m = 9;
  data.n = 9;
  data.a = 1.0;
  data.epsilon = 0.1;
  data.dx = 1.0/(data.m+1.0);
  data.dy = 1.0/(data.n+1.0);
  mn = (data.m)*(data.n);

  ierr = PetscOptionsGetInt(PETSC_NULL,"-time",&time_steps,PETSC_NULL);CHKERRA(ierr);
    
  /* set initial conditions */
  ierr = VecCreate(PETSC_COMM_WORLD,PETSC_DECIDE,mn,&global);CHKERRA(ierr);
  ierr = VecSetFromOptions(global);CHKERRA(ierr);
  ierr = Initial(global,&data);CHKERRA(ierr);
  ierr = VecDuplicate(global,&x);CHKERRA(ierr);
 
  /* make timestep context */
  ierr = TSCreate(PETSC_COMM_WORLD,tsproblem,&ts);CHKERRA(ierr);
  ierr = TSSetMonitor(ts,Monitor,PETSC_NULL,PETSC_NULL);CHKERRA(ierr);

  dt = 0.1;

  /*
    The user provides the RHS and Jacobian
  */
  ierr = MatCreate(PETSC_COMM_WORLD,PETSC_DECIDE,PETSC_DECIDE,mn,mn,&A);CHKERRA(ierr);
  ierr = MatSetFromOptions(A);CHKERRA(ierr);

  /* Create SNES context  */
  ierr = SNESCreate(PETSC_COMM_WORLD,SNES_NONLINEAR_EQUATIONS,&snes);
 CHKERRA(ierr);

  /* setting the RHS function and the Jacobian's non-zero structutre */
  ierr = SNESSetFunction(snes,global,FormFunction,&data);CHKERRA(ierr);
  ierr = SNESSetJacobian(snes,A,A,FormJacobian,&data);CHKERRA(ierr);

  /* set TSPVodeRHSFunction and TSPVodeRHSJacobian, so PETSc will pick up the 
     RHS function from SNES and compute the Jacobian by FD */
  /*
  ierr = TSSetRHSFunction(ts,TSPVodeSetRHSFunction,snes);CHKERRA(ierr);
  ierr = TSPVodeSetRHSJacobian(ts,0.0,global,&A,&A,&A_structure,snes);CHKERRA(ierr);
  ierr = TSSetRHSJacobian(ts,A,A,TSPVodeSetRHSJacobian,snes);CHKERRA(ierr);
  */
  
  ierr = TSSetRHSFunction(ts,RHSFunction,&data);CHKERRA(ierr);
  ierr = RHSJacobian(ts,0.0,global,&A,&A,&A_structure,&data);CHKERRA(ierr);
  ierr = TSSetRHSJacobian(ts,A,A,RHSJacobian,&data);CHKERRA(ierr);

  /* Use PVODE */
  ierr = TSSetType(ts,TS_PVODE);CHKERRA(ierr);

  ierr = TSSetFromOptions(ts);CHKERRA(ierr);

  ierr = TSSetInitialTimeStep(ts,0.0,dt);CHKERRA(ierr);
  ierr = TSSetDuration(ts,time_steps,1);CHKERRA(ierr);
  ierr = TSSetSolution(ts,global);CHKERRA(ierr);


  /* Pick up a Petsc preconditioner */
  /* one can always set method or preconditioner during the run time */
#if defined(PETSC_HAVE_PVODE) && !defined(__cplusplus)
  ierr = TSPVodeGetPC(ts,&pc);CHKERRA(ierr);
  ierr = PCSetType(pc,PCJACOBI);CHKERRA(ierr);
  ierr = TSPVodeSetType(ts,PVODE_BDF);CHKERRA(ierr);
  /* ierr = TSPVodeSetMethodFromOptions(ts);CHKERRA(ierr); */
#endif

  ierr = TSSetUp(ts);CHKERRA(ierr);
  ierr = TSStep(ts,&steps,&ftime);CHKERRA(ierr);

  ierr = TSGetSolution(ts,&global);CHKERRA(ierr);
  ierr = PetscViewerASCIIOpen(PETSC_COMM_SELF,"out.m",&viewfile);CHKERRA(ierr); 
  ierr = PetscViewerSetFormat(viewfile,PETSC_VIEWER_FORMAT_ASCII_MATLAB,"u");CHKERRA(ierr);
  ierr = VecView(global,viewfile);CHKERRA(ierr);

#if defined(PETSC_HAVE_PVODE) && !defined(__cplusplus)
  /* extracts the PC  from ts */
  ierr = TSPVodeGetPC(ts,&pc);CHKERRA(ierr);
  ierr = PetscViewerStringOpen(PETSC_COMM_WORLD,tsinfo,120,&viewer);CHKERRA(ierr);
  ierr = TSView(ts,viewer);CHKERRA(ierr);
  ierr = PetscViewerStringOpen(PETSC_COMM_WORLD,pcinfo,120,&viewer);CHKERRA(ierr);
  ierr = PCView(pc,viewer);CHKERRA(ierr);
  ierr = PetscPrintf(PETSC_COMM_WORLD,"%d Procs,%s Preconditioner,%s\n",
                     size,tsinfo,pcinfo);CHKERRA(ierr);
  ierr = PCDestroy(pc);CHKERRA(ierr);
#endif

  /* free the memories */
  ierr = TSDestroy(ts);CHKERRA(ierr);
  ierr = VecDestroy(global);CHKERRA(ierr);
  if (A) {ierr= MatDestroy(A);CHKERRA(ierr);}
  PetscFinalize();
  return 0;
}

/* -------------------------------------------------------------------*/
#undef __FUNC__
#define __FUNC__ "Initial"
int Initial(Vec global,void *ctx)
{
  Data *data = (Data*)ctx;
  int m;
  int row,col;
  double x,y,dx,dy;
  Scalar *localptr;
  int    i,mybase,myend,ierr,locsize;

  /* make the local  copies of parameters */
  m = data->m;
  dx = data->dx;
  dy = data->dy;

  /* determine starting point of each processor */
  ierr = VecGetOwnershipRange(global,&mybase,&myend);CHKERRQ(ierr);
  ierr = VecGetLocalSize(global,&locsize);CHKERRQ(ierr);

  /* Initialize the array */
  ierr = VecGetArray(global,&localptr);CHKERRQ(ierr);

  for (i=0; i<locsize; i++) {
    row = 1+(mybase+i)-((mybase+i)/m)*m;
    col = (mybase+i)/m+1;
    x = dx*row;
    y = dy*col;
    localptr[i] = f_ini(x,y);
  }
  
  ierr = VecRestoreArray(global,&localptr);CHKERRQ(ierr);
  return 0;
}

#undef __FUNC__
#define __FUNC__ "Monitor"
int Monitor(TS ts,int step,double time,Vec global,void *ctx)
{
  VecScatter scatter;
  IS from,to;
  int i,n,*idx;
  Vec tmp_vec;
  int      ierr;
  Scalar   *tmp;

  /* Get the size of the vector */
  ierr = VecGetSize(global,&n);CHKERRQ(ierr);

  /* Set the index sets */
ierr = PetscMalloc(n*sizeof(int),&(  idx));
  for(i=0; i<n; i++) idx[i]=i;
 
  /* Create local sequential vectors */
  ierr = VecCreateSeq(PETSC_COMM_SELF,n,&tmp_vec);CHKERRQ(ierr);

  /* Create scatter context */
  ierr = ISCreateGeneral(PETSC_COMM_SELF,n,idx,&from);CHKERRQ(ierr);
  ierr = ISCreateGeneral(PETSC_COMM_SELF,n,idx,&to);CHKERRQ(ierr);
  ierr = VecScatterCreate(global,from,tmp_vec,to,&scatter);CHKERRQ(ierr);
  ierr = VecScatterBegin(global,tmp_vec,INSERT_VALUES,SCATTER_FORWARD,scatter);CHKERRA(ierr);
  ierr = VecScatterEnd(global,tmp_vec,INSERT_VALUES,SCATTER_FORWARD,scatter);CHKERRA(ierr);

  ierr = VecGetArray(tmp_vec,&tmp);CHKERRQ(ierr);
  ierr = PetscPrintf(PETSC_COMM_WORLD,"At t =%14.6e u= %14.6e at the center \n",time,PetscRealPart(tmp[n/2]));CHKERRA(ierr);
  ierr = VecRestoreArray(tmp_vec,&tmp);CHKERRQ(ierr);

  ierr = PetscFree(idx);CHKERRQ(ierr);
  return 0;
}


#undef __FUNC__
#define __FUNC__ "FormFunction"
int FormFunction(SNES snes,Vec globalin,Vec globalout,void *ptr)
{ 
  Data *data = (Data*)ptr;
  int m,n,mn;
  double dx,dy;
  double xc,xl,xr,yl,yr;
  double a,epsilon;
  Scalar *inptr,*outptr;
  int i,j,len,ierr;

  IS from,to;
  int *idx;
  VecScatter scatter;
  Vec tmp_in,tmp_out;

  m = data->m;
  n = data->n;
  mn = m*n;
  dx = data->dx;
  dy = data->dy;
  a = data->a;
  epsilon = data->epsilon;

  xc = -2.0*epsilon*(1.0/(dx*dx)+1.0/(dy*dy));
  xl = 0.5*a/dx+epsilon/(dx*dx);
  xr = -0.5*a/dx+epsilon/(dx*dx);
  yl = 0.5*a/dy+epsilon/(dy*dy);
  yr = -0.5*a/dy+epsilon/(dy*dy);
  
  /* Get the length of parallel vector */
  ierr = VecGetSize(globalin,&len);CHKERRQ(ierr);

  /* Set the index sets */
ierr = PetscMalloc(len*sizeof(int),&(  idx));
  for(i=0; i<len; i++) idx[i]=i;
 
  /* Create local sequential vectors */
  ierr = VecCreateSeq(PETSC_COMM_SELF,len,&tmp_in);CHKERRQ(ierr);
  ierr = VecDuplicate(tmp_in,&tmp_out);CHKERRQ(ierr);

  /* Create scatter context */
  ierr = ISCreateGeneral(PETSC_COMM_SELF,len,idx,&from);CHKERRQ(ierr);
  ierr = ISCreateGeneral(PETSC_COMM_SELF,len,idx,&to);CHKERRQ(ierr);
  ierr = VecScatterCreate(globalin,from,tmp_in,to,&scatter);CHKERRQ(ierr);
  ierr = VecScatterBegin(globalin,tmp_in,INSERT_VALUES,SCATTER_FORWARD,scatter);
  

 CHKERRA(ierr);
  ierr = VecScatterEnd(globalin,tmp_in,INSERT_VALUES,SCATTER_FORWARD,scatter);
 CHKERRA(ierr);

  /*Extract income array */
  ierr = VecGetArray(tmp_in,&inptr);CHKERRQ(ierr);

  /* Extract outcome array*/
  ierr = VecGetArray(tmp_out,&outptr);CHKERRQ(ierr);

  outptr[0] = xc*inptr[0]+xr*inptr[1]+yr*inptr[m];
  outptr[m-1] = 2.0*xl*inptr[m-2]+xc*inptr[m-1]+yr*inptr[m-1+m];
  for (i=1; i<m-1; i++) {
    outptr[i] = xc*inptr[i]+xl*inptr[i-1]+xr*inptr[i+1]
      +yr*inptr[i+m];
  }

  for (j=1; j<n-1; j++) {
    outptr[j*m] = xc*inptr[j*m]+xr*inptr[j*m+1]+
      yl*inptr[j*m-m]+yr*inptr[j*m+m];
    outptr[j*m+m-1] = xc*inptr[j*m+m-1]+2.0*xl*inptr[j*m+m-1-1]+
      yl*inptr[j*m+m-1-m]+yr*inptr[j*m+m-1+m];
    for(i=1; i<m-1; i++) {
      outptr[j*m+i] = xc*inptr[j*m+i]+xl*inptr[j*m+i-1]+xr*inptr[j*m+i+1]
        +yl*inptr[j*m+i-m]+yr*inptr[j*m+i+m];
    }
  }

  outptr[mn-m] = xc*inptr[mn-m]+xr*inptr[mn-m+1]+2.0*yl*inptr[mn-m-m];
  outptr[mn-1] = 2.0*xl*inptr[mn-2]+xc*inptr[mn-1]+2.0*yl*inptr[mn-1-m];
  for (i=1; i<m-1; i++) {
    outptr[mn-m+i] = xc*inptr[mn-m+i]+xl*inptr[mn-m+i-1]+xr*inptr[mn-m+i+1]
      +2*yl*inptr[mn-m+i-m];
  }

  ierr = VecRestoreArray(tmp_in,&inptr);
  ierr = VecRestoreArray(tmp_out,&outptr);

  ierr = VecScatterCreate(tmp_out,from,globalout,to,&scatter);CHKERRQ(ierr);
  ierr = VecScatterBegin(tmp_out,globalout,INSERT_VALUES,SCATTER_FORWARD,scatter
);
 CHKERRA(ierr);
  ierr = VecScatterEnd(tmp_out,globalout,INSERT_VALUES,SCATTER_FORWARD,scatter);
 CHKERRA(ierr);
 
  /* Destroy idx aand scatter */
  ierr = ISDestroy(from);CHKERRQ(ierr);
  ierr = ISDestroy(to);CHKERRQ(ierr);
  ierr = VecScatterDestroy(scatter);CHKERRQ(ierr);

  ierr = PetscFree(idx);CHKERRQ(ierr);
  return 0;
}  

#undef __FUNC__
#define __FUNC__ "FormJacobian"
int FormJacobian(SNES snes,Vec x,Mat *AA,Mat *BB,MatStructure *flag,void *ptr)
{  
  Data *data = (Data*)ptr;
  Mat A = *AA;
  Scalar v[1],one = 1.0;
  int idx[1],i,j,row,ierr;
  int m,n,mn;

  m = data->m;
  n = data->n;
  mn = (data->m)*(data->n);
  
  for(i=0; i<mn; i++) {
    idx[0] = i;
    v[0] = one;
    ierr = MatSetValues(A,1,&i,1,idx,v,INSERT_VALUES);CHKERRQ(ierr);
  }

  for(i=0; i<mn-m; i++) {
    idx[0] = i+m;
    v[0]= one;
    ierr = MatSetValues(A,1,&i,1,idx,v,INSERT_VALUES);CHKERRQ(ierr);
  }

  for(i=m; i<mn; i++) {
    idx[0] = i-m;
    v[0] = one;
    ierr = MatSetValues(A,1,&i,1,idx,v,INSERT_VALUES);CHKERRQ(ierr);
  }

  for(j=0; j<n; j++) {
    for(i=0; i<m-1; i++) {
      row = j*m+i;
      idx[0] = j*m+i+1;
      v[0] = one;
      ierr = MatSetValues(A,1,&row,1,idx,v,INSERT_VALUES);CHKERRQ(ierr);
    }
  }

  for(j=0; j<n; j++) {
    for(i=1; i<m; i++) {
      row = j*m+i;
      idx[0] = j*m+i-1;
      v[0] = one;
      ierr = MatSetValues(A,1,&row,1,idx,v,INSERT_VALUES);CHKERRQ(ierr);
    }
  }  
        
  ierr = MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);


  *flag = SAME_NONZERO_PATTERN;
  return 0;
} 

#undef __FUNC__
#define __FUNC__ "RHSJacobian"
int RHSJacobian(TS ts,double t,Vec x,Mat *AA,Mat *BB,MatStructure *flag,void *ptr)
{
  Data *data = (Data*)ptr;
  Mat A = *AA;
  Scalar v[5];
  int idx[5],i,j,row,ierr;
  int m,n,mn;
  double dx,dy,a,epsilon,xc,xl,xr,yl,yr;

  m = data->m;
  n = data->n;
  mn = m*n;
  dx = data->dx;
  dy = data->dy;
  a = data->a;
  epsilon = data->epsilon;

  xc = -2.0*epsilon*(1.0/(dx*dx)+1.0/(dy*dy));
  xl = 0.5*a/dx+epsilon/(dx*dx);
  xr = -0.5*a/dx+epsilon/(dx*dx);
  yl = 0.5*a/dy+epsilon/(dy*dy);
  yr = -0.5*a/dy+epsilon/(dy*dy);

  row=0;
  v[0] = xc; v[1]=xr; v[2]=yr;
  idx[0]=0; idx[1]=2; idx[2]=m;
  ierr = MatSetValues(A,1,&row,3,idx,v,INSERT_VALUES);CHKERRQ(ierr);

  row=m-1;
  v[0]=2.0*xl; v[1]=xc; v[2]=yr;
  idx[0]=m-2; idx[1]=m-1; idx[2]=m-1+m;
  ierr = MatSetValues(A,1,&row,3,idx,v,INSERT_VALUES);CHKERRQ(ierr);

  for (i=1; i<m-1; i++) {
    row=i;
    v[0]=xl; v[1]=xc; v[2]=xr; v[3]=yr;
    idx[0]=i-1; idx[1]=i; idx[2]=i+1; idx[3]=i+m;
    ierr = MatSetValues(A,1,&row,4,idx,v,INSERT_VALUES);CHKERRQ(ierr);
  }

  for (j=1; j<n-1; j++) {
    row=j*m;
    v[0]=xc; v[1]=xr; v[2]=yl; v[3]=yr;
    idx[0]=j*m; idx[1]=j*m; idx[2]=j*m-m; idx[3]=j*m+m; 
    ierr = MatSetValues(A,1,&row,4,idx,v,INSERT_VALUES);CHKERRQ(ierr);
  
    row=j*m+m-1;
    v[0]=xc; v[1]=2.0*xl; v[2]=yl; v[3]=yr;
    idx[0]=j*m+m-1; idx[1]=j*m+m-1-1; idx[2]=j*m+m-1-m; idx[3]=j*m+m-1+m;
    ierr = MatSetValues(A,1,&row,4,idx,v,INSERT_VALUES);CHKERRQ(ierr);

    for(i=1; i<m-1; i++) {
      row=j*m+i;
      v[0]=xc; v[1]=xl; v[2]=xr; v[3]=yl; v[4]=yr; 
      idx[0]=j*m+i; idx[1]=j*m+i-1; idx[2]=j*m+i+1; idx[3]=j*m+i-m;
      idx[4]=j*m+i+m;
      ierr = MatSetValues(A,1,&row,5,idx,v,INSERT_VALUES);CHKERRQ(ierr);
    }
  }

  row=mn-m;
  v[0] = xc; v[1]=xr; v[2]=2.0*yl;
  idx[0]=mn-m; idx[1]=mn-m+1; idx[2]=mn-m-m;
  ierr = MatSetValues(A,1,&row,3,idx,v,INSERT_VALUES);CHKERRQ(ierr);
 
  row=mn-1;
  v[0] = xc; v[1]=2.0*xl; v[2]=2.0*yl;
  idx[0]=mn-1; idx[1]=mn-2; idx[2]=mn-1-m;
  ierr = MatSetValues(A,1,&i,3,idx,v,INSERT_VALUES);CHKERRQ(ierr);

  for (i=1; i<m-1; i++) {
    row=mn-m+i;
    v[0]=xl; v[1]=xc; v[2]=xr; v[3]=2.0*yl;
    idx[0]=mn-m+i-1; idx[1]=mn-m+i; idx[2]=mn-m+i+1; idx[3]=mn-m+i-m;
    ierr = MatSetValues(A,1,&row,4,idx,v,INSERT_VALUES);CHKERRQ(ierr);
  }


  ierr = MatAssemblyBegin(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);
  ierr = MatAssemblyEnd(A,MAT_FINAL_ASSEMBLY);CHKERRQ(ierr);


  *flag = SAME_NONZERO_PATTERN;
  return 0;
}

#undef __FUNC__
#define __FUNC__ "RHSFunction"
int RHSFunction(TS ts,double t,Vec globalin,Vec globalout,void *ctx)
{
  int ierr;
  SNES snes = PETSC_NULL;

  ierr = FormFunction(snes,globalin,globalout,ctx);
CHKERRQ(ierr);

  return 0;
}

