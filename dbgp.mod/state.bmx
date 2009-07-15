SuperStrict

Rem
bbdoc: The debugger state.
End Rem
Type DebugState

	Rem
	bbdoc: State prior to execution of any code.
	End Rem
	Const STARTING:Int = 0
	Rem
	bbdoc: State after completion of code execution.
	about: This typically happens at the end of code execution, allowing the IDE to further interact with the debugger
	 engine (for example, to collect performance data, or use other extended commands).
	End Rem
	Const STOPPING:Int = 1
	Rem
	bbdoc: IDE is detached from process, no further interaction is possible.
	end rem
	Const STOPPED:Int = 2
	Rem
	bbdoc: Code is currently executing.
	about: Note that this state would only be seen with async support turned on, otherwise the typical state during IDE/debugger
	 interaction would be 'break'.
	End Rem
	Const RUNNING:Int = 3
	Rem
	bbdoc: Code execution is paused, for whatever reason.
	about: The IDE/debugger can pass information back and forth.
	End Rem
	Const BREAK:Int = 4
	
	Rem
	bbdoc: Map a DebugState to a String.
	about:  @param state
	 @return a string representation of the state.
	End Rem
	Function map:String(state:Int)
	
		Select state
			Case BREAK
				Return "break"
			Case RUNNING
				Return "running"
			Case STARTING
				Return "starting"
			Case STOPPED
				Return "stopped"
			Case STOPPING
				Return "stopping"
		End Select
		
		Return Null
	
	End Function

End Type

