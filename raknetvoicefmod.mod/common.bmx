SuperStrict

Import BaH.FMOD
Import BRL.Blitz

Import "../fmod.mod/include/*.hpp"
Import "../raknet.mod/src/*.h"
Import "../raknetvoice.mod/src/*.h"
Import "src/*.h"
Import "*.h"

Import "glue.cpp"
Import "src/FMODVoiceAdapter.cpp"

Extern

	Function bmx_raknetvoicefmod_adapter_new:Byte Ptr(system:Byte Ptr, rakVoice:Byte Ptr)
	Function bmx_raknetvoicefmod_adapter_update(handle:Byte Ptr)

End Extern

