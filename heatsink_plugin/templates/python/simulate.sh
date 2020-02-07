#!/bin/bash
cd ../../..
make plugin || exit 1
cd -
../../../bin/3D-ICE-Emulator pluggable_heatsink_test.stk
