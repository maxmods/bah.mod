SuperStrict

Framework BaH.Crypto
Import BRL.StandardIO
Import BRL.textstream


' Bogus key And IV: we'd normally set these from another source.
Local key:Byte[] = IntToByteArray([0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15])
Local iv:Byte[] = IntToByteArray([1,2,3,4,5,6,7,8])
Local intext:String = LoadText("war_and_peace.txt")


Local arr:Byte[] = StringToByteArray(intext)
Local length:Int = arr.length

Local tmplen:Int

Local inbuf:Byte[]

Print "Original       : " + length + " bytes"

' +++++++++++++++++++++++++++++++++++++++
' encrypt
' +++++++++++++++++++++++++++++++++++++++

Local ds:String

Local ctx:EVP_CIPHER_CTX = New EVP_CIPHER_CTX.Create()

ctx.EncryptInit(EVP_CIPHER.bf_cbc(), Null, key, iv)

Local count:Int = 0
Const BUF_SIZE:Int = 2048 ' our block size

' size should be big enough to hold the block plus a bit more
Local outbuf:Byte[BUF_SIZE + ctx.GetBlockSize()]
Local outlen:Int


' loop all the full blocks
For Local i:Int = 0 Until length / BUF_SIZE

	If Not ctx.EncryptUpdate(outbuf, outlen, arr[count..count + BUF_SIZE], BUF_SIZE) Then
		Print "Error..."
		End
	End If

	' update input buffer offset
	count :+ BUF_SIZE
	
	' store encrypted
	inbuf :+ outbuf[0..outlen]
	
Next

' encrypt remaining part block
If length Mod BUF_SIZE Then

	If Not ctx.EncryptUpdate(outbuf, outlen, arr[count..count + length Mod BUF_SIZE], length Mod BUF_SIZE) Then
		Print "Error..."
		End
	End If
	
	' update input buffer offset
	count :+ length Mod BUF_SIZE
	
	' store encrypted
	inbuf :+ outbuf[0..outlen]

End If

' Tidy up
If Not ctx.EncryptFinal(outbuf, outlen) Then
	Print "Error..."
	End
End If

' add any padding
inbuf :+ outbuf[0..outlen]

ctx.Cleanup()


Print "Processed      : " + count
Print "Encrypted data : " + inbuf.length
Print "~n"

Local s:String

' +++++++++++++++++++++++++++++++++++++++
' decrypt
' +++++++++++++++++++++++++++++++++++++++


length = inbuf.length

Print "Input data     : " + length + " bytes"

ctx.DecryptInit(EVP_CIPHER.bf_cbc(), Null, key, iv)

outbuf = New Byte[BUF_SIZE + ctx.GetBlockSize() + 10]

count = 0

For Local i:Int = 0 Until length / BUF_SIZE

	If Not ctx.DecryptUpdate(outbuf, outlen, inbuf[count..count + BUF_SIZE], BUF_SIZE)
		Print "Error..."
		End
	End If

	s :+ String.FromBytes(outbuf, outlen)

	count :+ BUF_SIZE
	
Next

If length Mod BUF_SIZE Then

	If Not ctx.DecryptUpdate(outbuf, outlen, inbuf[count..count + length Mod BUF_SIZE], length Mod BUF_SIZE)
		Print "Error..."
		End
	End If

	s :+ String.FromBytes(outbuf, outlen)

	count :+ length Mod BUF_SIZE
End If


If Not ctx.DecryptFinal(outbuf, outLen) Then
	Print "Error..."
	End
End If

s :+ String.FromBytes(outbuf, outlen)

ctx.Cleanup()

Print "Processed      : " + count
Print "Decrypted      : " + s.length + " bytes"
Print "~n"


