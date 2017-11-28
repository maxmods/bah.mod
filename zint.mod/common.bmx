' Copyright (c) 2017 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of the project nor the names of its contributors
'       may be used to endorse or promote products derived from this software
'       without specific prior written permission. 
'
' THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import BaH.Cairo
Import "source.bmx"

Extern

	Function ZBarcode_Create:Byte Ptr()
	Function ZBarcode_Delete(handle:Byte Ptr)
	Function ZBarcode_Clear(handle:Byte Ptr)
	Function ZBarcode_Encode_and_Buffer:Int(handle:Byte Ptr, inp:Byte Ptr, length:Int, rotateAngle:Int)
	
	Function bmx_zint_setsymbology(handle:Byte Ptr, symbology:Int)
	Function bmx_zint_bitmapdimensions(handle:Byte Ptr, w:Int Ptr, h:Int Ptr)
	Function bmx_zint_bitmap:Byte Ptr(handle:Byte Ptr)
	Function bmx_zint_setscale(handle:Byte Ptr, scale:Float)
	Function bmx_zint_setwhitespacewidth(handle:Byte Ptr, width:Int)
	Function bmx_zint_setborderwidth(handle:Byte Ptr, width:Int)
	Function bmx_zint_setfgcolour(handle:Byte Ptr, r:Int, g:Int, b:Int)
	Function bmx_zint_setbgcolour(handle:Byte Ptr, r:Int, g:Int, b:Int)
	
	Function bmx_zint_cairoencode:Int(handle:Byte Ptr, ctx:Byte Ptr, inp:Byte Ptr, length:Int, rotateAngle:Int)
	
End Extern


Const BARCODE_CODE11:Int = 1
Const BARCODE_C25MATRIX:Int = 2
Const BARCODE_C25INTER:Int = 3
Const BARCODE_C25IATA:Int = 4
Const BARCODE_C25LOGIC:Int = 6
Const BARCODE_C25IND:Int = 7
Const BARCODE_CODE39:Int = 8
Const BARCODE_EXCODE39:Int = 9
Const BARCODE_EANX:Int = 13
Const BARCODE_EANX_CHK:Int = 14
Const BARCODE_EAN128:Int = 16
Const BARCODE_CODABAR:Int = 18
Const BARCODE_CODE128:Int = 20
Const BARCODE_DPLEIT:Int = 21
Const BARCODE_DPIDENT:Int = 22
Const BARCODE_CODE16K:Int = 23
Const BARCODE_CODE49:Int = 24
Const BARCODE_CODE93:Int = 25
Const BARCODE_FLAT:Int = 28
Const BARCODE_RSS14:Int = 29
Const BARCODE_RSS_LTD:Int = 30
Const BARCODE_RSS_EXP:Int = 31
Const BARCODE_TELEPEN:Int = 32
Const BARCODE_UPCA:Int = 34
Const BARCODE_UPCA_CHK:Int = 35
Const BARCODE_UPCE:Int = 37
Const BARCODE_UPCE_CHK:Int = 38
Const BARCODE_POSTNET:Int = 40
Const BARCODE_MSI_PLESSEY:Int = 47
Const BARCODE_FIM:Int = 49
Const BARCODE_LOGMARS:Int = 50
Const BARCODE_PHARMA:Int = 51
Const BARCODE_PZN:Int = 52
Const BARCODE_PHARMA_TWO:Int = 53
Const BARCODE_PDF417:Int = 55
Const BARCODE_PDF417TRUNC:Int = 56
Const BARCODE_MAXICODE:Int = 57
Const BARCODE_QRCODE:Int = 58
Const BARCODE_CODE128B:Int = 60
Const BARCODE_AUSPOST:Int = 63
Const BARCODE_AUSREPLY:Int = 66
Const BARCODE_AUSROUTE:Int = 67
Const BARCODE_AUSREDIRECT:Int = 68
Const BARCODE_ISBNX:Int = 69
Const BARCODE_RM4SCC:Int = 70
Const BARCODE_DATAMATRIX:Int = 71
Const BARCODE_EAN14:Int = 72
Const BARCODE_CODABLOCKF:Int = 74
Const BARCODE_NVE18:Int = 75
Const BARCODE_JAPANPOST:Int = 76
Const BARCODE_KOREAPOST:Int = 77
Const BARCODE_RSS14STACK:Int = 79
Const BARCODE_RSS14STACK_OMNI:Int = 80
Const BARCODE_RSS_EXPSTACK:Int = 81
Const BARCODE_PLANET:Int = 82
Const BARCODE_MICROPDF417:Int = 84
Const BARCODE_ONECODE:Int = 85
Const BARCODE_PLESSEY:Int = 86

' Tbarcode 8 codes
Const BARCODE_TELEPEN_NUM:Int = 87
Const BARCODE_ITF14:Int = 89
Const BARCODE_KIX:Int = 90
Const BARCODE_AZTEC:Int = 92
Const BARCODE_DAFT:Int = 93
Const BARCODE_MICROQR:Int = 97

' Tbarcode 9 codes
Const BARCODE_HIBC_128:Int = 98
Const BARCODE_HIBC_39:Int = 99
Const BARCODE_HIBC_DM:Int = 102
Const BARCODE_HIBC_QR:Int = 104
Const BARCODE_HIBC_PDF:Int = 106
Const BARCODE_HIBC_MICPDF:Int = 108
Const BARCODE_HIBC_BLOCKF:Int = 110
Const BARCODE_HIBC_AZTEC:Int = 112
    
' Tbarcode 10 codes
Const BARCODE_DOTCODE:Int = 115
Const BARCODE_HANXIN:Int = 116

' Zint specific
Const BARCODE_AZRUNE:Int = 128
Const BARCODE_CODE32:Int = 129
Const BARCODE_EANX_CC:Int = 130
Const BARCODE_EAN128_CC:Int = 131
Const BARCODE_RSS14_CC:Int = 132
Const BARCODE_RSS_LTD_CC:Int = 133
Const BARCODE_RSS_EXP_CC:Int = 134
Const BARCODE_UPCA_CC:Int = 135
Const BARCODE_UPCE_CC:Int = 136
Const BARCODE_RSS14STACK_CC:Int = 137
Const BARCODE_RSS14_OMNI_CC:Int = 138
Const BARCODE_RSS_EXPSTACK_CC:Int = 139
Const BARCODE_CHANNEL:Int = 140
Const BARCODE_CODEONE:Int = 141
Const BARCODE_GRIDMATRIX:Int = 142
Const BARCODE_UPNQR:Int = 143


' Warning and error conditions
Const ZINT_WARN_INVALID_OPTION:Int = 2
Const ZINT_WARN_USES_ECI:Int = 3
Const ZINT_ERROR_TOO_LONG:Int = 5
Const ZINT_ERROR_INVALID_DATA:Int = 6
Const ZINT_ERROR_INVALID_CHECK:Int = 7
Const ZINT_ERROR_INVALID_OPTION:Int = 8
Const ZINT_ERROR_ENCODING_PROBLEM:Int = 9
Const ZINT_ERROR_FILE_ACCESS:Int = 10
Const ZINT_ERROR_MEMORY:Int = 11

