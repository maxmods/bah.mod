' Copyright (c) 2016 Bruce A Henderson
' 
' Permission to use, copy, modify, and/or distribute this software for any purpose
' with or without fee is hereby granted, provided that the above copyright notice
' and this permission notice appear in all copies.
' 
' THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
' REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
' FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
' INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
' OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
' TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
' THIS SOFTWARE.
'
SuperStrict

Rem
bbdoc: Sodium Crypto Library
End Rem
Module bah.libsodium

ModuleInfo "Version: 1.00"
ModuleInfo "License: ISC"
ModuleInfo "Copyright: libsodium - 2013-2016 Frank Denis"
ModuleInfo "Copyright: Wrapper - 2016 Bruce A Henderson"

ModuleInfo "History: 1.00 Initial Release"

?bmxng


Import "common.bmx"


If sodium_init() = -1 Then
	Throw "Sodium initialisation error."
End If


Type TUtil

	Rem
	bbdoc: 
	End Rem
	Function Bin2Hex:String(Bin:Byte Ptr, binlen:Int)
		Local buf:Byte[binlen * 2 + 1]
		
		sodium_bin2hex(buf, Size_T(buf.length), Bin, Size_T(binlen))
		
		Return String.FromCString(buf)
	End Function

End Type



Rem
bbdoc: Random data generation.
about: A set of functions to generate unpredictable data, suitable for creating secret keys.
End Rem
Type TRandomBytes

	Rem
	bbdoc: Returns an unpredictable value between 0 and $ffffffff
	End Rem
	Function Random:UInt()
		Return randombytes_random()
	End Function
	
	Rem
	bbdoc: Returns an unpreditable value between 0 upperBound (excluded).
	about: 
	End Rem
	Function Uniform:UInt(upperBound:UInt)
		Return randombytes_uniform(upperBound)
	End Function
	
	Rem
	bbdoc:
	End Rem
	Function Buffer(buf:Byte[])
		randombytes_buf(buf, Size_T(buf.length))
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function Buffer(buf:Byte Ptr, size:Int)
		randombytes_buf(buf, Size_T(size))
	End Function
	

End Type


Rem
bbdoc: Computes a fixed-length fingerprint for an arbitrary long message.
about: Sample use cases :
File integrity checking.
Creating unique identifiers to index aribtrary long data.
NOTE : This algorithm is not suitable for hashing passwords.
End Rem
Type TGenericHash
	
	Rem
	bbdoc: Returns the minimum recommended output size.
	about: This size makes it practically impossible for two messages to produce the same fingerprint.
	End Rem
	Function Bytes:Size_T()
		Return crypto_generichash_bytes()
	EndFunction
	
	Rem
	bbdoc: 
	End Rem
	Function KeyBytes:Size_T()
		Return crypto_generichash_keybytes()
	End Function
	
	Rem
	bbdoc: Puts a fingerprint of the message @in into @out.
	about: The minimum recommended output size is #Bytes().
	End Rem
	Function Hash:Int(out:Byte Ptr, outlen:Int, in:Byte Ptr, inlen:Int, key:Byte Ptr = Null, keylen:Int = 0)
		If key Then
			Return crypto_generichash(out, Size_T(outlen), in, ULong(inlen), key, Size_T(keylen))
		Else
			Return crypto_generichash(out, Size_T(outlen), in, ULong(inlen), Null, 0)
		End If
	End Function

	Rem
	bbdoc: Puts a fingerprint of the message @in into @out.
	about: The minimum recommended output size is #Bytes().
	End Rem
	Function Hash:Int(out:Byte Ptr, outlen:Int, in:String, key:String = Null)
		Local res:Int
	
		Local i:Byte Ptr = in.ToUTF8String()
		Local inlen:Int = strlen_(i)
	
		If key Then
			Local k:Byte Ptr = key.ToUTF8String()
			Local keylen:Int = strlen_(k)
			
			res = crypto_generichash(out, Size_T(outlen), i, ULong(inlen), k, Size_T(keylen))
			
			MemFree(k)
		Else
			res = crypto_generichash(out, Size_T(outlen), i, ULong(inlen), Null, 0)
		End If
		
		MemFree(i)
		
		Return res
	End Function
	
	Rem
	bbdoc: Puts a fingerprint of the message @in into @out.
	about: The minimum recommended output size is #Bytes().
	End Rem
	Function Hash:Int(out:Byte Ptr, outlen:Int, in:String, key:Byte Ptr, keylen:Int)
		Local res:Int
	
		Local i:Byte Ptr = in.ToUTF8String()
		Local inlen:Int = strlen_(i)
	
		If key Then
			res = crypto_generichash(out, Size_T(outlen), i, ULong(inlen), key, Size_T(keylen))
		Else
			res = crypto_generichash(out, Size_T(outlen), i, ULong(inlen), Null, 0)
		End If
		
		MemFree(i)
		
		Return res
	End Function
	
End Type

Rem
bbdoc: Computes a fixed-length fingerprint for an arbitrary long message.
about: This multipart streaming API is especially useful to process very large files and datastreams.
NOTE : This algorithm is not suitable for hashing passwords.
End Rem
Type TGenericMultipartHash

	Field statePtr:Byte Ptr
	
	Rem
	bbdoc: 
	End Rem
	Method New(key:String, outlen:Int)
		statePtr = MemAlloc(crypto_generichash_statebytes())
		
		If key Then
			Local k:Byte Ptr = key.ToUTF8String()
			Local keylen:Int = strlen_(k)
		
			crypto_generichash_init(statePtr, k, Size_T(keylen), Size_T(outlen))
			
			MemFree(k)
		Else
			crypto_generichash_init(statePtr, Null, 0, Size_T(outlen))
		End If
	End Method

	Rem
	bbdoc: 
	End Rem
	Method New(key:Byte Ptr, keylen:Int, outlen:Int)
		statePtr = MemAlloc(crypto_generichash_statebytes())
		
		If key Then
			crypto_generichash_init(statePtr, key, Size_T(keylen), Size_T(outlen))
		Else
			crypto_generichash_init(statePtr, Null, 0, Size_T(outlen))
		End If
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method HashUpdate:Int(in:Byte Ptr, inlen:Int)
		Return crypto_generichash_update(statePtr, in, ULong(inlen))
	End Method

	Rem
	bbdoc: 
	End Rem
	Method HashUpdate:Int(in:String)
		Local k:Byte Ptr = in.ToUTF8String()
		Local inlen:Int = strlen_(k)

		Local res:Int = crypto_generichash_update(statePtr, k, ULong(inlen))
		
		MemFree(k)
		Return res
	End Method
	
	Rem
	bbdoc: 
	End Rem
	Method HashFinal:Int(out:Byte Ptr, outlen:Int)
		Return crypto_generichash_final(statePtr, out, Size_T(outlen))
	End Method
	
	Method Delete()
		If statePtr Then
			MemFree statePtr
			statePtr = Null
		End If
	End Method

End Type


Rem
bbdoc: 
End Rem
Type TPWHash

	Rem
	bbdoc: 
	End Rem
	Function SaltBytes:Size_T()
		Return crypto_pwhash_saltbytes()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function AlgDefault:Int()
		Return crypto_pwhash_alg_default()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function StrBytes:Int()
		Return crypto_pwhash_strbytes()
	End Function

	Rem
	bbdoc: Derives an @outlen bytes long key from a password with a salt whose fixed length is #SaltBytes() bytes.
	about: @outlen should be at least 16 (128 bits).
	End Rem
	Function Hash:Int(out:Byte Ptr, outlen:Int, password:Byte Ptr, passwordlen:ULong, salt:Byte Ptr, opslimit:ULong, memlimit:Size_T, alg:Int)
		Return crypto_pwhash(out, ULong(outlen), password, passwordlen, salt, ULong(opslimit), Size_T(memlimit), alg)
	End Function

	Rem
	bbdoc: Derives an @out.length bytes long key from a password with a salt whose fixed length is #SaltBytes() bytes.
	about: @out should be at least 16 (128 bits).
	End Rem
	Function Hash:Int(out:Byte[], password:String, salt:Byte Ptr, opslimit:ULong, memlimit:Size_T, alg:Int)
		Local p:Byte Ptr = password.ToUTF8String()
		Local plen:Int = strlen_(p)
		
		Local res:Int = crypto_pwhash(out, ULong(out.length), p, ULong(plen), salt, ULong(opslimit), Size_T(memlimit), alg)
		
		MemFree(p)
		
		Return res
	End Function

	Rem
	bbdoc: 
	End Rem
	Function HashStr:Int(out:Byte Ptr, password:Byte Ptr, passwordlen:ULong, opslimit:ULong, memlimit:Size_T)
		Return crypto_pwhash_str(out, password, passwordlen, ULong(opslimit), Size_T(memlimit))
	End Function

	Rem
	bbdoc: 
	End Rem
	Function HashStr:Int(out:Byte Ptr, password:String, opslimit:ULong, memlimit:Size_T)
		Local p:Byte Ptr = password.ToUTF8String()
		Local plen:Int = strlen_(p)

		Local res:Int = crypto_pwhash_str(out, p, ULong(plen), ULong(opslimit), Size_T(memlimit))
		
		MemFree(p)
		
		Return res
	End Function

	Rem
	bbdoc: 
	End Rem
	Function HashStrVerify:Int(str:String, password:String)
		Local s:Byte Ptr = str.ToCString()

		Local p:Byte Ptr = password.ToUTF8String()
		Local plen:Int = strlen_(p)
		
		Local res:Int = crypto_pwhash_str_verify(str, password, ULong(plen))
		
		MemFree(p)
		MemFree(s)
		
		Return res
	End Function

	Rem
	bbdoc: 
	End Rem
	Function HashStrVerify:Int(str:Byte Ptr, password:Byte Ptr, passwordlen:ULong)
		Return crypto_pwhash_str_verify(str, password, passwordlen)
	End Function

	Rem
	bbdoc: Memory limit suitable for interactive, online operations.
	about: Requires 32MB of dedicated RAM.
	End Rem
	Function MemLimitInteractive:Size_T()
		Return crypto_pwhash_memlimit_interactive()
	End Function

	Rem
	bbdoc: Moderate memory limit.
	about: Requires 128MB of dedicated RAM.
	End Rem
	Function MemLimitModerate:Size_T()
		Return crypto_pwhash_memlimit_moderate()
	End Function

	Rem
	bbdoc: Memory limit suitable for sensitive data and non-interactive operations.
	about: Requires 512MB of dedicated RAM.
	End Rem
	Function MemLimitSensitive:Size_T()
		Return crypto_pwhash_memlimit_sensitive()
	End Function

	Rem
	bbdoc: Number of computations suitable for interactive, online operations.
	End Rem
	Function OpsLimitInteractive:Size_T()
		Return crypto_pwhash_opslimit_interactive()
	End Function

	Rem
	bbdoc: A moderate number of computations.
	about: Higher values requires more CPU cycles to compute a key.
	End Rem
	Function OpsLimitModerate:Size_T()
		Return crypto_pwhash_opslimit_moderate()
	End Function

	Rem
	bbdoc: Number of computations suitable for sensitive data and non-interactive operations.
	about: Higher values requires more CPU cycles to compute a key.
	End Rem
	Function OpsLimitSensitive:Size_T()
		Return crypto_pwhash_opslimit_sensitive()
	End Function
	
End Type

Rem
bbdoc: Encrypts a message with a key and a nonce to keep it confidential, and computes an authentication tag.
about: This tag is used to make sure that the message hasn't been tampered with before decrypting it.
A single key is used both to encrypt/sign and verify/decrypt messages. For this reason, it is critical to keep
the key confidential.
The nonce doesn't have to be confidential, but it should never ever be reused with the same key.
End Rem
Type TSecretBox

	Rem
	bbdoc: 
	End Rem
	Function Easy:Int(ciphertext:Byte[], message:String, nonce:String, key:String)
		Local m:Byte Ptr = message.ToUTF8String()
		Local mlen:Int = strlen_(m)
		Local n:Byte Ptr = nonce.ToCString()
		Local k:Byte Ptr = key.ToCString()
		
		Local res:Int = crypto_secretbox_easy(ciphertext, m, ULong(mlen), n, k)
		
		MemFree(k)
		MemFree(n)
		MemFree(m)
		Return res
	End Function

	Rem
	bbdoc: Encrypts a @message with a @nonce and @key.
	about: @key should be KeyBytes() long
	@nonce should be NonceBytes() long.
	@ciphertext should be at least MacBytes() + message length long.
	End Rem
	Function Easy:Int(ciphertext:Byte[], message:String, nonce:Byte[], key:Byte[])
		Local m:Byte Ptr = message.ToUTF8String()
		Local mlen:Int = strlen_(m)
		
		Local res:Int = crypto_secretbox_easy(ciphertext, m, ULong(mlen), nonce, key)
		
		MemFree(m)
		Return res
	End Function

	Rem
	bbdoc: 
	End Rem
	Function Easy:Int(ciphertext:Byte[], message:Byte Ptr, mlen:ULong, nonce:Byte[], key:Byte[])
		Return crypto_secretbox_easy(ciphertext, message, mlen, nonce, key)
	End Function
	
	Rem
	bbdoc: 
	returns: -1 if the verification fails, and 0 on success.
	End Rem
	Function OpenEasy:Int(decrypted:Byte[], ciphertext:Byte[], nonce:Byte[], key:Byte[])
		Return crypto_secretbox_open_easy(decrypted, ciphertext, ULong(ciphertext.length), nonce, key)
	End Function

	Rem
	bbdoc: 
	End Rem
	Function OpenEasy:Int(decrypted:Byte[], ciphertext:Byte[], nonce:String, key:String)
		Local n:Byte Ptr = nonce.ToCString()
		Local k:Byte Ptr = key.ToCString()

		Local res:Int = crypto_secretbox_open_easy(decrypted, ciphertext, ULong(ciphertext.length), n, k)
		
		MemFree(k)
		MemFree(n)
		Return res
	End Function

	Rem
	bbdoc: 
	End Rem
	Function KeyBytes:Size_T()
		Return crypto_secretbox_keybytes()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function NonceBytes:Size_T()
		Return crypto_secretbox_noncebytes()
	End Function
	
	Rem
	bbdoc: 
	End Rem
	Function MacBytes:Size_T()
		Return crypto_secretbox_macbytes()
	End Function
	
End Type

Rem
bbdoc: Computes an authentication tag for a message and a secret key
about: Provides a way to verify that a given tag is valid for a given message and key.
Does not encrypt the message. It only computes and verifies an authentication tag.
End Rem
Type TAuth

	Function Auth()
	End Function
	
	Function Verify:Int()
	End Function
	
End Type

Rem
bbdoc: 
End Rem
Type TBox

	Function SeedBytes:Size_T()
		Return crypto_box_seedbytes()
	End Function

End Type

?
