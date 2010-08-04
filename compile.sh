#!/bin/sh

./build.py && ./llc $1 2> out.ll && llvm-as out.ll && llvm-ld out.bc
