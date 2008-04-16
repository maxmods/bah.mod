'
' This example shows how you can create your own multi-subsound stream, then in realtime replace
' each the subsound as it plays them.  Using a looping sentence, it will seamlessly stich between
' 2 subsounds in this example, and each time it switches to a new sound, it will replace the old
' one with another sound in our list.
'
' These sounds can go on forever as long as they are the same bitdepth (when decoded) and number
' of channels (ie mono / stereo).  The reason for this is the hardware channel cannot change 
' formats mid sentence, and using different hardware channels would mean it wouldn't be gapless.
'
'
SuperStrict

Framework BaH.FMOD
Import BRL.GLMax2d
Import BRL.StandardIO

Const NUMSOUNDS:Int = 32

Local soundname:String[] = [ ..
        "media/e.ogg", ..   ' Ma-    
        "media/d.ogg", ..   ' ry     
        "media/c.ogg", ..   ' had    
        "media/d.ogg", ..   ' a      
        "media/e.ogg", ..   ' lit-   
        "media/e.ogg", ..   ' tle    
        "media/e.ogg", ..   ' lamb,  
        "media/e.ogg", ..   ' .....  
        "media/d.ogg", ..   ' lit-   
        "media/d.ogg", ..   ' tle    
        "media/d.ogg", ..   ' lamb,  
        "media/d.ogg", ..   ' .....  
        "media/e.ogg", ..   ' lit-   
        "media/e.ogg", ..   ' tle    
        "media/e.ogg", ..   ' lamb,  
        "media/e.ogg", ..   ' .....  
        "media/e.ogg", ..   ' Ma-    
        "media/d.ogg", ..   ' ry     
        "media/c.ogg", ..   ' had    
        "media/d.ogg", ..   ' a      
        "media/e.ogg", ..   ' lit-  
        "media/e.ogg", ..   ' tle   
        "media/e.ogg", ..   ' lamb, 
        "media/e.ogg", ..   ' its   
        "media/d.ogg", ..   ' fleece
        "media/d.ogg", ..   ' was   
        "media/e.ogg", ..   ' white 
        "media/d.ogg", ..   ' as    
        "media/c.ogg", ..   ' snow.
        "media/c.ogg", ..   ' .....  
        "media/c.ogg", ..   ' .....  
        "media/c.ogg" ]     ' .....  

Local subsound:TFMODSound[] = New TFMODSound[2]


' Create a System object and initialize.
Local system:TFMODSystem = New TFMODSystem.Create()

system.Init(1)

' Set up the FMOD_CREATESOUNDEXINFO structure for the user stream with room for 2 subsounds. (our subsound double buffer)
Local exinfo:TFMODCreateSoundExInfo = New TFMODCreateSoundExInfo
exinfo.SetDefaultFrequency(44100)
exinfo.SetNumSubSounds(2)
exinfo.SetNumChannels(1)
exinfo.SetFormat(FMOD_SOUND_FORMAT_PCM16)

' Create the 'parent' stream that contains the substreams.  Set it to loop so that it loops between subsound 0 and 1.


Local sound:TFMODSound = system.CreateStreamPtr(Null, FMOD_LOOP_NORMAL | FMOD_OPENUSER, exinfo)


' Add 2 of our streams as children of the parent.  They should be the same format (ie mono/stereo and bitdepth) as the parent sound.
' When subsound 0 has finished and it is playing subsound 1, we will swap subsound 0 with a new sound, and the same for when subsound 1 has finished,
' causing a continual double buffered flip, which means continuous sound.

subsound[0] = system.CreateStreamURL(soundname[0], FMOD_DEFAULT)
subsound[1] = system.CreateStreamURL(soundname[1], FMOD_DEFAULT)

sound.SetSubSound(0, subsound[0])
sound.SetSubSound(1, subsound[1])

' Set up the gapless sentence to contain these first 2 streams.

Local soundlist:Int[] = [ 0, 1 ]
sound.SetSubSoundSentence(soundlist)

Local subsoundid:Int = 0
Local sentenceid:Int = 2 ' The next sound to be appeneded to the stream.


Print "Inserted subsound 0 / 2 with sound 0 / " + NUMSOUNDS
Print "Inserted subsound 1 / 2 with sound 1 / " + NUMSOUNDS


' Play the sound.
Local channel:TFMODChannel = system.PlaySound(FMOD_CHANNEL_FREE, sound, False)


Graphics 640, 480, 0

While Not KeyDown(KEY_ESCAPE)

	Cls
	
		DrawText "===================================================================", 50, 10
		DrawText "Real-time stitching example.", 50, 30
		DrawText "===================================================================", 50, 50
		DrawText "Press space to pause, Esc to quit", 50, 90

	
		system.Update()
		
		
		If KeyHit(KEY_SPACE) Then
			Local paused:Int
			channel.GetPaused(paused)
			channel.SetPaused(Not paused)
		End If
		
		
		Local currentsubsoundid:Int
		
		
		' Replace the subsound that just finished with a new subsound, to create endless seamless stitching!

         ' Note that this polls the currently playing subsound using the FMOD_TIMEUNIT_BUFFERED flag.  
         ' Remember streams are decoded / buffered ahead in advance! 
         ' Don't use the 'audible time' which is FMOD_TIMEUNIT_SENTENCE_SUBSOUND by itself.  When streaming, sound is 
         ' processed ahead of time, and things like stream buffer / sentence manipulation (as done below) is required 
         ' to be in 'buffered time', or else there will be synchronization problems and you might end up releasing a
         ' sub-sound that is still playing!

		channel.GetPosition(currentsubsoundid, FMOD_TIMEUNIT_SENTENCE_SUBSOUND | FMOD_TIMEUNIT_BUFFERED)
		
		If currentsubsoundid <> subsoundid Then
		
			' Release the sound that isn't playing any more. 
			subsound[subsoundid].SoundRelease()
			
			' Replace it with a new sound in our list.
			subsound[subsoundid] = system.CreateStreamURL(soundname[sentenceid], FMOD_DEFAULT)
			
			sound.SetSubSound(subsoundid, subsound[subsoundid])
			
			Print "Replacing subsound " + subsoundid + " / 2 with sound " + sentenceid + " / " + NUMSOUNDS
			
			sentenceid:+ 1
			
			If sentenceid >= NUMSOUNDS Then
				sentenceid = 0
			End If
			
			subsoundid = currentsubsoundid
		
		End If
		
	Flip

Wend

' shut down
sound.SoundRelease() ' Freeing a parent subsound also frees its children.

system.Close()
system.SystemRelease()


End

Function Pad:String(val:Int, size:Int = 2)
	Local s:String = "00" + val
	Return s[s.length - size..]
End Function

