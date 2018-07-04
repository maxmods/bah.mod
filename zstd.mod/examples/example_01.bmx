SuperStrict

Framework brl.standardio
Import bah.zstd

Local data:Byte[] = LoadByteArray("warandpeace.txt")
Local dst:Byte Ptr = MemAlloc(Size_T(data.length))

Print data.length

Local compressor:TZstdCompress = New TZstdCompress

Local size:Size_T = compressor.Compress(dst, Size_T(data.length), data, Size_T(data.length), 20)

Print size

Local stream:TStream = WriteStream("warandpeace.zst")
stream.WriteBytes(dst, size)
stream.Close()

MemFree(dst)
