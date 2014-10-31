SuperStrict

Framework brl.glmax2d
Import bah.freetypegl

AppTitle = "FreeType-GL Cartoon"


Local fontPath:String = "../freetype-gl/fonts/ObelixPro.ttf"


Graphics 800, 600, 0

SetClsColor 0, 0, 0
SetBlend AlphaBlend


Local fontManger:TFontManager = New TFontManager.Create(1024, 1024)

' create a font buffer object
Local fb:TFontBuffer = fontManger.FontBuffer(fontPath, 128)
' tell the font renderer about our dimensions
fb.SetViewport(0, 0, GraphicsWidth(), GraphicsHeight())

fb.SetColor(0, 0, 0)
fb.SetOutlineType(OUTLINE_INNER)
fb.SetOutlineThickness(7)
fb.Draw "Freetype GL", 50, 50

fb.SetColor(255, 255, 0)
fb.SetOutlineType(OUTLINE_INNER)
fb.SetOutlineThickness(5)
fb.Draw "Freetype GL", 50, 50

fb.SetColor(0, 0, 0)
fb.SetOutlineType(OUTLINE_LINE)
fb.SetOutlineThickness(3)
fb.Draw "Freetype GL", 50, 50

fb.SetGradientColor(255, 220, 0, 255, 255, 150, 0, 255)
fb.SetOutlineType(OUTLINE_NONE)
fb.SetOutlineThickness(0)
fb.Draw "Freetype GL", 50, 50

While Not KeyDown(KEY_ESCAPE)

	Cls

	fb.Render()

	Flip
	
Wend

