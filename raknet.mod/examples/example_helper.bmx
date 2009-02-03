'
' Don't run me. I'm imported by some of the examples.
'
'
SuperStrict

Import BaH.CEGUI
Import BRL.Max2D
Import BRL.GLMax2D
Import BRL.LinkedList

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

		Init_CEGUI()
		
		TCESchemeManager.loadScheme("datafiles/schemes/WindowsLook.scheme")
		
		If Not TCEFontManager.isFontPresent( "DejaVuSans-10" ) Then
			TCEFontManager.createFont( "datafiles/fonts/DejaVuSans-10.font" )
		End If

		TCESystem.setDefaultMouseCursor("WindowsLook", "MouseArrow")

		Local root:TCEWindow = TCEWindowManager.loadWindowLayout("datafiles/layouts/helper.layout")
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
		
		Local i:Int = 0
		For Local s:String = EachIn messages
			DrawText s, 10, i * 14
			i:+ 1
		Next
		
		DrawText "ESC to Quit", 700, 580
		
		RenderExtra()
		
		TCESystem.renderGUI()
		Flip
	End Method

	Method RequestValue(title:String, label:String)
		helperWindow.setText(title)
		helperLabel.setText(label)

		helperText.setText("")

		helperWindow.show()
		
		helperText.activate()
	End Method

End Type

