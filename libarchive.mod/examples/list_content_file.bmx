SuperStrict


Framework BaH.LibArchive
Import BRL.StandardIO


Local archive:TReadArchive = New TReadArchive.Create()
Local entry:TArchiveEntry = New TArchiveEntry.Create()


archive.SupportFilterAll()
archive.SupportFormatAll()

Local result:Int = archive.OpenFilename("verne.tar")

If result <> ARCHIVE_OK Then
	End
End If

While archive.ReadNextHeader(entry) = ARCHIVE_OK
	Print entry.Pathname()
	archive.DataSkip()
Wend

result = archive.Free()

