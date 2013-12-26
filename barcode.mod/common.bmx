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

Import BRL.Pixmap
Import BaH.libiconv
Import BaH.BigInt

Import "source.bmx"

Extern

	Function bmx_barcode_result_getText:String(handle:Byte Ptr)
	Function bmx_barcode_result_getBarcodeFormat:Int(handle:Byte Ptr)
	Function bmx_barcode_result_free(handle:Byte Ptr)
	Function bmx_barcode_result_getResultPoints:Float[](handle:Byte Ptr)

	Function bmx_barcode_multiFormatReader_create:Byte Ptr()
	Function bmx_barcode_multiformatreader_free(handle:Byte Ptr)
	Function bmx_barcode_multiFormatReader_decode:Byte Ptr(handle:Byte Ptr, pixels:Byte Ptr, width:Int, height:Int, hints:Int)
	
End Extern


Const BarcodeFormat_NONE:Int = 0
Const BarcodeFormat_AZTEC:Int = 1
Const BarcodeFormat_CODABAR:Int = 2
Const BarcodeFormat_CODE_39:Int = 3
Const BarcodeFormat_CODE_93:Int = 4
Const BarcodeFormat_CODE_128:Int = 5
Const BarcodeFormat_DATA_MATRIX:Int = 6
Const BarcodeFormat_EAN_8:Int = 7
Const BarcodeFormat_EAN_13:Int = 8
Const BarcodeFormat_ITF:Int = 9
Const BarcodeFormat_MAXICODE:Int = 10
Const BarcodeFormat_PDF_417:Int = 11
Const BarcodeFormat_QR_CODE:Int = 12
Const BarcodeFormat_RSS_14:Int = 13
Const BarcodeFormat_RSS_EXPANDED:Int = 14
Const BarcodeFormat_UPC_A:Int = 15
Const BarcodeFormat_UPC_E:Int = 16
Const BarcodeFormat_UPC_EAN_EXTENSION:Int = 16

Const BARCODEFORMAT_AZTEC_HINT:Int = 1 Shl BarcodeFormat_AZTEC
Const BARCODEFORMAT_CODABAR_HINT:Int = 1 Shl BarcodeFormat_CODABAR
Const BARCODEFORMAT_CODE_39_HINT:Int = 1 Shl BarcodeFormat_CODE_39
Const BARCODEFORMAT_CODE_93_HINT:Int = 1 Shl BarcodeFormat_CODE_93
Const BARCODEFORMAT_CODE_128_HINT:Int = 1 Shl BarcodeFormat_CODE_128
Const BARCODEFORMAT_DATA_MATRIX_HINT:Int = 1 Shl BarcodeFormat_DATA_MATRIX
Const BARCODEFORMAT_EAN_8_HINT:Int = 1 Shl BarcodeFormat_EAN_8
Const BARCODEFORMAT_EAN_13_HINT:Int = 1 Shl BarcodeFormat_EAN_13
Const BARCODEFORMAT_ITF_HINT:Int = 1 Shl BarcodeFormat_ITF
Const BARCODEFORMAT_MAXICODE_HINT:Int = 1 Shl BarcodeFormat_MAXICODE
Const BARCODEFORMAT_PDF_417_HINT:Int = 1 Shl BarcodeFormat_PDF_417
Const BARCODEFORMAT_QR_CODE_HINT:Int = 1 Shl BarcodeFormat_QR_CODE
Const BARCODEFORMAT_RSS_14_HINT:Int = 1 Shl BarcodeFormat_RSS_14
Const BARCODEFORMAT_RSS_EXPANDED_HINT:Int = 1 Shl BarcodeFormat_RSS_EXPANDED
Const BARCODEFORMAT_UPC_A_HINT:Int = 1 Shl BarcodeFormat_UPC_A
Const BARCODEFORMAT_UPC_E_HINT:Int = 1 Shl BarcodeFormat_UPC_E
Const BARCODEFORMAT_UPC_EAN_EXTENSION_HINT:Int = 1 Shl BarcodeFormat_UPC_EAN_EXTENSION
Const BARCODEFORMAT_ASSUME_GS1_HINT:Int = 1 Shl 27
Const BARCODEFORMAT_CHARACTER_SET_HINT:Int = 1 Shl 30
Const BARCODEFORMAT_TRYHARDER_HINT:Int = 1 Shl 31

Const PRODUCT_HINT:Int = BARCODEFORMAT_UPC_A_HINT | ..
  BARCODEFORMAT_UPC_E_HINT | ..
  BARCODEFORMAT_EAN_13_HINT | ..
  BARCODEFORMAT_EAN_8_HINT | ..
  BARCODEFORMAT_RSS_14_HINT

Const ONED_HINT:Int = BARCODEFORMAT_CODE_39_HINT | ..
	BARCODEFORMAT_CODE_93_HINT | ..
	BARCODEFORMAT_CODE_128_HINT | ..
	BARCODEFORMAT_ITF_HINT | ..
	BARCODEFORMAT_CODABAR_HINT | ..
	PRODUCT_HINT

Const DEFAULT_HINT:Int = ONED_HINT | ..
	BARCODEFORMAT_QR_CODE_HINT | ..
	BARCODEFORMAT_DATA_MATRIX_HINT | ..
	BARCODEFORMAT_AZTEC_HINT | ..
	BARCODEFORMAT_PDF_417_HINT
