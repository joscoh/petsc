# --------------------------------------------------------------------

cdef extern from * nogil:

    struct _n_DMLabel
    ctypedef _n_DMLabel* PetscDMLabel "DMLabel"

    int DMPlexCreate(MPI_Comm,PetscDM*)
    int DMPlexCreateCohesiveSubmesh(PetscDM,PetscBool,const_char[],PetscInt,PetscDM*)
    int DMPlexCreateFromCellList(MPI_Comm,PetscInt,PetscInt,PetscInt,PetscInt,PetscBool,int[],PetscInt,double[],PetscDM*)
    #int DMPlexCreateFromDAG(PetscDM,PetscInt,const_PetscInt[],const_PetscInt[],const_PetscInt[],const_PetscInt[],const_PetscScalar[])

    int DMPlexGetDimension(PetscDM,PetscInt*)
    int DMPlexSetDimension(PetscDM,PetscInt)
    int DMPlexGetChart(PetscDM,PetscInt*,PetscInt*)
    int DMPlexSetChart(PetscDM,PetscInt,PetscInt)
    int DMPlexGetConeSize(PetscDM,PetscInt,PetscInt*)
    int DMPlexSetConeSize(PetscDM,PetscInt,PetscInt)
    int DMPlexGetCone(PetscDM,PetscInt,const_PetscInt*[])
    int DMPlexSetCone(PetscDM,PetscInt,const_PetscInt[])
    int DMPlexInsertCone(PetscDM,PetscInt,PetscInt,PetscInt)
    int DMPlexInsertConeOrientation(PetscDM,PetscInt,PetscInt,PetscInt)
    int DMPlexGetConeOrientation(PetscDM,PetscInt,const_PetscInt*[])
    int DMPlexSetConeOrientation(PetscDM,PetscInt,const_PetscInt[])
    int DMPlexGetSupportSize(PetscDM,PetscInt,PetscInt*)
    int DMPlexSetSupportSize(PetscDM,PetscInt,PetscInt)
    int DMPlexGetSupport(PetscDM,PetscInt,const_PetscInt*[])
    int DMPlexSetSupport(PetscDM,PetscInt,const_PetscInt[])
    #int DMPlexInsertSupport(PetscDM,PetscInt,PetscInt,PetscInt)
    #int DMPlexGetConeSection(PetscDM,PetscSection*)
    #int DMPlexGetSupportSection(PetscDM,PetscSection*)
    #int DMPlexGetCones(PetscDM,PetscInt*[])
    #int DMPlexGetConeOrientations(PetscDM,PetscInt*[])
    int DMPlexGetMaxSizes(PetscDM,PetscInt*,PetscInt*)
    int DMPlexSymmetrize(PetscDM)
    int DMPlexStratify(PetscDM)
    #int DMPlexEqual(PetscDM,PetscDM,PetscBool*)
    int DMPlexOrient(PetscDM)
    #int DMPlexInterpolate(PetscDM,PetscDM*)
    #int DMPlexUninterpolate(PetscDM,PetscDM*)
    #int DMPlexLoad(PetscViewer,PetscDM)
    #int DMPlexSetPreallocationCenterDimension(PetscDM,PetscInt)
    #int DMPlexGetPreallocationCenterDimension(PetscDM,PetscInt*)
    #int DMPlexPreallocateOperator(PetscDM,PetscInt,PetscSection,PetscSection,PetscInt[],PetscInt[],PetscInt[],PetscInt[],Mat,PetscBool)
    #int DMPlexGetPointLocal(PetscDM,PetscInt,PetscInt*,PetscInt*)
    #int DMPlexPointLocalRef(PetscDM,PetscInt,PetscScalar*,void*)
    #int DMPlexPointLocalRead(PetscDM,PetscInt,const_PetscScalar*,const_void*)
    #int DMPlexGetPointGlobal(PetscDM,PetscInt,PetscInt*,PetscInt*)
    #int DMPlexPointGlobalRef(PetscDM,PetscInt,PetscScalar*,void*)
    #int DMPlexPointGlobalRead(PetscDM,PetscInt,const_PetscScalar*,const_void*)

    int DMPlexCreateLabel(PetscDM,const_char[])
    int DMPlexGetLabelValue(PetscDM,const_char[],PetscInt,PetscInt*)
    int DMPlexSetLabelValue(PetscDM,const_char[],PetscInt,PetscInt)
    int DMPlexClearLabelValue(PetscDM,const_char[],PetscInt,PetscInt)
    int DMPlexGetLabelSize(PetscDM,const_char[],PetscInt*)
    int DMPlexGetLabelIdIS(PetscDM,const_char[],PetscIS*)
    int DMPlexGetStratumSize(PetscDM,const_char[],PetscInt,PetscInt*)
    int DMPlexGetStratumIS(PetscDM,const_char[],PetscInt,PetscIS*)
    int DMPlexClearLabelStratum(PetscDM,const_char[],PetscInt)
    #int PetscSectionCreateGlobalSectionLabel(PetscSection,PetscSF,PetscBool,PetscDMLabel,PetscInt,PetscSection*)

    int DMPlexGetNumLabels(PetscDM,PetscInt*)
    int DMPlexGetLabelName(PetscDM,PetscInt,const_char**)
    int DMPlexHasLabel(PetscDM,const_char[],PetscBool*)
    int DMPlexGetLabel(PetscDM,const_char*,PetscDMLabel*)
    #int DMPlexAddLabel(PetscDM,PetscDMLabel)
    int DMPlexRemoveLabel(PetscDM,const_char[],PetscDMLabel*)
    int DMPlexGetCellNumbering(PetscDM,PetscIS*)
    int DMPlexGetVertexNumbering(PetscDM,PetscIS*)

    int DMPlexGetDepth(PetscDM,PetscInt*)
    #int DMPlexGetDepthLabel(PetscDM,PetscDMLabel*)
    int DMPlexGetDepthStratum(PetscDM,PetscInt,PetscInt*,PetscInt*)
    int DMPlexGetHeightStratum(PetscDM,PetscInt,PetscInt*,PetscInt*)

    int DMPlexGetMeet(PetscDM,PetscInt,const_PetscInt[],PetscInt*,const_PetscInt**)
    #int DMPlexGetFullMeet(PetscDM,PetscInt,const_PetscInt[],PetscInt*,const_PetscInt**)
    int DMPlexRestoreMeet(PetscDM,PetscInt,const_PetscInt[],PetscInt*,const_PetscInt**)
    int DMPlexGetJoin(PetscDM,PetscInt,const_PetscInt[],PetscInt*,const_PetscInt**)
    #int DMPlexGetFullJoin(PetscDM,PetscInt,const_PetscInt[],PetscInt*,const_PetscInt**)
    int DMPlexRestoreJoin(PetscDM,PetscInt,const_PetscInt[],PetscInt*,const_PetscInt**)
    int DMPlexGetTransitiveClosure(PetscDM,PetscInt,PetscBool,PetscInt*,PetscInt*[])
    int DMPlexRestoreTransitiveClosure(PetscDM,PetscInt,PetscBool,PetscInt*,PetscInt*[])
    int DMPlexVecGetClosure(PetscDM,PetscSection,PetscVec,PetscInt,PetscInt*,PetscScalar*[])
    int DMPlexVecRestoreClosure(PetscDM,PetscSection,PetscVec,PetscInt,PetscInt*,PetscScalar*[])

    int DMPlexGenerate(PetscDM,const_char[],PetscBool ,PetscDM*)
    #int DMPlexCopyCoordinates(PetscDM,PetscDM)
    #int DMPlexCopyLabels(PetscDM,PetscDM)
    #int DMPlexCreateDoublet(MPI_Comm,PetscInt,PetscBool,PetscBool,PetscBool,PetscReal,PetscDM*)
    int DMPlexCreateSquareBoundary(PetscDM,const_PetscReal[],const_PetscReal[],const_PetscInt[])
    int DMPlexCreateCubeBoundary(PetscDM,const_PetscReal[],const_PetscReal[],const_PetscInt[])
    int DMPlexCreateBoxMesh(MPI_Comm,PetscInt,PetscBool,PetscDM*)
    int DMPlexCreateHexBoxMesh(MPI_Comm,PetscInt,const_PetscInt[],PetscDMBoundaryType,PetscDMBoundaryType,PetscDMBoundaryType,PetscDM*)
    int DMPlexCreateCGNS(MPI_Comm,PetscInt,PetscBool,PetscDM*)
    int DMPlexCreateCGNSFromFile(MPI_Comm,const_char[],PetscBool,PetscDM*)
    int DMPlexCreateExodus(MPI_Comm,PetscInt,PetscBool,PetscDM*)
    int DMPlexCreateExodusFromFile(MPI_Comm,const_char[],PetscBool,PetscDM*)
    int DMPlexCreateGmsh(MPI_Comm,PetscViewer,PetscBool,PetscDM*)

    #int DMPlexCreateConeSection(PetscDM,PetscSection*)
    #int DMPlexInvertCell(PetscInt,PetscInt,int[])
    #int DMPlexCheckSymmetry(PetscDM)
    #int DMPlexCheckSkeleton(PetscDM,PetscBool,PetscInt)
    #int DMPlexCheckFaces(PetscDM,PetscBool,PetscInt)

    int DMPlexSetAdjacencyUseCone(PetscDM,PetscBool)
    int DMPlexSetAdjacencyUseClosure(PetscDM,PetscBool)
    #int DMPlexCreateNeighborCSR(PetscDM,PetscInt,PetscInt*,PetscInt**,PetscInt**)
    #int DMPlexCreatePartition(PetscDM,const_char[],PetscInt,PetscBool,PetscSection*,PetscIS*,PetscSection*,PetscIS*)
    #int DMPlexCreatePartitionClosure(PetscDM,PetscSection,PetscIS,PetscSection*,PetscIS*)
    int DMPlexDistribute(PetscDM,const_char[],PetscInt,PetscSF*,PetscDM*)
    #int DMPlexDistributeField(PetscDM,PetscSF,PetscSection,Vec,PetscSection,Vec)
    #int DMPlexDistributeData(PetscDM,PetscSF,PetscSection,MPI_Datatype,void*,PetscSection,void**)

    int DMPlexGetOrdering(PetscDM,PetscMatOrderingType,PetscIS*)
    int DMPlexPermute(PetscDM,PetscIS,PetscDM*)

    #int DMPlexCreateSubmesh(PetscDM,PetscDMLabel,PetscInt,PetscDM*)
    #int DMPlexCreateHybridMesh(PetscDM,PetscDMLabel,PetscDMLabel*,PetscDM*)
    #int DMPlexGetSubpointMap(PetscDM,PetscDMLabel*)
    #int DMPlexSetSubpointMap(PetscDM,PetscDMLabel)
    #int DMPlexCreateSubpointIS(PetscDM,PetscIS*)

    int DMPlexCreateCoarsePointIS(PetscDM,PetscIS*)
    int DMPlexMarkBoundaryFaces(PetscDM,PetscDMLabel)
    #int DMPlexLabelComplete(PetscDM,PetscDMLabel)
    #int DMPlexLabelCohesiveComplete(PetscDM,PetscDMLabel,PetscBool,PetscDM)

    int DMPlexGetRefinementLimit(PetscDM,PetscReal*)
    int DMPlexSetRefinementLimit(PetscDM,PetscReal)
    int DMPlexGetRefinementUniform(PetscDM,PetscBool*)
    int DMPlexSetRefinementUniform(PetscDM,PetscBool)

    #int DMPlexGetNumFaceVertices(PetscDM,PetscInt,PetscInt,PetscInt*)
    #int DMPlexGetOrientedFace(PetscDM,PetscInt,PetscInt,const_PetscInt[],PetscInt,PetscInt[],PetscInt[],PetscInt[],PetscBool*)

    int DMPlexCreateSection(PetscDM,PetscInt,PetscInt,const_PetscInt[],const_PetscInt[],PetscInt,const_PetscInt[],const_PetscIS[],PetscIS,PetscSection*)

    int DMPlexComputeCellGeometryFVM(PetscDM,PetscInt,PetscReal*,PetscReal[],PetscReal[])
