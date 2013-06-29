
Strict

NoDebug

Rem
bbdoc: Remote Debug Appstub
End Rem
Module BaH.AppStubRdb

ModuleInfo "Version: 1.00"
ModuleInfo "Authors: Mark Sibly, Simon Armstrong, Bruce A Henderson"
ModuleInfo "License: zlib/libpng"
ModuleInfo "Copyright: Blitz Research Ltd"
ModuleInfo "Copyright: (TCP, Remote Debugging) Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release"
ModuleInfo "History: Based on BRL.Appstub 1.20"

?Debug
Import "debugger.tcp.bmx"	' TCP-based debugger
?

?MacOS
Import "appstub.macos.m"
Import "-framework Cocoa"
Import "-framework Carbon"
?Win32
Import "appstub.win32.c"
?Linux
Import "appstub.linux.c"
?

Extern
Function _bb_main()
End Extern

_bb_main
