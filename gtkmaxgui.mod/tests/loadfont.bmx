
SuperStrict

Framework bah.gtkmaxgui

'DebugStop
Local font:TGuiFont = LoadGuiFont("sans", 10)

If font <> Null Then
	Print "font = " + font.name
Else
	Print "No font found !"
End If

