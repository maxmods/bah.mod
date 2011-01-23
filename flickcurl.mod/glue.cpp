/*
  Copyright 2008-20011 Bruce A Henderson

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
 
      http://www.apache.org/licenses/LICENSE-2.0
 
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

*/
#include "flickcurl.h"

extern "C" {

#include "blitz.h"

	BBObject * _bah_flickcurl_TFCPhotoField__create(BBString * svalue, flickcurl_photo_field_type value, flickcurl_field_value_type type);
	BBObject * _bah_flickcurl_TFCLocation__create(double latitude, double longitude, int accuracy);
	BBObject * _bah_flickcurl_TFCComment__create(BBString * id, BBString * author, BBString * authorname, int datecreate, BBString * permalink, BBString * text, flickcurl * fc);
	BBObject * _bah_flickcurl_TFCPersonField__create(BBString * svalue, flickcurl_person_field_type value, flickcurl_field_value_type type);
	BBObject * _bah_flickcurl_TFCLicense__create(int id, BBString * url, BBString * name);
	BBObject * _bah_flickcurl_TFCInstitution__create(BBString * id, int date_launch, BBString * name, BBArray * pa);
	BBObject * _bah_flickcurl_TFCContact__create(BBString * nsid, BBString * username, int iconserver, BBString * realname,
		int is_friend, int is_family, int ignored, int uploaded);

	BBArray * _bah_flickcurl_TFCTagCluster__createClusterArray(int size);
	BBObject * _bah_flickcurl_TFCTagCluster__setCluster(BBArray * array, int index, int size);
	void _bah_flickcurl_TFCTagCluster__setTag(BBObject * cluster, int index, BBString * tag);

	flickcurl * bmx_flickcurl_new();
	void bmx_flickcurl_free(flickcurl * fc);

	void bmx_flickcurl_setapikey(flickcurl * fc, BBString * apiKey);
	void bmx_flickcurl_setauthtoken(flickcurl * fc, BBString * authToken);
	void bmx_flickcurl_setsharedsecret(flickcurl * fc, BBString * sharedSecret);

	BBString * bmx_flickcurl_getfrob(flickcurl * fc);
	BBString * bmx_flickcurl_getfulltoken(flickcurl * fc, BBString * frob);
	BBString * bmx_flickcurl_getToken(flickcurl * fc, BBString * frob);

	flickcurl_photo * bmx_flickcurl_photosgetinfo(flickcurl * fc, BBString * photoID);
	flickcurl_license ** bmx_flickcurl_photos_license_getinfo(flickcurl * fc);
	BBObject * bmx_flickcurl_photos_license_getinfobyid(flickcurl * fc, int id);

	BBString * bmx_flickcurl_photo_getid(flickcurl_photo * photo);
	BBObject * bmx_flickcurl_photo_getfield(flickcurl_photo * photo, int index);
	BBString * bmx_flickcurl_photo_geturi(flickcurl_photo * photo);
	BBString * bmx_flickcurl_photo_getsourceuri(flickcurl_photo * photo, int size);
	void bmx_flickcurl_photo_free(flickcurl_photo * photo);
	flickcurl_place * bmx_flickcurl_photo_getplace(flickcurl_photo * photo);
	flickcurl_tag * bmx_flickcurl_photo_gettag(flickcurl_photo * photo, int index);
	int bmx_flickcurl_photo_gettagcount(flickcurl_photo * photo);
	int bmx_flickcurl_photo_addtags(flickcurl * fc, flickcurl_photo * photo, BBString * tags);
	BBObject * bmx_flickcurl_photo_addcomment(flickcurl * fc, flickcurl_photo * photo, BBString * comment);
	flickcurl_comment ** bmx_flickcurl_photo_getcommentlist(flickcurl * fc, flickcurl_photo * photo);
	int bmx_flickcurl_photo_setsafetylevel(flickcurl * fc, flickcurl_photo * photo, int safetyLevel, int hidden);
	int bmx_flickcurl_photo_settags(flickcurl * fc, flickcurl_photo * photo, BBString * tags);
	int bmx_flickcurl_photo_transformrotate(flickcurl * fc, flickcurl_photo * photo, int degrees);
	int bmx_flickcurl_photo_removefavorite(flickcurl * fc, flickcurl_photo * photo);
	int bmx_flickcurl_photo_addfavorite(flickcurl * fc, flickcurl_photo * photo);
	int bmx_flickcurl_photo_setcontenttype(flickcurl * fc, flickcurl_photo * photo, int contentType);
	int bmx_flickcurl_photo_setdates(flickcurl * fc, flickcurl_photo * photo, int datePosted, int dateTaken, int dateTakenGranularity);
	int bmx_flickcurl_photo_setmeta(flickcurl * fc, flickcurl_photo * photo, BBString * title, BBString * description);
	flickcurl_perms * bmx_flickcurl_photo_getperms(flickcurl * fc, flickcurl_photo * photo);
	flickcurl_perms * bmx_flickcurl_photo_getgeoperms(flickcurl * fc, flickcurl_photo * photo);
	int bmx_flickcurl_photo_setgeoperms(flickcurl * fc, flickcurl_photo * photo, flickcurl_perms * permissions);
	int bmx_flickcurl_photo_setperms(flickcurl * fc, flickcurl_photo * photo, flickcurl_perms * permissions);
	int bmx_flickcurl_photo_setlicense(flickcurl * fc, flickcurl_photo * photo, int licenseId);
	BBString * bmx_flickcurl_photo_addnote(flickcurl * fc, flickcurl_photo * photo, int x, int y, int w, int h, BBString * text);
	int bmx_flickcurl_photo_delete(flickcurl * fc, flickcurl_photo * photo);

	BBString * bmx_flickcurl_photofield_getlabel(flickcurl_photo_field_type fieldType);
	BBString * bmx_flickcurl_photofield_getvaluetypelabel(flickcurl_field_value_type valueType);

	flickcurl_place * bmx_flickcurl_resolveplaceid(flickcurl * fc, BBString * placeID);
	flickcurl_place * bmx_flickcurl_resolveplaceurl(flickcurl * fc, BBString * url);
	flickcurl_place * bmx_flickcurl_findplacebylatlon(flickcurl * fc, double lat, double lon, int accuracy);
	flickcurl_place ** bmx_flickcurl_placesfind(flickcurl * fc, BBString * query);

	BBString * bmx_flickcurl_place_gettypelabel(flickcurl_place_type placeType);
	BBString * bmx_flickcurl_place_getname(flickcurl_place * place, int index);
	BBString * bmx_flickcurl_place_getid(flickcurl_place * place, int index);
	BBString * bmx_flickcurl_place_geturl(flickcurl_place * place, int index);
	flickcurl_place_type bmx_flickcurl_place_gettype(flickcurl_place * place);
	BBString * bmx_flickcurl_place_getwoeid(flickcurl_place * place, int index);
	BBObject * bmx_flickcurl_place_getlocation(flickcurl_place * place);
	int bmx_flickcurl_place_getcount(flickcurl_place * place);
	void bmx_flickcurl_place_free(flickcurl_place * place);
	int bmx_flickcurl_place_gettypebylabel(BBString * label);

	flickcurl_search_params * bmx_flickcurl_searchparams_new();
	void bmx_flickcurl_searchparams_setuserid(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_settags(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_settagmode(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_settext(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_setminuploaddate(flickcurl_search_params * params, int value);
	void bmx_flickcurl_searchparams_setmaxuploaddate(flickcurl_search_params * params, int value);
	void bmx_flickcurl_searchparams_setmintakendate(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_setmaxtakendate(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_setlicense(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_setsort(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_setprivacyfilter(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_setbbox(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_setaccuracy(flickcurl_search_params * params, int value);
	void bmx_flickcurl_searchparams_setsafesearch(flickcurl_search_params * params, int value);
	void bmx_flickcurl_searchparams_setcontenttype(flickcurl_search_params * params, int value);
	void bmx_flickcurl_searchparams_setmachinetags(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_setmachinetagmode(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_setgroupid(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_setextras(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_setperpage(flickcurl_search_params * params, int value);
	void bmx_flickcurl_searchparams_setpage(flickcurl_search_params * params, int value);
	void bmx_flickcurl_searchparams_setplaceid(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_setmedia(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_sethasgeo(flickcurl_search_params * params, int value);
	void bmx_flickcurl_searchparams_setlat(flickcurl_search_params * params, double value);
	void bmx_flickcurl_searchparams_setlon(flickcurl_search_params * params, double value);
	void bmx_flickcurl_searchparams_setradius(flickcurl_search_params * params, double value);
	void bmx_flickcurl_searchparams_setradiusunits(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_setcontacts(flickcurl_search_params * params, BBString * value);
	void bmx_flickcurl_searchparams_setwoeid(flickcurl_search_params * params, int value);
	void bmx_flickcurl_searchparams_setgeocontext(flickcurl_search_params * params, int value);
	void bmx_flickcurl_searchparams_setiscommons(flickcurl_search_params * params, int value);
	void bmx_flickcurl_searchparams_setingallery(flickcurl_search_params * params, int value);
	void bmx_flickcurl_searchparams_delete(flickcurl_search_params * params);

	flickcurl_photo * bmx_flickcurl_tag_getphoto(flickcurl_tag * tag);
	BBString * bmx_flickcurl_tag_getid(flickcurl_tag * tag);
	BBString * bmx_flickcurl_tag_getauthor(flickcurl_tag * tag);
	BBString * bmx_flickcurl_tag_getauthorname(flickcurl_tag * tag);
	BBString * bmx_flickcurl_tag_getraw(flickcurl_tag * tag);
	BBString * bmx_flickcurl_tag_getcooked(flickcurl_tag * tag);
	int bmx_flickcurl_tag_getmachinetag(flickcurl_tag * tag);
	int bmx_flickcurl_tag_getcount(flickcurl_tag * tag);
	BBArray * bmx_flickcurl_tags_getclusters(flickcurl * fc, BBString * tag);
	int bmx_flickcurl_tag_remove(flickcurl * fc, flickcurl_tag * tag);
	int bmx_flickcurl_tag_removetxt(flickcurl * fc, BBString * tag);

	int bmx_flickcurl_getprefscontenttype(flickcurl * fc);
	int bmx_flickcurl_getprefsgeoperms(flickcurl * fc);
	int bmx_flickcurl_getprefshidden(flickcurl * fc);
	int bmx_flickcurl_getprefsprivacy(flickcurl * fc);
	int bmx_flickcurl_getprefssafetylevel(flickcurl * fc);

	BBObject * bmx_flickcurl_commentlist_getcomment(flickcurl_comment ** comments, int index, flickcurl * fc);

	int bmx_flickcurl_comment_deletecomment(flickcurl * fc, BBString * id);
	int bmx_flickcurl_comment_editcomment(flickcurl * fc, BBString * id, BBString * commentText);

	BBString * bmx_flickcurl_photolist_getformat(flickcurl_photos_list * list);
	int bmx_flickcurl_photolist_getphotocount(flickcurl_photos_list * list);
	flickcurl_photo * bmx_flickcurl_photolist_getphoto(flickcurl_photos_list * list, int index);
	BBString * bmx_flickcurl_photolist_getcontent(flickcurl_photos_list * list);

	flickcurl_photo** bmx_flickcurl_searchphotos(flickcurl * fc, flickcurl_search_params *params);
	int bmx_flickcurl_listofphotos_getphotocount(flickcurl_photo** list);
	flickcurl_photo * bmx_flickcurl_listofphotos_getphoto(flickcurl_photo** list, int index);

	flickcurl_photo** bmx_flickcurl_getfavoriteslist(flickcurl * fc, flickcurl_person * person, BBString * extras, int perPage, int page);
	flickcurl_photo** bmx_flickcurl_getpublicfavoriteslist(flickcurl * fc, flickcurl_person * person, BBString * extras, int perPage, int page);

	BBString * bmx_flickcurl_size_getlabel(flickcurl_size * size);
	int bmx_flickcurl_size_getwidth(flickcurl_size * size);
	int bmx_flickcurl_size_getheight(flickcurl_size * size);
	BBString * bmx_flickcurl_size_getsource(flickcurl_size * size);
	BBString * bmx_flickcurl_size_geturl(flickcurl_size * size);
	BBString * bmx_flickcurl_size_getmedia(flickcurl_size * size);

	int bmx_flickcurl_listofsizes_getsizecount(flickcurl_size ** list);
	flickcurl_size * bmx_flickcurl_listofsizes_getsize(flickcurl_size ** list, int index);

	flickcurl_photo** bmx_flickcurl_getinterestingnesslist(flickcurl * fc, BBString * date, BBString * extras, int perPage, int page);

	BBString * bmx_flickcurl_personfield_getlabel(flickcurl_person_field_type fieldType);

	BBString * bmx_flickcurl_person_getuserid(flickcurl_person * person);
	BBObject * bmx_flickcurl_person_getfield(flickcurl_person * person, int index);
	BBString * bmx_flickcurl_person_getphotosurl(flickcurl * fc, flickcurl_person * person);
	BBString * bmx_flickcurl_person_getprofileurl(flickcurl * fc, flickcurl_person * person);

	flickcurl_person * bmx_flickcurl_findpeoplebyemail(flickcurl * fc, BBString * email);
	flickcurl_person * bmx_flickcurl_findpeoplebyusername(flickcurl * fc, BBString * username);
	flickcurl_person * bmx_flickcurl_people_getinfo(flickcurl * fc, BBString * userID);
	flickcurl_person * bmx_flickcurl_url_lookupuser(flickcurl * fc, BBString * url);

	BBString * bmx_flickcurl_group_getgroupid(flickcurl_group * group);
	BBString * bmx_flickcurl_group_getname(flickcurl_group * group);
	BBString * bmx_flickcurl_group_getdescription(flickcurl_group * group);
	BBString * bmx_flickcurl_group_getlang(flickcurl_group * group);
	int bmx_flickcurl_group_isadmin(flickcurl_group * group);
	int bmx_flickcurl_group_ispoolmoderated(flickcurl_group * group);
	int bmx_flickcurl_group_iseighteenplus(flickcurl_group * group);
	int bmx_flickcurl_group_getprivacy(flickcurl_group * group);
	int bmx_flickcurl_group_getphotos(flickcurl_group * group);
	int bmx_flickcurl_group_geticonserver(flickcurl_group * group);
	int bmx_flickcurl_group_getmembers(flickcurl_group * group);
	BBString * bmx_flickcurl_group_getthrottlemode(flickcurl_group * group);
	int bmx_flickcurl_group_getthrottlecount(flickcurl_group * group);
	int bmx_flickcurl_group_getthrottleremaining(flickcurl_group * group);
	BBString * bmx_flickcurl_group_geturl(flickcurl * fc, flickcurl_group * group);
	int bmx_flickcurl_group_pools_add(flickcurl * fc, flickcurl_group * group, flickcurl_photo * photo);

	int bmx_flickcurl_listofgroups_getgroupcount(flickcurl_group ** list);
	flickcurl_group * bmx_flickcurl_listofgroups_getgroup(flickcurl_group ** list, int index);
	flickcurl_group * bmx_flickcurl_url_lookupgroup(flickcurl * fc, BBString * url, BBString * lang);
	flickcurl_group * bmx_flickcurl_groups_getinfo(flickcurl * fc, BBString * groupID, BBString * lang);

	int bmx_flickcurl_perms_ispublic(flickcurl_perms * perms);
	void bmx_flickcurl_perms_setpublic(flickcurl_perms * perms, int value);
	int bmx_flickcurl_perms_iscontact(flickcurl_perms * perms);
	void bmx_flickcurl_perms_setcontact(flickcurl_perms * perms, int value);
	int bmx_flickcurl_perms_isfriend(flickcurl_perms * perms);
	void bmx_flickcurl_perms_setfriend(flickcurl_perms * perms, int value);
	int bmx_flickcurl_perms_isfamily(flickcurl_perms * perms);
	void bmx_flickcurl_perms_setfamily(flickcurl_perms * perms, int value);
	int bmx_flickcurl_perms_getpermcomment(flickcurl_perms * perms);
	void bmx_flickcurl_perms_setpermcomment(flickcurl_perms * perms, int value);
	int bmx_flickcurl_perms_getpermaddmeta(flickcurl_perms * perms);
	void bmx_flickcurl_perms_setpermaddmeta(flickcurl_perms * perms, int value);

	flickcurl_perms * bmx_flickcurl_perms_create();
	void bmx_flickcurl_perms_delete(flickcurl_perms * perms);

	int bmx_flickcurl_notes_deletenote(flickcurl * fc, BBString * noteID);
	int bmx_flickcurl_notes_editnote(flickcurl * fc, BBString * noteID, int x, int y, int w, int h, BBString * text);

	int bmx_flickcurl_listoflicenses_getlicensecount(flickcurl_license** list);
	BBObject * bmx_flickcurl_listoflicenses_getlicense(flickcurl_license** list, int index);

	BBArray * bmx_flickcurl_panda_getlist(flickcurl * fc);
	flickcurl_photo** bmx_flickcurl_panda_getphotos(flickcurl * fc, BBString * pandaName);

	int bmx_flickcurl_listofactivities_getactivitycount(flickcurl_activity** list);
	flickcurl_activity* bmx_flickcurl_listofactivities_getactivity(flickcurl_activity** list, int index);
	flickcurl_activity** bmx_flickcurl_activity_usercomments(flickcurl * fc, int perPage, int page);
	flickcurl_activity** bmx_flickcurl_activity_userphotos(flickcurl * fc, BBString * timeFrame, int perPage, int page);

	BBString * bmx_flickcurl_activity_gettype(flickcurl_activity* ac);
	BBString * bmx_flickcurl_activity_getowner(flickcurl_activity* ac);
	BBString * bmx_flickcurl_activity_getownername(flickcurl_activity* ac);
	BBString * bmx_flickcurl_activity_getprimary(flickcurl_activity* ac);
	BBString * bmx_flickcurl_activity_getid(flickcurl_activity* ac);
	BBString * bmx_flickcurl_activity_getsecret(flickcurl_activity* ac);
	int bmx_flickcurl_activity_getserver(flickcurl_activity* ac);
	int bmx_flickcurl_activity_getfarm(flickcurl_activity* ac);
	int bmx_flickcurl_activity_getoldcomments(flickcurl_activity* ac);
	int bmx_flickcurl_activity_getnewcomments(flickcurl_activity* ac);
	int bmx_flickcurl_activity_getoldnotes(flickcurl_activity* ac);
	int bmx_flickcurl_activity_getnewnotes(flickcurl_activity* ac);
	int bmx_flickcurl_activity_getviews(flickcurl_activity* ac);
	int bmx_flickcurl_activity_getcomments(flickcurl_activity* ac);
	int bmx_flickcurl_activity_getphotos(flickcurl_activity* ac);
	int bmx_flickcurl_activity_getfaves(flickcurl_activity* ac);
	int bmx_flickcurl_activity_getmore(flickcurl_activity* ac);
	BBString * bmx_flickcurl_activity_gettitle(flickcurl_activity* ac);

	flickcurl_institution** bmx_flickcurl_commons_getinstitutions(flickcurl * fc);
	int bmx_flickcurl_listofinstitutions_getinstitutioncount(flickcurl_institution** list);
	BBObject * bmx_flickcurl_listofinstitutions_getinstitution(flickcurl_institution** list, int index);
	BBString * bmx_flickcurl_institution_url_type_label(flickcurl_institution_url_type urlType);

	flickcurl_contact** bmx_flickcurl_contacts_getlist(flickcurl * fc, BBString * filter, int page, int perPage);
	flickcurl_contact** bmx_flickcurl_contacts_getlistrecentlyuploaded(flickcurl * fc, int dateLastUpload, BBString * filter);
	flickcurl_contact** bmx_flickcurl_contacts_getpubliclist(flickcurl * fc, BBString * user, int page, int perPage);
	int bmx_flickcurl_listofcontacts_getcontactcount(flickcurl_contact** list);
	BBObject * bmx_flickcurl_listofcontacts_getcontact(flickcurl_contact** list, int index);

	flickcurl_photo** bmx_flickcurl_getcontactsphotos(flickcurl * fc, int contactCount, int justFriends, int singlePhoto,
			int includeSelf, BBString * extras);
	flickcurl_photo** bmx_flickcurl_getcontactspublicphotos(flickcurl * fc, BBString * user, int photoCount, int justFriends,
			int singlePhoto, int includeSelf, BBString * extras);


	int bmx_flickcurl_listofplaces_getplacecount(flickcurl_place ** list);
	flickcurl_place * bmx_flickcurl_listofplaces_getplace(flickcurl_place ** list, int index);

}


flickcurl * bmx_flickcurl_new() {
	return flickcurl_new();
}

void bmx_flickcurl_free(flickcurl * fc) {
	flickcurl_free(fc);
}

void bmx_flickcurl_setapikey(flickcurl * fc, BBString * apiKey) {
	char *p=bbStringToCString( apiKey );
	flickcurl_set_api_key(fc, p);
	bbMemFree( p );
}

void bmx_flickcurl_setauthtoken(flickcurl * fc, BBString * authToken) {
	char *p=bbStringToCString( authToken );
	flickcurl_set_auth_token(fc, p);
	bbMemFree( p );
}

void bmx_flickcurl_setsharedsecret(flickcurl * fc, BBString * sharedSecret) {
	char *p=bbStringToCString( sharedSecret );
	flickcurl_set_shared_secret(fc, p);
	bbMemFree( p );
}

BBString * bmx_flickcurl_getfrob(flickcurl * fc) {
	return bbStringFromCString(flickcurl_auth_getFrob(fc));
}

BBString * bmx_flickcurl_getfulltoken(flickcurl * fc, BBString * frob) {
	char *p=bbStringToCString( frob );
	BBString * tok = bbStringFromCString(flickcurl_auth_getFullToken(fc, p));
	bbMemFree( p );
	return tok;
}

BBString * bmx_flickcurl_getToken(flickcurl * fc, BBString * frob) {
	char *p=bbStringToCString( frob );
	BBString * tok = bbStringFromCString(flickcurl_auth_getToken(fc, p));
	bbMemFree( p );
	return tok;
}

flickcurl_photo * bmx_flickcurl_photosgetinfo(flickcurl * fc, BBString * photoID) {
	char *p=bbStringToCString( photoID );
	flickcurl_photo * photo = flickcurl_photos_getInfo(fc, p);
	bbMemFree( p );
	return photo;
}

flickcurl_place * bmx_flickcurl_resolveplaceid(flickcurl * fc, BBString * placeID) {
	char *p=bbStringToCString( placeID );
	flickcurl_place * place = flickcurl_places_resolvePlaceId(fc, p);
	bbMemFree( p );
	return place;
}

flickcurl_place * bmx_flickcurl_resolveplaceurl(flickcurl * fc, BBString * url) {
	char *p=bbStringToCString( url );
	flickcurl_place * place = flickcurl_places_resolvePlaceURL(fc, p);
	bbMemFree( p );
	return place;
}

flickcurl_place * bmx_flickcurl_findplacebylatlon(flickcurl * fc, double lat, double lon, int accuracy) {
	return flickcurl_places_findByLatLon(fc, lat, lon, accuracy);
}

flickcurl_place ** bmx_flickcurl_placesfind(flickcurl * fc, BBString * query) {
	char *p=bbStringToCString( query );
	flickcurl_place ** places = flickcurl_places_find(fc, p);
	bbMemFree( p );
	return places;
}

int bmx_flickcurl_getprefscontenttype(flickcurl * fc) {
	return flickcurl_prefs_getContentType(fc);
}

int bmx_flickcurl_getprefsgeoperms(flickcurl * fc) {
	return flickcurl_prefs_getGeoPerms(fc);
}

int bmx_flickcurl_getprefshidden(flickcurl * fc) {
	return flickcurl_prefs_getHidden(fc);
}

int bmx_flickcurl_getprefsprivacy(flickcurl * fc) {
	return flickcurl_prefs_getPrivacy(fc);
}

int bmx_flickcurl_getprefssafetylevel(flickcurl * fc) {
	return flickcurl_prefs_getSafetyLevel(fc);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBObject * bmx_flickcurl_photo_getfield(flickcurl_photo * photo, int index) {
	flickcurl_photo_field field = photo->fields[index];
	return _bah_flickcurl_TFCPhotoField__create(bbStringFromCString(field.string), field.integer, field.type);
}

BBString * bmx_flickcurl_photo_geturi(flickcurl_photo * photo) {
	return bbStringFromCString(photo->uri);
}

BBString * bmx_flickcurl_photo_getsourceuri(flickcurl_photo * photo, int size) {
	char type = 'm';
	switch (size) {
		case 1:
			type = 's';
			break;
		case 2:
			type = 't';
			break;
		case 3:
			type = 'm';
			break;
		case 4:
			type = '-';
			break;
		case 5:
			type = 'b';
			break;
		case 6:
			type = 'o';
			break;
	}
	return bbStringFromCString(flickcurl_photo_as_source_uri(photo, type));
}

void bmx_flickcurl_photo_free(flickcurl_photo * photo) {
	flickcurl_free_photo(photo);
}

flickcurl_place * bmx_flickcurl_photo_getplace(flickcurl_photo * photo) {
	return photo->place;
}

flickcurl_tag * bmx_flickcurl_photo_gettag(flickcurl_photo * photo, int index) {
	return photo->tags[index];
}

int bmx_flickcurl_photo_gettagcount(flickcurl_photo * photo) {
	return photo->tags_count;
}

int bmx_flickcurl_photo_addtags(flickcurl * fc, flickcurl_photo * photo, BBString * tags) {
	char *p=bbStringToCString( tags );
	int res = flickcurl_photos_addTags(fc, photo->id, p);
	bbMemFree( p );
	return res;
}

BBObject * bmx_flickcurl_photo_addcomment(flickcurl * fc, flickcurl_photo * photo, BBString * comment) {
	char *p=bbStringToCString( comment );
	char * comment_id = flickcurl_photos_comments_addComment(fc, photo->id, p);
	bbMemFree( p );
	
	if (comment_id) {
		
		flickcurl_comment ** comments = flickcurl_photos_comments_getList(fc, photo->id);
		
		for (int i = 0; comments[i]; i++) {
		
			flickcurl_comment * cmt = comments[i];
			
			if (strcmp(comment_id, cmt->id) == 0) {
				
				BBObject * bbcomment = _bah_flickcurl_TFCComment__create(bbStringFromCString(cmt->id), bbStringFromCString(cmt->author), 
					bbStringFromCString(cmt->authorname), cmt->datecreate, bbStringFromCString(cmt->permalink),
					bbStringFromCString(cmt->text), fc);
					
				flickcurl_free_comments(comments);
					
				return bbcomment;
			}
			
			cmt++;
		}
		
		flickcurl_free_comments(comments);
		
	}

	return &bbNullObject;
}

flickcurl_comment ** bmx_flickcurl_photo_getcommentlist(flickcurl * fc, flickcurl_photo * photo) {
	return flickcurl_photos_comments_getList(fc, photo->id);
}

BBString * bmx_flickcurl_photo_getid(flickcurl_photo * photo) {
	return bbStringFromCString(photo->id);
}

int bmx_flickcurl_photo_setsafetylevel(flickcurl * fc, flickcurl_photo * photo, int safetyLevel, int hidden) {
	return flickcurl_photos_setSafetyLevel(fc, photo->id, safetyLevel, hidden);
}

int bmx_flickcurl_photo_settags(flickcurl * fc, flickcurl_photo * photo, BBString * tags) {
	char *p=bbStringToCString( tags );
	int res = flickcurl_photos_setTags(fc, photo->id, p);
	bbMemFree( p );
	return res;	
}

int bmx_flickcurl_photo_transformrotate(flickcurl * fc, flickcurl_photo * photo, int degrees) {
	return flickcurl_photos_transform_rotate(fc, photo->id, degrees);
}

int bmx_flickcurl_photo_removefavorite(flickcurl * fc, flickcurl_photo * photo) {
	return flickcurl_favorites_remove(fc, photo->id);
}

int bmx_flickcurl_photo_addfavorite(flickcurl * fc, flickcurl_photo * photo) {
	return flickcurl_favorites_add(fc, photo->id);
}

int bmx_flickcurl_photo_setcontenttype(flickcurl * fc, flickcurl_photo * photo, int contentType) {
	return flickcurl_photos_setContentType(fc, photo->id, contentType);
}

int bmx_flickcurl_photo_setdates(flickcurl * fc, flickcurl_photo * photo, int datePosted, int dateTaken, int dateTakenGranularity) {
	return flickcurl_photos_setDates(fc, photo->id, datePosted, dateTaken, dateTakenGranularity);
}

int bmx_flickcurl_photo_setmeta(flickcurl * fc, flickcurl_photo * photo, BBString * title, BBString * description) {
	char *t=bbStringToCString( title );
	char *d=bbStringToCString( description );
	int res = flickcurl_photos_setMeta(fc, photo->id, t, d);
	bbMemFree(t);
	bbMemFree(d);
	return res;
}

flickcurl_perms * bmx_flickcurl_photo_getperms(flickcurl * fc, flickcurl_photo * photo) {
	return flickcurl_photos_getPerms(fc, photo->id);
}

flickcurl_perms * bmx_flickcurl_photo_getgeoperms(flickcurl * fc, flickcurl_photo * photo) {
	return flickcurl_photos_geo_getPerms(fc, photo->id);
}

int bmx_flickcurl_photo_setgeoperms(flickcurl * fc, flickcurl_photo * photo, flickcurl_perms * permissions) {
	return flickcurl_photos_geo_setPerms(fc, photo->id, permissions);
}

int bmx_flickcurl_photo_setperms(flickcurl * fc, flickcurl_photo * photo, flickcurl_perms * permissions) {
	return flickcurl_photos_setPerms(fc, photo->id, permissions);
}

int bmx_flickcurl_photo_setlicense(flickcurl * fc, flickcurl_photo * photo, int licenseId) {
	return flickcurl_photos_licenses_setLicense(fc, photo->id, licenseId);
}

BBString * bmx_flickcurl_photo_addnote(flickcurl * fc, flickcurl_photo * photo, int x, int y, int w, int h, BBString * text) {
	char *p=bbStringToCString( text );
	char * res = flickcurl_photos_notes_add(fc, photo->id, x, y, w, h, p);
	bbMemFree( p );
	if (res) {
		return bbStringFromCString(res);
	} else {
		return &bbEmptyString;
	}
}

int bmx_flickcurl_photo_delete(flickcurl * fc, flickcurl_photo * photo) {
	return flickcurl_photos_delete(fc, photo->id);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_flickcurl_photofield_getlabel(flickcurl_photo_field_type fieldType) {
	return bbStringFromCString(flickcurl_get_photo_field_label(fieldType));
}

BBString * bmx_flickcurl_photofield_getvaluetypelabel(flickcurl_field_value_type valueType) {
	return bbStringFromCString(flickcurl_get_field_value_type_label(valueType));
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_flickcurl_place_gettypelabel(flickcurl_place_type placeType) {
	return bbStringFromCString(flickcurl_get_place_type_label(placeType));
}

BBString * bmx_flickcurl_place_getname(flickcurl_place * place, int index) {
	return bbStringFromCString(place->names[index]);
}

BBString * bmx_flickcurl_place_getid(flickcurl_place * place, int index) {
	return bbStringFromCString(place->ids[index]);
}

BBString * bmx_flickcurl_place_geturl(flickcurl_place * place, int index) {
	return bbStringFromCString(place->urls[index]);
}

flickcurl_place_type bmx_flickcurl_place_gettype(flickcurl_place * place) {
	return place->type;
}

BBString * bmx_flickcurl_place_getwoeid(flickcurl_place * place, int index) {
	return bbStringFromCString(place->woe_ids[index]);
}

BBObject * bmx_flickcurl_place_getlocation(flickcurl_place * place) {
	flickcurl_location location = place->location;
	return _bah_flickcurl_TFCLocation__create(location.latitude, location.longitude, location.accuracy);
}

int bmx_flickcurl_place_getcount(flickcurl_place * place) {
	return place->count;
}

void bmx_flickcurl_place_free(flickcurl_place * place) {
	flickcurl_free_place(place);
}

int bmx_flickcurl_place_gettypebylabel(BBString * label) {
	char *p=bbStringToCString( label );
	int res = flickcurl_get_place_type_by_label(p);
	bbMemFree(p);
	return res;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++


flickcurl_search_params * bmx_flickcurl_searchparams_new() {
	flickcurl_search_params * params = new flickcurl_search_params;
	flickcurl_search_params_init(params);
	return params;
}

void bmx_flickcurl_searchparams_setuserid(flickcurl_search_params * params, BBString * value) {
	if (params->user_id) {
		bbMemFree(params->user_id);
	}
	char *p=bbStringToCString( value );
	params->user_id = p;
}

void bmx_flickcurl_searchparams_settags(flickcurl_search_params * params, BBString * value) {
	if (params->tags) {
		bbMemFree(params->tags);
	}
	char *p=bbStringToCString( value );
	params->tags = p;
}

void bmx_flickcurl_searchparams_settagmode(flickcurl_search_params * params, BBString * value) {
	if (params->tag_mode) {
		bbMemFree(params->tag_mode);
	}
	char *p=bbStringToCString( value );
	params->tag_mode = p;
}

void bmx_flickcurl_searchparams_settext(flickcurl_search_params * params, BBString * value) {
	if (params->text) {
		bbMemFree(params->text);
	}
	char *p=bbStringToCString( value );
	params->text = p;
}

void bmx_flickcurl_searchparams_setminuploaddate(flickcurl_search_params * params, int value) {
	params->min_upload_date = value;
}

void bmx_flickcurl_searchparams_setmaxuploaddate(flickcurl_search_params * params, int value) {
	params->max_upload_date = value;
}

void bmx_flickcurl_searchparams_setmintakendate(flickcurl_search_params * params, BBString * value) {
	if (params->min_taken_date) {
		bbMemFree(params->min_taken_date);
	}
	char *p=bbStringToCString( value );
	params->min_taken_date = p;
}

void bmx_flickcurl_searchparams_setmaxtakendate(flickcurl_search_params * params, BBString * value) {
	if (params->max_taken_date) {
		bbMemFree(params->max_taken_date);
	}
	char *p=bbStringToCString( value );
	params->max_taken_date = p;
}

void bmx_flickcurl_searchparams_setlicense(flickcurl_search_params * params, BBString * value) {
	if (params->license) {
		bbMemFree(params->license);
	}
	char *p=bbStringToCString( value );
	params->license = p;
}

void bmx_flickcurl_searchparams_setsort(flickcurl_search_params * params, BBString * value) {
	if (params->sort) {
		bbMemFree(params->sort);
	}
	char *p=bbStringToCString( value );
	params->sort = p;
}

void bmx_flickcurl_searchparams_setprivacyfilter(flickcurl_search_params * params, BBString * value) {
	if (params->privacy_filter) {
		bbMemFree(params->privacy_filter);
	}
	char *p=bbStringToCString( value );
	params->privacy_filter = p;
}

void bmx_flickcurl_searchparams_setbbox(flickcurl_search_params * params, BBString * value) {
	if (params->bbox) {
		bbMemFree(params->bbox);
	}
	char *p=bbStringToCString( value );
	params->bbox = p;
}

void bmx_flickcurl_searchparams_setaccuracy(flickcurl_search_params * params, int value) {
	params->accuracy = value;
}

void bmx_flickcurl_searchparams_setsafesearch(flickcurl_search_params * params, int value) {
	params->safe_search = value;
}

void bmx_flickcurl_searchparams_setcontenttype(flickcurl_search_params * params, int value) {
	params->content_type = value;
}

void bmx_flickcurl_searchparams_setmachinetags(flickcurl_search_params * params, BBString * value) {
	if (params->machine_tags) {
		bbMemFree(params->machine_tags);
	}
	char *p=bbStringToCString( value );
	params->machine_tags = p;
}

void bmx_flickcurl_searchparams_setmachinetagmode(flickcurl_search_params * params, BBString * value) {
	if (params->machine_tag_mode) {
		bbMemFree(params->machine_tag_mode);
	}
	char *p=bbStringToCString( value );
	params->machine_tag_mode = p;
}

void bmx_flickcurl_searchparams_setgroupid(flickcurl_search_params * params, BBString * value) {
	if (params->group_id) {
		bbMemFree(params->group_id);
	}
	char *p=bbStringToCString( value );
	params->group_id = p;
}

void bmx_flickcurl_searchparams_setextras(flickcurl_search_params * params, BBString * value) {
	if (params->extras) {
		bbMemFree(params->extras);
	}
	char *p=bbStringToCString( value );
	params->extras = p;
}

void bmx_flickcurl_searchparams_setperpage(flickcurl_search_params * params, int value) {
	params->per_page = value;
}

void bmx_flickcurl_searchparams_setpage(flickcurl_search_params * params, int value) {
	params->page = value;
}

void bmx_flickcurl_searchparams_setplaceid(flickcurl_search_params * params, BBString * value) {
	if (params->place_id) {
		bbMemFree(params->place_id);
	}
	char *p=bbStringToCString( value );
	params->place_id = p;
}

void bmx_flickcurl_searchparams_setmedia(flickcurl_search_params * params, BBString * value) {
	if (params->media) {
		bbMemFree(params->media);
	}
	char *p=bbStringToCString( value );
	params->media = p;
}

void bmx_flickcurl_searchparams_sethasgeo(flickcurl_search_params * params, int value) {
	params->has_geo = value;
}

void bmx_flickcurl_searchparams_setlat(flickcurl_search_params * params, double value) {
	params->lat = value;
}

void bmx_flickcurl_searchparams_setlon(flickcurl_search_params * params, double value) {
	params->lon = value;
}

void bmx_flickcurl_searchparams_setradius(flickcurl_search_params * params, double value) {
	params->radius = value;
}

void bmx_flickcurl_searchparams_setradiusunits(flickcurl_search_params * params, BBString * value) {
	if (params->radius_units) {
		bbMemFree(params->radius_units);
	}
	char *p=bbStringToCString( value );
	params->radius_units = p;
}

void bmx_flickcurl_searchparams_setcontacts(flickcurl_search_params * params, BBString * value) {
	if (params->contacts) {
		bbMemFree(params->contacts);
	}
	char *p=bbStringToCString( value );
	params->contacts = p;
}

void bmx_flickcurl_searchparams_setwoeid(flickcurl_search_params * params, int value) {
	params->woe_id = value;
}

void bmx_flickcurl_searchparams_setgeocontext(flickcurl_search_params * params, int value) {
	params->geo_context = value;
}

void bmx_flickcurl_searchparams_setiscommons(flickcurl_search_params * params, int value) {
	params->is_commons = value;
}

void bmx_flickcurl_searchparams_setingallery(flickcurl_search_params * params, int value) {
	params->in_gallery = value;
}

void bmx_flickcurl_searchparams_delete(flickcurl_search_params * params) {
	if (params->user_id) {
		bbMemFree(params->user_id);
	}
	if (params->tags) {
		bbMemFree(params->tags);
	}
	if (params->tag_mode) {
		bbMemFree(params->tag_mode);
	}
	if (params->text) {
		bbMemFree(params->text);
	}
	if (params->min_taken_date) {
		bbMemFree(params->min_taken_date);
	}
	if (params->max_taken_date) {
		bbMemFree(params->max_taken_date);
	}
	if (params->license) {
		bbMemFree(params->license);
	}
	if (params->sort) {
		bbMemFree(params->sort);
	}
	if (params->privacy_filter) {
		bbMemFree(params->privacy_filter);
	}
	if (params->bbox) {
		bbMemFree(params->bbox);
	}
	if (params->machine_tags) {
		bbMemFree(params->machine_tags);
	}
	if (params->machine_tag_mode) {
		bbMemFree(params->machine_tag_mode);
	}
	if (params->group_id) {
		bbMemFree(params->group_id);
	}
	if (params->extras) {
		bbMemFree(params->extras);
	}
	if (params->place_id) {
		bbMemFree(params->place_id);
	}
	if (params->media) {
		bbMemFree(params->media);
	}
	if (params->radius_units) {
		bbMemFree(params->radius_units);
	}
	if (params->contacts) {
		bbMemFree(params->contacts);
	}

	delete params;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

flickcurl_photo * bmx_flickcurl_tag_getphoto(flickcurl_tag * tag) {
	return tag->photo;
}

BBString * bmx_flickcurl_tag_getid(flickcurl_tag * tag) {
	return bbStringFromCString(tag->id);
}

BBString * bmx_flickcurl_tag_getauthor(flickcurl_tag * tag) {
	return bbStringFromCString(tag->author);
}

BBString * bmx_flickcurl_tag_getauthorname(flickcurl_tag * tag) {
	return bbStringFromCString(tag->authorname);
}

BBString * bmx_flickcurl_tag_getraw(flickcurl_tag * tag) {
	return bbStringFromCString(tag->raw);
}

BBString * bmx_flickcurl_tag_getcooked(flickcurl_tag * tag) {
	return bbStringFromCString(tag->cooked);
}

int bmx_flickcurl_tag_getmachinetag(flickcurl_tag * tag) {
	return tag->machine_tag;
}

int bmx_flickcurl_tag_getcount(flickcurl_tag * tag) {
	return tag->count;
}

BBArray * bmx_flickcurl_tags_getclusters(flickcurl * fc, BBString * tag) {

	BBArray * cls = &bbEmptyArray;

	char *p=bbStringToCString( tag );
	
	flickcurl_tag_clusters * clusters = flickcurl_tags_getClusters(fc, p);
	
	bbMemFree(p);

	if (clusters) {
		
		if (clusters->count > 0) {
		
			cls = _bah_flickcurl_TFCTagCluster__createClusterArray(clusters->count);
		
			for (int i = 0; i < clusters->count; i++) {
				
				flickcurl_tag_cluster * cluster = clusters->clusters[i];
				
				BBObject * bbCluster = _bah_flickcurl_TFCTagCluster__setCluster(cls, i, cluster->count);
				
				for (int n = 0; n < cluster->count; n++) {
					_bah_flickcurl_TFCTagCluster__setTag(bbCluster, n, bbStringFromCString(cluster->tags[n]));
				}
				
			}
			
		}	
		
		flickcurl_free_tag_clusters(clusters);
	
	}
	
	return cls;
	
}

int bmx_flickcurl_tag_remove(flickcurl * fc, flickcurl_tag * tag) {
	return flickcurl_photos_removeTag(fc, tag->id);
}

int bmx_flickcurl_tag_removetxt(flickcurl * fc, BBString * tag) {
	char *t=bbStringToCString( tag );
	int res = flickcurl_photos_removeTag(fc, t);
	bbMemFree(t);
	return res;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBObject * bmx_flickcurl_commentlist_getcomment(flickcurl_comment ** comments, int index, flickcurl * fc) {

	flickcurl_comment * cmt = comments[index];
	
	if (cmt) {

		return _bah_flickcurl_TFCComment__create(bbStringFromCString(cmt->id), bbStringFromCString(cmt->author), 
			bbStringFromCString(cmt->authorname), cmt->datecreate, bbStringFromCString(cmt->permalink),
			bbStringFromCString(cmt->text), fc);
	}

	return &bbNullObject;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int bmx_flickcurl_comment_deletecomment(flickcurl * fc, BBString * id) {
	char *p=bbStringToCString( id );
	int res = flickcurl_photos_comments_deleteComment(fc, p);
	bbMemFree(p);
}

int bmx_flickcurl_comment_editcomment(flickcurl * fc, BBString * id, BBString * commentText) {
	char *i=bbStringToCString( id );
	char *c=bbStringToCString( commentText );
	
	int res = flickcurl_photos_comments_editComment(fc, i, c);
	
	bbMemFree(i);
	bbMemFree(c);
	return res;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_flickcurl_photolist_getformat(flickcurl_photos_list * list) {
	return bbStringFromCString(list->format);
}

int bmx_flickcurl_photolist_getphotocount(flickcurl_photos_list * list) {
	return list->photos_count;
}

flickcurl_photo * bmx_flickcurl_photolist_getphoto(flickcurl_photos_list * list, int index) {
	return list->photos[index];
}

BBString * bmx_flickcurl_photolist_getcontent(flickcurl_photos_list * list) {
	return bbStringFromCString(list->content);	
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

flickcurl_photo** bmx_flickcurl_searchphotos(flickcurl * fc, flickcurl_search_params *params) {
	return flickcurl_photos_search(fc, params);
}

int bmx_flickcurl_listofphotos_getphotocount(flickcurl_photo** list) {
	int count = 0;
	
	for (int i = 0; list[i]; i++) {
		count++;
	}

	return count;
}

flickcurl_photo * bmx_flickcurl_listofphotos_getphoto(flickcurl_photo** list, int index) {
	return list[index];
}

flickcurl_photo** bmx_flickcurl_getcontactsphotos(flickcurl * fc, int contactCount, int justFriends, int singlePhoto,
		int includeSelf, BBString * extras) {

	char * e = 0;
	
	if (extras != &bbEmptyString) {
		e = bbStringToCString(extras);
	}
	
	flickcurl_photo** list = flickcurl_photos_getContactsPhotos(fc, contactCount, justFriends, singlePhoto, includeSelf, e);
	
	if (e) bbMemFree(e);
	
	return list;
}

flickcurl_photo** bmx_flickcurl_getcontactspublicphotos(flickcurl * fc, BBString * user, int photoCount, int justFriends,
		int singlePhoto, int includeSelf, BBString * extras) {

	char * e = 0;
	char * u = bbStringToCString(user);
	
	if (extras != &bbEmptyString) {
		e = bbStringToCString(extras);
	}
	
	flickcurl_photo** list = flickcurl_photos_getContactsPublicPhotos(fc, u, photoCount, justFriends, singlePhoto, includeSelf, e);
	
	bbMemFree(u);
	if (e) bbMemFree(e);
	
	return list;

}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

flickcurl_photo** bmx_flickcurl_getfavoriteslist(flickcurl * fc, flickcurl_person * person, BBString * extras, int perPage, int page) {
	char *e=bbStringToCString( extras );
	flickcurl_photo** list = flickcurl_favorites_getList(fc, person->nsid, e, perPage, page);
	bbMemFree(e);
	return list;
}

flickcurl_photo** bmx_flickcurl_getpublicfavoriteslist(flickcurl * fc, flickcurl_person * person, BBString * extras, int perPage, int page) {
	char *e=bbStringToCString( extras );
	flickcurl_photo** list = flickcurl_favorites_getPublicList(fc, person->nsid, e, perPage, page);
	bbMemFree(e);
	return list;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_flickcurl_size_getlabel(flickcurl_size * size) {
	return bbStringFromCString(size->label);
}

int bmx_flickcurl_size_getwidth(flickcurl_size * size) {
	return size->width;
}

int bmx_flickcurl_size_getheight(flickcurl_size * size) {
	return size->width;
}

BBString * bmx_flickcurl_size_getsource(flickcurl_size * size) {
	return bbStringFromCString(size->source);
}

BBString * bmx_flickcurl_size_geturl(flickcurl_size * size) {
	return bbStringFromCString(size->url);
}

BBString * bmx_flickcurl_size_getmedia(flickcurl_size * size) {
	return bbStringFromCString(size->media);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int bmx_flickcurl_listofsizes_getsizecount(flickcurl_size ** list) {
	int count = 0;
	
	for (int i = 0; list[i]; i++) {
		count++;
	}

	return count;
}

flickcurl_size * bmx_flickcurl_listofsizes_getsize(flickcurl_size ** list, int index) {
	return list[index];
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

flickcurl_photo** bmx_flickcurl_getinterestingnesslist(flickcurl * fc, BBString * date, BBString * extras, int perPage, int page) {
	char *d=(date != &bbEmptyString) ? bbStringToCString( date ) : 0;
	char *e=(extras != &bbEmptyString) ? bbStringToCString( extras ) : 0;
	flickcurl_photo** list = flickcurl_interestingness_getList(fc, d, e, perPage, page);
	if (d) bbMemFree(d);
	if (e) bbMemFree(e);
	return list;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_flickcurl_personfield_getlabel(flickcurl_person_field_type fieldType) {
	return bbStringFromCString(flickcurl_get_person_field_label(fieldType));
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_flickcurl_person_getuserid(flickcurl_person * person) {
	return bbStringFromCString(person->nsid);
}

BBObject * bmx_flickcurl_person_getfield(flickcurl_person * person, int index) {
	flickcurl_person_field field = person->fields[index];
	return _bah_flickcurl_TFCPersonField__create(bbStringFromCString(field.string), field.integer, field.type);
}

flickcurl_person * bmx_flickcurl_findpeoplebyemail(flickcurl * fc, BBString * email) {
	flickcurl_person * person = 0;
	char *e=bbStringToCString( email );
	char * nsid = flickcurl_people_findByEmail(fc, e);
	if (nsid) {
		person = flickcurl_people_getInfo(fc, nsid);
	}
	bbMemFree(e);
	return person;
}

flickcurl_person * bmx_flickcurl_findpeoplebyusername(flickcurl * fc, BBString * username) {
	flickcurl_person * person = 0;
	char *u=bbStringToCString( username );
	char * nsid = flickcurl_people_findByUsername(fc, u);
	if (nsid) {
		person = flickcurl_people_getInfo(fc, nsid);
	}
	bbMemFree(u);
	return person;
}

BBString * bmx_flickcurl_person_getphotosurl(flickcurl * fc, flickcurl_person * person) {
	return bbStringFromCString(flickcurl_urls_getUserPhotos(fc, person->nsid));
}

BBString * bmx_flickcurl_person_getprofileurl(flickcurl * fc, flickcurl_person * person) {
	return bbStringFromCString(flickcurl_urls_getUserProfile(fc, person->nsid));
}

flickcurl_person * bmx_flickcurl_people_getinfo(flickcurl * fc, BBString * userID) {
	char *e=bbStringToCString( userID );
	flickcurl_person * person = flickcurl_people_getInfo(fc, e);
	bbMemFree(e);
	return person;	
}

flickcurl_person * bmx_flickcurl_url_lookupuser(flickcurl * fc, BBString * url) {
	flickcurl_person * person = 0;
	char *u=bbStringToCString( url );
	char * nsid = flickcurl_urls_lookupUser(fc, u);
	if (nsid) {
		person = flickcurl_people_getInfo(fc, nsid);
	}
	bbMemFree(u);
	return person;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_flickcurl_group_getgroupid(flickcurl_group * group) {
	return bbStringFromCString(group->nsid);
}

BBString * bmx_flickcurl_group_getname(flickcurl_group * group) {
	return bbStringFromCString(group->name);
}

BBString * bmx_flickcurl_group_getdescription(flickcurl_group * group) {
	return bbStringFromCString(group->description);
}

BBString * bmx_flickcurl_group_getlang(flickcurl_group * group) {
	return bbStringFromCString(group->lang);
}

int bmx_flickcurl_group_isadmin(flickcurl_group * group) {
	return group->is_admin;
}

int bmx_flickcurl_group_ispoolmoderated(flickcurl_group * group) {
	return group->is_pool_moderated;
}

int bmx_flickcurl_group_iseighteenplus(flickcurl_group * group) {
	return group->is_eighteenplus;
}

int bmx_flickcurl_group_getprivacy(flickcurl_group * group) {
	return group->privacy;
}

int bmx_flickcurl_group_getphotos(flickcurl_group * group) {
	return group->photos;
}

int bmx_flickcurl_group_geticonserver(flickcurl_group * group) {
	return group->iconserver;
}

int bmx_flickcurl_group_getmembers(flickcurl_group * group) {
	return group->members;
}

BBString * bmx_flickcurl_group_getthrottlemode(flickcurl_group * group) {
	return bbStringFromCString(group->throttle_mode);
}

int bmx_flickcurl_group_getthrottlecount(flickcurl_group * group) {
	return group->throttle_count;
}

int bmx_flickcurl_group_getthrottleremaining(flickcurl_group * group) {
	return group->throttle_remaining;
}

BBString * bmx_flickcurl_group_geturl(flickcurl * fc, flickcurl_group * group) {
	return bbStringFromCString(flickcurl_urls_getGroup(fc, group->nsid));
}

int bmx_flickcurl_group_pools_add(flickcurl * fc, flickcurl_group * group, flickcurl_photo * photo) {
	return flickcurl_groups_pools_add(fc, photo->id, group->nsid);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int bmx_flickcurl_listofgroups_getgroupcount(flickcurl_group ** list) {
	int count = 0;
	
	for (int i = 0; list[i]; i++) {
		count++;
	}

	return count;
}

flickcurl_group * bmx_flickcurl_listofgroups_getgroup(flickcurl_group ** list, int index) {
	return list[index];
}

flickcurl_group * bmx_flickcurl_url_lookupgroup(flickcurl * fc, BBString * url, BBString * lang) {
	flickcurl_group * group = 0;
	char *u=bbStringToCString( url );
	char *l=bbStringToCString( lang );
	char * nsid = flickcurl_urls_lookupGroup(fc, u);
	if (nsid) {
		group = flickcurl_groups_getInfo(fc, nsid, l);
	}
	bbMemFree(u);
	bbMemFree(l);
	return group;
}

flickcurl_group * bmx_flickcurl_groups_getinfo(flickcurl * fc, BBString * groupID, BBString * lang) {
	char *g=bbStringToCString( groupID );
	char *l=bbStringToCString( lang );
	flickcurl_group * group = flickcurl_groups_getInfo(fc, g, l);
	bbMemFree(g);
	bbMemFree(l);
	return group;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int bmx_flickcurl_perms_ispublic(flickcurl_perms * perms) {
	return perms->is_public;
}

void bmx_flickcurl_perms_setpublic(flickcurl_perms * perms, int value) {
	perms->is_public = value;
}

int bmx_flickcurl_perms_iscontact(flickcurl_perms * perms) {
	return perms->is_contact;
}

void bmx_flickcurl_perms_setcontact(flickcurl_perms * perms, int value) {
	perms->is_contact = value;
}

int bmx_flickcurl_perms_isfriend(flickcurl_perms * perms) {
	return perms->is_friend;
}

void bmx_flickcurl_perms_setfriend(flickcurl_perms * perms, int value) {
	perms->is_friend = value;
}

int bmx_flickcurl_perms_isfamily(flickcurl_perms * perms) {
	return perms->is_family;
}

void bmx_flickcurl_perms_setfamily(flickcurl_perms * perms, int value) {
	perms->is_family = value;
}

int bmx_flickcurl_perms_getpermcomment(flickcurl_perms * perms) {
	return perms->perm_comment;
}

void bmx_flickcurl_perms_setpermcomment(flickcurl_perms * perms, int value) {
	perms->perm_comment = value;
}

int bmx_flickcurl_perms_getpermaddmeta(flickcurl_perms * perms) {
	return perms->perm_addmeta;
}

void bmx_flickcurl_perms_setpermaddmeta(flickcurl_perms * perms, int value) {
	perms->perm_addmeta = value;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

flickcurl_perms * bmx_flickcurl_perms_create() {
	return new flickcurl_perms;
}

void bmx_flickcurl_perms_delete(flickcurl_perms * perms) {
	delete perms;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int bmx_flickcurl_notes_deletenote(flickcurl * fc, BBString * noteID) {
	char *n=bbStringToCString( noteID );
	int res = flickcurl_photos_notes_delete(fc, n);
	bbMemFree(n);
	return res;
}

int bmx_flickcurl_notes_editnote(flickcurl * fc, BBString * noteID, int x, int y, int w, int h, BBString * text) {
	char *n=bbStringToCString( noteID );
	char *t=bbStringToCString( text );
	int res = flickcurl_photos_notes_edit(fc, n, x, y, w, h, t);
	bbMemFree(n);
	bbMemFree(t);
	return res;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

flickcurl_license** bmx_flickcurl_photos_license_getinfo(flickcurl * fc) {
	return flickcurl_photos_licenses_getInfo(fc);
}

BBObject * bmx_flickcurl_photos_license_getinfobyid(flickcurl * fc, int id) {
	flickcurl_license* lic = flickcurl_photos_licenses_getInfo_by_id(fc, id);
	if (lic) {
		return _bah_flickcurl_TFCLicense__create(lic->id, bbStringFromCString(lic->url), bbStringFromCString(lic->name));
	}

	return &bbNullObject;
}

int bmx_flickcurl_listoflicenses_getlicensecount(flickcurl_license** list) {
	int count = 0;
	
	for (int i = 0; list[i]; i++) {
		count++;
	}

	return count;
}

BBObject * bmx_flickcurl_listoflicenses_getlicense(flickcurl_license** list, int index) {
	flickcurl_license* lic = list[index];
	if (lic) {
		return _bah_flickcurl_TFCLicense__create(lic->id, bbStringFromCString(lic->url), bbStringFromCString(lic->name));
	}

	return &bbNullObject;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBArray * bmx_flickcurl_panda_getlist(flickcurl * fc) {

	int count = 0;
	char** pandas = flickcurl_panda_getList(fc);
	
	BBArray * pa = &bbEmptyArray;
	
	for (int i = 0; pandas[i]; i++) {
		count++;
	}
	
	if (count > 0) {
		pa = bbArrayNew1D( "$",count );
		BBString **s=(BBString**)BBARRAYDATA( pa,pa->dims );
		for( int i=0;i<count;++i ){
			s[i]=bbStringFromCString( pandas[i] );
			BBRETAIN( s[i] );
		}
	}
	
	flickcurl_array_free(pandas);

	return pa;
}

flickcurl_photo** bmx_flickcurl_panda_getphotos(flickcurl * fc, BBString * pandaName) {
	char *n=bbStringToCString( pandaName );
	
	flickcurl_photo** list = flickcurl_panda_getPhotos(fc, n);
	
	bbMemFree(n);
	
	return list;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int bmx_flickcurl_listofactivities_getactivitycount(flickcurl_activity** list) {
	int count = 0;
	
	for (int i = 0; list[i]; i++) {
		count++;
	}

	return count;
}

flickcurl_activity* bmx_flickcurl_listofactivities_getactivity(flickcurl_activity** list, int index) {
	return list[index];
}

flickcurl_activity** bmx_flickcurl_activity_usercomments(flickcurl * fc, int perPage, int page) {
	return flickcurl_activity_userComments(fc, perPage, page);
}

flickcurl_activity** bmx_flickcurl_activity_userphotos(flickcurl * fc, BBString * timeFrame, int perPage, int page) {
	char *t = 0;
	
	if (timeFrame != &bbEmptyString) {
		bbStringToCString( timeFrame );
	}
	
	flickcurl_activity** list = flickcurl_activity_userPhotos(fc, t, perPage, page);
	
	if (t) bbMemFree(t);
	
	return list;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_flickcurl_activity_gettype(flickcurl_activity* ac) {
	return bbStringFromCString(ac->type);
}

BBString * bmx_flickcurl_activity_getowner(flickcurl_activity* ac) {
	return bbStringFromCString(ac->owner);
}

BBString * bmx_flickcurl_activity_getownername(flickcurl_activity* ac) {
	return bbStringFromCString(ac->owner_name);
}

BBString * bmx_flickcurl_activity_getprimary(flickcurl_activity* ac) {
	return bbStringFromCString(ac->primary);
}

BBString * bmx_flickcurl_activity_getid(flickcurl_activity* ac) {
	return bbStringFromCString(ac->id);
}

BBString * bmx_flickcurl_activity_getsecret(flickcurl_activity* ac) {
	return bbStringFromCString(ac->secret);
}

int bmx_flickcurl_activity_getserver(flickcurl_activity* ac) {
	return ac->server;
}

int bmx_flickcurl_activity_getfarm(flickcurl_activity* ac) {
	return ac->farm;
}

int bmx_flickcurl_activity_getoldcomments(flickcurl_activity* ac) {
	return ac->comments_old;
}

int bmx_flickcurl_activity_getnewcomments(flickcurl_activity* ac) {
	return ac->comments_new;
}

int bmx_flickcurl_activity_getoldnotes(flickcurl_activity* ac) {
	return ac->notes_old;
}

int bmx_flickcurl_activity_getnewnotes(flickcurl_activity* ac) {
	return ac->notes_new;
}

int bmx_flickcurl_activity_getviews(flickcurl_activity* ac) {
	return ac->views;
}

int bmx_flickcurl_activity_getcomments(flickcurl_activity* ac) {
	return ac->comments;
}

int bmx_flickcurl_activity_getphotos(flickcurl_activity* ac) {
	return ac->photos;
}

int bmx_flickcurl_activity_getfaves(flickcurl_activity* ac) {
	return ac->faves;
}

int bmx_flickcurl_activity_getmore(flickcurl_activity* ac) {
	return ac->more;
}

BBString * bmx_flickcurl_activity_gettitle(flickcurl_activity* ac) {
	return bbStringFromCString(ac->title);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

flickcurl_institution** bmx_flickcurl_commons_getinstitutions(flickcurl * fc) {
	return flickcurl_commons_getInstitutions(fc);
}

int bmx_flickcurl_listofinstitutions_getinstitutioncount(flickcurl_institution** list) {
	int count = 0;
	
	for (int i = 0; list[i]; i++) {
		count++;
	}

	return count;
}

BBObject * bmx_flickcurl_listofinstitutions_getinstitution(flickcurl_institution** list, int index) {
	flickcurl_institution* in = list[index];
	
	if (in) {

		int count = 0;
	
		BBArray * pa = &bbEmptyArray;
		
		for (int i = 0; in->urls[i]; i++) {
			count++;
		}
		
		if (count > 0) {
			pa = bbArrayNew1D( "$",count );
			BBString **s=(BBString**)BBARRAYDATA( pa,pa->dims );
			for( int i=0;i<count;++i ){
				s[i]=bbStringFromCString( in->urls[i] );
				BBRETAIN( s[i] );
			}
		}

		return _bah_flickcurl_TFCInstitution__create(bbStringFromCString(in->nsid), in->date_launch, 
			bbStringFromCString(in->name), pa);
	}

	return &bbNullObject;
}

BBString * bmx_flickcurl_institution_url_type_label(flickcurl_institution_url_type urlType) {
	const char* name = flickcurl_get_institution_url_type_label(urlType);
	if (name) {
		return bbStringFromCString(name);
	} else {
		return &bbEmptyString;
	}
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

flickcurl_contact** bmx_flickcurl_contacts_getlist(flickcurl * fc, BBString * filter, int page, int perPage) {
	char * f = 0;
	
	if (filter != &bbEmptyString) {
		f = bbStringToCString(filter);
	}
	
	flickcurl_contact** list = flickcurl_contacts_getList(fc, f, page, perPage);
	
	if (f) bbMemFree(f);
	
	return list;
}

flickcurl_contact** bmx_flickcurl_contacts_getlistrecentlyuploaded(flickcurl * fc, int dateLastUpload, BBString * filter) {
	char * f = 0;
	
	if (filter != &bbEmptyString) {
		f = bbStringToCString(filter);
	}
	
	flickcurl_contact** list = flickcurl_contacts_getListRecentlyUploaded(fc, dateLastUpload, f);
	
	if (f) bbMemFree(f);
	
	return list;
}

flickcurl_contact** bmx_flickcurl_contacts_getpubliclist(flickcurl * fc, BBString * user, int page, int perPage) {
	char * u = bbStringToCString(user);
	flickcurl_contact** list = flickcurl_contacts_getPublicList(fc, u, page, perPage);
	bbMemFree(u);
	return list;
}

int bmx_flickcurl_listofcontacts_getcontactcount(flickcurl_contact** list) {
	int count = 0;
	
	for (int i = 0; list[i]; i++) {
		count++;
	}

	return count;
}

BBObject * bmx_flickcurl_listofcontacts_getcontact(flickcurl_contact** list, int index) {
	flickcurl_contact * c = list[index];

	return _bah_flickcurl_TFCContact__create(bbStringFromCString(c->nsid), bbStringFromCString(c->username), 
			c->iconserver, bbStringFromCString(c->realname), c->is_friend, c->is_family, c->ignored, c->uploaded);

}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int bmx_flickcurl_listofplaces_getplacecount(flickcurl_place ** list) {
	int count = 0;
	
	for (int i = 0; list[i]; i++) {
		count++;
	}

	return count;
}

flickcurl_place * bmx_flickcurl_listofplaces_getplace(flickcurl_place ** list, int index) {
	return list[index];
}

