SuperStrict

Framework bah.libsodium
Import brl.standardio


Local password:String = "Correct Horse Battery Staple"

Local hashedPassword:Byte[TPWHash.StrBytes()]

If TPWHash.HashStr(hashedPassword, password, TPWHash.OpsLimitSensitive(), TPWHash.MemLimitSensitive()) Then

	Print "out of memory..."
	End
	
End If

Local hashed:String = String.FromCString(hashedPassword)

Print "~npassword        = " + password
Print "hashed password = " + hashed

Print "~nverifying...~n"

If TPWHash.HashStrVerify(hashed, password) Then

	Print "Wrong password !"

Else

	Print "Correct password :-)"

End If

