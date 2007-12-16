
Strict

NoDebug

' customized for OSX
Module BaH.AppStub

ModuleInfo "Version: 1.02"
ModuleInfo "Authors: Mark Sibly, Simon Armstrong, Bruce A Henderson"
ModuleInfo "License: Blitz Shared Source Code"
ModuleInfo "Copyright: Blitz Research Ltd"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.02"
ModuleInfo "History: Added SetFrontProcess code."
ModuleInfo "History: 1.00"
ModuleInfo "History: Customized for Mac - for wxWidgets goodness!"
ModuleInfo "History: 1.20 BRL Release"
ModuleInfo "History: Official BRL release"

?Debug
Import "debugger.stdio.bmx"
?

?MacOS
Import "appstub.macos.m"
Import "-framework Cocoa"
Import "-framework Carbon"
?

Extern
Function _bb_main()
End Extern

_bb_main
