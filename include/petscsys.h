/* $Id: petscsys.h,v 1.53 2000/09/22 20:47:47 bsmith Exp bsmith $ */
/*
    Provides access to system related and general utility routines.
*/
#if !defined(__PETSCSYS_H)
#define __PETSCSYS_H

#include "petsc.h"

EXTERN int  PetscGetArchType(char[],int);
EXTERN int  PetscGetHostName(char[],int);
EXTERN int  PetscGetUserName(char[],int);
EXTERN int  PetscGetProgramName(char[],int);
EXTERN int  PetscSetProgramName(const char[]);
EXTERN int  PetscGetDate(char[],int);
EXTERN int  PetscSetInitialDate(void);
EXTERN int  PetscGetInitialDate(char[],int);

EXTERN int  PetscSortInt(int,int[]);
EXTERN int  PetscSortIntWithPermutation(int,const int[],int[]);
EXTERN int  PetscSortIntWithArray(int,int[],int[]);
EXTERN int  PetscSortDouble(int,double[]);
EXTERN int  PetscSortDoubleWithPermutation(int,const double[],int[]);

EXTERN int  PetscSetDisplay(void);
EXTERN int  PetscGetDisplay(char[],int);

#define PETSCRANDOM_COOKIE PETSC_COOKIE+19

typedef enum { RANDOM_DEFAULT,RANDOM_DEFAULT_REAL,
               RANDOM_DEFAULT_IMAGINARY } PetscRandomType;

typedef struct _p_PetscRandom*   PetscRandom;

EXTERN int PetscRandomCreate(MPI_Comm,PetscRandomType,PetscRandom*);
EXTERN int PetscRandomGetValue(PetscRandom,Scalar*);
EXTERN int PetscRandomSetInterval(PetscRandom,Scalar,Scalar);
EXTERN int PetscRandomDestroy(PetscRandom);

EXTERN int PetscGetFullPath(const char[],char[],int);
EXTERN int PetscGetRelativePath(const char[],char[],int);
EXTERN int PetscGetWorkingDirectory(char[],int);
EXTERN int PetscGetRealPath(char[],char[]);
EXTERN int PetscGetHomeDirectory(char[],int);
EXTERN int PetscTestFile(const char[],char,PetscTruth*);
EXTERN int PetscBinaryRead(int,void*,int,PetscDataType);
EXTERN int PetscSynchronizedBinaryRead(MPI_Comm,int,void*,int,PetscDataType);
EXTERN int PetscBinaryWrite(int,void*,int,PetscDataType,int);
EXTERN int PetscBinaryOpen(const char[],int,int *);
EXTERN int PetscBinaryClose(int);
EXTERN int PetscSharedTmp(MPI_Comm,PetscTruth *);
EXTERN int PetscSharedWorkingDirectory(MPI_Comm,PetscTruth *);
EXTERN int PetscGetTmp(MPI_Comm,char *,int);
EXTERN int PetscFileRetrieve(MPI_Comm,const char *,char *,int,PetscTruth*);

/*
   In binary files variables are stored using the following lengths,
  regardless of how they are stored in memory on any one particular
  machine. Use these rather then sizeof() in computing sizes for 
  PetscBinarySeek().
*/
#define PETSC_BINARY_INT_SIZE    (32/8)
#define PETSC_BINARY_FLOAT_SIZE  (32/8)
#define PETSC_BINARY_CHAR_SIZE    (8/8)
#define PETSC_BINARY_SHORT_SIZE  (16/8)
#define PETSC_BINARY_DOUBLE_SIZE (64/8)
#define PETSC_BINARY_SCALAR_SIZE sizeof(Scalar)

typedef enum {PETSC_BINARY_SEEK_SET = 0,PETSC_BINARY_SEEK_CUR = 1,PETSC_BINARY_SEEK_END = 2} PetscBinarySeekType;
EXTERN int PetscBinarySeek(int,int,PetscBinarySeekType,int*);
EXTERN int PetscSynchronizedBinarySeek(MPI_Comm,int,int,PetscBinarySeekType,int*);

EXTERN int PetscSetDebugger(const char[],PetscTruth);
EXTERN int PetscSetDefaultDebugger(void);
EXTERN int PetscSetDebuggerFromString(char*);
EXTERN int PetscAttachDebugger(void);
EXTERN int PetscStopForDebugger(void);

#endif      

