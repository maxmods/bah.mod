SuperStrict

Framework bah.libsodium
Import brl.standardio

Local message:String = "test"
Local ciphertextLength:Int = TSecretBox.MacBytes() + message.length

Local nonce:Byte[TSecretBox.NonceBytes()]
Local key:Byte[TSecretBox.KeyBytes()]
Local ciphertext:Byte[ciphertextLength]

' fill nonce and key with random bytes
TRandomBytes.Buffer(nonce)
TRandomBytes.Buffer(key)

TSecretBox.Easy(ciphertext, message, nonce, key)

Print "message    = " + message
Print "ciphertext = " + TUtil.Bin2Hex(ciphertext, ciphertext.length)

Local decrypted:Byte[message.length]
If TSecretBox.OpenEasy(decrypted, ciphertext, nonce, key) = 0 Then
	Print "decrypted  = " + String.FromBytes(decrypted, decrypted.length)
End If