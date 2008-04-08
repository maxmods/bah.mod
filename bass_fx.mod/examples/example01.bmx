'
' Load and play a mod
'
'
SuperStrict

Framework BaH.Bass
Import BaH.Bass_FX
Import BRL.GLMax2D


If Not TBass.Init(-1,44100,0,Null,Null) Then
	DebugLog "Can't initialize device : " + TBass.ErrorGetCode()
	End
End If

Local isMod:Int = False

Local file:String = "../../bass.mod/examples/resources/HarbourBizarre.mod"

Local channel:TBassChannel = New TBassStream.CreateFile(file, 0, 0, 0)

If Not channel Then
	channel = New TBassMusic.FileLoad(file, 0, 0, BASS_SAMPLE_LOOP | BASS_MUSIC_RAMPS | BASS_MUSIC_PRESCAN, 0)
	If channel Then
		isMod = True
	End If
End If

If channel Then
	Local length:Long = channel.GetLength(BASS_POS_BYTE)
	
	' add a phaser effect
	channel.SetFX(BASS_FX_BFX_PHASER, 1)
	' add an echo effect
	channel.SetFX(BASS_FX_BFX_ECHO, 2)

	If Not channel.Play(False) Then
		DebugLog "can't play... : "  + TBass.ErrorGetCode()
	Else
	
	Graphics 640, 480, 0

	Local active:Int = channel.IsActive()
	
	While active And Not KeyDown(key_escape)
	
		Cls
		

		' display some stuff And wait a bit
		Local _left:Int, _right:Int

		channel.GetLevel(_left, _right)

		Local pos:Long = channel.GetPosition(BASS_POS_BYTE)
		Local time:Double = channel.Bytes2Seconds(pos)

		DrawText "position - " + pos, 50, 50
		DrawText "elapsed  - " + Int(time/60) + ":" + Int(time Mod 60), 50, 65

		If isMod Then
			Local low:Int, high:Int
			pos = channel.GetPositionLowHigh(BASS_POS_MUSIC_ORDER, low, high)
			DrawText "         - " + low + ":" + high, 50, 80
		End If
		
		If active = BASS_ACTIVE_STALLED And TBassStream(channel) Then  ' playback has stalled

			DrawText "-- Buffering : " + TBassStream(channel).GetFilePosition(BASS_FILEPOS_BUFFER) + " --", 20, 90

		Else
			Local lwidth:Int = _left / 410.0
			DrawRect 100 - lwidth, 100, lwidth, 20
			DrawRect 110, 100, _right / 410.0, 20
		End If

		DrawText "CPU - " + TBass.GetCPU(), 240, 50

		Delay 50
		
		Flip

		active = channel.IsActive()

	Wend

	End If
Else
	DebugLog "didn't load : " + TBass.ErrorGetCode()
End If

TBass.Free()

End


