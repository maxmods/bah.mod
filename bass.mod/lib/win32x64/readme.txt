; generate def file

gendef bass.dll


; generate .a

dlltool -k --output-lib libbass.a --def bass.def

