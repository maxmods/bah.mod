SuperStrict

Type TBlitzMaxBreakpointHandler

	Field debugStopped:Int = False
	
	Method setDebugStopped()
		debugStopped = True
	End Method
	
	Method hasDebugStopped:Int()
		If debugStopped Then
			debugStopped = False
			Return True
		End If
		
		Return False
	End Method

End Type

