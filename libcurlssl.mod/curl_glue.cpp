/*
 Copyright (c) 2007-2009 Bruce A Henderson
 
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

extern "C" {

class MaxHttppost;
class MaxSList;

#include <blitz.h>
#include <curl/curl.h>

	/* for Win32, we need to reference the functions, otherwise we get "undefined reference to _imp__ ...." */
	#ifdef WIN32
	extern CURL *curl_easy_init(void);
	extern CURLcode curl_easy_setopt(CURL *curl, CURLoption option, ...);
	#endif

	CURLcode bmx_curl_easy_setopt_long(CURL *curl, CURLoption option, long param);
	CURLcode bmx_curl_easy_setopt_str(CURL *curl, CURLoption option, char * param);
	CURLcode bmx_curl_easy_setopt_obj(CURL *curl, CURLoption option, void * param);
	CURLcode bmx_curl_easy_setopt_bbint64(CURL *curl, CURLoption option, BBInt64 param);

	MaxHttppost * bmx_curl_new_httppostPtr();
	void bmx_curl_formadd_name_content(MaxHttppost * postPtr, const char * name, const char * content);
	void bmx_curl_formadd_name_content_type(MaxHttppost * postPtr, const char * name, const char * content, const char * type);
	void bmx_curl_formadd_name_file(MaxHttppost * postPtr, const char * name, const char * file, int kind);
	void bmx_curl_formadd_name_file_type(MaxHttppost * postPtr, const char * name, const char * file, const char * type, int kind);
	void bmx_curl_formadd_name_buffer(MaxHttppost * postPtr, const char * name, const char * bname, void * buffer, int length);
	void bmx_curl_httppostPtr_delete(MaxHttppost * postPtr);
	
	void bmx_curl_easy_setopt_httppost(CURL *curl, MaxHttppost * postPtr);
	
	CURLcode bmx_curl_easy_getinfo_string(CURL *curl, CURLINFO info, char * s);
	CURLcode bmx_curl_easy_getinfo_long(CURL *curl, CURLINFO info, long * value);
	CURLcode bmx_curl_easy_getinfo_double(CURL *curl, CURLINFO info, double * value);
	char * bmx_curl_easy_getinfo_obj(CURL * curl, CURLINFO info, CURLcode * error);
	MaxSList * bmx_curl_easy_getinfo_slist(CURL * curl, CURLINFO info, CURLcode * error);
	
	int bmx_curl_multiselect(CURLM * multi, double timeout);
	
	CURLMSG bmx_curl_CURLMsg_msg(CURLMsg * message);
	CURLcode bmx_curl_CURLMsg_result(CURLMsg * message);
	CURL * bmx_curl_CURLMsg_easy_handle(CURLMsg * message);
	
	struct curl_slist * bmx_curl_get_slist(MaxSList * slist);
	void bmx_curl_slist_free(MaxSList * slist);
	char * bmx_curl_get_slist_data(curl_slist * slist);
	curl_slist * bmx_curl_get_slist_next(curl_slist * slist);
	int bmx_curl_slist_count(MaxSList * slist);
	MaxSList * bmx_curl_slist_new();
	void bmx_curl_add_element(MaxSList * slist, const char * txt);
	
	void bmx_curl_easy_setopt_slist(CURL *curl, CURLoption option, MaxSList * slist);
	
	void bmx_curl_multi_setopt_long(CURLM * multi, CURLMoption option, long value);
}


CURLcode bmx_curl_easy_setopt_long(CURL *curl, CURLoption option, long param) {
	return curl_easy_setopt(curl, option, param);
}

CURLcode bmx_curl_easy_setopt_str(CURL *curl, CURLoption option, char * param) {
	return curl_easy_setopt(curl, option, param);
}

CURLcode bmx_curl_easy_setopt_obj(CURL *curl, CURLoption option, void * param) {
	return curl_easy_setopt(curl, option, param);
}

CURLcode bmx_curl_easy_setopt_bbint64(CURL *curl, CURLoption option, BBInt64 param) {
	return curl_easy_setopt(curl, option, param);
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class MaxHttppost
{
public:
	MaxHttppost();
	~MaxHttppost();
	
	void formadd_name_content(const char * name, const char * content);
	void formadd_name_content_type(const char * name, const char * content, const char * type);
	void formadd_name_file(const char * name, const char * file, int kind);
	void formadd_name_file_type(const char * name, const char * file, const char * type, int kind);
	void formadd_name_buffer(const char * name, const char * bname, void * buffer, int length);

	void setopt(CURL * curl);
	
private:
	struct curl_httppost * post;
	struct curl_httppost * last;
};

MaxHttppost::MaxHttppost()
{
	post = NULL;
	last = NULL;
}

MaxHttppost::~MaxHttppost() {
	curl_formfree(post);
}


MaxHttppost * bmx_curl_new_httppostPtr() {
	return new MaxHttppost();
}

void MaxHttppost::formadd_name_content(const char * name, const char * content) {
	curl_formadd(&post, &last, CURLFORM_PTRNAME, name, CURLFORM_PTRCONTENTS, content, CURLFORM_END);
}

void MaxHttppost::formadd_name_content_type(const char * name, const char * content, const char * type) {
	curl_formadd(&post, &last, CURLFORM_PTRNAME, name, CURLFORM_PTRCONTENTS, content, CURLFORM_CONTENTTYPE, type, CURLFORM_END);
}

void MaxHttppost::formadd_name_file(const char * name, const char * file, int kind) {
	if (kind == 1) {
		curl_formadd(&post, &last, CURLFORM_PTRNAME, name, CURLFORM_FILE, file, CURLFORM_END);
	} else {
		curl_formadd(&post, &last, CURLFORM_PTRNAME, name, CURLFORM_FILECONTENT, file, CURLFORM_END);
	}
}

void MaxHttppost::formadd_name_file_type(const char * name, const char * file, const char * type, int kind) {
	if (kind == 1) {
		curl_formadd(&post, &last, CURLFORM_PTRNAME, name, CURLFORM_FILE, file, CURLFORM_CONTENTTYPE, type, CURLFORM_END);
	} else {
		curl_formadd(&post, &last, CURLFORM_PTRNAME, name, CURLFORM_FILECONTENT, file, CURLFORM_CONTENTTYPE, type, CURLFORM_END);
	}
}

void MaxHttppost::formadd_name_buffer(const char * name, const char * bname, void * buffer, int length) {
	curl_formadd(&post, &last, CURLFORM_PTRNAME, name, CURLFORM_BUFFER, bname, CURLFORM_BUFFERPTR, buffer, CURLFORM_BUFFERLENGTH, length, CURLFORM_END);
}

void MaxHttppost::setopt(CURL * curl) {
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class MaxSList
{
public:
	MaxSList();
	~MaxSList();
	
	struct curl_slist * getSList();
	void setCount();
	int getCount();
	CURLcode getinfo(CURL * curl, CURLINFO info);
	void addElement(const char * txt);
	
private:
	struct curl_slist * slist;
	int count;
};

MaxSList::MaxSList()
{
	slist = NULL;
	count = 0;
}

MaxSList::~MaxSList() {
	curl_slist_free_all(slist);
}

struct curl_slist * MaxSList::getSList() {
	return slist;
}

void MaxSList::setCount() {
	struct curl_slist * s = slist;
	count = 0;
   	while (s != NULL) {
		count++;
		s = s->next;
     }
}

int MaxSList::getCount() {
	return count;
}

CURLcode MaxSList::getinfo(CURL * curl, CURLINFO info) {
	CURLcode error = curl_easy_getinfo(curl, info, &slist);
	if (error == CURLE_OK) {
		setCount();
	}
	return error;
}

void MaxSList::addElement(const char * txt) {
	slist = curl_slist_append(slist, txt);
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void bmx_curl_formadd_name_content(MaxHttppost * postPtr, const char * name, const char * content) {
	postPtr->formadd_name_content(name, content);
}

void bmx_curl_formadd_name_content_type(MaxHttppost * postPtr, const char * name, const char * content, const char * type){
	postPtr->formadd_name_content_type(name, content, type);
}

void bmx_curl_formadd_name_file(MaxHttppost * postPtr, const char * name, const char * file, int kind) {
	postPtr->formadd_name_file(name, file, kind);
}

void bmx_curl_formadd_name_file_type(MaxHttppost * postPtr, const char * name, const char * file, const char * type, int kind) {
	postPtr->formadd_name_file_type(name, file, type, kind);
}

void bmx_curl_formadd_name_buffer(MaxHttppost * postPtr, const char * name, const char * bname, void * buffer, int length) {
	postPtr->formadd_name_buffer(name, bname, buffer, length);
}

void bmx_curl_httppostPtr_delete(MaxHttppost * postPtr) {
	delete postPtr;
}

void bmx_curl_easy_setopt_httppost(CURL *curl, MaxHttppost * postPtr) {
	postPtr->setopt(curl);
}

CURLcode bmx_curl_easy_getinfo_string(CURL *curl, CURLINFO info, char * s) {
	return curl_easy_getinfo(curl, info, s);

}

CURLcode bmx_curl_easy_getinfo_long(CURL *curl, CURLINFO info, long * value) {
	return curl_easy_getinfo(curl, info, value);
}

CURLcode bmx_curl_easy_getinfo_double(CURL *curl, CURLINFO info, double * value) {
	return curl_easy_getinfo(curl, info, value);
}

char * bmx_curl_easy_getinfo_obj(CURL * curl, CURLINFO info, CURLcode * error) {
	char * priv = NULL;
	*error = curl_easy_getinfo(curl, info, priv);
	return priv;
}

int bmx_curl_multiselect(CURLM * multi, double timeout) {
	fd_set          readfds;
	fd_set          writefds;
	fd_set          exceptfds;
	int             maxfd;
	struct timeval  to;
	unsigned long conv;

	conv = (unsigned long) (timeout * 1000000.0);
	to.tv_sec = conv / 1000000;
	to.tv_usec = conv % 1000000;
		
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	curl_multi_fdset(multi, &readfds, &writefds, &exceptfds, &maxfd);
	return select(maxfd + 1, &readfds, &writefds, &exceptfds, &to);
}

CURLMSG bmx_curl_CURLMsg_msg(CURLMsg * message) {
	return message->msg;
}

CURLcode bmx_curl_CURLMsg_result(CURLMsg * message) {
	return message->data.result;
}

CURL * bmx_curl_CURLMsg_easy_handle(CURLMsg * message) {
	return message->easy_handle;
}

MaxSList * bmx_curl_easy_getinfo_slist(CURL * curl, CURLINFO info, CURLcode * error) {
	MaxSList * mslist = new MaxSList;
	
	*error = mslist->getinfo(curl, info);
	
	return mslist;
}

MaxSList * bmx_curl_slist_new() {
	return new MaxSList;
}

struct curl_slist * bmx_curl_get_slist(MaxSList * slist) {
	return slist->getSList();
}

int bmx_curl_slist_count(MaxSList * slist) {
	return slist->getCount();
}

void bmx_curl_slist_free(MaxSList * slist) {
	delete slist;
}

char * bmx_curl_get_slist_data(struct curl_slist * slist) {

	if (slist) {
		return slist->data;
	}
	
	return NULL;
}

struct curl_slist * bmx_curl_get_slist_next(struct curl_slist * slist) {

	if (slist) {
		return slist->next;
	}
	
	return NULL;
}

void bmx_curl_add_element(MaxSList * slist, const char * txt) {
	slist->addElement(txt);
}

void bmx_curl_easy_setopt_slist(CURL *curl, CURLoption option, MaxSList * slist) {
	curl_easy_setopt(curl, option, slist->getSList());
}

void bmx_curl_multi_setopt_long(CURLM * multi, CURLMoption option, long value) {
	curl_multi_setopt(multi, option, value);
}




