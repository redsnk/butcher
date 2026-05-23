#!/bin/bash

../../../build/Butcher -lc -a -n"../../../samples/libffi-6.txt" -e"0x66f388" -u"0x48651fe,0x435ae1,0x4865590,0x488cd10,0x486536a" "../../../samples/libffi-6.dll" "0x044FC7AC" > test.c
bash ../../sed/c/32/patch.sh test.c
patch test.c test4.patch
