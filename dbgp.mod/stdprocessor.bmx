SuperStrict

Import Pub.FreeProcess
Import BRL.StandardIO

Import "ideprocessor.bmx"
Import "stack.bmx"
Import "handler.bmx"


Type TBlitzMaxStdDebugProcessor

	Const MAX_BUFFER:Int = 2048

	Field sourcePath:String
	Field args:String[]
	
	Field inStack:Int = False
	Field stack:TList = New TList
	
	Field inScope:Int
	Field currentFile:String
	
	Field scope:TBlitzMaxStackScope
	Field objScope:TBlitzMaxObjectScope
	
	Field inVariable:Int
	Field inFile:Int
	
	Field blitzStdErr:TPipeStream
	Field blitzStdInOut:TPipeStream
	
	Field stderrRedirect:TBlitzMaxStreamRedirect
	Field stdoutRedirect:TBlitzMaxStreamRedirect
	
	Field breakpointHandler:TBlitzMaxBreakpointHandler
	
	Field inputBuffer:TDataBuffer = New TDataBuffer
	Field debugProcess:TProcess
	
	Field ready:Int
	
	Field bmxPath:String
	
	Field sizeRequests:TList = New TList
	
	Method Create:TBlitzMaxStdDebugProcessor(sourcePath:String, args:String[])
		Self.sourcePath = sourcePath
		Self.args = args
		
		breakpointHandler = New TBlitzMaxBreakpointHandler
		
		bmxPath = getenv_("BMXPATH")
	
		Return Self
	End Method
	
	Method start:Int()

		Local command:String = commandFromArgs()
	
		ChangeDir(ExtractDir(sourcePath)) 'set the working directory
		
		debugProcess = TProcess.Create(command, 1)
		
		blitzStdInOut = debugProcess.pipe
		blitzStdErr = debugProcess.err
	
		Return True
	End Method
	
	Method commandFromArgs:String()
		Local command:String
		
		For Local arg:String = EachIn args
			If arg.Find(" ") >= 0 Then
				command :+ "~q" + arg + "~q"
			Else
				command :+ arg
			End If
			
			command:+ " "
		Next
		
		Return command
	End Method
	
	Method reset:Int()
		' clear the buffers
		Local finished:Int = monitor()
		
		inVariable = False
		inFile = False
		stack.Clear()
		inStack = False
		scope = Null
		objScope = Null
		
		ready = False
		
		Return finished
	End Method

	Method monitor:Int()
	
		Local inBuffer:Byte[MAX_BUFFER]
		
		Local inBytes:Int = blitzStdInOut.ReadAvail()
		Local errBytes:Int = blitzStdErr.ReadAvail()
		
		' loop until there's nothing left in the buffers
		While inBytes > 0 Or errBytes > 0

			Local bytesRead:Int = 0
			
			If inBytes > 0 Then
			
				bytesRead = blitzStdInOut.ReadBytes(inBuffer, Min(Max(0, inBytes), MAX_BUFFER))
				
				If Not stdoutRedirect Then
					WriteStdout String.FromBytes(inBuffer, bytesRead)
				Else
					stdoutRedirect.println(String.FromBytes(inBuffer, bytesRead))
				End If
			
			End If
			
			If errBytes > 0 Then
			
				bytesRead = blitzStdErr.ReadBytes(inBuffer, Min(Max(0, errBytes), MAX_BUFFER))
				
				inputBuffer.addLineBasedData(inBuffer, bytesRead)
			
			End If
			
			While inputBuffer.lineAvail()
			
				Local line:String = inputBuffer.ReadLine()
				
				line = processLine(line)
				
				If line Then
				
					If Not stderrRedirect Then
						WriteStdout line
					Else
						stderrRedirect.println(line)
					End If
				
				End If
			
			
			Wend
		
			inBytes = blitzStdInOut.ReadAvail()
			errBytes = blitzStdErr.ReadAvail()
		
		Wend

		If Not debugProcess.Status() Then
			Return True
		End If
	
		Return False
	
	End Method
	
	Method processLine:String(line:String)

		Local pos:Int = 0
		Local length:Int = line.length
		
		While pos < length And line[pos] = ">"
			pos:+ 1
		Wend
		
		If pos = length Then
			Return Null
		End If
		
		pos :- 1
		
		If pos > 0
			line = line[pos..]
		End If
	
		' This is not a debug-command line? Return it
		If Not line.StartsWith("~~>") Then
			Return line
		End If

		line = line[2..]
		
		If inVariable Then
		
			If line = "}" Then
				inVariable = False
				ready = True
			Else
				objScope.addVariable(line)
			End If
		
			Return Null
		End If
		
		' we are processing the stack?
		If inStack Then
		
			' end of the stack?
			If line = "}" Then
				inStack = False
				inScope = False
				
				ready = True
				
				Return Null
			End If
			
			If inFile Then
			
				If line <> "Local <local>" Then
					scope = New TBlitzMaxStackScope.Create(line, stack.Count())
					stack.AddLast(scope)
					inScope = True
				End If
				
				If inScope Then
					If currentFile.StartsWith("$BMXPATH") Then
						currentFile = currentFile.Replace("$BMXPATH", bmxPath)
					End If
					
					scope.setSource(currentFile)
				End If
				
				currentFile = Null
				inFile = False
				Return Null
			End If
			
			If line.StartsWith("@") And line.Find("<") > 0 Then
				currentFile = line[1..]
				inFile = True
			Else
				If inScope Then
					checkForChildSizes(scope.addVariable(line))
				End If
			End If
		
			Return Null
		
		End If
		
		If line.StartsWith("Unhandled Exception:") Then
			' TODO: we should tell the IDE something bad happened.
			Return Null
		End If
		
		If line = "StackTrace{" Then
			inStack = True
			stack = New TList
			Return Null
		End If
		
		If line = "Debug:" Or line = "DebugStop:" Then
			' flag as stopped for IDE
			breakpointHandler.setDebugStopped()
			Return Null
		End If
		
		If line.StartsWith("ObjectDump@") Then
			If line.Find("{") > 0 Then
				inVariable = True
				objScope = New TBlitzMaxObjectScope.Create(line[1..])
			Else
				Return line
			End If
			
			Return Null
		End If
	
		Return Null
	End Method
	
	Method requestCurrentStack()
		sendCommand("t~n")
	End Method
	
	Method run()
		sendCommand("r~n")
	End Method
	
	Method stepIn()
		sendCommand("e~n")
	End Method
	
	Method stepOver()
		sendCommand("s~n")
	End Method
	
	Method stepOut()
		sendCommand("l~n")
	End Method
	
	Method quit()
		sendCommand("q~n")
	End Method
	
	Method dump(key:String)
		sendCommand("d" + key + "~n")
	End Method
	
	Method sendCommand(command:String)
		blitzStdInOut.WriteString(command)
		blitzStdInOut.Flush()
	End Method
	
	Method shutdown()
		Try
			quit()
		Catch e:Object
		End Try
		
		' hang around long enough for the quit to kick in...
		Delay(200)
		
		Try
			debugProcess.Terminate()
		Catch e:Object
		End Try
	End Method
	
	Method setStderrRedirect(redirect:TBlitzMaxStreamRedirect)
		stderrRedirect = redirect
	End Method
	
	Method setStdoutRedirect(redirect:TBlitzMaxStreamRedirect)
		stdoutRedirect = redirect
	End Method
	
	Method getBreakpointHandler:TBlitzMaxBreakpointHandler()
		Return breakpointHandler
	End Method

	Method stackGet:TList(depth:Int)
		reset()
		requestCurrentStack()
		
		' we expect something on the stack
		Local attempts:Int = 0
		Local loopCount:Int = 0
		
		While stack.Count() = 0
		
			' are we still getting data?
			While Not ready
			
				monitor()
				
				Delay(50)
			
			Wend
			
			Delay(50)
			
			If loopCount > 10 Then
				If attempts > 0 Then
					Exit
				End If
				attempts:+ 1
				
				loopCount = 0
				
				' let's try to get it again!
				reset()
				requestCurrentStack()
			End If
			
			loopCount:+ 1
		
		
		Wend
		
		If sizeRequests.Count() > 0 Then
			fixObjectChildren()
		End If
		
		renumberStack()
		
		If depth >= 0 Then
			Local list:TList = New TList
			
			For Local scope:TBlitzMaxStackScope = EachIn stack
				If depth = scope.getLevel() Then
					list.AddLast(scope)
				End If
			Next
			
			Return list
		Else
		
			Return stack
		
		End If
		
	End Method
	
	' stack level : 0 newest, n oldest
	Method renumberStack()
		If stack.Count() > 1 Then
			Local level:Int = stack.Count() - 1
			For Local scope:TBlitzMaxStackScope = EachIn stack
				scope.level = level
				level:- 1
			Next
		End If
	End Method
	
	Method checkForChildSizes:Int(variable:TBlitzMaxScopeVariable, requestIfMissing:Int = True)
		' do we need to look up the count?
		If variable.getBaseType() = BlitzMaxType._OBJECT And Not variable.isArray And variable.getChildrenCount() = 0 Then
			Local v:TIntValue = TIntValue(TBlitzMaxScopeVariable.typeSizeMap.ValueForKey(variable.getType().ToLower()))
			If Not v Then
				If requestIfMissing Then
					sizeRequests.AddLast(variable)
				Else
					Return True
				End If
			Else
				variable.setChildrenCount(v.value)
			End If
		End If
		
		Return False
	End Method

	Method fixObjectChildren()
	
		For Local variable:TBlitzMaxScopeVariable = EachIn sizeRequests
			' do we need to lookup for this type?
			If checkForChildSizes(variable, False) Then
			
				ready = False
				
				dump(variable.getKey())
				
				While Not ready
				
					monitor()
					
					Delay(50)
				
				Wend
				
				If objScope.getVariables() Then
					variable.setchildrenCount(objScope.getVariables().Count())
				End If
				
			End If
		Next
		
	End Method
	
	Method getObjectDetails:TBlitzMaxObjectScope(key:String)
		reset()
		dump(key)

		While Not ready
		
			monitor()
			
			Delay(50)
		
		Wend
		
		Local obj:TBlitzMaxObjectScope = objScope

		If sizeRequests.Count() > 0 Then
			fixObjectChildren()
		End If

		Return obj
	End Method
	
	
End Type

