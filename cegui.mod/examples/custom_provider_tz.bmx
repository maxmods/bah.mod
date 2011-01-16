'
' Demonstrates using a custom Resource Provider :-)
'
'
SuperStrict

Framework BaH.CEGUIOpenGL
Import BRL.GLGraphics
Import BRL.GLMax2d

Graphics 800, 600, 0

HideMouse


' Create an instance of our provider
Local provider:CustomResourceProvider = New CustomResourceProvider

' Pass it into CEGUI's initialization
Init_CEGUI(New TCEOpenGLRenderer, provider)

initialiseDefaultResourceGroups()

TCESchemeManager.loadScheme("TaharezLook.scheme")

TCESystem.setDefaultFont("DejaVuSans-10")
TCESystem.setDefaultMouseCursor("TaharezLook", "MouseArrow")


Local root:TCEWindow = TCEWindowManager.loadWindowLayout("test_02.layout")
TCESystem.setGUISheet(root)


' coded button
Local button:TCEWindow = TCEWindowManager.CreateWindow("TaharezLook/Button", "button")
root.addChildWindow(button)

button.setText("Show")
button.setPosition(40, 100)
button.setSize(90, 20)
button.subscribeEvent(TCEPushButton.EventClicked, clicked)

' coded editbox
Local eb:TCEWindow = TCEWindowManager.CreateWindow("TaharezLook/Editbox", "editbox")
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


' Our custom resource provider !
Type CustomResourceProvider Extends TCEResourceProvider

	Method loadRawDataContainer(filename:String, dataContainer:TCERawDataContainer, resourceGroup:String)
		
		DebugLog "Loading file : " + filename

		Select resourceGroup
			Case "imagesets"
				filename = "../datafiles/imagesets/" + filename
			Case "fonts"
				filename = "../datafiles/fonts/" + filename
			Case "looknfeels"
				filename = "../datafiles/looknfeel/" + filename
		End Select
		
		Local stream:TStream = ReadStream(filename)
		
		Local size:Int = stream.Size()
		
		' create some memory - we will free this later
		Local data:Byte Ptr = MemAlloc(size)
		
		stream.ReadBytes(data, size)
		stream.Close()
		
		' Set size and data within container
		dataContainer.SetSize(size)
		dataContainer.SetData(data)
	End Method

	Method unloadRawDataContainer(dataContainer:TCERawDataContainer)
	
		Local data:Byte Ptr = dataContainer.GetDataPtr()
		
		If data Then
			' free our data
			MemFree(data)
			' we also need to Null out the pointer within the container
			dataContainer.SetData(Null)
		End If
	End Method

End Type


Function initialiseDefaultResourceGroups()
	' set the default resource groups to be used
	TCEImageset.setDefaultResourceGroup("imagesets")
	TCEFont.setDefaultResourceGroup("fonts")
	TCEScheme.setDefaultResourceGroup("schemes")
	TCEWidgetLookManager.setDefaultResourceGroup("looknfeels")
	TCEWindowManager.setDefaultResourceGroup("layouts")
	'TCEScriptModule.setDefaultResourceGroup("lua_scripts")
End Function