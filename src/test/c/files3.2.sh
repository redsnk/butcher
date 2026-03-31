#!/bin/bash

../../../build/Butcher -lc -a -i"0x409404,0x68e348" -n"/home/alex/src/butcher/samples/libffi-6.txt" -m "/home/alex/src/butcher/samples/libffi-6.dll" "0x044FCAF8" > test.c
patch test.c getmem.patch
patch test.c freemem.patch
patch test.c realloc.patch
