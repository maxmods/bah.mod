SuperStrict

Framework BaH.TheoraPlayer
Import BRL.GLMax2D

Local manager:TTheoraVideoManager = New TTheoraVideoManager.Create()

Local clip:TTheoraVideoClip = manager.createVideoClip("video/PipeDream2001.ogv")

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

