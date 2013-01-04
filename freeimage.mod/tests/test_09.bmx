' TEST 9
'
' Load a RAW image using FIF_LOAD_NOPIXELS.
'
SuperStrict

Framework BaH.FreeImage
Import BRL.StandardIO


Local fif:Int = GetFormatFromFile("DSC_0347.NEF")

If FormatSupportsNoPixels(fif) Then
	Print "Format supports FIF_LOAD_NOPIXELS flag"
	
	' cool, let's try it !
	
	Local image:TFreeImage = LoadFreeImage("DSC_0347.NEF", FIF_LOAD_NOPIXELS)
	
	If image.hasPixels() Then
		Print "Hmmm, we loaded pixels!?"
	Else
		Print "Loaded without pixels!"

		' we got metadata ?
		Local models:String[] = ["FIMD_COMMENTS", "FIMD_EXIF_MAIN", "FIMD_EXIF_EXIF", "FIMD_EXIF_GPS", "FIMD_EXIF_MAKERNOTE", ..
			"FIMD_EXIF_INTEROP", "FIMD_IPTC", "FIMD_XMP", "FIMD_GEOTIFF", "FIMD_ANIMATION", "FIMD_CUSTOM"]

		Print "Available EXIF Data :"
		
		For Local i:Int = 0 Until 10
			Print "    " + models[i] + " - (" + image.getMetadataCount(i) + ")"
		Next

		Print " "
	End If
	
	' includes embedded thumbnail?
	Local thumb:TFreeImage = image.GetThumbnail()
	
	If thumb Then
		Print "Found thumbnail !"
	Else
		Print "No embedded thumbnail available"
	End If
	
Else
	Print "Format does NOT support FIF_LOAD_NOPIXELS flag"
End If

