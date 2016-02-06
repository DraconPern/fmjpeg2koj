#!/bin/sh
#TYPE=Release
TYPE=Debug

# a top level directory for all PACS related code
DEVSPACE=`pwd`

SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk
MACOSX_DEPLOYMENT_TARGET=10.9

cd $DEVSPACE/dcmtk
mkdir build-$TYPE
cd build-$TYPE
cmake .. -DDCMTK_WIDE_CHAR_FILE_IO_FUNCTIONS=1 -DCMAKE_INSTALL_PREFIX=$DEVSPACE/dcmtk/$TYPE -DCMAKE_OSX_SYSROOT=$SYSROOT -DCMAKE_OSX_DEPLOYMENT_TARGET=$MACOSX_DEPLOYMENT_TARGET
make -j8 install
cd ../..

cd $DEVSPACE/openjpeg
mkdir build-$TYPE
cd build-$TYPE
cmake .. -DBUILD_SHARED_LIBS=OFF -DBUILD_THIRDPARTY=ON -DCMAKE_INSTALL_PREFIX=$DEVSPACE/openjpeg/$TYPE
make -j8 install
cd ../..

cd $DEVSPACE
mkdir build-$TYPE
cd build-$TYPE
cmake .. -DOPENJPEG=$DEVSPACE/openjpeg/$TYPE -DDCMTK_DIR=$DEVSPACE/dcmtk/$TYPE -DCMAKE_INSTALL_PREFIX=$DEVSPACE/$TYPE
make -j8 install
cd ../..

echo "If you are getting a fchmodat error, please modify boost/libs/filessystem/src/operations.cpp.  Find the call to fchmodat and disable the #if using '#if 0 &&"
echo "If you are getting macosx-version-min error, in boost/tools/build/src/tools/darwin.jam after feature macosx-version-min add a new line"
echo "feature.extend macosx-version-min : 10.9 ;"
