' Copyright (c) 2018-2019 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
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

Rem
bbdoc: ZStandard Compression.
End Rem
Module BaH.zstd

Import "common.bmx"


Rem
bbdoc: 
End Rem
Type TZstdCompress

	Field zstdPtr:Byte Ptr
	
	Method New()
		zstdPtr = ZSTD_createCCtx()
	End Method

	Rem
	bbdoc: 
	End Rem
?bmxng
	Method Compress:Size_T(dst:Byte Ptr, dstCapacity:Size_T, src:Byte Ptr, srcSize:Size_T, compressionLevel:Int)
?Not bmxng
	Method Compress:Int(dst:Byte Ptr, dstCapacity:Int, src:Byte Ptr, srcSize:Int, compressionLevel:Int) 
?
		Return ZSTD_compressCCtx(zstdPtr, dst, dstCapacity, src, srcSize, compressionLevel)
	End Method
	
	Method CompressWithDict:Size_T(dst:Byte Ptr, dstCapacity:Size_T, src:Byte Ptr, srcSize:Size_T, dict:TZstdCDict)
		Return ZSTD_compress_usingCDict(zstdPtr, dst, dstCapacity, src, srcSize, dict.zstdPtr)
	End Method
	
	Method Free()
		If zstdPtr Then
			ZSTD_freeCCtx(zstdPtr)
			zstdPtr = Null
		End If
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TZstdDecompress

	Field zstdPtr:Byte Ptr

	Method New()
		zstdPtr = ZSTD_createDCtx()
	End Method
	
	Rem
	bbdoc: 
	End Rem
?bmxng
	Method Decompress:Size_T(dst:Byte Ptr, dstCapacity:Size_T, src:Byte Ptr, compressedSize:Size_T)
?Not bmxng
	Method Decompress:Int(dst:Byte Ptr, dstCapacity:Int, src:Byte Ptr, compressedSize:Int)
?
		Return ZSTD_decompressDCtx(zstdPtr, dst, dstCapacity, src, compressedSize)
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TZstdCDict

	Field zstdPtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TZstdCDict(dictBuffer:Byte Ptr, dictSize:Size_T, compressionLevel:Int)
		zstdPtr = ZSTD_createCDict(dictBuffer, dictSize, compressionLevel)
		Return Self
	End Method
	
	Method Free()
		If zstdPtr Then
			ZSTD_freeCDict(zstdPtr)
			zstdPtr = Null
		End If
	End Method

End Type
