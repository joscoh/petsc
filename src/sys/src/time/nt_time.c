#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: nt_time.c,v 1.12 1998/12/17 21:56:35 balay Exp balay $";
#endif

#include <petsc.h>
#if defined (PARCH_win32_gnu) || defined (PARCH_win32)
#include <Windows.h>
#define FACTOR   4294967296.0

#undef __FUNC__  
#define __FUNC__ "nt_time"
PetscDouble nt_time(void) 
{
  static int    flag = 1;
  int           ierr;

  static LARGE_INTEGER StartTime,PerfFreq,CurTime; 
  static PetscDouble SecInTick=0.0;
  
  DWORD       dwStartHigh, dwCurHigh;
  PetscDouble dTime, dHigh;
  double      time;
  
  
  PetscFunctionBegin;
  if (flag) {
    ierr = QueryPerformanceCounter( &StartTime ); CHKERRQ(!ierr);
    ierr = QueryPerformanceFrequency( &PerfFreq ); CHKERRQ(!ierr);
    SecInTick = 1.0/((double)PerfFreq.HighPart*FACTOR+(double)PerfFreq.LowPart);
    flag = 0;
  }		
  
  ierr        = QueryPerformanceCounter( &CurTime ); CHKERRQ(!ierr);
  dwCurHigh   = (DWORD)CurTime.HighPart;
  dwStartHigh = (DWORD)StartTime.HighPart;
  dHigh       = (signed)(dwCurHigh - dwStartHigh);

  dTime = dHigh*(double)FACTOR + (double)CurTime.LowPart - (double)StartTime.LowPart;
  time  = (double)SecInTick*dTime;

  PetscFunctionReturn(time);
}
#endif
