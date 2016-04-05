#!/bin/bash

LLVM_DIR=$HOME/LLVM/llvm-3.4.2/advcompiler
LLVM_SRC_DIR=$LLVM_DIR/llvm-3.4.2.src.git
CLANG_SRC_DIR=$LLVM_SRC_DIR/tools/clang

# Directory where the source files are located
SEARCH_DIR="$LLVM_SRC_DIR/include $LLVM_SRC_DIR/lib $CLANG_SRC_DIR/include $CLANG_SRC_DIR/lib"

# Directory where the resulting files will be located
DEST_DIR=$LLVM_DIR

cd $DEST_DIR
rm -rf cscope.out cscope.files
find $SEARCH_DIR  \( -name '*.c' -o -name '*.h' -o -name '*.cpp' \) -print > cscope.files
cscope -b
