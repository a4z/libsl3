
if [ -z "$1" ] ; then
    RDIR=$(pwd)
else
    RDIR=$(readlink -f $1)
fi

BUILDDIR=buildDebug


if [ -d $BUILDDIR ] ; then
 rm -rf $BUILDDIR/*
else
 mkdir $BUILDDIR
fi



if [ -f $RDIR/configureCustomFlags ]; then
        . $RDIR/configureCustomFlags
fi

 
cd $BUILDDIR && cmake ../ -DCMAKE_BUILD_TYPE=Debug \
-DUSE_BOOSTTESTING_DYNLINK=ON \
$CUSTOM_CMAKE_FLAG


