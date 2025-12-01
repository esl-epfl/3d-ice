#!/bin/bash
# Install SuperLU

echo "Install SuperLU_MT"
unzip superlu_mt-4.0.0.zip
cd superlu_mt-4.0.0/
cp MAKE_INC/make.linux.openmp make.inc


# 1) Enable vendor BLAS
sed -i 's/^BLASDEF.*/BLASDEF   = -DUSE_VENDOR_BLAS/' make.inc

# 2) Point BLASLIB to OpenBLAS
#    (adjust path if your libopenblas.so lives somewhere else,
#     e.g. /usr/lib/x86_64-linux-gnu)
sed -i 's@^BLASLIB .*@BLASLIB = -L/lib/x86_64-linux-gnu -lopenblas@' make.inc

# 3) Fortran naming convention: routines have a trailing underscore
sed -i 's/^CDEFS.*/CDEFS        = -DAdd_/' make.inc

#Build SuperLU
make
