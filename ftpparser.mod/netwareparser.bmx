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


Type TNetwareFTPParser Extends TRegexFTPParser

	Const _months:String = "(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)"
	
	Const _regex:String = "([-d])\s+" + ..
		"(\S+)\s+" + ..
		"(\d+|\S+)\s+" + ..
		"(\d+)\s+" + ..
		_months + "\s+" + ..
		"(\d{2})\s+" + ..
		"((([0-2][0-9]):([0-5][0-9]))|(\d{4}))\s+" + ..
		"(\S.*)"

	Function Create:TNetwareFTPParser()
		Local this:TNetwareFTPParser = New TNetwareFTPParser
		
		this.init(_regex)
		
		Return this
	End Function

	Method parseEntry:TFilePath(parent:TFilePath, entry:String)

		Local file:TFilePath = TFilePath.Create(parent)
		
		If matches(entry) Then

			Local name:String = getGroup(12)
		
			If Not name Or name = "" Or name = "." Or name = ".." Then
				Return Null
			End If
			
			Local typeStr:String = getGroup(1)
			Local usr:String = getGroup(3)
			Local size:String = getGroup(4)
			Local mo:String = getGroup(5)
			Local da:String = getGroup(6)
			Local hr:String = getGroup(9)
			Local mins:String = getGroup(10)
			Local yr:String = getGroup(11)

			Local _type:Int
			Select typeStr[0]
				Case 100 ' d
					_type = FTP_TYPE_DIRECTORY
				Default
					_type = FTP_TYPE_FILE					
			End Select
			
			file.attributes.fType = _type
			file.attributes.owner = usr
			
			file.attributes.size = size.toLong()

			' timestamp
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
