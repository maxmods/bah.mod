' Copyright (c) 2013 Bruce A Henderson
' All rights reserved.
'
' Redistribution and use in source and binary forms, with or without
' modification, are permitted provided that the following conditions are met:
'     * Redistributions of source code must retain the above copyright
'       notice, this list of conditions and the following disclaimer.
'     * Redistributions in binary form must reproduce the above copyright
'       notice, this list of conditions and the following disclaimer in the
'       documentation and/or other materials provided with the distribution.
'     * Neither the name of Bruce A Henderson nor the
'       names of its contributors may be used to endorse or promote products
'       derived from this software without specific prior written permission.
'
' THIS SOFTWARE IS PROVIDED BY Bruce A Henderson ``AS IS'' AND ANY
' EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
' WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
' DISCLAIMED. IN NO EVENT SHALL Bruce A Henderson BE LIABLE FOR ANY
' DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
' (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
' LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
' ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
' (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
' SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'
SuperStrict

Import BRL.Blitz

Import "../libxml.mod/src/*.h"
Import "../xz.mod/src/liblzma/api/*.h"
?win32
Import "../libiconv.mod/include/*.h"
?

Import "bzip2/*.h"
Import "libarchive/*.h"
Import "zlib/*.h"

Import "libarchive/archive_acl.c"
Import "libarchive/archive_check_magic.c"
Import "libarchive/archive_cmdline.c"
Import "libarchive/archive_crypto.c"
Import "libarchive/archive_entry_copy_stat.c"
Import "libarchive/archive_entry_link_resolver.c"
Import "libarchive/archive_entry_sparse.c"
Import "libarchive/archive_entry_stat.c"
Import "libarchive/archive_entry_strmode.c"
Import "libarchive/archive_entry_xattr.c"
Import "libarchive/archive_entry.c"
Import "libarchive/archive_getdate.c"
Import "libarchive/archive_match.c"
Import "libarchive/archive_options.c"
Import "libarchive/archive_pathmatch.c"
Import "libarchive/archive_ppmd7.c"
Import "libarchive/archive_rb.c"
Import "libarchive/archive_read_append_filter.c"
Import "libarchive/archive_read_data_into_fd.c"
Import "libarchive/archive_read_disk_entry_from_file.c"
Import "libarchive/archive_read_disk_posix.c"
Import "libarchive/archive_read_disk_set_standard_lookup.c"
Import "libarchive/archive_read_extract.c"
Import "libarchive/archive_read_open_fd.c"
Import "libarchive/archive_read_open_file.c"
Import "libarchive/archive_read_open_filename.c"
Import "libarchive/archive_read_open_memory.c"
Import "libarchive/archive_read_set_format.c"
Import "libarchive/archive_read_set_options.c"
Import "libarchive/archive_read_support_filter_all.c"
Import "libarchive/archive_read_support_filter_bzip2.c"
Import "libarchive/archive_read_support_filter_compress.c"
Import "libarchive/archive_read_support_filter_grzip.c"
Import "libarchive/archive_read_support_filter_gzip.c"
Import "libarchive/archive_read_support_filter_lrzip.c"
Import "libarchive/archive_read_support_filter_lzop.c"
Import "libarchive/archive_read_support_filter_none.c"
Import "libarchive/archive_read_support_filter_program.c"
Import "libarchive/archive_read_support_filter_rpm.c"
Import "libarchive/archive_read_support_filter_uu.c"
Import "libarchive/archive_read_support_filter_xz.c"
Import "libarchive/archive_read_support_format_7zip.c"
Import "libarchive/archive_read_support_format_all.c"
Import "libarchive/archive_read_support_format_ar.c"
Import "libarchive/archive_read_support_format_by_code.c"
Import "libarchive/archive_read_support_format_cab.c"
Import "libarchive/archive_read_support_format_cpio.c"
Import "libarchive/archive_read_support_format_empty.c"
Import "libarchive/archive_read_support_format_iso9660.c"
Import "libarchive/archive_read_support_format_lha.c"
Import "libarchive/archive_read_support_format_mtree.c"
Import "libarchive/archive_read_support_format_rar.c"
Import "libarchive/archive_read_support_format_raw.c"
Import "libarchive/archive_read_support_format_tar.c"
Import "libarchive/archive_read_support_format_xar.c"
Import "libarchive/archive_read_support_format_zip.c"
Import "libarchive/archive_read.c"
Import "libarchive/archive_string_sprintf.c"
Import "libarchive/archive_string.c"
Import "libarchive/archive_util.c"
Import "libarchive/archive_virtual.c"
Import "libarchive/archive_write_add_filter_b64encode.c"
Import "libarchive/archive_write_add_filter_by_name.c"
Import "libarchive/archive_write_add_filter_bzip2.c"
Import "libarchive/archive_write_add_filter_compress.c"
Import "libarchive/archive_write_add_filter_grzip.c"
Import "libarchive/archive_write_add_filter_gzip.c"
Import "libarchive/archive_write_add_filter_lrzip.c"
Import "libarchive/archive_write_add_filter_lzop.c"
Import "libarchive/archive_write_add_filter_none.c"
Import "libarchive/archive_write_add_filter_program.c"
Import "libarchive/archive_write_add_filter_uuencode.c"
Import "libarchive/archive_write_add_filter_xz.c"
Import "libarchive/archive_write_add_filter.c"
Import "libarchive/archive_write_disk_acl.c"
Import "libarchive/archive_write_disk_posix.c"
Import "libarchive/archive_write_disk_set_standard_lookup.c"
Import "libarchive/archive_write_open_fd.c"
Import "libarchive/archive_write_open_file.c"
Import "libarchive/archive_write_open_filename.c"
Import "libarchive/archive_write_open_memory.c"
Import "libarchive/archive_write_set_format_7zip.c"
Import "libarchive/archive_write_set_format_ar.c"
Import "libarchive/archive_write_set_format_by_name.c"
Import "libarchive/archive_write_set_format_cpio_newc.c"
Import "libarchive/archive_write_set_format_cpio.c"
Import "libarchive/archive_write_set_format_gnutar.c"
Import "libarchive/archive_write_set_format_iso9660.c"
Import "libarchive/archive_write_set_format_mtree.c"
Import "libarchive/archive_write_set_format_pax.c"
Import "libarchive/archive_write_set_format_shar.c"
Import "libarchive/archive_write_set_format_ustar.c"
Import "libarchive/archive_write_set_format_v7tar.c"
Import "libarchive/archive_write_set_format_xar.c"
Import "libarchive/archive_write_set_format_zip.c"
Import "libarchive/archive_write_set_format.c"
Import "libarchive/archive_write_set_options.c"
Import "libarchive/archive_write.c"
?Not win32
Import "libarchive/filter_fork_posix.c"
?
?win32
Import "libarchive/archive_entry_copy_bhfi.c"
Import "libarchive/archive_read_disk_windows.c"
Import "libarchive/archive_windows.c"
Import "libarchive/archive_write_disk_windows.c"
Import "libarchive/filter_fork_windows.c"
?

' BZip2
Import "bzip2/blocksort.c"
Import "bzip2/huffman.c"
Import "bzip2/crctable.c"
Import "bzip2/randtable.c"
Import "bzip2/compress.c"
Import "bzip2/decompress.c"
Import "bzip2/bzlib.c"

' zlib
Import "zlib/adler32.c"
Import "zlib/crc32.c"
Import "zlib/deflate.c"
Import "zlib/infback.c"
Import "zlib/inffast.c"
Import "zlib/inflate.c"
Import "zlib/inftrees.c"
Import "zlib/trees.c"
Import "zlib/zutil.c"
Import "zlib/zlib_compress.c"
Import "zlib/uncompr.c"
Import "zlib/gzclose.c"
Import "zlib/gzlib.c"
Import "zlib/gzread.c"
Import "zlib/gzwrite.c"

Import "glue.cpp"

