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

Import BRL.Blitz

Import "src/*.h"

Import "src/xmlparse.c"
Import "src/xmlrole.c"
Import "src/xmltok.c"
Import "src/xmltok_impl.c"
Import "src/xmltok_ns.c"

Import "glue.cpp"

Extern

	Function bmx_expat_XML_ParserCreate:Byte Ptr(encoding:String)
	Function bmx_expat_XML_Parse:Int(handle:Byte Ptr, text:String, isFinal:Int)
	Function bmx_expat_XML_SetStartElementHandler(handle:Byte Ptr)

	Function bmx_expat_XML_ErrorString:String(code:Int)

	Function XML_ParserFree(handle:Byte Ptr)
	Function XML_SetUserData(handle:Byte Ptr, data:Object)
	Function XML_StopParser:Int(handle:Byte Ptr, resumable:Int)
	Function XML_ResumeParser:Int(handle:Byte Ptr)
	Function XML_SetStartElementHandler(handle:Byte Ptr, cb(data:Byte Ptr, name:Byte Ptr, attr:Byte Ptr Ptr))
	Function XML_GetErrorCode:Int(handle:Byte Ptr)
	Function XML_GetCurrentLineNumber:Int(handle:Byte Ptr)
	
End Extern



Const XML_STATUS_ERROR:Int = 0
Const XML_STATUS_OK:Int = 1
