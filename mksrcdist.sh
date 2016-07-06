#!/bin/sh

#get the version number from generated config file
version=$(grep -i MAJOR_VERSION include/sl3/config.hpp | tr -d -c '[0-9]').\
$(grep -i MINOR_VERSION include/sl3/config.hpp | tr -d -c '[0-9]').\
$(grep -i PATCH_VERSION include/sl3/config.hpp | tr -d -c '[0-9]')

#define tmp dir and archive name
tmpdir=/tmp
outname=../libsl3-${version}.tar.gz

#remove possible existings 
rm -rf ${outname} ${tmpdir}/libsl3-${version}

#copy wanted stuff into versioned dir in tmp 
rsync -a \
--exclude=*~ \
--exclude=.?* \
--exclude=*.sh \
--exclude=build* \
--exclude=configureCustomFlags* \
$(pwd)/*  ${tmpdir}/libsl3-${version} \

 #pack the stuff 
tar -C ${tmpdir} -cvzf ${outname} libsl3-${version}
 
