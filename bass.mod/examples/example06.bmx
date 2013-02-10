'
' Play a shoutcast stream...
'
'
SuperStrict

Framework BaH.Bass
Import BRL.GLMax2D


If Not TBass.Init(-1,44100,0,Null,Null) Then
	DebugLog "Can't initialize device : " + TBass.ErrorGetCode()
	End
End If

BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST,1) ' enable playlist processing
BASS_SetConfig(BASS_CONFIG_NET_PREBUF,0) ' minimize automatic pre-buffering, so we can do it (And display it) instead


Local isMod:Int = False

' rocking!
Local url:String = "http://www.sky.fm/mp3/soundtracks.pls"

Local syncData:TBassSyncData = New TBassSyncData

Local channel:TBassChannel = New TBassStream.StreamCreateURL(url, 0, BASS_SAMPLE_FLOAT|BASS_STREAM_STATUS|BASS_STREAM_AUTOFREE, Null, Null)

If channel Then

	channel.SetSync(BASS_SYNC_META, 0, syncData)

	Local length:Long = channel.GetLength(BASS_POS_BYTE)

	If Not channel.Play(False) Then
		DebugLog "can't play... : "  + TBass.ErrorGetCode()
	Else
	
		' output some stream information
		Local tags:String[] = channel.getTags(BASS_TAG_ICY)
		For Local s:String = EachIn tags
			DebugLog s
		Next

	
	Graphics 640, 480, 0

	Local active:Int = channel.IsActive()
	
	While active And Not KeyDown(key_escape)
	
		Cls

		' lets check for new meta data!		
		If syncData.IsSet() Then
			tags = channel.getTags(BASS_TAG_META)
			For Local s:String = EachIn tags
				DebugLog "meta : " + s
			Next
		End If

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


