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

Rem
bbdoc: FTP list parser
End Rem
Module BaH.FTPParser

ModuleInfo "Version: 1.02"
ModuleInfo "Author: Bruce A Henderson"
ModuleInfo "License: Apache 2.0"
ModuleInfo "Copyright: 2007 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.02"
ModuleInfo "History: Fixed double-slashes in expressions."
ModuleInfo "History: 1.01"
ModuleInfo "History: Fixed problem on Win32, which has a different newline."
ModuleInfo "History: 1.00"
ModuleInfo "History: Based on Apache Commons FTP Parsers and Cyberduck source"

Import BRL.LinkedList

Import "unixparser.bmx"
Import "netwareparser.bmx"
Import "enterpriseunixparser.bmx"
Import "ntparser.bmx"
Import "mvsparser.bmx"
Import "os2parser.bmx"
Import "os400parser.bmx"
Import "vmsparser.bmx"

Rem
bbdoc: Tries to determine the FTP server kind based on directory list, and provides a parser for it.
about: Current FTP servers supported : Unix, Netware, Enterprise Unix, NT, MVS, OS2, OS400 and VMS.
End Rem
Type TFTPParserFactory

	Field cache:TRegexFTPParser
	
	Global parserList:TList
	
	Rem
	bbdoc: Creates a new #TFTPParserFactory object.
	End Rem
	Function Create:TFTPParserFactory()
		Local this:TFTPParserFactory = New TFTPParserFactory
		
		this.init()
		
		Return this
	End Function
	
	Method init()
		If Not parserList Then
		
			parserList = New TList
			
			parserList.addLast(TUnixFTPParser.Create())
			parserList.addLast(TNetwareFTPParser.Create())
			parserList.addLast(TEnterpriseUnixFTPParser.Create())
			parserList.addLast(TNTFTPParser.Create())
			parserList.addLast(TMVSFTPParser.Create())
			parserList.addLast(TOS2FTPParser.Create())
			parserList.addLast(TOS400FTPParser.Create())
			parserList.addLast(TVMSFTPParser.Create())
		
		End If
	End Method
	
	Rem
	bbdoc: Clears the parser cache, forcing it to re-test the next time #parseList is invoked.
	End Rem
	Method clearCache()
		cache = Null
	End Method
	
	Rem
	bbdoc: Parse the directory list (as a multi-line String)
	about: Will try different parsers until it finds one that works on the list.
	Once found, it reuses that specific parser from that point on every call to this method.
	End Rem
	Method parseList:TList(parent:TFilePath, list:String)
		Local fileList:TList
	
		Local split:String = "~n"
		
		' try and find the correct newline...
		If list.find("~r~n") Then
			split = "~r~n"
		Else If list.find("~n") < 0 Then
			split = "~r"
		End If
		
		Local entries:String[] = list.split(split)
		
		For Local i:Int = 0 Until entries.length
		
			Local entry:String = entries[i]
			Local file:TFilePath = Null
		
			If cache = Null Then
				For Local p:TRegexFTPParser = EachIn parserList
					file = p.parseEntry(parent, entry)
					If file Then
						cache = p
						Exit
					End If
				Next
			Else
				file = cache.parseEntry(parent, entry)
			End If
		
			If file Then
				If Not fileList Then
					fileList = New TList
				End If
				
				fileList.addLast(file)
			End If
		Next
	
		Return fileList
	End Method

End Type
