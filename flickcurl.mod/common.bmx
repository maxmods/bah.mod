SuperStrict

Import "source.bmx"

Extern


	Function bmx_flickcurl_new:Byte Ptr()
	Function bmx_flickcurl_free(handle:Byte Ptr)

	Function bmx_flickcurl_setapikey(handle:Byte Ptr, apiKey:String)
	Function bmx_flickcurl_setauthtoken(handle:Byte Ptr, authToken:String)
	Function bmx_flickcurl_setsharedsecret(handle:Byte Ptr, sharedSecret:String)

	Function bmx_flickcurl_photosgetinfo:Byte Ptr(handle:Byte Ptr, photoID:String)

	Function bmx_flickcurl_photo_getfield:Object(handle:Byte Ptr, index:Int)
	Function bmx_flickcurl_photo_geturi:String(handle:Byte Ptr)

	Function bmx_flickcurl_photofield_getlabel:String(fieldType:Int)
	Function bmx_flickcurl_photofield_getvaluetypelabel:String(valueType:Int)

	Function bmx_flickcurl_getfrob:String(handle:Byte Ptr)
	Function bmx_flickcurl_getfulltoken:String(handle:Byte Ptr, frob:String)



	Function flickcurl_init:Int()
	Function flickcurl_finish()


End Extern




Const PHOTO_FIELD_none:Int = 0
Const PHOTO_FIELD_dateuploaded:Int = 1
Const PHOTO_FIELD_farm:Int = 2
Const PHOTO_FIELD_isfavorite:Int = 3
Const PHOTO_FIELD_license:Int = 4
Const PHOTO_FIELD_originalformat:Int = 5
Const PHOTO_FIELD_rotation:Int = 6
Const PHOTO_FIELD_server:Int = 7
Const PHOTO_FIELD_dates_lastupdate:Int = 8
Const PHOTO_FIELD_dates_posted:Int = 9
Const PHOTO_FIELD_dates_taken:Int = 10
Const PHOTO_FIELD_dates_takengranularity:Int = 11
Const PHOTO_FIELD_description:Int = 12
Const PHOTO_FIELD_editability_canaddmeta:Int = 13
Const PHOTO_FIELD_editability_cancomment:Int = 14
Const PHOTO_FIELD_geoperms_iscontact:Int = 15
Const PHOTO_FIELD_geoperms_isfamily:Int = 16
Const PHOTO_FIELD_geoperms_isfriend:Int = 17
Const PHOTO_FIELD_geoperms_ispublic:Int = 18
Const PHOTO_FIELD_location_accuracy:Int = 19
Const PHOTO_FIELD_location_latitude:Int = 20
Const PHOTO_FIELD_location_longitude:Int = 21
Const PHOTO_FIELD_owner_location:Int = 22
Const PHOTO_FIELD_owner_nsid:Int = 23
Const PHOTO_FIELD_owner_realname:Int = 24
Const PHOTO_FIELD_owner_username:Int = 25
Const PHOTO_FIELD_title:Int = 26
Const PHOTO_FIELD_visibility_isfamily:Int = 27
Const PHOTO_FIELD_visibility_isfriend:Int = 28
Const PHOTO_FIELD_visibility_ispublic:Int = 29
Const PHOTO_FIELD_secret:Int = 30
Const PHOTO_FIELD_originalsecret:Int = 31
Const PHOTO_FIELD_location_neighbourhood:Int = 32
Const PHOTO_FIELD_location_neighborhood:Int = PHOTO_FIELD_location_neighbourhood
Const PHOTO_FIELD_location_locality:Int = 33
Const PHOTO_FIELD_location_county:Int = 34
Const PHOTO_FIELD_location_region:Int = 35
Const PHOTO_FIELD_location_country:Int = 36
Const PHOTO_FIELD_location_placeid:Int = 37
Const PHOTO_FIELD_neighbourhood_placeid:Int = 38
Const PHOTO_FIELD_neighborhood_placeid:Int = PHOTO_FIELD_neighbourhood_placeid
Const PHOTO_FIELD_locality_placeid:Int = 39
Const PHOTO_FIELD_county_placeid:Int = 40
Const PHOTO_FIELD_region_placeid:Int = 41
Const PHOTO_FIELD_country_placeid:Int = 42
Const PHOTO_FIELD_location_woeid:Int = 43
Const PHOTO_FIELD_neighbourhood_woeid:Int = 44
Const PHOTO_FIELD_neighborhood_woeid:Int = PHOTO_FIELD_neighbourhood_woeid
Const PHOTO_FIELD_locality_woeid:Int = 45
Const PHOTO_FIELD_county_woeid:Int = 46
Const PHOTO_FIELD_region_woeid:Int = 47
Const PHOTO_FIELD_country_woeid:Int = 48
Const PHOTO_FIELD_usage_candownload:Int = 49
Const PHOTO_FIELD_usage_canblog:Int = 50
Const PHOTO_FIELD_usage_canprint:Int = 51
Const PHOTO_FIELD_FIRST:Int = PHOTO_FIELD_dateuploaded
Const PHOTO_FIELD_LAST:Int = PHOTO_FIELD_usage_canprint


Const VALUE_TYPE_NONE:Int = 0
Const VALUE_TYPE_PHOTO_ID:Int = 1
Const VALUE_TYPE_PHOTO_URI:Int = 2
Const VALUE_TYPE_UNIXTIME:Int = 3
Const VALUE_TYPE_BOOLEAN:Int = 4
Const VALUE_TYPE_DATETIME:Int = 5
Const VALUE_TYPE_FLOAT:Int = 6
Const VALUE_TYPE_INTEGER:Int = 7
Const VALUE_TYPE_STRING:Int = 8
Const VALUE_TYPE_URI:Int = 9
Const VALUE_TYPE_PERSON_ID:Int = 10
Const VALUE_TYPE_MEDIA_TYPE:Int = 11
Const VALUE_TYPE_LAST:Int = VALUE_TYPE_MEDIA_TYPE

