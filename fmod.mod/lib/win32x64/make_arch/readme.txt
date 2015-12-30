; generate def file

gendef fmodex64.dll

; strip out C++ functions

; generate .a

dlltool -k --output-lib libfmodex64.a --def fmodex64.def

