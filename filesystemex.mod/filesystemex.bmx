' Copyright (c) 2011 Bruce A Henderson
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
bbdoc: Boost Filesystem
End Rem
Module BaH.FileSystemEX


ModuleInfo "CC_OPTS: -fexceptions"

Import "common.bmx"


'Type TFilePath
'End Type

Rem
bbdoc: Strip directory from a file path
End Rem
Function StripDir:String(path:String)
	Return bmx_filesystem_stripdir(path)
End Function

Rem
bbdoc: Strip extension from a file path
End Rem
Function StripExt:String(path:String)
	Return bmx_filesystem_extractdir(path) + "/" + bmx_filesystem_stripext(path)
End Function

Rem
bbdoc: Extract directory from a file path
End Rem
Function ExtractDir:String(path:String)
	Return bmx_filesystem_extractdir(path)
End Function

Rem
bbdoc: Extract extension from a file path
End Rem
Function ExtractExt:String(path:String)
	Local ext:String = bmx_filesystem_extractext(path)
	If ext.StartsWith(".") Then
		Return ext[1..]
	End If
	Return ext
End Function


Rem
bbdoc: Get Current Directory
returns: The current directory
End Rem
Function CurrentDir:String()
	Return bmx_filesystem_currentpath()
End Function

Rem
bbdoc: Get real, absolute path of a file path
End Rem
Function RealPath:String(path:String)
	Return bmx_filesystem_realpath(path)
End Function

Rem
bbdoc: Get file type
returns: 0 if file at @path doesn't exist, FILETYPE_FILE (1) if the file is a plain file or FILETYPE_DIR (2) if the file is a directory, or one of
FILETYPE_SYMLINK, FILETYPE_BLOCKFILE, FILETYPE_CHARACTERFILE, FILETYPE_FIFOFILE, FILETYPE_SOCKETFILE
End Rem
Function FileType:Int(path:String)
	Return bmx_filesystem_filetype(path)
End Function

Rem
bbdoc: Get file time
returns: The time the file at @path was last modified 
End Rem
Function FileTime:Int(path:String)
	Return bmx_filesystem_filetime(path)
End Function

Rem
bbdoc: Get file size
returns: Size, in bytes, of the file at @path, or -1 if the file does not exist
end rem
Function FileSize:Long(path:String)
	Local size:Long
	bmx_filesystem_filesize(path, Varptr size)
	Return size
End Function

Rem
bbdoc: Get file mode
returns: file mode flags
End Rem
Function FileMode:Int(path:String)
	'Return bmx_filesystem_filemode(path)
	path = RealPath(path)
	Local Mode:Int, size:Int, mtime:Int, ctime:Int
	If stat_( path,Mode,size,mtime,ctime ) Return -1
	Return Mode & 511
End Function

Rem
bbdoc: Set file mode
end rem
Function SetFileMode(path:String, Mode:Int)
	path = RealPath(path)
	chmod_ path,Mode
End Function

Rem
bbdoc: Create a file
returns: True if successful
End Rem
Function CreateFile:Int(path:String)
	path = RealPath(path)
	DeleteFile(path)
	Local t:Int = fopen_( path,"wb" )
	If t fclose_ t
	If FileType( path )=FILETYPE_FILE Return True
End Function

Rem
bbdoc: Create a directory
returns: True if successful
about:
If @recurse is true, any required subdirectories are also created.
End Rem
Function CreateDir:Int(path:String, recurse:Int = False)
	Return bmx_filesystem_createdir(path, recurse)
End Function

Rem
bbdoc: Delete a file
returns: True if successful
End Rem
Function DeleteFile:Int(path:String)
	Return bmx_filesystem_deletefile(path)
End Function

Rem
bbdoc: Renames a file
returns: True if successful
End Rem
Function RenameFile:Int(oldpath:String, newpath:String)
	Return bmx_filesystem_renamefile(oldpath, newpath)
End Function

Rem
bbdoc: Copy a file
returns: True if successful
End Rem
Function CopyFile:Int(src:String, dst:String)
	Return bmx_filesystem_copyfile(src, dst)
End Function

Rem
bbdoc: Copy a directory
returns: True if successful
End Rem
Function CopyDir:Int(src:String, dst:String)
	Return bmx_filesystem_copydir(src, dst)
End Function


Rem
bbdoc: Delete a directory
returns: True if successful
about: Set @recurse to true to delete all subdirectories and files recursively - 
but be careful!
End Rem
Function DeleteDir:Int(path:String, recurse:Int = False)
	Return bmx_filesystem_deletedir(path, recurse)
End Function

Rem
bbdoc: Change current directory
returns: True if successful
End Rem
Function ChangeDir:Int(path:String)
	Return bmx_filesystem_changedir(path)
End Function

Rem
bbdoc: Open a directory
returns: An integer directory handle, or 0 if the directory does not exist
End Rem
Function ReadDir:Int(path:String)
	Return bmx_filesystem_readdir(path)
End Function

Rem
bbdoc: Return next file in a directory
returns: File name of next file in directory opened using #ReadDir, or an empty string if there are no more files to read.
End Rem
Function NextFile:String(dir:Int)
	Return bmx_filesystem_nextfile(dir)
End Function

Rem
bbdoc: Close a directory
End Rem
Function CloseDir(dir:Int)
	bmx_filesystem_closedir(dir)
End Function

Rem
bbdoc: Load a directory
returns: A string array containing contents of @dir
about: The @skip_dots parameter, if true, removes the '.' (current) and '..'
(parent) directories from the returned array.
end rem
Function LoadDir:String[](dir:String, skip_dots:Int = True)
	If FileType(dir) <> FILETYPE_DIR Then
		Return Null
	End If
	Local d:String[] = bmx_filesystem_loaddir(dir)
	If Not skip_dots And d Then
		Return [".", ".."] + d
	Else
		Return d
	End If
End Function


Rem
bbdoc: Open a file for input and/or output.
about:
This command is similar to the #OpenStream command but will attempt
to cache the contents of the file to ensure serial streams such as 
http: based url's are seekable. Use the #CloseStream command when
finished reading and or writing to a Stream returned by #OpenFile.
End Rem
Function OpenFile:TStream( url:Object,readable:Int=True,writeable:Int=True )
	Local stream:TStream = OpenStream( url,readable,writeable )
	If Not stream Return Null
	If stream.Pos()=-1 Return TBankStream.Create( TBank.Load(stream) )
	Return stream
End Function

Rem
bbdoc: Open a file for input.
about:
This command is similar to the #ReadStream command but will attempt
to cache the contents of the file to ensure serial streams such as 
http: based url's are seekable. Use the #CloseStream command when
finished reading and or writing to a Stream returned by #OpenFile.
End Rem
Function ReadFile:TStream( url:Object )
	Return OpenFile( url,True,False )
End Function

Rem
bbdoc: Open a file for output.
about:
This command is identical to the #WriteStream command.
End Rem
Function WriteFile:TStream( url:Object )
	Return OpenFile( url,False,True )
End Function

Rem
bbdoc: Closes a file stream.
about:
After performing file operations on an open file make sure to
close the file stream with either #CloseFile or the identical
#CloseStream command.
End Rem
Function CloseFile( stream:TStream )
	stream.Close
End Function
