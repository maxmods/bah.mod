' Copyright (c) 2019 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
'
SuperStrict

Import "source.bmx"

Extern

	Function BrotliEncoderCreateInstance:Byte Ptr(allocFunc:Byte Ptr, freeFunc:Byte Ptr, opaque:Object)
	Function BrotliEncoderDestroyInstance(state:Byte Ptr)
?bmxng
	Function BrotliEncoderMaxCompressedSize:Size_T(inputSize:Size_T)
	Function bmx_brotli_BrotliEncoderCompress:Int(quality:Int, lgwin:Int, encoderMode:Int, inputSize:Size_T, inputBuffer:Byte Ptr, encodedSize:Size_T Var, encodedBuffer:Byte Ptr) 

	Function bmx_brotli_BrotliDecoderDecompress:Int(encodedSize:Size_T , encodedBuffer:Byte Ptr, decodedSize:Size_T Var, decodedBuffer:Byte Ptr)
	Function BrotliEncoderSetParameter:Int(handle:Byte Ptr, param:Int, value:UInt)
?Not bmxng
	Function BrotliEncoderMaxCompressedSize:Int(inputSize:Int)
	Function bmx_brotli_BrotliEncoderCompress:Int(quality:Int, lgwin:Int, encoderMode:Int, inputSize:Int, inputBuffer:Byte Ptr, encodedSize:Int Var, encodedBuffer:Byte Ptr) 

	Function bmx_brotli_BrotliDecoderDecompress:Int(encodedSize:Int , encodedBuffer:Byte Ptr, decodedSize:Int Var, decodedBuffer:Byte Ptr)
	Function BrotliEncoderSetParameter:Int(handle:Byte Ptr, param:Int, value:Int)
?
	Function BrotliEncoderIsFinished:Int(handle:Byte Ptr)
	Function BrotliEncoderHasMoreOutput:Int(handle:Byte Ptr)
End Extern

Const BROTLI_MIN_WINDOW_BITS:Int = 10
Const BROTLI_MAX_WINDOW_BITS:Int = 24

Const BROTLI_MODE_GENERIC:Int = 0
Const BROTLI_MODE_TEXT:Int = 1
Const BROTLI_MODE_FONT:Int = 2

Const BROTLI_DEFAULT_QUALITY:Int = 11
Const BROTLI_DEFAULT_WINDOW:Int = 22
Const BROTLI_DEFAULT_MODE:Int = BROTLI_MODE_GENERIC

Rem
bbdoc: Processes input.
about: Encoder may postpone producing output, until it has processed enough input.
End Rem
Const BROTLI_OPERATION_PROCESS:Int = 0
Rem
bbdoc: Produces output for all processed input.
about:  Actual flush is performed when input stream is depleted and there is enough space in output stream.
This means that client should repeat #BROTLI_OPERATION_FLUSH operation until @availableIn becomes `0`, and 
#HasMoreOutput returns #False. If output is acquired via #TakeOutput, then operation should be repeated after
output buffer is drained.

When flush is complete, output data will be sufficient for decoder to reproduce all the given input.
End Rem
Const BROTLI_OPERATION_FLUSH:Int = 1
Rem
bbdoc: Finalizes the stream.
about: Actual finalization is performed when input stream is depleted and there is enough space in output stream.
This means that client should repeat #BROTLI_OPERATION_FINISH operation until @availableIn becomes `0`, and
#HasMoreOutput returns #False. If output is acquired via #TakeOutput, then operation should be repeated after
output buffer is drained.

Helper function #IsFinished checks if stream is finalized and output fully dumped.

Adding more input data to finalized stream is impossible.
End Rem
Const BROTLI_OPERATION_FINISH:Int = 2
Rem
bbdoc: Emits metadata block to stream.
about: Metadata is opaque to Brotli: neither encoder, nor decoder processes this data or relies on it.
It may be used to pass some extra information from encoder client to decoder client without interfering with main data stream.

> Encoder may emit empty metadata blocks internally, to pad encoded stream to byte boundary.

The whole content of input buffer is considered to be the content of metadata block.
Do NOT append metadata to input stream, before it is depleted with other operations.
End Rem
Const BROTLI_OPERATION_EMIT_METADATA:Int = 3

Rem
bbdoc: Tunes encoder for specific input.
about: Brotli Encoder Mode enumerates all available values.
End Rem
Const BROTLI_PARAM_MODE:Int = 0
Rem
bbdoc: The main compression speed-density lever.
about: The higher the quality, the slower the compression. Range is from #BROTLI_MIN_QUALITY to #BROTLI_MAX_QUALITY.
End Rem
Const BROTLI_PARAM_QUALITY:Int = 1
Rem
bbdoc: Recommended sliding LZ77 window size.
about: Encoder may reduce this value, e.g. if input is much smaller than window size.
 
Window size is `(1 << value) - 16`.

Range is from #BROTLI_MIN_WINDOW_BITS to #BROTLI_MAX_WINDOW_BITS.
End Rem
Const BROTLI_PARAM_LGWIN:Int = 2
Rem
bbdoc: Recommended input block size.
about: Encoder may reduce this value, e.g. if input is much smaller than input block size.

Range is from #BROTLI_MIN_INPUT_BLOCK_BITS to #BROTLI_MAX_INPUT_BLOCK_BITS.

> Bigger input block size allows better compression, but consumes more memory. The rough formula of memory used for temporary input storage is `3 << lgBlock`.
End Rem
Const BROTLI_PARAM_LGBLOCK:Int = 3
Rem
bbdoc: 
End Rem
Const BROTLI_PARAM_DISABLE_LITERAL_CONTEXT_MODELING:Int = 4
Rem
bbdoc: 
End Rem
Const BROTLI_PARAM_SIZE_HINT:Int = 5
Rem
bbdoc: 
End Rem
Const BROTLI_PARAM_LARGE_WINDOW:Int = 6
Rem
bbdoc: 
End Rem
Const BROTLI_PARAM_NPOSTFIX:Int = 7
Rem
bbdoc: 
End Rem
Const BROTLI_PARAM_NDIRECT:Int = 8
Rem
bbdoc: 
End Rem
Const BROTLI_PARAM_STREAM_OFFSET:Int = 9

