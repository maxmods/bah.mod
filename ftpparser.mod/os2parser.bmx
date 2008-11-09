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

Import "common.bmx"


Type TOS2FTPParser Extends TRegexFTPParser

	Const _regex:String = "(\s+|[0-9]+)\s*" + ..
		"(\s+|[A-Z]+)\s*" + ..
		"(DIR|\s+)\s*" + ..
		"((?:0[1-9])|(?:1[0-2]))-" + ..
		"((?:0[1-9])|(?:[1-2]\d)|(?:3[0-1]))-" + ..
		"(\d\d)\s*" + ..
		"(?:([0-1]\d)|(?:2[0-3])):" + ..
		"([0-5]\\d)\s*" + ..
		"(\S.*)"

	Function Create:TOS2FTPParser()
		Local this:TOS2FTPParser = New TOS2FTPParser
		
		this.init(_regex)
		
		Return this
	End Function

	Method parseEntry:TFilePath(parent:TFilePath, entry:String)

		Local file:TFilePath = TFilePath.Create(parent)
		
		If matches(entry) Then

			Local name:String = getGroup(9)
		
			If Not name Or name = "" Or name = "." Or name = ".." Then
				Return Null
			End If
		
			Local size:String = getGroup(1)
			Local attrib:String = getGroup(2)
			Local dirStr:String = getGroup(3)
			Local mo:String = getGroup(4)
			Local da:String = getGroup(5)
			Local yr:String = getGroup(6)
			Local hr:String = getGroup(7)
			Local mins:String = getGroup(8)
			
			If dirStr.Trim() = "DIR" Or attrib.Trim() = "DIR" Then
				file.attributes.fType = FTP_TYPE_DIRECTORY
			Else
				file.attributes.fType = FTP_TYPE_FILE
			End If
			
			file.attributes.size = size.Trim().toLong()

			' timestamp
			Local dateTime:TFTPDateTime = New TFTPDateTime
			
			dateTime.year = yr.toInt() + 2000
			If dateTime.year > 2080 Then
				dateTime.year:- 100
			End If
			
			dateTime.Month = mo.toInt()
			dateTime.day = da.toInt()
			dateTime.hour = hr.toInt()
			dateTime.minute = mins.toInt()
			
			file.attributes.modified = dateTime
			
			If parent Then
				file.setPath(parent.absolutePath, name.Trim())
			Else
				file.setPath(Null, name.Trim())
			End If

		Else
			Return Null
		End If
		
		
		
		Return file
	
	End Method


End Type


