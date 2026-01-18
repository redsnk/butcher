#!/bin/bash

../../build/Butcher "/home/alex/src/butcher/samples/api.node" 0x180002350 > test.c
ln -s ../emu/x64.c x64.c
ln -s ../emu/x64.h x64.h

