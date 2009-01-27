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


Type TVMSFTPParser Extends TRegexFTPParser

	Const _months:String = "(JAN|FEB|MAR|APR|MAY|JUN|JUL|AUG|SEP|OCT|NOV|DEC)"
	
	Const _regex:String = "(.*;[0-9]+)\s*" + ..
		"(\d+)/\d+\s*" + ..
		"(\d{1,2})-" + ..
		_months + ..
		"-([0-9]{4})\s*" + ..
		"((?:[01]\d)|(?:2[0-3])):([012345]\d):([012345]\d)\s*" + ..
		"\[(([0-9$A-Za-z_]+)|([0-9$A-Za-z_]+),([0-9$a-zA-Z_]+))\]?\s*" + ..
		"\([a-zA-Z]*,[a-zA-Z]*,[a-zA-Z]*,[a-zA-Z]*\)"

	Function Create:TVMSFTPParser()
		Local this:TVMSFTPParser = New TVMSFTPParser
		
		this.init(_regex)
		
		Return this
	End Function

	Method parseEntry:TFilePath(parent:TFilePath, entry:String)
	
		Local longBlock:Long = 512

		Local file:TFilePath = TFilePath.Create(parent)
		
		If matches(entry) Then

			Local name:String = getGroup(1)
		
			If Not name Or name = "" Or name = "." Or name = ".." Then
				Return Null
			End If
		
			Local size:String = getGroup(2)
			Local da:String = getGroup(3)
			Local mo:String = getGroup(4)
			Local yr:String = getGroup(5)
			Local hr:String = getGroup(6)
			Local mins:String = getGroup(7)


			If name.findLast(".DIR") > -1 Then
				file.attributes.fType = FTP_TYPE_DIRECTORY
			Else
				file.attributes.fType = FTP_TYPE_FILE
			End If
						
			file.attributes.size = size.toLong() * longBlock

			' timestamp
			Local dateTime:TFTPDateTime = New TFTPDateTime
			
			dateTime.year = yr.toInt()
			dateTime.Month = _months.find(mo) / 4 + 1
			dateTime.day = da.toInt()
			dateTime.hour = hr.toInt()
			dateTime.minute = mins.toInt()
			
			file.attributes.modified = dateTime
			
			name = name[..name.findLast(";")]
			
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


