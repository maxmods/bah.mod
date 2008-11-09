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


Type TMVSFTPParser Extends TRegexFTPParser

	Const _dsorg:String = "(PO|PS|VSAM)"
	
	Const _recfm:String = "(VB|FB|FBS|U|VBS)"
	
	Const _regex:String = "([A-Z0-9]{6})\s+" + ..                ' Volume
 	                     "([0-9]{4})\s+" + ..                  ' Unit
 	                     "([0-9]{4})/([0-9]{2})/([0-9]{2})\s+" + ..  ' year/Month/day
 	                     "([0-9]{1,3})\s+" + ..                ' Extends
 	                     "([0-9]{1,3})\s+" + ..                ' Used
 	                     _recfm + "\s+" + ..                      ' Record Format
 	                     "([0-9]{1,4})\s+" + ..                ' Logical Record Length
 	                     "([0-9]{1,5})\s+" + ..                ' Block Size
 	                     _dsorg + "\s+" + ..                      ' Dataset Organisation
 	                     "((([A-Z0-9#.]{1,8})[.]?){2,6})+"

	Function Create:TMVSFTPParser()
		Local this:TMVSFTPParser = New TMVSFTPParser
		
		this.init(_regex)
		
		Return this
	End Function

	Method parseEntry:TFilePath(parent:TFilePath, entry:String)

		Local file:TFilePath = TFilePath.Create(parent)
		
		If matches(entry) Then

			Local dsname:String = getGroup(12)
		
			If Not dsname Or dsname = "" Then
				Return Null
			End If
			
			Local dsorg:String = getGroup(11)

			Local yr:String = getGroup(3)
			Local mo:String = getGroup(4)
			Local da:String = getGroup(5)

			If dsorg = "PS" Then
				file.attributes.fType = FTP_TYPE_FILE
			Else
				file.attributes.fType = FTP_TYPE_DIRECTORY
			End If
			
			file.attributes.size = 0

			' timestamp
			Local dateTime:TFTPDateTime = New TFTPDateTime
			
			dateTime.year = yr.toInt()
			dateTime.Month = mo.toInt()
			dateTime.day = da.toInt()
			
			file.attributes.modified = dateTime
			
			file.setPath(Null, dsname)

		Else
			Return Null
		End If
		
		Return file
	End Method

End Type
