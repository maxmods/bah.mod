'
' REMEMBER : Compile as a Console app, not as GUI !
'

SuperStrict

Framework BaH.Curses
Import BRL.StandardIO

' create entry field
Local entry:TCursesEntry = TCursesEntry.Create(POS_CENTER, POS_CENTER, ..
	"<C>Type in anything you want~n<C>but the dreaded letter </B>G<!B>!", ..
	Null, A_NORMAL, ".", vMixed, 40, 0, 256)

If entry Then

	entry.setPreprocess(Null, entryPreprocessCallback)
	
	' show the entry
	Local value:String = entry.activate()

	Select entry.exitType()
	
		Case vESCAPE_HIT
		
			' remove the entry widget
			entry.free()
			
			' display a message
			popupLabel(["<C>You hit escape. No information passed back.", "", "<C>Press any key to continue."])
			
		Case vNORMAL
		
			' remove the entry widget
			entry.free()
			
			' display a message
			popupLabel(["<C>You typed in the following", "<C>" + value , "", "<C>Press any key to continue."])
	
		Default
		
			entry.free()
			
	End Select
	
End If

Function entryPreprocessCallback:Int(widget:TCursesWidget, data:Object, key:Int)

	If key = Asc("g") Or key = Asc("G") Then

		Local dialog:TCursesDialog = TCursesDialog.Create(POS_CENTER, POS_CENTER, [ "<C><#HL(30)>", ..
			"<C>I told you </B>NOT<!B> to type G", ..
			"<C><#HL(30)>"], ..
			["OK"], A_REVERSE, False, True, False)
		
		
		If dialog Then
		
			dialog.activate()
			dialog.free()
			widget.show()
	
		End If
		
		Return False
		
	End If

	Return True

End Function
