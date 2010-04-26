'
' This example shows how to how to pick up a music spectrum via fmod
'
'
SuperStrict

Framework BaH.FMOD
Import BRL.GLMax2d

Local system:TFMODSystem = New TFMODSystem.Create()

system.Init(32)

Local sound:TFMODSound = system.CreateStreamURL("media/drumloop.wav", FMOD_SOFTWARE)

Local channel:TFMODChannel

Global WindowHeight:Int = 800
Const SpectrumSize:Int = 64	' must be power of 2, smaller the faster > 200 isn't that much use
Global WindowWidth:Int = 512
Global f_Sensitivity:Float = 8000.0	

Graphics WindowWidth,WindowHeight

Const SPECTRUM_GROUP_SIZE:Int = (SpectrumSize/4)

'	Averages of spectrums
Global AverageLeft:Float[SpectrumSize/SPECTRUM_GROUP_SIZE]
Global AverageRight:Float[SpectrumSize/SPECTRUM_GROUP_SIZE]

channel = system.PlaySound(FMOD_CHANNEL_FREE, sound)

channel.SetMode(FMOD_LOOP_NORMAL)

Local spectrum:Float[SpectrumSize]
SetBlend LIGHTBLEND

Global b_Decibels:Int
Local b_average:Int


While Not KeyDown(KEY_ESCAPE) And Not AppTerminate()

	Cls
			
	DrawText "** Spectrum Example **", 20, 10
	DrawText "=> A: Average", 20, 30
	DrawText "=> D: Decibels", 20, 50

	system.Update()

	Local playing:Int
	Local paused:Int
	Local pos:Int
	Local lenms:Int
	Local channelsPlaying:Int
		
	If channel Then
		channel.IsPlaying(playing)
		
		channel.GetPaused(paused)
		
		channel.GetPosition(pos, FMOD_TIMEUNIT_MS)
		
		Local currentSound:TFMODSound = channel.GetCurrentSound()			
		If currentSound Then
			currentSound.GetLength(lenms, FMOD_TIMEUNIT_MS)
		End If
		
		'	1 = right, 0 = left
		If channel.GetSpectrum(spectrum, 0,FMOD_DSP_FFT_WINDOW_TRIANGLE) <> FMOD_OK
			DrawText "no left spectrum", 50, 110
		Else
			
			'	Left spectrum
			SetColor 255,0,0
			
								
						
			GetAverage(spectrum,AverageLeft)
			
			If b_average
				DrawSpectrum(AverageLeft)
			Else
				DrawSpectrum(spectrum)	
			EndIf		
					
		EndIf
	
		If channel.GetSpectrum(spectrum, 1,FMOD_DSP_FFT_WINDOW_TRIANGLE) <> FMOD_OK
			DrawText "no right spectrum", 50, 110
		Else
			
			
			'	Right spectrum
			SetColor 0,0,255
					
			GetAverage(spectrum,AverageRight)
			
			If b_average
				DrawSpectrum(AverageRight)
			Else
				DrawSpectrum(spectrum)	
			EndIf		
			SetColor 200,200,200

		EndIf

	End If
	
	system.GetChannelsPlaying(channelsPlaying)
	
	Local s:String = "Time " + Pad(pos / 1000 / 60) + ":" + Pad(pos / 1000 Mod 60) + ..
		":" + Pad(pos / 10 Mod 100) + " /" + Pad(lenms / 1000 / 60) + ":" + Pad(lenms / 1000 Mod 60) + ..
		":" + Pad(lenms / 10 Mod 100)
		
	If paused Then
		s:+ " : Paused"
	Else If playing Then
		s:+ " : Playing"
	Else
		s:+ " : Stopped"
	End If
	
	s:+ " : # Channels : " + channelsPlaying
	s :+ " : db : "+b_decibels
	DrawText s, 20, 70
	
	Flip
	
	If KeyDown(KEY_D) Then b_Decibels :~ 1
	If KeyDown(KEY_A) Then b_Average :~ 1
		
Wend

' shut down
sound.SoundRelease()
system.Close()
system.SystemRelease()


End

Function Pad:String(val:Int, size:Int = 2)
	Local s:String = "00" + val
	Return s[s.length - size..]
End Function

Function GetAverage(_source:Float[], _dest:Float[])

	If _source And _dest And _dest.length
	
		Local sample_size:Int = _source.length / _dest.length
		Local sample_index:Int
		Local dest_index:Int
		Local f_sum:Float
		
		For Local f_value:Float = EachIn _source
			
			f_sum :+ f_value 
					
			sample_index :+ 1
			
			If sample_index >= sample_size
				_dest[dest_index] = f_sum/Float(sample_size)
				dest_index :+ 1
				sample_index = 0
				f_sum = 0
			EndIf
		Next
	
	EndIf

End Function


Function DrawSpectrum(_spectrum:Float[])

	If _spectrum And _spectrum.length

		Local index_offset:Int = 0
		Local bar_width:Int = WindowWidth / _spectrum.length
		
		For Local f_val:Float = EachIn _spectrum
				
			If b_Decibels
				DrawRect index_offset, WindowHeight-1,bar_width,  4 *  Float( -95.0- 20.0 * Float(Log10(f_val))) 
			Else
				DrawRect index_offset, WindowHeight-1,bar_width,-Min(WindowHeight-90,f_val * f_Sensitivity )
			EndIf
					
			index_offset :+ bar_width
					
		Next
	EndIf

End Function

		
