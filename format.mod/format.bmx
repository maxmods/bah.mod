' Copyright (c) 2007,2008 Bruce A Henderson
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
bbdoc: String Formatter
End Rem
Module BaH.Format

ModuleInfo "Version: 1.01"
ModuleInfo "License: MIT"
ModuleInfo "Copyright: 2007,2008 Bruce A Henderson"
ModuleInfo "Modserver: BRL"

ModuleInfo "History: 1.01"
ModuleInfo "History: Rewritten to use native snfprintf."
ModuleInfo "History: License change to MIT."
ModuleInfo "History: 1.00"
ModuleInfo "History: Initial Release."


Import BaH.RegEx
Import BRL.LinkedList

Import BRL.Blitz

Import "glue.cpp"

Extern
	Function bmx_sprintf_string:String(format:Byte Ptr, value:String)
	Function bmx_sprintf_float:String(format:Byte Ptr, value:Float)
	Function bmx_sprintf_int:String(format:Byte Ptr, value:Int)
	Function bmx_sprintf_double:String(format:Byte Ptr, value:Double)
	Function bmx_sprintf_long:String(format:Byte Ptr, value:Long)
End Extern



Rem
bbdoc: The string formatter.
about: Processes printf-style format strings.
End Rem
Type TFormatter

	' Private
	Global regex:TRegEx = TRegEx.Create("%(\d+\$)?([-#+ 0,l(\<]*)?(\d+)?(\.\d+)?([a-zA-Z%])")
	Field text:String
	
	Field formatParts:TStringFormatPart[]
	Field args:TArg[]
	
	
	Rem
	bbdoc: Creates a new #TFormatter object.
	about: Parameters:
	<ul>
	<li><b>text</b> : The text containing formatting instructions</li>
	</ul>
	End Rem
	Function Create:TFormatter(text:String)
		Local this:TFormatter = New TFormatter
		
		this.text = text
		
		If text Then
			this.parse()
		End If
		
		Return this
	End Function
	
	Method parse()
		Local parts:TList = New TList
		
		Local match:TRegExMatch = regex.Find(text)
		
		Local i:Int = 0
		
		While i < text.length
			
			If match Then
				
				' is there text before the formatting text ?
				If i <> match.SubStart(0) Then
					
					parts.addLast(TPlainText.Create(text[i..match.SubStart(0)]))
					
				End If
				
				' this is the formatting part
				Local sections:String[] = New String[5]
				For Local n:Int = 1 Until 6
					sections[n-1] = match.SubExp(n)
				Next
				
				parts.addLast(TFormattingText.Create(sections))
				
			Else
				' is there text at the end of all the formatting parts?
				parts.addLast(TPlainText.Create(text[i..]))
				
				Exit
			End If
			
			i = match.SubEnd(0)
			
			match = regex.find()
		Wend

		formatParts = New TStringFormatPart[parts.count()]
		i = 0
		For Local part:TStringFormatPart = EachIn parts
			formatParts[i] = part
			i:+ 1
		Next
		
	End Method
	
	Method addArg(arg:TArg)
		If Not args Then
			args = New TArg[1]
			
			args[0] = arg
		Else
		
			args = args[0..args.length + 1]
			args[args.length - 1] = arg
			
		End If
	End Method
	
	Rem
	bbdoc: Appends a Byte argument to the formatter.
	about: Parameters:
	<ul>
	<li><b>value</b> : The Byte argument value</li>
	</ul>
	End Rem
	Method ByteArg:TFormatter(value:Byte)
		
		Local arg:TByteArg = New TByteArg
		arg.value = value
		
		addArg(arg)
		
		Return Self
	End Method

	Rem
	bbdoc: Appends an Short argument to the formatter.
	about: Parameters:
	<ul>
	<li><b>value</b> : The Short argument value</li>
	</ul>
	End Rem
	Method ShortArg:TFormatter(value:Short)
	
		Local arg:TShortArg = New TShortArg
		arg.value = value
		
		addArg(arg)

		Return Self
	End Method

	Rem
	bbdoc: Appends an Int argument to the formatter.
	about: Parameters:
	<ul>
	<li><b>value</b> : The Int argument value</li>
	</ul>
	End Rem
	Method IntArg:TFormatter(value:Int)
	
		Local arg:TIntArg = New TIntArg
		arg.value = value
		
		addArg(arg)

		Return Self
	End Method

	Rem
	bbdoc: Appends a Long argument to the formatter.
	about: Parameters:
	<ul>
	<li><b>value</b> : The Long argument value</li>
	</ul>
	End Rem
	Method LongArg:TFormatter(value:Long)
	
		Local arg:TLongArg = New TLongArg
		arg.value = value
		
		addArg(arg)

		Return Self
	End Method

	Rem
	bbdoc: Appends a Float argument to the formatter.
	about: Parameters:
	<ul>
	<li><b>value</b> : The Float argument value</li>
	</ul>
	End Rem
	Method FloatArg:TFormatter(value:Float)
	
		Local arg:TFloatArg = New TFloatArg
		arg.value = value
		
		addArg(arg)

		Return Self
	End Method

	Rem
	bbdoc: Appends a Double argument to the formatter.
	about: Parameters:
	<ul>
	<li><b>value</b> : The Double argument value</li>
	</ul>
	End Rem
	Method DoubleArg:TFormatter(value:Double)
	
		Local arg:TDoubleArg = New TDoubleArg
		arg.value = value
		
		addArg(arg)

		Return Self
	End Method

	Rem
	bbdoc: Appends a String argument to the formatter.
	about: Parameters:
	<ul>
	<li><b>value</b> : The String argument value</li>
	</ul>
	End Rem
	Method StringArg:TFormatter(value:String)
	
		Local arg:TStringArg = New TStringArg
		arg.value = value
		
		addArg(arg)

		Return Self
	End Method

	Rem
	bbdoc: Processes and returns the formatted string.
	returns: The formatted String.
	End Rem
	Method Format:String()
		Local s:String
		Local arg:Int = 0
		
		If text Then
			For Local i:Int = 0 Until formatParts.length
				If TPlainText(formatParts[i]) Then
					s:+ formatParts[i].ToString()		
				Else
					If (Not TFormattingText(formatParts[i]).invalid) And (args And arg < args.length) Then
						s:+ TFormattingText(formatParts[i]).processArg(args[arg])
						' next arg only if this was a "real" arg format
						If TFormattingText(formatParts[i]).formatType <> TFormattingText.FTYPE_LINEBREAK And ..
								TFormattingText(formatParts[i]).formatType <> TFormattingText.FTYPE_PERCENT Then
							arg:+ 1
						End If
					Else
						If Not TFormattingText(formatParts[i]).invalid Then
							s:+ TFormattingText(formatParts[i]).processArg(New TNullArg)
						Else
							s:+ formatParts[i].ToString()
						End If
					End If
				End If
			Next
		End If
		
		Return s
	End Method

	Rem
	bbdoc: Clears the formatter argument list, ready for new arguments.
	End Rem
	Method Clear()
		args = Null
	End Method
	
End Type

Type TStringFormatPart
	
	Method ToString:String() Abstract
End Type

Type TPlainText Extends TStringFormatPart

	Field text:String

	Function Create:TPlainText(text:String)
		Local this:TPlainText = New TPlainText
		
		this.text = text
		
		Return this
	End Function
	
	Method ToString:String()
		Return text
	End Method
	
End Type

Type TFormattingText Extends TStringFormatPart

	Const FORMAT_LEFT_JUSTIFY:Int = $0001
	Const FORMAT_UPPERCASE:Int = $0002
	Const FORMAT_ALTERNATE:Int = $0004
	Const FORMAT_PLUS:Int = $0008
	Const FORMAT_SPACE_PAD:Int = $0010
	Const FORMAT_ZERO_PAD:Int = $0020
	Const FORMAT_GROUP:Int = $0040
	Const FORMAT_BRACKET:Int = $0080
	
	Const FTYPE_INTEGER:Int = 100    ' d
	Const FTYPE_OCTAL:Int = 111      ' o
	Const FTYPE_HEX:Int = 104        ' h
	Const FTYPE_SCIENTIFIC:Int = 101 ' e
	Const FTYPE_GENERAL:Int = 103    ' g
	Const FTYPE_FLOAT:Int = 102      ' f
	Const FTYPE_STRING:Int = 115     ' s
	Const FTYPE_LINEBREAK:Int = 110  ' n
	Const FTYPE_PERCENT:Int = 37     ' %
	
	Field sections:String[]
	Field index:Int = -1
	Field width:Int = -1
	Field flags:Int = 0
	Field precision:Int = -1
	Field formatType:Int = 0
	
	Field formatText:String
	Field formatPtr:Byte Ptr
	
	Field invalid:Int = False
	
	Function Create:TFormattingText(sections:String[])
		Local this:TFormattingText = New TFormattingText
		
		this.sections = sections
		this.configure()
		
		Return this
	End Function
	
	Method configure()
		formatText = "%"
		For Local i:Int = 0 Until sections.length
			formatText:+ sections[i]
		Next
		formatPtr = formatText.ToCString()
	End Method
	
	Method ProcessArg:String(arg:TArg)
		Local s:String
		
		If TStringArg(arg) Then
			s = bmx_sprintf_string(formatText, TStringArg(arg).value)
		Else If TIntArg(arg) Then
			s = bmx_sprintf_int(formatText, TIntArg(arg).value)
		Else If TFloatArg(arg) Then
				s = bmx_sprintf_float(formatText, TFloatArg(arg).value)
		Else If TDoubleArg(arg) Then
			s = bmx_sprintf_double(formatText, TDoubleArg(arg).value)
		Else If TLongArg(arg) Then
			s = bmx_sprintf_long(formatText, TLongArg(arg).value)
		Else If TShortArg(arg) Then
			s = bmx_sprintf_int(formatText, Int(TShortArg(arg).value))
		Else If TByteArg(arg) Then
			s = bmx_sprintf_int(formatText, Int(TByteArg(arg).value))
		End If

		Return s
	End Method

	Method ToString:String()
		Return "-"
	End Method
	
	Method Delete()
		If formatPtr Then
			MemFree(formatPtr)
			formatPtr = Null
		End If
	End Method
	
End Type

Type TArg

	Method ToString:String() Abstract
End Type

Type TNullArg Extends TArg
	Method ToString:String()
		Return Null
	End Method
End Type

Type TByteArg Extends TArg
	Field value:Byte

	Method ToString:String()
		Return String.fromInt(Int(value))
	End Method
End Type

Type TShortArg Extends TArg
	Field value:Short

	Method ToString:String()
		Return String.fromInt(Int(value))
	End Method
End Type

Type TIntArg Extends TArg
	Field value:Int

	Method ToString:String()
		Return String.fromInt(value)
	End Method
End Type

Type TFloatArg Extends TArg
	Field value:Float

	Method ToString:String()
		Return String.fromFloat(value)
	End Method
End Type

Type TDoubleArg Extends TArg
	Field value:Double

	Method ToString:String()
		Return String.fromDouble(value)
	End Method
End Type

Type TStringArg Extends TArg
	Field value:String

	Method ToString:String()
		Return value
	End Method
End Type

Type TLongArg Extends TArg
	Field value:Long

	Method ToString:String()
		Return String.fromLong(value)
	End Method
End Type

