#!/bin/bash

../../build/Butcher > test.c
#cp ../emu/x64.* .
ln -s ../emu/x64.c x64.c
ln -s ../emu/x64.h x64.h

