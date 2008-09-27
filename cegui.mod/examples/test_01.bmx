SuperStrict

Framework BaH.CEGUI
Import BRL.GLGraphics
Import BRL.GLMax2d

Graphics 800, 600, 0

HideMouse

Init_CEGUI()

TCESchemeManager.loadScheme("WindowsLook.scheme")

TCESystem.setDefaultFont("Commonwealth-12")
TCESystem.setDefaultMouseCursor("WindowsLook", "MouseArrow")


Local root:TCEWindow = TCEWindowManager.loadWindowLayout("test_01.layout")
TCESystem.setGUISheet(root)


' coded button
Local button:TCEWindow = TCEWindowManager.CreateWindow("WindowsLook/Button", "button")
root.addChildWindow(button)

button.setText("Show")
button.setPosition(40, 100)
button.setSize(90, 20)
button.subscribeEvent(TCEPushButton.EventClicked, clicked)

' coded editbox
Local eb:TCEWindow = TCEWindowManager.CreateWindow("WindowsLook/Editbox", "editbox")
root.addChildWindow(eb)

eb.setText("Type here")
eb.setPosition(40, 130)
eb.setSize(180, 25)

' loaded window
Local window:TCEWindow = TCEWindowManager.getWindow("HelloWorldWindow")
window.subscribeEvent(TCEFrameWindow.EventCloseClicked, closed)
window.activate()

Local time:Int = MilliSecs()

SetColor(255, 255, 255)

While Not KeyDown(KEY_ESCAPE)

	Cls
	
	TCEEvent.mouseEvents(MouseX(), MouseY())
	TCESystem.renderGUI()

	Flip

Wend

cegui_cleanup()

End



Function closed:Int(args:TCEEventArgs)
	TCEWindow(args.owner).hide()
	Return True
End Function

Function clicked:Int(args:TCEEventArgs)
	Local window:TCEWindow = TCEWindowManager.getWindow("HelloWorldWindow")
	window.show()
	Return True
End Function

