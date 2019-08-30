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

Rem
bbdoc: Brotli compression.
End Rem
Module BaH.Brotli

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: Wrapper - 2019 Bruce A Henderson"

ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release. brotli 1.0.7.5c3a9a9"

Import "common.bmx"

Rem
bbdoc: A Brotli compressor.
End Rem
Type TBrotliCompressor

	Field statePtr:Byte Ptr

	Rem
	bbdoc: Performs one-shot memory-to-memory compression.
	returns: #True on success, #False in case of compression error or if output buffer is too small.
	about: Compresses the data in @inputBuffer into @encodedBuffer, and sets @encodedSize to the compressed length.
	> If #MaxCompressedSize returns non-zero value, then output is guaranteed to be no longer than that.
	End Rem
?bmxng
	Function Compress:Int(inputSize:Size_T, inputBuffer:Byte Ptr, encodedSize:Size_T Var, encodedBuffer:Byte Ptr, quality:Int = BROTLI_DEFAULT_QUALITY, lgwin:Int = BROTLI_DEFAULT_WINDOW, encoderMode:Int = BROTLI_DEFAULT_MODE)
?Not bmxng
	Function Compress:Int(inputSize:Int, inputBuffer:Byte Ptr, encodedSize:Int Var, encodedBuffer:Byte Ptr, quality:Int = BROTLI_DEFAULT_QUALITY, lgwin:Int = BROTLI_DEFAULT_WINDOW, encoderMode:Int = BROTLI_DEFAULT_MODE)
?
		Return bmx_brotli_BrotliEncoderCompress(quality, lgwin, encoderMode, inputSize, inputBuffer, encodedSize, encodedBuffer) 
	End Function

	Rem
	bbdoc: Calculates the output size bound for the given @inputSize.
	about: Result is only valid if quality is at least `2` and, in case #CompressStream was used, no flushes (#BROTLI_OPERATION_FLUSH) were performed.
	End Rem
?bmxng
	Function MaxCompressedSize:Size_T(inputSize:Size_T)
?Not bmxng
	Function MaxCompressedSize:Int(inputSize:Int)
?
		Return BrotliEncoderMaxCompressedSize(inputSize)
	End Function
	
	Rem
	bbdoc: Creates an instance of #TBrotliCompressor and initializes it.
	End Rem
	Method Create:TBrotliCompressor()
		statePtr = BrotliEncoderCreateInstance(Null, Null, Null)
		Return Self
	End Method

	Rem
	bbdoc: Sets the specified parameter.
	returns: #True if value is accepted.
	End Rem
?bmxng
	Method SetParameter:Int(param:Int, value:UInt)
?Not bmxng
	Method SetParameter:Int(param:Int, value:Int)
?
		Return BrotliEncoderSetParameter(statePtr, param, value)
	End Method

	Rem
	bbdoc: Checks if encoder instance reached the final state.
	returns: #True if encoder is in a state where it reached the end of the input and produced all of the output, #False otherwise.
	End Rem
	Method IsFinished:Int()
		Return BrotliEncoderIsFinished(statePtr)
	End Method
	
	Rem
	bbdoc: Checks if encoder has more output.
	returns: #True if encoder has some unconsumed output, #False otherwise.
	End Rem
	Method HasMoreOutput:Int()
		Return BrotliEncoderHasMoreOutput(statePtr)
	End Method
	
	Rem
	bbdoc: Deinitializes and frees #TBrotliCompressor instance.
	End Rem
	Method Free()
		If statePtr Then
			BrotliEncoderDestroyInstance(statePtr)
			statePtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: A Brotli decompressor
End Rem
Type TBrotliDecompressor

	Rem
	bbdoc: Performs one-shot memory-to-memory decompression.
	about: Decompresses the data in @encodedBuffer into @decodedBuffer, and sets @decodedSize to the decompressed length.
	End Rem
?bmxng
	Function Decompress:Int(encodedSize:Size_T, encodedBuffer:Byte Ptr, decodedSize:Size_T Var, decodedBuffer:Byte Ptr)
?Not bmxng
	Function Decompress:Int(encodedSize:Int, encodedBuffer:Byte Ptr, decodedSize:Int Var, decodedBuffer:Byte Ptr)
?
		Return bmx_brotli_BrotliDecoderDecompress(encodedSize, encodedBuffer, decodedSize, decodedBuffer)
	End Function

End Type
