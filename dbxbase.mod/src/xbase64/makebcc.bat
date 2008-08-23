
rem 2/14/04 
rem This batch file builds the xbase64 library using Borland C++ 5.5
rem use -v for source level debugging

del *.bak
del *.obj

bcc32 -c -I.. -Id:\borland\bcc55\include xbdate.cpp    > compout
bcc32 -c -I.. -Id:\borland\bcc55\include xblock.cpp   >> compout
bcc32 -c -I.. -Id:\borland\bcc55\include xbdbf.cpp    >> compout
bcc32 -c -I.. -Id:\borland\bcc55\include xbexp.cpp    >> compout
bcc32 -c -I.. -Id:\borland\bcc55\include xbexpfnc.cpp >> compout
bcc32 -c -I.. -Id:\borland\bcc55\include xbexpprc.cpp >> compout
bcc32 -c -I.. -Id:\borland\bcc55\include xbfields.cpp >> compout
bcc32 -c -I.. -Id:\borland\bcc55\include xbindex.cpp  >> compout
bcc32 -c -I.. -Id:\borland\bcc55\include xbase64.cpp  >> compout
bcc32 -c -I.. -Id:\borland\bcc55\include xbmemo.cpp   >> compout
bcc32 -c -I.. -Id:\borland\bcc55\include xbstring.cpp >> compout
bcc32 -c -I.. -Id:\borland\bcc55\include xbfilter.cpp >> compout
bcc32 -c -I.. -Id:\borland\bcc55\include xbndx.cpp    >> compout
bcc32 -c -I.. -Id:\borland\bcc55\include xbntx.cpp    >> compout
bcc32 -c -I.. -Id:\borland\bcc55\include xbcdx.cpp    >> compout
bcc32 -c -I.. -Id:\borland\bcc55\include xbfile.cpp   >> compout


del xbase64.lib
tlib xbase64.lib /C +xbdbf.obj +xbexp.obj +xbexpfnc.obj      >> compout
tlib xbase64.lib /C +xbndx.obj +xbntx.obj +xbexpprc.obj      >> compout
tlib xbase64.lib /C +xbfields.obj +xbfile.obj +xbcdx.obj     >> compout
tlib xbase64.lib /C +xbindex.obj +xbfilter.obj +xbase64.obj  >> compout
tlib xbase64.lib /C +xbmemo.obj +xbdate.obj +xbstring.obj    >> compout
