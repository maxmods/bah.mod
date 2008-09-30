SuperStrict

Framework BaH.CEGUI
Import BRL.GLGraphics
Import BRL.GLMax2d

Graphics 800, 600, 0

HideMouse

'New CustomLogger

' Initialize CEGUI !
Init_CEGUI()

' load scheme and set up defaults
TCESchemeManager.loadScheme("TaharezLook.scheme")
TCESystem.setDefaultMouseCursor("TaharezLook", "MouseArrow")

' We need a font
If Not TCEFontManager.isFontPresent("DejaVuSans-10") Then
	TCEFontManager.createFont("DejaVuSans-10.font")
End If

'  Font defaulting
If TCEFontManager.isFontPresent("DejaVuSans-10") Then
	TCESystem.setDefaultFont("DejaVuSans-10")
Else If TCEFontManager.isFontPresent("Commonwealth-10") Then
	TCESystem.setDefaultFont("Commonwealth-10")
End If

' Load an image To use as a background
TCEImagesetManager.createImagesetFromImageFile("BackgroundImage", "../datafiles/imagesets/GPN-2000-001437.tga")

' here we will use a StaticImage as the root, Then we can use it To place a background image
Local background:TCEWindow = TCEWindowManager.createWindow("TaharezLook/StaticImage", "background_wnd")

' set position And size
background.setPosition(0, 0)
background.setSize(1, 1)

' disable frame And standard background
background.setProperty("FrameEnabled", "false")
background.setProperty("BackgroundEnabled", "false")

' set the background image
background.setProperty("Image", "set:BackgroundImage image:full_image")

' install this as the root GUI sheet
TCESystem.setGUISheet(background)

' Load our layout as a basic
'DebugStop
background.addChildWindow(TCEWindowManager.loadWindowLayout("TextDemo.layout"))

' Init the seperate blocks which make up this sample
initStaticText()
initSingleLineEdit()
initMultiLineEdit()



SetColor(255, 255, 255)

While Not KeyDown(KEY_ESCAPE)

	Cls
	
	TCEEvent.mouseEvents(MouseX(), MouseY())
	TCESystem.renderGUI()

	Flip

Wend

cegui_cleanup()

End



Function initStaticText()
    ' Name, Group, Selected
    initRadio("TextDemo/HorzLeft", 0, True)
    initRadio("TextDemo/HorzRight", 0, False)
    initRadio("TextDemo/HorzCentered", 0, False)
    ' New group!
    initRadio("TextDemo/VertTop", 1, True)
    initRadio("TextDemo/VertBottom", 1, False)
    initRadio("TextDemo/VertCentered", 1, False)
    '
    ' Events
    '
    ' Word-wrap checkbox 
    subscribeEvent("TextDemo/Wrap", TCECheckbox.EventCheckStateChanged, formatChangedHandler)
    subscribeEvent("TextDemo/HorzLeft", TCERadioButton.EventSelectStateChanged, formatChangedHandler)
    subscribeEvent("TextDemo/HorzRight", TCERadioButton.EventSelectStateChanged, formatChangedHandler)
    subscribeEvent("TextDemo/HorzCentered", TCERadioButton.EventSelectStateChanged, formatChangedHandler)
    subscribeEvent("TextDemo/VertTop", TCERadioButton.EventSelectStateChanged, formatChangedHandler)
    subscribeEvent("TextDemo/VertBottom", TCERadioButton.EventSelectStateChanged, formatChangedHandler)
    subscribeEvent("TextDemo/VertCentered", TCERadioButton.EventSelectStateChanged, formatChangedHandler)
End Function

Function initSingleLineEdit()
	' TODO
End Function

Function initMultiLineEdit()
	' TODO
End Function

Function initRadio(radio:String, group:Int, selected:Int)
    If TCEWindowManager.isWindowPresent(radio) Then
        Local button:TCERadioButton = TCERadioButton(TCEWindowManager.getWindow(radio))
        button.setGroupID(group)
        button.setSelected(selected)
    End If
End Function


Function isRadioSelected:Int(radio:String)
    ' Check
    If TCEWindowManager.isWindowPresent(radio) Then
        Local button:TCERadioButton = TCERadioButton(TCEWindowManager.getWindow(radio))
        Return button.isSelected()
    End If

    Return False
End Function


Function subscribeEvent(widget:String, event:String, cb:Int(args:TCEEventArgs))
    If TCEWindowManager.isWindowPresent(widget) Then
        Local window:TCEWindow = TCEWindowManager.getWindow(widget)
        window.subscribeEvent(event, cb)
    End If
End Function

Function isCheckboxSelected:Int(checkbox:String)
    ' Check
    If TCEWindowManager.isWindowPresent(checkbox) Then
        Local button:TCECheckbox = TCECheckbox(TCEWindowManager.getWindow(checkbox))
        Return button.isSelected()
    End If

    Return False
End Function


Function formatChangedHandler:Int(args:TCEEventArgs)
    If TCEWindowManager.isWindowPresent("TextDemo/StaticText") Then
        ' And also the static text For which we will set the formatting options
        Local st:TCEWindow = TCEWindowManager.getWindow("TextDemo/StaticText")

        ' handle vertical formatting settings
        If isRadioSelected("TextDemo/VertTop") Then
            st.setProperty("VertFormatting", "TopAligned")
        Else If isRadioSelected("TextDemo/VertBottom") Then
            st.setProperty("VertFormatting", "BottomAligned")
        Else If isRadioSelected("TextDemo/VertCentered") Then
            st.setProperty("VertFormatting", "VertCentred")
	    End If

        ' handle horizontal formatting settings
        Local wrap:Int = isCheckboxSelected("TextDemo/Wrap")

        If isRadioSelected("TextDemo/HorzLeft") Then
			If wrap Then
				st.setProperty("HorzFormatting", "WordWrapLeftAligned")
			Else
				st.setProperty("HorzFormatting", "LeftAligned")
			End If
        Else If isRadioSelected("TextDemo/HorzRight") Then
			If wrap Then
	            st.setProperty("HorzFormatting", "WordWrapRightAligned")
			Else
	            st.setProperty("HorzFormatting", "RightAligned")
			End If
        Else If isRadioSelected("TextDemo/HorzCentered") Then
			If wrap Then
	            st.setProperty("HorzFormatting", "WordWrapCentred")
			Else
	            st.setProperty("HorzFormatting", "WordWrapCentred")
			End If
		End If
    End If

    ' event was handled
    Return True

End Function


' a custom logger
Type CustomLogger Extends TCECustomLogger

	' does nothing - we are outputting to the debuglog
	Method setLogFilename(filename:String, append:Int = False)
	End Method

	' called for *every* event
	Method logEvent(message:String, level:Int = LOG_STANDARD)	
		DebugLog "EVENT : " + message
	End Method

End Type
