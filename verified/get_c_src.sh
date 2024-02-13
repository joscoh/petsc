#!/bin/bash

#given as input ${PETSC_DIR}/verified/path/foo_verified.txt
#returns ${PETSC_DIR}/src/path/foo.c

ARG=$1
ARGPATH=$(dirname "${ARG}")
PATH1=${ARGPATH#"${PETSC_DIR}/verified/"}
FILENAME=$(basename "${ARG}")
CNAME=${FILENAME%"_verified.txt"}

CFILE=${PETSC_DIR}/${PATH1}/${CNAME}.c

echo "$CFILE"