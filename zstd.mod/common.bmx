' Copyright (c) 2018-2021 Bruce A Henderson
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

Import "source.bmx"


Extern
	Function ZSTD_createCCtx:Byte Ptr()
	Function ZSTD_createDCtx:Byte Ptr()
?bmxng
	Function ZSTD_compress:Size_T(dst:Byte Ptr, dstCapacity:Size_T, src:Byte Ptr, srcSize:Size_T, compressionLevel:Int)
	Function ZSTD_decompress:Size_T(dst:Byte Ptr, dstCapacity:Size_T, src:Byte Ptr, compressedSize:Size_T)
	Function ZSTD_freeCCtx:Size_T(handle:Byte Ptr)
	Function ZSTD_compressCCtx:Size_T(handle:Byte Ptr, dst:Byte Ptr, dstCapacity:Size_T, src:Byte Ptr, srcSize:Size_T, compressionLevel:Int)
	Function ZSTD_freeDCtx:Size_T(handle:Byte Ptr)
	Function ZSTD_decompressDCtx:Size_T(handle:Byte Ptr, dst:Byte Ptr, dstCapacity:Size_T, src:Byte Ptr, compressedSize:Size_T)
	Function ZSTD_compressBound:Size_T(srcSize:Size_T)
	Function ZSTD_isError:Int(code:Size_T)
	Function ZSTD_getErrorName$Z(code:Size_T)
	
	Function ZSTD_createCDict:Byte Ptr(dictBuffer:Byte Ptr, dictSize:Size_T, compressionLevel:Int)
	Function ZSTD_freeCDict:Size_T(handle:Byte Ptr)
	Function ZSTD_compress_usingCDict:Size_T(handle:Byte Ptr, dst:Byte Ptr, dstCapacity:Size_T, src:Byte Ptr, srcSize:Size_T, cdict:Byte Ptr)
	Function ZSTD_createDDict:Byte Ptr(dictBuffer:Byte Ptr, dictSize:Size_T)
	Function ZSTD_freeDDict:Size_T(handle:Byte Ptr)
	Function ZSTD_decompress_usingDDict:Size_T(handle:Byte Ptr, dst:Byte Ptr, dstCapacity:Size_T, src:Byte Ptr, compressedSize:Size_T, ddict:Byte Ptr)
?Not bmxng
	Function ZSTD_compress:Int(dst:Byte Ptr, dstCapacity:Int, src:Byte Ptr, srcSize:Int, compressionLevel:Int)
	Function ZSTD_decompress:Int(dst:Byte Ptr, dstCapacity:Int, src:Byte Ptr, compressedSize:Int)
	Function ZSTD_freeCCtx:Int(handle:Byte Ptr)
	Function ZSTD_compressCCtx:Int(handle:Byte Ptr, dst:Byte Ptr, dstCapacity:Int, src:Byte Ptr, srcSize:Int, compressionLevel:Int)
	Function ZSTD_freeDCtx:Int(handle:Byte Ptr)
	Function ZSTD_decompressDCtx:Int(handle:Byte Ptr, dst:Byte Ptr, dstCapacity:Int, src:Byte Ptr, compressedSize:Int)
	Function ZSTD_compressBound:Int(srcSize:Int)
	Function ZSTD_isError:Int(code:Int)
	Function ZSTD_getErrorName$Z(code:Int)

	Function ZSTD_createCDict:Byte Ptr(dictBuffer:Byte Ptr, dictSize:Int, compressionLevel:Int)
	Function ZSTD_freeCDict:Size_T(handle:Byte Ptr)
	Function ZSTD_compress_usingCDict:Int(handle:Byte Ptr, dst:Byte Ptr, dstCapacity:Int, src:Byte Ptr, srcSize:Int, cdict:Byte Ptr)
	Function ZSTD_createDDict:Byte Ptr(dictBuffer:Byte Ptr, dictSize:Int)
	Function ZSTD_freeDDict:Int(handle:Byte Ptr)
	Function ZSTD_decompress_usingDDict:Int(handle:Byte Ptr, dst:Byte Ptr, dstCapacity:Int, src:Byte Ptr, compressedSize:Int, ddict:Byte Ptr)
?
End Extern
