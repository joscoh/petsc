/*$Id: iscomp.c,v 1.33 2000/10/24 20:24:56 bsmith Exp bsmith $*/

#include "petscsys.h"   /*I "petscsys.h" I*/
#include "petscis.h"    /*I "petscis.h"  I*/

#undef __FUNC__  
#define __FUNC__ "ISEqual"
/*@C
   ISEqual  - Compares if two index sets have the same set of indices.

   Collective on IS

   Input Parameters:
.  is1, is2 - The index sets being compared

   Output Parameters:
.  flg - output flag, either PETSC_TRUE (if both index sets have the
         same indices), or PETSC_FALSE if the index sets differ by size 
         or by the set of indices)

   Level: intermediate

   Note: 
   This routine sorts the contents of the index sets before
   the comparision is made, so the order of the indices on a processor is immaterial.

   Each processor has to have the same indices in the two sets, for example,
$           Processor 
$             0      1
$    is1 = {0, 1} {2, 3}
$    is2 = {2, 3} {0, 1}
   will return false.

    Concepts: index sets^equal
    Concepts: IS^equal

@*/
int ISEqual(IS is1,IS is2,PetscTruth *flg)
{
  int        sz1,sz2,ierr,*ptr1,*ptr2,*a1,*a2;
  PetscTruth flag;
  MPI_Comm   comm;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(is1,IS_COOKIE);
  PetscValidHeaderSpecific(is2,IS_COOKIE);
  PetscValidIntPointer(flg);
  PetscCheckSameComm(is1,is2);
  
  ierr = ISGetSize(is1,&sz1);CHKERRQ(ierr);
  ierr = ISGetSize(is2,&sz2);CHKERRQ(ierr);
  if (sz1 != sz2) { 
    *flg = PETSC_FALSE;
  } else {
    ierr = ISGetLocalSize(is1,&sz1);CHKERRQ(ierr);
    ierr = ISGetLocalSize(is2,&sz2);CHKERRQ(ierr);

    if (sz1 != sz2) {
      flag = PETSC_FALSE;
    } else {
      ierr = ISGetIndices(is1,&ptr1);CHKERRQ(ierr);
      ierr = ISGetIndices(is2,&ptr2);CHKERRQ(ierr);
  
      ierr = PetscMalloc((sz1+1)*sizeof(int),&a1);CHKERRQ(ierr);
      ierr = PetscMalloc((sz2+1)*sizeof(int),&a2);CHKERRQ(ierr);

      ierr = PetscMemcpy(a1,ptr1,sz1*sizeof(int));CHKERRQ(ierr);
      ierr = PetscMemcpy(a2,ptr2,sz2*sizeof(int));CHKERRQ(ierr);

      ierr = PetscSortInt(sz1,a1);CHKERRQ(ierr);
      ierr = PetscSortInt(sz2,a2);CHKERRQ(ierr);
      ierr = PetscMemcmp(a1,a2,sz1*sizeof(int),&flag);CHKERRQ(ierr);

      ierr = ISRestoreIndices(is1,&ptr1);CHKERRQ(ierr);
      ierr = ISRestoreIndices(is2,&ptr2);CHKERRQ(ierr);
  
      ierr = PetscFree(a1);CHKERRQ(ierr);
      ierr = PetscFree(a2);CHKERRQ(ierr);
    }
    ierr = PetscObjectGetComm((PetscObject)is1,&comm);CHKERRQ(ierr);  
    ierr = MPI_Allreduce(&flag,flg,1,MPI_INT,MPI_MIN,comm);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}
  
