/*
 Copyright (c) 2011-2013 Bruce A Henderson
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of Bruce A Henderson nor the
       names of its contributors may be used to endorse or promote products
       derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <boost/filesystem.hpp>
#include <sstream>
#include <iostream>

class MaxDirectoryIterator;

extern "C" {

#include "blitz.h"

	BBString * bmx_filesystem_currentpath();
	BBString * bmx_filesystem_stripdir(BBString * path);
	BBString * bmx_filesystem_stripext(BBString * path);
	BBString * bmx_filesystem_extractdir(BBString * path);
	BBString * bmx_filesystem_extractext(BBString * path);
	BBString * bmx_filesystem_realpath(BBString * path);
	void bmx_filesystem_filesize(BBString * path, BBInt64 * size);
	int bmx_filesystem_filetype(BBString * path);
	int bmx_filesystem_filemode(BBString * path);
	int bmx_filesystem_createdir(BBString * path, int recurse);
	int bmx_filesystem_deletefile(BBString * path);
	int bmx_filesystem_renamefile(BBString * oldpath, BBString * newpath);
	int bmx_filesystem_copyfile(BBString * src, BBString * dst);
	int bmx_filesystem_copydir(BBString * src, BBString * dst);
	int bmx_filesystem_deletedir(BBString * path, int recurse);
	int bmx_filesystem_changedir(BBString * path);
	int bmx_filesystem_filetime(BBString * path);
	BBArray * bmx_filesystem_loaddir(BBString * dir);
	MaxDirectoryIterator * bmx_filesystem_readdir(BBString * path);
	BBString * bmx_filesystem_nextfile(MaxDirectoryIterator * iter);
	void bmx_filesystem_closedir(MaxDirectoryIterator * iter);
	BBString * bmx_filesystem_uniquepath();
	BBString * bmx_filesystem_tempdirectorypath();

}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class MaxDirectoryIterator
{
public:
	MaxDirectoryIterator(boost::filesystem::path & path)
		: iter(path)
	{
	}
	
	boost::filesystem::path NextFile() {
		boost::filesystem::path p;
		
		if (iter != boost::filesystem::directory_iterator()) {
			p = *iter++;
		}
		return p;
	}

private:
	boost::filesystem::directory_iterator iter;
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bbStringFromPath(boost::filesystem::path & path) {
	std::wstring s(path.generic_wstring());
	int l = s.size();
	unsigned short buf[l];
	for (int i = 0; i < l; i++) {
		buf[i] = s[i];
	}
	return bbStringFromShorts(buf, l);
}

boost::filesystem::path bbStringToPath(BBString * s) {
	int l = s->length;
	wchar_t buf[l + 1];
	
	for (int i = 0; i < l; i++) {
		buf[i] = s->buf[i];
	}
	buf[l] = 0;
	
	std::wstring w(buf);
	return boost::filesystem::path(w);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BBString * bmx_filesystem_currentpath() {
	boost::filesystem::path path(boost::filesystem::current_path());
	
	return bbStringFromPath(path);
}

BBString * bmx_filesystem_stripdir(BBString * path) {
	boost::filesystem::path p(bbStringToPath(path).filename());
	
	return bbStringFromPath(p);
}

BBString * bmx_filesystem_stripext(BBString * path) {
	boost::filesystem::path p(bbStringToPath(path).stem());
	
	return bbStringFromPath(p);
}

BBString * bmx_filesystem_extractdir(BBString * path) {
	boost::filesystem::path p(bbStringToPath(path).parent_path());
	
	return bbStringFromPath(p);
}

BBString * bmx_filesystem_extractext(BBString * path) {
	boost::filesystem::path p(bbStringToPath(path).extension());
	
	return bbStringFromPath(p);
}

BBString * bmx_filesystem_realpath(BBString * path) {
	boost::filesystem::path p(boost::filesystem::absolute(bbStringToPath(path)));
	
	return bbStringFromPath(p);
}

void bmx_filesystem_filesize(BBString * path, BBInt64 * size) {
	boost::filesystem::path p(bbStringToPath(path));
	*size = boost::filesystem::file_size(p);
}

int bmx_filesystem_filetype(BBString * path) {
	boost::filesystem::path p(bbStringToPath(path));
	boost::filesystem::file_status s = status(p);
	int t = ((int)s.type()) - 1;
	if (t < 0) t = 0;
	return t;
}

int bmx_filesystem_filemode(BBString * path) {
	// TODO : implement if boost ever supports file permissions...
	return 0;
}

int bmx_filesystem_createdir(BBString * path, int recurse) {
	boost::filesystem::path p(bbStringToPath(path));

	if (recurse) {
		return static_cast<int>(boost::filesystem::create_directories(p));
	} else {
		return static_cast<int>(boost::filesystem::create_directory(p));
	}
}

int bmx_filesystem_deletefile(BBString * path) {
	boost::filesystem::path p(bbStringToPath(path));
	
	return static_cast<int>(boost::filesystem::remove(p));
}

int bmx_filesystem_renamefile(BBString * oldpath, BBString * newpath) {
	boost::filesystem::path o(bbStringToPath(oldpath));
	boost::filesystem::path n(bbStringToPath(newpath));
	
	boost::system::error_code e;
	
	boost::filesystem::rename(o, n, e);
	
	return !e;
}

int bmx_filesystem_copyfile(BBString * src, BBString * dst) {
	boost::filesystem::path s(bbStringToPath(src));
	boost::filesystem::path d(bbStringToPath(dst));

	boost::system::error_code e;
	
	boost::filesystem::copy_file(s, d, e);

	return !e;
}

int bmx_filesystem_copydir(BBString * src, BBString * dst) {
	boost::filesystem::path s(bbStringToPath(src));
	boost::filesystem::path d(bbStringToPath(dst));

	boost::system::error_code e;
	
	copy_directory(s, d, e);

	return !e;
}

int bmx_filesystem_deletedir(BBString * path, int recurse) {
	boost::filesystem::path p(bbStringToPath(path));
	
	boost::system::error_code e;

	if (recurse) {
		boost::filesystem::remove_all(p, e);
	} else {
		boost::filesystem::remove(p, e);
	}

	return !e;
}

int bmx_filesystem_changedir(BBString * path) {
	boost::filesystem::path p(bbStringToPath(path));

	boost::system::error_code e;

	boost::filesystem::current_path(p, e);	

	return !e;
}

int bmx_filesystem_filetime(BBString * path) {
	boost::filesystem::path p(bbStringToPath(path));

	return last_write_time(p);
}

BBArray * bmx_filesystem_loaddir(BBString * dir) {
	boost::filesystem::path p(bbStringToPath(dir));
	std::vector<boost::filesystem::path> v;

	if (boost::filesystem::exists(p)) {
		copy(boost::filesystem::directory_iterator(p), boost::filesystem::directory_iterator(), back_inserter(v));
		
		
		int n = v.size();
		
		if (n > 0) {
			int i = 0;
			
			BBArray *arr = bbArrayNew1D( "$",n );
			BBString **s = (BBString**)BBARRAYDATA( arr, arr->dims );
		
			for (std::vector<boost::filesystem::path>::const_iterator it (v.begin()); it != v.end(); ++it) {
				boost::filesystem::path path((*it).filename());
				s[i] = bbStringFromPath( path );
				BBRETAIN( s[i] );
				i++;
			}
			return arr;
		}
	}
	
	return &bbEmptyArray;
}

MaxDirectoryIterator * bmx_filesystem_readdir(BBString * path) {
	boost::filesystem::path p(bbStringToPath(path));
	
	if (boost::filesystem::exists(p)) {
		return new MaxDirectoryIterator(p);
	} else {
		return 0;
	}
}

BBString * bmx_filesystem_nextfile(MaxDirectoryIterator * iter) {
	boost::filesystem::path p(iter->NextFile().filename());
	return bbStringFromPath(p);
}

void bmx_filesystem_closedir(MaxDirectoryIterator * iter) {
	delete iter;
}

BBString * bmx_filesystem_uniquepath() {
	boost::filesystem::path path(boost::filesystem::unique_path());
	
	return bbStringFromPath(path);
}

BBString * bmx_filesystem_tempdirectorypath() {
	boost::filesystem::path path(boost::filesystem::temp_directory_path());
	
	return bbStringFromPath(path);
}

