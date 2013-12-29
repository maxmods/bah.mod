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

#include "libsmbclient.h"

extern "C" {

#include "blitz.h"

	BBObject * _bah_libsmbclient_TSMBCAuth__new(SMBCCTX * context, BBString * server, BBString * share);
	BBString * _bah_libsmbclient_TSMBCAuth__workgroup(BBObject * auth);
	BBString * _bah_libsmbclient_TSMBCAuth__username(BBObject * auth);
	BBString * _bah_libsmbclient_TSMBCAuth__password(BBObject * auth);
	
	BBObject * _bah_libsmbclient_TSMBDirHandle__new(SMBCFILE * fd);
	BBObject * _bah_libsmbclient_TSMBDirent__new(BBString * name, int type);
	
	
	void _bah_libsmbclient_TSMBC__authDataCallback(BBObject * auth);
	

	void bmx_smbc_get_auth_data_fn(const char *srv, const char *shr, char *wg, int wglen, 
		char *un, int unlen, char *pw, int pwlen);

	SMBCCTX * bmx_smbc_new_context(int smbDebug);
	BBObject * bmx_smbc_opendir(SMBCCTX * context, BBString * path);
	BBObject * bmx_smbc_readdir(SMBCCTX * context, SMBCFILE * dir);
	BBObject * bmx_smbc_closedir(SMBCCTX * context, SMBCFILE * dir);

	void bmx_smbc_purgecachedservers(SMBCCTX * context);

}


// ********************************************************

void bmx_smbc_get_auth_data_fn(SMBCCTX * context, const char *srv, const char *shr, char *wg, int wglen, 
		char *un, int unlen, char *pw, int pwlen) {

	BBObject * auth = _bah_libsmbclient_TSMBCAuth__new(context, bbStringFromUTF8String(srv), bbStringFromUTF8String(shr));

	// callback	and populate
	_bah_libsmbclient_TSMBC__authDataCallback(auth);
	
	BBString * w = _bah_libsmbclient_TSMBCAuth__workgroup(auth);
	BBString * u = _bah_libsmbclient_TSMBCAuth__username(auth);
	BBString * p = _bah_libsmbclient_TSMBCAuth__password(auth);
	
	// fill char buffers
	
	if (w != &bbEmptyString) {
		char * workgroup = bbStringToUTF8String(_bah_libsmbclient_TSMBCAuth__workgroup(auth));
		strncpy(wg, workgroup, wglen - 1);
		bbMemFree(workgroup);
	}
	
	if (u != &bbEmptyString) {
		char * username = bbStringToUTF8String(_bah_libsmbclient_TSMBCAuth__username(auth));
		strncpy(un, username, unlen - 1);
		bbMemFree(username);
	}

	if (p != &bbEmptyString) {
		char * password = bbStringToUTF8String(_bah_libsmbclient_TSMBCAuth__password(auth));
		strncpy(pw, password, pwlen - 1);
		bbMemFree(password);
	}
	
}

SMBCCTX * bmx_smbc_new_context(int smbDebug) {

	SMBCCTX * context = smbc_new_context();
	
	smbc_setDebug(context, smbDebug);
	smbc_setFunctionAuthDataWithContext(context, bmx_smbc_get_auth_data_fn);

	if (!smbc_init_context(context)){
		smbc_free_context(context, 1);
		return 0;
	}

	return context;
}

BBObject * bmx_smbc_opendir(SMBCCTX * context, BBString * path) {
	char * p = bbStringToUTF8String(path);

	BBObject * dir = _bah_libsmbclient_TSMBDirHandle__new(smbc_getFunctionOpendir(context)(context, p));
	
	bbMemFree(p);
	
	return dir;
}

BBObject * bmx_smbc_readdir(SMBCCTX * context, SMBCFILE * dir) {

	struct smbc_dirent * dirent = smbc_getFunctionReaddir(context)(context, dir);
	
	if (dirent) {
		BBObject * file = _bah_libsmbclient_TSMBDirent__new(bbStringFromUTF8String(dirent->name), dirent->smbc_type);
		return file;
	}
	
	return &bbNullObject;

}

BBObject * bmx_smbc_closedir(SMBCCTX * context, SMBCFILE * dir) {
	smbc_getFunctionClose(context)(context, dir);
}

void bmx_smbc_purgecachedservers(SMBCCTX * context) {
	smbc_getFunctionPurgeCachedServers(context)(context);
}

