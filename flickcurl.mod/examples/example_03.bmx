'
' Show a screen-full of thumbnails for a given search
'
'
SuperStrict

Framework BaH.Flickcurl
Import BRL.System
Import BRL.StandardIO
Import BRL.GLMax2D
Import BRL.JPGLoader
Import BRL.HttpStream

InitFlickcurl

Local fc:TFlickcurl = New TFlickcurl

fc.SetAPIKey("xxxxxxxxxxxx")
fc.SetSharedSecret("xxxxxxxxxx")
fc.SetAuthToken(fc.GetFullToken("xxxxxxxxxxx")) ' frob

Local params:TFCSearchParams = New TFCSearchParams

params.SetTags("sunset,mountain")
params.SetTagMode("all")

Local list:TFCPhotoList = fc.SearchPhotos(params)

If list Then

	Local count:Int = 0
	Local total:Int = list.GetPhotoCount()

	Local maxHeight:Int = 0
	Local x:Int = 0
	Local y:Int = 0
	
	Local gw:Int = 800
	Local gh:Int = 600
	
	Graphics gw, gh, 0
	
	SetColor 255, 255, 255
	
	Cls
	Flip
	
	While count < total And y < gh And Not KeyHit(key_escape)
	
		
		Local photo:TFCPhoto = list.GetPhoto(count)

		Local uri:String = photo.GetSourceURI(FCIMAGE_THUMB).Replace("http://", "http::")

		Local pixmap:TPixmap = LoadPixmap(OpenFile(uri))
		
		Local w:Int = pixmap.width
		Local h:Int = pixmap.height
		
		If x + w > gw Then
			x = 0
			y :+ maxHeight + 5
			maxHeight = 0
		End If
		
		Flip
		DrawPixmap pixmap, x, y
		
		x:+ w + 2
		If h > maxHeight Then
			maxHeight = h
		End If
	
		Flip
	
		Delay(10)
		count:+ 1
	Wend
	
	WaitKey
	
	list.Free()
	
End If


fc.Free()
FinishFlickcurl



