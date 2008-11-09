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


Type TNTFTPParser Extends TRegexFTPParser

	Const _regex:String = "((?:0[1-9])|(?:1[0-2]))-" + ..
 	                     "((?:0[1-9])|(?:[1-2]\d)|(?:3[0-1]))-" + ..
 	                     "(\d\d|\d\d\d\d)\\s*" + ..
 	                     "((?:0[1-9])|(?:1[012])):" + ..
 	                     "([0-5]\d)\s*" + ..
 	                     "([AP])M\s*" + ..
 	                     "(?:(<DIR>)|([0-9]+))\s+" + ..
 	                     "(\S.*)"

	Function Create:TNTFTPParser()
		Local this:TNTFTPParser = New TNTFTPParser
		
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
			
			Local mo:String = getGroup(1)
			Local da:String = getGroup(2)
			Local yr:String = getGroup(3)
			Local hr:String = getGroup(4)
			Local mins:String = getGroup(5)
			Local ampm:String = getGroup(6)
			Local dirStr:String = getGroup(7)
			Local size:String = getGroup(8)
			
			If dirStr And dirStr = "<DIR>" Then
				file.attributes.fType = FTP_TYPE_DIRECTORY
			Else
				file.attributes.fType = FTP_TYPE_FILE

				If size Then
					file.attributes.size = size.toLong()
				End If
			End If

			' timestamp
			Local dateTime:TFTPDateTime = New TFTPDateTime
			
			dateTime.year = yr.toInt() + 2000
			If dateTime.year > 2080 Then
				dateTime.year :- 100
			End If
			dateTime.Month = mo.toInt()
			dateTime.day = da.toInt()
			dateTime.minute = mins.toInt()
			dateTime.hour = hr.toInt()
			
			If ampm = "P" Then
				If dateTime.hour <> 12 Then
					dateTime.hour:+ 12
				End If
			Else If dateTime.hour = 12 Then
				dateTime.hour = 0
			End If
			
			file.attributes.modified = dateTime
			
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

