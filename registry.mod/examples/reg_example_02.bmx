SuperStrict

Framework bah.registry
Import brl.standardio


Local key:TRegistryKey = RegOpenKey(HKEY_CURRENT_USER, "Software\Microsoft", KEY_READ)

If key Then

	Print key.path
	
	For Local sub:String = EachIn key.Keys()
		Print sub
	Next

Else
	Print "Not found..."
	
End If
