SuperStrict

' Based on a Cairo Tutorial by Davyd Madeley
'
' part1: http://gnomejournal.org/article/34/writing-a-widget-using-cairo-and-gtk28
' part2: http://gnomejournal.org/article/36/writing-a-widget-using-cairo-and-gtk28-part-2
'
'
' Maxed by Bruce Henderson ;-)

Framework BaH.GTKMaxGUI
Import BaH.Cairo
Import BRL.Timer

Local window:TGadget = CreateWindow("Cairo Clock - Panel Pixmap",100,100,300,300, Null)
Local panel:TGadget = CreatePanel(0, 0, 256, 256, window)
Local pixmap:TPixmap = CreatePixmap(256, 256, PF_BGRA8888)

Local label:TGadget = CreateLabel("I am a label", 90, 100, 100, 20, panel)
Local combo:TGadget = CreateComboBox(65, 256, 170, 26, window)
Local style:Int = 0

AddGadgetItem(combo, "PANELPIXMAP_TILE")
AddGadgetItem(combo, "PANELPIXMAP_CENTER")
AddGadgetItem(combo, "PANELPIXMAP_FIT")
AddGadgetItem(combo, "PANELPIXMAP_STRETCH")
SelectGadgetItem(combo, style)

SetStatusText(window, "~tResize me !!!!!")

SetGadgetLayout(panel, 1, 1, 1, 1)
SetGadgetLayout(combo, 0, 0, 0, 1)


Local clock:TClock = TClock.create(pixmap)
Local timer:TTimer = CreateTimer( 1 )


clock.paint()
RedrawGadget(panel)

While True
	WaitEvent 

	Select EventID()
		Case EVENT_WINDOWCLOSE
			End
		Case EVENT_TIMERTICK
			RedrawGadget panel
		Case EVENT_GADGETPAINT
			clock.paint()
			Local ps:Int
			Select style
				Case 0
					ps = PANELPIXMAP_TILE
				Case 1
					ps = PANELPIXMAP_CENTER
				Case 2
					ps = PANELPIXMAP_FIT
				Case 3
					ps = PANELPIXMAP_STRETCH
			End Select
			' Note, due to the difference between cairo and max colorspace, we need to convert
			' the cairo'd image before sending it to the panel.
			SetPanelPixmap(panel, ConvertPixmap(pixmap, PF_RGBA8888), ps)
		Case EVENT_GADGETACTION
			style = EventData()
	End Select
	
Wend

End


Type TClock

	Const width:Int = 256
	Const height:Int = 256

	Field cairo:TCairo

	Function Create:TClock(pixmap:TPixmap)
		Local this:TClock = New TClock
		
		this.cairo = TCairo.Create(TCairoImageSurface.CreateFromPixmap(pixmap))
		
		Return this
	End Function

	Method Paint()
		
		cairo.rectangle(0, 0, width, height)
		cairo.clip()
		
		cairo.clear(1, 1, 1, width, height)
		
		draw()

	End Method

	Method draw()
		Local x:Double = width / 2.0
		Local y:Double = height / 2.0
		Local radius:Double = Min (width / 2.0, height / 2.0) - 5.0
	
		' clock back
		cairo.arc(x, y, radius, 0, 360)
		cairo.setSourceRgb(1, 1, 1)
		cairo.fillPreserve()
		cairo.setSourceRgb(0, 0, 0)
		cairo.stroke()

		cairo.SelectFontFace( "tahoma", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD)
		cairo.SetFontSize( 12 )
		
		' clock ticks
		For Local i:Int = 0 Until 12
			Local inset:Double = 0
		
			cairo.Save() ' stack-pen-size
			
			If i Mod 3 = 0 Then
				inset = 0.2 * radius
			Else
				inset = 0.1 * radius
				cairo.setLineWidth(0.5 * cairo.getLineWidth())
			End If
			
			cairo.moveTo(x + (radius - inset) * Cos(i * 30), y + (radius - inset) * Sin(i * 30))
			cairo.lineTo(x + radius * Cos(i * 30), y + radius * Sin (i * 30))
			cairo.stroke()
			cairo.restore() ' stack-pen-size
			
			' draw numbers
			inset = 0.3 * radius
			cairo.moveTo(x + (radius - inset) * Cos(i * 30) - 7, y + (radius - inset) * Sin(i * 30) + 4)
			' the circle starts at 3 o'clock so we need to adjust the digits accordingly
			Local t:Int = (i + 2) Mod 12 + 1
			cairo.ShowText( romanNumerals(t) )
		Next
		
		cairo.SelectFontFace( "arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL)
		cairo.SetFontSize( 12 )
		cairo.moveTo(x - 17, y + 30)
		cairo.ShowText("Cairo")
		
		
		' clock hands
		Local hours:Int = CurrentTime()[0..2].toInt() Mod 12
		Local minutes:Int = CurrentTime()[3..5].toInt()
		Local seconds:Int = CurrentTime()[6..8].toInt()
		
		' hour hand:
		' the hour hand is rotated 30 degrees  per hour + 1/2 a degree per minute
		'
		cairo.Save()
		cairo.setLineWidth(2.5 * cairo.getLineWidth())
		cairo.moveTo(x, y)
		cairo.lineTo(x + radius / 2 * Sin(30 * hours + minutes / 2.0), ..
				   y + radius / 2 * -Cos(30 * hours + minutes / 2.0))
		cairo.stroke()
		cairo.restore()
		
		' minute hand:
		' the minute hand is rotated 6 degrees per minute + 1/10 a degree per second
		'
		cairo.moveTo(x, y)
		cairo.lineTo(x + radius * 0.75 * Sin(6 * minutes + seconds / 10.0), ..
				   y + radius * 0.75 * -Cos(6 * minutes + seconds / 10.0))
		cairo.stroke()
		
		' seconds hand:
		' he second hand is rotated 6 degrees per second
		'
		cairo.save()
		cairo.setSourceRgb(1, 0, 0) ' red
		cairo.moveTo(x, y)
		cairo.lineTo(x + radius * 0.7 * Sin(6 * seconds), ..
				   y + radius * 0.7 * -Cos(6 * seconds))
		cairo.stroke()
		cairo.restore()
	End Method

	Method romanNumerals:String(value:Int)
		Local rn:String = ""
		
		While value > 9
			rn:+ "X"
			value:- 10
		Wend
	
		If value > 8 Then
			rn:+ "IX"
			value:- 9
		End If
		
		If value > 4 Then
			rn:+ "V"
			value:- 5
		End If
		
		If value > 3 Then
			rn:+ "IV"
			value:- 4
		End If
		
		While value > 0
			rn:+ "I"
			value:- 1
		Wend
		
		Return rn
	End Method
End Type
