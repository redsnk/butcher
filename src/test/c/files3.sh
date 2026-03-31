#!/bin/bash

../../../build/Butcher -lc -a -n"/home/alex/src/butcher/samples/libffi-6.txt" -m "/home/alex/src/butcher/samples/libffi-6.dll" "0x044FCB68" > test.c
patch test.c getmem.patch
patch test.c freemem.patch
