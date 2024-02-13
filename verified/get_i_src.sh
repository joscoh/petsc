#!/bin/bash

#given as input ${PETSC_DIR}/src/path/foo.c
#returns ${PETSC_DIR}/verified/build/path/foo.i

#given as input ${PETSC_DIR}/verified/path/foo_verified.txt
#returns ${PETSC_DIR}/src/path/foo.c

ARG=$1
ARGPATH=$(dirname "${ARG}")
PATH1=${ARGPATH#"${PETSC_DIR}/src/"}
PATH2=${PETSC_DIR}/verified/build/$PATH1
FILENAME=$(basename "${ARG}")
CNAME=${FILENAME%".c"}

IFILE=${PATH2}/${CNAME}.i

echo -n "$IFILE"