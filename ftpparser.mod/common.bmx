'   Copyright 2007 Bruce A Henderson
'
'   Licensed under the Apache License, Version 2.0 (the "License");
'   you may not use this file except in compliance with the License.
'   You may obtain a copy of the License at
'
'       http://www.apache.org/licenses/LICENSE-2.0
'
'   Unless required by applicable law or agreed to in writing, software
'   distributed under the License is distributed on an "AS IS" BASIS,
'   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
'   See the License for the specific language governing permissions and
'   limitations under the License.

SuperStrict

Import BRL.System
Import BaH.RegEx

Type TRegexFTPParser Abstract

	Field regex:TRegEx
	
	Field result:TRegExMatch
	
	Method parseEntry:TFilePath(parent:TFilePath, name:String) Abstract

	Method init(reg:String)
		regex = TRegEx.Create(reg)
	End Method
	
	Method matches:Int(s:String)
	
		result = regex.find(s)
		
		If Not result Then
			Return False
		End If
		
		Return True
		
	End Method

	Method getGroupCount:Int()
		If Not result Then
			Return 0
		End If
		
		Return result.SubCount()
	End Method

	Method getGroup:String(group:Int)
		If Not result Then
			Return Null
		End If
		
		Return result.SubExp(group)
	End Method
	
End Type

Const FTP_TYPE_FILE:Int = 1
Const FTP_TYPE_DIRECTORY:Int = 2
Const FTP_TYPE_SYMBOLIC_LINK:Int = 4

Rem
bbdoc: A specific File/Directory/Symbolic Link.
End Rem
Type TFilePath

	Field parent:TFilePath
	Rem
	bbdoc: The absolute path of this #TFilePath
	End Rem
	Field absolutePath:String
	
	Rem
	bbdoc: The name of this #TFilePath.
	End Rem
	Field name:String
	
	Rem
	bbdoc: The symbolic link that this #TFilePath points to, if appropriate.
	End Rem
	Field symbolic:String
	
	Rem
	bbdoc: The attributes.
	about: See #TFileAttributes
	End Rem
	Field attributes:TFileAttributes
	
	Rem
	bbdoc: Creates a new #TFilePath, with an option @parent.
	about: The @parent will deteremine the proper absolutePath value.
	End Rem
	Function Create:TFilePath(parent:TFilePath = Null)
		Local this:TFilePath = New TFilePath
	
		this.parent = parent
		this.attributes = New TFileAttributes
	
		Return this
	End Function

	Method setPath(_parent:String, name:String)
	
		If _parent Then
			If _parent.endsWith("/") Then
				absolutePath = _parent + name
			Else
				absolutePath = _parent + "/" + name
			End If
		End If
	
		Self.name = name
		
	End Method
	
	Method setSymbolicLinkPath(_parent:String, name:String)

		If _parent Then
			If _parent.endsWith("/") Then
				symbolic = _parent + name
			Else
				symbolic = _parent + "/" + name
			End If
		Else
			symbolic = name
		End If
	
	End Method
	
	Rem
	bbdoc: Returns True if this is a symbolic link.
	End Rem
	Method isSymbolic:Int()
		Return attributes.fType & FTP_TYPE_SYMBOLIC_LINK = FTP_TYPE_SYMBOLIC_LINK
	End Method
	
	Rem
	bbdoc: Returns True if this is a file.
	End Rem
	Method isFile:Int()
		Return attributes.fType & FTP_TYPE_FILE = FTP_TYPE_FILE
	End Method
	
	Rem
	bbdoc: Returns true if this is a directory.
	End Rem
	Method isDir:Int()
		Return attributes.fType & FTP_TYPE_DIRECTORY = FTP_TYPE_DIRECTORY
	End Method
	
End Type

Rem
bbdoc: File attributes.
End Rem
Type TFileAttributes

	Rem
	bbdoc: The file type.
	about: One of, FTP_TYPE_FILE, FTP_TYPE_DIRECTORY or FTP_TYPE_SYMBOLIC_LINK.
	End Rem
	Field fType:Int
	Rem
	bbdoc: The file permissions, if available.
	End Rem
	Field permissions:TFilePermissions

	Rem
	bbdoc: The owner/user, if available.
	End Rem
	Field owner:String

	Rem
	bbdoc: The group, if available.
	End Rem
	Field group:String
	
	Rem
	bbdoc: The file size.
	End Rem
	Field size:Long
	
	Rem
	bbdoc: The timestamp, if available.
	End Rem
	Field modified:TFTPDateTime

End Type

Rem
bbdoc: File Permissions.
End Rem
Type TFilePermissions

	Const INDEX_OWNER:Int = 0
	Const INDEX_GROUP:Int = 1
	Const INDEX_OTHER:Int = 2
	Const INDEX_READ:Int = 0
	Const INDEX_WRITE:Int = 1
	Const INDEX_EXECUTE:Int = 2


	Const defaultMask:String = "---------"
	
	Field mask:String
	
	Field owner:Int[] = New Int[3]
	Field group:Int[] = New Int[3]
	Field other:Int[] = New Int[3]

	Function Create:TFilePermissions(mask:String = Null)
		Local this:TFilePermissions = New TFilePermissions
	
		If Not mask Then
			this.mask = defaultMask
		Else
			this.mask = mask
		End If
		
		this.owner = this.getOwnerPermissions(this.mask)
		this.group = this.getGroupPermissions(this.mask)
		this.other = this.getOtherPermissions(this.mask)
	
		Return this
	End Function
	
	
	Method getOwnerPermissions:Int[](p:String = Null)

		If Not p Then
			Return owner
		End If
	
		Return [ p[0] = 114, p[1] = 119, ..
			p[2] = 120 | p[2] = 115 | p[2] = 83 | p[2] = 116 | p[2] = 84 | p[2] = 76]
		
	End Method

	Method getGroupPermissions:Int[](p:String = Null)

		If Not p Then
			Return group
		End If
	
		Return [ p[3] = 114, p[4] = 119, ..
			p[5] = 120 | p[5] = 115 | p[5] = 83 | p[5] = 116 | p[5] = 84 | p[5] = 76]
		
	End Method

	Method getOtherPermissions:Int[](p:String = Null)
	
		If Not p Then
			Return other
		End If
	
		Return [ p[6] = 114, p[7] = 119, ..
			p[8] = 120 | p[8] = 115 | p[8] = 83 | p[8] = 116 | p[8] = 84 | p[8] = 76]
		
	End Method

End Type

Rem
bbdoc: A date-time representation for FTP files and directories.
End Rem
Type TFTPDateTime

	Field day:Int
	Field Month:Int
	Field year:Int
	
	Field hour:Int
	Field minute:Int
	Field Second:Int
	
	Function Create:TFTPDateTime(day:Int, Month:Int, year:Int, hour:Int, minute:Int, Second:Int)
		Local this:TFTPDateTime = New TFTPDateTime
		
		this.day = day
		this.Month = Month
		this.year = year
		this.hour = hour
		this.minute = minute
		this.Second = Second
		
		Return this
	End Function
	
	Method toString:String()
		Local s:String = year + "/"
		
		If Month < 10 Then
			s:+ "0"
		End If
		s:+ Month + "/"
		
		If day < 10 Then
			s:+ "0"
		End If
		s:+ day + " "
		
		If hour < 10 Then
			s:+ "0"
		End If
		s:+ hour + ":"
		
		If minute < 10 Then
			s:+ "0"
		End If
		s:+ minute + ":"
		
		If Second < 10 Then
			s:+ "0"
		End If
		
		s:+ Second
		
		Return s
	End Method
	
End Type


