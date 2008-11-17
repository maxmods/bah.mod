#include "flickcurl.h"

extern "C" {

#include "blitz.h"

	BBObject * _bah_flickcurl_TFCPhotoField__create(BBString * svalue, flickcurl_photo_field_type value, flickcurl_field_value_type type);


	flickcurl * bmx_flickcurl_new();
	void bmx_flickcurl_free(flickcurl * fc);

	void bmx_flickcurl_setapikey(flickcurl * fc, BBString * apiKey);
	void bmx_flickcurl_setauthtoken(flickcurl * fc, BBString * authToken);
	void bmx_flickcurl_setsharedsecret(flickcurl * fc, BBString * sharedSecret);

	BBString * bmx_flickcurl_getfrob(flickcurl * fc);
	BBString * bmx_flickcurl_getfulltoken(flickcurl * fc, BBString * frob);

	flickcurl_photo * bmx_flickcurl_photosgetinfo(flickcurl * fc, BBString * photoID);

	BBObject * bmx_flickcurl_photo_getfield(flickcurl_photo * photo, int index);
	BBString * bmx_flickcurl_photo_geturi(flickcurl_photo * photo);
	BBString * bmx_flickcurl_photo_getsourceuri(flickcurl_photo * photo, int size);
	void bmx_flickcurl_photo_free(flickcurl_photo * photo);

	BBString * bmx_flickcurl_photofield_getlabel(flickcurl_photo_field_type fieldType);
	BBString * bmx_flickcurl_photofield_getvaluetypelabel(flickcurl_field_value_type valueType);

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

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_flickcurl_photofield_getlabel(flickcurl_photo_field_type fieldType) {
	return bbStringFromCString(flickcurl_get_photo_field_label(fieldType));
}

BBString * bmx_flickcurl_photofield_getvaluetypelabel(flickcurl_field_value_type valueType) {
	return bbStringFromCString(flickcurl_get_field_value_type_label(valueType));
}
