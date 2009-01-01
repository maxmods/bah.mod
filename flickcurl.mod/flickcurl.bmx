' Copyright 2008,2009 Bruce A Henderson
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
ModuleInfo "Copyright: Flickurl - 2007-2008, David Beckett http://www.dajobe.org/"
ModuleInfo "Copyright: Wrapper - 2008,2009 Bruce A Henderson"

ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H -DFLICKCURL_STATIC -DCURL_STATICLIB -DLIBXML_STATIC"

Import "common.bmx"


'
' Notes :
' flickcurl.h - added "_" suffix to "namespace" parameters
'


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
	bbdoc: Sets application API Key for flickcurl requests.
	End Rem
	Method SetAPIKey(apiKey:String)
		bmx_flickcurl_setapikey(fcPtr, apiKey)
	End Method
	
	Rem
	bbdoc: Sets Auth Token for flickcurl requests.
	End Rem
	Method SetAuthToken(authToken:String)
		bmx_flickcurl_setauthtoken(fcPtr, authToken)
	End Method
	
	Rem
	bbdoc: Sets Shared Secret for flickcurl requests.
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
	
	Rem
	bbdoc: Get a frob to be used during authentication.
	End Rem
	Method GetFrob:String()
		Return bmx_flickcurl_getfrob(fcPtr)
	End Method
	
	Rem
	bbdoc: Turn a frob into an authToken.
	End Rem
	Method GetFullToken:String(frob:String)
		Return bmx_flickcurl_getfulltoken(fcPtr, frob)
	End Method

	Rem
	bbdoc: Get the auth token for the given frob, if one has been attached.
	End Rem	
	Method GetToken:String(frob:String)
		Return bmx_flickcurl_gettoken(fcPtr, frob)
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
	
	Rem
	bbdoc: Returns the default content type preference for the user.
	End Rem
	Method GetPrefsContentType:Int()
		Return bmx_flickcurl_getprefscontenttype(fcPtr)
	End Method
	
	Rem
	bbdoc: Returns the default privacy level for geographic information attached to the user's photos.
	about: Possible values are: 0: no default, 1: public, 2: contacts only, 3: friends and family only,
	4: friends only, 5: family only, 6: private.
	End Rem
	Method GetPrefsGeoPerms:Int()
		Return bmx_flickcurl_getprefsgeoperms(fcPtr)
	End Method
	
	Rem
	bbdoc: Returns the default hidden preference for the user.
	End Rem
	Method GetPrefsHidden:Int()
		Return bmx_flickcurl_getprefshidden(fcPtr)
	End Method
	
	Rem
	bbdoc: Returns the default privacy level preference for the user.
	about: Possible values are: Public (1), Friends only (2), Family only (3) Friends and Family (4) and Private (5)
	End Rem
	Method GetPrefsPrivacy:Int()
		Return bmx_flickcurl_getprefsprivacy(fcPtr)
	End Method
	
	Rem
	bbdoc: Returns the default safety level preference for the user.
	End Rem
	Method GetPrefsSafetyLevel:Int()
		Return bmx_flickcurl_getprefssafetylevel(fcPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SearchPhotos:TFCPhotoList(params:TFCSearchParams)
		Return TFCListOfPhotos._create(bmx_flickcurl_searchphotos(fcPtr, params.paramsPtr), fcPtr)
	End Method

	Rem
	bbdoc: Returns the list of interesting photos for the most recent day or a user-specified date.
	End Rem
	Method GetInterestingnessList:TFCPhotoList(date:String, extras:String, perPage:Int, page:Int)
		Return TFCListOfPhotos._create(bmx_flickcurl_getinterestingnesslist(fcPtr, date, extras, perPage, page), fcPtr)
	End Method
	
	Rem
	bbdoc: Get a user, given their email address.
	End Rem
	Method FindPeopleByEmail:TFCPerson(email:String)
		Return TFCPerson._create(bmx_flickcurl_findpeoplebyemail(fcPtr, email), fcPtr)
	End Method
	
	Rem
	bbdoc: Get a user, given their username.
	End Rem
	Method FindPeopleByUsername:TFCPerson(username:String)
		Return TFCPerson._create(bmx_flickcurl_findpeoplebyusername(fcPtr, username), fcPtr)
	End Method
	
	Rem
	bbdoc: Returns an array of tag clusters for a tag.
	End Rem
	Method GetClusters:TFCTagCluster[](tag:String)
		Return bmx_flickcurl_tags_getclusters(fcPtr, tag)
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
	' TODO
	End Method
	
	Method GetID:String()
	' TODO
	End Method
	
	Method GetUser:String()
	' TODO
	End Method
	
	Method GetUsername:String()
	End Method
	
	' TODO
	Method GetValue:String()
	End Method
	
	Method GetDateAdded:Int()
	' TODO
	End Method

End Type



Rem
bbdoc: A photo comment.
End Rem
Type TFCComment

	Field id:String
	Field author:String
	Field authorname:String
	Field datecreate:Int
	Field permalink:String
	Field text:String
	
	Field fcPtr:Byte Ptr

	Function _create:TFCComment(id:String, author:String, authorname:String, datecreate:Int, permalink:String, text:String, fcPtr:Byte Ptr)
		Local this:TFCComment = New TFCComment
		this.id = id
		this.author = author
		this.authorname = authorname
		this.datecreate = datecreate
		this.permalink = permalink
		this.text = text
		this.fcPtr = fcPtr
		Return this
	End Function
	
	Rem
	bbdoc: Gets the comment ID.
	End Rem
	Method GetID:String()
		Return id
	End Method
	
	Rem
	bbdoc: Gets the author ID.
	End Rem
	Method GetAuthor:String()
		Return author
	End Method
	
	Rem
	bbdoc: Gets the author name.
	End Rem
	Method GetAuthorName:String()
		Return authorname
	End Method
	
	Rem
	bbdoc: Gets the date of creation.
	End Rem
	Method GetDateCreated:Int()
		Return datecreate
	End Method
	
	Rem
	bbdoc: Gets the permanent link of comment.
	End Rem
	Method GetPermalink:String()
		Return permalink
	End Method
	
	Rem
	bbdoc: Gets the comment text.
	End Rem
	Method GetText:String()
		Return text
	End Method
	
	Rem
	bbdoc: Delete a comment as the currently authenticated user.
	End Rem
	Method DeleteComment:Int()
		Return bmx_flickcurl_comment_deletecomment(fcPtr, id)
	End Method
	
	Rem
	bbdoc: Edit the text of a comment as the currently authenticated user.
	End Rem
	Method EditComment:Int(commentText:String)
		Return bmx_flickcurl_comment_editcomment(fcPtr, id, commentText)
	End Method

End Type


Rem
bbdoc: A photo or video.
End Rem
Type TFCPhoto

	Field photoPtr:Byte Ptr
	Field owner:Int
	
	Field fcPtr:Byte Ptr
	
	Function _create:TFCPhoto(photoPtr:Byte Ptr, fcPtr:Byte Ptr, owner:Int = True)
		If photoPtr Then
			Local this:TFCPhoto = New TFCPhoto
			this.photoPtr = photoPtr
			this.fcPtr = fcPtr
			this.owner = owner
			Return this
		End If
	End Function
	
	Rem
	bbdoc: The photo/video ID.
	End Rem
	Method GetID:String()
		Return bmx_flickcurl_photo_getid(photoPtr)
	End Method
	
	Rem
	bbdoc: The photo/video page URI.
	End Rem
	Method GetURI:String()
		Return bmx_flickcurl_photo_geturi(photoPtr)
	End Method
	
	Rem
	bbdoc: Gets the tag at @index.
	End Rem
	Method GetTag:TFCTag(index:Int)
		Return TFCTag._create(bmx_flickcurl_photo_gettag(photoPtr, index), fcPtr, False)
	End Method
	
	Rem
	bbdoc: Returns the tag count for the photo.
	End Rem
	Method GetTagCount:Int()
		Return bmx_flickcurl_photo_gettagcount(photoPtr)
	End Method
	
	Rem
	bbdoc: Returns the field at @index.
	about: Valid @index. One of :
<table>
<tr><th>Constant</th><th>Description</th></tr>
<tr><td>PHOTO_FIELD_none</td><td> internal</td></tr>
<tr><td>PHOTO_FIELD_dateuploaded</td><td> date uploaded</td></tr>
<tr><td>PHOTO_FIELD_farm</td><td> farm number</td></tr>
<tr><td>PHOTO_FIELD_isfavorite</td><td> is favorite boolean</td></tr>
<tr><td>PHOTO_FIELD_license</td><td> license</td></tr>
<tr><td>PHOTO_FIELD_originalformat</td><td> original format</td></tr>
<tr><td>PHOTO_FIELD_rotation</td><td> rotation</td></tr>
<tr><td>PHOTO_FIELD_server</td><td> server</td></tr>
<tr><td>PHOTO_FIELD_dates_lastupdate</td><td> last update date</td></tr>
<tr><td>PHOTO_FIELD_dates_posted</td><td> posted date</td></tr>
<tr><td>PHOTO_FIELD_dates_taken</td><td> taken date</td></tr>
<tr><td>PHOTO_FIELD_dates_takengranularity</td><td> taken granularity</td></tr>
<tr><td>PHOTO_FIELD_description</td><td> description</td></tr>
<tr><td>PHOTO_FIELD_editability_canaddmeta</td><td> can add metadata boolean</td></tr>
<tr><td>PHOTO_FIELD_editability_cancomment</td><td> can comment boolean</td></tr>
<tr><td>PHOTO_FIELD_geoperms_iscontact</td><td> geo perms are for contacts</td></tr>
<tr><td>PHOTO_FIELD_geoperms_isfamily</td><td> geo perms are for family</td></tr>
<tr><td>PHOTO_FIELD_geoperms_isfriend</td><td> geo perms are for frind</td></tr>
<tr><td>PHOTO_FIELD_geoperms_ispublic</td><td> geo perms are for public</td></tr>
<tr><td>PHOTO_FIELD_location_accuracy</td><td> location accuracy</td></tr>
<tr><td>PHOTO_FIELD_location_latitude</td><td> location latitude</td></tr>
<tr><td>PHOTO_FIELD_location_longitude</td><td> location longitude</td></tr>
<tr><td>PHOTO_FIELD_owner_location</td><td> owner location</td></tr>
<tr><td>PHOTO_FIELD_owner_nsid</td><td> owner NSID</td></tr>
<tr><td>PHOTO_FIELD_owner_realname</td><td> owner real name</td></tr>
<tr><td>PHOTO_FIELD_owner_username</td><td> owner user name</td></tr>
<tr><td>PHOTO_FIELD_title</td><td> title</td></tr>
<tr><td>PHOTO_FIELD_visibility_isfamily</td><td> visibility is for family</td></tr>
<tr><td>PHOTO_FIELD_visibility_isfriend</td><td> visibility is for friend</td></tr>
<tr><td>PHOTO_FIELD_visibility_ispublic</td><td> visibility is for public</td></tr>
<tr><td>PHOTO_FIELD_secret</td><td> photo secret</td></tr>
<tr><td>PHOTO_FIELD_originalsecret</td><td> photo original secret</td></tr>
<tr><td>PHOTO_FIELD_location_neighbourhood</td><td> location neighbourhood</td></tr>
<tr><td>PHOTO_FIELD_location_neighborhood</td><td> deprecated</td></tr>
<tr><td>PHOTO_FIELD_location_locality</td><td> location locality</td></tr>
<tr><td>PHOTO_FIELD_location_county</td><td> location county</td></tr>
<tr><td>PHOTO_FIELD_location_region</td><td> location region</td></tr>
<tr><td>PHOTO_FIELD_location_country</td><td> location country</td></tr>
<tr><td>PHOTO_FIELD_location_placeid</td><td> location place ID</td></tr>
<tr><td>PHOTO_FIELD_neighbourhood_placeid</td><td> neighborhood place ID</td></tr>
<tr><td>PHOTO_FIELD_neighborhood_placeid</td><td> dprecated</td></tr>
<tr><td>PHOTO_FIELD_locality_placeid</td><td> locality place ID</td></tr>
<tr><td>PHOTO_FIELD_county_placeid</td><td> county place ID</td></tr>
<tr><td>PHOTO_FIELD_region_placeid</td><td> region place ID</td></tr>
<tr><td>PHOTO_FIELD_country_placeid</td><td> country place ID</td></tr>
<tr><td>PHOTO_FIELD_location_woeid</td><td> location WOE ID</td></tr>
<tr><td>PHOTO_FIELD_neighbourhood_woeid</td><td> neighborhood WOE ID</td></tr>
<tr><td>PHOTO_FIELD_neighborhood_woeid</td><td> deprecated</td></tr>
<tr><td>PHOTO_FIELD_locality_woeid</td><td> locality WOE ID</td></tr>
<tr><td>PHOTO_FIELD_county_woeid</td><td> county WOE ID</td></tr>
<tr><td>PHOTO_FIELD_region_woeid</td><td> region WOE ID</td></tr>
<tr><td>PHOTO_FIELD_country_woeid</td><td> country WOE ID</td></tr>
<tr><td>PHOTO_FIELD_usage_candownload</td><td> can download</td></tr>
<tr><td>PHOTO_FIELD_usage_canblog</td><td> can blog</td></tr>
<tr><td>PHOTO_FIELD_usage_canprint</td><td> can print</td></tr>
<tr><td>PHOTO_FIELD_FIRST</td><td> internal offset to first in enum list</td></tr>
<tr><td>PHOTO_FIELD_LAST</td><td> internal offset to last in enum list</td></tr>
</table>
	End Rem
	Method GetField:TFCPhotoField(index:Int)
		Return TFCPhotoField(bmx_flickcurl_photo_getfield(photoPtr, index))
	End Method
	
	Rem
	bbdoc: Returns the place for the photo.
	End Rem
	Method GetPlace:TFCPlace()
		Return TFCPlace._create(bmx_flickcurl_photo_getplace(photoPtr), False)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetVideo:TFCVideo()
	' TODO
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method GetMediaType:String()
	' TODO
	End Method
	
	Rem
	bbdoc: Get the photo's image source URI.
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
	bbdoc: Add tags to the photo.
	about: @tags to add as a space-separated list.
	End Rem
	Method AddTags:Int(tags:String)
		Return bmx_flickcurl_photo_addtags(fcPtr, photoPtr, tags)
	End Method
	
	Rem
	bbdoc: Add comment to a photo as the currently authenticated user.
	returns: A TFCComment, or Null on failure.
	End Rem
	Method AddComment:TFCComment(comment:String)
		Return TFCComment(bmx_flickcurl_photo_addcomment(fcPtr, photoPtr, comment))
	End Method
	
	Rem
	bbdoc: Returns the comments for the photo.
	End Rem
	Method GetCommentList:TFCCommentList()
		Return TFCCommentList._create(bmx_flickcurl_photo_getcommentlist(fcPtr, photoPtr), fcPtr)
	End Method
	
	Rem
	bbdoc: Set the safety level of a photo.
	returns: Non-zero on failure.
	End Rem
	Method SetSafetyLevel:Int(safetyLevel:Int, hidden:Int)
		Return bmx_flickcurl_photo_setsafetylevel(fcPtr, photoPtr, safetyLevel, hidden)
	End Method
	
	Rem
	bbdoc: Set the tags for a photo.
	returns: Non-zero on failure.
	about: Note that this replaces all existing tags with the @tags here.
	End Rem
	Method SetTags:Int(tags:String)
		Return bmx_flickcurl_photo_settags(fcPtr, photoPtr, tags)
	End Method
	
	Rem
	bbdoc: Rotate a photo.
	about: Valid values are 90, 180 and 270.
	End Rem
	Method TransformRotate:Int(degrees:Int)
		Return bmx_flickcurl_photo_transformrotate(fcPtr, photoPtr, degrees)
	End Method
	
	Rem
	bbdoc: Adds the photo to a user's favorites list. 
	End Rem
	Method AddFavorite:Int()
		Return bmx_flickcurl_photo_addfavorite(fcPtr, photoPtr)
	End Method
	
	Rem
	bbdoc: Removes the photo from a user's favorites list. 
	End Rem
	Method RemoveFavorite:Int()
		Return bmx_flickcurl_photo_removefavorite(fcPtr, photoPtr)
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

Type TFCListOfPhotos Extends TFCPhotoList

	Function _create:TFCListOfPhotos(photoListPtr:Byte Ptr, fcPtr:Byte Ptr)
		If photoListPtr Then
			Local this:TFCListOfPhotos = New TFCListOfPhotos
			this.photoListPtr = photoListPtr
			this.fcPtr = fcPtr
			Return this
		End If
	End Function
	
	Method GetFormat:String()
	' TODO
	End Method
	
	Rem
	bbdoc: Returns the number of photos.
	End Rem
	Method GetPhotoCount:Int()
		Return bmx_flickcurl_listofphotos_getphotocount(photoListPtr)
	End Method
	
	Rem
	bbdoc: Returns the photo at the given @index.
	End Rem
	Method GetPhoto:TFCPhoto(index:Int)
		Return TFCPhoto._create(bmx_flickcurl_listofphotos_getphoto(photoListPtr, index), fcPtr, False)
	End Method
	
	Method GetContent:String()
	' TODO
	End Method

	Method Free()
		If photoListPtr Then
			flickcurl_free_photos(photoListPtr)
			photoListPtr = Null
		End If
	End Method

End Type

Rem
bbdoc: Photos List.
End Rem
Type TFCPhotoList

	Field photoListPtr:Byte Ptr
	
	Field fcPtr:Byte Ptr

	Function _create:TFCPhotoList(photoListPtr:Byte Ptr, fcPtr:Byte Ptr)
		If photoListPtr Then
			Local this:TFCPhotoList = New TFCPhotoList
			this.photoListPtr = photoListPtr
			this.fcPtr = fcPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Returns the requested content format or NULL if a list of photos was wanted.
	about: On the result from API calls this is set to the requested feed format or "xml" if none was given.
	End Rem
	Method GetFormat:String()
		Return bmx_flickcurl_photolist_getformat(photoListPtr)
	End Method
	
	Rem
	bbdoc: Returns the number of photos.
	End Rem
	Method GetPhotoCount:Int()
		Return bmx_flickcurl_photolist_getphotocount(photoListPtr)
	End Method
	
	Rem
	bbdoc: Returns the photo at the given @index.
	End Rem
	Method GetPhoto:TFCPhoto(index:Int)
		Return TFCPhoto._create(bmx_flickcurl_photolist_getphoto(photoListPtr, index), fcPtr, False)
	End Method
	
	Rem
	bbdoc: Returns the raw content if format is not null.
	End Rem
	Method GetContent:String()
		Return bmx_flickcurl_photolist_getcontent(photoListPtr)
	End Method
	
	Rem
	bbdoc: Destructor for photos list.
	End Rem
	Method Free()
		If photoListPtr Then
			flickcurl_free_photos_list(photoListPtr)
			photoListPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: A tag OR a posting of a tag about a photo by a user OR a tag in a histogram.
about: Most of these fields may be Null, 0 for numbers but not all. Either raw or cooked MUST appear.
End Rem
Type TFCTag

	Field tagPtr:Byte Ptr
	Field owner:Int
	
	Field fcPtr:Byte Ptr

	Function _create:TFCTag(tagPtr:Byte Ptr, fcPtr:Byte Ptr, owner:Int = True)
		If tagPtr Then
			Local this:TFCTag = New TFCTag
			this.tagPtr = tagPtr
			this.fcPtr = fcPtr
			this.owner = owner
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Associated photo object if any.
	End Rem
	Method GetPhoto:TFCPhoto()
		Return TFCPhoto._create(bmx_flickcurl_tag_getphoto(tagPtr), fcPtr, False)
	End Method
	
	Rem
	bbdoc: Tag identifier.
	End Rem
	Method GetID:String()
		Return bmx_flickcurl_tag_getid(tagPtr)
	End Method
	
	Rem
	bbdoc: Author (may be NULL) 
	End Rem
	Method GetAuthor:String()
		Return bmx_flickcurl_tag_getauthor(tagPtr)
	End Method
	
	Rem
	bbdoc: Author real name (may be NULL) 
	End Rem
	Method GetAuthorName:String()
		Return bmx_flickcurl_tag_getauthorname(tagPtr)
	End Method
	
	Rem
	bbdoc: Raw tag as user typed it (may be NULL, but if so cooked must be not NULL) 
	End Rem
	Method GetRaw:String()
		Return bmx_flickcurl_tag_getraw(tagPtr)
	End Method
	
	Rem
	bbdoc: Cooked tag (may be NULL, but if so raw must not be NULL) 
	End Rem
	Method GetCooked:String()
		Return bmx_flickcurl_tag_getcooked(tagPtr)
	End Method
	
	Rem
	bbdoc: Boolean (non-0 true) if tag is a Machine Tag.
	End Rem
	Method GetMachineTag:Int()
		Return bmx_flickcurl_tag_getmachinetag(tagPtr)
	End Method
	
	Rem
	bbdoc: The tag count in a histogram (or 0).
	End Rem
	Method GetCount:Int()
		Return bmx_flickcurl_tag_getcount(tagPtr)
	End Method
	
	Method Free()
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

Extern
	Function bmx_flickcurl_tags_getclusters:TFCTagCluster[](handle:Byte Ptr, tag:String)
End Extern

Rem
bbdoc: A cluster (set) of tag names
End Rem
Type TFCTagCluster

	Rem
	bbdoc: Tags in this cluster.
	End Rem
	Field tags:String[]
	
	Function _createClusterArray:TFCTagCluster[](size:Int)
		Return New TFCTagCluster[size]
	End Function
	
	Function _setCluster:TFCTagCluster(array:TFCTagCluster[], index:Int, size:Int)
		Local cluster:TFCTagCluster = TFCTagCluster._create(size)
		array[index] = cluster
		Return cluster
	End Function
	
	Function _create:TFCTagCluster(size:Int)
		Local this:TFCTagCluster = New TFCTagCluster
		this.tags = New String[size]
		Return this
	End Function
	
	Function _setTag(cluster:TFCTagCluster, index:Int, tag:String)
		cluster.tags[index] = tag
	End Function
	
End Type

Rem
bbdoc: 
End Rem
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
Type TFCCommentList

	Field commentListPtr:Byte Ptr
	
	Field fcPtr:Byte Ptr

	Function _create:TFCCommentList(commentListPtr:Byte Ptr, fcPtr:Byte Ptr)
		If commentListPtr Then
			Local this:TFCCommentList = New TFCCommentList
			this.commentListPtr = commentListPtr
			this.fcPtr = fcPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method GetComment:TFCComment(index:Int)
		Return TFCComment(bmx_flickcurl_commentlist_getcomment(commentListPtr, index, fcPtr))
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Free()
		If commentListPtr Then
			flickcurl_free_comments(commentListPtr)
			commentListPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
		
End Type

Rem
bbdoc: 
End Rem
Type TFCField

	Rem
	bbdoc: 
	End Rem
	Field svalue:String
	Rem
	bbdoc: 
	End Rem
	Field ivalue:Int
	Rem
	bbdoc: 
	End Rem
	Field valueType:Int
	
	Rem
	bbdoc: 
	End Rem
	Function GetValueTypeLabel:String(valueType:Int)
		Return bmx_flickcurl_photofield_getvaluetypelabel(valueType)
	End Function

End Type

Rem
bbdoc: 
End Rem
Type TFCPhotoField Extends TFCField

	Function _create:TFCPhotoField(svalue:String, ivalue:Int, valueType:Int)
		Local this:TFCPhotoField = New TFCPhotoField
		this.svalue = svalue
		this.ivalue = ivalue
		this.valueType = valueType
		Return this
	End Function

	Function GetLabel:String(fieldType:Int)
		Return bmx_flickcurl_photofield_getlabel(fieldType)
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
bbdoc: A Photo size.
End Rem
Type TFCSize

	Field sizePtr:Byte Ptr
	Field owner:Int
	
	Function _create:TFCSize(sizePtr:Byte Ptr, owner:Int = True)
		If sizePtr Then
			Local this:TFCSize = New TFCSize
			this.sizePtr = sizePtr
			this.owner = owner
			Return this
		End If
	End Function
	
	
	Rem
	bbdoc: The label.
	End Rem
	Method GetLabel:String()
		Return bmx_flickcurl_size_getlabel(sizePtr)
	End Method
	
	Rem
	bbdoc: The width in pixels.
	End Rem
	Method GetWidth:Int()
		Return bmx_flickcurl_size_getwidth(sizePtr)
	End Method
	
	Rem
	bbdoc: The height in pixels.
	End Rem
	Method GetHeight:Int()
		Return bmx_flickcurl_size_getheight(sizePtr)
	End Method
	
	Rem
	bbdoc: The raw image source URL.
	End Rem
	Method GetSource:String()
		Return bmx_flickcurl_size_getsource(sizePtr)
	End Method
	
	Rem
	bbdoc: The url of photo page.
	End Rem
	Method GetURL:String()
		Return bmx_flickcurl_size_geturl(sizePtr)
	End Method
	
	Rem
	bbdoc: Returns 'photo' or 'video'.
	End Rem
	Method GetMedia:String()
		Return bmx_flickcurl_size_getmedia(sizePtr)
	End Method
	
	Rem
	bbdoc: Destructor for size object.
	End Rem
	Method Free()
		If sizePtr And owner Then
			flickcurl_free_size(sizePtr)
			sizePtr = Null
		End If
	End Method

	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TFCSizeList

	Field slPtr:Byte Ptr
	
	Field fcPtr:Byte Ptr

	Function _create:TFCSizeList(slPtr:Byte Ptr, fcPtr:Byte Ptr)
		If slPtr Then
			Local this:TFCSizeList = New TFCSizeList
			this.slPtr = slPtr
			this.fcPtr = fcPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Returns the number of sizes.
	End Rem
	Method GetSizeCount:Int()
		Return bmx_flickcurl_listofsizes_getsizecount(slPtr)
	End Method
	
	Rem
	bbdoc: Returns the size at the given @index.
	End Rem
	Method GetSize:TFCSize(index:Int)
		Return TFCSize._create(bmx_flickcurl_listofsizes_getsize(slPtr, index), False)
	End Method

	Rem
	bbdoc: Destructor for List object.
	End Rem
	Method Free()
		If slPtr Then
			flickcurl_free_sizes(slPtr)
			slPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
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


Rem
bbdoc: A user.
End Rem
Type TFCPerson

	Field personPtr:Byte Ptr
	
	Field fcPtr:Byte Ptr

	Function _create:TFCPerson(personPtr:Byte Ptr, fcPtr:Byte Ptr)
		If personPtr Then
			Local this:TFCPerson = New TFCPerson
			this.personPtr = personPtr
			this.fcPtr = fcPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: The person's userID.
	End Rem
	Method GetUserID:String()
		Return bmx_flickcurl_person_getuserid(personPtr)
	End Method
	
	Rem
	bbdoc: Returns the field at @index.
	about: Valid @index. One of :
<table>
<tr><th>Constant</th><th>Description</th></tr>
<tr><td>PERSON_FIELD_none</td><td> internal</td></tr>
<tr><td>PERSON_FIELD_isadmin</td><td> is admin field boolean</td></tr>
<tr><td>PERSON_FIELD_ispro</td><td>  is pro field boolean</td></tr>
<tr><td>PERSON_FIELD_iconserver</td><td> icon server integer</td></tr>
<tr><td>PERSON_FIELD_iconfarm</td><td> icon farm integer</td></tr>
<tr><td>PERSON_FIELD_username</td><td> username</td></tr>
<tr><td>PERSON_FIELD_realname</td><td> real name</td></tr>
<tr><td>PERSON_FIELD_mbox_sha1sum</td><td> Email SHA1 sum</td></tr>
<tr><td>PERSON_FIELD_location</td><td> location</td></tr>
<tr><td>PERSON_FIELD_photosurl</td><td> photos URL</td></tr>
<tr><td>PERSON_FIELD_profileurl</td><td> profile URL</td></tr>
<tr><td>PERSON_FIELD_mobileurl</td><td> mobile URL</td></tr>
<tr><td>PERSON_FIELD_photos_firstdate</td><td> photos first date</td></tr>
<tr><td>PERSON_FIELD_photos_firstdatetaken</td><td> photos first date taken</td></tr>
<tr><td>PERSON_FIELD_photos_count</td><td> photos count</td></tr>
<tr><td>PERSON_FIELD_photos_views</td><td> photos views</td></tr>
<tr><td>PERSON_FIELD_favedate</td><td> favorite date</td></tr>
<tr><td>PERSON_FIELD_FIRST</td><td> internal offset to first field</td></tr>
<tr><td>PERSON_FIELD_LAST</td><td> internal offset to last field</td></tr>
</table>
End Rem
	Method GetField:TFCPersonField(index:Int)
		Return TFCPersonField(bmx_flickcurl_person_getfield(personPtr, index))
	End Method


	Rem
	bbdoc: Returns a list of the user's favorite photos. 
	End Rem
	Method GetFavoritesList:TFCPhotoList(extras:String, perPage:Int, page:Int)
		Return TFCListOfPhotos._create(bmx_flickcurl_getfavoriteslist(fcPtr, personPtr, extras, perPage, page), fcPtr)
	End Method
	
'	Method GetFavoritesListParams:TFCPhotoList(userId:String, params:TFCPhotoListParams)
'	End Method

	Rem
	bbdoc: Returns a list of favorite public photos for the given user. 
	End Rem
	Method GetPublicFavoritesList:TFCPhotoList(extras:String, perPage:Int, page:Int)
		Return TFCListOfPhotos._create(bmx_flickcurl_getpublicfavoriteslist(fcPtr, personPtr, extras, perPage, page), fcPtr)
	End Method
	
'	Method GetPublicFavoritesListParams:TFCPhotoList(userId:String, params:TFCPhotoListParams)
'	End Method
	

End Type

Rem
bbdoc: Person details.
End Rem
Type TFCPersonField Extends TFCField

	Function _create:TFCPersonField(svalue:String, ivalue:Int, valueType:Int)
		Local this:TFCPersonField = New TFCPersonField
		this.svalue = svalue
		this.ivalue = ivalue
		this.valueType = valueType
		Return this
	End Function

	Rem
	bbdoc: 
	End Rem
	Function GetLabel:String(fieldType:Int)
		Return bmx_flickcurl_personfield_getlabel(fieldType)
	End Function
	
End Type

Rem
bbdoc: A group.
End Rem
Type TFCGroup

	Field groupPtr:Byte Ptr
	
	Field owner:Int
	
	Field fcPtr:Byte Ptr
	
	Function _create:TFCGroup(groupPtr:Byte Ptr, fcPtr:Byte Ptr, owner:Int = True)
		If groupPtr Then
			Local this:TFCGroup = New TFCGroup
			this.groupPtr = groupPtr
			this.fcPtr = fcPtr
			this.owner = owner
			Return this
		End If
	End Function
	
	Rem
	bbdoc: The group's NSID.
	End Rem
	Method GetGroupID:String()
		Return bmx_flickcurl_group_getgroupid(groupPtr)
	End Method
	
	Rem
	bbdoc: Group name.
	End Rem
	Method GetName:String()
		Return bmx_flickcurl_group_getname(groupPtr)
	End Method
	
	Rem
	bbdoc: Description.
	End Rem
	Method GetDescription:String()
		Return bmx_flickcurl_group_getdescription(groupPtr)
	End Method
	
	Rem
	bbdoc: Language.
	End Rem
	Method GetLang:String()
		Return bmx_flickcurl_group_getlang(groupPtr)
	End Method
	
	Rem
	bbdoc: is admin flag
	End Rem
	Method IsAdmin:Int()
		Return bmx_flickcurl_group_isadmin(groupPtr)
	End Method
	
	Rem
	bbdoc: is the pool moderated
	End Rem
	Method IsPoolModerated:Int()
		Return bmx_flickcurl_group_ispoolmoderated(groupPtr)
	End Method
	
	Rem
	bbdoc: 18+ group
	End Rem
	Method IsEighteenPlus:Int()
		Return bmx_flickcurl_group_iseighteenplus(groupPtr)
	End Method
	
	Rem
	bbdoc: privacy level
	End Rem
	Method GetPrivacy:Int()
		Return bmx_flickcurl_group_getprivacy(groupPtr)
	End Method
	
	Rem
	bbdoc: photos in group count
	End Rem
	Method GetPhotos:Int()
		Return bmx_flickcurl_group_getphotos(groupPtr)
	End Method
	
	Rem
	bbdoc: icon server ID
	End Rem
	Method GetIconServer:Int()
		Return bmx_flickcurl_group_geticonserver(groupPtr)
	End Method
	
	Rem
	bbdoc: member count 
	End Rem
	Method GetMembers:Int()
		Return bmx_flickcurl_group_getmembers(groupPtr)
	End Method
	
	Rem
	bbdoc: throttle mode (day, ...)
	End Rem
	Method GetThrottleMode:String()
		Return bmx_flickcurl_group_getthrottlemode(groupPtr)
	End Method
	
	Rem
	bbdoc: throttle count
	End Rem
	Method GetThrottleCount:Int()
		Return bmx_flickcurl_group_getthrottlecount(groupPtr)
	End Method
	
	Rem
	bbdoc: throttle remaining
	End Rem
	Method GetThrottleRemaining:Int()
		Return bmx_flickcurl_group_getthrottleremaining(groupPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method Free()
		If groupPtr And owner Then
			flickcurl_free_group(groupPtr)
			groupPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TFCGroupList

	Field glPtr:Byte Ptr
	
	Field fcPtr:Byte Ptr

	Function _create:TFCGroupList(glPtr:Byte Ptr, fcPtr:Byte Ptr)
		If glPtr Then
			Local this:TFCGroupList = New TFCGroupList
			this.glPtr = glPtr
			this.fcPtr = fcPtr
			Return this
		End If
	End Function
	
	Rem
	bbdoc: Returns the number of groups.
	End Rem
	Method GetGroupCount:Int()
		Return bmx_flickcurl_listofgroups_getgroupcount(glPtr)
	End Method
	
	Rem
	bbdoc: Returns the group at the given @index.
	End Rem
	Method GetGroup:TFCGroup(index:Int)
		Return TFCGroup._create(bmx_flickcurl_listofgroups_getgroup(glPtr, index), fcPtr, False)
	End Method

	Rem
	bbdoc: Destructor for List object.
	End Rem
	Method Free()
		If glPtr Then
			flickcurl_free_groups(glPtr)
			glPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type



