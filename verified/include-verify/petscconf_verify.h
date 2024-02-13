#if !defined(INCLUDED_PETSCVERIFCONF_H)
#define INCLUDED_PETSCVERIFCONF_H

//Overwrite some flags for verification purposes

#ifdef PETSC_HAVE_BUILTIN_EXPECT
#undef PETSC_HAVE_BUILTIN_EXPECT
#endif

#ifdef PETSC_USE_DEBUG
#undef PETSC_USE_DEBUG
#endif

#ifdef PETSC_USE_INFO
#undef PETSC_USE_INFO
#endif

#ifdef PETSC_USE_LOG
#undef PETSC_USE_LOG
#endif

#ifdef PETSC_HAVE_ISINF
#undef PETSC_HAVE_ISINF
#endif

#ifdef PETSC_HAVE_ISNAN
#undef PETSC_HAVE_ISNAN
#endif

#ifdef PETSC_HAVE_COMPLEX
#undef PETSC_HAVE_COMPLEX
#endif

//TODO: should these go in the next category?
#ifdef petsc_EXPORTS
#undef petsc_EXPORTS
#endif

#ifdef PETSC_HAVE_MPI_INIT_THREAD
#undef PETSC_HAVE_MPI_INIT_THREAD
#endif

//TODO: eventually, this could change
#ifdef PETSC_HAVE_THREADSAFETY
#undef PETSC_HAVE_THREADSAFETY
#endif

#ifdef PETSC_HAVE_CXX
#undef PETSC_HAVE_CXX
#endif

#ifdef PETSC_HAVE_ELEMENTAL
#undef PETSC_HAVE_ELEMENTAL
#endif

#ifdef PETSC_HAVE_DYNAMIC_LIBRARIES
#undef PETSC_HAVE_DYNAMIC_LIBRARIES
#endif

#ifdef PETSC_HAVE_OPENMP
#undef PETSC_HAVE_OPENMP
#endif

#ifdef PETSC_USE_PETSC_MPI_EXTERNAL32
#undef PETSC_USE_PETSC_MPI_EXTERNAL32
#endif

#ifdef PETSC_SERIALIZE_FUNCTIONS
#undef PETSC_SERIALIZE_FUNCTIONS
#endif

#ifdef PETSC_HAVE_HWLOC
#undef PETSC_HAVE_HWLOC
#endif

#ifdef PETSC_USE_MALLOC_COALESCED
#undef PETSC_USE_MALLOC_COALESCED
#endif

#ifdef HAVE_MEMKIND
#undef HAVE_MEMKIND
#endif

#ifdef PETSC_HAVE_DEVICE
#undef PETSC_HAVE_DEVICE
#endif

#ifdef PREFER_DCOPY_FOR_MEMCPY
#undef PREFER_DCOPY_FOR_MEMCPY
#endif

#ifdef PREFER_FORTRAN_FORMEMCPY
#undef PREFER_FORTRAN_FORMEMCPY
#endif

#ifdef PETSC_USE_SHARED_MEMORY
#undef PETSC_USE_SHARED_MEMORY
#endif

#ifdef PETSC_HAVE_SAWS
#undef PETSC_HAVE_SAWS
#endif

#ifdef PETSC_USE_64BIT_INDICES
#undef PETSC_USE_64BIT_INDICES
#endif

#ifdef PETSC_USE_COMPLEX
#undef PETSC_USE_COMPLEX
#endif

#ifdef PETSC_USE_REAL_SINGLE
#undef PETSC_USE_REAL_SINGLE
#endif

#ifdef PETSC_HAVE_64BIT_BLAS_INDICES
#undef PETSC_HAVE_64BIT_BLAS_INDICES
#endif

#ifdef PETSC_BLASLAPACK_SNRM2_RETURNS_DOUBLE
#undef PETSC_BLASLAPACK_SNRM2_RETURNS_DOUBLE
#endif

#ifdef PETSC_HAVE_SSL
#undef PETSC_HAVE_SSL
#endif

#ifdef PETSC_HAVE_DLFCN_H
#undef PETSC_HAVE_DLFCN_H
#endif

#ifdef PETSC_USE_DEBUGGER
#undef PETSC_USE_DEBUGGER
#endif

#ifdef HAVE_SETJMP_H
#undef HAVE_SETJMP_H
#endif

#ifdef PETSC_HAVE_POPEN
#undef PETSC_HAVE_POPEN
#endif

#ifdef PETSC_HAVE_X
#undef PETSC_HAVE_X
#endif

//TODO: maybe change these 3?
#ifdef PETSC_HAVE_UNAME
#undef PETSC_HAVE_UNAME
#endif

#ifdef PETSC_HAVE_GETHOSTNAME
#undef PETSC_HAVE_GETHOSTNAME
#endif

#ifdef PETSC_HAVE_GETDOMAINNAME
#undef PETSC_HAVE_GETDOMAINNAME
#endif

#ifdef PETSC_USE_CTABLE
#undef PETSC_USE_CTABLE
#endif

//TODO: for now, CompCert doesn't like __float128
#ifdef PETSC_HAVE_REAL___FLOAT128
#undef PETSC_HAVE_REAL___FLOAT128
#endif

#ifdef PETSC_HAVE_DLADDR
#undef PETSC_HAVE_DLADDR
#endif

//TODO: make sure this is OK
#ifdef PETSC_HAVE_XMMINTRIN_H
#undef PETSC_HAVE_XMMINTRIN_H
#endif

//Flags incompatible with verification
#ifdef _WIN32
#error "Verification does not work on Windows"
#endif

#ifdef __cplusplus
#error "Can only verify C code"
#endif

#ifndef PETSC_HAVE_STRCASECMP
#error "Need strcasecmp for verification"
#endif

//Flags we need for verification that may not be set
//TODO: add others from petscconf?

#ifndef PETSC_USE_STRICT_PETSCERRORCODE
#define PETSC_USE_STRICT_PETSCERRORCODE 1
#endif

#ifndef PETSC_SKIP_ATTRIBUTE_MPI_TYPE_TAG
#define PETSC_SKIP_ATTRIBUTE_MPI_TYPE_TAG 1
#endif

#ifndef PETSC_USE_SINGLE_LIBRARY
#define PETSC_USE_SINGLE_LIBRARY 1
#endif

#ifndef PETSC_SKIP_COMPLEX
#define PETSC_SKIP_COMPLEX 1
#endif

#endif