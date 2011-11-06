SuperStrict

Framework BaH.TheoraPlayer
Import BaH.TheoraPlayerBASS ' audio
Import BRL.GLMax2D

' Initialise BASS audio
If Not TBass.Init(-1,44100,0,Null,Null) Then
	DebugLog "Can't initialize device : " + TBass.ErrorGetCode()
	End
End If


Local manager:TTheoraVideoManager = New TTheoraVideoManager.Create()

' set the audio driver
manager.setAudioInterfaceFactory(New TBASSTheoraAudioInterfaceFactory.Create())

' load the video
Local clip:TTheoraVideoClip = manager.createVideoClip("../../theoraplayer.mod/examples/video/PipeDream2001.ogv")

' create a pixmap of the appropriate size
Local pix:TPixmap = TPixmap.Create(clip.getWidth(), clip.getHeight(), PF_RGB888)


Graphics 800, 600, 0

Local time:Int = MilliSecs()
Local t:Int = time

SetScale 2, 2

While Not KeyDown(KEY_ESCAPE)

	Cls
	
	
	Local frame:TTheoraVideoFrame = clip.getNextFrame()
	
	If frame Then
		MemCopy(pix.pixels, frame.getBuffer(), pix.capacity)
		clip.popFrame()
	End If
	
	Local image:TImage = LoadImage(pix)
	DrawImage image, 0, 0 
	
	t = MilliSecs()

	manager.update((t - time)/1000.0)

	time = t
	
	Flip

Wend


