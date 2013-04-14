SuperStrict


Framework BaH.LibArchive
Import BRL.Standardio
Import BRL.RamStream

Incbin "verne.tar.gz"


Local archive:TReadArchive = New TReadArchive.Create()
Local entry:TArchiveEntry = New TArchiveEntry.Create()


archive.SupportFilterGZip()
archive.SupportFormatTar()

Local result:Int = archive.OpenMemory(IncbinPtr("verne.tar.gz"), IncbinLen("verne.tar.gz"))

If result <> ARCHIVE_OK Then
	End
End If

While archive.ReadNextHeader(entry) = ARCHIVE_OK
	Print entry.Pathname()
	archive.DataSkip()
Wend

result = archive.Free()

