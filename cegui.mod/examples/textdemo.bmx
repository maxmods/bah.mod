SuperStrict

Framework BaH.CEGUIOpenGL
Import BRL.GLGraphics
Import BRL.GLMax2d

Import "examples_base.bmx"

Graphics 800, 600, 0

HideMouse

New CustomLogger
TCELogger.GetLogger().setLogFilename("test2.log")

' Initialize CEGUI !
Init_CEGUI(New TCEOpenGLRenderer)


Local app:MyApp = New MyApp
app.run()


Type MyApp Extends CEGuiBaseApplication

	Method initialise:Int()

		' load scheme and set up defaults
		TCESchemeManager.createScheme("TaharezLook.scheme")
		TCESystem.setDefaultMouseCursor("TaharezLook", "MouseArrow")

		' We need a font
		TCEFontManager.createFont("DejaVuSans-10.font")
		
		'  Font defaulting
		If TCEFontManager.isDefined("DejaVuSans-10") Then
			TCESystem.setDefaultFont("DejaVuSans-10")
		End If
		
		' Load an image To use as a background
		TCEImagesetManager.createImagesetFromImageFile("BackgroundImage", "GPN-2000-001437.tga")
		
		' here we will use a StaticImage as the root, Then we can use it To place a background image
		Local background:TCEWindow = TCEWindowManager.CreateWindow("TaharezLook/StaticImage", "background_wnd")
		
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
		background.addChild(TCEWindowManager.loadWindowLayout("TextDemo.layout"))
		
		' Init the seperate blocks which make up this sample
		initStaticText()
		initSingleLineEdit()
		initMultiLineEdit()
		
		' Quit button
		subscribeEvent("TextDemo/Quit", TCEPushButton.EventClicked, quitDemo);

		Return True
	End Method


	Method initStaticText()
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
	End Method
	
	Method initSingleLineEdit()
	    ' Only accepts digits For the age Field
	    If TCEWindowManager.isWindowPresent("TextDemo/editAge") Then
	        TCEEditbox(TCEWindowManager.getWindow("TextDemo/editAge")).setValidationString("[0-9]*")
	    End If
	
	    ' Set password restrictions
	    If TCEWindowManager.isWindowPresent("TextDemo/editAge") Then
	        Local passwd:TCEEditbox = TCEEditbox(TCEWindowManager.getWindow("TextDemo/editPasswd"))
	        passwd.setValidationString("[A-Za-z0-9]*")
	        ' Render masked
	        passwd.setTextMasked(True)
	    End If
	
	End Method
	
	Method initMultiLineEdit()
	    ' Scrollbar checkbox
	    subscribeEvent("TextDemo/forceScroll", TCECheckbox.EventCheckStateChanged, vertScrollChangedHandler)
	End Method
	
	Method initRadio(radio:String, group:Int, selected:Int)
	    If TCEWindowManager.isWindowPresent(radio) Then
	        Local button:TCERadioButton = TCERadioButton(TCEWindowManager.getWindow(radio))
	        button.setGroupID(group)
	        button.setSelected(selected)
	    End If
	End Method



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
	
	Function vertScrollChangedHandler:Int(args:TCEEventArgs)
	    If TCEWindowManager.isWindowPresent("TextDemo/editMulti") Then
	        Local multiEdit:TCEMultiLineEditbox = TCEMultiLineEditbox(TCEWindowManager.getWindow("TextDemo/editMulti"))
	        ' Use setter For a change
	        multiEdit.setShowVertScrollbar(isCheckboxSelected("TextDemo/forceScroll"))
	    End If
	
	    ' event was handled
	    Return True
	End Function
	
	Function quitDemo:Int(args:TCEEventArgs)
		quitting = True
	
	    ' event was handled
	    Return True
	End Function
	
End Type

' a custom logger
Type CustomLogger Extends TCECustomLogger

	Field stream:TStream

	' does nothing - we are outputting to the debuglog
	Method setLogFilename(filename:String, append:Int = False)
		If stream Then
			CloseStream(stream)
		End If
		
		stream = WriteStream(filename)
	End Method

	' called for *every* event
	Method logEvent(message:String, level:Int = LOG_STANDARD)
		If stream Then
			stream.WriteLine("EVENT : " + message)
		End If
	End Method

End Type
