#!/bin/bash
# Install 3d-ice

echo "Install SuperLU"
wget http://crd.lbl.gov/~xiaoye/SuperLU/superlu_4.3.tar.gz -O superlu_4.3.tar.gz
tar xvfz superlu_4.3.tar.gz
rm -rf superlu_4.3.tar.gz
cd SuperLU_4.3/
cp MAKE_INC/make.linux make.inc

sed -i -e '24 s/$(HOME)\/Codes\/SuperLU_4.3/../' make.inc

##Use blas provided by SuperLU
#sed -i -e '27 s/#//' make.inc
#sed -i -e '27 s/$(SuperLUroot)\/lib\/libblas.a/..\/lib\/blas$(PLAT).a/' make.inc
#sed -i -e '34,35 s/^/#/' make.inc
#make blaslib

##Use vendor blas
echo "Install blas and lapack"
apt-get install libblas-dev liblapack-dev
sed -i -e '35 s/-L\/usr\/lib/-L\/usr\/lib\/x86_64-linux-gnu/' make.inc


echo "Install flex"
apt-get install flex

echo "Install bison"
apt-get install bison

#Build SuperLU
make

cd ..

#Build 3d-ice
make


