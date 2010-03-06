'
' RakVoice FMOD example
'
'
SuperStrict
Framework BaH.RaknetVoiceFMOD
Import "../../raknet.mod/examples/example_helper.bmx"

AppTitle = "RakVoice (FMOD)"


New FMOD.Run()



Type FMOD Extends TExampleHelper

	Const STAGE_NONE:Int = 0
	Const STAGE_GET_PORT:Int = 1
	Const STAGE_REMOTE_IP:Int = 2
	Const STAGE_REMOTE_PORT:Int = 3


	Const SAMPLE_RATE:Int = 8000  ' Speex also supports 16000 and 32000
	Const FRAMES_PER_BUFFER:Int = (2048 / (32000 / SAMPLE_RATE))
	Const SAMPLE_SIZE:Int = 2
	
	Field stage:Int = STAGE_NONE
	
	Field rakPeer:TRKRakPeerInterface = TRKRakNetworkFactory.GetRakPeerInterface()
	Field rakVoice:TRKRakVoice = New TRKRakVoice
	
	Field port:Int
	Field fmodSystem:TFMODSystem
	
	Field remoteIP:String
	Field remotePort:Int
	
	
	Method Init()
		Super.Init()

		fmodSystem = New TFMODSystem.Create()
		fmodSystem.Init(100, FMOD_INIT_NORMAL)
		

		AddMessage "(Q)uit. (C)onnect. (D)isconnect. (M)ute. ' ' for stats."
		AddMessage "(+/-)encoder complexity.  (N)oise filter on/off. (V)AD on/off. (B)vbr on/off."



		stage = STAGE_GET_PORT
		RequestValue("Enter local port", "Enter the local port to listen on : (60000)")
	End Method

	Method RenderExtra()
		Local s:String = "Complexity=" + rakVoice.GetEncoderComplexity() + "   Noise filter="

		If rakVoice.IsNoiseFilterActive() Then
			s:+ "ON "
		Else
			s:+ "OFF"
		End If
		
		s:+ " VAD="
		
		If rakVoice.IsVADActive() Then
			s:+ "ON "
		Else
			s:+ "OFF"
		End If
			
		
		s:+ " VBR="

		If rakVoice. IsVBRActive() Then
			s:+ "ON "
		Else
			s:+ "OFF"
		End If

		DrawText s, 100, 580		

	End Method

	Method Update()
		
		If stage = STAGE_NONE Then
		
			If KeyHit(KEY_Q) Then
				AddMessage "Quitting."
				quit = True
			End If
		
			
			If KeyHit(KEY_C) Then
			
				stage = STAGE_REMOTE_IP
				RequestValue("Enter remote IP", "Enter IP of remote system : (127.0.0.1)")
		
			End If
		
		End If
	
	
	


		Local p:TRKPacket = rakPeer.Receive()

		While p

			If p.GetData()[0] = ID_CONNECTION_REQUEST_ACCEPTED Then
				AddMessage "ID_CONNECTION_REQUEST_ACCEPTED from " + p.GetGuid().ToString()
				rakVoice.RequestVoiceChannel(p.GetGuid())

			Else If p.GetData()[0] = ID_RAKVOICE_OPEN_CHANNEL_REQUEST Then
				AddMessage "Open Channel request from " + p.GetGuid().ToString()

			Else If p.GetData()[0] = ID_RAKVOICE_OPEN_CHANNEL_REPLY Then
				AddMessage "Got new channel from " + p.GetGuid().ToString()
			End If

			rakPeer.DeallocatePacket(p)
			p = rakPeer.Receive()
		Wend

		fmodSystem.update()
		' Update Or connection with FMOD
		rakVoice.Update()
		'LogStats()
		Delay 20

	End Method

	Method Finalize()
		' Release any FMOD resources we used, And shutdown FMOD itself
'		rakVoice.Release()
'		fmodSystem.Release()

		rakPeer.Shutdown(300)
		TRKRakNetworkFactory.DestroyRakPeerInterface(rakPeer)
	End Method
	
	Method OnSubmit(value:String)
		Select stage
			Case STAGE_GET_PORT
				
				port = value.ToInt()
				If Not port Then
					port = 60000
				End If

				Local socketDescriptor:TRKSocketDescriptor = New TRKSocketDescriptor.Create(port)
		
				rakPeer.Startup(4, 30, socketDescriptor)
		
				rakPeer.SetMaximumIncomingConnections(4)
				rakPeer.AttachPlugin(rakVoice)
		
					
				rakVoice.Init(SAMPLE_RATE, FRAMES_PER_BUFFER * SAMPLE_SIZE, fmodSystem)
				
				stage = STAGE_NONE
				
			Case STAGE_REMOTE_IP
			
				remoteIP = value
				If Not remoteIP Then
					remoteIP = "127.0.0.1"
				End If
			
				stage = STAGE_REMOTE_PORT
				RequestValue("Enter remote port", "Enter port of remote system : (60000)")
			
			Case STAGE_REMOTE_PORT
			
				remotePort = value.ToInt()
				If Not remotePort Then
					remotePort = 60000
				End If
				
				rakPeer.Connect(remoteIP, remotePort)
			
				stage = STAGE_NONE
		End Select
		
		FlushKeys
	End Method
	
End Type
