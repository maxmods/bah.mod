' Copyright 2008-2011 Bruce A Henderson
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

Import "source.bmx"

Extern


	Function bmx_flickcurl_new:Byte Ptr()
	Function bmx_flickcurl_free(handle:Byte Ptr)

	Function bmx_flickcurl_setapikey(handle:Byte Ptr, apiKey:String)
	Function bmx_flickcurl_setauthtoken(handle:Byte Ptr, authToken:String)
	Function bmx_flickcurl_setsharedsecret(handle:Byte Ptr, sharedSecret:String)

	Function bmx_flickcurl_photosgetinfo:Byte Ptr(handle:Byte Ptr, photoID:String)
	Function bmx_flickcurl_photos_license_getinfo:Byte Ptr(handle:Byte Ptr)
	Function bmx_flickcurl_photos_license_getinfobyid:Object(handle:Byte Ptr, id:Int)

	Function bmx_flickcurl_photo_getid:String(handle:Byte Ptr)
	Function bmx_flickcurl_photo_getfield:Object(handle:Byte Ptr, index:Int)
	Function bmx_flickcurl_photo_geturi:String(handle:Byte Ptr)
	Function bmx_flickcurl_photo_getsourceuri:String(handle:Byte Ptr, size:Int)
	Function bmx_flickcurl_photo_free(handle:Byte Ptr)
	Function bmx_flickcurl_photo_getplace:Byte Ptr(handle:Byte Ptr)
	Function bmx_flickcurl_photo_gettag:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_flickcurl_photo_gettagcount:Int(handle:Byte Ptr)
	Function bmx_flickcurl_photo_addtags:Int(handle:Byte Ptr, photo:Byte Ptr, tags:String)
	Function bmx_flickcurl_photo_addcomment:Object(handle:Byte Ptr, photo:Byte Ptr, comment:String)
	Function bmx_flickcurl_photo_getcommentlist:Byte Ptr(handle:Byte Ptr, photo:Byte Ptr)
	Function bmx_flickcurl_photo_setsafetylevel:Int(handle:Byte Ptr, photo:Byte Ptr, safetyLevel:Int, hidden:Int)
	Function bmx_flickcurl_photo_settags:Int(handle:Byte Ptr, photo:Byte Ptr, tags:String)
	Function bmx_flickcurl_photo_transformrotate:Int(handle:Byte Ptr, photo:Byte Ptr, degrees:Int)
	Function bmx_flickcurl_photo_removefavorite:Int(handle:Byte Ptr, photo:Byte Ptr)
	Function bmx_flickcurl_photo_addfavorite:Int(handle:Byte Ptr, photo:Byte Ptr)
	Function bmx_flickcurl_photo_setcontenttype:Int(handle:Byte Ptr, photo:Byte Ptr, contentType:Int)
	Function bmx_flickcurl_photo_setdates:Int(handle:Byte Ptr, photo:Byte Ptr, datePosted:Int, dateTaken:Int, dateTakenGranularity:Int)
	Function bmx_flickcurl_photo_setmeta:Int(handle:Byte Ptr, photo:Byte Ptr, title:String, description:String)
	Function bmx_flickcurl_photo_getperms:Byte Ptr(handle:Byte Ptr, photo:Byte Ptr)
	Function bmx_flickcurl_photo_getgeoperms:Byte Ptr(handle:Byte Ptr, photo:Byte Ptr)
	Function bmx_flickcurl_photo_setgeoperms:Int(handle:Byte Ptr, photo:Byte Ptr, permissions:Byte Ptr)
	Function bmx_flickcurl_photo_setperms:Int(handle:Byte Ptr, photo:Byte Ptr, permissions:Byte Ptr)
	Function bmx_flickcurl_photo_setlicense:Int(handle:Byte Ptr, photo:Byte Ptr, licenseId:Int)
	Function bmx_flickcurl_photo_addnote:String(handle:Byte Ptr, photo:Byte Ptr, x:Int, y:Int, w:Int, h:Int, text:String)
	Function bmx_flickcurl_photo_delete:Int(handle:Byte Ptr, photo:Byte Ptr)

	Function bmx_flickcurl_photofield_getlabel:String(fieldType:Int)
	Function bmx_flickcurl_photofield_getvaluetypelabel:String(valueType:Int)

	Function bmx_flickcurl_getfrob:String(handle:Byte Ptr)
	Function bmx_flickcurl_getfulltoken:String(handle:Byte Ptr, frob:String)
	Function bmx_flickcurl_getToken:String(handle:Byte Ptr, frob:String)

	Function bmx_flickcurl_resolveplaceid:Byte Ptr(handle:Byte Ptr, placeID:String)
	Function bmx_flickcurl_resolveplaceurl:Byte Ptr(handle:Byte Ptr, url:String)
	Function bmx_flickcurl_findplacebylatlon:Byte Ptr(handle:Byte Ptr, lat:Double, lon:Double, accuracy:Int)
	Function bmx_flickcurl_placesfind:Byte Ptr(handle:Byte Ptr, query:String)

	Function bmx_flickcurl_getprefscontenttype:Int(handle:Byte Ptr)
	Function bmx_flickcurl_getprefsgeoperms:Int(handle:Byte Ptr)
	Function bmx_flickcurl_getprefshidden:Int(handle:Byte Ptr)
	Function bmx_flickcurl_getprefsprivacy:Int(handle:Byte Ptr)
	Function bmx_flickcurl_getprefssafetylevel:Int(handle:Byte Ptr)

	Function bmx_flickcurl_place_gettypelabel:String(placeType:Int)
	Function bmx_flickcurl_place_getname:String(handle:Byte Ptr, index:Int)
	Function bmx_flickcurl_place_getid:String(handle:Byte Ptr, index:Int)
	Function bmx_flickcurl_place_geturl:String(handle:Byte Ptr, index:Int)
	Function bmx_flickcurl_place_gettype:Int(handle:Byte Ptr)
	Function bmx_flickcurl_place_getwoeid:String(handle:Byte Ptr, index:Int)
	Function bmx_flickcurl_place_getlocation:Object(handle:Byte Ptr)
	Function bmx_flickcurl_place_getcount:Int(handle:Byte Ptr)
	Function bmx_flickcurl_place_free(handle:Byte Ptr)
	Function bmx_flickcurl_place_gettypebylabel:Int(label:String)

	Function bmx_flickcurl_searchparams_new:Byte Ptr()
	Function bmx_flickcurl_searchparams_setuserid(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_settags(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_settagmode(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_settext(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_setminuploaddate(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_searchparams_setmaxuploaddate(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_searchparams_setmintakendate(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_setmaxtakendate(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_setlicense(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_setsort(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_setprivacyfilter(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_setbbox(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_setaccuracy(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_searchparams_setsafesearch(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_searchparams_setcontenttype(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_searchparams_setmachinetags(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_setmachinetagmode(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_setgroupid(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_setextras(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_setperpage(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_searchparams_setpage(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_searchparams_setplaceid(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_setmedia(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_sethasgeo(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_searchparams_setlat(handle:Byte Ptr, value:Double)
	Function bmx_flickcurl_searchparams_setlon(handle:Byte Ptr, value:Double)
	Function bmx_flickcurl_searchparams_setradius(handle:Byte Ptr, value:Double)
	Function bmx_flickcurl_searchparams_setradiusunits(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_setcontacts(handle:Byte Ptr, value:String)
	Function bmx_flickcurl_searchparams_setwoeid(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_searchparams_setgeocontext(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_searchparams_setiscommons(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_searchparams_setingallery(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_searchparams_delete(handle:Byte Ptr)

	Function bmx_flickcurl_tag_getphoto:Byte Ptr(handle:Byte Ptr)
	Function bmx_flickcurl_tag_getid:String(handle:Byte Ptr)
	Function bmx_flickcurl_tag_getauthor:String(handle:Byte Ptr)
	Function bmx_flickcurl_tag_getauthorname:String(handle:Byte Ptr)
	Function bmx_flickcurl_tag_getraw:String(handle:Byte Ptr)
	Function bmx_flickcurl_tag_getcooked:String(handle:Byte Ptr)
	Function bmx_flickcurl_tag_getmachinetag:Int(handle:Byte Ptr)
	Function bmx_flickcurl_tag_getcount:Int(handle:Byte Ptr)
	Function bmx_flickcurl_tag_remove:Int(handle:Byte Ptr, tag:Byte Ptr)
	Function bmx_flickcurl_tag_removetxt:Int(handle:Byte Ptr, tag:String)

	Function bmx_flickcurl_commentlist_getcomment:Object(handle:Byte Ptr, index:Int, fc:Byte Ptr)
	
	Function bmx_flickcurl_comment_deletecomment:Int(handle:Byte Ptr, id:String)
	Function bmx_flickcurl_comment_editcomment:Int(handle:Byte Ptr, id:String, commentText:String)

	Function bmx_flickcurl_photolist_getformat:String(handle:Byte Ptr)
	Function bmx_flickcurl_photolist_getphotocount:Int(handle:Byte Ptr)
	Function bmx_flickcurl_photolist_getphoto:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_flickcurl_photolist_getcontent:String(handle:Byte Ptr)

	Function bmx_flickcurl_searchphotos:Byte Ptr(handle:Byte Ptr, params:Byte Ptr)
	Function bmx_flickcurl_listofphotos_getphotocount:Int(handle:Byte Ptr)
	Function bmx_flickcurl_listofphotos_getphoto:Byte Ptr(handle:Byte Ptr, index:Int)

	Function bmx_flickcurl_getfavoriteslist:Byte Ptr(handle:Byte Ptr, person:Byte Ptr, extras:String, perPage:Int, page:Int)
	Function bmx_flickcurl_getpublicfavoriteslist:Byte Ptr(handle:Byte Ptr, person:Byte Ptr, extras:String, perPage:Int, page:Int)

	Function bmx_flickcurl_size_getlabel:String(handle:Byte Ptr)
	Function bmx_flickcurl_size_getwidth:Int(handle:Byte Ptr)
	Function bmx_flickcurl_size_getheight:Int(handle:Byte Ptr)
	Function bmx_flickcurl_size_getsource:String(handle:Byte Ptr)
	Function bmx_flickcurl_size_geturl:String(handle:Byte Ptr)
	Function bmx_flickcurl_size_getmedia:String(handle:Byte Ptr)

	Function bmx_flickcurl_listofsizes_getsizecount:Int(handle:Byte Ptr)
	Function bmx_flickcurl_listofsizes_getsize:Byte Ptr(handle:Byte Ptr, index:Int)

	Function bmx_flickcurl_getinterestingnesslist:Byte Ptr(handle:Byte Ptr, date:String, extras:String, perPage:Int, page:Int)

	Function bmx_flickcurl_personfield_getlabel:String(fieldType:Int)
	Function bmx_flickcurl_person_getuserid:String(handle:Byte Ptr)
	Function bmx_flickcurl_person_getfield:Object(handle:Byte Ptr, index:Int)
	Function bmx_flickcurl_person_getphotosurl:String(handle:Byte Ptr, person:Byte Ptr)
	Function bmx_flickcurl_person_getprofileurl:String(handle:Byte Ptr, person:Byte Ptr)

	Function bmx_flickcurl_findpeoplebyemail:Byte Ptr(handle:Byte Ptr, email:String)
	Function bmx_flickcurl_findpeoplebyusername:Byte Ptr(handle:Byte Ptr, username:String)
	Function bmx_flickcurl_people_getinfo:Byte Ptr(handle:Byte Ptr, userID:String)
	Function bmx_flickcurl_url_lookupuser:Byte Ptr(handle:Byte Ptr, url:String)
	Function bmx_flickcurl_url_lookupgroup:Byte Ptr(handle:Byte Ptr, url:String, lang:String)
	Function bmx_flickcurl_groups_getinfo:Byte Ptr(handle:Byte Ptr, groupID:String, lang:String)

	Function bmx_flickcurl_group_getgroupid:String(handle:Byte Ptr)
	Function bmx_flickcurl_group_getname:String(handle:Byte Ptr)
	Function bmx_flickcurl_group_getdescription:String(handle:Byte Ptr)
	Function bmx_flickcurl_group_getlang:String(handle:Byte Ptr)
	Function bmx_flickcurl_group_isadmin:Int(handle:Byte Ptr)
	Function bmx_flickcurl_group_ispoolmoderated:Int(handle:Byte Ptr)
	Function bmx_flickcurl_group_iseighteenplus:Int(handle:Byte Ptr)
	Function bmx_flickcurl_group_getprivacy:Int(handle:Byte Ptr)
	Function bmx_flickcurl_group_getphotos:Int(handle:Byte Ptr)
	Function bmx_flickcurl_group_geticonserver:Int(handle:Byte Ptr)
	Function bmx_flickcurl_group_getmembers:Int(handle:Byte Ptr)
	Function bmx_flickcurl_group_getthrottlemode:String(handle:Byte Ptr)
	Function bmx_flickcurl_group_getthrottlecount:Int(handle:Byte Ptr)
	Function bmx_flickcurl_group_getthrottleremaining:Int(handle:Byte Ptr)
	Function bmx_flickcurl_group_geturl:String(handle:Byte Ptr, group:Byte Ptr)
	Function bmx_flickcurl_group_pools_add:Int(handle:Byte Ptr, group:Byte Ptr, photo:Byte Ptr)

	Function bmx_flickcurl_listofgroups_getgroupcount:Int(handle:Byte Ptr)
	Function bmx_flickcurl_listofgroups_getgroup:Byte Ptr(handle:Byte Ptr, index:Int)

	Function bmx_flickcurl_perms_ispublic:Int(handle:Byte Ptr)
	Function bmx_flickcurl_perms_setpublic(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_perms_iscontact:Int(handle:Byte Ptr)
	Function bmx_flickcurl_perms_setcontact(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_perms_isfriend:Int(handle:Byte Ptr)
	Function bmx_flickcurl_perms_setfriend(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_perms_isfamily:Int(handle:Byte Ptr)
	Function bmx_flickcurl_perms_setfamily(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_perms_getpermcomment:Int(handle:Byte Ptr)
	Function bmx_flickcurl_perms_setpermcomment(handle:Byte Ptr, value:Int)
	Function bmx_flickcurl_perms_getpermaddmeta:Int(handle:Byte Ptr)
	Function bmx_flickcurl_perms_setpermaddmeta(handle:Byte Ptr, value:Int)
	
	Function bmx_flickcurl_perms_create:Byte Ptr()
	Function bmx_flickcurl_perms_delete(handle:Byte Ptr)

	Function bmx_flickcurl_notes_deletenote:Int(handle:Byte Ptr, noteID:String)
	Function bmx_flickcurl_notes_editnote:Int(handle:Byte Ptr, noteID:String, x:Int, y:Int, w:Int, h:Int, text:String)

	Function bmx_flickcurl_listoflicenses_getlicensecount:Int(handle:Byte Ptr)
	Function bmx_flickcurl_listoflicenses_getlicense:Object(handle:Byte Ptr, index:Int)

	Function bmx_flickcurl_panda_getlist:String[](handle:Byte Ptr)
	Function bmx_flickcurl_panda_getphotos:Byte Ptr(handle:Byte Ptr, pandaName:String)

	Function bmx_flickcurl_listofactivities_getactivitycount:Int(handle:Byte Ptr)
	Function bmx_flickcurl_listofactivities_getactivity:Byte Ptr(handle:Byte Ptr, index:Int)
	Function bmx_flickcurl_activity_usercomments:Byte Ptr(handle:Byte Ptr, perPage:Int, page:Int)
	Function bmx_flickcurl_activity_userphotos:Byte Ptr(handle:Byte Ptr, timeFrame:String, perPage:Int, page:Int)

	Function bmx_flickcurl_activity_gettype:String(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getowner:String(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getownername:String(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getprimary:String(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getid:String(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getsecret:String(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getserver:Int(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getfarm:Int(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getoldcomments:Int(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getnewcomments:Int(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getoldnotes:Int(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getnewnotes:Int(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getviews:Int(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getcomments:Int(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getphotos:Int(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getfaves:Int(handle:Byte Ptr)
	Function bmx_flickcurl_activity_getmore:Int(handle:Byte Ptr)
	Function bmx_flickcurl_activity_gettitle:String(handle:Byte Ptr)

	Function bmx_flickcurl_commons_getinstitutions:Byte Ptr(handle:Byte Ptr)
	Function bmx_flickcurl_listofinstitutions_getinstitutioncount:Int(handle:Byte Ptr)
	Function bmx_flickcurl_listofinstitutions_getinstitution:Object(handle:Byte Ptr, index:Int)
	Function bmx_flickcurl_institution_url_type_label:String(urlType:Int)

	Function bmx_flickcurl_contacts_getlist:Byte Ptr(handle:Byte Ptr, filter:String, page:Int, perPage:Int)
	Function bmx_flickcurl_contacts_getlistrecentlyuploaded:Byte Ptr(handle:Byte Ptr, dateLastUpload:Int, filter:String)
	Function bmx_flickcurl_contacts_getpubliclist:Byte Ptr(handle:Byte Ptr, user:String, page:Int, perPage:Int)
	Function bmx_flickcurl_listofcontacts_getcontactcount:Int(handle:Byte Ptr)
	Function bmx_flickcurl_listofcontacts_getcontact:Object(handle:Byte Ptr, index:Int)

	Function bmx_flickcurl_getcontactsphotos:Byte Ptr(handle:Byte Ptr, contactCount:Int, justFriends:Int, singlePhoto:Int, ..
			includeSelf:Int, extras:String)
	Function bmx_flickcurl_getcontactspublicphotos:Byte Ptr(handle:Byte Ptr, user:String, photoCount:Int, justFriends:Int, ..
			singlePhoto:Int, includeSelf:Int, extras:String)

	Function bmx_flickcurl_listofplaces_getplacecount:Int(handle:Byte Ptr)
	Function bmx_flickcurl_listofplaces_getplace:Byte Ptr(handle:Byte Ptr, index:Int)


	' API direct functions
	Function flickcurl_init:Int()
	Function flickcurl_finish()
	Function flickcurl_free_comments(handle:Byte Ptr)
	Function flickcurl_free_photos_list(handle:Byte Ptr)
	Function flickcurl_free_photos(handle:Byte Ptr)
	Function flickcurl_free_size(handle:Byte Ptr)
	Function flickcurl_free_sizes(handle:Byte Ptr)
	Function flickcurl_free_group(handle:Byte Ptr)
	Function flickcurl_free_groups(handle:Byte Ptr)
	Function flickcurl_free_perms(handle:Byte Ptr)
	Function flickcurl_free_activities(handle:Byte Ptr)
	Function flickcurl_free_institutions(handle:Byte Ptr)
	Function flickcurl_free_contacts(handle:Byte Ptr)
	Function flickcurl_free_places(handle:Byte Ptr)

End Extern


Const FCIMAGE_SQUARE:Int = 1
Const FCIMAGE_THUMB:Int = 2
Const FCIMAGE_SMALL:Int = 3
Const FCIMAGE_MEDIUM:Int = 4
Const FCIMAGE_LARGE:Int = 5
Const FCIMAGE_ORIGINAL:Int = 6

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
Const PHOTO_FIELD_owner_iconserver:Int = 52
Const PHOTO_FIELD_owner_iconfarm:Int = 53
Const PHOTO_FIELD_original_width:Int = 54
Const PHOTO_FIELD_original_height:Int = 55
Const PHOTO_FIELD_views:Int = 56
Const PHOTO_FIELD_comments:Int = 57
Const PHOTO_FIELD_favorites:Int = 58
Const PHOTO_FIELD_gallery_comment:Int = 59
Const PHOTO_FIELD_FIRST:Int = PHOTO_FIELD_dateuploaded
Const PHOTO_FIELD_LAST:Int = PHOTO_FIELD_gallery_comment


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
Const VALUE_TYPE_TAG_STRING:Int = 12
Const VALUE_TYPE_COLLECTION_ID:Int = 13
Const VALUE_TYPE_ICON_PHOTOS:Int = 14
Const VALUE_TYPE_LAST:Int = VALUE_TYPE_ICON_PHOTOS

Const FLICKCURL_PLACE_LOCATION:Int = 0
Const FLICKCURL_PLACE_NEIGHBOURHOOD:Int = 1
Const FLICKCURL_PLACE_NEIGHBORHOOD:Int = FLICKCURL_PLACE_NEIGHBOURHOOD
Const FLICKCURL_PLACE_LOCALITY:Int = 2
Const FLICKCURL_PLACE_COUNTY:Int = 3
Const FLICKCURL_PLACE_REGION:Int = 4
Const FLICKCURL_PLACE_COUNTRY:Int = 5
Const FLICKCURL_PLACE_CONTINENT:Int = 6
Const FLICKCURL_PLACE_LAST:Int = FLICKCURL_PLACE_CONTINENT

Const PERSON_FIELD_none:Int = 0
Const PERSON_FIELD_isadmin:Int = 1
Const PERSON_FIELD_ispro:Int = 2
Const PERSON_FIELD_iconserver:Int = 3
Const PERSON_FIELD_iconfarm:Int = 4
Const PERSON_FIELD_username:Int = 5
Const PERSON_FIELD_realname:Int = 6
Const PERSON_FIELD_mbox_sha1sum:Int = 7
Const PERSON_FIELD_location:Int = 8
Const PERSON_FIELD_photosurl:Int = 9
Const PERSON_FIELD_profileurl:Int = 10
Const PERSON_FIELD_mobileurl:Int = 11
Const PERSON_FIELD_photos_firstdate:Int = 12
Const PERSON_FIELD_photos_firstdatetaken:Int = 13
Const PERSON_FIELD_photos_count:Int = 14
Const PERSON_FIELD_photos_views:Int = 15
Const PERSON_FIELD_favedate:Int = 16
Const PERSON_FIELD_FIRST:Int = PERSON_FIELD_isadmin
Const PERSON_FIELD_LAST:Int = PERSON_FIELD_favedate

Const FLICKCURL_INSTITUTION_URL_NONE:Int = 0
Const FLICKCURL_INSTITUTION_URL_SITE:Int = 1
Const FLICKCURL_INSTITUTION_URL_LICENSE:Int = 2
Const FLICKCURL_INSTITUTION_URL_FLICKR:Int = 3

