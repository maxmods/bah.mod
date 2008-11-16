SuperStrict

Framework BaH.Flickcurl
Import BRL.System
Import BRL.StandardIO

InitFlickcurl

Local fc:TFlickcurl = New TFlickcurl

fc.SetAPIKey("xxxxxxxxxxxx")
fc.SetSharedSecret("xxxxxxxxxx")
fc.SetAuthToken(fc.GetFullToken("xxxxxxxxxxx")) ' frob

Local photo:TFCPhoto = fc.GetPhotoInfo("123456789") ' photo ID

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
	
	Print photo.GetURI()
	
	OpenURL("http://www.flickr.com/photos/thaggards/123456789/")
	
	Rem
	  For(i=0; i < photo->tags_count; i++) {
	    flickcurl_tag* tag=photo->tags[i];
	    fprintf(stderr,
	            "%d) %s tag: id %s author ID %s name %s raw '%s' cooked '%s' count %d\n",
	            i, (tag->machine_tag ? "machine" : "regular"),
	            tag->id, tag->author, 
	            (tag->authorname ? tag->authorname : "(Unknown)"), 
	            tag->raw, tag->cooked,
	            tag->count);
	  }
	
	  flickcurl_free_photo(photo);
	  flickcurl_free(fc);
	  flickcurl_finish(); /* optional static free of resources */
	
	End Rem
	
	'photo.Free()
	
End If
fc.Free()
FinishFlickcurl