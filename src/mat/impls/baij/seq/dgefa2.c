/*$Id: dgefa2.c,v 1.7 2000/09/28 21:11:23 bsmith Exp bsmith $*/
/*
     Inverts 2 by 2 matrix using partial pivoting.

       Used by the sparse factorization routines in 
     src/mat/impls/baij/seq and src/mat/impls/bdiag/seq

       See also src/inline/ilu.h

       This is a combination of the Linpack routines
    dgefa() and dgedi() specialized for a size of 2.

*/
#include "petsc.h"

#undef __FUNC__  
#define __FUNC__ "Kernel_A_gets_inverse_A_2"
int Kernel_A_gets_inverse_A_2(MatScalar *a)
{
    int        i__2,i__3,kp1,j,k,l,ll,i,ipvt_l[2],*ipvt = ipvt_l-1,k3;
    int        k4,j3;
    MatScalar  *aa,*ax,*ay,work_l[4],*work = work_l-1,stmp;
    MatReal    tmp,max;

/*     gaussian elimination with partial pivoting */

    PetscFunctionBegin;
    /* Parameter adjustments */
    a       -= 3;

    /*for (k = 1; k <= 1; ++k) {*/
        k   = 1; 
	kp1 = k + 1;
        k3  = 2*k;
        k4  = k3 + k;
/*        find l = pivot index */

	i__2 = 2 - k;
        aa = &a[k4];
        max = PetscAbsScalar(aa[0]);
        l = 1;
        for (ll=1; ll<i__2; ll++) {
          tmp = PetscAbsScalar(aa[ll]);
          if (tmp > max) { max = tmp; l = ll+1;}
        }
        l       += k - 1;
	ipvt[k] = l;

	if (a[l + k3] == 0.) {
	  SETERRQ(k,"Zero pivot");
	}

/*           interchange if necessary */

	if (l != k) {
	  stmp      = a[l + k3];
	  a[l + k3] = a[k4];
	  a[k4]     = stmp;
        }

/*           compute multipliers */

	stmp = -1. / a[k4];
	i__2 = 2 - k;
        aa = &a[1 + k4]; 
        for (ll=0; ll<i__2; ll++) {
          aa[ll] *= stmp;
        }

/*           row elimination with column indexing */

	ax = &a[k4+1]; 
        for (j = kp1; j <= 2; ++j) {
            j3   = 2*j;
	    stmp = a[l + j3];
	    if (l != k) {
	      a[l + j3] = a[k + j3];
	      a[k + j3] = stmp;
            }

	    i__3 = 2 - k;
            ay = &a[1+k+j3];
            for (ll=0; ll<i__3; ll++) {
              ay[ll] += stmp*ax[ll];
            }
	}
    /*}*/
    ipvt[2] = 2;
    if (a[6] == 0.) {
	SETERRQ(3,"Zero pivot,final row");
    }

    /*
         Now form the inverse 
    */

   /*     compute inverse(u) */

    for (k = 1; k <= 2; ++k) {
        k3    = 2*k;
        k4    = k3 + k;
	a[k4] = 1.0 / a[k4];
	stmp  = -a[k4];
	i__2  = k - 1;
        aa    = &a[k3 + 1]; 
        for (ll=0; ll<i__2; ll++) aa[ll] *= stmp;
	kp1 = k + 1;
	if (2 < kp1) continue;
        ax = aa;
        for (j = kp1; j <= 2; ++j) {
            j3        = 2*j;
	    stmp      = a[k + j3];
	    a[k + j3] = 0.0;
            ay        = &a[j3 + 1];
            for (ll=0; ll<k; ll++) {
              ay[ll] += stmp*ax[ll];
            }
	}
    }

   /*    form inverse(u)*inverse(l) */

    /*for (kb = 1; kb <= 1; ++kb) {*/
        
	k   = 1;
        k3  = 2*k;
	kp1 = k + 1;
        aa  = a + k3;
	for (i = kp1; i <= 2; ++i) {
            work_l[i-1] = aa[i];
            /* work[i] = aa[i]; Fix for -O3 error on Origin 2000 */ 
	    aa[i]   = 0.0;
	}
	for (j = kp1; j <= 2; ++j) {
	    stmp  = work[j];
            ax    = &a[2*j + 1];
            ay    = &a[k3 + 1];
            ay[0] += stmp*ax[0];
            ay[1] += stmp*ax[1];
	}
	l = ipvt[k];
	if (l != k) {
            ax = &a[k3 + 1]; 
            ay = &a[2*l + 1];
            stmp = ax[0]; ax[0] = ay[0]; ay[0] = stmp;
            stmp = ax[1]; ax[1] = ay[1]; ay[1] = stmp;
	}
    
    PetscFunctionReturn(0);
}


