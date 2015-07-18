SuperStrict

Framework bah.xcb
Import brl.polledinput

EnablePolledInput()

Local win:TXCBWindow = New TXCBWindow.Create("Hello World!")

win.show()

While Not AppTerminate() 
	PollSystem()
	Delay 10
Wend

