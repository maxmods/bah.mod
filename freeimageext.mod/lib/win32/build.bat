ECHO OFF
SET _LIB=libFIExt.dll
SET _DIR=%cd%
DEL %_LIB%
cd ..\..\..\freeimage.mod\src\LibRawLite

g++ -shared -m32 -DLIBRAW_NODLL -I . -I internal -I libraw -o "%_DIR%/%_LIB%" internal/dcraw_common.cpp internal/dcraw_fileio.cpp src/libraw_c_api.cpp src/libraw_cxx.cpp -lws2_32 

cd %_DIR%

dlltool -d libFIExt.def -l libFIExt.a -D libFIExt.dll
