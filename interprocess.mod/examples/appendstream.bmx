SuperStrict

Import BRL.Stream

Type TAppendableCStream Extends TCStream

	Function OpenFile:TCStream( path$, readable:Int, writeable:Int )
		Local mode$,_mode:Int
		If readable And writeable
			mode="a+b"
			_mode=MODE_READ|MODE_WRITE
		Else If writeable
			mode="ab"
			_mode=MODE_WRITE
		Else
			mode="rb"
			_mode=MODE_READ
		EndIf
		path=path.Replace( "\","/" )
		Local cstream:Int = fopen_( path,mode )
?Linux
		If (Not cstream) And (Not writeable)
			path=CasedFileName(path)
			If path cstream=fopen_( path,mode )
		EndIf
?
		If cstream Return CreateWithCStream( cstream,_mode )
	End Function

End Type

Type TAppendableCStreamFactory Extends TStreamFactory

	Method CreateStream:TStream( url:Object,proto$,path$,readable:Int, writeable:Int )
		If proto="append"
			Return TAppendableCStream.OpenFile( path,readable,writeable )
		EndIf
	End Method

End Type

New TAppendableCStreamFactory

