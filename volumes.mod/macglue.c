/*
  Copyright (c) 2006-2016 Bruce A Henderson
 
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

#include <sys/param.h>
#include <sys/mount.h>

#include "brl.mod/blitz.mod/blitz.h"

struct statfs * bmx_vol_statfs_new() {
	return (struct statfs *)malloc(sizeof(struct statfs));
}

void bmx_vol_statfs_free(struct statfs * fs) {
	free(fs);
}

int bmx_vol_statfs(BBString * vol, struct statfs * fs) {
	char * v = bbStringToUTF8String(vol);
	int res = statfs(v, fs);
	bbMemFree(v);
	return res;
}

int bmx_vol_getfsstat(struct statfs ** buf) {
	int count = getfsstat(NULL, 0, MNT_WAIT);

	*buf = malloc(sizeof(struct statfs) * count);
	
	count = getfsstat(*buf, sizeof(struct statfs) * count, MNT_NOWAIT);

	return count;
}

void bmx_vol_getfsstat_free(struct statfs * buf) {
	free(buf);
}

void bmx_vol_statfs_array_copy(struct statfs * fs, struct statfs * buf, int index) {
	memcpy(fs, buf + index, sizeof(struct statfs));
}

int bmx_vol_statfs_bsize(struct statfs * fs) {
	return fs->f_bsize;
}

int bmx_vol_statfs_iosize(struct statfs * fs) {
	return fs->f_iosize;
}

void bmx_vol_statfs_blocks(struct statfs * fs, BBInt64 * value) {
	*value = fs->f_blocks;
}

void bmx_vol_statfs_bfree(struct statfs * fs, BBInt64 * value) {
	*value = fs->f_bfree;
}

void bmx_vol_statfs_bavail(struct statfs * fs, BBInt64 * value) {
	*value = fs->f_bavail;
}

void bmx_vol_statfs_files(struct statfs * fs, BBInt64 * value) {
	*value = fs->f_files;
}

void bmx_vol_statfs_ffree(struct statfs * fs, BBInt64 * value) {
	*value = fs->f_ffree;
}

BBString * bmx_vol_statfs_mntfromname(struct statfs * fs) {
	return bbStringFromUTF8String(fs->f_mntfromname);
}

BBString * bmx_vol_statfs_mntonname(struct statfs * fs) {
	return bbStringFromUTF8String(fs->f_mntonname);
}

BBString * bmx_vol_statfs_fstypename(struct statfs * fs) {
	return bbStringFromUTF8String(fs->f_fstypename);
}

