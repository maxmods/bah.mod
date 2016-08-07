SuperStrict

Framework brl.glmax2d
Import brl.standardio
Import bah.libembroidery


Local file:String = "samples/Applpupy.pes"

Local pattern:TEmbPattern = New TEmbPattern.Create()
If Not pattern.Read(file) Then
	Print "Error loading " + file
	End
End If

Local list:TEmbStitchList = pattern.StitchList()
If Not list Then
	pattern.CopyPolylinesToStitchList()
	list = pattern.StitchList()
	If Not list Then
		Print "no stitches..."
		End
	End If
End If
Local threads:TEmbThreadList = pattern.ThreadList()
Local stitch:SEmbStitch = list.Stitch()
Local lastStitch:SEmbStitch

Graphics 1024, 768, 0

Local index:Int
Local centerX:Float = GraphicsWidth()/2
Local centerY:Float = GraphicsHeight()/2

Local scale:Float = 6
Local ended:Int = False
Local colorIndex:Int = -1
Local color:SEmbColor

SetClsColor 255, 255, 255
Cls
Flip
Cls


While Not KeyDown(key_escape)

	If KeyHit(key_h) Then
		Cls;Flip;Cls
		pattern.Flip(True, False)
		list = pattern.StitchList()
		stitch = list.Stitch()
		colorIndex = -1
	End If

	If KeyHit(key_v) Then
		Cls;Flip;Cls
		pattern.Flip(False, True)
		list = pattern.StitchList()
		stitch = list.Stitch()
		colorIndex = -1
	End If


	If list And Not ended Then

		list = list.NextStitch()
	
		If stitch.flags = STITCH_JUMP Or stitch.flags = STITCH_NORMAL Then
			lastStitch = stitch
		End If
		
		stitch = list.Stitch()
		
		If stitch.flags = STITCH_JUMP Or stitch.flags = STITCH_TRIM Or stitch.flags = STITCH_STOP Then
			Continue
		End If
		
		If stitch.flags = STITCH_END Then
			ended = True
			Continue
		End If
		
		If colorIndex <> stitch.colorIndex Then
			colorIndex = stitch.colorIndex
			
			Local thread:SEmbThread = threads.GetAt(colorIndex)
			color = thread.color
		End If

		SetColor color.r, color.g, color.b
		
		Local x0:Float = centerX + (lastStitch.xx * scale)
		Local y0:Float = centerY + (lastStitch.yy * scale)
		Local x1:Float = centerX + (stitch.xx * scale)
		Local y1:Float = centerY + (stitch.yy * scale)
		
		DrawLine x0, y0, x1, y1

		Flip 0

		DrawLine x0, y0, x1, y1

		Flip 0
	
	
	End If
	
Wend


