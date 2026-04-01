#!/bin/bash

../../../build/Butcher -lc -a -m -i"0x409404,0x45a2534,0x409950" -n"/home/alex/src/butcher/samples/libffi-6.txt" "/home/alex/src/butcher/samples/libffi-6.dll" "0x045A35E4" > test.c
patch test.c test5.patch
patch test.c getmem.patch
patch test.c freemem.patch
patch test.c realloc.patch
patch test.c localtime.patch
patch test.c getcpinfo.patch
