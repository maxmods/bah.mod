#!/bin/ksh
export _LIB=libFIExt.dylib
export _DIR=`pwd`
rm $_LIB 2>/dev/null
cd ../../../freeimage.mod/src/LibRawLite

g++ -shared -fPIC -m32 -I . -I internal -I libraw -o $_DIR/$_LIB internal/dcraw_common.cpp internal/dcraw_fileio.cpp src/libraw_c_api.cpp src/libraw_cxx.cpp

cd $_DIR

install_name_tool -id @executable_path/$_LIB $_LIB
