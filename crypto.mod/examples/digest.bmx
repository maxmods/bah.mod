SuperStrict

Framework BaH.Crypto
Import BRL.StandardIO

' create a new message digest context
Local ctx:EVP_MD_CTX = New EVP_MD_CTX.Create()

' initialise the context with a hash function
ctx.DigestInit(EVP_MD.sha1())

' some text to hash
Local mess1:String = "Test Message"
Local mess2:String = "Hello World"

' update the context with the first message
If Not ctx.DigestUpdate(mess1, mess1.length) Then
	Print "Error..."
	End
End If

' update the context with the second message
If Not ctx.DigestUpdate(mess2, mess2.length) Then
	Print "Error..."
	End
End If


Local buf:Byte[EVP_MAX_MD_SIZE]
Local length:Int

' perform the hash
If Not ctx.DigestFinal(buf, length) Then
	Print "Error..."
	End
End If

' output the digest
Print BytesToHex(buf, length)

' house cleaning :-)
ctx.Cleanup()

