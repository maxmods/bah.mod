'
' This example shows how simply play generated tones using TFMODSystem.PlayDSP instead of 
' manually connecting and disconnecting DSP units.
'
'
SuperStrict

Framework BaH.FMOD
Import BRL.GLMax2d

Local system:TFMODSystem = New TFMODSystem.Create()

system.Init(32)


' Create DSP units for each type of noise we want.

Local dsp:TFMODDsp = system.CreateDSPByType(FMOD_DSP_TYPE_OSCILLATOR)
dsp.SetParameter(FMOD_DSP_OSCILLATOR_RATE, 440.0)


Local channel:TFMODChannel

Graphics 640, 480, 0

While Not KeyDown(KEY_ESCAPE)

	Cls
	
		DrawText "===================================================================", 50, 10
		DrawText "GenerateTone Example.", 50, 30
		DrawText "===================================================================", 50, 50
		DrawText "Press '1' to play a sine wave", 50, 70
		DrawText "Press '2' to play a square wave", 50, 90
		DrawText "Press '3' to play a triangle wave", 50, 110
		DrawText "Press '4' to play a saw wave", 50, 130
		DrawText "Press '5' to play a white noise", 50, 150
		DrawText "Press 's' to stop channel", 50, 170

		DrawText "Press 'v'/'V' to change channel volume", 50, 200
		DrawText "Press 'f'/'F' to change channel frequency", 50, 220
		DrawText "Press '['/']' to change channel pan", 50, 240
		DrawText "Press 'Esc' to quit", 50, 260

		If KeyHit(KEY_1) Then
			channel = system.PlayDSP(FMOD_CHANNEL_REUSE, dsp, True, channel)
			channel.SetVolume(0.5)
			dsp.SetParameter(FMOD_DSP_OSCILLATOR_TYPE, 0)
			
			channel.SetPaused(False)
		End If

		If KeyHit(KEY_2) Then
			channel = system.PlayDSP(FMOD_CHANNEL_REUSE, dsp, True, channel)
			channel.SetVolume(0.125)
			dsp.SetParameter(FMOD_DSP_OSCILLATOR_TYPE, 1)

			channel.SetPaused(False)
		End If

		If KeyHit(KEY_3) Then
			channel = system.PlayDSP(FMOD_CHANNEL_REUSE, dsp, True, channel)
			channel.SetVolume(0.5)
			dsp.SetParameter(FMOD_DSP_OSCILLATOR_TYPE, 2)

			channel.SetPaused(False)
		End If

		If KeyHit(KEY_4) Then
			channel = system.PlayDSP(FMOD_CHANNEL_REUSE, dsp, True, channel)
			channel.SetVolume(0.125)
			dsp.SetParameter(FMOD_DSP_OSCILLATOR_TYPE, 4)

			channel.SetPaused(False)
		End If

		If KeyHit(KEY_5) Then
			channel = system.PlayDSP(FMOD_CHANNEL_REUSE, dsp, True, channel)
			channel.SetVolume(0.25)
			dsp.SetParameter(FMOD_DSP_OSCILLATOR_TYPE, 5)

			channel.SetPaused(False)
		End If
		
		If KeyHit(KEY_S) Then
			channel.Stop()
		End If
		
		If KeyHit(KEY_V) Then
'			If KeyDown(
		End If
	
		system.Update()
	
		Local frequency:Float
		Local volume:Float
		Local pan:Float
		Local playing:Int
		
		If channel Then
			channel.IsPlaying(playing)
			
			channel.GetFrequency(frequency)
			channel.GetVolume(volume)
			channel.GetPan(pan)
		End If
		
'		Local s:String = "Time " + Pad(pos / 1000 / 60) + ":" + Pad(pos / 1000 Mod 60) + ..
'			":" + Pad(pos / 10 Mod 100) + " /" + Pad(lenms / 1000 / 60) + ":" + Pad(lenms / 1000 Mod 60) + ..
'			":" + Pad(lenms / 10 Mod 100)
'			
'		If paused Then
'			s:+ " : Paused"
'		Else If playing Then
'			s:+ " : Playing"
'		Else
'			s:+ " : Stopped"
'		End If
'		
'		s:+ " : Channels Playing : " + channelsPlaying
'		
'		DrawText s, 50, 200
		
	Flip

Wend

' shut down
dsp.DSPRelease()

system.Close()
system.SystemRelease()


End

Function Pad:String(val:Int, size:Int = 2)
	Local s:String = "00" + val
	Return s[s.length - size..]
End Function

