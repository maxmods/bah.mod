SuperStrict

Framework BaH.dbgp

Global dbgpHost:String
Global dbgpPort:Int

Global dbgpSession:String
Global sourcePath:String

Global blitzMaxArgs:String[]

Global manager:TBlitzMaxDebugManager

dbgpHost = AppArgs[1]
dbgpPort = AppArgs[2].ToInt()
dbgpSession = AppArgs[3]

' find source path...
getSourcePath(AppArgs)

blitzMaxArgs = AppArgs[4..]

start()

run()

End

Function start()

	Local ideProcessor:TBlitzMaxIDEDebugProcessor = New TBlitzMaxIDEDebugProcessor.Create(dbgpHost, dbgpPort, dbgpSession)
	
	Local processor:TBlitzMaxStdDebugProcessor = New TBlitzMaxStdDebugProcessor.Create(sourcePath, blitzMaxArgs)
	
	manager = New TBlitzMaxDebugManager.Create(processor, ideProcessor)

End Function

Function run()
	manager.initIDEProcessor()
	
	manager.run(sourcePath)
	
	finish()
End Function

Function finish()
	manager.destroy()
End Function

Function getSourcePath(args:String[])
	Local count:Int = 0
	
	For Local arg:String = EachIn args
		If arg = "-o" Then
			count:+ 1
			Continue
		End If
		
		If count > 0 Then
			If count = 2 Then
				sourcePath = arg
				Return
			End If
			
			count:+ 1
		End If
	Next
End Function


