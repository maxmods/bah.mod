' TEST 7
'
' Load a jpg, examine metadata
'
' Example image comes from the exif.org website.
' See this page for comparison of output to actual data: http://www.exif.org/samples/kodak-dc240.html
'
SuperStrict

Framework BaH.FreeImage
Import BRL.StandardIO


' Load the image
Local img:TFreeImage = LoadFreeImage("exif.jpg")

' did the image load?
If img Then
	DebugLog("Loaded ok.")
Else
	DebugLog("Couldn't load...")
	End
End If

Local models:String[] = ["FIMD_COMMENTS", "FIMD_EXIF_MAIN", "FIMD_EXIF_EXIF", "FIMD_EXIF_GPS", "FIMD_EXIF_MAKERNOTE", ..
	"FIMD_EXIF_INTEROP", "FIMD_IPTC", "FIMD_XMP", "FIMD_GEOTIFF", "FIMD_ANIMATION", "FIMD_CUSTOM"]

Print "EXIF Data :~n"

For Local i:Int = 0 Until 10

	Print models[i] + " - (" + img.getMetadataCount(i) + ")"
	
	If img.getMetadataCount(i) = 0 Then
		Print "~tn/a"
	Else

		For Local tag:TFreeImageTag = EachIn img.metadata(i)
			displayTag(tag)
			Print ""
		Next
		
	End If
	
	Print ""
	
Next

End

Function displayTag(tag:TFreeImageTag)

	Global types:String[] = ["FIDT_NOTYPE", "FIDT_BYTE", "FIDT_ASCII", "FIDT_SHORT", "FIDT_INT", "FIDT_RATIONAL", ..
		"FIDT_SBYTE", "FIDT_UNDEFINED", "FIDT_SSHORT", "FIDT_SINT", "FIDT_SRATIONAL", "FIDT_FLOAT", "FIDT_DOUBLE", ..
		"FIDT_IFD", "FIDT_PALETTE"]

	Print "~tKey         = " + tag.getKey()
	Print "~tDescription = " + tag.getDescription()
	Print "~tID          = " + tag.getID()
	Print "~tType        = " + types[tag.getType()]

	' show some tags	
	Select tag.getType()
		Case FIDT_ASCII
			Print "~tValue       = " + String.FromCString(tag.getValue())
		Case FIDT_RATIONAL, FIDT_SRATIONAL
			Local rat:Int Ptr = Int Ptr(tag.getValue())
			Print "~tValue       = " + rat[0] + " / " + rat[1]
	End Select
	
End Function


