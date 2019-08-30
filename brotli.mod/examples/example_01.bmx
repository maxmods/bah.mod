SuperStrict

Framework brl.standardio
Import bah.brotli

Local data:Byte[] = LoadByteArray("warandpeace.txt")

Local maxSize:Size_T = TBrotliCompressor.MaxCompressedSize(Size_T(data.length))
Local size:Size_T = maxSize

Local comp:Byte[size]

' compress

If TBrotliCompressor.Compress(Size_T(data.length), data, size, comp) Then
	Print "original     : " + data.length
	Print "compressed   : " + size
Else
	Print "Failed to compress"
	End
End If

Local decomp:Byte[data.length]
size = decomp.length

' decompress

If TBrotliDecompressor.Decompress(Size_T(comp.length), comp, size, decomp) Then
	Print "decompressed : " + size
Else
	Print "Failed to decompress"
End If
