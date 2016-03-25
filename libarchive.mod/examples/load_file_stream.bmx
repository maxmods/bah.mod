'
' extract file from zip as stream
'
SuperStrict

Framework BaH.LibArchive
?bmxng
Import BRL.TextStream
?Not bmxng
Import BaH.TextSStream
?
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
?bmxng
		Local Text:String = LoadText(archive.DataStream())
?Not bmxng
		Local Text:String = BaH.TextSStream.LoadText(archive.DataStream())
?
		Print "  text = " + Text.length + " chars"
	Else
		archive.DataSkip()
	End If
Wend

archive.Free()


