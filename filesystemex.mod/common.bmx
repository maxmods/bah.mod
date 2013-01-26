' Copyright (c) 2011-2013 Bruce A Henderson
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

Import BaH.Boost
Import BRL.Blitz
Import Pub.StdC
Import BRL.BankStream

Import "../boost.mod/src/*.hpp"

Import "../boost.mod/src/libs/filesystem/src/*.hpp"

Import "../boost.mod/src/libs/filesystem/src/codecvt_error_category.cpp"
Import "../boost.mod/src/libs/filesystem/src/operations.cpp"
Import "../boost.mod/src/libs/filesystem/src/path_traits.cpp"
Import "../boost.mod/src/libs/filesystem/src/path.cpp"
Import "../boost.mod/src/libs/filesystem/src/portability.cpp"
Import "../boost.mod/src/libs/filesystem/src/unique_path.cpp"
Import "../boost.mod/src/libs/filesystem/src/windows_file_codecvt.cpp"
Import "../boost.mod/src/libs/filesystem/src/utf8_codecvt_facet.cpp"

Import "glue.cpp"

Extern

	Function bmx_filesystem_currentpath:String()
	Function bmx_filesystem_stripdir:String(path:String)
	Function bmx_filesystem_stripext:String(path:String)
	Function bmx_filesystem_extractdir:String(path:String)
	Function bmx_filesystem_extractext:String(path:String)
	Function bmx_filesystem_realpath:String(path:String)
	Function bmx_filesystem_filesize(path:String, size:Long Ptr)
	Function bmx_filesystem_filetype:Int(path:String)
	Function bmx_filesystem_filemode:Int(path:String)
	Function bmx_filesystem_createdir:Int(path:String, recurse:Int)
	Function bmx_filesystem_deletefile:Int(path:String)
	Function bmx_filesystem_renamefile:Int(oldpath:String, newpath:String)
	Function bmx_filesystem_copyfile:Int(src:String, dst:String)
	Function bmx_filesystem_copydir:Int(src:String, dst:String)
	Function bmx_filesystem_deletedir:Int(path:String, recurse:Int)
	Function bmx_filesystem_changedir:Int(path:String)
	Function bmx_filesystem_filetime:Int(path:String)
	Function bmx_filesystem_loaddir:String[](dir:String)
	Function bmx_filesystem_readdir:Int(path:String)
	Function bmx_filesystem_nextfile:String(dir:Int)
	Function bmx_filesystem_closedir(dir:Int)
	Function bmx_filesystem_uniquepath:String()
	Function bmx_filesystem_tempdirectorypath:String()

End Extern

Const FILETYPE_NONE:Int = 0
Const FILETYPE_FILE:Int = 1
Const FILETYPE_DIR:Int = 2
Const FILETYPE_SYMLINK:Int = 3
Const FILETYPE_BLOCKFILE:Int = 4
Const FILETYPE_CHARACTERFILE:Int = 5
Const FILETYPE_FIFOFILE:Int = 6
Const FILETYPE_SOCKETFILE:Int = 7
