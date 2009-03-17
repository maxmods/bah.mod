SuperStrict

Framework BaH.Interprocess
Import BRL.StandardIO
Import BRL.Random

Import "appendstream.bmx"

Try

	' Create a named mutex object.
	Local mutex:TNamedMutex = New TNamedMutex.Create(OPEN_OR_CREATE, "stream_named_mutex")

	For Local i:Int = 0 Until 100
	
		' do some operations...
		
		' write to file automically
		Local lock:TScopedLock = New TScopedLock.Create(mutex)
		
		Local stream:TStream = WriteStream("append::file.txt")
		stream.WriteLine("Process name, This is iteration B#" + i)
		stream.Close()
		
		lock.Free()
		
		Delay(Rand(10, 200))
	
	Next
	
	TNamedMutex.Remove("stream_named_mutex")

Catch e:TInterprocessException
	TNamedMutex.Remove("stream_named_mutex")
	Print "Unexpected exception: " + e.ToString()
End Try


