'
' Looks up password for specific site and account.
' If it cannot find it, it adds a new entry.
'
' The first time you run this, it should add the password.
' The second and subsequent times, it should retrieve the password from the default keychain.
'
SuperStrict

Framework BaH.Keychain
Import BRL.StandardIO

' open the default keychain
Local kc:TKeychain = New TKeychain.Open()

' display its path
Print "Path to default Keychain = " + kc.GetPath()

' find a password for a given ftp site
Local password:String = kc.FindInternetPassword(kSecProtocolTypeFTP, "ftp.somesite.com", "myusername", "", kSecAuthenticationTypeAny)

' check for error
If TKeychain.LastError() Then
	Print "Error : " + TKeychain.ErrorMessage(TKeychain.LastError())

	' not found?  Let's create one!
	If TKeychain.LastError() = errSecItemNotFound Then
		password = "Bl1tzm4x R0kz!"
		
		Print "Adding password for site"
		kc.AddInternetPassword(password, kSecProtocolTypeFTP, "ftp.somesite.com", "myusername", "", kSecAuthenticationTypeDefault)
		
		If TKeychain.LastError() Then
			Print "Error : " + TKeychain.ErrorMessage(TKeychain.LastError())
			End
		End If
		
	End If
	
End If

Print "The password = " + password
