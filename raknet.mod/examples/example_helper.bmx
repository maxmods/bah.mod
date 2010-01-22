'
' Don't run me. I'm imported by some of the examples.
'
'
SuperStrict

Import BaH.CEGUIOpenGL
Import BRL.Max2D
Import BRL.GLMax2D
Import BRL.LinkedList
Import BRL.FreeTypeFont

Global _helper:TExampleHelper

Type TExampleHelper

	Const MAX_MESSAGES:Int = 41

	Field messages:TList = New TList
	Field quit:Int
	
	Field helperWindow:TCEWindow
	Field helperLabel:TCEWindow
	Field helperText:TCEEditbox
	Field helperSubmit:TCEPushButton
	
	Function GetInstance:TExampleHelper()
		Return _helper
	End Function
	
	Method Run()
		_helper = Self

		Init()
		MainLoop()
	End Method
	
	Method Init()
		Graphics 800, 600, 0
		SetBlend ALPHABLEND
		HideMouse

		SetImageFont(LoadImageFont("datafiles/fonts/DejaVuSans.ttf", 11))

		Init_CEGUI(New TCEOpenGLRenderer)
		
		Local rp:TCEDefaultResourceProvider = TCEDefaultResourceProvider(TCESystem.getResourceProvider())
		rp.setResourceGroupDirectory("schemes", "datafiles/schemes/")
		rp.setResourceGroupDirectory("imagesets", "datafiles/imagesets/")
		rp.setResourceGroupDirectory("fonts", "datafiles/fonts/")
		rp.setResourceGroupDirectory("layouts", "datafiles/layouts/")
		rp.setResourceGroupDirectory("looknfeels", "datafiles/looknfeel/")

		TCEImageset.setDefaultResourceGroup("imagesets")
		TCEFont.setDefaultResourceGroup("fonts")
		TCEScheme.setDefaultResourceGroup("schemes")
		TCEWidgetLookManager.setDefaultResourceGroup("looknfeels")
		TCEWindowManager.setDefaultResourceGroup("layouts")
		
		TCESchemeManager.createScheme("WindowsLook.scheme")
		
		If Not TCEFontManager.isDefined( "DejaVuSans-10" ) Then
			TCEFontManager.createFont( "DejaVuSans-10.font" )
		End If

		TCESystem.setDefaultMouseCursor("WindowsLook", "MouseArrow")

		Local root:TCEWindow = TCEWindowManager.loadWindowLayout("helper.layout")
		TCESystem.setGUISheet(root)
		
		helperWindow = TCEWindowManager.getWindow("HelperWindow")
		helperWindow.Hide()
		
		helperLabel = TCEWindowManager.getWindow("Helper/label")
		helperText = TCEEditbox(TCEWindowManager.getWindow("Helper/Editbox"))
		helperSubmit = TCEPushButton(TCEWindowManager.getWindow("Helper/Submit"))
		
		helperSubmit.subscribeEvent(TCEPushButton.EventClicked, submitted) ' click
		helperText.subscribeEvent(TCEEditbox.EventTextAccepted, submitted) ' enter
	End Method
	
	Function submitted:Int(args:TCEEventArgs)
		_helper.helperWindow.hide()
		_helper.OnSubmit(_helper.helperText.GetText())
		Return True
	End Function

	
	Method AddMessage(message:String)
		If messages.Count() > MAX_MESSAGES Then
			messages.RemoveFirst()
		End If
		
		messages.AddLast(message)
	End Method
	

	Method MainLoop()
		While Not quit
			If KeyHit(KEY_ESCAPE) Then
				AddMessage("Quitting.")
				quit = True
			End If

			Update()
			TCEEvent.mouseEvents(MouseX(), MouseY(), MouseZ())
			Render()
		Wend
		
		Delay 1000
		
		Finalize()
		cegui_cleanup()
	End Method

	Method Update() Abstract
	Method RenderExtra() Abstract
	Method Finalize() Abstract
	Method OnSubmit(value:String) Abstract

	Method Render()
		Cls
		
		DrawMessages()
		
		DrawText "ESC to Quit", 700, 580
		
		RenderExtra()
		
		TCESystem.renderGUI()
		Flip
	End Method
	
	Method DrawMessages()
		Local i:Int = 0
		For Local s:String = EachIn messages
			DrawText s, 10, i * 14
			i:+ 1
		Next
	End Method

	Method RequestValue(title:String, label:String)
		helperWindow.setText(title)
		helperLabel.setText(label)

		helperText.setText("")

		helperWindow.show()
		
		helperText.activate()
	End Method

End Type

