CC=gcc
C_FLAGS=-std=c2x -Wall -fsanitize=address -Wpedantic -Werror -O2

EXECUTABLE=bin/main

OBJECT_DIR=src/obj
LOCAL_LIB_DIR=src/lib
MAIN_DIR=src/main

MAIN_FILE=main.c

MAIN_OBJ=main.o

LOCAL_LIB_SOURCE_FILES=chess.c

OBJECT_FILES=${LOCAL_LIB_SOURCE_FILES:%.c=${OBJECT_DIR}/%.o} ${OBJECT_DIR}/${MAIN_OBJ}

all: build

# Compiles relevant c source file found in lib into the obj file.
${OBJECT_DIR}/%.o: ${LOCAL_LIB_DIR}/%.c
	${CC} -c $^ -o $@


${OBJECT_DIR}/${MAIN_OBJ}: ${MAIN_DIR}/${MAIN_FILE}
	${CC} -c $^ -o $@

# build
${EXECUTABLE}: ${OBJECT_FILES}
	${CC} ${C_FLAGS} -o $@ $^ 


build: ${EXECUTABLE}

run: build
	${EXECUTABLE}

clean:
	rm -f ${EXECUTABLE} ${OBJECT_DIR}/*.o