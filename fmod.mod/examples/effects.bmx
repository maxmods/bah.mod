'
'
' This example shows how to apply some of the built in software effects to sounds. 
' This example filters the global mix.  All software sounds played here would be filtered in the 
' same way.
' To filter per channel, and not have other channels affected, simply replace TFMODSystem.AddDSP with
' TFMODChannel.AddDSP.
' Note in this example you don't have to add and remove units each time, you could simply add them 
' all at the start then use TFMODDsp.SetActive to toggle them on and off.
'
'
SuperStrict

Framework BaH.FMOD
Import BRL.GLMax2d

Local system:TFMODSystem = New TFMODSystem.Create()

system.Init(32)

Local sound:TFMODSound = system.CreateSoundURL("media/drumloop.wav", FMOD_SOFTWARE)


Local dsplowpass:TFMODDsp = system.CreateDSPByType(FMOD_DSP_TYPE_LOWPASS)
Local dsphighpass:TFMODDsp = system.CreateDSPByType(FMOD_DSP_TYPE_HIGHPASS)
Local dspecho:TFMODDsp = system.CreateDSPByType(FMOD_DSP_TYPE_ECHO)
Local dspflange:TFMODDsp = system.CreateDSPByType(FMOD_DSP_TYPE_FLANGE)
Local dspdistortion:TFMODDsp = system.CreateDSPByType(FMOD_DSP_TYPE_DISTORTION)
Local dspchorus:TFMODDsp = system.CreateDSPByType(FMOD_DSP_TYPE_CHORUS)
Local dspparameq:TFMODDsp = system.CreateDSPByType(FMOD_DSP_TYPE_PARAMEQ)

Local channel:TFMODChannel = system.PlaySound(FMOD_CHANNEL_FREE, sound)

Graphics 640, 480, 0

While Not KeyDown(KEY_ESCAPE)

	Cls
	
		DrawText "===================================================================", 50, 10
		DrawText "Effects Example.", 50, 30
		DrawText "===================================================================", 50, 50
		DrawText "Press SPACE to paused/unpause sound.", 50, 70
		DrawText "Press '1' to toggle dsplowpass effect.", 50, 90
		DrawText "Press '2' to toggle dsphighpass effect.", 50, 110
		DrawText "Press '3' to toggle dspecho effect.", 50, 130
		DrawText "Press '4' to toggle dspflange effect.", 50, 150
		DrawText "Press '5' to toggle dspdistortion effect.", 50, 170
		DrawText "Press '6' to toggle dspchorus effect.", 50, 190
		DrawText "Press '7' to toggle dspparameq effect.", 50, 210
		DrawText "Press 'Esc' to quit", 50, 230

		If KeyHit(KEY_SPACE) Then
			Local paused:Int
			channel.GetPaused(paused)
			paused = Not paused
			channel.SetPaused(paused)
		End If

		If KeyHit(KEY_1) Then
			Local active:Int
			dsplowpass.GetActive(active)
			
			If active Then
				dsplowpass.Remove()
			Else
				system.AddDSP(dsplowpass)
			End If
		End If

		If KeyHit(KEY_2) Then
			Local active:Int
			dsphighpass.GetActive(active)
			
			If active Then
				dsphighpass.Remove()
			Else
				system.AddDSP(dsphighpass)
			End If
		End If

		If KeyHit(KEY_3) Then
			Local active:Int
			dspecho.GetActive(active)
			
			If active Then
				dspecho.Remove()
			Else
				system.AddDSP(dspecho)
				dspecho.SetParameter(FMOD_DSP_ECHO_DELAY, 50.0)
			End If
		End If

		If KeyHit(KEY_4) Then
			Local active:Int
			dspflange.GetActive(active)
			
			If active Then
				dspflange.Remove()
			Else
				system.AddDSP(dspflange)
			End If
		End If

		If KeyHit(KEY_5) Then
			Local active:Int
			dspdistortion.GetActive(active)
			
			If active Then
				dspdistortion.Remove()
			Else
				system.AddDSP(dspdistortion)
				dspdistortion.SetParameter(FMOD_DSP_DISTORTION_LEVEL, 0.8)
			End If
		End If

		If KeyHit(KEY_6) Then
			Local active:Int
			dspchorus.GetActive(active)
			
			If active Then
				dspchorus.Remove()
			Else
				system.AddDSP(dspchorus)
			End If
		End If

		If KeyHit(KEY_7) Then
			Local active:Int
			dspparameq.GetActive(active)
			
			If active Then
				dspparameq.Remove()
			Else
				system.AddDSP(dspparameq)
				dspparameq.SetParameter(FMOD_DSP_PARAMEQ_CENTER, 5000)
				dspparameq.SetParameter(FMOD_DSP_PARAMEQ_GAIN, 0)
			End If
		End If

		system.Update()

		Local paused:Int
		Local dsplowpassActive:Int
		Local dsphighpassActive:Int
		Local dspechoActive:Int
		Local dspflangeActive:Int
		Local dspdistortionActive:Int
		Local dspchorusActive:Int
		Local dspparameqActive:Int

		dsplowpass.GetActive(dsplowpassActive)
		dsphighpass.GetActive(dsphighpassActive)
		dspecho.GetActive(dspechoActive)
		dspflange.GetActive(dspflangeActive)
		dspdistortion.GetActive(dspdistortionActive)
		dspchorus.GetActive(dspchorusActive)
		dspparameq.GetActive(dspparameqActive)

		If channel Then
			channel.GetPaused(paused)
		End If
				
		Local s:String
					
		If paused Then
			s:+ "Paused  : "
		Else
			s:+ "Playing : "
		End If
		
		If dsplowpassActive Then
			s:+ "lowpass[x] "
		Else
			s:+ "lowpass[ ] "
		End If

		If dsphighpassActive Then
			s:+ "highpass[x] "
		Else
			s:+ "highpass[ ] "
		End If

		If dspechoActive Then
			s:+ "echo[x] "
		Else
			s:+ "echo[ ] "
		End If

		If dspflangeActive Then
			s:+ "flange[x] "
		Else
			s:+ "flange[ ] "
		End If

		If dspdistortionActive Then
			s:+ "dist[x] "
		Else
			s:+ "dist[ ] "
		End If

		If dspchorusActive Then
			s:+ "chorus[x] "
		Else
			s:+ "chorus[ ] "
		End If

		If dspparameqActive Then
			s:+ "parameq[x] "
		Else
			s:+ "parameq[ ] "
		End If
		
		DrawText s, 5, 300

	Flip

Wend

' shut down
sound.SoundRelease()

system.Close()
system.SystemRelease()


End


