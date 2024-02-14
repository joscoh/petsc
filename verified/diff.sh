#!/bin/bash

#Must be run from within the main directory

#first, compile verified subset
cd verified
make compile

cd ..

#Now, get the last commit and compile

NEW_PETSC=$PETSC_DIR/temp

#Get the last commit, save in "temp"
mkdir -p temp
git archive HEAD --output=temp/old.tar
cd temp
tar -xf old.tar
cp -r ../arch-linux-c-debug/ ./arch-linux-c-debug/

cd verified

#Need to temporarily change environmental var PETSC_DIR
PETSC_DIR=$NEW_PETSC make compile

#Run python script to compare files
cd $PETSC_DIR/verified
python3 compare.py $PETSC_DIR/verified/build $NEW_PETSC/verified/build

exit_code=$?

#clean up - delete temp directories
cd $PETSC_DIR
rm -rf temp

exit "$exit_code"