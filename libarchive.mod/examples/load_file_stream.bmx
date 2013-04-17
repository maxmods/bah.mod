'
' extract file from zip as stream
'
SuperStrict

Framework BaH.LibArchive
Import BaH.TextSStream
Import BRL.StandardIO


Local archive:TReadArchive = New TReadArchive.Create()
Local entry:TArchiveEntry = New TArchiveEntry.Create()


archive.SupportFilterAll()
archive.SupportFormatAll()

Local result:Int = archive.OpenFilename("verne.zip")

If result <> ARCHIVE_OK Then
	End
End If

While archive.ReadNextHeader(entry) = ARCHIVE_OK
	Print entry.Pathname()
	If entry.Pathname() = "verne/center_of_the_earth.txt" Then
		Local text:String = BaH.TextSStream.LoadText(archive.DataStream())
		Print "  text = " + text.length + " chars"
	Else
		archive.DataSkip()
	End If
Wend

result = archive.Free()


