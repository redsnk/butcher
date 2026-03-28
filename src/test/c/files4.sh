#!/bin/bash

#../../../build/Butcher -lc -a -m "/home/alex/src/butcher/samples/libffi-6.dll" "0x044FCAF8" > test.c
../../../build/Butcher -lc -a -m -n"/home/alex/src/butcher/samples/libffi-6.txt" "/home/alex/src/butcher/samples/libffi-6.dll" "0x044FC7AC" > test.c
patch test.c test4.patch
