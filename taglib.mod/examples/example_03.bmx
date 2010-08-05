'
' Show framelists, tags and properties for 
'
SuperStrict

Framework BaH.TagLib
Import BRL.StandardIO
Import BRL.FileSystem
Import brl.glmax2d
Import brl.bankstream
Import bah.freeimage

' path to an mp3...
Local path:String = "C:/000_programming/blitz/music/01 - Our moon.mp3"

Graphics 800, 600, 0

Global images:TList = New TList


ShowTagInfo(path)

' something to display?
If Not images.IsEmpty() Then

	While Not KeyHit(key_escape)
		Cls
		
		Local x:Int = 0, y:Int = 0, yoff:Int = 0
		
		For Local image:TImage = EachIn images
			DrawImage image, x, y
			
			x:+ image.width + 5
			yoff = Max(yoff, image.height)
			
			If x > GraphicsWidth() Then
				y:+ yoff + 5
				yoff = 0
			End If
		Next
		
		
		Flip
	Wend

End If

End


Function ShowTagInfo(filename:String)

	Local file:TTLMPEGFile = New TTLMPEGFile.Create(filename)
	
	If file.isValid() Then

		Print "******************** ~q" + filename + "~q ********************"

		Local id3v2tag:TTLID3v2Tag = file.ID3v2Tag()
		
		If id3v2tag Then
			Print "ID3v2." + id3v2tag.header().majorVersion() + "." + ..
				id3v2tag.header().revisionNumber() + ", " + id3v2tag.header().tagSize() + " bytes in tag"

			For Local frame:TTLID3v2Frame = EachIn id3v2tag.frameList()
				Print frame.frameID().toString() + " - ~q" + frame.toString() + "~q"
				
				If TTLID3v2AttachedPictureFrame(frame) Then

					' retrieve the picture data
					Local data:TTLByteVector = TTLID3v2AttachedPictureFrame(frame).picture()

					If data Then
						' make a bankified version of the data
						Local bank:TBank = data.bank()
						If bank Then
							' load into a pixmap
							Local pixmap:TPixmap = LoadPixmap(CreateBankStream(bank))
							If pixmap Then
								Print "   w = " + pixmap.width + " : h = " + pixmap.height
								' create an image
								images.AddLast(LoadImage(pixmap))
							End If
						End If
					
					End If
				End If
			Next

		Else
			Print "file does not have a valid id3v2 tag"
		End If

		Print "~nID3v1"
		
		Local id3v1tag:TTLID3v1Tag = file.ID3v1Tag()
		
		If id3v1tag Then
			Print "title   - ~q" + id3v1tag.title()   + "~q"
			Print "artist  - ~q" + id3v1tag.artist()  + "~q"
			Print "album   - ~q" + id3v1tag.album()   + "~q"
			Print "year    - ~q" + id3v1tag.year()    + "~q"
			Print "comment - ~q" + id3v1tag.comment() + "~q"
			Print "track   - ~q" + id3v1tag.track()   + "~q"
			Print "genre   - ~q" + id3v1tag.genre()   + "~q"
		Else
			Print "file does not have a valid id3v1 tag"

		End If
		
		Print "~nAPE"
		
		Local ape:TTLAPETag = file.APETag()
		
		If ape Then
			' TODO
		Else
			Print "file does not have a valid APE tag"
		End If

		Print "~n"

		' free the file
		file.Free()
	End If

End Function


