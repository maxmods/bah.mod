SuperStrict

Framework BaH.Interprocess
Import BRL.StandardIO

Try

	' Open already created shared memory object.
	Local shm:TSHMO = New TSHMO.Create(TSHMO.OPEN_ONLY, "shared_memory", MODE_RO)
	
	' Map the whole shared memory in this process
	Local region:TMappedRegion = New TMappedRegion.Create(shm, MODE_RO)
	
	Local mem:Byte Ptr = region.GetAddress()
	
	' Check that memory was initialized To 1
	For Local i:Int = 0 Until region.GetSize()
		If mem[i] <> 1 Then
			Print "Error checking memory!"
			End
		End If
	Next
	
	Print "Test successful!"

Catch e:TInterprocessException
	Print "Unexpected exception: " + e.ToString()
End Try

TSHMO.Remove("shared_memory")
