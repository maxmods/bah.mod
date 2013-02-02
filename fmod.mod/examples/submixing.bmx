'
' This example shows how to put channels into channel groups, so that you can affect a group
' of channels at a time instead of just one channel at a time.
'
'
SuperStrict

Framework BaH.FMOD
Import BRL.GLMax2d

' Create a System object and initialize.
Local system:TFMODSystem = New TFMODSystem.Create()

system.Init(32)

Local sound:TFMODSound[] = New TFMODSound[5]
Local channel:TFMODChannel[] = New TFMODChannel[5]

sound[0] = system.CreateSoundURL("media/drumloop.wav", FMOD_SOFTWARE | FMOD_LOOP_NORMAL)
sound[1] = system.CreateSoundURL("media/jaguar.wav", FMOD_SOFTWARE | FMOD_LOOP_NORMAL)
sound[2] = system.CreateSoundURL("media/c.ogg", FMOD_SOFTWARE | FMOD_LOOP_NORMAL)
sound[3] = system.CreateSoundURL("media/d.ogg", FMOD_SOFTWARE | FMOD_LOOP_NORMAL)
sound[4] = system.CreateSoundURL("media/e.ogg", FMOD_SOFTWARE | FMOD_LOOP_NORMAL)

Local groupA:TFMODChannelGroup = system.CreateChannelGroup("Group A")
Local groupB:TFMODChannelGroup = system.CreateChannelGroup("Group B")

Local masterGroup:TFMODChannelGroup = system.GetMasterChannelGroup()


'  Instead of being independent, set the group A and B to be children of the master group.
masterGroup.AddGroup(groupA)
masterGroup.AddGroup(groupB)

' Start all the sounds!
For Local count:Int = 0 Until 5
	channel[count] = system.PlaySound(FMOD_CHANNEL_FREE, sound[count], True)

	If count < 2 Then
		channel[count].SetChannelGroup(groupA)
	Else
		channel[count].SetChannelGroup(groupB)
	End If
	
	channel[count].SetPaused(False)
Next

' Create the DSP effects we want to apply to our submixes.
Local dspreverb:TFMODDsp = system.CreateDSPByType(FMOD_DSP_TYPE_SFXREVERB)

Local dspflange:TFMODDsp = system.CreateDSPByType(FMOD_DSP_TYPE_FLANGE)
dspflange.SetParameter(FMOD_DSP_FLANGE_RATE, 1.0)

Local dsplowpass:TFMODDsp = system.CreateDSPByType(FMOD_DSP_TYPE_LOWPASS)
dsplowpass.SetParameter(FMOD_DSP_LOWPASS_CUTOFF, 500)

Graphics 640, 480, 0

While Not KeyDown(KEY_ESCAPE)

	Cls
	
		DrawText "===================================================================", 50, 10
		DrawText "Sub-mixing example.", 50, 30
		DrawText "===================================================================", 50, 50
		DrawText "                                                       (drumloop.wav)", 50, 70
		DrawText "                                                      /              ", 50, 90
		DrawText "                                              (groupA)               ", 50, 110
		DrawText "                                     (reverb)/        \\             ", 50, 130
		DrawText "                                    /                  (jaguar.wav)  ", 50, 150
		DrawText "(soundcard)--(lowpass)--(mastergroup)                                ", 50, 170
		DrawText "                                    \\                  (c.ogg)      ", 50, 190
		DrawText "                                     (flange)         /              ", 50, 210
		DrawText "                                             \\(groupB)--(d.ogg)     ", 50, 230
		DrawText "                                                      \\             ", 50, 250
		DrawText "                                                       (e.ogg)       ", 50, 270
		DrawText "Press 'A' to mute/unmute group A", 50, 300
		DrawText "Press 'B' to mute/unmute group B", 50, 320
		DrawText "Press 'R' to place reverb on group A", 50, 350
		DrawText "Press 'F' to place flange on group B", 50, 370
		DrawText "Press 'L' to place lowpass on master group (everything)", 50, 390
		DrawText "Press 'Esc' to quit", 50, 420

		If KeyHit(KEY_A) Then
			Global mute:Int = True
			groupA.SetMute(mute)
			mute = Not mute
		End If

		If KeyHit(KEY_B) Then
			Global mute:Int = True
			groupB.SetMute(mute)
			mute = Not mute
		End If

		If KeyHit(KEY_R) Then
			Global reverb:Int = True
			If reverb Then
				groupA.AddDSP(dspreverb)
			Else
				dspreverb.Remove()
			End If
			reverb = Not reverb
		End If

		If KeyHit(KEY_F) Then
			Global flange:Int = True
			If flange Then
				groupB.AddDSP(dspflange)
			Else
				dspflange.Remove()
			End If
			flange = Not flange
		End If

		If KeyHit(KEY_L) Then
			Global lowpass:Int = True
			If lowpass Then
				masterGroup.AddDSP(dsplowpass)
			Else
				dsplowpass.Remove()
			End If
			lowpass = Not lowpass
		End If
	
		system.Update()
		
		Local channelsplaying:Int
		system.GetChannelsPlaying(channelsplaying)
		
		DrawText "Channels Playing : " + channelsplaying, 50, 240
		
	Flip

Wend

' shut down
For Local count:Int = 0 Until 5
	sound[count].SoundRelease()
Next

dspreverb.DSPRelease()
dspflange.DSPRelease()
dsplowpass.DSPRelease()

groupA.ChannelGroupRelease()
groupB.ChannelGroupRelease()

system.Close()
system.SystemRelease()

End



