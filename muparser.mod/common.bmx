' Copyright (c) 2008-2010 Bruce A Henderson
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

Import "include/*.h"

Import "src/muParser.cpp"
Import "src/muParserTokenReader.cpp"
Import "src/muParserBase.cpp"
Import "src/muParserBytecode.cpp"
Import "src/muParserCallback.cpp"
Import "src/muParserComplex.cpp"
Import "src/muParserError.cpp"
Import "src/muParserInt.cpp"


Import "glue.cpp"


Extern

	Function bmx_muparser_parser_new:Byte Ptr()
	Function bmx_muparser_parser_delete(handle:Byte Ptr)

	Function bmx_muparser_parserbase_eval:Double(handle:Byte Ptr)
	Function bmx_muparser_parserbase_setexpr(handle:Byte Ptr, expr:String)
	Function bmx_muparser_parserbase_definevar(handle:Byte Ptr, name:String, value:Double Ptr)
	Function bmx_muparser_parserbase_clearvar(handle:Byte Ptr)
	Function bmx_muparser_parserbase_clearfun(handle:Byte Ptr)
	Function bmx_muparser_parserbase_clearconst(handle:Byte Ptr)
	Function bmx_muparser_parserbase_clearinfixoprt(handle:Byte Ptr)
	Function bmx_muparser_parserbase_clearpostfixoprt(handle:Byte Ptr)
	Function bmx_muparser_parserbase_clearoprt(handle:Byte Ptr)


	Function bmx_muparser_parserbase_definefun0(handle:Byte Ptr, name:String, callback:Double(), allowOpt:Int)
	Function bmx_muparser_parserbase_definefun1(handle:Byte Ptr, name:String, callback:Double(val1:Double), allowOpt:Int)
	Function bmx_muparser_parserbase_definefun2(handle:Byte Ptr, name:String, callback:Double(val1:Double, val2:Double), allowOpt:Int)
	Function bmx_muparser_parserbase_definefun3(handle:Byte Ptr, name:String, callback:Double(val1:Double, val2:Double, val3:Double), allowOpt:Int)
	Function bmx_muparser_parserbase_definefun4(handle:Byte Ptr, name:String, callback:Double(val1:Double, val2:Double, val3:Double, val4:Double), allowOpt:Int)
	Function bmx_muparser_parserbase_definefun5(handle:Byte Ptr, name:String, callback:Double(val1:Double, val2:Double, val3:Double, val4:Double, val5:Double), allowOpt:Int)
	Function bmx_muparser_parserbase_definemultfun(handle:Byte Ptr, name:String, callback:Double(val:Double Ptr, size:Int), allowOpt:Int)
	Function bmx_muparser_parserbase_defineoprt(handle:Byte Ptr, name:String, callback:Double(val1:Double, val2:Double), priority:Int, allowOpt:Int)
	Function bmx_muparser_parserbase_definepostfixoprt(handle:Byte Ptr, name:String, callback:Double(val1:Double), allowOpt:Int)
	Function bmx_muparser_parserbase_defineinfixoprt(handle:Byte Ptr, name:String, callback:Double(val1:Double), precidence:Int, allowOpt:Int)

	Function bmx_muparser_parserbase_removevar(handle:Byte Ptr, name:String)
	Function bmx_muparser_parserbase_enableoptimizer(handle:Byte Ptr, isOn:Int)
	Function bmx_muparser_parserbase_enablebytecode(handle:Byte Ptr, isOn:Int)
	Function bmx_muparser_parserbase_enablebuiltintoprt(handle:Byte Ptr, isOn:Int)
	Function bmx_muparser_parserbase_hasbuiltinoprt:Int(handle:Byte Ptr)

	Function bmx_muparser_parserbase_defineconst(handle:Byte Ptr, name:String, value:Double)
	Function bmx_muparser_parserbase_definestrconst(handle:Byte Ptr, name:String, value:String)

	Function bmx_muparser_parserbase_setvarfactory(handle:Byte Ptr, callback:Double Ptr(name:Byte Ptr, userData:Object), userData:Object)

End Extern

Const prLOGIC:Int = 1  ' logic operators
Const prCMP:Int = 2  ' comparsion operators
Const prADD_SUB:Int = 3  ' addition
Const prMUL_DIV:Int = 4  ' multiplication/division
Const prPOW:Int = 5  ' power operator priority (highest)

' infix operators
Const prINFIX:Int = 4 ' Signs have a higher priority than ADD_SUB, but Lower than power operator
Const prPOSTFIX:Int = 4  ' Postfix operator priority (currently unused)


