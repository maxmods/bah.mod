/*
  Copyright 2008 Bruce A Henderson

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

	flickcurl * bmx_flickcurl_new();
	void bmx_flickcurl_free(flickcurl * fc);

	void bmx_flickcurl_setapikey(flickcurl * fc, BBString * apiKey);
	void bmx_flickcurl_setauthtoken(flickcurl * fc, BBString * authToken);
	void bmx_flickcurl_setsharedsecret(flickcurl * fc, BBString * sharedSecret);

	BBString * bmx_flickcurl_getfrob(flickcurl * fc);
	BBString * bmx_flickcurl_getfulltoken(flickcurl * fc, BBString * frob);

	flickcurl_photo * bmx_flickcurl_photosgetinfo(flickcurl * fc, BBString * photoID);

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

	BBString * bmx_flickcurl_photofield_getlabel(flickcurl_photo_field_type fieldType);
	BBString * bmx_flickcurl_photofield_getvaluetypelabel(flickcurl_field_value_type valueType);

	flickcurl_place * bmx_flickcurl_resolveplaceid(flickcurl * fc, BBString * placeID);
	flickcurl_place * bmx_flickcurl_resolveplaceurl(flickcurl * fc, BBString * url);
	flickcurl_place * bmx_flickcurl_findplacebylatlon(flickcurl * fc, double lat, double lon, int accuracy);

	BBString * bmx_flickcurl_place_gettypelabel(flickcurl_place_type placeType);
	BBString * bmx_flickcurl_place_getname(flickcurl_place * place, int index);
	BBString * bmx_flickcurl_place_getid(flickcurl_place * place, int index);
	BBString * bmx_flickcurl_place_geturl(flickcurl_place * place, int index);
	flickcurl_place_type bmx_flickcurl_place_gettype(flickcurl_place * place);
	BBString * bmx_flickcurl_place_getwoeid(flickcurl_place * place, int index);
	BBObject * bmx_flickcurl_place_getlocation(flickcurl_place * place);
	int bmx_flickcurl_place_getcount(flickcurl_place * place);
	void bmx_flickcurl_place_free(flickcurl_place * place);

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
	void bmx_flickcurl_searchparams_delete(flickcurl_search_params * params);

	flickcurl_photo * bmx_flickcurl_tag_getphoto(flickcurl_tag * tag);
	BBString * bmx_flickcurl_tag_getid(flickcurl_tag * tag);
	BBString * bmx_flickcurl_tag_getauthor(flickcurl_tag * tag);
	BBString * bmx_flickcurl_tag_getauthorname(flickcurl_tag * tag);
	BBString * bmx_flickcurl_tag_getraw(flickcurl_tag * tag);
	BBString * bmx_flickcurl_tag_getcooked(flickcurl_tag * tag);
	int bmx_flickcurl_tag_getmachinetag(flickcurl_tag * tag);
	int bmx_flickcurl_tag_getcount(flickcurl_tag * tag);

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


