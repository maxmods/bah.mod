SuperStrict

Import BRL.Socket
Import BRL.SocketStream
Import BRL.Stream
Import BRL.LinkedList

Import "databuffer.bmx"
Import "response.bmx"
Import "commands.bmx"
Import "common.bmx"


Type TBlitzMaxIDEDebugProcessor

	Field ideHost:String
	Field idePort:Int
	Field session:String
	
	Field socket:TSocket
	
	Field inout:TStream
	'Field out:TStream
	
	Field response:TDdbgpReponse

	Field commands:TList
	Field inputBuffer:TDataBuffer = New TDataBuffer
	
	
	Method Create:TBlitzMaxIDEDebugProcessor(ideHost:String, idePort:Int, session:String)
		Self.ideHost = ideHost
		Self.idePort = idePort
		Self.session = session
		Return Self
	End Method
	
	Method connect:Int()
		response = New TDdbgpReponse.Create()
		
		Local success:Int = True
		
		socket = TSocket.CreateTCP()
		socket.Connect(HostIp(ideHost), idePort)
		
		inout = TSocketStream.Create(socket)
		
		
		Return success
		
	End Method
	
	Method sendResponse(xml:String)
		DebugLog "Sending response : " + xml
		
		Local bytes:Byte[] = StringToByteArray(xml)
		Local length:Int = bytes.length
		
		'length
		inout.WriteString(length)
		inout.WriteByte(0)
		
		'response
		inout.WriteBytes(bytes, length)
		inout.WriteByte(0)
		
		inout.Flush()
		
	End Method
	
	Method monitor(needCommand:Int)
		Local inBuffer:Byte[1024]
		
		Local inBytes:Int = socket.ReadAvail()
		
		If inBytes > 0 Or needCommand Then
			Local bytesRead:Int = 0

			Local c:Int = inout.ReadByte()
			
			While c <> 0
				If c = -1 Then
					Exit
				End If
				
				inBuffer[bytesRead] = c
				bytesRead:+ 1
				
				c = inout.ReadByte()
			Wend
			
			inputBuffer.addNullBasedData(inBuffer, bytesRead)
			
			DebugLog "Receiving command data... : " + String.FromBytes(inBuffer, bytesRead)
			
			While inputBuffer.lineAvail()
			
				commands.addLast(TDbgpCommand.parseCommand(inputBuffer.readBytes()))
			
			Wend
			
		End If
		
	End Method
	
	
	Rem
	bbdoc: Send init response.
	End Rem
	Method init(file:String)
		sendResponse(response.init(file, session))
	End Method
	
	Rem
	bbdoc: Send status response.
	End Rem
	Method status(id:String, state:Int, reason:String)
		sendResponse(response.status(id, state, reason))
	End Method

	Method stream(_type:String, buf:Byte[])
		sendResponse(response.stream(_type, buf))
	End Method
	
End Type


Type TBlitzMaxStreamRedirect

	Const STDERR:Int = 0
	Const STDIN:Int = 1
	Const STDOUT:Int = 2
	
	Field processor:TBlitzMaxIDEDebugProcessor
	Field _type:Int

	Method Create:TBlitzMaxStreamRedirect(processor:TBlitzMaxIDEDebugProcessor, _type:Int)
		Self.processor = processor
		Self._type = _type
		Return Self
	End Method
	
	Method println(line:String)
		printBytes(StringToByteArray(line))
	End Method
	
	Method printBytes(buf:Byte[])
		processor.stream(getType(), buf)
	End Method
	
	Method getType:String()
		Select _type
			Case STDERR
				Return "stderr"
			Case STDIN
				Return "stdin"
			Case STDOUT
				Return "stdout"
		End Select
	End Method


End Type
