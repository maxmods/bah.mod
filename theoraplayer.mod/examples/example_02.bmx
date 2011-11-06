SuperStrict

Framework BaH.TheoraPlayer
Import BaH.TheoraPlayerOpenAL ' audio
Import BRL.GLMax2D

Local manager:TTheoraVideoManager = New TTheoraVideoManager.Create()

' set the audio driver, if available
If OpenALInstalled() Then
	manager.setAudioInterfaceFactory(New TOpenALTheoraAudioInterfaceFactory.Create())
End If

' load the video
Local clip:TTheoraVideoClip = manager.createVideoClip("video/PipeDream2001.ogv")

Local duration:Float = clip.getDuration()

' create a pixmap of the appropriate size
Local pix:TPixmap = TPixmap.Create(clip.getWidth(), clip.getHeight(), PF_RGB888)


Graphics 800, 600, 0

Local time:Int = MilliSecs()
Local t:Int = time

Local pauseFlash:Int = 0

While Not KeyDown(KEY_ESCAPE)

	Cls
	
	' SPACE to toggle Pause/Play
	If KeyHit(KEY_SPACE) Then
		If clip.isPaused() Then
			clip.Play()
		Else
			clip.Pause()
		End If
	End If
	
	Local frame:TTheoraVideoFrame = clip.getNextFrame()
	
	If frame Then
		MemCopy(pix.pixels, frame.getBuffer(), pix.capacity)
		clip.popFrame()
	End If
	
	Local image:TImage = LoadImage(pix)

	SetScale 2, 2
	DrawImage image, 0, 0
	
	SetScale 1, 1
	DrawText secondsToTime(duration), 750, 0
	
	drawTime(clip.getTimePosition(), duration, 610, 15)
	
	If clip.isPaused() Then
		If (MilliSecs() / 1000) Mod 2 Then
			DrawText "||", 590, 10
		End If
	Else
		DrawText ">", 594, 10
	End If
	
	t = MilliSecs()

	manager.update((t - time)/1000.0)

	time = t
	
	Flip

Wend

Function secondsToTime:String(sec:Float)
	Local time:String

	Local mins:Int = sec / 60
	Local secs:Int = sec Mod 60

	Local s:String = "0" + secs

	time = mins + ":" + s[s.length - 2..]

	Return time
End Function

Function drawTime(time:Float, duration:Float, x:Int, y:Int)
	Local t:String = secondsToTime(time)
	
	DrawLine x, y, x + 150, y
	DrawLine x + 150, y, x + 150, y + 5
	DrawLine x, y + 5, x + 150, y + 5
	DrawLine x, y, x, y + 5
	
	Local xOffset:Int = time / (duration / 150)
	
	DrawRect x, y, xOffset, 5
	
	DrawText t, x + xOffset - 12, y + 8

End Function


