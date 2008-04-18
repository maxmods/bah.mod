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

Local sound:TFMODSound[] = New TFMODSound[6]
Local channel:TFMODChannel[] = New TFMODChannel[6]

sound[0] = system.CreateSoundURL("media/drumloop.wav", FMOD_LOOP_NORMAL)
sound[1] = system.CreateSoundURL("media/jaguar.wav", FMOD_LOOP_NORMAL)
sound[2] = system.CreateSoundURL("media/swish.wav", FMOD_LOOP_NORMAL)
sound[3] = system.CreateSoundURL("media/c.ogg", FMOD_LOOP_NORMAL)
sound[4] = system.CreateSoundURL("media/d.ogg", FMOD_LOOP_NORMAL)
sound[5] = system.CreateSoundURL("media/e.ogg", FMOD_LOOP_NORMAL)

Local groupA:TFMODChannelGroup = system.CreateChannelGroup("Group A")
Local groupB:TFMODChannelGroup = system.CreateChannelGroup("Group B")

Local masterGroup:TFMODChannelGroup = system.GetMasterChannelGroup()


'  Instead of being independent, set the group A and B to be children of the master group.
masterGroup.AddGroup(groupA)
masterGroup.AddGroup(groupB)

' Start all the sounds!
For Local count:Int = 0 Until 6
	channel[count] = system.PlaySound(FMOD_CHANNEL_FREE, sound[count], True)
	
	If count < 3 Then
		channel[count].SetChannelGroup(groupA)
	Else
		channel[count].SetChannelGroup(groupB)
	End If
	
	channel[count].SetPaused(False)
Next

' Change the volume of each group, just because we can!  (And makes it less of a loud noise).
groupA.SetVolume(0.5)
groupB.SetVolume(0.5)


Graphics 640, 480, 0

While Not KeyDown(KEY_ESCAPE)

	Cls
	
		DrawText "===================================================================", 50, 10
		DrawText "ChannelGroups Example.", 50, 30
		DrawText "===================================================================", 50, 50
		DrawText "Group A : drumloop.wav, jaguar.wav, swish.wav", 50, 70
		DrawText "Group B : c.ogg, d.ogg, e.ogg", 50, 90
		DrawText "Press 'A' to mute/unmute group A", 50, 120
		DrawText "Press 'B' to mute/unmute group B", 50, 140
		DrawText "Press 'C' to mute/unmute group A and B (master group)", 50, 160
		DrawText "Press 'Esc' to quit", 50, 190

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

		If KeyHit(KEY_C) Then
			Global mute:Int = True
			masterGroup.SetMute(mute)
			mute = Not mute
		End If
	
		system.Update()
		
		Local channelsplaying:Int
		system.GetChannelsPlaying(channelsplaying)
		
		DrawText "Channels Playing : " + channelsplaying, 50, 240
		
	Flip

Wend

' A little fade out. (over 2 seconds)
Local pitch:Float = 1
Local vol:Float = 1

For Local count:Int = 0 Until 200
	masterGroup.SetPitch(pitch)
	masterGroup.SetVolume(vol)
	
	vol:- (1.0 / 200.0)
	pitch:- (0.5 / 200.0)
	
	Delay(10)
Next


' shut down
For Local count:Int = 0 Until 6
	sound[count].SoundRelease()
Next

groupA.ChannelGroupRelease()
groupB.ChannelGroupRelease()

system.Close()
system.SystemRelease()

End


