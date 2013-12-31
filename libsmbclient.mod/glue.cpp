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
#include <map>
#include <errno.h>

extern "C" {

#include "blitz.h"

	BBObject * _bah_libsmbclient_TSMBCAuth__new(BBObject * context, BBString * server, BBString * share);
	BBString * _bah_libsmbclient_TSMBCAuth__workgroup(BBObject * auth);
	BBString * _bah_libsmbclient_TSMBCAuth__username(BBObject * auth);
	BBString * _bah_libsmbclient_TSMBCAuth__password(BBObject * auth);
	
	BBObject * _bah_libsmbclient_TSMBCDirHandle__new(SMBCFILE * fd);
	BBObject * _bah_libsmbclient_TSMBCDirent__new(BBString * name, int type);
	
	
	void _bah_libsmbclient_TSMBC__authDataCallback(BBObject * auth);
	

	void bmx_smbc_get_auth_data_fn(const char *srv, const char *shr, char *wg, int wglen, 
		char *un, int unlen, char *pw, int pwlen);

	SMBCCTX * bmx_smbc_new_context(BBObject * maxHandle, int smbDebug);
	void bmx_smbc_free_context(SMBCCTX * context);
	
	BBObject * bmx_smbc_opendir(SMBCCTX * context, BBString * path);
	BBObject * bmx_smbc_readdir(SMBCCTX * context, SMBCFILE * dir);
	BBObject * bmx_smbc_closedir(SMBCCTX * context, SMBCFILE * dir);

	int bmx_smbc_mkdir(SMBCCTX * context, BBString * path, int mode);
	int bmx_smbc_rmdir(SMBCCTX * context, BBString * path);
	int bmx_smbc_unlink(SMBCCTX * context, BBString * path);

	int bmx_smbc_filetype(SMBCCTX * context, BBString * path);
	int bmx_smbc_filesize(SMBCCTX * context, BBString * path);
	int bmx_smbc_filetime(SMBCCTX * context, BBString * path);
	int bmx_smbc_filemode(SMBCCTX * context, BBString * path);

	void bmx_smbc_purgecachedservers(SMBCCTX * context);
	int bmx_smbc_getoptioncasesensitive(SMBCCTX * context);
	void bmx_smbc_setoptioncasesensitive(SMBCCTX * context, int value);

	SMBCFILE * bmx_smbc_open(SMBCCTX * context, BBString * path, int readable, int writeable);
	int bmx_smbc_seek(SMBCCTX * context, SMBCFILE * file, int pos);
	int bmx_smbc_read(SMBCCTX * context, SMBCFILE * file, char * buf, int count);
	int bmx_smbc_write(SMBCCTX * context, SMBCFILE * file, char * buf, int count);
	void bmx_smbc_close(SMBCCTX * context, SMBCFILE * file);

}

// ********************************************************

typedef std::map<SMBCCTX*, BBObject*> ContextMap;

static ContextMap contextMap;

void cxbind( SMBCCTX *obj, BBObject *peer ) {
	if( !obj || peer==&bbNullObject ) return;
	contextMap.insert( std::make_pair( obj,peer ) );
	BBRETAIN( peer );
}

BBObject *cxfind( SMBCCTX *obj ){
	if( !obj ) return &bbNullObject;
	ContextMap::iterator it = contextMap.find( obj );
	if( it != contextMap.end() ) return it->second;
	return &bbNullObject;
}

void cxunbind(SMBCCTX *obj) {
	BBObject * peer = cxfind(obj);
	if (peer != &bbNullObject) {
		contextMap.erase(obj);
		BBRELEASE(peer);
	}
}

// ********************************************************

void bmx_smbc_get_auth_data_fn(SMBCCTX * context, const char *srv, const char *shr, char *wg, int wglen, 
		char *un, int unlen, char *pw, int pwlen) {

	BBObject * auth = _bah_libsmbclient_TSMBCAuth__new((BBObject*)smbc_getOptionUserData(context), bbStringFromUTF8String(srv),
			bbStringFromUTF8String(shr));

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

SMBCCTX * bmx_smbc_new_context(BBObject * maxHandle, int smbDebug) {

	SMBCCTX * context = smbc_new_context();
	
	smbc_setDebug(context, smbDebug);
	smbc_setOptionUserData(context, maxHandle);
	smbc_setFunctionAuthDataWithContext(context, bmx_smbc_get_auth_data_fn);

	if (!smbc_init_context(context)){
		smbc_free_context(context, 1);
		return 0;
	}
	
	cxbind(context, maxHandle);

	return context;
}

void bmx_smbc_free_context(SMBCCTX * context) {
	cxunbind(context);
	smbc_free_context(context, 1);
}

BBObject * bmx_smbc_opendir(SMBCCTX * context, BBString * path) {
	char * p = bbStringToUTF8String(path);

	BBObject * dir = _bah_libsmbclient_TSMBCDirHandle__new(smbc_getFunctionOpendir(context)(context, p));
	
	bbMemFree(p);
	
	return dir;
}

BBObject * bmx_smbc_readdir(SMBCCTX * context, SMBCFILE * dir) {

	struct smbc_dirent * dirent = smbc_getFunctionReaddir(context)(context, dir);
	
	if (dirent) {
		BBObject * file = _bah_libsmbclient_TSMBCDirent__new(bbStringFromUTF8String(dirent->name), dirent->smbc_type);
		return file;
	}
	
	return &bbNullObject;

}

BBObject * bmx_smbc_closedir(SMBCCTX * context, SMBCFILE * dir) {
	smbc_getFunctionClose(context)(context, dir);
}

int bmx_smbc_mkdir(SMBCCTX * context, BBString * path, int mode) {
	char * p = bbStringToUTF8String(path);
	int res = smbc_getFunctionMkdir(context)(context, p, static_cast<mode_t>(mode));
	bbMemFree(p);
	
	return res;
}

int bmx_smbc_rmdir(SMBCCTX * context, BBString * path) {
	char * p = bbStringToUTF8String(path);
	int res = smbc_getFunctionRmdir(context)(context, p);
	bbMemFree(p);
	
	return res;
}

int bmx_smbc_unlink(SMBCCTX * context, BBString * path) {
	char * p = bbStringToUTF8String(path);
	int res = smbc_getFunctionUnlink(context)(context, p);
	bbMemFree(p);
	
	return res;
}

void bmx_smbc_purgecachedservers(SMBCCTX * context) {
	smbc_getFunctionPurgeCachedServers(context)(context);
}

int bmx_smbc_stat(SMBCCTX * context, BBString * path, struct stat * s) {
	char * p = bbStringToUTF8String(path);
	int res = smbc_getFunctionStat(context)(context, p, s);
	bbMemFree(p);
	return res;
}

int bmx_smbc_filetype(SMBCCTX * context, BBString * path) {
	struct stat s;
	int res = bmx_smbc_stat(context, path, &s);
	
	if (res) {
		return 0;
	}
	
	switch(s.st_mode & S_IFMT) {
		case S_IFDIR:
			return 2;
		case S_IFREG:
			return 1; 
	}

	return 0;
}

int bmx_smbc_filesize(SMBCCTX * context, BBString * path) {
	struct stat s;
	int res = bmx_smbc_stat(context, path, &s);
	
	if (res) {
		return 0;
	}
	
	return static_cast<int>(s.st_size);
}

int bmx_smbc_filetime(SMBCCTX * context, BBString * path) {
	struct stat s;
	int res = bmx_smbc_stat(context, path, &s);
	
	if (res) {
		return 0;
	}
	
	return static_cast<int>(s.st_mtime);
}

int bmx_smbc_filemode(SMBCCTX * context, BBString * path) {
	struct stat s;
	int res = bmx_smbc_stat(context, path, &s);
	
	if (res) {
		return -1;
	}
	
	return static_cast<int>(s.st_mode) & 511;
}

int bmx_smbc_getoptioncasesensitive(SMBCCTX * context) {
	return static_cast<int>(smbc_getOptionCaseSensitive(context));
}

void bmx_smbc_setoptioncasesensitive(SMBCCTX * context, int value) {
	smbc_setOptionCaseSensitive(context, static_cast<smbc_bool>(value));
}

// ********************************************************

SMBCFILE * bmx_smbc_open(SMBCCTX * context, BBString * path, int readable, int writeable) {

	char * p = bbStringToUTF8String(path);
	
	int flags = 0;
	
	if (readable && writeable) {
		flags = O_RDWR | O_CREAT;
	} else if (readable) {
		flags = O_RDONLY;
	} else {
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	}
printf("flags = %d : %s\n", flags, p);
	SMBCFILE * file = smbc_getFunctionOpen(context)(context, p, flags, (mode_t)0);
	
	bbMemFree(p);
	
	if (file) {
printf("got file handle\n");fflush(stdout);
		return file;
	}

printf("errno = %d", errno);
	return 0;
}

int bmx_smbc_seek(SMBCCTX * context, SMBCFILE * file, int pos) {
	return static_cast<int>(smbc_getFunctionLseek(context)(context, file, static_cast<off_t>(pos), SEEK_SET));
}

int bmx_smbc_read(SMBCCTX * context, SMBCFILE * file, char * buf, int count) {
	return static_cast<int>(smbc_getFunctionRead(context)(context, file, buf, static_cast<size_t>(count)));
}

int bmx_smbc_write(SMBCCTX * context, SMBCFILE * file, char * buf, int count) {
	return static_cast<int>(smbc_getFunctionWrite(context)(context, file, buf, static_cast<size_t>(count)));
}

void bmx_smbc_close(SMBCCTX * context, SMBCFILE * file) {
	smbc_getFunctionClose(context)(context, file);
}

