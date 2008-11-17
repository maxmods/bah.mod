' Copyright 2008 Bruce A Henderson
'
' Licensed under the Apache License, Version 2.0 (the "License");
' you may not use this file except in compliance with the License.
' You may obtain a copy of the License at
'
'     http://www.apache.org/licenses/LICENSE-2.0
'
' Unless required by applicable law or agreed to in writing, software
' distributed under the License is distributed on an "AS IS" BASIS,
' WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
' See the License for the specific language governing permissions and
' limitations under the License.
'
SuperStrict

Rem
bbdoc: Flickcurl
End Rem
Module BaH.Flickcurl

ModuleInfo "Version: 1.00"
ModuleInfo "License: Apache 2.0"
ModuleInfo "Copyright: "
ModuleInfo "Copyright: Wrapper - 2008 Bruce A Henderson"

ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H -DFLICKCURL_STATIC -DCURL_STATICLIB -DLIBXML_STATIC"

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
		Return TFCPhoto._create(bmx_flickcurl_photosgetinfo(fcPtr, photoID))
	End Method
	
	
	Method GetFrob:String()
		Return bmx_flickcurl_getfrob(fcPtr)
	End Method
	
	Method GetFullToken:String(frob:String)
		Return bmx_flickcurl_getfulltoken(fcPtr, frob)
	End Method
	
	Rem
	bbdoc: Find Flickr Places information by Place Id.
	End Rem
	Method ResolvePlaceId:TFCPlace(placeID:String)
		Return TFCPlace._create(bmx_flickcurl_resolveplaceid(fcPtr, placeID))
	End Method
	
	Rem
	bbdoc: Find Flickr Places information by Place URL.
	End Rem
	Method ResolvePlaceURL:TFCPlace(url:String)
		Return TFCPlace._create(bmx_flickcurl_resolveplaceurl(fcPtr, url))
	End Method
	
	Rem
	bbdoc: Return a place ID for a latitude, longitude and accuracy triple.
	about: The flickr.places.findByLatLon method is not meant to be a (reverse) geocoder in the traditional sense.
	It is designed to allow users to find photos for "places" and will round up to the nearest place type
	to which corresponding place IDs apply. 
	End Rem
	Method FindPlaceByLatLon:TFCPlace(lat:Double, lon:Double, accuracy:Int = 16)
		Return TFCPlace._create(bmx_flickcurl_findplacebylatlon(fcPtr, lat, lon, accuracy))
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


Rem
bbdoc: 
End Rem
Type TFCPhoto

	Field photoPtr:Byte Ptr
	Field owner:Int
	
	Function _create:TFCPhoto(photoPtr:Byte Ptr, owner:Int = True)
		If photoPtr Then
			Local this:TFCPhoto = New TFCPhoto
			this.photoPtr = photoPtr
			this.owner = owner
			Return this
		End If
	End Function
	
	Method GetID:String()
	End Method
	
	Method GetURI:String()
		Return bmx_flickcurl_photo_geturi(photoPtr)
	End Method
	
	Method GetTag:TFCTag(index:Int)
		Return TFCTag._create(bmx_flickcurl_photo_gettag(photoPtr, index), False)
	End Method
	
	Method GetTagCount:Int()
		Return bmx_flickcurl_photo_gettagcount(photoPtr)
	End Method
	
	Method GetField:TFCPhotoField(index:Int)
		Return TFCPhotoField(bmx_flickcurl_photo_getfield(photoPtr, index))
	End Method
	
	Method GetPlace:TFCPlace()
		Return TFCPlace._create(bmx_flickcurl_photo_getplace(photoPtr), False)
	End Method
	
	Method GetVideo:TFCVideo()
	End Method
	
	Method GetMediaType:String()
	End Method
	
	Rem
	bbdoc: Get a photo's image source URI.
	about: Size should be one of :
	<table>
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td> FCIMAGE_SQUARE </td><td>small square 75x75</td></tr>
	<tr><td> FCIMAGE_THUMB </td><td> thumbnail, 100 on longest side </td></tr>
	<tr><td> FCIMAGE_SMALL </td><td> small, 240 on longest side </td></tr>
	<tr><td> FCIMAGE_MEDIUM </td><td> medium, 500 on longest side </td></tr>
	<tr><td> FCIMAGE_LARGE </td><td> large, 1024 on longest side (only exists for very large original images) </td></tr>
	<tr><td> FCIMAGE_ORIGINAL </td><td> original image, either a jpg, gif or png, depending on source format </td></tr>
	</table>
	End Rem
	Method GetSourceURI:String(size:Int = FCIMAGE_SMALL)
		Return bmx_flickcurl_photo_getsourceuri(photoPtr, size)
	End Method
	
	Rem
	bbdoc: Destructor for photo object.
	End Rem
	Method Free()
		If photoPtr And owner Then
			bmx_flickcurl_photo_free(photoPtr)
			photoPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TFCTag

	Field tagPtr:Byte Ptr
	Field owner:Int

	Function _create:TFCTag(tagPtr:Byte Ptr, owner:Int = True)
		If tagPtr Then
			Local this:TFCTag = New TFCTag
			this.tagPtr = tagPtr
			this.owner = owner
			Return this
		End If
	End Function
	
	Method GetPhoto:TFCPhoto()
		Return TFCPhoto._create(bmx_flickcurl_tag_getphoto(tagPtr), False)
	End Method
	
	Method GetID:String()
		Return bmx_flickcurl_tag_getid(tagPtr)
	End Method
	
	Method GetAuthor:String()
		Return bmx_flickcurl_tag_getauthor(tagPtr)
	End Method
	
	Method GetAuthorName:String()
		Return bmx_flickcurl_tag_getauthorname(tagPtr)
	End Method
	
	Method GetRaw:String()
		Return bmx_flickcurl_tag_getraw(tagPtr)
	End Method
	
	Method GetCooked:String()
		Return bmx_flickcurl_tag_getcooked(tagPtr)
	End Method
	
	Method GetMachineTag:Int()
		Return bmx_flickcurl_tag_getmachinetag(tagPtr)
	End Method
	
	Method GetCount:Int()
		Return bmx_flickcurl_tag_getcount(tagPtr)
	End Method
	
	Method Free()
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

Type TFCTagList

	Field tagListPtr:Byte Ptr

	Method GetTag:TFCTag(index:Int)
	End Method
	
	Method Free()
	End Method
	
	Method Delete()
		Free()
	End Method

End Type

Rem
bbdoc: 
End Rem
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

Rem
bbdoc: A Place.
about: Index 0 in the array is the location itself.
End Rem
Type TFCPlace

	Field placePtr:Byte Ptr
	Field owner:Int

	Function _create:TFCPlace(placePtr:Byte Ptr, owner:Int = True)
		If placePtr Then
			Local this:TFCPlace = New TFCPlace
			this.placePtr = placePtr
			this.owner = owner
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Gets the place name for the type.
	End Rem
	Method GetName:String(placeType:Int)
		Return bmx_flickcurl_place_getname(placePtr, placeType)
	End Method
	
	Rem
	bbdoc: Gets the place ID for the type.
	End Rem
	Method GetID:String(placeType:Int)
		Return bmx_flickcurl_place_getid(placePtr, placeType)
	End Method
	
	Rem
	bbdoc: Gets the place URL for the type.
	End Rem
	Method GetURL:String(placeType:Int)
		Return bmx_flickcurl_place_geturl(placePtr, placeType)
	End Method
	
	Rem
	bbdoc: Gets the location type of index 0.
	about: Usually FLICKCURL_PLACE_LOCATION but may be wider 
	End Rem
	Method GetType:Int()
		Return bmx_flickcurl_place_gettype(placePtr)
	End Method
	
	Rem
	bbdoc: Gets the WOE (Where On Earth) ID.
	End Rem
	Method GetWOEID:String(placeType:Int)
		Return bmx_flickcurl_place_getwoeid(placePtr, placeType)
	End Method
	
	Rem
	bbdoc: Gets the location for this place.
	End Rem
	Method GetLocation:TFCLocation()
		Return TFCLocation(bmx_flickcurl_place_getlocation(placePtr))
	End Method
	
	Rem
	bbdoc: Gets the count of photos (when used for 
	End Rem
	Method GetCount:Int()
		Return bmx_flickcurl_place_getcount(placePtr)
	End Method
	
	Rem
	bbdoc: Destructor for place object.
	End Rem
	Method Free()
		If placePtr And owner Then
			bmx_flickcurl_place_free(placePtr)
			placePtr = Null
		End If
	End Method
	
	Rem
	bbdoc: Get label for a place type.
	about: One of
	<table>
	<tr><th>Constant</th><th>Description</th></tr>
	<tr><td>FLICKCURL_PLACE_LOCATION</td><td>a general location</td></tr>
	<tr><td>FLICKCURL_PLACE_NEIGHBOURHOOD</td><td>neighborhood (narrowest place)</td></tr>
	<tr><td>FLICKCURL_PLACE_NEIGHBORHOOD</td><td>deprecated</td></tr>
	<tr><td>FLICKCURL_PLACE_LOCALITY</td><td>locality</td></tr>
	<tr><td>FLICKCURL_PLACE_COUNTY</td><td>county</td></tr>
	<tr><td>FLICKCURL_PLACE_REGION</td><td>region</td></tr>
	<tr><td>FLICKCURL_PLACE_COUNTRY</td><td>country (widest place)</td></tr>
	<tr><td>FLICKCURL_PLACE_LAST</td><td>internal offset to last place type</td></tr>
	</table>
	End Rem
	Function GetTypeLabel:String(placeType:Int)
		Return bmx_flickcurl_place_gettypelabel(placeType)
	End Function
	
	Method Delete()
		Free()
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TFCVideo
End Type

Rem
bbdoc: A Location in the world with an optional accuracy.
End Rem
Type TFCLocation

	Function _create:TFCLocation(latitude:Double, longitude:Double, accuracy:Int)
		Local this:TFCLocation = New TFCLocation
		this.latitude = latitude
		this.longitude = longitude
		this.accuracy = accuracy
		Return this
	End Function

	Rem
	bbdoc: The latitude whose valid range is -90 to 90.
	End Rem
	Field latitude:Double
	Rem
	bbdoc: The longitude whose valid range is -180 to 180.
	End Rem
	Field longitude:Double
	Rem
	bbdoc: Recorded accuracy level of the location information.
	about: World level is 1, Country is ~3, Region ~6, City ~11, Street ~16. Current range is 1-16. The default is 16. 
	End Rem
	Field accuracy:Int
	
End Type

Rem
bbdoc: 
End Rem
Type TFCPermissions
End Type

Rem
bbdoc: 
End Rem
Type TFCSize
End Type

Rem
bbdoc: Search parameters for SearchPhotos().
End Rem
Type TFCSearchParams

	Field paramsPtr:Byte Ptr
	
	Method New()
		paramsPtr = bmx_flickcurl_searchparams_new()
	End Method

	Rem
	bbdoc: The NSID of the user who's photo to search, or 'me'.
	End Rem
	Method SetUserID(value:String)
		bmx_flickcurl_searchparams_setuserid(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: A comma-delimited list of tags.
	End Rem
	Method SetTags(value:String)
		bmx_flickcurl_searchparams_settags(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Either 'any' for an OR combination of tags, or 'all' for an AND combination.
	about: Defaults to 'any' if not specified
	End Rem
	Method SetTagMode(value:String)
		bmx_flickcurl_searchparams_settagmode(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Free text search.
	End Rem
	Method SetText(value:String)
		bmx_flickcurl_searchparams_settext(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Minimum upload date as a unix timestamp.
	End Rem
	Method SetMinUploadDate(value:Int)
		bmx_flickcurl_searchparams_setminuploaddate(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Maximum upload date as a unix timestamp.
	End Rem
	Method SetMaxUploadDate(value:Int)
		bmx_flickcurl_searchparams_setmaxuploaddate(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Minimum taken date in the form of a mysql datetime.
	End Rem
	Method SetMinTakenDate(value:String)
		bmx_flickcurl_searchparams_setmintakendate(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Maximum taken date in the form of a mysql datetime.
	End Rem
	Method SetMaxTakenDate(value:String)
		bmx_flickcurl_searchparams_setmaxtakendate(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Comma-separated list of photo licenses.
	End Rem
	Method SetLicense(value:String)
		bmx_flickcurl_searchparams_setlicense(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: The order in which to sort returned photos.
	about: Defaults to date-posted-desc. The possible values are: date-posted-asc, date-posted-desc,
	date-taken-asc, date-taken-desc, interestingness-desc, interestingness-asc, and relevance.
	End Rem
	Method SetSort(value:String)
		bmx_flickcurl_searchparams_setsort(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Return photos only matching a certain privacy level.
	End Rem
	Method SetPrivacyFilter(value:String)
		bmx_flickcurl_searchparams_setprivacyfilter(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: A comma-delimited list of 4 values defining the Bounding Box of the area that will be searched.
	End Rem
	Method SetBbox(value:String)
		bmx_flickcurl_searchparams_setbbox(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Recorded accuracy level of the location information.
	about: Current range is 1-16 .
	End Rem
	Method SetAccuracy(value:Int)
		bmx_flickcurl_searchparams_setaccuracy(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Safe search setting.
	about: 1 safe, 2 moderate, 3 restricted.
	End Rem
	Method SetSafeSearch(value:Int)
		bmx_flickcurl_searchparams_setsafesearch(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Content Type setting.
	about: 1 for photos only, 2 for screenshots only, 3 for 'other' only, 4 for all types.
	End Rem
	Method SetContentType(value:Int)
		bmx_flickcurl_searchparams_setcontenttype(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Machine tag search syntax.
	End Rem
	Method SetMachineTags(value:String)
		bmx_flickcurl_searchparams_setmachinetags(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Either 'any' for an OR combination of tags, or 'all' for an AND combination.
	about: Defaults to 'any' if not specified. 
	End Rem
	Method SetMachineTagMode(value:String)
		bmx_flickcurl_searchparams_setmachinetagmode(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: The id of a group who's pool to search.
	about: If specified, only matching photos posted to the group's pool will be returned.
	End Rem
	Method SetGroupID(value:String)
		bmx_flickcurl_searchparams_setgroupid(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: A comma-delimited list of extra information to fetch for each returned record.
	about: Currently supported fields are: license, date_upload, date_taken, owner_name, icon_server,
	original_format, last_update, geo, tags, machine_tags.
	End Rem
	Method SetExtras(value:String)
		bmx_flickcurl_searchparams_setextras(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Number of photos to return per page.
	about: If this argument is omitted, it defaults to 100.
	The maximum allowed value is 500.
	End Rem
	Method SetPerPage(value:Int)
		bmx_flickcurl_searchparams_setperpage(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: The page of results to return.
	about: If this argument is omitted, it defaults to 1.
	End Rem
	Method SetPage(value:Int)
		bmx_flickcurl_searchparams_setpage(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: A Flickr place id.
	about: (only used if bbox argument isn't present). Experimental. Geo queries require some sort of
	limiting agent in order to prevent the database from crying. This is basically like the check against
	"parameterless searches" for queries without a geo component. A tag, for instance, is considered a limiting
	agent as are user defined min_date_taken and min_date_upload parameters - If no limiting factor is passed
	we return only photos added in the last 12 hours (though we may extend the limit in the future)
	End Rem
	Method SetPlaceID(value:String)
		bmx_flickcurl_searchparams_setplaceid(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: "photos" or "videos".
	End Rem
	Method SetMedia(value:String)
		bmx_flickcurl_searchparams_setmedia(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: Non-0 if a photo has been geotagged.
	End Rem
	Method SetHasGeo(value:Int)
		bmx_flickcurl_searchparams_sethasgeo(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: A valid latitude, in decimal format, for doing radial geo queries.
	End Rem
	Method SetLat(value:Double)
		bmx_flickcurl_searchparams_setlat(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: A valid longitude, in decimal format, for doing radial geo queries.
	End Rem
	Method SetLon(value:Double)
		bmx_flickcurl_searchparams_setlon(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: A valid radius used for geo queries, greater than zero and less than 20 miles (or 32 kilometers), for use with point-based geo queries.
	about: The default value is 5 (km) (or 0.0 for not used) 
	End Rem
	Method SetRadius(value:Double)
		bmx_flickcurl_searchparams_setradius(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: The unit of measure when doing radial geo queries.
	about: Valid options are "mi" (miles) and "km" (kilometers). The default is "km"
	End Rem
	Method SetRadiusUnits(value:String)
		bmx_flickcurl_searchparams_setradiusunits(paramsPtr, value)
	End Method
	
	Rem
	bbdoc: (Experimental) Requires userID field be set and limits queries to photos beloing to that user's photos.
	about: Valid arguments are 'all' or 'ff' for just friends and family.
	End Rem
	Method SetContacts(value:String)
		bmx_flickcurl_searchparams_setcontacts(paramsPtr, value)
	End Method

	Method Delete()
		If paramsPtr Then
			bmx_flickcurl_searchparams_delete(paramsPtr)
			paramsPtr = Null
		End If
	End Method

End Type


