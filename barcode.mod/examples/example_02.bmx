'
' Live barcode scanner test
'
SuperStrict

Framework ofx.ofxglmax2d
Import brl.glmax2d
Import BaH.Barcode

Local window:GLMax2DWindow = New GLMax2DWindow
ofSetupOpenGL(window, 800, 600, OF_WINDOW)

ofRunApp(New TTestApp)


Type TTestApp Extends ofBaseApp

	Field mx:Int
	Field my:Int

	Field camWidth:Int
	Field camHeight:Int
	
	Field vidGrabber:ofVideoGrabber
	Field videoTexture:ofTexture
	
	Field bcResult:TBCResult
	Field points:Float[]
	Field pix:TPixmap

	Method setup()
		camWidth 		= 320	' Try To grab at this size. 
		camHeight 	= 240
		
		vidGrabber = New ofVideoGrabber
		vidGrabber.initGrabber(camWidth,camHeight)
		
		videoTexture = New ofTexture
		videoTexture.allocate(camWidth, camHeight, GL_RGB)
	End Method
		
	Method update()
		vidGrabber.grabFrame()
		
		pix = CreateStaticPixmap(vidGrabber.getPixels(), camWidth, camHeight, camWidth * 3,PF_RGB888)
		bcResult = decodeBarcode(pix)
		
		If KeyHit(key_escape) Then
			End
		End If
	End Method
	
	Method draw()
		vidGrabber.draw(20, 20)
		
		If bcResult And Not bcResult.error Then
			Local points:Float[] = bcResult.getPoints()
			If points.length = 4 Then
				SetOrigin 20, 20
				SetColor 255, 0, 0
				DrawLine points[0], points[1], points[2], points[3]
				SetColor 255, 255, 255
				SetOrigin 0, 0
				
				DrawText bcResult.getText(), 50, camHeight + 60
				
				DebugLog bcResult.getText()
			End If
		End If

	End Method
	

	
End Type



