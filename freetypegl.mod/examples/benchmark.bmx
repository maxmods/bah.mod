SuperStrict

Framework brl.glmax2d
Import bah.freetypegl
Import BRL.TextStream
Import BRL.FreeTypeFont
Import brl.standardio

AppTitle = "FreeType-GL Benchmark"


Local fontPath:String = "../freetype-gl/fonts/Vera.ttf"

Local fullText:String = LoadText("monkey.txt")
Local text:String[] = fullText.Split("~n")

Graphics 800, 600, 0

SetClsColor 0, 0, 0
SetBlend AlphaBlend

Local origFont:TImageFont = LoadImageFont(fontPath, 14)


Local fontManger:TFontManager = New TFontManager.Create()

' create a font buffer object
Local fbCached:TFontBuffer = fontManger.FontBuffer(fontPath, 14)
' preload the glyphs
fbCached.LoadGlyphs(fullText)
' tell the font renderer about our dimensions
fbCached.SetViewport(0, 0, GraphicsWidth(), GraphicsHeight())

Local fbText:TFontBuffer = fontManger.FontBuffer(fontPath, 14)
fbText.LoadGlyphs(fullText)
fbText.SetViewport(0, 0, GraphicsWidth(), GraphicsHeight())


Local fps:Int
Local dispFPS:Int
Local flag:Int = 0
Local startTime:Int = MilliSecs()
Local elapsedTime:Int
Local renderTime:Int
Local cacheEmtpy:Int = True


While Not KeyDown(KEY_ESCAPE)

	Cls

	SetColor 255, 255, 255

	fps:+1
	elapsedTime = MilliSecs() - startTime
	If elapsedTime >= 1000
		dispFPS = fps
		fps = 0
		startTime = MilliSecs()
		Print dispFPS + " : " + renderTime
	End If
	
	If KeyHit(KEY_1) Then
		flag = 0
	Else If KeyHit(KEY_2) Then
		flag = 1
	Else If KeyHit(KEY_3) Then
		flag = 2
	End If
	
	SetImageFont Null
	DrawText "1 : Standard    2 : Draw Per Frame    3 : Cached Draw          FPS : " + dispFPS + "   Render (ms) : " + renderTime, 50, 10
	
	Local time:Int = MilliSecs()
	
	If (time/500) Mod 2 = 0 Then
		Select flag
			Case 0
				DrawText "_", 50, 13
			Case 1
				DrawText "_", 178, 13
			Case 2
				DrawText "_", 354, 13
		End Select
	End If
	
	
	renderTime = MilliSecs()
	Select flag
		Case 0
			SetImageFont origFont
			Local x:Int = 10
			Local y:Int = 50
			For Local n:Int = 0 Until 6
				SetColor 100 + n * 31, 100 + n * 31, 100 + n * 31
				For Local i:Int = 0 Until text.length
					DrawText text[i], x + n * 90, y + i * 16 + n * 8
				Next
			Next
		Case 1
			fbText.Clear()

			Local x:Int = 10
			Local y:Int = 50
			For Local n:Int = 0 Until 6
				fbText.SetColor 100 + n * 31, 100 + n * 31, 100 + n * 31
				For Local i:Int = 0 Until text.length
					fbText.Draw text[i], x + n * 90, y + i * 16 + n * 8
				Next
			Next

			fbText.Render()
		Case 2
			If cacheEmtpy Then
				cacheEmtpy = False
				Local x:Int = 10
				Local y:Int = 50
				For Local n:Int = 0 Until 6
					fbCached.SetColor 100 + n * 31, 100 + n * 31, 100 + n * 31
					For Local i:Int = 0 Until text.length
						fbCached.Draw text[i], x + n * 90, y + i * 16 + n * 8
					Next
				Next
			End If

			fbCached.Render()
	End Select
	
	renderTime = MilliSecs() - renderTime
	
	
	
	Flip 0

Wend

