SuperStrict

Framework bah.registry
Import brl.standardio



Local key:TRegistryKey = RegOpenKey(HKEY_CURRENT_USER, "Software\TestDir")

If Not key Then
	Print "Creating registry entry..."
	
	key = RegCreateKey(HKEY_CURRENT_USER, "Software\TestDir")
	
	If Not key Then
		Print "Failed to create entry."
		End
	End If
Else
	Print "Registry key exists."
End If

Print "Setting values"
key.SetString("name", "BlitzMax")
key.SetString("style", "Kind of blue")
key.SetInt("num1", 69)
key.SetLong("num2", 9876547382:Long)

Print "Values..."
For Local value:TRegistryValue = EachIn key.values()
	Print "  Value : " + value.name + " : " + value.ToString()
Next
