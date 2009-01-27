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


Type TUnixFTPParser Extends TRegexFTPParser

	Const _months:String = "(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)"
	
	Const _regex:String = "([bcdlfmpSs-])" + ..
		"(((r|-)(w|-)([xsStTL-]))((r|-)(w|-)([xsStTL-]))((r|-)(w|-)([xsStTL-])))\s+" + ..
		"(\d+)?\s*" + ..
		"(\S+)\s+" + ..
		"(?:(\S+)\s+)?" + ..
		"(\d+)?\s+" + ..
		_months + "\s+" + ..
		"((?:[0-9])|(?:[0-2][0-9])|(?:3[0-1]))\s+" + ..
		"((\d{4})|((?:[01]?\d)|(?:2[0123])):([012345]\d))\s" + ..
		"(\s*\S+)(\s*.*)"

	Function Create:TUnixFTPParser()
		Local this:TUnixFTPParser = New TUnixFTPParser
		
		this.init(_regex)
		
		Return this
	End Function

	Method parseEntry:TFilePath(parent:TFilePath, entry:String)

		Local file:TFilePath = TFilePath.Create(parent)
		
		If entry.startsWith("+") Then
			Return Null
		End If
		
		If matches(entry) Then

			Local name:String = getGroup(25)
		
			If Not name Or name = "" Or name = "." Or name = ".." Then
				Return Null
			End If
		
			Local typeStr:String = getGroup(1)
			Local permStr:String = getGroup(2)
			Local hardLinkCount:String = getGroup(15)
			Local usr:String = getGroup(16)
			Local grp:String = getGroup(17)
			Local size:String = getGroup(18)
			Local mo:String = getGroup(19)
			Local da:String = getGroup(20)
			Local yr:String = getGroup(22)
			Local hr:String = getGroup(23)
			Local mins:String = getGroup(24)

			Local endToken:String = getGroup(26)
			
			Local _type:Int
			Select typeStr[0]
				Case 100 ' d
					_type = FTP_TYPE_DIRECTORY
				Case 108 ' l
					_type = FTP_TYPE_SYMBOLIC_LINK
				Case 98, 99 ' b, c - device...
					_type = FTP_TYPE_FILE
				Default
					_type = FTP_TYPE_FILE					
			End Select
			
			file.attributes.fType = _type
			file.attributes.permissions = TFilePermissions.Create(permStr)
			file.attributes.owner = usr
			file.attributes.group = grp
			
			file.attributes.size = size.toLong()


			Local dateTime:TFTPDateTime = New TFTPDateTime

			Local pos:Int = _months.find(mo)
			Local Month:Int = pos / 4
			
			If yr And yr <> "" Then
				' it's a year
				dateTime.year = yr.toInt()
			Else
				' it must be  hour/minute or we wouldn't have matched
				Local year:Int = CurrentDate()[7..].toInt()
				
				' if the month we're reading is greater than now, it must be last year
				pos = _months.find(CurrentDate()[3..6])
				If pos / 4 < Month Then
					year:-1
				End If
				
				dateTime.year = year
				dateTime.hour = hr.toInt()
				dateTime.minute = mins.toInt()
			
			End If
			
			dateTime.Month = Month + 1
			dateTime.day = da.toInt()
			
			file.attributes.modified = dateTime
			
			If Not endToken Or endToken = "" Then
				If parent Then
					file.setPath(parent.absolutePath, name)
				Else
					file.setPath(Null, name)
				End If
			Else
			
				name:+ endToken
				
				If _type = FTP_TYPE_SYMBOLIC_LINK Then
				
					Local _end:Int = name.find(" -> ")
					
					If _end < 0 Then
					
						If parent Then
							file.setPath(parent.absolutePath, name)
						Else
							file.setPath(Null, name)
						End If
					
					Else
					
						If parent Then
							file.setPath(parent.absolutePath, name[0.._end])
							file.setSymbolicLinkPath(parent.absolutePath, name[_end + 4..])
						Else
							file.setPath(Null, name[0.._end])
							file.setSymbolicLinkPath(Null, name[_end + 4..])
						End If
					
					End If
					
				
				End If
				
			End If
		Else
			Return Null
		End If
		
		Return file
	
	End Method


End Type

