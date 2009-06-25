SuperStrict

Framework BaH.libnotify
Import BRL.StandardIO

If TNotify.Initialize("My BlitzMax App") Then

	Local name:String, vendor:String, version:String, specVersion:String

	If TNotify.GetServerInfo(name, vendor, version, specVersion) Then
	
		Print "Name     : " + name
		Print "Vendor   : " + vendor
		Print "Version  : " + version
		Print "Spec Ver : " + specVersion
	
	Else
		Print "Server Information unavailable..."
	End If

End If


