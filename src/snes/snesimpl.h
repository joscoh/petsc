/* $Id: snesimpl.h,v 1.55 2000/09/02 02:49:31 bsmith Exp bsmith $ */

#ifndef __SNESIMPL_H
#define __SNESIMPL_H

#include "petscsnes.h"

/*
   Nonlinear solver context
 */
#define MAXSNESMONITORS 5

struct _p_SNES {
  PETSCHEADER(int)

  /*  ------------------------ User-provided stuff -------------------------------*/
  void  *user;		                        /* user-defined context */

  Vec   vec_sol,vec_sol_always;                /* pointer to solution */
  Vec   vec_sol_update_always;                  /* pointer to solution update */

  int   (*computefunction)(SNES,Vec,Vec,void*); /* function routine */
  Vec   vec_func,vec_func_always;              /* pointer to function (or gradient) */
  void  *funP;                                  /* user-defined function context */

  int   (*computejacobian)(SNES,Vec,Mat*,Mat*,MatStructure*,void*);
  Mat   jacobian;                               /* Jacobian (or Hessian) matrix */
  Mat   jacobian_pre;                           /* preconditioner matrix */
  void  *jacP;                                  /* user-defined Jacobian context */
  SLES  sles;                                   /* linear solver context */

  int   (*computescaling)(Vec,Vec,void*);       /* scaling routine */
  Vec   scaling;                                /* scaling vector */
  void  *scaP;                                  /* scaling context */

  /* ---------------- PETSc-provided (or user-provided) stuff ---------------------*/

  int   (*monitor[MAXSNESMONITORS])(SNES,int,double,void*); /* monitor routine */
  int   (*monitordestroy[MAXSNESMONITORS])(void*);          /* monitor context destroy routine */
  void  *monitorcontext[MAXSNESMONITORS];                   /* monitor context */
  int   numbermonitors;                                     /* number of monitors */
  int   (*converged)(SNES,double,double,double,SNESConvergedReason*,void*);      /* convergence routine */
  void  *cnvP;	                                            /* convergence context */
  SNESConvergedReason reason;

  /* --- Routines and data that are unique to each particular solver --- */

  int   (*setup)(SNES);             /* routine to set up the nonlinear solver */
  int   setupcalled;                /* true if setup has been called */
  int   (*solve)(SNES,int*);        /* actual nonlinear solver */
  int   (*setfromoptions)(SNES);    /* sets options from database */
  void  *data;                      /* implementation-specific data */

  /* --------------------------  Parameters -------------------------------------- */

  int      max_its;            /* max number of iterations */
  int      max_funcs;          /* max number of function evals */
  int      nfuncs;             /* number of function evaluations */
  int      iter;               /* global iteration number */
  int      linear_its;         /* total number of linear solver iterations */
  double   norm;               /* residual norm of current iterate
				  (or gradient norm of current iterate) */
  double   rtol;               /* relative tolerance */
  double   atol;               /* absolute tolerance */
  double   xtol;               /* relative tolerance in solution */
  double   trunctol;           /* truncation tolerance */

  /* ------------------------ Default work-area management ---------------------- */

  int      nwork;              
  Vec      *work;

  /* ------------------------- Miscellaneous Information ------------------------ */

  double     *conv_hist;         /* If !0, stores function norm (or
                                    gradient norm) at each iteration */
  int        *conv_hist_its;     /* linear iterations for each Newton step */
  int        conv_hist_len;      /* size of convergence history array */
  int        conv_hist_max;      /* actual amount of data in conv_history */
  PetscTruth conv_hist_reset;    /* reset counter for each new SNES solve */
  int        nfailures;          /* number of unsuccessful step attempts */

  /* ------------------  Data for unconstrained minimization  ------------------ */
  /* unconstrained minimization info ... For now we share everything else
     with the nonlinear equations code.  We should find a better way to deal 
     with this; the naming conventions are confusing.  Perhaps use unions? */

  int             (*computeumfunction)(SNES,Vec,double*,void*);
  double          fc;                /* function value */
  void            *umfunP;           /* function pointer */
  SNESProblemType method_class;      /* type of solver */
  double          deltatol;          /* trust region convergence tolerance */
  double          fmin;              /* minimum tolerance for function value */
  int             set_method_called; /* flag indicating set_method has been called */

 /*
   These are REALLY ugly and don't belong here, but since they must 
  be destroyed at the conclusion we have to put them somewhere.
 */
  PetscTruth  ksp_ewconv;        /* flag indicating use of Eisenstat-Walker KSP convergence criteria */
  void        *kspconvctx;       /* KSP convergence context */

  Mat         mfshell;           /* MatShell for runtime matrix-free option */

  double      ttol;              /* used by default convergence test routine */

  Vec         *vwork;            /* more work vectors for Jacobian/Hessian approx */
  int         nvwork;
  int        (*destroy)(SNES);
  int        (*view)(SNES,PetscViewer);
};

/* Context for Eisenstat-Walker convergence criteria for KSP solvers */
typedef struct {
  int    version;             /* flag indicating version 1 or 2 of test */
  double rtol_0;              /* initial rtol */
  double rtol_last;           /* last rtol */
  double rtol_max;            /* maximum rtol */
  double gamma;               /* mult. factor for version 2 rtol computation */
  double alpha;               /* power for version 2 rtol computation */
  double alpha2;              /* power for safeguard */
  double threshold;           /* threshold for imposing safeguard */
  double lresid_last;         /* linear residual from last iteration */
  double norm_last;           /* function norm from last iteration */
} SNES_KSP_EW_ConvCtx;

#define SNESLogConvHistory(snes,res,its) \
  { if (snes->conv_hist && snes->conv_hist_max > snes->conv_hist_len) \
    { snes->conv_hist[snes->conv_hist_len]       = res; \
      snes->conv_hist_its[snes->conv_hist_len++] = its; \
    }}

#define SNESMonitor(snes,it,rnorm) \
        { int _ierr,_i,_im = snes->numbermonitors; \
          for (_i=0; _i<_im; _i++) {\
            _ierr = (*snes->monitor[_i])(snes,it,rnorm,snes->monitorcontext[_i]);CHKERRQ(_ierr); \
	  } \
	}

int SNES_KSP_EW_Converged_Private(KSP,int,double,KSPConvergedReason*,void*);
int SNES_KSP_EW_ComputeRelativeTolerance_Private(SNES,KSP);
int SNESScaleStep_Private(SNES,Vec,double*,double*,double*,double*);

#endif
