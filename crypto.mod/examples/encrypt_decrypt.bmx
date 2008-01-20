SuperStrict

Framework BaH.Crypto
Import BRL.StandardIO

Local outbuf:Byte[1024]
Local outlen:Int, tmplen:Int

' Bogus key And IV: we'd normally set these from another source.
Local key:Byte[] = IntToByteArray([0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15])
Local iv:Byte[] = IntToByteArray([1,2,3,4,5,6,7,8])
Local intext:String = "Some Crypto Text"

Print "Original  : " + intext
Print "~n"

' +++++++++++++++++++++++++++++++++++++++
' encrypt
' +++++++++++++++++++++++++++++++++++++++

Local ctx:EVP_CIPHER_CTX = New EVP_CIPHER_CTX.Create()

ctx.EncryptInit(EVP_CIPHER.bf_cbc(), Null, key, iv)

If Not ctx.EncryptUpdate(outbuf, outlen, intext, intext.length) Then

	Print "Error..."
	End
	
End If

' Buffer passed to EncryptFinal() must be after data just encrypted to avoid overwriting it.
If Not ctx.EncryptFinal(Varptr(outbuf)[outlen], tmplen) Then
	Print "Error..."
	End
End If
outlen :+ tmplen

ctx.Cleanup()

Print "Encrypted : " + String.FromBytes(outbuf, outlen)
Print "~n"

' +++++++++++++++++++++++++++++++++++++++
' decrypt
' +++++++++++++++++++++++++++++++++++++++
Local inbuf:Byte[] = outbuf[..outlen]

ctx.DecryptInit(EVP_CIPHER.bf_cbc(), Null, key, iv)

If Not ctx.DecryptUpdate(outbuf, outlen, inbuf, inbuf.length) Then

	Print "Error..."
	End
	
End If

If Not ctx.DecryptFinal(Varptr(outbuf)[outlen], tmplen) Then
	Print "Error..."
	End
End If
outlen :+ tmplen

ctx.Cleanup()

Print "Decrypted : " + String.FromBytes(outbuf, outlen)
Print "~n"

