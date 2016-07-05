SuperStrict

Framework bah.libsodium
Import brl.standardio


Local password:String = "Correct Horse Battery Staple"
Local keylen:Int = TBox.SeedBytes()

Local salt:Byte[TPWHash.SaltBytes()]
Local key:Byte[keylen]

TRandomBytes.Buffer(salt)

If TPWHash.Hash(key, password, salt, TPWHash.OpsLimitInteractive(), TPWHash.MemLimitInteractive(), TPWHash.AlgDefault()) Then

	Print "out of memory..."
	End
	
End If

Print "~npassword = " + password
Print "key      = " + TUtil.Bin2Hex(key, key.length)

