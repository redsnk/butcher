#!/bin/bash

../../../build/Butcher -lc -a -n"/home/alex/src/butcher/samples/libffi-6.txt" "/home/alex/src/butcher/samples/libffi-6.dll" "0x044FC7AC" > test.c
patch test.c test4.patch
