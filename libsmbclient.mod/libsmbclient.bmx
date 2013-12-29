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
bbdoc: Samba Client
End Rem
Module BaH.LibSMBClient

?linux

Import "common.bmx"


Rem
bbdoc: An authorization object.
about: This is used as part of the authorization callback process. Workgroup, username and password fields should
be populated as required.
End Rem
Type TSMBCAuth

	Field contextPtr:Byte Ptr

	Field server:String
	Field share:String
	
	Field workgroup:String
	Field username:String
	Field password:String

	' private access functions
	Function _new:TSMBCAuth(contextPtr:Byte Ptr, server:String, share:String)
		Local auth:TSMBCAuth = New TSMBCAuth
		auth.contextPtr = contextPtr
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
Type TSMBDirHandle

	Field fdPtr:Byte Ptr
	
	Function _new:TSMBDirHandle(fdPtr:Byte Ptr)
		If fdPtr Then
			Local this:TSMBDirHandle = New TSMBDirHandle
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
Type TSMBDirent

	Field name:String
	Field ftype:Int
	
	Function _new:TSMBDirent(name:String, ftype:Int)
		Local this:TSMBDirent = New TSMBDirent
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

	Global map:TMap = New TMap

	Field contextPtr:Byte Ptr

	Rem
	bbdoc: Creates a new instance of a Samba client context.
	End Rem
	Method Create:TSMBC(smbDebug:Int = 0)
	
		contextPtr = bmx_smbc_new_context(smbDebug)
		
		If contextPtr Then
			map.Insert(String(Int(contextPtr)), Self)
			Return Self
		End If
		
		Return Null
	End Method
	
	Function _authDataCallback(auth:TSMBCAuth)
	
		' get instance from map
		Local smbc:TSMBC = TSMBC(map.ValueForKey(String(Int(auth.contextPtr))))
		
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
	returns: A TSMBDirHandle directory handle, or 0 if the directory does not exist
	End Rem
	Method ReadDir:TSMBDirHandle(path:String)
		Return TSMBDirHandle(bmx_smbc_opendir(contextPtr, path))
	End Method
	
	Rem
	bbdoc: Returns next file in a directory
	returns: The next file in directory opened using #ReadDir, or an empty object if there are no more files to read.
	End Rem
	Method NextFile:TSMBDirent(dir:TSMBDirHandle)
		Return TSMBDirent(bmx_smbc_readdir(contextPtr, dir.fdPtr))
	End Method
	
	Rem
	bbdoc: Closes a directory
	End Rem
	Method CloseDir(dir:TSMBDirHandle)
		bmx_smbc_closedir(contextPtr, dir.fdPtr)
	End Method

	Rem
	bbdoc: Loads a directory
	returns: An array of directory entity contents of @dir
	about: The @skipDots parameter, if true, removes the '.' (current) and '..'
	(parent) directories from the returned array.
	End Rem 
	Method LoadDir:TSMBDirent[](path:String, skipDots:Int = True)
		Local dir:TSMBDirHandle = ReadDir(path)
		If Not dir Then
			Return Null
		End If
			
		Local entries:TSMBDirent[100]
		Local n:Int
		
		Repeat
			Local f:TSMBDirent = NextFile(dir)
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
	bbdoc: Frees the client context. 
	End Rem
	Method Free()
		If contextPtr Then
			smbc_free_context(contextPtr, 1)
			contextPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method
	
End Type



?