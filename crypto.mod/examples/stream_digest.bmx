'
' Digests with String or TStream
'
SuperStrict

Framework BaH.Crypto
Import BRL.StandardIO

' the file we want to calculate a digest for
Local file:String = "some_text.txt"

Local stream:TStream = ReadStream(file)

' MD5 the stream
Print MD5(stream)

stream.close()

Local txt:String = LoadText(file)

' MD5 the text
Print MD5(txt)


