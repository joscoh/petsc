#!/bin/bash

#Directory structure:
#List of files containing any verified/stubbed code: petsc/verified/verified_files.txt
#FILES=${PETSC_DIR}/verified/verified_files.txt

#List of functions to keep (all other stubbed) (TODO: may split up eventually)
#FUNCTIONS=${PETSC_DIR}/verified/verified_funcs.txt
#C file to preprocess
ARG=$1

PATH1=$(dirname "${ARG}")
ARGFILE=$(basename "${ARG}")
ARGPATH=${PATH1#"${PETSC_DIR}/"}

ARGNAME=${ARGFILE%.c}

#If given path/file.c, we want to find verified/path/file_verified.txt if exists
FUNCTIONS=${PETSC_DIR}/verified/${ARGPATH}/${ARGNAME}_verified.txt

if test -f "$FUNCTIONS"
then 
	#create build directory
	NEWPATH=${PETSC_DIR}/verified/build/${ARGPATH#"src/"}
	mkdir -p "${NEWPATH}"

	#create the full function list
	#(TODO: do in 1 command?)
	find "${PETSC_DIR}"/verified/include/ -name '*_verified.txt' -print0 | xargs -0 -I file cat file > "${PETSC_DIR}"/verified/include_verified.txt
	#add newline to end of include_verified (TODO: only create once per compile?)
	#sed -i -e '$a\' "${PETSC_DIR}"/verified/include_verified.txt
	cat "${PETSC_DIR}"/verified/include_verified.txt "$FUNCTIONS" > "${PETSC_DIR}"/verified/temp_verified.txt

	#invoke the modified preprocessor, output to build directory
	gcc -m64 -U__GNUC__ -U__SIZEOF_INT128__ -E -std=c99 -D__COMPCERT__ -D__COMPCERT_MAJOR__=3 \
	-D__COMPCERT_MINOR__=14 -D__COMPCERT_VERSION__=314 -U__STDC_IEC_559_COMPLEX__ \
	-D__STDC_NO_ATOMICS__ -D__STDC_NO_COMPLEX__ -D__STDC_NO_THREADS__ -D__STDC_NO_VLA__ \
	-D__COMPCERT_WCHAR_TYPE__=int -D PETSC_VERIFIED -I/usr/local/lib/compcert/include \
	-I "${PETSC_DIR}/verified/include-verify/" -I"${PETSC_DIR}"/include/ -I"${PETSC_DIR}"/"${PETSC_ARCH}"/include/ \
	"$ARG" | \
	"${PETSC_DIR}"/verified/stubify -std c11 -names "${PETSC_DIR}"/verified/temp_verified.txt > "${NEWPATH}"/"${ARGFILE%".c"}".i

	#clean up
	rm "${PETSC_DIR}"/verified/include_verified.txt
	rm "${PETSC_DIR}"/verified/temp_verified.txt

else
	#should not happen
	echo "SKIPPED" "$ARG"
	#Don't run anything else
	false
fi
