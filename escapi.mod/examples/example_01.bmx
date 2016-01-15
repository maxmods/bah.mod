SuperStrict

Framework bah.escapi
Import brl.standardio
Import brl.pixmap
Import brl.glmax2d


Local escapi:TEscapi = New TEscapi.Create()

If Not escapi Then

	Print "Could not load escapi"

	End
	
End If

Local deviceCount:Int = escapi.Count()
Print "Devices = " + deviceCount

If deviceCount = 0 Then
	Print "No devices found :-("
	End
End If

Local width:Int = 512
Local height:Int = 512

Local pix:TPixmap = CreatePixmap(width, height, PF_BGRA8888)

escapi.SetBuffer(pix.pixels)
escapi.SetWidth(width)
escapi.SetHeight(height)

If Not escapi.Open(0) Then
	Print "Could not open device"
	End
End If

Print "Opened device : " + escapi.DeviceName(0)

Graphics 800, 600, 0

Local waitingForCapture:Int
While Not KeyDown(KEY_ESCAPE)

	Cls
	
	If Not waitingForCapture Then
		escapi.Capture(0)
		waitingForCapture = True
	Else
		Delay 1
		If escapi.IsCaptureDone(0) Then
			waitingForCapture = False
		End If
	End If
	
	' don't use DrawPixmap in proper apps. It's very slow.
	DrawPixmap pix, 100, 100
	
	Flip

Wend

escapi.Close(0)


