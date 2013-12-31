' Copyright (c) 2013 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
' 
SuperStrict

Rem
bbdoc: Linux Samba Client
End Rem
Module BaH.LibSMBClient

ModuleInfo "Version: 1.00"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2013 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release"

?linux

Import "common.bmx"


Rem
bbdoc: An authorization object.
about: This is used as part of the authorization callback process. Workgroup, username and password fields should
be populated as required.
End Rem
Type TSMBCAuth

	Field context:TSMBC

	Field server:String
	Field share:String
	
	Field workgroup:String
	Field username:String
	Field password:String

	' private access functions
	Function _new:TSMBCAuth(context:TSMBC, server:String, share:String)
		Local auth:TSMBCAuth = New TSMBCAuth
		auth.context = context
		auth.server = server
		auth.share = share
		Return auth
	End Function
	
	Function _workgroup:String(auth:TSMBCAuth)
		Return auth.workgroup
	End Function

	Function _username:String(auth:TSMBCAuth)
		Return auth.username
	End Function

	Function _password:String(auth:TSMBCAuth)
		Return auth.password
	End Function
	
End Type

Rem
bbdoc: A directory handle.
about: For use with ReadDir, NextFile and CloseDir methods of TSMBC.
End Rem
Type TSMBCDirHandle

	Field fdPtr:Byte Ptr
	
	Function _new:TSMBCDirHandle(fdPtr:Byte Ptr)
		If fdPtr Then
			Local this:TSMBCDirHandle = New TSMBCDirHandle
			this.fdPtr = fdPtr
			Return this
		End If
	End Function

End Type

Rem
bbdoc: A directory entity object.
about: This can represent one of several different entity types : SMBC_WORKGROUP,
	SMBC_SERVER, SMBC_FILE_SHARE, SMBC_PRINTER_SHARE, SMBC_COMMS_SHARE,
	SMBC_IPC_SHARE, SMBC_DIR, SMBC_FILE, or SMBC_LINK.
End Rem
Type TSMBCDirent

	Field name:String
	Field ftype:Int
	
	Function _new:TSMBCDirent(name:String, ftype:Int)
		Local this:TSMBCDirent = New TSMBCDirent
		this.name = name
		this.ftype = ftype
		Return this
	End Function
	
	Method ToString:String()
		Return name
	End Method
	
End Type

Rem
bbdoc: An Samba client context.
about: Subclass this to implement your authorization callback.
End Rem
Type TSMBC

	Const FILETYPE_NONE:Int = 0
	Const FILETYPE_FILE:Int = 1
	Const FILETYPE_DIR:Int = 2

	Field contextPtr:Byte Ptr

	Rem
	bbdoc: Creates a new instance of a Samba client context.
	End Rem
	Method Create:TSMBC(smbDebug:Int = 0)
	
		contextPtr = bmx_smbc_new_context(Self, smbDebug)
		
		If contextPtr Then
			TSMBCStreamFactory.AddContext(Self)
			Return Self
		End If
		
		Return Null
	End Method
	
	Function _authDataCallback(auth:TSMBCAuth)
	
		Local smbc:TSMBC = auth.context
		
		If smbc Then
			smbc.GetAuthorization(auth)
		End If		
		
	End Function

	Rem
	bbdoc: Populates the TSMBCAuth object with valid workgroup, username and password details.
	about: Implement as required. 
	End Rem	
	Method GetAuthorization(auth:TSMBCAuth) Abstract


	Rem
	bbdoc: Opens a directory
	returns: A TSMBCDirHandle directory handle, or 0 if the directory does not exist
	End Rem
	Method ReadDir:TSMBCDirHandle(path:String)
		Return TSMBCDirHandle(bmx_smbc_opendir(contextPtr, path))
	End Method
	
	Rem
	bbdoc: Returns next file in a directory
	returns: The next file in directory opened using #ReadDir, or an empty object if there are no more files to read.
	End Rem
	Method NextFile:TSMBCDirent(dir:TSMBCDirHandle)
		Return TSMBCDirent(bmx_smbc_readdir(contextPtr, dir.fdPtr))
	End Method
	
	Rem
	bbdoc: Closes a directory
	End Rem
	Method CloseDir(dir:TSMBCDirHandle)
		bmx_smbc_closedir(contextPtr, dir.fdPtr)
	End Method

	Rem
	bbdoc: Loads a directory
	returns: An array of directory entity contents of @dir
	about: The @skipDots parameter, if true, removes the '.' (current) and '..'
	(parent) directories from the returned array.
	End Rem 
	Method LoadDir:TSMBCDirent[](path:String, skipDots:Int = True)
		Local dir:TSMBCDirHandle = ReadDir(path)
		If Not dir Then
			Return Null
		End If
			
		Local entries:TSMBCDirent[100]
		Local n:Int
		
		Repeat
			Local f:TSMBCDirent = NextFile(dir)
			If Not f Exit
			If f.name = "" Then
				Continue
			End If
			If skipDots And (f.name = "." Or f.name = "..") Then
				Continue
			End If
			If n = entries.length Then
				entries = entries[..n+100]
			End If
			entries[n] = f
			n :+ 1
		Forever
		CloseDir(dir)
		Return entries[..n]
	End Method
	
	Rem
	bbdoc: Server cache purging, try to remove all cached servers.
	End Rem
	Method PurgeCachedServers()
		bmx_smbc_purgecachedservers(contextPtr)
	End Method
	
	Rem
	bbdoc: Gets whether to treat file names as case-sensitive if we can't determine when connecting to the remote share whether the file system is case sensitive.
	about: This defaults to False since it's most likely that if we can't retrieve the file system attributes, it's a
	very old file system that does not support case sensitivity.
	End Rem
	Method GetOptionCaseSensitive:Int()
		Return bmx_smbc_getoptioncasesensitive(contextPtr)
	End Method
	
	Rem
	bbdoc: Sets whether to treat file names as case-sensitive if we can't determine when connecting to the remote share whether the file system is case sensitive.
	about: This defaults to False since it's most likely that if we can't retrieve the file system attributes, it's 
	very old file system that does not support case sensitivity.
	End Rem
	Method SetOptionCaseSensitive(value:Int)
		bmx_smbc_setoptioncasesensitive(contextPtr, value)
	End Method
	
	Rem
	bbdoc: Gets file type.
	returns: 0 if file at @path doesn't exist, FILETYPE_FILE (1) if the file is a plain file or FILETYPE_DIR (2) if the file is a directory.
	End Rem
	Method FileType:Int(path:String)
		Return bmx_smbc_filetype(contextPtr, path)
	End Method
	
	Rem
	bbdoc: Gets the file size.
	End Rem
	Method FileSize:Int(path:String)
		Return bmx_smbc_filesize(contextPtr, path)
	End Method

	Rem
	bbdoc: Gets file time
	returns: The time the file at @path was last modified 
	End Rem
	Method FileTime:Int(path:String)
		Return bmx_smbc_filetime(contextPtr, path)
	End Method

	Rem
	bbdoc: Gets file mode
	returns: file mode flags
	End Rem
	Method FileMode:Int(path:String)
		Return bmx_smbc_filemode(contextPtr, path)
	End Method

	Rem
	bbdoc: Deletes a file
	returns: True if successful
	End Rem
	Method DeleteFile:Int(path:String)
		bmx_smbc_unlink(contextPtr, path)
		Return FileType(path) = FILETYPE_NONE
	End Method

	Rem
	bbdoc:  Deletes a directory.
	returns: True if successful
	about: Set @recurse to true to delete all subdirectories and files recursively - but be careful!
	End Rem
	Method DeleteDir:Int(path:String, recurse:Int = False)
		If Not path.EndsWith("/") Then
			path :+ "/"
		End If
		
		If recurse
			Local dir:TSMBCDirHandle = ReadDir(path)
			If Not dir Return False
			Repeat
				Local t:TSMBCDirent = NextFile(dir)
				If t.name = "" Exit
				If t.name = "." Or t.name = ".." Continue
				Local f:String = path + "/" + t.name
				Select FileType( f )
					Case 1 DeleteFile(f)
					Case 2 DeleteDir(f, True)
				End Select
			Forever
			CloseDir(dir)
		EndIf
		bmx_smbc_rmdir(contextPtr, path)
		If FileType( path )=0 Return True
	End Method
	
	Rem
	bbdoc: Creates a directory.
	returns: True if successful
	about: If @recurse is true, any required subdirectories are also created.
	End Rem
	Method CreateDir:Int(path:String, recurse:Int = False)
		If Not path.EndsWith("/") Then
			path :+ "/"
		End If
		
		If Not recurse
			bmx_smbc_mkdir(contextPtr, path, 1023)
			Return FileType(path) = FILETYPE_DIR
		EndIf
		
		Local t:String
		Local i:Int = path.find("smb://") + 1
		t :+ path[..i]
		path = path[i..]
		
		While path
			i = path.find("/")+1
			t :+ path[..i]
			path = path[i..]
			Select FileType(t)
			Case FILETYPE_DIR
			Case FILETYPE_NONE
				Local s:String
				If t.EndsWith("/") Then
					s = t[..t.length-1]
				Else
					s = t
				End If
				bmx_smbc_mkdir(contextPtr, s, 1023)
				If FileType(s)<>FILETYPE_DIR Return False
			Default
				Return False
			End Select
		Wend
		Return True
	End Method

	Rem
	bbdoc: Frees the client context. 
	End Rem
	Method Free()
		If contextPtr Then
			bmx_smbc_free_context(contextPtr)
			TSMBCStreamFactory.RemoveContext(Self)
			contextPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type

Type TSMBCStream Extends TStream

	Const MODE_READ:Int = $1
	Const MODE_WRITE:Int = $2

	Field context:TSMBC
	Field filePtr:Byte Ptr
	
	Field _pos:Int
	Field _size:Int
	Field _mode:Int

	Method Pos:Int()
		Return _pos
	End Method

	Method Size:Int()
		Return _size
	End Method

	Method Seek:Int(pos:Int)
		Assert filePtr Else "Attempt to seek closed stream"
		_pos = bmx_smbc_seek(context.contextPtr, filePtr, pos)
		Return _pos
	End Method

	Method Read:Int(buf:Byte Ptr, count:Int)
		Assert filePtr Else "Attempt to read from closed stream"
		Assert _mode & MODE_READ Else "Attempt to read from write-only stream"
		count = bmx_smbc_read(context.contextPtr, filePtr, buf, count)	
		_pos :+ count
		Return count
	End Method

	Method Write:Int(buf:Byte Ptr, count:Int)
		Assert filePtr Else "Attempt to write to closed stream"
		Assert _mode & MODE_WRITE Else "Attempt to write to read-only stream"
		count = bmx_smbc_write(context.contextPtr, filePtr, buf, count)
		_pos :+ count
		If _pos > _size Then
			_size=_pos
		End If
		Return count
	End Method

	Method Close()
		If filePtr Then
			bmx_smbc_close(context.contextPtr, filePtr)
			_pos=0
			_size=0
			filePtr = Null
		End If		
	End Method
	
	Method Delete()
		Close()
	End Method

	Function Create:TSMBCStream(context:TSMBC, path:String, readable:Int, writeable:Int)
		Local stream:TSMBCStream = New TSMBCStream
		stream.context = context
		stream._pos = 0
		stream._size = 0
		
		If readable Then
			stream._mode :| MODE_READ
		End If
		If writeable Then
			stream._mode :| MODE_WRITE
		End If
		
		stream.filePtr = bmx_smbc_open(context.contextPtr, path, readable, writeable)
		
		If stream.filePtr Then
			stream._size = context.FileSize(path)
			
			Return stream
		End If
		
		Return Null
	End Function

End Type

Type TSMBCStreamFactory Extends TStreamFactory

	Global contexts:TList = New TList

	Method CreateStream:TStream(url:Object, proto:String, path:String, readable:Int, writeable:Int)
		If proto = "smb" Then
			Local context:TSMBC = TSMBC(contexts.Last())	
		
			If context Then
				Return TSMBCStream.Create(context, "smb://" + path, readable, writeable)
			End If
		End If
	End Method
	
	Function AddContext(ctx:TSMBC)
		contexts.AddLast(ctx)
	End Function
	
	Function RemoveContext(ctx:TSMBC)
		contexts.Remove(ctx)
	End Function
	
End Type

New TSMBCStreamFactory

?
