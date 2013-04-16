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


Import BaH.LibXML
Import BaH.Libiconv
Import BaH.xz

Import "source.bmx"


Extern

	Function bmx_libarchive_read_archive_new:Byte Ptr()

	Function bmx_libarchive_archive_read_support_filter_all:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_filter_bzip2:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_filter_compress:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_filter_gzip:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_filter_grzip:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_filter_lrzip:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_filter_lzip:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_filter_lzma:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_filter_lzop:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_filter_none:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_filter_rpm:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_filter_uu:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_filter_xz:Int(handle:Byte Ptr)

	Function bmx_libarchive_archive_read_support_format_all:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_format_7zip:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_format_ar:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_format_by_code:Int(handle:Byte Ptr, code:Int)
	Function bmx_libarchive_archive_read_support_format_cab:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_format_cpio:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_format_empty:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_format_gnutar:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_format_iso9660:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_format_lha:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_format_mtree:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_format_rar:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_format_raw:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_format_tar:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_format_xar:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_read_support_format_zip:Int(handle:Byte Ptr)

	Function bmx_libarchive_archive_read_open_filename:Int(handle:Byte Ptr, filename:String, blockSize:Int)
	Function bmx_libarchive_archive_read_open_memory:Int(handle:Byte Ptr, buf:Byte Ptr, size:Int)

	Function bmx_libarchive_archive_read_next_header:Int(handle:Byte Ptr, entry:Byte Ptr)
	Function bmx_libarchive_archive_read_data_skip:Int(handle:Byte Ptr)

	Function bmx_libarchive_archive_read_free:Int(handle:Byte Ptr)

	Function bmx_libarchive_write_archive_new:Byte Ptr()
	Function bmx_libarchive_archive_write_free:Int(handle:Byte Ptr)


	Function bmx_libarchive_archive_clear_error(handle:Byte Ptr)
	Function bmx_libarchive_archive_errno:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_error_string:String(handle:Byte Ptr)
	Function bmx_libarchive_archive_file_count:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_filter_code:Int(handle:Byte Ptr, filter:Int)
	Function bmx_libarchive_archive_filter_count:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_filter_name:String(handle:Byte Ptr, filter:Int)
	Function bmx_libarchive_archive_format:Int(handle:Byte Ptr)
	Function bmx_libarchive_archive_format_name:String(handle:Byte Ptr)
	Function bmx_libarchive_archive_position(handle:Byte Ptr, filter:Int, v:Long Ptr)


	Function bmx_libarchive_archive_entry_new:Byte Ptr()
	Function bmx_libarchive_archive_entry_free(handle:Byte Ptr)
	Function bmx_libarchive_archive_entry_clear:Byte Ptr(handle:Byte Ptr)
	Function bmx_libarchive_archive_entry_clone:Byte Ptr(handle:Byte Ptr)
	
	Function bmx_libarchive_archive_entry_pathname:String(handle:Byte Ptr)

End Extern



Const ARCHIVE_FILTER_NONE:Int = 0
Const ARCHIVE_FILTER_GZIP:Int = 1
Const ARCHIVE_FILTER_BZIP2:Int = 2
Const ARCHIVE_FILTER_COMPRESS:Int = 3
Const ARCHIVE_FILTER_PROGRAM:Int = 4
Const ARCHIVE_FILTER_LZMA:Int = 5
Const ARCHIVE_FILTER_XZ:Int = 6
Const ARCHIVE_FILTER_UU:Int = 7
Const ARCHIVE_FILTER_RPM:Int = 8
Const ARCHIVE_FILTER_LZIP:Int = 9
Const ARCHIVE_FILTER_LRZIP:Int = 10
Const ARCHIVE_FILTER_LZOP:Int = 11
Const ARCHIVE_FILTER_GRZIP:Int = 12

Const ARCHIVE_FORMAT_BASE_MASK:Int = $ff0000
Const ARCHIVE_FORMAT_CPIO:Int = $10000
Const ARCHIVE_FORMAT_CPIO_POSIX:Int = ARCHIVE_FORMAT_CPIO | 1
Const ARCHIVE_FORMAT_CPIO_BIN_LE:Int = ARCHIVE_FORMAT_CPIO | 2
Const ARCHIVE_FORMAT_CPIO_BIN_BE:Int = ARCHIVE_FORMAT_CPIO | 3
Const ARCHIVE_FORMAT_CPIO_SVR4_NOCRC:Int = ARCHIVE_FORMAT_CPIO | 4
Const ARCHIVE_FORMAT_CPIO_SVR4_CRC:Int = ARCHIVE_FORMAT_CPIO | 5
Const ARCHIVE_FORMAT_CPIO_AFIO_LARGE:Int = ARCHIVE_FORMAT_CPIO | 6
Const ARCHIVE_FORMAT_SHAR:Int = $20000
Const ARCHIVE_FORMAT_SHAR_BASE:Int = ARCHIVE_FORMAT_SHAR | 1
Const ARCHIVE_FORMAT_SHAR_DUMP:Int = ARCHIVE_FORMAT_SHAR | 2
Const ARCHIVE_FORMAT_TAR:Int = $30000
Const ARCHIVE_FORMAT_TAR_USTAR:Int = ARCHIVE_FORMAT_TAR | 1
Const ARCHIVE_FORMAT_TAR_PAX_INTERCHANGE:Int = ARCHIVE_FORMAT_TAR | 2
Const ARCHIVE_FORMAT_TAR_PAX_RESTRICTED:Int = ARCHIVE_FORMAT_TAR | 3
Const ARCHIVE_FORMAT_TAR_GNUTAR:Int = ARCHIVE_FORMAT_TAR | 4
Const ARCHIVE_FORMAT_ISO9660:Int = $40000
Const ARCHIVE_FORMAT_ISO9660_ROCKRIDGE:Int = ARCHIVE_FORMAT_ISO9660 | 1
Const ARCHIVE_FORMAT_ZIP:Int = $50000
Const ARCHIVE_FORMAT_EMPTY:Int = $60000
Const ARCHIVE_FORMAT_AR:Int = $70000
Const ARCHIVE_FORMAT_AR_GNU:Int = ARCHIVE_FORMAT_AR | 1
Const ARCHIVE_FORMAT_AR_BSD:Int = ARCHIVE_FORMAT_AR | 2
Const ARCHIVE_FORMAT_MTREE:Int = $80000
Const ARCHIVE_FORMAT_RAW:Int = $90000
Const ARCHIVE_FORMAT_XAR:Int = $A0000
Const ARCHIVE_FORMAT_LHA:Int = $B0000
Const ARCHIVE_FORMAT_CAB:Int = $C0000
Const ARCHIVE_FORMAT_RAR:Int = $D0000
Const ARCHIVE_FORMAT_7ZIP:Int = $E0000

Rem
bbdoc:  Found end of archive.
End Rem
Const ARCHIVE_EOF:Int = 1
Rem
bbdoc:  Operation was successful.
End Rem
Const ARCHIVE_OK:Int = 0
Rem
bbdoc:  Retry might succeed.
End Rem
Const ARCHIVE_RETRY:Int = -10
Rem
bbdoc:  Partial success.
End Rem
Const ARCHIVE_WARN:Int = -20
Rem
bbdoc:  Current operation cannot complete.
about: For example, if WriteHeader "fails", tThen you can't push data.
End Rem
Const ARCHIVE_FAILED:Int = -25
Rem
bbdoc:  No more operations are possible.
about: For example, if WriteHeader is "fatal," then this archive is dead and useless.
End Rem
Const ARCHIVE_FATAL:Int = -30



