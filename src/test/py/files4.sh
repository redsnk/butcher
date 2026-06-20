#!/bin/bash

../../../build/Butcher -lp -a -n"../../../samples/libffi-6.txt" -e"0x66f388" -u"0x48651fe,0x435ae1,0x4865590,0x488cd10,0x486536a" "../../../samples/libffi-6.dll" "0x044FC7AC" > test.py
bash ../../sed/py/32/patch.sh test.py
patch test.py test4.patch
python3 test.py
