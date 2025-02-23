#include <petsc/private/deviceimpl.h>
#include <petsc/private/kokkosimpl.hpp>
#include <petscpkg_version.h>
#include <petsc_kokkos.hpp>

PetscBool    PetscKokkosInitialized = PETSC_FALSE;
PetscScalar *PetscScalarPool        = nullptr;
PetscInt     PetscScalarPoolSize    = 0;

Kokkos::DefaultExecutionSpace *PetscKokkosExecutionSpacePtr = nullptr;

PetscErrorCode PetscKokkosFinalize_Private(void)
{
  PetscFunctionBegin;
  PetscCallCXX(delete PetscKokkosExecutionSpacePtr);
  PetscCallCXX(Kokkos::kokkos_free(PetscScalarPool));
  PetscScalarPoolSize = 0;
  if (PetscBeganKokkos) {
    PetscCallCXX(Kokkos::finalize());
    PetscBeganKokkos = PETSC_FALSE;
  }
  PetscFunctionReturn(PETSC_SUCCESS);
}

PetscErrorCode PetscKokkosIsInitialized_Private(PetscBool *isInitialized)
{
  PetscFunctionBegin;
  *isInitialized = Kokkos::is_initialized() ? PETSC_TRUE : PETSC_FALSE;
  PetscFunctionReturn(PETSC_SUCCESS);
}

/* Initialize Kokkos if not yet */
PetscErrorCode PetscKokkosInitializeCheck(void)
{
  PetscFunctionBegin;
  if (!Kokkos::is_initialized()) {
#if PETSC_PKG_KOKKOS_VERSION_GE(3, 7, 0)
    auto args = Kokkos::InitializationSettings();
#else
    auto args             = Kokkos::InitArguments{}; /* use default constructor */
#endif

#if (defined(KOKKOS_ENABLE_CUDA) && PetscDefined(HAVE_CUDA)) || (defined(KOKKOS_ENABLE_HIP) && PetscDefined(HAVE_HIP)) || (defined(KOKKOS_ENABLE_SYCL) && PetscDefined(HAVE_SYCL))
    /* Kokkos does not support CUDA and HIP at the same time (but we do :)) */
    PetscDevice device;
    PetscInt    deviceId;
    PetscCall(PetscDeviceCreate(PETSC_DEVICE_DEFAULT(), PETSC_DECIDE, &device));
    PetscCall(PetscDeviceGetDeviceId(device, &deviceId));
    PetscCall(PetscDeviceDestroy(&device));
  #if PETSC_PKG_KOKKOS_VERSION_GE(4, 0, 0)
    // if device_id is not set, and no gpus have been found, kokkos will use CPU
    if (deviceId >= 0) args.set_device_id(static_cast<int>(deviceId));
  #elif PETSC_PKG_KOKKOS_VERSION_GE(3, 7, 0)
    args.set_device_id(static_cast<int>(deviceId));
  #else
    PetscCall(PetscMPIIntCast(deviceId, &args.device_id));
  #endif
#endif

#if PETSC_PKG_KOKKOS_VERSION_GE(3, 7, 0)
    args.set_disable_warnings(!PetscDefined(HAVE_KOKKOS_INIT_WARNINGS));
#else
    args.disable_warnings = !PetscDefined(HAVE_KOKKOS_INIT_WARNINGS);
#endif

    /* To use PetscNumOMPThreads, one has to configure petsc --with-openmp.
       Otherwise, let's keep the default value (-1) of args.num_threads.
    */
#if defined(KOKKOS_ENABLE_OPENMP) && PetscDefined(HAVE_OPENMP)
  #if PETSC_PKG_KOKKOS_VERSION_GE(3, 7, 0)
    args.set_num_threads(PetscNumOMPThreads);
  #else
    args.num_threads = PetscNumOMPThreads;
  #endif
#endif
    PetscCallCXX(Kokkos::initialize(args));
    PetscBeganKokkos = PETSC_TRUE;
  }
  if (!PetscKokkosExecutionSpacePtr) { // No matter Kokkos is init'ed by petsc or by user, we need to init PetscKokkosExecutionSpacePtr
#if defined(PETSC_HAVE_CUDA)
    extern cudaStream_t PetscDefaultCudaStream;
    PetscCallCXX(PetscKokkosExecutionSpacePtr = new Kokkos::DefaultExecutionSpace(PetscDefaultCudaStream));
#elif defined(PETS_HAVE_HIP)
    extern hipStream_t PetscDefaultHipStream;
    PetscCallCXX(PetscKokkosExecutionSpacePtr = new Kokkos::DefaultExecutionSpace(PetscDefaultHipStream));
#else
    PetscCallCXX(PetscKokkosExecutionSpacePtr = new Kokkos::DefaultExecutionSpace());
#endif
  }
  if (!PetscScalarPoolSize) { // A pool for a small count of PetscScalars
    PetscScalarPoolSize = 1024;
    PetscCallCXX(PetscScalarPool = static_cast<PetscScalar *>(Kokkos::kokkos_malloc(sizeof(PetscScalar) * PetscScalarPoolSize)));
  }

  PetscKokkosInitialized = PETSC_TRUE; // PetscKokkosInitializeCheck() was called
  PetscFunctionReturn(PETSC_SUCCESS);
}
