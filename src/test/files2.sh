#!/bin/bash

../../build/Butcher "/home/alex/src/butcher/samples/bash" "0x05CAE0" > test.c
ln -s ../emu/x64.c x64.c
ln -s ../emu/x64.h x64.h

