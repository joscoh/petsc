/*$Id: dlregispetsc.c,v 1.12 2000/07/10 03:38:34 bsmith Exp bsmith $*/

#include "petsc.h"

  
EXTERN_C_BEGIN
#undef __FUNC__  
#define __FUNC__ "PetscDLLibraryRegister" 
/*
  PetscDLLibraryRegister - This function is called when the dynamic library it is in is opened.

  This one registers all the draw and PetscViewer objects.

  Input Parameter:
  path - library path
 */
int PetscDLLibraryRegister(char *path)
{
  int ierr;

  ierr = PetscInitializeNoArguments(); if (ierr) return 1;

  /* this follows the Initialize() to make sure PETSc was setup first */
  PetscFunctionBegin;
  /*
      If we got here then PETSc was properly loaded
  */
  ierr = PetscDrawRegisterAll(path);CHKERRQ(ierr);
  ierr = PetscViewerRegisterAll(path);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}
EXTERN_C_END

/* --------------------------------------------------------------------------*/
static char *contents = "PETSc Graphics and PetscViewer libraries. \n\
     ASCII, Binary, Sockets, X-windows, ...\n";

#include "src/sys/src/utils/dlregis.h"

#if !defined(PETSC_USE_DYNAMIC_LIBRARIES)
#undef __FUNC__  
#define __FUNC__ "PetscDLLibraryRegister_Petsc" 
int PetscDLLibraryRegister_Petsc(char *path)
{
  int ierr;

  PetscFunctionBegin;
  ierr = PetscDrawRegisterAll(path);CHKERRQ(ierr);
  ierr = PetscViewerRegisterAll(path);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}
#endif







