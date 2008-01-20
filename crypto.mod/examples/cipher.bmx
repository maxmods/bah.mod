SuperStrict

Framework BaH.Crypto
Import BRL.StandardIO
Import BRL.Stream

Local orig:TStream = OpenStream("some_text.txt", True, False)
Local encrypted:TStream = OpenStream("encrypted.bin", False, True)


doCrypt(orig, encrypted, 1)

CloseStream(orig)
CloseStream(encrypted)

encrypted = OpenStream("encrypted.bin", True, False)
Local unencrypted:TStream = OpenStream("new_some_text.txt", False, True)

doCrypt(encrypted, unencrypted, 0)

CloseStream(encrypted)
CloseStream(unencrypted)


Function doCrypt:Int(in:TStream, out:TStream, encrypt:Int)

	Local inbuf:Byte[1024]
	' Allow enough space in output buffer for additional block
	Local outbuf:Byte[1024 + EVP_MAX_BLOCK_LENGTH]
	Local inLen:Int, outlen:Int
	
	Local key:String = "0123456789"
	Local iv:String = "12345678"
	
	Local ctx:EVP_CIPHER_CTX = New EVP_CIPHER_CTX.Create()

	' Don't set key or IV because we will modify the parameters
	ctx.CipherInit(EVP_CIPHER.rc2_ofb(), Null, Null, Null, encrypt)
	ctx.SetKeyLength(10)
	' We finished modifying parameters so now we can set key and IV
	ctx.CipherInit(Null, Null, StringToByteArray(key), StringToByteArray(iv), encrypt)

	While True
		
		inlen = in.Read(inbuf, 1024)
		If inlen = 0 Then
			Exit
		End If
		
		If Not ctx.CipherUpdate(outbuf, outlen, inbuf, inlen)
			' Error */
			ctx.Cleanup()
			Return 0
		End If

		out.WriteBytes(outbuf, outlen)

	Wend

	If Not ctx.CipherFinal(outbuf, outlen) Then
		' Error 
		ctx.Cleanup()
		Return 0
	End If
	
	out.WriteBytes(outbuf, outlen)

	ctx.Cleanup()
	Return 1	

End Function

