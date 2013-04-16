/*
 Copyright (c) 2013 Bruce A Henderson
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

#include "archive.h"
#include "archive_entry.h"

extern "C" {

#include "blitz.h"

	struct archive * bmx_libarchive_read_archive_new();

	int bmx_libarchive_archive_read_support_filter_all(struct archive * arc);
	int bmx_libarchive_archive_read_support_filter_bzip2(struct archive * arc);
	int bmx_libarchive_archive_read_support_filter_compress(struct archive * arc);
	int bmx_libarchive_archive_read_support_filter_gzip(struct archive * arc);
	int bmx_libarchive_archive_read_support_filter_grzip(struct archive * arc);
	int bmx_libarchive_archive_read_support_filter_lrzip(struct archive * arc);
	int bmx_libarchive_archive_read_support_filter_lzip(struct archive * arc);
	int bmx_libarchive_archive_read_support_filter_lzma(struct archive * arc);
	int bmx_libarchive_archive_read_support_filter_lzop(struct archive * arc);
	int bmx_libarchive_archive_read_support_filter_none(struct archive * arc);
	int bmx_libarchive_archive_read_support_filter_rpm(struct archive * arc);
	int bmx_libarchive_archive_read_support_filter_uu(struct archive * arc);
	int bmx_libarchive_archive_read_support_filter_xz(struct archive * arc);

	int bmx_libarchive_archive_read_support_format_all(struct archive * arc);
	int bmx_libarchive_archive_read_support_format_7zip(struct archive * arc);
	int bmx_libarchive_archive_read_support_format_ar(struct archive * arc);
	int bmx_libarchive_archive_read_support_format_by_code(struct archive * arc, int code);
	int bmx_libarchive_archive_read_support_format_cab(struct archive * arc);
	int bmx_libarchive_archive_read_support_format_cpio(struct archive * arc);
	int bmx_libarchive_archive_read_support_format_empty(struct archive * arc);
	int bmx_libarchive_archive_read_support_format_gnutar(struct archive * arc);
	int bmx_libarchive_archive_read_support_format_iso9660(struct archive * arc);
	int bmx_libarchive_archive_read_support_format_lha(struct archive * arc);
	int bmx_libarchive_archive_read_support_format_mtree(struct archive * arc);
	int bmx_libarchive_archive_read_support_format_rar(struct archive * arc);
	int bmx_libarchive_archive_read_support_format_raw(struct archive * arc);
	int bmx_libarchive_archive_read_support_format_tar(struct archive * arc);
	int bmx_libarchive_archive_read_support_format_xar(struct archive * arc);
	int bmx_libarchive_archive_read_support_format_zip(struct archive * arc);

	int bmx_libarchive_archive_read_open_filename(struct archive * arc, BBString * filename, int blockSize);
	int bmx_libarchive_archive_read_open_memory(struct archive * arc, void * buf, int size);

	int bmx_libarchive_archive_read_next_header(struct archive * arc, struct archive_entry * entry);
	int bmx_libarchive_archive_read_data_skip(struct archive * arc);

	int bmx_libarchive_archive_read_free(struct archive * arc);


	struct archive * bmx_libarchive_write_archive_new();

	int bmx_libarchive_archive_write_free(struct archive * arc);


	void bmx_libarchive_archive_clear_error(struct archive * arc);
	int bmx_libarchive_archive_errno(struct archive * arc);
	BBString * bmx_libarchive_archive_error_string(struct archive * arc);
	int bmx_libarchive_archive_file_count(struct archive * arc);
	int bmx_libarchive_archive_filter_code(struct archive * arc, int filter);
	int bmx_libarchive_archive_filter_count(struct archive * arc);
	BBString * bmx_libarchive_archive_filter_name(struct archive * arc, int filter);
	int bmx_libarchive_archive_format(struct archive * arc);
	BBString * bmx_libarchive_archive_format_name(struct archive * arc);
	void bmx_libarchive_archive_position(struct archive * arc, int filter, BBInt64 * v);

	struct archive_entry * bmx_libarchive_archive_entry_new();
	void bmx_libarchive_archive_entry_free(struct archive_entry * entry);
	struct archive_entry * bmx_libarchive_archive_entry_clear(struct archive_entry * entry);
	struct archive_entry * bmx_libarchive_archive_entry_clone(struct archive_entry * entry);
	BBString * bmx_libarchive_archive_entry_pathname(struct archive_entry * entry);


}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++ */

struct archive * bmx_libarchive_read_archive_new() {
	return archive_read_new();
}


int bmx_libarchive_archive_read_support_filter_all(struct archive * arc) {
	return archive_read_support_filter_all(arc);
}

int bmx_libarchive_archive_read_support_filter_bzip2(struct archive * arc) {
	return archive_read_support_filter_bzip2(arc);
}

int bmx_libarchive_archive_read_support_filter_compress(struct archive * arc) {
	return archive_read_support_filter_compress(arc);
}

int bmx_libarchive_archive_read_support_filter_gzip(struct archive * arc) {
	return archive_read_support_filter_gzip(arc);
}

int bmx_libarchive_archive_read_support_filter_grzip(struct archive * arc) {
	return archive_read_support_filter_grzip(arc);
}

int bmx_libarchive_archive_read_support_filter_lrzip(struct archive * arc) {
	return archive_read_support_filter_lrzip(arc);
}

int bmx_libarchive_archive_read_support_filter_lzip(struct archive * arc) {
	return archive_read_support_filter_lzip(arc);
}

int bmx_libarchive_archive_read_support_filter_lzma(struct archive * arc) {
	return archive_read_support_filter_lzma(arc);
}

int bmx_libarchive_archive_read_support_filter_lzop(struct archive * arc) {
	return archive_read_support_filter_lzop(arc);
}

int bmx_libarchive_archive_read_support_filter_none(struct archive * arc) {
	return archive_read_support_filter_none(arc);
}

int bmx_libarchive_archive_read_support_filter_rpm(struct archive * arc) {
	return archive_read_support_filter_rpm(arc);
}

int bmx_libarchive_archive_read_support_filter_uu(struct archive * arc) {
	return archive_read_support_filter_uu(arc);
}

int bmx_libarchive_archive_read_support_filter_xz(struct archive * arc) {
	return archive_read_support_filter_xz(arc);
}

int bmx_libarchive_archive_read_support_format_all(struct archive * arc) {
	return archive_read_support_format_all(arc);
}

int bmx_libarchive_archive_read_support_format_7zip(struct archive * arc) {
	return archive_read_support_format_7zip(arc);
}

int bmx_libarchive_archive_read_support_format_ar(struct archive * arc) {
	return archive_read_support_format_ar(arc);
}

int bmx_libarchive_archive_read_support_format_by_code(struct archive * arc, int code) {
	return archive_read_support_format_by_code(arc, code);
}

int bmx_libarchive_archive_read_support_format_cab(struct archive * arc) {
	return archive_read_support_format_cab(arc);
}

int bmx_libarchive_archive_read_support_format_cpio(struct archive * arc) {
	return archive_read_support_format_cpio(arc);
}

int bmx_libarchive_archive_read_support_format_empty(struct archive * arc) {
	return archive_read_support_format_empty(arc);
}

int bmx_libarchive_archive_read_support_format_gnutar(struct archive * arc) {
	return archive_read_support_format_gnutar(arc);
}

int bmx_libarchive_archive_read_support_format_iso9660(struct archive * arc) {
	return archive_read_support_format_iso9660(arc);
}

int bmx_libarchive_archive_read_support_format_lha(struct archive * arc) {
	return archive_read_support_format_lha(arc);
}

int bmx_libarchive_archive_read_support_format_mtree(struct archive * arc) {
	return archive_read_support_format_mtree(arc);
}

int bmx_libarchive_archive_read_support_format_rar(struct archive * arc) {
	return archive_read_support_format_rar(arc);
}

int bmx_libarchive_archive_read_support_format_raw(struct archive * arc) {
	return archive_read_support_format_raw(arc);
}

int bmx_libarchive_archive_read_support_format_tar(struct archive * arc) {
	return archive_read_support_format_tar(arc);
}

int bmx_libarchive_archive_read_support_format_xar(struct archive * arc) {
	return archive_read_support_format_xar(arc);
}

int bmx_libarchive_archive_read_support_format_zip(struct archive * arc) {
	return archive_read_support_format_zip(arc);
}

int bmx_libarchive_archive_read_open_filename(struct archive * arc, BBString * filename, int blockSize) {
	char * v = bbStringToUTF8String(filename);
	
	int res = archive_read_open_filename(arc, v, blockSize);
	
	bbMemFree(v);
	
	return res;
}

int bmx_libarchive_archive_read_open_memory(struct archive * arc, void * buf, int size) {
	return archive_read_open_memory(arc, buf, size);
}

int bmx_libarchive_archive_read_next_header(struct archive * arc, struct archive_entry * entry) {
	return archive_read_next_header2(arc, entry);
}

int bmx_libarchive_archive_read_data_skip(struct archive * arc) {
	return archive_read_data_skip(arc);
}

int bmx_libarchive_archive_read_free(struct archive * arc) {
	return archive_read_free(arc);
}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++ */


struct archive * bmx_libarchive_write_archive_new() {
	return archive_write_new();
}

int bmx_libarchive_archive_write_free(struct archive * arc) {
	return archive_write_free(arc);
}


/* +++++++++++++++++++++++++++++++++++++++++++++++++++++ */

void bmx_libarchive_archive_clear_error(struct archive * arc) {
	archive_clear_error(arc);
}

int bmx_libarchive_archive_errno(struct archive * arc) {
	return archive_errno(arc);
}

BBString * bmx_libarchive_archive_error_string(struct archive * arc) {
	return bbStringFromCString(archive_error_string(arc));
}

int bmx_libarchive_archive_file_count(struct archive * arc) {
	return archive_file_count(arc);
}

int bmx_libarchive_archive_filter_code(struct archive * arc, int filter) {
	return archive_filter_code(arc, filter);
}

int bmx_libarchive_archive_filter_count(struct archive * arc) {
	return archive_filter_count(arc);
}

BBString * bmx_libarchive_archive_filter_name(struct archive * arc, int filter) {
	return bbStringFromCString(archive_filter_name(arc, filter));
}

int bmx_libarchive_archive_format(struct archive * arc) {
	return archive_format(arc);
}

BBString * bmx_libarchive_archive_format_name(struct archive * arc) {
	return bbStringFromCString(archive_format_name(arc));
}

void bmx_libarchive_archive_position(struct archive * arc, int filter, BBInt64 * v) {
	*v = archive_filter_bytes(arc, filter);
}


/* +++++++++++++++++++++++++++++++++++++++++++++++++++++ */

struct archive_entry * bmx_libarchive_archive_entry_new() {
	return archive_entry_new();
}

void bmx_libarchive_archive_entry_free(struct archive_entry * entry) {
	archive_entry_free(entry);
}

struct archive_entry * bmx_libarchive_archive_entry_clear(struct archive_entry * entry) {
	return archive_entry_clear(entry);
}

struct archive_entry * bmx_libarchive_archive_entry_clone(struct archive_entry * entry) {
	return archive_entry_clone(entry);
}

BBString * bmx_libarchive_archive_entry_pathname(struct archive_entry * entry) {
	return bbStringFromUTF8String(archive_entry_pathname(entry));
}
