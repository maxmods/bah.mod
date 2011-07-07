SuperStrict

Framework BaH.Libiconv
Import BRL.TextStream
Import BRL.StandardIO

Local inp:TInputTextStream = TInputTextStream.Create("Hi there!")
Local out:TOutputTextStream  = New TOutputTextStream 

'Local s:String = LoadText(inp)
'SaveText(s, out)

'Print out.text

Local iconv:TIConv = TIConv.Create("ASCII", "ASCII")

If iconv Then

	'DebugStop
	iconv.ConvertStream(inp, out, 5)
	
	Print out.text
	
End If


Type TInputTextStream Extends TStream

	Field text:String
	Field length:Int
	Field offset:Int
	
	Function Create:TInputTextStream(text:String)
		Local this:TInputTextStream = New TInputTextStream
		this.text = text
		this.length = text.length
		Return this
	End Function
	
	Method Read:Int( buf:Byte Ptr, count:Int )
		If offset + count > length
			count = length - offset
		End If
		
		If count > 0
			For Local i:Int = 0 Until count
				buf[i] = text[offset + i]
			Next
			
			offset :+ count
		End If
		
		Return count
	End Method

	Method Size:Int()
		Return length
	End Method
	
	Method Pos:Int()
		Return offset
	End Method
	
End Type

Type TOutputTextStream Extends TStream

	Field text:String
	
	Method Write:Int( buf:Byte Ptr, count:Int )
		For Local i:Int = 0 Until count
'Print Chr(buf[i])
			text:+ Chr(buf[i])
		Next
		Return count
	End Method
	
End Type
