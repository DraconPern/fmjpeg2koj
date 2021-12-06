#!/bin/bash
set -xe

if [ "$1" == "Release" ]
then
  TYPE=Release
else
  TYPE=Debug
fi

BUILD_DIR=`pwd`
DEVSPACE=`pwd`

unamestr=`uname`

cd $DEVSPACE
[[ -d dcmtk ]] || git clone https://github.com/DCMTK/dcmtk.git
cd dcmtk
git fetch
git checkout -f DCMTK-3.6.5
mkdir -p build-$TYPE
cd build-$TYPE
cmake .. -DCMAKE_BUILD_TYPE=$TYPE -DDCMTK_ENABLE_CXX11=ON -DDCMTK_ENABLE_STL=ON -DDCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS=1 -DDCMTK_ENABLE_BUILTIN_DICTIONARY=1 -DDCMTK_WITH_TIFF=OFF -DDCMTK_WITH_PNG=OFF -DDCMTK_WITH_OPENSSL=OFF -DDCMTK_WITH_XML=OFF -DDCMTK_WITH_ZLIB=ON -DDCMTK_WITH_SNDFILE=OFF -DDCMTK_WITH_ICONV=ON -DDCMTK_WITH_WRAP=OFF -DCMAKE_INSTALL_PREFIX=$DEVSPACE/dcmtk/$TYPE
make -j8 install

cd $DEVSPACE
[[ -d openjpeg ]] || git clone --branch=v2.4.0 --single-branch --depth 1 https://github.com/uclouvain/openjpeg.git
cd openjpeg
git pull
mkdir -p build-$TYPE
cd build-$TYPE
cmake .. -DCMAKE_BUILD_TYPE=$TYPE -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=$DEVSPACE/openjpeg/$TYPE
make -j8 install

cd $BUILD_DIR
mkdir -p build-$TYPE
cd build-$TYPE
cmake .. -DCMAKE_BUILD_TYPE=$TYPE -DBUILD_SHARED_LIBS=OFF -DDCMTK_ROOT=$DEVSPACE/dcmtk/$TYPE -Dfmjpeg2k_ROOT=$DEVSPACE/fmjpeg2koj/$TYPE -DOpenJPEG_ROOT=$DEVSPACE/openjpeg/$TYPE
make -j8