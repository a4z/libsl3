
BUILDDIR=buildLLVMDebug

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

A4ZDIR=$(dirname $(pwd))/a4z

if [ -d $BUILDDIR ] ; then
 rm -r $BUILDDIR/*
else
 mkdir $BUILDDIR
fi


cd $BUILDDIR && cmake ../ -DCMAKE_BUILD_TYPE=Debug \
-DUSE_BOOSTTESTING_DYNLINK=ON


