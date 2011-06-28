SuperStrict

Import BRL.Blitz

?win32
Import "-lqrencode"
?

Import "src/*.h"
Import "glue.cpp"


Extern

	Function bmx_qrcode_getdata:Byte Ptr(handle:Byte Ptr)
	Function bmx_qrcode_getwidth:Int(handle:Byte Ptr)
	Function bmx_qrcode_getversion:Int(handle:Byte Ptr)
	Function bmx_qrcode_free(handle:Byte Ptr)

	Function bmx_qrcode_encodestring:Byte Ptr(text:String, version:Int, level:Int, casesensitive:Int)
	
End Extern



Const QR_MODE_NUL:Int = -1      ' Terminator (NUL character). Internal use only
Const QR_MODE_NUM:Int = 0       ' Numeric mode
Const QR_MODE_AN:Int = 1        ' Alphabet-numeric mode
Const QR_MODE_8:Int = 2         ' 8-bit data mode
Const QR_MODE_KANJI:Int = 3     ' Kanji (shift-jis) mode
Const QR_MODE_STRUCTURE:Int = 4 ' Internal use only

Const QR_ECLEVEL_L:Int = 0 ' lowest
Const QR_ECLEVEL_M:Int = 1
Const QR_ECLEVEL_Q:Int = 2
Const QR_ECLEVEL_H:Int = 3 ' highest

