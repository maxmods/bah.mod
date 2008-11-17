SuperStrict

Framework BaH.Flickcurl
Import BRL.System
Import BRL.StandardIO

InitFlickcurl

Local fc:TFlickcurl = New TFlickcurl

fc.SetAPIKey("xxxxxxxxxxxx")
fc.SetSharedSecret("xxxxxxxxxx")
fc.SetAuthToken(fc.GetFullToken("xxxxxxxxxxx")) ' frob


Local photo:TFCPhoto = fc.GetPhotoInfo("983570868") ' photo ID

If photo Then

	For Local fieldType:Int = 0 To PHOTO_FIELD_LAST
	
		Local photoField:TFCPhotoField = photo.GetField(fieldType)
		Local datatype:Int = photoField.valueType
	    
		If datatype <> VALUE_TYPE_NONE Then
			Print "field " + TFCPhotoField.GetLabel(fieldType) + " (" + fieldType + ..
				") with " + TFCPhotoField.GetValueTypeLabel(datatype) + " value: '" + photoField.svalue + ..
				"' / " + photoField.ivalue
		End If
	Next
	
	For Local i:Int = 0 Until photo.GetTagCount()
		Local tag:TFCTag = photo.GetTag(i)
		
		Local s:String = i + ") " 
		
		If tag.GetMachineTag() Then
			s:+ "machine"
		Else
			S:+ "regular"
		End If
		
		s :+ " tag: id " + tag.GetID() + " author ID " + tag.GetAuthor() + " name "
		
		If tag.GetAuthorName() Then
			s:+ tag.GetAuthorName()
		Else
			s:+ "(Unknown)"
		End If
		
		s :+ "~n~t~traw '" + tag.GetRaw() + "' cooked '" + tag.GetCooked() + "' count " + tag.GetCount()

		Print s		
	
	Next	
	
	OpenURL(photo.GetSourceURI(FCIMAGE_MEDIUM))
	
	photo.Free()
	
End If

fc.Free()
FinishFlickcurl

