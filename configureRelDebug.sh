
BUILDDIR=buildRelDebug

A4ZDIR=$(dirname $(pwd))/a4z

if [ -d $BUILDDIR ] ; then
 rm -r $BUILDDIR/*
else
 mkdir $BUILDDIR
fi



cd $BUILDDIR && cmake ../ -DCMAKE_BUILD_TYPE=RelWithDebInfo \
-DUSE_BOOSTTESTING_DYNLINK=ON 
 


