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

Rem
bbdoc: Streaming Archive Library
End Rem
Module BaH.LibArchive


ModuleInfo "Version: 1.00"
ModuleInfo "License: BSD"
ModuleInfo "Copyright: libarchive - 2003-2010 Tim Kientzle"
ModuleInfo "Copyright: Wrapper - 2013 Bruce A Henderson"


ModuleInfo "CC_OPTS: -DHAVE_CONFIG_H -D_FILE_OFFSET_BITS=64"
?win32
ModuleInfo "CC_OPTS: -DLIBARCHIVE_STATIC"
?macos
'ModuleInfo "CC_OPTS: -D_DARWIN_USE_64_BIT_INODE=1"
?

Import "common.bmx"



Rem
bbdoc: 
End Rem
Type TArchive

	Field archivePtr:Byte Ptr

	Rem
	bbdoc: Clears any error information left over from a previous call.
	about: Not generally used in client code.
	End Rem
	Method ClearError()
		bmx_libarchive_archive_clear_error(archivePtr)
	End Method
	
	Rem
	bbdoc: Returns a numeric error code indicating the reason for the most recent error return.
	about: Note that this can not be reliably used to detect whether an error has occurred.
	It should be used only after another libarchive function has returned an error status.
	End Rem
	Method Errno:Int()
		Return bmx_libarchive_archive_errno(archivePtr)
	End Method
	
	Rem
	bbdoc: Returns a textual error message suitable for display.
	about: The error message here is usually more specific than that obtained from passing the result of archive_errno() to strerror(3).
	End Rem
	Method ErrorString:String()
		Return bmx_libarchive_archive_error_string(archivePtr)
	End Method
	
	Rem
	bbdoc: Returns a count of the number of files processed by this archive object.
	about: The count is incremented by calls to WriteHeader or ReadNextHeader.
	End Rem
	Method FileCount:Int()
		Return bmx_libarchive_archive_file_count(archivePtr)
	End Method
	
	Rem
	bbdoc: Returns a numeric code identifying the indicated filter.
	about: See FilterCount() for details of the numbering.
	End Rem
	Method FilterCode:Int(filter:Int)
		Return bmx_libarchive_archive_filter_code(archivePtr, filter)
	End Method
	
	Rem
	bbdoc: Returns the number of filters in the current pipeline.
	about: For read archive handles, these filters are added automatically by the automatic format detection.
	For write archive handles, these filters are added by calls to the various AddFilterXXX() methods. Filters in the
	resulting pipeline are numbered so that filter 0 is the filter closest to the format handler. As a convenience,
	methods that expect a filter number will accept -1 as a synonym for the highest-numbered filter.
	<p>
	For example, when reading a uuencoded gzipped tar archive, there are three filters: filter 0 is the gunzip filter, filter 1 is the uudecode
	filter, and filter 2 is the pseudo-filter that wraps the archive read methods. In this case, requesting Position(-1) would be a synonym
	for Position(2) which would return the number of bytes currently read from the archive, while Position(1) would return the number of
	bytes after uudecoding, and Position(0) would return the number of bytes after decompression.
	</p>
	End Rem
	Method FilterCount:Int()
		Return bmx_libarchive_archive_filter_count(archivePtr)
	End Method
	
	Rem
	bbdoc: Returns a textual name identifying the indicated filter.
	about: See FilterCount() for details of the numbering.
	End Rem
	Method FilterName:String(filter:Int)
		Return bmx_libarchive_archive_filter_name(archivePtr, filter)
	End Method
	
	Rem
	bbdoc: Returns a numeric code indicating the format of the current archive entry.
	about: This value is set by a successful call to ReadNextHeader(). Note that it is common for this value to
	change from entry to entry. For example, a tar archive might have several entries that utilize GNU
	tar extensions and several entries that do not. These entries will have different format codes.
	End Rem
	Method Format:Int()
		Return bmx_libarchive_archive_format(archivePtr)
	End Method
	
	Rem
	bbdoc: A textual description of the format of the current entry.
	End Rem
	Method FormatName:String()
		Return bmx_libarchive_archive_format_name(archivePtr)
	End Method
	
	Rem
	bbdoc: Returns the number of bytes read from or written to the indicated filter.
	about: In particular, Position(0) returns the number of bytes read or written by the format handler,
	while Position(-1) returns the number of bytes read or written to the archive. See FilterCount() for details of the numbering here.
	End Rem
	Method Position:Long(filter:Int)
		Local v:Long
		bmx_libarchive_archive_position(archivePtr, filter, Varptr v)
		Return v
	End Method


	Method Free() Abstract
	
	Method Delete()
		Free()
	End Method
	
End Type

Rem
bbdoc: 
End Rem
Type TReadArchive Extends TArchive

	Field callbackData:TArchiveCallbackData

	Function CreateArchive:TReadArchive()
		Return New TReadArchive.Create()
	End Function
	
	Method Create:TReadArchive()
		archivePtr = bmx_libarchive_read_archive_new()
		Return Self
	End Method
	
	Rem
	bbdoc: Enables all available decompression filters.
	End Rem
	Method SupportFilterAll:Int()
		Return bmx_libarchive_archive_read_support_filter_all(archivePtr)
	End Method

	Rem
	bbdoc: Enables auto-detection code and decompression support for BZip2.
	about: May fall back on external programs if an appropriate library was not available at build time.
	Decompression using an external program is usually slower than decompression through built-in libraries.
	End Rem
	Method SupportFilterBzip2:Int()
		Return bmx_libarchive_archive_read_support_filter_bzip2(archivePtr)
	End Method

	Rem
	bbdoc: Enables auto-detection code and decompression support for Compress.
	about: May fall back on external programs if an appropriate library was not available at build time.
	Decompression using an external program is usually slower than decompression through built-in libraries.
	End Rem
	Method SupportFilterCompress:Int()
		Return bmx_libarchive_archive_read_support_filter_compress(archivePtr)
	End Method

	Rem
	bbdoc: Enables auto-detection code and decompression support for GZip.
	about: May fall back on external programs if an appropriate library was not available at build time.
	Decompression using an external program is usually slower than decompression through built-in libraries.
	End Rem
	Method SupportFilterGzip:Int()
		Return bmx_libarchive_archive_read_support_filter_gzip(archivePtr)
	End Method

	Rem
	bbdoc: Enables auto-detection code and decompression support for GRZip.
	about: May fall back on external programs if an appropriate library was not available at build time.
	Decompression using an external program is usually slower than decompression through built-in libraries.
	End Rem
	Method SupportFilterGrzip:Int()
		Return bmx_libarchive_archive_read_support_filter_grzip(archivePtr)
	End Method

	Rem
	bbdoc: Enables auto-detection code and decompression support for LRZip.
	about: May fall back on external programs if an appropriate library was not available at build time.
	Decompression using an external program is usually slower than decompression through built-in libraries.
	End Rem
	Method SupportFilterLrzip:Int()
		Return bmx_libarchive_archive_read_support_filter_lrzip(archivePtr)
	End Method

	Rem
	bbdoc: Enables auto-detection code and decompression support for LZip.
	about: May fall back on external programs if an appropriate library was not available at build time.
	Decompression using an external program is usually slower than decompression through built-in libraries.
	End Rem
	Method SupportFilterLzip:Int()
		Return bmx_libarchive_archive_read_support_filter_lzip(archivePtr)
	End Method

	Rem
	bbdoc: Enables auto-detection code and decompression support for LZMA.
	about: May fall back on external programs if an appropriate library was not available at build time.
	Decompression using an external program is usually slower than decompression through built-in libraries.
	End Rem
	Method SupportFilterLzma:Int()
		Return bmx_libarchive_archive_read_support_filter_lzma(archivePtr)
	End Method

	Rem
	bbdoc: Enables auto-detection code and decompression support for LZop.
	about: May fall back on external programs if an appropriate library was not available at build time.
	Decompression using an external program is usually slower than decompression through built-in libraries.
	End Rem
	Method SupportFilterLzop:Int()
		Return bmx_libarchive_archive_read_support_filter_lzop(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SupportFilterNone:Int()
		Return bmx_libarchive_archive_read_support_filter_none(archivePtr)
	End Method

	Rem
	bbdoc: Enables auto-detection code and decompression support for RPM.
	about: May fall back on external programs if an appropriate library was not available at build time.
	Decompression using an external program is usually slower than decompression through built-in libraries.
	End Rem
	Method SupportFilterRpm:Int()
		Return bmx_libarchive_archive_read_support_filter_rpm(archivePtr)
	End Method

	Rem
	bbdoc: Enables auto-detection code and decompression support for UUencode.
	about: May fall back on external programs if an appropriate library was not available at build time.
	Decompression using an external program is usually slower than decompression through built-in libraries.
	End Rem
	Method SupportFilterUu:Int()
		Return bmx_libarchive_archive_read_support_filter_uu(archivePtr)
	End Method

	Rem
	bbdoc: Enables auto-detection code and decompression support for Xz.
	about: May fall back on external programs if an appropriate library was not available at build time.
	Decompression using an external program is usually slower than decompression through built-in libraries.
	End Rem
	Method SupportFilterXz:Int()
		Return bmx_libarchive_archive_read_support_filter_xz(archivePtr)
	End Method


	Method SupportFormatAll:Int()
		Return bmx_libarchive_archive_read_support_format_all(archivePtr)
	End Method

	Method SupportFormat7zip:Int()
		Return bmx_libarchive_archive_read_support_format_7zip(archivePtr)
	End Method

	Method SupportFormatAr:Int()
		Return bmx_libarchive_archive_read_support_format_ar(archivePtr)
	End Method

	Method SupportFormatByCode:Int(code:Int)
		Return bmx_libarchive_archive_read_support_format_by_code(archivePtr, code)
	End Method

	Method SupportFormatCab:Int()
		Return bmx_libarchive_archive_read_support_format_cab(archivePtr)
	End Method

	Method SupportFormatCpio:Int()
		Return bmx_libarchive_archive_read_support_format_cpio(archivePtr)
	End Method

	Method SupportFormatEmpty:Int()
		Return bmx_libarchive_archive_read_support_format_empty(archivePtr)
	End Method

	Method SupportFormatGnutar:Int()
		Return bmx_libarchive_archive_read_support_format_gnutar(archivePtr)
	End Method

	Method SupportFormatIso9660:Int()
		Return bmx_libarchive_archive_read_support_format_iso9660(archivePtr)
	End Method

	Method SupportFormatLha:Int()
		Return bmx_libarchive_archive_read_support_format_lha(archivePtr)
	End Method

	Method SupportFormatMtree:Int()
		Return bmx_libarchive_archive_read_support_format_mtree(archivePtr)
	End Method

	Method SupportFormatRar:Int()
		Return bmx_libarchive_archive_read_support_format_rar(archivePtr)
	End Method

	Method SupportFormatRaw:Int()
		Return bmx_libarchive_archive_read_support_format_raw(archivePtr)
	End Method

	Method SupportFormatTar:Int()
		Return bmx_libarchive_archive_read_support_format_tar(archivePtr)
	End Method

	Method SupportFormatXar:Int()
		Return bmx_libarchive_archive_read_support_format_xar(archivePtr)
	End Method

	Method SupportFormatZip:Int()
		Return bmx_libarchive_archive_read_support_format_zip(archivePtr)
	End Method


	Rem
	bbdoc: 
	End Rem
	Method OpenFilename:Int(filename:String, blockSize:Int = 10240)
		Return bmx_libarchive_archive_read_open_filename(archivePtr, filename, blockSize)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method OpenMemory:Int(buf:Byte Ptr, size:Int)
		Return bmx_libarchive_archive_read_open_memory(archivePtr, buf, size)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method OpenStream:Int(stream:TSStream, blockSize:Int = 10240)
		If Not callbackData Then
			callbackData = TArchiveCallbackData.Create(stream, blockSize)
		Else
			callbackData.stream = stream
			callbackData.size = blockSize
		End If
		
		bmx_libarchive_archive_read_set_read_callback(archivePtr)
		bmx_libarchive_archive_read_set_seek_callback(archivePtr)
		bmx_libarchive_archive_read_set_callback_data(archivePtr, callbackData)
		
		Return bmx_libarchive_archive_read_open1(archivePtr)
	End Method
	
	Rem
	bbdoc: Read the header for the next entry and populate the provided entry
	returns: ARCHIVE_OK (the operation succeeded), ARCHIVE_WARN (the operation succeeded but a non-critical error was encountered), ARCHIVE_EOF (end-of-archive was encountered), ARCHIVE_RETRY (the operation failed but can be retried), and ARCHIVE_FATAL (there was a fatal error; the archive should be closed immediately).
	End Rem
	Method ReadNextHeader:Int(entry:TArchiveEntry)
		Return bmx_libarchive_archive_read_next_header(archivePtr, entry.entryPtr)
	End Method


	Method DataSkip:Int()
		Return bmx_libarchive_archive_read_data_skip(archivePtr)
	End Method
	
	Rem
	bbdoc: A convenience method that wraps the corresponding TWriteDiskArchive interfaces.
	about: The first call to Extract() creates a restore object using SetStandardLookup, then transparently invokes SetOptions, TWriteArchive.Header, TWriteArchive.Data, and TWriteArchive.FinishEntry to create the entry on disk and copy data into it.
	about: The flags argument is passed unmodified to SetOptions.
	End Rem
	Method Extract:Int(entry:TArchiveEntry, flags:Int)
		Return bmx_libarchive_archive_read_extract(archivePtr, entry.entryPtr, flags)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Data:Int(buf:Byte Ptr, size:Int)
		Return bmx_libarchive_archive_read_data(archivePtr, buf, size)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method DataStream:TSStream()
		Return TArchiveStream.Create(Self)
	End Method
	
	Method Free()
		If archivePtr Then
			bmx_libarchive_archive_read_free(archivePtr)
			archivePtr = Null
		End If
	End Method

End Type

Type TArchiveStream Extends TSStream

	Field archive:TReadArchive
	
	Field _eof:Int
	
	Function Create:TArchiveStream(archive:TReadArchive)
		Local this:TArchiveStream = New TArchiveStream
		this.archive = archive
		Return this
	End Function

	Method Read:Long( buf:Byte Ptr,count:Long )
		Local size:Long = archive.Data(buf, count)
		If Not size Then
			_eof = True
		End If
		Return size
	End Method

	Method ReadBytes:Long( buf:Byte Ptr,count:Long )
		Local t:Long=0
		While count>0
			Local n:Long=Read( buf,count )
			If Not n Exit
			count:-n
			buf:+n
			t:+ n
		Wend
		Return t
	End Method

	Method Eof:Int()
		Return _eof
	End Method

End Type

Type TArchiveCallbackData

	Field data:Byte Ptr
	Field stream:TSStream
	Field size:Int

	Function Create:TArchiveCallbackData(stream:TSStream, size:Int)
		Local this:TArchiveCallbackData = New TArchiveCallbackData
		this.data = MemAlloc(size)
		this.stream = stream
		this.size = size
		Return this
	End Function
	
	Function _read:Byte Ptr(cbData:Object, count:Int Var)
		Local data:TArchiveCallbackData = TArchiveCallbackData(cbData)
		
		Local buf:Byte Ptr = data.data
		count = data.size
		Local ncount:Long
		While count
			Local n:Long = data.stream.Read(buf, count)
			If Not n Then
				Exit
			End If
			count:-n
			ncount:+ n
			buf:+n
		Wend

		count = ncount
		Return data.data
	End Function
	
	
	Function _seek(cbData:Object, offset:Long, whence:Int, count:Long Var)
		Local data:TArchiveCallbackData = TArchiveCallbackData(cbData)

		Select whence
			Case SEEK_SET_
				count = data.stream.seek(offset)
			Case SEEK_CUR_
				count = data.stream.seek(offset + data.stream.pos())
			Case SEEK_END_
				count = data.stream.seek(data.stream.size())
		End Select
	End Function
	

	Method Delete()
		MemFree(data)
		data = Null
	End Method
End Type

Rem
bbdoc: 
End Rem
Type TWriteArchive Extends TArchive

	Function CreateArchive:TWriteArchive()
		Return New TWriteArchive.Create()
	End Function
	
	Method Create:TWriteArchive()
		archivePtr = bmx_libarchive_write_archive_new()
		Return Self
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilter:Int(filterCode:Int)
		Return bmx_libarchive_archive_write_add_filter(archivePtr, filterCode)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilterByName:Int(name:String)
		Return bmx_libarchive_archive_write_add_filter_by_name(archivePtr, name)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilterB64encode:Int()
		Return bmx_libarchive_archive_write_add_filter_b64encode(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilterBzip2:Int()
		Return bmx_libarchive_archive_write_add_filter_bzip2(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilterCompress:Int()
		Return bmx_libarchive_archive_write_add_filter_compress(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilterGrzip:Int()
		Return bmx_libarchive_archive_write_add_filter_grzip(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilterGzip:Int()
		Return bmx_libarchive_archive_write_add_filter_gzip(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilterLrzip:Int()
		Return bmx_libarchive_archive_write_add_filter_lrzip(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilterLzip:Int()
		Return bmx_libarchive_archive_write_add_filter_lzip(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilterLzma:Int()
		Return bmx_libarchive_archive_write_add_filter_lzma(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilterLzop:Int()
		Return bmx_libarchive_archive_write_add_filter_lzop(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilterNone:Int()
		Return bmx_libarchive_archive_write_add_filter_none(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilterUuencode:Int()
		Return bmx_libarchive_archive_write_add_filter_uuencode(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method AddFilterXz:Int()
		Return bmx_libarchive_archive_write_add_filter_xz(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormat:Int(formatCode:Int)
		Return bmx_libarchive_archive_write_set_format(archivePtr, formatCode)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatByName:Int(name:String)
		Return bmx_libarchive_archive_write_set_format_by_name(archivePtr, name)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormat7zip:Int()
		Return bmx_libarchive_archive_write_set_format_7zip(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatArBsd:Int()
		Return bmx_libarchive_archive_write_set_format_ar_bsd(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormat_ArSvr4:Int()
		Return bmx_libarchive_archive_write_set_format_ar_svr4(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatCpio:Int()
		Return bmx_libarchive_archive_write_set_format_cpio(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatCpioNewc:Int()
		Return bmx_libarchive_archive_write_set_format_cpio_newc(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatGnutar:Int()
		Return bmx_libarchive_archive_write_set_format_gnutar(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatIso9660:Int()
		Return bmx_libarchive_archive_write_set_format_iso9660(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatMtree:Int()
		Return bmx_libarchive_archive_write_set_format_mtree(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatMtreeClassic:Int()
		Return bmx_libarchive_archive_write_set_format_mtree_classic(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatPax:Int()
		Return bmx_libarchive_archive_write_set_format_pax(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatPaxRestricted:Int()
		Return bmx_libarchive_archive_write_set_format_pax_restricted(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatShar:Int()
		Return bmx_libarchive_archive_write_set_format_shar(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatSharDump:Int()
		Return bmx_libarchive_archive_write_set_format_shar_dump(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatUstar:Int()
		Return bmx_libarchive_archive_write_set_format_ustar(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatV7tar:Int()
		Return bmx_libarchive_archive_write_set_format_v7tar(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatXar:Int()
		Return bmx_libarchive_archive_write_set_format_xar(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetFormatZip:Int()
		Return bmx_libarchive_archive_write_set_format_zip(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetZipCompressionDeflate:Int()
		Return bmx_libarchive_archive_write_zip_set_compression_deflate(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetZipCompressionStore:Int()
		Return bmx_libarchive_archive_write_zip_set_compression_store(archivePtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method OpenFilename:Int(filename:String)
		Return bmx_libarchive_archive_write_open_filename(archivePtr, filename)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method OpenMemory:Int(buf:Byte Ptr, size:Int, used:Int Var)
		Return bmx_libarchive_archive_write_open_memory(archivePtr, buf, size, Varptr used)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method OpenStream:Int(stream:TSStream, blockSize:Int = 10240)
	End Method
	
	Rem
	bbdoc: Build and write a header using the data in the provided in the TArchiveEntry object.
	about: See TArchiveEntry for information on creating and populating such objects.
	End Rem
	Method WriteHeader:Int(entry:TArchiveEntry)
		Return bmx_libarchive_archive_write_header(archivePtr, entry.entryPtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method Close:Int()
		Return bmx_libarchive_archive_write_close(archivePtr)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method WriteData:Int(data:Byte Ptr, size:Int)
		Return bmx_libarchive_archive_write_data(archivePtr, data, size)
	End Method

	Method Free()
		If archivePtr Then
			bmx_libarchive_archive_write_free(archivePtr)
			archivePtr = Null
		End If
	End Method

End Type

Rem
bbdoc: 
End Rem
Type TReadDiskArchive Extends TArchive

	Method Create:TReadDiskArchive()
		archivePtr = bmx_libarchive_archive_read_disk_new()
		Return Self
	End Method
	
	Method EntryFromFile:Int(entry:TArchiveEntry)
		Return bmx_libarchive_archive_read_disk_entry_from_file(archivePtr, entry.entryPtr)
	End Method

	Method SetSymlinkLogical:Int()
	End Method

	Method SetSymlinkPhysical:Int()
	End Method

	Method SetSymlinkhybrid:Int()
	End Method
	
	Method GName:String(gid:Long)
	End Method
	
	Method UName:String(uid:Long)
	End Method
	
	Method SetStandardLookup:Int()
		Return bmx_libarchive_archive_read_disk_set_standard_lookup(archivePtr)
	End Method
	
	Method Free()
		If archivePtr Then
			bmx_libarchive_archive_read_free(archivePtr)
			archivePtr = Null
		End If
	End Method

End Type

Rem
bbdoc: Represents entries within an archive.
about: You can think of a TArchiveEntry as a heavy-duty version of the c struct stat: it includes everything from struct stat plus
associated pathname, textual group and user names, etc. These objects are used by libarchive to represent the metadata associated
with a particular entry in an archive.
End Rem
Type TArchiveEntry

	Field entryPtr:Byte Ptr
	
	Function _create:TArchiveEntry(entryPtr:Byte Ptr)
		If entryPtr Then
			Local this:TArchiveEntry = New TArchiveEntry
			this.entryPtr = entryPtr
			Return this
		End If
		Return Null
	End Function

	Function CreateEntry:TArchiveEntry()
		Return New TArchiveEntry.Create()
	End Function
	
	Method Create:TArchiveEntry()
		entryPtr = bmx_libarchive_archive_entry_new()
		Return Self
	End Method
	
	Rem
	bbdoc: Erases the object, resetting all internal fields to the same state as a newly-created object.
	about: This is provided to allow you to quickly recycle objects.
	End Rem
	Method Clear()
		entryPtr = bmx_libarchive_archive_entry_clear(entryPtr)
	End Method
	
	Rem
	bbdoc: A deep copy operation; all text fields are duplicated.
	End Rem
	Method Clone:TArchiveEntry()
		Return TArchiveEntry._create(bmx_libarchive_archive_entry_clone(entryPtr))
	End Method

	Rem
	bbdoc: Destination of the hardlink.
	End Rem
	Method Hardlink:String()
	End Method

	Rem
	bbdoc: Path in the archive.
	End Rem
	Method Pathname:String()
		Return bmx_libarchive_archive_entry_pathname(entryPtr)
	End Method
	
	Rem
	bbdoc: Path on the disk for use by TReadArchive.Disk()
	End Rem
	Method SourcePath:String()
	End Method
	
	Rem
	bbdoc: Destination of the symbolic link.
	End Rem
	Method Symlink:String()
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method SeHardlink(path:String)
	End Method
	
	Rem
	bbdoc: For a symlink, update the destination, otherwise, make the entry a hardlink and alter the destination for that.
	about: Update only. 
	End Rem
	Method SetLink(path:String)
		bmx_libarchive_archive_entry_set_link(entryPtr, path)
	End Method
	
	Method SetPathname(path:String)
		bmx_libarchive_archive_entry_set_pathname(entryPtr, path)
	End Method
	
	Method SetSourcePath(path:String)
		bmx_libarchive_archive_entry_set_sourcepath(entryPtr, path)
	End Method
	
	Method SetSymlink(path:String)
		bmx_libarchive_archive_entry_set_symlink(entryPtr, path)
	End Method

	Rem
	bbdoc: Releases the archive entry object.
	End Rem
	Method Free()
		If entryPtr Then
			bmx_libarchive_archive_entry_free(entryPtr)
			entryPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type


