' Copyright (c) 2009-2010 Bruce A Henderson
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

Import "-lobjc"
Import "-framework ScriptingBridge"

Import "glue.m"


Extern
	Function bmx_sb_sbapplication_applicationWithBundleIdentifier:Byte Ptr(ident:String)
	Function bmx_sb_sbapplication_applicationWithProcessIdentifier:Byte Ptr(pid:Int)
	Function bmx_sb_sbapplication_applicationWithURL:Byte Ptr(url:String)
	Function bmx_sb_sbapplication_isRunning:Int(handle:Byte Ptr)
	Function bmx_sb_sbapplication_activate(handle:Byte Ptr)

	Function bmx_sb_sbobject_propertyAsString:String(handle:Byte Ptr, name:String)
	Function bmx_sb_sbobject_propertyAsObject:Object(handle:Byte Ptr, name:String, predicate:String)
	Function bmx_sb_sbobject_propertyAsInt:Int(handle:Byte Ptr, name:String)
	Function bmx_sb_sbobject_propertyAsDouble:Double(handle:Byte Ptr, name:String)
	Function bmx_sb_sbobject_propertyAsList:Byte Ptr(handle:Byte Ptr, name:String, predicate:String, immediate:Int)
	Function bmx_sb_sbobject_propertyAsPtr:Byte Ptr(handle:Byte Ptr, name:String, length:Int Ptr)
	Function bmx_sb_sbobject_call(handle:Byte Ptr, name:String)
	Function bmx_sb_sbobject_callWithInt(handle:Byte Ptr, name:String, value:Int)
	Function bmx_sb_sbobject_callReturningInt:Int(handle:Byte Ptr, name:String)
	Function bmx_sb_sbobject_setPropertyAsInt(handle:Byte Ptr, name:String, value:Int)
	Function bmx_sb_sbobject_setPropertyAsString(handle:Byte Ptr, name:String, value:String)
	Function bmx_sb_sbobject_propertyAsDate:Double(handle:Byte Ptr, name:String)
	Function bmx_sb_sbobject_description:String(handle:Byte Ptr)
	Function bmx_sb_sbobject_propertyAsURL:String(handle:Byte Ptr, name:String, kind:Int)

	Function bmx_sb_sbelementarray_free(handle:Byte Ptr)
	Function bmx_sb_sbelementarray_count:Int(handle:Byte Ptr)
	Function bmx_sb_sbelementarray_propertyArrayAsString:String[](handle:Byte Ptr, name:String)
	Function bmx_sb_sbelementarray_setPropertyArrayAsInt(handle:Byte Ptr, name:String, value:Int)
	Function bmx_sb_sbelementarray_description:String(handle:Byte Ptr)

	Function bmx_sb_sbobjectenum_nextObject:Byte Ptr(handle:Byte Ptr)

End Extern

Rem
bbdoc: Returns the path of a URL conforming to RFC 1808.
End Rem
Const URL_PATH:Int = 0
Rem
bbdoc: Returns the string for the receiver as if it were an absolute URL.
End Rem
Const URL_ABSOLUTE:Int = 1
