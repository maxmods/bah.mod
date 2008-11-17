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

	Local comments:TFCCommentList = photo.GetCommentList()
	
	If comments Then
	
		Local i:Int
		Local comment:TFCComment = comments.GetComment(i)
		
		While comment
		
			Print i + ") " + comment.text
		
			i:+ 1
			comment = comments.GetComment(i)
		Wend
	
	End If
	
	photo.Free()
	
End If

fc.Free()
FinishFlickcurl


