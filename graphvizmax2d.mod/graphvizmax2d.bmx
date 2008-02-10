SuperStrict

Rem
bbdoc: GraphvizMax2D
about: A Max2D Graphviz Renderer
End Rem
Module BaH.GraphvizMax2D

ModuleInfo "Version: 1.01"
ModuleInfo "License: Common Public License"
ModuleInfo "Copyright: 2007,2008 Bruce A Henderson"

ModuleInfo "History: 1.01"
ModuleInfo "History: Fixes array out of bounds error."
ModuleInfo "History: 1.00 Initial Release"


Import BRL.Max2D
Import BaH.Graphviz
Import BRL.FreeTypeFont

' Set this as the current driver...
graphvizDriver = New TGVMax2DDriver


Type TGVMax2DDriver Extends TGVDriver

	Method setPenColor(r:Double, g:Double, b:Double, a:Double)

		SetColor(255 * r, 255 * g, 255 * b)
		SetAlpha(a)
		
	End Method

	Method setFillColor(r:Double, g:Double, b:Double, a:Double)

		SetColor(255 * r, 255 * g, 255 * b)
		SetAlpha(a)
		
	End Method

	Method textPara(text:String, size:Double, x:Double, y:Double, width:Double, height:Double, Name:String, just:Int)

		Local font:TGVFont = TGVFont(graphviz.fontList.valueForKey(TGVFont.key(name, size, 0)))

		SetImageFont(TImageFont(font.imageFont))
		
		SetScale graphviz.jZoom * font.scale, graphviz.jZoom * font.scale
		
		If Not width Then
			width = TextWidth(text)
		End If
		
		' TODO : We need to take this off?
		'If height > size Then
			y:+ height - (height - size)
		'End If
		
		' text justify
		Select just
			Case 108 ' l
			Case 110 ' n
				x:+ width * -0.5
			Case 114 ' r
				x:+ width * -1.0
		End Select

		graphviz.setPenColor()

		DrawText text, graphviz.jZoom * (x - graphviz.jXOffset), graphviz.jZoom * (graphviz.jYOffset - y)

		SetScale 1, 1
	End Method

	Method ellipse(x0:Double, y0:Double, x1:Double, y1:Double, filled:Int)
				
		Local w:Int = x1 - x0
		Local h:Int = y1 - y0
		
		graphviz.setDrawStyle()
		
		If filled Then

			SetScale graphviz.jZoom, graphviz.jZoom
			
			graphviz.setFillColor()
			DrawOval graphviz.jZoom * ((x0 - w) - graphviz.jXOffset), graphviz.jZoom * (graphviz.jYOffset - (y0 + h)), w * 2, h * 2

			SetScale 1,1
		End If
		
		If graphviz.drawStyle.pen <> GV_PEN_NONE Then
		
			graphviz.setPenStyle(graphviz.drawStyle.pen)
			
			graphviz.setPenColor()
			
			drawUnfilledOval graphviz.jZoom * ((x0 - w) - graphviz.jXOffset), graphviz.jZoom * (graphviz.jYOffset - (y0 + h)), ..
				graphviz.jZoom * (w * 2), graphviz.jZoom * (h * 2)
				
			graphviz.setPenStyle(GV_PEN_SOLID)
		End If
			
	End Method

	Method polygon(xy:Float[], filled:Int)
		
		If filled Then
			graphviz.setFillColor()
			DrawPoly(xy)
		End If

		If graphviz.drawStyle.pen <> GV_PEN_NONE Then

			graphviz.setPenStyle(graphviz.drawStyle.pen)

			graphviz.setPenColor()
			drawUnfilledPoly(xy)
			
			graphviz.setPenStyle(GV_PEN_SOLID)
		End If
		
	End Method

	Method bezier(points:Double Ptr, n:Int, arrowAtStart:Int, arrowAtEnd:Int, filled:Int)

		If graphviz.drawStyle.pen <> GV_PEN_NONE Then
		
			graphviz.setPenStyle(graphviz.drawStyle.pen)
			
			graphviz.setPenColor()
		
			For Local i:Int = 0 Until (n - 1) / 3
				Local a:Int = i * 6
				drawBezier(points[a], points[a + 1], points[a + 2], points[a + 3], ..
					points[a + 4], points[a + 5], points[a + 6], points[a + 7])
			Next
			
			graphviz.setPenStyle(GV_PEN_SOLID)
		End If

	End Method
	
	' callback - draws a polyline
	Method polyline(xy:Float[])
		
		If graphviz.drawStyle.pen <> GV_PEN_NONE Then
		
			graphviz.setPenStyle(graphviz.drawStyle.pen)
			
			graphviz.setPenColor()
			
			For Local i:Int = 0 To xy.length / 4 - 1
				DrawLine xy[i * 2], xy[i * 2 + 1], xy[i * 2 + 2], xy[i * 2 + 3]
			Next
			
			graphviz.setPenStyle(GV_PEN_SOLID)
		End If
		
	End Method

	Method drawTooltip(txt:String, xy:Float[], scale:Double, style:TGVStyle)
		' fill
		If style.fcColor Then
			style.fcColor.doColor(Self, 2)
			DrawPoly(xy)
		End If
		
		' pen
		If style.pcColor Then
			style.pcColor.doColor(Self, 1)
			drawUnfilledPoly(xy)
		End If
		
		' text
		style.foColor.doColor(Self, 1)
		
		SetScale scale , scale
		
		DrawText txt, xy[0] + 4, xy[1] + 2
		
		SetScale 1,1
		
		SetColor 255, 255, 255
		SetAlpha 1
	End Method
	
	' draws an unfilled poly
	Method drawUnfilledPoly(xy:Float[])
	
		Local points:Float[] = xy[..xy.length + 2]
		points[xy.length] = xy[0]
		points[xy.length + 1] = xy[1]
	
		For Local i:Int = 0 Until xy.length / 2
			DrawLine points[i * 2], points[i * 2 + 1], points[i * 2 + 2], points[i * 2 + 3]
		Next
	
	End Method

	' draws an unfilled oval
	Method drawUnfilledOval(x:Int, y:Int, width:Int, height:Int)

		Global points:Float[]
		
		If Not points Then
			points = New Float[74]
		End If
	
		Local rh:Int = height / 2
		Local rw:Int = width / 2
		For Local n:Int = 0 To 36
			points[n * 2] = rw + x + rw * Cos(n * 10)
			points[n * 2 + 1] = rh + y + rh * Sin(n * 10)
		Next
		
		For Local i:Int = 0 Until 36
			DrawLine points[i * 2], points[i * 2 + 1], points[i * 2 + 2], points[i * 2 + 3]
		Next
		
	End Method
	
	' draw a bezier curve
	Method drawBezier(ax#,ay#,bx#,by#,cx#,cy#,dx#,dy#)
	
		Local ox:Float = ax
		Local oy:Float = ay
		bx:* 3
		by:* 3
		cx:* 3
		cy:* 3
		
		Local t:Float
		Local count:Int
		
		While count < 10
			count:+ 1
			t:+ .1

			Local b:Float = 1.0 - t
			Local bsqr:Float = b * b
			Local asqr:Float = t * t
			Local x:Float = ax * bsqr * b + bx * bsqr * t + cx * b * asqr + dx * asqr * t
			Local y:Float = ay * bsqr * b + by * bsqr * t + cy * b * asqr + dy * asqr * t
			
			DrawLine graphviz. jZoom * (ox - graphviz. jXOffset), graphviz. jZoom * (graphviz. jYOffset - oy), ..
				graphviz. jZoom * (x - graphviz. jXOffset), graphviz. jZoom * (graphviz. jYOffset - y)
			ox=x
			oy=y
		Wend

	End Method

	Method getFontMetrics(font:TGVFont, txt:String, width:Double Var, height:Double Var)

		SetImageFont(TImageFont(font.imageFont))
		width = TextWidth(txt) * font.scale
		height = TextHeight(txt) * font.scale

	End Method

	Method textSize(txt:String, font:TGVFont, width:Int Var, height:Int Var, scale:Float Var)

		SetImageFont(TImageFont(font.imageFont))
		scale = font.scale
		width = TextWidth(txt) * scale
		height = TextHeight(txt) * scale
	
	End Method

	Method currentSettings(originX:Float Var, originY:Float Var, r:Int Var, g:Int Var, b:Int Var, a:Float Var, ..
			vpX:Int Var, vpY:Int Var, vpW:Int Var, vpH:Int Var)

		GetOrigin originX, originY
		GetColor r, g, b
		a = GetAlpha()

		GetViewport vpX, vpY, vpW, vpH

	End Method

	Method setViewPortAndOrigin(vpX:Int, vpY:Int, vpW:Int, vpH:Int, originX:Int, originY:Int)

		SetViewport vpX, vpY, vpW, vpH
		SetOrigin originX, originY
		
	End Method

	Method setSettings(originX:Float, originY:Float, r:Int, g:Int, b:Int, a:Float, ..
			vpX:Int, vpY:Int, vpW:Int, vpH:Int)

		' set the origin
		SetOrigin originX, originY
		' set viewport
		SetViewport vpX, vpY, vpW, vpH

		' set color
		SetColor(r, g, b)
		' set alpha
		SetAlpha a

	End Method
	
	Method loadFont:TGVFont(name:String, size:Float, flags:Int)
		Return New TGVMax2DFont.loadFont(name, size, flags)
	End Method

	Method GetOriginXY(originX:Float Var, originY:Float Var)
		GetOrigin originX, originY
	End Method

End Type

Type TGVMax2DFont Extends TGVFont

	Method Init()
		
		Local paths:String
		
		If TGVGraphviz.userFontDirs Then
			paths = TGVGraphviz.userFontDirs + kPATHSEPARATOR + kDEFAULT_FONTPATH
		Else
			paths = kDEFAULT_FONTPATH
		End If
		
		Local fname:String = alternateFontList(name)
		
		If Not fname Then
			fname = name
		End If
		
		fontPath = getFontPath(paths, fname)
		
		If fontPath Then
			fpc = fontPath.toCString()
			Local smooth:Int = SMOOTHFONT
			
			'If size <= 8 Then
			'	smooth = 0
			'End If
			scale = GV_FONT_SCALE
			imageFont = LoadImageFont(fontPath, size / scale, smooth)
		Else
			' font not found, so we'll be using the default font...
			' no scaling...
			scale = 1
			fontPath = ""
			imageFont = Null
			fpc = TGVGraphviz.fp ' important... otherwise graphviz thinks there's no font
		End If
		
	End Method

	Method alternateFontList:String(fname:String)
		Local fontList:String
		
		fname = fname.toLower()
		
		Local fontNoSep:String = ""
		
		If fname.find("-") > 0 Then
			fontNoSep = fname[0..fname.find("-")]
		Else If fname.find("_") > 0 Then
			fontNoSep = fname[0..fname.find("_")]
		Else
			fontNoSep = fname
		End If
		
		If fname = "times-bold" Or fontNoSep = "timesbd" Or fontNoSep = "timesb" Then
			fontList = "timesbd;Timesbd;TIMESBD;timesb;Timesb;TIMESB;VeraSeBd;Times"
		Else If fname = "times-italic" Or fontNoSep = "timesi" Then
			fontList = "timesi;Timesi;TIMESI;VeraIt;Times"
		Else If fname = "timesnewroman" Or fname = "timesnew" Or fname = "timesroman" Or fontNoSep = "times" Then
			fontList = "times;Times;TIMES;VeraSe"
		Else If fname = "arial-bold" Or fontNoSep = "arialb" Or fontNoSep = "arialbd" Then
			fontList = "arialb;Arialb;ARIALB;arialbd;Arialbd;ARIALBD;VeraBd;Helvetica"
		Else If fname = "arial-italic" Or fontNoSep = "ariali" Then
			fontList = "ariali;Ariali;ARIALI;VeraIt;Helvetica"
		Else If fname = "arial-bold-italic" Or fontNoSep = "arialbi" Then
			fontList = "arialbi;Arialbi;ARIALBI;VeraBI;Helvetica"
		Else If fontNoSep = "helvetica" Then
			fontList = "helvetica;Helvetica;HELVETICA;arial;Arial;ARIAL;Vera"
		Else If fontNoSep = "arial" Then
			fontList = "arial;Arial;ARIAL;Vera;helvetica;Helvetica;HELVETICA"
		Else If fontNoSep = "courier" Then
			fontList = "courier;Courier;COURIER;cour;VeraMono"
		End If
		
		Return fontList
	End Method
	
	Method getFontPath:String(paths:String, fontList:String)
	
		Local fontNames:String[] = fontList.Split(";")
		Local pathNames:String[] = paths.Split(kPATHSEPARATOR)
		Local fullname:String
		Local found:Int = False
		
		For Local f:Int = 0 Until fontNames.length
		
		
			For Local p:Int = 0 Until pathNames.length
			
				Local baseName:String = pathNames[p] + "/" + fontNames[f]
				
				' already has a suffix ?
				If fontNames[f].find(".") > 0 Then
					
					If FileType(baseName) Then
						Return baseName
					End If
					
					' try next path...
					Continue
				End If
			
				fullName = baseName + ".ttf"

				If FileType(fullName) Then
					Return fullName
				End If

				fullName = baseName + ".pfa"
				If FileType(fullName) Then
					Return fullName
				End If

				fullName = baseName + ".pfb"
				If FileType(fullName) Then
					Return fullName
				End If

				fullName = baseName + ".dfont"
				If FileType(fullName) Then
					Return fullName
				End If
		
			Next
		
		Next
	End Method

End Type

