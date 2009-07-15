SuperStrict

Import BRL.Map

Type CommandType

	Const UNKOWN:Int = 0
	Const STATUS:Int = 1
	Const FEATURE_GET:Int = 2
	Const FEATURE_SET:Int = 3
	Const RUN:Int = 4
	Const STEP_INTO:Int = 5
	Const STEP_OUT:Int = 6
	Const STEP_OVER:Int = 7
	Const STOP:Int = 8
	Const DETACH:Int = 9
	Const STACK_DEPTH:Int = 10
	Const STACK_GET:Int = 11
	Const PROP_SET:Int = 12
	Const PROP_GET:Int = 13
	Const PROP_VALUE:Int = 14
	Const CONTEXT_GET:Int = 15
	Const CONTEXT_NAMES:Int = 16
	Const EVAL:Int = 17
	Const STDOUT:Int = 18
	Const STDERR:Int = 19
	Const BREAK:Int = 20
	Const STDIN:Int = 21


	Function fromString:Int(cmd:String)
		If "status" = cmd Then
			Return STATUS
		Else If "feature_get" = cmd Then
			Return FEATURE_GET
		Else If "feature_set" = cmd Then
			Return FEATURE_SET
		Else If "run" = cmd Then
			Return RUN
		Else If "step_into" = cmd Then
			Return STEP_INTO
		Else If "step_out" = cmd Then
			Return STEP_OUT
		Else If "step_over" = cmd Then
			Return STEP_OVER
		Else If "stop" = cmd Then
			Return STOP
		Else If "detach" = cmd Then
			Return DETACH
		Else If "stack-depth" = cmd Then
			Return STACK_DEPTH
		Else If "stack_get" = cmd Then
			Return STACK_GET
		Else If "property_set" = cmd Then
			Return PROP_SET
		Else If "property_get" = cmd Then
			Return PROP_GET
		Else If "property_value" = cmd Then
			Return PROP_VALUE
		Else If "context_get" = cmd Then
			Return CONTEXT_GET
		Else If "context_names" = cmd Then
			Return CONTEXT_NAMES
		Else If "eval" = cmd Then
			Return EVAL
		Else If "stdout" = cmd Then
			Return STDOUT
		Else If "stderr" = cmd Then
			Return STDERR
		Else If "stdin" = cmd Then
			Return STDIN
		Else If "break" = cmd Then
			Return BREAK
		End If
		
		Return UNKOWN
	End Function
	
End Type


Rem
bbdoc: A dbgp command from the IDE
End Rem
Type TCommand Abstract

	Field commands:TMap
	
	Method init(cmdParts:String[])
		commands = New TMap
		
		Local i:Int = 1
		Local count:Int = cmdParts.length
		
		While i < count
			Local attr:String = cmdParts[i]
			i:+ 1
			Local value:String = cmdParts[i]
			i:+ 1
		
			commands.Insert(attr, value)
		Wend
		
	End Method
	
	Method getTransactionId:String()
		Return String(commands.ValueForKey("-i"))
	End Method

	Method getType:Int() Abstract

End Type

Type TContextGetCommand Extends TCommand

	Method Create:TContextGetCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method
	
	Method getType:Int()
		Return CommandType.CONTEXT_GET
	End Method
	
	Method getDepth:Int()
		Local depth:String = String(commands.ValueForKey("-d"))
		If Not depth Then
			Return -1
		Else
			Return depth.ToInt()
		End If
	End Method

	Method getContextId:Int()
		Local id:String = String(commands.ValueForKey("-c"))
		If Not id Then
			Return 0
		Else
			Return id.ToInt()
		End If
	End Method
	
End Type

Type TBreakCommand Extends TCommand

	Method Create:TBreakCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.BREAK
	End Method	

End Type

Type TContextNamesCommand Extends TCommand

	Method Create:TContextNamesCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.CONTEXT_NAMES
	End Method	

	Method getDepth:Int()
		Local depth:String = String(commands.ValueForKey("-d"))
		If Not depth Then
			Return -1
		Else
			Return depth.ToInt()
		End If
	End Method

End Type

Type TDetachCommand Extends TCommand

	Method Create:TDetachCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.DETACH
	End Method	

End Type

Type TEvalCommand Extends TCommand

	Method Create:TEvalCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.EVAL
	End Method	

End Type

Type TFeatureGetCommand Extends TCommand

	Method Create:TFeatureGetCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.FEATURE_GET
	End Method	

	Method getName:String()
		Return String(commands.ValueForKey("-n"))
	End Method
	
End Type

Type TFeatureSetCommand Extends TCommand

	Method Create:TFeatureSetCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.FEATURE_SET
	End Method	

	Method getName:String()
		Return String(commands.ValueForKey("-n"))
	End Method
	
	Method getValue:String()
		Return String(commands.ValueForKey("-v"))
	End Method
	
End Type

Type TPropGetCommand Extends TCommand

	Method Create:TPropGetCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.PROP_GET
	End Method	

End Type

Type TPropSetCommand Extends TCommand

	Method Create:TPropSetCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.PROP_SET
	End Method	

End Type

Type TPropValueCommand Extends TCommand

	Method Create:TPropValueCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.PROP_VALUE
	End Method	

End Type

Type TRunCommand Extends TCommand

	Method Create:TRunCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.RUN
	End Method	

End Type

Type TStackDepthCommand Extends TCommand

	Method Create:TStackDepthCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.STACK_DEPTH
	End Method	

End Type

Type TStackGetCommand Extends TCommand

	Method Create:TStackGetCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.STACK_GET
	End Method	

	Method getDepth:Int()
		Local depth:String = String(commands.ValueForKey("-d"))
		If Not depth Then
			Return -1
		Else
			Return depth.ToInt()
		End If
	End Method

End Type

Type TStatusCommand Extends TCommand

	Method Create:TStatusCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.STATUS
	End Method	

End Type

Type TStderrCommand Extends TCommand

	Method Create:TStderrCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.STDERR
	End Method	

End Type

Type TStdinCommand Extends TCommand

	Method Create:TStdinCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.STDIN
	End Method	

End Type

Type TStdoutCommand Extends TCommand

	Method Create:TStdoutCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.STDOUT
	End Method	

End Type

Type TStepIntoCommand Extends TCommand

	Method Create:TStepIntoCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.STEP_INTO
	End Method	

End Type

Type TStepOutCommand Extends TCommand

	Method Create:TStepOutCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.STEP_OUT
	End Method	

End Type

Type TStepOverCommand Extends TCommand

	Method Create:TStepOverCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.STEP_OVER
	End Method	

End Type

Type TStopCommand Extends TCommand

	Method Create:TStopCommand(cmdParts:String[])
		init(cmdParts)
		Return Self
	End Method

	Method getType:Int()
		Return CommandType.STOP
	End Method	

End Type


Type TDbgpCommand

	Function parseCommand:TCommand(command:Byte[])
		Local cmd:String = String.FromBytes(command, command.length)
		Local cmdParts:String[] = cmd.split(" ")
		
		Local _type:Int = CommandType.fromString(cmdParts[0])
		
		Select _type
			Case CommandType.STATUS
				 Return New TStatusCommand.Create(cmdParts)
			Case CommandType.FEATURE_GET
				 Return New TFeatureGetCommand.Create(cmdParts)
			Case CommandType.FEATURE_SET
				 Return New TFeatureSetCommand.Create(cmdParts)
			Case CommandType.RUN
				 Return New TRunCommand.Create(cmdParts)
			Case CommandType.STOP
				 Return New TStopCommand.Create(cmdParts)
			Case CommandType.STEP_OVER
				 Return New TStepOverCommand.Create(cmdParts)
			Case CommandType.STEP_INTO
				 Return New TStepIntoCommand.Create(cmdParts)
			Case CommandType.STEP_OUT
				 Return New TStepOutCommand.Create(cmdParts)
			Case CommandType.CONTEXT_GET
				 Return New TContextGetCommand.Create(cmdParts)
			Case CommandType.CONTEXT_NAMES
				 Return New TContextNamesCommand.Create(cmdParts)
			Case CommandType.PROP_GET
				 Return New TPropGetCommand.Create(cmdParts)
			Case CommandType.PROP_SET
				 Return New TPropSetCommand.Create(cmdParts)
			Case CommandType.PROP_VALUE
				 Return New TPropValueCommand.Create(cmdParts)
			Case CommandType.STACK_DEPTH
				 Return New TStackDepthCommand.Create(cmdParts)
			Case CommandType.STACK_GET
				 Return New TStackGetCommand.Create(cmdParts)
			Case CommandType.BREAK
				 Return New TBreakCommand.Create(cmdParts)
			Case CommandType.DETACH
				 Return New TDetachCommand.Create(cmdParts)
			Case CommandType.EVAL
				 Return New TEvalCommand.Create(cmdParts)
			Case CommandType.STDERR
				 Return New TStderrCommand.Create(cmdParts)
			Case CommandType.STDIN
				 Return New TStdinCommand.Create(cmdParts)
			Case CommandType.STDOUT
				 Return New TStdoutCommand.Create(cmdParts)
		End Select
			
		Return Null
	
	End Function
	

End Type


