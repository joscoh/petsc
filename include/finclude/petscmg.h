!
!  $Id: petscmg.h,v 1.10 2000/09/25 18:01:01 balay Exp bsmith $;
!
!  Include file for Fortran use of the MG preconditioner in PETSc
!
#if !defined (__PETSCMG_H)
#define __PETSCMG_H

#define MGType integer

#endif

#if !defined (PETSC_AVOID_DECLARATIONS)
!
!
      integer MGMULTIPLICATIVE,MGADDITIVE,MGFULL,MGKASKADE,MGCASCADE
      parameter (MGMULTIPLICATIVE=0,MGADDITIVE=1,MGFULL=2,MGKASKADE=3)
      parameter (MGCASCADE=3)

!
!  Other defines
!
      integer MG_V_CYCLE,MG_W_CYCLE
      parameter (MG_V_CYCLE=1,MG_W_CYCLE=2)

      external MGDefaultResidual
!
!     End of Fortran include file for the  MG include file in PETSc

#endif
