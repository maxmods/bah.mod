
Strict

Module BaH.OggVorbis

ModuleInfo "Version: 1.07a"
ModuleInfo "License: OggVorbis License"
ModuleInfo "Copyright: Xiph.Org Foundation http://www.xiph.org/"
ModuleInfo "Credit: Adapted for BlitzMax by Simon Armstrong"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.07a"
ModuleInfo "History: Update to libogg-1.3.0 and libvorbis-1.2.3"
ModuleInfo "History: 1.07 Release"
ModuleInfo "History: Hacked around Lion llvm bug in libvorbis-1.1.2/lib/os.h"
ModuleInfo "History: 1.06 Release"
ModuleInfo "History: Moved Encode_OGG stub to axe.oggsaver module"
ModuleInfo "History: 1.05 Release"
ModuleInfo "History: Added Encode_OGG stub"
ModuleInfo "History: Updated to libogg-1.1.3 and libvorbis-1.2.3"

Import "libogg-1.3.0/include/*.h"
Import "libogg-1.3.0/src/bitwise.c"
Import "libogg-1.3.0/src/framing.c"

Import "libvorbis-1.2.3/include/*.h"
Import "libvorbis-1.2.3/lib/bitrate.c"
Import "libvorbis-1.2.3/lib/block.c"
Import "libvorbis-1.2.3/lib/codebook.c"
Import "libvorbis-1.2.3/lib/envelope.c"
Import "libvorbis-1.2.3/lib/floor0.c"
Import "libvorbis-1.2.3/lib/floor1.c"
Import "libvorbis-1.2.3/lib/info.c"
Import "libvorbis-1.2.3/lib/lpc.c"
Import "libvorbis-1.2.3/lib/lsp.c"
Import "libvorbis-1.2.3/lib/mapping0.c"
Import "libvorbis-1.2.3/lib/mdct.c"
Import "libvorbis-1.2.3/lib/psy.c"
Import "libvorbis-1.2.3/lib/registry.c"
Import "libvorbis-1.2.3/lib/res0.c"
Import "libvorbis-1.2.3/lib/sharedbook.c"
Import "libvorbis-1.2.3/lib/smallft.c"
Import "libvorbis-1.2.3/lib/synthesis.c"
Import "libvorbis-1.2.3/lib/vorbisfile.c"
Import "libvorbis-1.2.3/lib/window.c"
Import "libvorbis-1.2.3/lib/analysis.c"

Import "oggdecoder.c"

Extern

Function Decode_Ogg:Byte Ptr(..
	datasource:Object,..
	reado(buf@Ptr,size,nmemb,src:Object),..
	seeko(src:Object,off0,off1,whence),..
	closeo(src:Object),..
	tello(src:Object ),..
	samples Var,channels Var,freq Var)

Function Read_Ogg(ogg:Byte Ptr,buf:Byte Ptr,size)	'null buffer to close

End Extern
