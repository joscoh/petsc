/*$Id: f90_t3e.c,v 1.11 2000/09/28 21:09:38 bsmith Exp bsmith $*/

/*-------------------------------------------------------------*/

#undef __FUNC__  
#define __FUNC__ "F90GetID"
int F90GetID(PetscDataType type,int *id)
{
  PetscFunctionBegin;
  if (type == PETSC_INT) {
    *id = F90_INT_ID;
  } else if (type == PETSC_DOUBLE) {
    *id = F90_DOUBLE_ID;
#if defined(PETSC_USE_COMPLEX)
  } else if (type == PETSC_COMPLEX) {
    *id = F90_COMPLEX_ID;
#endif
  } else if (type == PETSC_LONG) {
    *id = F90_INT_ID;
    /*  } else if (type == PETSC_CHAR) {
    *id = F90_CHAR_ID; */
  } else {
    SETERRQ(PETSC_ERR_ARG_OUTOFRANGE,"Unknown PETSc datatype");
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "F90Array1dCreate"
int F90Array1dCreate(void *array,PetscDataType type,int start,int len,F90Array1d *ptr)
{
  int size,size_int,ierr,id;

  PetscFunctionBegin;
  PetscValidPointer(array);
  PetscValidPointer(ptr);  
  ierr               = PetscDataTypeGetSize(type,&size);CHKERRQ(ierr);
  ierr               = F90GetID(type,&id);
  ptr->addr          = array;
  ptr->id            = id;
  ptr->cookie        = F90_COOKIE;
  ptr->sd            = size*8;
  ptr->ndim          = 1;
  ptr->dim[0].extent = len;
  ptr->dim[0].mult   = 1;
  ptr->dim[0].lower  = start;

  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "F90Array2dCreate"
int F90Array2dCreate(void *array,PetscDataType type,int start1,int len1,int start2,int len2,F90Array2d *ptr)
{

  int size,size_int,ierr,id;

  PetscFunctionBegin;
  PetscValidPointer(array);
  PetscValidPointer(ptr);  
  ierr               = PetscDataTypeGetSize(type,&size);CHKERRQ(ierr);
  ierr               = F90GetID(type,&id);
  ptr->addr          = array;
  ptr->id            = id;
  ptr->cookie        = F90_COOKIE;
  ptr->sd            = size*8;
  ptr->ndim          = 2;
  ptr->dim[0].extent = len1;
  ptr->dim[0].mult   = 1;
  ptr->dim[0].lower  = start1;
  ptr->dim[1].extent = len2;
  ptr->dim[1].mult   = len1*1;
  ptr->dim[1].lower  = start2;

  PetscFunctionReturn(0);
}
/*-------------------------------------------------------------*/
