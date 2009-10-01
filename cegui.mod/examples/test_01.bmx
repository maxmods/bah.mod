SuperStrict

Framework BaH.CEGUIOpenGL
Import BRL.GLGraphics
Import BRL.GLMax2d

Graphics 800, 600, 0

HideMouse

' use our own logger (choice of LOG_ERRORS, LOG_WARNINGS, LOG_STANDARD, LOG_INFORMATIVE, LOG_INSANE)
New CustomLogger
TCELogger.GetLogger().setLoggingLevel(LOG_ERRORS)

Init_CEGUI(New TCEOpenGLRenderer)

TCESchemeManager.createScheme("WindowsLook.scheme")

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
eb.setSize(180, 30)

' loaded window
Local window:TCEWindow = TCEWindowManager.getWindow("HelloWorldWindow")
window.subscribeEvent(TCEFrameWindow.EventCloseClicked, closed)
window.activate()

Local time:Int = MilliSecs()

SetColor(255, 255, 255)

While Not KeyDown(KEY_ESCAPE)

	Cls
	
	TCEEvent.mouseEvents(MouseX(), MouseY(), MouseZ())
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

' a custom logger
Type CustomLogger Extends TCECustomLogger

	' does nothing - we are outputting to the debuglog
	Method setLogFilename(filename:String, append:Int = False)
	End Method

	' called for *every* event
	Method logEvent(message:String, level:Int = LOG_STANDARD)	
		If getLoggingLevel() >= level Then
			DebugLog "EVENT : " + message
		End If
	End Method

End Type

