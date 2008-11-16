SuperStrict

Rem
bbdoc: Flickcurl
End Rem
Module BaH.Flickcurl

ModuleInfo "Version: 1.00"
ModuleInfo "License: Apache 2.0"
ModuleInfo "Copyright: "
ModuleInfo "Copyright: Wrapper - 2008 Bruce A Henderson"

ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H -DFLICKCURL_STATIC"

Import "common.bmx"


Rem
bbdoc: Library initialization.
about: Call once before creating anything.
End Rem
Function InitFlickcurl:Int()
	Return flickcurl_init()
End Function

Rem
bbdoc: Library cleanup.
about: Call once before exit.
End Rem
Function FinishFlickcurl()
	flickcurl_finish()
End Function


Rem
bbdoc: 
End Rem
Type TFlickcurl

	Field fcPtr:Byte Ptr
	
	Method New()
		fcPtr = bmx_flickcurl_new()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If fcPtr Then
			bmx_flickcurl_free(fcPtr)
			fcPtr = Null
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetAPIKey(apiKey:String)
		bmx_flickcurl_setapikey(fcPtr, apiKey)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetAuthToken(authToken:String)
		bmx_flickcurl_setauthtoken(fcPtr, authToken)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SetSharedSecret(sharedSecret:String)
		bmx_flickcurl_setsharedsecret(fcPtr, sharedSecret)
	End Method
	
	
	Rem
	bbdoc: 
	End Rem
	Method GetPhotoInfo:TFCPhoto(photoID:String)
		Return TFCPhoto._create(bmx_flickcurl_photosgetinfo(fcPtr, photoID), fcPtr)
	End Method
	
	
	Method GetFrob:String()
		Return bmx_flickcurl_getfrob(fcPtr)
	End Method
	
	Method GetFullToken:String(frob:String)
		Return bmx_flickcurl_getfulltoken(fcPtr, frob)
	End Method
	
	
	Method Delete()
		Free()
	End Method

End Type


Rem
bbdoc: 
End Rem
Type TFCActivity

	Field activityPtr:Byte Ptr
	
	Method GetType:String()
	End Method
	
	Method GetOwner:String()
	End Method
	
	Method GetOwnerName:String()
	End Method
	
	Method GetPrimary:String()
	End Method
	
	Method GetID:String()
	End Method
	
	Method GetSecret:String()
	End Method
	
	Method GetServer:Int()
	End Method
	
	Method GetFarm:Int()
	End Method
	
	Method GetOldComments:Int()
	End Method
	
	Method GetNewComments:Int()
	End Method
	
	Method GetOldNotes:Int()
	End Method
	
	Method GetNewNotes:Int()
	End Method
	
	Method GetViews:Int()
	End Method
	
	Method GetComments:Int()
	End Method
	
	Method GetPhotos:Int()
	End Method
	
	Method GetFaves:Int()
	End Method
	
	Method GetMore:Int()
	End Method
	
	Method GetTitle:String()
	End Method
	
	Method GetEvents:TFCActivityEvent[]()
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TFCActivityEvent

	Field eventPtr:Byte Ptr
	
	Method GetType:String()
	End Method
	
	Method GetID:String()
	End Method
	
	Method GetUser:String()
	End Method
	
	Method GetUsername:String()
	End Method
	
	Method GetValue:String()
	End Method
	
	Method GetDateAdded:Int()
	End Method

End Type



Rem
bbdoc: 
End Rem
Type TFCComment

	Field commentPtr:Byte Ptr
	
	Method GetID:String()
	End Method
	
	Method GetAuthor:String()
	End Method
	
	Method GetAuthorName:String()
	End Method
	
	Method GetDateCreated:Int()
	End Method
	
	Method GetPermalink:String()
	End Method
	
	Method GetText:String()
	End Method

End Type


Type TFCPhoto

	Field photoPtr:Byte Ptr
	
	Field fcPtr:Byte Ptr
	
	Function _create:TFCPhoto(photoPtr:Byte Ptr, fcPtr:Byte Ptr)
		If photoPtr Then
			Local this:TFCPhoto = New TFCPhoto
			this.photoPtr = photoPtr
			this.fcPtr = fcPtr
			Return this
		End If
	End Function
	
	Method GetID:String()
	End Method
	
	Method GetURI:String()
		Return bmx_flickcurl_photo_geturi(photoPtr)
	End Method
	
	Method GetTags:TFCTag[]()
	End Method
	
	Method GetField:TFCPhotoField(index:Int)
		Return TFCPhotoField(bmx_flickcurl_photo_getfield(photoPtr, index))
	End Method
	
	Method GetPlace:TFCPlace()
	End Method
	
	Method GetVideo:TFCVideo()
	End Method
	
	Method GetMediaType:String()
	End Method
	
End Type

Type TFCTag
End Type

Type TFCPhotoField

	Function _create:TFCPhotoField(svalue:String, ivalue:Int, valueType:Int)
		Local this:TFCPhotoField = New TFCPhotoField
		this.svalue = svalue
		this.ivalue = ivalue
		this.valueType = valueType
		Return this
	End Function

	Field svalue:String
	Field ivalue:Int
	Field valueType:Int
	
	Function GetLabel:String(fieldType:Int)
		Return bmx_flickcurl_photofield_getlabel(fieldType)
	End Function
	
	Function GetValueTypeLabel:String(valueType:Int)
		Return bmx_flickcurl_photofield_getvaluetypelabel(valueType)
	End Function
	
	
End Type

Type TFCPlace
End Type

Type TFCVideo
End Type
