' Copyright 2010-2011 Bruce A Henderson
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
	Function bmx_barcode_result_getResultPoints:Float[](handle:Byte Ptr)

	Function bmx_barcode_multiFormatReader_create:Byte Ptr()
	Function bmx_barcode_multiformatreader_free(handle:Byte Ptr)
	Function bmx_barcode_multiFormatReader_decode:Byte Ptr(handle:Byte Ptr, pixels:Byte Ptr, width:Int, height:Int, hints:Int)
	
End Extern


Const BarcodeFormat_None:Int = 0
Const BarcodeFormat_QR_CODE:Int = 1
Const BarcodeFormat_DATA_MATRIX:Int = 2
Const BarcodeFormat_UPC_E:Int = 3
Const BarcodeFormat_UPC_A:Int = 4
Const BarcodeFormat_EAN_8:Int = 5
Const BarcodeFormat_EAN_13:Int = 6
Const BarcodeFormat_CODE_128:Int = 7
Const BarcodeFormat_CODE_39:Int = 8
Const BarcodeFormat_ITF:Int = 9

Const BARCODEFORMAT_QR_CODE_HINT:Int = 1 Shl BarcodeFormat_QR_CODE
Const BARCODEFORMAT_DATA_MATRIX_HINT:Int = 1 Shl BarcodeFormat_DATA_MATRIX
Const BARCODEFORMAT_UPC_E_HINT:Int = 1 Shl BarcodeFormat_UPC_E
Const BARCODEFORMAT_UPC_A_HINT:Int = 1 Shl BarcodeFormat_UPC_A
Const BARCODEFORMAT_EAN_8_HINT:Int = 1 Shl BarcodeFormat_EAN_8
Const BARCODEFORMAT_EAN_13_HINT:Int = 1 Shl BarcodeFormat_EAN_13
Const BARCODEFORMAT_CODE_128_HINT:Int = 1 Shl BarcodeFormat_CODE_128
Const BARCODEFORMAT_CODE_39_HINT:Int = 1 Shl BarcodeFormat_CODE_39
Const BARCODEFORMAT_ITF_HINT:Int = 1 Shl BarcodeFormat_ITF
Const TRYHARDER_HINT:Int = 1 Shl 31

Const DEFAULT_HINT:Int = BARCODEFORMAT_UPC_E_HINT | ..
    BARCODEFORMAT_UPC_A_HINT |..
    BARCODEFORMAT_EAN_8_HINT |..
    BARCODEFORMAT_EAN_13_HINT |..
    BARCODEFORMAT_CODE_128_HINT |..
    BARCODEFORMAT_CODE_39_HINT |..
    BARCODEFORMAT_ITF_HINT |..
    BARCODEFORMAT_DATA_MATRIX_HINT |..
    BARCODEFORMAT_QR_CODE_HINT
