SuperStrict

Rem
bbdoc: A QR Code generator.
End Rem
Module BaH.qrencode

ModuleInfo "Version: 1.00"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: (qrencode) 2006-2010 Kentaro Fukuchi"
ModuleInfo "Copyright: (wrapper) 2011 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release"

?win32
ModuleInfo "LD_OPTS: -L%PWD%/lib/win32"
?

Import BRL.Pixmap
Import "common.bmx"


Rem
bbdoc: A QRCode symbol.
End Rem
Type TQRCode

	Field qrcodePtr:Byte Ptr
	
	Function _create:TQRCode(qrcodePtr:Byte Ptr)
		If qrcodePtr Then
			Local this:TQRCode = New TQRCode
			this.qrcodePtr = qrcodePtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Create a symbol from the string.
	about: Parameters: 
	<ul>
	<li><b>text</b> : xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx </li>
	<li><b>version</b> : Version of the symbol (0 - 40). If 0, the library chooses the minimum version for the given input data.</li>
	<li><b>level</b> : The error correction level. One of QR_ECLEVEL_L, QR_ECLEVEL_M, QR_ECLEVEL_Q or QR_ECLEVEL_H. </li>
	<li><b>caseSensitive</b> : True to keep the case, False to store as non-case text.</li>
	</ul>
	End Rem
	Function encodeString:TQRCode(text:String, version:Int = 0, level:Int = QR_ECLEVEL_H, caseSensitive:Int = False)
		Return _create(bmx_qrcode_encodestring(text, version, level, caseSensitive))
	End Function
	
	Rem
	bbdoc: Returns a QR Code for the specified email address.
	End Rem
	Function encodeEmail:TQRCode(address:String)
		Return encodeString("mailto:" + address)
	End Function
	
	Rem
	bbdoc: Returns a QR Code for the specified phone number.
	about: It is advisable to include prefixes that make the number accessible internationally, for example using +1 for numbers in the US, etc.
	End Rem
	Function encodeTel:TQRCode(number:String)
		Return encodeString("tel:" + number)
	End Function

	Rem
	bbdoc: Returns a QR Code for the specified sms shortcode or phone number.
	about: It is advisable to include prefixes for phone numbers that make the number accessible internationally, for example using +1 for numbers in the US, etc.
	End Rem
	Function encodeSms:TQRCode(number:String)
		Return encodeString("sms:" + number)
	End Function
	
	Rem
	bbdoc: Returns the symbol data that is represented as an array of width*width bytes.
	about: Each byte represents a module (dot). If the less significant bit of the uchar is 1, the corresponding module is black.
	The other bits are meaningless for usual applications, but here its specification is described.
	<pre>
	MSB 76543210 LSB
	    |||||||\- 1=black/0=white
	    ||||||\-- data and ecc code area
	    |||||\--- format information
	    ||||\---- version information
	    |||\----- timing pattern
	    ||\------ alignment pattern
	    |\------- finder pattern and separator
	    \-------- non-data modules (format, timing, etc.)
	</pre>
	End Rem
	Method getData:Byte Ptr()
		Return bmx_qrcode_getdata(qrcodePtr)
	End Method
	
	Rem
	bbdoc: Returns the width of the symbol, in modules (dots).
	End Rem
	Method getWidth:Int()
		Return bmx_qrcode_getwidth(qrcodePtr)
	End Method
	
	Rem
	bbdoc: Returns the version of the symbol.
	about: The symbol versions of QR Code range from Version 1 to Version 40.
	about: Each version has a different module configuration or number of modules. (The module refers to the black and white dots that make up QR Code.)<br>
	Each QR Code symbol version has the maximum data capacity according to the amount of data, character type and error correction level.
	Check the maximum data capacity for each version.
	<br>
	In other words, as the amount of data increases, more modules are required to comprise QR Code, resulting in larger QR Code symbols.
	End Rem
	Method getVersion:Int()
		Return bmx_qrcode_getversion(qrcodePtr)
	End Method
	
	Rem
	bbdoc: Returns a pixmap representation of the symbol, where @size is the pixel-square size of each module (dot).
	about: 
	End Rem
	Method getPixmap:TPixmap(size:Int)
		Local w:Int = getWidth()
		Local pix:TPixmap = TPixmap.Create(w * size, w * size, PF_I8)
		pix.ClearPixels(0)
		
		Local data:Byte Ptr = getData()
		
		For Local x:Int = 0 Until w
		
			Local xs:Int = x * size
		
			For Local y:Int = 0 Until w

				Local ys:Int = y * size
				
				Local bit:Int = data[x + y * w] & 1
				
				If bit Then
					For Local x0:Int = 0 Until size
						For Local y0:Int = 0 Until size
							pix.WritePixel(xs + x0, ys + y0, $ffffffff)
						Next
					Next
				End If
			Next
		Next
		
		Return pix
	End Method
	
	Method Free()
		If qrcodePtr Then
			bmx_qrcode_free(qrcodePtr)
			qrcodePtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type

