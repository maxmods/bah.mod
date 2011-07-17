SuperStrict


Rem
bbdoc: OSMesa Library
about: Imports the library. Required separately for linking order.
End Rem
Module BaH.OSMesa

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2011 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release"

?linux

ModuleInfo "LD_OPTS: -L/usr/lib/mesa"
Import "-lOSMesa"

?
