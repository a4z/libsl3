if [ -z "$1" ] ; then
    RDIR=$(pwd)
else
    RDIR=$(readlink -f $1)
fi


BUILDDIR=buildCodeCov


if [ -d $BUILDDIR ] ; then
 rm -r $BUILDDIR/*
else
 mkdir $BUILDDIR
fi

if [ -f $RDIR/configureCustomFlags ]; then
        . $RDIR/configureCustomFlags
fi


cd $BUILDDIR && cmake ../ -DCMAKE_BUILD_TYPE=Debug \
-DCODECOVERAGE=ON \
$CUSTOM_CMAKE_FLAG



