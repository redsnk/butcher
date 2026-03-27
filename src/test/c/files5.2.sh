#!/bin/bash

../../../build/Butcher -lc -a -m -e"0x045A2CA0" -n"/home/alex/src/butcher/samples/libffi-6.txt" "/home/alex/src/butcher/samples/libffi-6.dll" "0x045A35E4" > test.c
patch test.c getmem.patch
patch test.c freemem.patch
patch test.c realloc.patch
