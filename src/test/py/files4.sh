#!/bin/bash

../../../build/Butcher -lp -a -m -n"/home/alex/src/butcher/samples/libffi-6.txt" "/home/alex/src/butcher/samples/libffi-6.dll" "0x044FC7AC" > test.py
patch test.py test4.patch
