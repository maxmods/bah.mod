' Copyright (c) 2007-2009 Bruce A Henderson
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
bbdoc: Expat XML Parser
End Rem
Module BaH.Expat

ModuleInfo "Version: 1.01"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: Wrapper - 2007-2009 Bruce A Henderson"

ModuleInfo "History: 1.01"
ModuleInfo "History: Implemented API for BlitzMax."
ModuleInfo "History: 1.00 Initial Release"
ModuleInfo "History: Includes Expat 2.01 source."

ModuleInfo "CC_OPTS: -DHAVE_EXPAT_CONFIG_H"
?ppc
ModuleInfo "CC_OPTS: -DWORDS_BIGENDIAN"
?win32
ModuleInfo "CC_OPTS: -DXML_STATIC"
?

' expat_config.h
' added ifdef WORDS_BIGENDIAN test to set BYTEORDER definition

Import "common.bmx"

Rem
bbdoc: An XML document parser.
about: A parser if good for parsing only one XML document or external parsed entity.
If your application needs to parse many XML documents, then it needs to create a parser
object for each one.
End Rem
Type TXMLParser

	Field parserPtr:Byte Ptr
	
	Field userData:Object
	
	' callbacks
	Field userStartElementHandler(userData:Object, name:String, attribs:String[])
	Field userEndElementHandler(userData:Object, name:String)

	Rem
	bbdoc: 
	End Rem
	Function CreateParser:TXMLParser(encoding:String = "UTF-8")
		Return New TXMLParser.Create(encoding)
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Method Create:TXMLParser(encoding:String = "UTF-8")
		parserPtr = bmx_expat_XML_ParserCreate(encoding)
		XML_SetUserData(parserPtr, Self)
		Return Self
	End Method


	Rem
	bbdoc: Parses some more of the document.
	about: The string @text is a string containing part (or perhaps all) of the document.
	The @isFinal parameter informs the parser that this is the last piece of the document.
	Frequently, the last piece is empty (i.e. len is zero.) If a parse error occurred, it
	returns XML_STATUS_ERROR. Otherwise it returns XML_STATUS_OK value. 
	End Rem
	Method Parse:Int(text:String, isFinal:Int = True)
		Return bmx_expat_XML_Parse(parserPtr, text, isFinal)
	End Method


	Rem
	bbdoc: 
	End Rem
	Method StopParser:Int(resumable:Int)
		Return XML_StopParser(parserPtr, resumable)
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method ResumeParser:Int()
		Return XML_ResumeParser(parserPtr)
	End Method

	Rem
	bbdoc: 
	End Rem
	Method SetStartElementHandler(handler(userData:Object, name:String, attribs:String[]))
		userStartElementHandler = handler
		If handler Then
			bmx_expat_XML_SetStartElementHandler(parserPtr)
		Else
			XML_SetStartElementHandler(parserPtr, Null)
		End If
	End Method
	
	' internal callback
	Function _StartElementHandler(parser:TXMLParser, name:String, attrs:String[])
		parser.userStartElementHandler(parser.userData, name, attrs)
	End Function


	Rem
	bbdoc: Returns what type of error has occurred.
	End Rem
	Method GetErrorCode:Int()
		Return XML_GetErrorCode(parserPtr)
	End Method
	
	Rem
	bbdoc: Returns the line number of the position.
	about: The first line is reported as 1.
	End Rem
	Method GetCurrentLineNumber:Int()
		Return XML_GetCurrentLineNumber(parserPtr)
	End Method
	
	Rem
	bbdoc: Sets the user data that gets passed to handlers.
	about: It overwrites any previous value. 
	End Rem
	Method SetUserData(data:Object)
		userData = data
	End Method
	
	Rem
	bbdoc: Returns the user data that gets passed to handlers.
	End Rem
	Method GetUserData:Object()
		Return userData
	End Method
	
	Rem
	bbdoc: Frees memory used by the parser. 
	End Rem
	Method Free()
		If parserPtr Then
			XML_ParserFree(parserPtr)
			parserPtr = Null
		End If
	End Method
	
	Method Delete()
		Free()
	End Method

End Type

Rem
bbdoc: Returns a string describing the error corresponding to code.
about: The code should be one of the values that can be returned from #GetErrorCode.
End Rem
Function XMLErrorString:String(code:Int)
	Return bmx_expat_XML_ErrorString(code)
End Function

