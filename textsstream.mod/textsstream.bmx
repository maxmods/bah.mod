
SuperStrict

Rem
bbdoc: Text streams with large file support
about:
The Text Stream module allows you to load and save text in a number
of formats: LATIN1, UTF8 and UTF16.

The LATIN1 format uses a single byte to represent each character, and 
is therefore only capable of manipulating 256 distinct character values.

The UTF8 and UTF16 formats are capable of manipulating up to 1114112
character values, but will generally use greater storage space. In addition,
many text processing applications are unable to handle UTF8 and UTF16 files.
End Rem
Module BaH.TextSStream

ModuleInfo "Version: 1.00"
ModuleInfo "Author: Mark Sibly and Bruce A Henderson"
ModuleInfo "License: zlib/libpng"
ModuleInfo "Copyright: Blitz Research Ltd"
ModuleInfo "Copyright: 2013 Bruce A Henderson"

ModuleInfo "History: 1.00 Release"
ModuleInfo "History: Initial port from BRL.TextStream 1.03"

Import BaH.SStream

Type TTextSStream Extends TSStreamWrapper

	'***** PUBLIC *****

	Const LATIN1:Int=1
	Const UTF8:Int=2
	Const UTF16BE:Int=3
	Const UTF16LE:Int=4

	Method Read:Long( buf:Byte Ptr,count:Long )
		For Local i:Long = 0 Until count
			If _bufcount=32 _FlushRead
			Local hi:Int=_ReadByte()
			Local lo:Int=_ReadByte()
			hi:-48;If hi>9 hi:-7
			lo:-48;If lo>9 lo:-7
			buf[i]=hi Shl 4 | lo
			_bufcount:+1
		Next
		Return count
	End Method
	
	Method Write:Long( buf:Byte Ptr,count:Long )
		For Local i:Long=0 Until count
			Local hi:Int=buf[i] Shr 4
			Local lo:Int=buf[i] & $f
			hi:+48;If hi>57 hi:+7
			lo:+48;If lo>57 lo:+7
			_WriteByte hi
			_WriteByte lo
			_bufcount:+1
			If _bufcount=32 _FlushWrite
		Next
		Return count
	End Method
	
	Method ReadByte:Int()
		_FlushRead
		Return Int( ReadLine() )
	End Method
	
	Method WriteByte( n:Int )
		_FlushWrite
		WriteLine n
	End Method
	
	Method ReadShort:Int()
		_FlushRead
		Return Int( ReadLine() )
	End Method
	
	Method WriteShort( n:Int )
		_FlushWrite
		WriteLine n
	End Method
	
	Method ReadInt:Int()
		_FlushRead
		Return Int( ReadLine() )
	End Method
	
	Method WriteInt( n:Int )
		_FlushWrite
		WriteLine n
	End Method
	
	Method ReadLong:Long()
		_FlushRead
		Return Long( ReadLine() )
	End Method
	
	Method WriteLong( n:Long )
		_FlushWrite
		WriteLine n
	End Method
	
	Method ReadFloat:Float()
		_FlushRead
		Return Float( ReadLine() )
	End Method
	
	Method WriteFloat( n:Float )
		_FlushWrite
		WriteLine n
	End Method
	
	Method ReadDouble:Double()
		_FlushRead
		Return Double( ReadLine() )
	End Method
	
	Method WriteDouble( n:Double )
		_FlushWrite
		WriteLine n
	End Method
	
	Method ReadLine$()
		_FlushRead
		Local buf:Short[1024],i:Int
		While Not Eof()
			Local n:Int=ReadChar()
			If n=0 Exit
			If n=10 Exit
			If n=13 Continue
			If buf.length=i buf=buf[..i+1024]
			buf[i]=n
			i:+1
		Wend
		Return String.FromShorts(buf,i)
	End Method
	
	Method ReadFile$()
		_FlushRead
		Local buf:Short[1024],i:Int
		While Not Eof()
			Local n:Int=ReadChar()
			If buf.length=i buf=buf[..i+1024]
			buf[i]=n
			i:+1
		Wend
		Return String.FromShorts( buf,i )
	End Method
	
	Method WriteLine( str$ )
		_FlushWrite
		WriteString str
		WriteString "~r~n"
	End Method
	
	Method ReadString$( length:Int )
		_FlushRead
		Local buf:Short[length]
		For Local i:Int=0 Until length
			buf[i]=ReadChar()
		Next
		Return String.FromShorts(buf,length)
	End Method
	
	Method WriteString( str$ )
		_FlushWrite
		For Local i:Int=0 Until str.length
			WriteChar str[i]
		Next
	End Method
	
	Method ReadChar:Int()
		Local c:Int=_ReadByte()
		Select _encoding
		Case LATIN1
			Return c
		Case UTF8
			If c<128 Return c
			Local d:Int=_ReadByte()
			If c<224 Return (c-192)*64+(d-128)
			Local e:Int=_ReadByte()
			If c<240 Return (c-224)*4096+(d-128)*64+(e-128)
		Case UTF16BE
			Local d:Int=_ReadByte()
			Return c Shl 8 | d
		Case UTF16LE
			Local d:Int=_ReadByte()
			Return d Shl 8 | c
		End Select
	End Method
	
	Method WriteChar( char:Int )
		Assert char>=0 And char<=$ffff
		Select _encoding
		Case LATIN1
			_WriteByte char
		Case UTF8
			If char<128
				_WriteByte char
			Else If char<2048
				_WriteByte char/64 | 192
				_WriteByte char Mod 64 | 128
			Else
				_WriteByte char/4096 | 224
				_WriteByte char/64 Mod 64 | 128
				_WriteByte char Mod 64 | 128
			EndIf
		Case UTF16BE
			_WriteByte char Shr 8
			_WriteByte char
		Case UTF16LE
			_WriteByte char
			_WriteByte char Shr 8
		End Select
	End Method

	Function Create:TTextSStream( stream:TSStream,encoding:Int )
		Local t:TTextSStream=New TTextSStream
		t._encoding=encoding
		t.SetStream stream
		Return t
	End Function

	'***** PRIVATE *****
	
	Method _ReadByte:Int()
		Return Super.ReadByte()
	End Method
	
	Method _WriteByte( n:Int )
		Super.WriteByte n
	End Method
	
	Method _FlushRead()
		If Not _bufcount Return
		Local n:Int=_ReadByte()
		If n=13 n=_ReadByte()
		If n<>10 Throw "Malformed line terminator"
		_bufcount=0
	End Method
	
	Method _FlushWrite()
		If Not _bufcount Return
		_WriteByte 13
		_WriteByte 10
		_bufcount=0
	End Method
	
	Field _encoding:Int,_bufcount:Int
	
End Type
	
Type TTextSStreamFactory Extends TSStreamFactory

	Method CreateStream:TSStream( url:Object,proto$,path$,readable:Int,writeable:Int )
		Local encoding:Int
		Select proto$
		Case "latin1"
			encoding=TTextSStream.LATIN1
		Case "utf8"
			encoding=TTextSStream.UTF8
		Case "utf16be"
			encoding=TTextSStream.UTF16BE
		Case "utf16le"
			encoding=TTextSStream.UTF16LE
		End Select
		If Not encoding Return Null
		Local stream:TSStream=OpenStream( path,readable,writeable )
		If stream Return TTextSStream.Create( stream,encoding )
	End Method
End Type

New TTextSStreamFactory

Rem
bbdoc: Load text from a stream
returns: A string containing the text
about:
#LoadText loads LATIN1, UTF8 or UTF16 text from @url.

The first bytes read from the stream control the format of the text:
[ &$fe $ff | Text is big endian UTF16
* &$ff $fe | Text is little endian UTF16
* &$ef $bb $bf | Text is UTF8
]

If the first bytes don't match any of the above values, the stream
is assumed to contain LATIN1 text.

A #TStreamReadException is thrown if not all bytes could be read.
End Rem
Function LoadText$( url:Object )

	Local stream:TSStream=ReadStream( url )
	If Not stream Throw New TSStreamReadException

	Local format:Int,size:Int,c:Int,d:Int,e:Int

	If Not stream.Eof()
		c=stream.ReadByte()
		size:+1
		If Not stream.Eof()
			d=stream.ReadByte()
			size:+1
			If c=$fe And d=$ff
				format=TTextSStream.UTF16BE
			Else If c=$ff And d=$fe
				format=TTextSStream.UTF16LE
			Else If c=$ef And d=$bb
				If Not stream.Eof()
					e=stream.ReadByte()
					size:+1
					If e=$bf format=TTextSStream.UTF8
				EndIf
			EndIf
		EndIf
	EndIf

	If Not format
		Local data:Byte[1024]
		data[0]=c;data[1]=d;data[2]=e
		While Not stream.Eof()
			If size=data.length data=data[..size*2]
			size:+stream.Read( (Byte Ptr data)+size,data.length-size )
		Wend
		stream.Close
		Return String.FromBytes( data,size )
	EndIf
	
	Local TSStream:TTextSStream=TTextSStream.Create( stream,format )
	Local str$=TSStream.ReadFile()
	TSStream.Close
	stream.Close
	Return str

End Function

Rem
bbdoc: Save text to a stream
about:
#SaveText saves the characters in @str to @url.

If @str contains any characters with a character code greater than 255,
then @str is saved in UTF16 format. Otherwise, @str is saved in LATIN1 format.

A #TStreamWriteException is thrown if not all bytes could be written.
End Rem
Function SaveText:Int( str$,url:Object )

	Local format:Int
	For Local i:Int=0 Until str.length
		If str[i]>255
?BigEndian
			format=TTextSStream.UTF16BE
?LittleEndian
			format=TTextSStream.UTF16LE
?
			Exit
		EndIf
	Next
	
	If Not format
		SaveString str,url
		Return True
	EndIf

	Local stream:TSStream=WriteStream( url )
	If Not stream Throw New TSStreamWriteException
	
	Select format
	Case TTextSStream.UTF8
		stream.WriteByte $ef
		stream.WriteByte $bb
	Case TTextSStream.UTF16BE
		stream.WriteByte $fe
		stream.WriteByte $ff
	Case TTextSStream.UTF16LE
		stream.WriteByte $ff
		stream.WriteByte $fe
	End Select
	
	Local TSStream:TTextSStream=TTextSStream.Create( stream,format )
	TSStream.WriteString str
	TSStream.Close
	stream.Close
	Return True

End Function

