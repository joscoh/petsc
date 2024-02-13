CC :=ccomp
CFLAGS :=-flongdouble -fstruct-passing
INC := -I include-verify -I ../include -I ../arch-linux-c-debug/include
C_SOURCES := $(shell find  -name '*.c')
C_EXECUTABLE :=$(C_SOURCES:.c=)

SRC := ${PETSC_DIR}/src
OBJ := build
CLIGHT := clight
BLAS := blas-verified

SOURCES := $(shell find src -name '*_verified.txt' -print0 | xargs -0 -I {} ./get_c_src.sh {})
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

BLASC := $(shell find $(BLAS) -name '*.c')
BLASO :=$(patsubst $(BLAS)/%.c, $(BLAS)/%.o, $(BLASC))

CLIGHTS := $(patsubst $(SRC)/%.c, $(CLIGHT)/%.v, $(SOURCES))

#Part 1: build and run the code

all: blas compile jacobi

run: jacobi.o
	./jacobi.o -skip_petscrc -no_signal_handler -viewfromoptions false -info ~sys -options_left false

$(OBJ)/%.o: $(SRC)/%.c
	./cpp_stub.sh $< && ./get_i_src.sh $< | xargs -0 -I {} $(CC) $(CFLAGS) $(INC) -c {} -o $@

$(BLAS)/%.o: $(BLAS)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

blas: $(BLASO)

compile: $(OBJECTS)

jacobi: stub_error.c jacobi.c
	$(CC) $(CFLAGS) -D PETSC_VERIFIED $(INC) $(BLASO) $(OBJECTS) $^ -o jacobi.o -lm

#Part 2: clightgen
$(CLIGHT)/%.v: $(SRC)/%.c
	mkdir -p $(@D) && ./get_i_src.sh $< | xargs -0 -I {} clightgen -normalize $(CFLAGS) {} -o $(CLIGHT)/$*.v  

clight: $(CLIGHTS) 

#Part 3: Clean

clean: clean-code clean-clight

clean-code:
	rm -rf build && rm -f jacobi.o

clean-clight:
	rm -rf clight