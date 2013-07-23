/*
 Copyright (c) 2013 Bruce A Henderson
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

*/

#include "libwebsockets.h"

extern "C" {

#include "blitz.h"

	void _bah_libwebsockets_TLWSContextCreationInfo__initProtocols(BBObject * info, struct libwebsocket_protocols * protocols);
	struct libwebsocket_protocols * _bah_libwebsockets_TLWSContextCreationInfo__getProtocol(BBObject * info, int index, struct libwebsocket_protocols * offset);

	int _bah_libwebsockets_TLWSProtocol__callback(BBString * name, struct libwebsocket_context * context, struct libwebsocket * wsi, int reason, BBObject * user, void * in, int len);
	BBObject * _bah_libwebsockets_TLWSProtocol__objectCallback(BBString * name);

	struct libwebsocket_context * bmx_libwebsockets_create_context(BBObject * handle, struct lws_context_creation_info * info);
	int bmx_libwebsockets_service(struct libwebsocket_context * context, int timeout);
	int bmx_libwebsockets_serve_http_file(struct libwebsocket_context * context, struct libwebsocket * wsi, BBString * file, BBString * contentType);
	void bmx_libwebsocket_context_destroy(struct libwebsocket_context * context);

	struct lws_context_creation_info * bmx_libwebsockets_contextcreationinfo_new();
	void bmx_libwebsockets_contextcreationinfo_free(struct lws_context_creation_info * info);
	void bmx_libwebsockets_contextcreationinfo_setport(struct lws_context_creation_info * info, int port);
	void bmx_libwebsockets_contextcreationinfo_buildprotocollist(struct lws_context_creation_info * info, BBObject * handle, int size);

	struct libwebsocket_protocols * bmx_libwebsockets_protocol_create(BBString * name);
	int bmx_libwebsocket_callback_on_writable_all_protocol(struct libwebsocket_protocols * protocol);

	int bmx_libwebsockets_callback(struct libwebsocket_context * context, struct libwebsocket * wsi, enum libwebsocket_callback_reasons reason, void * user, void * in, size_t len);
	
	int bmx_libwebsockets_websocket_writetext(struct libwebsocket * wsi, unsigned char * buf, int length);
	int bmx_libwebsockets_websocket_write(struct libwebsocket * wsi, unsigned char * buf, int length, int protocol);
	int bmx_libwebsockets_lws_frame_is_binary(struct libwebsocket * wsi);
	int bmx_libwebsockets_remaining_packet_payload(struct libwebsocket * wsi);

	struct per_session_data_obj {
		BBObject * handle;
	};

}

// ********************************************************

struct libwebsocket_context * bmx_libwebsockets_create_context(BBObject * handle, struct lws_context_creation_info * info) {
	info->user = handle;
	info->extensions = libwebsocket_get_internal_extensions();

	return libwebsocket_create_context(info);
}

int bmx_libwebsockets_service(struct libwebsocket_context * context, int timeout) {
	return libwebsocket_service(context, timeout);
}

int bmx_libwebsockets_serve_http_file(struct libwebsocket_context * context, struct libwebsocket * wsi, BBString * file, BBString * contentType) {
	char * f = bbStringToUTF8String(file);
	char * c = bbStringToUTF8String(contentType);
	
	int ret = libwebsockets_serve_http_file(context, wsi, f, c);
	
	bbMemFree(c);
	bbMemFree(f);
	
	return ret;
}

void bmx_libwebsocket_context_destroy(struct libwebsocket_context * context) {
	libwebsocket_context_destroy(context);
}

// ********************************************************

struct lws_context_creation_info * bmx_libwebsockets_contextcreationinfo_new() {
	return (struct lws_context_creation_info*) calloc(1, sizeof(struct lws_context_creation_info));
}

void bmx_libwebsockets_contextcreationinfo_free(struct lws_context_creation_info * info) {
	free(info);
}

void bmx_libwebsockets_contextcreationinfo_setport(struct lws_context_creation_info * info, int port) {
	info->port = port;
}

void bmx_libwebsockets_contextcreationinfo_buildprotocollist(struct lws_context_creation_info * info, BBObject * handle, int size) {
	struct libwebsocket_protocols * protocols = (struct libwebsocket_protocols *) calloc(size + 1, sizeof(struct libwebsocket_protocols));
	
	info->protocols = protocols;

	_bah_libwebsockets_TLWSContextCreationInfo__initProtocols(handle, protocols);
	
	for (int i = 0; i < size; i++) {
		struct libwebsocket_protocols * protocol = _bah_libwebsockets_TLWSContextCreationInfo__getProtocol(handle, i, protocols);
		
		memcpy(protocols, protocol , sizeof(struct libwebsocket_protocols));
		
		protocols++;
	}
}

// ********************************************************

int bmx_libwebsockets_callback(struct libwebsocket_context * context, struct libwebsocket * wsi, enum libwebsocket_callback_reasons reason, void * user, void * in, size_t len, struct libwebsocket_protocols * protocol) {
	BBString * name = bbStringFromCString(protocol->name);
	struct per_session_data_obj *maxObj = (struct per_session_data_obj *)user;
	
	if (reason == LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION) {
		maxObj = 0;
	}

	// on a new connection, we should initialise the object
	if (maxObj && ((reason == LWS_CALLBACK_ESTABLISHED) || (!maxObj->handle))) {
		if (maxObj->handle && maxObj->handle != &bbNullObject) {
			BBRELEASE(maxObj->handle);
		}

		// call the object creation function
		maxObj->handle = _bah_libwebsockets_TLWSProtocol__objectCallback(name);

		// prevent early GC
		if (maxObj->handle != &bbNullObject) {
			BBRETAIN(maxObj->handle);
		}
	}
	
	int result = _bah_libwebsockets_TLWSProtocol__callback(name, context, wsi, reason, (maxObj) ? maxObj->handle : &bbNullObject, in, len);

	// release on close of connection
	if (reason == LWS_CALLBACK_CLOSED) {
		if (maxObj && maxObj->handle) {
			if (maxObj->handle != &bbNullObject) {
				BBRELEASE(maxObj->handle);
			}
		}
	}
	
	return result;
}

// ********************************************************

struct libwebsocket_protocols * bmx_libwebsockets_protocol_create(BBString * name) {
	struct libwebsocket_protocols * protocol = (struct libwebsocket_protocols *)calloc(1, sizeof(struct libwebsocket_protocols));
	
	protocol->name = bbStringToUTF8String(name);
	protocol->callback = bmx_libwebsockets_callback;
	protocol->per_session_data_size = sizeof(BBObject *);
	
	return protocol;
}

int bmx_libwebsocket_callback_on_writable_all_protocol(struct libwebsocket_protocols * protocol) {
	return libwebsocket_callback_on_writable_all_protocol(protocol);
}

// ********************************************************

int bmx_libwebsockets_websocket_writetext(struct libwebsocket * wsi, unsigned char * buf, int length) {
	return libwebsocket_write(wsi, buf, length, LWS_WRITE_TEXT);
}

int bmx_libwebsockets_websocket_write(struct libwebsocket * wsi, unsigned char * buf, int length, int protocol) {
	return libwebsocket_write(wsi, buf, length, static_cast<enum libwebsocket_write_protocol>(protocol));
}

int bmx_libwebsockets_lws_frame_is_binary(struct libwebsocket * wsi) {
	return lws_frame_is_binary(wsi);
}

int bmx_libwebsockets_remaining_packet_payload(struct libwebsocket * wsi) {
	return static_cast<int>(libwebsockets_remaining_packet_payload(wsi));
}
