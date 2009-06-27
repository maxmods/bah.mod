SuperStrict

Framework BaH.Magick
Import BRL.StandardIO

Try
	Local info:TMCoderInfo = TMCoderInfo.info("png")
	
	Print info.name
	Print info.description
	Print "readable  : " + info.isReadable
	Print "writable  : " + info.isWritable
	Print "multiFrame: " + info.isMultiFrame

Catch e:TMException

	Print e.message

End Try