
Strict

Rem
bbdoc: Streams/Ram streams
End Rem
Module BaH.RamSStream

ModuleInfo "Version: 1.00"
ModuleInfo "Author: Mark Sibly and Bruce A Henderson"
ModuleInfo "License: zlib/libpng"
ModuleInfo "Copyright: Blitz Research Ltd"

ModuleInfo "History: 1.00 Release"
ModuleInfo "History: Initial port from BRL.RamStream 1.01"

Import BaH.SStream

Type TRamSStream Extends TSStream

	Field _pos:Long,_size:Long,_buf:Byte Ptr,_read:Int,_write:Int

	Method Pos:Long()
		Return _pos
	End Method

	Method Size:Long()
		Return _size
	End Method

	Method Seek:Long( pos:Long )
		If pos<0 pos=0 Else If pos>_size pos=_size
		_pos=pos
		Return _pos
	End Method

	Method Read:Long( buf:Byte Ptr,count:Long )
		If count<=0 Or _read=False Return 0
		If _pos+count>_size count=_size-_pos
		MemCopy buf,_buf+_pos,count
		_pos:+count
		Return count
	End Method

	Method Write:Long( buf:Byte Ptr,count:Long )
		If count<=0 Or _write=False Return 0
		If _pos+count>_size count=_size-_pos
		MemCopy _buf+_pos,buf,count
		_pos:+count
		Return count
	End Method

	Function Create:TRamSStream( buf:Byte Ptr,size:Long,readable:Int,writeable:Int )
		Local stream:TRamSStream=New TRamSStream
		stream._pos=0
		stream._size=size
		stream._buf=buf
		stream._read=readable
		stream._write=writeable
		Return stream
	End Function

End Type

Rem
bbdoc: Create a ram stream
returns: A ram stream object
about: A ram stream allows you to read and/or write data directly from/to memory.
A ram stream extends a stream object so can be used anywhere a stream is expected.

Be careful when working with ram streams, as any attempt to access memory
which has not been allocated to your application can result in a runtime crash.
End Rem
Function CreateRamSStream:TRamSStream( ram:Byte Ptr,size:Long,readable:Int,writeable:Int )
	Return TRamSStream.Create( ram,size,readable,writeable )
End Function

Type TRamSStreamFactory Extends TSStreamFactory
	Method CreateStream:TRamSStream( url:Object,proto$,path$,readable:Int,writeable:Int )
		If proto="incbin" And writeable=False
			Local buf:Byte Ptr=IncbinPtr( path )
			If Not buf Return
			Local size:Int=IncbinLen( path )
			Return TRamSStream.Create( buf,size,readable,writeable )
		EndIf
	End Method
End Type

New TRamSStreamFactory
