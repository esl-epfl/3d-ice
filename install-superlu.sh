#!/bin/bash
# Install SuperLU

echo "Install SuperLU"
wget https://crd.lbl.gov/~xiaoye/SuperLU/superlu_4.3.tar.gz -O superlu_4.3.tar.gz
tar xvfz superlu_4.3.tar.gz
rm -rf superlu_4.3.tar.gz
cd SuperLU_4.3/
cp MAKE_INC/make.linux make.inc

sed -i -e '/^SuperLUroot/ s/$(HOME)\/Codes\/SuperLU_4.3/../' make.inc

##Use vendor blas
sed -i -e '/^BLASLIB/ s/-L\/usr\/lib/-L\/usr\/lib\/x86_64-linux-gnu/' make.inc

#Build SuperLU
make
