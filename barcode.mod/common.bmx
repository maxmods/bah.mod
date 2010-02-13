' Copyright 2010 Bruce A Henderson
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

Import BRL.Pixmap
Import BaH.libiconv

Import "source.bmx"

Extern

	Function bmx_barcode_result_getText:String(handle:Byte Ptr)
	Function bmx_barcode_result_getBarcodeFormat:Int(handle:Byte Ptr)
	Function bmx_barcode_result_free(handle:Byte Ptr)

	Function bmx_barcode_multiFormatReader_create:Byte Ptr()
	Function bmx_barcode_multiformatreader_free(handle:Byte Ptr)
	Function bmx_barcode_multiFormatReader_decode:Byte Ptr(handle:Byte Ptr, pixels:Byte Ptr, width:Int, height:Int)
	
End Extern


Const BarcodeFormat_None:Int = 0
Const BarcodeFormat_QR_CODE:Int = 1
Const BarcodeFormat_UPC_E:Int = 2
Const BarcodeFormat_UPC_A:Int = 3
Const BarcodeFormat_EAN_8:Int = 4
Const BarcodeFormat_EAN_13:Int = 5
Const BarcodeFormat_CODE_128:Int = 6
Const BarcodeFormat_CODE_39:Int = 7
Const BarcodeFormat_ITF:Int = 8



