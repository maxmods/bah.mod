SuperStrict

Module BaH.Expat

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: Wrapper - 2007 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.00 Initial Release"
ModuleInfo "History: Includes Expat 2.01 source."

ModuleInfo "CC_OPTS: -DHAVE_EXPAT_CONFIG_H"
?ppc
ModuleInfo "CC_OPTS: -DWORDS_BIGENDIAN"
?

' expat_config.h
' added ifdef WORDS_BIGENDIAN test to set BYTEORDER definition

Import "common.bmx"
