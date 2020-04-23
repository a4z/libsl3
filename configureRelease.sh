
BUILDDIR=buildRelease

A4ZDIR=$(dirname $(pwd))/a4z

if [ -d $BUILDDIR ] ; then
 rm -r $BUILDDIR/*
else
 mkdir $BUILDDIR
fi



cd $BUILDDIR && cmake ../ -DCMAKE_BUILD_TYPE=Release -Dsl3_BUILD_TESTS=ON

