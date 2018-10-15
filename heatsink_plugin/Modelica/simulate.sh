#!/bin/sh
make || exit 1
../../bin/3D-ICE-Emulator pluggable_heatsink_test.stk
