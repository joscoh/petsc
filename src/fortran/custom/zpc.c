/*$Id: zpc.c,v 1.41 2000/11/28 17:32:51 bsmith Exp bsmith $*/

#include "src/fortran/custom/zpetsc.h"
#include "petscsles.h"
#include "petscmg.h"

#ifdef PETSC_HAVE_FORTRAN_CAPS
#define mgdefaultresidual_         MGDEFAULTRESIDUAL
#define mgsetresidual_             MGSETRESIDUAL
#define pcasmsetlocalsubdomains_   PCASMSETLOCALSUBDOMAINS
#define pcasmsetglobalsubdomains_  PCASMSETGLOBALSUBDOMAINS
#define pcasmgetlocalsubdomains_   PCASMGETLOCALSUBDOMAINS
#define pcregisterdestroy_         PCREGISTERDESTROY
#define pcdestroy_                 PCDESTROY
#define pccreate_                  PCCREATE
#define pcgetoperators_            PCGETOPERATORS
#define pcgetfactoredmatrix_       PCGETFACTOREDMATRIX
#define pcsetoptionsprefix_        PCSETOPTIONSPREFIX
#define pcappendoptionsprefix_     PCAPPENDOPTIONSPREFIX
#define pcbjacobigetsubsles_       PCBJACOBIGETSUBSLES
#define pcasmgetsubsles_           PCASMGETSUBSLES
#define mggetcoarsesolve_          MGGETCOARSESOLVE
#define mggetsmoother_             MGGETSMOOTHER
#define mggetsmootherup_           MGGETSMOOTHERUP
#define mggetsmootherdown_         MGGETSMOOTHERDOWN
#define pcshellsetapply_           PCSHELLSETAPPLY
#define pcshellsetapplyrichardson_ PCSHELLSETAPPLYRICHARDSON
#define pcgettype_                 PCGETTYPE
#define pcsettype_                 PCSETTYPE
#define pcgetoptionsprefix_        PCGETOPTIONSPREFIX
#define pcnullspaceattach_         PCNULLSPACEATTACH
#define matnullspacecreate_        MATNULLSPACECREATE
#define pcview_                    PCVIEW
#define mgsetlevels_               MGSETLEVELS
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE)
#define mgdefaultresidual_         mgdefaultresidual
#define mgsetresidual_             mgsetresidual
#define pcasmsetlocalsubdomains_   pcasmsetlocalsubdomains
#define pcasmsetglobalsubdomains_  pcasmsetglobalsubdomains
#define pcasmgetlocalsubdomains_   pcasmgetlocalsubdomains
#define matnullspacecreate_        matnullspacecreate
#define pcnullspaceattach_         pcnullspaceattach
#define pcregisterdestroy_         pcregisterdestroy
#define pcdestroy_                 pcdestroy
#define pccreate_                  pccreate
#define pcgetoperators_            pcgetoperators
#define pcgetfactoredmatrix_       pcgetfactoredmatrix
#define pcsetoptionsprefix_        pcsetoptionsprefix
#define pcappendoptionsprefix_     pcappendoptionsprefix
#define pcbjacobigetsubsles_       pcbjacobigetsubsles
#define pcasmgetsubsles_           pcasmgetsubsles
#define mggetcoarsesolve_          mggetcoarsesolve
#define mggetsmoother_             mggetsmoother
#define mggetsmootherup_           mggetsmootherup
#define mggetsmootherdown_         mggetsmootherdown
#define pcshellsetapplyrichardson_ pcshellsetapplyrichardson
#define pcshellsetapply_           pcshellsetapply
#define pcgettype_                 pcgettype
#define pcsettype_                 pcsettype
#define pcgetoptionsprefix_        pcgetoptionsprefix
#define pcview_                    pcview
#define mgsetlevels_               mgsetlevels
#endif

EXTERN_C_BEGIN

void PETSC_STDCALL mgsetlevels_(PC *pc,int *levels,MPI_Comm *comms, int *__ierr )
{
  MPI_Comm *comm = comms;
  if (FORTRANNULLOBJECT(comms)) comm = 0;
  *__ierr = MGSetLevels(*pc,*levels,comm);
}

void PETSC_STDCALL pcview_(PC *pc,PetscViewer *viewer, int *ierr)
{
  PetscViewer v;
  PetscPatchDefaultViewers_Fortran(viewer,v);
  *ierr = PCView(*pc,v);
}

void PETSC_STDCALL matnullspacecreate_(MPI_Comm comm,int *has_cnst,int *n,Vec *vecs,MatNullSpace *SP,int *ierr)
{
  *ierr = MatNullSpaceCreate((MPI_Comm)PetscToPointerComm(*(int*)(comm)),*has_cnst,*n,vecs,SP);
}

void PETSC_STDCALL pcnullspaceattach_(PC *pc,MatNullSpace *nullsp,int *ierr)
{
  *ierr = PCNullSpaceAttach(*pc,*nullsp);
}

void PETSC_STDCALL pcsettype_(PC *pc,CHAR type PETSC_MIXED_LEN(len),int *ierr PETSC_END_LEN(len))
{
  char *t;

  FIXCHAR(type,len,t);
  *ierr = PCSetType(*pc,t);
  FREECHAR(type,t);
}


static void (PETSC_STDCALL *f1)(void *,Vec*,Vec*,int*);
static int ourshellapply(void *ctx,Vec x,Vec y)
{
  int              ierr = 0;
  (*f1)(ctx,&x,&y,&ierr);CHKERRQ(ierr);
  return 0;
}

void PETSC_STDCALL pcshellsetapply_(PC *pc,void (PETSC_STDCALL *apply)(void*,Vec *,Vec *,int*),void *ptr,
                      int *ierr)
{
  f1 = apply;
  *ierr = PCShellSetApply(*pc,ourshellapply,ptr);
}

static void (PETSC_STDCALL *f9)(void *,int*);
static int ourshellsetup(void *ctx)
{
  int              ierr = 0;

  (*f9)(ctx,&ierr);CHKERRQ(ierr);
  return 0;
}

void PETSC_STDCALL pcshellsetsetup_(PC *pc,void (PETSC_STDCALL *setup)(void*,int*),int *ierr)
{
  f9 = setup;
  *ierr = PCShellSetSetUp(*pc,ourshellsetup);
}

/* -----------------------------------------------------------------*/
static void (PETSC_STDCALL *f2)(void*,Vec*,Vec*,Vec*,int*,int*);
static int ourapplyrichardson(void *ctx,Vec x,Vec y,Vec w,int m)
{
  int              ierr = 0;

  (*f2)(ctx,&x,&y,&w,&m,&ierr);CHKERRQ(ierr);
  return 0;
}

void PETSC_STDCALL pcshellsetapplyrichardson_(PC *pc,
         void (PETSC_STDCALL *apply)(void*,Vec *,Vec *,Vec *,int*,int*),
         void *ptr,int *ierr)
{
  f2 = apply;
  *ierr = PCShellSetApplyRichardson(*pc,ourapplyrichardson,ptr);
}

void PETSC_STDCALL mggetcoarsesolve_(PC *pc,SLES *sles,int *ierr)
{
  *ierr = MGGetCoarseSolve(*pc,sles);
}

void PETSC_STDCALL mggetsmoother_(PC *pc,int *l,SLES *sles,int *ierr)
{
  *ierr = MGGetSmoother(*pc,*l,sles);
}

void PETSC_STDCALL mggetsmootherup_(PC *pc,int *l,SLES *sles,int *ierr)
{
  *ierr = MGGetSmootherUp(*pc,*l,sles);
}

void PETSC_STDCALL mggetsmootherdown_(PC *pc,int *l,SLES *sles,int *ierr)
{
  *ierr = MGGetSmootherDown(*pc,*l,sles);
}

void PETSC_STDCALL pcbjacobigetsubsles_(PC *pc,int *n_local,int *first_local,SLES *sles,int *ierr)
{
  SLES *tsles;
  int  i;
  if (FORTRANNULLINTEGER(n_local)) n_local = PETSC_NULL;
  if (FORTRANNULLINTEGER(first_local)) first_local = PETSC_NULL;
  *ierr = PCBJacobiGetSubSLES(*pc,n_local,first_local,&tsles);
  for (i=0; i<*n_local; i++){
    sles[i] = tsles[i];
  }
}

void PETSC_STDCALL pcasmgetsubsles_(PC *pc,int *n_local,int *first_local,SLES *sles,int *ierr)
{
  SLES *tsles;
  int  i,nloc;
  if (FORTRANNULLINTEGER(n_local)) n_local = PETSC_NULL;
  if (FORTRANNULLINTEGER(first_local)) first_local = PETSC_NULL;
  *ierr = PCASMGetSubSLES(*pc,&nloc,first_local,&tsles);
  if (n_local) *n_local = nloc;
  for (i=0; i<nloc; i++){
    sles[i] = tsles[i];
  }
}

void PETSC_STDCALL pcgetoperators_(PC *pc,Mat *mat,Mat *pmat,MatStructure *flag,int *ierr)
{
  if (FORTRANNULLINTEGER(flag)) flag = PETSC_NULL;
  if (FORTRANNULLOBJECT(mat))   mat = PETSC_NULL;
  if (FORTRANNULLOBJECT(pmat))  pmat = PETSC_NULL;
  *ierr = PCGetOperators(*pc,mat,pmat,flag);
}

void PETSC_STDCALL pcgetfactoredmatrix_(PC *pc,Mat *mat,int *ierr)
{
  *ierr = PCGetFactoredMatrix(*pc,mat);
}
 
void PETSC_STDCALL pcsetoptionsprefix_(PC *pc,CHAR prefix PETSC_MIXED_LEN(len),
                                       int *ierr PETSC_END_LEN(len))
{
  char *t;

  FIXCHAR(prefix,len,t);
  *ierr = PCSetOptionsPrefix(*pc,t);
  FREECHAR(prefix,t);
}

void PETSC_STDCALL pcappendoptionsprefix_(PC *pc,CHAR prefix PETSC_MIXED_LEN(len),
                                          int *ierr PETSC_END_LEN(len))
{
  char *t;

  FIXCHAR(prefix,len,t);
  *ierr = PCAppendOptionsPrefix(*pc,t);
  FREECHAR(prefix,t);
}

void PETSC_STDCALL pcdestroy_(PC *pc,int *ierr)
{
  *ierr = PCDestroy(*pc);
}

void PETSC_STDCALL pccreate_(MPI_Comm comm,PC *newpc,int *ierr)
{
  *ierr = PCCreate((MPI_Comm)PetscToPointerComm(*(int*)(comm)),newpc);
}

void PETSC_STDCALL pcregisterdestroy_(int *ierr)
{
  *ierr = PCRegisterDestroy();
}

void PETSC_STDCALL pcgettype_(PC *pc,CHAR name PETSC_MIXED_LEN(len),int *ierr PETSC_END_LEN(len))
{
  char *tname;

  *ierr = PCGetType(*pc,&tname);
#if defined(PETSC_USES_CPTOFCD)
  {
  char *t = _fcdtocp(name); int len1 = _fcdlen(name);
  *ierr = PetscStrncpy(t,tname,len1); if (*ierr) return;
  }
#else
  *ierr = PetscStrncpy(name,tname,len);if (*ierr) return;
#endif
}

void PETSC_STDCALL pcgetoptionsprefix_(PC *pc,CHAR prefix PETSC_MIXED_LEN(len),
                                       int *ierr PETSC_END_LEN(len))
{
  char *tname;

  *ierr = PCGetOptionsPrefix(*pc,&tname);
#if defined(PETSC_USES_CPTOFCD)
  {
    char *t = _fcdtocp(prefix); int len1 = _fcdlen(prefix);
    *ierr = PetscStrncpy(t,tname,len1);if (*ierr) return;
  }
#else
  *ierr = PetscStrncpy(prefix,tname,len);if (*ierr) return;
#endif
}

void PETSC_STDCALL pcasmsetlocalsubdomains_(PC *pc,int *n,IS *is, int *ierr )
{
  if (FORTRANNULLOBJECT(is)) is = PETSC_NULL;
  *ierr = PCASMSetLocalSubdomains(*pc,*n,is);
}

void PETSC_STDCALL pcasmsettotalsubdomains_(PC *pc,int *N,IS *is, int *ierr )
{
  if (FORTRANNULLOBJECT(is)) is = PETSC_NULL;
  *ierr = PCASMSetTotalSubdomains(*pc,*N,is);
}

void PETSC_STDCALL pcasmgetlocalsubdomains_(PC *pc,int *n,IS **is, int *ierr )
{
  if (FORTRANNULLOBJECT(is)) is = PETSC_NULL;
  if (FORTRANNULLINTEGER(n)) n  = PETSC_NULL;
  *ierr = PCASMGetLocalSubdomains(*pc,n,is);
}

void PETSC_STDCALL mgdefaultresidual_(Mat *mat,Vec *b,Vec *x,Vec *r, int *ierr )
{
  *ierr = MGDefaultResidual(*mat,*b,*x,*r);
}

static int ourresidualfunction(Mat mat,Vec b,Vec x,Vec R)
{
  int ierr = 0;
  (*(void (PETSC_STDCALL *)(Mat*,Vec*,Vec*,Vec*,int*))(((PetscObject)mat)->fortran_func_pointers[0]))(&mat,&b,&x,&R,&ierr);
  return 0;
}

void PETSC_STDCALL mgsetresidual_(PC *pc,int *l,int (*residual)(Mat*,Vec*,Vec*,Vec*,int*),Mat *mat, int *ierr )
{
  int (*rr)(Mat,Vec,Vec,Vec);
  if ((void*)residual == (void*)mgdefaultresidual_) rr = MGDefaultResidual;
  else {
    if (!((PetscObject)*mat)->fortran_func_pointers) {
      ((PetscObject)*mat)->fortran_func_pointers = (void**)PetscMalloc(1*sizeof(void *));
    }
    ((PetscObject)*mat)->fortran_func_pointers[0] = (void*)residual;
    rr = ourresidualfunction;
  }
  *ierr = MGSetResidual(*pc,*l,rr,*mat);
}
EXTERN_C_END

