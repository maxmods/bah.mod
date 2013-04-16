SuperStrict


Framework BaH.LibArchive
Import BRL.StandardIO


Local archive:TReadArchive = New TReadArchive.Create()
Local entry:TArchiveEntry = New TArchiveEntry.Create()

Local stream:TSStream = BaH.SStream.ReadStream("verne.tar")


archive.SupportFilterAll()
archive.SupportFormatAll()

'DebugStop

Local result:Int = archive.OpenStream(stream)

If result <> ARCHIVE_OK Then
	End
End If

While archive.ReadNextHeader(entry) = ARCHIVE_OK
	Print entry.Pathname()
	archive.DataSkip()
Wend

result = archive.Free()


