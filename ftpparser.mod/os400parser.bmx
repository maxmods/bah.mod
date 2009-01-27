'   Copyright 2007-2009 Bruce A Henderson
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


Type TOS400FTPParser Extends TRegexFTPParser

	Const _regex:String = "(\S+)\s+" + ..                ' user
		"(\d+)\s+" + ..               ' size
		"(\d\d)/(\d\d)/(\d\d)\s+" + ..' year/Month/day
		"([0-2][0-9]):([0-5][0-9]):([0-5][0-9])\s+" + ..' hour:minutes:seconds
		"(\\*\S+)\s+" + ..
		"(\S+/?)\s*"

	Function Create:TOS400FTPParser()
		Local this:TOS400FTPParser = New TOS400FTPParser
		
		this.init(_regex)
		
		Return this
	End Function

	Method parseEntry:TFilePath(parent:TFilePath, entry:String)

		Local file:TFilePath = TFilePath.Create(parent)
		
		If matches(entry) Then

			Local name:String = getGroup(10)
		
			If Not name Or name = "" Or name = "." Or name = ".." Then
				Return Null
			End If
		
			Local usr:String = getGroup(1)
			Local size:String = getGroup(2)
			Local yr:String = getGroup(3)
			Local mo:String = getGroup(4)
			Local da:String = getGroup(5)
			Local hr:String = getGroup(6)
			Local mins:String = getGroup(7)
			Local sec:String = getGroup(8)
			Local typeStr:String = getGroup(9)

			
			If typeStr.toLower() = "*dir" Then
				file.attributes.fType = FTP_TYPE_DIRECTORY
			Else
				file.attributes.fType = FTP_TYPE_FILE
			End If
			
			file.attributes.owner = usr
			
			file.attributes.size = size.toLong()

			' timestamp
			Local dateTime:TFTPDateTime = New TFTPDateTime

			dateTime.year = yr.toInt()
			If dateTime.year < 70 Then
				dateTime.year:+ 2000
			Else
				dateTime.year:+ 1900
			End If
			
			dateTime.Month = mo.toInt()
			dateTime.day = da.toInt()
			dateTime.hour = hr.toInt()
			dateTime.minute = mins.toInt()
			dateTime.Second = sec.toInt()

			file.attributes.modified = dateTime

			
			If name.endsWith("/") Then
				name = name[..name.length - 1]
			End If
			
			Local pos:Int = name.find("/")
			
			If pos > -1 Then
				name = name[pos + 1..]
			End If
			
			If parent Then
				file.setPath(parent.absolutePath, name)
			Else
				file.setPath(Null, name)
			End If

		Else
			Return Null
		End If
		
		
		
		Return file
	
	End Method


End Type



