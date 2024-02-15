Verifiable Subset of PETSc

To generate the stubified files (and compile them with CompCert), run `make compile`.

To compile all needed files, including `jacobi.c`, run `make`.

To run the resulting binary `jacobi.o` (with the supported options), run `make run`.

`make clight` generates the clight code for this verifiable subset.

Running `diff.sh` (from the main PETSc directory) generates stub files for the current working directory and the previous commit, calls `compare.py` to compare these, and returns a non-zero error code (with an error message) if the verifiable subset has changed.


Individual components:

`stubify`: binary for Stubify tool, found at https://github.com/joscoh/stubify

`ccp_stub.sh`: a wrapper around `stubify` that preprocesses and stubifies the C file, creating the build directories

`get_c_src.sh` finds the C source file corresponding to a function name list

`get_i_src.sh` finds the stubified, preprocessed file corresponding to a C source file
