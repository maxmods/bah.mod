SuperStrict

Framework BaH.Interprocess
Import BRL.StandardIO

Try

	' Erase previous shared memory
	TSHMO.Remove("shared_memory")
	
	
	' Create a shared memory object.
	Local shm:TSHMO = New TSHMO.Create(CREATE_ONLY, "shared_memory", MODE_RW)
	
	' Set size
	shm.Truncate(1000)
	
	' Map the whole shared memory in this process
	Local region:TMappedRegion = New TMappedRegion.Create(shm, MODE_RW)
	
	Local mem:Byte Ptr = region.GetAddress()
	
	' Write all the memory to 1
	For Local i:Int = 0 Until region.GetSize()
		mem[i] = 1
	Next

Catch e:TInterprocessException
	Print "Unexpected exception: " + e.ToString()
End Try
