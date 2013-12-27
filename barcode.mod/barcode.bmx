' Copyright 2010-2013 Bruce A Henderson
'
' Licensed under the Apache License, Version 2.0 (the "License");
' you may not use this file except in compliance with the License.
' You may obtain a copy of the License at
'
'     http://www.apache.org/licenses/LICENSE-2.0
'
' Unless required by applicable law or agreed to in writing, software
' distributed under the License is distributed on an "AS IS" BASIS,
' WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
' See the License for the specific language governing permissions and
' limitations under the License.
'
SuperStrict

Rem
bbdoc: Barcode Reader
End Rem
Module BaH.Barcode

ModuleInfo "Version: 1.01"
ModuleInfo "License: Apache 2.0"
ModuleInfo "Copyright: ZXing - 2013 ZXing authors All rights reserved"
ModuleInfo "Copyright: Wrapper - 2010-2013 Bruce A Henderson"

ModuleInfo "History: 1.01"
ModuleInfo "History: Update to ZXIng 2.3.0."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. (ZXing - SVN rev 1633)"

ModuleInfo "CC_OPTS: -fexceptions"
?win32
ModuleInfo "CC_OPTS: -DZXING_ICONV_CONST"
?

' Changes
'   ZXing.h
'       Added __GNUC__ check
'
'   aztec/decoder/Decoder.cpp
'       Added cast for const char**

Import "common.bmx"


Private

Global _reader:TBCMultiFormatReader = TBCMultiFormatReader.Create()

Public

Rem
bbdoc: Decode a barcode contained within a pixmap.
about: Check TBCResult's error field for validity.
End Rem	
Function decodeBarcode:TBCResult(pixmap:TPixmap)
	If pixmap Then
		Return _reader.decode(pixmap)
	Else
		Local res:TBCResult = New TBCResult
		res.error = "Null Pixmap"
		Return res
	End If
End Function


Type TBCMultiFormatReader

	Field readerPtr:Byte Ptr

	Function Create:TBCMultiFormatReader()
		Local this:TBCMultiFormatReader = New TBCMultiFormatReader
		this.readerPtr = bmx_barcode_multiFormatReader_create()
		Return this
	End Function
	
	Method decode:TBCResult(pixmap:TPixmap, hints:Int = DEFAULT_HINT)

		Local pixels:Byte[] = New Byte[pixmap.width * pixmap.height]
		Local p:Int = 0
		
		Local bpp:Int = BytesPerPixel[pixmap.format]
		Local r:Int, g:Int, b:Int
		
		For Local y:Int = 0 Until pixmap.height
			
			Local pixPtr:Byte Ptr = pixmap.pixels + pixmap.pitch * y
		
			For Local x:Int = 0 Until pixmap.width
			
				Select pixmap.format
				Case PF_RGB888, PF_RGBA8888
					r = pixPtr[0]
					g = pixPtr[1]
					b = pixPtr[2]
				Case PF_BGR888, PF_BGRA8888
					r = pixPtr[2]
					g = pixPtr[1]
					b = pixPtr[0]
				End Select

				pixels[p] = (306 * r + 601 * g + 117 * b) Shr 10
				
				pixPtr:+ bpp
				p:+ 1
			
			Next
		Next
		
		Try
			Return TBCResult._create(bmx_barcode_multiFormatReader_decode(readerPtr, pixels, pixmap.width, pixmap.height, hints))
		Catch ex:String
			Local res:TBCResult = New TBCResult
			res.error = ex
			Return res
		End Try

	End Method

	Method Delete()
		If readerPtr Then
			bmx_barcode_multiformatreader_free(readerPtr)
			readerPtr = Null
		End If
	End Method
	
End Type


Rem
bbdoc: A barcode decoding result.
End Rem	
Type TBCResult

	Field resultPtr:Byte Ptr
	
	Rem
	bbdoc: Error text, if applicable.
	End Rem
	Field error:String
	
	Function _create:TBCResult(resultPtr:Byte Ptr)
		If resultPtr Then
			Local this:TBCResult = New TBCResult
			this.resultPtr = resultPtr
			Return this
		End If
	End Function

	Rem
	bbdoc: Returns the barcode text, if decoded successfully.
	End Rem
	Method getText:String()
		If resultPtr Then
			Return bmx_barcode_result_getText(resultPtr)
		End If
	End Method
	
	Rem
	bbdoc: Returns the barcode format.
	about: One of BarcodeFormat_None, BarcodeFormat_QR_CODE, BarcodeFormat_UPC_E, BarcodeFormat_UPC_A, BarcodeFormat_EAN_8, 
	BarcodeFormat_EAN_13, BarcodeFormat_CODE_128, BarcodeFormat_CODE_39 or BarcodeFormat_ITF.
	End Rem
	Method getBarcodeFormat:Int()
		If resultPtr Then
			Return bmx_barcode_result_getBarcodeFormat(resultPtr)
		Else
			Return BarcodeFormat_None
		End If
	End Method
	
	Rem
	bbdoc: Returns a list of points for the detected bounding box around the barcode.
	End Rem
	Method getPoints:Float[]()
		If resultPtr Then
			Return bmx_barcode_result_getResultPoints(resultPtr)
		End If
	End Method
	
	Method Delete()
		If resultPtr Then
			bmx_barcode_result_free(resultPtr)
			resultPtr = Null
		End If
	End Method
	
End Type
