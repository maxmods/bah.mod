'
' Play a shoutcast stream...
'
'
SuperStrict

Framework BaH.Bass
?Not bmxng
Import BRL.GLMax2D
?bmxng
Import sdl.gl2sdlmax2d
?

If Not TBass.Init(-1,44100,0,Null,Null) Then
	DebugLog "Can't initialize device : " + TBass.ErrorGetCode()
	End
End If

BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST,1) ' enable playlist processing
BASS_SetConfig(BASS_CONFIG_NET_PREBUF,0) ' minimize automatic pre-buffering, so we can do it (And display it) instead


Local isMod:Int = False

' cinemix.us
Local url:String = "http://209.9.238.4:6046/listen.pls"

Local syncData:TBassSyncData = New TBassSyncData

Local channel:TBassChannel = New TBassStream.StreamCreateURL(url, 0, BASS_SAMPLE_FLOAT|BASS_STREAM_STATUS|BASS_STREAM_AUTOFREE, Null, Null)

Local messageList:String[20]

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

	
	Graphics 1024, 768, 0

	Local active:Int = channel.IsActive()
	
	While active And Not KeyDown(key_escape)
	
		Cls

		' lets check for new meta data!		
		If syncData.IsSet() Then
			tags = channel.getTags(BASS_TAG_META)
			For Local s:String = EachIn tags
				addMessage(s, messageList)
				'DebugLog "meta : " + s
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
		
		For Local i:Int = 0 Until messageList.length
			DrawText "* " + messageList[i], 10, 200 + i * 12
		Next

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

Function addMessage(text:String, messages:String[])

	Local i:Int = messages.length - 1
	While i
		messages[i] = messages[i - 1]
		i :- 1
	Wend
	
	messages[0] = text

End Function

