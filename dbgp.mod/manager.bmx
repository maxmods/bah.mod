SuperStrict

Import "stdprocessor.bmx"
Import "features.bmx"
Import "state.bmx"


Type TBlitzMaxDebugManager

	Field processor:TBlitzMaxStdDebugProcessor
	Field ideProcessor:TBlitzMaxIDEDebugProcessor
	
	Field terminated:Int = False
	Field needCommand:Int = False
	Field transactionId:String
	
	Field state:Int = DebugState.STARTING
	
	Const MODE_WAITING:Int = 0
	Const MODE_RUNNING:Int = 1
	Const MODE_RUNWAIT:Int = 2

	Field Mode:Int = MODE_WAITING
	Field features:TBlitzMaxFeatures

	Method Create:TBlitzMaxDebugManager(processor:TBlitzMaxStdDebugProcessor, ideProcessor:TBlitzMaxIDEDebugProcessor)
		Self.processor = processor;
		Self.ideProcessor = ideProcessor
		
		features = New TBlitzMaxFeatures
		
		Return Self
	End Method

	Method initIDEProcessor()
		If Not ideProcessor.connect() Then
			' error... need to quit
			Throw "Failed to connect to IDE"
		End If
	End Method
	
	Method initDebugProcessor()
		processor.start()
	End Method
	
	Method run(file:String)

		' initialise IDE connection
		ideProcessor.init(file)
		needCommand = True
		
		While Not terminated And (state <> DebugState.STOPPED)
		
			' check the IDE commands.
			checkForCommands()
			
			If Mode > MODE_WAITING Then
			
				' check the app buffers
				terminated = processor.monitor()
				
				checkForBreak()
			End If
			
			' sleep a little
			Delay(10)
		Wend
		
	End Method
	
	Method checkForBreak()
		' we are currently stopped? (perhaps due to DebugStop, or step, etc.
		If processor.getBreakpointHandler().hasDebugStopped() Then
			state = DebugState.BREAK
			ideProcessor.breakpoint(transactionId)
		End If
	End Method
	
	Method checkForCommands()
	
		ideProcessor.monitor(needCommand)
		
		needCommand = False
		
		While Not ideProcessor.getCommands().isEmpty()
		
			Local command:TCommand = TCommand(ideProcessor.getCommands().removeFirst())
			
			If command Then
			
				transactionId = command.getTransactionId()
				
				Select command.getType()
					Case CommandType.FEATURE_SET
						Local name:String = TFeatureSetCommand(command).getName()
						Local value:String = TFeatureSetCommand(command).getValue()
						Local supported:Int = features.isSupported(name)
						Local success:Int = False
	
						If supported Then
							success = features.setValue(name, value)
						End If
	
						ideProcessor.featureSet(transactionId, name, success)
	
	
					Case CommandType.FEATURE_GET
						Local name:String = TFeatureGetCommand(command).getName()
						Local supported:Int = features.isSupported(name)
	
						ideProcessor.featureGet(transactionId, name, supported, features.getValue(name))
	
	
					Case CommandType.STATUS
						' TODO	implement me
	
					Case CommandType.BREAK
						' TODO	implement me
	
					Case CommandType.CONTEXT_GET
	
						Local depth:Int = TContextGetCommand(command).getDepth()
	
						Local stack:TList = processor.stackGet(depth)
	
						ideProcessor.contextGet(transactionId, stack, depth, TContextGetCommand(command).getContextId())
	
	
					Case CommandType.CONTEXT_NAMES
	
						Local stack:TList = processor.stackGet(TContextNamesCommand(command).getDepth())
	
						ideProcessor.contextNames(transactionId, stack)
	
	
					Case CommandType.DETACH
						' TODO	implement me
	
					Case CommandType.EVAL
						' TODO	implement me
	
					Case CommandType.PROP_GET
						' TODO	implement me
	
					Case CommandType.PROP_SET
						' TODO	implement me
	
					Case CommandType.PROP_VALUE
						' TODO	implement me
	
					Case CommandType.RUN
	
						Local success:Int = True
	
						Select state
							Case DebugState.STARTING
								success = processor.start()
		
			
							Case DebugState.BREAK
								processor.run()
			
						End Select
	
						' ideProcessor.run(transactionId, state, success) - spec says doesn't expect response yet.
	
						If success Then
							state = DebugState.RUNNING
							Mode = MODE_RUNNING
						End If
	
	
					Case CommandType.STACK_DEPTH
						' TODO	implement me
	
					Case CommandType.STDIN
						' TODO set something and redirect
						Local success:Int = True
						ideProcessor.stdin(transactionId, success)
	
	
					Case CommandType.STDERR
						Local success:Int = True
	
						processor.setStderrRedirect(ideProcessor.getStderrRedirect())
	
						ideProcessor.stderr(transactionId, success)
	
	
					Case CommandType.STDOUT
						Local success:Int = True
	
						processor.setStdoutRedirect(ideProcessor.getStdoutRedirect())
	
						ideProcessor.stdout(transactionId, success)
	
	
					Case CommandType.STACK_GET
						Local stack:TList = processor.stackGet(TStackGetCommand(command).getDepth())
	
						ideProcessor.stackGet(transactionId, stack)
	
	
					Case CommandType.STEP_INTO
	
						processor.stepIn()
	
	
					Case CommandType.STEP_OUT
	
						processor.stepOut()
	
	
					Case CommandType.STEP_OVER
	
						processor.stepOver()
	
	
					Case CommandType.STOP
						processor.shutdown()
	
						Local state:Int = DebugState.STOPPING
	
						ideProcessor.stop(transactionId, state)
	
						state = DebugState.STOPPED
				End Select
			Else
				
			End If
			
		Wend
		
		
	End Method

	Method destroy()
		' have we been asked to close?
		If Not terminated Then
			processor.shutdown()
		Else
			' or has our session been terminated?
			ideProcessor.stop(transactionId, DebugState.STOPPED)
		End If
	End Method
	
End Type
