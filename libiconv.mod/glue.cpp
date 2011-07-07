/*
 Copyright (c) 2010-2011 Bruce A Henderson
 
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

#ifndef __WIN32__
#include <iconv.h>
#include <errno.h>
#else
#include "iconv.h"
#endif

extern "C" {
#ifdef __APPLE__
	size_t bmx_iconv(iconv_t handle, char ** inbuf, size_t * inBytesLeft, char ** outbuf, size_t * outBytesLeft);
#else
	size_t bmx_iconv(iconv_t handle, const char ** inbuf, size_t * inBytesLeft, char ** outbuf, size_t * outBytesLeft);
#endif
	int bmx_getErrno();
}


#ifdef __APPLE__
size_t bmx_iconv(iconv_t handle, char ** inbuf, size_t * inBytesLeft, char ** outbuf, size_t * outBytesLeft) {
#else
size_t bmx_iconv(iconv_t handle, const char ** inbuf, size_t * inBytesLeft, char ** outbuf, size_t * outBytesLeft) {
#endif
#ifdef __WIN32__
	return libiconv(handle, inbuf, inBytesLeft, outbuf, outBytesLeft);
#else
	return iconv(handle, inbuf, inBytesLeft, outbuf, outBytesLeft);
#endif
}

int bmx_getErrno() {
	return errno;
}
